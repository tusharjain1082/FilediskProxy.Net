// proxyapp.cpp : Defines the entry point for the application.
//

#include <windows.h>
#include <tchar.h>
#include <stdio.h>
#include <strsafe.h>
#include <winioctl.h>
#include <shlobj.h>
#include <stdlib.h>
#include <iostream>
#include <winternl.h>
#include <fileapi.h>
#include <winbase.h>
#include <namedpipeapi.h>
#include <securitybaseapi.h>
#include <cstdio>
#include <iostream>
#include <synchapi.h>
#include "framework.h"
#include "proxyapp.h"
#include "..\filedisk-proxy\FileDiskShared.h"
#include <tchar.h>
#include <WinNT.h>

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);


//***************************************************************
//***************************************************************
//***************************************************************
//***************************************************************
//FILEDISK-PROXY*************************************************
//***************************************************************
//***************************************************************
//***************************************************************
//***************************************************************
#pragma region "FileDisk Proxy"


// Proxy Configuration*******************************************
static HANDLE file_handle;
static const char file_name[] = "C:\\FileDiskProxy-Disk.img";
static HANDLE  myThreadHandle;
static DWORD   myThreadId;
static BOOLEAN terminateProxy = false;

// Server pipelines
static HANDLE                   ReqPipeThreadHandle;
static DWORD                    ReqPipeThreadId;
static HANDLE                   ReqPipeHandle;
static unsigned char            DriverRequest[1025];
PCONTEXT_REQUEST pctx = (PCONTEXT_REQUEST)&DriverRequest;
DWORD WINAPI RequestPipelineThreadFunction(LPVOID lpParam);
BOOL CreatePipeServers();
_ATOMIC_ static BOOLEAN PipeServerStarted = FALSE;
static ULONG requestCtr = 0;

BOOLEAN initServerClientThread();
BOOLEAN closeProxy();
DWORD WINAPI ServerClientThreadFunction(LPVOID lpParam);
void tests();

static LPCWSTR    VolumeName = L"\\\\.\\W:";
static LPCSTR    lpcstrVolumeName = "\\\\.\\W:";
static LPCWSTR    DriveName = L"W:\\";
static LPCWSTR DriveNameAlphabet = L"W:";
static OPEN_FILE_INFORMATION  OpenFileInformation;
static HANDLE Device;
static HANDLE DriverRequestDataSet;
static HANDLE ProxyIdle;
static HANDLE RequestComplete;
static HANDLE shmHandle;
static LPVOID shmMappedBuffer;
static HANDLE shmSemaphoreSync;
static CONTEXT_REQUEST ctx;

// Proxy Configuration*******************************************


__int64 myFileSeek(HANDLE hf, __int64 distance, DWORD MoveMethod)
{
    LARGE_INTEGER li;

    li.QuadPart = distance;

    li.LowPart = SetFilePointer(hf,
        li.LowPart,
        &li.HighPart,
        MoveMethod);

    if (li.LowPart == INVALID_SET_FILE_POINTER && GetLastError()
        != NO_ERROR)
    {
        li.QuadPart = -1;
    }

    return li.QuadPart;
}

LONGLONG GetFilePointerEx(HANDLE hFile) {
    LARGE_INTEGER liOfs = { 0 };
    LARGE_INTEGER liNew = { 0 };
    SetFilePointerEx(hFile, liOfs, &liNew, FILE_CURRENT);
    return liNew.QuadPart;
}

static long long toInteger(LARGE_INTEGER const& integer)
{
#ifdef INT64_MAX // Does the compiler natively support 64-bit integers?
    return integer.QuadPart;
#else
    return (static_cast<long long>(integer.HighPart) << 32) | integer.LowPart;
#endif
}

// signed long long to LARGE_INTEGER object:

static LARGE_INTEGER toLargeInteger(long long value)
{
    LARGE_INTEGER result;

#ifdef INT64_MAX // Does the compiler natively support 64-bit integers?
    result.QuadPart = value;
#else
    result.high_part = value & 0xFFFFFFFF00000000;
    result.low_part = value & 0xFFFFFFFF;
#endif
    return result;
}

LARGE_INTEGER intToLargeInt(int i) {
    LARGE_INTEGER li;
    li.QuadPart = i;
    return li;
}

WCHAR* ConvertCharToUnicode(char* input, int nChars)
{
    WCHAR* wcharstring; //LPCWSTR
    // required size
    int size = MultiByteToWideChar(CP_ACP, 0, input, -1, NULL, 0);
    // allocate it
    wcharstring = new WCHAR[nChars];
    MultiByteToWideChar(CP_ACP, 0, input, -1, (LPWSTR)wcharstring, size);
    // use it....
    return wcharstring;
}

BOOL CreateSHMSemaphore()
{
    char link[256];
    int devicenumber = OpenFileInformation.DeviceNumber;
    memset(link, 0, 256);
    sprintf_s((char*)link, sizeof(link), "%s%u", USERMODEAPP_SHM_SEMAPHORE_NAME, devicenumber);
    
    PSECURITY_DESCRIPTOR psd = (PSECURITY_DESCRIPTOR)LocalAlloc(LPTR, SECURITY_DESCRIPTOR_MIN_LENGTH);
    InitializeSecurityDescriptor(psd, SECURITY_DESCRIPTOR_REVISION);
    SetSecurityDescriptorDacl(psd, TRUE, NULL, FALSE);
    SECURITY_ATTRIBUTES sa = { 0 };
    sa.nLength = sizeof(sa);
    sa.lpSecurityDescriptor = psd;
    sa.bInheritHandle = FALSE;

    shmSemaphoreSync = CreateSemaphoreA(&sa, 0, 1, NULL);// link);
    LocalFree(psd);

    if (shmSemaphoreSync == NULL)
    {
        std::cout << "CreateSHMSemaphore function failed.\r\n";
        return FALSE;
    }

    std::cout << "CreateSHMSemaphore function completed.\r\n";
    return TRUE;
}

BOOL OpenKernelDriverEvents()
{
    char link[256];
    int devicenumber = OpenFileInformation.DeviceNumber;
    memset(link, 0, 256);
    sprintf_s((char*)link, sizeof(link), "%s%u", USERMODEAPP_REQUESTDATAEVENT_NAME, devicenumber);
    WCHAR* wcharlink = ConvertCharToUnicode(link, 256);
    DriverRequestDataSet = OpenEvent(EVENT_ALL_ACCESS, FALSE, wcharlink);

    // delete it
    delete[] wcharlink;

    if (DriverRequestDataSet == NULL)
    {
        std::cout << "OpenEvent call failure\r\n";
        return FALSE;
    }

    memset(link, 0, 256);
    sprintf_s((char*)link, sizeof(link), "%s%u", USERMODEAPP_PROXYIDLEEVENT_NAME, devicenumber);
    wcharlink = ConvertCharToUnicode(link, 256);
    ProxyIdle = OpenEvent(EVENT_ALL_ACCESS, FALSE, wcharlink);

    // delete it
    delete[] wcharlink;

    if (ProxyIdle == NULL)
    {
        std::cout << "OpenEvent call failure\r\n";
        return FALSE;
    }

    memset(link, 0, 256);
    sprintf_s((char*)link, sizeof(link), "%s%u", USERMODEAPP_REQUESTCOMPLETEEVENT_NAME, devicenumber);
    wcharlink = ConvertCharToUnicode(link, 256);
    RequestComplete = OpenEvent(EVENT_ALL_ACCESS, FALSE, wcharlink);

    // delete it
    delete[] wcharlink;

    if (RequestComplete == NULL)
    {
        std::cout << "OpenEvent call failure\r\n";
        return FALSE;
    }

    std::cout << "OpenEvent call completed\r\n";
    return TRUE;

}

BOOL OpenSharedMemory()
{
    char shmlink[256];
    int devicenumber = OpenFileInformation.DeviceNumber;
    memset(shmlink, 0, 256);
    sprintf_s((char*)shmlink, sizeof(shmlink), "%s%u", USERMODEAPP_SHM_NAME, devicenumber);

    shmHandle = OpenFileMappingA(FILE_MAP_ALL_ACCESS, TRUE, (LPCSTR)&shmlink);
    if (!shmHandle || shmHandle == INVALID_HANDLE_VALUE)
    {
        printf("OpenFileMappingA(read) fail! Error: %u\n", GetLastError());
        //std::cin.get();
        return FALSE;
    }

    shmMappedBuffer = MapViewOfFile(shmHandle, FILE_MAP_ALL_ACCESS, 0, 0, DEVICE_OBJECT_SHM_SIZE_BYTES);
    if (!shmMappedBuffer)
    {
        printf("OpenFileMappingA(read) fail! Error: %u\n", GetLastError());
        //std::cin.get();
        return FALSE;
    }
    printf("OpenSharedMemory call completed\r\n");
    return TRUE;
}

BOOLEAN initProxy()
{
    BOOLEAN status;

    char link[256];
    memset(link, 0, 256);
    sprintf_s((char*)link, sizeof(link), "%s", file_name);
    WCHAR* wcharlink = ConvertCharToUnicode(link, 256);


    file_handle = CreateFileW(wcharlink,                // name of the write
        GENERIC_ALL,          // open for writing
        FILE_SHARE_READ | FILE_SHARE_WRITE,                      // do not share
        NULL,                   // default security
        OPEN_EXISTING,             // create new file only
        FILE_ATTRIBUTE_NORMAL | FILE_FLAG_RANDOM_ACCESS | FILE_FLAG_NO_BUFFERING | FILE_FLAG_WRITE_THROUGH,  // normal file
          //FILE_ATTRIBUTE_NORMAL | FILE_FLAG_RANDOM_ACCESS | FILE_FLAG_WRITE_THROUGH,  // normal file
        NULL);                  // no attr. template

    if (file_handle == INVALID_HANDLE_VALUE)
    {
        // meaning the file does not exists, so create new file
        file_handle = CreateFileW(wcharlink,                // name of the write
            GENERIC_ALL,          // open for writing
            FILE_SHARE_READ | FILE_SHARE_WRITE,                      // do not share
            NULL,                   // default security
            CREATE_NEW,             // create new file only
            FILE_ATTRIBUTE_NORMAL | FILE_FLAG_RANDOM_ACCESS | FILE_FLAG_NO_BUFFERING | FILE_FLAG_WRITE_THROUGH,  // normal file
            //FILE_ATTRIBUTE_NORMAL | FILE_FLAG_RANDOM_ACCESS | FILE_FLAG_WRITE_THROUGH,  // normal file
            NULL);                  // no attr. template
        if (file_handle == INVALID_HANDLE_VALUE)
        {
            // so file cannot be created or opened, meaning system error, so abort
            std::cout << "initProxy Method: unable to create otherwise open the virtual disk file. aborted\r\n";
            return false;
        }
    
        // file created, so set it's new length
        FILE_END_OF_FILE_INFO lenInfo;
        lenInfo.EndOfFile.QuadPart = BACKEND_VHD_FILE_SIZE; // 1GB Virtual Disk File Size
        status = SetFileInformationByHandle(file_handle, FileEndOfFileInfo, &lenInfo, sizeof(FILE_END_OF_FILE_INFO));
        if (!status)
        {
            std::cout << "initProxy Method: unable to virtual disk file size. aborted\r\n";
            CloseHandle(file_handle);
            return false;
        }
        FlushFileBuffers(file_handle);
    }
    std::cout << "init: disk file created/opened.\r\n";

    OpenFileInformation.FileSize.HighPart = OpenFileInformation.FileSize.LowPart = 0;
    OpenFileInformation.FileSize.QuadPart = 0;
    OpenFileInformation.FileSize.u = { 0,0 };

    WIN32_FILE_ATTRIBUTE_DATA info;
    if (GetFileAttributesExA(file_name, GetFileExInfoStandard, &info) != 0)
    {
        // success
        OpenFileInformation.FileSize.HighPart = info.nFileSizeHigh;
        OpenFileInformation.FileSize.LowPart = info.nFileSizeLow;
    }
    else
    {
        std::cout << "initProxy Method: GetFileAttributesExA method failed. aborted\r\n";
        CloseHandle(file_handle);
        return false;
    }

    // delete it
    delete[] wcharlink;

    // reset the thread configuration
    terminateProxy = false;

    // now finally initialize the server client "primary" thread
    status = initServerClientThread();
    if (!status)
    {
        // error, so abort
        CloseHandle(file_handle);
        return false;
    }

    // success
    return true;
}

BOOLEAN closeProxy()
{
    DWORD                   BytesReturned;

    std::cout << "closeProxy init...\r\n";

    Device = CreateFileA(
        lpcstrVolumeName,                      // lpFileName
        GENERIC_READ | GENERIC_WRITE,       // dwDesiredAccess
        FILE_SHARE_READ | FILE_SHARE_WRITE, // dwShareMode
        NULL,                               // lpSecurityAttributes
        OPEN_EXISTING,                      // dwCreationDistribution
        FILE_FLAG_NO_BUFFERING,             // dwFlagsAndAttributes
        NULL                                // hTemplateFile
    );

    if (!DeviceIoControl(
        Device,
        FSCTL_LOCK_VOLUME,
        NULL,
        0,
        NULL,
        0,
        &BytesReturned,
        NULL
    ))
    {

    }

    if (!DeviceIoControl(
        Device,
        IOCTL_DEREGISTER_FILE,
        NULL,
        0,
        NULL,
        0,
        &BytesReturned,
        NULL
        ))
    {
        std::cout << "closeProxy: DeviceIoControl method failure...\r\n";
    }

    if (!DeviceIoControl(
        Device,
        FSCTL_DISMOUNT_VOLUME,
        NULL,
        0,
        NULL,
        0,
        &BytesReturned,
        NULL
        ))
    {
    }

    if (!DeviceIoControl(
        Device,
        FSCTL_UNLOCK_VOLUME,
        NULL,
        0,
        NULL,
        0,
        &BytesReturned,
        NULL
        ))
    {
    }

    CloseHandle(Device);

    DefineDosDevice(DDD_REMOVE_DEFINITION, DriveNameAlphabet, NULL);
    SHChangeNotify(SHCNE_DRIVEREMOVED, SHCNF_PATH, DriveName, NULL);

    std::cout << "closeProxy: DeviceIoControl and dismout methods completed...\r\n";

    terminateProxy = true;
    WaitForSingleObject(myThreadHandle, -1);

    std::cout << "closeProxy: Server Thread completed...\r\n";

    UnmapViewOfFile(shmMappedBuffer);
    CloseHandle(shmHandle);
    FlushFileBuffers(file_handle);
    CloseHandle(file_handle);
    CloseHandle(DriverRequestDataSet);
    CloseHandle(ProxyIdle);
    CloseHandle(RequestComplete);
    CloseHandle(shmSemaphoreSync);
    std::cout << "closeProxy Method: completed. press any key to exit.\r\n";
    std::cin.get();
    return true;
}

BOOLEAN initServerClientThread()
{
    // phase 0 = connect and register with the driver
    DWORD errNum = 0;
    DWORD                   BytesReturned;
    char DeviceName[255];
    strcpy((char*)&OpenFileInformation.FileName, "\\??\\");
    strcat(OpenFileInformation.FileName, file_name);
    OpenFileInformation.FileNameLength = (USHORT)strlen(OpenFileInformation.FileName);
    OpenFileInformation.DriveLetter = *DriveNameAlphabet;
    OpenFileInformation.ReadOnly = FALSE;

    Device = CreateFileA(
        lpcstrVolumeName,
        GENERIC_READ | GENERIC_WRITE,
        FILE_SHARE_READ | FILE_SHARE_WRITE,
        NULL,
        OPEN_EXISTING,
        FILE_FLAG_NO_BUFFERING,
        NULL
    );

    if (Device != INVALID_HANDLE_VALUE)
    {
        CloseHandle(Device);
        std::cout << "ServerClientThreadFunction Method: drive letter busy failure. aborted.\r\n";
        return FALSE;
    }

    if (!DefineDosDevice(
        DDD_RAW_TARGET_PATH,
        DriveNameAlphabet,
        DEVICE_NAME_APP
    ))
    {
        std::cout << "ServerClientThreadFunction Method: DefineDosDevice failure. aborted.\r\n";
        return FALSE;
    }

    Device = CreateFileA(
        lpcstrVolumeName,                      // lpFileName
        GENERIC_READ | GENERIC_WRITE,       // dwDesiredAccess
        FILE_SHARE_READ | FILE_SHARE_WRITE, // dwShareMode
        NULL,                               // lpSecurityAttributes
        OPEN_EXISTING,                      // dwCreationDistribution
        0,                                  // dwFlagsAndAttributes
        NULL                                // hTemplateFile
    );
    if (Device == INVALID_HANDLE_VALUE)
    {
        DefineDosDevice(DDD_REMOVE_DEFINITION, DriveNameAlphabet, DEVICE_NAME_APP);
        std::cout << "ServerClientThreadFunction Method: invalid device and handle. aborted.\r\n";
        return FALSE;
    }
    std::cout << "ServerClientThreadFunction Method: driver device object and virtual drive path created success.\r\n";

    // initialize shm mutex
    /*
    if (!CreateSHMSemaphore())
    {
        std::cout << "initServerClientThread Method: CreateSHMSemaphore- failure. aborted\r\n";
        DefineDosDevice(DDD_REMOVE_DEFINITION, DriveNameAlphabet, NULL);
        CloseHandle(Device);
        return FALSE;
    }

    // set shm mutex
    OpenFileInformation.shmSemaphoreSync = shmSemaphoreSync;

    */

    if (!DeviceIoControl(
        Device,
        IOCTL_REGISTER_FILE,
        (LPVOID)&OpenFileInformation,
        sizeof(OPEN_FILE_INFORMATION),
        (LPVOID)&OpenFileInformation,
        sizeof(OPEN_FILE_INFORMATION), 
        &BytesReturned,
        NULL
    ))
    {
        std::cout << "initServerClientThread Method: DeviceIoControl-IOCTL_REGISTER_FILE- failure. aborted\r\n";
        CloseHandle(shmSemaphoreSync);
        CloseHandle(Device);
        DefineDosDevice(DDD_REMOVE_DEFINITION, DriveNameAlphabet, NULL);
        return FALSE;
    }
    
    CloseHandle(Device);
    
    // finally load shm and shared events
    OpenSharedMemory();
    OpenKernelDriverEvents();

    myThreadHandle = CreateThread(
        NULL,                   // default security attributes
        0,                      // use default stack size  
        ServerClientThreadFunction,       // thread function name
        NULL,          // argument to thread function 
        0,                      // use default creation flags 
        &myThreadId);   // returns the thread identifier 

    if (myThreadHandle == NULL)
    {
        std::cout << "initServerClientThread Method: unable to create primary server client thread. aborted\r\n";
        closeProxy();
        return FALSE;
    }
    std::cout << "initServerClientThread Method: DeviceIoControl-IOCTL_REGISTER_FILE->Success.\r\n";
    return TRUE;

}

DWORD WINAPI ServerClientThreadFunction(LPVOID lpParam)
{
    DWORD WaitStatus;
    std::cout << "ServerClientThreadFunction Thread Method: running.\r\n";

    SHChangeNotify(SHCNE_DRIVEADD, SHCNF_PATH, DriveName, NULL);

    // phase 1 = infinite loop server and client
    while (TRUE)
    {
        // terminate if user set the termination flag
        if (terminateProxy == TRUE)
        {
            break;
        }

        // wait for driver event to occur
        WaitStatus = WaitForSingleObject(DriverRequestDataSet, 10000);
        //ResetEvent(DriverRequestDataSet);
        if (WaitStatus == WAIT_OBJECT_0)
        {
            // success, the driver set the event flag
            //std::cout << "Driver set DriverRequestDataSet Event\r\n";
        }
        else if (WaitStatus == WAIT_TIMEOUT)
        {
            // the driver did not set the event flag, so continue
            //std::cout << "ServerClientThreadFunction->DriverRequestDataSet timeout, it is not set by the driver.\r\n";
            //std::cout << "Wait timeout at DriverRequestDataSet Event\r\n";
            continue;
        }
        else
        {
            // error or exception, continue
            //std::cout << "WaitForSingleObject else section, bug.\r\n";
            //std::cout << "Unknown error at waiting for DriverRequestDataSet Event\r\n";
            continue;
        }


        // delay is important for kernel and user mode combined synchronisation.
        //Sleep(40);
        
        // unblock when the driver sets the event
        // success, the driver set the event flag
        // then process the driver's request here

        // open and read shared memory
        PCONTEXT_REQUEST request = (PCONTEXT_REQUEST)shmMappedBuffer; //(PCONTEXT_REQUEST)malloc(SHM_HEADER_SIZE);
        LPBYTE procBuffer = (LPBYTE)shmMappedBuffer;
        procBuffer += SHM_HEADER_SIZE;

        // process the request of the driver
        // first set the file offset
        //INT64 offset = toInteger(request->ByteOffset);
        //myFileSeek(file_handle, offset, FILE_BEGIN);

        if (request->reply != DRIVER_SIGNATURE)
        {
            std::cout << "signature not matched, bug.\r\n";
            printf("signature:%u \r\n", request->reply);
        }
        request->reply = USERMODEAPP_SIGNATURE;

        LARGE_INTEGER newOffset = { 0,0 };
        SetFilePointerEx(file_handle, request->ByteOffset, &newOffset, FILE_BEGIN);
        request->ByteOffset.HighPart = newOffset.HighPart;
        request->ByteOffset.LowPart = newOffset.LowPart;
        request->ByteOffset.QuadPart = newOffset.QuadPart;

        if (request->MajorFunction == IRP_MJ_READ)
        {
            // driver request read into buffer
            BOOL result = ReadFile(file_handle, procBuffer, request->Length, &request->totalBytesReadWrite, NULL);
            //printf("ServerClientThreadFunction->ReadFile = %u from offset %u.\r\n", request->totalBytesReadWrite, offset);
        }
        else if (request->MajorFunction == IRP_MJ_WRITE)
        {
            // driver request write through buffer into virtual disk file
            BOOL result = WriteFile(file_handle, procBuffer, request->Length, &request->totalBytesReadWrite, NULL);
            FlushFileBuffers(file_handle);
            //printf("ServerClientThreadFunction->WriteFile = %u from offset %u.\r\n", request->totalBytesReadWrite, offset);
        }
        else
        {
            std::cout << "ctl messages not matched, bug.\r\n";
        }

        // set the shared memory with the processed request

        // delay is important for kernel and user mode combined synchronisation.
        //Sleep(40);

        // finally set the proxy idle event for the driver to unblock and process the reply
        ResetEvent(DriverRequestDataSet);
        SetEvent(ProxyIdle);
//        PulseEvent(ProxyIdle);

        //WaitStatus = WaitForSingleObject(RequestComplete, -1);
        //ResetEvent(RequestComplete);

        while (TRUE)
        {
            // terminate if user set the termination flag
            if (terminateProxy == TRUE)
            {
                ExitThread(0);
            }

            // wait for driver event to occur
            // When Driver sets RequestComplete Event, it completes the sequence of entire request.
            WaitStatus = WaitForSingleObject(RequestComplete, 10000);
            if (WaitStatus == WAIT_OBJECT_0)
            {
                ResetEvent(RequestComplete);
                // success, the driver set the event flag, so break.
                break;
            }
            else if (WaitStatus == WAIT_TIMEOUT)
            {
                // the driver did not set the event flag, so continue
                //std::cout << "ServerClientThreadFunction->DriverRequestDataSet timeout, it is not set by the driver.\r\n";
                continue;
            }
            else
            {
                // error or exception, continue
                //std::cout << "WaitForSingleObject else section, bug.\r\n";
                continue;
            }
        }
        // here, the request sequence completes, then loop reloops.
    }
    // phase 2 = completion and close
    // Server Thread completes and terminates here.
    //myThreadHandle = NULL;
    ExitThread(0);
//    return TRUE;
}

#pragma region "Pipe Servers" 

BOOL CreatePipeServers()
{
    char link[256];
    int devicenumber = OpenFileInformation.DeviceNumber;
    memset(link, 0, 256);
    sprintf_s((char*)link, sizeof(link), "%s%u", USERMODEAPP_REQUEST_PIPE_NAME, devicenumber);
    WCHAR* wcharlink = ConvertCharToUnicode(link, 256);
    ReqPipeHandle = CreateNamedPipe(wcharlink,
        PIPE_ACCESS_DUPLEX,
        PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT,   // FILE_FLAG_FIRST_PIPE_INSTANCE is not needed but forces CreateNamedPipe(..) to fail if the pipe already exists...
        //PIPE_TYPE_BYTE | PIPE_READMODE_BYTE | PIPE_WAIT,   // FILE_FLAG_FIRST_PIPE_INSTANCE is not needed but forces CreateNamedPipe(..) to fail if the pipe already exists...
        1,
        PIPE_BUFFER_SIZE + 1,
        PIPE_BUFFER_SIZE + 1,
        10000,
        NULL);

    // delete it
    delete[] wcharlink;

    if (ReqPipeHandle == INVALID_HANDLE_VALUE)
    {
        std::cout << "CreatePipeServers->CreateThread->CreateNamedPipe->USERMODEAPP_REQUEST_PIPE_NAME function failed. aborted.\r\n";
        return FALSE;
    }

    ReqPipeThreadHandle = CreateThread(
        NULL,                   // default security attributes
        0,                      // use default stack size  
        RequestPipelineThreadFunction,       // thread function name
        NULL,          // argument to thread function 
        0,                      // use default creation flags 
        &myThreadId);   // returns the thread identifier 

    if (ReqPipeThreadHandle == NULL)
    {
        std::cout << "CreatePipeServers->CreateThread->RequestPipelineThreadFunction function failed. aborted.\r\n";
        return FALSE;
    }

    while (TRUE)
    {
        if (PipeServerStarted == TRUE)
        {
            break;
        }
        Sleep(50);
    }
    std::cout << "CreatePipeServers method completed.\r\n";
    return TRUE;

}

DWORD WINAPI RequestPipelineThreadFunction(LPVOID lpParam)
{
    DWORD dwRead;
    OVERLAPPED ol;
    //unsigned char tmp[PIPE_BUFFER_SIZE];

    bool ReadFileStatus;
    int ctr = 0;

    while (TRUE)//ReqPipeHandle != INVALID_HANDLE_VALUE)
    {/*
        if (terminateProxy)
        {
            std::cout << "terminateProxy flag set. aborting thread...\r\n";
            break;
        }
        */
        PipeServerStarted = TRUE;

        SetEvent(ProxyIdle);
        WaitForSingleObject(DriverRequestDataSet, -1);
        ResetEvent(DriverRequestDataSet);

        if (ConnectNamedPipe(ReqPipeHandle, NULL) != FALSE)   // wait for someone to connect to the pipe
        {
            //std::cout << "driver connected to this application through pipe.\r\n";
            memset(&DriverRequest, 0, PIPE_BUFFER_SIZE);
            ReadFileStatus = ReadFile(ReqPipeHandle, &DriverRequest, PIPE_BUFFER_SIZE, &dwRead, NULL);// &ol);
            pctx = (PCONTEXT_REQUEST)&DriverRequest;
            if (pctx->signature == DRIVER_SIGNATURE)
            {
                printf("driver's signature verified successfully at %u\r\n", pctx->requestCtr);
            }
            else
            {
                printf("driver's signature failed verification at %u\r\n", pctx->requestCtr);

            }
            ctr++;
            DisconnectNamedPipe(ReqPipeHandle);
            SetEvent(RequestComplete);
        }
    }
    CloseHandle(ReqPipeHandle);
    ReqPipeHandle = INVALID_HANDLE_VALUE;
    SetEvent(ProxyIdle);
    return TRUE;

}


void tests()
{
    DWORD                   BytesReturned;

    // finally load shm and shared events
//    OpenSharedMemory();
    OpenFileInformation.DeviceNumber = 0;
    OpenKernelDriverEvents();
    CreatePipeServers();

    for (ULONG i = 0; i < 100; i++)
    {

        OpenFileInformation.requestCtr = i;
        if (!DeviceIoControl(
            Device,
            IOCTL_TEST,
            (LPVOID)&OpenFileInformation,
            sizeof(OPEN_FILE_INFORMATION),
            (LPVOID)&OpenFileInformation,
            sizeof(OPEN_FILE_INFORMATION),
            &BytesReturned,
            NULL
        ))
        {
            //          std::cout << "tests Method: DeviceIoControl-IOCTL_TEST- failure. aborted\r\n";
          //            return;
        }
        Sleep(50);
        printf("Round %u\r\n", i);
    }
    //WaitForSingleObject(ProxyIdle, -1);
    std::cout << "tests Method: completed.\r\n";

}
DWORD WINAPI ReadPipelineThreadFunction(LPVOID lpParam)
{
}

DWORD WINAPI WritePipelineThreadFunction(LPVOID lpParam)
{
}

#pragma endregion

#pragma endregion
//***************************************************************
//***************************************************************
//***************************************************************
//***************************************************************
//FILEDISK-PROXY*************************************************
//***************************************************************
//***************************************************************
//***************************************************************
//***************************************************************

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: Place code here.
    /// other code
    AllocConsole();
    FILE* fpstdin = stdin, * fpstdout = stdout, * fpstderr = stderr;
    freopen_s(&fpstdin, "CONIN$", "r", stdin);
    freopen_s(&fpstdout, "CONOUT$", "w", stdout);
    freopen_s(&fpstderr, "CONOUT$", "w", stderr);
    std::cout << "Hello\n"; //you should see this on the console

    // Initialize global strings
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_PROXYAPP, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Perform application initialization:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_PROXYAPP));

    MSG msg;

    // Main message loop:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_PROXYAPP));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_PROXYAPP);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // Store instance handle in our global variable

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE: Processes messages for the main window.
//
//  WM_COMMAND  - process the application menu
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // Parse the menu selections:
            switch (wmId)
            {
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case IDM_EXIT:
                closeProxy();
                DestroyWindow(hWnd);
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    case WM_SHOWWINDOW:
    {
        initProxy();
        break;
    }
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            // TODO: Add any drawing code that uses hdc here...
            EndPaint(hWnd, &ps);
        }
        break;
    case WM_DESTROY:
    {
        //closeProxy();
        PostQuitMessage(0);
        break;
    }
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}
