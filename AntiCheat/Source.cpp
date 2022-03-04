#include "includes.h"
#include "memory.h"


FILE* f;
GLOBALS Globals;
UNICODE_STRING driverPath = { 0 };

tNtLoadDriver oNtLoadDriver;


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR nCmdLine, int nCmdShow)
{
	AllocConsole();
	freopen_s(&f, "conout$", "w", stdout);
	system("Color 0B");
	std::cout << "[+] Anticheat Initializing..." << std::endl;

	HMODULE hNtdll = GetModuleHandle(L"ntdll.dll");
	if (hNtdll == INVALID_HANDLE_VALUE)
		return 0;

	oNtLoadDriver = (tNtLoadDriver)GetProcAddress(hNtdll, "NtLoadDriver");
	
	DriverObject KernelDriver = DriverObject("\\\\.\\achelper_x64");
	if (!KernelDriver.isConnected())
	{
		Handler::TroubleshootError(DRIVER_CONNECTION_ERROR);

		if (!KernelDriver.isConnected())
		{
			Handler::ExitWithError(DRIVER_CONNECTION_ERROR);
		}
	}

	std::cout << "[+] Driver loaded" << std::endl;

	while (!Util::isProcessRunning(PROTECTED_PROCESS))
	{
		Handler::TroubleshootError(PROCESS_NOT_RUNNING);
		Sleep(5000);
	}

	if (!Util::getRunningProcessId(PROTECTED_PROCESS))
	{
		if (!Handler::TroubleshootError(PROCESS_INVALID_PROCESSID))
		{
			Handler::ExitWithError(PROCESS_INVALID_PROCESSID);
			return 0;
		}

		Globals.processProcID = Util::getRunningProcessId(PROTECTED_PROCESS);
	}

	Globals.hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, Globals.processProcID);

	if (Globals.hProcess == INVALID_HANDLE_VALUE)
	{
		if (!Handler::TroubleshootError(PROCESS_INVALID_HANDLE))
		{
			Handler::ExitWithError(PROCESS_INVALID_HANDLE);
			return 0;
		}

		Globals.hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, Globals.processProcID);
	}

	std::cout << "[+] Attached to process" << std::endl;

	UNICODE_STRING protectedProcessU = {};
	//RtlInitUnicodeString(&protectedProcessU, PROTECTED_PROCESS);
	KernelDriver.protectProcess(protectedProcessU);

	std::cout << "[+] ObRegisterCallbacks in place, stripping handles" << std::endl;

	while (true)
	{
		Sleep(3000);
	}
}