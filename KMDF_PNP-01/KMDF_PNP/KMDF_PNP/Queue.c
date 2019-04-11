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

	KdPrint(("”–io«Î«Û\n"));
	WdfRequestComplete(Request, STATUS_SUCCESS);
	
}
