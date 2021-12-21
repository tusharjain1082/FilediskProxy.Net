#pragma once

/*
#include <tchar.h>
#include <stdio.h>
#include <strsafe.h>
#include <stdlib.h>
#include <tchar.h>
#include <string.h> // CBC mode, for memset
#include <assert.h>     /* assert 
*/

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

// driver
#define DEVICE_BASE_NAME	L"\\FileDisk"
#define DEVICE_DIR_NAME		L"\\Device\\FileDisk"
#define BASE_DEVICE_NAME    L"\\Device\\FileDisk\\FileDisk0"
#define BASE_DEVICE_LINK_NAME   L"\\DosDevices\\FileDisk0"

static const char BASE_DEVICE_LINK_NAME_APP[] = "\\\\.\\FileDisk0";
static const char APP_LOG_FILENAME_FORMAT[] = "c:\\FileDiskProxyNativeLog-Device%u.txt";
#define DEVICE_NAME_PREFIX	DEVICE_DIR_NAME	DEVICE_BASE_NAME
#define DEVICE_OBJECT_SHM_NAME L"\\BaseNamedObjects\\FileDiskSHM"
#define DEVICE_OBJECT_SHM_EVENT_REQUEST_DATA L"\\BaseNamedObjects\\FileDiskReqData"
#define DEVICE_OBJECT_SHM_EVENT_PROXY_IDLE L"\\BaseNamedObjects\\FileDiskProxyIdle"
#define DEVICE_OBJECT_SHM_REQUESTCOMPLETE L"\\BaseNamedObjects\\FileDiskRC"
#define DEVICE_OBJECT_SHM_SHUTDOWN L"\\BaseNamedObjects\\FileDiskShutdown"
#define DEVICE_OBJECT_SHM_SHUTDOWNCOMPLETE L"\\BaseNamedObjects\\FileDiskShutdownComplete"
#define DEVICE_OBJECT_SHM_SEMAPHORE L"\\BaseNamedObjects\\FileDiskSHMSync"
#define DEVICE_OBJECT_SHM_SIZE_BYTES 52428800 //10485760
#define SHM_HEADER_SIZE 4096
#define BACKEND_VHD_FILE_SIZE 1048576000 // 1 GB

#define DEVICE_OBJECT_SHM_SIZE_BYTES 52428800 //10485760
//#define SOCKET_BUFFER_SIZE 104857600

static const ULONG DRIVER_SIGNATURE = 5555;
static const ULONG USERMODEAPP_SIGNATURE = 8888;

// app
static LPCWSTR DEVICE_NAME_APP = L"\\Device\\FileDisk\\FileDisk0";
static LPCSTR DEVICE_NAME_APP_char = "\\\\.\\FileDisk\\FileDisk0";
static const char USERMODEAPP_SHM_NAME[] = "Global\\FileDiskSHM";
static const char USERMODEAPP_REQUESTDATAEVENT_NAME[] = "Global\\FileDiskReqData";
static const char USERMODEAPP_PROXYIDLEEVENT_NAME[] = "Global\\FileDiskProxyIdle";
static const char USERMODEAPP_REQUESTCOMPLETEEVENT_NAME[] = "Global\\FileDiskRC";
static const char USERMODEAPP_SHUTDOWNEVENT_NAME[] = "Global\\FileDiskShutdown";
static const char USERMODEAPP_SHUTDOWNCOMPLETEEVENT_NAME[] = "Global\\FileDiskShutdownComplete";
static const char USERMODEAPP_SHM_SEMAPHORE_NAME[] = "Global\\FileDiskSHMSync";

//#define REQUESTPIPE_NAME_DRIVER L"\\??\\pipe\\FileDiskReqPipe"
//static const char USERMODEAPP_REQUEST_PIPE_NAME[] = "\\\\.\\pipe\\FileDiskReqPipe";
//#define PIPE_NAME_DRIVER L"\\DosDevices\\pipe\\FileDiskPipe" // L"\\??\\pipe\\FileDiskPipe"
#define PIPE_NAME_DRIVER L"\\??\\pipe\\FileDiskPipe"
static const char PIPE_NAME_USERMODEAPP[] = "\\\\.\\pipe\\FileDiskPipe";
static const int PIPE_BUFFER_SIZE = 1048576;
#define REQUEST_BUFFER_SIZE 1024
static const int SOCKET_BUFFER_SIZE = 10485760;
static const int IO_BUFFER_SIZE = 104857600; // 100 mb // 52428800: 50 mb

//static const int PIPE_BUFFER_SIZE_TRUE = 4096;

//static const char SERVER_CLIENT_SHM_EVENT_REQUEST[] = "Global\\FileDiskReqData0";
//static const WCHAR DEVICE_OBJECT_SHM_EVENT_PROXY_IDLE[] = L"Global\\FileDiskProxyIdle";

#define FILE_DISK_POOL_TAG 'ksiD'
///
/// Base value for the IOCTL's.
///
#define FILE_DEVICE_FILEDISK		    0x8372


#define MAX_PASSWORD_SIZE 16

typedef struct _OPEN_FILE_INFORMATION {
    LARGE_INTEGER   FileSize;
    BOOLEAN         ReadOnly;
    UCHAR           DriveLetter;
    USHORT          FileNameLength;
    CHAR            FileName[256];
    BOOLEAN         DriverReply;
    ULONG           DeviceNumber;
    BOOL            usePipe;
    BOOL            useShm;
    BOOL            useSocket;
    ULONG           lport;
} OPEN_FILE_INFORMATION, * POPEN_FILE_INFORMATION;

typedef struct _BASE_DEVICE_QUERY {
    int DeviceNumber;
    BOOLEAN ReadOnly;
}BASE_DEVICE_QUERY, * PBASE_DEVICE_QUERY;

typedef struct _CONTEXT_REQUEST {
    UCHAR MajorFunction;
    UINT64 ByteOffset;
    ULONG Length;
    DWORD totalBytesReadWrite;
    ULONG signature;
}CONTEXT_REQUEST, * PCONTEXT_REQUEST;
static const char SIGNATURE_PROXYREPLY[] = "PROXY-REPLY";
static const char SIGNATURE_HELLO[] = "HELLO";

#define IRP_MJ_READ                     0x03
#define IRP_MJ_WRITE                    0x04

//#define IOCTL_FILE_DISK_OPEN_FILE   CTL_CODE(FILE_DEVICE_DISK, 0x800, METHOD_BUFFERED, FILE_READ_ACCESS | FILE_WRITE_ACCESS)
//#define IOCTL_FILE_DISK_CLOSE_FILE  CTL_CODE(FILE_DEVICE_DISK, 0x801, METHOD_BUFFERED, FILE_READ_ACCESS | FILE_WRITE_ACCESS)
#define IOCTL_FILE_DISK_QUERY_FILE  CTL_CODE(FILE_DEVICE_DISK, 0x802, METHOD_BUFFERED, FILE_READ_ACCESS)

#define IOCTL_REGISTER_FILE  CTL_CODE(FILE_DEVICE_DISK, 0x803, METHOD_BUFFERED, FILE_ANY_ACCESS) 
#define IOCTL_DEREGISTER_FILE  CTL_CODE(FILE_DEVICE_DISK, 0x804, METHOD_BUFFERED, FILE_ANY_ACCESS) 
#define IOCTL_TEST  CTL_CODE(FILE_DEVICE_DISK, 0x805, METHOD_BUFFERED, FILE_ANY_ACCESS) 
#define IOCTL_FINDAVAILABLEDEVICE  CTL_CODE(FILE_DEVICE_DISK, 0x806, METHOD_BUFFERED, FILE_ANY_ACCESS) 
#define IOCTL_DEREGISTER_FILE_FROM_DEVICE  CTL_CODE(FILE_DEVICE_DISK, 0x807, METHOD_BUFFERED, FILE_ANY_ACCESS) 
#define IOCTL_LOCK_WRITE_ACCESS_DISK_DEVICE  CTL_CODE(FILE_DEVICE_DISK, 0x808, METHOD_BUFFERED, FILE_ANY_ACCESS) 

#define LOG_FILE_NAME_PATH L"\\DosDevices\\C:\\FileDiskProxyDevice"

