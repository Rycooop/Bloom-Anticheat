#include "includes.h"


//Loop to check the memory detections and look for manually mapped modules. This will be effective if a blacklisted module is
//loaded before the anticheat is loaded, and it will also look for executable pages outside a valid module, indicating
//that a DLL or shellcode has been put in place

void Memory::ScanMemory()
{
    HANDLE hThread = GetCurrentThread();

    while (true)
    {
        if (Memory::isBlacklistedModuleFound())
        {
            Report::SendReport(BLACKLISTED_DLL_DETECTED);
            break;
        }

        Sleep(7000);
    }
}


//================================================================================================================


//Scan through the PEB loaded module list looking for blacklisted modules

BOOL Memory::isBlacklistedModuleFound()
{
    static PPEB pPeb = (PPEB)__readgsword(0x60);
    int numBlacklistedMods = sizeof(BlacklistedModulesW) / sizeof(BlacklistedModulesW[0]);
    
    PPEB_LDR_DATA pebLdr = pPeb->Ldr;

    return FALSE;
}


//use VirtualQuery to scan through all memory pages within the process and find anomalies

BOOL Memory::ScanForExecutablePages()
{
    AllocConsole();
    FILE* f;
    freopen_s(&f, "conout$", "w", stdout);

    MEMORY_BASIC_INFORMATION memInfo;
    
    for (int i = 0; i < 1000; i++)
    {
        VirtualQuery((LPVOID)(i * 4096), &memInfo, sizeof(MEMORY_BASIC_INFORMATION));

        std::cout << "Executable page: " << memInfo.AllocationBase << "  " << memInfo.Protect << std::endl;
    }
   
    return TRUE;
}


//Pattern scan for known cheat images

void* Memory::patternScan(const char* pattern, const char* mask)
{

}


//Is the address given inside of a valid module 

bool Memory::isValidModuleAddr(uintptr_t address)
{
    MODULEENTRY32 modEntry;
    HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, GetCurrentProcessId());

    if (hSnap == INVALID_HANDLE_VALUE)
        return false;

    modEntry.dwSize = sizeof(MODULEENTRY32);

    if (!Module32First(hSnap, &modEntry))
    {
        return false;
    }

    if (address >= (uintptr_t)modEntry.modBaseAddr && address <= (uintptr_t)modEntry.modBaseAddr + (uintptr_t)modEntry.modBaseSize)
        return true;

    while (Module32Next(hSnap, &modEntry))
    {
        if (address >= (uintptr_t)modEntry.modBaseAddr && address <= (uintptr_t)modEntry.modBaseAddr + (uintptr_t)modEntry.modBaseSize)
            return true;
    }

    return false;
}