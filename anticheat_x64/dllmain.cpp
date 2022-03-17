#include "includes.h"


//Defining main EXE PE header
PIMAGE_DOS_HEADER DosHeader;
PIMAGE_NT_HEADERS NtHeader;
PIMAGE_FILE_HEADER FileHeader;


DWORD WINAPI StartupThread(HMODULE hModule)
{
    //Console for troubleshooting
    /*
    AllocConsole();
    FILE* f;
    freopen_s(&f, "conout$", "w", stdout);
    */

    //Get main EXE PE header information
    PVOID baseAddress = GetModuleHandle(NULL);
    DosHeader = (PIMAGE_DOS_HEADER)baseAddress;
    NtHeader = (PIMAGE_NT_HEADERS)((UINT_PTR)baseAddress + DosHeader->e_lfanew);
    FileHeader = (PIMAGE_FILE_HEADER)&NtHeader->FileHeader;

    //Start Debugger thread
    HANDLE hDebuggerThread = CreateThread(0, 0, (PTHREAD_START_ROUTINE)Debugger::DebuggerThread, 0, 0, 0);
    if (hDebuggerThread == INVALID_HANDLE_VALUE)
    {
        CloseHandle(hDebuggerThread);

        std::thread debuggerThread(Debugger::DebuggerThread);
        debuggerThread.detach();
    }

    if (!Hooks::InstallHooks())
    {
        Report::SendReport(INVALID_ANTICHEAT_START);
    }

    while (true)
    {
        if (GetAsyncKeyState(VK_F7) & 1)
        {

        }
        Sleep(6000);
    }

    return 0;
}

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

