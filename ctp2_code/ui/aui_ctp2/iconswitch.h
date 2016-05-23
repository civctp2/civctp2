#ifdef HAVE_PRAGMA_ONCE
#pragma once
#endif
#ifndef __ICONSWITCH_H__
#define __ICONSWITCH_H__

#include "aui_switch.h"
#include "patternbase.h"

class Pattern;
class Icon;

class IconSwitch : public aui_Switch, public PatternBase
{
public:

	IconSwitch(
		AUI_ERRCODE *retval,
		uint32 id,
		sint32 x,
		sint32 y,
		const MBCHAR *pattern = NULL,
		Icon *icon = NULL,
		uint16 color = 0,
		ControlActionCallback *ActionFunc = NULL,
		void *cookie = NULL,
		BOOL selected = FALSE );
	virtual ~IconSwitch() {}

	Pattern *&ThePattern( void ) { return m_pattern; }
	Icon *&TheIcon( void ) { return m_icon; }
	uint16 TheColor( void ) { return m_color; }

	virtual AUI_ERRCODE DrawThis(
		aui_Surface *surface = NULL,
		sint32 x = 0,
		sint32 y = 0 );

protected:
	Icon *m_icon;
	uint16 m_color;
};

#endif
