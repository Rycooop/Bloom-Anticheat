#include "includes.h"


//Try and deal with all errors before exiting
BOOL Handler::TroubleshootError(DWORD errorNum)
{
	//Self explanatory
	switch (errorNum)
	{
		case PROCESS_NOT_RUNNING:
		{
			std::wstring processNotRunningStr = L"[-] Please start ";
			processNotRunningStr += PROTECTED_PROCESS;

			std::wcout << processNotRunningStr.c_str() << "\r";
			break;
		}
		case PROCESS_INVALID_PROCESSID:
		{
			if (Util::isProcessRunning(PROTECTED_PROCESS))
			{
				DWORD procID = Util::getRunningProcessId(PROTECTED_PROCESS);

				if (procID)
					return TRUE;
				else return FALSE;
			}
			else return FALSE;
		}
		case PROCESS_INVALID_HANDLE:
		{
			if (OpenProcess(PROCESS_VM_READ, FALSE, Globals.processProcID) == INVALID_HANDLE_VALUE)
				return FALSE;
			else return TRUE;
		}
		case DLL_INVALID_PATH:
		{
			std::cout << "\nDll path invalid, Enter valid path: ";
			std::string newPath;
			std::cin >> newPath;

			if (!PathFileExistsA(newPath.c_str()))
			{
				Handler::ExitWithError(DLL_INVALID_PATH);
			}
			else
			{
				dllPath = newPath;
				return TRUE;
			}
		}
		case DLL_CANT_LOAD:
		{
			break;
		}
	}

	return FALSE;
}

//Provide the reason for failure and exit
void Handler::ExitWithError(DWORD errorNum)
{
	switch (errorNum)
	{
		case PROCESS_NOT_RUNNING:
		{
			std::cout << "\n\n==========================================================================================\n" << std::endl;
			std::cout << "		Was unable to find the target process, shutting down..." << std::endl;
			break;
		}
		case PROCESS_INVALID_PROCESSID:
		{
			std::cout << "\n\n==========================================================================================\n" << std::endl;
			std::cout << "		Was unable to find the target process ID, shutting down..." << std::endl;
			break;
		}
		case DLL_INVALID_PATH:
		{
			std::cout << "\n\n==========================================================================================\n" << std::endl;
			std::cout << "		Anticheat DLL path was invalid, shutting down..." << std::endl;
			break;
		}
	}

	Sleep(4000);
	ExitProcess(0);
}