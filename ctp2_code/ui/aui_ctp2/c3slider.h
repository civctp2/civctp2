//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ header
// Description  : User interface slider
//
//----------------------------------------------------------------------------
//
// Disclaimer
//
// THIS FILE IS NOT GENERATED OR SUPPORTED BY ACTIVISION.
//
// This material has been developed at apolyton.net by the Apolyton CtP2 
// Source Code Project. Contact the authors at ctp2source@apolyton.net.
//
//----------------------------------------------------------------------------
//
// Compiler flags
// 
// _MSC_VER		
// - Use Microsoft C++ extensions when set.
//
//----------------------------------------------------------------------------
//
// Modifications from the original Activision code:
//
// - Event handlers declared in a notation that is more standard C++.
// - #pragma once commented out.
//
//----------------------------------------------------------------------------

#ifdef HAVE_PRAGMA_ONCE
#pragma once
#endif

#ifndef __C3SLIDER_H__
#define __C3SLIDER_H__


#include "aui_ranger.h"
#include "patternbase.h"


class aui_Surface;


#define k_C3SLIDER_LDL_TICKS	"ticks"



class C3Slider : public aui_Ranger, public PatternBase
{
public:
	
	C3Slider(
		AUI_ERRCODE *retval,
		uint32 id,
		MBCHAR *ldlBlock,
		ControlActionCallback *ActionFunc = NULL,
		void *cookie = NULL );
	C3Slider(
		AUI_ERRCODE *retval,
		uint32 id,
		sint32 x,
		sint32 y,
		sint32 width,
		sint32 height,
		BOOL isVertical,
		MBCHAR *pattern,
		ControlActionCallback *ActionFunc = NULL,
		void *cookie = NULL );
	virtual ~C3Slider() {}

protected:
	C3Slider() : aui_Ranger() {}
	AUI_ERRCODE InitCommon( MBCHAR *ldlBlock );
	AUI_ERRCODE InitCommon( void );
	AUI_ERRCODE CreateThumb( MBCHAR *ldlBlock );

public:
	virtual AUI_ERRCODE DrawThis(
		aui_Surface *surface = NULL,
		sint32 x = 0,
		sint32 y = 0 );

	BOOL	IsVertical( void ) const
	{ return m_orientation == AUI_RANGER_ORIENTATION_VERTICAL; }

#if defined(_MSC_VER)
	virtual MouseEventCallback MouseRGrabInside;
#else
	virtual void	MouseRGrabInside(aui_MouseEvent * mouseData);
#endif

protected:
	sint32 m_ticks;
};

aui_Control::ControlActionCallback C3SliderThumbActionCallback;
aui_Control::ControlActionCallback C3SliderButtonActionCallback;

#endif 
