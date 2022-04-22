#include "includes.h"

#pragma comment(lib, "ntdll.lib")


tNtQuerySystemInformation oNtQuerySystemInformation;
tNtQueryObject oNtQueryObject;

tNtLoadDriver oNtLoadDriver;
tNtUnloadDriver oNtUnloadDriver;

LPCSTR driverPath = "\\SystemRoot\\System32\\drivers\\AnticheatDriver.sys";
std::wstring registryPath = L"System\\CurrentControlSet\\Services\\anticheatdriver";

//Create a new registry key for the driver so windows knows how to load the driver

bool Driver::CreateRegistry()
{	
	HKEY hKey;
	if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, registryPath.c_str(), 0, KEY_READ, &hKey) == ERROR_SUCCESS)
	{
		//This would indicate the registry key already exists, so we can just return true
		return true;
	}

	if (RegCreateKeyW(HKEY_LOCAL_MACHINE, registryPath.c_str(), &hKey) != ERROR_SUCCESS)
	{
		//Key was not created
		return false;
	}

	DWORD dwData = 1;
	RegSetValueExA(hKey, "Type", 0, REG_DWORD, (BYTE*)&dwData, 4u);
	RegSetValueExA(hKey, "ErrorControl", 0, REG_DWORD, (BYTE*)&dwData, 4u);

	dwData = 3;
	RegSetValueExA(hKey, "Start", 0, REG_DWORD, (BYTE*)&dwData, 4u);
	RegSetValueExA(hKey, "ImagePath", 0, REG_SZ, (const BYTE*)driverPath, strlen(driverPath) + 1);

	return true;
}

//Start the new service by using NtLoadDriver. There are other methods but im using this since it works well
bool Driver::LoadDriver()
{
	if (!Util::EscalatePrivelages())
	{
		Handler::ExitWithError(CANT_ESCALATE_PRIV);
	}

	if (!PathFileExistsA("C:\\Windows\\System32\\drivers\\AnticheatDriver.sys"))
	{
		Handler::ExitWithError(DRIVER_BAD_PATH);
	}

	if (!Driver::CreateRegistry())
	{
		Handler::ExitWithError(CANT_ESCALATE_PRIV);
	}

	std::cout << "[+] Loading Driver..." << std::endl;

	HMODULE hNtdll = GetModuleHandle(L"ntdll.dll");
	if (hNtdll == INVALID_HANDLE_VALUE || hNtdll == 0)
		return 0;

	//Cast the address of NtLoadDriver to our function pointer so we can call it
	oNtLoadDriver = (tNtLoadDriver)GetProcAddress(hNtdll, "NtLoadDriver");
	oNtUnloadDriver = (tNtUnloadDriver)GetProcAddress(hNtdll, "NtUnloadDriver");
	
	oNtQuerySystemInformation = (tNtQuerySystemInformation)GetProcAddress(hNtdll, "NtQuerySystemInformation");
	oNtQueryObject = (tNtQueryObject)GetProcAddress(hNtdll, "NtQueryObject");

	if (Driver::IsDriverAlreadyLoaded())
	{
		std::cout << "[+] Driver already loaded..." << std::endl;
		return true;
	}

	std::wstring loadRegPath = L"\\Registry\\Machine\\System\\CurrentControlSet\\Services\\anticheatdriver";

	UNICODE_STRING regPath;
	RtlInitUnicodeString(&regPath, loadRegPath.c_str());

	if (!NT_SUCCESS(oNtLoadDriver(&regPath)))
	{
		return false;
	}

	Sleep(1000);
	std::cout << "[+] Driver Loaded!" << std::endl;

	return true;
}

bool Driver::IsDriverAlreadyLoaded()
{
	NTSTATUS Status;
	ULONG size;

	Status = oNtQuerySystemInformation((SYSTEM_INFORMATION_CLASS)11, NULL, 0, &size);
	if (Status != STATUS_INFO_LENGTH_MISMATCH)
	{
		//Error
	}

	PRTL_PROCESS_MODULES Modules = (PRTL_PROCESS_MODULES)VirtualAlloc(0, size, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);

	if (!Modules)
		return false;

	if (NT_SUCCESS(oNtQuerySystemInformation((SYSTEM_INFORMATION_CLASS)11, Modules, size, &size)))
	{
		for (int i = 0; i < Modules->NumberOfModules; i++)
		{
			if (strstr((char*)Modules->Modules[i].FullPathName, "AnticheatDriver.sys"))
			{
				VirtualFree(Modules, 0, MEM_RELEASE);
				return true;
			}
		}
	}

	if (Modules)
		VirtualFree(Modules, 0, MEM_RELEASE);

	return false;
}

bool Driver::Cleanup()
{
	std::wstring loadRegPath = L"\\Registry\\Machine\\System\\CurrentControlSet\\Services\\anticheatdriver";

	UNICODE_STRING regPath;
	RtlInitUnicodeString(&regPath, loadRegPath.c_str());

	if (NT_SUCCESS(oNtUnloadDriver(&regPath)))
		std::cout << "[+] Driver unloaded" << std::endl;;

	HKEY hKey;
	if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, registryPath.c_str(), 0, KEY_READ, &hKey) == ERROR_SUCCESS)
	{
		SHDeleteKeyW(HKEY_LOCAL_MACHINE, registryPath.c_str());
	}
	std::cout << "[+] Registry paths cleaned..." << std::endl;

	return true;
}


//==============================================================================================================================


//Default Constructor for the driver interface takes the service RegistryPath in which to establish communication 
DriverObject::DriverObject()
{
	hDriver = CreateFileA("\\\\.\\acDriverDevice", GENERIC_ALL, FILE_SHARE_READ | FILE_SHARE_WRITE, 0, OPEN_EXISTING, 0, 0);
}

//Check communication with the driver by sending a request and recieving the correct key back
BOOL DriverObject::isConnected()
{
	KERNEL_REQUEST Request;
	ZeroMemory(&Request, sizeof(KERNEL_REQUEST));
	Request.Buffer = 0;

	if (DeviceIoControl(this->hDriver, IO_STARTUPREQUEST, &Request, sizeof(Request), &Request, sizeof(Request), NULL, NULL))
	{
		if (Request.Buffer == 4)
		{
			std::cout << "[+] Driver connection established..." << std::endl;
			return TRUE;
		}
		else return FALSE;
	}
	else return FALSE;
}

//Cleanup the driver before unloading it
void DriverObject::Shutdown()
{
	
}

//Send a message to the driver telling it to use ObRegisterCallbacks on any handle created for that process
BOOL DriverObject::protectProcesses(ULONG ProcessIDS[2])
{
	KERNEL_REQUEST Request;
	ZeroMemory(&Request, sizeof(KERNEL_REQUEST));
	Request.ProcessIDs[0] = ProcessIDS[0];
	Request.ProcessIDs[1] = ProcessIDS[1];

	if (DeviceIoControl(this->hDriver, IO_PROTECTEDPROCESSINFO, &Request, sizeof(Request), &Request, sizeof(Request), NULL, NULL))
	{
		if (Request.Buffer == 1)
		{
			this->isProcessProtected = TRUE;
			std::cout << "[+] Processes protected by ObRegisterCallbacks!" << std::endl;
			return TRUE;
		}
		else return FALSE;
	}
	else return FALSE;

}

//Checks to see if there are any privelaged open handles with the game process and anticheat process that should not be there
BOOL DriverObject::areProcessesProtected()
{
	return this->isProcessProtected;

	if (!oNtQuerySystemInformation)
		return FALSE;

	PSYSTEM_HANDLE_INFORMATION HandleInfo;
	ULONG infoSize = 0x40000;

	HandleInfo = (PSYSTEM_HANDLE_INFORMATION)malloc(infoSize);

	if (NT_SUCCESS(oNtQuerySystemInformation((SYSTEM_INFORMATION_CLASS)0x10, HandleInfo, infoSize, NULL)))
	{
		for (int i = 0; i < HandleInfo->HandleCount; i++)
		{
			SYSTEM_HANDLE_TABLE_ENTRY_INFO currHandle = HandleInfo->Handles[i];

			if (currHandle.ProcessId != Globals.processProcID)
				continue;

			if ((currHandle.GrantedAccess & PROCESS_ALL_ACCESS) == 0 || (currHandle.GrantedAccess & PROCESS_VM_OPERATION) == 0)
				continue;

			POBJECT_TYPE_INFORMATION TypeInfo;
			
		}
	}

	free(HandleInfo);
	return TRUE;
}

//Grabs any report structures that have been filled out by the driver. Will exit if the number of reports reaches 3
BOOL DriverObject::QueryReports()
{
	KERNEL_QUERY Reports;
	ZeroMemory(&Reports, sizeof(KERNEL_QUERY));
	static int violationCount = 0;

	if (DeviceIoControl(this->hDriver, IO_QUERYREPORTS, NULL, NULL, &Reports, sizeof(Reports), NULL, NULL))
	{
		for (int i = 0; i < Reports.NumOfReports; i++)
		{
			if (Reports.Reports[i] == NULL)
				continue;

			violationCount++;
			if (violationCount >= 3)
				Handler::ExitWithError(DRIVER_TOO_MANY_VIOLATIONS);

			switch (Reports.Reports[i]->ReportID)
			{
				case KERNEL_HOOK:
				{
					std::cout << "[*] Anticheat has detected a hook in the kernel, violation number: " << violationCount << std::endl;
				}
			}
		}

		return TRUE;
	}

	return FALSE;
}