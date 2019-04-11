#include <windows.h>
#include <stdio.h>
#include <winioctl.h>

#define IOCTL_TEST CTL_CODE(FILE_DEVICE_UNKNOWN,0X800,METHOD_BUFFERED,FILE_ANY_ACCESS)

int main()
{
	HANDLE hDevice = CreateFile(TEXT("\\\\.\\HelloWDF"),
		GENERIC_READ | GENERIC_WRITE | GENERIC_EXECUTE,
		FILE_SHARE_READ | FILE_SHARE_WRITE,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL);
	if (hDevice == INVALID_HANDLE_VALUE)
	{
		printf("文件打开失败%x\n", GetLastError());
		return -1;
	}
	DWORD dwret;
	DeviceIoControl(hDevice, IOCTL_TEST, NULL, 0, NULL, 0, &dwret, NULL);


	CloseHandle(hDevice);




	return 0;
}