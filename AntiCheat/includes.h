#define _CRT_SECURE_NO_WARNINGS
#define _WIN32_LEAN_AND_MEAN

#include <Windows.h>
#include <iostream>
#include <winternl.h>
#include <thread>
#include <string>
#include <Shlwapi.h>

#pragma comment(lib, "Shlwapi.lib")

#include "util.h"
#include "driver.h"
#include "injection.h"
#include "error.h"

#define PROTECTED_PROCESS	L"notepad.exe"

typedef NTSTATUS(* tNtLoadDriver)(PUNICODE_STRING DriverServiceName);

typedef struct GLOBALS
{
	HANDLE hProcess;
	DWORD processProcID;
	BOOL isInjected;
	int error;
};

extern GLOBALS Globals;
extern FILE* f;
