







#pragma once

#ifndef __C3_TRADELISTITEM_H__
#define __C3_TRADELISTITEM_H__

#include "c3_listitem.h"

class aui_Surface;
class TradeRoute;

class c3_TradeListItem : public c3_ListItem
{
public:
	
	
	c3_TradeListItem(AUI_ERRCODE *retval, TradeRoute *route, sint32 gold, sint32 resIndex, MBCHAR *ldlBlock);

	
	
	virtual void Update(void);

	
	sint32	GetGold(void) const { return m_gold; }
	TradeRoute *GetRoute(void) const { return m_route; }

protected:
	c3_TradeListItem() : c3_ListItem() {}

	
	
	AUI_ERRCODE InitCommonLdl(TradeRoute *route, sint32 gold, sint32 resIndex, MBCHAR *ldlBlock);
	
public:
	
	virtual sint32 Compare(c3_ListItem *item2, uint32 column);

private:
	TradeRoute		*m_route;
	sint32			m_gold;
	sint32			m_resIndex;	
};


#endif