#include "ctp/c3.h"
#include "ui/aui_common/aui_input.h"

aui_Input::aui_Input()
:	m_acquired	(false)
{ ; }

aui_Input::~aui_Input()
{ ; }

AUI_ERRCODE aui_Input::Acquire( void )
{
	m_acquired = true;
	return AUI_ERRCODE_OK;
}

AUI_ERRCODE aui_Input::Unacquire( void )
{
	m_acquired = false;
	return AUI_ERRCODE_OK;
}
