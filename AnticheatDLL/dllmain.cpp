#include "includes.h"


//Defining main EXE PE header
PIMAGE_DOS_HEADER DosHeader;
PIMAGE_NT_HEADERS NtHeader;
PIMAGE_FILE_HEADER FileHeader;

FILE* f;

DWORD WINAPI StartupThread(HMODULE hModule)
{
    //Allocate a console for troubleshooting, this should be commented out when you are not debugging as the Report system
    //will create its own console and output there. This is just to check certain values
    
    AllocConsole();
    freopen_s(&f, "conout$", "w", stdout);
    SetConsoleTitleA("Debug");

    //Obtain information on the PE header of the executable. This will be used to traverse the module list
    //and gather other information stored in the PE header such as imports

    PVOID baseAddress = GetModuleHandle(NULL);
    DosHeader = (PIMAGE_DOS_HEADER)baseAddress;
    NtHeader = (PIMAGE_NT_HEADERS)((UINT_PTR)baseAddress + DosHeader->e_lfanew);
    FileHeader = (PIMAGE_FILE_HEADER)&NtHeader->FileHeader;


    //Install all of our hooks on commonly used API's such as GetAsyncKeyState. We can also block blacklisted modules from
    //being loaded by hooking LoadLibraryA

    if (!Hooks::InstallHooks())
    {
        Report::SendReport(INVALID_ANTICHEAT_START);
    }

    //Create threads for each of the detection loops to have efficiency as well as keep everything nice and neat. If a handle is invalid
    //the thread did not start which will not execute that set of detection techniques so we must throw an error

    HANDLE hDebuggerThread = CreateThread(0, 0, (PTHREAD_START_ROUTINE)Debugger::DebuggerThread, 0, 0, 0);
    HANDLE hMemoryThread = CreateThread(0, 0, (PTHREAD_START_ROUTINE)Memory::ScanMemory, 0, 0, 0);
    HANDLE hThreadMonitor = CreateThread(0, 0, (PTHREAD_START_ROUTINE)Thread::MonitorThreads, 0, 0, 0);
    HANDLE hOverlayThread = CreateThread(0, 0, (PTHREAD_START_ROUTINE)Overlay::OverlayThread, 0, 0, 0);
    HANDLE hIntegrityThread = CreateThread(0, 0, (PTHREAD_START_ROUTINE)Integrity::IntegrityThread, 0, 0, 0);

    if (hDebuggerThread == INVALID_HANDLE_VALUE || hMemoryThread == INVALID_HANDLE_VALUE || hThreadMonitor == INVALID_HANDLE_VALUE || hOverlayThread == INVALID_HANDLE_VALUE || hIntegrityThread == INVALID_HANDLE_VALUE)
    {
        Report::SendReport(INVALID_ANTICHEAT_START);
    }


    //Constant DLL main loop, any other checks you want to do that arent related to the other detection methods should
    //be done inside this loop for efficiency

    while (true)
    {
        if (GetAsyncKeyState(VK_F4) & 1)
        {
        }

        Sleep(6000);
    }

    return 0;
}

//Dll entry point, always best practice to create a new thread and then close the handle

BOOL APIENTRY DllMain( HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        CloseHandle(CreateThread(0, 0, (LPTHREAD_START_ROUTINE)StartupThread, hModule, 0, 0));
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

