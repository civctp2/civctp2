










 

#pragma once
#ifndef __WORKWINDOW_H__
#define __WORKWINDOW_H__


#include "c3window.h"


class WorkWindow : public C3Window 
{
public:
	WorkWindow(
		AUI_ERRCODE *retval,
		uint32 id,
		MBCHAR *ldlBlock,
		sint32 bpp,
		AUI_WINDOW_TYPE type = AUI_WINDOW_TYPE_STANDARD );
	WorkWindow(
		AUI_ERRCODE *retval,
		uint32 id,
		sint32 x,
		sint32 y,
		sint32 width,
		sint32 height,
		sint32 bpp,
		MBCHAR *pattern,
		AUI_WINDOW_TYPE type = AUI_WINDOW_TYPE_STANDARD );

	virtual ~WorkWindow() {}

	virtual AUI_ERRCODE InitCommon(void);



	virtual AUI_ERRCODE DrawThis(
		aui_Surface *surface = NULL,
		sint32 x = 0,
		sint32 y = 0 );

protected:
	virtual MouseEventCallback MouseMoveInside;
	virtual MouseEventCallback MouseMoveAway;

};


#endif 
