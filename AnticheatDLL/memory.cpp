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
    HANDLE hSnap;
    MODULEENTRY32 modEntry;
    hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, GetCurrentProcessId());

    if (hSnap == INVALID_HANDLE_VALUE)
        return FALSE;

    modEntry.dwSize = sizeof(MODULEENTRY32);

    if (!Module32First(hSnap, &modEntry))
    {
        return FALSE;
    }

    for (int i = 0; i < sizeof(BlacklistedModulesW) / sizeof(BlacklistedModulesW[0]); i++)
    {
        if (wcsstr(modEntry.szModule, BlacklistedModulesW[i]))
            return TRUE;
    }

    while (Module32Next(hSnap, &modEntry))
    {
        for (int i = 0; i < sizeof(BlacklistedModulesW) / sizeof(BlacklistedModulesW[0]); i++)
        {
            if (wcsstr(modEntry.szModule, BlacklistedModulesW[i]))
                return TRUE;
        }
    }

    return FALSE;
}


//use VirtualQuery to scan through all memory pages within the process and find anomalies

BOOL Memory::ScanForExecutablePages()
{
    AllocConsole();
    FILE* f;
    freopen_s(&f, "conout$", "w", stdout);

    MEMORY_BASIC_INFORMATION memInfo;
    VirtualQuery(0, &memInfo, 4096);

    return TRUE;
}


//Pattern scan for known cheat images. I made this at like 2am so it probably is very inneficient but it should only
//return an addr when the full pattern is fine
//
// @param Pattern of bytes to search
// @param Mask of the pattern
// @return Address of the found pattern, NULL if not found
//
void* Memory::patternScan(const char* pattern, const char* mask)
{
    if (strlen(pattern) != strlen(mask) || mask[0] == '?')
        return NULL;

    BYTE* modBegin = (BYTE*)GetModuleHandle(0);
    MODULEINFO ModInfo;
    
    if (!GetModuleInformation(GetCurrentProcess(), GetModuleHandle(0), &ModInfo, sizeof(ModInfo)))
        return NULL;

    if (ModInfo.lpBaseOfDll != modBegin)
        return NULL;

    for (int i = 0; i < ModInfo.SizeOfImage; i++)
    {
        if (*(char*)(modBegin + i) == pattern[0])
        {
            for (int j = 1; j < strlen(pattern); j++)
            {
                if (mask[j] == '?')
                    continue;

                if (*(char*)(modBegin + i + j) != pattern[j])
                    break;

                if (j == strlen(pattern) - 1)
                    return (void*)(modBegin + i);
            }
        }
    }

    return NULL;
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