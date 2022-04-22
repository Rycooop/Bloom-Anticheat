#include <Windows.h>
#include <mbstring.h>

#ifndef _DRIVER_INFO
#define _DRIVER_INFO

#include <windef.h>

//Define the IO Control codes
#define IO_STARTUPREQUEST CTL_CODE(FILE_DEVICE_UNKNOWN, 0x1003, METHOD_BUFFERED, FILE_SPECIAL_ACCESS)
#define IO_PROTECTEDPROCESSINFO CTL_CODE(FILE_DEVICE_UNKNOWN, 0x1004, METHOD_BUFFERED, FILE_SPECIAL_ACCESS)
#define IO_QUERYREPORTS CTL_CODE(FILE_DEVICE_UNKNOWN, 0x1005, METHOD_BUFFERED, FILE_SPECIAL_ACCESS)
#define IO_CLEANUP CTL_CODE(FILE_DEVICE_UNKNOWN, 0x1006, METHOD_BUFFERED, FILE_SPECIAL_ACCESS)

#define STATUS_INFO_LENGTH_MISMATCH 0x4

#endif

typedef NTSTATUS(*tNtQuerySystemInformation)(SYSTEM_INFORMATION_CLASS SystemInformationClass, PVOID SystemInformation, ULONG SystemInformationLength, PULONG ReturnLength);
typedef NTSTATUS(*tNtQueryObject)(HANDLE Handle, OBJECT_INFORMATION_CLASS ObjectInformationClass, PVOID ObjectInformation, ULONG ObjectInformationLength, PULONG ReturnLength);

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
	bool IsDriverAlreadyLoaded();
	bool Cleanup();
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
	BOOL QueryReports();
};

enum KernelReports
{
	BLACKLISTED_MODULE_LOAD = 1,
	VULNERABLE_DRIVER_LOAD,
	KERNEL_HOOK,
	INVALID_RIP,
};

//Communication Structure with Kernel
typedef struct _KERNEL_REQUEST
{
	ULONG ProcessIDs[2];
	ULONG Buffer;
}KERNEL_REQUEST, * PKERNEL_REQUEST;

typedef struct _KERNEL_REPORT
{
	DWORD ReportID;
}KERNEL_REPORT, *PKERNEL_REPORT;

typedef struct _KERNEL_QUERY
{
	ULONG NumOfReports;
	PKERNEL_REPORT Reports[ANYSIZE_ARRAY];
}KERNEL_QUERY, * PKERNEL_QUERY;

typedef struct _SYSTEM_HANDLE_TABLE_ENTRY_INFO
{
	ULONG ProcessId;
	BYTE ObjectTypeNumber;
	BYTE Flags;
	USHORT Handle;
	PVOID Object;
	ACCESS_MASK GrantedAccess;
}SYSTEM_HANDLE_TABLE_ENTRY_INFO, *PSYSTEM_HANDLE_TABLE_ENTRY_INFO;

typedef struct _SYSTEM_HANDLE_INFORMATION {
	ULONG HandleCount;
	SYSTEM_HANDLE_TABLE_ENTRY_INFO Handles[1];
} SYSTEM_HANDLE_INFORMATION, * PSYSTEM_HANDLE_INFORMATION;

typedef struct _OBJECT_TYPE_INFORMATION {
	UNICODE_STRING TypeName;
	ULONG TotalNumberOfObjects;
	ULONG TotalNumberOfHandles;
} OBJECT_TYPE_INFORMATION, * POBJECT_TYPE_INFORMATION;

typedef struct _OBJECT_ALL_TYPES_INFORMATION {
	ULONG NumberOfObjectTypes;
	OBJECT_TYPE_INFORMATION ObjectTypeInformation[1];
} OBJECT_ALL_TYPES_INFORMATION, * POBJECT_ALL_TYPES_INFORMATION;

typedef struct _RTL_PROCESS_MODULE_INFORMATION
{
	HANDLE Section;
	PVOID MappedBase;
	PVOID ImageBase;
	ULONG ImageSize;
	ULONG Flags;
	USHORT LoadOrderIndex;
	USHORT InitOrderIndex;
	USHORT LoadCount;
	USHORT OffsetToFileName;
	UCHAR FullPathName[256];
} RTL_PROCESS_MODULE_INFORMATION, * PRTL_PROCESS_MODULE_INFORMATION;

typedef struct _RTL_PROCESS_MODULES
{
	ULONG NumberOfModules;
	RTL_PROCESS_MODULE_INFORMATION Modules[1];
} RTL_PROCESS_MODULES, * PRTL_PROCESS_MODULES;