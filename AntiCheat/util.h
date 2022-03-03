#include <Windows.h>
#include <TlHelp32.h>


namespace Util
{
	BOOL isProcessRunning(LPCWSTR processName);
	DWORD getRunningProcessId(LPCWSTR processName);
}