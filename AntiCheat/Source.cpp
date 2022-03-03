#include "includes.h"
#include "memory.h"

FILE* f;


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR nCmdLine, int nCmdShow)
{
	AllocConsole();
	freopen_s(&f, "conout$", "w", stdout);
	system("Color 0B");
	std::cout << "[+] Anticheat Initializing..." << std::endl;


	while (!Util::isProcessRunning(PROTECTED_PROCESS))
	{
		if (Globals.error >= 5)
		{
			Handler::TroubleshootError(PROCESS_NOT_RUNNING);
		}

		Sleep(2000);
		Globals.error++;
	}

	Globals.processProcID = Util::getRunningProcessId(PROTECTED_PROCESS);

	if (Globals.processProcID == 0)
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

	Memory Reader = Memory(Globals.hProcess);

	while (true)
	{
		Sleep(2000);
	}
}