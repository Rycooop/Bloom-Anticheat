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

BOOL Integrity::AreFunctionsSpoofed()
{
	//An address we know will be vacant, should return FALSE unless someone hooks it
	if (Memory::isValidModuleAddr(0x2000) == TRUE)
	{
		return TRUE;
	}

	return FALSE;
}