#include <ntddk.h>
#include <wdf.h>
#include "Device.h"
#include "Queue.h"
#include "Driver.h"





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
	case IOCTL_READ_DWORD:
	{
		READ_DWORD(Request, OutputBufferLength, InputBufferLength);
	}
		break;
	case IOCTL_READ_BOOLEAN:
	{
		READ_BOOLEAN(Request, OutputBufferLength, InputBufferLength);
	}
		break;
	case IOCTL_READ_STRING:
	{
		READ_STRING(Request, OutputBufferLength, InputBufferLength);
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


VOID READ_DWORD(WDFREQUEST Request, ULONG OutputBufferLength, ULONG InputBufferLength)
{
	NTSTATUS status;
	PVOID Buffer;
	PDRIVER_CONTEXT pDriverContext;
	WDFDEVICE Device;
	WDFDRIVER Driver;
	Device = WdfIoQueueGetDevice(WdfRequestGetIoQueue(Request));
	Driver = WdfDeviceGetDriver(Device);
	pDriverContext = GetDriverContext(Driver);
	if (OutputBufferLength>=sizeof(ULONG))
	{
		status = WdfRequestRetrieveOutputBuffer(Request, sizeof(ULONG), &Buffer, NULL);
		if (!NT_SUCCESS(status))
		{
			WdfRequestComplete(Request, STATUS_UNSUCCESSFUL);
			return;
		}
		*(PULONG)Buffer = pDriverContext->Number;
		WdfRequestCompleteWithInformation(Request, STATUS_SUCCESS, 4);
	}
	else
	{
		WdfRequestCompleteWithInformation(Request, STATUS_BUFFER_TOO_SMALL, 0);
	}

}

VOID READ_STRING(WDFREQUEST Request, ULONG OutputBufferLength, ULONG InputBufferLength)
{
	NTSTATUS status;
	PVOID Buffer;
	PDRIVER_CONTEXT pDriverContext;
	WDFDEVICE Device;
	WDFDRIVER Driver;
	ULONG slen;
	Device = WdfIoQueueGetDevice(WdfRequestGetIoQueue(Request));
	Driver = WdfDeviceGetDriver(Device);
	pDriverContext = GetDriverContext(Driver);
	slen = wcslen(pDriverContext->sz)*sizeof(WCHAR);
	if (OutputBufferLength >= slen)
	{
		status = WdfRequestRetrieveOutputBuffer(Request, sizeof(ULONG), &Buffer, NULL);
		if (!NT_SUCCESS(status))
		{
			WdfRequestComplete(Request, STATUS_UNSUCCESSFUL);
			return;
		}
		
		memcpy(Buffer, pDriverContext->sz, slen);
		WdfRequestCompleteWithInformation(Request, STATUS_SUCCESS, slen);
	}
	else
	{
		WdfRequestCompleteWithInformation(Request, STATUS_BUFFER_TOO_SMALL, 0);
	}

}

VOID READ_BOOLEAN(WDFREQUEST Request, ULONG OutputBufferLength, ULONG InputBufferLength)
{
	NTSTATUS status;
	PDRIVER_CONTEXT pDriverContext = NULL;
	PREQUEST_CONTEXT pRequestContext = NULL;
	WDFDEVICE Device;
	WDFDRIVER Driver;
	PVOID outBuf;
	size_t outLen;
	Device = WdfIoQueueGetDevice(WdfRequestGetIoQueue(Request));
	Driver = WdfDeviceGetDriver(Device);
	pDriverContext = GetDriverContext(Driver);
	
	
	pRequestContext = GetRequestContext(Request);
	if (OutputBufferLength>=sizeof(ULONG))
	{
		outBuf = WdfMemoryGetBuffer(pRequestContext->OutputMemory, &outLen);
		if (outBuf == NULL)
		{
			WdfRequestComplete(Request, STATUS_INVALID_PARAMETER);
			return;
		}
		*(PULONG)outBuf = pDriverContext->Boolean;
		WdfRequestCompleteWithInformation(Request, STATUS_SUCCESS, sizeof(ULONG));
	}
	else
	{
		WdfRequestComplete(Request, STATUS_INVALID_PARAMETER);
	}

}