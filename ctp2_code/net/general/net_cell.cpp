#include "c3.h"
#include "Cell.h"

#include "Globals.h"        // k_GAME_OBJ_TERRAIN_IMPROVEMENT
#include "net_cell.h"
#include "net_util.h"
#include "network.h"
#include "TerrImprove.h"
#include "GoodyHuts.h"
#include "TileInfo.h"
#include "GoodActor.h"
#include "tiledmap.h"
#include "cellunitlist.h"
#include "TradeDynArr.h"
#include "ObjPool.h"
#include "TerrImprovePool.h"
#include "World.h"

#define SEND_MOVE_COST


void
NetCellData::Packetize(uint8* buf, uint16& size)
{
	size = 0;
	buf[0] = 'G';
	buf[1] = 'C';
	size = 2;
	PUSHSHORT(m_x);
	PUSHSHORT(m_y);

	PUSHLONG(m_cell->m_env);
	PUSHBYTE(m_cell->m_terrain_type);
	PUSHSHORT(m_cell->m_move_cost);
	PUSHSHORT(m_cell->m_continent_number);
	PUSHBYTE(m_cell->m_cellOwner);
	PUSHLONG(m_cell->m_city.m_id);

	sint32 i;
	PUSHBYTE((uint8)m_cell->m_objects->Num());
	for(i = 0; i < m_cell->m_objects->Num(); i++) {
		PUSHLONG(m_cell->m_objects->Access(i).m_id);
	}
}


void
NetCellData::Unpacketize(uint16 id, uint8* buf, uint16 size)
{
	uint16 pos = 0;

	Assert(buf[0] == 'G' && buf[1] == 'C');
	pos = 2;

	PULLSHORT(m_x);
	PULLSHORT(m_y);

	MapPoint mp(m_x, m_y);

	DPRINTF(k_DBG_NET, ("Handling cell at %d/%d\n", m_x, m_y));
	m_cell = g_theWorld->GetCell(mp);
	PULLLONG(m_cell->m_env);
	PULLBYTE(m_cell->m_terrain_type);
	sint16 old_move_cost = m_cell->m_move_cost;
	PULLSHORT(m_cell->m_move_cost);
	
	if (old_move_cost != m_cell->m_move_cost)
		g_theWorld->SetCapitolDistanceDirtyFlags(0xffffffff);
	PULLSHORT(m_cell->m_continent_number);
	
	
	uint8 owner;
	PULLBYTE(owner);
	m_cell->SetOwner(owner);
	PULLLONG(m_cell->m_city.m_id);

	delete m_cell->m_objects;
	m_cell->m_objects = NULL;

	uint8 numObjects;
	PULLBYTE(numObjects);
	if(numObjects > 0) {
		m_cell->m_objects = new DynamicArray<ID>;
		uint8 i;
		for(i = 0; i < numObjects; i++) {
			uint32 id;
			PULLLONG(id);
			if((id & k_ID_TYPE_MASK) == k_BIT_GAME_OBJ_TYPE_TERRAIN_IMPROVEMENT) {
				if(!g_theTerrainImprovementPool->IsValid(id)) {
					continue;
				}
			}

			m_cell->m_objects->Insert(id);
		}
	}

	if(g_tiledMap) {
		g_tiledMap->PostProcessTile(mp, g_theWorld->GetTileInfo(mp));
		g_tiledMap->TileChanged(mp);

		MapPoint npos;
		for(WORLD_DIRECTION d = NORTH; d < NOWHERE; 
			d = (WORLD_DIRECTION)((sint32)d + 1)) {
			if(mp.GetNeighborPosition(d, npos)) {
				g_tiledMap->PostProcessTile(
					npos, 
					g_theWorld->GetTileInfo(npos));
				g_tiledMap->TileChanged(npos);
				g_tiledMap->RedrawTile(&npos);
			}
		}
		
		g_tiledMap->RedrawTile(&mp);
	}
}


void NetCellList::Packetize(uint8* buf, uint16& size)
{
	MapPoint* mapsize = g_theWorld->GetSize();
	
	uint8 cells = 0;
	size = 0;

	PUSHID(k_PACKET_CELL_LIST_ID);
	
	MapPoint pos(m_x, m_y);
	PUSHLONG(g_network.PackedPos(pos));
	PUSHBYTE(m_cells);

	

	int sx = m_x;
	int sy = m_y;
	

	for(sint32 x = sx; x < mapsize->x; x++) {
		
		
		sx = 0;
		for(sint32 y = sy; y < mapsize->y; y++) {
			sy = 0;
			MapPoint mp(x,y);
			Cell* cell = g_theWorld->GetCell(mp);
			PUSHLONG(cell->m_env);
			
			uint8 terrainPlusFlags = cell->m_terrain_type;
			if(cell->GetCity().m_id != 0) {
				terrainPlusFlags |= 0x80;
			}
			
			
			if(cell->m_jabba) {
				terrainPlusFlags |= 0x40;
			}
			PUSHBYTE(terrainPlusFlags);
			PUSHBYTE(cell->m_cellOwner);
			
			PUSHLONG(cell->m_city.m_id);
			
			if(cell->m_jabba) {
				PUSHBYTE((uint8)cell->m_jabba->m_typeValue);
				PUSHSHORT((uint16)cell->m_jabba->m_value)
				Assert(cell->m_jabba->m_value >= 0 && cell->m_jabba->m_value < k_VALUE_RANGE);
			}
			
#ifdef SEND_MOVE_COST
			
			
			
			
			PUSHSHORT(cell->m_move_cost);
#endif
			
			sint32 i;
			PUSHBYTE((uint8)cell->m_objects->Num());
			for(i = 0; i < cell->m_objects->Num(); i++) {
				PUSHLONG(cell->m_objects->Access(i).m_id);
			}

			cells++;
			if(cells >= m_cells) {
				
				return;
			}
		}
	}
}


void NetCellList::Unpacketize(uint16 id, uint8* buf, uint16 len)
{
	MapPoint* mapsize = g_theWorld->GetSize();
	uint16 cells = 0;

	Assert(buf[0] = 'G' && buf[1] == 'L');

	uint16 pos = 2;
	uint32 packedPos;
	PULLLONG(packedPos);
	MapPoint mapPos;
	g_network.UnpackedPos(packedPos, mapPos);
	m_x = mapPos.x;
	m_y = mapPos.y;
	PULLBYTE(m_cells);

	int sx = m_x;
	int sy = m_y;
	
	for(sint32 x = sx; x < mapsize->x; x++) {
		
		
		sx = 0;
		for(sint32 y = sy; y < mapsize->y; y++) {
			sy = 0;
			MapPoint mp(x,y);
			Cell* cell = g_theWorld->GetCell(mp);
			
			if(cell->m_unit_army) {
				delete cell->m_unit_army;
				cell->m_unit_army = NULL;
			}
			cell->SetCity(Unit());
			
			if(cell->m_objects) {
				delete cell->m_objects;
				cell->m_objects = NULL;
			}
			
			
			
			
			
			
			
			
			
			
			
			
			PULLLONG(cell->m_env);
			
			uint8 terrainPlusFlags;
			PULLBYTE(terrainPlusFlags);
			
			
			cell->m_terrain_type = terrainPlusFlags & 0x3f;
			
			
			uint8 owner;
			PULLBYTE(owner);
			cell->SetOwner(owner);

			PULLLONG(cell->m_city.m_id);
			
			
			if(cell->m_jabba) {
				delete cell->m_jabba;
				cell->m_jabba = NULL;
			}
			uint8 tmp;
			uint16 value;
			if(terrainPlusFlags & 0x40) {
				PULLBYTE(tmp);
				PULLSHORT(value);
				cell->m_jabba = new GoodyHut(tmp & 0x7f,
											 (uint32)value);
				Assert(cell->m_jabba->m_value >= 0 && cell->m_jabba->m_value < k_VALUE_RANGE);
			}
				
			
			
			

			

#ifdef SEND_MOVE_COST
			uint16 actualCost;
			PULLSHORT(actualCost);
#endif

			uint8 numObjects;
			PULLBYTE(numObjects);
			if(numObjects > 0) {
				cell->m_objects = new DynamicArray<ID>;
				uint8 i;
				for(i = 0; i < numObjects; i++) {
					uint32 id;
					PULLLONG(id);
					cell->m_objects->Insert(id);
				}
			}
			cell->CalcTerrainMoveCost();
#ifdef SEND_MOVE_COST
			
			
			
#endif

			
			

			if(g_theWorld->GetTileInfo(mp) && g_theWorld->GetTileInfo(mp)->m_goodActor) {
				delete g_theWorld->GetTileInfo(mp)->m_goodActor;
				g_theWorld->GetTileInfo(mp)->m_goodActor = NULL;
			}
			cells++;
			if(cells >= m_cells) {
				Assert(pos == len);
				return;
			}
		}
	}
}

NetCellUnitOrder::NetCellUnitOrder(sint32 x, sint32 y)
{
	m_x = (uint16)x;
	m_y = (uint16)y;
}

void NetCellUnitOrder::Packetize(uint8 *buf, uint16 &size)
{
	size = 0;
	PUSHID(k_PACKET_CELL_UNIT_ORDER_ID);
	PUSHSHORT(m_x);
	PUSHSHORT(m_y);
	CellUnitList *units = g_theWorld->GetCell(m_x, m_y)->UnitArmy();

	Assert(units);
	if(!units)
		return;

	Assert(units->Num() > 1);
	if(units->Num() <= 1)
		return;

	uint8 i, n = (uint8)units->Num();
	PUSHBYTE(n);
	for(i = 0; i < n; i++) {
		PUSHLONG((uint32)units->Access(i));
	}
}

void NetCellUnitOrder::Unpacketize(uint16 id, uint8 *buf, uint16 size)
{
	uint16 pos = 0;
	uint16 packid;
	PULLID(packid);
	Assert(packid == k_PACKET_CELL_UNIT_ORDER_ID);
	
	PULLSHORT(m_x);
	PULLSHORT(m_y);
	CellUnitList *units = g_theWorld->GetCell(m_x, m_y)->UnitArmy();
	Assert(units);
	if(!units)
		return;

	Assert(units->Num() > 1);
	if(units->Num() <= 1)
		return;

	uint8 i, n;
	PULLBYTE(n);
	Assert(n == (uint8)units->Num());
	for(i = 0; i < n; i++) {
		Unit u;
		PULLLONGTYPE(u, Unit);
		units->m_array[i] = u;
	}
}
