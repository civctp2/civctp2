










#ifndef __AUI_WIN_H__
#define __AUI_WIN_H__


#include "aui_control.h"



class aui_Win : public aui_Control
{
public:
	
	aui_Win(
		AUI_ERRCODE *retval,
		uint32 id,
		MBCHAR *ldlBlock,
		ControlActionCallback *ActionFunc = NULL,
		void *cookie = NULL );
	aui_Win(
		AUI_ERRCODE *retval,
		uint32 id,
		sint32 x,
		sint32 y,
		sint32 width,
		sint32 height,
		ControlActionCallback *ActionFunc = NULL,
		void *cookie = NULL );
	virtual ~aui_Win();

protected:
	aui_Win() : aui_Control() {}
	AUI_ERRCODE InitCommonLdl( MBCHAR *ldlBlock );
	AUI_ERRCODE InitCommon( void );

public:
	BOOL	IsRegistered( void ) const { return m_registered; }
	MBCHAR	*GetWindowClass( void ) const { return m_windowClass; }

	HWND	TheHWND( void ) const { return m_hwnd; }

	virtual aui_Control	*SetKeyboardFocus( void );

	static aui_Win *GetWinFromHWND( HWND hwnd );

	virtual AUI_ERRCODE	DrawThis(
		aui_Surface *surface,
		sint32 x,
		sint32 y );

protected:
	static BOOL		m_registered;	
	static MBCHAR	*m_windowClass;	
	static sint32	m_winRefCount;	

	HWND			m_hwnd;			

	POINT			m_offscreen;	

	HDC				m_memdc;
	HBITMAP			m_hbitmap;
	HBITMAP			m_hbitmapOld;

	static tech_WLList<aui_Win *> *m_winList;
									

	
	
	

	
	MouseEventCallback WinMouseMove;
	MouseEventCallback WinMouseLDrag;
	MouseEventCallback WinMouseRDrag;

	
	virtual MouseEventCallback MouseMoveOver;
	virtual MouseEventCallback MouseMoveAway;
	virtual MouseEventCallback MouseMoveInside;
	virtual MouseEventCallback MouseMoveOutside;

	
	virtual MouseEventCallback MouseLDragOver;
	virtual MouseEventCallback MouseLDragAway;
	virtual MouseEventCallback MouseLDragInside;
	virtual MouseEventCallback MouseLDragOutside;
	virtual MouseEventCallback MouseRDragOver;
	virtual MouseEventCallback MouseRDragAway;
	virtual MouseEventCallback MouseRDragInside;
	virtual MouseEventCallback MouseRDragOutside;

	
	virtual MouseEventCallback MouseLGrabInside;
	virtual MouseEventCallback MouseLGrabOutside;
	virtual MouseEventCallback MouseLDropInside;
	virtual MouseEventCallback MouseLDropOutside;
	virtual MouseEventCallback MouseRGrabInside;
	virtual MouseEventCallback MouseRGrabOutside;
	virtual MouseEventCallback MouseRDropInside;
	virtual MouseEventCallback MouseRDropOutside;

	
	virtual MouseEventCallback MouseLDoubleClickInside;
	virtual MouseEventCallback MouseRDoubleClickInside;
};


#endif 
