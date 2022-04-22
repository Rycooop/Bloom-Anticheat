#include "includes.h"


//Anticheat dll file path here
std::string dllPath;
BOOL injectDLL = TRUE;


//Using a handle that was created before ObRegisterCallbacks began stripping, we will inject our DLL into the target process, which
//will handle its own integrity checks

BOOL InjectDLL(HANDLE hProc)
{
	if (!injectDLL)
		return TRUE;

	CHAR pathBuff[MAX_PATH] = { 0 };
	GetCurrentDirectoryA(MAX_PATH, pathBuff);

	//Path of anticheat_x64.dll, not relative
	dllPath = pathBuff;
	dllPath += "\\anticheatDLL.dll";

	//If the given dll path is not valid
	if (!PathFileExistsA(dllPath.c_str()))
	{
		if (!Handler::TroubleshootError(DLL_INVALID_PATH))
		{
			Handler::ExitWithError(DLL_INVALID_PATH);
		}
	}

	if (hProc == INVALID_HANDLE_VALUE)
		return FALSE;

	//Allocate memory for the dll path in the target process
	PVOID memAllocation = VirtualAllocEx(hProc, 0, MAX_PATH, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
	if (!memAllocation)
		return FALSE;

	//Write the dll path to the allocated memory
	if (!WriteProcessMemory(hProc, memAllocation, dllPath.c_str(), strlen(dllPath.c_str()) + 1, NULL))
		return FALSE;

	//Create a thread on LoadLibraryA and pass through the dllpath to load the new module
	HANDLE hThread = CreateRemoteThread(hProc, 0, 0, (LPTHREAD_START_ROUTINE)LoadLibraryA, memAllocation, 0, 0);
	if (hThread == INVALID_HANDLE_VALUE || hThread == 0)
		return FALSE;

	WaitForSingleObject(hThread, INFINITE);

	if (hThread)
		CloseHandle(hThread);

	std::cout << "[+] Anticheat DLL injected" << std::endl;
	return TRUE;
}

BOOL InjectLSASS()
{

}

BOOL InjectCSRSS()
{

}