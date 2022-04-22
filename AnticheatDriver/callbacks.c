#include "includes.h"


//Driver manual mappers set the name of the driver to nothing, which in turn will bypass MmUnloadedDrivers. The 
//rest of these are known vulnerable drivers that someone may be abusing, this list could be much longer
WCHAR* blacklistedModules[20] = { L"iqvw64e.sys", L"AsUpIO64.sys", L"BS_Flash64.sys", L"Phymemx64.sys", L"kdmapper.exe"};

//Define variables and structures we will need for ObRegisterCallbacks()
OB_OPERATION_REGISTRATION OperationRegistration;
OB_CALLBACK_REGISTRATION CallbackRegistration;
PVOID ObRegistrationHandle;


//One of the greatest things about having a driver is the abillity to register callbacks ranging from dealing with Non-maskable interupts, to
//image loading of any type. The main functionality of our driver will be held within our callbacks. We will call ObRegisterCallbacks on our
//protected process, as well as our AntiCheat process to prevent anyone in usermode from tampering with them. We will also check for any abused
//drivers being loaded on the system while the driver is running :)

//Setup the callback registration structs and call ObRegisterCallbacks. Here we will define a callback for Process handles but
//you can create another one to monitor thread(this may be added in the future)

NTSTATUS RegisterObCallbacks()
{
	RtlSecureZeroMemory(&OperationRegistration, sizeof(OB_OPERATION_REGISTRATION));
	RtlSecureZeroMemory(&CallbackRegistration, sizeof(OB_CALLBACK_REGISTRATION));

	UNICODE_STRING callbackAltitude;
	RtlInitUnicodeString(&callbackAltitude, L"1986");

	OperationRegistration.ObjectType = PsProcessType;
	OperationRegistration.Operations |= OB_OPERATION_HANDLE_CREATE;
	OperationRegistration.Operations |= OB_OPERATION_HANDLE_DUPLICATE;
	OperationRegistration.PreOperation = PreOperationCallback;
	OperationRegistration.PostOperation = NULL;

	CallbackRegistration.Version = OB_FLT_REGISTRATION_VERSION;
	CallbackRegistration.Altitude = callbackAltitude;
	CallbackRegistration.OperationRegistrationCount = 1;
	CallbackRegistration.RegistrationContext = NULL;
	CallbackRegistration.OperationRegistration = &OperationRegistration;

	return ObRegisterCallbacks(&CallbackRegistration, &ObRegistrationHandle);
}


//Our callback function that will be called everytime something tries to create a handle. We will filter this out to only monitor the processes
//we care about and want to protect

OB_PREOP_CALLBACK_STATUS PreOperationCallback(PVOID RegistrationContext, POB_PRE_OPERATION_INFORMATION OpInfo)
{ 
	UNREFERENCED_PARAMETER(RegistrationContext);
	ASSERT(OpInfo->CallContext == NULL);

	ACCESS_MASK BitsToClear = 0;
	ACCESS_MASK BitsToSet = 0;
	PACCESS_MASK DesiredAccess = NULL;

	if (OpInfo->ObjectType == *PsProcessType)
	{
		if (ProtectedProcesses[0] == NULL || ( ProtectedProcesses[0] != NULL && OpInfo->Object != ProtectedProcesses[0]) && (ProtectedProcesses[1] != NULL && OpInfo->Object != ProtectedProcesses[1]))
			return OB_PREOP_SUCCESS;
		
		if (OpInfo->Object == PsGetCurrentProcess || PsGetCurrentProcess() == ProtectedProcesses[0] || PsGetCurrentProcess() == ProtectedProcesses[1])
		{
			return OB_PREOP_SUCCESS;
		}

		//Clear out PROCESS_ALL_ACCESS as well as PROCESS_VM_OPERATION
		BitsToClear = PROCESS_ALL_ACCESS | 0x8;
		BitsToSet = DELETE;
	}
	else goto Exit;

	switch (OpInfo->Operation)
	{
		case OB_OPERATION_HANDLE_CREATE:
		{
			DesiredAccess = &OpInfo->Parameters->CreateHandleInformation.DesiredAccess;
			break;
		}
		case OB_OPERATION_HANDLE_DUPLICATE:
		{
			DesiredAccess = &OpInfo->Parameters->DuplicateHandleInformation.DesiredAccess;
			break;
		}
		default:
			ASSERT(FALSE);
			break;
	}

	//IF the handle is being created in the kernel, ignore it, otherwise strip it of PROCESS_ALL_ACCESS
	if (OpInfo->KernelHandle != 1)
	{
		*DesiredAccess &= ~BitsToClear;
		*DesiredAccess |= BitsToSet;
	}

Exit:
	return OB_PREOP_SUCCESS;
}


//=======================================================================================================================================


//This callback will be executed on any image load. We can see if any blacklisted modules are loaded such as commonly abused drivers

PLOAD_IMAGE_NOTIFY_ROUTINE ImageLoadCallback(PUNICODE_STRING FullImageName, HANDLE ProcessId, PIMAGE_INFO ImageInfo)
{
	UNREFERENCED_PARAMETER(FullImageName);
	UNREFERENCED_PARAMETER(ProcessId);
	UNREFERENCED_PARAMETER(ImageInfo);

	//If our anticheat is loaded again(should be handled in usermode but just in case)
	if (wcsstr(FullImageName->Buffer, L"AntiCheat.exe"))
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
		if (blacklistedModules[i] == NULL)
			continue;

		if (wcsstr(FullImageName->Buffer, blacklistedModules[i]))
		{
			DbgPrintEx(0, 0, "[Bloom Anticheat] Vulnerable driver or blacklisted module loaded %ls\n", FullImageName->Buffer);
		}
	}
	
	return STATUS_SUCCESS;
}

PCREATE_PROCESS_NOTIFY_ROUTINE ProcessCreateCallback(HANDLE ParentID, HANDLE ProcessID, BOOLEAN Create)
{
	UNREFERENCED_PARAMETER(ParentID);
	UNREFERENCED_PARAMETER(ProcessID);
	UNREFERENCED_PARAMETER(Create);

	return STATUS_SUCCESS;
}