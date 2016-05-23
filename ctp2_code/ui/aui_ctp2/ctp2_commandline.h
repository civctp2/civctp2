#ifndef __CTP2_COMMANDLINE_H__
#define __CTP2_COMMANDLINE_H__

#include "aui_surface.h"
#include "aui_bitmapfont.h"

#define k_MAX_COMMAND_LINE_LENGTH		256

class ctp2_CommandLine
{
public:
	ctp2_CommandLine(aui_Surface *surface, RECT *clRect);
	~ctp2_CommandLine();

	void	Draw(void);

	void	HandleKeypress(WPARAM wParam, LPARAM lParam);

	RECT	*GetRect(void) { return &m_rect; }
	MBCHAR	*GetString(void) { return m_string; }

private:
	aui_BitmapFont		*m_font;
	aui_Surface			*m_surface;
	RECT				m_rect;
	MBCHAR				m_string[k_MAX_COMMAND_LINE_LENGTH];
	sint32				m_caret;
	sint32				m_end;
};

#endif
