#ifndef __C3_ICON_H__
#define __C3_ICON_H__

#include "c3_static.h"

class aui_Surface;

#include "colorset.h"
#include "tileset.h"

class c3_Icon : public c3_Static {
public:
	c3_Icon(AUI_ERRCODE *retval,
						uint32 id,
						MBCHAR *ldlBlock );
	c3_Icon(AUI_ERRCODE *retval,
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

	void SetMapIcon(MAPICON mapIcon) { m_mapIcon = mapIcon; }
	MAPICON GetMapIcon(void) { return m_mapIcon; }

	virtual AUI_ERRCODE DrawThis(aui_Surface *surface, sint32 x, sint32 y);

private:
	COLOR	m_color;
	MAPICON	m_mapIcon;
};

#endif
