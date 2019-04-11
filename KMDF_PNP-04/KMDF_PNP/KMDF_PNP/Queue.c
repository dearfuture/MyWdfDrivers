#include <ntddk.h>
#include <wdf.h>
#include "Device.h"
#include "Queue.h"

#define IOCTL_TEST	CTL_CODE(FILE_DEVICE_UNKNOWN,0X800,METHOD_BUFFERED,FILE_ANY_ACCESS)
VOID EvtIoDeviceControl(
	_In_  WDFQUEUE Queue,
	_In_  WDFREQUEST Request,
	_In_  size_t OutputBufferLength,
	_In_  size_t InputBufferLength,
	_In_  ULONG IoControlCode
	)
{

	KdPrint(("有io请求\n"));
	PVOID Buffer = NULL;
	size_t Length = 0;
	NTSTATUS status;
	CHAR n;
	CHAR cc[] = "〇一二三四五六七八九";
	PDEVICE_CONTEXT pDeviceContext = NULL;
	pDeviceContext = GetDeviceContext(WdfIoQueueGetDevice(Queue));
	switch (IoControlCode)
	{
	case IOCTL_TEST:
		if (InputBufferLength == 0 || OutputBufferLength < 2)
		{
			KdPrint(("参数无效\n"));
			WdfRequestComplete(Request, STATUS_INVALID_PARAMETER);
			break;
		}
		else
		{
			status = WdfRequestForwardToIoQueue(Request, pDeviceContext->Queue);
			if (!NT_SUCCESS(status))
			{
				WdfRequestComplete(Request, STATUS_UNSUCCESSFUL);
				return;
			}
			WdfTimerStart(pDeviceContext->Timer, WDF_REL_TIMEOUT_IN_SEC(3));
		}
		break;
	default:
		WdfRequestCompleteWithInformation(Request, STATUS_INVALID_PARAMETER, 0);

		break;
	}



	
	
}


VOID EvtTimerFunc(
	_In_  WDFTIMER Timer
	)
{
	WDFREQUEST Request;
	NTSTATUS status;
	WDFDEVICE Device = WdfTimerGetParentObject(Timer);
	PDEVICE_CONTEXT pDeviceContext = GetDeviceContext(Device);
	PVOID Buffer = NULL;
	size_t Length = 0;
	CHAR n;
	CHAR cc[] = "〇一二三四五六七八九";
	status = WdfIoQueueRetrieveNextRequest(pDeviceContext->Queue, &Request);
	if (!NT_SUCCESS(status))
	{
		return;
	}

	status = WdfRequestRetrieveInputBuffer(Request, 1, &Buffer, &Length);
	if (!NT_SUCCESS(status))
	{
		WdfRequestComplete(Request, STATUS_INVALID_PARAMETER);
	}

	n = *(PCHAR)Buffer;
	if (n >= '0'&&n <= '9')
	{
		n -= '0';
		status = WdfRequestRetrieveOutputBuffer(Request, 1, &Buffer, &Length);
		if (!NT_SUCCESS(status))
		{
			WdfRequestComplete(Request, STATUS_INVALID_PARAMETER);
			return;
		}
		strncpy(Buffer, &cc[n * 2], 2);
		WdfRequestCompleteWithInformation(Request, STATUS_SUCCESS, 2);
	}
	else
	{
		WdfRequestComplete(Request, STATUS_INVALID_PARAMETER);
	}

}


VOID EvtIoCanceledOnQueue(
	_In_ WDFQUEUE   Queue,
	_In_ WDFREQUEST Request
	)
{
	
	WDFDEVICE Device;
	PDEVICE_CONTEXT pDeviceContext = NULL;
	Device = WdfIoQueueGetDevice(Queue);
	pDeviceContext = GetDeviceContext(Device);
	WdfRequestCompleteWithInformation(Request, STATUS_CANCELLED, 0);
	WdfTimerStop(pDeviceContext->Timer, FALSE);
}
