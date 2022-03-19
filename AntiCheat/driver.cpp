#include "includes.h"


tNtLoadDriver oNtLoadDriver;

bool Driver::LoadDriver()
{
	std::cout << "[+] Loading Driver..." << std::endl;

	HMODULE hNtdll = GetModuleHandle(L"ntdll.dll");
	if (hNtdll == INVALID_HANDLE_VALUE)
		return 0;

	oNtLoadDriver = (tNtLoadDriver)GetProcAddress(hNtdll, "NtLoadDriver");
}

DriverObject::DriverObject(LPCSTR RegistryPath)
{
	hDriver = CreateFileA(RegistryPath, GENERIC_ALL, FILE_SHARE_READ | FILE_SHARE_WRITE, 0, OPEN_EXISTING, 0, 0);
}

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

void DriverObject::Shutdown()
{

}

VOID DriverObject::protectProcess(UNICODE_STRING processName)
{

}

BOOL DriverObject::areProcessesProtected()
{
	return this->isProcessProtected;
}