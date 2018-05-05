#ifndef __CTP2_TEXTBUFFER__
#define __CTP2_TEXTBUFFER__

#include "ui/aui_common/aui_bitmapfont.h"
#include "ui/aui_common/aui_surface.h"
#include "gfx/gfx_utils/pixelutils.h"
#include "gfx/gfx_utils/colorset.h"

class ctp2_TextBuffer
{
public:
	ctp2_TextBuffer(aui_Surface *destSurface, RECT *destRect, sint32 scrollbackLines);
	~ctp2_TextBuffer();

	void DrawLine(sint32 lineNum, COLOR color = COLOR_WHITE);

	void Redraw(void);
	void Scroll(sint32 numLines);

	void AddLine(MBCHAR *line, sint32 len, COLOR color = COLOR_WHITE);
	void AddText(MBCHAR *text, COLOR color = COLOR_WHITE);

	void PageUp(void);
	void PageDown(void);

private:
	aui_BitmapFont	*m_font;
	aui_Surface		*m_surface;
	RECT			m_rect;
	sint32			m_charWidth;
	sint32			m_charHeight;

	sint32			m_numDisplayedRows;
	sint32			m_numDisplayedColumns;

	sint32			m_maxRows;

	sint32			m_topLine;

	sint32			m_curLine;
	sint32			m_numLines;
	MBCHAR			**m_rowData;
};

#endif
