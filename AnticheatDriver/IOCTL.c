#include "includes.h"


ULONG ProtectedProcessIDs[2];
PEPROCESS ProtectedProcesses[2];

//All of our device major functions to deal with our IOCTL Requests will be put in here. Create call and close call are used per every call, while
//IoControl is what decides what to do with the given code. In this function we will handle all IOCTL codes

NTSTATUS CreateCall(PDEVICE_OBJECT pDeviceObject, PIRP Irp)
{
	UNREFERENCED_PARAMETER(pDeviceObject);
	UNREFERENCED_PARAMETER(Irp);

	Irp->IoStatus.Status = STATUS_SUCCESS;
	Irp->IoStatus.Information = 0;

	IoCompleteRequest(Irp, IO_NO_INCREMENT);
	return STATUS_SUCCESS;
}

NTSTATUS CloseCall(PDEVICE_OBJECT pDeviceObject, PIRP Irp)
{
	UNREFERENCED_PARAMETER(pDeviceObject);
	UNREFERENCED_PARAMETER(Irp);

	Irp->IoStatus.Status = STATUS_SUCCESS;
	Irp->IoStatus.Information = 0;

	IoCompleteRequest(Irp, IO_NO_INCREMENT);
	return STATUS_SUCCESS;
}

NTSTATUS IoControl(PDEVICE_OBJECT pDeviceObject, PIRP Irp)
{
	UNREFERENCED_PARAMETER(pDeviceObject);
	UNREFERENCED_PARAMETER(Irp);

	NTSTATUS Status = STATUS_UNSUCCESSFUL;
	ULONG BytesIo = 0;

	PIO_STACK_LOCATION Stack = IoGetCurrentIrpStackLocation(Irp);
	ULONG ControlCode = Stack->Parameters.DeviceIoControl.IoControlCode;

	//Handle our IOCTL codes here
	if (ControlCode == IO_STARTUPREQUEST)
	{
		PKERNEL_REQUEST Request = (PKERNEL_REQUEST)Irp->AssociatedIrp.SystemBuffer;

		//Send the number 4 to the buffer so the usermode application knows the driver is operation normally
		Request->Buffer = 4;

		Status = STATUS_SUCCESS;
		BytesIo = sizeof(KERNEL_REQUEST);
	}
	else if (ControlCode == IO_PROTECTEDPROCESSINFO)
	{
		PKERNEL_REQUEST Request = (PKERNEL_REQUEST)Irp->AssociatedIrp.SystemBuffer;
		if (NT_SUCCESS(PsLookupProcessByProcessId((HANDLE)Request->ProcessIDs[0], &ProtectedProcesses[0])) && NT_SUCCESS(PsLookupProcessByProcessId((HANDLE)Request->ProcessIDs[1], &ProtectedProcesses[1])))
		{
			//If we are updating the processes to protect we dont want to create a new callback
			if (!ObRegistrationHandle)
				RegisterObCallbacks();
	
			Request->Buffer = 1;
		}
		else Request->Buffer = 0;

		Status = STATUS_SUCCESS;
		BytesIo = sizeof(KERNEL_REQUEST);
	}

	Irp->IoStatus.Status = Status;
	Irp->IoStatus.Information = BytesIo;

	IoCompleteRequest(Irp, IO_NO_INCREMENT);
	return STATUS_SUCCESS;
}