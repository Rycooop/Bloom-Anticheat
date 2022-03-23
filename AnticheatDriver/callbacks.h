#ifndef _DRIVER_INCLUDES
#define _DRIVER_INCLUDES

#include <ntifs.h>
#include <ntddk.h>
#include <ntdef.h>

#endif


WCHAR* blacklistedModules[20];

PLOAD_IMAGE_NOTIFY_ROUTINE ImageLoadCallback(PUNICODE_STRING FullImageName, HANDLE ProcessId, PIMAGE_INFO ImageInfo);
