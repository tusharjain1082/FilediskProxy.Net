// testapp2.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <windows.h>
#include "..\testapp1\shared.h"

using namespace std;
///// CLIENT PROGRAM /////
PCONTEXT_REQUEST request = NULL;
//static unsigned char buffer[1024];


BOOL clientStepRequestToPipe(UCHAR function);
BOOL clientStepWriteFileToPipe();
BOOL clientStepReadFileFromPipe();

int main()
{
    std::cout << "Hello World!\n";

    while (true)
    {
        // step 1: send request to server
        clientStepRequestToPipe(IRP_MJ_READ);

        // step 2: read file from server
        clientStepReadFileFromPipe();

        // step 3: send request to server
        clientStepRequestToPipe(IRP_MJ_WRITE);

        // step 4: write file to server
        clientStepWriteFileToPipe();

    }
    // step 3: complete.
    wcout << "Done." << endl;
    system("pause");
    return 0;

}

BOOL clientStepRequestToPipe(UCHAR function)
{
    BOOL result = false;
    DWORD numBytesWritten = 0;
    HANDLE pipe;

    // reset and clean the context configuration
    memset(&__requestBuffer, 0, 1024);
    request = (PCONTEXT_REQUEST)&__requestBuffer;

    wcout << "preparing request" << endl;
    WIN32_FILE_ATTRIBUTE_DATA info;
    LARGE_INTEGER lilen;
    GetFileAttributesExA("c:\\src.exe", GetFileExInfoStandard, &info);
    lilen.HighPart = info.nFileSizeHigh;
    lilen.LowPart = info.nFileSizeLow;
    request->Length = lilen.QuadPart;
    request->MajorFunction = function;

    // request configuration prepared, now connect and send the request
    wcout << "request prepared." << endl;
    wcout << "Connecting to pipe..." << endl;
    // Open the named pipe
    // Most of these parameters aren't very relevant for pipes.
    //WaitNamedPipe(L"\\\\.\\pipe\\my_pipe", NMPWAIT_WAIT_FOREVER);
    while (true)
    {
        pipe = CreateFile(
            L"\\\\.\\pipe\\my_pipe",
            GENERIC_READ | GENERIC_WRITE, // only need read access
            FILE_SHARE_READ | FILE_SHARE_WRITE,
            NULL,
            OPEN_EXISTING,
            FILE_ATTRIBUTE_NORMAL,
            NULL
        );
        if (pipe == INVALID_HANDLE_VALUE) {
  //          wcout << "Failed to connect to pipe." << endl;
            // look up error code here using GetLastError()
            //system("pause");
            //return false;
            continue;
        }
        else
        {
            break;
        }
    }
    wcout << "step 1: sending request..." << endl;
    result = WriteFile(pipe, &__requestBuffer, sizeof(CONTEXT_REQUEST), &numBytesWritten, NULL);
    if (result) {
        wcout << "Number of bytes sent: " << numBytesWritten << endl;
    }
    else {
        wcout << "Failed to send data." << endl;
        // look up error code here using GetLastError()
    }
    // Close our pipe handle
    CloseHandle(pipe);
//    wcout << "Done." << endl;
//    system("pause");
    return TRUE;
}

BOOL clientStepWriteFileToPipe()
{
    BOOL result = false;
    HANDLE file_handle;
    HANDLE pipe;

    // request configuration prepared, now connect and send the request
    wcout << "source file prepared for transfer." << endl;
    wcout << "Connecting to pipe..." << endl;
    // Open the named pipe
    // Most of these parameters aren't very relevant for pipes.
    //WaitNamedPipe(L"\\\\.\\pipe\\my_pipe", NMPWAIT_WAIT_FOREVER);
    while (true)
    {
        pipe = CreateFile(
            L"\\\\.\\pipe\\my_pipe",
            GENERIC_READ | GENERIC_WRITE, // only need read access
            FILE_SHARE_READ | FILE_SHARE_WRITE,
            NULL,
            OPEN_EXISTING,
            FILE_ATTRIBUTE_NORMAL,
            NULL
        );
        if (pipe == INVALID_HANDLE_VALUE) {
//            wcout << "Failed to connect to pipe." << endl;
            // look up error code here using GetLastError()
            //system("pause");
            //return false;
            continue;
        }
        else
        {
            break;
        }
    }

    // Open the named pipe
// Most of these parameters aren't very relevant for pipes.
    file_handle = CreateFile(
        L"c:\\src.exe",
        GENERIC_READ | GENERIC_WRITE, // only need read access
        FILE_SHARE_READ | FILE_SHARE_WRITE,
        NULL,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        NULL
    );

    wcout << "step 2: sending file. size: " << request->Length << endl;
    fastFileToPipe(file_handle, pipe, 0, request->Length);
    wcout << "step 2: sending file completed." << endl;
    CloseHandle(file_handle);
    CloseHandle(pipe);
    wcout << "Done." << endl;
    system("pause");
    return TRUE;
}

BOOL clientStepReadFileFromPipe()
{
    BOOL result = false;
    HANDLE file_handle;
    HANDLE pipe;

    wcout << "Connecting to pipe..." << endl;
    while (true)
    {
        pipe = CreateFile(
            L"\\\\.\\pipe\\my_pipe",
            GENERIC_READ | GENERIC_WRITE, // only need read access
            FILE_SHARE_READ | FILE_SHARE_WRITE,
            NULL,
            OPEN_EXISTING,
            FILE_ATTRIBUTE_NORMAL,
            NULL
        );
        if (pipe == INVALID_HANDLE_VALUE) {
            //            wcout << "Failed to connect to pipe." << endl;
                        // look up error code here using GetLastError()
                        //system("pause");
                        //return false;
            continue;
        }
        else
        {
            break;
        }
    }

    // Open the named pipe
// Most of these parameters aren't very relevant for pipes.
    file_handle = CreateFile(
        L"c:\\dest2.exe",
        GENERIC_READ | GENERIC_WRITE, // only need read access
        FILE_SHARE_READ | FILE_SHARE_WRITE,
        NULL,
        CREATE_ALWAYS,
        FILE_ATTRIBUTE_NORMAL,
        NULL
    );

    wcout << "step 2: receiving file from server. size: " << request->Length << endl;
    fastPipeToFile(file_handle, pipe, 0, request->Length);
    wcout << "step 2: receiving file completed." << endl;
    CloseHandle(file_handle);
    CloseHandle(pipe);
    wcout << "Done." << endl;
    system("pause");
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
