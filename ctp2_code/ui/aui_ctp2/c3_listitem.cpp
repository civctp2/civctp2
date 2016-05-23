#include "c3.h"
#include "aui.h"
#include "aui_item.h"
#include "aui_uniqueid.h"
#include "aui_ldl.h"
#include "aui_bitmapfont.h"

#include "pattern.h"
#include "patternbase.h"

#include "c3_listitem.h"
#include "c3_static.h"

c3_ListItem::c3_ListItem(AUI_ERRCODE *retval, MBCHAR *ldlBlock)
	:
	aui_ImageBase(ldlBlock),
	aui_TextBase(ldlBlock, (MBCHAR *)NULL),
	aui_Item( retval, aui_UniqueId(), ldlBlock),
	PatternBase(ldlBlock, (MBCHAR *)NULL)
{
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;

	*retval = InitCommonLdl(ldlBlock);
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;
}


c3_ListItem::~c3_ListItem()
{
	ListPos position = m_childList->GetHeadPosition();

	for ( sint32 i = m_childList->L(); i; i-- )
		delete m_childList->GetNext( position );
}

AUI_ERRCODE c3_ListItem::InitCommonLdl(MBCHAR *ldlBlock)
{
	return AUI_ERRCODE_OK;
}


sint32 c3_ListItem::Compare(c3_ListItem *item2, uint32 column)
{

	return 0;
}

SingleListItem::SingleListItem(AUI_ERRCODE *retval, MBCHAR *name, sint32 value, MBCHAR *ldlBlock)
	:
	aui_ImageBase(ldlBlock),
	aui_TextBase(ldlBlock, (MBCHAR *)NULL),
	c3_ListItem( retval, ldlBlock)
{
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;

	*retval = InitCommonLdl(name, value, ldlBlock);
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;
}

AUI_ERRCODE SingleListItem::InitCommonLdl(MBCHAR *name, sint32 value, MBCHAR *ldlBlock)
{
	MBCHAR			block[ k_AUI_LDL_MAXBLOCK + 1 ];
	AUI_ERRCODE		retval;

	strcpy(m_name, name);
	m_value = value;

	c3_Static		*subItem;

	sprintf(block, "%s.%s", ldlBlock, "Name");
	subItem = new c3_Static(&retval, aui_UniqueId(), block);
	AddChild(subItem);

	Update();

	return AUI_ERRCODE_OK;
}

void SingleListItem::Update(void)
{

	c3_Static *subItem;

	subItem = (c3_Static *)GetChildByIndex(0);

	MBCHAR name[ 256 + 1 ];
	strncpy( name, m_name, 256 );

	if ( !subItem->GetTextFont() )
		subItem->TextReloadFont();

	subItem->GetTextFont()->TruncateString(
		name,
		subItem->Width() );

	subItem->SetText(name);
}

sint32 SingleListItem::Compare(c3_ListItem *item2, uint32 column)
{
	c3_Static		*i1, *i2;
	MBCHAR			strbuf1[256];
	MBCHAR			strbuf2[256];

	if (column < 0) return 0;

	switch (column) {
	case 0:

		i1 = (c3_Static *)this->GetChildByIndex(column);
		i2 = (c3_Static *)item2->GetChildByIndex(column);
		strcpy(strbuf1,i1->GetText());
		strcpy(strbuf2,i2->GetText());

		return (strbuf1[0] - strbuf2[0]);
		break;
	}
	return 0;
}
