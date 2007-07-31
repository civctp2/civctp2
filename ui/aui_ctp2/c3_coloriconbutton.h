












#pragma once
#ifndef __C3_COLORICONBUTTON_H__
#define __C3_COLORICONBUTTON_H__


#include "c3_button.h"



enum C3_COLORICONBUTTON_ACTION {
	C3_COLORICONBUTTON_ACTION_DOUBLECLK = AUI_BUTTON_ACTION_LAST,
	C3_COLORICONBUTTON_ACTION_RIGHT_PRESS,
	C3_COLORICONBUTTON_ACTION_RIGHTCLK
};

class Picture;



class c3_ColorIconButton : public c3_Button
{
public:
	
	c3_ColorIconButton(
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

	c3_ColorIconButton(
		AUI_ERRCODE *retval,
		uint32 id,
		MBCHAR *ldlBlock,
		ControlActionCallback *ActionFunc = NULL,
		void *cookie = NULL );

	virtual AUI_ERRCODE	InitCommon( MBCHAR *ldlBlock, BOOL isLDL = FALSE);

	virtual ~c3_ColorIconButton();

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

	virtual MouseEventCallback MouseLDoubleClickInside;

	
	virtual MouseEventCallback MouseRGrabInside;
	virtual MouseEventCallback MouseRDropInside;
	virtual MouseEventCallback MouseRDropOutside;

};


#endif 
