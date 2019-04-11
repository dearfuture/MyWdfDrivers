#include <ntddk.h>
#include <wdf.h>
#include "Device.h"

NTSTATUS DriverEntry(IN PDRIVER_OBJECT DriverObject, IN PUNICODE_STRING RegistryPath)
{
	NTSTATUS status = STATUS_SUCCESS;
	WDF_DRIVER_CONFIG config;
	WDF_DRIVER_CONFIG_INIT(&config, EvtDriverDeviceAdd);
	
	status = WdfDriverCreate(DriverObject, RegistryPath, WDF_NO_OBJECT_ATTRIBUTES, &config, WDF_NO_HANDLE);
	if (!NT_SUCCESS(status))
	{
		KdPrint(("注册驱动的框架对象失败%d\n", status));
	}
	KdPrint(("注册驱动的框架对象成功\n"));
	return status;
}