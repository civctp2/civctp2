










#ifndef __AUI_BUTTON_H__
#define __AUI_BUTTON_H__


#include "aui_control.h"


class aui_Surface;
class aui_Image;



enum AUI_BUTTON_ACTION
{
	AUI_BUTTON_ACTION_FIRST = 0,
	AUI_BUTTON_ACTION_NULL = 0,
	AUI_BUTTON_ACTION_PRESS,
	AUI_BUTTON_ACTION_RELEASE,
	AUI_BUTTON_ACTION_EXECUTE,
	AUI_BUTTON_ACTION_LAST
};



class aui_Button : public aui_Control
{
public:
	
	aui_Button(
		AUI_ERRCODE *retval,
		uint32 id,
		MBCHAR *ldlBlock,
		ControlActionCallback *ActionFunc = NULL,
		void *cookie = NULL );
	aui_Button(
		AUI_ERRCODE *retval,
		uint32 id,
		sint32 x,
		sint32 y,
		sint32 width,
		sint32 height,
		ControlActionCallback *ActionFunc = NULL,
		void *cookie = NULL );
	virtual ~aui_Button() {}

protected:
	aui_Button() : aui_Control() {}
	AUI_ERRCODE InitCommonLdl( MBCHAR *ldlBlock );
	AUI_ERRCODE InitCommon( void );
	
public:
	virtual AUI_ERRCODE ResetThis( void );

	
	sint32 ExtractRepeatCount( uint32 data ) const { return (sint32)data; }

	virtual AUI_ERRCODE DrawThis(
		aui_Surface *surface = NULL,
		sint32 x = 0,
		sint32 y = 0 );

protected:
	BOOL	m_isRepeating;	
	sint32	m_repeatCount;	
							

	
	
	

	
	virtual KeyboardEventCallback KeyboardCallback;

	
	virtual MouseEventCallback PostChildrenCallback;

	
	virtual MouseEventCallback MouseLDragOver;
	virtual MouseEventCallback MouseLDragAway;

	
	virtual MouseEventCallback MouseLGrabInside;
	virtual MouseEventCallback MouseLDropInside;
	virtual MouseEventCallback MouseLDropOutside;

	virtual MouseEventCallback MouseRDropInside;
};


#endif 
