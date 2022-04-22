#include <Windows.h>
#include <iostream>


//The main error handling system
namespace Handler
{
	BOOL TroubleshootError(DWORD errorNum);
	void ExitWithError(DWORD errorNum);
}

//Enum of all possible errors the Anticheat may encounter, used to troubleshoot
enum ERRORS
{
	PROCESS_NOT_RUNNING = 0,
	CANT_ESCALATE_PRIV,
	DRIVER_CONNECTION_ERROR,
	PROCESS_INVALID_PROCESSID,
	PROCESS_INVALID_HANDLE,
	PROCESS_FAULTY_START,
	ANTICHEAT_FAULT_START,
	DLL_INVALID_PATH,
	DLL_CANT_LOAD,
	DLL_INVALID_LOAD,
	DRIVER_BAD_PATH,
	DRIVER_INVALID_LOAD,
	DRIVER_NO_COMMUNICATION,
	DRIVER_CANT_PROTECT,
	DRIVER_TOO_MANY_VIOLATIONS,
};