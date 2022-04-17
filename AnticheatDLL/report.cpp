#include "includes.h"


#define FLAGS_TO_BAN 7

//Anything you wish to do once an anomaly is found, such as uploading the file to the server, should be done here. 
//Ideally, the game/process would not close and you would instead flag/ban the user. In one of the switch cases I will
//create a struct containing info on the current user such as HWID info, and an example of uploading that to the server

//As stated before, some of the detection vectors used in this anticheat may throw false positives in specific cases, but
//of them have been tested and I feel confortable including them in this project


uint8_t flags = 0;

void Report::SendReport(DWORD reportCode)
{
	AllocConsole();
	freopen_s(&f, "conout$", "w", stdout);
	system("Color 0C");
	SetConsoleTitleA("Attention!");

	std::cout << "Anticheat has encountered the following issue(s):\n " << std::endl;

	switch (reportCode)
	{
		case INVALID_ANTICHEAT_START:
		{
			std::cout << "[*] Anticheat DLL failed to initialize correctly, closing..." << std::endl;
			break;
		}
		case INTEGRITY_CHECK_VIOLATION:
		{
			std::cout << "[*] One or more integrity checks have failed, closing..." << std::endl;
			break;
		}
		case CANNOT_QUEUE_APCS:
		{
			std::cout << "[*] Anticheat was unable to queue APC's to process thread, closing..." << std::endl;
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
		case IAT_HOOK_DETECTED:
		{
			std::cout << "[*] An IAT Hook has been detected in AnticheatDLL.dll, shutting down..." << std::endl;
			break;
		}
		case DEBUGGER_DETECTED:
		{
			std::cout << "[*] Debugger was detected, closing..." << std::endl;
			break;
		}
		case KERNEL_DEBUGGING_ENABLED:
		{
			std::cout << "[*] Please disable kernel debugging" << std::endl;
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
		case OVERLAY_DETECTED:
		{
			std::cout << "[*] Suspicious overlay found above game process, closing..." << std::endl;
			break;
		}
		case RIP_OUTSIDE_VALID_MODULE:
		{
			std::cout << "[*] Instruction Pointer has been detected outside a valid module" << std::endl;
			break;
		}
	}

	Sleep(5000);
	ExitProcess(0);
}