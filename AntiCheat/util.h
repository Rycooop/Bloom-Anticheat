#include <Windows.h>
#include <TlHelp32.h>


//Utility functions that will be used to initialize
namespace Util
{
	BOOL RespawnAsChild(LPCWSTR ProcessName);
	BOOL EscalatePrivelages();
	BOOL isProcessRunning(LPCWSTR processName);
	DWORD getRunningProcessId(LPCWSTR processName);
}