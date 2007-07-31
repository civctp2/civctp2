#ifndef __AUI_JOYSTICK_H__
#define __AUI_JOYSTICK_H__

#include "aui_base.h"
#include "aui_input.h"


enum AUI_JOYSTICK_CONTROL
{
	AUI_JOYSTICK_X_AXIS = 0,
	AUI_JOYSTICK_Y_AXIS,
	AUI_JOYSTICK_Z_AXIS,
	AUI_JOYSTICK_X_AXIS_ROTATION,
	AUI_JOYSTICK_Y_AXIS_ROTATION,
	AUI_JOYSTICK_Z_AXIS_ROTATION,
	AUI_JOYSTICK_POV_UP,
	AUI_JOYSTICK_POV_DOWN,
	AUI_JOYSTICK_POV_LEFT,
	AUI_JOYSTICK_POV_RIGHT,
	AUI_JOYSTICK_BUTTON_1,
	AUI_JOYSTICK_BUTTON_2,
	AUI_JOYSTICK_BUTTON_3,
	AUI_JOYSTICK_BUTTON_4,
	AUI_JOYSTICK_BUTTON_5,
	AUI_JOYSTICK_BUTTON_6,
	AUI_JOYSTICK_BUTTON_7,
	AUI_JOYSTICK_BUTTON_8
};


struct aui_JoystickEvent
{
	LONG		lX;
	LONG		lY;
	LONG		lZ;
	DWORD		POV[4];
	BYTE		buttons[32];
	uint32   key;
	BOOL		down;	
	uint32	time;	
};











class aui_Joystick : public aui_Base, public virtual aui_Input
{
public:
	
	aui_Joystick(
		AUI_ERRCODE *retval );
	virtual ~aui_Joystick();

protected:
	aui_Joystick() {}
	AUI_ERRCODE	InitCommon( void );

public:
	

	aui_JoystickEvent	*GetLatestJoystickEvent( void ) { return &m_data; }

protected:
	aui_JoystickEvent m_data;	

		
};

#endif 
