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
#endif