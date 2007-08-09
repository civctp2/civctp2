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



aui_Keyboard::aui_Keyboard(
	AUI_ERRCODE *retval )
	:
	aui_Input( retval )
{
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;

	*retval = InitCommon();
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;
}



AUI_ERRCODE aui_Keyboard::InitCommon( void )
{
	memset( &m_data, 0, sizeof( m_data ) );
	memset( &m_keyboardState, 0, sizeof( m_keyboardState ) );

	return AUI_ERRCODE_OK;
}



aui_Keyboard::~aui_Keyboard()
{
	Unacquire();
}



AUI_ERRCODE aui_Keyboard::GetKeyboardState( uint8 *keyboardState )
{
	Assert( keyboardState != NULL );
	if ( !keyboardState ) return AUI_ERRCODE_INVALIDPARAM;

	memcpy( keyboardState, m_keyboardState, sizeof( m_keyboardState ) );

	return AUI_ERRCODE_OK;
}
