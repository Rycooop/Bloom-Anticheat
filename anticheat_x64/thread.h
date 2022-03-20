#include <Windows.h>
#include <winnt.h>
#include <DbgHelp.h>


extern DWORD processThreads[100];

//Apc handler and function to queue
PAPCFUNC ApcFunction(ULONG_PTR Arg);
DWORD QueueApcs(PAPCFUNC func);

//Obtain all threads in current process
BOOL GetProcessThreads();
void scanThreads();

namespace Thread
{
	bool isRipValid(HANDLE hThread, PBOOL isValid);
	bool checkReturnAddr(HANDLE hThread);
}

enum APCQUEUE
{
	SUCCESSFULLY_QUEUED,
	NO_THREAD_INFORMATION,
	FAILED_TO_QUEUE
};