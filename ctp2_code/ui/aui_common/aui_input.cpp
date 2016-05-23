#include "c3.h"
#include "aui_input.h"


aui_Input::aui_Input(
	AUI_ERRCODE *retval )
{
	*retval = InitCommon();
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;
}


AUI_ERRCODE aui_Input::InitCommon( void )
{
	m_acquired = FALSE;

	return AUI_ERRCODE_OK;
}


aui_Input::~aui_Input()
{
	Unacquire();
}


AUI_ERRCODE aui_Input::Acquire( void )
{
	m_acquired = TRUE;

	return AUI_ERRCODE_OK;
}


AUI_ERRCODE aui_Input::Unacquire( void )
{
	m_acquired = FALSE;

	return AUI_ERRCODE_OK;
}
