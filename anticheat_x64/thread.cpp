#include "includes.h"


//Checks if the instruction pointer of a given thread is currently in a valid module
bool Thread::isRipValid(HANDLE hThread, PBOOL isValid)
{
	STACKFRAME64 sFrame;
	CONTEXT sContext;
	ZeroMemory(&sFrame, sizeof(STACKFRAME64));
	ZeroMemory(&sContext, sizeof(CONTEXT));

	if (GetThreadId(hThread) == GetCurrentThreadId())
		RtlCaptureContext(&sContext);
	else
	{
		SuspendThread(hThread);
		GetThreadContext(hThread, &sContext);
		ResumeThread(hThread);
	}

	sFrame.AddrPC.Offset = sContext.Rip;
	sFrame.AddrPC.Mode = AddrModeFlat;
	sFrame.AddrStack.Offset = sContext.Rsp;
	sFrame.AddrStack.Mode = AddrModeFlat;
	sFrame.AddrFrame.Offset = sContext.Rbp;
	sFrame.AddrFrame.Mode = AddrModeFlat;
	
	//This isnt necessary but good practice because it obtains a full stack trace of the given thread
	if (StackWalk64(FileHeader->Machine, GetCurrentProcess(), hThread, &sFrame, &sContext, NULL, NULL, NULL, NULL))
	{
		if (Memory::isValidModuleAddr((PVOID)sFrame.AddrPC.Offset))
		{
			*isValid = TRUE;
			return true;
		}
	}
	else return false;

	*isValid = FALSE;
	return true;
}

bool Thread::checkReturnAddr(HANDLE hThread)
{
	CONTEXT sContext;
	ZeroMemory(&sContext, sizeof(CONTEXT));

	if (GetThreadId(hThread) == GetCurrentThreadId())
	{
		RtlCaptureContext(&sContext);
	}
	else
	{
		SuspendThread(hThread);
		GetThreadContext(hThread, &sContext);
		ResumeThread(hThread);
	}

	CaptureStackBackTrace(0, 5, NULL, NULL);

	return false;
}