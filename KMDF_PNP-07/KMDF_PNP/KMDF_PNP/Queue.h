#ifndef QUEUE_H
#define QUEUE_H
#include <ntddk.h>
#include <wdf.h>
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
#endif