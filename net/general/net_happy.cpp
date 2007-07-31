








#include "c3.h"
#include "network.h"
#include "net_happy.h"
#include "net_util.h"
#include "Unit.h"
#include "UnitData.h"
#include "CityData.h"
#include "Happy.h"
#include "UnitPool.h"
#include "PlayHap.h"
#include "Player.h"

extern UnitPool *g_theUnitPool;
extern Player **g_player;

NetHappy::NetHappy(Unit city, Happy *data, BOOL isInitial)
{
	m_city = city;
	m_data = data;
	m_isInitialPacket = isInitial;
}

void
NetHappy::Packetize(uint8 *buf, uint16 &size)
{
	buf[0] = k_PACKET_HAPPY_ID >> 8;
	buf[1] = k_PACKET_HAPPY_ID & 0xff;

	size = 2;
	PUSHLONG((uint32)m_city);
	PUSHBYTE(m_isInitialPacket);
	PUSHDOUBLE(m_data->m_happiness);

	PUSHDOUBLE(m_data->m_last_captured);

	PUSHDOUBLE(m_data->m_base); 
	PUSHDOUBLE(m_data->m_size); 
	PUSHDOUBLE(m_data->m_pollution); 
	PUSHDOUBLE(m_data->m_conquest_distress);
	PUSHDOUBLE(m_data->m_empire_dist);
	PUSHDOUBLE(m_data->m_enemy_action); 
	PUSHDOUBLE(m_data->m_peace); 
	
	PUSHDOUBLE(m_data->m_workday); 
	PUSHDOUBLE(m_data->m_wages); 
	PUSHDOUBLE(m_data->m_rations); 
	
	PUSHDOUBLE(m_data->m_martial_law);
	
	PUSHDOUBLE(m_data->m_pop_ent); 
	PUSHDOUBLE(m_data->m_improvement); 
	PUSHDOUBLE(m_data->m_wonders); 
	
	PUSHDOUBLE(m_data->m_dist_to_capitol);
	PUSHDOUBLE(m_data->m_too_many_cities);

	PUSHDOUBLE(m_data->m_crime); 
	PUSHLONG(m_data->m_cost_to_capitol);
	PUSHDOUBLE(m_data->m_dist_to_capitol);
	PUSHLONG(m_data->m_fullHappinessTurns);
	PUSHDOUBLE(m_data->m_timed);
	uint8 n = (uint8)m_data->m_timedChanges->Num();
	PUSHBYTE(n);
	uint8 i;
	for(i = 0; i < n; i++) {
		PUSHLONG(m_data->m_timedChanges->Access(i).m_turnsRemaining);
		PUSHDOUBLE(m_data->m_timedChanges->Access(i).m_adjustment);
		PUSHBYTE((uint8)m_data->m_timedChanges->Access(i).m_reason);
	}
	
}

void
NetHappy::Unpacketize(uint16 id, uint8 *buf, uint16 size)
{
	Assert(MAKE_CIV3_ID(buf[0], buf[1]) == k_PACKET_HAPPY_ID);

	sint32 pos = 2;
	uint32 unitid;

	PULLLONG(unitid);
	
	if(!g_theUnitPool->IsValid(unitid))
		return;

	Unit u(unitid);
	CityData *cd = u.AccessData()->GetCityData();
	Assert(cd);
	if(!cd)
		return;

	PULLBYTE(m_isInitialPacket);

	bool resync = false;

	m_data = cd->m_happy;
#define PDCHK(x) {double tmp = x; PULLDOUBLE(x); if(cd->GetOwner() == g_network.GetPlayerIndex() && !m_isInitialPacket) { Assert((tmp >= (x - 0.00000001)) && (tmp <= (x + 0.000000001))); if((tmp < (x - 0.000000001)) || (tmp > (x + 0.000000001))) resync = true; }}

	PDCHK(m_data->m_happiness);

	PDCHK(m_data->m_last_captured);

	PDCHK(m_data->m_base); 
	PDCHK(m_data->m_size); 
	PDCHK(m_data->m_pollution); 
	PDCHK(m_data->m_conquest_distress);
	PDCHK(m_data->m_empire_dist);
	PDCHK(m_data->m_enemy_action); 
	PDCHK(m_data->m_peace); 
	
	PDCHK(m_data->m_workday); 
	PDCHK(m_data->m_wages); 
	PDCHK(m_data->m_rations); 
	
	PDCHK(m_data->m_martial_law);
	
	PDCHK(m_data->m_pop_ent); 
	PDCHK(m_data->m_improvement); 
	PDCHK(m_data->m_wonders); 
	
	PDCHK(m_data->m_dist_to_capitol);
	PDCHK(m_data->m_too_many_cities);

	PDCHK(m_data->m_crime); 
	sint32 oldCost = m_data->m_cost_to_capitol;
	PULLLONG(m_data->m_cost_to_capitol);
	if(!m_isInitialPacket && cd->GetOwner() == g_network.GetPlayerIndex()) {
		Assert(oldCost == m_data->m_cost_to_capitol);
		if(oldCost != m_data->m_cost_to_capitol)
			resync = true;
	}

	PDCHK(m_data->m_dist_to_capitol);
	sint32 oldFullHappiness = m_data->m_fullHappinessTurns;
	PULLLONG(m_data->m_fullHappinessTurns);
	if(!m_isInitialPacket && cd->GetOwner() == g_network.GetPlayerIndex()) {
		Assert(oldFullHappiness == m_data->m_fullHappinessTurns);
		if(oldFullHappiness != m_data->m_fullHappinessTurns)
			resync = true;
	}

	PULLDOUBLE(m_data->m_timed);

	if(m_isInitialPacket || cd->GetOwner() != g_network.GetPlayerIndex()) {
		m_data->m_timedChanges->Clear();
	}
	
	uint8 n;
	PULLBYTE(n);
	if(!m_isInitialPacket && cd->GetOwner() == g_network.GetPlayerIndex()) {
		Assert(n == (uint8)m_data->m_timedChanges->Num());
		if(n != (uint8)m_data->m_timedChanges->Num()) {
			resync = true;
		}
	}

	if(!resync) {
		uint8 i;
		for(i = 0; i < n; i++) {
			HappyTimer ins;
			PULLLONG(ins.m_turnsRemaining);
			PULLDOUBLE(ins.m_adjustment);
			PULLBYTETYPE(ins.m_reason, HAPPY_REASON);
			
			if(m_isInitialPacket || cd->GetOwner() != g_network.GetPlayerIndex()) {
				m_data->m_timedChanges->Insert(ins);
			} else {
				Assert(m_data->m_timedChanges->Access(i).m_turnsRemaining ==
					   ins.m_turnsRemaining);
				if(m_data->m_timedChanges->Access(i).m_turnsRemaining !=
				   ins.m_turnsRemaining)
					resync = TRUE;

				Assert(m_data->m_timedChanges->Access(i).m_adjustment ==
					   ins.m_adjustment);
				if(m_data->m_timedChanges->Access(i).m_adjustment !=
				   ins.m_adjustment)
					resync = TRUE;

				Assert(m_data->m_timedChanges->Access(i).m_reason ==
					   ins.m_reason);
				if(m_data->m_timedChanges->Access(i).m_reason !=
				   ins.m_reason)
					resync = TRUE;
			}
		}
		Assert(pos == size);
	}
	if(resync) {
		g_network.RequestResync(RESYNC_CITY_STATS);
	}
}

NetPlayerHappy::NetPlayerHappy(uint8 owner, PlayerHappiness *hap,
							   uint8 isInitialPacket)
{
	m_owner = owner;
	m_playerHappiness = hap;
	m_isInitialPacket = isInitialPacket;
}

void
NetPlayerHappy::Packetize(uint8 *buf, uint16 &size)
{
	buf[0] = k_PACKET_PLAYER_HAPPY_ID >> 8;
	buf[1] = k_PACKET_PLAYER_HAPPY_ID & 0xff;
	
	size = 2;

	PUSHBYTE(m_owner);
	PUSHBYTE(m_isInitialPacket);

	
    PUSHDOUBLE(m_playerHappiness->m_workday_per_person); 
    PUSHLONG(m_playerHappiness->m_unitless_workday); 

    PUSHDOUBLE(m_playerHappiness->m_rations_per_person); 
    PUSHLONG(m_playerHappiness->m_unitless_rations); 

    PUSHDOUBLE(m_playerHappiness->m_wages_per_person); 
    PUSHLONG(m_playerHappiness->m_unitless_wages); 

    PUSHDOUBLE(m_playerHappiness->m_raw_overseas_defeat);
    PUSHDOUBLE(m_playerHappiness->m_raw_home_defeat);

    PUSHDOUBLE(m_playerHappiness->m_overseas_defeat);
    PUSHDOUBLE(m_playerHappiness->m_home_defeat);

    PUSHDOUBLE(m_playerHappiness->m_dist_overseas);
    PUSHDOUBLE(m_playerHappiness->m_overseas); 
}

void
NetPlayerHappy::Unpacketize(uint16 id, uint8 *buf, uint16 size)
{
	Assert(MAKE_CIV3_ID(buf[0], buf[1]) == k_PACKET_PLAYER_HAPPY_ID);

	sint32 pos = 2;
	PULLBYTE(m_owner);
	m_playerHappiness = g_player[m_owner]->m_global_happiness;
	PULLBYTE(m_isInitialPacket);

#define PDCHK2(x) {double tmp = x; PULLDOUBLE(x); if((m_owner == g_network.GetPlayerIndex()) && !m_isInitialPacket) { Assert(tmp == x); }}

    PDCHK2(m_playerHappiness->m_workday_per_person); 
    PULLLONG(m_playerHappiness->m_unitless_workday); 

    PDCHK2(m_playerHappiness->m_rations_per_person); 
    PULLLONG(m_playerHappiness->m_unitless_rations); 

    PDCHK2(m_playerHappiness->m_wages_per_person); 
    PULLLONG(m_playerHappiness->m_unitless_wages); 

    PDCHK2(m_playerHappiness->m_raw_overseas_defeat);
    PDCHK2(m_playerHappiness->m_raw_home_defeat);

    PDCHK2(m_playerHappiness->m_overseas_defeat);
    PDCHK2(m_playerHappiness->m_home_defeat);

    PDCHK2(m_playerHappiness->m_dist_overseas);
    PDCHK2(m_playerHappiness->m_overseas); 
}
