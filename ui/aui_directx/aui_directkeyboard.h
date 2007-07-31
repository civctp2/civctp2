










#ifndef __AUI_DIRECTKEYBOARD_H__
#define __AUI_DIRECTKEYBOARD_H__

#ifdef __AUI_USE_DIRECTX__


#include "aui_keyboard.h"
#include "aui_directinput.h"




class aui_DirectKeyboard : public aui_Keyboard, public aui_DirectInput
{
public:
	
	aui_DirectKeyboard(
		AUI_ERRCODE *retval );
	virtual ~aui_DirectKeyboard() {}

protected:
	aui_DirectKeyboard() {}
	AUI_ERRCODE CreateDirectKeyboard( void );

public:
	
	virtual AUI_ERRCODE Acquire( void ) { return aui_DirectInput::Acquire(); }
	virtual AUI_ERRCODE Unacquire( void ) { return aui_DirectInput::Unacquire(); }

	
	virtual AUI_ERRCODE GetInput( void );
};


#endif 

#endif 
