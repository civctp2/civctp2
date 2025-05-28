#include "c3.h"
#include "aui_textfield.h"

#include "aui_ui.h"
#include "aui_window.h"
#include "aui_surface.h"
#include "aui_ldl.h"
#include "aui_action.h"

#include "soundmanager.h"
#include "gamesounds.h"
#include "chatbox.h"

#include "ldl_data.hpp"

#if defined(__AUI_USE_SDL__)
#include "aui_sdlsurface.h"
#include <string>
#include <codecvt>
#include <locale>
#endif

WNDPROC aui_TextField::m_windowProc = NULL;
extern aui_Win* g_winFocus;

aui_TextField::aui_TextField(
	AUI_ERRCODE *retval,
	uint32 id,
	const MBCHAR *ldlBlock,
	ControlActionCallback *ActionFunc,
	void *cookie )
	:
	aui_ImageBase( ldlBlock ),
	aui_TextBase( ldlBlock, (const MBCHAR *)NULL ),
	aui_Win( retval, id, ldlBlock, ActionFunc, cookie ),
#ifndef __AUI_USE_DIRECTX__
	m_Font( NULL ),
#endif
	m_holdfont( NULL )
{
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;

	*retval = InitCommonLdl( ldlBlock );
	Assert( AUI_SUCCESS(*retval) );
}


aui_TextField::aui_TextField(
	AUI_ERRCODE *retval,
	uint32 id,
	sint32 x,
	sint32 y,
	sint32 width,
	sint32 height,
	const MBCHAR * text,
	ControlActionCallback *ActionFunc,
	void *cookie )
	:
	aui_ImageBase( (sint32)0 ),
	aui_TextBase( NULL ),
	aui_Win( retval, id, x, y, width, height, ActionFunc, cookie ),
#ifndef __AUI_USE_DIRECTX__
	m_Text( NULL ),
	m_Font( NULL ),
#endif
	m_holdfont( NULL )
{
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;

	*retval = InitCommon( text, NULL, 0, FALSE );
	Assert( AUI_SUCCESS(*retval) );
}


AUI_ERRCODE aui_TextField::InitCommonLdl( const MBCHAR *ldlBlock )
{
	ldl_datablock * block = aui_Ldl::FindDataBlock(ldlBlock);
	Assert( block != NULL );
	if ( !block ) return AUI_ERRCODE_LDLFINDDATABLOCKFAILED;

	const MBCHAR *text = block->GetString( k_AUI_TEXTFIELD_LDL_TEXT );
	BOOL multiLine = block->GetBool( k_AUI_TEXTFIELD_LDL_MULTILINE );
	BOOL autovscroll =
		block->GetAttributeType( k_AUI_TEXTFIELD_LDL_AUTOVSCROLL ) == ATTRIBUTE_TYPE_BOOL ?
		block->GetBool( k_AUI_TEXTFIELD_LDL_AUTOVSCROLL ) :
		TRUE;
	BOOL autohscroll =
		block->GetAttributeType( k_AUI_TEXTFIELD_LDL_AUTOHSCROLL ) == ATTRIBUTE_TYPE_BOOL ?
		block->GetBool( k_AUI_TEXTFIELD_LDL_AUTOHSCROLL ) :
		TRUE;
	BOOL isfilename = block->GetBool( k_AUI_TEXTFIELD_LDL_ISFILENAME );
	BOOL passwordReady = block->GetBool( k_AUI_TEXTFIELD_LDL_PASSWORD );
	const MBCHAR *font = block->GetString( k_AUI_TEXTFIELD_LDL_FONT );
	sint32 fontheight = block->GetInt( k_AUI_TEXTFIELD_LDL_FONT );

	size_t maxFieldLen =
		block->GetAttributeType( k_AUI_TEXTFIELD_LDL_MAXFIELDLEN ) == ATTRIBUTE_TYPE_INT ?
		block->GetInt( k_AUI_TEXTFIELD_LDL_MAXFIELDLEN ) :
		1024;

	AUI_ERRCODE errcode = InitCommon(
		text, font, fontheight,
		multiLine,
		autovscroll,
		autohscroll,
		isfilename,
		maxFieldLen,
		passwordReady );
	Assert( AUI_SUCCESS(errcode) );
	return errcode;
}

AUI_ERRCODE aui_TextField::InitCommon(
	const MBCHAR *text,
	const MBCHAR *font,
	sint32 fontheight,
	BOOL multiLine,
	BOOL autovscroll,
	BOOL autohscroll,
	BOOL isfilename,
	size_t maxFieldLen,
	BOOL passwordReady )
{
	m_blink = FALSE;
	m_blinkThisFrame = FALSE;
	m_multiLine = multiLine;
	m_isFileName = isfilename;
	m_maxFieldLen = maxFieldLen;
	m_passwordReady = passwordReady;
#if defined(__AUI_USE_DIRECTX__)
	m_textHeight = 0;
	m_hfont = NULL;
	m_holdfont = NULL;
#else
	m_textHeight = 12;
	m_Font = NULL;
	m_holdfont = NULL;
#endif

	if (font) strcpy(m_desiredFont, font);
#if defined(__AUI_USE_DIRECTX__)
	else strcpy(m_desiredFont, "\0");
#else
	else strcpy(m_desiredFont, "times.ttf");
#endif

#if defined(__AUI_USE_DIRECTX__)
	if ( !m_registered ) return AUI_ERRCODE_INVALIDPARAM;

	uint32 style = WS_CHILD;
	if ( m_multiLine )
	{
		style |= ES_MULTILINE;
		if ( autovscroll )
			style |= ES_AUTOVSCROLL;
	}
	else if ( autohscroll )
	{
		style |= ES_AUTOHSCROLL;
	}

	if ( m_passwordReady )
		style |= ES_PASSWORD;

	m_hwnd = CreateWindowEx(
		0,
		"EDIT",
		text ? text : "",
		style,
		m_offscreen.x,
		m_offscreen.y,
		m_width,
		m_height,
		g_ui->TheHWND(),
		NULL,
		g_ui->TheHINSTANCE(),
		NULL );
	Assert( m_hwnd != NULL );
	if ( !m_hwnd ) return AUI_ERRCODE_MEMALLOCFAILED;

	g_ui->AddWin( m_hwnd );

#if defined(_WIN64)
	if ( !m_windowProc )
		m_windowProc = (WNDPROC)GetWindowLongPtr( m_hwnd, GWLP_WNDPROC );

	SetWindowLongPtr( m_hwnd, GWLP_WNDPROC, (LONG_PTR)TextFieldWindowProc );
#else
	if (!m_windowProc)
		m_windowProc = (WNDPROC)GetWindowLong(m_hwnd, GWL_WNDPROC);

	SetWindowLong(m_hwnd, GWL_WNDPROC, (LONG)TextFieldWindowProc);
#endif

	UpdateWindow( m_hwnd );

	ShowWindow( m_hwnd, SW_SHOWNORMAL );

	HDC hdc = GetDC( m_hwnd );
	TEXTMETRIC tm;
	GetTextMetrics( hdc, &tm );

	SetBkColor(hdc, RGB(255, 255, 255));
	::SetTextColor(hdc, RGB(20, 20, 20));

	if (fontheight) m_textHeight = fontheight;
	else m_textHeight = tm.tmHeight + tm.tmExternalLeading;

	if (font) EnumFonts( hdc, font, (FONTENUMPROC) EnumTextFontsProc, (LPARAM)this);

	if (m_hfont)
	{

		m_holdfont = (HFONT)SendMessage( m_hwnd, WM_GETFONT, NULL, NULL );

		SendMessage( m_hwnd, WM_SETFONT, (WPARAM)m_hfont, MAKELPARAM(TRUE,0));

		::SetTextColor(hdc, RGB(20, 20, 20));
	}

	ReleaseDC( m_hwnd, hdc );
#else
	m_Text=new MBCHAR[m_maxFieldLen+1];
	m_Text[m_maxFieldLen] = '\0';
	if (text == NULL)
		*m_Text = '\0';
	else
		strncpy(m_Text, text, m_maxFieldLen);

	// select nothing, move insertion point to end
	m_selStart = m_selEnd = static_cast<sint32>(strlen(m_Text));
	m_viewStart = 0;

	// This supposed to set the font size as far as I understand it. However,
	// it doesn't do it without SetPointSize. At least it fixes an assert.
	m_Font = g_ui->LoadBitmapFont(m_desiredFont, k_AUI_TEXTBASE_DEFAULT_FONTSIZE);
	Assert(m_Font);
	// FIXME: HACK: I'm setting the font size here because it doesn't seem to be
	// being set anywhere else, which was causing textboxes to display no text.
	// With this fix they do display text, but it's usually of the wrong size.
	// More needs to be done on this problem

	// m_Font->SetMaxHeight(m_textHeight); //adjusting font to boxhight does not work
	m_Font->SetPointSize(k_AUI_TEXTBASE_DEFAULT_FONTSIZE);
	if (fontheight)
		m_textHeight = fontheight;
	else
		m_textHeight = m_Font->GetMaxHeight(); //well, let's set at least the box height to something
#endif

	sint32 newHeight = m_height - Mod(m_height,m_textHeight);
	if ( newHeight > 0 )
		Resize( m_width, m_height - Mod(m_height,m_textHeight) );
	else
		Resize( m_width, m_textHeight );

	return AUI_ERRCODE_OK;
}

aui_TextField::~aui_TextField()
{
#ifdef __AUI_USE_DIRECTX__
	if ( m_hfont )
	{
		SendMessage( m_hwnd, WM_SETFONT, (WPARAM)m_holdfont, MAKELPARAM(TRUE,0));

		DeleteObject( m_hfont );
		m_hfont = NULL;
	}

	if ( m_winRefCount == 1 && m_windowProc )
#if defined(_WIN64)
		SetWindowLongPtr(m_hwnd, GWLP_WNDPROC, (LONG_PTR)m_windowProc);
#else
		SetWindowLong(m_hwnd, GWL_WNDPROC, (LONG)m_windowProc);
#endif

	g_ui->RemoveWin(m_hwnd);
#else
	if (m_Font )
	{
		g_ui->UnloadBitmapFont(m_Font);;
		m_Font = NULL;
	}
	if(m_Text)
		delete[] m_Text;
#endif
}

size_t aui_TextField::GetFieldText( MBCHAR *text, size_t maxCount )
{
#ifdef __AUI_USE_DIRECTX__
	return GetWindowText(m_hwnd, text, std::min(m_maxFieldLen, maxCount));
#else
	size_t n = std::min(m_maxFieldLen, maxCount);

	strncpy(text, m_Text, n-1);
	text[n-1] = '\0';
	return strlen(text);
#endif
}

BOOL aui_TextField::SetFieldText(const MBCHAR *text, size_t caretPos)
{
	m_draw |= m_drawMask & k_AUI_REGION_DRAWFLAG_UPDATE;

#ifdef __AUI_USE_DIRECTX__
	BOOL success = SetWindowText( m_hwnd, text );

	if ( GetKeyboardFocus() == this ) SetFocus( m_hwnd );

	return success;
#else
	strncpy(m_Text, text, m_maxFieldLen);
	m_Text[m_maxFieldLen-1] = '\0'; // strncpy does not append '\0' if text is longer than m_maxFieldLen

	if (caretPos < 0 || caretPos > GetTextLength())
	{
		// select nothing, move insertion point to end
		m_selStart = m_selEnd = GetTextLength();
	}
	else
	{
		m_selStart = m_selEnd = caretPos;
	}

	if ( GetKeyboardFocus() == this ) g_winFocus = this;

	UpdateView();

	return TRUE;
#endif
}

#ifdef __AUI_USE_DIRECTX__
BOOL aui_TextField::SetMultiLine( BOOL multiLine )
{
	BOOL wasMultiLine = m_multiLine;

	if ( (m_multiLine = multiLine) != wasMultiLine )
	{
		uint32 style = GetWindowLong( m_hwnd, GWL_STYLE );
		SetWindowLong( m_hwnd, GWL_STYLE, style ^ ES_MULTILINE );
	}

	return wasMultiLine;
}

BOOL aui_TextField::SetPasswordReady( BOOL passwordReady )
{
	BOOL wasPasswordReady = m_passwordReady;

	if ( (m_passwordReady = passwordReady) != wasPasswordReady )
	{
		uint32 style = GetWindowLong( m_hwnd, GWL_STYLE );
		SetWindowLong( m_hwnd, GWL_STYLE, style ^ ES_PASSWORD );
	}

	return wasPasswordReady;
}
#endif // __AUI_USE_DIRECTX__

BOOL aui_TextField::SetIsFileName( BOOL isFileName )
{
	BOOL wasFileName = m_isFileName;

	if ( (m_isFileName = isFileName) != wasFileName )
	{

	}

	return wasFileName;
}

size_t aui_TextField::SetMaxFieldLen( size_t maxFieldLen )
{
	size_t prevMaxFieldLen = m_maxFieldLen;

	m_maxFieldLen = maxFieldLen;

	if (m_maxFieldLen != prevMaxFieldLen)
	{
#if !defined(__AUI_USE_DIRECTX__)
		char* newText = new char[maxFieldLen + 1];
		strncpy(newText, m_Text, maxFieldLen);
		newText[maxFieldLen] = '\0';
		delete[] m_Text;
		m_Text = newText;
#endif
	}

	return prevMaxFieldLen;
}

aui_Control *aui_TextField::SetKeyboardFocus( void )
{
	if ( !IsDisabled() )
	{
		m_blink = TRUE;
		m_blinkThisFrame = FALSE;
		m_startWaitTime = 0;
	}
	return aui_Win::SetKeyboardFocus();
}

AUI_ERRCODE aui_TextField::ReleaseKeyboardFocus( void )
{
	m_blink = FALSE;
	m_blinkThisFrame = FALSE;
	m_draw |= m_drawMask & k_AUI_REGION_DRAWFLAG_UPDATE;

	return aui_Win::ReleaseKeyboardFocus();
}

#ifdef __AUI_USE_DIRECTX__
void aui_TextField::HitEnter( HWND hwnd )
#else
void aui_TextField::HitEnter() // Is this ; intended?
#endif // __AUI_USE_DIRECTX__
{
#ifdef __AUI_USE_DIRECTX__
	aui_TextField *textfield = (aui_TextField *)GetWinFromHWND( hwnd );
	Assert( textfield != NULL );
	if ( !textfield ) return;

	if ( textfield->IsMultiLine() ) return;
#else
	aui_TextField *textfield = this;
#endif

	if ( textfield->GetActionFunc() )
		textfield->GetActionFunc()(
			textfield,
			AUI_TEXTFIELD_ACTION_EXECUTE,
			0,
			textfield->GetCookie() );
	else if ( textfield->GetAction() )
		textfield->GetAction()->Execute(
			textfield,
			AUI_TEXTFIELD_ACTION_EXECUTE,
			0 );
}

#if defined(__AUI_USE_DIRECTX__)
BOOL aui_TextField::IsFileName( HWND hwnd )
{
	aui_TextField *textfield = (aui_TextField *)GetWinFromHWND( hwnd );
	Assert( textfield != NULL );
	if ( !textfield ) return FALSE;

	return textfield->IsFileName();
}

sint32 aui_TextField::GetMaxFieldLen( HWND hwnd )
{
	aui_TextField *textfield = (aui_TextField *)GetWinFromHWND( hwnd );
	Assert( textfield != NULL );
	if ( !textfield ) return FALSE;

	return textfield->GetMaxFieldLen();
}
#endif // __AUI_USE_DIRECTX__

AUI_ERRCODE aui_TextField::DrawThis( aui_Surface *surface, sint32 x, sint32 y )
{
	if ( IsHidden() ) return AUI_ERRCODE_OK;

	if ( !surface ) surface = m_window->TheSurface();

	RECT rect = { 0, 0, m_width, m_height };
#ifdef __AUI_USE_DIRECTX__
	RECT srcRect = rect;
#endif
	OffsetRect( &rect, m_x + x, m_y + y );
	ToWindow( &rect );

#ifdef __AUI_USE_DIRECTX__
	if ( m_hwnd && m_memdc )
	{

		FillRect( m_memdc, &srcRect, (HBRUSH)(COLOR_WINDOW + 1) );

		InvalidateRect( m_hwnd, NULL, FALSE );
		SendMessage( m_hwnd, WM_PAINT, (WPARAM)m_memdc, 0 );

		static POINT point = { 0, 0 };
		if ( GetKeyboardFocus() == this && GetCaretPos( &point ) )
		{
			static POINT lastPoint = point;

			if ( m_blink || memcmp( &point, &lastPoint, sizeof( point ) ) )
			{
				BitBlt(
					m_memdc,
					point.x,
					point.y,
					2,
					m_textHeight,
					m_memdc,
					0,
					0,
					PATINVERT );
			}

			memcpy( &lastPoint, &point, sizeof( lastPoint ) );
		}

		HDC destDC = NULL;
		AUI_ERRCODE errcode = surface->GetDC( &destDC );
		Assert( errcode == AUI_ERRCODE_OK );

		if ( destDC )
		{
			::SetTextColor(destDC, RGB(20, 20, 20));

			BitBlt(
				destDC,
				rect.left,
				rect.top,
				rect.right - rect.left,
				rect.bottom - rect.top,
				m_memdc,
				0,
				0,
				SRCCOPY );

			errcode = surface->ReleaseDC( destDC );
			Assert( errcode == AUI_ERRCODE_OK );
		}
	}
#elif defined(__AUI_USE_SDL__)

#if defined(WIN32)
	COLORREF winColor              = GetSysColor(COLOR_WINDOW);
	COLORREF winTextColor          = GetSysColor(COLOR_WINDOWTEXT);
	COLORREF winHighLightColor     = GetSysColor(COLOR_HIGHLIGHT);
	COLORREF winHighLightTextColor = GetSysColor(COLOR_HIGHLIGHTTEXT);
#else
	// Maybe there is also a method on Linux for getting the colorefs by name
	COLORREF winColor              = RGB(0xff, 0xff, 0xff);
	COLORREF winTextColor          = RGB(20, 20, 20);
	COLORREF winHighLightColor     = RGB(0x00, 0x78, 0xd7);
	COLORREF winHighLightTextColor = RGB(0xff, 0xff, 0xff);
#endif

	SDL_Surface* SDLsurf = static_cast<aui_SDLSurface*>(surface)->DDS();

	MBCHAR* text     =  m_Text     + m_viewStart;
	size_t  selStart = (m_selStart > m_viewStart) ? m_selStart - m_viewStart : 0;
	size_t  selEnd   = (m_selEnd   > m_viewStart) ? m_selEnd   - m_viewStart : 0;

	// Fill background
	SDL_Rect r1 = { rect.left, rect.top, rect.right-rect.left, rect.bottom-rect.top };
	SDL_FillRect(SDLsurf, &r1, SDL_MapRGB(SDLsurf->format, GetRValue(winColor), GetGValue(winColor), GetBValue(winColor)));

	m_Font->DrawString(surface, &rect, &rect, text,
		k_AUI_BITMAPFONT_DRAWFLAG_JUSTLEFT,
		winTextColor, 0, selStart, selEnd, winHighLightTextColor, winHighLightColor);

	// Calculate caret position
	// ToDo: Implement multiline
	char save    = text[selEnd];
	text[selEnd] = '\0';
	int offset   = m_Font->GetStringWidth(text);
	text[selEnd] = save;

	// Draw blinking caret
	if (m_blink && GetKeyboardFocus() == this)
	{
		SDL_Rect r2 = { rect.left + offset - 1, rect.top + 2, 2, rect.bottom - rect.top - 4 };
		SDL_FillRect(SDLsurf, &r2, SDL_MapRGB(SDLsurf->format, GetRValue(winTextColor), GetGValue(winTextColor), GetBValue(winTextColor)));
	}
#endif

	if ( surface == m_window->TheSurface() )
		m_window->AddDirtyRect( &rect );

	return AUI_ERRCODE_OK;
}

#if defined(__AUI_USE_SDL__)
size_t aui_TextField::GetInvisibleNumToSelStart()
{
	RECT  rect   = { 0, 0, m_width, m_height };
	POINT penPos = { 0, 0 };

	const MBCHAR* start = m_Text + m_viewStart;
	const MBCHAR*  stop = m_Text + m_selEnd;

	m_Font->GetLineInfo(&rect, &penPos, NULL, NULL, &start, stop, true, true);

	return stop - start;
}

void aui_TextField::UpdateView()
{
	if(m_selEnd <= m_viewStart)
	{
		m_viewStart = m_selEnd;
	}
	else
	{
		size_t invisibleNum = GetInvisibleNumToSelStart();
		if(invisibleNum > 0)
		{
			m_viewStart += invisibleNum;
		}
	}

	m_blink = TRUE;
}
#endif

void aui_TextField::PostChildrenCallback( aui_MouseEvent *mouseData )
{
	if ( !mouseData->framecount
	&&   GetKeyboardFocus() == this )
	{
#ifdef __AUI_USE_DIRECTX__
		if ( GetFocus() != m_hwnd ) SetFocus( m_hwnd );
#else
		g_winFocus = this;
#endif

		m_blinkThisFrame = TRUE;
		m_draw |= m_drawMask & k_AUI_REGION_DRAWFLAG_UPDATE;
	}

	if ( m_blinkThisFrame && mouseData->time - m_startWaitTime > m_timeOut )
	{
		m_blinkThisFrame = FALSE;
		m_startWaitTime = mouseData->time;
		m_blink = !m_blink;
	}
}

#if defined(__AUI_USE_SDL__)
void aui_TextField::MouseLDragInside(aui_MouseEvent * mouseData)
{
	if ( IsDisabled() ) return;
	aui_Win::MouseLDragInside(mouseData);
	POINT mousePos = mouseData->position;
	ToWindow(&mousePos);

	RECT  rect   = { 0, 0, mousePos.x, m_height };
	POINT penPos = { 0, 0 };

	const MBCHAR* start = m_Text + m_viewStart;
	const MBCHAR*  stop = m_Text + GetTextLength();

	m_Font->GetLineInfo(&rect, &penPos, NULL, NULL, &start, stop, true, true);

	m_selEnd = start - m_Text;

	UpdateView();
}

void aui_TextField::MouseLDragOutside(aui_MouseEvent * mouseData)
{
	if ( IsDisabled() ) return;
	aui_Win::MouseLDragOutside(mouseData);
	POINT mousePos = mouseData->position;
	ToWindow(&mousePos);

	RECT  rect   = { 0, 0, mousePos.x, m_height };
	POINT penPos = { 0, 0 };

	const MBCHAR* start = m_Text + m_viewStart;
	const MBCHAR*  stop = m_Text + GetTextLength();

	m_Font->GetLineInfo(&rect, &penPos, NULL, NULL, &start, stop, true, true);

	m_selEnd = start - m_Text;

	UpdateView();
}

void aui_TextField::MouseLGrabInside(aui_MouseEvent * mouseData)
{
	if ( IsDisabled() ) return;
	aui_Win::MouseLGrabInside(mouseData);
	POINT mousePos = mouseData->position;
	ToWindow(&mousePos);

	RECT  rect   = { 0, 0, mousePos.x, m_height };
	POINT penPos = { 0, 0 };

	const MBCHAR* start = m_Text + m_viewStart;
	const MBCHAR*  stop = m_Text + GetTextLength();

	m_Font->GetLineInfo(&rect, &penPos, NULL, NULL, &start, stop, true, true);

	m_selEnd = start - m_Text;

	if(!mouseData->IsSetLeftShift()
	&& !mouseData->IsSetRightShift())
	{
		m_selStart = m_selEnd;
	}

	UpdateView();
}

void aui_TextField::MouseLDoubleClickInside(aui_MouseEvent * mouseData)
{
	if ( IsDisabled() ) return;
	aui_Win::MouseLGrabInside(mouseData);
	POINT mousePos = mouseData->position;
	ToWindow(&mousePos);

	RECT  rect   = { 0, 0, mousePos.x, m_height };
	POINT penPos = { 0, 0 };

	const MBCHAR* start = m_Text + m_viewStart;
	const MBCHAR*  stop = m_Text + GetTextLength();

	m_Font->GetLineInfo(&rect, &penPos, NULL, NULL, &start, stop, true, true);

	// Get the system preferred locale
	SDL_Locale* locales = SDL_GetPreferredLocales();

	std::locale loc;
	try
	{
		loc = std::locale(locales[0].language);
	}
	catch(std::runtime_error&)
	{
		// Nothing to do here we have a valid locale, just not the one we want
	}

	SDL_free(locales);

	const MBCHAR* wordStart = start;
	for(; wordStart > m_Text && std::isalnum(*wordStart, loc); wordStart--)
	{
		// Nothing to do
	}

	// If we are at the beginning and it is a word character, we don't add if space or something else we must add one
	m_selStart = wordStart - m_Text + (std::isalnum(*wordStart, loc) ? 0 : 1);

	if(m_selStart < m_viewStart)
		m_viewStart = m_selStart;

	const MBCHAR* wordEnd = start;
	for(; wordEnd < stop && std::isalnum(*wordEnd, loc); wordEnd++)
	{
		// Nothing to do
	}

	m_selEnd = wordEnd - m_Text;

	UpdateView();
}

void aui_TextField::SelectWordStart()
{
	// Get the system preferred locale
	SDL_Locale* locales = SDL_GetPreferredLocales();

	std::locale loc;
	try
	{
		loc = std::locale(locales[0].language);
	}
	catch(std::runtime_error&)
	{
		// Nothing to do here we have a valid locale, just not the one we want
	}

	SDL_free(locales);

	const MBCHAR* wordStart = m_Text + m_selEnd;
	const MBCHAR*      stop = m_Text + GetTextLength();

	bool isNotAlphaNum = true;
	for(; wordStart > m_Text && (std::isalnum(*wordStart, loc) || isNotAlphaNum); wordStart--)
	{
		if(isNotAlphaNum)
			isNotAlphaNum = !std::isalnum(*wordStart, loc);

		m_selEnd--;
	}
}

void aui_TextField::SelectWordEnd()
{
	// Get the system preferred locale
	SDL_Locale* locales = SDL_GetPreferredLocales();

	std::locale loc;
	try
	{
		loc = std::locale(locales[0].language);
	}
	catch(std::runtime_error&)
	{
		// Nothing to do here we have a valid locale, just not the one we want
	}

	SDL_free(locales);

	const MBCHAR* wordEnd = m_Text + m_selEnd;
	const MBCHAR*    stop = m_Text + GetTextLength();

	bool isNotAlphaNum = true;
	for(; wordEnd < stop && (std::isalnum(*wordEnd, loc) || isNotAlphaNum); wordEnd++)
	{
		if(isNotAlphaNum)
			isNotAlphaNum = !std::isalnum(*wordEnd, loc);

		m_selEnd++;
	}
}

#endif

void aui_TextField::MouseLGrabOutside( aui_MouseEvent *mouseData )
{
	if ( IsDisabled() ) return;
	aui_Win::MouseLGrabOutside( mouseData );

	if ( GetActionFunc() )
		GetActionFunc()(
			this,
			AUI_TEXTFIELD_ACTION_DISMISS,
			0,
			GetCookie() );
	else if ( GetAction() )
		GetAction()->Execute(
			this,
			AUI_TEXTFIELD_ACTION_DISMISS,
			0 );
}

void aui_TextField::SetSelection(size_t start, size_t end)
{
#ifdef __AUI_USE_DIRECTX__
	SendMessage( m_hwnd, EM_SETSEL, (WPARAM)start, (LPARAM)end);
	UpdateWindow( m_hwnd );
#else
	size_t textLength = GetTextLength();

	m_selStart = std::min(start, textLength);
	m_selEnd   = std::min(end,   textLength);
#endif
}

void aui_TextField::GetSelection(size_t *start, size_t *end)
{
#ifdef __AUI_USE_DIRECTX__
	SendMessage(m_hwnd, EM_GETSEL, (WPARAM)start, (LPARAM)end);
#else
	*start = m_selStart;
	*end = m_selEnd;
#endif
}

void aui_TextField::SelectAll(void)
{
	SetSelection(0, GetTextLength());
}

#ifdef __AUI_USE_DIRECTX__
LRESULT CALLBACK TextFieldWindowProc( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam )
{
	switch ( message )
	{
	case WM_CHAR:
		{
		MBCHAR	ch = (MBCHAR)wParam;
		if ( aui_TextField::IsFileName( hwnd ) ) {

			switch (ch) {
			case '\\':
			case '*':
			case '"':
			case '/':
			case ':':
			case '|':
			case '?':
			case '<':
			case '>':
				return 0;
			}
		}

		// Don't let any more characters in if you're at the max.
		// Backspace is ok.  FIXME - are there any other legal characters here?
		static MBCHAR text[ 1025 ];
		GetWindowText( hwnd, text, 1024 );
		if ( (sint32)strlen( text ) >= aui_TextField::GetMaxFieldLen( hwnd )
			&& ch != VK_BACK ) return 0;
		}
		break;
	case WM_KEYDOWN:
		switch ( wParam )
		{
		// Have to handle the enter key here so that buffered input will
		// be handled correctly with the Windows message queue.
		case VK_RETURN:
			aui_TextField::HitEnter( hwnd );
			break;
		// No tags allowed, they are for "tabbing focus" between controls.
		case VK_TAB:
			return 0;
		}
		break;

	case WM_KEYUP:
		g_soundManager->AddGameSound(GAMESOUNDS_EDIT_TEXT);
		break;
	case WM_RBUTTONDOWN:
	case WM_RBUTTONUP:
		// Disable the popup window that normally occurs over Windows edit fields.
		return 0;
	}

#if defined(_WIN64)
	return CallWindowProc(
		(__int64(__stdcall *)(HWND, unsigned int, unsigned __int64, __int64))aui_TextField::m_windowProc,
		hwnd, message, wParam, lParam);
#else
	return CallWindowProc(
		(long(__stdcall *)(HWND, unsigned int, unsigned int, long))aui_TextField::m_windowProc,
		hwnd, message, wParam, lParam );
#endif
}

// Font enumeration proc
int CALLBACK EnumTextFontsProc( LOGFONT *lplf, TEXTMETRIC *lptm, DWORD dwType, LPARAM lParam )
{
	double fraction;

	// get the textfield info
	aui_TextField *tf = (aui_TextField *)lParam;

	// if no text field is given, use the system fonts
	if (!tf)
	{
		// reset the font to NULL
		tf->m_hfont = NULL;
		return 0;
	}

	// if the desired font is found set the hfont
	if ( stricmp( lplf->lfFaceName, tf->m_desiredFont ) == 0 )
	{
		fraction = (double)tf->m_textHeight / (double)lplf->lfHeight;
		lplf->lfHeight = tf->m_textHeight;
		lplf->lfWidth = LONG(lplf->lfWidth * fraction);
		tf->m_hfont = CreateFontIndirect( lplf );
		tf->m_textHeight = lplf->lfHeight;
		return 0;
	}

	return TRUE;
}
#elif defined(__AUI_USE_SDL__)
/* 
Handling key input for text field with SDL

while it seems that directx/windows can provide a callback function (TextFieldWindowProc) to the window manager input callback (CallWindowProc)
SDL needs basic key handling (or use GUI lib), see e.g.:
http://lazyfoo.net/tutorials/SDL/32_text_input_and_clipboard_handling/index.php
https://wiki.libsdl.org/Tutorials/TextInput

civ3_main.cpp has the SDLMessageHandler which extracts the char from the key pressed from SDL_Event &event and converts to wParam
this is then processed by ui_HandleKeypress (keypress.cpp)
where first HandleKey of the topWindow is called, which passes HandleKey to child elements of aui_control, as is aui_win and parent of aui_textfield
here in aui_TextField HandleKey of aui_control is overwritten such that the keys are appended to the current text field string
 */
bool aui_TextField::HandleKey(uint32 wParam)
{
	if ( GetKeyboardFocus() == this )
	{
		// That's a bit hacky, but works
		// We simply translate what we get for Ctrl+c, Ctrl+x, and Ctrl+v
		// from SDLMessageHandler in civ3_main.cpp.
		switch(wParam)
		{
			case 'a' - 'a' + 1:
				wParam = SDLK_SELECT;
				break;
			case 'c' - 'a' + 1:
				wParam = SDLK_COPY;
				break;
			case 'x' - 'a' + 1:
				wParam = SDLK_CUT;
				break;
			case 'v' - 'a' + 1:
				wParam = SDLK_PASTE;
				break;
			case VK_BACK:
			case '\t' + 128:
				break;
			default:
				if(wParam < ' ')
					return true;
				break;
		}

		switch ( wParam )
		{
			// Have to handle the enter key here so that buffered input will
			// be handled correctly with the Windows message queue.
			case VK_RETURN + 128: // Set to VK_RETURN + 128 to hit escape rules in keymap
				aui_TextField::HitEnter();
				break;
			// No tags allowed, they are for "tabbing focus" between controls.
			case '\t' + 128:
			//	fprintf(stderr, "%s L%d: Tab ignored in TextField!\n", __FILE__, __LINE__);
				return false;
			case VK_BACK:
			{
				std::string str(m_Text); // char array to c++ string
				if (m_selStart == m_selEnd)
				{
					if (str.length() > 0 && m_selStart > 0)
					{
						if (str.length() <= m_selStart)
						{
							str.pop_back(); //lop off character
							SetFieldText(str.c_str()); // c++ string to char array, use SetFieldText (not just modify m_Text) to cause re-drawing
						}
						else
						{
							m_selStart--;
							m_selEnd--;
							str.erase(m_selStart, 1);
							SetFieldText(str.c_str(), m_selStart); // c++ string to char array, use SetFieldText (not just modify m_Text) to cause re-drawing
						}
					}
				}
				else if(m_selEnd > m_selStart)
				{
					str.erase(m_selStart, m_selEnd - m_selStart);
					SetFieldText(str.c_str(), m_selStart); // c++ string to char array, use SetFieldText (not just modify m_Text) to cause re-drawing
					m_selEnd = m_selStart;
				}
				else if(m_selEnd < m_selStart)
				{
					str.erase(m_selEnd, m_selStart - m_selEnd);
					SetFieldText(str.c_str(), m_selEnd); // c++ string to char array, use SetFieldText (not just modify m_Text) to cause re-drawing
					m_selStart = m_selEnd;
				}
				else
				{
					Assert(0);
				}
				break;
			}
			case SDLK_DELETE:
			{
				std::string str(m_Text); // char array to c++ string
				if(m_selStart == m_selEnd)
				{
					if (str.length() > 0 && static_cast<size_t>(m_selStart) < str.length())
					{
						str.erase(m_selStart, 1);
						SetFieldText(str.c_str(), m_selStart); // c++ string to char array, use SetFieldText (not just modify m_Text) to cause re-drawing
					}
				}
				else if(m_selEnd > m_selStart)
				{
					str.erase(m_selStart, m_selEnd - m_selStart);
					SetFieldText(str.c_str(), m_selStart); // c++ string to char array, use SetFieldText (not just modify m_Text) to cause re-drawing
					m_selEnd = m_selStart;
				}
				else if(m_selEnd < m_selStart)
				{
					str.erase(m_selEnd, m_selStart - m_selEnd);
					SetFieldText(str.c_str(), m_selEnd); // c++ string to char array, use SetFieldText (not just modify m_Text) to cause re-drawing
					m_selStart = m_selEnd;
				}
				else
				{
					Assert(0);
				}
				break;
			}
			case SDLK_UP    + 256:
		//		if(m_multiLine) /* ToDo: Implement multiline */;
				break;
			case SDLK_DOWN  + 256:
		//		if (m_multiLine) /* ToDo: Implement multiline */;
				break;
			case SDLK_LEFT  + 256:
				if(m_selStart == m_selEnd)
				{
					if(m_selEnd > 0) m_selEnd--;

					m_selStart = m_selEnd;
				}
				else if(m_selEnd > m_selStart)
				{
					m_selEnd = m_selStart;
				}
				else if(m_selEnd < m_selStart)
				{
					m_selStart = m_selEnd;
				}

				UpdateView();
				break;
			case SDLK_RIGHT + 256:
				if(m_selStart == m_selEnd)
				{
					if(strlen(m_Text) > m_selEnd) m_selEnd++;
					m_selStart = m_selEnd;
				}
				else if(m_selEnd > m_selStart)
				{
					m_selStart = m_selEnd;
				}
				else if(m_selEnd < m_selStart)
				{
					m_selEnd = m_selStart;
				}

				UpdateView();
				break;
			case SDLK_UP    + 512:
				//		if(m_multiLine) /* ToDo: Implement multiline */;
				break;
			case SDLK_DOWN  + 512:
				//		if (m_multiLine) /* ToDo: Implement multiline */;
				break;
			case SDLK_LEFT  + 512:
				if(m_selEnd > 0) m_selEnd--;
				UpdateView();
				break;
			case SDLK_RIGHT + 512:
				if(strlen(m_Text) > m_selEnd) m_selEnd++;
				UpdateView();
				break;
			case SDLK_UP    + 768:
				//		if(m_multiLine) /* ToDo: Implement multiline */;
				break;
			case SDLK_DOWN  + 768:
				//		if (m_multiLine) /* ToDo: Implement multiline */;
				break;
			case SDLK_LEFT  + 768:
				SelectWordStart();
				m_selStart = m_selEnd;
				UpdateView();
				break;
			case SDLK_RIGHT + 768:
				SelectWordEnd();
				m_selStart = m_selEnd;
				UpdateView();
				break;
			case SDLK_UP    + 1024:
				//		if(m_multiLine) /* ToDo: Implement multiline */;
				break;
			case SDLK_DOWN  + 1024:
				//		if (m_multiLine) /* ToDo: Implement multiline */;
				break;
			case SDLK_LEFT  + 1024:
				SelectWordStart();
				UpdateView();
				break;
			case SDLK_RIGHT + 1024:
				SelectWordEnd();
				UpdateView();
				break;
			case SDLK_SELECT:
				SelectAll();
				break;
			case SDLK_COPY:
			{
				if(m_selStart != m_selEnd)
				{
					size_t start  = (m_selStart < m_selEnd) ? m_selStart : m_selEnd;
					size_t end    = (m_selStart < m_selEnd) ? m_selEnd : m_selStart;
					size_t length = end - start;

					std::string str(m_Text);
					SDL_SetClipboardText(str.substr(start, length).c_str());
				}

				break;
			}
			case SDLK_CUT:
			{
				if(m_selStart != m_selEnd)
				{
					size_t start  = (m_selStart < m_selEnd) ? m_selStart : m_selEnd;
					size_t end    = (m_selStart < m_selEnd) ? m_selEnd : m_selStart;
					size_t length = end - start;

					std::string str(m_Text);
					SDL_SetClipboardText(str.substr(start, length).c_str());

					str.erase(start, length);
					SetFieldText(str.c_str(), start); // c++ string to char array, use SetFieldText (not just modify m_Text) to cause re-drawing
					m_selStart = start;
					m_selEnd = start;
				}

				break;
			}
			case SDLK_PASTE:
			{
				if(!SDL_HasClipboardText())
					break;

				char* clipboardText = SDL_GetClipboardText();

				size_t selLength = (m_selStart < m_selEnd) ? m_selEnd - m_selStart: m_selStart - m_selEnd;

				if(GetTextLength() + strlen(clipboardText) - selLength >= GetMaxFieldLen() - 1)
				{
					SDL_free(clipboardText);
					break;
				}

				std::string str(m_Text);

				if(m_selStart != m_selEnd)
				{
					size_t start  = (m_selStart < m_selEnd) ? m_selStart : m_selEnd;
					size_t end    = (m_selStart < m_selEnd) ? m_selEnd : m_selStart;
					size_t length = end - start;

					str.erase(start, length);
					m_selStart = start;
					m_selEnd = start;
				}

				str.insert(m_selStart, clipboardText);
				SetFieldText(str.c_str(), m_selStart + strlen(clipboardText)); // c++ string to char array, use SetFieldText (not just modify m_Text) to cause re-drawing

				SDL_free(clipboardText);

				break;
			}
			case ' ':
			// fprintf(stderr, "%s L%d: space!\n", __FILE__, __LINE__);
			default:
			{ // append char to char array, apparently easiest with std::string

				size_t selLength = (m_selStart < m_selEnd) ? m_selEnd - m_selStart: m_selStart - m_selEnd;
				// Don't let any more characters in if you're at the max.
				if ( (sint32)strlen( m_Text ) - selLength >= GetMaxFieldLen() - 1) break;

#if 0
				// Attempt to put unicode characters in, but does not work
				// Maybe the whole thing has to be converted
				std::string source(m_Text); // char array to c++ string
				std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
				std::wstring wide_str = converter.from_bytes(source);

				wide_str += static_cast<wchar_t>(wParam); // Append wide char to wide string
				std::string str = converter.to_bytes(wide_str);
#else
				std::string str(m_Text); // char array to c++ string

				if (m_selEnd > m_selStart)
				{
					str.erase(m_selStart, m_selEnd - m_selStart);
					m_selEnd = m_selStart;
				}
				else if(m_selEnd < m_selStart)
				{
					str.erase(m_selEnd, m_selStart - m_selEnd);
					m_selStart = m_selEnd;
				}


				str.insert(m_selStart, 1, static_cast<char>(wParam));
				m_selStart++;
				m_selEnd++;
#endif
				SetFieldText(str.c_str(), m_selStart); // c++ string to char array, use SetFieldText (not just modify m_Text) to cause re-drawing
				g_soundManager->AddGameSound(GAMESOUNDS_EDIT_TEXT);// play key sound ;-)
				break;
			}
		}
		return true;
	}
	else
	{
		return false;
	}
}
#endif
