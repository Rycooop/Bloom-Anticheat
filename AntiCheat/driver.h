#include <Windows.h>

#ifndef _DRIVER_INFO
#define _DRIVER_INFO

#include <windef.h>

//Define the IO Control codes
#define IO_ISCONNECTED CTL_CODE(FILE_DEVICE_UNKNOWN, 0x999, METHOD_BUFFERED, FILE_SPECIAL_ACCESS)
#define IO_STARTUPREQUEST CTL_CODE(FILE_DEVICE_UNKNOWN, 0x1000, METHOD_BUFFERED, FILE_SPECIAL_ACCESS)

#endif

//Create a function pointer for the functions needed to work with a driver
typedef NTSTATUS(*tNtLoadDriver)(PUNICODE_STRING DriverServiceName);

extern const std::wstring driverPath;
extern const std::wstring registryPath;

//Loading and unloading the driver
namespace Driver
{
	bool CreateRegistry();
	bool LoadDriver();
}


//Create an interface to communicate with the driver. This will handle all driver requests for us and will be called through the object
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
