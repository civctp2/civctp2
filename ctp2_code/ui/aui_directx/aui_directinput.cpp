//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : DirectInput device handling
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
// Has to be defined, or no code will be generated at all.
//
//----------------------------------------------------------------------------
//
// Modifications from the original Activision code:
//
// - Enabled mousewheel for the non-debug versions.
//
//----------------------------------------------------------------------------

#include "c3.h"

#ifdef __AUI_USE_DIRECTX__

#include "aui_ui.h"

#include "aui_directinput.h"


aui_DirectInput::aui_DirectInput
(
	AUI_ERRCODE *retval,
	BOOL useExclusiveMode
)
:
	aui_Input		(),
	aui_DirectX		(),
	m_lpdid		(NULL),
	m_inputEvent	(NULL)

{
	*retval = InitCommon(useExclusiveMode);
}


AUI_ERRCODE aui_DirectInput::InitCommon( BOOL useExclusiveMode )
{
	m_inputEvent = CreateEvent( NULL, FALSE, FALSE, NULL );
	if ( m_inputEvent == NULL ) return AUI_ERRCODE_CREATEEVENTFAILED;

	AUI_ERRCODE errcode = aui_DirectX::InitCommon( useExclusiveMode );
	return errcode;
}


aui_DirectInput::~aui_DirectInput()
{
	Unacquire();

	if ( m_lpdi && m_lpdid )
	{
        m_lpdid->Release();
        m_lpdid = NULL;
      }

	if ( m_inputEvent )
	{
		CloseHandle( m_inputEvent );
		m_inputEvent = NULL;
	}
}


AUI_ERRCODE aui_DirectInput::Acquire( void )
{
    if (!g_ui)  return AUI_ERRCODE_ACQUIREFAILED;

#if defined(_DEBUG)
	uint32 const coopFlags = DISCL_NONEXCLUSIVE | DISCL_BACKGROUND;
#else
	uint32 const coopFlags = DISCL_NONEXCLUSIVE | DISCL_FOREGROUND;
#endif

	HRESULT hr  = m_lpdid->SetCooperativeLevel(g_ui->TheHWND(), coopFlags);
	if ( hr != DI_OK ) return AUI_ERRCODE_SETCOOPLEVELFAILED;

	hr = m_lpdid->Acquire();
	if ( hr != DI_OK ) return AUI_ERRCODE_ACQUIREFAILED;

	AUI_ERRCODE result  = aui_Input::Acquire();
    if (!AUI_SUCCESS(result))
    {
        m_lpdid->Unacquire();
    }

    return result;
}


AUI_ERRCODE aui_DirectInput::Unacquire( void )
{
	aui_Input::Unacquire();

	if ( !m_lpdid ) return AUI_ERRCODE_HACK;

	HRESULT hr = m_lpdid->Unacquire();

	if ( hr != DI_OK ) return AUI_ERRCODE_UNACQUIREFAILED;

		uint32 coopFlags = DISCL_NONEXCLUSIVE | DISCL_FOREGROUND;
		if ( m_exclusiveMode )
			coopFlags = DISCL_EXCLUSIVE | DISCL_FOREGROUND;

		hr = m_lpdid->SetCooperativeLevel( g_ui->TheHWND(), coopFlags );
		if ( hr != DI_OK ) return AUI_ERRCODE_SETCOOPLEVELFAILED;

	return AUI_ERRCODE_OK;
}

#endif
