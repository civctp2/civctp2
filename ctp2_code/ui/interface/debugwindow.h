#ifdef HAVE_PRAGMA_ONCE
#pragma once
#endif
#ifndef __DEBUGWINDOW_H__
#define __DEBUGWINDOW_H__

#include "c3window.h"

class TextBox;

class aui_TabGroup;

class DebugWindow : public C3Window
{
    uint32 m_debug_mask;
    BOOL m_allow_next;
public:
	DebugWindow(
		AUI_ERRCODE *retval,
		uint32 id,
		const MBCHAR *ldlBlock,
		sint32 bpp,
		AUI_WINDOW_TYPE type = AUI_WINDOW_TYPE_BACKGROUND );
	DebugWindow(
		AUI_ERRCODE *retval,
		uint32 id,
		sint32 x,
		sint32 y,
		sint32 width,
		sint32 height,
		sint32 bpp,
		const MBCHAR *pattern,
		AUI_WINDOW_TYPE type = AUI_WINDOW_TYPE_BACKGROUND );

	virtual ~DebugWindow() {}

	virtual AUI_ERRCODE InitCommon(void);

	virtual AUI_ERRCODE DrawThis(
		aui_Surface *surface = NULL,
		sint32 x = 0,
		sint32 y = 0 );






	void SetTextBox(TextBox *box) { m_textBox = box; }
	TextBox *GetTextBox(void) const { return m_textBox; }

    void SetDebugMask(uint32 m);
    void AddText(const uint32 m, const MBCHAR *text);

    void AddMask(uint32 m);
    void AddText(const char *err, ...)
#ifdef __GNUC__
	__attribute__((__format__ (__printf__, 2, 3)))
#endif
	;

private:
		TextBox				*m_textBox;
};

#ifdef _DEBUG
#define WPRINTF(wmask, warg) { g_debugWindow->AddMask(wmask); g_debugWindow->AddText warg; }
#else
#define WPRINTF(wmask, warg)
#endif

#endif
