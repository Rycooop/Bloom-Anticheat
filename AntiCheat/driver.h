#include <Windows.h>

#ifndef _DRIVER_INFO
#define _DRIVER_INFO

#include <windef.h>

//Define the IO Control codes
#define IO_STARTUPREQUEST CTL_CODE(FILE_DEVICE_UNKNOWN, 0x1003, METHOD_BUFFERED, FILE_SPECIAL_ACCESS)
#define IO_PROTECTEDPROCESSINFO CTL_CODE(FILE_DEVICE_UNKNOWN, 0x1004, METHOD_BUFFERED, FILE_SPECIAL_ACCESS)

#endif

//Create a function pointer for the functions needed to work with a driver
typedef NTSTATUS(*tNtLoadDriver)(PUNICODE_STRING DriverServiceName);
typedef NTSTATUS(*tNtUnloadDriver)(PUNICODE_STRING DriverServiceName);

extern LPCSTR driverPath;
extern std::wstring registryPath;

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

	DriverObject();

	BOOL isConnected();
	void Shutdown();

	BOOL protectProcesses(ULONG ProcessIDS[2]);
	BOOL areProcessesProtected();
};


//Communication Structure with Kernel
typedef struct _KERNEL_REQUEST
{
	ULONG ProcessIDs[2];
	ULONG Buffer;
}KERNEL_REQUEST, * PKERNEL_REQUEST;