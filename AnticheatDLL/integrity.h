#include <Windows.h>

#ifndef _MODULE_INTEGRITY
#define _MODULE_INTEGRITY



#endif


namespace Integrity
{
	VOID IntegrityThread();
	BOOL AreHooksInPlace();
	BOOL AreFunctionsSpoofed();
}