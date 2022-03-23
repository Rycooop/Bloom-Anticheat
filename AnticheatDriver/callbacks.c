#include "includes.h"


//kdmapper uses iqvw64e.sys, so if someone manually maps a driver while the anticheat is running it will be detected automatically
WCHAR* blacklistedModules[20] = { L"iqvw64e.sys" };


//==================================================================================================================================


PLOAD_IMAGE_NOTIFY_ROUTINE ImageLoadCallback(PUNICODE_STRING FullImageName, HANDLE ProcessId, PIMAGE_INFO ImageInfo)
{
	UNREFERENCED_PARAMETER(FullImageName);
	UNREFERENCED_PARAMETER(ProcessId);
	UNREFERENCED_PARAMETER(ImageInfo);

	//If our anticheat is loaded again(should be handled in usermode but just in case)
	if (wcsstr(FullImageName->Buffer, L"AntiCheat"))
	{
		PEPROCESS pEprocess;
		KAPC_STATE State;

		if (NT_SUCCESS(PsLookupProcessByProcessId(ProcessId, &pEprocess)))
		{
			KeStackAttachProcess(pEprocess, &State);
			ZwTerminateProcess(NULL, 0);
			KeUnstackDetachProcess(&State);
		}
	}

	//Loop through all our blacklisted modules and prevent loading
	for (int i = 0; i < sizeof(blacklistedModules) / sizeof(blacklistedModules[0]); i++)
	{
		if (wcsstr(FullImageName->Buffer, blacklistedModules[i]))
		{
			//Handle blacklisted module load here

		}
	}

	return STATUS_SUCCESS;
}