







#pragma once
#ifndef __MESSAGEICONBUTTON_H__
#define __MESSAGEICONBUTTON_H__


#include "aui_Button.h"



class MessageIconButton : public aui_Button
{
public:
	
	MessageIconButton(
		AUI_ERRCODE *retval,
		uint32 id,
		MBCHAR *ldlBlock,
		ControlActionCallback *ActionFunc = NULL,
		void *cookie = NULL );

	virtual ~MessageIconButton();
	virtual AUI_ERRCODE DrawThis(
		aui_Surface *surface = NULL,
		sint32 x = 0,
		sint32 y = 0 );

	void SetCurrentIconButton( MessageIconButton *button );


	

private:
	static MessageIconButton	*m_currentButton;
protected:


	



	
	virtual MouseEventCallback MouseRGrabInside;
	virtual MouseEventCallback MouseRDropInside;
	virtual MouseEventCallback MouseRDropOutside;

};


#endif 
