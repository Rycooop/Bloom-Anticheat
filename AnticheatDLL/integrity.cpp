#include "includes.h"


const char* hookedFuns[] = {"GetAsyncKeyState", "LoadLibraryA", "LoadLibraryW"};


VOID Integrity::IntegrityThread()
{
	while (true)
	{
		if (Integrity::AreHooksInPlace() != TRUE)
		{
			Report::SendReport(INTEGRITY_CHECK_VIOLATION);
		}

		if (Integrity::AreFunctionsSpoofed() == TRUE)
		{
			Report::SendReport(INTEGRITY_CHECK_VIOLATION);
		}

		if (IAT::ScanForIATHook(GetModuleHandle(L"AnticheatDLL.dll")))
		{
			Report::SendReport(IAT_HOOK_DETECTED);
		}

		Sleep(10000);
	}
}


//=======================================================================================================


//Check to see if the hooks we placed on initialization are still in place. If they arent, something has purposely overwritten
//them and action should be taken. We could also replace our hooks everytime this function is called as a counter measure.
//
BOOL Integrity::AreHooksInPlace()
{
	static HMODULE hUser32 = GetModuleHandle(L"user32.dll");
	if (hUser32 == INVALID_HANDLE_VALUE)
		return TRUE;

	BYTE* funcStart = (BYTE*)GetProcAddress(hUser32, "GetAsyncKeyState");

	if (*funcStart != 0xE9)
	{
		return FALSE;
	}

	return TRUE;
}


//Are any of our functions hooked or spoofed. Someone could hook isValidModuleAddr to always return true which will prevent
//our primary detection of suspicious threads and manually mapped modules
//
BOOL Integrity::AreFunctionsSpoofed()
{
	//An address we know will be vacant, should return FALSE unless someone hooks it
	if (Memory::isValidModuleAddr(0x2000) == TRUE)
	{
		return TRUE;
	}

	return FALSE;
}