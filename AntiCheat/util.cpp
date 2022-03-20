#include "util.h"


//Check if process is running
BOOL Util::isProcessRunning(LPCWSTR processName)
{
	//Use ToolHelp32 to enumerate all running processes on the system
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

//Obtain a process ID for a given process
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
