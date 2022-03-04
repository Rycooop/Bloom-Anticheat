#include <Windows.h>


namespace Report
{
	void SendReport(DWORD reportCode);
}

enum reports
{
	INVALID_ANTICHEAT_START = 1,
	MANUALMAPPED_MODULE,
	HOOK_DETECTED,
	DEBUGGER_DETECTED,
	BLACKLISTED_DLL_DETECTED,
	ODD_BAHAVIOR,
	OVERLAY_DETECTED
};