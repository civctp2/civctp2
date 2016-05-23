#include "c3.h"

#include "c3ui.h"

#include "pixelutils.h"
#include "colorset.h"           // g_colorSet
#include "primitives.h"

#include "ctp2_commandline.h"

#define k_CTP2_COMMANDLINE_FONT_FILE	"lucon.ttf"
#define k_CTP2_COMMANDLINE_FONT_SIZE	8
#define k_CTP2_COMMANDLINE_WIDTH_CHAR	"W"

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
	if (g_c3ui && m_font)
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
	case VK_TAB:
		break;
	case VK_RETURN:
		break;
	case VK_BACK:
			m_caret--;
			if (m_caret < 0)
				m_caret = 0;

			m_end--;
			if (m_end < 0)
				m_end = 0;

			m_string[m_end] = '\0';
		break;
	case VK_LEFT:
			m_caret--;
			if (m_caret < 0)
				m_caret = 0;
		break;
	case VK_RIGHT:
			m_caret++;
			if (m_caret > (k_MAX_COMMAND_LINE_LENGTH-1))
				m_caret = k_MAX_COMMAND_LINE_LENGTH-1;
		break;
	case VK_UP:
	case VK_HOME:
			m_caret = 0;
		break;
	case VK_DOWN:
	case VK_END:
			m_caret = m_end;
		break;
	case VK_DELETE:
		break;
	case VK_INSERT:
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
