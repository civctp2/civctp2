












#pragma once
#ifndef __C3THUMB_H__
#define __C3THUMB_H__


#include "aui_thumb.h"
#include "patternbase.h"
#include "aui_soundbase.h"


class C3Thumb : public aui_Thumb, public PatternBase
{
public:
	
	C3Thumb(
		AUI_ERRCODE *retval,
		uint32 id,
		MBCHAR *ldlBlock,
		ControlActionCallback *ActionFunc = NULL,
		void *cookie = NULL );
	C3Thumb(
		AUI_ERRCODE *retval,
		uint32 id,
		sint32 x,
		sint32 y,
		sint32 width,
		sint32 height,
		MBCHAR *pattern,
		ControlActionCallback *ActionFunc = NULL,
		void *cookie = NULL );
	virtual ~C3Thumb() {}

	virtual AUI_ERRCODE DrawThis(
		aui_Surface *surface = NULL,
		sint32 x = 0,
		sint32 y = 0 );

	AUI_ERRCODE	PlaySound( AUI_SOUNDBASE_SOUND sound );
};


#endif 
