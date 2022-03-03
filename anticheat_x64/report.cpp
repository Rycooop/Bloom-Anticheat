#include "includes.h"


void Report::SendReport(DWORD reportCode)
{
	AllocConsole();
	FILE* f;
	freopen_s(&f, "conout$", "w", stdout);
	system("Color 0C");

	std::cout << "Anticheat has encountered the following issue:\n " << std::endl;

	switch (reportCode)
	{
		case INVALID_ANTICHEAT_START:
		{
			std::cout << "[*] Anticheat DLL failed to initialize correctly, closing..." << std::endl;
			break;
		}
		case MANUALMAPPED_MODULE:
		{
			std::cout << "[*] Manually maped module detected, closing..." << std::endl;
			break;
		}
		case HOOK_DETECTED:
		{
			std::cout << "[*] Hook has been detected, closing..." << std::endl;
			break;
		}
		case DEBUGGER_DETECTED:
		{
			std::cout << "[*] Debugger was detected, closing..." << std::endl;
			break;
		}
		case BLACKLISTED_DLL_DETECTED:
		{
			std::cout << "[*] Blacklisted DLL attempted to load, closing..." << std::endl;
			break;
		}
		case ODD_BAHAVIOR:
		{
			std::cout << "[*] Odd process bahavior, possible Manually Mapped module..." << std::endl;
			break;
		}
	}

	Sleep(4000);
	ExitProcess(0);
}