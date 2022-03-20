#include "includes.h"


tNtLoadDriver oNtLoadDriver;


//Start the new service by using NtLoadDriver. There are other methods but im using this since it works well
bool Driver::LoadDriver()
{
	std::cout << "[+] Loading Driver..." << std::endl;

	HMODULE hNtdll = GetModuleHandle(L"ntdll.dll");
	if (hNtdll == INVALID_HANDLE_VALUE || hNtdll == 0)
		return 0;

	//Cast the address of NtLoadDriver to our function pointer so we can call it
	oNtLoadDriver = (tNtLoadDriver)GetProcAddress(hNtdll, "NtLoadDriver");
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