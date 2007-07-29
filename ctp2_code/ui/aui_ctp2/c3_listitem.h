#ifdef HAVE_PRAGMA_ONCE
#pragma once
#endif

#ifndef __C3_LISTITEM_H__
#define __C3_LISTITEM_H__

class c3_ListItem;
class SingleListItem;

#include "aui_item.h"
#include "patternbase.h"

class c3_ListItem : public aui_Item, public PatternBase
{
public:
	c3_ListItem(AUI_ERRCODE *retval, MBCHAR const *ldlBlock);
	virtual ~c3_ListItem();

	virtual void Update(void) {}

	virtual sint32 Compare(c3_ListItem *item2, uint32 column);

protected:
	c3_ListItem() : aui_Item() {}
};

class SingleListItem : public c3_ListItem
{
public:
	SingleListItem(AUI_ERRCODE *retval, MBCHAR const *name, sint32 value, MBCHAR const *ldlBlock);

	virtual void Update(void);

	MBCHAR const * GetName( void ) const { return m_name; }
	sint32 GetValue( void ) const { return m_value; }

	virtual sint32 Compare(c3_ListItem *item2, uint32 column);

protected:
	SingleListItem() : c3_ListItem() {}

	AUI_ERRCODE InitCommonLdl(MBCHAR const *name, sint32 value, MBCHAR const *ldlBlock);
	
private:
	MBCHAR			m_name[256];
	sint32			m_value;
};


#endif
