#include <Windows.h>


namespace Memory
{
	BOOL isBlacklistedModuleFound();
	BOOL ScanForExecutablePages();
	void* patternScan(const char* pattern, const char* mask);
}