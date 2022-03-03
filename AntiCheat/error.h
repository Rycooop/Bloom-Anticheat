#include <Windows.h>
#include <iostream>


enum ERRORS
{
	PROCESS_NOT_RUNNING = 0,
	PROCESS_INVALID_PROCESSID,
	PROCESS_INVALID_HANDLE,
	PROCESS_FAULTY_START,
	ANTICHEAT_FAULT_START,
	DLL_CANT_LOAD,
	DLL_INVALID_LOAD,
	DRIVER_INVALID_LOAD,
	DRIVER_NO_COMMUNICATION
};

namespace Handler
{
	BOOL TroubleshootError(DWORD errorNum);
	void ExitWithError(DWORD errorNum);
}