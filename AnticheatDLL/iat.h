#include <windows.h>

#ifndef _IAT_HEADER
#define _IAT_HEADER


#endif


namespace IAT
{
	BOOL ScanForIATHook(PVOID ImageBase);
	BOOL IsGetProcAddressValid(PVOID ImageBase);
	PVOID ManuallyResolveImports(LPCSTR FuncName, LPCSTR libraryName);
	BOOL IatHook(PVOID ImageBase, LPCSTR RoutineName, PVOID hkFunc);
}