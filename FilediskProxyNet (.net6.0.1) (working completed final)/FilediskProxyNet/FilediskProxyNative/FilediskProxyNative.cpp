/*****************************************************************************/
/* Includes:                                                                 */
/*****************************************************************************/

#define _CRT_SECURE_NO_WARNINGS


#include "FilediskProxyNative.h"
#include "log.h"
#include "..\filedisk-proxy\FileDiskShared.h"

using namespace std;

namespace FilediskProxyNative
{
    FilediskProxyNative::FilediskProxyNative()
    {

    }


    void FilediskProxyNative::InitializeLogFile(int64_t ctxref, int DeviceNumber)
    {
        MYCONTEXTCONFIG* ctx = (MYCONTEXTCONFIG*)ctxref;
        char link[256];
        memset(link, 0, 256);
        sprintf_s(link, 256, APP_LOG_FILENAME_FORMAT, DeviceNumber);

        FILELog::ReportingLevel() = ldebug3;
        ctx->log_fd = fopen(link, "w");
        Output2FILE::Stream() = ctx->log_fd;

    }

    int FilediskProxyNative::setWriteAccess(int64_t ctxref, BOOL set)
    {
        MYCONTEXTCONFIG* ctx = (MYCONTEXTCONFIG*)ctxref;
        DWORD BytesReturned;
        int DeviceNumber = ctx->DeviceNumber;

        FILE_LOG(linfo) << "setWriteAccess method started";

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
            FILE_LOG(linfo) << "setWriteAccess->CreateFileA method failure.";
            return -1;
        }

        BASE_DEVICE_QUERY* query = new BASE_DEVICE_QUERY();
        query->DeviceNumber = DeviceNumber;
        query->ReadOnly = (BOOLEAN)set;
        FILE_LOG(linfo) << ((set) ? "setWriteAccess::true" : "setWriteAccess::false");

        // success, now query the device 0 and set/unset the attribute/config
        if (!DeviceIoControl(
            Device,
            IOCTL_LOCK_WRITE_ACCESS_DISK_DEVICE,
            (LPVOID)query,
            sizeof(BASE_DEVICE_QUERY),
            (LPVOID)query,
            sizeof(BASE_DEVICE_QUERY),
            &BytesReturned,
            NULL
        ))
        {
            // error
            FILE_LOG(linfo) << "setWriteAccess->DeviceIoControl:IOCTL_LOCK_WRITE_ACCESS_DISK_DEVICE failure";
            delete query;
            CloseHandle(Device);
            return -1;
        }

        // success
        FILE_LOG(linfo) << "setWriteAccess method completed successfully.";
        CloseHandle(Device);
        delete query;
        return 1;
    }

    int FilediskProxyNative::deregister_file(int64_t ctxref)
    {
        MYCONTEXTCONFIG* ctx = (MYCONTEXTCONFIG*)ctxref;
        DWORD BytesReturned;
        int DeviceNumber = ctx->DeviceNumber;

        FILE_LOG(linfo) << "deregister_file method started";

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
            FILE_LOG(linfo) << "deregister_file->CreateFileA method failure.";
            return -1;
        }

        BASE_DEVICE_QUERY* query = new BASE_DEVICE_QUERY();
        query->DeviceNumber = DeviceNumber;

        // success, now query the device 0 for available device object.
        if (!DeviceIoControl(
            Device,
            IOCTL_DEREGISTER_FILE_FROM_DEVICE,
            (LPVOID)query,
            sizeof(BASE_DEVICE_QUERY),
            (LPVOID)query,
            sizeof(BASE_DEVICE_QUERY),
            &BytesReturned,
            NULL
        ))
        {
            // no available device object, so return error
            FILE_LOG(linfo) << "deregister_file->DeviceIoControl:IOCTL_DEREGISTER_FILE_FROM_DEVICE failure";
            delete query;
            CloseHandle(Device);
            return -1;
        }

        // success
        FILE_LOG(linfo) << "deregister_file method completed successfully.";
        CloseHandle(Device);
        delete query;
        return 1;
    }

    // delete context's objects
    void FilediskProxyNative::delete_objects(int64_t ctxref)
    {
        MYCONTEXTCONFIG* ctx = (MYCONTEXTCONFIG*)ctxref;
        if (ctx->useShm)
        {
            UnmapViewOfFile(ctx->shmMappedBuffer);
            CloseHandle(ctx->shmHandle);
            FILE_LOG(linfo) << "FilediskProxyNative::delete_objects: shm released";
        }
 
        CloseHandle(ctx->DriverRequestDataSet);
        CloseHandle(ctx->ProxyIdle);
        CloseHandle(ctx->RequestComplete);
        CloseHandle(ctx->Shutdown);
        CloseHandle(ctx->ShutdownComplete);
        FILE_LOG(linfo) << "FilediskProxyNative::delete_objects: events released";

        if (ctx->usePipe)
        {
            CloseHandle(ctx->pipe);
            ctx->pipe = NULL;
            FILE_LOG(linfo) << "FilediskProxyNative::delete_objects: pipe deleted";
        }

        if (ctx->useSocket)
        {
            closesocket(ctx->ClientSocket);
            closesocket(ctx->ListenSocket);
            WSACleanup();
        FILE_LOG(linfo) << "FilediskProxyNative::delete_objects: sockets deleted";
        }

        if (ctx->useFile)
        {
            CloseHandle(ctx->useFileHandle);
            ctx->useFileHandle = NULL;
            FILE_LOG(linfo) << "FilediskProxyNative::delete_objects: file mode handle deleted";
        }

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

        // first deregister the file so that the driver device shuts down
        //deregister_file(ctxref);

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

        /*
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
        */


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

        FILE_LOG(linfo) << "FilediskProxyNative::delete_ctx completed successfully.";
        delete[] DeviceLink;
        delete[] DriveNameAlphabet;
        return TRUE;

    }


    // initialize new session context and initialize everything including kernel driver handles and shared memory
    BOOL FilediskProxyNative::init_ctx(UCHAR DriveLetter, size_t filesize, BOOL usePipe, BOOL useShm, BOOL useSocket, BOOL useFile, char* useFileValue, BOOL readOnlyDisk, ULONG port, OUT int64_t& ctxOut)
    {

        MYCONTEXTCONFIG* ctx = new MYCONTEXTCONFIG();
        ctx->DriveLetter = DriveLetter;
        ctx->fileConfig = new OPEN_FILE_INFORMATION();
        ctx->fileConfig->FileSize.QuadPart = filesize;
        ctx->usePipe = ctx->fileConfig->usePipe = usePipe;
        ctx->useShm = ctx->fileConfig->useShm = useShm;
        ctx->useSocket = ctx->fileConfig->useSocket = useSocket;
        ctx->useFile = ctx->fileConfig->useFile = useFile;
        ctx->useFileValue = useFileValue;
        std::sprintf(ctx->fileConfig->useFileValue, "%s", ctx->useFileValue);

        std::sprintf(ctx->port, "%u", port);
        ctx->lport = ctx->fileConfig->lport = port;
        ctx->readOnlyDisk = readOnlyDisk;

        // initialize basic configuration
        int DeviceNumber = FindAvailableDevice();
        if (DeviceNumber == -1)
        {
            return FALSE;
        }
        ctx->DeviceNumber = DeviceNumber;

        char devicenumtxt[8];
        memset(devicenumtxt, 0, 8);
        sprintf_s(devicenumtxt, 8, "%u", ctx->DeviceNumber);
        InitializeLogFile((int64_t)ctx, ctx->DeviceNumber);
        FILE_LOG(linfo) << "FilediskProxyNative::init_ctx started with discovered available device number:" << devicenumtxt;

        if (ctx->useFile == 1)
        {
            FILE_LOG(linfo) << "FilediskProxyNative::init_ctx->useFile option selected::local file is:: " << ctx->useFileValue;
        }

        if (!InitializeDevice((int64_t)ctx))
        {
            // error in binding and or connection to any available device. abort with error.
            FILE_LOG(lerror) << "FilediskProxyNative::init_ctx->InitializeDevice error. aborted.";
            return FALSE;
        }

        if (!OpenKernelDriverEvents((int64_t)ctx))
        {
            // error in opening events, abort with error
            FILE_LOG(lerror) << "FilediskProxyNative::init_ctx->OpenKernelDriverEvents error. aborted.";
            return FALSE;
        }

        if (useShm)
        {
            if (!OpenSharedMemory((int64_t)ctx))
            {
                // error in opening events, abort with error
                FILE_LOG(lerror) << "FilediskProxyNative::init_ctx->OpenSharedMemory error. aborted.";
                return FALSE;
            }
        }

        if (usePipe)
        {
            if (!CreateIoPipe((int64_t)ctx))
            {
                // error in opening events, abort with error
                FILE_LOG(lerror) << "FilediskProxyNative::init_ctx->CreateIoPipe error. aborted.";
                return FALSE;
            }
        }

        if (useSocket)
        {
            if (!CreateSocketServer((int64_t)ctx))
            {
                // error in socket creation, abort with error
                FILE_LOG(lerror) << "FilediskProxyNative::init_ctx->CreateSocketServer error. aborted.";
                return FALSE;
            }
        }

        if (useFile)
        {
            if (!InitializeFileMode((int64_t)ctx))
            {
                // error in socket creation, abort with error
                FILE_LOG(lerror) << "FilediskProxyNative::init_ctx->InitializeFileMode error. aborted.";
                return FALSE;
            }            
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

    // notifies the windows about attributes change
    void FilediskProxyNative::NotifyWindowsAtributesChanged(int64_t ctxref)
    {
        MYCONTEXTCONFIG* ctx = (MYCONTEXTCONFIG*)ctxref;

        FILE_LOG(linfo) << "FilediskProxyNative::NotifyWindowsAtributesChanged started";

        // alert windows for the new drive path.
        char link[256];
        char driveNameFormat[] = "%s:\\";
        memset(link, 0, 256);
        sprintf_s(link, 256, driveNameFormat, &ctx->DriveLetter);
        WCHAR* DriveName = commonMethods::ConvertCharToUnicode(link, 256);

        SHChangeNotify(SHCNE_ALLEVENTS, SHCNF_PATH, DriveName, NULL);
        SHChangeNotify(SHCNE_ATTRIBUTES, SHCNF_PATH, DriveName, NULL);        
        delete[] DriveName;

        FILE_LOG(linfo) << "FilediskProxyNative::NotifyWindowsAtributesChanged completed successfully";
    }

    // contacts the base device 0 and requests available device number
    int FilediskProxyNative::FindAvailableDevice()
    {
        // initialize basic configuration
        int DeviceNumber = -1;
        DWORD BytesReturned;

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
            delete query;
            CloseHandle(Device);
            return -1;
        }

        // success, we found an available device number, so return it and complete.
        CloseHandle(Device);
        DeviceNumber = query->DeviceNumber;
        delete query;
        return DeviceNumber;
    }

    // find available device number
    BOOL FilediskProxyNative::InitializeDevice(int64_t ctxref)
    {
        // todo: we need to open the device through the drive letter link
        DWORD BytesReturned;

        FILE_LOG(linfo) << "FilediskProxyNative::init_ctx->InitializeDevice started";

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
            FILE_LOG(linfo) << "FilediskProxyNative::init_ctx->InitializeDevice->CreateFileA error 1 = drive letter busy. aborted.";
            return FALSE;
        }

        // success. drive letter was unused. so take it and proceed.
        CloseHandle(Device);
        FILE_LOG(linfo) << "FilediskProxyNative::init_ctx->InitializeDevice->CreateFileA success 1 = drive letter unused, so taking it.";

        memset(link, 0, 256);
        sprintf_s(link, 256, "%s:", &ctx->DriveLetter);
        WCHAR* DriveNameAlphabet = commonMethods::ConvertCharToUnicode(link, 256);

        memset(link, 0, 256);
        sprintf_s(link, 256, "%s%u", DEVICE_NAME_APP, ctx->DeviceNumber);
        WCHAR* DeviceLink = commonMethods::ConvertCharToUnicode(link, 256);

        if (!DefineDosDevice(
            DDD_RAW_TARGET_PATH,
            DriveNameAlphabet,
            DeviceLink
        ))
        {
            // unable to define and bind device number link and the drive letter. so try out the next device number
            FILE_LOG(linfo) << "FilediskProxyNative::init_ctx->InitializeDevice->DefineDosDevice error 2 at " << link;
            delete[] DeviceLink;
            delete[] DriveNameAlphabet;
            return FALSE;
        }

        FILE_LOG(linfo) << "FilediskProxyNative::init_ctx->InitializeDevice->DefineDosDevice success 2 at " << link;

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
            FILE_LOG(linfo) << "FilediskProxyNative::init_ctx->InitializeDevice->CreateFileA error 3 at " << link;
            DefineDosDevice(DDD_REMOVE_DEFINITION, DriveNameAlphabet, NULL);
            delete[] DeviceLink;
            delete[] DriveNameAlphabet;
            return FALSE;
        }

        // success, set the configuration and end the loop
        FILE_LOG(linfo) << "FilediskProxyNative::init_ctx->InitializeDevice->CreateFileA success 3 at " << link;
        delete[] DeviceLink;
        delete[] DriveNameAlphabet;

        // setup final configuration
        //ctx->DeviceNumber = DeviceNumber;
        ctx->Device = Device;
        ctx->fileConfig->DeviceNumber = ctx->DeviceNumber;
        ctx->fileConfig->ReadOnly = (BOOLEAN)ctx->readOnlyDisk;
        
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
            FILE_LOG(linfo) << "FilediskProxyNative::init_ctx->InitializeDevice->DeviceIoControl:IOCTL_REGISTER_FILE error 4 at " << link;
            DefineDosDevice(DDD_REMOVE_DEFINITION, DriveNameAlphabet, NULL);
            CloseHandle(Device);
            return FALSE;
        }

        // close the device's handle as it's not needed to remain open
        CloseHandle(Device);

        // successfully registered file with the driver, proceed and load the proxy server thread back in the C# Application.
        FILE_LOG(linfo) << "FilediskProxyNative::init_ctx->InitializeDevice->DeviceIoControl:IOCTL_REGISTER_FILE success 4 at " << link;

        // success
        FILE_LOG(linfo) << "FilediskProxyNative::init_ctx->InitializeDevice completed successfully.";
        return TRUE;
    }

    // load kernel driver's events
    BOOL FilediskProxyNative::OpenKernelDriverEvents(int64_t ctxref)
    {
        MYCONTEXTCONFIG* ctx = (MYCONTEXTCONFIG*)ctxref;

        FILE_LOG(linfo) << "FilediskProxyNative::init_ctx->OpenKernelDriverEvents started";

        char link[512];
        int devicenumber = ctx->DeviceNumber;

        // Event 1
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

        // Event 2
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

        // Event 3
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

        // Event 4
        memset(link, 0, 512);
        sprintf_s(link, 512, "%s%u", USERMODEAPP_SHUTDOWNEVENT_NAME, devicenumber);
        wcharlink = commonMethods::ConvertCharToUnicode(link, 512);
        ctx->Shutdown = OpenEvent(EVENT_ALL_ACCESS, FALSE, wcharlink);

        // delete it
        delete[] wcharlink;

        if (ctx->Shutdown == NULL)
        {
            FILE_LOG(linfo) << "FilediskProxyNative::init_ctx->OpenKernelDriverEvents: Shutdown Event open failure";
            return FALSE;
        }

        // Event 5
        memset(link, 0, 512);
        sprintf_s(link, 512, "%s%u", USERMODEAPP_SHUTDOWNCOMPLETEEVENT_NAME, devicenumber);
        wcharlink = commonMethods::ConvertCharToUnicode(link, 512);
        ctx->ShutdownComplete = OpenEvent(EVENT_ALL_ACCESS, FALSE, wcharlink);

        // delete it
        delete[] wcharlink;

        if (ctx->ShutdownComplete == NULL)
        {
            FILE_LOG(linfo) << "FilediskProxyNative::init_ctx->OpenKernelDriverEvents: ShutdownComplete Event open failure";
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

    BOOL FilediskProxyNative::CreateIoPipe(int64_t ctxref)
    {
        MYCONTEXTCONFIG* ctx = (MYCONTEXTCONFIG*)ctxref;

        FILE_LOG(linfo) << "FilediskProxyNative::init_ctx->CreateIoPipe started";

        char link[512];
        WCHAR* wcharlink;
        int devicenumber = ctx->DeviceNumber;
        memset(link, 0, 512);
        sprintf_s(link, 512, "%s%u", PIPE_NAME_USERMODEAPP, devicenumber);
        wcharlink = commonMethods::ConvertCharToUnicode(link, 512);

        // Setup the named pipe with a security attribute so it is open to anyone that enquires.
        SECURITY_ATTRIBUTES sa;
        SECURITY_DESCRIPTOR sd;
        InitializeSecurityDescriptor(&sd, SECURITY_DESCRIPTOR_REVISION);
        SetSecurityDescriptorDacl(&sd, TRUE, (PACL)NULL, FALSE);
        sa.nLength = (DWORD)sizeof(SECURITY_ATTRIBUTES);
        sa.lpSecurityDescriptor = (LPVOID)&sd;
        sa.bInheritHandle = TRUE;

        ctx->pipe = CreateNamedPipe(
            wcharlink, // name of the pipe
            PIPE_ACCESS_DUPLEX, // 2 way pipe
//            PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT | PIPE_ACCEPT_REMOTE_CLIENTS, //| PIPE_READMODE_BYTE | PIPE_WAIT,// | FILE_FLAG_WRITE_THROUGH, // send data as a byte stream
            PIPE_TYPE_BYTE | PIPE_READMODE_BYTE | PIPE_WAIT,// | FILE_FLAG_WRITE_THROUGH, // send data as a byte stream
            1, // only allow 1 instance of this pipe
            DEVICE_OBJECT_SHM_SIZE_BYTES, // no outbound buffer
            DEVICE_OBJECT_SHM_SIZE_BYTES, // no inbound buffer
            NMPWAIT_USE_DEFAULT_WAIT, //0, // use default wait time
            &sa // use default security attributes
        );
        if (ctx->pipe == NULL || ctx->pipe == INVALID_HANDLE_VALUE)
        {
            FILE_LOG(linfo) << "FilediskProxyNative::init_ctx->CreateIoPipe->CreateNamedPipe::requestPipe creation failure.";
            delete[] wcharlink;
            return FALSE;
        }

        //success

        delete[] wcharlink;

        FILE_LOG(linfo) << "FilediskProxyNative::init_ctx->CreateIoPipe completed successfully. server-pipe i/o pipe created.";
        return TRUE;

    }

    // checks if a socket port is free or is in use
    BOOL FilediskProxyNative::CheckSocketPortFree(ULONG port)
    {
        SOCKET ListenSocket = INVALID_SOCKET;
        WSADATA wsaData;
        int iResult;

        struct addrinfo* result = NULL;
        struct addrinfo hints;

        // Initialize Winsock
        iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
        if (iResult != 0) {
            return FALSE;
        }

        ZeroMemory(&hints, sizeof(hints));
        hints.ai_family = AF_INET;
        hints.ai_socktype = SOCK_STREAM;
        hints.ai_protocol = IPPROTO_TCP;
        hints.ai_flags = AI_PASSIVE;

                // port 1 - request
        char portString[8];
        std::sprintf(portString, "%u", port);

                // Resolve the server address and port
        iResult = getaddrinfo(NULL, portString, &hints, &result);
        if (iResult != 0) {
            WSACleanup();
            return FALSE;
        }


        // Create a SOCKET for connecting to server
        ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
        if (ListenSocket == INVALID_SOCKET) {
            freeaddrinfo(result);
            WSACleanup();
            return FALSE;

        }

        // Setup the TCP listening socket
        iResult = bind(ListenSocket, (SOCKADDR*)result->ai_addr, (int)result->ai_addrlen);
        if (iResult == SOCKET_ERROR) {
            freeaddrinfo(result);
            closesocket(ListenSocket);
            WSACleanup();
            return FALSE;
        }

        // success, port was free
        freeaddrinfo(result);
        closesocket(ListenSocket);
        WSACleanup();
        return TRUE;
    }

    // create and initialize socket and port
    BOOL FilediskProxyNative::CreateSocketServer(int64_t ctxref)
    {
        MYCONTEXTCONFIG* ctx = (MYCONTEXTCONFIG*)ctxref;

        FILE_LOG(linfo) << "FilediskProxyNative::init_ctx->CreateSocketServer started";

        WSADATA wsaData;
        int iResult;

        struct addrinfo* result = NULL;
        struct addrinfo hints;

        // Initialize Winsock
        iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
        if (iResult != 0) {
            FILE_LOG(linfo) << "FilediskProxyNative::init_ctx->CreateSocketServer->WSAStartup method failure.";
            return FALSE;
        }

        ZeroMemory(&hints, sizeof(hints));
        hints.ai_family = AF_INET;
        hints.ai_socktype = SOCK_STREAM;
        hints.ai_protocol = IPPROTO_TCP;
        hints.ai_flags = AI_PASSIVE;

        // port 1 - request

        // Resolve the server address and port
        iResult = getaddrinfo(NULL, ctx->port, &hints, &result);
        if (iResult != 0) {
            FILE_LOG(linfo) << "FilediskProxyNative::init_ctx->CreateSocketServer->getaddrinfo method failure.";
            WSACleanup();
            return FALSE;
        }

       
        // Create a SOCKET for connecting to server
        ctx->ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
        if (ctx->ListenSocket == INVALID_SOCKET) {
            FILE_LOG(linfo) << "FilediskProxyNative::init_ctx->CreateSocketServer->socket method failure.";
            freeaddrinfo(result);
            WSACleanup();
            return FALSE;

        }

        // Setup the TCP listening socket
        iResult = bind(ctx->ListenSocket, result->ai_addr, (int)result->ai_addrlen);
        if (iResult == SOCKET_ERROR) {
            FILE_LOG(linfo) << "FilediskProxyNative::init_ctx->CreateSocketServer->bind method failure.";
            freeaddrinfo(result);
            closesocket(ctx->ListenSocket);
            WSACleanup();
            return FALSE;
        }

        freeaddrinfo(result);

        // now start listening on this socket
        iResult = listen(ctx->ListenSocket, SOMAXCONN);
        FILE_LOG(linfo) << "FilediskProxyNative::init_ctx->CreateSocketServer->Listen: listening at port: " << ctx->port;

        // completed.
        FILE_LOG(linfo) << "FilediskProxyNative::init_ctx->CreateSocketServer completed successfully. socket port created and initialized.";
        return TRUE;
    }

    // initialize open the temporary file mode
    BOOL FilediskProxyNative::InitializeFileMode(int64_t ctxref)
    {
        // todo: we need to open the device through the drive letter link
        DWORD BytesReturned;

        // file mode shares a single local user path trasmission file between both driver and user mode app for all interations.

        FILE_LOG(linfo) << "FilediskProxyNative::init_ctx->InitializeFileMode started";

        MYCONTEXTCONFIG* ctx = (MYCONTEXTCONFIG*)ctxref;

        // Setup the security attribute so it is open to anyone that enquires.
        SECURITY_ATTRIBUTES sa;
        SECURITY_DESCRIPTOR sd;
        InitializeSecurityDescriptor(&sd, SECURITY_DESCRIPTOR_REVISION);
        SetSecurityDescriptorDacl(&sd, TRUE, (PACL)NULL, FALSE);
        sa.nLength = (DWORD)sizeof(SECURITY_ATTRIBUTES);
        sa.lpSecurityDescriptor = (LPVOID)&sd;
        sa.bInheritHandle = TRUE;

        // find if this drive letter isn't in use, otherwise return with error.
        HANDLE file = CreateFileA(
            (LPCSTR)ctx->useFileValue,
            GENERIC_READ | GENERIC_WRITE,
            FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
            &sa,
            OPEN_EXISTING,
            FILE_FLAG_WRITE_THROUGH | FILE_ATTRIBUTE_NORMAL | FILE_FLAG_RANDOM_ACCESS,
            NULL
        );

        if (file == INVALID_HANDLE_VALUE)
        {
            // drive letter is busy. abort with error.
            FILE_LOG(linfo) << "FilediskProxyNative::init_ctx->InitializeFileMode->CreateFileA file open error. aborted.";
            return FALSE;
        }

        // success
        ctx->useFileHandle = file;
        FILE_LOG(linfo) << "FilediskProxyNative::init_ctx->InitializeFileMode completed successfully.";
        return TRUE;
    }

    // set/reset Event
    void FilediskProxyNative::PulseEventDriverRequestDataSet(int64_t ctxref)
    {
        MYCONTEXTCONFIG* ctx = (MYCONTEXTCONFIG*)ctxref;
        PulseEvent(ctx->DriverRequestDataSet);
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
    void FilediskProxyNative::PulseEventProxyIdle(int64_t ctxref)
    {
        MYCONTEXTCONFIG* ctx = (MYCONTEXTCONFIG*)ctxref;
        PulseEvent(ctx->ProxyIdle);
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
    void FilediskProxyNative::PulseEventRequestComplete(int64_t ctxref)
    {
        MYCONTEXTCONFIG* ctx = (MYCONTEXTCONFIG*)ctxref;
        PulseEvent(ctx->RequestComplete);
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

    // set/reset Event
    void FilediskProxyNative::PulseEventShutdown(int64_t ctxref)
    {
        MYCONTEXTCONFIG* ctx = (MYCONTEXTCONFIG*)ctxref;
        PulseEvent(ctx->Shutdown);
    }

    // set/reset Event
    void FilediskProxyNative::SetEventShutdown(int64_t ctxref, BOOL set)
    {
        MYCONTEXTCONFIG* ctx = (MYCONTEXTCONFIG*)ctxref;

        if (set)
            SetEvent(ctx->Shutdown);
        else
            ResetEvent(ctx->Shutdown);
    }

    // set/reset Event
    void FilediskProxyNative::PulseEventShutdownComplete(int64_t ctxref)
    {
        MYCONTEXTCONFIG* ctx = (MYCONTEXTCONFIG*)ctxref;
        PulseEvent(ctx->ShutdownComplete);
    }

    // set/reset Event
    void FilediskProxyNative::SetEventShutdownComplete(int64_t ctxref, BOOL set)
    {
        MYCONTEXTCONFIG* ctx = (MYCONTEXTCONFIG*)ctxref;

        if (set)
            SetEvent(ctx->ShutdownComplete);
        else
            ResetEvent(ctx->ShutdownComplete);
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

    DWORD FilediskProxyNative::WaitEventShutdownComplete(int64_t ctxref, DWORD miliSeconds)
    {
        MYCONTEXTCONFIG* ctx = (MYCONTEXTCONFIG*)ctxref;
        return WaitForSingleObject(ctx->ShutdownComplete, miliSeconds);
    }

    int FilediskProxyNative::isEventSignalled(HANDLE hEvent)
    {
        DWORD WaitStatus = WaitForSingleObject(hEvent, 0);
        if (WaitStatus == WAIT_OBJECT_0)
        {
            // success, the event was signalled
            return TRUE;
        }
        else if (WaitStatus == WAIT_TIMEOUT)
        {
            // the event wasn't signelled, so return error
            return FALSE;
        }
        else
        {
            // error or exception, continue
            return -2;
        }

    }

    void FilediskProxyNative::GetFileModeHeader(int64_t ctxref, OUT int64_t& byteOffset, OUT DWORD& length, OUT UCHAR& function, OUT DWORD& totalBytesReadWrite)
    {
        MYCONTEXTCONFIG* ctx = (MYCONTEXTCONFIG*)ctxref;

        // reset and clean the context configuration
        memset(&ctx->__requestBuffer, 0, REQUEST_BUFFER_SIZE);
        ctx->requestSet = false;

        LARGE_INTEGER ptr = { 0 };
        ptr.LowPart = ptr.HighPart = 0;             
        LARGE_INTEGER newptr = { 0 };
        SetFilePointerEx(ctx->useFileHandle, ptr, &newptr, FILE_BEGIN);

        // success, now receive the request
        BOOL result = ReadFile(ctx->useFileHandle, &ctx->__requestBuffer, REQUEST_BUFFER_SIZE + 1, &ctx->request->totalBytesReadWrite, NULL);
        ctx->requestSet = true;

        // set output destinations with received request
        byteOffset = ctx->request->ByteOffset;
        length = ctx->request->Length;
        function = ctx->request->MajorFunction;
        totalBytesReadWrite = ctx->request->totalBytesReadWrite;
    }

    void FilediskProxyNative::ReadFileMode(int64_t ctxref, void* outputBuffer, size_t length)
    {
        DWORD bytesDone;
        MYCONTEXTCONFIG* ctx = (MYCONTEXTCONFIG*)ctxref;

        LARGE_INTEGER ptr = { REQUEST_BUFFER_SIZE + 1 };
        LARGE_INTEGER newptr = { 0 };
        SetFilePointerEx(ctx->useFileHandle, ptr, &newptr, FILE_BEGIN);

        ReadFile(ctx->useFileHandle, outputBuffer, length, &bytesDone, NULL);
    }

    void FilediskProxyNative::WriteFileMode(int64_t ctxref, void* inputBuffer, size_t length)
    {
        DWORD bytesDone;
        MYCONTEXTCONFIG* ctx = (MYCONTEXTCONFIG*)ctxref;

        LARGE_INTEGER ptr = { REQUEST_BUFFER_SIZE + 1 };
        LARGE_INTEGER newptr = { 0 };
        SetFilePointerEx(ctx->useFileHandle, ptr, &newptr, FILE_BEGIN);

        WriteFile(ctx->useFileHandle, inputBuffer, length, &bytesDone, NULL);
        FlushFileBuffers(ctx->useFileHandle);
    }

    void FilediskProxyNative::GetSHMHeader(int64_t ctxref, OUT int64_t& byteOffset, OUT DWORD& length, OUT UCHAR& function, OUT DWORD& totalBytesReadWrite)
    {
        MYCONTEXTCONFIG* ctx = (MYCONTEXTCONFIG*)ctxref;
        PCONTEXT_REQUEST request = (PCONTEXT_REQUEST)ctx->shmMappedBuffer;
        byteOffset = request->ByteOffset;
        length = request->Length;
        function = request->MajorFunction;
        totalBytesReadWrite = request->totalBytesReadWrite;
    }

    void FilediskProxyNative::SetSHMHeader(int64_t ctxref, int64_t byteOffset, DWORD length, UCHAR function, DWORD totalBytesReadWrite)
    {
        MYCONTEXTCONFIG* ctx = (MYCONTEXTCONFIG*)ctxref;
        PCONTEXT_REQUEST request = (PCONTEXT_REQUEST)ctx->shmMappedBuffer;
        
        if (byteOffset != NULL)
            request->ByteOffset = byteOffset;

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

    void FilediskProxyNative::ReadPipe(int64_t ctxref, void* outputBuffer, size_t length)
    {
        DWORD bytesDone;
        MYCONTEXTCONFIG* ctx = (MYCONTEXTCONFIG*)ctxref;

        ReadFile(ctx->pipe, outputBuffer, length, &bytesDone, NULL);
   //     commonMethods::fastPipeToBuffer(outputBuffer, ctx->pipe, 0, length);
        //FILE_LOG(linfo) << "ioPipe::ReadPipe at " << ctx->request->ByteOffset;
    }

    void FilediskProxyNative::WritePipe(int64_t ctxref, void* inputBuffer, size_t length)
    {
        DWORD bytesDone;
        MYCONTEXTCONFIG* ctx = (MYCONTEXTCONFIG*)ctxref;

        WriteFile(ctx->pipe, inputBuffer, length, &bytesDone, NULL);
        FlushFileBuffers(ctx->pipe);
 //       commonMethods::fastBufferToPipe(inputBuffer, ctx->pipe, 0, length);
        //FILE_LOG(linfo) << "WritePipe at " << ctx->request->ByteOffset;
    }

    BOOL FilediskProxyNative::Step1PipeGetRequest(int64_t ctxref, OUT uint64_t& byteOffset, OUT DWORD& length, OUT UCHAR& function, OUT DWORD& totalBytesReadWrite)
    {
        BOOL result;
        MYCONTEXTCONFIG* ctx = (MYCONTEXTCONFIG*)ctxref;

        // reset and clean the context configuration
        memset(&ctx->__requestBuffer, 0, REQUEST_BUFFER_SIZE);
        ctx->requestSet = false;

        // success, now receive the request
        result = ReadFile(ctx->pipe, &ctx->__requestBuffer, sizeof(CONTEXT_REQUEST), &ctx->request->totalBytesReadWrite, NULL);
        ctx->requestSet = true;

        // set output destinations with received request
        byteOffset = ctx->request->ByteOffset;
        length = ctx->request->Length;
        function = ctx->request->MajorFunction;
        totalBytesReadWrite = ctx->request->totalBytesReadWrite;

        /*
        if (function == IRP_MJ_READ)
            FILE_LOG(linfo) << "Step1PipeGetRequest::IRP_MJ_READ";
        else
            FILE_LOG(linfo) << "Step1PipeGetRequest::IRP_MJ_WRITE";

        if (ctx->request->signature == DRIVER_SIGNATURE)
        {
            FILE_LOG(linfo) << "Step1PipeGetRequest: driver's signature matched.";
        }
        else
        {
            FILE_LOG(linfo) << "Step1PipeGetRequest: driver's signature did not match. failure...";
        }
        */
        return TRUE;
    }

    int FilediskProxyNative::ConnectPipe(int64_t ctxref)
    {
        MYCONTEXTCONFIG* ctx = (MYCONTEXTCONFIG*)ctxref;

        if (ctx->pipe == NULL)
            return -2;

        // This call blocks until a client process connects to the pipe
        BOOL status = ConnectNamedPipe(ctx->pipe, NULL);
        if (status == FALSE)
            FILE_LOG(linfo) << "ConnectPipe failure";
        return status;
    }

    int FilediskProxyNative::DisconnectPipe(int64_t ctxref)
    {
        MYCONTEXTCONFIG* ctx = (MYCONTEXTCONFIG*)ctxref;

        if (ctx->pipe == NULL)
            return -2;

        // completed. disconnect and go to the next round which is i/o
        return DisconnectNamedPipe(ctx->pipe);
    }

    BOOL FilediskProxyNative::Step1SocketGetRequest(int64_t ctxref, OUT uint64_t& byteOffset, OUT DWORD& length, OUT UCHAR& function, OUT DWORD& totalBytesReadWrite)
    {
        BOOL result;
        MYCONTEXTCONFIG* ctx = (MYCONTEXTCONFIG*)ctxref;

        // reset and clean the context configuration
        memset(&ctx->__requestBuffer, 0, REQUEST_BUFFER_SIZE);
        ctx->requestSet = false;

        // Step 1: receive request
        //original working: int iResult = recv(ctx->ClientSocket, (char*)&ctx->__requestBuffer, REQUEST_BUFFER_SIZE, MSG_WAITALL);
        ReadSocketComplete(ctxref, &ctx->__requestBuffer, REQUEST_BUFFER_SIZE);
        ctx->request = (PCONTEXT_REQUEST)&ctx->__requestBuffer;
        ctx->requestSet = true;

        // set output destinations with received request
        byteOffset = ctx->request->ByteOffset;
        length = ctx->request->Length;
        function = ctx->request->MajorFunction;
        totalBytesReadWrite = ctx->request->totalBytesReadWrite;
        FILE_LOG(linfo) << "Step1SocketGetRequest completed. request offset: " << byteOffset << " and length: " << length;
        return TRUE;
    }

    void FilediskProxyNative::ReadSocketComplete(int64_t ctxref, void* outputBuffer, size_t length)
    {
        MYCONTEXTCONFIG* ctx = (MYCONTEXTCONFIG*)ctxref;

        uint64_t totalVectors = 1;
        size_t stride = totalVectors * SOCKET_BUFFER_SIZE;
        char* dstptr = (char*)outputBuffer;

        while (length)
        {
            int sockResult = recv(ctx->ClientSocket, dstptr, stride, 0);
            if (sockResult <= 0)
            {
                continue;
            }
            dstptr += sockResult;
            length -= sockResult;
            if (length < SOCKET_BUFFER_SIZE)
                stride = length;
        }        
    }

    void FilediskProxyNative::ReadSocket(int64_t ctxref, void* outputBuffer, size_t length)
    {
        MYCONTEXTCONFIG* ctx = (MYCONTEXTCONFIG*)ctxref;
        //int sockResult = recv(ctx->ClientSocket, (char*)outputBuffer, length, MSG_WAITALL);
        ReadSocketComplete(ctxref, outputBuffer, length);
        FILE_LOG(linfo) << "ReadSocket completed";
    }

    void FilediskProxyNative::WriteSocket(int64_t ctxref, void* inputBuffer, size_t length)
    {
        MYCONTEXTCONFIG* ctx = (MYCONTEXTCONFIG*)ctxref;
        int sockResult = send(ctx->ClientSocket, (const char*)inputBuffer, length, 0);
        FILE_LOG(linfo) << "WriteSocket completed";
    }

    // Accept a client socket
    void FilediskProxyNative::SocketAcceptClient(int64_t ctxref)
    {
        // note that this is a blocking call
        MYCONTEXTCONFIG* ctx = (MYCONTEXTCONFIG*)ctxref;
        ctx->ClientSocket = accept(ctx->ListenSocket, NULL, NULL);
        FILE_LOG(linfo) << "SocketAcceptClient completed";
    }

    void FilediskProxyNative::CloseClientSocket(int64_t ctxref)
    {
        // note that this is a blocking call
        MYCONTEXTCONFIG* ctx = (MYCONTEXTCONFIG*)ctxref;
        closesocket(ctx->ClientSocket);
    }

}
