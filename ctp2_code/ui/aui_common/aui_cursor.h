#ifndef __AUI_CURSOR_H__
#define __AUI_CURSOR_H__

#include "aui_image.h"


class aui_Cursor : public aui_Image
{
public:

	aui_Cursor(
		AUI_ERRCODE *retval,
		MBCHAR const * filename = NULL );
	virtual ~aui_Cursor() {}

protected:
	aui_Cursor() : aui_Image() {}
	AUI_ERRCODE InitCommon( void );

public:
	AUI_ERRCODE	SetHotspot( sint32 x, sint32 y );
	AUI_ERRCODE	GetHotspot( sint32 *x, sint32 *y );
    void        GetHotspot(POINT & a_Hotspot) const;

	virtual AUI_ERRCODE LoadEmpty( sint32 width, sint32 height, sint32 bpp );

protected:
	POINT		m_hotspot;
};

#endif
