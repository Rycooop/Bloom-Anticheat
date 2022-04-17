#include <Windows.h>
#include <winnt.h>
#include <DbgHelp.h>

#define MAX_TRACE_DEPTH 10

extern DWORD processThreads[100];

//Apc handler and function to queue
PAPCFUNC ApcFunction(ULONG_PTR Arg);
DWORD QueueApcs(PAPCFUNC func);

//Obtain all threads in current process
BOOL GetProcessThreads();

namespace Thread
{
	bool isRipValid(HANDLE hThread, PBOOL isValid);
	bool checkPrevAddr(HANDLE hThread);
	bool WalkStack(HANDLE hThread);

	//Thread in charge of queueing apcs and checking RIP
	void MonitorThreads();
}

enum APCQUEUE
{
	SUCCESSFULLY_QUEUED,
	NO_THREAD_INFORMATION,
	FAILED_TO_QUEUE
};
