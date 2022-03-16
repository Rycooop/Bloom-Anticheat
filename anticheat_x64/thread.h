#include <Windows.h>
#include <winnt.h>
#include <DbgHelp.h>


namespace Thread
{
	bool isRipValid(HANDLE hThread, PBOOL isValid);
	bool checkReturnAddr(HANDLE hThread);

	void scanThreads();
}