

#pragma once
#ifndef __UNITTABBUTTON_H__
#define __UNITTABBUTTON_H__

#include "aui_control.h"
#include "patternbase.h"

#define k_DEFAULT_HEALTHBAR_HEIGHT	4
#define k_CARGO_CAPACITY			5
#define k_CARGO_OFFSET				1
#define k_BIG_CARGO_OFFSET			4

class Thermometer;
class c3_ColorIconButton;
class c3_Static;
class c3_ColoredStatic;
class Unit;
class aui_StringTable;

class UnitTabButton : public aui_Control, public PatternBase
{
public:

	
	UnitTabButton(
		AUI_ERRCODE *retval,
		uint32 id,
		MBCHAR *ldlBlock,
		ControlActionCallback *ActionFunc = NULL,
		void *cookie = NULL );
	UnitTabButton(
		AUI_ERRCODE *retval,
		uint32 id,
		sint32 x,
		sint32 y,
		sint32 width,
		sint32 height,
		MBCHAR *pattern,
		sint32 barHeight = k_DEFAULT_HEALTHBAR_HEIGHT,
		ControlActionCallback *ActionFunc = NULL,
		void *cookie = NULL );
	virtual ~UnitTabButton();

	virtual AUI_ERRCODE DrawThis(aui_Surface *surface = NULL,
								sint32 x = 0,
								sint32 y = 0);

	AUI_ERRCODE InitCommon( void );

	c3_ColorIconButton	*IconButton( void ) const { return m_button; }

	sint32	UpdateData( Unit *unit );
	
private:
	Thermometer			*m_healthBar;
	c3_ColorIconButton		*m_button;
	c3_Static			*m_fortify;
	c3_Static			*m_veteran;
	c3_Static			*m_arrow;
	c3_ColoredStatic	*m_cargo[ k_CARGO_CAPACITY ];

	sint32	m_barHeight;

};


#endif 
