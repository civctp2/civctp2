//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ header file
// Description  :
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
//----------------------------------------------------------------------------
//
// Modifications from the original Activision code:
//
//
//----------------------------------------------------------------------------
#ifndef __AUI_KEYBOARD_H__
#define __AUI_KEYBOARD_H__


#include "aui_base.h"
#include "aui_input.h"

#if defined(__AUI_USE_DIRECTX__)
enum AUI_KEYBOARD_KEY
{
	AUI_KEYBOARD_KEY_FIRST = 0,
	AUI_KEYBOARD_KEY_INVALID = 0,
	AUI_KEYBOARD_KEY_ESCAPE = DIK_ESCAPE,
	AUI_KEYBOARD_KEY_RETURN = DIK_RETURN,
	AUI_KEYBOARD_KEY_SPACE = DIK_SPACE,
	AUI_KEYBOARD_KEY_TAB = DIK_TAB,
	AUI_KEYBOARD_KEY_UPARROW = DIK_UPARROW,
	AUI_KEYBOARD_KEY_DOWNARROW = DIK_DOWNARROW,
	AUI_KEYBOARD_KEY_LEFTARROW = DIK_LEFTARROW,
	AUI_KEYBOARD_KEY_RIGHTARROW = DIK_RIGHTARROW,
	AUI_KEYBOARD_KEY_LAST
};
#else
enum AUI_KEYBOARD_KEY
{
	AUI_KEYBOARD_KEY_FIRST = 0,
	AUI_KEYBOARD_KEY_INVALID = 0,
	AUI_KEYBOARD_KEY_ESCAPE,
	AUI_KEYBOARD_KEY_RETURN,
	AUI_KEYBOARD_KEY_SPACE,
	AUI_KEYBOARD_KEY_TAB,
	AUI_KEYBOARD_KEY_UPARROW,
	AUI_KEYBOARD_KEY_DOWNARROW,
	AUI_KEYBOARD_KEY_LEFTARROW,
	AUI_KEYBOARD_KEY_RIGHTARROW,
	AUI_KEYBOARD_KEY_LAST
};
#endif 

const uint32 AUI_KEYBOARD_MOD_LSHIFT = 1;
const uint32 AUI_KEYBOARD_MOD_RSHIFT = 2;
const uint32 AUI_KEYBOARD_MOD_LCTRL   = 4;
const uint32 AUI_KEYBOARD_MOD_RCTRL = 8;
const uint32 AUI_KEYBOARD_MOD_LALT = 16;
const uint32 AUI_KEYBOARD_MOD_RALT = 32;
const uint32 AUI_KEYBOARD_MOD_LWIN = 64;
const uint32 AUI_KEYBOARD_MOD_RWIN = 128;
const uint32 AUI_KEYBOARD_MOD_WINMENU = 256;


struct aui_KeyboardEvent
{
	uint32	key;	
	BOOL	down;	
	uint32	time;	
};



#define k_KEYBOARD_MAXINPUT		24

#define k_KEYBOARD_MAXSTATE		256



class aui_Keyboard : public aui_Base, public virtual aui_Input
{
public:
	
	aui_Keyboard(
		AUI_ERRCODE *retval );
	virtual ~aui_Keyboard();

protected:
	aui_Keyboard() {}
	AUI_ERRCODE	InitCommon( void );

public:
	
	AUI_ERRCODE			GetKeyboardState( uint8 *keyboardState );
	aui_KeyboardEvent	*GetLatestKeyboardEvent( void ) { return &m_data; }

protected:
	aui_KeyboardEvent m_data;	
	uint8 m_keyboardState[ k_KEYBOARD_MAXSTATE ];
		
};


#endif 
