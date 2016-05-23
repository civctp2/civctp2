//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
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
// USE_WORLDSIZE_CLASS
// - Implementation with separate WorldSize class (decoupled from MapPoint)
//
//----------------------------------------------------------------------------
//
// Modifications from the original Activision code:
//
// - Corrected possible crashes and memory leaks
//
//----------------------------------------------------------------------------
//
/// \file   net_cell.cpp
/// \brief  Multiplayer cell packet handling (definitions)

#include "c3.h"
#include "net_cell.h"

#include "Cell.h"
#include "cellunitlist.h"
#include "Globals.h"        // k_GAME_OBJ_TERRAIN_IMPROVEMENT
#include "GoodActor.h"
#include "GoodyHuts.h"
#include "net_util.h"
#include "network.h"
#include "ObjPool.h"
#include "TerrImprove.h"
#include "TerrImprovePool.h"
#include "tiledmap.h"
#include "TileInfo.h"
#include "TradeDynArr.h"
#include "World.h"

#define SEND_MOVE_COST

void NetCellData::Packetize(uint8 * buf, uint16 & size)
{
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

	int const objectCount = (m_cell->m_objects) ? m_cell->m_objects->Num() : 0;
	PUSHBYTE(objectCount);
	for (int i = 0; i < objectCount; ++i)
	{
		PUSHLONG(m_cell->m_objects->Get(i).m_id);
	}
}

void NetCellData::Unpacketize(uint16 id, uint8* buf, uint16 size)
{
	Assert(buf[0] == 'G' && buf[1] == 'C');
	uint16 pos = 2;

	PULLSHORT(m_x);
	PULLSHORT(m_y);

	MapPoint mp(m_x, m_y);

	DPRINTF(k_DBG_NET, ("Handling cell at %d/%d\n", m_x, m_y));
	m_cell = g_theWorld->AccessCell(mp);
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

	uint8 numObjects;
	PULLBYTE(numObjects);
	delete m_cell->m_objects;
	m_cell->m_objects = (numObjects > 0) ? new DynamicArray<ID> : NULL;

	for (uint8 i = 0; i < numObjects; ++i)
	{
		uint32 id;
		PULLLONG(id);
		if (    ((id & k_ID_TYPE_MASK) == k_BIT_GAME_OBJ_TYPE_TERRAIN_IMPROVEMENT)
		     && g_theTerrainImprovementPool->IsValid(id)
		   )
		{
			m_cell->m_objects->Insert(id);
		}
	}

	if (g_tiledMap)
	{
		g_tiledMap->PostProcessTile(mp, g_theWorld->GetTileInfo(mp));
		g_tiledMap->TileChanged(mp);

		MapPoint npos;
		for (int d = 0; d < NOWHERE; ++d)
		{
			if (mp.GetNeighborPosition(static_cast<WORLD_DIRECTION>(d), npos))
			{
				g_tiledMap->PostProcessTile
					(npos, g_theWorld->GetTileInfo(npos));
				g_tiledMap->TileChanged(npos);
				g_tiledMap->RedrawTile(&npos);
			}
		}

		g_tiledMap->RedrawTile(&mp);
	}
}

void NetCellList::Packetize(uint8* buf, uint16& size)
{
	size = 0;

	PUSHID(k_PACKET_CELL_LIST_ID);

	MapPoint pos(m_x, m_y);
	PUSHLONG(g_network.PackedPos(pos));
	PUSHBYTE(m_cells);

	size_t cells = 0;

#if defined(USE_WORLDSIZE_CLASS)
	WorldSize mapsize = g_theWorld->GetSize();
#else
	MapPoint  mapsize = *g_theWorld->GetSize();
#endif
	for (sint32 x = m_x; x < mapsize.x; x++)
	{
		for (sint32 y = m_y; y < mapsize.y; y++)
		{
			MapPoint mp(x,y);
			Cell const * cell = g_theWorld->GetCell(mp);
			PUSHLONG(cell->m_env);

			uint8 terrainPlusFlags = cell->m_terrain_type;
			if (cell->GetCity().m_id != 0)
			{
				terrainPlusFlags |= 0x80;
			}
			if (cell->m_jabba)
			{
				terrainPlusFlags |= 0x40;
			}
			PUSHBYTE(terrainPlusFlags);

			PUSHBYTE(cell->m_cellOwner);
			PUSHLONG(cell->m_city.m_id);

			if (cell->m_jabba)
			{
				PUSHBYTE((uint8)cell->m_jabba->m_typeValue);
				Assert(cell->m_jabba->m_value >= 0 && cell->m_jabba->m_value < k_VALUE_RANGE);
				PUSHSHORT((uint16)cell->m_jabba->m_value)
			}

#ifdef SEND_MOVE_COST
			PUSHSHORT(cell->m_move_cost);
#endif

			int const objectCount = (cell->m_objects) ? cell->m_objects->Num() : 0;
			PUSHBYTE(objectCount);
			for (int i = 0; i < objectCount; ++i)
			{
				PUSHLONG(cell->m_objects->Get(i).m_id);
			}

			++cells;
			if (cells >= m_cells)
			{
				return;
			}
		}
	}
}

void NetCellList::Unpacketize(uint16 id, uint8* buf, uint16 len)
{
	Assert(buf[0] = 'G' && buf[1] == 'L');

	uint16 pos = 2;
	uint32 packedPos;
	PULLLONG(packedPos);
	MapPoint mapPos;
	g_network.UnpackedPos(packedPos, mapPos);
	m_x = mapPos.x;
	m_y = mapPos.y;
	PULLBYTE(m_cells);

	size_t cells = 0;

#if defined(USE_WORLDSIZE_CLASS)
	WorldSize mapsize = g_theWorld->GetSize();
#else
	MapPoint  mapsize = *g_theWorld->GetSize();
#endif
	for (sint32 x = m_x; x < mapsize.x; x++)
	{
		for (sint32 y = m_y; y < mapsize.y; y++)
		{
			MapPoint mp(x,y);
			Cell* cell = g_theWorld->AccessCell(mp);

			delete cell->m_unit_army;
			cell->m_unit_army = NULL;

			cell->SetCity(Unit());

			delete cell->m_objects;
			cell->m_objects = NULL;

			PULLLONG(cell->m_env);

			uint8 terrainPlusFlags;
			PULLBYTE(terrainPlusFlags);
			cell->m_terrain_type = terrainPlusFlags & 0x3f;

			uint8 owner;
			PULLBYTE(owner);
			cell->SetOwner(owner);

			PULLLONG(cell->m_city.m_id);

			delete cell->m_jabba;
			if (terrainPlusFlags & 0x40)
			{
				uint8 tmp;
				PULLBYTE(tmp);
				uint16 value;
				PULLSHORT(value);
				cell->m_jabba = new GoodyHut(tmp & 0x7f, (uint32) value);
				Assert(cell->m_jabba->m_value >= 0 && cell->m_jabba->m_value < k_VALUE_RANGE);
			}
			else
			{
				cell->m_jabba = NULL;
			}

#ifdef SEND_MOVE_COST
			uint16 actualCost;
			PULLSHORT(actualCost);
#endif

			uint8 numObjects;
			PULLBYTE(numObjects);
			delete cell->m_objects;
			cell->m_objects = (numObjects > 0) ? new DynamicArray<ID> : NULL;

			for (uint8 i = 0; i < numObjects; i++)
			{
				uint32 id;
				PULLLONG(id);
				cell->m_objects->Insert(id);
			}

			cell->CalcTerrainMoveCost();

			if (g_theWorld->GetTileInfo(mp))
			{
				delete g_theWorld->GetTileInfo(mp)->m_goodActor;
				g_theWorld->GetTileInfo(mp)->m_goodActor = NULL;
			}

			cells++;
			if (cells >= m_cells)
			{
				Assert(pos == len);
				return;
			}
		}
	}
}

void NetCellUnitOrder::Packetize(uint8 *buf, uint16 &size)
{
	size = 0;
	PUSHID(k_PACKET_CELL_UNIT_ORDER_ID);
	PUSHSHORT(m_x);
	PUSHSHORT(m_y);
	CellUnitList *units = g_theWorld->GetCell(m_x, m_y)->UnitArmy();

	Assert(units && (units->Num() > 1));
	if (units && (units->Num() > 1))
	{
		uint8 n = (uint8)units->Num();
		PUSHBYTE(n);
		for (uint8 i = 0; i < n; i++)
		{
			PUSHLONG((uint32)units->Access(i));
		}
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
	Assert(units && (units->Num() > 1));
	if (units && (units->Num() > 1))
	{
		uint8 n;
		PULLBYTE(n);
		Assert(n == (uint8)units->Num());
		for (uint8 i = 0; i < n; i++)
		{
			Unit u;
			PULLLONGTYPE(u, Unit);
			(*units)[i] = u;
		}
	}
}
