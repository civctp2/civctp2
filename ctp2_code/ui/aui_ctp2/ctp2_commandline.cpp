


#include "c3.h"

#include "c3ui.h"

#include "pixelutils.h"
#include "colorset.h"
#include "primitives.h"

#include "ctp2_commandline.h"


#define k_CTP2_COMMANDLINE_FONT_FILE	"lucon.ttf"
#define k_CTP2_COMMANDLINE_FONT_SIZE	8
#define k_CTP2_COMMANDLINE_WIDTH_CHAR	"W"

extern ColorSet		*g_colorSet;

extern C3UI			*g_c3ui;


ctp2_CommandLine::ctp2_CommandLine(aui_Surface *surface, RECT *rect)
{
	m_surface = surface;
	m_rect = *rect;


	
	
	
	static MBCHAR descriptor[ k_AUI_BITMAPFONT_MAXDESCLEN + 1 ];

	aui_BitmapFont::AttributesToDescriptor(
		descriptor,
		k_CTP2_COMMANDLINE_FONT_FILE,	
		k_CTP2_COMMANDLINE_FONT_SIZE,	
		FALSE,							
		FALSE );						

	m_font = g_c3ui->LoadBitmapFont( descriptor );
	Assert( m_font != NULL );
	if ( !m_font ) return;

	memset(m_string, 0, k_MAX_COMMAND_LINE_LENGTH);

	m_caret = 0;
	m_end = 0;
}

ctp2_CommandLine::~ctp2_CommandLine()
{
	if (m_font) 
		g_c3ui->UnloadBitmapFont(m_font);
}

void ctp2_CommandLine::Draw(void)
{
	
	primitives_PaintRect16(m_surface, &m_rect, g_colorSet->GetColor(COLOR_BLACK));

	m_font->DrawString(m_surface, &m_rect, &m_rect, m_string);
}

void ctp2_CommandLine::HandleKeypress(WPARAM wParam, LPARAM lParam)
{
	switch (wParam) {
#ifdef __AUI_USE_SDL__
	case SDLK_TAB:
#else
	case VK_TAB:
#endif
		break;
#ifdef __AUI_USE_SDL__
	case SDLK_RETURN:
#else
	case VK_RETURN:
#endif
		break;
#ifdef __AUI_USE_SDL__
	case SDLK_BACKSPACE:
#else
	case VK_BACK:
#endif
			m_caret--;
			if (m_caret < 0)
				m_caret = 0;
			
			m_end--;
			if (m_end < 0)
				m_end = 0;
			
			m_string[m_end] = '\0';
		break;
#ifdef __AUI_USE_SDL__
	case SDLK_LEFT:
#else
	case VK_LEFT:
#endif
			m_caret--;
			if (m_caret < 0)
				m_caret = 0;
		break;
#ifdef __AUI_USE_SDL__
	case SDLK_RIGHT:
#else
	case VK_RIGHT:
#endif
			m_caret++;
			if (m_caret > (k_MAX_COMMAND_LINE_LENGTH-1))
				m_caret = k_MAX_COMMAND_LINE_LENGTH-1;
		break;
#ifdef __AUI_USE_SDL__
	case SDLK_UP:
	case SDLK_HOME:
#else
	case VK_UP:
	case VK_HOME:
#endif
			m_caret = 0;
		break;
#ifdef __AUI_USE_SDL__
	case SDLK_DOWN:
	case SDLK_END:
#else
	case VK_DOWN:
	case VK_END:
#endif
			m_caret = m_end;
		break;
#ifdef __AUI_USE_SDL__
	case SDLK_DELETE:
#else
	case VK_DELETE:
#endif
		break;
#ifdef __AUI_USE_SDL__
	case SDLK_INSERT:
#else
	case VK_INSERT:
#endif
		break;
	default: 
		{
			char		ch = (char)wParam;


			


















			
			if (m_caret + 1 < (k_MAX_COMMAND_LINE_LENGTH-1)) {
				m_string[m_caret] = ch;
				m_caret++;
				m_string[m_caret] = '\0';
				m_end++;
			}
		}
	}

	Draw();
}
