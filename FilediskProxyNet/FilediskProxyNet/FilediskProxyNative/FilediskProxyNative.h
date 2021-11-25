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
#include "log.h"
#include "..\..\filedisk-proxy\FileDiskShared.h"

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


    static const char USERMODEAPP_REQUESTDATAEVENT_NAME[] = "Global\\FileDiskReqData";
    static const char USERMODEAPP_PROXYIDLEEVENT_NAME[] = "Global\\FileDiskProxyIdle";
    static const char USERMODEAPP_REQUESTCOMPLETEEVENT_NAME[] = "Global\\FileDiskRC";

    static const char USERMODEAPP_SHM_NAME[] = "Global\\FileDiskSHM";
    #define DEVICE_OBJECT_SHM_SIZE_BYTES 52428800 //10485760

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

        UCHAR DriveLetter;
        HANDLE Device;
        ULONG DeviceNumber;
        POPEN_FILE_INFORMATION fileConfig;

        HANDLE DriverRequestDataSet;
        HANDLE ProxyIdle;
        HANDLE RequestComplete;
        HANDLE shmHandle;
        LPVOID shmMappedBuffer;

    };

    class FilediskProxyNative
    {
        // TODO: Add your methods for this class here.
        // todo: this class is the core of the interop between C++ Native and C# .Net Application.

    public:

        FilediskProxyNative();

//        static int fastAesCBC_DynamicSize(int64_t ctxref, bool encrypt, int64_t prevStateIV, int64_t newStateIV, int64_t pvSrc, size_t srcIndex, size_t nBytes, int64_t lastBlockOutput,
  //          OUT int64_t& lastBlockIndex, OUT int64_t& lastBlockProcessingBytes);
    //    static int fastAesCTR_DynamicSize(int64_t ctxref, int64_t pvSrc, size_t srcIndex, size_t nBytes, int64_t lastBlockOutput,
      //      OUT int64_t& lastBlockIndex, OUT int64_t& lastBlockProcessingBytes);
        //static void aes_init_cbc_ctr(int64_t encryptionKey, int64_t encryptionIV, OUT int64_t& ctxOut);
//        static void aes_delete_ctx(int64_t ctxref);



#pragma region "filedisk proxy context based primary methods"


    // initialize
        FILE* log_fd;


        static BOOL OpenKernelDriverEvents(int64_t ctxref);
        static BOOL OpenSharedMemory(int64_t ctxref);
        static BOOL FindInitializeAvailableDevice(int64_t ctxref);
        static int FindAvailableDevice();
        BOOL init_ctx(UCHAR DriveLetter, size_t filesize, OUT int64_t& ctxOut);
        static BOOL delete_ctx(int64_t ctxref);

        static void SetEventDriverRequestDataSet(int64_t ctxref, BOOL set);
        static void SetEventProxyIdle(int64_t ctxref, BOOL set);
        static void SetEventRequestComplete(int64_t ctxref, BOOL set);
        static void NotifyWindows(int64_t ctxref, BOOL DriveAdded);
        static DWORD WaitEventDriverRequestDataSet(int64_t ctxref, DWORD miliSeconds);
        static DWORD WaitEventProxyIdle(int64_t ctxref, DWORD miliSeconds);
        static DWORD WaitEventRequestComplete(int64_t ctxref, DWORD miliSeconds);
        static void GetSHMHeader(int64_t ctxref, OUT int64_t& byteOffset, OUT DWORD& length, OUT UCHAR& function, OUT DWORD& totalBytesReadWrite);
        static void SetSHMHeader(int64_t ctxref, int64_t byteOffset, DWORD length, UCHAR function, DWORD totalBytesReadWrite);
        static void GetSHMBuffer(int64_t ctxref, int64_t byteOffset, DWORD length, void* outputBuffer);
        static void SetSHMBuffer(int64_t ctxref, int64_t byteOffset, DWORD length, void* inputBuffer);

#pragma endregion

    private:

        //static int __fastAesCBC_DynamicSize(int64_t ctxref, bool encrypt, int64_t prevStateIV, int64_t newStateIV, int64_t pvSrc, size_t srcIndex, size_t nBytes, int64_t lastBlockOutput,
        //    OUT int64_t& lastBlockIndex, OUT int64_t& lastBlockProcessingBytes);
        //static void __Process_Aes_Block_CBC(AES_ctx* ctx, bool encrypt, uint8_t* buffer, int inputbufferoffset, size_t length);
        //static void __Process_Aes_Block_CTR(AES_ctx* ctx, uint8_t* buffer, int inputbufferoffset, size_t length);


    };

}