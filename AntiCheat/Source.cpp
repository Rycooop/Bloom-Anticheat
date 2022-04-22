#include "includes.h"
#include "memory.h"


FILE* f;
GLOBALS Globals;

//Make sure to change all the paths to where your actual driver file and dll are located, otherwise you will recieve errors
//The main purpose of this module is just to communicate with the driver, for all usermode detections take a look at the DLL

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR nCmdLine, int nCmdShow)
{
	//Allocate a console that can take input to debug errors, the file pointer will be stored globally

	AllocConsole();
	freopen_s(&f, "conout$", "w", stdout);
	freopen_s(&f, "conin$", "r", stdin);
	system("Color 0B");
	std::cout << "[+] Anticheat Initializing..." << std::endl;

	std::wstring consoleName = L"Anticheat currently protecting: ";
	consoleName += PROTECTED_PROCESS;
	SetConsoleTitleW(consoleName.c_str());

	//Wait for the process to be running, if it isnt the error handler will print a message saying to start the process

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

	//Store some of our variables globally since we will need them to monitor the protected process
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


	//Create a registry path and load the driver using the undocumented NtLoadDriver(). Test signing MUST be enabled unless your driver is signed
	//If you are not interested in using the driver just inject the DLL manually and do not worry about this module

	if (!Driver::LoadDriver())
	{
		if (!Handler::TroubleshootError(DRIVER_INVALID_LOAD))
		{
			Handler::ExitWithError(DRIVER_INVALID_LOAD);
			return 0;
		}
	}

	//Create a driver interface and establish communication with driver
	DriverObject KernelDriver = DriverObject();
	if (!KernelDriver.isConnected())
	{
		Handler::TroubleshootError(DRIVER_CONNECTION_ERROR);

		if (!KernelDriver.isConnected())
		{
			Handler::ExitWithError(DRIVER_CONNECTION_ERROR);
		}
	}

	ULONG ProcessIDs[2] = { GetCurrentProcessId(), Globals.processProcID };
	if (!KernelDriver.protectProcesses(ProcessIDs))
	{
		Handler::TroubleshootError(DRIVER_CANT_PROTECT);

		if (!KernelDriver.protectProcesses(ProcessIDs))
		{
			Handler::ExitWithError(DRIVER_CANT_PROTECT);
		}
	}
	

	//Inject dll to target process via a simple loadlibraryA injector. This is where all the usermode detection vectors are. This is critical in ensuring
	//usermode detection of cheats. We will pass through the already created handle since ObRegisterCallbacks will be stripping at this point

	if (!InjectDLL(Globals.hProcess))
	{
		if (!Handler::TroubleshootError(DLL_CANT_LOAD))
		{
			Handler::ExitWithError(DLL_CANT_LOAD);
			return 0;
		}
	}

	std::cout << "\n ===================================================" << std::endl;
	std::cout << " ================ Anticheat Running ================" << std::endl;
	std::cout << " ===================================================" << std::endl;

	while (Util::isProcessRunning(PROTECTED_PROCESS))
	{

		Sleep(6000);
	}
	system("CLS");
	std::cout << "[+] Cleaning up..." << std::endl;

	KernelDriver.Shutdown();
	Driver::Cleanup();

	std::cout << "[+] Closing..." << std::endl;
	Sleep(3000);
	return 0;
}