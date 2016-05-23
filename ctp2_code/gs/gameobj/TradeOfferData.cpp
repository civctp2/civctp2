#include "c3.h"
#include "TradeOfferData.h"
#include "civarchive.h"
#include "player.h"
#include "Gold.h"
#include "network.h"
#include "net_action.h"
#include "net_info.h"
#include "SlicEngine.h"
#include "SlicObject.h"

extern Player **g_player;

void TradeOfferData::Serialize(CivArchive &archive)
{
	uint8 haveChild;

	if(archive.IsStoring()) {
		GAMEOBJ::Serialize(archive);
		archive.StoreChunk((uint8 *)&m_owner, ((uint8 *)&m_toCity)+sizeof(m_toCity));

		haveChild = (m_lesser != NULL);
		archive << haveChild;
		if (m_lesser)
			((TradeOfferData *)(m_lesser))->Serialize(archive) ;

		haveChild = (m_greater != NULL);
		archive << haveChild;
		if (m_greater)
			((TradeOfferData *)(m_greater))->Serialize(archive) ;

	} else {
		GAMEOBJ::Serialize(archive);
		archive.LoadChunk((uint8 *)&m_owner, ((uint8 *)&m_toCity)+sizeof(m_toCity));
		archive >> haveChild;
		if(haveChild) {
			m_lesser = new TradeOfferData(archive);
		} else {
			m_lesser = NULL;
		}

		archive >> haveChild;
		if(haveChild) {
			m_greater = new TradeOfferData(archive);
		} else {
			m_greater = NULL;
		}
	}
}

BOOL TradeOfferData::Accept(PLAYER_INDEX player,
                            const Unit &sourceCity,
                            const Unit &destCity)
{
#ifndef RECIPROCAL_ROUTES
	Assert(m_askingType == ROUTE_TYPE_GOLD);
	if(m_askingType != ROUTE_TYPE_GOLD)
		return FALSE;
#endif

	SlicObject *so = new SlicObject("363TradeOfferAccepted");
	so->AddRecipient(destCity.GetOwner());
	so->AddCivilisation(m_fromCity.GetOwner());
	so->AddCity(m_fromCity);
	so->AddCity(destCity);
	so->AddGood(m_offerResource);
	g_slicEngine->Execute(so);

	if(g_network.IsHost()) {
		g_network.Enqueue(new NetInfo(NET_INFO_CODE_SEND_OFFER_ACCEPT_MESSAGE,
									  m_fromCity.m_id, destCity.m_id, m_offerResource));
	}

	if(g_network.IsClient()) {
		g_network.SendAction(new NetAction(NET_ACTION_ACCEPT_TRADE_OFFER,
										   m_id,
										   player,
										   (uint32)sourceCity,
										   (uint32)destCity));
		return FALSE;
	}

	if(m_offerType == ROUTE_TYPE_SLAVE) {
		if(m_fromCity.SendSlaveTo(destCity)) {
			Gold giveAmount;
			giveAmount.SetLevel(m_askingResource);
			g_player[player]->GiveGold(m_fromCity.GetOwner(), giveAmount);
			return TRUE;
		}
		return FALSE;
	}

	if(m_offerType == ROUTE_TYPE_RESOURCE &&
	   !m_fromCity.HasResource(m_offerResource)) {

		return TRUE;
	}

	TradeRoute fromRoute, toRoute;
	fromRoute = g_player[m_fromCity.GetOwner()]->CreateTradeRoute(
		m_fromCity, m_offerType, m_offerResource, destCity, destCity.GetOwner(), m_askingResource);
	if(fromRoute == TradeRoute(0))
		return FALSE;

#ifdef RECIPROCAL_ROUTES
	toRoute = g_player[player]->CreateTradeRoute(sourceCity,
												 m_askingType,
												 m_askingResource,
												 m_toCity,
												 m_fromCity.GetOwner());
	if(toRoute == TradeRoute(0)) {
		fromRoute.Kill();
		return FALSE;
	}
	toRoute.SetRecip(fromRoute);
	fromRoute.SetRecip(toRoute);
#endif

	return TRUE;
}
