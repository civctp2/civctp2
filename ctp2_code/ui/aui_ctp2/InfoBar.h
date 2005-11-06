

#ifdef HAVE_PRAGMA_ONCE
#pragma once
#endif
#ifndef __INFOBAR_H__
#define __INFOBAR_H__


#include "c3window.h"


#define k_MAX_INFOBAR_TEXT 1024
#define k_INFOBAR_HEIGHT 17

class MapPoint;


class InfoBar : public C3Window 
{
private:
	MBCHAR m_str[k_MAX_INFOBAR_TEXT + 1];
	sint32 m_curX, m_curY;
	bool m_textFromMap;

public:
	InfoBar(
		AUI_ERRCODE *retval,
		uint32 id,
		sint32 x,
		sint32 y,
		sint32 width,
		sint32 height,
		sint32 bpp,
		MBCHAR *pattern );
	virtual ~InfoBar() {}

	virtual AUI_ERRCODE DrawThis(
		aui_Surface *surface = NULL,
		sint32 x = 0,
		sint32 y = 0 );

	AUI_ERRCODE DrawText(aui_Surface *surface, sint32 x, sint32 y);
	void SetText(const char *str);
	void SetTextFromMap(const MapPoint &point);

	MBCHAR *GetText(void) { return m_str; }

	void Clear();
	void Concat(const char *s);

	static void Initialize();
	static void Cleanup();
};

extern InfoBar *g_infoBar;

#endif 
