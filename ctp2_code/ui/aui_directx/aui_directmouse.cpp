//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : DirectX mouse handler
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
// ACTIVISION_ORIGINAL		
// - When defined, generates the original Activision code.
// - When not defined, generates the modified Apolyton code.
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
#include "aui_surface.h"

#include "aui_directmouse.h"

#include "profiledb.h"
extern ProfileDB *g_theProfileDB;



aui_DirectMouse::aui_DirectMouse(
	AUI_ERRCODE *retval,
	MBCHAR *ldlBlock,
	BOOL useExclusiveMode )
	:
	aui_Mouse( retval, ldlBlock ),
	aui_Input( retval ),
	aui_DirectInput( retval, useExclusiveMode )
{
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;

	*retval = CreateDirectMouse();
   
    Assert(*retval != AUI_ERRCODE_CREATEFAILED);
    Assert(*retval != AUI_ERRCODE_SETFORMATFAILED);
    Assert(*retval != AUI_ERRCODE_SETCOOPLEVELFAILED);
    Assert(*retval != AUI_ERRCODE_SETEVENTFAILED);
    Assert(*retval != AUI_ERRCODE_SETPROPERTYFAILED);
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;
}




AUI_ERRCODE aui_DirectMouse::CreateDirectMouse( void )
{
	HRESULT hr;

	
	hr = m_lpdi->CreateDevice( GUID_SysMouse, &m_lpdid, NULL );
	if ( hr != DI_OK ) return AUI_ERRCODE_CREATEFAILED;

	
	hr = m_lpdid->SetDataFormat( &c_dfDIMouse );
	if ( hr != DI_OK ) return AUI_ERRCODE_SETFORMATFAILED;

	
	uint32 coopFlags = DISCL_NONEXCLUSIVE | DISCL_FOREGROUND;
	if ( m_exclusiveMode )
		coopFlags = DISCL_EXCLUSIVE | DISCL_FOREGROUND;

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
		k_MOUSE_MAXINPUT
	};

	hr = m_lpdid->SetProperty( DIPROP_BUFFERSIZE, &dipdw_buffersize.diph );
	if ( hr != DI_OK ) return AUI_ERRCODE_SETPROPERTYFAILED;

	return AUI_ERRCODE_OK;
}




AUI_ERRCODE aui_DirectMouse::GetInput( void )
{
	if ( !m_lpdid ) return AUI_ERRCODE_NODIRECTINPUTDEVICE;

	
	
	m_data.time = GetTickCount();

	bool haveMoves = false;
	
	for ( sint32 numInputs = 200; numInputs; numInputs-- )
	{
		uint32 numElements = 1;
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

		if ( numElements == 0 ) {
			if(haveMoves)
				return AUI_ERRCODE_OK;
			else
				return AUI_ERRCODE_NOINPUT;
		}

		
		switch ( ptrOd->dwOfs )
		{
		
		case DIMOFS_X:
			m_data.position.x += sint32(m_sensitivity * sint32(ptrOd->dwData));
			if ( m_data.position.x >= g_ui->Primary()->Width() )
				m_data.position.x = g_ui->Primary()->Width() - 1;
			else if ( m_data.position.x < 0 )
				m_data.position.x = 0;
			m_data.flags = m_flags;
			haveMoves = true;
			break;

		
		case DIMOFS_Y:
			m_data.position.y += sint32(m_sensitivity * sint32(ptrOd->dwData));
			if ( m_data.position.y >= g_ui->Primary()->Height() )
				m_data.position.y = g_ui->Primary()->Height() - 1;
			else if ( m_data.position.y < 0 )
				m_data.position.y = 0;
			m_data.flags = m_flags;
			haveMoves = true;
			break;

		
		case DIMOFS_BUTTON0:
			
			if (g_theProfileDB && g_theProfileDB->GetLeftHandedMouse()) {
				
				m_data.rbutton = uint8(ptrOd->dwData) & 0x80;
				m_data.flags = m_flags;
			} else {
				m_data.lbutton = uint8(ptrOd->dwData) & 0x80;
				m_data.flags = m_flags;
			}
			return AUI_ERRCODE_OK;

		
		case DIMOFS_BUTTON1:
			
			if (g_theProfileDB && g_theProfileDB->GetLeftHandedMouse()) {
				
				m_data.lbutton = uint8(ptrOd->dwData) & 0x80;
				m_data.flags = m_flags;
			} else {
				m_data.rbutton = uint8(ptrOd->dwData) & 0x80;
				m_data.flags = m_flags;
			}
			return AUI_ERRCODE_OK;

		case DIMOFS_BUTTON2:
			m_data.mbutton = uint8(ptrOd->dwData) & 0x80;
			m_data.flags = m_flags;
			return AUI_ERRCODE_OK;

		case DIMOFS_BUTTON3:
			m_data.tbutton = uint8(ptrOd->dwData) & 0x80;
			m_data.flags = m_flags;
			return AUI_ERRCODE_OK;

		case DIK_LSHIFT :
			if (uint8(ptrOd->dwData) & 0x80) {
				
				m_flags |= k_MOUSE_EVENT_FLAG_LSHIFT;
			} else {
				
				m_flags &= ~k_MOUSE_EVENT_FLAG_LSHIFT;
			}
			return AUI_ERRCODE_OK;
		case DIK_RSHIFT :
			if (uint8(ptrOd->dwData) & 0x80) {
				
				m_flags |= k_MOUSE_EVENT_FLAG_RSHIFT;
			} else {
				
				m_flags &= ~k_MOUSE_EVENT_FLAG_RSHIFT;
			}
			return AUI_ERRCODE_OK;
		case DIK_LCONTROL :
			if (uint8(ptrOd->dwData) & 0x80) {
				
				m_flags |= k_MOUSE_EVENT_FLAG_LCONTROL;
			} else {
				
				m_flags &= ~k_MOUSE_EVENT_FLAG_LCONTROL;
			}
			return AUI_ERRCODE_OK;
		case DIK_RCONTROL :
			if (uint8(ptrOd->dwData) & 0x80) {
				
				m_flags |= k_MOUSE_EVENT_FLAG_RCONTROL;
			} else {
				
				m_flags &= ~k_MOUSE_EVENT_FLAG_RCONTROL;
			}
			return AUI_ERRCODE_OK;
		}
	}

	
	
	
	

	return AUI_ERRCODE_OK;
}


#endif 
