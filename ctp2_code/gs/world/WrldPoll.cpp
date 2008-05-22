//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : World pollution handling
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
// - Initialized local variables. (Sep 9th 2005 Martin Gühmann)
// - Replaced old GlobalWarming database by new one. (July 9th 2005 Martin Gühmann)
// - Replaced old const database by new one. (5-Aug-2007 Martin Gühmann)
// - GobalWarming and OzoneDepletion are now event handled. (29-Oct-2007 Martin Gühmann)
//
//----------------------------------------------------------------------------

#include "c3.h"
#include "World.h"

#include "Globals.h"
#include "XY_Coordinates.h"
#include "Cell.h"
#include "StrDB.h"
#include "TerrainRecord.h"
#include "GlobalWarmingRecord.h"
#include "UVDB.h"
#include "RandGen.h"
#include "citydata.h"
#include "ConstRecord.h"
#include "network.h"
#include "GoodyHuts.h"
#include "tiledmap.h"
#include "player.h"
#include "TerrImprovePool.h"
#include "UnitData.h"

#include "net_info.h"
#include "AICause.h"
#include "installationtree.h"
#include "radarmap.h"
#include "cellunitlist.h"

#include "director.h"

#include "UnitRecord.h"

#include "MoveFlags.h"

#include "SlicEngine.h"
#include "SlicObject.h"

#include "terrainutil.h"
#include "tradeutil.h"
#include "GameEventManager.h"           // g_gevManager

extern  OzoneDatabase   *g_theUVDB ;
extern  RadarMap        *g_radarMap;

#define N_X(d)      (d)
#define N_Y(d)      (d-1)
#define NE_X(d)     (d+1)
#define NE_Y(d)     (d-1)
#define E_X(d)      (d+1)
#define E_Y(d)      (d)
#define SE_X(d)     (d+1)
#define SE_Y(d)     (d+1)
#define S_X(d)      (d)
#define S_Y(d)      (d+1)
#define SW_X(d)     (d-1)
#define SW_Y(d)     (d+1)
#define W_X(d)      (d-1)
#define W_Y(d)      (d)
#define NW_X(d)     (d-1)
#define NW_Y(d)     (d-1)

void World::RaiseWaters(void)
{
	Cell	*c ;

	sint32 const    w = GetWidth();
	sint32 const    h = GetHeight();

	for(sint32 y = 0; y < h; y++)
	{
		for (sint32 x = 0; x < w; x++)
		{
			c = GetCell(x, y);
			if (c->GetScratch()<0 && !(c->GetEnv() & k_BIT_MOVEMENT_TYPE_WATER))
				FloodCell(x, y, c);
		}
	}

	MapPoint cp(0,0), n;
	for(cp.x = 0; cp.x < m_size.x; cp.x++)
	{
		for(cp.y = 0; cp.y < m_size.y; cp.y++)
		{
			if(!IsRiver(cp.x,cp.y))
				continue;

			if(IsSurroundedByWater(cp.x,cp.y))
			{
				GetCell(cp.x,cp.y)->m_env &= ~(k_BIT_ENV_RIV_CUR);
			}
			else
			{
				bool nextToRiver = false;
				for(sint32 d = 0; d < sint32(NOWHERE); d++)
				{
					if(cp.GetNeighborPosition((WORLD_DIRECTION)d, n))
					{
						if(IsRiver(n.x, n.y))
						{
							nextToRiver = true;
							break;
						}
					}
				}

				if(!nextToRiver)
				{
					GetCell(cp.x,cp.y)->m_env &= ~(k_BIT_ENV_RIV_CUR);
				}
			}
		}
	}
}

void World::FloodCell(sint32 x, sint32 y, Cell *c)
{
	Assert(c);

	FloodEverythingInCell(x, y, c);
	ConvertToShallowWater(x, y, c);
}

void World::FloodEverythingInCell(sint32 x, sint32 y, Cell *c)
{
	FloodArmies(c);
	FloodCity(c);
	FloodGoodyHut(c);
	FloodImprovements(x, y, c);
}

void World::FloodImprovements(sint32 x, sint32 y, Cell *c)
{
	MapPoint    pos    (x, y);
	Cell *cell = GetCell(x, y);

	if (cell->GetNumDBImprovements() > 0 || cell->GetNumImprovements() > 0)
	{
		g_theWorld->CutImprovements(pos);
		if(g_network.IsHost())
		{
			g_network.Enqueue(c, pos.x, pos.y);
		}

		DynamicArray<Installation> instArray;
		g_theInstallationTree->GetAt(pos, instArray);
		instArray.KillList();
	}

	if(cell->GetNumImprovements())
	{
		for(sint32 i = cell->GetNumImprovements() - 1; i >= 0; i--)
		{
			cell->AccessImprovement(i).Kill();
		}
	}
}

void World::FloodGoodyHut(Cell *c)
{
#if 0
		delete c->m_jabba ;
		c->m_jabba = NULL ;
#endif
}

void World::FloodCity(Cell *c)
{
	Unit	u   = c->GetCity();

	if (u.IsValid())
	{
		SlicObject *so = new SlicObject("04CitiesKilledByCalamity");
		so->AddCity(u);
		so->AddRecipient(u.GetOwner());
		g_slicEngine->Execute(so);

		u.KillUnit(CAUSE_REMOVE_ARMY_POLLUTION, -1);
	}
}



void World::FloodArmies(Cell *c)
{
	Assert(c);

	for (sint32 i = c->GetNumUnits() - 1; i >= 0; i--)
	{
		Unit u  = c->AccessUnit(i);
		if (u.IsValid() && !u.GetMovementTypeSea())
		{
			u.KillUnit(CAUSE_REMOVE_ARMY_POLLUTION, -1);
		}
	}
}


void World::ConvertToShallowWater(sint32 x, sint32 y, Cell *c)
{
	Assert(c);

	c->m_terrain_type = (sint8)TERRAIN_WATER_SHALLOW;
	SetMovementType(x, y, k_MOVEMENT_TYPE_SHALLOW_WATER);
	c->m_search_count = 0;

	c->m_env &= ~k_MASK_ENV_RIV_CUR;
	c->m_env &= ~k_MASK_ENV_CANAL_TUNNEL;
	Assert(IsWater(x,y));
}

void World::ConvertToBeach(sint32 x, sint32 y, Cell *c)
{
	Assert(c);

	c->m_terrain_type = (sint8)TERRAIN_WATER_BEACH;
	SetMovementType(x, y, k_MOVEMENT_TYPE_SHALLOW_WATER);
	c->m_search_count = 0;
}

void World::MakeBeaches()
{
	Cell	*c;

	sint32 const    w = GetWidth();
	sint32 const    h = GetHeight();

	for(sint32 y = 0; y < h; y++)
	{
		for(sint32 x = 0; x < w; x++)
		{
			c = GetCell(x, y) ;
			const TerrainRecord *rec = g_theTerrainDB->Get(c->GetTerrainType());
			if(rec->GetMovementTypeLand())
			{
				SetMovementType(x,y,k_MOVEMENT_TYPE_LAND);
			}
			else if(rec->GetMovementTypeMountain())
			{
				SetMovementType(x,y,k_MOVEMENT_TYPE_MOUNTAIN);
			}
			else if(rec->GetMovementTypeShallowWater())
			{
				SetMovementType(x,y,k_MOVEMENT_TYPE_WATER | k_MOVEMENT_TYPE_SHALLOW_WATER);
				if(c->m_env & k_MASK_ENV_CANAL_TUNNEL)
				{
					c->m_env |= k_BIT_MOVEMENT_TYPE_LAND;
				}
			}
			else if(rec->GetMovementTypeSea())
			{
				SetMovementType(x,y,k_MOVEMENT_TYPE_WATER);
				if(c->m_env & k_MASK_ENV_CANAL_TUNNEL)
				{
					c->m_env |= k_BIT_MOVEMENT_TYPE_LAND;
				}
			}

			if(IsWater(x,y))
			{
				if(c->GetCity().m_id != 0)
				{
					if(!c->GetCity().GetDBRec()->GetMovementTypeSea())
					{
						c->GetCity().Kill(CAUSE_REMOVE_ARMY_FLOOD, -1);
						Assert(c->GetCity().m_id==(0));
					}
				}
				if(!IsSurroundedByWater(x, y))
				{
					ConvertToBeach(x, y, c);
				}
			}
		}
	}
}

void World::RemoveBeaches()
{
	sint32	x, y;

	Cell	*c ;
	
	sint32 const    w = GetWidth();
	sint32 const    h = GetHeight();

	for(y = 0; y < h; y++)
	{
		for(x = 0; x < w; x++)
		{
			c = GetCell(x, y);
			c->m_search_count = 0 ;
			if (c->m_terrain_type == TERRAIN_WATER_BEACH)
			{
				c->m_search_count = -3 ;
				c->m_terrain_type = (sint8)TERRAIN_WATER_SHALLOW ;
				SetMovementType(x, y, k_MOVEMENT_TYPE_WATER | k_MOVEMENT_TYPE_SHALLOW_WATER) ;
			}
		}
	}
}

void world_AddUnseenForHumans(sint32 x, sint32 y)
{
	for(sint32 i = 0; i < k_MAX_PLAYERS; i++)
	{
		if(!g_player[i]) continue;

		if(!g_player[i]->IsRobot() ||
		   (g_network.IsClient() && g_network.IsLocalPlayer(i))) {
			MapPoint pos(x, y);
			g_player[i]->m_vision->AddUnseen(pos);
		}
	}
}

void World::GWPhase(const sint32 phase)
{
	Cell	*c;

	sint32 x;
	sint32 y;

	sint32 newtype;
	sint32 terrain;

	const GlobalWarmingRecord* gwrec = GetGlobalWarmingDBRec();

	double	baseProbability = 0.0;
	
	sint32 const    w = GetWidth();
	sint32 const    h = GetHeight();

	for (y=0; y<h; y++)
	{
		for (x=0; x<w; x++)
		{
			c = GetCell(x, y);
			terrain = sint32(c->m_terrain_type);
			if(phase == 0)
			{
				if(g_theTerrainDB->Get(terrain)->GetInternalTypeSwamp())
				{
					ConvertToShallowWater(x, y, c);
				}
			}
			else
			{
				if (!IsWater(x,y))
				{
					Assert(!g_theTerrainDB->Get(terrain)->GetMovementTypeShallowWater());
					if (IsNextToWater(x, y) && !IsMountain(x, y))
						baseProbability = gwrec->GetPollutionNextToOcean();
					
					newtype = ChangeType(baseProbability, terrain);
					bool wasLand = IsLand(x,y) || IsMountain(x,y);

					if(g_theTerrainDB->Get(newtype)->GetMovementTypeShallowWater())
					{
						c->m_search_count = -3 ;
					}
					else
					{
						if(g_theTerrainDB->Get(newtype)->GetMovementTypeSea()
						&&!g_theTerrainDB->Get(newtype)->GetMovementTypeShallowWater()
						){
							c->m_search_count = -3;
							newtype = GetTerrainChangeType(&TerrainRecord::GetMovementTypeShallowWater);
						}
						
						c->m_terrain_type = (sint8)newtype;
						if(terrain != newtype)
						{
							if(g_theTerrainDB->Get(newtype)->GetMovementTypeShallowWater())
							{
								if(wasLand)
								{
									FloodEverythingInCell(x, y, c);
								}
							}
						}
					}
				}
			}
		}
	}

	if(phase != 0)
	{
		for(sint32 x = 0; x < m_size.x; x++)
		{
			for(sint32 y = 0; y < m_size.y; y++)
			{
				if( IsLand(x,y)
				&& !IsMountain(x,y)
				&&  IsNextToWaterNotDiagonals(x,y)
				){
					if(g_rand->Next(100) < sint32(g_theConstDB->Get(0)->GetFloodChangesCoastToWaterChance() * 100.0))
					{
						GetCell(x,y)->m_search_count = -3;
					}
				}
			}
		}
	}

	RemoveIsolatedWater();
}

void World::GlobalWarming(const sint32 phase)
{
	g_gevManager->AddEvent(GEV_INSERT_Tail, GEV_GlobalWarming,
	                       GEA_Int, phase,
	                       GEA_End);
}

// Only called by the event
void World::GlobalWarmingEvent(const sint32 phase)
{
	if(g_network.IsHost())
	{
		g_network.SyncRand();
		g_network.Enqueue(new NetInfo(NET_INFO_CODE_GLOBAL_WARMING, phase));
	}

	InformPlayersOfFloodingCatastrophe();
	RemoveBeaches();
	GWPhase(phase);
	RaiseWaters();
	MakeBeaches();
	RegenerateRivers();
	MakeBeaches();

	for(sint32 p = 0; p < k_MAX_PLAYERS; p++)
	{
		if (g_player[p])
		{
			g_player[p]->RemoveEmptyCities(CAUSE_REMOVE_ARMY_FLOOD);
		}
	}

	sint32 x, y;
	for(x = 0; x < m_size.x; x++)
	{
		for(y = 0; y < m_size.y; y++)
		{
			Cell *theCell = GetCell(x,y);
			MapPoint pos(x,y);
			if(theCell->UnitArmy())
			{
				if(!theCell->UnitArmy()->CanEnter(pos))
					theCell->UnitArmy()->KillList(CAUSE_REMOVE_ARMY_FLOOD, -1);
			}
			if(theCell->GetCity().IsValid())
			{
				if((IsWater(pos) || IsShallowWater(pos)))
				{
					if(!theCell->GetCity().GetMovementTypeSea())
					{
						theCell->GetCity().Kill(CAUSE_REMOVE_ARMY_FLOOD, -1);
					}
				}
				else
				{
					if(!theCell->GetCity().GetMovementTypeLand())
					{
						theCell->GetCity().Kill(CAUSE_REMOVE_ARMY_FLOOD, -1);
					}
				}
			}
		}
	}

	g_tiledMap->PostProcessMap();
	g_tiledMap->Refresh();
	g_radarMap->Update();
	g_radarMap->ShouldDraw();

	for(sint32 i = 0; i < k_MAX_PLAYERS; i++)
	{
		if (g_player[i])
		{
			g_player[i]->m_vision->ClearUnseen();
		}
	}

	for(x = 0; x < m_size.x; x++)
	{
		for(y = 0; y < m_size.y; y++)
		{
			if(m_map[x][y]->GetCity().m_id != 0)
			{
				world_AddUnseenForHumans(x,y);
			}
		}
	}

	g_director->AddCopyVision();
	g_director->CatchUp();

	NumberContinents();
	SetAllMoveCost();
	ClearScratch();
}

void World::InformPlayersOfFloodingCatastrophe(void)
{
	for(sint32 i = 0; i < k_MAX_PLAYERS; i++)
	{
		if (g_player[i] && !g_player[i]->IsDead())
		{
			g_player[i]->IndicateTerrainPolluted();
		}
	}
}

void World::FloodRivers(void)
{
	static	sint32	offset_pass1[4][2] = { { -1, +1 }, { -1, 0 }, { -1, -1 }, { 0, -1 } },
					offset_pass2[4][2] = { { +1, -1 }, { +1, 0 }, {+1, +1 }, { 0, +1 } } ;

	sint32	o,
			x, y;

	sint32 const    h = GetHeight();
	sint32 const    w = GetWidth();
	
	for(y = 0; y < h; y++)
	{
		for(x = 0; x < w; x++)
		{
			if(!IsMountain(x, y))
			{
				for(o = 0; o < 4; o++)
				{
					static MapPoint pnt;

					pnt.x = sint16(x+offset_pass1[o][0]);
					pnt.y = sint16(y+offset_pass1[o][1]);

					if(pnt.x < 0 || pnt.x >= m_size.x || pnt.y < 0 || pnt.y >= m_size.y)
						continue;
					if ((GetCell(pnt)->GetScratch()==-2) && IsRiver(x, y))
					{
						GetCell(x, y)->m_search_count = -3;
					}
					else if ((GetCell(pnt)->GetScratch()==-1) && IsRiver(x, y))
					{
						GetCell(x, y)->m_search_count = -2;
					}
					else if ((IsWater(pnt)))
					{
						GetCell(x, y)->m_search_count = -1;
					}
				}
			}
		}
	}

	for(y = h-1; y >= 0; y--)
	{
		for(x = w-1;x >= 0; x--)
		{
			if(!IsMountain(x, y))
			{
				for(o = 0; o < 4; o++)
				{
					static MapPoint pnt;
					
					pnt.x = sint16(x+offset_pass2[o][0]);
					pnt.y = sint16(y+offset_pass2[o][1]);

					if(pnt.x < 0 || pnt.x >= m_size.x || pnt.y < 0 || pnt.y >= m_size.y)
						continue;
					if((GetCell(pnt)->GetScratch()==-2) && IsRiver(x, y))
					{
						GetCell(x, y)->m_search_count = -3;
					}
					else if((GetCell(pnt)->GetScratch()==-1) && IsRiver(x, y))
					{
						GetCell(x, y)->m_search_count = -2;
					}
					else if((IsWater(pnt)))
					{
						GetCell(x, y)->m_search_count = -1;
					}
				}
			}
		}
	}
}

void World::OzoneDepletion(void)
{
	g_gevManager->AddEvent(GEV_INSERT_Tail, GEV_OzoneDepletion,
	                       GEA_End);
}

// Only called by the event
void World::OzoneDepletionEvent(void)
{
	if(g_network.IsHost())
	{
		sint32  phase   = 0;
		g_network.SyncRand();
		g_network.Enqueue(new NetInfo(NET_INFO_CODE_OZONE_DEPLETION,
									  phase));
	}

	for(sint32 i = 0; i < k_MAX_PLAYERS; i++)
	{
		if (g_player[i])
			g_player[i]->IndicateTerrainPolluted();
	}

	RemoveBeaches();
	sint32 const    w = GetWidth();
	sint32 const    h = GetHeight();
	sint32 m;
	MapPoint pos;
	for(sint32 y = 0; y < h; y++)
	{
		for(sint32 x = 0; x < w; x++)
		{
			Cell* c = GetCell(x, y);

			constutil_y2meridian(y, m);

			Assert(k_NUM_MERIDIANS == 6);
			Assert(m<=k_NUM_MERIDIANS);
			double flip_prob = g_theUVDB->GetProb(m);
			if ((c->GetCanDie()) && (g_rand->NextF() < flip_prob))
			{
				pos.Set(x, y);

				c->Kill();
				ClearGoods(x, y);
				CutImprovements(pos);
			}
		}
	}

	MakeBeaches();
	SetAllMoveCost();
	g_tiledMap->PostProcessMap();
	g_tiledMap->Refresh();
	g_radarMap->Update();
	g_radarMap->ShouldDraw();
}

void World::RegenerateRivers()
{
	sint8 *map = new sint8[m_size.y * m_size.x];
	sint8 *wetmap = new sint8[m_size.y * m_size.x];

#define k_BASE_GROUND_HEIGHT 0
#define k_BASE_HILL_HEIGHT 50
#define k_BASE_MOUNTAIN_HEIGHT 100
#define k_HEIGHT_RANGE 40
#define k_GROUND_HEIGHT static_cast<sint8>(k_BASE_GROUND_HEIGHT + (g_rand->Next(k_HEIGHT_RANGE) - ((k_HEIGHT_RANGE) / 2)))
#define k_HILL_HEIGHT static_cast<sint8>(k_BASE_HILL_HEIGHT + (g_rand->Next(k_HEIGHT_RANGE) - ((k_HEIGHT_RANGE) / 2)))
#define k_MOUNTAIN_HEIGHT static_cast<sint8>(k_BASE_MOUNTAIN_HEIGHT + (g_rand->Next(k_HEIGHT_RANGE) - ((k_HEIGHT_RANGE) / 2)))
#define k_WATER_HEIGHT -50

	sint32 x, y;
	for(x = 0; x < m_size.x; x++) {
		for(y = 0; y < m_size.y; y++) {
			m_map[x][y]->m_env &= ~(k_MASK_ENV_RIV_CUR);
			sint32 tt = m_map[x][y]->m_terrain_type;
			if(tt == TERRAIN_FOREST) {
				map[y * m_size.x + x] = k_GROUND_HEIGHT;
				wetmap[y * m_size.x + x] = 30;
					
			} else if(tt == TERRAIN_PLAINS) {
				map[y * m_size.x + x] = k_GROUND_HEIGHT;
				wetmap[y * m_size.x + x] = 10;
					
			} else if(tt == TERRAIN_TUNDRA) {
				map[y * m_size.x + x] = k_GROUND_HEIGHT;
				wetmap[y * m_size.x + x] = 0;
					
			} else if(tt == TERRAIN_GLACIER) {
				map[y * m_size.x + x] = k_GROUND_HEIGHT;
				wetmap[y * m_size.x + x] = 0;
					
			} else if(tt == TERRAIN_GRASSLAND) {
				map[y * m_size.x + x] = k_GROUND_HEIGHT;
				wetmap[y * m_size.x + x] = 20;
					
			} else if(tt == TERRAIN_DESERT) {
				map[y * m_size.x + x] = k_GROUND_HEIGHT;
				wetmap[y * m_size.x + x] = -10;
					
			} else if(tt == TERRAIN_SWAMP) {
				map[y * m_size.x + x] = k_GROUND_HEIGHT;
				wetmap[y * m_size.x + x] = 40;
					
			} else if(tt == TERRAIN_JUNGLE) {
				map[y * m_size.x + x] = k_GROUND_HEIGHT;
				wetmap[y * m_size.x + x] = 30;
					
			} else if(tt == TERRAIN_MOUNTAIN) {
				map[y * m_size.x + x] = k_MOUNTAIN_HEIGHT;
				wetmap[y * m_size.x + x] = 10;
					
			} else if(tt == TERRAIN_HILL) {
				map[y * m_size.x + x] = k_HILL_HEIGHT;
				wetmap[y * m_size.x + x] = 10;
					
			} else if(tt == TERRAIN_WATER_SHALLOW) {
				map[y * m_size.x + x] = k_WATER_HEIGHT;
				wetmap[y * m_size.x + x] = 0;
					
			} else if(tt == TERRAIN_WATER_DEEP) {
				map[y * m_size.x + x] = k_WATER_HEIGHT;
				wetmap[y * m_size.x + x] = 0;
					
			} else if(tt == TERRAIN_WATER_VOLCANO) {
				map[y * m_size.x + x] = k_WATER_HEIGHT;
				wetmap[y * m_size.x + x] = 0;
					
			} else if(tt == TERRAIN_WATER_BEACH) {
				map[y * m_size.x + x] = k_WATER_HEIGHT;
				wetmap[y * m_size.x + x] = 0;
					
			} else if(tt == TERRAIN_WATER_SHELF) {
				map[y * m_size.x + x] = k_WATER_HEIGHT;
				wetmap[y * m_size.x + x] = 0;
					
			} else if(tt == TERRAIN_WATER_TRENCH) {
				map[y * m_size.x + x] = k_WATER_HEIGHT;
				wetmap[y * m_size.x + x] = 0;
					
			} else if(tt == TERRAIN_WATER_RIFT) {
				map[y * m_size.x + x] = k_WATER_HEIGHT;
				wetmap[y * m_size.x + x] = 0;
					
			} else if(tt == TERRAIN_DEAD) {
				map[y * m_size.x + x] = k_GROUND_HEIGHT;
				wetmap[y * m_size.x + x] = 0;
					
			} else if(tt == TERRAIN_BROWN_HILL) {
				map[y * m_size.x + x] = k_HILL_HEIGHT;
				wetmap[y * m_size.x + x] = -10;
					
			} else if(tt == TERRAIN_BROWN_MOUNTAIN) {
				map[y * m_size.x + x] = k_MOUNTAIN_HEIGHT;
				wetmap[y * m_size.x + x] = -10;
					
			} else if(tt == TERRAIN_WHITE_HILL) {
				map[y * m_size.x + x] = k_HILL_HEIGHT;
				wetmap[y * m_size.x + x] = 0;
					
			} else if(tt == TERRAIN_WHITE_MOUNTAIN) {
				map[y * m_size.x + x] = k_MOUNTAIN_HEIGHT;
				wetmap[y * m_size.x + x] = 0;
			}
			wetmap[y * m_size.x + x] += static_cast<sint8>(g_rand->Next(10) - 5);
		}
	}

	NewGenerateRivers(map, wetmap);
	delete [] map;
	delete [] wetmap;
}

const GlobalWarmingRecord* World::GetGlobalWarmingDBRec() const
{
	// Can be improvemed (more records, diff dependent)
	return g_theGlobalWarmingDB->Get(0);
}

sint32 World::ChangeType(const double baseProb, const sint32 terrain) const
{
	sint32 newTerrainType = terrain;
	double probability = g_rand->NextF();
	double lastChance = 999.0f;
	sint32 i;

	const GlobalWarmingRecord* gwrec = GetGlobalWarmingDBRec();

	for(i = 0; i < gwrec->GetNumChangeTypeProbability(); i++)
	{
		const GlobalWarmingRecord::ChangeTypeProbability* ctprec = gwrec->GetChangeTypeProbability(i);
		sint32 fromType = -1;

		if (ctprec->GetFromTypeIndex(fromType))
		{
			if (terrain == fromType)
			{
				double chanceProb = 0.0;
				ctprec->GetProbability(chanceProb);

				if (   chanceProb < lastChance
				    && probability < (chanceProb + baseProb)
				   )
				{
					lastChance      = chanceProb;
					newTerrainType  = -1;
					ctprec->GetToTypeIndex(newTerrainType);
				}
			}
		}
		else
		{
		    // Ugly but needed for backwards compatibility with MedPack2
			double chanceProb = 0.0;

			if(     ctprec->GetPOLLUTION_TILE_BROWN_HILL()     > -1.0){
				fromType = GetTerrainChangeType(&TerrainRecord::GetInternalTypeBrownHill);
				chanceProb = ctprec->GetPOLLUTION_TILE_BROWN_HILL();
			}
			else if(ctprec->GetPOLLUTION_TILE_BROWN_MOUNTAIN() > -1.0){
				fromType = GetTerrainChangeType(&TerrainRecord::GetInternalTypeBrownMountain);
				chanceProb = ctprec->GetPOLLUTION_TILE_BROWN_MOUNTAIN();
			}
			else if(ctprec->GetPOLLUTION_TILE_DESERT()         > -1.0){
				fromType = GetTerrainChangeType(&TerrainRecord::GetInternalTypeDesert);
				chanceProb = ctprec->GetPOLLUTION_TILE_DESERT();
			}
			else if(ctprec->GetPOLLUTION_TILE_FOREST()         > -1.0){
				fromType = GetTerrainChangeType(&TerrainRecord::GetInternalTypeForest);
				chanceProb = ctprec->GetPOLLUTION_TILE_FOREST();
			}
			else if(ctprec->GetPOLLUTION_TILE_GLACIER()        > -1.0){
				fromType = GetTerrainChangeType(&TerrainRecord::GetInternalTypeGlacier);
				chanceProb = ctprec->GetPOLLUTION_TILE_GLACIER();
			}
			else if(ctprec->GetPOLLUTION_TILE_GRASSLAND()      > -1.0){
				fromType = GetTerrainChangeType(&TerrainRecord::GetInternalTypeGrassland);
				chanceProb = ctprec->GetPOLLUTION_TILE_GRASSLAND();
			}
			else if(ctprec->GetPOLLUTION_TILE_HILL()           > -1.0){
				fromType = GetTerrainChangeType(&TerrainRecord::GetInternalTypeHill);
				chanceProb = ctprec->GetPOLLUTION_TILE_HILL();
			}
			else if(ctprec->GetPOLLUTION_TILE_JUNGLE()         > -1.0){
				fromType = GetTerrainChangeType(&TerrainRecord::GetInternalTypeJungle);
				chanceProb = ctprec->GetPOLLUTION_TILE_JUNGLE();
			}
			else if(ctprec->GetPOLLUTION_TILE_MOUNTAIN()       > -1.0){
				fromType = GetTerrainChangeType(&TerrainRecord::GetInternalTypeMountain);
				chanceProb = ctprec->GetPOLLUTION_TILE_MOUNTAIN();
			}
			else if(ctprec->GetPOLLUTION_TILE_PLAINS()         > -1.0){
				fromType = GetTerrainChangeType(&TerrainRecord::GetInternalTypePlains);
				chanceProb = ctprec->GetPOLLUTION_TILE_PLAINS();
			}
			else if(ctprec->GetPOLLUTION_TILE_SWAMP()          > -1.0){
				fromType = GetTerrainChangeType(&TerrainRecord::GetInternalTypeSwamp);
				chanceProb = ctprec->GetPOLLUTION_TILE_SWAMP();
			}
			else if(ctprec->GetPOLLUTION_TILE_TUNDRA()         > -1.0){
				fromType = GetTerrainChangeType(&TerrainRecord::GetInternalTypeTundra);
				chanceProb = ctprec->GetPOLLUTION_TILE_TUNDRA();
			}
			else if(ctprec->GetPOLLUTION_TILE_WHITE_HILL()     > -1.0){
				fromType = GetTerrainChangeType(&TerrainRecord::GetInternalTypeWhiteHill);
				chanceProb = ctprec->GetPOLLUTION_TILE_WHITE_HILL();
			}
			else if(ctprec->GetPOLLUTION_TILE_WHITE_MOUNTAIN() > -1.0){
				fromType = GetTerrainChangeType(&TerrainRecord::GetInternalTypeWhiteMountain);
				chanceProb = ctprec->GetPOLLUTION_TILE_WHITE_MOUNTAIN();
			}

			if (terrain == fromType)
			{
				if (chanceProb < lastChance
				    && probability < (chanceProb + baseProb)
				   )
				{
					lastChance = chanceProb;
					newTerrainType = GetTerrainChangeToType(ctprec);
				}
			}
		}
	}

	return newTerrainType;
}

sint32 World::GetTerrainChangeType(bool (TerrainRecord::*terrainFunc)() const) const
{
	for(sint32 i = 0; i < g_theTerrainDB->NumRecords(); ++i){
		if((g_theTerrainDB->Get(i)->*terrainFunc)()){
			return i;
		}
	}

	return -1;
}

sint32 World::GetTerrainChangeToType(const GlobalWarmingRecord::ChangeTypeProbability* ctprec) const
{
	if(     ctprec->GetPOLLUTION_TILE_TO_BROWN_HILL()){
		return GetTerrainChangeType(&TerrainRecord::GetInternalTypeBrownHill);
	}
	else if(ctprec->GetPOLLUTION_TILE_TO_BROWN_MOUNTAIN()){
		return GetTerrainChangeType(&TerrainRecord::GetInternalTypeBrownMountain);
	}
	else if(ctprec->GetPOLLUTION_TILE_TO_DESERT()){
		return GetTerrainChangeType(&TerrainRecord::GetInternalTypeDesert);
	}
	else if(ctprec->GetPOLLUTION_TILE_TO_FOREST()){
		return GetTerrainChangeType(&TerrainRecord::GetInternalTypeForest);
	}
	else if(ctprec->GetPOLLUTION_TILE_TO_GRASSLAND()){
		return GetTerrainChangeType(&TerrainRecord::GetInternalTypeGrassland);
	}
	else if(ctprec->GetPOLLUTION_TILE_TO_JUNGLE()){
		return GetTerrainChangeType(&TerrainRecord::GetInternalTypeJungle);
	}
	else if(ctprec->GetPOLLUTION_TILE_TO_MOUNTAIN()){
		return GetTerrainChangeType(&TerrainRecord::GetInternalTypeMountain);
	}
	else if(ctprec->GetPOLLUTION_TILE_TO_OCEAN()){
		return GetTerrainChangeType(&TerrainRecord::GetMovementTypeShallowWater);
	}
	else if(ctprec->GetPOLLUTION_TILE_TO_PLAINS()){
		return GetTerrainChangeType(&TerrainRecord::GetInternalTypePlains);
	}
	else if(ctprec->GetPOLLUTION_TILE_TO_SWAMP()){
		return GetTerrainChangeType(&TerrainRecord::GetInternalTypeSwamp);
	}
	else if(ctprec->GetPOLLUTION_TILE_TO_TUNDRA()){
		return GetTerrainChangeType(&TerrainRecord::GetInternalTypeTundra);
	}
	else if(ctprec->GetPOLLUTION_TILE_TO_WHITE_HILL()){
		return GetTerrainChangeType(&TerrainRecord::GetInternalTypeWhiteHill);
	}
	else if(ctprec->GetPOLLUTION_TILE_TO_WHITE_MOUNTAIN()){
		return GetTerrainChangeType(&TerrainRecord::GetInternalTypeWhiteMountain);
	}

	return -1;
}
