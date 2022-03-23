#ifndef _DRIVER_INCLUDES
#define _DRIVER_INCLUDES

#include <ntifs.h>
#include <ntddk.h>
#include <ntdef.h>

#endif

#define IO_STARTUPREQUEST CTL_CODE(FILE_DEVICE_UNKNOWN, 0x1003, METHOD_BUFFERED, FILE_SPECIAL_ACCESS)


NTSTATUS CreateCall(PDEVICE_OBJECT pDeviceObject, PIRP Irp);
NTSTATUS CloseCall(PDEVICE_OBJECT pDeviceObject, PIRP Irp);
NTSTATUS IoControl(PDEVICE_OBJECT pDeviceObject, PIRP Irp);