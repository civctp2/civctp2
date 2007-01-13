//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ header
// Description  : User interface thumb?
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
//
//----------------------------------------------------------------------------

#ifndef __AUI_THUMB_H__
#define __AUI_THUMB_H__


#include "aui_control.h"


class aui_Surface;



enum AUI_THUMB_ACTION
{
	AUI_THUMB_ACTION_FIRST = 0,
	AUI_THUMB_ACTION_NULL = 0,
	AUI_THUMB_ACTION_GRAB,
	AUI_THUMB_ACTION_DRAG,
	AUI_THUMB_ACTION_DROP,
	AUI_THUMB_ACTION_LAST
};



class aui_Thumb : public aui_Control
{
public:
	
	aui_Thumb(
		AUI_ERRCODE *retval,
		uint32 id,
		MBCHAR *ldlBlock,
		ControlActionCallback *ActionFunc = NULL,
		void *cookie = NULL );
	aui_Thumb(
		AUI_ERRCODE *retval,
		uint32 id,
		sint32 x,
		sint32 y,
		sint32 width,
		sint32 height,
		ControlActionCallback *ActionFunc = NULL,
		void *cookie = NULL );
	virtual ~aui_Thumb() {}

protected:
	aui_Thumb() : aui_Control() {}
	AUI_ERRCODE	InitCommon( void );

public:
	AUI_ERRCODE	Reposition( sint32 x, sint32 y );

protected:
	POINT	m_grabPoint;	

#if defined(_MSC_VER)	
	virtual MouseEventCallback MouseLDragOver;
	virtual MouseEventCallback MouseLDragAway;
	virtual MouseEventCallback MouseLDragInside;
	virtual MouseEventCallback MouseLDragOutside;

	
	virtual MouseEventCallback MouseLGrabInside;
	virtual MouseEventCallback MouseRGrabInside {} 
	virtual MouseEventCallback MouseLDropInside;
	virtual MouseEventCallback MouseLDropOutside;

	virtual MouseEventCallback MouseRDropInside;
#else
	virtual void	MouseLDragOver(aui_MouseEvent * mouseData);
	virtual void	MouseLDragAway(aui_MouseEvent * mouseData);
	virtual void	MouseLDragInside(aui_MouseEvent * mouseData);
	virtual void	MouseLDragOutside(aui_MouseEvent * mouseData);

	virtual void	MouseLGrabInside(aui_MouseEvent * mouseData);
	virtual void	MouseRGrabInside(aui_MouseEvent * mouseData) {};
	virtual void	MouseLDropInside(aui_MouseEvent * mouseData);
	virtual void	MouseLDropOutside(aui_MouseEvent * mouseData);

	virtual void	MouseRDropInside(aui_MouseEvent * mouseData);
#endif
};


#endif 
