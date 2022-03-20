#include <Windows.h>


//This is a good place to add other functions to gather HWID info and upload information to your server

namespace Report
{
	void SendReport(DWORD reportCode);
}


//Anythign you want to report for should be thrown into the enum and handled inside the Report::SendReport handler

enum reports
{
	INVALID_ANTICHEAT_START = 1,
	CANNOT_QUEUE_APCS,
	MANUALMAPPED_MODULE,
	HOOK_DETECTED,
	DEBUGGER_DETECTED,
	BLACKLISTED_DLL_DETECTED,
	ODD_BAHAVIOR,
	OVERLAY_DETECTED,
	RIP_OUTSIDE_VALID_MODULE
};