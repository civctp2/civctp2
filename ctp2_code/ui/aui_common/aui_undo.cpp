










#include "c3.h"
#include "aui_ui.h"

#include "aui_undo.h"



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
