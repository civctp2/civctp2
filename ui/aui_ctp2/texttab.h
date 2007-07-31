












#pragma once
#ifndef __TEXTTAB_H__
#define __TEXTTAB_H__


#include "aui_tab.h"
#include "patternbase.h"



class TextTab : public aui_Tab, public PatternBase
{
public:
	
	TextTab(
		AUI_ERRCODE *retval,
		uint32 id,
		MBCHAR *ldlBlock,
		ControlActionCallback *ActionFunc = NULL,
		void *cookie = NULL);
	TextTab(
		AUI_ERRCODE *retval,
		uint32 id,
		sint32 x,
		sint32 y,
		sint32 width,
		sint32 height,
		sint32 paneWidth,
		sint32 paneHeight,
		MBCHAR *pattern = NULL,
		MBCHAR *text = NULL,
		ControlActionCallback *ActionFunc = NULL,
		void *cookie = NULL,
		BOOL selected = FALSE );
	virtual ~TextTab() {}

protected:
	TextTab() : aui_Tab() {}

public:
	virtual AUI_ERRCODE DrawThis(
		aui_Surface *surface = NULL,
		sint32 x = 0,
		sint32 y = 0 );
};


#endif 
