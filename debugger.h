#include <Windows.h>


namespace Debugger
{
	void DebuggerThread();
	BOOL IsDebuggerAttached();
	BOOL SpawnedUnderDebugger();
}