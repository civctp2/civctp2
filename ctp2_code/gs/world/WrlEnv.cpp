//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : part of the World class implementation
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
// _DEBUG
// Generate debug version when set.
//
//----------------------------------------------------------------------------
//
// Modifications from the original Activision code:
//
// - Prevented memory leak report (not an actual leak).
// - Added second World::GetGood method, usefull if you already have a Cell
//   pointer. - May 18th 2005 Martin Gühmann
// - Prevented crash with multiple instances of an improvement that is deleted.
//
//----------------------------------------------------------------------------

#include "c3.h"

#include "World.h"
#include "Cell.h"
#include "StrDB.h"
#include "TerrainRecord.h"
#include "installation.h"
#include "installationtree.h"
#include "CityRadius.h"
#ifdef _DEBUG
#include "pixelutils.h"
#include "colorset.h"           // g_colorSet
#endif
#include "TradeDynArr.h"
#include "RandGen.h"
#include "tiledmap.h"
#include "network.h"
#include "net_info.h"
#include "TerrImprove.h"
#include "terrainutil.h"
#include "ResourceRecord.h"
#include "MoveFlags.h"
#include "Unit.h"
#include "Globals.h"

extern RandomGenerator *g_rand;
extern TiledMap *g_tiledMap;

extern	StringDB *g_theStringDB ;

TERRAIN_TYPES World::GetTerrainType(const MapPoint &pos) const
{
	return GetCell(pos)->GetTerrainType();
}

#if 0

bool World::EnvIsWater(const uint32 env) const
{
	return (env & (k_MASK_ENV_MOVEMENT_TYPE & (k_BIT_MOVEMENT_TYPE_WATER | k_BIT_MOVEMENT_TYPE_SHALLOW_WATER)) != 0;
}

bool World::IsWater(const MapPoint &pos) const
{
	return EnvIsWater(GetCell(pos)->m_env);
}

bool World::IsWater(const sint32 x, const sint32 y) const
{
	return EnvIsWater(m_map[x][y]->m_env);
}
#endif

bool World::EnvIsShallowWater(const uint32 env) const
{
	return ((env & k_MASK_ENV_MOVEMENT_TYPE) & (k_BIT_MOVEMENT_TYPE_SHALLOW_WATER)) != 0;
}

bool World::IsShallowWater(const MapPoint &pos) const
{
	return EnvIsShallowWater(GetCell(pos)->m_env);
}

bool World::IsShallowWater(const sint32 x, const sint32 y) const
{
	return ((m_map[x][y]->m_env & k_MASK_ENV_MOVEMENT_TYPE) & (k_BIT_MOVEMENT_TYPE_SHALLOW_WATER)) != 0;
}

sint32 World::IsCurrent(const MapPoint &pos)
{
	if (IsWater(pos)) {
		return GetCell(pos)->m_env & k_MASK_ENV_RIV_CUR;
	} else {
		return FALSE;
	}
}

sint32 World::IsCurrent(const sint32 x, const sint32 y)
{
	if (IsWater(x, y)) {
		return m_map[x][y]->m_env & k_MASK_ENV_RIV_CUR;
	} else {
		return FALSE;
	}
}

bool World::EnvIsLand(const uint32 env) const
{
	return ((env & k_MASK_ENV_MOVEMENT_TYPE) & k_BIT_MOVEMENT_TYPE_LAND) != 0;
}

bool World::IsLand(const MapPoint &pos) const
{
	return EnvIsLand(GetCell(pos)->m_env);
}

bool World::IsLand(const sint32 x, const sint32 y) const
{
	Assert (m_isYwrap ? (-k_MAP_WRAPAROUND < y) : 0 <= y);
	Assert (m_isYwrap ? (y < (m_size.y + k_MAP_WRAPAROUND)) : y < m_size.y);

	return EnvIsLand(m_map[x][y]->m_env);
}

bool World::EnvIsSpace(const uint32 env) const
{
	return ((env & k_MASK_ENV_MOVEMENT_TYPE) & k_BIT_MOVEMENT_TYPE_SPACE) != 0;
}

bool World::IsSpace(const MapPoint &pos) const
{
	return EnvIsSpace(GetCell(pos)->m_env);
}

bool World::IsSpace(const sint32 x, const sint32 y) const
{
	Assert(0<= x);
	Assert(x<m_size.x);
	Assert (0 <= y);
	Assert(y < m_size.y);

	return EnvIsSpace(m_map[x][y]->m_env);
}

bool World::EnvIsRiver(const uint32 env) const
{
	return (env & k_MASK_ENV_RIV_CUR) != 0;
}

bool World::IsRiver(const sint32 x, const sint32 y) const
{
	return EnvIsRiver(m_map[x][y]->m_env);
}

bool World::IsRiver(const MapPoint &pos) const
{
	return EnvIsRiver(GetCell(pos)->m_env);
}

void World::SetCurrent(const MapPoint &pos)
{
	Assert(IsWater(pos));

	GetCell(pos)->m_env |= k_BIT_ENV_RIV_CUR;
}


void World::SetCurrent(const sint32 x, const sint32 y)
{
	Assert(IsWater(x, y));

	m_map[x][y]->m_env |= k_BIT_ENV_RIV_CUR;
}

void World::SetRiver(const MapPoint &pos)
{
	GetCell(pos)->m_env |= k_BIT_ENV_RIV_CUR;
	Cell *thisCell = m_map[pos.x][pos.y];
	thisCell->CalcTerrainMoveCost();
}

void World::UnsetRiver(const sint32 x, const sint32 y)
{
	m_map[x][y]->m_env &= ~k_BIT_ENV_RIV_CUR;
	Cell *thisCell = m_map[x][y];
	thisCell->CalcTerrainMoveCost();
}

bool World::IsGood(const MapPoint &pos) const
{
	return IsGood(pos.x, pos.y);
}

bool World::IsGood(const sint32 x, const sint32 y) const
{
	sint32 i;
	return GetCell(x, y)->GetGoodsIndex(i);

}

//----------------------------------------------------------------------------
//
// Name       : World::GetGood
//
// Description: Checks whether the given location has a good.
//
// Parameters : const MapPoint &pos: Map position which should be checked,
//                                   whether it has a good.
//              sint32 &good:        Filled with the good database index
//                                   of the good at the map position.
//
// Globals    : g_theTerrainDB: The terrain database.
//
// Returns    : Whether the given location has a good.
//
// Remark(s)  : -
//
//----------------------------------------------------------------------------
bool World::GetGood(const MapPoint &pos, sint32 &good) const
{

	sint32 i;
	Cell *c = GetCell(pos);

	if (c->GetGoodsIndex(i)) {
		good= g_theTerrainDB->Get(c->m_terrain_type)->GetResourcesIndex(i);
		return true;
	} else {
		return false;
	}
}

//----------------------------------------------------------------------------
//
// Name       : World::GetGood
//
// Description: Checks whether the given location has a good.
//
// Parameters : Cell *cell:   Pointer on the cell for which should be checked,
//                            whether it has a good.
//              sint32 &good: Filled with the good database index of the good
//                            in the cell.
//
// Globals    : g_theTerrainDB: The terrain database.
//
// Returns    : Whether the given location has a good.
//
// Remark(s)  : -
//
//----------------------------------------------------------------------------
bool World::GetGood(const Cell *c, sint32 &good) const
{
	sint32 i;

	if (c->GetGoodsIndex(i)) {
		good = g_theTerrainDB->Get(c->m_terrain_type)->GetResourcesIndex(i);
		return true;
	} else {
		return false;
	}
}

void World::ClearGoods(const sint32 x, const sint32 y)
{
	m_map[x][y]->m_env &= ~k_MASK_ENV_GOOD;
}

void World::SetGood(const sint32 x, const sint32 y, const sint32 g)

{
	Assert(0<= x);
	Assert(x<m_size.x);
	Assert (0<= y);
	Assert(y < m_size.y);

	if (g_theTerrainDB->Access(m_map[x][y]->m_terrain_type)->GetNumResources() <= g -1)
		return;

	switch(g) {
	case 0:   m_map[x][y]->m_env &= ~k_MASK_ENV_GOOD; break;
	case 1:   m_map[x][y]->m_env = (m_map[x][y]->m_env & ~k_MASK_ENV_GOOD) | k_BIT_ENV_GOOD1; break;
	case 2:   m_map[x][y]->m_env = (m_map[x][y]->m_env & ~k_MASK_ENV_GOOD) | k_BIT_ENV_GOOD2; break;
	case 3:   m_map[x][y]->m_env = (m_map[x][y]->m_env & ~k_MASK_ENV_GOOD) | k_BIT_ENV_GOOD3; break;
	case 4:   m_map[x][y]->m_env = (m_map[x][y]->m_env & ~k_MASK_ENV_GOOD) | k_BIT_ENV_GOOD4; break;
	default:
		Assert(0);
	}
#ifdef _DEBUG

#endif
}

void World::SetRandomGood(const sint32 x, const sint32 y)
{
	double totalProb = 0;
	double prob[k_MAX_GOODS_TYPES_PER_TERRAIN];

	Cell *cell = m_map[x][y];

	sint32 i;
	for(i = 0; i < k_MAX_GOODS_TYPES_PER_TERRAIN; i++)
	{
		if(g_theTerrainDB->Get(cell->m_terrain_type)->GetNumResources() > i)
		{
			prob[i] = g_theTerrainDB->Get(cell->m_terrain_type)->GetResources(i)->GetProbability();
			totalProb += prob[i];
		}
		else
		{
			prob[i] = 0;
		}
	}

#if defined(_DEBUG) || defined(USE_LOGGING)
	// Rounding error
	if(static_cast<sint32>(totalProb *100) > 100)
	{
		DPRINTF(k_DBG_FIX, ("The acummulated probability of goods on terrain %s is to with %f too high. The goods are:\n", g_theTerrainDB->Get(cell->m_terrain_type)->GetNameText(), totalProb));
		for(i = 0; i < k_MAX_GOODS_TYPES_PER_TERRAIN; i++)
		{
			if(g_theTerrainDB->Get(cell->m_terrain_type)->GetNumResources() > i)
			{
				DPRINTF(k_DBG_FIX, ("Good: %s Probability: %f\n", g_theTerrainDB->Get(cell->m_terrain_type)->GetResources(i)->GetNameText(), g_theTerrainDB->Get(cell->m_terrain_type)->GetResources(i)->GetProbability()));
			}
			else
			{
				break;
			}
		}
	}

	Assert(static_cast<sint32>(totalProb * 100) <= 100);
#endif

	totalProb = 0;
	sint32 val = g_rand->Next(1000);
	for(i = 0; i < k_MAX_GOODS_TYPES_PER_TERRAIN; i++)
	{
		if(val < (totalProb + prob[i]) * 1000)
		{
			SetGood(x,y,i+1);
			return;
		}
		totalProb += prob[i];
	}
}

GoodyHut *World::GetGoodyHut(const MapPoint & pos)
{
	Cell *c = GetCell(pos);

	return c->GetGoodyHut();
}

bool World::EnvIsHill(const uint32 env) const
{
	return (env & k_MASK_ENV_MOVEMENT_TYPE) == k_BIT_MOVEMENT_TYPE_MOUNTAIN;
}

bool World::IsHill(const sint32 x, const sint32 y) const
{
	return EnvIsHill(m_map[x][y]->m_env);
}

bool World::IsHill(const MapPoint &pos) const
{
	return EnvIsHill(GetCell(pos)->m_env);
}

bool World::EnvIsMountain(const uint32 env) const
{
	return ((env & k_MASK_ENV_MOVEMENT_TYPE) & k_BIT_MOVEMENT_TYPE_MOUNTAIN) != 0;
}

bool World::IsMountain(const sint32 x, const sint32 y) const
{
	return EnvIsMountain(m_map[x][y]->m_env);
}

bool World::IsMountain(const MapPoint &pos) const
{
	return EnvIsMountain(GetCell(pos)->m_env);
}

void World::SetMovementType (const sint32 x, const sint32 y, const sint32 t)
{
	Assert(0 <= y);
	Assert(y < m_size.y);

	m_map[x][y]->m_env = (m_map[x][y]->m_env & ~k_MASK_ENV_MOVEMENT_TYPE) | (t << k_SHIFT_ENV_MOVEMENT_TYPE);
}

uint32 World::GetMovementType(const MapPoint &pos)
{
	return GetCell(pos)->m_env & k_MASK_ENV_MOVEMENT_TYPE;
}

double World::GetMoveCost(const MapPoint &pos) const
{
	return GetCell(pos)->GetMoveCost();
}

sint32 World::GetEnvFlags(const MapPoint &pos) const
{
	return GetCell(pos)->m_env;
}

bool World::IsCity(const MapPoint &pos) const
{
	return GetCell(pos)->HasCity();
}

bool World::IsCanal(const MapPoint &pos) const
{
	if (IsLand(pos))
	{
		return (GetCell(pos)->m_env & k_MASK_ENV_CANAL_TUNNEL) != 0;
	}
	else
	{
		return false;
	}
}

bool World::EnvIsTunnel(const uint32 env) const
{
	return (env & k_MASK_ENV_CANAL_TUNNEL) != 0;
}

bool World::IsTunnel(const MapPoint &pos) const
{
	if (IsWater(pos))
	{
		return EnvIsTunnel(GetCell(pos)->m_env);
	}
	else
	{
		return false;
	}
}

bool World::EnvIsRoad(const sint32 i, const uint32 env) const
{
	return uint32(env & k_MASK_ENV_ROAD) == uint32((i+1) << k_SHIFT_ENV_ROAD);
}

bool World::IsRoad(const sint32 i, const MapPoint &pos) const
{
	return EnvIsRoad(i, GetCell(pos)->m_env);
}

bool World::EnvIsAnyRoad(const uint32 env) const
{
	return ((env & k_MASK_ENV_ROAD) != 0x0);
}

bool World::IsAnyRoad(const MapPoint &pos) const
{
	return ((GetCell(pos)->m_env & k_MASK_ENV_ROAD) != 0x0);
}

bool World::EnvIsIrrigation(const sint32 i, const uint32 env) const
{
	return uint32(env & k_MASK_ENV_IRRIGATION) == uint32((i+1) << k_SHIFT_ENV_IRRIGATION);
}

bool World::IsIrrigation(const sint32 i, const MapPoint &pos) const
{
	return EnvIsIrrigation(i, GetCell(pos)->m_env);
}

bool World::EnvIsMine(const sint32 i, const uint32 env) const
{
	return uint32(env & k_MASK_ENV_MINE) == uint32((i+1) << k_SHIFT_ENV_MINE);
}

bool World::IsMine(const sint32 i, const MapPoint &pos) const
{
	Assert(0 <= i);
	Assert(i < k_MAX_MINE_TYPES);
	return EnvIsMine(i, GetCell(pos)->m_env);

}

bool World::IsInstallation(const MapPoint &pos) const
{
	return (GetCell(pos)->m_env & k_BIT_ENV_INSTALLATION) != 0;
}

#ifdef _DEBUG
#ifdef CELL_COLOR

sint32 World::GetColor(const MapPoint &pos) const
{
	int c = GetCell(pos)->m_color;

	int foo;
	if (0 <c) {
		foo = 1;
	}
	return c;

}

void World::ShowCellOwners()
{
	for(sint32 x = 0; x < m_size.x; x++) {
		for(sint32 y = 0; y < m_size.y; y++) {
			Cell *cell = GetCell(x,y);
			if(cell->GetOwner() >= 0) {
				cell->m_color = g_colorSet->GetPlayerColor(cell->GetOwner());
			} else {
				cell->m_color = 0;
			}
		}
	}
	extern void WhackScreen();
	WhackScreen();
}
#endif
#endif

const char * World::GetTerrainName (const MapPoint &pos)
{
	StringId id = g_theTerrainDB->Get(GetCell(pos)->m_terrain_type)->m_name;
	return g_theStringDB->GetNameStr(id);
}

#if 0
TileInfo *World::GetTileInfo(const MapPoint &pos)
{
	Cell *cell = GetCell(pos);
	Assert(cell != NULL);
	if (cell == NULL) return NULL;

	return cell->GetTileInfo();
}
void World::SetTileInfo(const MapPoint &pos, TileInfo *tileInfo)
{
	Cell *cell = GetCell(pos);
	Assert(cell != NULL);
	if (cell == NULL) return;

	cell->SetTileInfo(tileInfo);
}
#endif

BOOL World::IsConnectedToCity(const MapPoint &pnt, sint32 owner,
							  uint8* array) const
{
	MapPoint neighbor;
	BOOL firstcall = FALSE;
	Cell *thisCell = m_map[pnt.x][pnt.y];

	if(thisCell->GetCity().m_id != (0)) {
		if(thisCell->GetCity().GetOwner() == owner)
			return TRUE;
		else
			return FALSE;
	}

	if(array == NULL) {
		array = new uint8[m_size.y * m_size.x];
		memset(array, 0, m_size.y * m_size.x);
		firstcall = TRUE;
	}

	array[pnt.y * m_size.x + pnt.x] = 1;

	sint32 d;
	for(d = (sint32)NORTH; d < (sint32)NOWHERE; d++) {
		if(pnt.GetNeighborPosition((WORLD_DIRECTION)d, neighbor)) {
			if(!array[neighbor.y * m_size.x + neighbor.x]) {
				Cell* neighborCell = m_map[neighbor.x][neighbor.y];
				if((neighborCell->GetOwner() == (sint8)owner) ||
				   (neighborCell->GetCity().m_id != (0))) {
					if(IsConnectedToCity(neighbor, owner, array)) {
						if(firstcall) {
							delete [] array;
						}
						return TRUE;
					}
				}
			}
		}
	}
	if(firstcall) {
		delete [] array;
	}
	return FALSE;
}

BOOL World::IsOnOrNextToOwner(const MapPoint &pos, sint32 owner)
{
	if(m_map[pos.x][pos.y]->GetOwner() == owner)
		return TRUE;
	sint32 dd;
	for(dd = 0; dd < (sint32)NOWHERE; dd++) {
		static MapPoint chk;
		if(pos.GetNeighborPosition((WORLD_DIRECTION)dd, chk)) {
			if(m_map[chk.x][chk.y]->GetOwner() == owner)
				return TRUE;
		}
	}
	return FALSE;
}

bool World::IsContinentSharedWithOthers(const MapPoint &pnt,
										sint32 owner,
										uint8* array) const
{
	MapPoint neighbor;
	BOOL firstcall = FALSE;
	MapPoint pos;
	Cell *thisCell = m_map[pnt.x][pnt.y];

	if(thisCell->GetOwner() >= 0 &&
		thisCell->GetOwner() != owner)
		return TRUE;

	if(array == NULL) {
		array = new uint8[m_size.y * m_size.x];
		memset(array, 0, m_size.y * m_size.x);
		firstcall = TRUE;
	}

	array[pnt.y * m_size.x + pnt.x] = 1;

	sint32 d;
	for(d = (sint32)NORTH; d < (sint32)NOWHERE; d++) {
		if(pnt.GetNeighborPosition((WORLD_DIRECTION)d, neighbor)) {
			if(!array[neighbor.y * m_size.x + neighbor.x]) {
				if(!IsWater(neighbor)) {
					if(IsContinentSharedWithOthers(neighbor, owner, array)) {
						if(firstcall) {
							delete [] array;
						}
						return TRUE;
					}
				}
			}
		}
	}
	if(firstcall)
		delete [] array;
	return FALSE;
}

BOOL World::IsContinentBiggerThan(uint32 size,
								  const MapPoint &pnt,
								  uint8* array,
								  uint32 *cursize) const
{
	MapPoint neighbor;
	BOOL firstcall = FALSE;
	MapPoint pos;

	if(array == NULL) {
		array = new uint8[m_size.y * m_size.x];
		memset(array, 0, m_size.y * m_size.x);
		cursize = new uint32;
		firstcall = TRUE;
	}

	array[pnt.y * m_size.x + pnt.x] = 1;
	if(!IsWater(pnt)) {
		*cursize = *cursize + 1;
		if(*cursize >= size) {
			if(firstcall) {
				delete [] array;
				delete cursize;
			}
			return TRUE;
		}
	}

	sint32 d;
	for(d = (sint32)NORTH; d < (sint32)NOWHERE; d++) {
		if(pnt.GetNeighborPosition((WORLD_DIRECTION)d, neighbor)) {
			if(!array[neighbor.y * m_size.x + neighbor.x]) {
				if(!IsWater(neighbor)) {
					if(IsContinentBiggerThan(size, neighbor, array, cursize) ||
					   *cursize >= size) {
						if(firstcall) {
							delete [] array;
							delete cursize;
						}
						return TRUE;
					}
				}
			}
		}
	}
	if(firstcall) {
		delete [] array;
		delete cursize;
	}
	return FALSE;
}

void World::ChangeOwner(const MapPoint &point, sint32 fromOwner, sint32 toOwner)
{
	Cell *thisCell = m_map[point.x][point.y];
	Cell *neighborCell;
	MapPoint neighbor;

	Assert(fromOwner != toOwner);
	if(fromOwner == toOwner)
		return;

	if(thisCell->GetOwner() == fromOwner)
	{
		thisCell->SetOwner(toOwner);

		if(g_network.IsHost())
		{
			uint32 packpos = g_network.PackedPos(point);
			g_network.Enqueue(new NetInfo(NET_INFO_CODE_CELL_OWNER,
										  packpos, toOwner));
		}

		DynamicArray<Installation> instArray;
		if(g_theInstallationTree->GetAt(point, instArray))
		{
			for(sint32 i = 0; i < instArray.Num(); i++)
			{
				instArray[i].ChangeOwner(toOwner);
			}
		}
		g_network.Enqueue(thisCell, point.x, point.y);

		for(sint32 d = (sint32)NORTH; d < (sint32)NOWHERE; d++)
		{
			if(point.GetNeighborPosition((WORLD_DIRECTION)d, neighbor))
			{
				neighborCell = GetCell(neighbor);
				if(neighborCell->GetOwner() == fromOwner)
				{
					if(fromOwner >= 0 ||
					   neighborCell->m_env & (k_MASK_ENV_ROAD | k_MASK_ENV_IRRIGATION |
											  k_MASK_ENV_MINE | k_MASK_ENV_INSTALLATION |
											  k_MASK_ENV_CANAL_TUNNEL)) {
						ChangeOwner(neighbor, fromOwner, toOwner);
					}
				}
			}
		}
	}
}

void World::CutImprovements(const MapPoint &point)
{
	MapPoint neighbor;
	Cell *thisCell = m_map[point.x][point.y];

	thisCell->SetEnv(thisCell->GetEnv() & ~(k_MASK_ENV_ROAD | k_MASK_ENV_IRRIGATION |
										    k_MASK_ENV_MINE | k_MASK_ENV_INSTALLATION |
											k_MASK_ENV_CANAL_TUNNEL));

	DynamicArray<Installation>	instArray;
	if(g_theInstallationTree->GetAt(point, instArray))
	{
		for(sint32 i = instArray.Num() - 1; i >= 0; i--)
		{
			instArray[i].Kill();
		}
	}

	// A for-loop looks better, but does not work, because Kill may modify
	// multiple indices.
	while (thisCell->GetNumImprovements() > 0)
	{
		thisCell->AccessImprovement(thisCell->GetNumImprovements()- 1).Kill();
	}

	while(thisCell->GetNumDBImprovements())
	{
		const TerrainImprovementRecord *rec = g_theTerrainImprovementDB->Get(thisCell->GetDBImprovement(0));
		sint32 intRad, sqRad;
		Assert(rec);
		if(rec && rec->GetIntBorderRadius(intRad))
		{
			rec->GetSquaredBorderRadius(sqRad);
			terrainutil_RemoveBorders(point, thisCell->GetOwner(), intRad, sqRad, Unit());
		}
		thisCell->RemoveDBImprovement(thisCell->GetDBImprovement(0));
	}

	thisCell->CalcMovementType();
	thisCell->CalcTerrainMoveCost();
	MapPoint pos = point;
	g_tiledMap->PostProcessTile(pos, GetTileInfo(point));
	g_tiledMap->RedrawTile(&pos);

	if(g_network.IsHost())
	{
		g_network.Enqueue(thisCell, point.x, point.y);
	}

#if 0
	if(thisCell->GetOwner() >= 0) {

		sint32 fromOwner = thisCell->GetOwner();
		thisCell->SetOwner(-1);
		uint32 packpos = g_network.PackedPos(point);
		g_network.Enqueue(new NetInfo(NET_INFO_CODE_CELL_OWNER,
									  packpos, -1));

		sint32 d;
		for(d = (sint32)NORTH; d < (sint32)NOWHERE; d++) {
			if(point.GetNeighborPosition((WORLD_DIRECTION)d, neighbor)) {
				if(!IsConnectedToCity(neighbor, fromOwner)) {
					ChangeOwner(neighbor, fromOwner, -1);
				}
			}
		}
	}
#endif
}

double World::GetDefenseBonus(const MapPoint &point) const
{
	double terrain_bonus, fort_bonus;
	terrainutil_GetDefenseBonus(point,terrain_bonus, fort_bonus);
	return fort_bonus;
}

bool World::IsAirfield(const MapPoint &point) const
{
	return terrainutil_HasAirfield(point);
}

bool World::IsListeningPost(const MapPoint &point) const
{
	return terrainutil_HasListeningPost(point);
}

bool World::IsFort(const MapPoint &point) const
{
	return terrainutil_HasFort(point);
}

//added by E
bool World::IsUrban(const MapPoint &point) const
{
	return terrainutil_HasUrban(point);
}
bool World::IsWonder(const MapPoint &point) const
{
	return terrainutil_HasWonder(point);
}

bool World::IsSafeFromNukes(const MapPoint &point) const
{
	Cell *theCell = m_map[point.x][point.y];
	Unit city = theCell->GetCity();
	if(city.m_id == (0))
		return false;

	return city.SafeFromNukes();
}

bool World::IsRadar(const MapPoint &point) const
{
	return terrainutil_HasRadar(point);
}

bool World::IsHealUnits(const MapPoint &point) const
{
	return terrainutil_HasFort(point);
}

bool World::CanEnter(const MapPoint &pos, const uint32 flag) const
{
	return GetCell(pos)->CanEnter(flag);
}

void World::SetCanalTunnel(MapPoint &pos, sint32 level)
{
}

sint32 World::GetTerrain(sint32 x, sint32 y)
{
	return m_map[x][y]->m_terrain_type;
}

void World::SetTerrain(sint32 x, sint32 y, sint32 type)
{
	m_map[x][y]->m_terrain_type = (sint8)type;
}

PLAYER_INDEX World::GetOwner(const MapPoint &pos) const
{
	return GetCell(pos)->GetOwner();
}

BOOL World::CanBeIrrigated(const MapPoint &point)
{
	m_tempIrrigation = FALSE;
	m_radiusOp = WORLD_RADIUS_OP_IRRIGATION_CHECK;
	AdjacentIterator(point, this);
	return m_tempIrrigation;
}

bool World::IsNextToCity(const MapPoint &pos) const
{
	MapPoint n;

	if(pos.GetNeighborPosition(NORTH, n)) {
		if (IsCity(n)) {
			return true;
		}
	}

	if(pos.GetNeighborPosition(NORTHWEST, n)) {
		if (IsCity(n)) {
			return true;
		}
	}

	if(pos.GetNeighborPosition(NORTHEAST, n)) {
		if (IsCity(n)) {
			return true;
		}
	}

	if(pos.GetNeighborPosition(SOUTH, n)) {
		if (IsCity(n)) {
			return true;
		}
	}

	if(pos.GetNeighborPosition(SOUTHWEST, n)) {
		if (IsCity(n)) {
			return true;
		}
	}

	if(pos.GetNeighborPosition(SOUTHEAST, n)) {
		if (IsCity(n)) {
			return true;
		}
	}

	if(pos.GetNeighborPosition(WEST, n)) {
		if (IsCity(n)) {
			return true;
		}
	}

	if(pos.GetNeighborPosition(EAST, n)) {
		if (IsCity(n)) {
			return true;
		}
	}

	return false;
}

void World::RegisterPlayerDead(sint32 owner)
{
	sint32 x, y;
	for(x = 0; x < m_size.x; x++) {
		for(y = 0; y < m_size.y; y++) {
			if(m_map[x][y]->GetOwner() == owner)
				m_map[x][y]->SetOwner(-1);
		}
	}
}

BOOL World::IsInsideCityRadiusOfPlayerOtherThan(const MapPoint &pos, sint32 owner, const Unit &ignoreThisCity)
{
	m_radiusOp = WORLD_RADIUS_OP_CITY_SEARCH;
	m_ignoreCity = ignoreThisCity.m_id;
	m_isInsideRadius = FALSE;
	m_cityOwnerCheck = owner;
	CityRadiusIterator(pos, this);
	return m_isInsideRadius;
}

const TerrainRecord *World::GetTerrain(const MapPoint &pos)
{
	Cell *cell = GetCell(pos);
	return g_theTerrainDB->Get(cell->m_terrain_type);
}

double World::GetGoodValue(sint32 good)
{
	Assert(good >= 0);
	Assert(good < g_theResourceDB->NumRecords());
	if(good < 0 || good >= g_theResourceDB->NumRecords())
		return 0;

	Assert(m_goodValue);
	if(!m_goodValue) return 0;

	return m_goodValue[good];
}
