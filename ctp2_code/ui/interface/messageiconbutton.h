//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ header
// Description  : User interface message icon button
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
//----------------------------------------------------------------------------
//
// Modifications from the original Activision code:
//
// - Event handlers declared in a notation that is more standard C++.
// - #pragma once commented out.
//
//----------------------------------------------------------------------------

#ifdef HAVE_PRAGMA_ONCE
#pragma once
#endif

#ifndef __MESSAGEICONBUTTON_H__
#define __MESSAGEICONBUTTON_H__


#include "aui_button.h"



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

protected:
    virtual void	MouseRGrabInside(aui_MouseEvent * data);
    virtual void	MouseRDropInside(aui_MouseEvent * data);
    virtual void	MouseRDropOutside(aui_MouseEvent * data);

private:
	static MessageIconButton	*m_currentButton;

};


#endif 
