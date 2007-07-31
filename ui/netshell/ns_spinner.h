

#pragma once

#ifndef __NS_SPINNER_H__
#define __NS_SPINNER_H__

#include "aui_ranger.h"
#include "patternbase.h"

class aui_Surface;


class ns_Spinner : public aui_Ranger, public PatternBase
{
public:
	
	ns_Spinner(
		AUI_ERRCODE *retval,
		uint32 id,
		MBCHAR *ldlBlock,
		ControlActionCallback *ActionFunc = NULL,
		void *cookie = NULL );
	ns_Spinner(
		AUI_ERRCODE *retval,
		uint32 id,
		sint32 x,
		sint32 y,
		sint32 width,
		sint32 height,
		AUI_RANGER_ORIENTATION orientation,
		MBCHAR *pattern,
		ControlActionCallback *ActionFunc = NULL,
		void *cookie = NULL );
	virtual ~ns_Spinner() {}

protected:
	ns_Spinner() : aui_Ranger() {}

public:
	virtual AUI_ERRCODE DrawThis(
		aui_Surface *surface = NULL,
		sint32 x = 0,
		sint32 y = 0 );
};

#endif
