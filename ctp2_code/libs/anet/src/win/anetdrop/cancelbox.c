/*
Copyright (C) 1995-2001 Activision, Inc.

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

*/

/*--------------------------------------------------------------------------
  A program that implements the modeless dialog box
--------------------------------------------------------------------------*/
#include <string.h>
#include <windows.h>

#include "cancelbox.h"

#define GetScreenHeight() GetSystemMetrics(SM_CYFULLSCREEN)
#define GetScreenWidth() GetSystemMetrics(SM_CXFULLSCREEN)

#define CANCELBOX_WIDTH 200
#define CANCELBUTTON_HEIGHT 20
#define CANCELBUTTON_WIDTH 80
#define CANCELBOX_MARGIN 10
#define MY_CMD_CANCEL 1

/*--------------------
   Global Variable
--------------------*/
static HANDLE hCancel;
static HWND	hWnd = NULL;
static BOOL CancelClicked = FALSE;

/*--------------------------------------------------------------------------
  Window procdure
--------------------------------------------------------------------------*/
long FAR PASCAL cancelbox_WndProc(HWND hWnd, UINT wMessage, WPARAM wParam, LONG lParam)
{
	switch(wMessage) {
	case WM_COMMAND:
		switch(wParam) {
		case MY_CMD_CANCEL:
			CancelClicked = TRUE;
			break;
		}
		break;
	case WM_CLOSE:
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, wMessage, wParam, lParam);
	}
	return (0L);
}

/*--------------------------------------------------------------------------
 create a modeless dialog box with a single cancel button.
 msg is the text to display in the dialog box
 The caption is used only if it is passed into CreateWindows.
--------------------------------------------------------------------------*/
void cancelbox_set(const char *caption, const char *msg)
{

	static HANDLE hStaticText;

	if (!hWnd) {
		HDC hdc;
		HINSTANCE hInstance = GetModuleHandle(NULL);
		SIZE sizeText, sizeWin;
		int temp;
		int titleBarHeight;

		WNDCLASS wndclass;
		wndclass.style 	= CS_HREDRAW | CS_VREDRAW;
		wndclass.lpfnWndProc = cancelbox_WndProc;
		wndclass.cbClsExtra = 0;
		wndclass.cbWndExtra = 0;
		wndclass.hInstance = hInstance;
		wndclass.hIcon = NULL;
		wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
		wndclass.hbrBackground = (HBRUSH)GetStockObject(LTGRAY_BRUSH);
		wndclass.lpszMenuName = NULL;
		wndclass.lpszClassName = "MyClass";

		if (!RegisterClass (&wndclass))
			return;
		hWnd = CreateWindow("MyClass", caption, WS_OVERLAPPED,
			0,0, 10,10,
			NULL, NULL, hInstance, NULL);

		hdc = GetDC(hWnd);
		GetTextExtentPoint32(hdc, msg, strlen(msg), &sizeText);
		ReleaseDC(hWnd, hdc);

		titleBarHeight = GetSystemMetrics(SM_CYSIZE);
		sizeWin.cy = CANCELBOX_MARGIN * 3 + sizeText.cy + titleBarHeight + CANCELBUTTON_HEIGHT;

		MoveWindow(hWnd, GetScreenWidth()/2-CANCELBOX_WIDTH/2,
			GetScreenHeight()/2-sizeWin.cy/2,
			CANCELBOX_WIDTH, sizeWin.cy, FALSE);

		hStaticText = CreateWindow("STATIC", msg, WS_CHILD|SS_LEFT,
			CANCELBOX_WIDTH/2 - sizeText.cx/2,
			CANCELBOX_MARGIN, /*sizeWin.cy/2 - sizeText.cy/2-CANCELBUTTON_HEIGHT - 2 * CANCELBOX_MARGIN,*/
			sizeText.cx, sizeText.cy,
			hWnd,NULL, hInstance, NULL);
		temp = sizeText.cy + CANCELBOX_MARGIN *2;

		hCancel = CreateWindow("BUTTON", "Cancel",
			WS_CHILD | BS_DEFPUSHBUTTON,
			CANCELBOX_WIDTH/2 - CANCELBUTTON_WIDTH/2 ,
			temp,
			CANCELBUTTON_WIDTH,CANCELBUTTON_HEIGHT,
			hWnd, (HMENU)MY_CMD_CANCEL, hInstance, NULL);
	} else {
		SetWindowText(hStaticText, msg);
	}
	ShowWindow(hCancel, SW_SHOWNORMAL);
	ShowWindow(hStaticText, SW_SHOWNORMAL);
	ShowWindow(hWnd, SW_SHOWNORMAL);

	CancelClicked = FALSE;
}

/*--------------------------------------------------------------------------
 Hide a cancelbox
--------------------------------------------------------------------------*/
void cancelbox_clear()
{
	ShowWindow(hWnd, SW_HIDE);
}

/*--------------------------------------------------------------------------
 call this often while the cancelbox is on the screen.
 It returns TRUE if the user has clicked on Cancel.
--------------------------------------------------------------------------*/
BOOL cancelbox_poll(void)
{
	MSG msg;
	if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		DispatchMessage(&msg);
	return CancelClicked;
}

#if 0
/*--------------------------------------------------------------------------
 Unit test
--------------------------------------------------------------------------*/
int WINAPI WinMain(HINSTANCE hinstExe, HINSTANCE hinstExePrev, LPSTR lpszCmdLine
		, int nCmdShow)
{
	int i;

	cancelbox_set("Hello.. This is only a test");
	for (i = 0; i < 500; i++)  {
		if (cancelbox_poll())
			break;
		Sleep(10);
	}
	/*cancelbox_clear();*/
	cancelbox_set("This is test2");
	for (i = 0; i < 500; i++)  {
		if (cancelbox_poll())
			break;
		Sleep(10);
	}
	cancelbox_clear();
	return 0;
}

#endif
