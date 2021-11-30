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

#include "../FilediskProxyNative/FilediskProxyNative.h"

using namespace System;
using namespace std;
using namespace System::Runtime::InteropServices;

namespace FilediskProxyManaged {

#define OUT

	public ref class FilediskProxyManaged
	{
		// TODO: Add your methods for this class here.
		// todo: this library is .net based and works as the interop wrapper between the native C++ Library and the C# .Net Application Server.


	public:
		FilediskProxyManaged();

		//int fastAesCBC_DynamicSize(int64_t ctxref, bool encrypt, int64_t prevStateIV, int64_t newStateIV, int64_t pvSrc, size_t srcIndex, size_t nBytes, int64_t lastBlockOutput,
		//	OUT int64_t% lastBlockIndex, OUT int64_t% lastBlockProcessingBytes);
		//int fastAesCTR_DynamicSize(int64_t ctxref, int64_t pvSrc, size_t srcIndex, size_t nBytes, int64_t lastBlockOutput,
		//	OUT int64_t% lastBlockIndex, OUT int64_t% lastBlockProcessingBytes);
		//void aes_init_cbc_ctr(int64_t encryptionKey, int64_t encryptionIV, OUT int64_t% ctxOut);
		//void aes_delete_ctx(int64_t ctxref);

		BOOL init_ctx(UCHAR DriveLetter, size_t filesize, BOOL usePipe, OUT int64_t% ctxOut);
		BOOL delete_ctx(int64_t ctxref);
		void SetEventDriverRequestDataSet(int64_t ctxref, BOOL set);
		void SetEventProxyIdle(int64_t ctxref, BOOL set);
		void SetEventRequestComplete(int64_t ctxref, BOOL set);
		void SetEventShutdown(int64_t ctxref, BOOL set);
		void NotifyWindows(int64_t ctxref, BOOL DriveAdded);
		uint32_t WaitEventDriverRequestDataSet(int64_t ctxref, DWORD miliSeconds);
		uint32_t WaitEventProxyIdle(int64_t ctxref, DWORD miliSeconds);
		uint32_t WaitEventRequestComplete(int64_t ctxref, DWORD miliSeconds);
		void GetSHMHeader(int64_t ctxref, OUT int64_t% byteOffset, OUT DWORD% length, OUT UCHAR% function, OUT DWORD% totalBytesReadWrite);
		void SetSHMHeader(int64_t ctxref, int64_t byteOffset, DWORD length, UCHAR function, DWORD totalBytesReadWrite);
		void GetSHMBuffer(int64_t ctxref, int64_t byteOffset, DWORD length, int64_t outputBuffer);
		void SetSHMBuffer(int64_t ctxref, int64_t byteOffset, DWORD length, int64_t inputBuffer);
		BOOL Step1PipeGetRequest(int64_t ctxref, OUT uint64_t% byteOffset, OUT DWORD% length, OUT UCHAR% function, OUT DWORD% totalBytesReadWrite);
		void ReadPipe(int64_t ctxref, int64_t outputBuffer, int64_t length);
		void WritePipe(int64_t ctxref, int64_t inputBuffer, int64_t length);
		int ConnectPipe(int64_t ctxref);
		int DisconnectPipe(int64_t ctxref);
		int deregister_file(int64_t ctxref);
		void delete_objects(int64_t ctxref);

		// Dispose method will make IDisposable
		~FilediskProxyManaged();

	private:
		FilediskProxyNative::FilediskProxyNative* _nativePtr;

	};
}
