//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ header
// Description  : User interface button
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
//
//----------------------------------------------------------------------------
//
// Modifications from the original Activision code:
//
// - Event handlers declared in a notation that is more standard C++.
//
//----------------------------------------------------------------------------

#if defined(HAVE_PRAGMA_ONCE)
#pragma once
#endif

#ifndef AUI_BUTTON_H_
#define AUI_BUTTON_H_

class aui_Button;

enum AUI_BUTTON_ACTION
{
	AUI_BUTTON_ACTION_FIRST = 0,
	AUI_BUTTON_ACTION_NULL = 0,
	AUI_BUTTON_ACTION_PRESS,
	AUI_BUTTON_ACTION_RELEASE,
	AUI_BUTTON_ACTION_EXECUTE,
	AUI_BUTTON_ACTION_LAST
};

#include "aui_control.h"
#include "aui_keyboard.h"
class aui_Image;
class aui_Surface;

class aui_Button : public aui_Control
{
public:
	aui_Button(
		AUI_ERRCODE *retval,
		uint32 id,
		const MBCHAR *ldlBlock,
		ControlActionCallback *ActionFunc = NULL,
		Cookie cookie = nullptr );
	aui_Button(
		AUI_ERRCODE *retval,
		uint32 id,
		sint32 x,
		sint32 y,
		sint32 width,
		sint32 height,
		ControlActionCallback *ActionFunc = NULL,
		Cookie cookie = nullptr );
	virtual ~aui_Button() {}

	virtual AUI_ERRCODE ResetThis( void );

	sint32 ExtractRepeatCount( uint32 data ) const { return (sint32)data; }

	virtual AUI_ERRCODE DrawThis(
		aui_Surface *surface = NULL,
		sint32 x = 0,
		sint32 y = 0 );

protected:
	aui_Button()
	:
	    aui_Control     (),
	    m_isRepeating   (false),
	    m_repeatCount   (0)
	{};

	AUI_ERRCODE InitCommonLdl( const MBCHAR *ldlBlock );

	virtual void	KeyboardCallback(aui_KeyboardEvent * keyboardData);

	virtual void	PostChildrenCallback(aui_MouseEvent * mouseData);

	virtual void	MouseLDragOver(aui_MouseEvent * mouseData);
	virtual void	MouseLDragAway(aui_MouseEvent * mouseData);
	virtual void	MouseLGrabInside(aui_MouseEvent * mouseData);
	virtual void	MouseLDropInside(aui_MouseEvent * mouseData);
	virtual void	MouseLDropOutside(aui_MouseEvent * mouseData);
	virtual void	MouseRDropInside(aui_MouseEvent * mouseData);

	bool	m_isRepeating;
	sint32	m_repeatCount;

private:
	AUI_ERRCODE InitCommon( void );
};

#endif
