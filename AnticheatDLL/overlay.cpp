#include "includes.h"


//Detecting overlays is crucial in preventing external cheats relying on ReadProcessMemory and WriteProcessMemory. By enumerating all
//windows currently running, we can check the size of them and compare to the size of our application window. If a window has the same
//Size as our main process window and is not a well known module, we can report this for further analysis

//Any window with WS_EX_TOPMOST, WS_EX_TRANSPARENT, or WS_EX_LAYERED should be looked at more in detail since these are a basic need to
//have a transparent external overlay. After we confirm this, we simply compare the size and location on the screen to our window

HWND ProcessMainWindow = NULL;
char windowName[255];
int windowWidth, windowHeight, windowX, windowY;

const char* blacklistedWindowNames[] = { "CheatOverlay", "ESP window", "ESP"};

//Constantly enumerate all windows on the system and look for suspicious ones

void Overlay::OverlayThread()
{
	while (true)
	{
		EnumWindows(Overlay::EnumerateWindows, NULL);

		for (const char* currWindow : blacklistedWindowNames)
		{
			if (FindWindowA(NULL, currWindow))
				Report::SendReport(OVERLAY_DETECTED);
		}

		Sleep(5000);
	}
}

//Our function that will recieve the windows callback and decide what to do with the HWND
//
BOOL CALLBACK Overlay::EnumerateWindows(HWND hwnd, LPARAM lParam)
{
	DWORD procId = 0;
	GetWindowThreadProcessId(hwnd, &procId);

	//If its our window from our process we need to update its data
	if (procId == GetCurrentProcessId())
	{
		Overlay::GetMainWindowInfo(hwnd);
		return TRUE;
	}

	//If the window is suspicious we will stop the enumeration
	if (Overlay::IsSuspiciousWindow(hwnd))
		Report::SendReport(OVERLAY_DETECTED);

	return TRUE;
}
 

//============================================================================================================


//Checks the extended window attributes as well as the size and location of the window to decide if it should report
//
// @param handle to the window to check
// @return is the window suspicious
//
BOOL Overlay::IsSuspiciousWindow(HWND hwnd)
{
	if (!ProcessMainWindow)
	{
		return FALSE;
	}

	//Make sure its not our window
	DWORD procId;
	GetWindowThreadProcessId(hwnd, &procId);
	if (procId == GetCurrentProcessId())
		return FALSE;

	char currWindowName[255];
	GetWindowTextA(hwnd, currWindowName, 255);

	//If the current window is the NVIDIA Geforce overlay we will return FALSE since its always running transparent and full screen
	if (strcmp(currWindowName, "NVIDIA GeForce Overlay") == 0)
		return FALSE;

	RECT rect;
	GetWindowRect(hwnd, &rect);
	int currWidth = rect.right - rect.left;
	int currHeight = rect.bottom - rect.top;
	int currX = rect.left;
	int currY = rect.top;

	DWORD currExtendedStyle = GetWindowLong(hwnd, GWL_EXSTYLE);

	if ((currExtendedStyle & WS_EX_TOPMOST) != 0 || ((currExtendedStyle & WS_EX_TRANSPARENT) != 0 && (currExtendedStyle & WS_EX_LAYERED) != 0))
	{
		if ((currWidth == 1920 && currHeight == 1080) || (currWidth == GetSystemMetrics(SM_CXSCREEN) && currHeight == GetSystemMetrics(SM_CYSCREEN)))
		{
			if (Overlay::GatherMoreWindowInfo(hwnd, &rect) == WINDOW_SUSPICIOUS)
				return TRUE;
			else return FALSE;
		}
		if (currWidth == windowWidth && currHeight == windowHeight)
			return TRUE;
	}

	return FALSE;
}

//Constantly update the information regarding our processes main window just so that the data being compared in the IsSuspiciousWindow function
//is up to date and not returning false positives
//
// @param handle to the main protected process window
//
void Overlay::GetMainWindowInfo(HWND mainWindow)
{
	if (!mainWindow)
		return;

	ProcessMainWindow = mainWindow;

	RECT rect;
	GetWindowRect(ProcessMainWindow, &rect);
	if (rect.right == 0 || rect.bottom == 0)
		return;

	windowWidth = rect.right - rect.left;
	windowHeight = rect.bottom - rect.top;
	windowX = rect.left;
	windowY = rect.top;

	if (!GetWindowTextA(mainWindow, windowName, 255))
		return;
}


//This function will check to see if the owner of the window is a blacklisted process. It will open a handle, grab
//basic window info, and then perform a search to see if any blacklisted executables match the owner
// 
// @param handle to the window to check
// @param pointer to a RECT of the window dimensions
// @return status of what the function found
//
DWORD Overlay::GatherMoreWindowInfo(HWND hwnd, PRECT winDimensions)
{
	int currWidth = winDimensions->right - winDimensions->left;
	int currHeight = winDimensions->bottom - winDimensions->top;

	char windowName[255];
	GetWindowTextA(hwnd, windowName, sizeof(windowName));

	for (const char* currBlacklistedWindow : blacklistedWindowNames)
	{
		if (strcmp(windowName, currBlacklistedWindow) == 0)
			return WINDOW_SUSPICIOUS;
	}

	DWORD procID;
	GetWindowThreadProcessId(hwnd, &procID);

	HANDLE hProc = OpenProcess(PROCESS_ALL_ACCESS, FALSE, procID);
	if (hProc == INVALID_HANDLE_VALUE)
		return WINDOW_SUSPICIOUS;
	
	wchar_t ProcessName[255];
	if (GetModuleFileNameExW(hProc, NULL, ProcessName, sizeof(ProcessName)))
	{
		for (const wchar_t* currProcess : BlacklistedModulesW)
		{
			if (currProcess == NULL || !wcsstr(currProcess, L".exe"))
				continue;

			if (wcsstr(ProcessName, currProcess))
			{
				return WINDOW_SUSPICIOUS;
			}
		}
	}

	CloseHandle(hProc);
	return WINDOW_OK;
}