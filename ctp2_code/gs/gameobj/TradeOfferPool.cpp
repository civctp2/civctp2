









#include "c3.h"
#include "TradeOfferPool.h"
#include "Player.h"
#include "DynArr.h"

#include "Globals.h"
#include "GameFile.h"

extern Player **g_player;


TradeOfferPool::TradeOfferPool() : ObjPool(k_BIT_GAME_OBJ_TYPE_TRADE_OFFER)
{
	m_all_offers = new DynamicArray<TradeOffer>;
}

TradeOfferPool::TradeOfferPool(CivArchive &archive) 
	: ObjPool(k_BIT_GAME_OBJ_TYPE_TRADE_OFFER)
{
	m_all_offers = new DynamicArray<TradeOffer>;
	Serialize(archive);
}

TradeOfferPool::~TradeOfferPool()
{
	if(m_all_offers)
		delete m_all_offers;
}

TradeOffer TradeOfferPool::Create(Unit fromCity, 
								  ROUTE_TYPE offerType, 
								  sint32 offerResource, 
								  ROUTE_TYPE askingType, 
								  sint32 askingResource, 
								  Unit toCity            
								  )
{
	TradeOfferData* newData;
	TradeOffer newOffer(NewKey(k_BIT_GAME_OBJ_TYPE_TRADE_OFFER));
	
	newData = new TradeOfferData(newOffer, fromCity,
								 offerType, offerResource,
								 askingType, askingResource,
								 toCity);
	Insert(newData);
	g_player[fromCity.GetOwner()]->AddTradeOffer(newOffer);
	m_all_offers->Insert(newOffer);





	return newOffer;
}

void TradeOfferPool::Remove(TradeOffer offer)
{
	m_all_offers->Del(offer);
	Del(offer);
}

void TradeOfferPool::Serialize(CivArchive &archive)
{
	TradeOfferData* offerData;
	sint32	i,
			count = 0 ;

#define TRADEOFFERPOOL_MAGIC 0x99FF0011

    CHECKSERIALIZE

	if(archive.IsStoring()) {
		Assert(FALSE);
	} else {
		if(g_saveFileVersion < 55) {
			archive.TestMagic(TRADEOFFERPOOL_MAGIC) ;
			ObjPool::Serialize(archive);

			archive>>count;
			for (i=0; i<count; i++) {
				offerData = new TradeOfferData(archive);
				Insert(offerData);
			}
		}
	}
	if(g_saveFileVersion < 55 && !archive.IsStoring()) {
		m_all_offers->Serialize(archive);
	}
}

sint32 TradeOfferPool::GetNumTradeOffers()
{
	Assert(m_all_offers);
	if(!m_all_offers)
		return 0;

	return m_all_offers->Num();
}

TradeOffer TradeOfferPool::GetTradeOffer(sint32 index)
{
	Assert(index >= 0 && index < m_all_offers->Num());
	if(index < 0 || index >= m_all_offers->Num()) {
		return TradeOffer(0);
	}
	Assert(m_all_offers);
	if(!m_all_offers)
		return TradeOffer(0);

	return m_all_offers->Access(index);
}

void TradeOfferPool::ReRegisterOffers()
{







}

void TradeOfferPool::RemoveTradeOffersFromCity(Unit &city)
{
	sint32 i;
	for(i = m_all_offers->Num() - 1; i >= 0; i--) {
		if(m_all_offers->Access(i).GetFromCity() == city)
			m_all_offers->Access(i).Kill();
	}
}

void TradeOfferPool::AddFromNetwork(const TradeOffer &offer)
{
	g_player[offer.GetFromCity().GetOwner()]->AddTradeOffer(offer);
	m_all_offers->Insert(offer);
}
