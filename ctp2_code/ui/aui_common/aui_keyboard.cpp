










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
