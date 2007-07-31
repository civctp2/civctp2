












#pragma once
#ifndef __C3_THUMB_H__
#define __C3_THUMB_H__


#include "aui_thumb.h"
#include "patternbase.h"
#include "aui_soundbase.h"


class c3_Thumb : public aui_Thumb, public PatternBase
{
public:
	
	c3_Thumb(
		AUI_ERRCODE *retval,
		uint32 id,
		MBCHAR *ldlBlock,
		ControlActionCallback *ActionFunc = NULL,
		void *cookie = NULL );
	c3_Thumb(
		AUI_ERRCODE *retval,
		uint32 id,
		sint32 x,
		sint32 y,
		sint32 width,
		sint32 height,
		MBCHAR *pattern,
		ControlActionCallback *ActionFunc = NULL,
		void *cookie = NULL );
	virtual ~c3_Thumb() {}

	virtual AUI_ERRCODE DrawThis(
		aui_Surface *surface = NULL,
		sint32 x = 0,
		sint32 y = 0 );

	AUI_ERRCODE	PlaySound( AUI_SOUNDBASE_SOUND sound );

};


#endif 
