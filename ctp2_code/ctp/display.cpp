#include "c3.h"

#include "display.h"

#include "pointerlist.h"
#include "appstrings.h"

#define COMPILE_MULTIMON_STUBS
#ifdef __AUI_USE_DIRECTX__
#include <multimon.h>
#elif defined(__AUI_USE_SDL__)
#include <SDL.h>
#endif

PointerList<CTPDisplayMode>	*g_displayModes = NULL;
#ifdef WIN32
PointerList<DisplayDevice>	*g_displayDevices = NULL;

DisplayDevice				g_displayDevice;
#endif

extern LPCSTR				gszMainWindowClass;
extern LPCSTR				gszMainWindowName;
extern HINSTANCE			gHInstance;
extern HWND					gHwnd;
extern LRESULT CALLBACK		WndProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam);
extern sint32				g_ScreenWidth;
extern sint32				g_ScreenHeight;
extern BOOL					g_exclusiveMode;
extern BOOL					g_createDirectDrawOnSecondary;

extern BOOL g_SDL_flags;

#include "profileDB.h"
extern ProfileDB			*g_theProfileDB;

#ifdef WIN32
BOOL CALLBACK display_FindDeviceCallbackEx(GUID* lpGUID, LPSTR szName,
								   LPSTR szDevice, LPVOID lParam, HMONITOR hMonitor)
{


	DisplayDevice *p = new DisplayDevice;

	if (lpGUID)
	{
		p->DisplayGUID = *lpGUID;
		p->szName = szName;
		p->lpGUID = &p->DisplayGUID;
		p->szDevice = szDevice;
	}
	else
	{
		p->lpGUID = NULL;
	}

	p->hMon = hMonitor;

	g_displayDevices->AddTail(p);

	return TRUE;
}

BOOL display_EnumerateDisplayDevices(void)
{
	HRESULT hres;
	HMODULE hModule;
	LPDIRECTDRAWENUMERATEEX pfnEnum;

	g_displayDevices = new PointerList<DisplayDevice>;

	memset(&g_displayDevice, 0, sizeof(DisplayDevice));

	hModule = GetModuleHandle("ddraw.dll");
	pfnEnum = (LPDIRECTDRAWENUMERATEEX)GetProcAddress(hModule, "DirectDrawEnumerateExA");

	if (pfnEnum != NULL)
	{


		hres = (*pfnEnum)(	display_FindDeviceCallbackEx,
							NULL,
							DDENUM_ATTACHEDSECONDARYDEVICES);
	}
	else
	{

		Assert(FALSE);
	}

	return TRUE;
}

HRESULT CALLBACK display_DisplayModeCallback(LPDDSURFACEDESC pdds, LPVOID lParam)
{
    sint32 width  = pdds->dwWidth;
    sint32 height = pdds->dwHeight;
    sint32 bpp    = pdds->ddpfPixelFormat.dwRGBBitCount;

	BOOL	legalSize = TRUE;

	if (width < 640 || height < 480)
		legalSize = FALSE;

	if (legalSize) {
		if (width < 1024 || height < 768) {
			if (!(width == 800 && height == 600))
                            legalSize = FALSE;
		}
	}

	if (bpp == 16 && legalSize) {
		CTPDisplayMode *mode = new CTPDisplayMode;
		mode->width = width;
		mode->height = height;
    		g_displayModes->AddTail(mode);
	}

    return S_FALSE;
}
#endif

void display_EnumerateDisplayModes(void)
{
#ifdef __AUI_USE_DIRECTX__
	HRESULT				hr;
	LPDIRECTDRAW		dd;

	hr = DirectDrawCreate(g_displayDevice.lpGUID, &dd, NULL);
	Assert(hr == DD_OK);
	if (hr != DD_OK) {
		c3errors_FatalDialog(appstrings_GetString(APPSTR_DIRECTX),
								appstrings_GetString(APPSTR_REINSTALLDIRECTX));
		return;
	}
#else

	SDL_Rect **modes = SDL_ListModes(NULL, g_SDL_flags);

        if(modes == (SDL_Rect **)0){
            printf("No modes available!\n");
            return;
            }
	if(modes == (SDL_Rect **)-1){
            printf("All resolutions available.\n");
            }
        else{
            /* Print valid modes */
            printf("Available Modes\n");
            for(int i=0;modes[i];++i)
                printf("  %d x %d\n", modes[i]->w, modes[i]->h);
            }

	g_displayModes = new PointerList<CTPDisplayMode>;

	if (0 == modes) {
		return;
	} else if ((SDL_Rect **) -1 == modes) {
		// Fallback if SDL reports us to support anything,
		// we'll pick 800 x 600 and 1024 x 768
		const SDL_VideoInfo *info = SDL_GetVideoInfo();
		if (0 == info) {
			return;
		}
		if (0 == info->vfmt) {
			return;
		}
		if (16 < info->vfmt->BitsPerPixel) {
			return;
		}
		CTPDisplayMode *mode;
		mode = new CTPDisplayMode;
		if  (!mode)
			return;
		mode->width  = 800;
		mode->height = 600;
		g_displayModes->AddTail(mode);
		mode = new CTPDisplayMode;
		if (!mode)
			return;
		mode->width  = 1024;
		mode->height = 768;
		g_displayModes->AddTail(mode);
	} else {
		for (int i = 0; modes[i]; i++) {
			// We might get modes multiple times for each bpp
			// supported. Thus, check if we got it already.
			if (!display_IsLegalResolution(modes[i]->w,
			                               modes[i]->h)) {
				CTPDisplayMode *mode = new CTPDisplayMode;
				if (!mode)
					return;
				mode->width = modes[i]->w;
				mode->height = modes[i]->h;
				g_displayModes->AddTail(mode);
			}
		}
	}
#endif

#ifdef __AUI_USE_DIRECTX__
	hr = dd->EnumDisplayModes(0, NULL, NULL, display_DisplayModeCallback);

	if (g_displayModes->GetCount() < 1) {
		c3errors_FatalDialog(appstrings_GetString(APPSTR_DIRECTX),
							appstrings_GetString(APPSTR_NO16BIT));

		return;
	}

	dd->Release();
#endif
}


BOOL display_IsLegalResolution(sint32 width, sint32 height)
{

	PointerList<CTPDisplayMode>::PointerListNode	*node;

	node = g_displayModes->GetHeadNode();
	while (node) {
		CTPDisplayMode *mode = node->GetObj();
		if (mode->width == width && mode->height == height) {

			return TRUE;
		}
		node = node->GetNext();
	}
	return FALSE;
}

#ifdef __AUI_USE_DIRECTX__
BOOL display_InitWindow( HINSTANCE hinst, int cmdshow )
{
	WNDCLASS wc;

	gHInstance = hinst;

	wc.style = CS_DBLCLKS;
	wc.lpfnWndProc = WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hinst;
	wc.hIcon = LoadIcon(hinst, MAKEINTATOM(IDI_APPLICATION));
	wc.hCursor = NULL;
	wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = gszMainWindowClass;

	if (!RegisterClass(&wc)) return FALSE;

	DWORD	exStyle;

	if (g_exclusiveMode)
		exStyle = WS_EX_TOPMOST;
	else
		exStyle = WS_EX_APPWINDOW;




	HWND hwnd = FindWindow(gszMainWindowClass, gszMainWindowName);
	if(hwnd) {

		if (IsIconic(hwnd)) {
			ShowWindow(hwnd, SW_RESTORE);
		}
		SetForegroundWindow (hwnd);

		exit(0);
	}

	gHwnd = CreateWindowEx(
		exStyle,
		gszMainWindowClass,
		gszMainWindowName,
		WS_POPUP | WS_VISIBLE,
		0,
		0,
		g_ScreenWidth,
		g_ScreenHeight,
		NULL,
		NULL,
		hinst,
		NULL);

	Assert(gHwnd != NULL);
	if (gHwnd == NULL) return FALSE;

    SetFocus(gHwnd);
	ShowWindow(gHwnd, cmdshow);
    UpdateWindow(gHwnd);

	return TRUE;
}
#endif

int display_Initialize(HINSTANCE hInstance, int iCmdShow)
{
#ifdef __AUI_USE_DIRECTX__
	display_EnumerateDisplayDevices();

	DisplayDevice	*device = g_displayDevices->GetTail();

	if (g_createDirectDrawOnSecondary && device != NULL) {

		g_displayDevice = *device;

		MONITORINFO	*monInfo = new MONITORINFO;

		monInfo->cbSize = sizeof(MONITORINFO);
		GetMonitorInfo(device->hMon, monInfo);

		g_ScreenWidth = monInfo->rcMonitor.right - monInfo->rcMonitor.left;
		g_ScreenHeight = monInfo->rcMonitor.bottom - monInfo->rcMonitor.top;

		g_displayDevice.rect = monInfo->rcMonitor;

		delete monInfo;
	} else {

		g_ScreenWidth = GetSystemMetrics(SM_CXSCREEN);
		g_ScreenHeight = GetSystemMetrics(SM_CYSCREEN);

		SetRect(&g_displayDevice.rect, 0, 0, g_ScreenWidth, g_ScreenHeight);
	}
#endif
	display_EnumerateDisplayModes();

        BOOL foundRes = FALSE;

	if (g_theProfileDB->IsTryWindowsResolution()) {
		if (display_IsLegalResolution(g_ScreenWidth, g_ScreenHeight))
			foundRes = TRUE;
	}


	if (!foundRes) {
		if (display_IsLegalResolution(g_theProfileDB->GetScreenResWidth(),
									g_theProfileDB->GetScreenResHeight())) {
			g_ScreenWidth = g_theProfileDB->GetScreenResWidth();
			g_ScreenHeight = g_theProfileDB->GetScreenResHeight();
		} else {
			CTPDisplayMode *mode = g_displayModes->GetHead();
			if (mode != NULL) {
                            g_ScreenWidth = mode->width;
                            g_ScreenHeight = mode->height;
 			} else {
                            g_ScreenWidth = 800;
                            g_ScreenHeight = 600;
			}
		}
	}


	if (g_createDirectDrawOnSecondary) {
		g_ScreenWidth = 800;
		g_ScreenHeight = 600;
	}

#ifdef __AUI_USE_DIRECTX__
	display_InitWindow(hInstance, iCmdShow);
#endif

	return 0;
}


void display_Cleanup()
{
#ifdef __AUI_USE_DIRECTX__
	if(g_displayDevices) {
		g_displayDevices->DeleteAll();
		delete g_displayDevices;
		g_displayDevices = NULL;
	}
#endif
	if(g_displayModes) {
		g_displayModes->DeleteAll();
		delete g_displayModes;
		g_displayModes = NULL;
	}
}
