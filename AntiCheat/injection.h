#include <Windows.h>
#include <iostream>

extern std::string dllPath;

BOOL InjectDLL(DWORD procID);