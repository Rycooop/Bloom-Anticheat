#ifndef _DRIVER_INCLUDES
#define _DRIVER_INCLUDES

#include <ntifs.h>
#include <ntddk.h>
#include <ntdef.h>

#endif

//Define all IOCTL codes here
#define IO_STARTUPREQUEST CTL_CODE(FILE_DEVICE_UNKNOWN, 0x1003, METHOD_BUFFERED, FILE_SPECIAL_ACCESS)
#define IO_PROTECTEDPROCESSINFO CTL_CODE(FILE_DEVICE_UNKNOWN, 0x1004, METHOD_BUFFERED, FILE_SPECIAL_ACCESS)


//Protected Process information
extern ULONG ProtectedProcessIDs[2];
extern PEPROCESS ProtectedProcesses[2];

//Major functions
NTSTATUS CreateCall(PDEVICE_OBJECT pDeviceObject, PIRP Irp);
NTSTATUS CloseCall(PDEVICE_OBJECT pDeviceObject, PIRP Irp);
NTSTATUS IoControl(PDEVICE_OBJECT pDeviceObject, PIRP Irp);


//Communication Structure with usermode
typedef struct _KERNEL_REQUEST
{
	ULONG ProcessIDs[2];
	ULONG Buffer;
}KERNEL_REQUEST, *PKERNEL_REQUEST;