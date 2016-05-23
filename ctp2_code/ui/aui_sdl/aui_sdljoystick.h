#ifdef HAVE_PRAGMA_ONCE
#pragma once
#endif

#ifndef __aui_sdl__aui_sdljoystick_h__
#define __aui_sdl__aui_sdljoystick_h__ 1

#include "ctp2_config.h"

#if defined(__AUI_USE_SDL__)

#include "aui_joystick.h"
#include "aui_sdlinput.h"

class aui_SDLJoystick : public aui_Joystick, public aui_SDLInput {
public:
	aui_SDLJoystick(
		AUI_ERRCODE *retval,
		uint32 lMin = -1000,
		uint32 lMax = 1000);
	virtual ~aui_SDLJoystick();

#if 0
	void SetDevice( LPDIRECTINPUTDEVICE device ) { m_lpdid = device;};
	LPDIRECTINPUTDEVICE Device( void ) { return m_lpdid;};
#endif

	void SetDeviceName( MBCHAR *name ) { strcpy( m_deviceName, name ); };
	MBCHAR *DeviceName( void ) { return m_deviceName; };

	uint32 GetLowerMin ( void ) { return m_lMin; };
	uint32 GetUpperMax ( void ) { return m_lMax; };

protected:
	aui_SDLJoystick() {}
#if 0
	AUI_ERRCODE CreateDirectJoystick( void );
#endif

public:
	virtual AUI_ERRCODE Acquire( void ) { return aui_SDLInput::Acquire(); }
	virtual AUI_ERRCODE Unacquire( void ) { return aui_SDLInput::Unacquire(); }

#if 0
	virtual AUI_ERRCODE GetInput( void );
#endif
protected:
#if 0
	LPDIRECTINPUTDEVICE2 	m_lpDI2Joystick;
#endif
	MBCHAR	m_deviceName[40];

	uint32	m_lMax;
	uint32	m_lMin;

};

#endif // defined(__AUI_USE_SDL__)

#endif
