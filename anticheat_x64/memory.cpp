#include "includes.h"



BOOL Memory::isBlacklistedModuleFound()
{
    return FALSE;

    static PPEB pPeb = (PPEB)__readgsword(0x60);

    for (PLIST_ENTRY entry = pPeb->Ldr->InMemoryOrderModuleList.Flink; entry != &pPeb->Ldr->InMemoryOrderModuleList; entry = entry->Flink)
    {
        if (!entry)
            continue;

        PLDR_DATA_TABLE_ENTRY modEntry = CONTAINING_RECORD(entry, LDR_DATA_TABLE_ENTRY, InMemoryOrderLinks);

        /*
        for (int i = 0; i < 3; i++)
        {
            if (wcsstr(modEntry->FullDllName.Buffer, BlacklistedModulesW[i]))
            {
                return TRUE;
            }
        }
        */
    }

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