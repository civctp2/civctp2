#ifdef HAVE_PRAGMA_ONCE
#pragma once
#endif
#ifndef __C3_SLIDOMETER_H__
#define __C3_SLIDOMETER_H__


#include "aui_ranger.h"
#include "patternbase.h"


class aui_Surface;

#define k_C3_SLIDOMETER_PERCENT_FILLED	"percent"
#define k_C3_SLIDOMETER_COLOR			"color"


class c3_Slidometer : public aui_Ranger, public PatternBase
{
public:
	
	c3_Slidometer(
		AUI_ERRCODE *retval,
		uint32 id,
		MBCHAR *ldlBlock,
		ControlActionCallback *ActionFunc = NULL,
		void *cookie = NULL );
	c3_Slidometer(
		AUI_ERRCODE *retval,
		uint32 id,
		sint32 x,
		sint32 y,
		sint32 width,
		sint32 height,
		BOOL isVertical,
		MBCHAR *pattern,
		sint32 percentFilled = NULL,
		ControlActionCallback *ActionFunc = NULL,
		void *cookie = NULL );
	virtual ~c3_Slidometer() {}

protected:
	c3_Slidometer() : aui_Ranger() {}
	AUI_ERRCODE InitCommon( MBCHAR *ldlBlock );
	AUI_ERRCODE InitCommon( void );
	AUI_ERRCODE CreateThumb( MBCHAR *ldlBlock );

	sint32	m_percentFilled;

public:
	virtual AUI_ERRCODE DrawThis(
		aui_Surface *surface = NULL,
		sint32 x = 0,
		sint32 y = 0 );

	sint32 GetPercentFilled() { return m_percentFilled; }
	void SetPercentFilled( sint32 percentFilled );

	BOOL	IsVertical( void ) const
	{ return m_orientation == AUI_RANGER_ORIENTATION_VERTICAL; }
};

#endif 
