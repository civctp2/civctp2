//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : Wormhole handling.
// Id           : $Id$
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
// - The good sprite index is now taken from the resource database instead of
//   the good's sprite state database. However this file isn't used as the
//   wormhole has been removed from the game, but maybe there is someone
//   who whishes to put it back into the game. (Aug 29th 2005 Martin Gühmann)
// - Replaced old const database by new one. (5-Aug-2007 Martin Gühmann)
//
//----------------------------------------------------------------------------

#include "ctp/c3.h"
#include "gs/gameobj/Wormhole.h"
#include "gs/gameobj/Player.h"
#include "gs/utility/TurnCnt.h"
#include "robot/aibackdoor/civarchive.h"
#include "gs/gameobj/XY_Coordinates.h"
#include "gs/world/World.h"
#include "gs/newdb/ConstRecord.h"
#include "ctp/ctp2_utils/pointerlist.h"
#include "gs/utility/RandGen.h"
#include "gs/newdb/UnitRec.h"
#include "gfx/spritesys/GoodActor.h"
#include "gfx/spritesys/director.h"
#include "gs/gameobj/UnitPool.h"
#include "gs/outcom/AICause.h"
#include "net/general/network.h"
#include "net/general/net_endgame.h"
#include "gs/utility/directions.h"
#include "gs/fileio/gamefile.h"
#include "gs/newdb/ResourceRecord.h"

#define k_WORMHOLE_GOOD_ID_STR		"WORMHOLE"

Wormhole *g_wormhole = NULL;

Wormhole::Wormhole(sint32 discoverer)
{
	m_discoverer = discoverer;

	sint16 centerY = static_cast<sint16>(g_theWorld->GetYHeight() / 2);
	sint32 orbitHeight = (g_theWorld->GetYHeight() * g_theConstDB->Get(0)->GetWormholeOrbitHeightPercentage()) / 100;
	m_topY = centerY - orbitHeight / 2;
	m_bottomY = centerY + orbitHeight / 2;
	m_pos.y = centerY;
	m_pos.x = sint16(g_rand->Next(g_theWorld->GetXWidth()));
	m_curDir = NORTHEAST;

	m_entries = new PointerList<EntryRecord>;
	m_discoveredAt = g_turn->GetRound();

	sint32 id = g_theResourceDB->Get(g_theResourceDB->FindRecordNameIndex(k_WORMHOLE_GOOD_ID_STR))->GetSpriteID();
	m_actor = new GoodActor(id, m_pos);
}

Wormhole::Wormhole(sint32 discoverer, MapPoint &startPos)
{
	m_discoverer = discoverer;
	sint16 centerY =  static_cast<sint16>(g_theWorld->GetYHeight() / 2);
	sint32 orbitHeight = (g_theWorld->GetYHeight() * g_theConstDB->Get(0)->GetWormholeOrbitHeightPercentage()) / 100;
	m_topY = centerY - orbitHeight / 2;
	m_bottomY = centerY + orbitHeight / 2;
	m_pos.y = startPos.y;
	m_pos.x = startPos.x;
	m_curDir = NORTHEAST;

	m_entries = new PointerList<EntryRecord>;
	m_discoveredAt = g_turn->GetRound();

	sint32 id = g_theResourceDB->Get(g_theResourceDB->FindRecordNameIndex(k_WORMHOLE_GOOD_ID_STR))->GetSpriteID();
	m_actor = new GoodActor(id, m_pos);
}

Wormhole::Wormhole(CivArchive &archive)
{
	m_entries = new PointerList<EntryRecord>;
	Serialize(archive);

	sint32 id = g_theResourceDB->Get(g_theResourceDB->FindRecordNameIndex(k_WORMHOLE_GOOD_ID_STR))->GetSpriteID();
	m_actor = new GoodActor(id, m_pos);
}

Wormhole::~Wormhole()
{
	if(m_entries) {
		m_entries->DeleteAll();
		delete m_entries;
	}

	if (m_actor)
		delete m_actor;
}

void Wormhole::Serialize(CivArchive &archive)
{
	sint32 i, c;
	if(archive.IsStoring()) {
	} else {
		if(g_saveFileVersion < 55) {
			archive.LoadChunk((uint8*)&m_pos, (uint8*)((uint8*)&m_discoveredAt + sizeof(m_discoveredAt)));
			archive >> c;
			for(i = 0; i < c; i++) {
				EntryRecord *rec = new EntryRecord();
				rec->m_unit.Serialize(archive);
				archive >> rec->m_round;
				m_entries->AddTail(rec);
			}
		}
	}
}

BOOL Wormhole::CheckEnter(const Unit &unit)
{
	MapPoint upos;
	if(!unit.GetDBRec()->GetWormholeProbe())
		return FALSE;

	unit.GetPos(upos);
	if(m_pos != upos) {
		return FALSE;
	}
	m_entries->AddTail(new EntryRecord(unit, g_turn->GetRound()));
	return TRUE;
}

void Wormhole::BeginTurn(sint32 player)
{
	Move();

	PointerList<EntryRecord>::Walker walk(m_entries);
	while(walk.IsValid()) {
		if(!g_theUnitPool->IsValid(walk.GetObj()->m_unit)) {
			delete walk.Remove();
			continue;
		}
		if(walk.GetObj()->m_unit.GetOwner() != player) {
			walk.Next();
			continue;
		}
		if(g_theUnitPool->IsValid(walk.GetObj()->m_unit)) {
			walk.GetObj()->m_unit.Kill(CAUSE_REMOVE_ARMY_PROBE_RECOVERED, -1);
		}

		delete walk.Remove();

#if 0

		if(walk.GetObj()->m_round + g_theConstDB->WormholeReturnTime() <= g_turn->GetRound()) {
			EntryRecord *erec = walk.GetObj();
			walk.Remove();
			erec->m_unit.ExitWormhole(m_pos);

			g_director->AddShow(erec->m_unit);

			delete erec;

		} else {
			walk.Next();
		}
#endif
	}
	if(g_network.IsHost()) {
		g_network.QueuePacketToAll(new NetWormhole());
	}
}

#define k_HORIZONTAL_MOVES 5

void Wormhole::Move()
{
	sint32 i;
	sint32 speed = g_theConstDB->Get(0)->GetWormholeSpeed();
	for(i = 0; i < speed; i++) {
		if((m_curDir == EAST || m_curDir == NORTHEAST) &&
		   !g_theWorld->IsXwrap()) {
			if(!m_pos.GetNeighborPosition(m_curDir, m_pos)) {
				m_pos.x = 0;
			}
		}
		m_pos.GetNeighborPosition(m_curDir, m_pos);
		switch(m_curDir) {
			case NORTHEAST:
				if(m_pos.y <= m_topY) {
					m_curDir = EAST;
					m_horizontalMoves = k_HORIZONTAL_MOVES;
				}
				break;
			case EAST:
				m_horizontalMoves--;
				if(m_horizontalMoves <= 0) {
					if(m_pos.y == m_topY) {
						m_curDir = SOUTHEAST;
					} else {
						m_curDir = NORTHEAST;
					}
				}
				break;
			case SOUTHEAST:
				if(m_pos.y >= m_bottomY) {
					m_curDir = EAST;
					m_horizontalMoves = k_HORIZONTAL_MOVES;
				}
				break;
		}
	}
	DPRINTF(k_DBG_INFO, ("Wormhole now at %d,%d\n", m_pos.x, m_pos.y));
}

BOOL Wormhole::IsVisible(sint32 player)
{
	if(m_discoveredAt + g_theConstDB->Get(0)->GetWormholeVisibleToAllTurns() <= g_turn->GetRound()) {
		return TRUE;
	} else if(player == m_discoverer) {
		return TRUE;
	}
	return FALSE;
}
