#include "includes.h"

#define FLG_HEAP_ENABLE_TAIL_CHECK   0x10
#define FLG_HEAP_ENABLE_FREE_CHECK   0x20
#define FLG_HEAP_VALIDATE_PARAMETERS 0x40
#define NT_GLOBAL_FLAG_CHECK (FLG_HEAP_ENABLE_TAIL_CHECK | FLG_HEAP_ENABLE_FREE_CHECK | FLG_HEAP_VALIDATE_PARAMETERS)


//Compilation of many different methods to detect/prevent debuggers. Some of these can throw false positives, but most
//of them will work only on the case of a debugger. As already stated, feel free to alter these and disable/add new 
//protections


PPEB pPeb = (PPEB)__readgsqword(0x60);

void Debugger::DebuggerThread()
{
	while (true)
	{
		if (Debugger::ReadPEBForDebugger() || Debugger::SpawnedUnderDebugger() || Debugger::CheckDebugString())
		{
			Report::SendReport(DEBUGGER_DETECTED);
		}

		Sleep(10000);
	}
}


//====================================================================================================================


BOOL Debugger::ReadPEBForDebugger()
{
	BOOL isDebugger = FALSE;

	if (pPeb->BeingDebugged)
	{
		isDebugger = TRUE;
	}

	CheckRemoteDebuggerPresent(GetCurrentProcess(), &isDebugger);

	return isDebugger;
}

BOOL Debugger::SpawnedUnderDebugger()
{
	DWORD ntGlobalFlag = *(DWORD*)((PBYTE)pPeb + 0xBC);

	if (ntGlobalFlag & NT_GLOBAL_FLAG_CHECK)
		return TRUE;
	else return FALSE;
}

BOOL Debugger::CheckDebugString()
{
	SetLastError(0);
	OutputDebugString(L"hello debugger!");

	if (GetLastError() != 0)
		return TRUE;

	return FALSE;
}