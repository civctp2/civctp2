










#ifndef __TESTWINDOW_H__
#define __TESTWINDOW_H__

#include "c3window.h"

class IconButton;

class Button1Action;
class Button2Action;


class UITestWindow : public C3Window
{
public:
	UITestWindow(
		AUI_ERRCODE *retval,
		uint32 id,
		sint32 x,
		sint32 y,
		sint32 width,
		sint32 height,
		sint32 bpp,
		MBCHAR *pattern,
		AUI_WINDOW_TYPE type = AUI_WINDOW_TYPE_STANDARD );

	virtual ~UITestWindow();
protected:
	AUI_ERRCODE InitCommon( );

public:
	virtual AUI_ERRCODE DrawThis(
		aui_Surface *surface,
		sint32 x = 0,
		sint32 y = 0 );

protected:
	IconButton				*m_button1;
	IconButton				*m_button2;

	Button1Action			*m_button1Action;
	Button2Action			*m_button2Action;
};


#endif 

