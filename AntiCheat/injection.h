#include <Windows.h>
#include <iostream>


//information needed for the injector
extern std::string dllPath;

//All DLL related functions
BOOL InjectDLL(DWORD procID);