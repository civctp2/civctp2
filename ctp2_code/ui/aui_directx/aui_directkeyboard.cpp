//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : DirectX keyboard handler
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
// - When defined, use DirectX.
// - When not defined, this file is not used at all.
//
//----------------------------------------------------------------------------
//
// Modifications from the original Activision code:
//
// - Continue reading from the buffer when an overflow is reported.
//
//----------------------------------------------------------------------------

#include "c3.h"


#ifdef __AUI_USE_DIRECTX__


#include "aui_ui.h"

#include "aui_directkeyboard.h"

#include "c3ui.h"
extern C3UI		*g_c3ui;

#include "civapp.h"
extern CivApp	*g_civApp;


aui_DirectKeyboard::aui_DirectKeyboard(AUI_ERRCODE *retval)
:
	aui_Keyboard(),
	aui_DirectInput(retval, FALSE)
{
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;

	*retval = CreateDirectKeyboard();
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;
}




AUI_ERRCODE aui_DirectKeyboard::CreateDirectKeyboard( void )
{
	HRESULT hr;

	
	hr = m_lpdi->CreateDevice( GUID_SysKeyboard, &m_lpdid, NULL );
	if ( hr != DI_OK ) return AUI_ERRCODE_CREATEFAILED;

	
	hr = m_lpdid->SetDataFormat( &c_dfDIKeyboard );
	if ( hr != DI_OK ) return AUI_ERRCODE_SETFORMATFAILED;

	
	uint32 coopFlags = DISCL_NONEXCLUSIVE | DISCL_FOREGROUND;

	hr = m_lpdid->SetCooperativeLevel( g_ui->TheHWND(), coopFlags );
	if ( hr != DI_OK ) return AUI_ERRCODE_SETCOOPLEVELFAILED;

	hr = m_lpdid->SetEventNotification( m_inputEvent );
	if ( hr != DI_OK ) return AUI_ERRCODE_SETEVENTFAILED;

	
	static DIPROPDWORD dipdw_buffersize =
	{
		{
			sizeof( DIPROPDWORD ),
			sizeof( DIPROPHEADER ),
			0,
			DIPH_DEVICE,
		},
		k_KEYBOARD_MAXINPUT
	};

	hr = m_lpdid->SetProperty( DIPROP_BUFFERSIZE, &dipdw_buffersize.diph );
	if ( hr != DI_OK ) return AUI_ERRCODE_SETPROPERTYFAILED;

	
	m_lpdid->Acquire();

	return AUI_ERRCODE_OK;
}




AUI_ERRCODE aui_DirectKeyboard::GetInput( void )
{
	if ( !m_lpdid ) return AUI_ERRCODE_NODIRECTINPUTDEVICE;

	
	
	m_data.time = GetTickCount();

	
	for ( sint32 numInputs = 1; numInputs; numInputs-- )
	{
		DWORD   numElements = 1;
		static DIDEVICEOBJECTDATA od[ 1 ];
		DIDEVICEOBJECTDATA *ptrOd = od;

		
		HRESULT hr = m_lpdid->GetDeviceData(
			sizeof( DIDEVICEOBJECTDATA ),
			ptrOd,
			&numElements,
			0 );

		switch ( hr )
		{
		case DI_BUFFEROVERFLOW:
		case DI_OK:
			break;

		case DIERR_NOTACQUIRED:
			return AUI_ERRCODE_NOTACQUIRED;

		case DIERR_INPUTLOST:
			Acquire();
			return AUI_ERRCODE_INPUTLOST;

		default:
			return AUI_ERRCODE_GETDEVICEDATAFAILED;
		}

		if ( numElements == 0 ) return AUI_ERRCODE_NOINPUT;

		switch (ptrOd->dwOfs) {
		case DIK_LSHIFT :
			if (g_c3ui->TheMouse()) {
				if (uint8(ptrOd->dwData) & 0x80) {
					
					g_c3ui->TheMouse()->SetFlags(g_c3ui->TheMouse()->GetFlags() | k_MOUSE_EVENT_FLAG_LSHIFT);
				} else {
					
					g_c3ui->TheMouse()->SetFlags(g_c3ui->TheMouse()->GetFlags() & ~k_MOUSE_EVENT_FLAG_LSHIFT);
				}
			}
			return AUI_ERRCODE_OK;
		case DIK_RSHIFT :
			if (g_c3ui->TheMouse()) {
				if (uint8(ptrOd->dwData) & 0x80) {
					
					g_c3ui->TheMouse()->SetFlags(g_c3ui->TheMouse()->GetFlags() | k_MOUSE_EVENT_FLAG_RSHIFT);
				} else {
					
					g_c3ui->TheMouse()->SetFlags(g_c3ui->TheMouse()->GetFlags() & ~k_MOUSE_EVENT_FLAG_RSHIFT);
				}
			}
			return AUI_ERRCODE_OK;
		case DIK_LCONTROL :
			if (g_c3ui->TheMouse()) {
				if (uint8(ptrOd->dwData) & 0x80) {
					
					g_c3ui->TheMouse()->SetFlags(g_c3ui->TheMouse()->GetFlags() | k_MOUSE_EVENT_FLAG_LCONTROL);
				} else {
					
					g_c3ui->TheMouse()->SetFlags(g_c3ui->TheMouse()->GetFlags() & ~k_MOUSE_EVENT_FLAG_LCONTROL);
				}
			}
			return AUI_ERRCODE_OK;
		case DIK_RCONTROL :
			if (g_c3ui->TheMouse()) {
				if (uint8(ptrOd->dwData) & 0x80) {
					
					g_c3ui->TheMouse()->SetFlags(g_c3ui->TheMouse()->GetFlags() | k_MOUSE_EVENT_FLAG_RCONTROL);
				} else {
					
					g_c3ui->TheMouse()->SetFlags(g_c3ui->TheMouse()->GetFlags() & ~k_MOUSE_EVENT_FLAG_RCONTROL);
				}
			}
			return AUI_ERRCODE_OK;
		case DIK_RETURN:
			
			#ifdef _DEBUG
				extern BOOL commandMode;
				if(commandMode)
					return AUI_ERRCODE_OK;
			#endif
			break;

		case DIK_UPARROW:
		case DIK_LEFTARROW:
		case DIK_RIGHTARROW:
		case DIK_DOWNARROW:
				if (uint8(ptrOd->dwData) & 0x80) {
					
					g_civApp->BeginKeyboardScrolling(ptrOd->dwOfs);
				} else {
					
					g_civApp->StopKeyboardScrolling(ptrOd->dwOfs);
				}
			break;
		}

		uint8 const l_DownMask  = static_cast<uint8>(ptrOd->dwData & 0x0080);
        m_data.down                     = l_DownMask;
		m_keyboardState[ptrOd->dwOfs]   = l_DownMask;
		m_data.key                      = ptrOd->dwOfs;
	}

	return AUI_ERRCODE_OK;
}


#endif 
