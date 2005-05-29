#ifdef HAVE_PRAGMA_ONCE
#pragma once
#endif
#ifndef __TRADE_BIDS_H__
#define __TRADE_BIDS_H__

#include "Unit.h"
#include "message.h"

class CivArchive;
template <class T> class PointerList;

class TradeBids {
private:

	class Bid {
	public:
		uint32 m_id;

		sint32 m_owner;
		Unit m_fromCity;
		sint32 m_resource;
		Unit m_toCity;
		sint32 m_price;
		Message m_message;

		Bid(uint32 id, sint32 owner, Unit fromCity, sint32 resource,
			Unit toCity, sint32 price) {
			m_id = id;
			m_owner = owner;
			m_fromCity = fromCity;
			m_resource = resource;
			m_toCity = toCity;
			m_price = price;
		}
		Bid(CivArchive &archive) { Serialize(archive); }
		void Serialize(CivArchive &archive);
	};

	
	sint32 m_nextId[k_MAX_PLAYERS];

	
	PointerList<Bid> *m_table[k_MAX_PLAYERS];

public:
	TradeBids();
	TradeBids(CivArchive &archive) { Serialize(archive); }
	~TradeBids();
	void Serialize(CivArchive &archive);

	uint32 AddBid(sint32 owner,
				  const Unit &fromCity,
				  sint32 resource,
				  const Unit &toCity,
				  sint32 price);
	void Reject(uint32 id);
	void Accept(uint32 id);
	void CancelBidsFrom(sint32 owner);
	void CancelBidsWithCity(const Unit &city);
	void SetMessage(uint32 id, const Message &msg);
};

extern TradeBids *g_theTradeBids;

#endif
