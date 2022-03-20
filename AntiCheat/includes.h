#define _CRT_SECURE_NO_WARNINGS
#define _WIN32_LEAN_AND_MEAN

//Define needed headers and libraries
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

//Global information that will be needed for the process
typedef struct GLOBALS
{
	HANDLE hProcess;
	DWORD processProcID;
	BOOL isInjected;
	int error;
};

//Define the globals
extern GLOBALS Globals;
extern FILE* f;
