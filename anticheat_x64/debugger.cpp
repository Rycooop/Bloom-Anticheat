#include "includes.h"

#define FLG_HEAP_ENABLE_TAIL_CHECK   0x10
#define FLG_HEAP_ENABLE_FREE_CHECK   0x20
#define FLG_HEAP_VALIDATE_PARAMETERS 0x40
#define NT_GLOBAL_FLAG_CHECK (FLG_HEAP_ENABLE_TAIL_CHECK | FLG_HEAP_ENABLE_FREE_CHECK | FLG_HEAP_VALIDATE_PARAMETERS)


//Plan on adding more in the future, currently reads from the PEB for the being debugged flag as well as the NT_GLOBAL
//to see if the process is being debugged or was spawned under a debugger. Also uses checkremotedebuggerpresent, which
//calls NtQueryInformationProcess with ProcessDebugPort(0x07) to check for debugger. This function can be easily hooked
//to hide the presence of a debugger, therefore, I install a hook to counter previous hooks at the beginning of the function


PPEB pPeb = (PPEB)__readgsqword(0x60);

void Debugger::DebuggerThread()
{
	while (true)
	{
		if (Debugger::IsDebuggerAttached() || Debugger::SpawnedUnderDebugger())
		{
			Report::SendReport(DEBUGGER_DETECTED);
		}

		Sleep(3000);
	}
}


//------------------------------------------------------------------------------------------------


BOOL Debugger::IsDebuggerAttached()
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