// testapp1.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <windows.h>
#include "shared.h"

using namespace std;

///// SERVER PROGRAM /////
HANDLE pipe;
PCONTEXT_REQUEST request = NULL;
BOOL requestSet = false;
BOOL terminateServerClient = false;

BOOL server_init();
BOOL serverStepReceiveRequest();
BOOL serverStepReadFileFromPipe();
BOOL serverStepWriteFileToPipe();
BOOL server_delete();
BOOL serverDecide();

int main()
{
    std::cout << "Hello World!\n";

    // server init
    if (!server_init())
    {
        return -1;
    }

    while (true)
    {
        // step 1: receive request
        serverStepReceiveRequest();

        // server decides function
        serverDecide();

        //server_delete();

        //if (pipe == NULL)
        //    break;
    }

    // Close the pipe (automatically disconnects client too)
    CloseHandle(pipe);
    wcout << "Done." << endl;
    system("pause");
    return 0;
}

BOOL server_init()
{
    // reset flags
    terminateServerClient = false;

    wcout << "Creating an instance of a named pipe..." << endl;
    // Create a pipe to send data
    pipe = CreateNamedPipe(
        L"\\\\.\\pipe\\my_pipe", // name of the pipe
        PIPE_ACCESS_DUPLEX, // 1-way pipe -- send only
        PIPE_TYPE_BYTE | PIPE_READMODE_BYTE, // send data as a byte stream
        1, // only allow 1 instance of this pipe
        PIPE_BUFFER_SIZE, // no outbound buffer
        PIPE_BUFFER_SIZE, // no inbound buffer
        0, // use default wait time
        NULL // use default security attributes
    );
    if (pipe == NULL || pipe == INVALID_HANDLE_VALUE) {
        wcout << "Failed to create outbound pipe instance.";
        // look up error code here using GetLastError()
        system("pause");
        return FALSE;
    }
    wcout << "named pipe created successfully." << endl;
    return TRUE;
}

BOOL server_delete()
{
    // set the termination flag
    terminateServerClient = true;
    CloseHandle(pipe);
    pipe = NULL;
    return TRUE;
}

BOOL serverStepReceiveRequest()
{
    BOOL result;
    DWORD numBytesRead = 0;

    // reset and clean the context configuration
    memset(&__requestBuffer, 0, 1024);
    request = (PCONTEXT_REQUEST)&__requestBuffer;
    requestSet = false;

    // receive request
    wcout << "Waiting for a client to connect to the pipe..." << endl;
    // This call blocks until a client process connects to the pipe
    result = ConnectNamedPipe(pipe, NULL);
    if (!result) {
        wcout << "connection failure or user terminated server and client. aborted." << endl;
        // look up error code here using GetLastError()
        CloseHandle(pipe); // close the pipe
        system("pause");
        return TRUE;
    }
    
    wcout << "step 1: receiving request..." << endl;
    result = ReadFile(pipe, &__requestBuffer, sizeof(CONTEXT_REQUEST), &numBytesRead, NULL);
    wcout << "request received. file size: " << request->Length << endl;
    requestSet = true;
    DisconnectNamedPipe(pipe);
    return TRUE;
}

BOOL serverDecide()
{
    if (request->MajorFunction == IRP_MJ_READ)
    {
        // client asked server give file to it.
        serverStepWriteFileToPipe();
    }
    else
    {
        // client asked server read file from it.
        serverStepReadFileFromPipe();
    }

    return TRUE;
}

BOOL serverStepWriteFileToPipe()
{
    BOOL result;
    HANDLE file_handle;

    wcout << "Waiting for a client to connect to the pipe..." << endl;

    // This call blocks until a client process connects to the pipe
    result = ConnectNamedPipe(pipe, NULL);
    if (!result) {
        wcout << "connection failure or user terminated server and client. aborted." << endl;
        // look up error code here using GetLastError()
        CloseHandle(pipe); // close the pipe
        system("pause");
        return TRUE;
    }

    // Open the named pipe
    // Most of these parameters aren't very relevant for pipes.
    file_handle = CreateFile(
        L"c:\\src.exe",
        GENERIC_READ, // only need read access
        FILE_SHARE_READ | FILE_SHARE_WRITE,
        NULL,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        NULL
    );

    wcout << "step 3: server sending file. size: " << request->Length << endl;
    fastFileToPipe(file_handle, pipe, 0, request->Length);
    wcout << "step 3: server sending file completed." << endl;
    CloseHandle(file_handle);
    DisconnectNamedPipe(pipe);
    wcout << "Done." << endl;
//    system("pause");
    return TRUE;

}

BOOL serverStepReadFileFromPipe()
{
    BOOL result;
    HANDLE file_handle;

    wcout << "Waiting for a client to connect to the pipe..." << endl;

    // This call blocks until a client process connects to the pipe
    result = ConnectNamedPipe(pipe, NULL);
    if (!result) {
        wcout << "connection failure or user terminated server and client. aborted." << endl;
        // look up error code here using GetLastError()
        CloseHandle(pipe); // close the pipe
        system("pause");
        return TRUE;
    }

    file_handle = CreateFile(
        L"c:\\dest.exe",
        GENERIC_READ | GENERIC_WRITE, // only need read access
        FILE_SHARE_READ | FILE_SHARE_WRITE,
        NULL,
        CREATE_ALWAYS,
        FILE_ATTRIBUTE_NORMAL,
        NULL
    );

    wcout << "step 2: receiving file..." << endl;
    fastPipeToFile(file_handle, pipe, 0, request->Length);
    wcout << "step 2: receiving file completed." << endl;
    CloseHandle(file_handle);
    DisconnectNamedPipe(pipe);
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
