#include <ntddk.h>
#include <wdf.h>
#include <initguid.h>
#include "Queue.h"
#include "Device.h"

// {a3c9b20a-9f17-40f3-a67b-bc37ba9569a6} 
DEFINE_GUID(DEVICEINTERFACE,
	0xa3c9b20a, 0x9f17, 0x40f3, 0xa6, 0x7b, 0xbc, 0x37, 0xba, 0x95, 0x69, 0xa6);
NTSTATUS EvtDriverDeviceAdd(
	_In_    WDFDRIVER       Driver,
	_Inout_ PWDFDEVICE_INIT DeviceInit
	)
{
	NTSTATUS status = STATUS_SUCCESS;
	WDFDEVICE Device;
	WDFQUEUE Queue;
	WDF_OBJECT_ATTRIBUTES deviceAttributes;
	PDEVICE_CONTEXT pDeviceContext = NULL;
	WDF_IO_QUEUE_CONFIG IoConfig;
	
	WDF_OBJECT_ATTRIBUTES_INIT_CONTEXT_TYPE(&deviceAttributes, DEVICE_CONTEXT);
	deviceAttributes.SynchronizationScope = WdfSynchronizationScopeDevice;
	status = WdfDeviceCreate(&DeviceInit, &deviceAttributes, &Device);
	if (!NT_SUCCESS(status))
	{
		KdPrint(("设备创建失败\n"));
		return status;
	}

	pDeviceContext = GetDeviceContext(Device);
	MyTimerCreate(&pDeviceContext->Timer, Device);
	WDF_IO_QUEUE_CONFIG_INIT_DEFAULT_QUEUE(&IoConfig, WdfIoQueueDispatchSequential);
	IoConfig.EvtIoDeviceControl = EvtIoDeviceControl;
	status = WdfIoQueueCreate(Device, &IoConfig, WDF_NO_OBJECT_ATTRIBUTES, &Queue);
	if (!NT_SUCCESS(status))
	{
		KdPrint(("对队创建失败\n"));
		return status;
	}

	WDF_IO_QUEUE_CONFIG_INIT(&IoConfig, WdfIoQueueDispatchManual);
	IoConfig.EvtIoCanceledOnQueue = EvtIoCanceledOnQueue;
	IoConfig.PowerManaged = WdfFalse;
	status = WdfIoQueueCreate(Device, &IoConfig, WDF_NO_OBJECT_ATTRIBUTES, &pDeviceContext->Queue);
	if (!NT_SUCCESS(status))
	{
		KdPrint(("手工队列创建失败%x\n",status));
		return status;
	}

	WdfDeviceCreateDeviceInterface(Device, &DEVICEINTERFACE, NULL);
	KdPrint(("创建设备成功\n"));
	return status;

}

NTSTATUS MyTimerCreate(IN WDFTIMER* TImer, IN WDFDEVICE Device)
{
	NTSTATUS status;
	WDF_TIMER_CONFIG timerConfig;
	WDF_OBJECT_ATTRIBUTES timerAttributes;

	WDF_TIMER_CONFIG_INIT(&timerConfig, EvtTimerFunc);
	WDF_OBJECT_ATTRIBUTES_INIT(&timerAttributes);
	timerAttributes.ParentObject = Device;
	status = WdfTimerCreate(&timerConfig, &timerAttributes, TImer);

	return status; 


}


