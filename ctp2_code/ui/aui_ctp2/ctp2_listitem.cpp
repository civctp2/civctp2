#include "c3.h"
#include "aui.h"
#include "aui_item.h"
#include "aui_uniqueid.h"

#include "pattern.h"
#include "patternbase.h"

#include "ctp2_listitem.h"




ctp2_ListItem::ctp2_ListItem(AUI_ERRCODE *retval, const MBCHAR *ldlBlock)
	:
	aui_ImageBase(ldlBlock),
	aui_TextBase(ldlBlock, (MBCHAR *)NULL),
	aui_Item( retval, aui_UniqueId(), ldlBlock),
	PatternBase(ldlBlock, (MBCHAR *)NULL)
{
	m_userData = NULL;
	m_compareCallback = NULL;

	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;

	*retval = InitCommonLdl(ldlBlock);
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;
}

/*
ctp2_ListItem::ctp2_ListItem() : aui_Item() {
	m_userData = NULL;
	m_compareCallback = NULL;
        printf("ctp2_listitem.cpp L38: Standard constructor called!!!\n");
    }
*/

ctp2_ListItem::~ctp2_ListItem()
{
	DeleteChildren();
}


AUI_ERRCODE ctp2_ListItem::InitCommonLdl(const MBCHAR *ldlBlock)
{
	return AUI_ERRCODE_OK;
}




sint32 ctp2_ListItem::Compare(ctp2_ListItem *item2, uint32 column)
{
	if(m_compareCallback) {
		return m_compareCallback(this, item2, column);
	}
	return 0;
}
