#include <Windows.h>


//This is a good place to add other functions to gather HWID info and upload information to your server

namespace Report
{
	void SendReport(DWORD reportCode);
}

//Our report structure to fill out so we can return info

typedef struct newReport
{
	DWORD reason;
	HANDLE hApplication, hThread;
	LPCSTR dllName;
	HWND hWindow;
	LPCSTR windowName;
};

//Anything you want to report for should be thrown into the enum and handled inside the Report::SendReport handler

enum reports
{
	INVALID_ANTICHEAT_START = 1,
	INTEGRITY_CHECK_VIOLATION,
	CANNOT_QUEUE_APCS,
	MANUALMAPPED_MODULE,
	HOOK_DETECTED,
	IAT_HOOK_DETECTED,
	DEBUGGER_DETECTED,
	KERNEL_DEBUGGING_ENABLED,
	BLACKLISTED_DLL_DETECTED,
	ODD_BAHAVIOR,
	OVERLAY_DETECTED,
	RIP_OUTSIDE_VALID_MODULE
};