#include "c3.h"

#include "aui_joystick.h"



aui_Joystick::aui_Joystick(
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



AUI_ERRCODE aui_Joystick::InitCommon( void )
{
	memset( &m_data, 0, sizeof( m_data ) );


	return AUI_ERRCODE_OK;
}



aui_Joystick::~aui_Joystick()
{
	Unacquire();
}













