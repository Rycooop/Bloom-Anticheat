#include <Windows.h>


namespace Debugger
{
	//The continuous thread that will check our detection techniques and try and spot a debugger
	void DebuggerThread();

	//Debugger detections as well as the exception handler
	BOOL ReadPEBForDebugger();
	BOOL SpawnedUnderDebugger();
	BOOL CheckDebugString();
	LONG HandleExceptions(PEXCEPTION_POINTERS exceptionInfo);
}