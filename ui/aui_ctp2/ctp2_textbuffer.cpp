

#include "c3.h"

#include "c3ui.h"
extern C3UI		*g_c3ui;


#include "ctp2_textbuffer.h"

#define k_CTP2_TEXTBUFFER_FONT_FILE		"lucon.ttf"
#define k_CTP2_TEXTBUFFER_FONT_SIZE		8


#define k_CTP2_TEXTBUFFER_WIDTH_CHAR	"W"

extern ColorSet		*g_colorSet;


ctp2_TextBuffer::ctp2_TextBuffer(aui_Surface *destSurface, RECT *destRect, sint32 scrollbackLines)
{
	Assert(destSurface != NULL);
	if (destSurface == NULL) return;

	
	
	
	static MBCHAR descriptor[ k_AUI_BITMAPFONT_MAXDESCLEN + 1 ];

	aui_BitmapFont::AttributesToDescriptor(
		descriptor,
		k_CTP2_TEXTBUFFER_FONT_FILE,	
		k_CTP2_TEXTBUFFER_FONT_SIZE,	
		FALSE,							
		FALSE );						

	m_font = g_c3ui->LoadBitmapFont( descriptor );
	Assert( m_font != NULL );
	if ( !m_font ) return;

	
	m_charHeight = m_font->GetMaxHeight();
	m_charWidth = m_font->GetStringWidth("W");

	m_rect = *destRect;

	
	
	m_numDisplayedRows = (m_rect.right - m_rect.left) / m_charHeight;
	m_numDisplayedColumns = (m_rect.bottom - m_rect.top) / m_charWidth;

	m_maxRows = scrollbackLines;

	if (m_maxRows < m_numDisplayedRows)
		m_maxRows = m_numDisplayedRows;

	
	m_rowData = new MBCHAR *[m_maxRows];
	
	for (sint32 i=0; i<m_maxRows; i++) {
		m_rowData[i] = new MBCHAR [m_numDisplayedColumns];
		memset(m_rowData[i], 0, m_numDisplayedColumns);
	}

	m_surface = destSurface;

	m_topLine = m_numDisplayedRows - 1;
}


ctp2_TextBuffer::~ctp2_TextBuffer()
{
	if (m_font) 
		g_c3ui->UnloadBitmapFont(m_font);

	
	if (m_rowData) {
		for (sint32 i=0; i<m_maxRows; i++) {
			delete[] m_rowData[i];
		}

		delete[] m_rowData;
	}
}


void ctp2_TextBuffer::DrawLine(sint32 lineNum, COLOR color)
{
	aui_BitmapFont::GlyphInfo		*gi;
	MBCHAR							*s;
	MBCHAR							ch;
	RECT							destRect;
	COLORREF						colorRef = g_colorSet->GetColorRef(color);
	sint32							col;
	sint32							top;

	

	top = destRect.top = (m_topLine - lineNum + 1) * m_charHeight - m_charHeight;

	destRect.bottom = destRect.top + m_charHeight;
	destRect.left = m_rect.left;
	destRect.right = m_rect.left + m_charWidth;

	s = m_rowData[lineNum];
	col = 0;

	while (*s != '\0') {
		ch = *s;

		gi = m_font->GetGlyphInfo( ch );
		
		if (gi) {
			if (gi->c == ' ') {
				
			} else {
				OffsetRect(
					&destRect,
					gi->bearingX,
					gi->bearingY);

				m_font->RenderGlyph16(
					m_surface,		
					&destRect,		
					gi->surface,	
					&gi->bbox,		
					colorRef );		
				
				OffsetRect(
					&destRect,
					-gi->bearingX,
					-gi->bearingY);
			}
		}

		s++;
		col++;
		destRect.left += m_charWidth;
		destRect.right += m_charWidth;
	}
}


void ctp2_TextBuffer::Redraw(void)
{
	
	sint32		start = m_topLine;
	sint32		end = m_topLine - (m_numDisplayedRows - 1);

	for (sint32 i=start; i>=end; i--) {
		DrawLine(i);
	}
}


void ctp2_TextBuffer::Scroll(sint32 numLines)
{
	
	sint32		errcode;
	char		*buffer;
	sint32		deltaY = numLines * m_charHeight;

	errcode = m_surface->Lock(NULL, (LPVOID *)&buffer, 0);
	Assert(errcode == AUI_ERRCODE_OK);
	if (errcode != AUI_ERRCODE_OK) return;

	sint32		x = m_rect.left;
	sint32		y = m_rect.top;

	sint32		h = m_rect.bottom - m_rect.top,
				w = m_rect.right - m_rect.left,
				copyHeight = h - abs(deltaY);
	
	sint32		pitch = m_surface->Pitch();

	uint32		*srcPtr, *destPtr;
	sint32		dy = abs(deltaY);
	sint32		i,j;
	
	sint32		slop;
	char		*scrollBuffer = buffer + (pitch * y + x * 2);

	if (deltaY) {
		if (deltaY < 0) {
			srcPtr =	(uint32 *)(scrollBuffer + (pitch * (h - dy - 1)));
			destPtr =	(uint32 *)(scrollBuffer + (pitch * (h - 1)));
			slop = (pitch / 4) + (w / 2);
			for (i=0; i<copyHeight; i++) {
				for (j=0; j<w>>1; j++) {
					*destPtr++ = *srcPtr++;
				}
				srcPtr -= slop;
				destPtr -= slop;
			}
			for (i=0; i<dy; i++) {
				for (j=0; j<w>>1; j++) {
					*destPtr++ = 0x00000000;
				}
				destPtr -= slop;
			}
		} else {
			srcPtr =	(uint32 *)(scrollBuffer + (pitch * dy));
			destPtr =	(uint32 *)(scrollBuffer);
			slop = (pitch / 4) - (w / 2);
			for (i=0; i<copyHeight; i++) {
				for (j=0; j<w>>1; j++) {
					*destPtr++ = *srcPtr++;
				}
				srcPtr += slop;
				destPtr += slop;
			}
			for (i=0; i<dy; i++) {
				for (j=0; j<w>>1; j++) {
					*destPtr++ = 0x00000000;
				}
				destPtr += slop;
			}
		}
	}

	errcode = m_surface->Unlock(buffer);
	Assert(errcode == AUI_ERRCODE_OK);
	if (errcode != AUI_ERRCODE_OK) return;
}



void ctp2_TextBuffer::AddLine(MBCHAR *text, sint32 len, COLOR color)
{
	
	MBCHAR *lastLine = m_rowData[m_maxRows-1];
	memset(lastLine, 0, m_numDisplayedColumns);

	
	for (sint32 i=m_maxRows-1; i>0; i--) {
		m_rowData[i] = m_rowData[i-1];
	}

	
	m_rowData[0] = lastLine;

	if (m_numLines >= m_maxRows) {
		
	} else {
		
		m_numLines++;
	}

	if (len >= m_numDisplayedColumns)
		len = m_numDisplayedColumns - 1;

	memcpy(m_rowData[0], text, len);
	m_rowData[0][len] = '\0';

	
	if (m_topLine != (m_numDisplayedRows - 1)) {
		Redraw();
	} else {
		Scroll(1);
		DrawLine(0, color);
	}
}


void ctp2_TextBuffer::AddText(MBCHAR *text, COLOR color)
{
	MBCHAR		*start, *end;
	sint32		col;

	start = text;
	end = start;

	col = 0;
	while (*end != '\0') {
		if (*end == '\n') {
			AddLine(start, (end-start), color);
			start = end+1;
			col = 0;
		}
		
		
		if (col >= m_numDisplayedColumns) {
			
			sint32 tempCol = col;
			MBCHAR *tempEnd = end;

			while (tempCol > 0 && *end != '\n') {
				tempCol--;
				tempEnd--;
			}
			
			
			
			if (tempCol == 0) {
				end = start + (m_numDisplayedColumns - 1);
				AddLine(start, (end-start), color);
				col = 0;
			} else {
				col = tempCol;
				end = tempEnd;
				AddLine(start, (end-start), color);
				col = 0;
			}
		}

		end++;
		col++;
	}
	if (start != end) {
		AddLine(start, (end-start)+1, color);
	}
}


void ctp2_TextBuffer::PageUp(void)
{
	m_topLine = m_topLine + (m_numDisplayedRows - 5);
	
	if (m_topLine >= m_maxRows)
		m_topLine = m_maxRows - 1;
	
	Redraw();
}


void ctp2_TextBuffer::PageDown(void)
{
	m_topLine = m_topLine - (m_numDisplayedRows - 5);

	if (m_topLine < (m_numDisplayedRows - 1)) {
		m_topLine = m_numDisplayedRows - 1;
	}

	Redraw();
}
