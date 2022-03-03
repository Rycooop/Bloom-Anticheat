#include <Windows.h>
#include <iostream>
#include <winternl.h>
#include <thread>

#include "util.h"
#include "error.h"

#define PROTECTED_PROCESS L"ac_client.exe"

extern FILE* f;


struct GLOBALS
{
	HANDLE hProcess;
	DWORD processProcID;
	BOOL isInjected = FALSE;
	int error = 0;
}Globals;