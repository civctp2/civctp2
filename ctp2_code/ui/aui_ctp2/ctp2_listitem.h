#if defined(HAVE_PRAGMA_ONCE)
#pragma once
#endif

#ifndef __CTP2_LISTITEM_H__
#define __CTP2_LISTITEM_H__

#include "aui_item.h"
#include "patternbase.h"

class aui_Surface;

class ctp2_ListItem : public aui_Item, public PatternBase
{
public:
	typedef sint32 (CTP2ItemCompareCallback)(ctp2_ListItem *item1,
											 ctp2_ListItem *item2,
											 sint32 column);

	void *m_userData;
	CTP2ItemCompareCallback *m_compareCallback;

	ctp2_ListItem(AUI_ERRCODE *retval, MBCHAR *ldlBlock);

	virtual ~ctp2_ListItem();





	virtual void Update(void) {}













	virtual sint32 Compare(ctp2_ListItem *item2, uint32 column);


	void SetUserData(void *data) { m_userData = data; }
	void *GetUserData() {
            if (!m_userData)
                printf("ctp2_listitem.h L54: m_userData == NULL!\n");
            return m_userData;
            }

	void SetCompareCallback(CTP2ItemCompareCallback *cb) {
            if (!cb)
                printf("ctp2_listitem.h L56: cb is NULL!\n)");
            m_compareCallback = cb;
            }

protected:





	AUI_ERRCODE InitCommonLdl(MBCHAR *ldlBlock);

	ctp2_ListItem() : aui_Item() {}

};

#endif
