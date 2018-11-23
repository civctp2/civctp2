#include "c3.h"
#include "aui.h"
#include "CivPaths.h"

#include "c3ui.h"
#include "directvideo.h"
#include "pattern.h"
#include "c3window.h"
#include "videowindow.h"
#include "textbutton.h"

#include "videoutils.h"

#define k_VIDEO_WINDOW_ID			20000
#define k_ID_VIDEOWINDOW_CLOSE_BOX	20001

C3Window				*g_videoWindow;
extern CivPaths			*g_civPaths;
extern C3UI				*g_c3ui;

void videoutils_VideoWindowCloseBox(aui_Control *control, uint32 action, uint32 data, void *cookie);

void videoutils_Initialize(void)
{
	g_videoWindow = NULL;
}

sint32 videoutils_PlayVideoInWindow(MBCHAR *name, MBCHAR *pattern)
{
	AUI_ERRCODE		errcode;

	VideoWindow *vidWin = new VideoWindow(&errcode, k_VIDEO_WINDOW_ID, 30, 30, 32, 32, 16, pattern, name, TRUE);

	sint32 controlWidth = 16;
	sint32 controlHeight = 16;
	sint32 controlX = vidWin->Width() - controlWidth - 2;
	sint32 controlY = 2;

	TextButton *button= new TextButton(
		&errcode,
		k_ID_VIDEOWINDOW_CLOSE_BOX,
		controlX, controlY, controlWidth, controlHeight,
		pattern,
		"",
		videoutils_VideoWindowCloseBox );
	if ( !button ) return -3;

	errcode = vidWin->AddControl( button );
	Assert( errcode == AUI_ERRCODE_OK );
	if ( errcode != AUI_ERRCODE_OK ) return -4;

	errcode = g_c3ui->AddWindow(vidWin);

	return 0;
}

void videoutils_Cleanup(void)
{
	if (g_videoWindow != NULL) {
		g_c3ui->RemoveWindow(k_VIDEO_WINDOW_ID);
		delete g_videoWindow;
	}
	g_videoWindow = NULL;
}

void videoutils_VideoWindowCloseBox(aui_Control *control, uint32 action, uint32 data, void *cookie)
{
	aui_Window *window = control->GetParentWindow();
	if (window != NULL) {
		g_c3ui->RemoveWindow(window->Id());

	}
}
