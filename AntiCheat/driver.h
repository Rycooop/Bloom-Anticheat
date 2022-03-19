#include <Windows.h>

#ifndef _DRIVER_INFO
#define _DRIVER_INFO

#include <windef.h>

#define IO_ISCONNECTED CTL_CODE(FILE_DEVICE_UNKNOWN, 0x999, METHOD_BUFFERED, FILE_SPECIAL_ACCESS)
#define IO_STARTUPREQUEST CTL_CODE(FILE_DEVICE_UNKNOWN, 0x1000, METHOD_BUFFERED, FILE_SPECIAL_ACCESS)

#endif


namespace Driver
{
	bool LoadDriver();
}

class DriverObject
{
private:
	HANDLE hDriver;
	BOOL isProcessProtected = FALSE;

public:

	DriverObject(LPCSTR RegistryPath);

	BOOL isConnected();
	void Shutdown();

	VOID protectProcess(UNICODE_STRING processName);
	BOOL areProcessesProtected();
};
