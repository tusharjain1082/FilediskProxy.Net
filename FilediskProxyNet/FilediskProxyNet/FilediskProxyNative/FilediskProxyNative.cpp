/*****************************************************************************/
/* Includes:                                                                 */
/*****************************************************************************/

#define _CRT_SECURE_NO_WARNINGS

#include "FilediskProxyNative.h"
#include "log.h"
#include "..\..\filedisk-proxy\FileDiskShared.h"
using namespace std;

namespace FilediskProxyNative
{
    FilediskProxyNative::FilediskProxyNative()
    {
        FILELog::ReportingLevel() = ldebug3;
        log_fd = fopen("c:\\FileDiskProxyNativeLog.txt", "w");
        Output2FILE::Stream() = log_fd;

    }

    // delete the entire session context and all it's handles and configuration.
    BOOL FilediskProxyNative::delete_ctx(int64_t ctxref)
    {

        MYCONTEXTCONFIG* ctx = (MYCONTEXTCONFIG*)ctxref;
        DWORD BytesReturned;
        int DeviceNumber = ctx->DeviceNumber;

        FILE_LOG(linfo) << "FilediskProxyNative::delete_ctx started";

        // init format
        char volNameFormat[] = "\\\\.\\%s:";
        char driveNameFormat[] = "%s:\\";
        char link[256];
        char volName[256];
        memset(volName, 0, 256);
        sprintf_s(volName, 256, volNameFormat, &ctx->DriveLetter);

        memset(link, 0, 256);
        sprintf_s(link, 256, "%s:", &ctx->DriveLetter);
        WCHAR* DriveNameAlphabet = commonMethods::ConvertCharToUnicode(link, 256);

        memset(link, 0, 256);
        sprintf_s(link, 256, "%s%u", DEVICE_NAME_APP, DeviceNumber);
        WCHAR* DeviceLink = commonMethods::ConvertCharToUnicode(link, 256);

        // open the device
        HANDLE Device = CreateFileA(
            volName,                      // lpFileName
            GENERIC_READ | GENERIC_WRITE,       // dwDesiredAccess
            FILE_SHARE_READ | FILE_SHARE_WRITE, // dwShareMode
            NULL,                               // lpSecurityAttributes
            OPEN_EXISTING,                      // dwCreationDistribution
            0,                                  // dwFlagsAndAttributes
            NULL                                // hTemplateFile
        );
        if (Device == INVALID_HANDLE_VALUE)
        {
            // unable to open device object with binding to the drive letter, continue with the next device number.
            FILE_LOG(linfo) << "FilediskProxyNative::delete_ctx->CreateFileA error 1 at " << link;
            DefineDosDevice(DDD_REMOVE_DEFINITION, DriveNameAlphabet, NULL);
            delete[] DeviceLink;
            delete[] DriveNameAlphabet;
            return FALSE;
        }

        // success, set the configuration and end the loop
        FILE_LOG(linfo) << "FilediskProxyNative::delete_ctx->CreateFileA success 1 at " << link;

        if (DeviceIoControl(
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
            FILE_LOG(linfo) << "FilediskProxyNative::delete_ctx->DeviceIoControl:FSCTL_LOCK_VOLUME success 2 at " << link;
        }

        if (DeviceIoControl(
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
            FILE_LOG(linfo) << "FilediskProxyNative::delete_ctx->DeviceIoControl:IOCTL_DEREGISTER_FILE success 3 at " << link;
        }

        if (DeviceIoControl(
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
            FILE_LOG(linfo) << "FilediskProxyNative::delete_ctx->DeviceIoControl:FSCTL_DISMOUNT_VOLUME success 4 at " << link;
        }

        if (DeviceIoControl(
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
            FILE_LOG(linfo) << "FilediskProxyNative::delete_ctx->DeviceIoControl:FSCTL_UNLOCK_VOLUME success 5 at " << link;
        }

        CloseHandle(Device);
        DefineDosDevice(DDD_REMOVE_DEFINITION, DriveNameAlphabet, NULL);
        NotifyWindows(ctxref, FALSE);
        UnmapViewOfFile(ctx->shmMappedBuffer);
        CloseHandle(ctx->shmHandle);
        CloseHandle(ctx->DriverRequestDataSet);
        CloseHandle(ctx->ProxyIdle);
        CloseHandle(ctx->RequestComplete);
        FILE_LOG(linfo) << "FilediskProxyNative::delete_ctx completed successfully.";
        delete[] DeviceLink;
        delete[] DriveNameAlphabet;
        return TRUE;

    }

    // initialize new session context and initialize everything including kernel driver handles and shared memory
    BOOL FilediskProxyNative::init_ctx(UCHAR DriveLetter, size_t filesize, OUT int64_t& ctxOut)
    {

        FILE_LOG(linfo) << "FilediskProxyNative::init_ctx started";
        MYCONTEXTCONFIG* ctx = new MYCONTEXTCONFIG();
        ctx->DriveLetter = DriveLetter;
        ctx->fileConfig = new OPEN_FILE_INFORMATION();
        ctx->fileConfig->FileSize.QuadPart = filesize;

        if (!FindInitializeAvailableDevice((int64_t)ctx))
        {
            // error in binding and or connection to any available device. abort with error.
            FILE_LOG(lerror) << "FilediskProxyNative::init_ctx->FindInitializeAvailableDevice error. aborted.";
            return FALSE;
        }
        if (!OpenKernelDriverEvents((int64_t)ctx))
        {
            // error in opening events, abort with error
            FILE_LOG(lerror) << "FilediskProxyNative::init_ctx->OpenKernelDriverEvents error. aborted.";
            return FALSE;
        }

        if (!OpenSharedMemory((int64_t)ctx))
        {
            // error in opening events, abort with error
            FILE_LOG(lerror) << "FilediskProxyNative::init_ctx->OpenSharedMemory error. aborted.";
            return FALSE;
        }

        // success
        ctxOut = (int64_t)ctx;
        FILE_LOG(linfo) << "FilediskProxyNative::init_ctx completed successfully.";

        return TRUE;
    }

    // notifies the windows about drive add or remove
    void FilediskProxyNative::NotifyWindows(int64_t ctxref, BOOL DriveAdded)
    {
        MYCONTEXTCONFIG* ctx = (MYCONTEXTCONFIG*)ctxref;

        FILE_LOG(linfo) << "FilediskProxyNative::NotifyWindows started";

        // alert windows for the new drive path.
        char link[256];
        char driveNameFormat[] = "%s:\\";
        memset(link, 0, 256);
        sprintf_s(link, 256, driveNameFormat, &ctx->DriveLetter);
        WCHAR* DriveName = commonMethods::ConvertCharToUnicode(link, 256);

        if (DriveAdded)
            SHChangeNotify(SHCNE_DRIVEADD, SHCNF_PATH, DriveName, NULL);
        else
            SHChangeNotify(SHCNE_DRIVEREMOVED, SHCNF_PATH, DriveName, NULL);

        delete[] DriveName;

        FILE_LOG(linfo) << "FilediskProxyNative::NotifyWindows completed successfully";
    }

    // contacts the base device 0 and requests available device number
    int FilediskProxyNative::FindAvailableDevice()
    {
        // initialize basic configuration
        int DeviceNumber = -1;
        DWORD BytesReturned;
        FILE_LOG(linfo) << "FilediskProxyNative::init_ctx->FindInitializeAvailableDevice->FindAvailableDevice started";

        char link[256];
        memset(link, 0, 256);
        sprintf_s(link, 256, "%s", BASE_DEVICE_LINK_NAME_APP);

        // open the base device object 0
        HANDLE Device = CreateFileA(
            link,                      // lpFileName
            GENERIC_READ | GENERIC_WRITE,       // dwDesiredAccess
            FILE_SHARE_READ | FILE_SHARE_WRITE, // dwShareMode
            NULL,                               // lpSecurityAttributes
            OPEN_EXISTING,                      // dwCreationDistribution
            0,                                  // dwFlagsAndAttributes
            NULL                                // hTemplateFile
        );
        if (Device == INVALID_HANDLE_VALUE)
        {
            // unable to open base device 0 object to query available device.
            FILE_LOG(linfo) << "FilediskProxyNative::init_ctx->FindInitializeAvailableDevice->FindAvailableDevice->CreateFileA failure";
            return -1;
        }

        BASE_DEVICE_QUERY* query = new BASE_DEVICE_QUERY();
        query->DeviceNumber = -1;

        // success, now query the device 0 for available device object.
        if (!DeviceIoControl(
            Device,
            IOCTL_FINDAVAILABLEDEVICE,
            (LPVOID)query,
            sizeof(BASE_DEVICE_QUERY),
            (LPVOID)query,
            sizeof(BASE_DEVICE_QUERY),
            &BytesReturned,
            NULL
        ))
        {
            // no available device object, so return error
            FILE_LOG(linfo) << "FilediskProxyNative::init_ctx->FindInitializeAvailableDevice->FindAvailableDevice->DeviceIoControl:IOCTL_FINDAVAILABLEDEVICE failure";
            delete query;
            CloseHandle(Device);
            return -1;
        }

        // success, we found an available device number, so return it and complete.
        FILE_LOG(linfo) << "FilediskProxyNative::init_ctx->FindInitializeAvailableDevice->FindAvailableDevice completed successfully.";
        CloseHandle(Device);
        DeviceNumber = query->DeviceNumber;
        delete query;
        return DeviceNumber;

    }

    // find available device number
    BOOL FilediskProxyNative::FindInitializeAvailableDevice(int64_t ctxref)
    {
        // todo: we need to open the device through the drive letter link
        DWORD BytesReturned;

        FILE_LOG(linfo) << "FilediskProxyNative::init_ctx->FindInitializeAvailableDevice started";

        // initialize basic configuration
        int DeviceNumber = FindAvailableDevice();
        if (DeviceNumber == -1)
        {
            FILE_LOG(linfo) << "FilediskProxyNative::init_ctx->FindInitializeAvailableDevice->FindAvailableDevice: no available device. aborted.";
            return FALSE;
        }
        FILE_LOG(linfo) << "FilediskProxyNative::init_ctx->FindInitializeAvailableDevice->FindAvailableDevice: available device number: " << DeviceNumber;

        MYCONTEXTCONFIG* ctx = (MYCONTEXTCONFIG*)ctxref;

        // init format
        char volNameFormat[] = "\\\\.\\%s:";
        char driveNameFormat[] = "%s:\\";
        char link[256];
        char volName[256];
        memset(volName, 0, 256);
        sprintf_s(volName, 256, volNameFormat, &ctx->DriveLetter);

        // find if this drive letter isn't in use, otherwise return with error.
        HANDLE Device = CreateFileA(
            (LPCSTR)volName,
            GENERIC_READ | GENERIC_WRITE,
            FILE_SHARE_READ | FILE_SHARE_WRITE,
            NULL,
            OPEN_EXISTING,
            FILE_FLAG_NO_BUFFERING,
            NULL
        );

        if (Device != INVALID_HANDLE_VALUE)
        {
            // drive letter is busy. abort with error.
            FILE_LOG(linfo) << "FilediskProxyNative::init_ctx->FindInitializeAvailableDevice->CreateFileA error 1 = drive letter busy. aborted.";
            return FALSE;
        }

        // success. drive letter was unused. so take it and proceed.
        CloseHandle(Device);
        FILE_LOG(linfo) << "FilediskProxyNative::init_ctx->FindInitializeAvailableDevice->CreateFileA success 1 = drive letter unused, so taking it.";

        memset(link, 0, 256);
        sprintf_s(link, 256, "%s:", &ctx->DriveLetter);
        WCHAR* DriveNameAlphabet = commonMethods::ConvertCharToUnicode(link, 256);

        memset(link, 0, 256);
        sprintf_s(link, 256, "%s%u", DEVICE_NAME_APP, DeviceNumber);
        WCHAR* DeviceLink = commonMethods::ConvertCharToUnicode(link, 256);

        if (!DefineDosDevice(
            DDD_RAW_TARGET_PATH,
            DriveNameAlphabet,
            DeviceLink
        ))
        {
            // unable to define and bind device number link and the drive letter. so try out the next device number
            FILE_LOG(linfo) << "FilediskProxyNative::init_ctx->FindInitializeAvailableDevice->DefineDosDevice error 2 at " << link;
            delete[] DeviceLink;
            delete[] DriveNameAlphabet;
            return FALSE;
        }

        FILE_LOG(linfo) << "FilediskProxyNative::init_ctx->FindInitializeAvailableDevice->DefineDosDevice success 2 at " << link;

        // successfully binded device object with the drive letter
        // now finally open the connection to device object based on available device number
        Device = CreateFileA(
            volName,                      // lpFileName
            GENERIC_READ | GENERIC_WRITE,       // dwDesiredAccess
            FILE_SHARE_READ | FILE_SHARE_WRITE, // dwShareMode
            NULL,                               // lpSecurityAttributes
            OPEN_EXISTING,                      // dwCreationDistribution
            0,                                  // dwFlagsAndAttributes
            NULL                                // hTemplateFile
        );
        if (Device == INVALID_HANDLE_VALUE)
        {
            // unable to open device object with binding to the drive letter, continue with the next device number.
            FILE_LOG(linfo) << "FilediskProxyNative::init_ctx->FindInitializeAvailableDevice->CreateFileA error 3 at " << link;
            DefineDosDevice(DDD_REMOVE_DEFINITION, DriveNameAlphabet, NULL);
            delete[] DeviceLink;
            delete[] DriveNameAlphabet;
            return FALSE;
        }

        // success, set the configuration and end the loop
        FILE_LOG(linfo) << "FilediskProxyNative::init_ctx->FindInitializeAvailableDevice->CreateFileA success 3 at " << link;
        delete[] DeviceLink;
        delete[] DriveNameAlphabet;

        // setup final configuration
        ctx->DeviceNumber = DeviceNumber;
        ctx->Device = Device;
        ctx->fileConfig->DeviceNumber = DeviceNumber;

        
        // now finally register the file
        if (!DeviceIoControl(
            Device,
            IOCTL_REGISTER_FILE,
            (LPVOID)ctx->fileConfig,
            sizeof(OPEN_FILE_INFORMATION),
            (LPVOID)ctx->fileConfig,
            sizeof(OPEN_FILE_INFORMATION),
            &BytesReturned,
            NULL
        ))
        {
            // unable to register with the opened device object, abort with error.
            FILE_LOG(linfo) << "FilediskProxyNative::init_ctx->FindInitializeAvailableDevice->DeviceIoControl:IOCTL_REGISTER_FILE error 4 at " << link;
            DefineDosDevice(DDD_REMOVE_DEFINITION, DriveNameAlphabet, NULL);
            CloseHandle(Device);
            return FALSE;
        }

        // close the device's handle as it's not needed to remain open
        CloseHandle(Device);

        // successfully registered file with the driver, proceed and load the proxy server thread back in the C# Application.
        FILE_LOG(linfo) << "FilediskProxyNative::init_ctx->FindInitializeAvailableDevice->DeviceIoControl:IOCTL_REGISTER_FILE success 4 at " << link;

        // success
        FILE_LOG(linfo) << "FilediskProxyNative::init_ctx->FindInitializeAvailableDevice completed successfully.";
        return TRUE;
    }

    // load kernel driver's events
    BOOL FilediskProxyNative::OpenKernelDriverEvents(int64_t ctxref)
    {
        MYCONTEXTCONFIG* ctx = (MYCONTEXTCONFIG*)ctxref;

        FILE_LOG(linfo) << "FilediskProxyNative::init_ctx->OpenKernelDriverEvents started";

        char link[512];
        int devicenumber = ctx->DeviceNumber;
        memset(link, 0, 512);
        sprintf_s(link, 512, "%s%u", USERMODEAPP_REQUESTDATAEVENT_NAME, devicenumber);
        WCHAR* wcharlink = commonMethods::ConvertCharToUnicode(link, 512);
        ctx->DriverRequestDataSet = OpenEvent(EVENT_ALL_ACCESS, FALSE, wcharlink);

        // delete it
        delete[] wcharlink;

        if (ctx->DriverRequestDataSet == NULL)
        {
            FILE_LOG(linfo) << "FilediskProxyNative::init_ctx->OpenKernelDriverEvents: DriverRequestDataSet Event open failure";
            return FALSE;
        }

        memset(link, 0, 512);
        sprintf_s(link, 512, "%s%u", USERMODEAPP_PROXYIDLEEVENT_NAME, devicenumber);
        wcharlink = commonMethods::ConvertCharToUnicode(link, 512);
        ctx->ProxyIdle = OpenEvent(EVENT_ALL_ACCESS, FALSE, wcharlink);

        // delete it
        delete[] wcharlink;

        if (ctx->ProxyIdle == NULL)
        {
            FILE_LOG(linfo) << "FilediskProxyNative::init_ctx->OpenKernelDriverEvents: ProxyIdle Event open failure";
            return FALSE;
        }

        memset(link, 0, 512);
        sprintf_s(link, 512, "%s%u", USERMODEAPP_REQUESTCOMPLETEEVENT_NAME, devicenumber);
        wcharlink = commonMethods::ConvertCharToUnicode(link, 512);
        ctx->RequestComplete = OpenEvent(EVENT_ALL_ACCESS, FALSE, wcharlink);

        // delete it
        delete[] wcharlink;

        if (ctx->RequestComplete == NULL)
        {
            FILE_LOG(linfo) << "FilediskProxyNative::init_ctx->OpenKernelDriverEvents: RequestComplete Event open failure";
            return FALSE;
        }

        FILE_LOG(linfo) << "FilediskProxyNative::init_ctx->OpenKernelDriverEvents completed, all events opened successfully.";
        return TRUE;

    }

    // load shared memory
    BOOL FilediskProxyNative::OpenSharedMemory(int64_t ctxref)
    {
        MYCONTEXTCONFIG* ctx = (MYCONTEXTCONFIG*)ctxref;

        FILE_LOG(linfo) << "FilediskProxyNative::init_ctx->OpenSharedMemory started";

        char shmlink[256];
        int devicenumber = ctx->DeviceNumber;
        memset(shmlink, 0, 256);
        sprintf_s(shmlink, sizeof(shmlink), "%s%u", USERMODEAPP_SHM_NAME, devicenumber);

        ctx->shmHandle = OpenFileMappingA(FILE_MAP_ALL_ACCESS, TRUE, (LPCSTR)&shmlink);
        if (!ctx->shmHandle || ctx->shmHandle == INVALID_HANDLE_VALUE)
        {
            FILE_LOG(linfo) << "FilediskProxyNative::init_ctx->OpenSharedMemory->OpenFileMappingA failure.";
            return FALSE;
        }

        ctx->shmMappedBuffer = MapViewOfFile(ctx->shmHandle, FILE_MAP_ALL_ACCESS, 0, 0, DEVICE_OBJECT_SHM_SIZE_BYTES);
        if (!ctx->shmMappedBuffer)
        {
            FILE_LOG(linfo) << "FilediskProxyNative::init_ctx->OpenSharedMemory->MapViewOfFile failure.";
            return FALSE;
        }

        FILE_LOG(linfo) << "FilediskProxyNative::init_ctx->OpenSharedMemory completed, shared memory opened successfully.";
        return TRUE;
    }

    // set/reset Event
    void FilediskProxyNative::SetEventDriverRequestDataSet(int64_t ctxref, BOOL set)
    {
        MYCONTEXTCONFIG* ctx = (MYCONTEXTCONFIG*)ctxref;

        if (set)
            SetEvent(ctx->DriverRequestDataSet);
        else
            ResetEvent(ctx->DriverRequestDataSet);
    }

    // set/reset Event
    void FilediskProxyNative::SetEventProxyIdle(int64_t ctxref, BOOL set)
    {
        MYCONTEXTCONFIG* ctx = (MYCONTEXTCONFIG*)ctxref;

        if (set)
            SetEvent(ctx->ProxyIdle);
        else
            ResetEvent(ctx->ProxyIdle);
    }

    // set/reset Event
    void FilediskProxyNative::SetEventRequestComplete(int64_t ctxref, BOOL set)
    {
        MYCONTEXTCONFIG* ctx = (MYCONTEXTCONFIG*)ctxref;

        if (set)
            SetEvent(ctx->RequestComplete);
        else
            ResetEvent(ctx->RequestComplete);
    }

    DWORD FilediskProxyNative::WaitEventDriverRequestDataSet(int64_t ctxref, DWORD miliSeconds)
    {
        MYCONTEXTCONFIG* ctx = (MYCONTEXTCONFIG*)ctxref;
        return WaitForSingleObject(ctx->DriverRequestDataSet, miliSeconds);
    }
        
    DWORD FilediskProxyNative::WaitEventProxyIdle(int64_t ctxref, DWORD miliSeconds)
    {
        MYCONTEXTCONFIG* ctx = (MYCONTEXTCONFIG*)ctxref;
        return WaitForSingleObject(ctx->ProxyIdle, miliSeconds);
    }

    DWORD FilediskProxyNative::WaitEventRequestComplete(int64_t ctxref, DWORD miliSeconds)
    {
        MYCONTEXTCONFIG* ctx = (MYCONTEXTCONFIG*)ctxref;
        return WaitForSingleObject(ctx->RequestComplete, miliSeconds);
    }

    void FilediskProxyNative::GetSHMHeader(int64_t ctxref, OUT int64_t& byteOffset, OUT DWORD& length, OUT UCHAR& function, OUT DWORD& totalBytesReadWrite)
    {
        MYCONTEXTCONFIG* ctx = (MYCONTEXTCONFIG*)ctxref;
        PCONTEXT_REQUEST request = (PCONTEXT_REQUEST)ctx->shmMappedBuffer;
        byteOffset = request->ByteOffset.QuadPart;
        length = request->Length;
        function = request->MajorFunction;
        totalBytesReadWrite = request->totalBytesReadWrite;
    }

    void FilediskProxyNative::SetSHMHeader(int64_t ctxref, int64_t byteOffset, DWORD length, UCHAR function, DWORD totalBytesReadWrite)
    {
        MYCONTEXTCONFIG* ctx = (MYCONTEXTCONFIG*)ctxref;
        PCONTEXT_REQUEST request = (PCONTEXT_REQUEST)ctx->shmMappedBuffer;
        
        if (byteOffset != NULL)
            request->ByteOffset.QuadPart = byteOffset;

        if (length != NULL)
            request->Length = length;

        if (function != NULL)
            request->MajorFunction = function;

        if (totalBytesReadWrite != NULL)
            request->totalBytesReadWrite = totalBytesReadWrite;
    }

    void FilediskProxyNative::GetSHMBuffer(int64_t ctxref, int64_t byteOffset, DWORD length, void* outputBuffer)
    {
        MYCONTEXTCONFIG* ctx = (MYCONTEXTCONFIG*)ctxref;
        PCONTEXT_REQUEST request = (PCONTEXT_REQUEST)ctx->shmMappedBuffer;
        LPBYTE procBuffer = (LPBYTE)ctx->shmMappedBuffer;
        procBuffer += SHM_HEADER_SIZE;
        GlobalBuffers::CopyGlobalBuffers(procBuffer, 0, outputBuffer, 0, length);
    }

    void FilediskProxyNative::SetSHMBuffer(int64_t ctxref, int64_t byteOffset, DWORD length, void* inputBuffer)
    {
        MYCONTEXTCONFIG* ctx = (MYCONTEXTCONFIG*)ctxref;
        PCONTEXT_REQUEST request = (PCONTEXT_REQUEST)ctx->shmMappedBuffer;
        LPBYTE procBuffer = (LPBYTE)ctx->shmMappedBuffer;
        procBuffer += SHM_HEADER_SIZE;
        GlobalBuffers::CopyGlobalBuffers(inputBuffer, 0, procBuffer, 0, length);
    }

}