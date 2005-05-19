#pragma once
#ifndef __NET_ARMY_H__
#define __NET_ARMY_H__

#include "net_packet.h"

class ArmyList;
class ArmyData;

typedef sint32 PLAYER_INDEX;
#include "aicause.h"
#include "cellunitlist.h"


class NetNewArmy : public Packetizer
{
public:
	NetNewArmy(PLAYER_INDEX player, const ArmyList &army, sint32 armyIndex,
			   CAUSE_NEW_ARMY cause);
	NetNewArmy() {};

	void Packetize(uint8 *buf, uint16 &size);
	void Unpacketize(uint16 id, uint8 *buf, uint16 size);
private:
	sint32 m_index;
	PLAYER_INDEX m_player;
	uint32 m_unitId[k_MAX_ARMY_SIZE];
	uint8 m_numUnits;
	CAUSE_NEW_ARMY m_cause;
};


class NetRemoveArmy : public Packetizer
{
public:
	NetRemoveArmy(PLAYER_INDEX player, const ArmyList &army, sint32 armyIndex,
				  CAUSE_REMOVE_ARMY cause);
	NetRemoveArmy() {};

	void Packetize(uint8 *buf, uint16 &size);
	void Unpacketize(uint16 id, uint8 *buf, uint16 size);

private:
	sint32 m_index;
	PLAYER_INDEX m_player;
	uint32 m_unitId[k_MAX_ARMY_SIZE];
	uint8 m_numUnits;
	CAUSE_REMOVE_ARMY m_cause;
};

class NetArmy : public Packetizer
{
public:
	NetArmy(ArmyData *data);
	NetArmy() {}

	void Packetize(uint8 *buf, uint16 &size);
	void Unpacketize(uint16 id, uint8 *buf, uint16 size);
private:
	ArmyData *m_data;
};

class NetGroupRequest:public Packetizer
{
  public:
	NetGroupRequest(const CellUnitList &units, const Army &army);
	NetGroupRequest() {}

	void Packetize(uint8 *buf, uint16 &size);
	void Unpacketize(uint16 id, uint8 *buf, uint16 size);

  private:
	uint32 m_armyId;
	CellUnitList m_units;
};

class NetUngroupRequest:public Packetizer
{
  public:
	NetUngroupRequest(const Army &army, const CellUnitList &units);
	NetUngroupRequest() {}

	void Packetize(uint8 *buf, uint16 &size);
	void Unpacketize(uint16 id, uint8 *buf, uint16 size);

  private:
	uint32 m_armyId;
	CellUnitList m_units;
};
#endif
