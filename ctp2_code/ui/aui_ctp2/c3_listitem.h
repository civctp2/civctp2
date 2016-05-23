#ifdef HAVE_PRAGMA_ONCE
#pragma once
#endif

#ifndef __C3_LISTITEM_H__
#define __C3_LISTITEM_H__

#include "aui_item.h"
#include "patternbase.h"

class aui_Surface;

class c3_ListItem : public aui_Item, public PatternBase
{
public:

	c3_ListItem(AUI_ERRCODE *retval, const MBCHAR *ldlBlock);

	virtual ~c3_ListItem();







	virtual void Update(void) {}













	virtual sint32 Compare(c3_ListItem *item2, uint32 column);

protected:





	AUI_ERRCODE InitCommonLdl(const MBCHAR *ldlBlock);

	c3_ListItem() : aui_Item() {}

};

class SingleListItem : public c3_ListItem
{
public:

	SingleListItem(AUI_ERRCODE *retval, const MBCHAR *name, sint32 value, const MBCHAR *ldlBlock);


	virtual void Update(void);

	const MBCHAR	*GetName( void ) { return m_name; }
	sint32	GetValue( void ) { return m_value; }

protected:
	SingleListItem() : c3_ListItem() {}


	AUI_ERRCODE InitCommonLdl(const MBCHAR *name,sint32 value, const MBCHAR *ldlBlock);

public:

	virtual sint32 Compare(c3_ListItem *item2, uint32 column);

private:
	MBCHAR			m_name[256];
	sint32			m_value;
};

#endif
