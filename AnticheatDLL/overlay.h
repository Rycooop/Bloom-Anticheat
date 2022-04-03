#include <Windows.h>

#ifndef _OVERLAY_DETECTION
#define _OVERLAY_DETECTION

//Nvidia should be whitelisted since its always on top and transparent
#define NVIDIAOVERLAY	"NVIDIA Geforce Experience"

//We will look for WS_EX_TOPMOST, WS_EX_LAYERED, and WS_EX_TRANSPARENT in the windows then check the dimensions

#endif // !_OVERLAY_DETECTION


//Store information on our processes main window so we can compare other external windows
extern HWND ProcessMainWindow;
extern char windowName[255];
extern int windowWidth, windowHeight, windowX, windowY;

namespace Overlay
{
	void OverlayThread();
	BOOL IsSuspiciousWindow(HWND hwnd);
	void GetMainWindowInfo(HWND mainWindow);
	BOOL CALLBACK EnumerateWindows(HWND hwnd, LPARAM lParam);
	DWORD GatherMoreWindowInfo(HWND hwnd, PRECT winDimensions);
}

enum SuspiciousWindow
{
	WINDOW_SUSPICIOUS = 1,
	WINDOW_OK
};