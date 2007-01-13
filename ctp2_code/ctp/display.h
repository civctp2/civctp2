#ifndef __DISPLAY_H__
#define __DISPLAY_H__

#ifndef WIN32
#include "windows.h"

struct DisplayDevice {
	LPSTR		szName;
	LPSTR		szDevice;
	GUID*		lpGUID;
	GUID	DisplayGUID;
	HMONITOR	hMon;
#endif
	RECT		rect;
};

struct CTPDisplayMode {
	sint32		width;
	sint32		height;
};

#ifdef WIN32
BOOL CALLBACK		display_FindDeviceCallbackEx(GUID* lpGUID, LPSTR szName,
								   LPSTR szDevice, LPVOID lParam, HMONITOR hMonitor);
#endif
BOOL				display_EnumerateDisplayDevices(void);

#ifdef WIN32
HRESULT CALLBACK	display_DisplayModeCallback(LPDDSURFACEDESC pdds, LPVOID lParam);
#endif
void				display_EnumerateDisplayModes(void);
BOOL				display_IsLegalResolution(sint32 width, sint32 height);

BOOL				display_InitWindow(HINSTANCE hinst, int cmdshow);


int					display_Initialize(HINSTANCE hInstance, int iCmdShow);

void display_Cleanup();

#endif
