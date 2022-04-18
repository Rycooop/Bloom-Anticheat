#include "includes.h"


PIMAGE_DOS_HEADER DllDosHeader;
PIMAGE_NT_HEADERS DllNtHeader;

//Walk through the Import Address table of a given module and compare all function addresses to the actual address.
//Since we will use GetProcAddress to manually resolve the address, it is important that GetProcAddress itself is
//not hooked so we will check for this first
//
// @param base of the module whos IAT will be checked
// @return was an IAT hook found
//
BOOL IAT::ScanForIATHook(PVOID ImageBase)
{
	if (!DllDosHeader || !DllNtHeader)
	{
		DllDosHeader = (PIMAGE_DOS_HEADER)ImageBase;
		DllNtHeader = (PIMAGE_NT_HEADERS)((ULONG_PTR)ImageBase + DllDosHeader->e_lfanew);
	}

	IMAGE_DATA_DIRECTORY DataDirectory = DllNtHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT];
	PIMAGE_IMPORT_DESCRIPTOR pImportDescriptor = (PIMAGE_IMPORT_DESCRIPTOR)((ULONG_PTR)ImageBase + DataDirectory.VirtualAddress);

	LPCSTR libraryName = NULL;
	PIMAGE_IMPORT_BY_NAME functionName = NULL;

	if (!pImportDescriptor)
		return NULL;
	
	while (pImportDescriptor->Name != NULL)
	{
		libraryName = (LPCSTR)((ULONG_PTR)ImageBase + pImportDescriptor->Name);

		if (!GetModuleHandleA(libraryName))
		{
			pImportDescriptor++;
			continue;
		}

		PIMAGE_THUNK_DATA FirstThunk, OriginalThunk;
		FirstThunk = (PIMAGE_THUNK_DATA)((ULONG_PTR)ImageBase + pImportDescriptor->FirstThunk);
		OriginalThunk = (PIMAGE_THUNK_DATA)((ULONG_PTR)ImageBase + pImportDescriptor->OriginalFirstThunk);

		while (OriginalThunk->u1.AddressOfData != NULL)
		{
			functionName = (PIMAGE_IMPORT_BY_NAME)((ULONG_PTR)ImageBase + OriginalThunk->u1.AddressOfData);

			if (IAT::ManuallyResolveImports(functionName->Name, libraryName) != (PVOID)FirstThunk->u1.Function)
				return TRUE;

			++OriginalThunk;
			++FirstThunk;
		}

		++pImportDescriptor;
	}

	return FALSE;
}


//This will check to make sure the address of GetProcAddress in the IAT is inside of KERNEL32.dll. If someone were to
//IAT hook GetProcAddress they could redirect all functions resolved manually to their own implementation which would
//defeat the purpose of checking the IAT in the first place
//
// @param base of the module to check IAT's GetProcAddress function
// @return does the GetProcAddress import point to the proper location
//
BOOL IAT::IsGetProcAddressValid(PVOID ImageBase)
{
	if (!DllDosHeader || !DllNtHeader)
	{
		DllDosHeader = (PIMAGE_DOS_HEADER)ImageBase;
		DllNtHeader = (PIMAGE_NT_HEADERS)((ULONG_PTR)ImageBase + DllDosHeader->e_lfanew);
	}

	IMAGE_DATA_DIRECTORY DataDirectory = DllNtHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT];
	PIMAGE_IMPORT_DESCRIPTOR pImportDescriptor = (PIMAGE_IMPORT_DESCRIPTOR)((ULONG_PTR)ImageBase + DataDirectory.VirtualAddress);

	LPCSTR libraryName = NULL;
	PIMAGE_IMPORT_BY_NAME functionName = NULL;

	if (!pImportDescriptor)
		return NULL;

	while (pImportDescriptor->Name != NULL)
	{
		libraryName = (LPCSTR)((ULONG_PTR)ImageBase + pImportDescriptor->Name);

		if (strcmp(libraryName, "KERNEL32.dll"))
		{
			pImportDescriptor++;
			continue;
		}

		PIMAGE_THUNK_DATA FirstThunk, OriginalThunk;
		FirstThunk = (PIMAGE_THUNK_DATA)((ULONG_PTR)ImageBase + pImportDescriptor->FirstThunk);
		OriginalThunk = (PIMAGE_THUNK_DATA)((ULONG_PTR)ImageBase + pImportDescriptor->OriginalFirstThunk);

		while (OriginalThunk->u1.AddressOfData != NULL)
		{
			functionName = (PIMAGE_IMPORT_BY_NAME)((ULONG_PTR)ImageBase + OriginalThunk->u1.AddressOfData);

			if (strcmp(functionName->Name, "GetProcAddress") == 0)
			{
				PVOID checkAddr = (PVOID)FirstThunk->u1.Function;
				PVOID kernel32Base = GetModuleHandle(L"KERNEL32.dll");
				if (!kernel32Base)
					return FALSE;

				MODULEINFO mInfo;
				if (GetModuleInformation(GetCurrentProcess(), (HMODULE)kernel32Base, &mInfo, sizeof(MODULEINFO)))
				{
					if (checkAddr >= kernel32Base && (ULONG_PTR)checkAddr <= (ULONG_PTR)kernel32Base + mInfo.SizeOfImage)
						return TRUE;
					else return FALSE;
				}
				else return FALSE;
			}

			++OriginalThunk;
			++FirstThunk;
		}

		++pImportDescriptor;
	}

	return FALSE;
}


//Manually get the address of a function using GetProcAddress, first checking to make sure GetProcAddress is legit and not IAT hooked
//
// @param function name to resolve
// @param library which the function is located
// @return address of the function requested
//
PVOID IAT::ManuallyResolveImports(LPCSTR FuncName, LPCSTR libraryName)
{
	PVOID currModBase = GetModuleHandle(L"AnticheatDLL.dll");
	if (!currModBase)
		return NULL;

	static HMODULE hKernel32dll = GetModuleHandle(L"KERNEL32.dll");
	static HMODULE hUser32dll = GetModuleHandle(L"user32.dll");

	HMODULE currLibrary = GetModuleHandleA(libraryName);
	if (!currLibrary)
		return NULL;

	if (!IAT::IsGetProcAddressValid(currModBase))
		return FALSE;

	return GetProcAddress(currLibrary, FuncName);
}


//We wont use this but in case we wanted to monitor function calls from a specific module this would be the ideal way to do it

BOOL IAT::IatHook(PVOID ImageBase, LPCSTR RoutineName, PVOID hkFunc)
{

}


//============================================================================================================