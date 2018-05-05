#include "ctp/c3.h"

#include "ui/aui_common/aui.h"
#include "ui/aui_common/aui_window.h"

#include "ui/aui_utils/primitives.h"

#include "ui/aui_ctp2/statictextitem.h"
#include "ui/aui_utils/textutils.h"

#include "gfx/gfx_utils/colorset.h"

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
