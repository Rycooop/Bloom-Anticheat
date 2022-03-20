#include "includes.h"


//The hooks on this page will consist of a trampoline to restore the functionality of the original functions
//As long as you pass through an appropriate length for the hook, the below functions will take care of any BYTE restoring
// and redirects. This gives us total control flow over subsequent functions.

//There are many other functions that may be beneficial to hook, but im going to keep it basic in this example. It is CRUCIAL
//that inside of our integrity check thread we are constantly making sure our hook is in place and the address is pointing
//to our module, to make sure an attacker did not overwrite our hook to jump to their own version of the function


//Declare the function pointers as well as our functions we want to hook, and our blacklisted modules

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


//=============================================================================================================


//Place all of our hooks and redirect them to our version of the function. The only thing here that you will need to change
//Will be the length of the hook. Make sure you do not cut off the middle of an instruction or the process will crash

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


	//

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


//Check the ASCI version of LoadLibrary to scan for blacklisted modules trying to load

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


//Check the WIDE version of LoadLibrary to scan for blacklisted modules trying to load

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


//Many cheats use GetAsyncKeyState to handle input since it is easy and they dont want to hijack the windows message handler. This can easily
//catch cheaters. Dont use this if your game uses this function as it will throw a false positive. 

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