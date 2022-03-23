#include <Windows.h>


//Function pointers to hooked functions
typedef HMODULE(APIENTRY* tHLoadLibraryA)(LPCSTR lpLibFileName);
typedef HMODULE(APIENTRY* tHLoadLibraryW)(LPCWSTR lpLibFileName);
typedef SHORT(APIENTRY* tHKeyAsyncKeyState)(int vKey);
typedef NTSTATUS(*tNtQueryInformationProcess)(HANDLE ProcessHandle, PROCESS_INFORMATION_CLASS InfoClass, PVOID ProcessInformation, ULONG ProcessInfoLength, PULONG ReturnLength);
typedef HWND(APIENTRY* tCreateWindowExA)(DWORD dwExStyle, LPCSTR lpClassName, LPCSTR lpWindowName, DWORD dwStyle, int x, int y, int width, int height, HWND nHwndParent, HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam);

//string arrays
extern const char* FuncsToHook[];
extern const char* BlacklistedModules[];
inline const wchar_t* BlacklistedModulesW[] = { L"cheat.dll", L"aimbot.dll", L"esp.dll", L"hack.dll" };

namespace Hooks
{
	//Functions that do our hooking
	BOOL InstallHooks();
	void* Hook(BYTE* src, BYTE* dst, size_t size);

	//Hooked functions
	HMODULE hkLoadLibraryA(LPCSTR lpLibFileName);
	HMODULE hkLoadLibraryW(LPCWSTR lpLibFileName);
	SHORT hkGetAsyncKeyState(int vKey);
	NTSTATUS hkNtQueryInformationProcess(HANDLE ProcessHandle, PROCESS_INFORMATION_CLASS InfoClass, PVOID ProcessInformation, ULONG ProcessInfoLength, PULONG ReturnLength);
	HWND hkCreateWindowExA(DWORD dwExStyle, LPCSTR lpClassName, LPCSTR lpWindowName, DWORD dwStyle, int x, int y, int width, int height, HWND nHwndParent, HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam);
}


