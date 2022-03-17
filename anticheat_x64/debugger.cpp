#include "includes.h"

#define FLG_HEAP_ENABLE_TAIL_CHECK   0x10
#define FLG_HEAP_ENABLE_FREE_CHECK   0x20
#define FLG_HEAP_VALIDATE_PARAMETERS 0x40
#define NT_GLOBAL_FLAG_CHECK (FLG_HEAP_ENABLE_TAIL_CHECK | FLG_HEAP_ENABLE_FREE_CHECK | FLG_HEAP_VALIDATE_PARAMETERS)


//Compilation of many different methods to detect/prevent debuggers. Some of these can throw false positives, but most
//of them will work only on the case of a debugger. This will detect VEH debuggers, and can be quite useful. As already stated, feel free to alter these and disable/add new 
//protections


PPEB pPeb = (PPEB)__readgsqword(0x60);

void Debugger::DebuggerThread()
{

	while (true)
	{
		//Create the exception handlers and keep registering to beat VEH debuggers
		PVOID exceptionHandle = AddVectoredExceptionHandler(1, (PVECTORED_EXCEPTION_HANDLER)Debugger::HandleExceptions);

		if (Debugger::ReadPEBForDebugger() || Debugger::SpawnedUnderDebugger() || Debugger::CheckDebugString())
		{
			Report::SendReport(DEBUGGER_DETECTED);
		}

		Sleep(5000);

		//Destroy the handler to re-register a new one as top priority
		if (exceptionHandle)
			RemoveVectoredExceptionHandler(exceptionHandle);
	}
}


//====================================================================================================================


BOOL Debugger::ReadPEBForDebugger()
{
	//Maually doing what IsDebuggerPresent() does
	if (pPeb->BeingDebugged)
	{
		return TRUE;
	}

	//Check process debug port via NtQueryInformationProcess()
	BOOL isDebugger = FALSE;
	CheckRemoteDebuggerPresent(GetCurrentProcess(), &isDebugger);

	return isDebugger;
}

//Will see if the current process was started under a debugger, such as IDA or x64dbg
BOOL Debugger::SpawnedUnderDebugger()
{
	DWORD ntGlobalFlag = *(DWORD*)((PBYTE)pPeb + 0xBC);

	if (ntGlobalFlag & NT_GLOBAL_FLAG_CHECK)
		return TRUE;
	else return FALSE;
}

//If a debugger is attached, OutputDebugString will register an error
BOOL Debugger::CheckDebugString()
{
	SetLastError(0);
	OutputDebugString(L"hello debugger!");

	if (GetLastError() != 0)
		return TRUE;

	return FALSE;
}

LONG Debugger::HandleExceptions(PEXCEPTION_POINTERS exceptionInfo)
{
	//if a single step exception is thrown
	if (exceptionInfo->ExceptionRecord->ExceptionCode == EXCEPTION_SINGLE_STEP)
	{
		if (exceptionInfo->ContextRecord->Dr0 != 0 || exceptionInfo->ContextRecord->Dr1 != 0
			|| exceptionInfo->ContextRecord->Dr2 != 0 || exceptionInfo->ContextRecord->Dr3 != 0
			|| exceptionInfo->ContextRecord->Dr6 != 0 || exceptionInfo->ContextRecord->Dr7 != 0)
		{
			//Debug registers are in use
			Report::SendReport(DEBUGGER_DETECTED);
		}
	}
	else if (exceptionInfo->ExceptionRecord->ExceptionCode == EXCEPTION_BREAKPOINT)
	{
		//This would indicate a softwar breakpoint has been set and hit
		Report::SendReport(DEBUGGER_DETECTED);
	}

	return EXCEPTION_CONTINUE_SEARCH;
}