#pragma once

#include <winsock2.h>
#include <ws2tcpip.h>

#pragma comment(lib, "Ws2_32.lib")

#define _WINSOCK2API_
#define _WINSOCKAPI_   /* Prevent inclusion of winsock.h in windows.h */

#define WIN32_LEAN_AND_MEAN

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
#include "log.h"
#include "..\filedisk-proxy\FileDiskShared.h"

using namespace std;

namespace FilediskProxyNative {

#define OUT
#ifndef __T
#ifdef _NTDDK_
#define __T(x) L ## x
#else
#define __T(x) x
#endif
#endif
#ifndef _T
#define _T(x) __T(x)
#endif)


    //static const char USERMODEAPP_REQUESTDATAEVENT_NAME[] = "Global\\FileDiskReqData";
    //static const char USERMODEAPP_PROXYIDLEEVENT_NAME[] = "Global\\FileDiskProxyIdle";
    //static const char USERMODEAPP_REQUESTCOMPLETEEVENT_NAME[] = "Global\\FileDiskRC";
    //static const char USERMODEAPP_SHUTDOWNEVENT_NAME[] = "Global\\FileDiskShutdown";

//    static const char USERMODEAPP_SHM_NAME[] = "Global\\FileDiskSHM";
 //   #define DEVICE_OBJECT_SHM_SIZE_BYTES 52428800 //10485760

    static const char BASEDEVICE_NAME_APP[] = "\\Device\\FileDisk\\FileDisk0";
    static const char DEVICE_NAME_APP[] = "\\Device\\FileDisk\\FileDisk";
    static const char DEVICE_NAME_APP_char[] = "\\\\.\\FileDisk\\FileDisk";

    class GlobalBuffers
    {
    public:

        static void CopyGlobalBuffers(void* srcGlobalBuffer, size_t srcindex, void* destGlobalBuffer, size_t destindex, size_t count)
        {
            unsigned char* src = (unsigned char*)srcGlobalBuffer;
            src += srcindex;
            unsigned char* dest = (unsigned char*)destGlobalBuffer;
            dest += destindex;

            for (size_t ctr = 0; ctr < count; ctr++)
            {
                *dest++ = *src++;
            }
        }

        static void ByteArrayCopy(unsigned char* src, int srcindex, unsigned char* dest, int destindex, int count)
        {
            for (int ctr = 0; ctr < count; ctr++)
                dest[destindex++] = src[srcindex++];
        }

        static void CopyFromGlobalBuffer(void* srcGlobalBuffer, int srcindex, unsigned char* dest, int destindex, int count)
        {
            unsigned char* src = (unsigned char*)srcGlobalBuffer;
            src += srcindex;
            for (int ctr = 0; ctr < count; ctr++)
                dest[destindex++] = (unsigned char)*src++;
        }

        static void ZeroBuffer(unsigned char* buffer, int len)
        {
            for (int ctr = 0; ctr < len; ctr++)
                buffer[ctr] = 0;
        }

        static void CopyToGlobalBuffer(unsigned char* src, int srcindex, void* destGlobalBuffer, int destindex, int count)
        {
            unsigned char* dest = (unsigned char*)destGlobalBuffer;
            dest += destindex;
            for (int ctr = 0; ctr < count; ctr++)
                *dest++ = src[srcindex++];
        }

    };

    class commonMethods
    {
    public:

#pragma region "common framework methods"

        static LONGLONG GetFilePointerEx(HANDLE hFile) {
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

        static LARGE_INTEGER intToLargeInt(int i) {
            LARGE_INTEGER li;
            li.QuadPart = i;
            return li;
        }

        static WCHAR* ConvertCharToUnicode(char* input, int nChars)
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

#pragma endregion
    };

    class MYCONTEXTCONFIG
    {
    public:

        // todo: this class is the current session's context which stores all session's initialized handles and objects.

        //array<uint8_t>^ RoundKey = gcnew array<uint8_t>(AES_keyExpSize);
        //array<uint8_t>^ Iv = gcnew array<uint8_t>(AES_BLOCKLEN);
//        uint8_t RoundKey[AES_keyExpSize];
  //      uint8_t Iv[AES_BLOCKLEN];

        FILE* log_fd;

        UCHAR DriveLetter;
        HANDLE Device;
        ULONG DeviceNumber;
        POPEN_FILE_INFORMATION fileConfig;

        HANDLE DriverRequestDataSet;
        HANDLE ProxyIdle;
        HANDLE RequestComplete;
        HANDLE Shutdown;
        HANDLE ShutdownComplete;
        HANDLE shmHandle;
        LPVOID shmMappedBuffer;

        BOOL            usePipe;
        HANDLE          pipe;
        BOOL            useShm;
        BOOL            useSocket;
        char            port[8];
        ULONG           lport;
        SOCKET ListenSocket = INVALID_SOCKET;
        SOCKET ClientSocket = INVALID_SOCKET;

        BYTE __requestBuffer[REQUEST_BUFFER_SIZE + 1];
        PCONTEXT_REQUEST request = (PCONTEXT_REQUEST)&__requestBuffer;
        BOOL requestSet = FALSE;

        BOOL readOnlyDisk;
    };

    class FilediskProxyNative
    {

    public:

        FilediskProxyNative();

        // TODO: Add your methods for this class here.
        // todo: this class is the core of the interop between C++ Native and C# .Net Application.


#pragma region "filedisk proxy context based primary methods"



        static void InitializeLogFile(int64_t ctxref, int DeviceNumber);
        static BOOL OpenKernelDriverEvents(int64_t ctxref);
        static BOOL OpenSharedMemory(int64_t ctxref);
        static BOOL CreateIoPipe(int64_t ctxref);
        static BOOL CreateSocketServer(int64_t ctxref);
        static BOOL CheckSocketPortFree(ULONG port);
        static BOOL InitializeDevice(int64_t ctxref);
        static int FindAvailableDevice();
        static int deregister_file(int64_t ctxref);
        static BOOL init_ctx(UCHAR DriveLetter, size_t filesize, BOOL usePipe, BOOL useShm, BOOL useSocket, BOOL readOnlyDisk, ULONG port, OUT int64_t& ctxOut);
        static BOOL delete_ctx(int64_t ctxref);
        static void delete_objects(int64_t ctxref);
        static int setWriteAccess(int64_t ctxref, BOOL set);

        static void SetEventDriverRequestDataSet(int64_t ctxref, BOOL set);
        static void SetEventProxyIdle(int64_t ctxref, BOOL set);
        static void SetEventRequestComplete(int64_t ctxref, BOOL set);
        static void SetEventShutdown(int64_t ctxref, BOOL set);
        static void SetEventShutdownComplete(int64_t ctxref, BOOL set);
        static void PulseEventDriverRequestDataSet(int64_t ctxref);
        static void PulseEventProxyIdle(int64_t ctxref);
        static void PulseEventRequestComplete(int64_t ctxref);
        static void PulseEventShutdown(int64_t ctxref);
        static void PulseEventShutdownComplete(int64_t ctxref);
        static void NotifyWindows(int64_t ctxref, BOOL DriveAdded);
        static void NotifyWindowsAtributesChanged(int64_t ctxref);
        static DWORD WaitEventDriverRequestDataSet(int64_t ctxref, DWORD miliSeconds);
        static DWORD WaitEventProxyIdle(int64_t ctxref, DWORD miliSeconds);
        static DWORD WaitEventRequestComplete(int64_t ctxref, DWORD miliSeconds);
        static DWORD WaitEventShutdownComplete(int64_t ctxref, DWORD miliSeconds);
        static int isEventSignalled(HANDLE hEvent);
        static void GetSHMHeader(int64_t ctxref, OUT int64_t& byteOffset, OUT DWORD& length, OUT UCHAR& function, OUT DWORD& totalBytesReadWrite);
        static void SetSHMHeader(int64_t ctxref, int64_t byteOffset, DWORD length, UCHAR function, DWORD totalBytesReadWrite);
        static void GetSHMBuffer(int64_t ctxref, int64_t byteOffset, DWORD length, void* outputBuffer);
        static void SetSHMBuffer(int64_t ctxref, int64_t byteOffset, DWORD length, void* inputBuffer);
        static BOOL Step1PipeGetRequest(int64_t ctxref, OUT uint64_t& byteOffset, OUT DWORD& length, OUT UCHAR& function, OUT DWORD& totalBytesReadWrite);
        static void ReadPipe(int64_t ctxref, void* outputBuffer, size_t length);
        static void WritePipe(int64_t ctxref, void* inputBuffer, size_t length);
        static int ConnectPipe(int64_t ctxref);
        static int DisconnectPipe(int64_t ctxref);

        // sockets
        static BOOL Step1SocketGetRequest(int64_t ctxref, OUT uint64_t& byteOffset, OUT DWORD& length, OUT UCHAR& function, OUT DWORD& totalBytesReadWrite);
        static void ReadSocket(int64_t ctxref, void* outputBuffer, size_t length);
        static void ReadSocketComplete(int64_t ctxref, void* outputBuffer, size_t length);
        static int recv_packet(int64_t ctxref, char* p_out_buffer, int buffer_size);
        static void WriteSocket(int64_t ctxref, void* inputBuffer, size_t length);
        static void SocketAcceptClient(int64_t ctxref);
        static void CloseClientSocket(int64_t ctxref);

#pragma endregion

    private:


    };

}