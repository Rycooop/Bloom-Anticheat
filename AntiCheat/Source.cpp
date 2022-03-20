#include "includes.h"
#include "memory.h"

FILE* f;
GLOBALS Globals;
UNICODE_STRING driverPath = { 0 };


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR nCmdLine, int nCmdShow)
{
	//Allocate a console that can take input in case of errors
	AllocConsole();
	freopen_s(&f, "conout$", "w", stdout);
	freopen_s(&f, "conin$", "r", stdin);
	system("Color 0B");
	std::cout << "[+] Anticheat Initializing..." << std::endl;

	std::wstring consoleName = L"Anticheat currently protecting: ";
	consoleName += PROTECTED_PROCESS;
	SetConsoleTitleW(consoleName.c_str());

	//Wait for the process to be running
	while (!Util::isProcessRunning(PROTECTED_PROCESS))
	{
		Handler::TroubleshootError(PROCESS_NOT_RUNNING);
		Sleep(500);
	}

	std::cout << "\n[+] Process Found" << std::endl;

	//Obtain the protected ProcessId
	if (!Util::getRunningProcessId(PROTECTED_PROCESS))
	{
		if (!Handler::TroubleshootError(PROCESS_INVALID_PROCESSID))
		{
			Handler::ExitWithError(PROCESS_INVALID_PROCESSID);
			return 0;
		}
	}

	//Set our globals
	Globals.processProcID = Util::getRunningProcessId(PROTECTED_PROCESS);
	Globals.hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, Globals.processProcID);

	//Make sure valid handle has been established
	if (Globals.hProcess == INVALID_HANDLE_VALUE)
	{
		if (!Handler::TroubleshootError(PROCESS_INVALID_HANDLE))
		{
			Handler::ExitWithError(PROCESS_INVALID_HANDLE);
			return 0;
		}
	}

	std::wstring updateString = L"[+] Attached to ";
	updateString += PROTECTED_PROCESS;
	updateString += L": Process ID: ";
	std::wcout << updateString.c_str() << Globals.processProcID << std::endl;

	//Inject dll to target process
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
		Sleep(2000);

	//Load the driver
	if (!Driver::LoadDriver())
	{
		if (!Handler::TroubleshootError(DRIVER_INVALID_LOAD))
		{
			Handler::ExitWithError(DRIVER_INVALID_LOAD);
			return 0;
		}
	}

	std::cout << "[+] Preparing Driver..." << std::endl;


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
	else
	{
		std::cout << "[+] Driver connection established..." << std::endl;
	}

	while (true)
	{
		Sleep(3000);
	}
}