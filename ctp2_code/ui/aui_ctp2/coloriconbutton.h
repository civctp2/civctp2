//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ header
// Description  : User interface color icon button?
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
// _MSC_VER		
// - Use Microsoft C++ extensions when set.
//
//----------------------------------------------------------------------------
//
// Modifications from the original Activision code:
//
// - Event handlers declared in a notation that is more standard C++.
// - #pragma once commented out.
//
//----------------------------------------------------------------------------

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef __COLORICONBUTTON_H__
#define __COLORICONBUTTON_H__


#include "c3_button.h"



enum COLORICONBUTTON_ACTION {
	COLORICONBUTTON_ACTION_DOUBLECLK = AUI_BUTTON_ACTION_LAST,
	COLORICONBUTTON_ACTION_RIGHT_PRESS,
	COLORICONBUTTON_ACTION_RIGHTCLK
};

class Picture;



class ColorIconButton : public c3_Button
{
public:
	
	ColorIconButton(
		AUI_ERRCODE *retval,
		uint32 id,
		sint32 x,
		sint32 y,
		sint32 width,
		sint32 height,
		MBCHAR *pattern,
		MBCHAR *icon,
		ControlActionCallback *ActionFunc = NULL,
		void *cookie = NULL );

	ColorIconButton(
		AUI_ERRCODE *retval,
		uint32 id,
		MBCHAR *ldlBlock,
		ControlActionCallback *ActionFunc = NULL,
		void *cookie = NULL );

	virtual AUI_ERRCODE	InitCommon( MBCHAR *ldlBlock, BOOL isLDL = FALSE);

	virtual ~ColorIconButton();

	virtual AUI_ERRCODE Resize(sint32 width, sint32 height);
	void	ResizePictureRect(void);

	
	void SetIcon(MBCHAR *name);
	void ShrinkToFit(BOOL fit) { m_shrinkToFit = fit; }

	
	virtual AUI_ERRCODE DrawThis(
		aui_Surface *surface = NULL,
		sint32 x = 0,
		sint32 y = 0 );

protected:
	MBCHAR		*m_filename;
	BOOL		m_shrinkToFit;
	RECT		m_pictureRect;

#if defined(_MSC_VER)
	virtual MouseEventCallback MouseLDoubleClickInside;

	
	virtual MouseEventCallback MouseRGrabInside;
	virtual MouseEventCallback MouseRDropInside;
	virtual MouseEventCallback MouseRDropOutside;
#else
    virtual void	MouseLDoubleClickInside(aui_MouseEvent * data);
    virtual void	MouseRGrabInside(aui_MouseEvent * data);
    virtual void	MouseRDropInside(aui_MouseEvent * data);
    virtual void	MouseRDropOutside(aui_MouseEvent * data);
#endif

};


#endif 
