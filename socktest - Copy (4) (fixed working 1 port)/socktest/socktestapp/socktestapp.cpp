// socktestapp.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

//#undef UNICODE

#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include "..\socktest\shared.h"

// Need to link with Ws2_32.lib
#pragma comment (lib, "Ws2_32.lib")
// #pragma comment (lib, "Mswsock.lib")

void Step1Request(SOCKET sock);
BOOL serverStepReadFileFromSocket(SOCKET sock);
BOOL serverStepWriteFileToSocket(SOCKET sock);
void createEvents();

HANDLE DataSetEvent;
HANDLE ServerIdleEvent;

SOCKET ListenSocket = INVALID_SOCKET;
SOCKET ClientSocket = INVALID_SOCKET;

BOOL initServer()
{
    WSADATA wsaData;
    int iResult;

    struct addrinfo* result = NULL;
    struct addrinfo hints;

    // Initialize Winsock
    iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != 0) {
        printf("WSAStartup failed with error: %d\n", iResult);
        return 1;
    }

    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_flags = AI_PASSIVE;

    // port 1 - request

    // Resolve the server address and port
    iResult = getaddrinfo(NULL, DEFAULT_PORT, &hints, &result);
    if (iResult != 0) {
        printf("getaddrinfo failed with error: %d\n", iResult);
        WSACleanup();
        return FALSE;
    }

    // Create a SOCKET for connecting to server
    ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if (ListenSocket == INVALID_SOCKET) {
        printf("socket failed with error: %ld\n", WSAGetLastError());
        freeaddrinfo(result);
        WSACleanup();
        return FALSE;

    }

    // Setup the TCP listening socket
    iResult = bind(ListenSocket, result->ai_addr, (int)result->ai_addrlen);
    if (iResult == SOCKET_ERROR) {
        printf("bind failed with error: %d\n", WSAGetLastError());
        freeaddrinfo(result);
        closesocket(ListenSocket);
        WSACleanup();
        return FALSE;
    }

    freeaddrinfo(result);
    std::cout << "sockets-ports created.\r\n";

    return TRUE;
}

int main()
{
    std::cout << "Hello World!\n";

    // first create events
    createEvents();

    // init server
    initServer();

    int iResult = listen(ListenSocket, SOMAXCONN);

    for (int ctr = 0; ctr < 400; ctr++)
    {
        //WaitForSingleObject(DataSetEvent, -1);
        //ResetEvent(DataSetEvent);

        std::cout << "loop init. listen on ports started...\r\n";

        // Accept a client socket
        ClientSocket = accept(ListenSocket, NULL, NULL);
        std::cout << "ClientSocket_Request listen completed. client connected...\r\n";

        // perform step 1 receive request
        Step1Request(ClientSocket);

        // perform step 2 receive file
        serverStepReadFileFromSocket(ClientSocket);

        // perform step 1 receive request
        //Step1Request(ClientSocket);

        // perform step 3 send file to driver
        serverStepWriteFileToSocket(ClientSocket);

        //std::cin.get();

        printf("counter: %u \r\n", ctr);

        SetEvent(ServerIdleEvent);

        closesocket(ClientSocket);

        printf("loop reset from %u \r\n", ctr);

    }

    // No longer need server socket
    closesocket(ListenSocket);
    WSACleanup();

    std::cin.get();
}

void createEvents()
{
    char link[512];

    // Event 1
    memset(link, 0, 512);
    sprintf_s(link, 512, "%s", USERMODEAPP_REQUESTDATAEVENT_NAME);
    DataSetEvent = CreateEventA(NULL, TRUE, FALSE, link);
    if (DataSetEvent == NULL)
    {
        std::cout << "DataSetEvent create failed\r\n";
    }
    // Event 2
    memset(link, 0, 512);
    sprintf_s(link, 512, "%s", USERMODEAPP_SERVERIDLEEVENT_NAME);
    ServerIdleEvent = CreateEventA(NULL, TRUE, FALSE, link);
    if (ServerIdleEvent == NULL)
    {
        std::cout << "ServerIdle create failed\r\n";
    }
    std::cout << "events created.\r\n";

}

void Step1Request(SOCKET sock)
{


    printf("receiving request...\n");

    // Step 1: receive request
    int iResult = recv(sock, (char*)&__requestBuffer, REQUEST_BUFFER_SIZE, 0);
    request = (PCONTEXT_REQUEST)&__requestBuffer;
    if (iResult > 0) {
        printf("Bytes received: %d\n", iResult);
    }
    else if (iResult == 0)
        printf("Connection closing...\n");
    else {
        printf("recv failed with error: %d\n", WSAGetLastError());
    }
    std::cout << "file length: " << request->Length << "\r\n";
    printf("request received.\n");


}

BOOL serverStepReadFileFromSocket(SOCKET sock)
{
    BOOL result;
    HANDLE file_handle;
    DWORD bytesDone;

    file_handle = CreateFile(
        L"Y:\\dest.exe",
        GENERIC_READ | GENERIC_WRITE, // only need read access
        FILE_SHARE_READ | FILE_SHARE_WRITE,
        NULL,
        CREATE_ALWAYS,
        FILE_ATTRIBUTE_NORMAL | FILE_FLAG_RANDOM_ACCESS,
        NULL
    );

    std::wcout << "step 2: receiving file..." << "\r\n";

    //PBYTE buffer = new BYTE[request->Length];
    int sockResult = recv(sock, (char*)&__buffer, request->Length, 0);
    BOOL writeFileResult = WriteFile(file_handle, &__buffer, request->Length, &bytesDone, NULL);
    FlushFileBuffers(file_handle);
    std::wcout << "step 2: receiving file completed.\r\n";
    CloseHandle(file_handle);
    return TRUE;
}


BOOL serverStepWriteFileToSocket(SOCKET sock)
{
    BOOL result;
    HANDLE file_handle;
    DWORD bytesDone;

    file_handle = CreateFile(
        L"C:\\src.exe",
        GENERIC_READ | GENERIC_WRITE, // only need read access
        FILE_SHARE_READ | FILE_SHARE_WRITE,
        NULL,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL | FILE_FLAG_RANDOM_ACCESS,
        NULL
    );

    std::wcout << "step 3: sending file..." << "\r\n";
    //PBYTE buffer = new BYTE[request->Length];
    ReadFile(file_handle, &__buffer, request->Length, &bytesDone, NULL);
    int sockResult = send(sock, (const char*)&__buffer, request->Length, 0);
    std::wcout << "step 3: sending file completed.\r\n";
    CloseHandle(file_handle);
    return TRUE;
}


// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
