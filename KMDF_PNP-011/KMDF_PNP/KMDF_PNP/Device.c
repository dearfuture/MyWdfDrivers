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
	WDF_PNPPOWER_EVENT_CALLBACKS pnpPowerCallbacks;
	WDF_DEVICE_POWER_POLICY_IDLE_SETTINGS idlesettings;
	
	WDF_OBJECT_ATTRIBUTES_INIT_CONTEXT_TYPE(&deviceAttributes, DEVICE_CONTEXT);
	deviceAttributes.SynchronizationScope = WdfSynchronizationScopeDevice;
	WDF_PNPPOWER_EVENT_CALLBACKS_INIT(&pnpPowerCallbacks);
	//pnp
	pnpPowerCallbacks.EvtDevicePrepareHardware = EvtDevicePrepareHardware;
	pnpPowerCallbacks.EvtDeviceReleaseHardware = EvtDeviceReleaseHardware;
	
	//power
	pnpPowerCallbacks.EvtDeviceD0Entry = EvtDeviceD0Entry;
	pnpPowerCallbacks.EvtDeviceD0Exit = EvtDeviceD0Exit;
	pnpPowerCallbacks.EvtDeviceD0EntryPostInterruptsEnabled = EvtDeviceD0EntryPostInterruptsEnabled;
	pnpPowerCallbacks.EvtDeviceD0ExitPreInterruptsDisabled = EvtDeviceD0ExitPreInterruptsDisabled;

	//self manager io
	pnpPowerCallbacks.EvtDeviceSelfManagedIoInit = EvtDeviceSelfManagedIoInit;
	pnpPowerCallbacks.EvtDeviceSelfManagedIoRestart = EvtDeviceSelfManagedIoRestart;
	pnpPowerCallbacks.EvtDeviceSelfManagedIoSuspend = EvtDeviceSelfManagedIoSuspend;
	pnpPowerCallbacks.EvtDeviceSelfManagedIoFlush = EvtDeviceSelfManagedIoFlush;
	pnpPowerCallbacks.EvtDeviceSelfManagedIoCleanup = EvtDeviceSelfManagedIoCleanup;
	WdfDeviceInitSetPnpPowerEventCallbacks(DeviceInit, &pnpPowerCallbacks);
	WdfDeviceInitSetIoInCallerContextCallback(DeviceInit, EvtIoInCallerContext);
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


	WDF_DEVICE_POWER_POLICY_IDLE_SETTINGS_INIT(&idlesettings, IdleCannotWakeFromS0);
	idlesettings.IdleTimeout = 10000;

	WdfDeviceAssignS0IdleSettings(Device, &idlesettings);
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


VOID EvtIoInCallerContext(
	_In_ WDFDEVICE  Device,
	_In_ WDFREQUEST Request
	)
{
	NTSTATUS status;
	WDF_REQUEST_PARAMETERS params;
	WDF_OBJECT_ATTRIBUTES attributes;
	PVOID outBuf;
	PREQUEST_CONTEXT pRequestContext = NULL;
	size_t outLen;
	
	WDF_REQUEST_PARAMETERS_INIT(&params);

	WdfRequestGetParameters(Request, &params);
	if (!(params.Type== WdfRequestTypeDeviceControl&&params.Parameters.DeviceIoControl.IoControlCode==IOCTL_READ_BOOLEAN))
	{
		status = WdfDeviceEnqueueRequest(Device, Request);
		if (!NT_SUCCESS(status))
		{ 
			goto exit;
		}
		return;
	}

	status = WdfRequestRetrieveUnsafeUserOutputBuffer(Request, 0, &outBuf, &outLen);
	if (!NT_SUCCESS(status))
	{
		goto exit;
	}
	WDF_OBJECT_ATTRIBUTES_INIT_CONTEXT_TYPE(&attributes, REQUEST_CONTEXT);
	status = WdfObjectAllocateContext(Request, &attributes, &pRequestContext);
	if (!NT_SUCCESS(status))
	{
		goto exit;
	}

	status = WdfRequestProbeAndLockUserBufferForWrite(Request, outBuf, outLen, &pRequestContext->OutputMemory);
	if (!NT_SUCCESS(status))
	{
		goto exit;
	}

	status = WdfDeviceEnqueueRequest(Device, Request);
	if (!NT_SUCCESS(status))
	{
		goto exit;
	}

	return;


exit:
	WdfRequestComplete(Request, status);

}


NTSTATUS EvtDevicePrepareHardware(
	_In_ WDFDEVICE    Device,
	_In_ WDFCMRESLIST ResourcesRaw,
	_In_ WDFCMRESLIST ResourcesTranslated
	)
{
	
	KdPrint(("EvtDevicePrepareHardware\n"));
	return STATUS_SUCCESS;
}


NTSTATUS EvtDeviceReleaseHardware(
	_In_  WDFDEVICE Device,
	_In_  WDFCMRESLIST ResourcesTranslated
	)
{
	KdPrint(("EvtDeviceReleaseHardware\n"));
	return STATUS_SUCCESS;
}

NTSTATUS EvtDeviceD0Entry(
	_In_ WDFDEVICE              Device,
	_In_ WDF_POWER_DEVICE_STATE PreviousState
	)
{
	static PCHAR PowerStates[] = {
		"WdfPowerDeviceInvalid				 ",
		"WdfPowerDeviceD0					 ",
		"WdfPowerDeviceD1					 ",
		"WdfPowerDeviceD2					 ",
		"WdfPowerDeviceD3					 ",
		"WdfPowerDeviceD3Final				 ",
		"WdfPowerDevicePrepareForHibernation ",
		"WdfPowerDeviceMaximum				 "
	};
	KdPrint(("EvtDeviceD0Entry %s\n", PowerStates[PreviousState]));
	return STATUS_SUCCESS;

}

NTSTATUS EvtDeviceD0EntryPostInterruptsEnabled(
	_In_ WDFDEVICE              Device,
	_In_ WDF_POWER_DEVICE_STATE PreviousState
	)
{
	KdPrint(("EvtDeviceD0EntryPostInterruptsEnabled\n"));
	return STATUS_SUCCESS;
}

NTSTATUS EvtDeviceD0Exit(
	_In_  WDFDEVICE Device,
	_In_  WDF_POWER_DEVICE_STATE TargetState
	)
{
	static PCHAR PowerStates[] = {
		"WdfPowerDeviceInvalid				 ",
		"WdfPowerDeviceD0					 ",
		"WdfPowerDeviceD1					 ",
		"WdfPowerDeviceD2					 ",
		"WdfPowerDeviceD3					 ",
		"WdfPowerDeviceD3Final				 ",
		"WdfPowerDevicePrepareForHibernation ",
		"WdfPowerDeviceMaximum				 "
	};
	KdPrint(("EvtDeviceD0Exit %s\n", PowerStates[TargetState]));
	return STATUS_SUCCESS;
}

NTSTATUS EvtDeviceD0ExitPreInterruptsDisabled(
	_In_ WDFDEVICE              Device,
	_In_ WDF_POWER_DEVICE_STATE TargetState
	)
{
	KdPrint(("EvtDeviceD0ExitPreInterruptsDisabled\n"));
	return STATUS_SUCCESS;
}

VOID EvtDeviceSelfManagedIoCleanup(
	_In_  WDFDEVICE Device
	)
{
	KdPrint(("EvtDeviceSelfManagedIoCleanup\n"));
}

VOID EvtDeviceSelfManagedIoFlush(
	_In_ WDFDEVICE Device
	)
{
	KdPrint(("EvtDeviceSelfManagedIoFlush\n"));
}

NTSTATUS EvtDeviceSelfManagedIoInit(
	_In_ WDFDEVICE Device
	)
{
	KdPrint(("EvtDeviceSelfManagedIoInit\n"));
	return STATUS_SUCCESS;
}

NTSTATUS EvtDeviceSelfManagedIoSuspend(
	_In_ WDFDEVICE Device
	)
{
	KdPrint(("EvtDeviceSelfManagedIoSuspend\n"));
	return STATUS_SUCCESS;
}

NTSTATUS EvtDeviceSelfManagedIoRestart(
	_In_  WDFDEVICE Device
	)
{
	KdPrint(("EvtDeviceSelfManagedIoRestart\n"));
	return STATUS_SUCCESS;
}





