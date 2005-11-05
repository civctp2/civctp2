//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : User interface: keyboard handling
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
// None
//
//----------------------------------------------------------------------------
//
// Modifications from the original Activision code:
//
// - 
//
//----------------------------------------------------------------------------

#include "c3.h"
#include "aui_keyboard.h"

#include <algorithm>

aui_Keyboard::aui_Keyboard()
:	aui_Base	(),
	aui_Input	(),
	m_data	()
{
	std::fill(m_keyboardState, m_keyboardState + k_KEYBOARD_MAXSTATE, 0);
}

aui_Keyboard::~aui_Keyboard()
{ ; }

AUI_ERRCODE aui_Keyboard::GetKeyboardState(uint8 * keyboardState) const
{
	Assert(keyboardState);
	if (!keyboardState) return AUI_ERRCODE_INVALIDPARAM;

	memcpy(keyboardState, m_keyboardState, sizeof(m_keyboardState));
	return AUI_ERRCODE_OK;
}
