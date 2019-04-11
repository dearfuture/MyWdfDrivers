#ifndef DEVICE_H
#define DEVICE_H
#include <ntddk.h>
#include <wdf.h>
NTSTATUS EvtDriverDeviceAdd(
	_In_    WDFDRIVER       Driver,
	_Inout_ PWDFDEVICE_INIT DeviceInit
	);
#endif