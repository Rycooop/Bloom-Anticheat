#include "includes.h"


const char* dllPath = "C:\\Users\\Ryan\\Desktop\\AntiCheat\\x64\\Release\\anticheat_x64.dll";

BOOL InjectDLL(DWORD procID)
{
	HANDLE hProc = OpenProcess(PROCESS_ALL_ACCESS, FALSE, procID);

	if (hProc == INVALID_HANDLE_VALUE)
		return FALSE;

	PVOID memAllocation = VirtualAllocEx(hProc, 0, MAX_PATH, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
	if (!memAllocation)
		return FALSE;

	if (!WriteProcessMemory(hProc, memAllocation, dllPath, strlen(dllPath) + 1, NULL))
		return FALSE;

	HANDLE hThread = CreateRemoteThread(hProc, 0, 0, (LPTHREAD_START_ROUTINE)LoadLibraryA, memAllocation, 0, 0);
	if (hThread == INVALID_HANDLE_VALUE)
		return FALSE;

	CloseHandle(hThread);
	CloseHandle(hProc);

	return TRUE;
}