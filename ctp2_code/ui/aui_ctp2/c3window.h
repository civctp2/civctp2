#ifdef HAVE_PRAGMA_ONCE
#pragma once
#endif
#ifndef __PATTERNWINDOW_H__
#define __PATTERNWINDOW_H__

#include "aui_window.h"
#include "patternbase.h"




class C3Window : public aui_Window, public PatternBase
{
public:
	C3Window(
		AUI_ERRCODE *retval,
		uint32 id,
		MBCHAR *ldlBlock,
		sint32 bpp,
		AUI_WINDOW_TYPE type = AUI_WINDOW_TYPE_STANDARD, bool bevel = true );
	C3Window(
		AUI_ERRCODE *retval,
		uint32 id,
		sint32 x,
		sint32 y,
		sint32 width,
		sint32 height,
		sint32 bpp,
		MBCHAR *pattern,
		AUI_WINDOW_TYPE type = AUI_WINDOW_TYPE_STANDARD, bool bevel = true );
	virtual ~C3Window() {}

protected:
	C3Window() : aui_Window() {}
	AUI_ERRCODE InitCommon( void );

public:
	virtual AUI_ERRCODE DrawThis(
		aui_Surface *surface = NULL,
		sint32 x = 0,
		sint32 y = 0 );

private:
	bool m_bevel;
};

#endif
