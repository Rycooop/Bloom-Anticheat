#pragma warning(disable : 4024 4047)

#ifndef _DRIVER_INCLUDES
#define _DRIVER_INCLUDES

#include <ntifs.h>
#include <ntddk.h>
#include <ntdef.h>
#include <fltKernel.h>

#endif

#include "callbacks.h"
#include "IOCTL.h"


//Our main IOCTL Device for communication with usermode
extern PDEVICE_OBJECT DeviceObject;


typedef struct _KERNEL_MODULE_INFO
{
	PVOID ModuleBase;
	ULONG ModuleSize;
	LPCWSTR ModuleName;
}KERNEL_MODULE_INFO, * PKERNEL_MODULE_INFO;