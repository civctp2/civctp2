

#include "c3.h"
#include "aui.h"
#include "aui_item.h"
#include "aui_UniqueId.h"

#include "Pattern.h"
#include "PatternBase.h"

#include "ctp2_listitem.h"



ctp2_ListItem::ctp2_ListItem(AUI_ERRCODE *retval, MBCHAR *ldlBlock)
	:
	aui_Item( retval, aui_UniqueId(), ldlBlock),
	aui_ImageBase(ldlBlock),
	aui_TextBase(ldlBlock, (MBCHAR *)NULL),
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




ctp2_ListItem::~ctp2_ListItem()
{
	DeleteChildren();
}



AUI_ERRCODE ctp2_ListItem::InitCommonLdl(MBCHAR *ldlBlock)
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
