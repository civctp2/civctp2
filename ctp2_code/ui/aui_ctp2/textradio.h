












#pragma once
#ifndef __TEXTRADIO_H__
#define __TEXTRADIO_H__


#include "aui_radio.h"
#include "patternbase.h"



class TextRadio : public aui_Radio, public PatternBase
{
public:
	
	TextRadio(
		AUI_ERRCODE *retval,
		uint32 id,
		MBCHAR *ldlBlock,
		ControlActionCallback *ActionFunc = NULL,
		void *cookie = NULL);
	TextRadio(
		AUI_ERRCODE *retval,
		uint32 id,
		sint32 x,
		sint32 y,
		sint32 width,
		sint32 height,
		MBCHAR *pattern = NULL,
		MBCHAR *text = NULL,
		ControlActionCallback *ActionFunc = NULL,
		void *cookie = NULL,
		BOOL selected = FALSE );
	virtual ~TextRadio() {}

protected:
	TextRadio() : aui_Radio() {}

public:
	virtual AUI_ERRCODE DrawThis(
		aui_Surface *surface = NULL,
		sint32 x = 0,
		sint32 y = 0 );
};


#endif 
