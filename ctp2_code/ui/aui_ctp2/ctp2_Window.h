//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ header
// Description  : User interface window
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

#ifndef __CTP2_WINDOW_H__
#define __CTP2_WINDOW_H__

#include "aui_window.h"
#include "patternbase.h"
#include "pointerlist.h"

class ctp2_Window;

typedef void (CTP2WindowWeaklyModalCancelCallback) (aui_MouseEvent *event,
													ctp2_Window *window,
													void *cookie,
													bool &passEventOn);

class ctp2_Window : public aui_Window, public PatternBase
{
public:
	ctp2_Window(
		AUI_ERRCODE *retval,
		uint32 id,
		MBCHAR *ldlBlock,
		sint32 bpp,
		AUI_WINDOW_TYPE type = AUI_WINDOW_TYPE_STANDARD, bool bevel = true );
	ctp2_Window(
		AUI_ERRCODE *retval,
		uint32 id,
		sint32 x,
		sint32 y,
		sint32 width,
		sint32 height,
		sint32 bpp,
		MBCHAR *pattern,
		AUI_WINDOW_TYPE type = AUI_WINDOW_TYPE_STANDARD, bool bevel = true );
	virtual ~ctp2_Window() {}

protected:
	ctp2_Window() : aui_Window() {}
	AUI_ERRCODE InitCommon( void );

public:
	virtual AUI_ERRCODE DrawThis(
		aui_Surface *surface = NULL,
		sint32 x = 0,
		sint32 y = 0 );

	void SetWeaklyModalCancelCallback(CTP2WindowWeaklyModalCancelCallback *cb, void *cookie) {
		m_weaklyModalCancelCallback = cb;
		m_weaklyModalCancelCookie = cookie;
	}

	virtual void	MouseLGrabOutside		(aui_MouseEvent * mouseData);
	virtual void	MouseLDropOutside		(aui_MouseEvent * mouseData);

	void	SetBevelMode(bool on_or_off) { m_bevel=on_or_off;};


	void    ResetCurrentMouseState();

	virtual AUI_ERRCODE DoneInstantiatingThis(const MBCHAR *ldlBlock);

	virtual AUI_ERRCODE Move(sint32 x, sint32 y);
	virtual AUI_ERRCODE Offset(sint32 dx, sint32 dy);

	void AddDockedWindow(ctp2_Window *window);
	void RemoveDockedWindow(ctp2_Window *window);
	void SetDock(ctp2_Window *window);

	virtual bool HandleKey(uint32 wParam);

private:
	bool m_bevel;
	CTP2WindowWeaklyModalCancelCallback *m_weaklyModalCancelCallback;
	void *m_weaklyModalCancelCookie;
	PointerList<ctp2_Window> m_dockedWindows;
	ctp2_Window *m_dockedTo;
	ctp2_Window *m_dock;
};

#endif
