










#ifndef __AUI_DIRECTMOUSE_H__
#define __AUI_DIRECTMOUSE_H__

#ifdef __AUI_USE_DIRECTX__


#include "aui_mouse.h"
#include "aui_directinput.h"




class aui_DirectMouse : public aui_Mouse, public aui_DirectInput
{
public:
	
	aui_DirectMouse(
		AUI_ERRCODE *retval,
		MBCHAR *ldlBlock,
		BOOL useExclusiveMode = FALSE );
	virtual ~aui_DirectMouse() {}

protected:
	aui_DirectMouse() {}
	AUI_ERRCODE CreateDirectMouse( void );

public:
	
	virtual AUI_ERRCODE Acquire( void ) { return aui_DirectInput::Acquire(); }
	virtual AUI_ERRCODE Unacquire( void ) { return aui_DirectInput::Unacquire(); }

	
	virtual AUI_ERRCODE ReactToInput( void ) { return aui_Mouse::ReactToInput(); }

	
	virtual AUI_ERRCODE GetInput( void );
};


#endif 

#endif 
