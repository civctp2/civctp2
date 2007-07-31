

#ifndef __C3_COLOREDSTATIC_H__
#define __C3_COLOREDSTATIC_H__

#include "ColorSet.h"
#include "c3_Static.h"

class aui_Surface;


class c3_ColoredStatic : public c3_Static {
public:
	c3_ColoredStatic(AUI_ERRCODE *retval,
						uint32 id,
						MBCHAR *ldlBlock );
	c3_ColoredStatic(AUI_ERRCODE *retval,
						uint32 id,
						sint32 x,
						sint32 y,
						sint32 width,
						sint32 height,
						MBCHAR *pattern,
						MBCHAR *text,
						uint32 maxLength,
						uint32 bevelWidth,
						uint32 bevelType);


	AUI_ERRCODE InitCommonLdl( MBCHAR *ldlBlock );
	AUI_ERRCODE InitCommon(void);

	void SetColor(COLOR color) { m_color = color; }
	COLOR GetColor(void) { return m_color; }

	virtual AUI_ERRCODE DrawThis(aui_Surface *surface, sint32 x, sint32 y);

private:
	COLOR	m_color;
};


#endif
