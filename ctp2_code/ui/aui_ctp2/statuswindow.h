










 

#pragma once
#ifndef __STATUSWINDOW_H__
#define __STATUSWINDOW_H__


#include "c3window.h"



class StatusWindow : public C3Window 
{
public:
	StatusWindow(
		AUI_ERRCODE *retval,
		uint32 id,
		sint32 x,
		sint32 y,
		sint32 width,
		sint32 height,
		sint32 bpp,
		MBCHAR *pattern )
		:
		C3Window( retval, id, x, y, width, height, bpp, pattern, AUI_WINDOW_TYPE_SINKING )
		{}
	virtual ~StatusWindow() {}

	virtual AUI_ERRCODE DrawThis(
		aui_Surface *surface = NULL,
		sint32 x = 0,
		sint32 y = 0 );

#ifdef _PLAYTEST
	AUI_ERRCODE DrawCommand(char* str);
#endif
};


#endif 
