#ifdef HAVE_PRAGMA_ONCE
#pragma once
#endif
#ifndef __NET_PLAYERDATA_H__
#define __NET_PLAYERDATA_H__

#include "pointerlist.h"
#include "net_hash.h"
#include "dynarr.h"

class UnitDynamicArray;




class PlayerData
{
public:
	PlayerData(const char* name, uint16 id);

	~PlayerData();

	char* m_name;
	uint16 m_id;
	sint32 m_index;
	sint32 m_group;
	PointerList<Packetizer> *m_packetList;
	DynamicArray<Unit> m_createdUnits;
	DynamicArray<Army> m_createdArmies;
	GUID m_guid;

	NetHash m_unitHash;
	BOOL m_frozen;




	PointerList<PointerList<Packetizer>::PointerListNode> *m_bookmarks;
	BOOL m_ready;
	sint32 m_blocked;

	sint32 m_totalTimeUsed;

	BOOL m_ackBeginTurn;
	BOOL m_sentResync;
	UnitDynamicArray *m_createdCities;
};

#endif
