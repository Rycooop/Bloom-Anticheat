#include "includes.h"


BOOL Utility::isBlacklistedModuleFound()
{
    static PPEB pPeb = (PPEB)__readgsword(0x60);

    for (PLIST_ENTRY entry = pPeb->Ldr->InMemoryOrderModuleList.Flink; entry != &pPeb->Ldr->InMemoryOrderModuleList; entry = entry->Flink)
    {
        if (!entry)
            continue;

        PLDR_DATA_TABLE_ENTRY modEntry = CONTAINING_RECORD(entry, LDR_DATA_TABLE_ENTRY, InMemoryOrderLinks);

        /*
        for (int i = 0; i < sizeof(BlacklistedModulesW) / sizeof(*BlacklistedModulesW); i++)
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