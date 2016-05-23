//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ header
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
// - None
//
//----------------------------------------------------------------------------
//
// Modifications from the original Activision code:
//
// - Improved include structure
// - Added functionality to support pattern information saving
//
//----------------------------------------------------------------------------
//
/// \file c3_popupwindow.h
/// \brief Pop-up window (declarations)

#ifdef HAVE_PRAGMA_ONCE
#pragma once
#endif

#ifndef C3_POPUPWINDOW_H_
#define C3_POPUPWINDOW_H_

#include <algorithm>
#include <string>

class c3_PopupWindow;

enum POPUP_BORDER {
	POPUP_BORDER_UL,
	POPUP_BORDER_UR,
	POPUP_BORDER_LL,
	POPUP_BORDER_LR,
	POPUP_BORDER_LEFT,
	POPUP_BORDER_TOP,
	POPUP_BORDER_RIGHT,
	POPUP_BORDER_BOTTOM,

	POPUP_BORDER_MAX
};

#include "c3window.h"
#include "keyboardhandler.h"
class c3_Static;
class c3_Button;
class ctp2_Button;
class Pattern;

class c3_PopupWindow : public C3Window, public KeyboardHandler
{
public:
	c3_PopupWindow(
		AUI_ERRCODE *retval,
		uint32 id,
		MBCHAR *ldlBlock,
		sint32 bpp,
		AUI_WINDOW_TYPE type = AUI_WINDOW_TYPE_STANDARD,
		bool bevel = true );
	c3_PopupWindow(
		AUI_ERRCODE *retval,
		uint32 id,
		sint32 x,
		sint32 y,
		sint32 width,
		sint32 height,
		sint32 bpp,
		MBCHAR *pattern,
		AUI_WINDOW_TYPE type = AUI_WINDOW_TYPE_STANDARD,
		bool bevel = true );

	virtual ~c3_PopupWindow();

	virtual AUI_ERRCODE Resize( sint32 width, sint32 height );

	virtual AUI_ERRCODE DrawThis(
		aui_Surface *surface = NULL,
		sint32 x = 0,
		sint32 y = 0 );

	sint32 AddTitle( MBCHAR *titleBlock = NULL );
	sint32 AddCancel(
		void (*actionFunc)( aui_Control *, uint32, uint32, void *) = NULL,
		void *cookie = NULL,
		MBCHAR *buttonBlock = "c3_PopupCancel" );
	sint32 AddOk(
		void (*actionFunc)( aui_Control *, uint32, uint32, void *) = NULL,
		void *cookie = NULL,
		MBCHAR *buttonBlock = "c3_PopupOk" );
	sint32 AddYes(
		void (*actionFunc)( aui_Control *, uint32, uint32, void *) = NULL,
		void *cookie = NULL,
		MBCHAR *buttonBlock = "c3_PopupYes" );
	sint32 AddClose(
		void (*actionFunc)( aui_Control *, uint32, uint32, void *) = NULL,
		void *cookie = NULL,
		MBCHAR *buttonBlock = "c3_PopupClose" );
	sint32 AddNo(
		void (*actionFunc)( aui_Control *, uint32, uint32, void *) = NULL,
		void *cookie = NULL,
		MBCHAR *buttonBlock = "c3_PopupNo" );

	c3_Static	*TitleText( void ) { return m_titleText; }
	c3_Static	*Title( void ) { return m_title; }

	c3_Button	*Cancel( void ) const { return m_cancel; }
	ctp2_Button	*Ok( void ) const { return m_ok; }

	virtual void    kh_Close();
	virtual void    PatternInfoRestore(void);
	virtual void    PatternInfoSave(void);

protected:
	c3_PopupWindow()
	:
		C3Window    (),
		m_title     (NULL),
		m_titleText (NULL),
		m_cancel    (NULL),
		m_ok        (NULL)
	{
		std::fill(m_border, m_border + POPUP_BORDER_MAX, (c3_Static *) NULL);
	}

private:
	virtual AUI_ERRCODE InitCommon(void);

	c3_Static *     m_border[POPUP_BORDER_MAX];
	c3_Static *     m_title;
	c3_Static *     m_titleText;
	c3_Button *     m_cancel;
	ctp2_Button	*   m_ok;
	/// Saved pattern information for borders and background
	std::string     m_patternInfo[POPUP_BORDER_MAX + 1];
};

#endif
