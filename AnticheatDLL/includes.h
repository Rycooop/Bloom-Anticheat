#define WIN32_LEAN_AND_MEAN
#define _CRT_SECURE_NO_WARNINGS


//This project was put together as a compilation of the knowledge I have learned from not only developing cheats, but analyzing software on windows platforms.
//If you plan on implementing this into your application or game, I highly recommend obfuscating strings, packing this executable(VMProtect ideally), and not
//allowing this module to even touch disk. The "framework" I have made for this anticheat easily allows changes, and additions. The only real weakness I
//can personally see from within this module is the weak integrity check I have implemented. I will probably improve it over time but it will keep away
//most. All real external detections(excluding overlays) will be handled by the driver, and by the module our main .exe injects into CSRSS and LSASS that
//proxies all ReadProcessMemory and WriteProcessMemory calls ;)

#ifndef MAIN_HEADERS
#define MAIN_HEADERS

#include <iostream>
#include <stdio.h>
#include <Windows.h>
#include <TlHelp32.h>
#include <intrin.h>
#include <Psapi.h>
#include <winternl.h>
#include <string>
#include <thread>

#endif

#include "memory.h"
#include "thread.h"
#include "overlay.h"
#include "hook.h"
#include "report.h"
#include "integrity.h"
#include "iat.h"
#include "debugger.h"


//Header information of main executable image
extern PIMAGE_DOS_HEADER DosHeader;
extern PIMAGE_NT_HEADERS NtHeader;
extern PIMAGE_FILE_HEADER FileHeader;

extern FILE* f;



