//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ header file
// Description  : User interface: keyboard handling
// Id           : $Id$
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
// __AUI_USE_DIRECTX__
// Use DirectX
//
//----------------------------------------------------------------------------
//
// Modifications from the original Activision code:
//
//
//----------------------------------------------------------------------------

#if defined(HAVE_PRAGMA_ONCE)
#pragma once
#endif

#ifndef __AUI_KEYBOARD_H__
#define __AUI_KEYBOARD_H__

//----------------------------------------------------------------------------
// Library dependencies
//----------------------------------------------------------------------------

#if defined(__AUI_USE_DIRECTX__)
#include <dinput.h>	    	// DIK_...
#endif

// Not really a library, but needed in the export section
#include "ctp2_inttypes.h"	// uint##

//----------------------------------------------------------------------------
// Export overview
//----------------------------------------------------------------------------

class aui_Keyboard;

enum AUI_KEYBOARD_KEY
{
	AUI_KEYBOARD_KEY_FIRST              = 0,
	AUI_KEYBOARD_KEY_INVALID            = 0,
#if defined(__AUI_USE_DIRECTX__)
	AUI_KEYBOARD_KEY_ESCAPE             = DIK_ESCAPE,
	AUI_KEYBOARD_KEY_RETURN             = DIK_RETURN,
	AUI_KEYBOARD_KEY_SPACE              = DIK_SPACE,
	AUI_KEYBOARD_KEY_TAB                = DIK_TAB,
	AUI_KEYBOARD_KEY_UPARROW            = DIK_UPARROW,
	AUI_KEYBOARD_KEY_DOWNARROW          = DIK_DOWNARROW,
	AUI_KEYBOARD_KEY_LEFTARROW          = DIK_LEFTARROW,
	AUI_KEYBOARD_KEY_RIGHTARROW         = DIK_RIGHTARROW,
#else
	AUI_KEYBOARD_KEY_ESCAPE,
	AUI_KEYBOARD_KEY_RETURN,
	AUI_KEYBOARD_KEY_SPACE,
	AUI_KEYBOARD_KEY_TAB,
	AUI_KEYBOARD_KEY_UPARROW,
	AUI_KEYBOARD_KEY_DOWNARROW,
	AUI_KEYBOARD_KEY_LEFTARROW,
	AUI_KEYBOARD_KEY_RIGHTARROW,
#endif // __AUI_USE_DIRECTX__
	AUI_KEYBOARD_KEY_LAST
};

const uint32 AUI_KEYBOARD_MOD_LSHIFT    = 0x0001;
const uint32 AUI_KEYBOARD_MOD_RSHIFT    = 0x0002;
const uint32 AUI_KEYBOARD_MOD_LCTRL     = 0x0004;
const uint32 AUI_KEYBOARD_MOD_RCTRL     = 0x0008;
const uint32 AUI_KEYBOARD_MOD_LALT      = 0x0010;
const uint32 AUI_KEYBOARD_MOD_RALT      = 0x0020;
const uint32 AUI_KEYBOARD_MOD_LWIN      = 0x0040;
const uint32 AUI_KEYBOARD_MOD_RWIN      = 0x0080;
const uint32 AUI_KEYBOARD_MOD_WINMENU   = 0x0100;

struct aui_KeyboardEvent
{
	uint32	    key;
	BOOL	    down;
	uint32	    time;

	aui_KeyboardEvent()
	:	key	    (0),
		down	(false),
		time	(0)
	{ ; };
};

#define k_KEYBOARD_MAXINPUT		24
#define k_KEYBOARD_MAXSTATE		256

//----------------------------------------------------------------------------
// Project dependencies
//----------------------------------------------------------------------------

#include "aui_base.h"
#include "aui_input.h"

//----------------------------------------------------------------------------
// Class declarations
//----------------------------------------------------------------------------

class aui_Keyboard : public aui_Base, public virtual aui_Input
{
public:
	aui_Keyboard();
	virtual ~aui_Keyboard();

	AUI_ERRCODE         GetKeyboardState(uint8 * keyboardState) const;
	aui_KeyboardEvent * GetLatestKeyboardEvent(void)
	{
		return &m_data;
	};

protected:
	aui_KeyboardEvent 	m_data;
	uint8 			    m_keyboardState[k_KEYBOARD_MAXSTATE];
};

#endif // __AUI_KEYBOARD_H__
