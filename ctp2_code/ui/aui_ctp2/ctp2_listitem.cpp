#include "c3.h"
#include "ctp2_listitem.h"

#include "aui.h"
#include "aui_item.h"
#include "aui_uniqueid.h"
#include "pattern.h"
#include "patternbase.h"

ctp2_ListItem::ctp2_ListItem(AUI_ERRCODE *retval, MBCHAR *ldlBlock)
:
	aui_ImageBase(ldlBlock),
	aui_TextBase(ldlBlock, (MBCHAR *)NULL),
	aui_Item            (retval, aui_UniqueId(), ldlBlock),
	PatternBase         (ldlBlock, (MBCHAR *) NULL),
    m_userData          (NULL),
    m_compareCallback   (NULL)
{
}




ctp2_ListItem::~ctp2_ListItem()
{
	DeleteChildren();
}



sint32 ctp2_ListItem::Compare(ctp2_ListItem *item2, uint32 column)
{
	if (m_compareCallback) 
    {
		return m_compareCallback(this, item2, column);
	}

	return 0;
}
