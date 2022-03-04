#include <Windows.h>

#define IO_ISCONNECTED CTL_CODE(FILE_DEVICE_UNKNOWN, 0x999, METHOD_BUFFERED, FILE_SPECIAL_ACCESS)
#define IO_STARTUPREQUEST CTL_CODE(FILE_DEVICE_UNKNOWN, 0x1000, METHOD_BUFFERED, FILE_SPECIAL_ACCESS)


class DriverObject
{
private:
	HANDLE hDriver;
	BOOL isProcessProtected = FALSE;

public:

	DriverObject(LPCSTR RegistryPath)
	{
		hDriver = CreateFileA(RegistryPath, GENERIC_ALL, FILE_SHARE_READ | FILE_SHARE_WRITE, 0, OPEN_EXISTING, 0, 0);
	}

	void Shutdown()
	{

	}

	BOOL isConnected()
	{
		ULONG buff = 0;

		if (DeviceIoControl(this->hDriver, IO_ISCONNECTED, &buff, sizeof(buff), 0, 0, NULL, NULL))
		{
			if (buff == 4)
				return TRUE;
			else return FALSE;
		}
		else return FALSE;
	}

	VOID protectProcess(UNICODE_STRING processName)
	{

	}

	BOOL areProcessesProtected()
	{
		return this->isProcessProtected;
	}
};