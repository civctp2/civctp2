












#pragma once
#ifndef __CONTROLSHEET_H__
#define __CONTROLSHEET_H__

#include "aui_control.h"
#include "patternbase.h"

class ControlSheet : public aui_Control, public PatternBase
{
public:

	
	ControlSheet(
		AUI_ERRCODE *retval,
		uint32 id,
		MBCHAR *ldlBlock,
		ControlActionCallback *ActionFunc = NULL,
		void *cookie = NULL );
	ControlSheet(
		AUI_ERRCODE *retval,
		uint32 id,
		sint32 x,
		sint32 y,
		sint32 width,
		sint32 height,
		MBCHAR *pattern,
		ControlActionCallback *ActionFunc = NULL,
		void *cookie = NULL );
	virtual ~ControlSheet();

	virtual AUI_ERRCODE DrawThis(aui_Surface *surface = NULL,
								sint32 x = 0,
								sint32 y = 0);
	
	void SetBorder( sint32 border ) { m_border = border; }

private:
	sint32 m_border;	
};


#endif 
