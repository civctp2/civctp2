#ifdef HAVE_PRAGMA_ONCE
#pragma once
#endif
#ifndef __RADIOGROUP_H__
#define __RADIOGROUP_H__

#include "aui_switchgroup.h"
#include "patternbase.h"


class RadioGroup : public aui_SwitchGroup, public PatternBase
{
public:

	RadioGroup(
		AUI_ERRCODE *retval,
		uint32 id,
		MBCHAR *ldlBlock );
	RadioGroup(
		AUI_ERRCODE *retval,
		uint32 id,
		sint32 x,
		sint32 y,
		sint32 width,
		sint32 height,
		MBCHAR *pattern = NULL,
		MBCHAR *text = NULL );
	virtual ~RadioGroup() {}

protected:
	RadioGroup() : aui_SwitchGroup() {}

public:
	virtual AUI_ERRCODE DrawThis(
		aui_Surface *surface = NULL,
		sint32 x = 0,
		sint32 y = 0 );
};

#endif
