//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : Multiplayer unit packet handling.
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
// - Made government modified for units work here. (July 29th 2006 Martin Gühmann)
//
//----------------------------------------------------------------------------

#include "c3.h"
#include "network.h"
#include "net_unit.h"
#include "net_util.h"

#include "UnitData.h"
#include "UnitPool.h"           // g_theUnitPool
#include "World.h"              // g_theWorld
#include "player.h"             // g_player

#include "DB.h"
#include "SelItem.h"            // g_selected_item
#include "director.h"           // g_director
#include "tech_wllist.h"

#include "Cell.h"

#include "AICause.h"
#include "ctpai.h"

//----------------------------------------------------------------------------
//
// Name       : NetUnit::NetUnit
//
// Description: Constructor
//
// Parameters : UnitData * unit:  Unit data to send through the network
//              Unit useActor:    According unit actor
//
// Globals    : -
//
// Returns    : -
//
// Remark(s)  : -
//
//----------------------------------------------------------------------------
NetUnit::NetUnit(UnitData * unit, Unit useActor)
:
    Packetizer      (),
    m_unitData      (unit),
    m_actorId       (useActor)
{
}

//----------------------------------------------------------------------------
//
// Name       : NetUnit::Packetize
//
// Description: Generate an application data packet to transmit.
//
// Parameters : buf         : buffer to store the message
//
// Globals    : -
//
// Returns    : size        : number of bytes stored in buf
//
// Remark(s)  : -
//
//----------------------------------------------------------------------------
void NetUnit::Packetize(uint8* buf, uint16& size)
{
	size = 0;
	PUSHID(k_PACKET_UNIT_ID);

	PUSHLONG(m_unitData->m_id);
	PUSHLONG(m_actorId);

	uint16 unitSize;
	PacketizeUnit(&buf[size], unitSize, m_unitData);

	size = size + unitSize;
}

//----------------------------------------------------------------------------
//
// Name       : NetUnit::Unpacketize
//
// Description: Retrieve the data from a received application data packet.
//
// Parameters : id          : Sender identification?
//              buf         : Buffer with received message
//              size        : Length of received message (in bytes)
//
// Globals    : -
//
// Returns    : -
//
// Remark(s)  : -
//
//----------------------------------------------------------------------------
void NetUnit::Unpacketize(uint16 id, uint8* buf, uint16 size)
{
	uint16 packid;
	sint32 pos = 0;
	PULLID(packid);
	Assert(packid == k_PACKET_UNIT_ID);

	Unit uid;
	PULLLONGTYPE(uid, Unit);
	PULLLONGTYPE(m_actorId, Unit);

	g_network.CheckReceivedObject(uid.m_id);

	uint16 unitSize;
	if (uid.IsValid())
	{
		m_unitData = g_theUnitPool->AccessUnit((const Unit)getlong(&buf[2]));
		MapPoint pnt = m_unitData->m_pos;
		PLAYER_INDEX oldowner = m_unitData->m_owner;
		uint32 oldFlags = m_unitData->m_flags;

		UnpacketizeUnit(&buf[pos], unitSize, m_unitData);
		pos += unitSize;

		if(pnt != m_unitData->m_pos)
		{
			UnitDynamicArray revealed;
			MapPoint newPos = m_unitData->m_pos;
			DPRINTF(k_DBG_NET, ("Net: Unit %lx moved to %d,%d via unit packet\n",
								m_unitData->m_id, newPos.x, newPos.y));
			m_unitData->m_pos = pnt;
			bool addVision = false;
			if(!(oldFlags & k_UDF_TEMP_SLAVE_UNIT))
			{
				g_theWorld->RemoveUnitReference(pnt, m_unitData->m_id);
				addVision = (m_unitData->m_flags & k_UDF_VISION_ADDED) != 0;
				m_unitData->RemoveUnitVision();
			}
			else if(!(m_unitData->m_flags & k_UDF_TEMP_SLAVE_UNIT))
			{
				addVision = true;
			}
			m_unitData->m_pos = newPos;

			if(addVision) {
				m_unitData->ClearFlag(k_UDF_VISION_ADDED);
				m_unitData->AddUnitVision();
			}
			g_theWorld->InsertUnit(m_unitData->m_pos, uid, revealed);

			if(m_unitData->m_visibility & (1 << g_selected_item->GetVisiblePlayer())) {
				sint32 numRevealed = revealed.Num();
				UnitActor **revealedActors;
				if(numRevealed > 0) {
					revealedActors = new UnitActor*[numRevealed];
					for (sint32 i=0; i<numRevealed; i++)
						revealedActors[i] = revealed[i].GetActor();
				} else {
					revealedActors = NULL;
				}

				g_director->AddMove(uid, pnt, m_unitData->m_pos, numRevealed, revealedActors,
										NULL, NULL, uid.GetMoveSoundID());
			}
		}
#if 0
		else if(oldVisionRange != m_unitData->m_vision_range) {
			g_player[m_unitData->GetOwner()]->RemoveUnitVision(m_unitData->m_pos, oldVisionRange);
			g_player[m_unitData->GetOwner()]->AddUnitVision(m_unitData->m_pos, m_unitData->m_vision_range);
		}
#endif

		if(oldowner != m_unitData->m_owner) {
			DPRINTF(k_DBG_NET, ("Resetting unit %lx (type %d) from owner %d to %d\n",
								uid.m_id, m_unitData->m_type,
								oldowner, m_unitData->m_owner));
			if(m_unitData->GetDBRec()->GetHasPopAndCanBuild()) {
				DPRINTF(k_DBG_NET, ("But it's a city and I'm going to assert and ignore it.\n"));
				BOOL ahaSoItDoesHappen = FALSE;
				Assert(ahaSoItDoesHappen);
			} else {
				g_player[oldowner]->RemoveUnitReference(uid, CAUSE_REMOVE_ARMY_UNKNOWN, m_unitData->m_owner);
				g_player[m_unitData->m_owner]->InsertUnitReference(uid, CAUSE_NEW_ARMY_UNKNOWN, Unit());
			}
		}

		if(m_unitData->m_army.m_id != (0)) {
			m_unitData->m_army.ResetPos();
		}
	} else {
		if(g_network.DeadUnit(uint32(uid))) {
			return;
		}

		PLAYER_INDEX unitOwner = (PLAYER_INDEX)getshort(&buf[10]);
		uint32 unitType = getlong(&buf[12]);
		MapPoint unitPos((sint32)getshort(&buf[16]),
						 (sint32)getshort(&buf[18]));
		uint32 flags = getlong(&buf[20]);

		g_theUnitPool->HackSetKey(((uint32)uid & k_ID_KEY_MASK) + 1);

		sint32 trans_t = 0;
        (void) g_theUnitDB->Get(unitType, g_player[unitOwner]->GetGovernmentType())->GetTransType(trans_t);
		if (m_actorId.m_id != 0)
        {
			m_unitData = new UnitData(unitType, trans_t, uid, unitOwner,
									  unitPos, Unit(),
									  m_actorId.AccessData()->m_actor);
			m_actorId.AccessData()->m_actor = NULL;
		}
        else
        {
			if(flags & k_UDF_TEMP_SLAVE_UNIT)
            {
				m_unitData = new UnitData(unitType, trans_t,
										  uid, unitOwner, unitPos);
            }
            else
            {
				m_unitData = new UnitData(unitType, trans_t,
										  uid, unitOwner, unitPos, Unit());
			}
		}

		UnpacketizeUnit(&buf[pos], unitSize, m_unitData);
		pos += unitSize;

		g_theUnitPool->Insert(m_unitData);

#if 0
		if(m_unitData->m_vision_range != oldVisionRange) {
			double newRange = m_unitData->m_vision_range;
			m_unitData->m_vision_range = oldVisionRange;
			m_unitData->RemoveUnitVision();
			m_unitData->m_vision_range = newRange;
			m_unitData->AddUnitVision();
		}
#endif

		if(m_unitData->GetDBRec()->GetHasPopAndCanBuild()) {
			m_unitData->GetCityData()->NetworkInitialize();

			g_player[m_unitData->m_owner]->AddCityReferenceToPlayer(
				uid, CAUSE_NEW_CITY_UNKNOWN);
			g_theWorld->InsertCity(m_unitData->m_pos, uid);

		} else if(m_unitData->GetDBRec()->GetIsTrader()) {
			g_player[m_unitData->m_owner]->AddTrader(uid);
		} else {
			UnitDynamicArray revealed;
			if(m_unitData->IsBeingTransported()) {

			} else if(!m_unitData->Flag(k_UDF_TEMP_SLAVE_UNIT)) {
				g_theWorld->InsertUnit(m_unitData->m_pos, uid, revealed);
			}
			if (!m_unitData->Flag(k_UDF_TEMP_SLAVE_UNIT))
            {
				g_player[m_unitData->m_owner]->InsertUnitReference
                    (uid, CAUSE_NEW_ARMY_NETWORK, Unit());
			}
		}
	}
	Assert(pos == size);
}

void NetUnit::PacketizeUnit(uint8* buf, uint16& size, UnitData* unitData)
{
	uint8* ptr = buf;
	putshort(ptr, (uint16)unitData->m_owner); ptr += 2;
	putlong(ptr, unitData->m_type); ptr += 4;
	putshort(ptr, (uint16)unitData->m_pos.x); ptr += 2;
	putshort(ptr, (uint16)unitData->m_pos.y); ptr += 2;

	putlong(ptr, unitData->m_flags); ptr += 4;

	putlong(ptr, unitData->m_fuel); ptr += 4;
	putdouble(ptr, unitData->m_hp); ptr += 8;
	memcpy(ptr, &unitData->m_movement_points, sizeof(double)); ptr += sizeof(double);


	putlong(ptr, unitData->m_temp_visibility_array.m_array_index); ptr += sizeof(unitData->m_temp_visibility_array.m_array_index);

#if 0
	uint32 mask = 0;
	sint32 i;

	for(i = 0; i < k_MAX_PLAYERS; i++) {
		if(g_player[i]) {
			mask |= (1 << i);
		}
	}

	putlong(ptr, mask); ptr += 4;

	for(i = 0; i < k_MAX_PLAYERS; i++) {
		if(!g_player[i]) continue;
		putbyte(ptr, unitData->m_temp_visibility_array.m_array_index[i]);
		ptr++;
	}
#endif

	putlong(ptr, (uint32)unitData->m_army); ptr += 4;

	putlong(ptr, (uint32)unitData->m_visibility); ptr += 4;
	putlong(ptr, (uint32)unitData->m_temp_visibility); ptr += 4;
	putlong(ptr, (uint32)unitData->m_radar_visibility); ptr += 4;
	putlong(ptr, (uint32)unitData->m_ever_visible); ptr += 4;


	putlong(ptr, (uint32)unitData->m_transport); ptr += 4;

	uint8 canHaveCargo = unitData->m_cargo_list != NULL;
	putbyte(ptr, canHaveCargo); ptr++;
	if(canHaveCargo) {
		uint8 transportedUnits = (uint8)unitData->m_cargo_list->Num();
		putbyte(ptr, transportedUnits); ptr++;
		for (uint8 i = 0; i < transportedUnits; i++) {
			putlong(ptr, (uint32)unitData->m_cargo_list->Access(i)); ptr += 4;
		}
	}

	putlong(ptr, (uint32)unitData->m_target_city.m_id); ptr += 4;

	size = static_cast<uint16>(ptr - buf);
}

void NetUnit::UnpacketizeUnit(uint8* buf, uint16& size, UnitData* unitData)
{
	uint8* ptr = buf;

	unitData->m_owner = (PLAYER_INDEX)getshort(ptr); ptr += 2;
	unitData->m_type = getlong(ptr); ptr += 4;
	unitData->m_pos.x = getshort(ptr); ptr += 2;
	unitData->m_pos.y = getshort(ptr); ptr += 2;

	unitData->m_flags = getlong(ptr); ptr += 4;

	unitData->m_fuel = getlong(ptr); ptr += 4;
	unitData->m_hp = getdouble(ptr); ptr += 8;
	memcpy(&unitData->m_movement_points, ptr, sizeof(double)); ptr += sizeof(double);

	unitData->m_temp_visibility_array.m_array_index = getlong(ptr); ptr += sizeof(unitData->m_temp_visibility_array.m_array_index);
#if 0
	uint32 mask = getlong(ptr); ptr += 4;

	for (sint32 i = 0; i < k_MAX_PLAYERS; i++) {
		if(!g_player[i]) continue;
		if(!(mask & (1 << i))) continue;
		unitData->m_temp_visibility_array.m_array_index[i] = getbyte(ptr); ptr++;
	}
#endif

	unitData->m_army = Army(getlong(ptr)); ptr += 4;

	unitData->m_visibility = getlong(ptr); ptr += 4;
	unitData->m_temp_visibility = getlong(ptr); ptr += 4;
	unitData->m_radar_visibility = getlong(ptr); ptr += 4;
	unitData->m_ever_visible = getlong(ptr); ptr += 4;


	if (unitData->m_actor) {
		g_director->AddSetVisibility(unitData->m_actor, unitData->GetVisibility());
	}


	unitData->m_transport = Unit(getlong(ptr)); ptr += 4;

	uint8 canHaveCargo = getbyte(ptr); ptr++;
	if(canHaveCargo) {
		if(!unitData->m_cargo_list) {
			unitData->m_cargo_list = new UnitDynamicArray;
		}
		uint8 transportedUnits = getbyte(ptr); ptr++;
		unitData->m_cargo_list->Clear();
		for (uint8 i = 0; i < transportedUnits; i++)
        {
			Unit tunit = Unit(getlong(ptr)); ptr += 4;
			unitData->m_cargo_list->Insert(tunit);
		}
	}

	unitData->m_target_city.m_id = getlong(ptr); ptr += 4;

	size = static_cast<uint16>(ptr - buf);
}

NetUnitMove::NetUnitMove(const Unit id, const MapPoint &pnt)
{
	m_id = id;
	m_point = pnt;
}

void NetUnitMove::Packetize(uint8 *buf, uint16 &size)
{
	size = 0;
	PUSHID(k_PACKET_UNIT_MOVE_ID);

	PUSHLONG(m_id);
	PUSHSHORT(m_point.x);
	PUSHSHORT(m_point.y);
}

void NetUnitMove::Unpacketize(uint16 id, uint8 *buf, uint16 size)
{
	sint32 pos = 0;
	uint16 packid;
	PULLID(packid);
	Assert(packid == k_PACKET_UNIT_MOVE_ID);

	PULLLONG(m_id);
	PULLSHORT(m_point.x);
	PULLSHORT(m_point.y);

	Unit u(m_id);
	Assert(u.IsValid());
	if(!u.IsValid())
		return;

	DPRINTF(k_DBG_NET, ("Net: Unit %lx moved to %d,%d via move packet\n",
						m_id, m_point.x, m_point.y));
	UnitData *ud = u.AccessData();

	UnitDynamicArray revealed;
	MapPoint oldPos = ud->m_pos;
	g_theWorld->RemoveUnitReference(ud->m_pos, u);
	g_player[ud->GetOwner()]->RemoveUnitVision(ud->m_pos, ud->GetVisionRange());
	ud->m_pos = m_point;
	g_player[ud->GetOwner()]->AddUnitVision(ud->m_pos, ud->GetVisionRange());
	g_theWorld->InsertUnit(ud->m_pos, u, revealed);

	sint32 numRevealed = revealed.Num();
	UnitActor **revealedActors = new UnitActor*[numRevealed];
	for (sint32 i=0; i<numRevealed; i++)
		revealedActors[i] = revealed[i].GetActor();

	g_director->AddMove(u, oldPos, ud->m_pos, numRevealed, revealedActors,
							NULL, NULL, u.GetMoveSoundID());
}

void NetUnitHP::Packetize(uint8 *buf, uint16 &size)
{
	size = 0;
	PUSHID(k_PACKET_UNIT_HP_ID);
	PUSHLONG(m_unit.m_id);
	PUSHDOUBLE(m_hp);
}

void NetUnitHP::Unpacketize(uint16 id, uint8 *buf, uint16 size)
{
	uint16 packid;
	uint16 pos = 0;
	PULLID(packid);
	Assert(packid == k_PACKET_UNIT_HP_ID);

	PULLLONGTYPE(m_unit, Unit);
	PULLDOUBLE(m_hp);
	Assert(m_unit.IsValid());
	if (m_unit.IsValid())
    {
		DPRINTF(k_DBG_NET, ("NetUnitHP for %lx (owner=%d): %lf\n", m_unit.m_id, m_unit.GetOwner(), m_hp));
		m_unit.SetHP(m_hp);
	}
}
