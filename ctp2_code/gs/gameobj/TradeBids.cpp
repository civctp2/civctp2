//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ header
// Description  : Trade bit handling
//
//----------------------------------------------------------------------------
//
// Disclaimer
//
// THIS FILE IS NOT GENERATED OR SUPPORTED BY ACTIVISION.
//
// This material has been developed at apolyton.net by the Apolyton CtP2 
// Source Code Project. Contact the authors at ctp2source@apolyton.net.
//
//----------------------------------------------------------------------------
//
// Compiler flags
//
// - None
//
//----------------------------------------------------------------------------
//
// Modifications from the original Activision code:
//
// - Made the Unit argments of AddBit method const. June 18th 2005 Martin Gühmann
//
//----------------------------------------------------------------------------

#include "c3.h"
#include "TradeBids.h"
#include "pointerlist.h"
#include "civarchive.h"
#include "player.h"
#include "MessagePool.h"
#include "gamefile.h"

extern Player **g_player;
extern MessagePool *g_theMessagePool;

TradeBids *g_theTradeBids = NULL;

void TradeBids::Bid::Serialize(CivArchive &archive)
{
	if(archive.IsStoring()) {
		Assert(FALSE);
	} else {
		if(g_saveFileVersion < 55) {
			archive.LoadChunk((uint8*)&m_id, (uint8*)&m_price + sizeof(m_price));
		}
	}
}

TradeBids::TradeBids()
{
	sint32 i;
	for(i = 0; i < k_MAX_PLAYERS; i++) {
		m_table[i] = new PointerList<Bid>;
		m_nextId[i] = 0;
	}
}

TradeBids::~TradeBids()
{
	sint32 i;
	for(i = 0; i < k_MAX_PLAYERS; i++) {
		m_table[i]->DeleteAll();
		delete m_table[i];
	}
}

void TradeBids::Serialize(CivArchive &archive)
{
	sint32 i, j, c;
	if(archive.IsStoring()) {
	} else {
		if(g_saveFileVersion < 55) {
			archive.LoadChunk((uint8*)m_nextId, (uint8*)m_nextId + sizeof(m_nextId));
			for(i = 0; i < k_MAX_PLAYERS; i++) {
				m_table[i] = new PointerList<Bid>;
				archive >> c;
				for(j = 0; j < c; j++) {
					m_table[i]->AddTail(new Bid(archive));
				}
			}
		}
	}
}

uint32 TradeBids::AddBid(sint32 owner,
                         const Unit &fromCity,
                         sint32 resource,
                         const Unit &toCity,
                         sint32 price)
{
	uint32 id = (owner << 24) | (m_nextId[owner]++);
	m_table[owner]->AddTail(new Bid(id,
	                                owner,
	                                fromCity,
	                                resource,
	                                toCity,
	                                price));
	return id;
}

void TradeBids::Reject(uint32 id)
{
	sint32 player = (id & 0xff000000) >> 24;
	PointerList<Bid>::Walker walk(m_table[player]);
	Bid *bid = NULL;
	for(; walk.IsValid(); walk.Next()) {
		if(walk.GetObj()->m_id == id) {
			bid = walk.Remove();
			break;
		}
	}
	sint32 rejecter = bid->m_fromCity.GetOwner();
	Assert(g_player[rejecter]);
	if(g_player[rejecter]) {
		g_player[rejecter]->RejectTradeBid(bid->m_fromCity,
										   bid->m_resource,
										   bid->m_toCity,
										   bid->m_price);
	}
	delete bid;
}

void TradeBids::Accept(uint32 id)
{
	sint32 player = (id & 0xff000000) >> 24;
	PointerList<Bid>::Walker walk(m_table[player]);
	Bid *bid = NULL;
	for(; walk.IsValid(); walk.Next()) {
		if(walk.GetObj()->m_id == id) {
			bid = walk.Remove();
			break;
		}
	}
	sint32 accepter = bid->m_fromCity.GetOwner();
	Assert(g_player[accepter]);
	if(g_player[accepter]) {
		g_player[accepter]->AcceptTradeBid(bid->m_fromCity,
										   bid->m_resource,
										   bid->m_toCity,
										   bid->m_price);
	}
	delete bid;
}

void TradeBids::CancelBidsFrom(sint32 owner)
{
	while(m_table[owner]->GetHead()) {
		Bid *bid = m_table[owner]->RemoveHead();
		if(g_theMessagePool->IsValid(bid->m_message)) {
			bid->m_message.Kill();
		}
		delete bid;
	}
}




void TradeBids::CancelBidsWithCity(const Unit &city)
{
	sint32 i;
	for(i = 0; i < k_MAX_PLAYERS; i++) {
		PointerList<Bid>::Walker walk(m_table[i]);
		while(walk.IsValid()) {
			if(walk.GetObj()->m_fromCity.m_id == city.m_id ||
			   walk.GetObj()->m_toCity.m_id == city.m_id) {
				if(g_theMessagePool->IsValid(walk.GetObj()->m_message)) {
					walk.GetObj()->m_message.Kill();
				}
				delete walk.Remove();
			} else {
				walk.Next();
			}
		}
	}
}

void TradeBids::SetMessage(uint32 bidId, const Message &msg)
{
	sint32 player = (bidId & 0xff000000) >> 24;
	PointerList<Bid>::Walker walk(m_table[player]);
	for(; walk.IsValid(); walk.Next()) {
		if(walk.GetObj()->m_id == bidId) {
			walk.GetObj()->m_message = msg;
			break;
		}
	}
}
