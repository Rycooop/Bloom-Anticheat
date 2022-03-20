#include "includes.h"



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


BOOL Memory::isBlacklistedModuleFound()
{
    static PPEB pPeb = (PPEB)__readgsword(0x60);
    int numBlacklistedMods = sizeof(BlacklistedModulesW) / sizeof(BlacklistedModulesW[0]);
    
    PPEB_LDR_DATA pebLdr = pPeb->Ldr;

    return FALSE;
}

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

void* Memory::patternScan(const char* pattern, const char* mask)
{

}

bool Memory::isValidModuleAddr(PVOID address)
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

    if (address >= modEntry.modBaseAddr && address <= modEntry.modBaseAddr + modEntry.modBaseSize)
        return true;

    while (Module32Next(hSnap, &modEntry))
    {
        if (address >= modEntry.modBaseAddr && address <= modEntry.modBaseAddr + modEntry.modBaseSize)
            return true;
    }

    return false;
}