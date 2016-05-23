#include "c3.h"
#include "C3Trade.h"
#include "civarchive.h"
#include "globals.h"
#include "player.h"
#include "UnitPool.h"
#include "UnitDynArr.h"

extern Player **g_player;
extern UnitPool *g_theUnitPool;

C3TradeOffer::C3TradeOffer(const TradeOffer &offer,
						   PLAYER_INDEX aiPlayer)
{
	m_offer = offer;
	m_aiPlayer = aiPlayer;
	m_refCount = 0;
}

C3TradeOffer::C3TradeOffer(CivArchive &archive)
{
	Serialize(archive);
}

C3TradeOffer::~C3TradeOffer()
{
}

void C3TradeOffer::Serialize(CivArchive &archive)
{
	m_offer.Serialize(archive);
	if(archive.IsStoring()) {
		archive << m_refCount;
		archive << m_aiPlayer;
	} else {
		archive >> m_refCount;
		archive >> m_aiPlayer;
	}
}

STDMETHODIMP C3TradeOffer::QueryInterface(REFIID riid, void **obj)
{
	*obj = NULL;
	if(IsEqualIID(riid, IID_IUnknown)) {
		*obj = (IUnknown *)this;
		AddRef();
		return S_OK;
	} else if(IsEqualIID(riid, CLSID_IC3TradeOffer)) {
		*obj = (IC3TradeOffer *)this;
		AddRef();
		return S_OK;
	}
	return E_NOINTERFACE;
}

STDMETHODIMP_(ULONG) C3TradeOffer::AddRef()
{
	return ++m_refCount;
}

STDMETHODIMP_(ULONG) C3TradeOffer::Release()
{
	if(--m_refCount)
		return m_refCount;
	delete this;
	return 0;
}

STDMETHODIMP_(uint32) C3TradeOffer::GetID()
{
	return (uint32)m_offer;
}

STDMETHODIMP_(PLAYER_INDEX) C3TradeOffer::GetOwner()
{
	return m_offer.GetFromCity().GetOwner();
}

STDMETHODIMP_(uint32) C3TradeOffer::GetFromCity()
{
	Unit fromCity = m_offer.GetFromCity();
	if(!g_theUnitPool->IsValid(fromCity))
		return 0;

	if(!g_player[fromCity.GetOwner()])
		return 0;

	if(!g_player[fromCity.GetOwner()]->m_all_cities->IsPresent(fromCity))
		return 0;

	return g_player[m_offer.GetFromCity().GetOwner()]->GetCityId(m_offer.GetFromCity());
}

STDMETHODIMP_(ROUTE_TYPE) C3TradeOffer::GetOfferType()
{
	return m_offer.GetOfferType();
}

STDMETHODIMP_(sint32) C3TradeOffer::GetOfferResource()
{
	return m_offer.GetOfferResource();
}

STDMETHODIMP_(sint32) C3TradeOffer::GetAskingGold()
{
	return m_offer.GetAskingResource();
}

STDMETHODIMP_(void) C3TradeOffer::Accept(uint32 destCity)
{
	BOOL isUnknown;
	Unit city;
	g_player[m_aiPlayer]->AiGetCity(isUnknown, destCity,
									city);
	Assert(!isUnknown);
	if(isUnknown)
		return;

	g_player[m_aiPlayer]->AcceptTradeOffer(m_offer, Unit(0), city);
}
