








#include "c3.h"

#include "network.h"
#include "net_city.h"
#include "net_unit.h"

#include "net_util.h"

#include "UnitData.h"
#include "CityData.h"

#include "UnitPool.h"
#include "Player.h"
#include "AiCause.h"
#include "Unit.h"
#include "BldQue.h"

#include "ResourceRecord.h"
#include "Resources.h"

#include "PointerList.h"

#include "MainControlPanel.h"
#include "SelItem.h"

NetCity::NetCity(UnitData* unit, BOOL isInitial) : 
	m_unitData(unit) 
{
	m_unitId = unit->m_id;
	m_isInitialPacket = isInitial;
}

void NetCity::Packetize(uint8* buf, uint16& size)
{
	buf[0] = 'C';
	buf[1] = 'D';
	uint16 unitSize;

	putlong(&buf[2], m_unitData->m_id);
	NetUnit::PacketizeUnit(&buf[6], unitSize, m_unitData);

	size = 6 + unitSize;
	CityData* cityData = m_unitData->m_city_data;

	PUSHBYTE(m_isInitialPacket);

	PUSHLONG(cityData->m_slaveBits);
	PUSHLONG(cityData->m_shieldstore);
	PUSHLONG(cityData->m_shieldstore_at_begin_turn);
	PUSHLONG(cityData->m_trade);
	PUSHLONG(cityData->m_science);
	PUSHLONG(cityData->m_luxury);
	PUSHLONG(cityData->m_city_attitude);
	PUSHLONG64((uint64)cityData->m_built_improvements);

	
	

#ifdef CTP1_TRADE
	uint8 numNonZeroResources = 0;
	
	sint32 resourceCountPosition = size++;

	sint32 i;

	for(i = 0; i < g_theResourceDB->NumRecords(); i++) {
		if(cityData->m_resources.GetResourceCount(i) > 0) {
			numNonZeroResources++;
			PUSHBYTE((uint8)i);
			PUSHSHORT((uint16)cityData->m_resources.GetResourceCount(i));
		}
	}
	buf[resourceCountPosition] = numNonZeroResources;
#endif

	sint32 i;
	PUSHLONG(cityData->m_population);
	for(i = 0; i < (sint32)POP_MAX; i++) {
		PUSHSHORT(cityData->m_numSpecialists[i]);
	}
	PUSHLONG(cityData->m_partialPopulation);
	PUSHLONG(cityData->m_sizeIndex);
	PUSHLONG(cityData->m_workerFullUtilizationIndex);
	PUSHLONG(cityData->m_workerPartialUtilizationIndex);

	PUSHBYTE((uint8)cityData->m_founder);

	PacketizeResources(cityData->m_collectingResources, buf, size);
	PacketizeResources(cityData->m_sellingResources, buf, size);
	PacketizeResources(cityData->m_buyingResources, buf, size);

	sint32 r;
	for(r = 0; r < g_theResourceDB->NumRecords(); r++) {
		PUSHLONG(cityData->m_distanceToGood[r]);
	}
	
}

void NetCity::PacketizeResources(const Resources &resources, uint8 *buf, uint16 &size)
{
	sint32 resourceCount;
	resourceCount = resources.m_numGoods;
	sint32 r;
	PUSHLONG(resourceCount);
	for(r = 0; r < resourceCount; r++) {
		PUSHLONG(resources.m_supply[r]);
	}
	PUSHLONG(resources.m_totalResources);
}

void NetCity::UnpacketizeResources(Resources &resources, uint8 *buf, sint32 &pos)
{
	sint32 resourceCount;
	PULLLONG(resourceCount);

	Assert(resourceCount == resources.m_numGoods);
	if(resourceCount != resources.m_numGoods) {
		resources.m_numGoods = resourceCount;
		delete [] resources.m_supply;
		resources.m_supply = new sint32[resourceCount];
	}

	sint32 r;
	for(r = 0; r < resourceCount; r++) {
		PULLLONG(resources.m_supply[r]);
	}
	PULLLONG(resources.m_totalResources);
}

void NetCity::Unpacketize(uint16 id, uint8* buf, uint16 size)
{
	Assert(buf[0] == 'C' && buf[1] == 'D');
	Unit uid(getlong(&buf[2]));
	sint32 pos;

	
	
	
	

	if(g_theUnitPool->IsValid(uid)) {
		DPRINTF(k_DBG_NET, ("Net: received city %lx\n", (uint32)uid));
		UnitData* unitData = g_theUnitPool->AccessUnit(uid);
		uint16 unitSize;
		PLAYER_INDEX oldOwner = unitData->m_owner;

		Assert(unitData->m_city_data != NULL);
		if(unitData->m_city_data == NULL) {
			g_network.RequestResync(RESYNC_INVALID_UNIT);
			return;
		}

		NetUnit::UnpacketizeUnit(&buf[6], unitSize, unitData);

		if(oldOwner != unitData->m_owner) {
			g_player[oldOwner]->RemoveCityReferenceFromPlayer(uid, CAUSE_REMOVE_CITY_UNKNOWN, unitData->m_owner);
			g_player[unitData->m_owner]->AddCityReferenceToPlayer(uid, CAUSE_NEW_CITY_UNKNOWN);
		}
		unitData->m_city_data->m_owner = unitData->m_owner;

		pos = 6 + unitSize;
		CityData* cityData = unitData->m_city_data;

		sint32 oldSizeIndex = cityData->m_sizeIndex;
		double oldVision = cityData->GetVisionRadius();

		PULLBYTE(m_isInitialPacket);

		bool resync = false;

#define PLCHK(x) { sint32 tmp = x; PULLLONG(x); if(cityData->GetOwner() == g_network.GetPlayerIndex() && !m_isInitialPacket) { Assert(tmp == x); if(tmp != x) resync = true; }}
#define PSCHK(x) { sint16 tmp = x; PULLSHORT(x); if(cityData->GetOwner() == g_network.GetPlayerIndex() && !m_isInitialPacket) { Assert(tmp == x); if(tmp != x) resync = true; }}
#define PL64CHK(x) { uint64 tmp = x; PULLLONG64(x); if(cityData->GetOwner() == g_network.GetPlayerIndex() && !m_isInitialPacket) { Assert(tmp == x); if(tmp != x) resync = true; }}
		PLCHK(cityData->m_slaveBits);
		sint32 shieldstore;
		PULLLONG(shieldstore);
		if(cityData->GetOwner() != g_network.GetPlayerIndex() || m_isInitialPacket) {
			cityData->m_shieldstore = shieldstore;
		}
		PLCHK(cityData->m_shieldstore_at_begin_turn);
		PLCHK(cityData->m_trade);
		PLCHK(cityData->m_science);
		PLCHK(cityData->m_luxury);
		PULLLONGTYPE(cityData->m_city_attitude, CITY_ATTITUDE);

		PL64CHK(cityData->m_built_improvements);

		sint32 i;
#ifdef CTP1_TRADE
		uint8 numNonZeroResources;
		PULLBYTE(numNonZeroResources);
		cityData->m_resources.Clear();
		for(i = 0; i < numNonZeroResources; i++) {
			uint8 resource;
			uint16 count;
			PULLBYTE(resource);
			PULLSHORT(count);
			cityData->m_resources.SetResourceCount(resource, count);
		}
#endif

		PLCHK(cityData->m_population);
		for(i = 0; i < (sint32)POP_MAX; i++) {
			PSCHK(cityData->m_numSpecialists[i]);
		}

		PLCHK(cityData->m_partialPopulation);
		PLCHK(cityData->m_sizeIndex);
		PLCHK(cityData->m_workerFullUtilizationIndex);
		PLCHK(cityData->m_workerPartialUtilizationIndex);

		PULLBYTE(cityData->m_founder);

		UnpacketizeResources(cityData->m_collectingResources, buf, pos);
		UnpacketizeResources(cityData->m_sellingResources, buf, pos);
		UnpacketizeResources(cityData->m_buyingResources, buf, pos);

		sint32 r;
		for(r = 0; r < g_theResourceDB->NumRecords(); r++) {
			PULLLONG(cityData->m_distanceToGood[r]);
		}

		cityData->UpdateSprite();	

		if(resync)
			g_network.RequestResync(RESYNC_CITY_STATS);
		else if (oldVision != cityData->GetVisionRadius()) {
			unitData->RemoveOldUnitVision(oldVision);
			BOOL revealed;
			unitData->AddUnitVision(revealed);
		}
	}
}

void NetCityName::Packetize(uint8* buf, uint16 &size)
{
	size = 0;
	PUSHID(k_PACKET_CITY_NAME_ID);
	PUSHLONG((uint32)m_cityData->m_home_city);
	PUSHSTRING(m_cityData->m_name);
	
}

void NetCityName::Unpacketize(uint16 id, uint8* buf, uint16 size)
{
	sint32 pos = 0;
	uint16 packid;
	PULLID(packid);
	Assert(packid == k_PACKET_CITY_NAME_ID);
	Unit home_city;
	PULLLONGTYPE(home_city, Unit);
	char name[k_MAX_NAME_LEN];
	PULLSTRING(name);
	if(g_theUnitPool->IsValid(home_city)) {		
		if(!home_city->GetCityData()) {
			
			
			
			
			
			
			if(g_network.IsHost()) {
				g_network.Resync(g_network.IdToIndex(id));
			} else {
				g_network.RequestResync(RESYNC_INVALID_UNIT);
			}
		} else {
			strcpy(home_city.GetData()->GetCityData()->m_name, name);
			if(g_network.IsHost()) {
				g_network.Block(home_city.GetOwner());
				g_network.SendCityName(home_city.GetData()->GetCityData());
				g_network.Unblock(home_city.GetOwner());
			}

			if(home_city.GetOwner() == g_selected_item->GetVisiblePlayer()) {
				MainControlPanel::UpdateCityList();
			}
		}
	}
	
	
	Assert(pos == size);
}

void NetCity2::Packetize(uint8* buf, uint16 &size)
{
	size = 0;
	PUSHID(k_PACKET_CITY2_ID);

	PUSHLONG((uint32)m_data->m_home_city);
	
	PUSHLONG(m_data->m_trade_lost_to_crime);
	PUSHLONG(m_data->m_gross_trade);
	PUSHLONG(m_data->m_goldFromTradeRoutes);
	PUSHLONG(m_data->m_gross_production_this_turn);
	PUSHLONG(m_data->m_shields_this_turn);
	PUSHLONG(m_data->m_shields_lost_to_crime);
	PUSHLONG64((uint64)m_data->m_builtWonders);
	PUSHDOUBLE(m_data->m_food_delta);
	PUSHDOUBLE(m_data->m_gross_food_this_turn);
	PUSHDOUBLE(m_data->m_food_produced_this_turn);
	PUSHDOUBLE(m_data->m_food_lost_to_crime);
	PUSHDOUBLE(m_data->m_food_consumed_this_turn);
	PUSHLONG(m_data->m_total_pollution);

	uint8 flags = 
		(uint8)m_data->m_contribute_materials |
		((uint8)m_data->m_contribute_military << 1) |
		((uint8)m_data->m_buildCapitalization << 4) |
		((uint8)m_data->m_walls_nullified << 5) |
		((uint8)m_data->m_isInjoined << 6) |
		((uint8)m_data->m_buildInfrastructure << 7);
	PUSHBYTE(flags);

#if 0
	DPRINTF(k_DBG_NET, ("city: %d, %d, %d, %d, %d, %d, %d, %d, %d\n",
						m_data->m_spied_upon,
						m_data->m_franchise_owner,
						m_data->m_franchiseTurnsRemaining,
						m_data->m_ignore_happiness,
						m_data->m_watchfulTurns,
						m_data->m_bioInfectionTurns,
						m_data->m_nanoInfectionTurns,
						m_data->m_convertedTo,
						m_data->m_convertedBy));
#endif
						
	PUSHBYTE((sint8)m_data->m_spied_upon);

	PUSHBYTE((sint8)m_data->m_franchise_owner);
	PUSHBYTE((sint8)m_data->m_franchiseTurnsRemaining);
	
#ifdef _DEBUG
	
#endif
	PUSHBYTE((sint8)m_data->m_watchfulTurns);
	PUSHBYTE((sint8)m_data->m_bioInfectionTurns);
	PUSHBYTE((sint8)m_data->m_nanoInfectionTurns);
	PUSHBYTE((sint8)m_data->m_convertedTo);
	PUSHBYTE((sint8)m_data->m_convertedBy);
	PUSHLONG(m_data->m_convertedGold);

	PUSHLONG(m_data->m_accumulated_food);
	

	
	

	PUSHLONG(m_data->m_foodVatPollution);
	sint8 govSetting = -1;
	if(m_data->m_useGovernor) {
		govSetting = (sint8)m_data->m_buildListSequenceIndex;
	}
	PUSHBYTE(govSetting);

	PUSHBYTE(m_isInitialPacket);
}

void NetCity2::Unpacketize(uint16 id, uint8 *buf, uint16 size)
{
	sint32 pos = 0;
	uint16 packid;
	PULLID(packid);
	Assert(packid == k_PACKET_CITY2_ID);
	if(packid != k_PACKET_CITY2_ID)
		return;

	
	Unit home_city;
	PULLLONGTYPE(home_city, Unit);
	
	if(!g_theUnitPool->IsValid(home_city))
		return;
	m_data = home_city.AccessData()->GetCityData();

	if(!m_data) {
		if(g_network.IsClient()) {
			g_network.RequestResync(RESYNC_INVALID_UNIT);
			return;
		} else {
			g_network.Resync(g_network.IdToIndex(id));
			return;
		}
	}

	double oldgross = m_data->m_gross_food_this_turn, 
		oldproduced = m_data->m_food_produced_this_turn, 
		oldconsumed = m_data->m_food_consumed_this_turn;
	double oldLostToCrime = m_data->m_food_lost_to_crime;
	sint32 oldaccum = m_data->m_accumulated_food;

	PULLLONG(m_data->m_trade_lost_to_crime);
	PULLLONG(m_data->m_gross_trade);
	PULLLONG(m_data->m_goldFromTradeRoutes);
	PULLLONG(m_data->m_gross_production_this_turn);
	PULLLONG(m_data->m_shields_this_turn);
	PULLLONG(m_data->m_shields_lost_to_crime);
	PULLLONG64(m_data->m_builtWonders);
	PULLDOUBLE(m_data->m_food_delta);
	PULLDOUBLE(m_data->m_gross_food_this_turn);
	PULLDOUBLE(m_data->m_food_produced_this_turn);
	PULLDOUBLE(m_data->m_food_lost_to_crime);
	PULLDOUBLE(m_data->m_food_consumed_this_turn);
	PULLLONG(m_data->m_total_pollution);

	uint8 flags;
	PULLBYTE(flags);
	m_data->m_contribute_materials = flags & 0x01;
	m_data->m_contribute_military = (flags & 0x02) >> 1;
	m_data->m_buildCapitalization = (flags & 0x10) >> 4;
	m_data->m_walls_nullified = (flags & 0x20) >> 5;
	m_data->m_isInjoined = (flags & 0x40) >> 6;
	m_data->m_buildInfrastructure = (flags & 0x80) >> 7;

	PULLBYTETYPE(m_data->m_spied_upon, sint32);

	PULLBYTETYPE(m_data->m_franchise_owner, sint8);
	PULLBYTETYPE(m_data->m_franchiseTurnsRemaining, sint8);
	
#ifdef _DEBUG
	
#endif
	PULLBYTETYPE(m_data->m_watchfulTurns, sint8);
	PULLBYTETYPE(m_data->m_bioInfectionTurns, sint8);
	PULLBYTETYPE(m_data->m_nanoInfectionTurns, sint8);
	PULLBYTETYPE(m_data->m_convertedTo, sint8);
	PULLBYTETYPE(m_data->m_convertedBy, CONVERTED_BY);
	PULLLONG(m_data->m_convertedGold);

	PULLLONG(m_data->m_accumulated_food);


	PULLLONG(m_data->m_foodVatPollution);

	sint8 govSetting;	
	PULLBYTE(govSetting);
	if(govSetting == -1) {
		m_data->m_useGovernor = FALSE;
		m_data->m_buildListSequenceIndex = 0;
	} else {
		m_data->m_useGovernor = TRUE;
		m_data->m_buildListSequenceIndex = govSetting;
	}

	uint8 isInitialPacket;
	PULLBYTE(isInitialPacket);

#if 0
	DPRINTF(k_DBG_NET, ("city: %d, %d, %d, %d, %d, %d, %d, %d, %d\n",
						m_data->m_spied_upon,
						m_data->m_franchise_owner,
						m_data->m_franchiseTurnsRemaining,
						m_data->m_ignore_happiness,
						m_data->m_watchfulTurns,
						m_data->m_bioInfectionTurns,
						m_data->m_nanoInfectionTurns,
						m_data->m_convertedTo,
						m_data->m_convertedBy));
#endif

	if(m_data->m_owner == g_network.GetPlayerIndex() &&
		!isInitialPacket) {
		Assert(oldgross == m_data->m_gross_food_this_turn);
		Assert(oldLostToCrime == m_data->m_food_lost_to_crime);
		Assert(oldproduced == m_data->m_food_produced_this_turn);
		Assert(oldconsumed == m_data->m_food_consumed_this_turn);
		Assert(oldaccum == m_data->m_accumulated_food);

		if(oldgross != m_data->m_gross_food_this_turn ||
		   oldLostToCrime != m_data->m_food_lost_to_crime ||
		   oldproduced != m_data->m_food_produced_this_turn ||
		   oldconsumed != m_data->m_food_consumed_this_turn ||
		   oldaccum != m_data->m_accumulated_food) {
			g_network.RequestResync(RESYNC_CITY_STATS);
		}
	}

	Assert(pos == size);
}

void NetCityBuildQueue::Packetize(uint8 *buf, uint16 &size)
{
	size = 0;
	PUSHID(k_PACKET_CITY_BQ_ID);

	PUSHLONG((uint32)m_cityData->m_home_city);
	PUSHLONG(m_cityData->m_build_queue.m_list->GetCount());

	PointerList<BuildNode>::Walker walk(m_cityData->m_build_queue.m_list);
	while(walk.IsValid()) {
		BuildNode *bn = walk.GetObj();
		PUSHLONG(bn->m_cost);
		PUSHLONG(bn->m_type);
		PUSHLONG(bn->m_category);
		PUSHBYTE(bn->m_flags);

		walk.Next();
	}
}

void NetCityBuildQueue::Unpacketize(uint16 id, uint8 *buf, uint16 size)
{
	sint32 pos = 0;
	uint16 packid;
	PULLID(packid);
	Assert(packid == k_PACKET_CITY_BQ_ID);
	if(packid != k_PACKET_CITY_BQ_ID)
		return;

	
	Unit city;
	PULLLONGTYPE(city, Unit);
	Assert(g_theUnitPool->IsValid(city));
	if(!g_theUnitPool->IsValid(city))
		return;
	m_cityData = city.AccessData()->GetCityData();

	m_cityData->m_build_queue.Clear(TRUE);
	sint32 len;
	PULLLONG(len);

	BuildNode *bn = NULL;
	for(sint32 i = 0; i < len; i++) {
		bn = new BuildNode;
		PULLLONG(bn->m_cost);
		PULLLONG(bn->m_type);
		PULLLONG(bn->m_category);
		PULLBYTE(bn->m_flags);
		m_cityData->m_build_queue.m_list->AddTail(bn);
	}

	if(g_network.IsHost()) {
		g_network.Block(m_cityData->m_owner);
		
		g_network.Enqueue(m_cityData);
		g_network.Unblock(m_cityData->m_owner);
	}
}

void NetCityResources::Packetize(uint8 *buf, uint16 &size)
{
	size = 0;
	PUSHID(k_PACKET_RESOURCES_ID);
	PUSHLONG((uint32)m_cityData->m_home_city);
#ifdef CTP1_TRADE
	PUSHLONG(m_cityData->m_resources.m_numGoods);
	PUSHLONG(m_cityData->m_resources.m_totalResources);
	sint32 i;
	for(i = 0; i < m_cityData->m_resources.m_numGoods; i++) {
		PUSHLONG(m_cityData->m_resources.m_supply[i]);
	}
#endif
}

void NetCityResources::Unpacketize(uint16 id, uint8 *buf, uint16 size)
{
	uint16 pos = 0;
	uint16 packid;
	PULLID(packid);
	Assert(packid == k_PACKET_RESOURCES_ID);
	
	Unit city;
	PULLLONGTYPE(city, Unit);
	Assert(g_theUnitPool->IsValid(city));
	if(!g_theUnitPool->IsValid(city))
		return;

	m_cityData = city.AccessData()->GetCityData();

#ifdef CTP1_TRADE
	PULLLONG(m_cityData->m_resources.m_numGoods);
	PULLLONG(m_cityData->m_resources.m_totalResources);

	Assert(m_cityData->m_resources.m_numGoods == g_theResourceDB->NumRecords());
	sint32 i;
	for(i = 0; i < m_cityData->m_resources.m_numGoods; i++) {
		PULLLONG(m_cityData->m_resources.m_supply[i]);
	}
#endif
	Assert(pos == size);
}

