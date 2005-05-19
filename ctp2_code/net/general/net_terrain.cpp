








#include "c3.h"

#include "network.h"
#include "net_terrain.h"
#include "net_util.h"

#include "TerrImprove.h"
#include "TerrImprovePool.h"
#include "XY_Coordinates.h"
#include "World.h"
#include "player.h"
#include "tiledmap.h"

extern World *g_theWorld;
extern Player **g_player;
extern TiledMap *g_tiledMap;

NetTerrainImprovement::NetTerrainImprovement(TerrainImprovementData *data)
{
	m_data = data;
}

void NetTerrainImprovement::Packetize(uint8 *buf, uint16 &size)
{
	buf[0] = k_PACKET_TERRAIN_ID >> 8;
	buf[1] = k_PACKET_TERRAIN_ID & 0xff;

	size = 2;
	PUSHLONG(m_data->m_id);
	PUSHBYTE((uint8)m_data->m_owner);
	PUSHBYTE((uint8)m_data->m_type);
	PUSHSHORT((uint16)m_data->m_point.x);
	PUSHSHORT((uint16)m_data->m_point.y);
	PUSHSHORT((uint16)m_data->m_turnsToComplete);
	PUSHBYTE((unsigned char) m_data->m_transformType);
	PUSHLONG(m_data->m_materialCost);
}

void NetTerrainImprovement::Unpacketize(uint16 id, uint8 *buf, uint16 size)
{
	Assert(MAKE_CIV3_ID(buf[0], buf[1]) == k_PACKET_TERRAIN_ID);
	sint32 pos = 2;
	MapPoint oldpoint;
	PLAYER_INDEX oldOwner;

	TerrainImprovement imp;
	PULLLONGTYPE(imp, TerrainImprovement);

	g_network.CheckReceivedObject((uint32)imp);

	if(!g_theTerrainImprovementPool->IsValid(imp)) {
		m_data = new TerrainImprovementData(imp);
	} else {
		m_data = g_theTerrainImprovementPool->AccessTerrainImprovement(imp);
		oldpoint = m_data->m_point;
		oldOwner = m_data->m_owner;
	}
	
	pos = 6;
	PULLBYTE(m_data->m_owner);
	PULLBYTETYPE(m_data->m_type, TERRAIN_IMPROVEMENT);
	PULLSHORT(m_data->m_point.x);
	PULLSHORT(m_data->m_point.y);
	PULLSHORT(m_data->m_turnsToComplete);
	PULLBYTETYPE(m_data->m_transformType,TERRAIN_TYPES);
	PULLLONG(m_data->m_materialCost);

	if(!g_theTerrainImprovementPool->IsValid(imp)) {
		g_theTerrainImprovementPool->HackSetKey(((uint32)imp & k_ID_KEY_MASK) + 1);
		g_theTerrainImprovementPool->Insert(m_data);
		g_theWorld->InsertImprovement(imp, m_data->m_point);
		g_tiledMap->RedrawTile(&m_data->m_point);
		g_player[m_data->m_owner]->AddImprovement(imp);
	} else {
		if(oldpoint != m_data->m_point) {
			
			
			g_theWorld->RemoveImprovement(imp, oldpoint);
			g_theWorld->InsertImprovement(imp, m_data->m_point);
			
			g_tiledMap->RedrawTile(&oldpoint);
			g_tiledMap->RedrawTile(&m_data->m_point);
		}

		if(oldOwner != m_data->m_owner) {
			

			g_player[oldOwner]->RemoveImprovementReferences(imp);
			g_player[m_data->m_owner]->AddImprovement(imp);
		}
	}
}
