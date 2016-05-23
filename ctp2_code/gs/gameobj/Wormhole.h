#pragma once
#ifndef __WORMHOLE_H__
#define __WORMHOLE_H__

#include "MapPoint.h"
#include "Unit.h"

class CivArchive;
class GoodActor;
template <class T> class PointerList;

class EntryRecord {
public:
	EntryRecord(const Unit &unit, sint32 round)
	{
		m_unit = unit;
		m_round = round;
	}
	EntryRecord() {}

	Unit m_unit;
	sint32 m_round;
};

class Wormhole
{
private:
	MapPoint m_pos;
	WORLD_DIRECTION m_curDir;
	sint32 m_horizontalMoves;
	sint32 m_topY, m_bottomY;
	sint32 m_discoverer;
	GoodActor	*m_actor;

	sint32 m_discoveredAt;




	PointerList<EntryRecord> *m_entries;
	void Move();

	friend class NetWormhole;

public:
	Wormhole(sint32 discoverer);
	Wormhole(sint32 discoverer, MapPoint &startPos);

	Wormhole(CivArchive &archive);
	~Wormhole();

	void Serialize(CivArchive &archive);

	BOOL IsVisible(sint32 player);

	BOOL CheckEnter(const Unit &unit);
	void BeginTurn(sint32 player);

	GoodActor *GetActor(void) { return m_actor;}
	MapPoint	GetPos(void) { return m_pos; }
};

extern Wormhole *g_wormhole;

#endif
