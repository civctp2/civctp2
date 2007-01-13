#include "c3.h"


#ifdef __AUI_USE_DIRECTX__

#if (DIRECTINPUT_VERSION >= 0x0500)

#include "aui_ui.h"

#include "aui_directjoystick.h"



BOOL CALLBACK enumMyJoystickCallback( LPDIDEVICEINSTANCE lpDeviceInst, LPVOID data );
BOOL CALLBACK enumJoystickObjectsCallback( LPCDIDEVICEOBJECTINSTANCE lpddoi, LPVOID data );


aui_DirectJoystick::aui_DirectJoystick(
	AUI_ERRCODE *retval,
	uint32 lMin,
	uint32 lMax)
	:
	aui_Joystick( retval ),
	aui_Input( retval ),
	aui_DirectInput( retval, FALSE ),
	m_lpDI2Joystick( NULL ),
	m_lMin( lMin ),
	m_lMax( lMax )
{
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;

	*retval = CreateDirectJoystick();

	if ( !AUI_SUCCESS(*retval) ) return;
}


aui_DirectJoystick::~aui_DirectJoystick()
{
	if (m_lpDI2Joystick)
	{
		m_lpDI2Joystick->Release();
		m_lpDI2Joystick = NULL;
	}
}


AUI_ERRCODE aui_DirectJoystick::CreateDirectJoystick( void )
{
	HRESULT hr;

	hr = m_lpdi->EnumDevices( DIDEVTYPE_JOYSTICK, (LPDIENUMDEVICESCALLBACK)enumMyJoystickCallback, this, DIEDFL_ATTACHEDONLY );
	if ( hr != DI_OK ) return AUI_ERRCODE_CREATEFAILED;

	
	if ( m_lpdid )
	{
	hr = m_lpdid->QueryInterface( IID_IDirectInputDevice2, (LPVOID *)&m_lpDI2Joystick );
	if ( hr != DI_OK ) return AUI_ERRCODE_CREATEFAILED;

	
	m_lpdid->Acquire();
	}
	else
		return AUI_ERRCODE_CREATEFAILED;

	return AUI_ERRCODE_OK;
}


AUI_ERRCODE aui_DirectJoystick::GetInput( void )
{
	BOOL bSame = TRUE;

	if ( !m_lpdid ) return AUI_ERRCODE_NODIRECTINPUTDEVICE;

	
	
	m_data.time = GetTickCount();

	
	for ( sint32 numInputs = 2; numInputs; numInputs-- )
	{
		DIJOYSTATE js;
		static DIDEVICEOBJECTDATA od[ 1 ];
		DIDEVICEOBJECTDATA *ptrOd = od;
		HRESULT hr;

		
		hr = m_lpDI2Joystick->Poll();

		
		hr = m_lpdid->GetDeviceState(
			sizeof( DIJOYSTATE ),
			&js);

		switch ( hr )
		{
		case DI_OK:
			break;

		case DIERR_NOTACQUIRED:
			return AUI_ERRCODE_NOTACQUIRED;

		case DIERR_INPUTLOST:
			{
			Acquire();
			m_lpDI2Joystick->Poll();
			return AUI_ERRCODE_INPUTLOST;
			}

		case DIERR_INVALIDPARAM:
			return AUI_ERRCODE_GETDEVICEDATAFAILED;

		case DI_BUFFEROVERFLOW:
			return AUI_ERRCODE_GETDEVICEDATAFAILED;

		default:
			return AUI_ERRCODE_GETDEVICEDATAFAILED;
		}

		
		
		uint32 key;

		key = AUI_JOYSTICK_X_AXIS;
		if ( ( abs(js.lX - m_data.lX) > (int ( m_lMax / 2 ) )) )
		{
			m_data.key = key;
			bSame = FALSE;
		}

		key = AUI_JOYSTICK_Y_AXIS;
		if ( ( abs(js.lY - m_data.lY) > (int ( m_lMax / 2 ) )) )
		{
			m_data.key = key;
			bSame = FALSE;
		}
	
		for ( int i = 0 ; i < 4 && bSame; i++ )
		{
			if ( js.rgdwPOV[i] != m_data.POV[i] )
			{	
				switch ( js.rgdwPOV[i] )
				{
				case 0:
					key = AUI_JOYSTICK_POV_UP;
					break;

				case 9000:
					key = AUI_JOYSTICK_POV_RIGHT;
					break;

				case 18000:
					key = AUI_JOYSTICK_POV_DOWN;
					break;

				case 27000:
					key = AUI_JOYSTICK_POV_LEFT;
					break;
				}
				m_data.key = key;
				bSame = FALSE;
			}
		}

		key = AUI_JOYSTICK_BUTTON_1;
		for ( int j = 0; j < 32 && bSame; j++, key++ )
		{
			if ( js.rgbButtons[j] != m_data.buttons[j] )
			{
				m_data.key = key;
				bSame = FALSE;
			}
		}

		
		if ( !bSame )
		{
			m_data.lX = js.lX;
			m_data.lY = js.lY;
			m_data.lZ = js.lZ;

			for ( int i = 0; i < 4; i++ )
				m_data.POV[i] = js.rgdwPOV[i];

			for ( int j = 0; j < 32; j++ )
				m_data.buttons[j] = js.rgbButtons[j];

			m_data.down = TRUE;
		}
		else
		{
			m_data.down = FALSE;
			return AUI_ERRCODE_NOINPUT;
		}
	}

	return AUI_ERRCODE_OK;
}

BOOL CALLBACK enumMyJoystickCallback( LPDIDEVICEINSTANCE lpDeviceInst, LPVOID data )
{
	DIPROPRANGE				diprg; 
	LPDIRECTINPUTDEVICE		lpTempJoystick;
	aui_DirectJoystick *joy = NULL;

	joy = (aui_DirectJoystick *)data;
	
	if( DI_OK != (joy->DI())->CreateDevice( lpDeviceInst->guidInstance, &lpTempJoystick, NULL ) )
		return( DIENUM_CONTINUE );

	if( DI_OK != lpTempJoystick->SetDataFormat( &c_dfDIJoystick ) )
	{
		lpTempJoystick->Release();
		return( DIENUM_CONTINUE );
	}

	if( DI_OK != lpTempJoystick->SetCooperativeLevel( g_ui->TheHWND(), DISCL_FOREGROUND | DISCL_NONEXCLUSIVE ) )
	{
		lpTempJoystick->Release();
		return( DIENUM_CONTINUE );
	}

	
	
	diprg.diph.dwSize = sizeof(DIPROPRANGE); 
	diprg.diph.dwHeaderSize = sizeof(DIPROPHEADER); 
	diprg.diph.dwHow = DIPH_BYOFFSET; 
	diprg.lMin = joy->GetLowerMin(); 
	diprg.lMax = joy->GetUpperMax(); 

	diprg.diph.dwObj = DIJOFS_X; 
	lpTempJoystick->SetProperty( DIPROP_RANGE, &diprg.diph );
	
	diprg.diph.dwObj = DIJOFS_Y; 
	lpTempJoystick->SetProperty( DIPROP_RANGE, &diprg.diph );
	
	diprg.diph.dwObj = DIJOFS_Z; 
	lpTempJoystick->SetProperty( DIPROP_RANGE, &diprg.diph );
	
	diprg.diph.dwObj = DIJOFS_RX; 
	lpTempJoystick->SetProperty( DIPROP_RANGE, &diprg.diph );
	
	diprg.diph.dwObj = DIJOFS_RY; 
	lpTempJoystick->SetProperty( DIPROP_RANGE, &diprg.diph );
	
	diprg.diph.dwObj = DIJOFS_RZ; 
	lpTempJoystick->SetProperty( DIPROP_RANGE, &diprg.diph );

	diprg.diph.dwObj = DIJOFS_SLIDER(0); 
	lpTempJoystick->SetProperty( DIPROP_RANGE, &diprg.diph );

 	diprg.diph.dwObj = DIJOFS_SLIDER(1); 
	lpTempJoystick->SetProperty( DIPROP_RANGE, &diprg.diph );

	joy->SetDevice( lpTempJoystick );
	joy->SetDeviceName( lpDeviceInst->tszProductName );

	return( DIENUM_CONTINUE );
}



#endif 

#endif 

