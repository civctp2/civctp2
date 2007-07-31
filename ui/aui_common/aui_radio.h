










#ifndef __AUI_RADIO_H__
#define __AUI_RADIO_H__


#include "aui_switch.h"



class aui_Radio : public aui_Switch
{
public:
	
	aui_Radio(
		AUI_ERRCODE *retval,
		uint32 id,
		MBCHAR *ldlBlock,
		ControlActionCallback *ActionFunc = NULL,
		void *cookie = NULL );
	aui_Radio(
		AUI_ERRCODE *retval,
		uint32 id,
		sint32 x,
		sint32 y,
		sint32 width,
		sint32 height,
		ControlActionCallback *ActionFunc = NULL,
		void *cookie = NULL,
		sint32 state = 0,
		sint32 numStates = k_AUI_SWITCH_DEFAULTNUMSTATES );
	virtual ~aui_Radio() {}

protected:
	aui_Radio() : aui_Switch() {}

protected:
	
	
	

	
	virtual MouseEventCallback MouseLDropInside;
	virtual MouseEventCallback MouseRDropInside;
};


#endif 
