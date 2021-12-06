/*
    This is a virtual disk driver for Windows that uses one or more files to
    emulate physical disks.
    Copyright (C) 1999-2015 Bo Brantén.
    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.
    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.
    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/
#include <ntifs.h>
#include <ntdddisk.h>
#include <ntddcdrm.h>
#include <ntstrsafe.h>
#include <wdmsec.h>
#include <mountmgr.h>
#include <ntddvol.h>
#include <ntddscsi.h>
#include <windef.h>
#include <tchar.h>
#include <wchar.h>
#include <math.h>
#include ".\SHM\helper.h"
#include <wsk.h>

#include ".\ksocket\ksocket.h"
#include ".\ksocket\berkeley.h"

NTSYSAPI
NTSTATUS
NTAPI
ZwOpenProcessToken(
    IN HANDLE       ProcessHandle,
    IN ACCESS_MASK  DesiredAccess,
    OUT PHANDLE     TokenHandle
);

NTSYSAPI
NTSTATUS
NTAPI
ZwAdjustPrivilegesToken(
    IN HANDLE               TokenHandle,
    IN BOOLEAN              DisableAllPrivileges,
    IN PTOKEN_PRIVILEGES    NewState,
    IN ULONG                BufferLength,
    OUT PTOKEN_PRIVILEGES   PreviousState OPTIONAL,
    OUT PULONG              ReturnLength
);

#include "FileDiskShared.h"

#define PARAMETER_KEY           L"\\Parameters"

#define NUMBEROFDEVICES_VALUE   L"NumberOfDevices"

#define DEFAULT_NUMBEROFDEVICES 5

#define TOC_DATA_TRACK          0x04

HANDLE dir_handle;

typedef struct _DEVICE_EXTENSION {
    BOOLEAN                     media_in_device;
    UNICODE_STRING              device_name;
    ULONG                       device_number;
    DEVICE_TYPE                 device_type;
    ANSI_STRING                 file_name;
    LARGE_INTEGER               file_size;
    BOOLEAN                     read_only;
    PSECURITY_CLIENT_CONTEXT    security_client_context;
    LIST_ENTRY                  list_head;
    KSPIN_LOCK                  list_lock;
    KEVENT                      request_event;
    PVOID                       thread_pointer;
    BOOLEAN                     terminate_thread;
    //BOOLEAN                     fileClosingNow;
    BOOLEAN                     isFoundationDevice;
    HANDLE                      LogFileDevice;
    BOOL                        usePipe;
    HANDLE                      pipe;
    BOOL                        useShm;
    BOOL                        useSocket;
    int                         sock;
    ULONG                       lport;
    PCONTEXT_REQUEST            request;

    // driver's shared memory
    PVOID                   	g_pSharedSection;
    PVOID                   	g_pSectionObj;
    HANDLE                  	g_hSection;
    PVOID                   	pDriverRequestDataSetObj;
    HANDLE                      DriverRequestDataSet;
    PKEVENT                     KeDriverRequestDataSetObj;
    PVOID                   	pSharedProxyIdle;
    PVOID                   	pProxyIdleObj;
    HANDLE                      ProxyIdle;
    PKEVENT                     KeProxyIdleObj;
    PVOID                   	pSharedRequestComplete;
    PVOID                   	pRequestCompleteObj;
    HANDLE                      RequestComplete;
    PKEVENT                     KeRequestCompleteObj;
    PVOID                   	pShutdownObj;
    HANDLE                      Shutdown;
    PKEVENT                     KeShutdownObj;
    PVOID                   	pShutdownCompleteObj;
    HANDLE                      ShutdownComplete;
    PKEVENT                     KeShutdownCompleteObj;

    // pipelines
    //HANDLE                      hReqServerPipe;
} DEVICE_EXTENSION, * PDEVICE_EXTENSION;

#ifdef _PREFAST_
DRIVER_INITIALIZE DriverEntry;
__drv_dispatchType(IRP_MJ_CREATE) __drv_dispatchType(IRP_MJ_CLOSE) DRIVER_DISPATCH FileDiskCreateClose;
__drv_dispatchType(IRP_MJ_READ) __drv_dispatchType(IRP_MJ_WRITE) DRIVER_DISPATCH FileDiskReadWrite;
__drv_dispatchType(IRP_MJ_DEVICE_CONTROL) DRIVER_DISPATCH FileDiskDeviceControl;
KSTART_ROUTINE FileDiskThread;
DRIVER_UNLOAD FileDiskUnload;
#endif // _PREFAST_

NTSTATUS FileDiskCreateFoundationDevice(IN PDRIVER_OBJECT   DriverObject);
NTSTATUS CreateSharedMemory(PDEVICE_OBJECT device_object);
void ReleaseSharedMemory(PDEVICE_OBJECT device_object);
NTSTATUS CreateSharedEvents(PDEVICE_OBJECT device_object);
NTSTATUS CreateSharedEventsKe(PDEVICE_OBJECT device_object);
void DeleteSharedEvents(PDEVICE_OBJECT device_object);
NTSTATUS SetSecurityAllAccess(HANDLE h, PVOID obj);
NTSTATUS openPipe(PDEVICE_OBJECT device_object);
NTSTATUS closePipe(PDEVICE_OBJECT device_object);
void printStatusFile(char* path, char* status);
//VOID CancelIrpRoutine(IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp);
//NTSTATUS DispatchCleanup(PDEVICE_OBJECT fdo, PIRP Irp);
//NTSTATUS CompleteRequest(PIRP Irp, NTSTATUS status, ULONG_PTR Information);
//VOID FileDiskClearQueue(IN PVOID Context);
VOID FileDiskFreeIrpWithMdls(PIRP Irp);
HANDLE initLog(char* file, ULONG DeviceNumber);
VOID Log(HANDLE handle, char* text);
VOID freeLog(HANDLE handle);
void ReadPipeToBuffer(LPBYTE buffer, HANDLE pipe, size_t size);
void fastPipeToBuffer(LPVOID buffer, HANDLE pipe, size_t pos, size_t nBytes);
void WriteBufferToPipe(LPBYTE buffer, HANDLE pipe, size_t size);
void fastBufferToPipe(LPVOID buffer, HANDLE pipe, size_t pos, size_t nBytes);
NTSTATUS FileDiskForceCloseFile(IN PDEVICE_OBJECT DeviceObject);
BOOL isEventSignalled(PKEVENT KeEvent);

NTSTATUS
FileDiskDeregisterFileFromDevice(
    IN PDRIVER_OBJECT DriverObject,
    ULONG DeviceNumber);

int
FileDiskFindFirstAvailableDevice(
    IN PDRIVER_OBJECT DriverObject
);

NTSTATUS
DriverEntry(
    IN PDRIVER_OBJECT   DriverObject,
    IN PUNICODE_STRING  RegistryPath
);

NTSTATUS
FileDiskCreateDevice(
    IN PDRIVER_OBJECT   DriverObject,
    IN ULONG            Number,
    IN DEVICE_TYPE      DeviceType
);

VOID
FileDiskUnload(
    IN PDRIVER_OBJECT   DriverObject
);

PDEVICE_OBJECT
FileDiskDeleteDevice(
    IN PDEVICE_OBJECT   DeviceObject
);

NTSTATUS
FileDiskCreateClose(
    IN PDEVICE_OBJECT   DeviceObject,
    IN PIRP             Irp
);

NTSTATUS
FileDiskReadWrite(
    IN PDEVICE_OBJECT   DeviceObject,
    IN PIRP             Irp
);

NTSTATUS
FileDiskDeviceControl(
    IN PDEVICE_OBJECT   DeviceObject,
    IN PIRP             Irp
);

VOID
FileDiskThread(
    IN PVOID            Context
);

NTSTATUS
FileDiskOpenFile(
    IN PDEVICE_OBJECT   DeviceObject,
    IN PIRP             Irp
);

NTSTATUS
FileDiskCloseFile(
    IN PDEVICE_OBJECT   DeviceObject,
    IN PIRP             Irp
);

NTSTATUS
FileDiskAdjustPrivilege(
    IN ULONG            Privilege,
    IN BOOLEAN          Enable
);

#pragma code_seg("INIT")

NTSTATUS
DriverEntry(
    IN PDRIVER_OBJECT   DriverObject,
    IN PUNICODE_STRING  RegistryPath
)
{
    UNICODE_STRING              parameter_path;
    RTL_QUERY_REGISTRY_TABLE    query_table[2];
    ULONG                       n_devices;
    NTSTATUS                    status;
    UNICODE_STRING              device_dir_name;
    OBJECT_ATTRIBUTES           object_attributes;
    ULONG                       n;
    USHORT                      n_created_devices;

    NTSTATUS Status;

    //*****************
    //
    // Initialize KSOCKET.
    // todo tushar

    Status = KsInitialize();

    if (!NT_SUCCESS(Status))
    {
        return Status;
    }
    //*****************

    parameter_path.Length = 0;

    parameter_path.MaximumLength = RegistryPath->Length + sizeof(PARAMETER_KEY);

    parameter_path.Buffer = (PWSTR)ExAllocatePoolWithTag(PagedPool, parameter_path.MaximumLength, FILE_DISK_POOL_TAG);

    if (parameter_path.Buffer == NULL)
    {
        return STATUS_INSUFFICIENT_RESOURCES;
    }

    RtlCopyUnicodeString(&parameter_path, RegistryPath);

    RtlAppendUnicodeToString(&parameter_path, PARAMETER_KEY);

    RtlZeroMemory(&query_table[0], sizeof(query_table));

    query_table[0].Flags = RTL_QUERY_REGISTRY_DIRECT | RTL_QUERY_REGISTRY_REQUIRED;
    query_table[0].Name = NUMBEROFDEVICES_VALUE;
    query_table[0].EntryContext = &n_devices;

    status = RtlQueryRegistryValues(
        RTL_REGISTRY_ABSOLUTE,
        parameter_path.Buffer,
        &query_table[0],
        NULL,
        NULL
    );

    ExFreePool(parameter_path.Buffer);

    if (!NT_SUCCESS(status))
    {
        DbgPrint("FileDisk: Query registry failed, using default values.\n");
        n_devices = DEFAULT_NUMBEROFDEVICES;
    }

    RtlInitUnicodeString(&device_dir_name, DEVICE_DIR_NAME);

    InitializeObjectAttributes(
        &object_attributes,
        &device_dir_name,
        OBJ_PERMANENT,
        NULL,
        NULL
    );

    status = ZwCreateDirectoryObject(
        &dir_handle,
        DIRECTORY_ALL_ACCESS,
        &object_attributes
    );

    if (!NT_SUCCESS(status))
    {
        return status;
    }

    ZwMakeTemporaryObject(dir_handle);

    for (n = 0, n_created_devices = 0; n < n_devices; n++)
    {
        status = FileDiskCreateDevice(DriverObject, n, FILE_DEVICE_DISK);

        if (NT_SUCCESS(status))
        {
            n_created_devices++;
        }
    }

    /* invalid original code
    for (n = 0; n < n_devices; n++)
    {
        status = FileDiskCreateDevice(DriverObject, n, FILE_DEVICE_CD_ROM);

        if (NT_SUCCESS(status))
        {
            n_created_devices++;
        }
    }
    */

    if (n_created_devices == 0)
    {
        ZwClose(dir_handle);
        return status;
    }
   

    DriverObject->MajorFunction[IRP_MJ_CREATE] = FileDiskCreateClose;
    DriverObject->MajorFunction[IRP_MJ_CLOSE] = FileDiskCreateClose;
    DriverObject->MajorFunction[IRP_MJ_READ] = FileDiskReadWrite;
    DriverObject->MajorFunction[IRP_MJ_WRITE] = FileDiskReadWrite;
    DriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL] = FileDiskDeviceControl;
    //DriverObject->MajorFunction[IRP_MJ_CLEANUP] = DispatchCleanup;

    DriverObject->DriverUnload = FileDiskUnload;

    return STATUS_SUCCESS;
}

NTSTATUS
FileDiskCreateFoundationDevice(
    IN PDRIVER_OBJECT   DriverObject)
{

}

NTSTATUS
FileDiskCreateDevice(
    IN PDRIVER_OBJECT   DriverObject,
    IN ULONG            Number,
    IN DEVICE_TYPE      DeviceType
)
{
    UNICODE_STRING      device_name;
    NTSTATUS            status;
    PDEVICE_OBJECT      device_object;
    PDEVICE_EXTENSION   device_extension;
    HANDLE              thread_handle;
    UNICODE_STRING      sddl;

    ASSERT(DriverObject != NULL);
    
    device_name.Buffer = (PWCHAR)ExAllocatePoolWithTag(PagedPool, MAXIMUM_FILENAME_LENGTH * 2, FILE_DISK_POOL_TAG);

    if (device_name.Buffer == NULL)
    {
        return STATUS_INSUFFICIENT_RESOURCES;
    }

    device_name.Length = 0;
    device_name.MaximumLength = MAXIMUM_FILENAME_LENGTH * 2;

    /* original
    if (DeviceType == FILE_DEVICE_CD_ROM)
    {
        RtlUnicodeStringPrintf(&device_name, DEVICE_NAME_PREFIX L"Cd" L"%u", Number);
    }
    else
    {
        RtlUnicodeStringPrintf(&device_name, DEVICE_NAME_PREFIX L"%u", Number);
    }
    */
    // todo tushar
    RtlUnicodeStringPrintf(&device_name, DEVICE_NAME_PREFIX L"%u", Number);

    /* original
    RtlInitUnicodeString(&sddl, _T("D:P(A;;GA;;;SY)(A;;GA;;;BA)(A;;GA;;;BU)"));

    
    status = IoCreateDeviceSecure(
        DriverObject,
        sizeof(DEVICE_EXTENSION),
        &device_name,
        DeviceType,
        0,
        FALSE,
        &sddl,
        NULL,
        &device_object
    );
    */

    // todo tushar: create the base base command and request device (this isn't any virtual drive)
    // device at index 0, is the base query command and request device
    if (Number == 0)
    {
        status = IoCreateDevice(
            DriverObject,
            sizeof(DEVICE_EXTENSION),
            &device_name,
            FILE_DEVICE_UNKNOWN,
            0, 
            FALSE,
            &device_object
        );
    }
    else
    {
        status = IoCreateDevice(
            DriverObject,
            sizeof(DEVICE_EXTENSION),
            &device_name,
            DeviceType,
            0,
            FALSE,
            &device_object
        );
    }
    
    if (!NT_SUCCESS(status))
    {
        ExFreePool(device_name.Buffer);
        return status;
    }

    device_object->Flags |= DO_DIRECT_IO;

    device_extension = (PDEVICE_EXTENSION)device_object->DeviceExtension;

    // todo tushar
    if (Number == 0)
    {
        device_extension->isFoundationDevice = TRUE;
    }
    else
    {
        device_extension->isFoundationDevice = FALSE;
    }

    device_extension->media_in_device = FALSE;

    device_extension->device_name.Length = device_name.Length;
    device_extension->device_name.MaximumLength = device_name.MaximumLength;
    device_extension->device_name.Buffer = device_name.Buffer;
    device_extension->device_number = Number;
    device_extension->device_type = DeviceType;

    /* original
    if (DeviceType == FILE_DEVICE_CD_ROM)
    {
        device_object->Characteristics |= FILE_READ_ONLY_DEVICE;
        device_extension->read_only = TRUE;
    }
    */

    InitializeListHead(&device_extension->list_head);

    KeInitializeSpinLock(&device_extension->list_lock);

    KeInitializeEvent(
        &device_extension->request_event,
        SynchronizationEvent,
        FALSE
    );

    device_extension->terminate_thread = FALSE;
    
    // TODO tushar: create current device's shared memory
    CreateSharedMemory(device_object);

    // TODO tushar: create current device's events
    CreateSharedEventsKe(device_object);

    // TODO tushar: allocate context request
    device_extension->request = (PCONTEXT_REQUEST)ExAllocatePoolZero(PagedPool, REQUEST_BUFFER_SIZE + 1, FILE_DISK_POOL_TAG);

    // TODO tushar: now initialize device number based log file
    device_extension->LogFileDevice = initLog(TEXT(LOG_FILE_NAME_PATH), Number);
    Log(device_extension->LogFileDevice, "hello world");

    status = PsCreateSystemThread(
        &thread_handle,
        (ACCESS_MASK)0L,
        NULL,
        NULL,
        NULL,
        FileDiskThread,
        device_object
    );

    if (!NT_SUCCESS(status))
    {
        IoDeleteDevice(device_object);
        ExFreePool(device_name.Buffer);
        return status;
    }

    status = ObReferenceObjectByHandle(
        thread_handle,
        THREAD_ALL_ACCESS,
        NULL,
        KernelMode,
        &device_extension->thread_pointer,
        NULL
    );

    if (!NT_SUCCESS(status))
    {
        ZwClose(thread_handle);

        device_extension->terminate_thread = TRUE;

        KeSetEvent(
            &device_extension->request_event,
            (KPRIORITY)0,
            FALSE
        );

        IoDeleteDevice(device_object);

        ExFreePool(device_name.Buffer);

        return status;
    }

    ZwClose(thread_handle);

    // todo tushar
    // finally create Base Device's link
    if (Number == 0)
    {
        UNICODE_STRING ustrLinkName;
        UNICODE_STRING ustrDevName;
        RtlInitUnicodeString(&ustrDevName, BASE_DEVICE_NAME);
        RtlInitUnicodeString(&ustrLinkName, BASE_DEVICE_LINK_NAME);
        status = IoCreateSymbolicLink(&ustrLinkName, &ustrDevName);
        if (NT_SUCCESS(status))
        {
            printStatusFile(L"\\DosDevices\\C:\\base-device-creation-success.txt", "base-device-creation-success.");
            DbgPrint("base device 0 and it's link created!\r\n");
        }
        else
        {
            printStatusFile(L"\\DosDevices\\C:\\base-device-creation-failure.txt", "base-device-creation-failure.");
            DbgPrint("failure creating base device 0 and it's link\r\n");
        }
    }

    //INT64 lProcessingBytes = 1.2;
    //char buffer[100];
    //sprintf_s(buffer, 100, "%s%d", "lProcessingBytes:", lProcessingBytes);
    //Log(device_extension->LogFileDevice, buffer);
    return STATUS_SUCCESS;
}

#pragma code_seg("PAGE")

VOID
FileDiskUnload(
    IN PDRIVER_OBJECT DriverObject
)
{
    PDEVICE_OBJECT device_object;

    PAGED_CODE();

    device_object = DriverObject->DeviceObject;

    while (device_object)
    {
        device_object = FileDiskDeleteDevice(device_object);
    }

    ZwClose(dir_handle);
}

int
FileDiskFindFirstAvailableDevice(
    IN PDRIVER_OBJECT DriverObject
)
{
    PDEVICE_OBJECT device_object;
    PDEVICE_EXTENSION   device_extension;

    PAGED_CODE();

    device_object = DriverObject->DeviceObject;
    while (device_object)
    {
        device_extension = (PDEVICE_EXTENSION)device_object->DeviceExtension;
        if (device_extension->media_in_device == FALSE)
        {
            if (!device_extension->isFoundationDevice)
                return device_extension->device_number;
        }

        device_object = device_object->NextDevice; 
    }
    // no available device, so return error
    return -1;
}

NTSTATUS
FileDiskDeregisterFileFromDevice(
    IN PDRIVER_OBJECT DriverObject,
    ULONG DeviceNumber
)
{
    PDEVICE_OBJECT device_object;
    PDEVICE_EXTENSION   device_extension;

    PAGED_CODE();

    device_object = DriverObject->DeviceObject;
    while (device_object)
    {
        device_extension = (PDEVICE_EXTENSION)device_object->DeviceExtension;
        if (device_extension->device_number == DeviceNumber)
        {
            // device found by number. force deregister the file.
            return FileDiskForceCloseFile(device_object);
        }

        device_object = device_object->NextDevice;
    }
    // no available device, so return error
    return STATUS_SUCCESS; // STATUS_UNSUCCESSFUL;
}


PDEVICE_OBJECT
FileDiskDeleteDevice(
    IN PDEVICE_OBJECT DeviceObject
)
{
    PDEVICE_EXTENSION   device_extension;
    PDEVICE_OBJECT      next_device_object;

    PAGED_CODE();

    ASSERT(DeviceObject != NULL);

    device_extension = (PDEVICE_EXTENSION)DeviceObject->DeviceExtension;

    device_extension->terminate_thread = TRUE;

    KeSetEvent(
        &device_extension->request_event,
        (KPRIORITY)0,
        FALSE
    );

    KeWaitForSingleObject(
        device_extension->thread_pointer,
        Executive,
        KernelMode,
        FALSE,
        NULL
    );

    ObDereferenceObject(device_extension->thread_pointer);

    if (device_extension->device_name.Buffer != NULL)
    {
        ExFreePool(device_extension->device_name.Buffer);
    }

    if (device_extension->security_client_context != NULL)
    {
        SeDeleteClientSecurity(device_extension->security_client_context);
        ExFreePool(device_extension->security_client_context);
    }

    // finally delete device's shared memory
    ReleaseSharedMemory(DeviceObject);

    // finally delete device's shared events
    DeleteSharedEvents(DeviceObject);

    // finally delete all other objects
    ExFreePoolWithTag(device_extension->request, FILE_DISK_POOL_TAG);

    // todo tushar
    // finally delete Base Device's link
    if (device_extension->device_number == 0)
    {
        UNICODE_STRING ustrLinkName;
        //UNICODE_STRING ustrDevName;
        //RtlInitUnicodeString(&ustrDevName, BASE_DEVICE_NAME);
        RtlInitUnicodeString(&ustrLinkName, BASE_DEVICE_LINK_NAME);
        NTSTATUS status = IoDeleteSymbolicLink(&ustrLinkName);
        if (NT_SUCCESS(status))
        {
            printStatusFile(L"\\DosDevices\\C:\\base-device-deletion-success.txt", "base-device-deletion-success.");
            DbgPrint("base device 0 and it's link deleted!\r\n");
        }
        else
        {
            printStatusFile(L"\\DosDevices\\C:\\base-device-deletion-failure.txt", "base-device-deletion-failure.");
            DbgPrint("failure deleting base device 0 and it's link\r\n");
        }
    }

#pragma prefast( suppress: 28175, "allowed in unload" )
    next_device_object = DeviceObject->NextDevice;

    Log(device_extension->LogFileDevice, "Deleting this device object...");
    // finally delete the log file handle of the device
    freeLog(device_extension->LogFileDevice);

    IoDeleteDevice(DeviceObject);

    return next_device_object;
}

#pragma code_seg() // end "PAGE"

NTSTATUS
FileDiskCreateClose(
    IN PDEVICE_OBJECT   DeviceObject,
    IN PIRP             Irp
)
{
    UNREFERENCED_PARAMETER(DeviceObject);

    Irp->IoStatus.Status = STATUS_SUCCESS;
    Irp->IoStatus.Information = FILE_OPENED;

    IoCompleteRequest(Irp, IO_NO_INCREMENT);

    return STATUS_SUCCESS;
}

NTSTATUS
FileDiskReadWrite(
    IN PDEVICE_OBJECT   DeviceObject,
    IN PIRP             Irp
)
{
    PDEVICE_EXTENSION   device_extension;
    PIO_STACK_LOCATION  io_stack;

    device_extension = (PDEVICE_EXTENSION)DeviceObject->DeviceExtension;

    if (!device_extension->media_in_device)
    {
        Irp->IoStatus.Status = STATUS_NO_MEDIA_IN_DEVICE;
        Irp->IoStatus.Information = 0;

        IoCompleteRequest(Irp, IO_NO_INCREMENT);

        return STATUS_NO_MEDIA_IN_DEVICE;
    }

    io_stack = IoGetCurrentIrpStackLocation(Irp);

    if (io_stack->Parameters.Read.Length == 0)
    {
        Irp->IoStatus.Status = STATUS_SUCCESS;
        Irp->IoStatus.Information = 0;

        IoCompleteRequest(Irp, IO_NO_INCREMENT);

        return STATUS_SUCCESS;
    }

    IoMarkIrpPending(Irp);

    // todo tushar
    // set the cancel routine
    //
    //IoSetCancelRoutine(Irp, CancelIrpRoutine);

    ExInterlockedInsertTailList(
        &device_extension->list_head,
        &Irp->Tail.Overlay.ListEntry,
        &device_extension->list_lock
    );

    KeSetEvent(
        &device_extension->request_event,
        (KPRIORITY)0,
        FALSE
    );

    return STATUS_PENDING;
}

NTSTATUS
FileDiskDeviceControl(
    IN PDEVICE_OBJECT   DeviceObject,
    IN PIRP             Irp
)
{
    PDEVICE_EXTENSION   device_extension;
    PIO_STACK_LOCATION  io_stack;
    NTSTATUS            status;
    NTSTATUS            WaitStatus;

    device_extension = (PDEVICE_EXTENSION)DeviceObject->DeviceExtension;

    io_stack = IoGetCurrentIrpStackLocation(Irp);
    
    
    if (!device_extension->media_in_device &&
        io_stack->Parameters.DeviceIoControl.IoControlCode !=
        IOCTL_REGISTER_FILE &&
        io_stack->Parameters.DeviceIoControl.IoControlCode !=
        IOCTL_FINDAVAILABLEDEVICE &&
        io_stack->Parameters.DeviceIoControl.IoControlCode !=
        IOCTL_DEREGISTER_FILE_FROM_DEVICE)
    {
        Irp->IoStatus.Status = STATUS_NO_MEDIA_IN_DEVICE;
        Irp->IoStatus.Information = 0;

        IoCompleteRequest(Irp, IO_NO_INCREMENT);

        return STATUS_NO_MEDIA_IN_DEVICE;
    }
    

    switch (io_stack->Parameters.DeviceIoControl.IoControlCode)
    {
    case IOCTL_FINDAVAILABLEDEVICE:
    {
        PBASE_DEVICE_QUERY query = (PBASE_DEVICE_QUERY)Irp->AssociatedIrp.SystemBuffer;
        int number = FileDiskFindFirstAvailableDevice(DeviceObject->DriverObject);
        query->DeviceNumber = number;
        Log(device_extension->LogFileDevice, "FileDiskDeviceControl::IOCTL_FINDAVAILABLEDEVICE request completed.");
        status = STATUS_SUCCESS;
        Irp->IoStatus.Information = sizeof(BASE_DEVICE_QUERY);
        break;
    }
    case IOCTL_DEREGISTER_FILE_FROM_DEVICE:
    {
        PBASE_DEVICE_QUERY query = (PBASE_DEVICE_QUERY)Irp->AssociatedIrp.SystemBuffer;
        FileDiskDeregisterFileFromDevice(DeviceObject->DriverObject, query->DeviceNumber);
        status = STATUS_SUCCESS;
        Irp->IoStatus.Information = sizeof(BASE_DEVICE_QUERY);
        break;
    }
    case IOCTL_REGISTER_FILE:
    {
        // TODO
        DbgPrint("FileDiskDeviceControl->IOCTL_REGISTER_FILE init.\r\n");

        SECURITY_QUALITY_OF_SERVICE security_quality_of_service;

        if (device_extension->media_in_device)
        {
            KdPrint(("FileDisk: IOCTL_REGISTER_FILE: Media already opened.\n"));

            status = STATUS_INVALID_DEVICE_REQUEST;
            Irp->IoStatus.Information = 0;
            break;
        }

        if (device_extension->security_client_context != NULL)
        {
            SeDeleteClientSecurity(device_extension->security_client_context);
        }
        else
        {
            device_extension->security_client_context =
                ExAllocatePoolWithTag(NonPagedPool, sizeof(SECURITY_CLIENT_CONTEXT), FILE_DISK_POOL_TAG);
        }

        RtlZeroMemory(&security_quality_of_service, sizeof(SECURITY_QUALITY_OF_SERVICE));

        security_quality_of_service.Length = sizeof(SECURITY_QUALITY_OF_SERVICE);
        security_quality_of_service.ImpersonationLevel = SecurityImpersonation;
        security_quality_of_service.ContextTrackingMode = SECURITY_STATIC_TRACKING;
        security_quality_of_service.EffectiveOnly = FALSE;

        SeCreateClientSecurity(
            PsGetCurrentThread(),
            &security_quality_of_service,
            FALSE,
            device_extension->security_client_context
        );

        // new code
        DbgPrint("FileDiskDeviceControl->IOCTL_REGISTER_FILE running.\r\n");
        SeImpersonateClient(device_extension->security_client_context, NULL);
        status = FileDiskOpenFile(DeviceObject, Irp);
        PsRevertToSelf();
        break;
    }

    case IOCTL_DEREGISTER_FILE:
    {
        //new code
        DbgPrint("FileDiskDeviceControl->IOCTL_DEREGISTER_FILE running.\r\n");
        status = FileDiskCloseFile(DeviceObject, Irp);
        break;
    }

    case IOCTL_FILE_DISK_QUERY_FILE:
    {
        POPEN_FILE_INFORMATION open_file_information;

        if (io_stack->Parameters.DeviceIoControl.OutputBufferLength <
            sizeof(OPEN_FILE_INFORMATION) + device_extension->file_name.Length - sizeof(UCHAR))
        {
            status = STATUS_BUFFER_TOO_SMALL;
            Irp->IoStatus.Information = 0;
            break;
        }

        open_file_information = (POPEN_FILE_INFORMATION)Irp->AssociatedIrp.SystemBuffer;

        open_file_information->FileSize.QuadPart = device_extension->file_size.QuadPart;
        open_file_information->ReadOnly = device_extension->read_only;
        open_file_information->FileNameLength = device_extension->file_name.Length;

        RtlCopyMemory(
            open_file_information->FileName,
            device_extension->file_name.Buffer,
            device_extension->file_name.Length
        );

        status = STATUS_SUCCESS;
        Irp->IoStatus.Information = sizeof(OPEN_FILE_INFORMATION) +
            open_file_information->FileNameLength - sizeof(UCHAR);

        break;
    }

    case IOCTL_DISK_CHECK_VERIFY:
    case IOCTL_CDROM_CHECK_VERIFY:
    case IOCTL_STORAGE_CHECK_VERIFY:
    case IOCTL_STORAGE_CHECK_VERIFY2:
    {
        status = STATUS_SUCCESS;
        Irp->IoStatus.Information = 0;
        break;
    }

    case IOCTL_DISK_GET_DRIVE_GEOMETRY:
    case IOCTL_CDROM_GET_DRIVE_GEOMETRY:
    {
        PDISK_GEOMETRY  disk_geometry;
        ULONGLONG       length;
        ULONG           sector_size;

        if (io_stack->Parameters.DeviceIoControl.OutputBufferLength <
            sizeof(DISK_GEOMETRY))
        {
            status = STATUS_BUFFER_TOO_SMALL;
            Irp->IoStatus.Information = 0;
            break;
        }

        disk_geometry = (PDISK_GEOMETRY)Irp->AssociatedIrp.SystemBuffer;

        length = device_extension->file_size.QuadPart;

        if (device_extension->device_type != FILE_DEVICE_CD_ROM)
        {
            sector_size = 512;
        }
        else
        {
            sector_size = 2048;
        }

        disk_geometry->Cylinders.QuadPart = length / sector_size / 32 / 2;
        disk_geometry->MediaType = RemovableMedia; // tushar original: FixedMedia 
        disk_geometry->TracksPerCylinder = 2;
        disk_geometry->SectorsPerTrack = 32;
        disk_geometry->BytesPerSector = sector_size;

        status = STATUS_SUCCESS;
        Irp->IoStatus.Information = sizeof(DISK_GEOMETRY);

        break;
    }

    case IOCTL_DISK_GET_LENGTH_INFO:
    {
        PGET_LENGTH_INFORMATION get_length_information;

        if (io_stack->Parameters.DeviceIoControl.OutputBufferLength <
            sizeof(GET_LENGTH_INFORMATION))
        {
            status = STATUS_BUFFER_TOO_SMALL;
            Irp->IoStatus.Information = 0;
            break;
        }

        get_length_information = (PGET_LENGTH_INFORMATION)Irp->AssociatedIrp.SystemBuffer;

        get_length_information->Length.QuadPart = device_extension->file_size.QuadPart;

        status = STATUS_SUCCESS;
        Irp->IoStatus.Information = sizeof(GET_LENGTH_INFORMATION);

        break;
    }

    case IOCTL_DISK_GET_PARTITION_INFO:
    {
        PPARTITION_INFORMATION  partition_information;
        ULONGLONG               length;

        if (io_stack->Parameters.DeviceIoControl.OutputBufferLength <
            sizeof(PARTITION_INFORMATION))
        {
            status = STATUS_BUFFER_TOO_SMALL;
            Irp->IoStatus.Information = 0;
            break;
        }

        partition_information = (PPARTITION_INFORMATION)Irp->AssociatedIrp.SystemBuffer;

        length = device_extension->file_size.QuadPart;

        partition_information->StartingOffset.QuadPart = 0;
        partition_information->PartitionLength.QuadPart = length;
        partition_information->HiddenSectors = 1;
        partition_information->PartitionNumber = 0;
        partition_information->PartitionType = 0;
        partition_information->BootIndicator = FALSE;
        partition_information->RecognizedPartition = FALSE;
        partition_information->RewritePartition = FALSE;

        status = STATUS_SUCCESS;
        Irp->IoStatus.Information = sizeof(PARTITION_INFORMATION);

        break;
    }

    case IOCTL_DISK_GET_PARTITION_INFO_EX:
    {
        PPARTITION_INFORMATION_EX   partition_information_ex;
        ULONGLONG                   length;

        if (io_stack->Parameters.DeviceIoControl.OutputBufferLength <
            sizeof(PARTITION_INFORMATION_EX))
        {
            status = STATUS_BUFFER_TOO_SMALL;
            Irp->IoStatus.Information = 0;
            break;
        }

        partition_information_ex = (PPARTITION_INFORMATION_EX)Irp->AssociatedIrp.SystemBuffer;

        length = device_extension->file_size.QuadPart;

        partition_information_ex->PartitionStyle = PARTITION_STYLE_MBR;
        partition_information_ex->StartingOffset.QuadPart = 0;
        partition_information_ex->PartitionLength.QuadPart = length;
        partition_information_ex->PartitionNumber = 0;
        partition_information_ex->RewritePartition = FALSE;
        partition_information_ex->Mbr.PartitionType = 0;
        partition_information_ex->Mbr.BootIndicator = FALSE;
        partition_information_ex->Mbr.RecognizedPartition = FALSE;
        partition_information_ex->Mbr.HiddenSectors = 1;

        status = STATUS_SUCCESS;
        Irp->IoStatus.Information = sizeof(PARTITION_INFORMATION_EX);

        break;
    }

    case IOCTL_DISK_IS_WRITABLE:
    {
        if (!device_extension->read_only)
        {
            status = STATUS_SUCCESS;
        }
        else
        {
            status = STATUS_MEDIA_WRITE_PROTECTED;
        }
        Irp->IoStatus.Information = 0;
        break;
    }

    case IOCTL_DISK_MEDIA_REMOVAL:
    case IOCTL_STORAGE_MEDIA_REMOVAL:
    {
        status = STATUS_SUCCESS;
        Irp->IoStatus.Information = 0;
        break;
    }

    case IOCTL_CDROM_READ_TOC:
    {
        PCDROM_TOC cdrom_toc;

        if (io_stack->Parameters.DeviceIoControl.OutputBufferLength <
            sizeof(CDROM_TOC))
        {
            status = STATUS_BUFFER_TOO_SMALL;
            Irp->IoStatus.Information = 0;
            break;
        }

        cdrom_toc = (PCDROM_TOC)Irp->AssociatedIrp.SystemBuffer;

        RtlZeroMemory(cdrom_toc, sizeof(CDROM_TOC));

        cdrom_toc->FirstTrack = 1;
        cdrom_toc->LastTrack = 1;
        cdrom_toc->TrackData[0].Control = TOC_DATA_TRACK;

        status = STATUS_SUCCESS;
        Irp->IoStatus.Information = sizeof(CDROM_TOC);

        break;
    }

    case IOCTL_CDROM_GET_LAST_SESSION:
    {
        PCDROM_TOC_SESSION_DATA cdrom_toc_s_d;

        if (io_stack->Parameters.DeviceIoControl.OutputBufferLength <
            sizeof(CDROM_TOC_SESSION_DATA))
        {
            status = STATUS_BUFFER_TOO_SMALL;
            Irp->IoStatus.Information = 0;
            break;
        }

        cdrom_toc_s_d = (PCDROM_TOC_SESSION_DATA)Irp->AssociatedIrp.SystemBuffer;

        RtlZeroMemory(cdrom_toc_s_d, sizeof(CDROM_TOC_SESSION_DATA));

        cdrom_toc_s_d->FirstCompleteSession = 1;
        cdrom_toc_s_d->LastCompleteSession = 1;
        cdrom_toc_s_d->TrackData[0].Control = TOC_DATA_TRACK;

        status = STATUS_SUCCESS;
        Irp->IoStatus.Information = sizeof(CDROM_TOC_SESSION_DATA);

        break;
    }

    case IOCTL_DISK_SET_PARTITION_INFO:
    {
        if (device_extension->read_only)
        {
            status = STATUS_MEDIA_WRITE_PROTECTED;
            Irp->IoStatus.Information = 0;
            break;
        }

        if (io_stack->Parameters.DeviceIoControl.InputBufferLength <
            sizeof(SET_PARTITION_INFORMATION))
        {
            status = STATUS_INVALID_PARAMETER;
            Irp->IoStatus.Information = 0;
            break;
        }

        status = STATUS_SUCCESS;
        Irp->IoStatus.Information = 0;

        break;
    }

    case IOCTL_DISK_VERIFY:
    {
        PVERIFY_INFORMATION verify_information;

        if (io_stack->Parameters.DeviceIoControl.InputBufferLength <
            sizeof(VERIFY_INFORMATION))
        {
            status = STATUS_INVALID_PARAMETER;
            Irp->IoStatus.Information = 0;
            break;
        }

        verify_information = (PVERIFY_INFORMATION)Irp->AssociatedIrp.SystemBuffer;

        status = STATUS_SUCCESS;
        Irp->IoStatus.Information = verify_information->Length;

        break;
    }

    case IOCTL_STORAGE_GET_DEVICE_NUMBER:
    {
        PSTORAGE_DEVICE_NUMBER number;

        if (io_stack->Parameters.DeviceIoControl.OutputBufferLength <
            sizeof(STORAGE_DEVICE_NUMBER))
        {
            status = STATUS_BUFFER_TOO_SMALL;
            Irp->IoStatus.Information = 0;
            break;
        }

        number = (PSTORAGE_DEVICE_NUMBER)Irp->AssociatedIrp.SystemBuffer;

        number->DeviceType = device_extension->device_type;
        number->DeviceNumber = device_extension->device_number;
        number->PartitionNumber = (ULONG)-1;

        status = STATUS_SUCCESS;
        Irp->IoStatus.Information = sizeof(STORAGE_DEVICE_NUMBER);

        break;
    }

    case IOCTL_STORAGE_GET_HOTPLUG_INFO:
    {
        PSTORAGE_HOTPLUG_INFO info;

        if (io_stack->Parameters.DeviceIoControl.OutputBufferLength <
            sizeof(STORAGE_HOTPLUG_INFO))
        {
            status = STATUS_BUFFER_TOO_SMALL;
            Irp->IoStatus.Information = 0;
            break;
        }

        info = (PSTORAGE_HOTPLUG_INFO)Irp->AssociatedIrp.SystemBuffer;

        info->Size = sizeof(STORAGE_HOTPLUG_INFO);
        info->MediaRemovable = 0;
        info->MediaHotplug = 0;
        info->DeviceHotplug = 0;
        info->WriteCacheEnableOverride = 0;

        status = STATUS_SUCCESS;
        Irp->IoStatus.Information = sizeof(STORAGE_HOTPLUG_INFO);

        break;
    }

    case IOCTL_VOLUME_GET_GPT_ATTRIBUTES:
    {
        PVOLUME_GET_GPT_ATTRIBUTES_INFORMATION attr;

        if (io_stack->Parameters.DeviceIoControl.OutputBufferLength <
            sizeof(VOLUME_GET_GPT_ATTRIBUTES_INFORMATION))
        {
            status = STATUS_BUFFER_TOO_SMALL;
            Irp->IoStatus.Information = 0;
            break;
        }

        attr = (PVOLUME_GET_GPT_ATTRIBUTES_INFORMATION)Irp->AssociatedIrp.SystemBuffer;

        attr->GptAttributes = 0;

        status = STATUS_SUCCESS;
        Irp->IoStatus.Information = sizeof(VOLUME_GET_GPT_ATTRIBUTES_INFORMATION);

        break;
    }

    case IOCTL_VOLUME_GET_VOLUME_DISK_EXTENTS:
    {
        PVOLUME_DISK_EXTENTS ext;

        if (io_stack->Parameters.DeviceIoControl.OutputBufferLength <
            sizeof(VOLUME_DISK_EXTENTS))
        {
            status = STATUS_INVALID_PARAMETER;
            Irp->IoStatus.Information = 0;
            break;
        }
        /*
                    // not needed since there is only one disk extent to return
                    if (io_stack->Parameters.DeviceIoControl.OutputBufferLength <
                        sizeof(VOLUME_DISK_EXTENTS) + ((NumberOfDiskExtents - 1) * sizeof(DISK_EXTENT)))
                    {
                        status = STATUS_BUFFER_OVERFLOW;
                        Irp->IoStatus.Information = 0;
                        break;
                    }
        */
        ext = (PVOLUME_DISK_EXTENTS)Irp->AssociatedIrp.SystemBuffer;

        ext->NumberOfDiskExtents = 1;
        ext->Extents[0].DiskNumber = device_extension->device_number;
        ext->Extents[0].StartingOffset.QuadPart = 0;
        ext->Extents[0].ExtentLength.QuadPart = device_extension->file_size.QuadPart;

        status = STATUS_SUCCESS;
        Irp->IoStatus.Information = sizeof(VOLUME_DISK_EXTENTS) /*+ ((NumberOfDiskExtents - 1) * sizeof(DISK_EXTENT))*/;

        break;
    }

#if (NTDDI_VERSION < NTDDI_VISTA)
#define IOCTL_DISK_IS_CLUSTERED CTL_CODE(IOCTL_DISK_BASE, 0x003e, METHOD_BUFFERED, FILE_ANY_ACCESS)
#endif  // NTDDI_VERSION < NTDDI_VISTA

    case IOCTL_DISK_IS_CLUSTERED:
    {
        PBOOLEAN clus;

        if (io_stack->Parameters.DeviceIoControl.OutputBufferLength <
            sizeof(BOOLEAN))
        {
            status = STATUS_BUFFER_TOO_SMALL;
            Irp->IoStatus.Information = 0;
            break;
        }

        clus = (PBOOLEAN)Irp->AssociatedIrp.SystemBuffer;

        *clus = FALSE;

        status = STATUS_SUCCESS;
        Irp->IoStatus.Information = sizeof(BOOLEAN);

        break;
    }

    case IOCTL_MOUNTDEV_QUERY_DEVICE_NAME:
    {
        PMOUNTDEV_NAME name;

        if (io_stack->Parameters.DeviceIoControl.OutputBufferLength <
            sizeof(MOUNTDEV_NAME))
        {
            status = STATUS_INVALID_PARAMETER;
            Irp->IoStatus.Information = 0;
            break;
        }

        name = (PMOUNTDEV_NAME)Irp->AssociatedIrp.SystemBuffer;
        name->NameLength = device_extension->device_name.Length * sizeof(WCHAR);

        if (io_stack->Parameters.DeviceIoControl.OutputBufferLength <
            name->NameLength + sizeof(USHORT))
        {
            status = STATUS_BUFFER_OVERFLOW;
            Irp->IoStatus.Information = sizeof(MOUNTDEV_NAME);
            break;
        }

        RtlCopyMemory(name->Name, device_extension->device_name.Buffer, name->NameLength);

        status = STATUS_SUCCESS;
        Irp->IoStatus.Information = name->NameLength + sizeof(USHORT);

        break;
    }

    case IOCTL_CDROM_READ_TOC_EX:
    {
        KdPrint(("FileDisk: Unhandled ioctl IOCTL_CDROM_READ_TOC_EX.\n"));
        status = STATUS_INVALID_DEVICE_REQUEST;
        Irp->IoStatus.Information = 0;
        break;
    }
    case IOCTL_DISK_GET_MEDIA_TYPES:
    {
        KdPrint(("FileDisk: Unhandled ioctl IOCTL_DISK_GET_MEDIA_TYPES.\n"));
        status = STATUS_INVALID_DEVICE_REQUEST;
        Irp->IoStatus.Information = 0;
        break;
    }
    case 0x66001b:
    {
        KdPrint(("FileDisk: Unhandled ioctl FT_BALANCED_READ_MODE.\n"));
        status = STATUS_INVALID_DEVICE_REQUEST;
        Irp->IoStatus.Information = 0;
        break;
    }
    case IOCTL_SCSI_GET_CAPABILITIES:
    {
        KdPrint(("FileDisk: Unhandled ioctl IOCTL_SCSI_GET_CAPABILITIES.\n"));
        status = STATUS_INVALID_DEVICE_REQUEST;
        Irp->IoStatus.Information = 0;
        break;
    }
    case IOCTL_SCSI_PASS_THROUGH:
    {
        KdPrint(("FileDisk: Unhandled ioctl IOCTL_SCSI_PASS_THROUGH.\n"));
        status = STATUS_INVALID_DEVICE_REQUEST;
        Irp->IoStatus.Information = 0;
        break;
    }
    case IOCTL_STORAGE_MANAGE_DATA_SET_ATTRIBUTES:
    {
        KdPrint(("FileDisk: Unhandled ioctl IOCTL_STORAGE_MANAGE_DATA_SET_ATTRIBUTES.\n"));
        status = STATUS_INVALID_DEVICE_REQUEST;
        Irp->IoStatus.Information = 0;
        break;
    }
    case IOCTL_STORAGE_QUERY_PROPERTY:
    {
        KdPrint(("FileDisk: Unhandled ioctl IOCTL_STORAGE_QUERY_PROPERTY.\n"));
        status = STATUS_INVALID_DEVICE_REQUEST;
        Irp->IoStatus.Information = 0;
        break;
    }

#if (NTDDI_VERSION < NTDDI_VISTA)
#define IOCTL_VOLUME_QUERY_ALLOCATION_HINT CTL_CODE(IOCTL_VOLUME_BASE, 20, METHOD_OUT_DIRECT, FILE_READ_ACCESS)
#endif  // NTDDI_VERSION < NTDDI_VISTA

    case IOCTL_VOLUME_QUERY_ALLOCATION_HINT:
    {
        KdPrint(("FileDisk: Unhandled ioctl IOCTL_VOLUME_QUERY_ALLOCATION_HINT.\n"));
        status = STATUS_INVALID_DEVICE_REQUEST;
        Irp->IoStatus.Information = 0;
        break;
    }
    default:
    {
        KdPrint((
            "FileDisk: Unknown IoControlCode %#x\n",
            io_stack->Parameters.DeviceIoControl.IoControlCode
            ));

        status = STATUS_INVALID_DEVICE_REQUEST;
        Irp->IoStatus.Information = 0;
    }
    }

    if (status != STATUS_PENDING)
    {
        Irp->IoStatus.Status = status;

        IoCompleteRequest(Irp, IO_NO_INCREMENT);
    }

    return status;
}

#pragma code_seg("PAGE")


VOID FileDiskClearQueue(IN PVOID Context)
{
    PDEVICE_OBJECT      device_object;
    PDEVICE_EXTENSION   device_extension;
    PLIST_ENTRY         request;
    PIRP                irp;
    PIO_STACK_LOCATION  io_stack;

    device_object = (PDEVICE_OBJECT)Context;
    device_extension = (PDEVICE_EXTENSION)device_object->DeviceExtension;

    // user requested to close the file, so cancel all requests and remove them from list and reset
    DbgPrint("FileDiskClearQueue method init\r\n");
    
    while ((request = ExInterlockedRemoveHeadList(
        &device_extension->list_head,
        &device_extension->list_lock
    )) != NULL)
    {
        irp = CONTAINING_RECORD(request, IRP, Tail.Overlay.ListEntry);
        io_stack = IoGetCurrentIrpStackLocation(irp);

        // cancel every queued irp-request if user set the termination flag
        irp->IoStatus.Status = STATUS_CANCELLED;// STATUS_SUCCESS;
        irp->IoStatus.Information = 0;
        IoCompleteRequest(
            irp,
            (CCHAR)(NT_SUCCESS(irp->IoStatus.Status) ?
                IO_DISK_INCREMENT : IO_NO_INCREMENT)
        );
        DbgPrint("FileDiskClearQueue method: queued irp cancelled.\r\n");
    }
    
    DbgPrint("FileDiskClearQueue method completed\r\n");

}


BOOL connectToSocket(IN PVOID Context)
{

    PDEVICE_OBJECT      device_object;
    PDEVICE_EXTENSION   device_extension;
    device_object = (PDEVICE_OBJECT)Context;
    device_extension = (PDEVICE_EXTENSION)device_object->DeviceExtension;

    int result;
    LARGE_INTEGER timeout = { 100 };
    LARGE_INTEGER timeout2 = { 100 };// 00 };
    struct addrinfo hints = { 0 };
    RtlZeroMemory(&hints, sizeof(struct addrinfo));
    hints.ai_family = AF_INET;// AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    struct addrinfo* res, * ptr = NULL;

    // port 1 - request

    // Resolve the server address and port
    //char port[16];
//    sprintf_s(&port, sizeof(port), "%d", device_extension->lport);
    //Log(device_extension->LogFileDevice, &port);

    PBYTE port = ExAllocatePoolWithTag(PagedPool, 16, FILE_DISK_POOL_TAG);
    sprintf_s(port, sizeof(port), "%d", device_extension->lport);

    //// Connect to server.
    while (TRUE)
    {
        result = getaddrinfo(NULL, port, &hints, &res);
        if (result != 0) {
            //DbgPrint("getaddrinfo failed with error: %d\n", result);
            KeDelayExecutionThread(KernelMode, FALSE, &timeout2);
            continue;
            //return FALSE;
        }

        ptr = res;

        // Create a SOCKET for connecting to server
        device_extension->sock = socket_connection(AF_INET, SOCK_STREAM, IPPROTO_TCP);

        result = connect(device_extension->sock, ptr->ai_addr, (int)ptr->ai_addrlen);
        if (result != 0)
        {
            freeaddrinfo(res);
            closesocket(device_extension->sock);
            KeDelayExecutionThread(KernelMode, FALSE, &timeout2);
            continue;
        }
        else
        {
            freeaddrinfo(res);
            break;
        }
    }
    ExFreePoolWithTag(port, FILE_DISK_POOL_TAG);

//    DbgPrint("connected...\r\n");
    return TRUE;
}

void ReadSocketComplete(int sock, void* outputBuffer, size_t length)
{
    UINT64 totalVectors = 1;
    size_t stride = totalVectors * SOCKET_BUFFER_SIZE;
    PBYTE dstptr = outputBuffer;

    while (length)
    {
        int sockResult = recv(sock, dstptr, stride, 0);
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

int Socket2Steps(IN PVOID Context, PIRP irp)
{
    PDEVICE_OBJECT      device_object;
    PDEVICE_EXTENSION   device_extension;
    PIO_STACK_LOCATION  io_stack;
    NTSTATUS status;
    device_object = (PDEVICE_OBJECT)Context;
    NTSTATUS WaitStatus;
    PVOID              system_buffer;

    device_extension = (PDEVICE_EXTENSION)device_object->DeviceExtension;

    io_stack = IoGetCurrentIrpStackLocation(irp);

//    Log(device_extension->LogFileDevice, "Socket2Steps started...");

    // phase 1: prepare configuration
    RtlZeroMemory(device_extension->request, REQUEST_BUFFER_SIZE);

    // prepare and set the request in shared memory
    if (io_stack->MajorFunction == IRP_MJ_READ)
    {
        system_buffer = MmGetSystemAddressForMdlSafe(irp->MdlAddress, HighPagePriority);// HighPagePriority);
        device_extension->request->MajorFunction = IRP_MJ_READ;
        device_extension->request->ByteOffset = io_stack->Parameters.Read.ByteOffset.QuadPart;
        device_extension->request->Length = io_stack->Parameters.Read.Length;
    }
    else
    {
        system_buffer = (PUCHAR)MmGetSystemAddressForMdlSafe(irp->MdlAddress, HighPagePriority | MdlMappingNoWrite);// NormalPagePriority);
        device_extension->request->MajorFunction = IRP_MJ_WRITE;
        device_extension->request->ByteOffset = io_stack->Parameters.Write.ByteOffset.QuadPart;
        device_extension->request->Length = io_stack->Parameters.Write.Length;
    }

    // set signature
    device_extension->request->signature = DRIVER_SIGNATURE;

    LARGE_INTEGER timeout = { 0,0 };
    timeout.QuadPart = 100;
    LARGE_INTEGER timeout2 = { 100 };// 0000 };


    // phase 1: open pipe in spin lock loop or directly if no problem

    //Log(device_extension->LogFileDevice, "Socket2Steps->KeSetEvent::KeDriverRequestDataSetObj starting...");

    // set the data event for the proxy app server to unblock and process the request
    KeSetEvent(device_extension->KeDriverRequestDataSetObj, 1, TRUE);
    KeDelayExecutionThread(KernelMode, FALSE, &timeout2);

    // connect
    //Log(device_extension->LogFileDevice, "Socket2Steps->connectToSocket starting.\r\n");
    connectToSocket(Context);
    //Log(device_extension->LogFileDevice, "Socket2Steps->connectToSocket completed.\r\n");

    // connected

    // phase 2: directly write request buffer to socket
    int result = send(device_extension->sock, device_extension->request, REQUEST_BUFFER_SIZE, 0);
    if (result == -1)
    {
//        Log(device_extension->LogFileDevice, "Socket2Steps: send request failure.\r\n");
  //      DbgPrint("send request failure.\r\n");
    }
//    Log(device_extension->LogFileDevice, "Socket2Steps->send completed. request sent.\r\n");

    // proper synchronisation between all processing in both client and server for each and every procedure.
    WaitStatus = KeWaitForSingleObject(device_extension->KeProxyIdleObj, UserRequest, UserMode, FALSE, NULL);
    KeResetEvent(device_extension->KeProxyIdleObj);
    KeSetEvent(device_extension->KeRequestCompleteObj, 1, TRUE);
    KeDelayExecutionThread(KernelMode, FALSE, &timeout2);

    // phase 3: direct i/o
    int sockResult;

    
    PBYTE buffer = ExAllocatePoolWithTag(PagedPool, MAJOR_BUFFER_SIZE, FILE_DISK_POOL_TAG);
    if (io_stack->MajorFunction == IRP_MJ_READ)
    {
        // use spin lock loop to successfully recieve data. it might be that the proxy app server is busy, so it takes time for the data to arrive.
        ReadSocketComplete(device_extension->sock, buffer, io_stack->Parameters.Read.Length);
        RtlCopyMemory(system_buffer, buffer, io_stack->Parameters.Read.Length);
//        Log(device_extension->LogFileDevice, "Socket2Steps->recv io completed. data received.\r\n");
    }
    else
    {
        // use spin lock loop to successfully send data. it might be that the proxy app server is busy, so it takes time for the data to be received by the proxy app server.
        RtlCopyMemory(buffer, system_buffer, io_stack->Parameters.Write.Length);
        sockResult = send(device_extension->sock, buffer, io_stack->Parameters.Write.Length, 0);
//        Log(device_extension->LogFileDevice, "Socket2Steps->send io completed. data sent.\r\n");
    }
    ExFreePoolWithTag(buffer, FILE_DISK_POOL_TAG);
    

    // proper synchronisation between all processing in both client and server for each and every procedure.
    WaitStatus = KeWaitForSingleObject(device_extension->KeProxyIdleObj, UserRequest, UserMode, FALSE, NULL);
    KeResetEvent(device_extension->KeProxyIdleObj);
    KeSetEvent(device_extension->KeRequestCompleteObj, 1, TRUE);
    KeDelayExecutionThread(KernelMode, FALSE, &timeout2);

    /*
    PBYTE buffer = (PBYTE)system_buffer;
    if (io_stack->MajorFunction == IRP_MJ_READ)
    {
        //sockResult = recv(device_extension->sock, system_buffer, io_stack->Parameters.Read.Length, 0);
        ReadSocketComplete(device_extension->sock, buffer, io_stack->Parameters.Read.Length);
//        Log(device_extension->LogFileDevice, "Socket2Steps->recv io completed. data received.\r\n");
    }
    else
    {
        sockResult = send(device_extension->sock, buffer, io_stack->Parameters.Write.Length, 0);
  //      Log(device_extension->LogFileDevice, "Socket2Steps->send io completed. data sent.\r\n");
    }
    */


    // phase 4: wait for Proxy Application Server to process entire request and complete and then set the ProxyIdle Event.


    // we cannot use indefinite wait, so we use spin lock loop which is breakable by user's command
    while (TRUE)
    {
        // wait for proxy idle event to occur, meaning until the proxy app processes entire request and takes time and completes, then signals the event, we must wait.
        WaitStatus = KeWaitForSingleObject(device_extension->KeProxyIdleObj, UserRequest, UserMode, FALSE, &timeout);// &timeout);
        if (WaitStatus == STATUS_SUCCESS)
        {
            //Log(device_extension->LogFileDevice, "Socket2Steps->KeWaitForSingleObject completed. proxy app set the event.\r\n");

            // success, the proxy app set the event flag
            KeResetEvent(device_extension->KeProxyIdleObj);

            // complete the entire sessin context
            KeSetEvent(device_extension->KeRequestCompleteObj, 1, TRUE);

            // delay for proper synchronisation of shared memory buffer and everything else in user mode and kernel mode
            KeDelayExecutionThread(KernelMode, FALSE, &timeout2);
            break;
        }
        else if (WaitStatus == STATUS_TIMEOUT)
        {
            continue;
        }
        else
        {
            continue;
        }
    }
    

    // finally close the client socket
    closesocket(device_extension->sock);

    // success completed
    //Log(device_extension->LogFileDevice, "Socket2Steps completed.\r\n");

    return 1;
}

VOID FileDiskSocketReadWrite(IN PVOID Context, PIRP irp)
{
    PDEVICE_OBJECT      device_object;
    PDEVICE_EXTENSION   device_extension;
    PLIST_ENTRY         request;
    PIO_STACK_LOCATION  io_stack;
    PUCHAR              system_buffer;
    PUCHAR              buffer;

    device_object = (PDEVICE_OBJECT)Context;

    device_extension = (PDEVICE_EXTENSION)device_object->DeviceExtension;

    io_stack = IoGetCurrentIrpStackLocation(irp);

    // TODO read and write pipe

    int requestStatus;

    switch (io_stack->MajorFunction)
    {
    case IRP_MJ_READ:

        if ((io_stack->Parameters.Read.ByteOffset.QuadPart +
            io_stack->Parameters.Read.Length) >
            device_extension->file_size.QuadPart)
        {
            irp->IoStatus.Status = STATUS_INVALID_PARAMETER;
            irp->IoStatus.Information = 0;
            break;
        }

        //Log(device_extension->LogFileDevice, "FileDiskSocketReadWrite->Socket2Steps starting.\r\n");

        // request and pipe i/o
        requestStatus = Socket2Steps(Context, irp);
        if (requestStatus == -1)
            goto Completed;


        // finally update current device's current Irp status
        irp->IoStatus.Status = STATUS_SUCCESS;
        irp->IoStatus.Information = io_stack->Parameters.Read.Length;
        break;
    case IRP_MJ_WRITE:

        if ((io_stack->Parameters.Write.ByteOffset.QuadPart +
            io_stack->Parameters.Write.Length) >
            device_extension->file_size.QuadPart)
        {
            irp->IoStatus.Status = STATUS_INVALID_PARAMETER;
            irp->IoStatus.Information = 0;
            break;
        }

        //Log(device_extension->LogFileDevice, "FileDiskSocketReadWrite->Socket2Steps starting.\r\n");

        // request and pipe i/o
        requestStatus = Socket2Steps(Context, irp);
        if (requestStatus == -1)
            goto Completed;

        // finally update current device's current Irp status
        irp->IoStatus.Status = STATUS_SUCCESS;
        irp->IoStatus.Information = io_stack->Parameters.Write.Length;
        break;
    default:
        irp->IoStatus.Status = STATUS_DRIVER_INTERNAL_ERROR;
    }
Completed:
    IoCompleteRequest(
        irp,
        (CCHAR)(NT_SUCCESS(irp->IoStatus.Status) ?
            IO_DISK_INCREMENT : IO_NO_INCREMENT)
    );
}

int Pipe2Steps(IN PVOID Context, PIRP irp)
{
    PDEVICE_OBJECT      device_object;
    PDEVICE_EXTENSION   device_extension;
    PIO_STACK_LOCATION  io_stack;
    NTSTATUS status;
    device_object = (PDEVICE_OBJECT)Context;
    NTSTATUS WaitStatus;
    PUCHAR              system_buffer;

    device_extension = (PDEVICE_EXTENSION)device_object->DeviceExtension;

    io_stack = IoGetCurrentIrpStackLocation(irp);

    //Log(device_extension->LogFileDevice, "Pipe2Steps started...");

    // phase 1: prepare configuration
    RtlZeroMemory(device_extension->request, REQUEST_BUFFER_SIZE);

    // prepare and set the request in shared memory
    if (io_stack->MajorFunction == IRP_MJ_READ)
    {
        system_buffer = (PUCHAR)MmGetSystemAddressForMdlSafe(irp->MdlAddress, HighPagePriority);
        device_extension->request->MajorFunction = IRP_MJ_READ;
        device_extension->request->ByteOffset = io_stack->Parameters.Read.ByteOffset.QuadPart;
        device_extension->request->Length = io_stack->Parameters.Read.Length;
    }
    else
    {
        system_buffer = (PUCHAR)MmGetSystemAddressForMdlSafe(irp->MdlAddress, HighPagePriority | MdlMappingNoWrite);// NormalPagePriority);
        device_extension->request->MajorFunction = IRP_MJ_WRITE;
        device_extension->request->ByteOffset = io_stack->Parameters.Write.ByteOffset.QuadPart;
        device_extension->request->Length = io_stack->Parameters.Write.Length;
    }

    // set signature
    device_extension->request->signature = DRIVER_SIGNATURE;

    LARGE_INTEGER timeout = { 0,0 };
    timeout.QuadPart = 100;
    LARGE_INTEGER timeout2 = { 0,0 };
    timeout2.QuadPart = 100; // 1000000; // 1 millisecond

    // phase 1: open pipe in spin lock loop or directly if no problem

    // set the data event for the proxy app server to unblock and process the request
    KeSetEvent(device_extension->KeDriverRequestDataSetObj, 100, TRUE);
    KeDelayExecutionThread(KernelMode, FALSE, &timeout);

    while (TRUE)
    {
        status = openPipe(device_object);
        if (status != STATUS_SUCCESS) {
            //Log(device_extension->LogFileDevice, "openPipe requestPipe failure.");
            KeDelayExecutionThread(KernelMode, FALSE, &timeout);
            continue;
        }
        else
        {
            // success, pipe opened, so break from the loop
            break;
        }
    }
    //Log(device_extension->LogFileDevice, "openPipe success. pipe opened.");

    // phase 2: directly write request buffer to pipe

    // success
    IO_STATUS_BLOCK iostatus;
    status = ZwWriteFile(device_extension->pipe, NULL, NULL, NULL, &iostatus, (PVOID)device_extension->request, sizeof(CONTEXT_REQUEST), NULL, NULL); //original REQUEST_BUFFER_SIZE, NULL, NULL);
    ZwFlushBuffersFile(device_extension->pipe, &iostatus);
    if (status != STATUS_SUCCESS)
    {
        Log(device_extension->LogFileDevice, "ZwWriteFile:request method failure...");
    }

    //Log(device_extension->LogFileDevice, "request written to pipe. success");

    // phase 3: direct pipe i/o

    LARGE_INTEGER offsettmp = { 0,0 };
    offsettmp.QuadPart = 0;

    if (device_extension->request->MajorFunction == IRP_MJ_READ)
    {
        //fastPipeToBuffer(system_buffer, device_extension->pipe, 0, device_extension->request->Length);
        status = ZwReadFile(device_extension->pipe, NULL, NULL, NULL, &iostatus, system_buffer, device_extension->request->Length, NULL, NULL);
        if (status != STATUS_SUCCESS)
        {
            Log(device_extension->LogFileDevice, "ZwReadFile:io method failure...");
        }
        //      Log(device_extension->LogFileDevice, "fastPipeToBuffer: read from pipe to buffer success");
    }
    else
    {
        status = ZwWriteFile(device_extension->pipe, NULL, NULL, NULL, &iostatus, system_buffer, device_extension->request->Length, NULL, NULL);// &offsettmp, NULL);
        ZwFlushBuffersFile(device_extension->pipe, &iostatus);
        if (status != STATUS_SUCCESS)
        {
            Log(device_extension->LogFileDevice, "ZwWriteFile:io method failure...");
        }
        //fastBufferToPipe(system_buffer, device_extension->pipe, 0, device_extension->request->Length);
//        Log(device_extension->LogFileDevice, "fastBufferToPipe: write buffer to pipe success");
    }

    // close the pipe as the work has been completed
    closePipe(device_object);

    // phase 4: wait for Proxy Application Server to process entire request and complete and then set the ProxyIdle Event.

    // we cannot use indefinite wait, so we use spin lock loop which is breakable by user's command
    while (TRUE)
    {
        // wait for proxy idle event to occur, meaning until the proxy app processes entire request and takes time and completes, then signals the event, we must wait.
        WaitStatus = KeWaitForSingleObject(device_extension->KeProxyIdleObj, UserRequest, UserMode, FALSE, &timeout);// &timeout);
        if (WaitStatus == STATUS_SUCCESS)
        {
            // success, the proxy app set the event flag
            KeResetEvent(device_extension->KeProxyIdleObj);

            // delay for proper synchronisation of shared memory buffer and everything else in user mode and kernel mode
            //KeDelayExecutionThread(KernelMode, FALSE, &timeout2);
            break;
        }
        else if (WaitStatus == STATUS_TIMEOUT)
        {
            continue;
        }
        else
        {
            continue;
        }
    }

    // success completed
    //Log(device_extension->LogFileDevice, "Pipe2Steps completed successfully.");
    return 1;
}

VOID FileDiskPipeReadWrite(IN PVOID Context, PIRP irp)
{
    PDEVICE_OBJECT      device_object;
    PDEVICE_EXTENSION   device_extension;
    PLIST_ENTRY         request;
    PIO_STACK_LOCATION  io_stack;
    PUCHAR              system_buffer;
    PUCHAR              buffer;

    device_object = (PDEVICE_OBJECT)Context;

    device_extension = (PDEVICE_EXTENSION)device_object->DeviceExtension;

    io_stack = IoGetCurrentIrpStackLocation(irp);

    // TODO read and write pipe

    int requestStatus;

    switch (io_stack->MajorFunction)
    {
    case IRP_MJ_READ:

        if ((io_stack->Parameters.Read.ByteOffset.QuadPart +
            io_stack->Parameters.Read.Length) >
            device_extension->file_size.QuadPart)
        {
            irp->IoStatus.Status = STATUS_INVALID_PARAMETER;
            irp->IoStatus.Information = 0;
            break;
        }

        // request and pipe i/o
        requestStatus = Pipe2Steps(Context, irp);
        if (requestStatus == -1)
            goto Completed;


        // finally update current device's current Irp status
        irp->IoStatus.Status = STATUS_SUCCESS;
        irp->IoStatus.Information = io_stack->Parameters.Read.Length;
        break;
    case IRP_MJ_WRITE:

        if ((io_stack->Parameters.Write.ByteOffset.QuadPart +
            io_stack->Parameters.Write.Length) >
            device_extension->file_size.QuadPart)
        {
            irp->IoStatus.Status = STATUS_INVALID_PARAMETER;
            irp->IoStatus.Information = 0;
            break;
        }

        // request and pipe i/o
        requestStatus = Pipe2Steps(Context, irp);
        if (requestStatus == -1)
            goto Completed;

        // finally update current device's current Irp status
        irp->IoStatus.Status = STATUS_SUCCESS;
        irp->IoStatus.Information = io_stack->Parameters.Write.Length;
        break;
    default:
        irp->IoStatus.Status = STATUS_DRIVER_INTERNAL_ERROR;
    }
Completed:
    IoCompleteRequest(
        irp,
        (CCHAR)(NT_SUCCESS(irp->IoStatus.Status) ?
            IO_DISK_INCREMENT : IO_NO_INCREMENT)
    );
}

VOID FileDiskSHMReadWrite(IN PVOID Context, PIRP irp)
{
    PDEVICE_OBJECT      device_object;
    PDEVICE_EXTENSION   device_extension;
    PLIST_ENTRY         request;
    PIO_STACK_LOCATION  io_stack;
    PUCHAR              system_buffer;
    PUCHAR              buffer;

    device_object = (PDEVICE_OBJECT)Context;

    device_extension = (PDEVICE_EXTENSION)device_object->DeviceExtension;

    io_stack = IoGetCurrentIrpStackLocation(irp);

    // TODO read and write shm
    PUCHAR shmBuffer = (PUCHAR)device_extension->g_pSharedSection;
    PCONTEXT_REQUEST shmRequest = (PCONTEXT_REQUEST)device_extension->g_pSharedSection;
    LARGE_INTEGER timeout = { 0,0 };
    // Force 60 sec timeout
//            timeout.QuadPart = -(60ll * 10 * 1000 * 1000);
    timeout.QuadPart = 100; // 1000000000; // = 1 second delay // 10000000000; // = 10 seconds // 50 ms: 50000000;
    LARGE_INTEGER timeout2 = { 0,0 };
    timeout2.QuadPart = 1000000; // = 1 milisecond // 100000; // 100000000;// 50000000;
    NTSTATUS WaitStatus;
    switch (io_stack->MajorFunction)
    {
    case IRP_MJ_READ:

        /*
        if ((io_stack->Parameters.Read.ByteOffset.QuadPart +
            io_stack->Parameters.Read.Length) >
            device_extension->file_size.QuadPart)
        {
            irp->IoStatus.Status = STATUS_INVALID_PARAMETER;
            irp->IoStatus.Information = 0;
            break;
        }
        */

        // prepare and set the request in shared memory
        RtlZeroMemory(shmRequest, SHM_HEADER_SIZE);
        shmRequest->MajorFunction = IRP_MJ_READ;
        shmRequest->ByteOffset = io_stack->Parameters.Read.ByteOffset.QuadPart;
        shmRequest->Length = io_stack->Parameters.Read.Length;

        // delay for proper synchronisation of shared memory buffer and everything else in user mode and kernel mode
        KeDelayExecutionThread(KernelMode, FALSE, &timeout2);
        // set the data event for the proxy app server to unblock and process the request
        KeSetEvent(device_extension->KeDriverRequestDataSetObj, 100, TRUE);

        // we cannot use indefinite wait, so we use spin lock loop which is breakable by user's command
        while (TRUE)
        {
            // wait for proxy idle event to occur, meaning until the proxy app processes entire request and takes time and completes, then signals the event, we must wait.
            WaitStatus = KeWaitForSingleObject(device_extension->KeProxyIdleObj, UserRequest, UserMode, FALSE, &timeout);// &timeout);
            if (WaitStatus == STATUS_SUCCESS)
            {
                // success, the proxy app set the event flag
                KeResetEvent(device_extension->KeProxyIdleObj);
                break;
            }
            else if (WaitStatus == STATUS_TIMEOUT)
            {
                continue;
            }
            else
            {
                continue;
            }
        }

        // delay for proper synchronisation of shared memory buffer and everything else in user mode and kernel mode
        KeDelayExecutionThread(KernelMode, FALSE, &timeout2);

        // the data has been read from the virtual disk file by the proxy app into the shared memory buffer, so write it to the window's provided system buffer.
        // set the systembuffer and configuration
        system_buffer = (PUCHAR)MmGetSystemAddressForMdlSafe(irp->MdlAddress, HighPagePriority);// NormalPagePriority);
        RtlCopyBytes(system_buffer, shmBuffer + SHM_HEADER_SIZE, io_stack->Parameters.Read.Length);

        // finally update current device's current Irp status
        irp->IoStatus.Status = STATUS_SUCCESS;
        irp->IoStatus.Information = io_stack->Parameters.Read.Length;
        KeSetEvent(device_extension->KeRequestCompleteObj, 100, TRUE);
        break;
    case IRP_MJ_WRITE:

        /*
        if ((io_stack->Parameters.Write.ByteOffset.QuadPart +
            io_stack->Parameters.Write.Length) >
            device_extension->file_size.QuadPart)
        {
            irp->IoStatus.Status = STATUS_INVALID_PARAMETER;
            irp->IoStatus.Information = 0;
            break;
        }
        */

        // prepare and set the request in shared memory
        RtlZeroMemory(shmRequest, SHM_HEADER_SIZE);
        shmRequest->MajorFunction = IRP_MJ_WRITE;
        shmRequest->ByteOffset = io_stack->Parameters.Write.ByteOffset.QuadPart;
        shmRequest->Length = io_stack->Parameters.Write.Length;
        KeDelayExecutionThread(KernelMode, FALSE, &timeout2);

        //char txt[256];
        //sprintf_s(&txt, 256, "%s: %u", "shmRequest->ByteOffset", shmRequest->ByteOffset);
        //Log(device_extension->LogFileDevice, &txt);

        system_buffer = (PUCHAR)MmGetSystemAddressForMdlSafe(irp->MdlAddress, HighPagePriority | MdlMappingNoWrite);// NormalPagePriority);
        RtlCopyBytes(shmBuffer + SHM_HEADER_SIZE, system_buffer, io_stack->Parameters.Write.Length);

        // delay for proper synchronisation of shared memory buffer and everything else in user mode and kernel mode
        KeDelayExecutionThread(KernelMode, FALSE, &timeout2);
        // set the data event for the proxy app server to unblock and process the request
        KeSetEvent(device_extension->KeDriverRequestDataSetObj, 100, TRUE);

        // we cannot use indefinite wait, so we use spin lock loop which is breakable by user's command
        while (TRUE)
        {
            // wait for proxy idle event to occur, meaning until the proxy app processes entire request and takes time and completes, then signals the event, we must wait.
            WaitStatus = KeWaitForSingleObject(device_extension->KeProxyIdleObj, UserRequest, UserMode, FALSE, &timeout);// &timeout);
            if (WaitStatus == STATUS_SUCCESS)
            {
                // success, the proxy app set the event flag
                KeResetEvent(device_extension->KeProxyIdleObj);
                break;
            }
            else if (WaitStatus == STATUS_TIMEOUT)
            {
                continue;
            }
            else
            {
                continue;
            }
        }

        // delay for proper synchronisation of shared memory buffer and everything else in user mode and kernel mode
        KeDelayExecutionThread(KernelMode, FALSE, &timeout2);

        // the data has been written by the proxy app into the backend file, so configure the final configuration and complete the request.

        // finally update current device's current Irp status
        irp->IoStatus.Status = STATUS_SUCCESS;
        irp->IoStatus.Information = io_stack->Parameters.Write.Length;
        KeSetEvent(device_extension->KeRequestCompleteObj, 100, TRUE);
        break;
    default:
        irp->IoStatus.Status = STATUS_DRIVER_INTERNAL_ERROR;
    }
Completed:
    IoCompleteRequest(
        irp,
        (CCHAR)(NT_SUCCESS(irp->IoStatus.Status) ?
            IO_DISK_INCREMENT : IO_NO_INCREMENT)
    );

}


VOID
FileDiskThread(
    IN PVOID Context
)
{
    PDEVICE_OBJECT      device_object;
    PDEVICE_EXTENSION   device_extension;
    PLIST_ENTRY         request;
    PIRP                irp;
    PIO_STACK_LOCATION  io_stack;
    PUCHAR              system_buffer;
    PUCHAR              buffer;

    PAGED_CODE();

    ASSERT(Context != NULL);

    device_object = (PDEVICE_OBJECT)Context;

    device_extension = (PDEVICE_EXTENSION)device_object->DeviceExtension;

    KeSetPriorityThread(KeGetCurrentThread(), LOW_REALTIME_PRIORITY);

    FileDiskAdjustPrivilege(SE_IMPERSONATE_PRIVILEGE, TRUE);

    //DbgPrint("FileDiskThread %u running \r\n", device_extension->device_number);

    LARGE_INTEGER timeout = { 0,0 };
    timeout.QuadPart = 100;

    for (;;)
    {
        KeWaitForSingleObject(
            &device_extension->request_event,
            Executive,
            KernelMode,
            FALSE,
            NULL
        );

        if (device_extension->terminate_thread) 
        {
            FileDiskClearQueue(device_object);
            PsTerminateSystemThread(STATUS_SUCCESS);
        }

        
        if (!device_extension->media_in_device)
        {
            // todo tushar proper shutdown
            FileDiskClearQueue(device_object);
            KeSetEvent(device_extension->KeShutdownCompleteObj, 100, TRUE);
            KeDelayExecutionThread(KernelMode, FALSE, &timeout);
            continue;
        }
        
        if (isEventSignalled(device_extension->KeShutdownObj))
        {
            // device/file shutdown event set, so cancel
            FileDiskClearQueue(device_object);
            // set the final event for the proxy server app to know that the driver has closed the file
            KeSetEvent(device_extension->KeShutdownCompleteObj, 100, TRUE);
            KeDelayExecutionThread(KernelMode, FALSE, &timeout);
            continue;
        }
        

        while ((request = ExInterlockedRemoveHeadList(
            &device_extension->list_head,
            &device_extension->list_lock
        )) != NULL)
        {
            irp = CONTAINING_RECORD(request, IRP, Tail.Overlay.ListEntry);

            if (device_extension->terminate_thread)
            {
                FileDiskClearQueue(device_object);
                irp->IoStatus.Status = STATUS_CANCELLED;// STATUS_SUCCESS;
                irp->IoStatus.Information = 0;
                IoCompleteRequest(
                    irp,
                    (CCHAR)(NT_SUCCESS(irp->IoStatus.Status) ?
                        IO_DISK_INCREMENT : IO_NO_INCREMENT)
                );
                continue;
            }

            
            if (!device_extension->media_in_device)
            {
                // todo tushar proper shutdown
                FileDiskClearQueue(device_object);
                // set the final event for the proxy server app to know that the driver has closed the file
                KeSetEvent(device_extension->KeShutdownCompleteObj, 100, TRUE);
                KeDelayExecutionThread(KernelMode, FALSE, &timeout);
                irp->IoStatus.Status = STATUS_CANCELLED;// STATUS_SUCCESS;
                irp->IoStatus.Information = 0;
                IoCompleteRequest(
                    irp,
                    (CCHAR)(NT_SUCCESS(irp->IoStatus.Status) ?
                        IO_DISK_INCREMENT : IO_NO_INCREMENT)
                );

                continue;
            }

            if (isEventSignalled(device_extension->KeShutdownObj))
            {
                // device/file shutdown event set, so cancel all queue

                FileDiskClearQueue(device_object);
                // set the final event for the proxy server app to know that the driver has closed the file
                KeSetEvent(device_extension->KeShutdownCompleteObj, 100, TRUE);
                KeDelayExecutionThread(KernelMode, FALSE, &timeout);
                irp->IoStatus.Status = STATUS_CANCELLED;// STATUS_SUCCESS;
                irp->IoStatus.Information = 0;
                IoCompleteRequest(
                    irp,
                    (CCHAR)(NT_SUCCESS(irp->IoStatus.Status) ?
                        IO_DISK_INCREMENT : IO_NO_INCREMENT)
                );
                continue;
            }

            // no termination or abortion event/flag was signalled, so continue and process the request
            if (device_extension->usePipe)
            {
                FileDiskPipeReadWrite(Context, irp);
            }
            else if (device_extension->useShm)
            {
                FileDiskSHMReadWrite(Context, irp);
            }
            else if (device_extension->useSocket)
            {
                FileDiskSocketReadWrite(Context, irp);
            }

        }
    }
}

NTSTATUS
FileDiskOpenFile(
    IN PDEVICE_OBJECT   DeviceObject,
    IN PIRP             Irp
)
{
    PDEVICE_EXTENSION               device_extension;
    POPEN_FILE_INFORMATION          open_file_information;
    NTSTATUS                        status;

    PAGED_CODE();

    ASSERT(DeviceObject != NULL);
    ASSERT(Irp != NULL);

    DbgPrint("IOCTL_REGISTER_FILE->FileDiskOpenFile method running.\r\n");

    device_extension = (PDEVICE_EXTENSION)DeviceObject->DeviceExtension;

    open_file_information = (POPEN_FILE_INFORMATION)Irp->AssociatedIrp.SystemBuffer;

    //set file's attributes in the context
    device_extension->read_only = open_file_information->ReadOnly;
    device_extension->file_size.HighPart = open_file_information->FileSize.HighPart;
    device_extension->file_size.LowPart = open_file_information->FileSize.LowPart;
    device_extension->file_size.QuadPart = open_file_information->FileSize.QuadPart;
    device_extension->file_size.u.HighPart = open_file_information->FileSize.u.HighPart;
    device_extension->file_size.u.LowPart = open_file_information->FileSize.u.LowPart;
    device_extension->usePipe = open_file_information->usePipe;
    device_extension->useShm = open_file_information->useShm;
    device_extension->useSocket = open_file_information->useSocket;
    //RtlCopyMemory(device_extension->port, open_file_information->port, sizeof(open_file_information->port));
    device_extension->lport = open_file_information->lport;

    device_extension->file_name.Length = open_file_information->FileNameLength;
    device_extension->file_name.MaximumLength = open_file_information->FileNameLength;
    device_extension->file_name.Buffer = ExAllocatePoolWithTag(NonPagedPool, open_file_information->FileNameLength, FILE_DISK_POOL_TAG);

    if (device_extension->file_name.Buffer == NULL)
    {
        return STATUS_INSUFFICIENT_RESOURCES;
    }

    RtlCopyMemory(
        device_extension->file_name.Buffer,
        open_file_information->FileName,
        open_file_information->FileNameLength
    );

    open_file_information->DriverReply = TRUE;
    open_file_information->DeviceNumber = device_extension->device_number;

    // finally set status

    // todo tushar
    // reset all the events and objects for a new start with a new virtual disk file session context.
    KeResetEvent(device_extension->KeDriverRequestDataSetObj);
    KeResetEvent(device_extension->KeProxyIdleObj);
    KeResetEvent(device_extension->KeRequestCompleteObj);
    KeResetEvent(device_extension->KeShutdownObj);
    KeResetEvent(device_extension->KeShutdownCompleteObj);
    device_extension->media_in_device = TRUE;

    Irp->IoStatus.Status = STATUS_SUCCESS;
    Irp->IoStatus.Information = sizeof(OPEN_FILE_INFORMATION);

    DbgPrint("IOCTL_REGISTER_FILE->FileDiskOpenFile method completed.\r\n");

    return STATUS_SUCCESS;
}

NTSTATUS
FileDiskForceCloseFile(
    IN PDEVICE_OBJECT   DeviceObject
)
{
    PDEVICE_EXTENSION device_extension;
    LARGE_INTEGER timeout = { 0,0 };
    timeout.QuadPart = 100;

    DbgPrint("FileDiskForceCloseFile method running.\r\n");

    device_extension = (PDEVICE_EXTENSION)DeviceObject->DeviceExtension;

    ExFreePool(device_extension->file_name.Buffer);

    device_extension->media_in_device = FALSE;

    // todo tushar
    FileDiskClearQueue(DeviceObject);
    KeSetEvent(device_extension->KeShutdownCompleteObj, 100, TRUE);
    KeDelayExecutionThread(KernelMode, FALSE, &timeout);

    DbgPrint("FileDiskForceCloseFile method completed.\r\n");
    return STATUS_SUCCESS;
}


NTSTATUS
FileDiskCloseFile(
    IN PDEVICE_OBJECT   DeviceObject,
    IN PIRP             Irp
)
{
    PDEVICE_EXTENSION device_extension;
    LARGE_INTEGER timeout = { 0,0 };
    timeout.QuadPart = 100;

    PAGED_CODE();

    ASSERT(DeviceObject != NULL);
    ASSERT(Irp != NULL);

    PLIST_ENTRY         request;
    PIRP                irp;
    PIO_STACK_LOCATION  io_stack;

    DbgPrint("IOCTL_DEREGISTER_FILE->FileDiskCloseFile method running.\r\n");

    device_extension = (PDEVICE_EXTENSION)DeviceObject->DeviceExtension;

    ExFreePool(device_extension->file_name.Buffer);

    device_extension->media_in_device = FALSE;

    // todo tushar
    FileDiskClearQueue(DeviceObject);
    KeSetEvent(device_extension->KeShutdownCompleteObj, 100, TRUE);
    KeDelayExecutionThread(KernelMode, FALSE, &timeout);

    Irp->IoStatus.Status = STATUS_SUCCESS;
    Irp->IoStatus.Information = 0;

    DbgPrint("IOCTL_DEREGISTER_FILE->FileDiskCloseFile method completed.\r\n");
    return STATUS_SUCCESS;
}

NTSTATUS
FileDiskAdjustPrivilege(
    IN ULONG    Privilege,
    IN BOOLEAN  Enable
)
{
    NTSTATUS            status;
    HANDLE              token_handle;
    TOKEN_PRIVILEGES    token_privileges;

    PAGED_CODE();

    status = ZwOpenProcessToken(
        NtCurrentProcess(),
        TOKEN_ALL_ACCESS,
        &token_handle
    );

    if (!NT_SUCCESS(status))
    {
        return status;
    }

    token_privileges.PrivilegeCount = 1;
    token_privileges.Privileges[0].Luid = RtlConvertUlongToLuid(Privilege);
    token_privileges.Privileges[0].Attributes = Enable ? SE_PRIVILEGE_ENABLED : 0;

    status = ZwAdjustPrivilegesToken(
        token_handle,
        FALSE,
        &token_privileges,
        sizeof(token_privileges),
        NULL,
        NULL
    );

    ZwClose(token_handle);

    return status;
}

NTSTATUS SetSecurityAllAccess(HANDLE h, PVOID obj)
{
    NTSTATUS status;

    status = ObReferenceObjectByHandle(h, STANDARD_RIGHTS_ALL | GENERIC_ALL, NULL, KernelMode, obj, 0);
    if (status != STATUS_SUCCESS)
    {
        DbgPrint("SetSecurityAllAccess-ObReferenceObjectByHandle failure!\r\n");
        return status;
    }
    DbgPrint("SetSecurityAllAccess-ObReferenceObjectByHandle completed!\r\n");

    // ---
    PACL pACL = NULL;
    PSECURITY_DESCRIPTOR pSecurityDescriptor = { 0 };
    status = CreateStandardSCAndACL(&pSecurityDescriptor, &pACL);
    if (status != STATUS_SUCCESS)
    {
        DbgPrint("SetSecurityAllAccess-CreateStandardSCAndACL failure! Status: %p\n", status);
        ObDereferenceObject(obj);
        return status;
    }

    status = GrantAccess(h, pACL);
    if (status != STATUS_SUCCESS)
    {
        DbgPrint("SetSecurityAllAccess-GrantAccess failure! Status: %p\n", status);
        ExFreePool(pACL);
        ExFreePool(pSecurityDescriptor);
        ObDereferenceObject(obj);
        return status;
    }

    DbgPrint("SetSecurityAllAccess method completed!\r\n");

    ExFreePool(pACL);
    ExFreePool(pSecurityDescriptor);
    return status;
}

NTSTATUS CreateSharedEventsKe(PDEVICE_OBJECT device_object)
{
    PDEVICE_EXTENSION device_extension = (PDEVICE_EXTENSION)device_object->DeviceExtension;
    WCHAR             symbolicNameBuffer[256];
//    RtlZeroBytes(symbolicNameBuffer, sizeof(wchar_t) * 2);
    OBJECT_ATTRIBUTES ObjectAttributes;
    NTSTATUS status;
    PKEVENT eventHandle;

    DbgPrint("CreateSharedEvents method started!\r\n");

    // event 1
    UNICODE_STRING uEventName = { 0 };
    uEventName.Length = 0;
    uEventName.MaximumLength = 256;
    uEventName.Buffer = (PWSTR)symbolicNameBuffer;
    RtlUnicodeStringPrintf(&uEventName, DEVICE_OBJECT_SHM_EVENT_REQUEST_DATA L"%u", device_extension->device_number);
    InitializeObjectAttributes(&ObjectAttributes, &uEventName, OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE, NULL, NULL);
    eventHandle = IoCreateSynchronizationEvent(&uEventName, &device_extension->DriverRequestDataSet);
//    RtlZeroBytes(symbolicNameBuffer, sizeof(wchar_t) * 2);

    if (eventHandle == NULL)
    {
        DbgPrint("CreateSharedEvents - DEVICE_OBJECT_SHM_EVENT_REQUEST_DATA Event fail!\r\n");
        return STATUS_UNSUCCESSFUL;
    }

    // set security event 1
    status = SetSecurityAllAccess(device_extension->DriverRequestDataSet, &device_extension->pDriverRequestDataSetObj);
    if (status != STATUS_SUCCESS)
    {
        ZwClose(device_extension->DriverRequestDataSet);
        return STATUS_UNSUCCESSFUL;
    }
    device_extension->KeDriverRequestDataSetObj = eventHandle;



    // event 2
    uEventName.Length = 0;
    uEventName.MaximumLength = 256;
    uEventName.Buffer = (PWSTR)symbolicNameBuffer;
    RtlUnicodeStringPrintf(&uEventName, DEVICE_OBJECT_SHM_EVENT_PROXY_IDLE L"%u", device_extension->device_number);
    InitializeObjectAttributes(&ObjectAttributes, &uEventName, OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE, NULL, NULL);
    eventHandle = IoCreateSynchronizationEvent(&uEventName, &device_extension->ProxyIdle);
//    RtlZeroBytes(symbolicNameBuffer, sizeof(wchar_t) * 2);

    if (eventHandle == NULL)
    {
        DbgPrint("CreateSharedEvents - DEVICE_OBJECT_SHM_EVENT_PROXY_IDLE Event fail!\r\n");
        ZwClose(device_extension->DriverRequestDataSet);
        return STATUS_UNSUCCESSFUL;
    }

    // set security event 2
    status = SetSecurityAllAccess(device_extension->ProxyIdle, &device_extension->pProxyIdleObj);
    if (status != STATUS_SUCCESS)
    {
        ZwClose(device_extension->DriverRequestDataSet);
        ZwClose(device_extension->ProxyIdle);
        return STATUS_UNSUCCESSFUL;
    }
    device_extension->KeProxyIdleObj = eventHandle;



    // event 3
    uEventName.Length = 0;
    uEventName.MaximumLength = 256;
    uEventName.Buffer = (PWSTR)symbolicNameBuffer;
    RtlUnicodeStringPrintf(&uEventName, DEVICE_OBJECT_SHM_REQUESTCOMPLETE L"%u", device_extension->device_number);
    InitializeObjectAttributes(&ObjectAttributes, &uEventName, OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE, NULL, NULL);
    eventHandle = IoCreateSynchronizationEvent(&uEventName, &device_extension->RequestComplete);
//    RtlZeroBytes(symbolicNameBuffer, sizeof(wchar_t) * 2);

    if (eventHandle == NULL)
    {
        DbgPrint("CreateSharedEvents - DEVICE_OBJECT_SHM_REQUESTCOMPLETE Event fail!\r\n");
        ZwClose(device_extension->DriverRequestDataSet);
        ZwClose(device_extension->ProxyIdle);
        return STATUS_UNSUCCESSFUL;
    }

    // set security event 3
    status = SetSecurityAllAccess(device_extension->RequestComplete, &device_extension->pRequestCompleteObj);
    if (status != STATUS_SUCCESS)
    {
        ZwClose(device_extension->DriverRequestDataSet);
        ZwClose(device_extension->ProxyIdle);
        ZwClose(device_extension->RequestComplete);
        return STATUS_UNSUCCESSFUL;
    }
    device_extension->KeRequestCompleteObj = eventHandle;

    // event 4
    uEventName.Length = 0;
    uEventName.MaximumLength = 256;
    uEventName.Buffer = (PWSTR)symbolicNameBuffer;
    RtlUnicodeStringPrintf(&uEventName, DEVICE_OBJECT_SHM_SHUTDOWN L"%u", device_extension->device_number);
    InitializeObjectAttributes(&ObjectAttributes, &uEventName, OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE, NULL, NULL);
    eventHandle = IoCreateSynchronizationEvent(&uEventName, &device_extension->Shutdown);

    if (eventHandle == NULL)
    {
        DbgPrint("CreateSharedEvents - DEVICE_OBJECT_SHM_SHUTDOWN Event fail!\r\n");
        ZwClose(device_extension->DriverRequestDataSet);
        ZwClose(device_extension->ProxyIdle);
        ZwClose(device_extension->RequestComplete);
        return STATUS_UNSUCCESSFUL;
    }

    // set security event 4
    status = SetSecurityAllAccess(device_extension->Shutdown, &device_extension->pShutdownObj);
    if (status != STATUS_SUCCESS)
    {
        ZwClose(device_extension->DriverRequestDataSet);
        ZwClose(device_extension->ProxyIdle);
        ZwClose(device_extension->RequestComplete);
        ZwClose(device_extension->Shutdown);
        return STATUS_UNSUCCESSFUL;
    }
    device_extension->KeShutdownObj = eventHandle;

    // event 5
    uEventName.Length = 0;
    uEventName.MaximumLength = 256;
    uEventName.Buffer = (PWSTR)symbolicNameBuffer;
    RtlUnicodeStringPrintf(&uEventName, DEVICE_OBJECT_SHM_SHUTDOWNCOMPLETE L"%u", device_extension->device_number);
    InitializeObjectAttributes(&ObjectAttributes, &uEventName, OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE, NULL, NULL);
    eventHandle = IoCreateSynchronizationEvent(&uEventName, &device_extension->ShutdownComplete);

    if (eventHandle == NULL)
    {
        DbgPrint("CreateSharedEvents - DEVICE_OBJECT_SHM_SHUTDOWNCOMPLETE Event fail!\r\n");
        ZwClose(device_extension->DriverRequestDataSet);
        ZwClose(device_extension->ProxyIdle);
        ZwClose(device_extension->RequestComplete);
        ZwClose(device_extension->Shutdown);
        return STATUS_UNSUCCESSFUL;
    }

    // set security event 5
    status = SetSecurityAllAccess(device_extension->ShutdownComplete, &device_extension->pShutdownCompleteObj);
    if (status != STATUS_SUCCESS)
    {
        ZwClose(device_extension->DriverRequestDataSet);
        ZwClose(device_extension->ProxyIdle);
        ZwClose(device_extension->RequestComplete);
        ZwClose(device_extension->Shutdown);
        ZwClose(device_extension->ShutdownComplete);
        return STATUS_UNSUCCESSFUL;
    }
    device_extension->KeShutdownCompleteObj = eventHandle;

    // reset initialize
    KeResetEvent(device_extension->KeDriverRequestDataSetObj);
    KeResetEvent(device_extension->KeProxyIdleObj);
    KeResetEvent(device_extension->KeRequestCompleteObj);
    KeResetEvent(device_extension->KeShutdownObj);
    KeResetEvent(device_extension->KeShutdownCompleteObj);

    DbgPrint("CreateSharedEvents method completed!\r\n");
    return status;
}

void DeleteSharedEvents(PDEVICE_OBJECT device_object)
{
    PDEVICE_EXTENSION device_extension = (PDEVICE_EXTENSION)device_object->DeviceExtension;
    DbgPrint("Driver-->DeviceObject->DeleteSharedEvents routine started - removing device shared events %u\r\n", device_extension->device_number);
    ObDereferenceObject(device_extension->pDriverRequestDataSetObj);
    ZwClose(device_extension->DriverRequestDataSet);
    ObDereferenceObject(device_extension->pProxyIdleObj);
    ZwClose(device_extension->ProxyIdle);
    ObDereferenceObject(device_extension->pRequestCompleteObj);
    ZwClose(device_extension->RequestComplete);
    ObDereferenceObject(device_extension->pShutdownObj);
    ZwClose(device_extension->Shutdown);
    ObDereferenceObject(device_extension->pShutdownCompleteObj);
    ZwClose(device_extension->ShutdownComplete);
    DbgPrint("Driver-->DeviceObject->DeleteSharedEvents routine completed!\r\n");
}

NTSTATUS CreateSharedMemory(PDEVICE_OBJECT device_object)
{
    NTSTATUS ntStatus = STATUS_UNSUCCESSFUL;
    PDEVICE_EXTENSION device_extension = (PDEVICE_EXTENSION)device_object->DeviceExtension;
    WCHAR             symbolicNameBuffer[256];
    RtlZeroBytes(symbolicNameBuffer, sizeof(wchar_t) * 2);

    DbgPrint("CreateSharedMemory method started!\r\n");

    UNICODE_STRING uSectionName = { 0 };
    uSectionName.Length = 0;
    uSectionName.MaximumLength = 256;
    uSectionName.Buffer = (PWSTR)symbolicNameBuffer;

    //RtlInitUnicodeString(&uSectionName, gc_wszSharedSectionName);
    RtlUnicodeStringPrintf(&uSectionName, DEVICE_OBJECT_SHM_NAME L"%u", device_extension->device_number);

    OBJECT_ATTRIBUTES objAttributes = { 0 };
    InitializeObjectAttributes(&objAttributes, &uSectionName, OBJ_CASE_INSENSITIVE, NULL, NULL);

    LARGE_INTEGER lMaxSize = { 0 };
    lMaxSize.HighPart = 0;
    lMaxSize.LowPart = DEVICE_OBJECT_SHM_SIZE_BYTES;// 1024 * 10;
    ntStatus = ZwCreateSection(&device_extension->g_hSection, SECTION_ALL_ACCESS, &objAttributes, &lMaxSize, PAGE_READWRITE, SEC_COMMIT, NULL);
    if (ntStatus != STATUS_SUCCESS)
    {
        DbgPrint("ZwCreateSection fail! Status: %p\n", ntStatus);
        return ntStatus;
    }
    DbgPrint("ZwCreateSection completed!\n");

    // set security shared memory all access
    ntStatus = SetSecurityAllAccess(device_extension->g_hSection, &device_extension->g_pSectionObj);
    if (ntStatus != STATUS_SUCCESS)
    {
        ZwClose(device_extension->g_hSection);
        return ntStatus;
    }

    SIZE_T ulViewSize = 0;
    ntStatus = ZwMapViewOfSection(device_extension->g_hSection, NtCurrentProcess(), &device_extension->g_pSharedSection, 0, lMaxSize.LowPart, NULL, &ulViewSize, ViewShare, 0, PAGE_READWRITE | PAGE_NOCACHE);
    DbgPrint("View size %u", ulViewSize);
    if (ntStatus != STATUS_SUCCESS)
    {
        DbgPrint("ZwMapViewOfSection fail! Status: %p\n", ntStatus);
        ObDereferenceObject(device_extension->g_pSectionObj);
        ZwClose(device_extension->g_hSection);
        return ntStatus;
    }
    DbgPrint("ZwMapViewOfSection completed!\n");

    //PCHAR TestString = "Message from kernel";
    //memcpy(g_pSharedSection, TestString, 19);
    //ReadSharedMemory();
    DbgPrint("CreateSharedMemory method completed!\r\n");

    return ntStatus;
}

void ReleaseSharedMemory(PDEVICE_OBJECT device_object)
{
    PDEVICE_EXTENSION device_extension = (PDEVICE_EXTENSION)device_object->DeviceExtension;
    DbgPrint("Driver-->DeviceObject->ReleaseSharedMemory routine started - removing device SHM %u\r\n", device_extension->device_number);

    if (device_extension->g_pSharedSection)
        ZwUnmapViewOfSection(NtCurrentProcess(), device_extension->g_pSharedSection);

    if (device_extension->g_pSectionObj)
        ObDereferenceObject(device_extension->g_pSectionObj);

    if (device_extension->g_hSection)
        ZwClose(device_extension->g_hSection);

    DbgPrint("Driver-->DeviceObject->ReleaseSharedMemory routine completed!\r\n");

    return TRUE;
}


NTSTATUS openPipe(PDEVICE_OBJECT device_object)
{
    NTSTATUS status = STATUS_UNSUCCESSFUL;
    PDEVICE_EXTENSION device_extension = (PDEVICE_EXTENSION)device_object->DeviceExtension;
    WCHAR symbolicNameBuffer[256];// = ExAllocatePoolWithTag(PagedPool, 1024, FILE_DISK_POOL_TAG);

    UNICODE_STRING link = { 0 };
    link.Length = 0;
    link.MaximumLength = 256;
    link.Buffer = (PWSTR)symbolicNameBuffer;
    RtlUnicodeStringPrintf(&link, L"%s%u", PIPE_NAME_DRIVER, device_extension->device_number);
    OBJECT_ATTRIBUTES object_attributes;
    IO_STATUS_BLOCK iostatus;

    InitializeObjectAttributes(
        &object_attributes,
        &link,
        OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE,
        NULL,
        NULL);
    
    status = ZwCreateFile(
        &device_extension->pipe, // File handle
        GENERIC_ALL | SYNCHRONIZE,
        //FILE_READ_DATA | FILE_WRITE_DATA | SYNCHRONIZE, // Desired access
        //GENERIC_READ | GENERIC_WRITE | SYNCHRONIZE,
        &object_attributes, // Attributes
        &iostatus, // IO Status Block
        NULL, // Allocation size
        0, // File attributes
        FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, // File share
        FILE_OPEN, // Create disposition (must exist)
        FILE_WRITE_THROUGH | FILE_NO_INTERMEDIATE_BUFFERING | FILE_NON_DIRECTORY_FILE | FILE_SYNCHRONOUS_IO_ALERT,// Create options
        NULL, // EA buffer
        0); // EA size

    return status;
}

NTSTATUS closePipe(PDEVICE_OBJECT device_object)
{
    PDEVICE_EXTENSION device_extension = (PDEVICE_EXTENSION)device_object->DeviceExtension;
    NTSTATUS status = ZwClose(device_extension->pipe);
    device_extension->pipe = NULL;
    return status;
}

BOOL isEventSignalled(PKEVENT KeEvent)
{
    LARGE_INTEGER timeout = { 0,0 };
    timeout.QuadPart = 0;
    NTSTATUS WaitStatus;

    WaitStatus = KeWaitForSingleObject(KeEvent, UserRequest, UserMode, FALSE, &timeout);
    if (WaitStatus == STATUS_SUCCESS)
    {
        return TRUE;
    }
    else if (WaitStatus == STATUS_TIMEOUT)
    {
        return FALSE;   
    }
    else
    {
        return FALSE;
    }
}

void printStatusFile(char* path, char* status)
{
    UNICODE_STRING     uniName;
    UNICODE_STRING     uniText;
    OBJECT_ATTRIBUTES  objAttr;

    RtlInitUnicodeString(&uniText, status);
    RtlInitUnicodeString(&uniName, path);  // or L"\\SystemRoot\\example.txt"
    InitializeObjectAttributes(&objAttr, &uniName,
        OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE,
        NULL, NULL);

    HANDLE   handle;
    NTSTATUS ntstatus;
    IO_STATUS_BLOCK    ioStatusBlock;

    // Do not try to perform any file operations at higher IRQL levels.
    // Instead, you may use a work item or a system worker thread to perform file operations.

    if (KeGetCurrentIrql() != PASSIVE_LEVEL)
        return STATUS_INVALID_DEVICE_STATE;

    ntstatus = ZwCreateFile(&handle,
        GENERIC_WRITE,
        &objAttr, &ioStatusBlock, NULL,
        FILE_ATTRIBUTE_NORMAL,
        0,
        FILE_OVERWRITE_IF,
        FILE_SYNCHRONOUS_IO_NONALERT,
        NULL, 0);

#define  BUFFER_SIZE 1024
    CHAR     buffer[BUFFER_SIZE];
    size_t  cb;

    if (NT_SUCCESS(ntstatus)) {
        ntstatus = RtlStringCbPrintfA(buffer, sizeof(buffer), status);// , 0x0);
        if (NT_SUCCESS(ntstatus)) {
            ntstatus = RtlStringCbLengthA(buffer, sizeof(buffer), &cb);
            if (NT_SUCCESS(ntstatus)) {
                ntstatus = ZwWriteFile(handle, NULL, NULL, NULL, &ioStatusBlock,
                    buffer, cb, NULL, NULL);
            }
        }
        ZwClose(handle);
    }
    //RtlFreeUnicodeString(&uniName);
    //RtlFreeUnicodeString(&uniText);

}

HANDLE initLog(char* file, ULONG DeviceNumber)
{
    HANDLE handle;
    UNICODE_STRING     uniFileWithDeviceNumber = { 0 };
    WCHAR             uniNameBuffer[512];
    OBJECT_ATTRIBUTES  objAttr;
    //ANSI_STRING ansiString;
    NTSTATUS ntstatus;
    IO_STATUS_BLOCK    ioStatusBlock;

    uniFileWithDeviceNumber.Length = 0;
    uniFileWithDeviceNumber.MaximumLength = 512;
    uniFileWithDeviceNumber.Buffer = (PWSTR)uniNameBuffer;
    //RtlInitAnsiString(&ansiString, file);    
//    RtlInitUnicodeString(&uniFile, file);
    //RtlAnsiStringToUnicodeString(&uniFile, &ansiString, TRUE);
    RtlUnicodeStringPrintf(&uniFileWithDeviceNumber, L"%s%u.txt", file, DeviceNumber);
    //RtlFreeUnicodeString(&uniFile);

    //RtlUnicodeStringPrintf(&uniFileWithDeviceNumber, L"%s%u", &uniFile, DeviceNumber);
    //RtlFreeAnsiString(&ansiString);

    InitializeObjectAttributes(&objAttr, &uniFileWithDeviceNumber,
        OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE,
        NULL, NULL);

    ntstatus = ZwCreateFile(&handle,
        GENERIC_WRITE,
        &objAttr, &ioStatusBlock, NULL,
        FILE_ATTRIBUTE_NORMAL,
        FILE_SHARE_READ,
        FILE_OVERWRITE_IF,
        FILE_SYNCHRONOUS_IO_NONALERT,
        NULL, 0);

    //printStatusFile(L"\\DosDevices\\C:\\initLog-failure.txt", "initlog failure.");

    if (ntstatus != STATUS_SUCCESS)
        return NULL;

    return handle;
}

VOID Log(HANDLE handle, char* text) {

    if (handle == NULL)
        return;

    CHAR     buffer[4096];
    size_t  cb;
    NTSTATUS ntstatus;
    IO_STATUS_BLOCK    ioStatusBlock;
    ntstatus = RtlStringCbPrintfA(buffer, sizeof(buffer), "%s\r\n", text);
    //ntstatus = RtlStringCbPrintfA(buffer, sizeof(buffer), text, 0x0);
    if (NT_SUCCESS(ntstatus)) {
        ntstatus = RtlStringCbLengthA(buffer, sizeof(buffer), &cb);
        if (NT_SUCCESS(ntstatus)) {
            ntstatus = ZwWriteFile(handle, NULL, NULL, NULL, &ioStatusBlock,
                buffer, (ULONG)cb, NULL, NULL);
            ZwFlushBuffersFile(handle, &ioStatusBlock);
        }
    }
}

VOID freeLog(HANDLE handle)
{
    ZwClose(handle);
}

void ReadPipeToBuffer(LPBYTE buffer, HANDLE pipe, size_t size)
{
    NTSTATUS status;
    DWORD bytesDone;
    UINT64 totalVectors = 1;
    size_t stride = totalVectors * PIPE_BUFFER_SIZE;
    IO_STATUS_BLOCK iostatus;

    while (size)
    {
        status = ZwReadFile(pipe, NULL, NULL, NULL, &iostatus, buffer, stride, NULL, NULL);
        size -= stride;
        buffer += stride;
    }
}

void fastPipeToBuffer(LPVOID buffer, HANDLE pipe, size_t pos, size_t nBytes)
{

    NTSTATUS status;
    DWORD bytesDone;
    double double_totalBlocks = (double)nBytes / (double)PIPE_BUFFER_SIZE;
    INT64 ltotalBlocks_floor = (INT64)floor(double_totalBlocks);
    INT64 ltotalBlocks_ceiling = (INT64)ceil(double_totalBlocks);
    INT64 lProcessingBytes = 0;
    IO_STATUS_BLOCK iostatus;

    // set buffer offset
    LPBYTE dest = (LPBYTE)buffer;
    dest += pos;

    // copy all the blocks of PIPE_BUFFER_SIZE number of bytes
    lProcessingBytes = ltotalBlocks_floor * PIPE_BUFFER_SIZE;
    ReadPipeToBuffer(dest, pipe, lProcessingBytes);

    pos += lProcessingBytes;
    dest += lProcessingBytes;

    INT64 lremainingBytes = nBytes - lProcessingBytes;
    if (lremainingBytes > 0)
    {
        status = ZwReadFile(pipe, NULL, NULL, NULL, &iostatus, dest, lremainingBytes, NULL, NULL);
        pos += lremainingBytes;
        dest += lremainingBytes;
    }
}

void WriteBufferToPipe(LPBYTE buffer, HANDLE pipe, size_t size)
{
    BOOL result;
    DWORD bytesDone;
    UINT64 totalVectors = 1;
    size_t stride = totalVectors * PIPE_BUFFER_SIZE;
    IO_STATUS_BLOCK iostatus;
    LARGE_INTEGER offsettmp = { 0,0 };
    offsettmp.QuadPart = 0;

    while (size)
    {
        ZwWriteFile(pipe, NULL, NULL, NULL, &iostatus, buffer, stride, &offsettmp, NULL);
        ZwFlushBuffersFile(pipe, &iostatus);
        size -= stride;
        buffer += stride;
    }
}

void fastBufferToPipe(LPVOID buffer, HANDLE pipe, size_t pos, size_t nBytes)
{

    NTSTATUS status;
    DWORD bytesDone;
    double double_totalBlocks = (double)nBytes / (double)PIPE_BUFFER_SIZE;
    INT64 ltotalBlocks_floor = (INT64)floor(double_totalBlocks);
    INT64 ltotalBlocks_ceiling = (INT64)ceil(double_totalBlocks);
    INT64 lProcessingBytes = 0;
    IO_STATUS_BLOCK iostatus;
    LARGE_INTEGER offsettmp = { 0,0 };
    offsettmp.QuadPart = 0;

    // set buffer offset
    LPBYTE src = (LPBYTE)buffer;
    src += pos;

    // copy all the blocks of PIPE_BUFFER_SIZE number of bytes
    lProcessingBytes = ltotalBlocks_floor * PIPE_BUFFER_SIZE;
    WriteBufferToPipe(src, pipe, lProcessingBytes);

    pos += lProcessingBytes;
    src += lProcessingBytes;

    INT64 lremainingBytes = nBytes - lProcessingBytes;
    if (lremainingBytes > 0)
    {
        ZwWriteFile(pipe, NULL, NULL, NULL, &iostatus, src, lremainingBytes, &offsettmp, NULL);
        ZwFlushBuffersFile(pipe, &iostatus);
        pos += lremainingBytes;
        src += lremainingBytes;
    }
}

#pragma code_seg() // end "PAGE"
