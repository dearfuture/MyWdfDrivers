#ifndef DEVICE_H
#define DEVICE_H
#include <ntddk.h>
#include <wdf.h>

typedef struct _DEVICE_CONTEXT 
{
	WDFTIMER Timer;
	WDFQUEUE Queue;
}DEVICE_CONTEXT, *PDEVICE_CONTEXT;

WDF_DECLARE_CONTEXT_TYPE_WITH_NAME(DEVICE_CONTEXT,GetDeviceContext)

NTSTATUS EvtDriverDeviceAdd(
	_In_    WDFDRIVER       Driver,
	_Inout_ PWDFDEVICE_INIT DeviceInit
	);
NTSTATUS MyTimerCreate(IN WDFTIMER* TImer, IN WDFDEVICE Device);
#endif