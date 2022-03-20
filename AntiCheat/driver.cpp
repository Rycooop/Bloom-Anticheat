#include "includes.h"


tNtLoadDriver oNtLoadDriver;

const std::wstring driverPath = L"asdas";
const std::wstring registryPath = L"Windows";

//Create a new registry key for the driver so windows knows how to load the driver

bool Driver::CreateRegistry()
{
	//First we must make sure we have privelages to write to the registry, if not, 
	HANDLE tokenHandle;
	TOKEN_PRIVILEGES tp;
	LUID luid;
	HKEY hKey;

	if (!LookupPrivilegeValue(NULL, SE_LOAD_DRIVER_NAME, &luid))
		return false;

	if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES, &tokenHandle))
		return false;

	tp.PrivilegeCount = 1;
	tp.Privileges[0].Luid = luid;
	tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

	if (!AdjustTokenPrivileges(tokenHandle, FALSE, &tp, sizeof(TOKEN_PRIVILEGES), NULL, NULL))
		return false;

	CloseHandle(tokenHandle);

	if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, registryPath.c_str(), 0, KEY_READ, &hKey) == ERROR_SUCCESS)
	{
		//This would indicate the registry key already exists, so we can just return true
		return true;
	}

	if (RegCreateKeyW(hKey, registryPath.c_str(), &hKey) != ERROR_SUCCESS)
	{
		//Key was not created
		return false;
	}

	return true;
}

//Start the new service by using NtLoadDriver. There are other methods but im using this since it works well
bool Driver::LoadDriver()
{
	if (!Driver::CreateRegistry())
		return false;

	std::cout << "[+] Loading Driver..." << std::endl;

	HMODULE hNtdll = GetModuleHandle(L"ntdll.dll");
	if (hNtdll == INVALID_HANDLE_VALUE || hNtdll == 0)
		return 0;

	//Cast the address of NtLoadDriver to our function pointer so we can call it
	oNtLoadDriver = (tNtLoadDriver)GetProcAddress(hNtdll, "NtLoadDriver");

	return true;
}

//Default Constructor for the driver interface takes the service RegistryPath in which to establish communication 
DriverObject::DriverObject(LPCSTR RegistryPath)
{
	hDriver = CreateFileA(RegistryPath, GENERIC_ALL, FILE_SHARE_READ | FILE_SHARE_WRITE, 0, OPEN_EXISTING, 0, 0);
}

//Check communication with the driver by sending a request and recieving the correct key back
BOOL DriverObject::isConnected()
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

//Cleanup the driver before unloading it
void DriverObject::Shutdown()
{

}

//Send a message to the driver telling it to use ObRegisterCallbacks on any handle created for that process
VOID DriverObject::protectProcess(UNICODE_STRING processName)
{

}

//Checks with the driver to make sure ObRegisterCallbacks is in use
BOOL DriverObject::areProcessesProtected()
{
	return this->isProcessProtected;
}