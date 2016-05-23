#ifndef __AUI_DIRECTJOYSTICK_H__
#define __AUI_DIRECTJOYSTICK_H__

#ifdef __AUI_USE_DIRECTX__

#if (DIRECTINPUT_VERSION >= 0x0500)

#include "aui_joystick.h"
#include "aui_directinput.h"

class aui_DirectJoystick : public aui_Joystick, public aui_DirectInput
{
public:

	aui_DirectJoystick(
		AUI_ERRCODE *retval,
		uint32 lMin = -1000,
		uint32 lMax = 1000);
	virtual ~aui_DirectJoystick();

	void SetDevice( LPDIRECTINPUTDEVICE device ) { m_lpdid = device;};
	LPDIRECTINPUTDEVICE Device( void ) { return m_lpdid;};

	void SetDeviceName( MBCHAR *name ) { strcpy( m_deviceName, name ); };
	MBCHAR *DeviceName( void ) { return m_deviceName; };

	uint32 GetLowerMin ( void ) { return m_lMin; };
	uint32 GetUpperMax ( void ) { return m_lMax; };

protected:
	aui_DirectJoystick() {}
	AUI_ERRCODE CreateDirectJoystick( void );

public:

	virtual AUI_ERRCODE Acquire( void ) { return aui_DirectInput::Acquire(); }
	virtual AUI_ERRCODE Unacquire( void ) { return aui_DirectInput::Unacquire(); }

	virtual AUI_ERRCODE GetInput( void );

protected:
	LPDIRECTINPUTDEVICE2 	m_lpDI2Joystick;
	MBCHAR	m_deviceName[40];
	uint32	m_lMax;
	uint32	m_lMin;

};

typedef aui_DirectJoystick aui_NativeJoystick;

#endif

#endif

#endif
