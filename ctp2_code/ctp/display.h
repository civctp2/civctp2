//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ header
// Description  : Handles interface to display device
// Id           : $Id$
//
//----------------------------------------------------------------------------
//
// Disclaimer
//
// THIS FILE IS NOT GENERATED OR SUPPORTED BY ACTIVISION.
//
// This material has been developed at apolyton.net by the Apolyton CtP2 
// Source Code Project. Contact the authors at ctp2source@apolyton.net.
//
//----------------------------------------------------------------------------
//
// Compiler flags
// WIN32
// - Defined when compiling under Win32
//
//----------------------------------------------------------------------------
//
// Modifications from the original Activision code:
//
// - Changed the name of one field of the DisplayDevice struct to help gcc
//   compile it
//
//----------------------------------------------------------------------------

#if defined(HAVE_PRAGMA_ONCE)
#pragma once
#endif

#ifndef __DISPLAY_H__
#define __DISPLAY_H__

#if defined(WIN32)
#include <windows.h>
#else
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
