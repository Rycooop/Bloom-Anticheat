#pragma warning(disable : 4024 4047)

#ifndef _DRIVER_INCLUDES
#define _DRIVER_INCLUDES

#include <ntifs.h>
#include <ntddk.h>
#include <ntdef.h>

#endif

#include "callbacks.h"
#include "IOCTL.h"


//Our main IOCTL Device for communication with usermode
extern PDEVICE_OBJECT DeviceObject;
