#ifndef DRIVER_H
#define DRIVER_H
#include <ntddk.h>
#include <wdf.h>
typedef struct _DRIVER_CONTEXT 
{
	ULONG Number;
	WCHAR sz[1024];
	ULONG Boolean;
}DRIVER_CONTEXT, *PDRIVER_CONTEXT;

WDF_DECLARE_CONTEXT_TYPE_WITH_NAME(DRIVER_CONTEXT, GetDriverContext)
NTSTATUS LoadParameter(IN WDFDRIVER Driver);
#endif