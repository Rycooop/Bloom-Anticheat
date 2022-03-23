#include "includes.h"


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

	if (ControlCode == IO_STARTUPREQUEST)
	{

	}

	Irp->IoStatus.Status = Status;
	Irp->IoStatus.Information = BytesIo;

	IoCompleteRequest(Irp, IO_NO_INCREMENT);
	return STATUS_SUCCESS;
}