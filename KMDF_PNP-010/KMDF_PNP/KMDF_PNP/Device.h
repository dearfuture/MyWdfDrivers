#ifndef DEVICE_H
#define DEVICE_H
#include <ntddk.h>
#include <wdf.h>

typedef struct _DEVICE_CONTEXT 
{
	WDFTIMER Timer;
	WDFQUEUE Queue;
}DEVICE_CONTEXT, *PDEVICE_CONTEXT;


typedef struct _REQUEST_CONTEXT
{
	WDFMEMORY OutputMemory;
}REQUEST_CONTEXT, *PREQUEST_CONTEXT;

WDF_DECLARE_CONTEXT_TYPE_WITH_NAME(REQUEST_CONTEXT, GetRequestContext)

WDF_DECLARE_CONTEXT_TYPE_WITH_NAME(DEVICE_CONTEXT,GetDeviceContext)

NTSTATUS EvtDriverDeviceAdd(
	_In_    WDFDRIVER       Driver,
	_Inout_ PWDFDEVICE_INIT DeviceInit
	);
NTSTATUS MyTimerCreate(IN WDFTIMER* TImer, IN WDFDEVICE Device);
VOID EvtIoInCallerContext(
	_In_ WDFDEVICE  Device,
	_In_ WDFREQUEST Request
	);


NTSTATUS EvtDevicePrepareHardware(
	_In_ WDFDEVICE    Device,
	_In_ WDFCMRESLIST ResourcesRaw,
	_In_ WDFCMRESLIST ResourcesTranslated
	);


NTSTATUS EvtDeviceReleaseHardware(
	_In_  WDFDEVICE Device,
	_In_  WDFCMRESLIST ResourcesTranslated
	);


NTSTATUS EvtDeviceD0Entry(
	_In_ WDFDEVICE              Device,
	_In_ WDF_POWER_DEVICE_STATE PreviousState
	);

NTSTATUS EvtDeviceD0EntryPostInterruptsEnabled(
	_In_ WDFDEVICE              Device,
	_In_ WDF_POWER_DEVICE_STATE PreviousState
	);

NTSTATUS EvtDeviceD0Exit(
	_In_  WDFDEVICE Device,
	_In_  WDF_POWER_DEVICE_STATE TargetState
	);

NTSTATUS EvtDeviceD0ExitPreInterruptsDisabled(
	_In_ WDFDEVICE              Device,
	_In_ WDF_POWER_DEVICE_STATE TargetState
	);


#endif