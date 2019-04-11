#include <ntddk.h>
#include <wdf.h>
#include "Device.h"
#include "Driver.h"

NTSTATUS DriverEntry(IN PDRIVER_OBJECT DriverObject, IN PUNICODE_STRING RegistryPath)
{
	NTSTATUS status = STATUS_SUCCESS;
	WDF_OBJECT_ATTRIBUTES driverAttributes;
	WDF_DRIVER_CONFIG config;
	//PDRIVER_CONTEXT pDriverContext = NULL;
	WDFDRIVER Driver;
	WDF_DRIVER_CONFIG_INIT(&config, EvtDriverDeviceAdd);
	
	WDF_OBJECT_ATTRIBUTES_INIT_CONTEXT_TYPE(&driverAttributes, DRIVER_CONTEXT);

	status = WdfDriverCreate(DriverObject, RegistryPath, &driverAttributes, &config, &Driver);
	if (!NT_SUCCESS(status))
	{
		KdPrint(("ע�������Ŀ�ܶ���ʧ��%d\n", status));
	}
	//pDriverContext = GetDriverContext(Driver);
	LoadParameter(Driver);

	KdPrint(("ע�������Ŀ�ܶ���ɹ�\n"));
	return status;
}

NTSTATUS LoadParameter(IN WDFDRIVER Driver)
{
	NTSTATUS status = STATUS_SUCCESS;
	PDRIVER_CONTEXT pDriverContext = NULL;
	WDFKEY hKey;
	UNICODE_STRING ValueName;
	UNICODE_STRING usValue;
	USHORT Len;
	pDriverContext = GetDriverContext(Driver);

	status = WdfDriverOpenParametersRegistryKey(Driver, KEY_READ | KEY_WRITE, WDF_NO_OBJECT_ATTRIBUTES, &hKey);
	if (!NT_SUCCESS(status))
	{
		return status;
	}
	RtlInitUnicodeString(&ValueName, L"����");
	status = WdfRegistryQueryULong(hKey, &ValueName, &pDriverContext->Number);
	if (NT_SUCCESS(status))
	{
		KdPrint(("����:%d\n", pDriverContext->Number));
	}
	RtlInitUnicodeString(&ValueName, L"����");
	status = WdfRegistryQueryULong(hKey, &ValueName, &pDriverContext->Boolean);
	if (NT_SUCCESS(status))
	{
		KdPrint(("����:%d\n", pDriverContext->Boolean));
	}

	usValue.Buffer = pDriverContext->sz;
	usValue.MaximumLength = sizeof(pDriverContext->sz);
	usValue.Length = 0;

	RtlInitUnicodeString(&ValueName, L"�ַ�");
	status = WdfRegistryQueryUnicodeString(hKey, &ValueName, &Len, &usValue);
	if (NT_SUCCESS(status))
	{
		KdPrint(("�ַ�:%wZ\n", &usValue));
	}

	return status;
}