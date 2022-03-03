#include "includes.h"


BOOL Handler::TroubleshootError(DWORD errorNum)
{
	switch (errorNum)
	{
		case PROCESS_NOT_RUNNING:
		{
			
		}
		case PROCESS_INVALID_PROCESSID:
		{
			if (Util::isProcessRunning(PROTECTED_PROCESS))
			{
				DWORD procID = Util::getRunningProcessId(PROTECTED_PROCESS);

				if (procID)
					return TRUE;
				else return FALSE;
			}
			else return FALSE;
		}
		case PROCESS_INVALID_HANDLE:
		{
			if (OpenProcess(PROCESS_VM_READ, FALSE, Globals.processProcID) == INVALID_HANDLE_VALUE)
				return FALSE;
			else return TRUE;
		}
	}

	return FALSE;
}

void Handler::ExitWithError(DWORD errorNum)
{

}