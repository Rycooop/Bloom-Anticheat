#include <Windows.h>


//Function pointers to hooked functions
typedef HMODULE(APIENTRY* tHLoadLibraryA)(LPCSTR lpLibFileName);
typedef HMODULE(APIENTRY* tHLoadLibraryW)(LPCWSTR lpLibFileName);
typedef SHORT(APIENTRY* tHKeyAsyncKeyState)(int vKey);

//string arrays
extern const char* FuncsToHook[];
extern const char* BlacklistedModules[];
extern const wchar_t* BlacklistedModulesW[];

namespace Hooks
{
	//Functions that do our hooking
	BOOL InstallHooks();
	void* Hook(BYTE* src, BYTE* dst, size_t size);

	//Hooked functions
	HMODULE hkLoadLibraryA(LPCSTR lpLibFileName);
	HMODULE hkLoadLibraryW(LPCWSTR lpLibFileName);
	SHORT hkGetAsyncKeyState(int vKey);
}


