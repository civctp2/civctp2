//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ header
// Description  : User interface control window
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

#ifndef __AUI_WIN_H__
#define __AUI_WIN_H__

#include "aui_control.h"


class aui_Win : public aui_Control
{
public:

	aui_Win(
		AUI_ERRCODE *retval,
		uint32 id,
		const MBCHAR *ldlBlock,
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
	AUI_ERRCODE InitCommonLdl( const MBCHAR *ldlBlock );
	AUI_ERRCODE InitCommon( void );

public:
	const MBCHAR	*GetWindowClass( void ) const { return m_windowClass; }

#if defined(__AUI_USE_DIRECTX__)
	BOOL	IsRegistered(void) const { return m_registered; }
	HWND	TheHWND( void ) const { return m_hwnd; }
	static aui_Win *GetWinFromHWND(HWND hwnd);
#endif

	virtual aui_Control	*SetKeyboardFocus( void );

	virtual AUI_ERRCODE	DrawThis(
		aui_Surface *surface,
		sint32 x,
		sint32 y );

protected:
	static const MBCHAR	*m_windowClass;

#if defined(__AUI_USE_DIRECTX__)
	static BOOL			m_registered;
	static sint32		m_winRefCount;
	HWND			m_hwnd;
	HDC				m_memdc;
	static tech_WLList<aui_Win *> *m_winList;
	HBITMAP			m_hbitmap;
	HBITMAP			m_hbitmapOld;

	POINT			m_offscreen;
#endif

	void			WinMouseMove(aui_MouseEvent * mouseData);
	void			WinMouseLDrag(aui_MouseEvent * mouseData);
	void			WinMouseRDrag(aui_MouseEvent * mouseData);

	virtual void	MouseMoveOver(aui_MouseEvent * mouseData);
	virtual void	MouseMoveAway(aui_MouseEvent * mouseData);
	virtual void	MouseMoveInside(aui_MouseEvent * mouseData);
	virtual void	MouseMoveOutside(aui_MouseEvent * mouseData);

	virtual void	MouseLDragOver(aui_MouseEvent * mouseData);
	virtual void	MouseLDragAway(aui_MouseEvent * mouseData);
	virtual void	MouseLDragInside(aui_MouseEvent * mouseData);
	virtual void	MouseLDragOutside(aui_MouseEvent * mouseData);
	virtual void	MouseRDragOver(aui_MouseEvent * mouseData);
	virtual void	MouseRDragAway(aui_MouseEvent * mouseData);
	virtual void	MouseRDragInside(aui_MouseEvent * mouseData);
	virtual void	MouseRDragOutside(aui_MouseEvent * mouseData);

	virtual void	MouseLGrabInside(aui_MouseEvent * mouseData);
	virtual void	MouseLGrabOutside(aui_MouseEvent * mouseData);
	virtual void	MouseLDropInside(aui_MouseEvent * mouseData);
	virtual void	MouseLDropOutside(aui_MouseEvent * mouseData);
	virtual void	MouseRGrabInside(aui_MouseEvent * mouseData);
	virtual void	MouseRGrabOutside(aui_MouseEvent * mouseData);
	virtual void	MouseRDropInside(aui_MouseEvent * mouseData);
	virtual void	MouseRDropOutside(aui_MouseEvent * mouseData);

	virtual void	MouseLDoubleClickInside(aui_MouseEvent * mouseData);
	virtual void	MouseRDoubleClickInside(aui_MouseEvent * mouseData);
};

#endif
