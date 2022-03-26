#include "includes.h"


//Anticheat dll file path here
std::string dllPath;
BOOL injectDLL = FALSE;


BOOL InjectDLL(DWORD procID)
{
	if (!injectDLL)
		return TRUE;

	//Path of anticheat_x64.dll, not relative
	dllPath = "full\\dll\\path\\here";

	//If the given dll path is not valid
	if (!PathFileExistsA(dllPath.c_str()))
	{
		if (!Handler::TroubleshootError(DLL_INVALID_PATH))
		{
			Handler::ExitWithError(DLL_INVALID_PATH);
		}
	}

	//Obtain handle and perform a simple LoadLibraryA injection
	HANDLE hProc = OpenProcess(PROCESS_ALL_ACCESS, FALSE, procID);

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
	if (hThread == INVALID_HANDLE_VALUE)
		return FALSE;

	if (hThread)
		CloseHandle(hThread);
	if (hProc)
		CloseHandle(hProc);

	std::cout << "[+] Anticheat DLL injected" << std::endl;
	return TRUE;
}

BOOL InjectLSASS()
{

}

BOOL InjectCSRSS()
{

}