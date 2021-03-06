#include <windows.h>
#include <stdio.h>
#include <initguid.h>
#include <SetupAPI.h>

#pragma comment(lib,"setupapi.lib")
#define IOCTL_TEST	CTL_CODE(FILE_DEVICE_UNKNOWN,0X800,METHOD_BUFFERED,FILE_ANY_ACCESS)

DEFINE_GUID(DEVICEINTERFACE,
	0xa3c9b20a, 0x9f17, 0x40f3, 0xa6, 0x7b, 0xbc, 0x37, 0xba, 0x95, 0x69, 0xa6);
LPTSTR GetDevicePath()
{

	HDEVINFO hInfo = SetupDiGetClassDevs(&DEVICEINTERFACE, NULL, NULL, DIGCF_DEVICEINTERFACE | DIGCF_PRESENT);
	if (hInfo == NULL)
	{
		return NULL;
	}
	SP_DEVICE_INTERFACE_DATA ifdata = { 0 };
	ifdata.cbSize = sizeof(ifdata);
	if (!SetupDiEnumDeviceInterfaces(hInfo, NULL, &DEVICEINTERFACE, 0, &ifdata))
	{
		printf("枚举接口失败%d\n", GetLastError());
		return NULL;
	}
	
	PSP_DEVICE_INTERFACE_DETAIL_DATA pdetailData = (PSP_DEVICE_INTERFACE_DETAIL_DATA)malloc(1024);
	DWORD dwSize = 0;
	RtlZeroMemory(pdetailData, 1024);
	pdetailData->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA);
	if (!SetupDiGetDeviceInterfaceDetail(hInfo, &ifdata, pdetailData, 1024, &dwSize, NULL))
	{
		return NULL;
	}
	else
	{
		return pdetailData->DevicePath;
	}
	

	
}

int main()
{
	LPTSTR DevicePath = NULL;
	if ((DevicePath = GetDevicePath()) != NULL)
	{
		printf("%ls\n", DevicePath);
		HANDLE hDevice = CreateFile(DevicePath, GENERIC_READ | GENERIC_READ | GENERIC_EXECUTE, FILE_SHARE_WRITE | FILE_SHARE_READ,
			NULL,
			OPEN_EXISTING,
			FILE_ATTRIBUTE_NORMAL,
			NULL);
		if (hDevice == INVALID_HANDLE_VALUE)
		{
			printf("打开失败%d\n", GetLastError());
			return -1;
		}
		DWORD dwret;
		printf("请输入一个数字\n");
		char n = getchar();
		char buffer[4] = { 0 };

		DeviceIoControl(hDevice, IOCTL_TEST, &n, sizeof(n), buffer, sizeof(buffer), &dwret, NULL);
		printf("%s\n", buffer);
		CloseHandle(hDevice);

	}
	else
	{
		printf("设备接口获取失败%d\n", GetLastError());
	}

	return 0;
}