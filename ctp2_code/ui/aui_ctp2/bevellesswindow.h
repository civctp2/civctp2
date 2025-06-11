#ifdef HAVE_PRAGMA_ONCE
#pragma once
#endif
#ifndef __BEVELLESSWINDOW_H__
#define __BEVELLESSWINDOW_H__

#include "aui_window.h"
#include "patternbase.h"




class BevelLessWindow : public aui_Window, public PatternBase
{
public:
	BevelLessWindow(
		AUI_ERRCODE *retval,
		uint32 id,
		const MBCHAR *ldlBlock,
		sint32 bpp,
		AUI_WINDOW_TYPE type = AUI_WINDOW_TYPE_STANDARD );
	BevelLessWindow(
		AUI_ERRCODE *retval,
		uint32 id,
		sint32 x,
		sint32 y,
		sint32 width,
		sint32 height,
		sint32 bpp,
		const MBCHAR *pattern,
		AUI_WINDOW_TYPE type = AUI_WINDOW_TYPE_STANDARD );
	virtual ~BevelLessWindow() {}

protected:
	BevelLessWindow() : aui_Window() {}
	AUI_ERRCODE InitCommon( void );

public:
	virtual AUI_ERRCODE DrawThis(
		aui_Surface *surface = NULL,
		sint32 x = 0,
		sint32 y = 0 );
};

#endif
