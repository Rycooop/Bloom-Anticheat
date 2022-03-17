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

	std::wstring consoleName = L"Anticheat currently protecting: ";
	consoleName += PROTECTED_PROCESS;
	SetConsoleTitleW(consoleName.c_str());

	while (!Util::isProcessRunning(PROTECTED_PROCESS))
	{
		Handler::TroubleshootError(PROCESS_NOT_RUNNING);
		Sleep(5000);
	}

	std::cout << "\n[+] Process Found" << std::endl;

	if (!Util::getRunningProcessId(PROTECTED_PROCESS))
	{
		if (!Handler::TroubleshootError(PROCESS_INVALID_PROCESSID))
		{
			Handler::ExitWithError(PROCESS_INVALID_PROCESSID);
			return 0;
		
		}
	}

	Globals.processProcID = Util::getRunningProcessId(PROTECTED_PROCESS);
	Globals.hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, Globals.processProcID);

	if (Globals.hProcess == INVALID_HANDLE_VALUE)
	{
		if (!Handler::TroubleshootError(PROCESS_INVALID_HANDLE))
		{
			Handler::ExitWithError(PROCESS_INVALID_HANDLE);
			return 0;
		}
	}

	std::cout << "[+] Attached to process: Process ID: " << Globals.processProcID << std::endl;

	if (!InjectDLL(Globals.processProcID))
	{
		if (!Handler::TroubleshootError(DLL_CANT_LOAD))
		{
			Handler::ExitWithError(DLL_CANT_LOAD);
			return 0;
		}
	}

	std::cout << "[+] Anticheat DLL injected" << std::endl;
	while (true)
	{
		Sleep(3000);
	}

	//Load the driver and establish communication
	std::cout << "[+] Preparing Driver..." << std::endl;

	HMODULE hNtdll = GetModuleHandle(L"ntdll.dll");
	if (hNtdll == INVALID_HANDLE_VALUE)
		return 0;

	oNtLoadDriver = (tNtLoadDriver)GetProcAddress(hNtdll, "NtLoadDriver");

	std::cout << "[+] Driver loaded" << std::endl;


	//Create a driver interface and establish communication with driver
	DriverObject KernelDriver = DriverObject("\\\\.\\achelper_x64");
	if (!KernelDriver.isConnected())
	{
		Handler::TroubleshootError(DRIVER_CONNECTION_ERROR);

		if (!KernelDriver.isConnected())
		{
			Handler::ExitWithError(DRIVER_CONNECTION_ERROR);
		}
	}

	std::cout << "[+] Driver connection established..." << std::endl;

	while (true)
	{
		Sleep(3000);
	}
}