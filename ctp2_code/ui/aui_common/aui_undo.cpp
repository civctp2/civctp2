#include "ctp/c3.h"
#include "ui/aui_common/aui_ui.h"

#include "ui/aui_common/aui_undo.h"


aui_Undo::aui_Undo(
	aui_Region *region,
	RECT rect )
{
	m_region = region;
	SetRect( &m_rect, rect.left, rect.top, rect.right, rect.bottom );
}

aui_Undo::~aui_Undo()
{
}
