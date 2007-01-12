












#pragma once
#ifndef __C3DROPDOWN_H__
#define __C3DROPDOWN_H__


#include "aui_dropdown.h"
#include "patternbase.h"


class aui_Surface;



class C3DropDown : public aui_DropDown, public PatternBase
{
public:
	
	C3DropDown(
		AUI_ERRCODE *retval,
		uint32 id,
		sint32 x,
		sint32 y,
		sint32 width,
		sint32 height,
		MBCHAR *pattern,
		sint32 buttonSize = k_CONTROL_DEFAULT_SIZE,
		sint32 windowSize = 0,
		ControlActionCallback *ActionFunc = NULL,
		void *cookie = NULL );
	C3DropDown(
		AUI_ERRCODE *retval,
		uint32 id,
		MBCHAR *ldlBlock,
		ControlActionCallback *ActionFunc = NULL,
		void *cookie = NULL);

	virtual ~C3DropDown() {}

protected:
	C3DropDown() : aui_DropDown() {}
	AUI_ERRCODE CreateComponents();

public:
	virtual AUI_ERRCODE DrawThis(
		aui_Surface *surface = NULL,
		sint32 x = 0,
		sint32 y = 0 );

protected:
	virtual AUI_ERRCODE	RepositionButton( void );
};


#endif 
