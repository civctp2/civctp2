#ifdef HAVE_PRAGMA_ONCE
#pragma once
#endif
#ifndef __VIDEOWINDOW_H__
#define __VIDEOWINDOW_H__

#include "c3window.h"


class VideoWindow : public C3Window
{
public:
	VideoWindow(
		AUI_ERRCODE *retval,
		uint32 id,
		sint32 x,
		sint32 y,
		sint32 width,
		sint32 height,
		sint32 bpp,
		const MBCHAR *pattern,
		const MBCHAR *name,
		BOOL modal,
		AUI_WINDOW_TYPE type = AUI_WINDOW_TYPE_FLOATING );
	virtual ~VideoWindow();

protected:
	VideoWindow() : C3Window() {};
	AUI_ERRCODE InitCommon( void );
	AUI_ERRCODE CreateVideoSurface(const MBCHAR *name, BOOL modal);

public:
	virtual AUI_ERRCODE Idle( void );
	virtual AUI_ERRCODE DrawThis(aui_Surface *surface = NULL, sint32 x = 0, sint32 y = 0 );

private:
#ifdef __AUI_USE_DIRECTX__
	DirectVideo		*m_video;
#endif
	MBCHAR			m_filename[_MAX_PATH];
	BOOL			m_modal;
};

#endif
