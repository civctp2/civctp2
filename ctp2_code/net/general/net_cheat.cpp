

#include "c3.h"
#ifdef _PLAYTEST
#include "network.h"
#include "net_util.h"
#include "net_cheat.h"
#include "Player.h"
#include "Unit.h"
#include "UnitDynArr.h"
#include "MapPoint.h"
#include "AICause.h"
#include "MaterialPool.h"
#include "XY_Coordinates.h"
#include "World.h"
#include "Gold.h"
#include "Advances.h"
#include "AdvanceRecord.h"
#include "UnitData.h"
#include "CityData.h"

extern Player **g_player;
extern World *g_theWorld;

const uint32 NetCheat::m_args[NET_CHEAT_MAX] = {
	5, 
	2, 
	1, 
	1, 
	2, 
	1, 
	0, 
	2, 
};

NetCheat::NetCheat(NET_CHEAT cheat, ...) :
	m_cheat(cheat)
{
	va_list vl;
	uint32 i;

	DPRINTF(k_DBG_NET, ("NetCheat: cheat=%d\n", m_cheat));
	if(m_args[m_cheat] > 0) {
		m_data = new uint32[m_args[m_cheat]];
		va_start( vl, cheat );
		for(i = 0; i < m_args[m_cheat]; i++) {
			m_data[i] = va_arg( vl, uint32 );
			DPRINTF(k_DBG_NET, ("NetCheat: arg %d: %d\n", i, m_data[i]));
		}
		va_end(vl);
	} else {
		m_data = NULL;
	}
}
	
NetCheat::~NetCheat()
{
	if(m_data)
		delete [] m_data;
}

void
NetCheat::Packetize(uint8 *buf, uint16 &size)
{
	size = 0;
	PUSHID(k_PACKET_CHEAT_ID);
	PUSHBYTE(m_cheat);
	for(uint32 i = 0; i < m_args[m_cheat]; i++) {
		PUSHLONG(m_data[i]);
	}
}

void
NetCheat::Unpacketize(uint16 id, uint8 *buf, uint16 size)
{
	uint16 packid;
	sint32 pos = 0;
	PULLID(packid);
	Assert(packid == k_PACKET_CHEAT_ID);
	if(packid != k_PACKET_CHEAT_ID)
		return;

	PULLBYTETYPE(m_cheat, NET_CHEAT);
	if(m_args[m_cheat] > 0) 
		m_data = new uint32[m_args[m_cheat]];
	for(uint32 i = 0; i < m_args[m_cheat]; i++) {
		PULLLONG(m_data[i]);
	}
	Assert(size == pos);
	sint32 index = g_network.IdToIndex(id);

	switch(m_cheat) {
		case NET_CHEAT_CREATE_UNIT:
		{
			DPRINTF(k_DBG_NET, ("Cheat create unit: pl: %d, city: %d, unit: %d, pos: %d,%d\n",
								m_data[2], m_data[0], m_data[1],
								m_data[3], m_data[4]));
			UnitDynamicArray* unitList = g_player[m_data[2]]->GetAllCitiesList();
			Unit city;
			if(unitList->Num() > 0) {
				city = unitList->Get(m_data[0]);
			} else {
				city.m_id = (0);
			}
			MapPoint pos(m_data[3], m_data[4]);
			Unit newu = g_player[m_data[2]]->CreateUnit(m_data[1], pos, city, 
													 FALSE, 
													 CAUSE_NEW_ARMY_INITIAL);
			break;
		}
		case NET_CHEAT_ADD_MATERIALS:
			DPRINTF(k_DBG_NET, ("Player %d, the cheater, added %d materials.\n", m_data[0], m_data[1]));
			g_player[m_data[0]]->m_materialPool->AddMaterials(m_data[1]);
			break;
		case NET_CHEAT_GLOBAL_WARMING:
			DPRINTF(k_DBG_NET, ("Player %d has caused global warming!\n", index));
			g_theWorld->GlobalWarming(m_data[0]);
			break;
		case NET_CHEAT_OZONE_DEPLETION:
			DPRINTF(k_DBG_NET, ("Player %d is a major source of ozone depletion.\n", index));
			g_theWorld->OzoneDepletion();
			break;
		case NET_CHEAT_ADD_GOLD:
		{
			DPRINTF(k_DBG_NET, ("Player %d, the big fat lousy CHEATER, added %d gold\n", m_data[0], m_data[1]));
			Gold amount;
			amount.SetLevel(m_data[1]);
			g_player[m_data[0]]->BequeathGold(amount);
			break;
		}
		case NET_CHEAT_GRANT_ADVANCE:
		{
			DPRINTF(k_DBG_NET, ("Player %d cheated to get advance #%d\n",
								index, m_data[0]));
			g_player[index]->m_advances->GiveAdvance(m_data[0], CAUSE_SCI_UNKNOWN);
			break;
		}

		case NET_CHEAT_GRANT_ALL:
		{
			DPRINTF(k_DBG_NET, ("Player %d is cheating up the wazoo by granting himself every advance\n", index));
			sint32 i;
			for(i = 0; i < g_theAdvanceDB->NumRecords(); i++) {
				g_player[index]->m_advances->GiveAdvance(i, CAUSE_SCI_UNKNOWN);
			}
			break;
		}
		case NET_CHEAT_CREATE_IMPROVEMENT:
		{
			DPRINTF(k_DBG_NET, ("The Winnie Lee Commemorative Cheat Has Been Activated"));
			Unit city(m_data[0]);
			uint64 built_improvements = city.AccessData()->GetCityData()->GetImprovements();
			city.AccessData()->GetCityData()->SetImprovements(built_improvements | ((uint64)1 << m_data[1]));
			g_network.Enqueue(city.AccessData(), city.AccessData()->GetCityData());
			break;
		}
		default:
			break;
	}
}

#endif 
