#ifdef HAVE_PRAGMA_ONCE
#pragma once
#endif
#ifndef __ICONBUTTON_H__
#define __ICONBUTTON_H__

#include "aui_button.h"
#include "patternbase.h"

class Icon;


class IconButton : public aui_Button, public PatternBase
{
public:

	IconButton(
		AUI_ERRCODE *retval,
		uint32 id,
		sint32 x,
		sint32 y,
		sint32 width,
		sint32 height,
		const MBCHAR *pattern,
		const MBCHAR *icon,
		uint16 color,
		ControlActionCallback *ActionFunc = NULL,
		void *cookie = NULL );

	IconButton(
		AUI_ERRCODE *retval,
		uint32 id,
		const MBCHAR *ldlBlock,
		ControlActionCallback *ActionFunc = NULL,
		void *cookie = NULL );

	virtual AUI_ERRCODE	InitCommon(const MBCHAR *ldlBlock,BOOL isLDL = FALSE);

	virtual ~IconButton();

	Icon *&TheIcon( void ) { return m_icon; }

	virtual AUI_ERRCODE DrawThis(
		aui_Surface *surface = NULL,
		sint32 x = 0,
		sint32 y = 0 );

protected:
	Icon		*m_icon;
	uint16		m_color;
	MBCHAR		*m_filename;
};

#endif
