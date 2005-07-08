












#include "c3.h"

#include "aui.h"
#include "aui_window.h"

#include "primitives.h"

#include "statictextitem.h"
#include "textutils.h"

#include "colorset.h"

extern ColorSet	*g_colorSet;


StaticTextItem::StaticTextItem(
	AUI_ERRCODE *retval,
	uint32 id,
	sint32 x,
	sint32 y,
	sint32 width,
	sint32 height,
	MBCHAR *text,
	uint32 maxLength,
	uint32 size,
	sint32 textId,
	sint32 category,
	sint32 itemType)
:
	aui_ImageBase( (sint32)0 ),
	aui_TextBase( text ),
	aui_Static( retval, id, x, y, width, height, text )
{
	


	m_infoId = textId;
	m_category = category;
	m_itemType = itemType;
}

