











#pragma once
#ifndef __ICONTAB_H__
#define __ICONTAB_H__

#include "aui_tab.h"
#include "patternbase.h"

class Pattern;
class Icon;

class IconTab : public aui_Tab, public PatternBase
{
public:
	
	IconTab(
		AUI_ERRCODE *retval,
		uint32 id,
		sint32 x,
		sint32 y,
		sint32 width,
		sint32 height,
		sint32 paneWidth,
		sint32 paneHeight,
		MBCHAR *pattern = NULL,
		Icon *icon = NULL,
		uint16 color = NULL,
		ControlActionCallback *ActionFunc = NULL,
		void *cookie = NULL,
		BOOL selected = FALSE );
	virtual ~IconTab() {}

	
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