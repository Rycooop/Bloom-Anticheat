#include "includes.h"


//Declarations
tHLoadLibraryA oLoadLibraryA = nullptr;
tHLoadLibraryW oLoadLibraryW = nullptr;
tHKeyAsyncKeyState oGetAsyncKeyState = nullptr;

const char* FuncsToHook[] =
{
	"LoadLibraryA", "LoadLibraryW",
	"NtQueryVirtualMemory",
	"GetAsynkKeyState"
	"Direct3DCreate9"
};

const char* BlacklistedModules[] =
{
	"cheat.dll", "aimbot.dll", "esp.dll", "hack.dll"
};

const wchar_t* BlacklistedModulesW[] =
{
	L"cheat.dll", L"aimbot.dll", L"esp.dll", L"hack.dll"
};


//------------------------------------------------------------------------------------


BOOL Hooks::InstallHooks()
{
	HMODULE hNtll =		GetModuleHandle(L"ntdll.dll");
	HMODULE hKernel32 = GetModuleHandle(L"kernel32.dll");
	HMODULE hKernelBase = GetModuleHandle(L"kernelbase.dll");
	HMODULE hUser32 = GetModuleHandle(L"user32.dll");

	if (hNtll && hKernel32 && hKernelBase && hUser32)
	{
		oLoadLibraryA = (tHLoadLibraryA)Hooks::Hook((BYTE*)GetProcAddress(hKernelBase, "LoadLibraryA"), (BYTE*)Hooks::hkLoadLibraryA, 5);
		oLoadLibraryW = (tHLoadLibraryW)Hooks::Hook((BYTE*)GetProcAddress(hKernelBase, "LoadLibraryW"), (BYTE*)Hooks::hkLoadLibraryW, 5);
		oGetAsyncKeyState = (tHKeyAsyncKeyState)Hooks::Hook((BYTE*)GetProcAddress(hUser32, "GetAsyncKeyState"), (BYTE*)Hooks::hkGetAsyncKeyState, 5);
	}
	else return FALSE;

	return TRUE;
}

void* Hooks::Hook(BYTE* src, BYTE* dst, size_t size)
{
	//Create a trampoline hook to automatically restore stolen bytes
	BYTE* newAlloc = (BYTE*)VirtualAlloc(0, size + 10, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
	uintptr_t newAddr = (uintptr_t)(src + size);
	DWORD relAddr = (DWORD)(dst - src - 5);

	DWORD gatewayProtect;
	if (VirtualProtect(newAlloc, size, PAGE_EXECUTE_READWRITE, &gatewayProtect))
	{
		memcpy(newAlloc, src, size);
		memcpy((BYTE*)(newAlloc + size), (BYTE*)"\x48\xBA", 2);
		memcpy((BYTE*)(newAlloc + size + 2), &newAddr, 8);
		memcpy((BYTE*)(newAlloc + size + 10), (BYTE*)"\xFF\xE2", 2);
		VirtualProtect(newAlloc, size, gatewayProtect, &gatewayProtect);
	}

	DWORD oldProtect;
	if (VirtualProtect(src, size, PAGE_EXECUTE_READWRITE, &oldProtect))
	{
		*src = 0xE9;
		*(DWORD*)(src + 1) = relAddr;
		VirtualProtect(src, size, oldProtect, &oldProtect);
	}

	return newAlloc;
}


//------------------------------------------------------------------------------------------------


HMODULE Hooks::hkLoadLibraryA(LPCSTR lpLibFileName)
{
	//Check if the module attempting to load is a blacklisted one
	for (int i = 0; i < sizeof(BlacklistedModules) / sizeof(*BlacklistedModules); i++)
	{
		if (strstr(lpLibFileName, BlacklistedModules[i]))
		{
			Report::SendReport(BLACKLISTED_DLL_DETECTED);
			break;
		}
	}

	return oLoadLibraryA(lpLibFileName);
}

HMODULE Hooks::hkLoadLibraryW(LPCWSTR lpLibFileName)
{
	for (int i = 0; i < sizeof(BlacklistedModulesW) / sizeof(*BlacklistedModulesW); i++)
	{
		if (wcsstr(lpLibFileName, BlacklistedModulesW[i]))
		{
			Report::SendReport(BLACKLISTED_DLL_DETECTED);
			break;
		}
	}

	return oLoadLibraryW(lpLibFileName);
}

SHORT Hooks::hkGetAsyncKeyState(int vKey)
{
	return oGetAsyncKeyState(vKey);
}