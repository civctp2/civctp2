










 

#pragma once
#ifndef __TIPWINDOW_H__
#define __TIPWINDOW_H__


#include "c3window.h"
#include "aui_textbase.h"



class TipWindow : public C3Window, public aui_TextBase
{
public:
	TipWindow(
		AUI_ERRCODE *retval,
		uint32 id,
		MBCHAR *ldlBlock,
		MBCHAR *text,
		sint32 bpp);
	TipWindow(
		AUI_ERRCODE *retval,
		uint32 id,
		sint32 x,
		sint32 y,
		sint32 width,
		sint32 height,
		sint32 bpp,
		MBCHAR *pattern,
		MBCHAR *text = NULL );

	virtual ~TipWindow() {}

	virtual AUI_ERRCODE DrawThis(
		aui_Surface *surface = NULL,
		sint32 x = 0,
		sint32 y = 0 );

protected:
	AUI_ERRCODE FitWindowToText( void );
};


#endif 
