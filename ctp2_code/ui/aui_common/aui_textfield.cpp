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

#ifdef __AUI_USE_SDL__
#include "aui_sdlsurface.h"
#include <string>
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
	m_Font( NULL ),
	m_Text( NULL ),
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

	sint32 maxFieldLen =
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
	sint32 maxFieldLen,
	BOOL passwordReady )
{
	m_blink = FALSE;
	m_blinkThisFrame = FALSE;
	m_multiLine = multiLine;
	m_isFileName = isfilename;
	m_maxFieldLen = maxFieldLen;
	m_passwordReady = passwordReady;
#ifdef __AUI_USE_DIRECTX__
	m_textHeight = 0;
	m_hfont = NULL;
	m_holdfont = NULL;
#else
	m_textHeight = 12;
	m_Font = NULL;
	m_holdfont = NULL;
#endif

	if (font) strcpy(m_desiredFont, font);
#ifdef __AUI_USE_DIRECTX__
	else strcpy(m_desiredFont, "\0");
#else
	else strcpy(m_desiredFont, "times.ttf");
#endif

	if ( !m_registered ) return AUI_ERRCODE_INVALIDPARAM;

#ifdef __AUI_USE_DIRECTX__
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

	if ( !m_windowProc )
		m_windowProc = (WNDPROC)GetWindowLong( m_hwnd, GWL_WNDPROC );

	SetWindowLong( m_hwnd, GWL_WNDPROC, (LONG)TextFieldWindowProc );

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
	m_selStart = m_selEnd = strlen(m_Text);

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
		SetWindowLong( m_hwnd, GWL_WNDPROC, (LONG)m_windowProc );

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

sint32 aui_TextField::GetFieldText( MBCHAR *text, sint32 maxCount )
{
#ifdef __AUI_USE_DIRECTX__
	return GetWindowText(m_hwnd, text, std::min(m_maxFieldLen, maxCount));
#else
	sint32 n = std::min(m_maxFieldLen,maxCount);
	if (n <= 0)
		return 0;
	strncpy(text, m_Text, n-1);
	text[n] = '\0';
	return strlen(text);
#endif
}

BOOL aui_TextField::SetFieldText( const MBCHAR *text )
{
	m_draw |= m_drawMask & k_AUI_REGION_DRAWFLAG_UPDATE;

#ifdef __AUI_USE_DIRECTX__
	BOOL success = SetWindowText( m_hwnd, text );

	if ( GetKeyboardFocus() == this ) SetFocus( m_hwnd );

	return success;
#else
	strncpy(m_Text, text, m_maxFieldLen);

	// select nothing, move insertion point to end
	m_selStart = m_selEnd = strlen(m_Text);

	if ( GetKeyboardFocus() == this ) g_winFocus = this;

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

sint32 aui_TextField::SetMaxFieldLen( sint32 maxFieldLen )
{
	sint32 prevMaxFieldLen = m_maxFieldLen;

	m_maxFieldLen = maxFieldLen;

#if 0 // not doing anything
	if (m_maxFieldLen != prevMaxFieldLen)
	{

	}
#ifndef __AUI_USE_DIRECTX__ // Merged in from Linux branch but I leave it as it was for Windows, see above
	char* newText = new char[maxFieldLen + 1];
	strncpy(newText, m_Text, maxFieldLen);
	newText[maxFieldLen] = '\0';
	delete[] m_Text;
	m_Text = newText;
#else
	fprintf(stderr, "%s L%d: SetMaxFieldLen doing nothing here!\n", __FILE__, __LINE__);
#endif
#endif

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

AUI_ERRCODE aui_TextField::DrawThis( aui_Surface *surface, sint32 x, sint32 y )
{
	if ( IsHidden() ) return AUI_ERRCODE_OK;

	if ( !surface ) surface = m_window->TheSurface();

	RECT rect = { 0, 0, m_width, m_height };
	RECT srcRect = rect;
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
	SDL_Surface* SDLsurf = static_cast<aui_SDLSurface*>(surface)->DDS();
	// fill background
	SDL_Rect r1 = { rect.left, rect.top, rect.right-rect.left, rect.bottom-rect.top };
	SDL_FillRect(SDLsurf, &r1, SDL_MapRGB(SDLsurf->format, 0xff, 0xff, 0xff));

	m_Font->DrawString(surface, &rect, &rect, m_Text,
	                   k_AUI_BITMAPFONT_DRAWFLAG_JUSTLEFT,
	                   RGB(20,20,20), 0);
	char save = m_Text[m_selStart];
	m_Text[m_selStart] = '\0';
	int offset = m_Font->GetStringWidth(m_Text);
	m_Text[m_selStart] = save;
	SDL_Rect r2 = { rect.left+offset-1, rect.top+2, 2, rect.bottom-rect.top-4 };
	SDL_FillRect(SDLsurf, &r2, 0);
#endif

	if ( surface == m_window->TheSurface() )
		m_window->AddDirtyRect( &rect );

	return AUI_ERRCODE_OK;
}

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

void aui_TextField::SetSelection(sint32 start, sint32 end)
{
#ifdef __AUI_USE_DIRECTX__
	SendMessage( m_hwnd, EM_SETSEL, (WPARAM)start, (LPARAM)end);
	UpdateWindow( m_hwnd );
#else
	m_selStart = start;
	m_selEnd = end;
#endif
}

void aui_TextField::GetSelection(sint32 *start, sint32 *end)
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
	SetSelection(9999, 9999);
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

	LRESULT lr = CallWindowProc(
		(long(__stdcall *)(HWND, unsigned int, unsigned int, long))aui_TextField::m_windowProc,
		hwnd, message, wParam, lParam );

	return lr;
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
		switch ( wParam )
		{
			// Have to handle the enter key here so that buffered input will
			// be handled correctly with the Windows message queue.
			case VK_RETURN:
				aui_TextField::HitEnter();
				break;
			// No tags allowed, they are for "tabbing focus" between controls.
			case VK_TAB:
				fprintf(stderr, "%s L%d: Tab ignored in TextField!\n", __FILE__, __LINE__);
				return false;
			case VK_BACK:
			{
				std::string str(m_Text); // char array to c++ string
				if( str.length() > 0 )
					str.pop_back(); //lop off character
					SetFieldText(str.c_str()); // c++ string to char array, use SetFieldText (not just modify m_Text) to cause re-drawing
					break;
			}
			case ' ':
			// fprintf(stderr, "%s L%d: space!\n", __FILE__, __LINE__);
			default:
			{ // append char to char array, apparently easiest with std::string

				static MBCHAR text[ 1025 ];
				GetFieldText( text, 1024 );
				// Don't let any more characters in if you're at the max.
				if ( (sint32)strlen( text ) >= GetMaxFieldLen() ) return 0;

				std::string str(m_Text); // char array to c++ string
				str += static_cast<char>(wParam); // append char to string
				SetFieldText(str.c_str()); // c++ string to char array, use SetFieldText (not just modify m_Text) to cause re-drawing
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
