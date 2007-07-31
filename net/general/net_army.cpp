

#include "c3.h"
#include "Globals.h"
#include "net_army.h"
#include "network.h"
#include "net_util.h"
#include "Player.h"
#include "UnitPool.h"
#include "ArmyPool.h"
#include "Army.h"
#include "ArmyData.h"
#include "CtpAi.h"
#include "net_info.h"

extern Player **g_player;
extern UnitPool *g_theUnitPool;


NetNewArmy::NetNewArmy(PLAYER_INDEX player, const ArmyList &army,
					   sint32 armyIndex, CAUSE_NEW_ARMY cause)
{
#if 0
	m_index = armyIndex;
	m_player = player;
	m_numUnits = (uint8)army.Num();
	m_cause = cause;
	for(sint32 i = 0; i < m_numUnits; i++) {
		m_unitId[i] = uint32(army.Get(i));
	}
#endif
}

void NetNewArmy::Packetize(uint8 *buf, uint16 &size)
{
#if 0
	size = 0;
	PUSHID(k_PACKET_NEW_ARMY_ID);

	PUSHLONG(m_player);
	PUSHLONG(m_index);
	PUSHLONG(m_cause);
	PUSHBYTE(m_numUnits);
	for(sint32 i = 0; i < m_numUnits; i++) {
		PUSHLONG(m_unitId[i]);
	}
#endif
}

void NetNewArmy::Unpacketize(uint16 id, uint8 *buf, uint16 size)
{
#if 0
	sint32 pos = 0;
	uint16 packid;
	sint32 i;
	PULLID(packid);
	Assert(packid == k_PACKET_NEW_ARMY_ID);

	PULLLONG(m_player);
	PULLLONG(m_index);
	PULLLONGTYPE(m_cause, CAUSE_NEW_ARMY);
	PULLBYTE(m_numUnits);

	Assert(m_numUnits <= k_MAX_ARMY_SIZE);

	for(i = 0; i < m_numUnits; i++) {
		PULLLONG(m_unitId[i]);
	}

	Assert(pos == size);

	if(g_network.IsHost()) {
		Assert(m_player == g_network.IdToIndex(id));
		if(m_player != g_network.IdToIndex(id))
			return;
	}

	Assert(g_player[m_player]->m_all_armies->Num() == m_index);
	if(g_player[m_player]->m_all_armies->Num() != m_index)
		return;
	
	static ArmyList al;
	al.Clear();
	for(i = 0; i < m_numUnits; i++) {
		Unit unit(m_unitId[i]);
		Assert(g_theUnitPool->IsValid(unit));
		al.Insert(unit);
	}
	g_player[m_player]->CopyArmyIntoPlayer(al, m_cause,
										   TRUE);
#endif
}

NetRemoveArmy::NetRemoveArmy(PLAYER_INDEX player, const ArmyList &army,
							 sint32 armyIndex, CAUSE_REMOVE_ARMY cause)
{
#if 0
	m_index = armyIndex;
	m_player = player;
	m_numUnits = uint8(army.Num());
	m_cause = cause;
	for(sint32 i = 0; i < m_numUnits; i++) {
		m_unitId[i] = uint32(army.Get(i));
	}
#endif
}

void NetRemoveArmy::Packetize(uint8 *buf, uint16 &size)
{
#if 0
	size = 0;
	PUSHID(k_PACKET_REMOVE_ARMY_ID);

	PUSHLONG(m_player);
	PUSHLONG(m_index);
	PUSHLONG(m_cause);
	PUSHBYTE(m_numUnits);
	for(sint32 i = 0; i < m_numUnits; i++) {
		PUSHLONG(m_unitId[i]);
	}
#endif
}

void NetRemoveArmy::Unpacketize(uint16 id, uint8 *buf, uint16 size)
{
#if 0
	sint32 pos = 0;
	uint16 packid;
	sint32 i;
	PULLID(packid);
	Assert(packid == k_PACKET_REMOVE_ARMY_ID);

	PULLLONG(m_player);
	PULLLONG(m_index);
	PULLLONGTYPE(m_cause, CAUSE_REMOVE_ARMY);
	PULLBYTE(m_numUnits);

	Assert(m_numUnits <= k_MAX_ARMY_SIZE);

	for(i = 0; i < m_numUnits; i++) {
		PULLLONG(m_unitId[i]);
	}

	if(g_network.IsHost()) {
		Assert(m_player == g_network.IdToIndex(id));
		if(m_player != g_network.IdToIndex(id))
			return;
	}

#ifdef _DEBUG
	static ArmyList al;
	al.Clear();
	for(i = 0; i < m_numUnits; i++) {
		Unit unit(m_unitId[i]);
		Assert(g_theUnitPool->IsValid(unit));
		al.Insert(unit);
	}

	ArmyList *realAl = g_player[m_player]->GetArmy(m_index);
	Assert(m_numUnits == realAl->Num());
	for(i = 0; i < m_numUnits; i++) {
		Assert(al[i] == realAl->Access(i));
	}
#endif

	g_player[m_player]->RemoveArmyFromPlayer(m_index, m_cause,
											 TRUE, -1); 
#endif
}

NetArmy::NetArmy(ArmyData *data)
{
	m_data = data;
}

void NetArmy::Packetize(uint8 *buf, uint16 &size)
{
	size = 0;
	PUSHID(k_PACKET_ARMY_ID);

	PUSHLONG(m_data->m_id);

	uint8 c = (uint8)m_data->m_nElements;
	PUSHBYTE(c);
	sint32 i;
	for(i = 0; i < c; i++) {
		PUSHLONG((uint32)m_data->Access(i));
	}
	PUSHBYTE((uint8)m_data->m_owner);
	PUSHSHORT((uint16)m_data->m_pos.x);
	PUSHSHORT((uint16)m_data->m_pos.y);
	PUSHLONG(m_data->m_removeCause);
}

void NetArmy::Unpacketize(uint16 id, uint8 *buf, uint16 size)
{
	sint32 pos = 0;
	uint16 packid;
	sint32 i;
	PULLID(packid);
	Assert(packid == k_PACKET_ARMY_ID);

	Army army;
	PULLLONGTYPE(army, Army);
	
	g_network.CheckReceivedObject((uint32)army);

	if(g_theArmyPool->IsValid(army)) {
		m_data = g_theArmyPool->AccessArmy(army);
	} else {
		m_data = new ArmyData(army);
	}

	uint8 c;
	PULLBYTE(c);
	m_data->Clear();
	for(i = 0; i < c; i++) {
		Unit u;
		PULLLONGTYPE(u, Unit);
		if(g_theUnitPool->IsValid(u))
			m_data->Insert(u);
	}

	PULLBYTE(m_data->m_owner);
	PULLSHORT(m_data->m_pos.x);
	PULLSHORT(m_data->m_pos.y);
	PULLLONGTYPE(m_data->m_removeCause, CAUSE_REMOVE_ARMY);

	if(!g_theArmyPool->IsValid(army)) {
		g_theArmyPool->HackSetKey(((uint32)m_data->m_id & k_ID_KEY_MASK) + 1);
		g_theArmyPool->Insert(m_data);

		CtpAi::AddGoalsForArmy(army);
	}
}

NetGroupRequest::NetGroupRequest(const CellUnitList &units, const Army &army)
{
	m_armyId = army.m_id;
	m_units = units;
}

void NetGroupRequest::Packetize(uint8 *buf, uint16 &size)
{
	size  = 0;
	PUSHID(k_PACKET_GROUP_REQUEST_ID);

	PUSHLONG(m_armyId);
	uint8 n;
	n = (uint8)m_units.Num();
	PUSHBYTE(n);
	uint8 i;
	for(i = 0; i < n; i++) {
		PUSHLONG(m_units[i].m_id);
	}
}

void NetGroupRequest::Unpacketize(uint16 id, uint8 *buf, uint16 size)
{
	sint32 pos = 0;
	uint16 packid;
	sint32 pl = g_network.IdToIndex(id);

	PULLID(packid);
	Assert(packid == k_PACKET_GROUP_REQUEST_ID);

	PULLLONG(m_armyId);

	Army theArmy;

	if(m_armyId != 0) {
		Assert(g_theArmyPool->IsValid(m_armyId));
		if(!g_theArmyPool->IsValid(m_armyId)) {
			g_network.Resync(g_network.IdToIndex(id));
			return;
		}
		theArmy.m_id = m_armyId;
	} else {
		theArmy = g_player[pl]->GetNewArmy(CAUSE_NEW_ARMY_REMOTE_GROUPING);
		g_network.Enqueue(new NetInfo(NET_INFO_CODE_ADD_ARMY, pl, CAUSE_NEW_ARMY_REMOTE_GROUPING, theArmy.m_id));
	}
	
	uint8 n, i;
	PULLBYTE(n);
	m_units.Clear();
	for(i = 0; i < n; i++) {
		uint32 uid;
		PULLLONG(uid);
		m_units.Insert(uid);
		theArmy->GroupUnit(m_units[i]);
		g_network.QueuePacket(id, new NetInfo(NET_INFO_CODE_REMOTE_GROUP, theArmy, m_units[i].m_id));
	}
}

NetUngroupRequest::NetUngroupRequest(const Army &army, const CellUnitList &units)
{
	m_armyId = army.m_id;
	m_units = units;
}

void NetUngroupRequest::Packetize(uint8 *buf, uint16 &size)
{
	size = 0;
	PUSHID(k_PACKET_UNGROUP_REQUEST_ID);

	PUSHLONG(m_armyId);
	uint8 n = (uint8)m_units.Num();
	PUSHBYTE(n);
	uint8 i;
	for(i = 0; i < n; i++) {
		PUSHLONG(m_units[i].m_id);
	}
}

void NetUngroupRequest::Unpacketize(uint16 id, uint8 *buf, uint16 size)
{
	sint32 pos = 0;
	uint16 packid;
	sint32 pl = g_network.IdToIndex(id);

	PULLID(packid);
	Assert(packid == k_PACKET_UNGROUP_REQUEST_ID);

	PULLLONG(m_armyId);

	Army theArmy(m_armyId);
	Assert(theArmy.IsValid());
	if(!theArmy.IsValid()) {
		g_network.Resync(pl);
		return;
	}

	uint8 n, i;
	PULLBYTE(n);
	for(i = 0; i < n; i++) {
		Army newArmy = g_player[pl]->GetNewArmy(CAUSE_NEW_ARMY_REMOTE_UNGROUPING);
		g_network.Enqueue(new NetInfo(NET_INFO_CODE_ADD_ARMY, pl, CAUSE_NEW_ARMY_REMOTE_UNGROUPING, newArmy.m_id));

		uint32 unitId;
		PULLLONG(unitId);
		m_units.Insert(Unit(unitId));
		m_units[i].ChangeArmy(newArmy, CAUSE_NEW_ARMY_REMOTE_UNGROUPING);
	}
	g_network.Enqueue(new NetInfo(NET_INFO_CODE_REMOTE_UNGROUP, theArmy, pl));
}

