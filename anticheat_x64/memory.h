#include <Windows.h>


namespace Memory
{
	void ScanMemory();

	BOOL isBlacklistedModuleFound();
	BOOL ScanForExecutablePages();
	void* patternScan(const char* pattern, const char* mask);
	bool isValidModuleAddr(PVOID address);
}