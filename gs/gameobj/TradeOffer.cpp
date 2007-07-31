








#include "c3.h"

#include "TradeOffer.h"
#include "CivArchive.h"
#include "TradeOfferPool.h"
#include "Player.h"
#include "net_info.h"
#include "network.h"

#include "Message.h"
#include "MessageData.h"
#include "TradeManager.h"

extern Player **g_player;


void TradeOffer::KillOffer()
{
	TradeOffer tmp(*this);
	tmp.RemoveAllReferences();
}

void TradeOffer::RemoveAllReferences()
{
	g_player[GetOwner()]->RemoveTradeOffer(*this);

	if(g_network.IsHost()) {
		g_network.Block(GetOwner());
		g_network.Enqueue(new NetInfo(NET_INFO_CODE_KILL_TRADE_OFFER,
									  (uint32)(*this)));
		g_network.Unblock(GetOwner());
	} else if(g_network.IsClient()) {
		g_network.AddDeadUnit(m_id);
	}



	g_theTradeOfferPool->Remove(*this);

	sint32 p;
	for(p = 0; p < k_MAX_PLAYERS; p++) {
		if(g_player[p]) {
			sint32 i;
			for(i = g_player[p]->m_messages->Num() - 1; i >= 0; i--) {
			}
		}
	}

	
	TradeManager::Notify();
}

const TradeOfferData* TradeOffer::GetData() const
{
	return g_theTradeOfferPool->GetTradeOffer(*this);
}

TradeOfferData* TradeOffer::AccessData()
{
	return g_theTradeOfferPool->AccessTradeOffer(*this);
}

BOOL TradeOffer::Accept(PLAYER_INDEX player, 
						Unit &sourceCity,
						Unit &destCity)
{
	return AccessData()->Accept(player, sourceCity, destCity);
}

