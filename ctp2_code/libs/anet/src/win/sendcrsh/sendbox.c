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
 Small module for creating a send/cancel dialog box.
--------------------------------------------------------------------------*/

#include <windows.h>
#include "sendbox.h"
#include "sendboxrc.h"
#include "eclock.h"
#include "logprint.h"

static struct {
	HWND hwnd;
	HWND hComments;
	HWND hCRC;
	int commentLen;
	int buttonClicked;
	clock_t timeout;
} sendbox;

static BOOL CALLBACK FindChildren(HWND hChild, LPARAM lParam)
{
	LONG idChild = GetWindowLong(hChild, GWL_ID);
	if (idChild == IDD_TEXT) {
		sendbox.hComments = hChild;
	} else if (idChild == IDD_CRC) {
		sendbox.hCRC = hChild;
	}
	return TRUE;
}

static BOOL CALLBACK SendboxProc(HWND hDlg, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	switch (iMsg) {
	case WM_INITDIALOG:
		SetFocus(GetDlgItem(hDlg, IDOK));
		DPRINT(("SendboxProc: received WM_INITDIALOG, set focus to OK\n"));
		return FALSE;

	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case IDOK:
			sendbox.buttonClicked = sendbox_CLICKED_OK;
			DPRINT(("SendboxProc: user clicked OK button\n"));
			return TRUE;

		case IDCANCEL:
			sendbox.buttonClicked = sendbox_CLICKED_CANCEL;
			DPRINT(("SendboxProc: user clicked Cancel button\n"));
			return TRUE;
		}
		break;

	case WM_DESTROY:
		DPRINT(("SendboxProc: received WM_DESTROY\n"));
		sendbox.buttonClicked = sendbox_CLICKED_CANCEL;
		return TRUE;
	}
	return FALSE;
}

/*--------------------------------------------------------------------------
 Create and show the send dialog box.
 The CRC field will display signature.
 The comments box will accept commentLen characters.
 If timeout is non-zero, the dialog box will cancel automatically in
 timeout seconds if the user does not interact with it.
 Returns 0 on success, non-zero on failure.
--------------------------------------------------------------------------*/
int sendbox_create(int iCmdShow, char *signature, int commentLen, int timeout)
{
	HINSTANCE hInstance = GetModuleHandle(NULL);
	clock_t now = eclock();

	sendbox.hwnd = CreateDialog(hInstance, "Sendbox", NULL, SendboxProc);
	if (sendbox.hwnd == NULL)
		return 1;
	sendbox.hComments = NULL;
	EnumChildWindows(sendbox.hwnd, &FindChildren, 0);
	if (!sendbox.hComments || !sendbox.hCRC)
		return 2;
	if (signature)
		SetWindowText(sendbox.hCRC, signature);

	sendbox.commentLen = commentLen;
	sendbox.buttonClicked = sendbox_CLICKED_NONE;
	if (timeout > 0)
		sendbox.timeout = now + timeout * ECLOCKS_PER_SEC;
	else
		sendbox.timeout = 0;
	DPRINT(("sendbox_create: t:%d, created with show:%d maxcomment:%d timeout:%d eclocks, at t:%d\n", now, iCmdShow, sendbox.commentLen, timeout * ECLOCKS_PER_SEC, sendbox.timeout));
	ShowWindow(sendbox.hwnd, iCmdShow);
	UpdateWindow(sendbox.hwnd);
	return 0;
}

/*--------------------------------------------------------------------------
 Process any window events for the send dialog box.
 If OK is returned, up to commentLen characters of text in the comments box
 are returned in comment.
 Returns sendbox_CLICKED_NONE if no interesting events have occured,
         sendbox_CLICKED_CANCEL if the user clicked cancel,
		 sendbox_CLICKED_OK if the user clicked OK.
--------------------------------------------------------------------------*/
int sendbox_poll(char *comment)
{
	MSG msg;
	int bUserInput = 0;
	const int oldButtonClicked = sendbox.buttonClicked;
	clock_t now = eclock();

	while ((oldButtonClicked == sendbox.buttonClicked)
	   &&  PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
		if (!IsDialogMessage(sendbox.hwnd, &msg)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		if (sendbox.timeout
		&&  (msg.hwnd == sendbox.hComments)
		&&  (msg.message == WM_KEYDOWN)) {
			DPRINT(("sendbox_poll: t:%d, user keystroke cancelled timeout\n", now));
			sendbox.timeout = 0;
		}
	}
	if (sendbox.buttonClicked == sendbox_CLICKED_OK) {
		comment[0] = '\0';
		GetWindowText(sendbox.hComments, comment, sendbox.commentLen);
		comment[sendbox.commentLen-1] = '\0';
		DPRINT(("sendbox_poll: t:%d, got comment:%d\n", now, comment));
	}
	if (sendbox.timeout && (long)(now - sendbox.timeout) > 0) {
		DPRINT(("sendbox_poll: t:%d, timed out waiting for input\n", now));
		sendbox.buttonClicked = sendbox_CLICKED_CANCEL;
	}
	return sendbox.buttonClicked;
}

/*--------------------------------------------------------------------------
 Destroy the dialog box.
--------------------------------------------------------------------------*/
void sendbox_destroy()
{
	DestroyWindow(sendbox.hwnd);
	DPRINT(("sendbox_destroy\n"));
}

#if 0
#include <assert.h>
/* test code */
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                   PSTR szCmdLine, int iCmdShow)
{
	int clicked;
	char comments[32];
	HINSTANCE hTest = GetModuleHandle(NULL);
	assert(hInstance == hTest);

	if (sendbox_create(iCmdShow, 32)) {
		MessageBox(NULL, "sendbox_create failed!", "Failure", MB_ICONEXCLAMATION);
		return 1;
	}

	while ((clicked = sendbox_poll(comments)) == sendbox_CLICKED_NONE) {
		Sleep(100);
	}
	sendbox_destroy();
	if (clicked == sendbox_CLICKED_OK) {
		MessageBox(NULL, comments, "Success", MB_ICONEXCLAMATION);
	} else {
		MessageBox(NULL, "User cancelled", "Success", MB_ICONEXCLAMATION);
	}
	return 0;
}
#endif
