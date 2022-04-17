#include "includes.h"

//Define certain debugger flags

#define FLG_HEAP_ENABLE_TAIL_CHECK   0x10
#define FLG_HEAP_ENABLE_FREE_CHECK   0x20
#define FLG_HEAP_VALIDATE_PARAMETERS 0x40
#define NT_GLOBAL_FLAG_CHECK (FLG_HEAP_ENABLE_TAIL_CHECK | FLG_HEAP_ENABLE_FREE_CHECK | FLG_HEAP_VALIDATE_PARAMETERS)

typedef NTSTATUS(WINAPI* tNtQuerySystemInformation)(SYSTEM_INFORMATION_CLASS SystemInformationClass, PVOID SystemInformation, ULONG SystemInformationLength, PULONG ReturnLength);
typedef NTSTATUS(WINAPI* tNtSetInformationThread)(HANDLE ThreadHandle, THREADINFOCLASS ThreadInfoClass, PVOID ThreadInformation, ULONG ThreadInformationLength);

//Compilation of many different methods to detect/prevent debuggers. Some of these can throw false positives, but most
//of them will work only on the case of a debugger. This will detect VEH debuggers, and can be quite useful. As already stated, feel free to alter these and disable/add new 
//protections

//You could use the FindWindow function and constantly search for things like cheat engine, memory-viewer, and x64dbgg however this is not a professional solution to debugging,
//its more of a hack. We want to keep this as less invasive as we can


tNtQuerySystemInformation oNtQuerySystemInformation;
tNtSetInformationThread oNtSetInformationThread;
PPEB pPeb = (PPEB)__readgsqword(0x60);

void Debugger::DebuggerThread()
{
	//Obtain the address and cast to a function pointer the ntdll export NtSetInformationThread()
	HMODULE hNtdll = GetModuleHandle(L"ntdll.dll");
	if (hNtdll)
	{
		oNtQuerySystemInformation = (tNtQuerySystemInformation)GetProcAddress(hNtdll, "NtQuerySystemInformation");
		oNtSetInformationThread = (tNtSetInformationThread)GetProcAddress(hNtdll, "NtSetInformationThread");
	}

	while (true)
	{
		//Create the exception handlers and keep registering to beat VEH debuggers
		PVOID exceptionHandle = AddVectoredExceptionHandler(1, (PVECTORED_EXCEPTION_HANDLER)Debugger::HandleExceptions);

		if (Debugger::ReadPEBForDebugger() || Debugger::SpawnedUnderDebugger() || Debugger::CheckDebugString())
		{
			Report::SendReport(DEBUGGER_DETECTED);
		}

		SYSTEM_KERNEL_DEBUGGER_INFORMATION SysInfo;
		if (NT_SUCCESS(oNtQuerySystemInformation((SYSTEM_INFORMATION_CLASS)0x23, &SysInfo, sizeof(SysInfo), NULL)))
		{
			if (SysInfo.DebuggerEnabled || !SysInfo.DebuggerNotPresent)
				Report::SendReport(KERNEL_DEBUGGING_ENABLED);
		}

		Sleep(4000);

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
	OutputDebugString(L"Hello Debugger!");

	if (GetLastError() != 0)
		return TRUE;

	return FALSE;
}

//Check all the debug registers if a debug exception is thrown
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
		//This would indicate a breakpoint has been set and hit
		Report::SendReport(DEBUGGER_DETECTED);
	}
	else if (exceptionInfo->ExceptionRecord->ExceptionCode == EXCEPTION_DEBUG_EVENT)
	{
		Report::SendReport(DEBUGGER_DETECTED);
	}
	else if (exceptionInfo->ExceptionRecord->ExceptionCode == STATUS_GUARD_PAGE_VIOLATION)
	{
		//This could throw false positives but most of the time this will be effective in detecting GUARD_PAGE hooks
		Report::SendReport(DEBUGGER_DETECTED);
	}

	return EXCEPTION_CONTINUE_SEARCH;
}