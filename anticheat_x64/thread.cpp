#include "includes.h"


DWORD processThreads[100];

//Apc function to be executed on every thread
PAPCFUNC ApcFunction(ULONG_PTR Arg)
{
	HANDLE hThread = GetCurrentThread();

	BOOL isValid = TRUE;
	if (Thread::isRipValid(hThread, &isValid))
	{
		if (isValid == FALSE)
		{
			//Report::SendReport(RIP_OUTSIDE_VALID_MODULE);
		}
	}

	return 0;
}
  
DWORD QueueApcs(PAPCFUNC func)
{
	if (!processThreads)
		return NO_THREAD_INFORMATION;

	DWORD numOfThread = sizeof(processThreads) / sizeof(processThreads[0]);

	if (numOfThread < 1)
		return NO_THREAD_INFORMATION;

	for (int i = 0; i < numOfThread; i++)
	{
		HANDLE hThread = OpenThread(THREAD_ALL_ACCESS, FALSE, processThreads[i]);
		if (hThread == INVALID_HANDLE_VALUE || hThread == 0)
			continue;

		if (!QueueUserAPC(func, hThread, 0))
			return FAILED_TO_QUEUE;
	}

	return SUCCESSFULLY_QUEUED;
}


//========================================================================================================


BOOL GetProcessThreads()
{
	THREADENTRY32 threadEntry;
	HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
	if (hSnap == INVALID_HANDLE_VALUE)
		return FALSE;
	 
	threadEntry.dwSize = sizeof(THREADENTRY32);

	if (!Thread32First(hSnap, &threadEntry))
		return FALSE;

	int curr = 0;
	while (Thread32Next(hSnap, &threadEntry))
	{
		if (curr >= 100)
			break;

		if (!threadEntry.th32ThreadID)
			continue;

		if (threadEntry.th32OwnerProcessID == GetCurrentProcessId())
		{
			processThreads[curr] = threadEntry.th32ThreadID;
			curr++;
		}
	}

	return TRUE;
}

void scanThreads()
{
	if (!processThreads)
		return;

	//Use an enhanced for loop to scan through all the threads in the thread array
	for (DWORD currThread : processThreads)
	{
		if (!currThread)
			continue;
		
		HANDLE hCurrThread = OpenThread(THREAD_ALL_ACCESS, FALSE, currThread);

		BOOL ripValid = TRUE;
		if (Thread::checkReturnAddr(hCurrThread))
		{
			if (ripValid == FALSE)
				Report::SendReport(RIP_OUTSIDE_VALID_MODULE);
		}

		if (hCurrThread)
			CloseHandle(hCurrThread);
	}
}


//========================================================================================================


//Checks if the instruction pointer of a given thread is currently in a valid module
bool Thread::isRipValid(HANDLE hThread, PBOOL isValid)
{
	STACKFRAME64 sFrame;
	CONTEXT sContext;
	ZeroMemory(&sFrame, sizeof(STACKFRAME64));
	ZeroMemory(&sContext, sizeof(CONTEXT));
	sContext.ContextFlags = CONTEXT_CONTROL;

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

	return true;
}