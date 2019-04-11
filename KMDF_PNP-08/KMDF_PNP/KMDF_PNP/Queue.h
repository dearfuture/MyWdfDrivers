#ifndef QUEUE_H
#define QUEUE_H
#include <ntddk.h>
#include <wdf.h>
#define IOCTL_TEST			CTL_CODE(FILE_DEVICE_UNKNOWN,0X800,METHOD_BUFFERED,FILE_ANY_ACCESS)
#define IOCTL_READ_DWORD	CTL_CODE(FILE_DEVICE_UNKNOWN,0X801,METHOD_BUFFERED,FILE_ANY_ACCESS)
#define IOCTL_READ_BOOLEAN	CTL_CODE(FILE_DEVICE_UNKNOWN,0X802,METHOD_NEITHER,FILE_ANY_ACCESS)
#define IOCTL_READ_STRING	CTL_CODE(FILE_DEVICE_UNKNOWN,0X803,METHOD_OUT_DIRECT,FILE_ANY_ACCESS)

VOID EvtIoDeviceControl(
	_In_  WDFQUEUE Queue,
	_In_  WDFREQUEST Request,
	_In_  size_t OutputBufferLength,
	_In_  size_t InputBufferLength,
	_In_  ULONG IoControlCode
	);


VOID EvtTimerFunc(
	_In_  WDFTIMER Timer
	);

VOID EvtIoCanceledOnQueue(
	_In_ WDFQUEUE   Queue,
	_In_ WDFREQUEST Request
	);


VOID READ_DWORD(WDFREQUEST Request, ULONG OutputBufferLength, ULONG InputBufferLength);
VOID READ_STRING(WDFREQUEST Request, ULONG OutputBufferLength, ULONG InputBufferLength);
VOID READ_BOOLEAN(WDFREQUEST Request, ULONG OutputBufferLength, ULONG InputBufferLength);
#endif