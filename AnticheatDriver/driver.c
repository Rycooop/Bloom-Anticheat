#include "includes.h"


UNICODE_STRING Dev, Dos;
PDEVICE_OBJECT DeviceObject;

NTSTATUS UnloadDriver(PDRIVER_OBJECT pDriverObject);
HANDLE hThread;


NTSTATUS DriverEntry(PDRIVER_OBJECT pDriverObject, PUNICODE_STRING pRegistryPath)
{
	UNREFERENCED_PARAMETER(pDriverObject);
	UNREFERENCED_PARAMETER(pRegistryPath);

	RtlInitUnicodeString(&Dev, L"\\Device\\acDriverDevice");
	RtlInitUnicodeString(&Dos, L"\\DosDevices\\acDriverDevice");

	IoCreateDevice(pDriverObject, 0, &Dev, FILE_DEVICE_UNKNOWN, FILE_DEVICE_SECURE_OPEN, FALSE, &DeviceObject);
	IoCreateSymbolicLink(&Dos, &Dev);

	pDriverObject->MajorFunction[IRP_MJ_CREATE] = CreateCall;
	pDriverObject->MajorFunction[IRP_MJ_CLOSE] = CloseCall;
	pDriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL] = IoControl;
	pDriverObject->DriverUnload = UnloadDriver;
	
	if (DeviceObject)
	{
		DeviceObject->Flags |= DO_DIRECT_IO;
		DeviceObject->Flags &= ~DO_DEVICE_INITIALIZING;
	}

	//PsCreateSystemThread(&hThread, THREAD_ALL_ACCESS, NULL, NULL, NULL, (PKSTART_ROUTINE)kPrint, DeviceObject);

	PsSetLoadImageNotifyRoutine((PLOAD_IMAGE_NOTIFY_ROUTINE)ImageLoadCallback);
	PsSetCreateProcessNotifyRoutine((PCREATE_PROCESS_NOTIFY_ROUTINE)ProcessCreateCallback, FALSE);
	
	DbgPrintEx(0, 0, "[Bloom Anticheat] Driver Initialized\n");

	return STATUS_SUCCESS;
}

NTSTATUS UnloadDriver(PDRIVER_OBJECT pDriverObject)
{
	UNREFERENCED_PARAMETER(pDriverObject);

	IoDeleteDevice(DeviceObject);
	IoDeleteSymbolicLink(&Dos);

	if (ObRegistrationHandle)
		ObUnRegisterCallbacks(ObRegistrationHandle);

	PsRemoveLoadImageNotifyRoutine((PLOAD_IMAGE_NOTIFY_ROUTINE)ImageLoadCallback);
	PsSetCreateProcessNotifyRoutine((PCREATE_PROCESS_NOTIFY_ROUTINE)ProcessCreateCallback, TRUE);

	return STATUS_SUCCESS;
}