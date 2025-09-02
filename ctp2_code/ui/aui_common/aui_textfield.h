//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ header
// Description  : User interface text field
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

#ifndef __AUI_TEXTFIELD_H__
#define __AUI_TEXTFIELD_H__

#include "aui_bitmapfont.h"
#include "aui_win.h"


enum AUI_TEXTFIELD_ACTION
{
	AUI_TEXTFIELD_ACTION_FIRST = 0,
	AUI_TEXTFIELD_ACTION_NULL = 0,
	AUI_TEXTFIELD_ACTION_EXECUTE,
	AUI_TEXTFIELD_ACTION_DISMISS,
	AUI_TEXTFIELD_ACTION_LAST
};


#define k_AUI_TEXTFIELD_LDL_TEXT		"fieldtext"
#define k_AUI_TEXTFIELD_LDL_MULTILINE	"multiline"
#define k_AUI_TEXTFIELD_LDL_AUTOVSCROLL	"autovscroll"
#define k_AUI_TEXTFIELD_LDL_AUTOHSCROLL	"autohscroll"
#define k_AUI_TEXTFIELD_LDL_ISFILENAME	"isfilename"
#define k_AUI_TEXTFIELD_LDL_PASSWORD	"password"
#define k_AUI_TEXTFIELD_LDL_FONT		"fontname"
#define k_AUI_TEXTFIELD_LDL_FONTHEIGHT	"fontheight"
#define k_AUI_TEXTFIELD_LDL_MAXFIELDLEN	"maxfieldlen"


class aui_TextField : public aui_Win
{
public:

	aui_TextField(
		AUI_ERRCODE *retval,
		uint32 id,
		const MBCHAR *ldlBlock,
		ControlActionCallback *ActionFunc = NULL,
		void *cookie = NULL );
	aui_TextField(
		AUI_ERRCODE *retval,
		uint32 id,
		sint32 x,
		sint32 y,
		sint32 width,
		sint32 height,
		const MBCHAR *text = NULL,
		ControlActionCallback *ActionFunc = NULL,
		void *cookie = NULL );
	virtual ~aui_TextField();

protected:
	aui_TextField() : aui_Win() {}
	AUI_ERRCODE InitCommonLdl( const MBCHAR *ldlBlock );
	AUI_ERRCODE InitCommon(
		const MBCHAR *text,
		const MBCHAR *font,
		sint32 fontheight,
		BOOL multiLine,
		BOOL autovscroll = TRUE,
		BOOL autohscroll = TRUE,
		BOOL isfilename = FALSE,
		size_t maxFieldLen = 1024,
		BOOL passwordReady = FALSE);

public:
	size_t	GetFieldText(MBCHAR *text, size_t maxCount);
	BOOL	SetFieldText(const MBCHAR *text, size_t caretPos = -1);

#ifdef __AUI_USE_DIRECTX__
	BOOL	IsMultiLine( void ) const { return m_multiLine; }
	BOOL	SetMultiLine( BOOL multiLine );

	BOOL	IsPasswordReady( void ) const { return m_passwordReady; }
	BOOL	SetPasswordReady( BOOL passwordReady );
#endif

	BOOL	IsFileName( void ) const { return m_isFileName; }
	BOOL	SetIsFileName( BOOL isFileName );

	size_t	GetMaxFieldLen( void ) const { return m_maxFieldLen; }
	size_t	SetMaxFieldLen( size_t maxFieldLen );
	size_t  GetTextLength() { return strlen(m_Text); };

	virtual aui_Control	*SetKeyboardFocus( void );
	virtual AUI_ERRCODE	ReleaseKeyboardFocus( void );

	virtual AUI_ERRCODE	DrawThis(
		aui_Surface *surface,
		sint32 x,
		sint32 y );

	static WNDPROC	m_windowProc;
#ifdef __AUI_USE_DIRECTX__
	static void HitEnter( HWND hwnd );
	static BOOL IsFileName(HWND hwnd);
	static sint32 GetMaxFieldLen(HWND hwnd);
#else
	void HitEnter();
	virtual bool HandleKey(uint32 wParam);
#endif

	void SetSelection(size_t start, size_t end);
	void GetSelection(size_t *start, size_t *end);
	void SelectAll(void);

protected:
	BOOL	m_blink;
	BOOL	m_blinkThisFrame;

	BOOL	m_multiLine;
	BOOL	m_passwordReady;

	BOOL	m_isFileName;

	size_t	m_maxFieldLen;
	MBCHAR *m_Text;
	size_t  m_selStart;
	size_t  m_selEnd;
	size_t  m_viewStart;

#if defined(__AUI_USE_SDL__)
	size_t GetInvisibleNumToSelStart();
	void UpdateView();
#endif

public:
	sint32	m_textHeight;
	MBCHAR	m_desiredFont[256];
#if defined(__AUI_USE_DIRECTX__)
	HFONT	m_hfont;
	HFONT	m_holdfont;
#else
	aui_BitmapFont *m_Font;
	aui_BitmapFont *m_holdfont;
#endif

	virtual void	MouseLGrabOutside(aui_MouseEvent * mouseData);
	virtual void	PostChildrenCallback(aui_MouseEvent * mouseData);

#if defined(__AUI_USE_SDL__)
	void SelectWordStart();
	void SelectWordEnd();

	virtual void	MouseLDragInside(aui_MouseEvent * mouseData);
	virtual void	MouseLDragOutside(aui_MouseEvent * mouseData);
	virtual void	MouseLGrabInside(aui_MouseEvent * mouseData);
	virtual void	MouseLDoubleClickInside(aui_MouseEvent * mouseData);
#endif
};


#if defined(__AUI_USE_DIRECTX__)
LRESULT CALLBACK TextFieldWindowProc( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );
int CALLBACK EnumTextFontsProc( LOGFONT *lplf, TEXTMETRIC *lptm, DWORD dwType, LPARAM lParam );
#elif defined(__AUI_USE_SDL__)
// void TextFieldWindowProc(SDL_Event &event); //this does not exist and should process key-press events, using HandleKey instead similar to SDLMessageHandler (civ3_main.cpp) and aui_ListBox::HandleKey
#endif

#endif
