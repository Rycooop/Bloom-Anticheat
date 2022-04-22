#include "util.h"


BOOL Util::RespawnAsChild(LPCWSTR ProcessName)
{
	DWORD ProcID = Util::getRunningProcessId(ProcessName);
	if (!ProcID)
		return FALSE;

	HANDLE hFirstInstance = OpenProcess(PROCESS_TERMINATE, FALSE, ProcID);

	if (!TerminateProcess(hFirstInstance, 0))
		return FALSE;

	STARTUPINFOA StartupInfo;
	PROCESS_INFORMATION ProcessInfo;
	ZeroMemory(&StartupInfo, sizeof(STARTUPINFOA));
	ZeroMemory(&ProcessInfo, sizeof(PROCESS_INFORMATION));

	if (CreateProcessA("C:\\Windows\\notepad.exe", NULL, NULL, NULL, FALSE, 0, NULL, NULL, &StartupInfo, &ProcessInfo))
		return TRUE;
	else return FALSE;
}

BOOL Util::EscalatePrivelages()
{
	HANDLE tokenHandle;
	TOKEN_PRIVILEGES tp;
	LUID luid;

	if (!LookupPrivilegeValue(NULL, SE_LOAD_DRIVER_NAME, &luid))
		return FALSE;

	if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES, &tokenHandle))
		return FALSE;

	tp.PrivilegeCount = 1;
	tp.Privileges[0].Luid = luid;
	tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

	if (!AdjustTokenPrivileges(tokenHandle, FALSE, &tp, sizeof(TOKEN_PRIVILEGES), NULL, NULL))
		return FALSE;

	CloseHandle(tokenHandle);
	return TRUE;
}


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
			return true;
	}

	while (Process32Next(hSnap, &procEntry))
	{
		if (lstrcmp(procEntry.szExeFile, processName) == 0)
			return true;
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
