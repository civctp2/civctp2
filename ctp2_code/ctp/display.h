#ifndef __DISPLAY_H__
#define __DISPLAY_H__

#ifndef WIN32
#include "windows.h"
#endif

struct DisplayDevice {
	LPSTR		szName;
	LPSTR		szDevice;
	GUID*		lpGUID;
	GUID	DisplayGUID;
	HMONITOR	hMon;
	RECT		rect;
};

struct CTPDisplayMode {
	sint32		width;
	sint32		height;
};

BOOL CALLBACK		display_FindDeviceCallbackEx(GUID* lpGUID, LPSTR szName,
								   LPSTR szDevice, LPVOID lParam, HMONITOR hMonitor);
BOOL				display_EnumerateDisplayDevices(void);


HRESULT CALLBACK	display_DisplayModeCallback(LPDDSURFACEDESC pdds, LPVOID lParam);
void				display_EnumerateDisplayModes(void);
BOOL				display_IsLegalResolution(sint32 width, sint32 height);

BOOL				display_InitWindow(HINSTANCE hinst, int cmdshow);


int					display_Initialize(HINSTANCE hInstance, int iCmdShow);

void display_Cleanup();

#endif
