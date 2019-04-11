#include <ntddk.h>
#include <wdf.h>



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
	switch (IoControlCode)
	{
	case IOCTL_TEST:
		if (InputBufferLength == 0 || OutputBufferLength < 2)
		{
			KdPrint(("参数无效\n"));
			WdfRequestComplete(Request, STATUS_INVALID_PARAMETER);
			break;
		}
		status = WdfRequestRetrieveInputBuffer(Request, 1, &Buffer, &Length);
		if (!NT_SUCCESS(status))
		{
			WdfRequestComplete(Request, STATUS_INVALID_PARAMETER);
		}

		n = *(PCHAR)Buffer;
		if (n>='0'&&n<='9')
		{
			n -= '0';
			status = WdfRequestRetrieveOutputBuffer(Request, 1, &Buffer, &Length);
			if (!NT_SUCCESS(status))
			{
				WdfRequestComplete(Request, STATUS_INVALID_PARAMETER);
				break;
			}
			strncpy(Buffer, &cc[n * 2], 2);
			WdfRequestCompleteWithInformation(Request, STATUS_SUCCESS, 2);
		}
		else
		{
			WdfRequestComplete(Request, STATUS_INVALID_PARAMETER);
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
	
}
