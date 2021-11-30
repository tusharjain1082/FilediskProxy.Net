#pragma once

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
#include <tchar.h>
#include <WinNT.h>
#include <stdint.h>
#include <string.h> // CBC mode, for memset
#include <cstdlib>
#include <cstdint>
#include <assert.h>     /* assert */
#include <stdint.h>
#include <Shlobj.h>


typedef struct _CONTEXT_REQUEST {
    UCHAR MajorFunction;
    int64_t Length;
}CONTEXT_REQUEST, * PCONTEXT_REQUEST;
#define IRP_MJ_READ                     0x03
#define IRP_MJ_WRITE                    0x04

static const int PIPE_BUFFER_SIZE = 1048576;
static unsigned char buffer[PIPE_BUFFER_SIZE];
static unsigned char __requestBuffer[1024];


static void ReadPipeToBuffer(LPBYTE buffer, HANDLE pipe, size_t size)
{
    BOOL result;
    DWORD bytesDone;
    uint64_t totalVectors = 1; // 1 Vectors of 32 bytes each makes 32 total bytes copy
    size_t stride = totalVectors * PIPE_BUFFER_SIZE;

    while (size)
    {
        result = ReadFile(pipe, buffer, stride, &bytesDone, NULL);
        size -= stride;
        buffer += stride;
    }
}

static void fastPipeToBuffer(LPVOID buffer, HANDLE pipe, size_t pos, size_t nBytes) {

    BOOL result;
    DWORD bytesDone;
    double double_totalBlocks = (double)nBytes / (double)PIPE_BUFFER_SIZE;
    int64_t ltotalBlocks_floor = (int64_t)floor(double_totalBlocks);
    int64_t ltotalBlocks_ceiling = (int64_t)ceil(double_totalBlocks);
    int64_t lProcessingBytes = 0;

    // set buffer offset
    LPBYTE dest = (LPBYTE)buffer;
    dest += pos;

    // copy all the blocks of PIPE_BUFFER_SIZE number of bytes
    lProcessingBytes = ltotalBlocks_floor * PIPE_BUFFER_SIZE;
    ReadPipeToBuffer(dest, pipe, lProcessingBytes);

    pos += lProcessingBytes;
    dest += lProcessingBytes;

    int64_t lremainingBytes = nBytes - lProcessingBytes;
    if (lremainingBytes > 0)
    {
        result = ReadFile(pipe, dest, lremainingBytes, &bytesDone, NULL);
        pos += lremainingBytes;
        dest += lremainingBytes;
    }
}

static void WriteBufferToPipe(LPBYTE buffer, HANDLE pipe, size_t size)
{
    BOOL result;
    DWORD bytesDone;
    uint64_t totalVectors = 1; // 1 Vectors of 32 bytes each makes 32 total bytes copy
    size_t stride = totalVectors * PIPE_BUFFER_SIZE;

    while (size)
    {
        result = WriteFile(pipe, buffer, stride, &bytesDone, NULL);
        FlushFileBuffers(pipe);
        size -= stride;
        buffer += stride;
    }
}

static void fastBufferToPipe(LPVOID buffer, HANDLE pipe, size_t pos, size_t nBytes) {

    BOOL result;
    DWORD bytesDone;
    double double_totalBlocks = (double)nBytes / (double)PIPE_BUFFER_SIZE;
    int64_t ltotalBlocks_floor = (int64_t)floor(double_totalBlocks);
    int64_t ltotalBlocks_ceiling = (int64_t)ceil(double_totalBlocks);
    int64_t lProcessingBytes = 0;

    // set buffer offset
    LPBYTE src = (LPBYTE)buffer;
    src += pos;

    // copy all the blocks of PIPE_BUFFER_SIZE number of bytes
    lProcessingBytes = ltotalBlocks_floor * PIPE_BUFFER_SIZE;
    WriteBufferToPipe(src, pipe, lProcessingBytes);

    pos += lProcessingBytes;
    src += lProcessingBytes;

    int64_t lremainingBytes = nBytes - lProcessingBytes;
    if (lremainingBytes > 0)
    {
        result = WriteFile(pipe, src, lremainingBytes, &bytesDone, NULL);
        FlushFileBuffers(pipe);
        pos += lremainingBytes;
        src += lremainingBytes;
    }
}

static void WriteFileToPipe(HANDLE file_handle, HANDLE pipe, size_t size)
{
    BOOL result;
    DWORD bytesDone;
    uint64_t totalVectors = 1; // 1 Vectors of 32 bytes each makes 32 total bytes copy
    size_t stride = totalVectors * PIPE_BUFFER_SIZE;

    while (size)
    {
        result = ReadFile(file_handle, &buffer, stride, &bytesDone, NULL);
        result = WriteFile(pipe, &buffer, stride, &bytesDone, NULL);
        FlushFileBuffers(pipe);
        size -= stride;
    }
}

static void fastFileToPipe(HANDLE file_handle, HANDLE pipe, size_t pos, size_t nBytes) {

    BOOL result;
    DWORD bytesDone;
    double double_totalBlocks = (double)nBytes / (double)PIPE_BUFFER_SIZE;
    int64_t ltotalBlocks_floor = (int64_t)floor(double_totalBlocks);
    int64_t ltotalBlocks_ceiling = (int64_t)ceil(double_totalBlocks);
    int64_t lProcessingBytes = 0;

    // set file pointer to the position specified by the user
    LARGE_INTEGER offset = { 0 };
    offset.QuadPart = pos;
    SetFilePointerEx(file_handle, offset, NULL, FILE_BEGIN);

    // copy all the blocks of PIPE_BUFFER_SIZE number of bytes
    lProcessingBytes = ltotalBlocks_floor * PIPE_BUFFER_SIZE;
    WriteFileToPipe(file_handle, pipe, lProcessingBytes);

    pos += lProcessingBytes;

    int64_t lremainingBytes = nBytes - lProcessingBytes;
    if (lremainingBytes > 0)
    {
        result = ReadFile(file_handle, &buffer, lremainingBytes, &bytesDone, NULL);
        result = WriteFile(pipe, &buffer, lremainingBytes, &bytesDone, NULL);
        FlushFileBuffers(pipe);
    }
}

static void ReadFileFromPipe(HANDLE file_handle, HANDLE pipe, size_t size)
{
    BOOL result;
    DWORD bytesDone;
    uint64_t totalVectors = 1; // 1 Vectors of 32 bytes each makes 32 total bytes copy
    size_t stride = totalVectors * PIPE_BUFFER_SIZE;

    while (size)
    {
        result = ReadFile(pipe, &buffer, stride, &bytesDone, NULL);
        result = WriteFile(file_handle, &buffer, stride, &bytesDone, NULL);
        FlushFileBuffers(file_handle);
        size -= stride;
    }
}

static void fastPipeToFile(HANDLE file_handle, HANDLE pipe, size_t pos, size_t nBytes)
{

    BOOL result;
    DWORD bytesDone;
    double double_totalBlocks = (double)nBytes / (double)PIPE_BUFFER_SIZE;
    int64_t ltotalBlocks_floor = (int64_t)floor(double_totalBlocks);
    int64_t ltotalBlocks_ceiling = (int64_t)ceil(double_totalBlocks);
    int64_t lProcessingBytes = 0;

    // set file pointer to the position specified by the user
    LARGE_INTEGER offset = { 0 };
    offset.QuadPart = pos;
    SetFilePointerEx(file_handle, offset, NULL, FILE_BEGIN);

    // copy all the blocks of PIPE_BUFFER_SIZE number of bytes
    lProcessingBytes = ltotalBlocks_floor * PIPE_BUFFER_SIZE;
    ReadFileFromPipe(file_handle, pipe, lProcessingBytes);

    pos += lProcessingBytes;

    int64_t lremainingBytes = nBytes - lProcessingBytes;
    if (lremainingBytes > 0)
    {
        result = ReadFile(pipe, &buffer, lremainingBytes, &bytesDone, NULL);
        result = WriteFile(file_handle, &buffer, lremainingBytes, &bytesDone, NULL);
        FlushFileBuffers(file_handle);
    }
}
