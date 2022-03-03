#include "util.h"


BOOL Util::isProcessRunning(LPCWSTR processName)
{
	HANDLE hSnap;
	PROCESSENTRY32 procEntry;

	hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

	procEntry.dwSize = sizeof(PROCESSENTRY32);

	if (Process32First(hSnap, &procEntry))
	{
		if (lstrcmp(procEntry.szExeFile, processName) == 0)
		{
			return true;
		}
	}

	while (Process32Next(hSnap, &procEntry))
	{
		if (lstrcmp(procEntry.szExeFile, processName) == 0)
		{
			return true;
		}
	}

	return false;
}

DWORD Util::getRunningProcessId(LPCWSTR processName)
{
	HANDLE hSnap;
	PROCESSENTRY32 procEntry;

	hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

	procEntry.dwSize = sizeof(PROCESSENTRY32);

	if (Process32First(hSnap, &procEntry))
	{
		if (lstrcmp(procEntry.szExeFile, processName) == 0)
		{
			return procEntry.th32ProcessID;
		}
	}

	while (Process32Next(hSnap, &procEntry))
	{
		if (lstrcmp(procEntry.szExeFile, processName) == 0)
		{
			return procEntry.th32ProcessID;
		}
	}

	return 0;
}
