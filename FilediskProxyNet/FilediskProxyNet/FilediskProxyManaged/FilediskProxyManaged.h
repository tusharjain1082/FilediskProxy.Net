#pragma once

/*
#include <winsock2.h>
#include <ws2tcpip.h>

#define _WINSOCK2API_
#define _WINSOCKAPI_   /* Prevent inclusion of winsock.h in windows.h 

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
#include <assert.h>     /* assert 
#include <stdint.h>
#include <Shlobj.h>
*/

#include "../FilediskProxyNative/FilediskProxyNative.h"

using namespace System;
using namespace std;
using namespace System::Runtime::InteropServices;

namespace FilediskProxyManaged {

#define OUT

	public ref class FilediskProxyManaged
	{


	public:
		FilediskProxyManaged();


		// TODO: Add your methods for this class here.
		// todo: this library is .net based and works as the interop wrapper between the native C++ Library and the C# .Net Application Server.



		BOOL init_ctx(UCHAR DriveLetter, size_t filesize, BOOL usePipe, BOOL useShm, BOOL useSocket, BOOL useFile, String^ useFileValue, BOOL readOnlyDisk, ULONG port, OUT int64_t% ctxOut);
		BOOL delete_ctx(int64_t ctxref);
		int setWriteAccess(int64_t ctxref, BOOL set);
		void SetEventDriverRequestDataSet(int64_t ctxref, BOOL set);
		void SetEventProxyIdle(int64_t ctxref, BOOL set);
		void SetEventRequestComplete(int64_t ctxref, BOOL set);
		void SetEventShutdown(int64_t ctxref, BOOL set);
		void SetEventShutdownComplete(int64_t ctxref, BOOL set);
		void PulseEventDriverRequestDataSet(int64_t ctxref);
		void PulseEventProxyIdle(int64_t ctxref);
		void PulseEventRequestComplete(int64_t ctxref);
		void PulseEventShutdown(int64_t ctxref);
		void PulseEventShutdownComplete(int64_t ctxref);
		void NotifyWindows(int64_t ctxref, BOOL DriveAdded);
		void NotifyWindowsAtributesChanged(int64_t ctxref);
		uint32_t WaitEventDriverRequestDataSet(int64_t ctxref, DWORD miliSeconds);
		uint32_t WaitEventProxyIdle(int64_t ctxref, DWORD miliSeconds);
		uint32_t WaitEventRequestComplete(int64_t ctxref, DWORD miliSeconds);
		DWORD WaitEventShutdownComplete(int64_t ctxref, DWORD miliSeconds);
		int isEventSignalled(HANDLE hEvent);
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

		// file mode
		void GetFileModeHeader(int64_t ctxref, OUT int64_t% byteOffset, OUT DWORD% length, OUT UCHAR% function, OUT DWORD% totalBytesReadWrite);
		void ReadFileMode(int64_t ctxref, int64_t outputBuffer, size_t length);
		void WriteFileMode(int64_t ctxref, int64_t inputBuffer, size_t length);

		// sockets
		BOOL CheckSocketPortFree(ULONG port);
		BOOL Step1SocketGetRequest(int64_t ctxref, OUT uint64_t% byteOffset, OUT DWORD% length, OUT UCHAR% function, OUT DWORD% totalBytesReadWrite);
		void ReadSocket(int64_t ctxref, int64_t outputBuffer, size_t length);
		void WriteSocket(int64_t ctxref, int64_t inputBuffer, size_t length);
		void SocketAcceptClient(int64_t ctxref);
		void CloseClientSocket(int64_t ctxref);

		// Dispose method will make IDisposable
		~FilediskProxyManaged();

	private:
		FilediskProxyNative::FilediskProxyNative* _nativePtr;

	};
}
