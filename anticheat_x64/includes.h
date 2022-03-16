#define WIN32_LEAN_AND_MEAN

#ifndef MAIN_HEADERS
#define MAIN_HEADERS

#include <iostream>
#include <Windows.h>
#include <TlHelp32.h>
#include <winternl.h>
#include <thread>

#endif

#include "memory.h"
#include "thread.h"
#include "hook.h"
#include "report.h"
#include "debugger.h"


//Header information of main executable image
extern PIMAGE_DOS_HEADER DosHeader;
extern PIMAGE_NT_HEADERS NtHeader;
extern PIMAGE_FILE_HEADER FileHeader;


