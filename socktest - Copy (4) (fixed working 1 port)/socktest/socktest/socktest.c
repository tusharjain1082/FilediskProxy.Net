
#include <ntifs.h>
#include <windef.h>
#include <stdlib.h> 
#include <ntstrsafe.h>
#include <stdio.h>
#include <string.h>
#include <wsk.h>
#include <math.h>
#include "shared.h"

#include "ksocket.h"
#include "berkeley.h"

BOOL Step2SendFile(int sockfd);
BOOL Step1Request(int sockfd);
BOOL Step3ReceiveFile(int sockfd);

HANDLE hDataSetEvent;
HANDLE hServerIdleEvent;
PKEVENT KeDataSetEventObj;
PKEVENT KeServerIdleEventObj;

static int sock;

NTSTATUS CreateSharedEventsKe()
{
    WCHAR             symbolicNameBuffer[256];
    OBJECT_ATTRIBUTES ObjectAttributes;
    NTSTATUS status;
    PKEVENT eventHandle;

    DbgPrint("CreateSharedEvents method started!\r\n");

    // event 1
    UNICODE_STRING uEventName = { 0 };
    uEventName.Length = 0;
    uEventName.MaximumLength = 256;
    uEventName.Buffer = (PWSTR)symbolicNameBuffer;
    RtlUnicodeStringPrintf(&uEventName, L"%s", DRIVER_REQUESTDATAEVENT_NAME);
    InitializeObjectAttributes(&ObjectAttributes, &uEventName, OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE, NULL, NULL);
    eventHandle = IoCreateSynchronizationEvent(&uEventName, &hDataSetEvent);

    if (eventHandle == NULL)
    {
        DbgPrint("CreateSharedEvents - DRIVER_REQUESTDATAEVENT_NAME Event fail!\r\n");
        return STATUS_UNSUCCESSFUL;
    }
    KeDataSetEventObj = eventHandle;

    // event 2
    uEventName.Length = 0;
    uEventName.MaximumLength = 256;
    uEventName.Buffer = (PWSTR)symbolicNameBuffer;
    RtlUnicodeStringPrintf(&uEventName, L"%s", DRIVER_SERVERIDLEEVENT_NAME);
    InitializeObjectAttributes(&ObjectAttributes, &uEventName, OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE, NULL, NULL);
    eventHandle = IoCreateSynchronizationEvent(&uEventName, &hServerIdleEvent);

    if (eventHandle == NULL)
    {
        DbgPrint("CreateSharedEvents - DRIVER_SERVERIDLEEVENT_NAME Event fail!\r\n");
        return STATUS_UNSUCCESSFUL;
    }
    KeServerIdleEventObj = eventHandle;

    DbgPrint("CreateSharedEvents method completed!\r\n");

}

BOOL connectServer()
{

    int result;
    LARGE_INTEGER timeout = { 100 };
    LARGE_INTEGER timeout2 = { 10000 };
    struct addrinfo hints = { 0 };
    RtlZeroMemory(&hints, sizeof(struct addrinfo));
    hints.ai_family = AF_INET;// AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    struct addrinfo* res, * ptr = NULL;

    DbgPrint("connecting...\r\n");

    // port 1 - request

    // Resolve the server address and port
    result = getaddrinfo(NULL, DEFAULT_PORT, &hints, &res);
    if (result != 0) {
        DbgPrint("getaddrinfo failed with error: %d\n", result);
    }

    ptr = res;

    //// Connect to server.
    while (TRUE)
    {
        // Create a SOCKET for connecting to server
        sock = socket_connection(AF_INET, SOCK_STREAM, IPPROTO_TCP);

        result = connect(sock, ptr->ai_addr, (int)ptr->ai_addrlen);
        if (result != 0)
        {
            closesocket(sock);
            KeDelayExecutionThread(KernelMode, FALSE, &timeout2);
            continue;
        }
        else
        {
            break;
        }
    }
    freeaddrinfo(res);
    DbgPrint("connected...\r\n");
    return TRUE;
}

NTSTATUS
NTAPI
DriverEntry(
    _In_ PDRIVER_OBJECT DriverObject,
    _In_ PUNICODE_STRING RegistryPath
)
{
    UNREFERENCED_PARAMETER(DriverObject);
    UNREFERENCED_PARAMETER(RegistryPath);

    LARGE_INTEGER timeout = { 100 };
    LARGE_INTEGER timeout2 = { 10000 };

    NTSTATUS Status;

    //
    // Initialize KSOCKET.
    //

    Status = KsInitialize();

    if (!NT_SUCCESS(Status))
    {
        return Status;
    }

    CreateSharedEventsKe();

    for (int i = 0; i < 400; i++)
    {

        //KeSetEvent(KeDataSetEventObj, 100, TRUE);
        //KeDelayExecutionThread(KernelMode, FALSE, &timeout2);
  
        connectServer();

        // perform step 1
        Step1Request(sock);

        // perform step 2
        Step2SendFile(sock);

        // perform step 1
        //Step1Request(sockfd);

        // perform step 3
        Step3ReceiveFile(sock);

        //result = recv(sockfd, recv_buffer, DEFAULT_BUFLEN, 0);

        KeWaitForSingleObject(KeServerIdleEventObj, UserRequest, UserMode, FALSE, NULL);
        KeResetEvent(KeServerIdleEventObj);
        KeDelayExecutionThread(KernelMode, FALSE, &timeout2);


        closesocket(sock);

    }



    //
    // Destroy KSOCKET.
    //

    KsDestroy();

    //
    // Unload the driver immediately.
    //

//    result = recv(sockfd, recv_buffer, sizeof(recv_buffer), 0);
    //recv_buffer[sizeof(recv_buffer) - 1] = '\0';

    return STATUS_UNSUCCESSFUL;
}


BOOL Step1Request(int sockfd)
{
    UNICODE_STRING     uniName;
    OBJECT_ATTRIBUTES  objAttr;
    HANDLE   handle;
    NTSTATUS ntstatus;
    IO_STATUS_BLOCK    ioStatusBlock;
    int result;

    RtlInitUnicodeString(&uniName, L"\\DosDevices\\C:\\src.exe");  // or L"\\SystemRoot\\example.txt"
    InitializeObjectAttributes(&objAttr, &uniName,
        OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE,
        NULL, NULL);

    ntstatus = ZwCreateFile(&handle,
        GENERIC_READ,
        &objAttr, &ioStatusBlock, NULL,
        FILE_ATTRIBUTE_NORMAL,
        0,
        FILE_OPEN,
        FILE_SYNCHRONOUS_IO_NONALERT,
        NULL, 0);

    FILE_STANDARD_INFORMATION fileInfo = { 0 };
    NTSTATUS ntStatus = ZwQueryInformationFile(
        handle,
        &ioStatusBlock,
        &fileInfo,
        sizeof(fileInfo),
        FileStandardInformation
    );
    if (NT_SUCCESS(ntstatus)) {
        //. fileInfo.EndOfFile is the file size of handle.
    }
    ZwClose(handle);

    // reset and clean the context configuration
    memset(&__requestBuffer, 0, REQUEST_BUFFER_SIZE);
    request = (PCONTEXT_REQUEST)&__requestBuffer;
    request->Length = fileInfo.EndOfFile.QuadPart;

    DbgPrint("sending request...\r\n");
    result = send(sockfd, request, REQUEST_BUFFER_SIZE, 0);
    if (result == -1)
    {
        DbgPrint("send request failure.\r\n");
    }
    else
    {
        DbgPrint("send request success.\r\n");
    }

}


BOOL Step2SendFile(int sockfd)
{
    UNICODE_STRING     uniName;
    OBJECT_ATTRIBUTES  objAttr;
    HANDLE   handle;
    NTSTATUS ntstatus;
    IO_STATUS_BLOCK    ioStatusBlock;
    int result;

    RtlInitUnicodeString(&uniName, L"\\DosDevices\\C:\\src.exe");  // or L"\\SystemRoot\\example.txt"
    InitializeObjectAttributes(&objAttr, &uniName,
        OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE,
        NULL, NULL);

    ntstatus = ZwCreateFile(&handle,
        GENERIC_ALL | SYNCHRONIZE,
        &objAttr, &ioStatusBlock, NULL,
        FILE_ATTRIBUTE_NORMAL,
        0,
        FILE_OPEN,
        FILE_SYNCHRONOUS_IO_NONALERT | FILE_RANDOM_ACCESS,
        NULL, 0);

    DbgPrint("sending file...\r\n");

    NTSTATUS readFileResult;
    int sockResult;
    //PBYTE buffer = (PBYTE)ExAllocatePoolWithTag(PagedPool, request->Length, POOL_TAG);
    readFileResult = ZwReadFile(handle, NULL, NULL, NULL, &ioStatusBlock, &__buffer, request->Length, NULL, NULL);
    sockResult = send(sockfd, &__buffer, request->Length, 0);
    //ExFreePoolWithTag(buffer, POOL_TAG);

    //fastFileToSocket(handle, sockfd, 0, request->Length);

    DbgPrint("file sent.\r\n");

    ZwClose(handle);

}


BOOL Step3ReceiveFile(int sockfd)
{
    UNICODE_STRING     uniName;
    OBJECT_ATTRIBUTES  objAttr;
    HANDLE   handle;
    NTSTATUS ntstatus;
    IO_STATUS_BLOCK    ioStatusBlock;
    int result;

    RtlInitUnicodeString(&uniName, L"\\DosDevices\\C:\\dest-driver-step3.exe");  // or L"\\SystemRoot\\example.txt"
    InitializeObjectAttributes(&objAttr, &uniName,
        OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE,
        NULL, NULL);

    ntstatus = ZwCreateFile(&handle,
        GENERIC_ALL | SYNCHRONIZE,
        &objAttr, &ioStatusBlock, NULL,
        FILE_ATTRIBUTE_NORMAL,
        0,
        FILE_OVERWRITE_IF,
        FILE_SYNCHRONOUS_IO_NONALERT | FILE_RANDOM_ACCESS,
        NULL, 0);

    DbgPrint("receiving file...\r\n");

    NTSTATUS readFileResult;
    int sockResult;
    //PBYTE buffer = (PBYTE)ExAllocatePoolWithTag(PagedPool, request->Length, POOL_TAG);
    sockResult = recv(sockfd, &__buffer, request->Length, 0);
    ZwWriteFile(handle, NULL, NULL, NULL, &ioStatusBlock, &__buffer, request->Length, NULL, NULL);
    ZwFlushBuffersFile(handle, &ioStatusBlock);
    //ExFreePoolWithTag(buffer, POOL_TAG);
    DbgPrint("file received.\r\n");

    ZwClose(handle);

}
