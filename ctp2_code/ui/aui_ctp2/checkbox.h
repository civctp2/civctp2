












#pragma once
#ifndef __CHECKBOX_H__
#define __CHECKBOX_H__


#include "aui_switch.h"
#include "patternbase.h"



class Checkbox : public aui_Switch,  public PatternBase
{
public:
	
	Checkbox(
		AUI_ERRCODE *retval,
		uint32 id,
		MBCHAR *ldlBlock,
		ControlActionCallback *ActionFunc = NULL,
		void *cookie = NULL);
	Checkbox(
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
	virtual ~Checkbox() {}

protected:
	Checkbox() : aui_Switch() {}

public:
	virtual AUI_ERRCODE DrawThis(
		aui_Surface *surface = NULL,
		sint32 x = 0,
		sint32 y = 0 );
};


#endif 
