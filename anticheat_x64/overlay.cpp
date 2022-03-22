#include "includes.h"


//Detecting overlays is crucial in preventing external cheats relying on ReadProcessMemory and WriteProcessMemory. By enumerating all
//windows currently running, we can check the size of them and compare to the size of our application window. If a window has the same
//Size as our main process window and is not a well known module, we can report this for further analysis

//Any window with WS_EX_TOPMOST, WS_EX_TRANSPARENT, or WS_EX_LAYERED should be looked at more in detail since these are a basic need to
//have a transparent external overlay. After we confirm this, we simply compare the size and location on the screen to our window


HWND ProcessMainWindow;
LPSTR windowName;
int windowWidth = 0, windowHeight = 0, windowX = 0, windowY = 0;

//Constantly enumerate all windows on the system and look for suspicious ones

void Overlay::OverlayThread()
{
	while (true)
	{
		EnumWindows(Overlay::EnumerateWindows, NULL);
		Sleep(5000);
	}
}


//Checks the extended window attributes as well as the size and location of the window to decide if it should report

BOOL Overlay::IsSuspiciousWindow(HWND hwnd)
{
	if (!ProcessMainWindow)
		return FALSE;

	//Make sure its not our window
	DWORD procId;
	GetWindowThreadProcessId(hwnd, &procId);
	if (procId == GetCurrentProcessId())
		return FALSE;

	char currWindowName[255];
	GetWindowTextA(hwnd, currWindowName, 255);

	//If the current window is the NVIDIA Geforce overlay we will return FALSE since its always running transparent and full screen
	if (strcmp(currWindowName, "NVIDIA Geforce Overlay") == 0)
		return FALSE;

	RECT rect;
	GetWindowRect(hwnd, &rect);
	int currWidth = rect.right - rect.left;
	int currHeight = rect.bottom - rect.top;
	int currX = rect.left;
	int currY = rect.top;

	DWORD currExtendedStyle = GetWindowLong(hwnd, GWL_EXSTYLE);

	if ((currExtendedStyle & WS_EX_TOPMOST) != 0 || (currExtendedStyle & WS_EX_TRANSPARENT) != 0 || (currExtendedStyle & WS_EX_LAYERED) != 0)
	{
		if (currWidth == windowWidth || currHeight == windowHeight)
			return TRUE;
		if (currX == windowX || currY == windowY)
			return TRUE;
	}

	return FALSE;
}


//Constantly update the information regarding our processes main window just so that the data being compared in the IsSuspiciousWindow function
//is up to date and not returning false positives

void Overlay::GetMainWindowInfo(HWND mainWindow)
{
	if (!mainWindow)
		return;

	RECT rect;
	GetWindowRect(mainWindow, &rect);
	windowWidth = rect.right - rect.left;
	windowHeight = rect.bottom - rect.top;
	windowX = rect.left;
	windowY = rect.top;

	if (!GetWindowTextA(mainWindow, windowName, 40))
		return;

	ProcessMainWindow = mainWindow;
}


//Our function that will recieve the windows callback and decide what to do with the HWND

BOOL Overlay::EnumerateWindows(HWND hwnd, LPARAM lParam)
{
	DWORD procId = 0;
	GetWindowThreadProcessId(hwnd, &procId);

	//If its our window from our process we need to update its data
	if (procId == GetCurrentProcessId())
	{
		Overlay::GetMainWindowInfo(ProcessMainWindow);
		return TRUE;
	}

	//If the window is suspicious we will stop the enumeration
	if (Overlay::IsSuspiciousWindow(hwnd))
		Report::SendReport(OVERLAY_DETECTED);

	return TRUE;
}