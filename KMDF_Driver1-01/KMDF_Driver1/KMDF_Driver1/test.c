#include <ntddk.h>
#include <wdf.h>

VOID EvtDeviceFileCreate(
	_In_ WDFDEVICE     Device,
	_In_ WDFREQUEST    Request,
	_In_ WDFFILEOBJECT FileObject
	);
VOID EvtFileClose(
	_In_ WDFFILEOBJECT FileObject
	);

VOID EvtFileCleanup(
	_In_  WDFFILEOBJECT FileObject
	);
VOID EvtIoDeviceControl(
	_In_  WDFQUEUE Queue,
	_In_  WDFREQUEST Request,
	_In_  size_t OutputBufferLength,
	_In_  size_t InputBufferLength,
	_In_  ULONG IoControlCode
	);
VOID EvtDriverUnload(IN WDFDRIVER Driver)
{
	KdPrint(("驱动卸载\n"));

}


NTSTATUS DriverEntry(IN PDRIVER_OBJECT DriverObject,IN PUNICODE_STRING RegistryPath)
{
	WDF_DRIVER_CONFIG config;
	WDFDEVICE Device;
	WDFDRIVER Driver;
	WDFQUEUE Queue;
	WDF_FILEOBJECT_CONFIG FileConfig;
	WDF_IO_QUEUE_CONFIG ioConfig;
	UNICODE_STRING DeviceName = RTL_CONSTANT_STRING(L"\\Device\\HelloWDF");
	UNICODE_STRING SymbolicLinkName = RTL_CONSTANT_STRING(L"\\??\\HelloWDF");
	PWDFDEVICE_INIT DeviceInit;
	NTSTATUS status = STATUS_SUCCESS;

	WDF_DRIVER_CONFIG_INIT(&config, NULL);
	config.DriverInitFlags = WdfDriverInitNonPnpDriver;
	config.EvtDriverUnload = EvtDriverUnload;

	status = WdfDriverCreate(DriverObject, RegistryPath, WDF_NO_OBJECT_ATTRIBUTES, &config, &Driver);
	if (!NT_SUCCESS(status))
	{
		KdPrint(("驱动框架创建失败\n"));
	}
	else
	{
		KdPrint(("驱动框架创建成功\n"));
	}
	DeviceInit = WdfControlDeviceInitAllocate(Driver, &SDDL_DEVOBJ_SYS_ALL_ADM_RWX_WORLD_RWX_RES_RWX);
	if (DeviceInit == NULL)
	{
		status = STATUS_INSUFFICIENT_RESOURCES;
		goto End;
	}

	status = WdfDeviceInitAssignName(DeviceInit, &DeviceName);
	if (!NT_SUCCESS(status))
	{
		KdPrint(("设备名分配失败%x\n", status));
		goto End;
	}

	WDF_FILEOBJECT_CONFIG_INIT(&FileConfig, EvtDeviceFileCreate, EvtFileClose, EvtFileCleanup);
	WdfDeviceInitSetFileObjectConfig(DeviceInit, &FileConfig, WDF_NO_OBJECT_ATTRIBUTES);

	WDF_IO_QUEUE_CONFIG_INIT_DEFAULT_QUEUE(&ioConfig, WdfIoQueueDispatchSequential);
	ioConfig.EvtIoDeviceControl = EvtIoDeviceControl;

	

	status = WdfDeviceCreate(&DeviceInit, WDF_NO_OBJECT_ATTRIBUTES, &Device);
	if (!NT_SUCCESS(status))
	{
		KdPrint(("设备创建失败%x\n", status));
		goto End;
	}

	status = WdfIoQueueCreate(Device, &ioConfig, WDF_NO_OBJECT_ATTRIBUTES, &Queue);
	if (!NT_SUCCESS(status))
	{
		KdPrint(("队列创建失败%x\n", status));
		goto End;
	}
	status = WdfDeviceCreateSymbolicLink(Device, &SymbolicLinkName);
	if (!NT_SUCCESS(status))
	{
		KdPrint(("符号链接创建失败%x\n", status));
		goto End;
	}

	WdfControlFinishInitializing(Device);


End:

	return status;
}

VOID EvtDeviceFileCreate(
	_In_ WDFDEVICE     Device,
	_In_ WDFREQUEST    Request,
	_In_ WDFFILEOBJECT FileObject
	)
{
	KdPrint(("create\n"));
	WdfRequestComplete(Request, STATUS_SUCCESS);
	WdfRequestGetStatus();
	WdfRequestSetInformation()
}

VOID EvtFileClose(
	_In_ WDFFILEOBJECT FileObject
	)
{
	KdPrint(("EvtFileClose\n"));
}

VOID EvtFileCleanup(
	_In_  WDFFILEOBJECT FileObject
	)
{
	KdPrint(("EvtFileCleanup\n"));
}


VOID EvtIoDeviceControl(
	_In_  WDFQUEUE Queue,
	_In_  WDFREQUEST Request,
	_In_  size_t OutputBufferLength,
	_In_  size_t InputBufferLength,
	_In_  ULONG IoControlCode
	)
{
	
	KdPrint(("驱动io操作\n")); 
	WdfRequestComplete(Request, STATUS_SUCCESS);

}
