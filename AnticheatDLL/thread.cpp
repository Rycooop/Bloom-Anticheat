#include "includes.h"


//One good way of monitoring code execution is by Queueing Asynchronous Procedure Calls to every thread running in a process.
//By doing this we can have our own function execute inside the context of every single thread, which makes it easier for us 
//to run a stack trace or analyze the stack frames. We must continuously queue APC's every few seconds

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
			Report::SendReport(RIP_OUTSIDE_VALID_MODULE);
		}
	}


	//This will check where each thread came from, if outside of a valid module it will submit a report

	if (!Thread::checkPrevAddr(GetCurrentThread()))
		Report::SendReport(RIP_OUTSIDE_VALID_MODULE);

	return 0;
}
 
//Queue the apcs to every thread in the process
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


//Obtain all of the threads currently running in the process using Tlhelp32 THREADENTRY32

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
		if (threadEntry.th32OwnerProcessID == GetCurrentProcessId())
		{
			if (!threadEntry.th32ThreadID)
				continue;

			if (curr >= 100)
				break;

			processThreads[curr] = threadEntry.th32ThreadID;
			curr++;
		}
	}

	return TRUE;
}


//========================================================================================================


//Checks if the instruction pointer of a given thread is currently in a valid module. If unsigned code is
//Being executed this will return false and we will know shellcode has been injected, or a manually mapped module

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
		if (Memory::isValidModuleAddr((uintptr_t)sFrame.AddrPC.Offset))
		{
			*isValid = TRUE;
			return true;
		}
	}
	else return false;

	*isValid = FALSE;
	return true;
}

bool Thread::checkPrevAddr(HANDLE hThread)
{
	PVOID stackTrace[1] = { 0 };

	if (RtlCaptureStackBackTrace(0, 1, stackTrace, NULL))
	{
		if (!Memory::isValidModuleAddr((uintptr_t)stackTrace[0]))
			return false;
	}

	return true;
}


//Will initiate a stack trace and log up to the previous 10 stack frames. If any of the return addresses are not from a valid module,
//the function will return false and a report will be submitted for RIP_OUTSIDE_VALID_MODULE

//this is EXTREMELY efficient in catching manually mapped modules as at some point, the cheat thread will call a function and we can
//immidiately catch the return address as being outside a valid module. To do further checks, you could VirtualQuery the address and
//upload the whole module or function to your server

bool Thread::WalkStack(HANDLE hThread)
{
	PVOID stackTrace[MAX_TRACE_DEPTH] = { 0 };

	if (RtlCaptureStackBackTrace(0, MAX_TRACE_DEPTH, stackTrace, NULL))
	{
		for (PVOID currRetAddr : stackTrace)
		{
			if (!currRetAddr)
				continue;

			if (!Memory::isValidModuleAddr((uintptr_t)currRetAddr))
				return false;
		}
	}

	return true;
}


//=================================================================================================================


//This thread is in charge of running our checks and constantly gathering thread information. We create a thread to execute this function
//inside the dllmain. Any other checks you want to run on threads should be initialized in here

void Thread::MonitorThreads()
{
	BOOL valid = TRUE;

	while (true)
	{
		if (valid == FALSE)
			Report::SendReport(RIP_OUTSIDE_VALID_MODULE);

		//Make sure all threads are up to date
		GetProcessThreads();


		//Queue the APCS to all the threads
		QueueApcs((PAPCFUNC)ApcFunction);

		Sleep(7000);
	}
}