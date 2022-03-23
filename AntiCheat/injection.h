#include <Windows.h>
#include <iostream>


//information needed for the injector
extern std::string dllPath;

//All DLL related functions
BOOL InjectDLLs(DWORD ProtectedProcessID);
BOOL InjectDLL(DWORD procID);


//Both LSASS and CSRSS need handles to the game, meaning someone could inject into them and create a handle from within the process, then do there stuff. By
//Injecting into both of these, we can proxy the Read and Write process memory calls and prevent these types of calls to our protected process. Our driver will
//Handle all other processes VIA ObRegisterCallbacks().

BOOL InjectLSASS();
BOOL InjectCSRSS();