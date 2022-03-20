#include <Windows.h>


namespace Memory
{
	//Continous loop to execute the below functions and loop for manually mapped modules or blacklisted modules
	void ScanMemory();

	BOOL isBlacklistedModuleFound();
	BOOL ScanForExecutablePages();
	void* patternScan(const char* pattern, const char* mask);
	bool isValidModuleAddr(uintptr_t address);
}