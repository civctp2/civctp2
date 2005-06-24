#ifdef HAVE_PRAGMA_ONCE
#pragma once
#endif
#ifndef __C3LISTBOX_H__
#define __C3LISTBOX_H__


#include "aui_listbox.h"
#include "patternbase.h"


class aui_Surface;



class C3ListBox : public aui_ListBox, public PatternBase
{
public:
	
	C3ListBox(
		AUI_ERRCODE *retval,
		uint32 id,
		MBCHAR *ldlBlock,
		ControlActionCallback *ActionFunc = NULL,
		void *cookie = NULL );
	C3ListBox(
		AUI_ERRCODE *retval,
		uint32 id,
		sint32 x,
		sint32 y,
		sint32 width,
		sint32 height,
		MBCHAR *pattern,
		ControlActionCallback *ActionFunc = NULL,
		void *cookie = NULL );
	virtual ~C3ListBox() {}

protected:
	C3ListBox() : aui_ListBox() {}
	AUI_ERRCODE CreateRangers( void );

public:
	virtual AUI_ERRCODE DrawThis(
		aui_Surface *surface = NULL,
		sint32 x = 0,
		sint32 y = 0 );
};


#endif 
