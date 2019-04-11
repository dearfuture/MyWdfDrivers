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
		KdPrint(("ע�������Ŀ�ܶ���ʧ��%d\n", status));
	}
	KdPrint(("ע�������Ŀ�ܶ���ɹ�\n"));
	return status;
}