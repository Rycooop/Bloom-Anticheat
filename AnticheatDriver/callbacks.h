#ifndef _DRIVER_INCLUDES
#define _DRIVER_INCLUDES

#include <ntifs.h>
#include <ntddk.h>
#include <ntdef.h>
#include <fltKernel.h>

#endif


//Variables
WCHAR* blacklistedModules[20];
extern PVOID ObRegistrationHandle;

//ObRegisterCallback functions
NTSTATUS RegisterObCallbacks();
OB_PREOP_CALLBACK_STATUS PreOperationCallback(PVOID RegistrationContext, POB_PRE_OPERATION_INFORMATION OpInfo);


//Image load callback functions
PLOAD_IMAGE_NOTIFY_ROUTINE ImageLoadCallback(PUNICODE_STRING FullImageName, HANDLE ProcessId, PIMAGE_INFO ImageInfo);
PCREATE_PROCESS_NOTIFY_ROUTINE ProcessCreateCallback(HANDLE ParentID, HANDLE ProcessID, BOOLEAN Create);
