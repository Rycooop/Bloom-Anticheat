#include "includes.h"


//The hooks on this page will consist of a trampoline to restore the functionality of the original functions
//As long as you pass through an appropriate length for the hook, the below functions will take care of any BYTE restoring
// and redirects. This gives us total control flow over subsequent functions.


//Declarations
tHLoadLibraryA oLoadLibraryA = nullptr;
tHLoadLibraryW oLoadLibraryW = nullptr;
tHKeyAsyncKeyState oGetAsyncKeyState = nullptr;
tNtQueryInformationProcess oNtQueryInformationProcess = nullptr;

const char* FuncsToHook[] =
{
	"LoadLibraryA", "LoadLibraryW",
	"NtQueryVirtualMemory",
	"GetAsyncKeyState"
	"Direct3DCreate9"
};

const char* BlacklistedModules[] =
{
	"cheat.dll", "aimbot.dll", "esp.dll", "hack.dll"
	"dll.dll", "csgo.dll", "ac.dll", "battleye.dll"
};


//===========================================================================================


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
		//oNtQueryInformationProcess = (tNtQueryInformationProcess)Hooks::Hook((BYTE*)GetProcAddress(hNtll, "NtQueryInformationProcess"), (BYTE*)Hooks::hkNtQueryInformationProcess, 8);
	}
	else return FALSE;

	return TRUE;
}

void* Hooks::Hook(BYTE* src, BYTE* dst, size_t size)
{
	//Create a trampoline hook to automatically restore stolen bytes
	BYTE* newAlloc = (BYTE*)VirtualAlloc(0, size + 10, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
	if (!newAlloc)
		return NULL;

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


//=================================================================================================


HMODULE Hooks::hkLoadLibraryA(LPCSTR lpLibFileName)
{
	//Check if the module attempting to load is a blacklisted one
	for (int i = 0; i < sizeof(BlacklistedModules) / sizeof(BlacklistedModules[0]); i++)
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
	for (int i = 0; i < sizeof(BlacklistedModulesW) / sizeof(BlacklistedModulesW[0]); i++)
	{
		if (wcsstr(lpLibFileName, BlacklistedModulesW[i]))
		{
			Report::SendReport(BLACKLISTED_DLL_DETECTED);
			break;
		}
	}

	return oLoadLibraryW(lpLibFileName);
}

//Do not use if your game/app uses this function
SHORT Hooks::hkGetAsyncKeyState(int vKey)
{
	if (!Thread::checkReturnAddr(GetCurrentThread()))
		Report::SendReport(MANUALMAPPED_MODULE);

	if (vKey == VK_INSERT || vKey == VK_F3)
		Report::SendReport(ODD_BAHAVIOR);

	return oGetAsyncKeyState(vKey);
}

//You can play around with this and spoof certain information, really not necessary though since you are already inside the protected process
NTSTATUS Hooks::hkNtQueryInformationProcess(HANDLE ProcessHandle, PROCESS_INFORMATION_CLASS InfoClass, PVOID ProcessInformation, ULONG ProcessInfoLength, PULONG ReturnLength)
{
	return oNtQueryInformationProcess(ProcessHandle, InfoClass, ProcessInformation, ProcessInfoLength, ReturnLength);
}