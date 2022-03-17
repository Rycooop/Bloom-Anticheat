#include <Windows.h>


namespace Debugger
{
	//The thread to catch a debugger
	void DebuggerThread();

	//Debugger detections
	BOOL ReadPEBForDebugger();
	BOOL SpawnedUnderDebugger();
	BOOL CheckDebugString();
	LONG HandleExceptions(PEXCEPTION_POINTERS exceptionInfo);
}