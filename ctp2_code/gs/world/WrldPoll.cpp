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
//
//----------------------------------------------------------------------------

#include "c3.h"
#include "Globals.h"

#include "XY_Coordinates.h"
#include "World.h"
#include "Cell.h"
#include "StrDB.h"
#include "TerrainRecord.h"
#include "gwdb.h"
#include "UVDB.h"
#include "RandGen.h"
#include "citydata.h"
#include "ConstDB.h"
#include "network.h"
#include "GoodyHuts.h"
#include "tiledmap.h"
#include "player.h"
#include "TerrImprovePool.h"
#include "UnitData.h"

#include "net_info.h"
#include "AICause.h"
#include "installationtree.h"
#include "PollutionDB.h"
#include "radarmap.h"
#include "cellunitlist.h"

#include "director.h"

#include "UnitRecord.h"

#include "MoveFlags.h"

#include "SlicEngine.h"
#include "SlicObject.h"

#include "terrainutil.h"

extern	PollutionDatabase	*g_thePollutionDB ;
extern	GlobalWarmingDatabase	*g_theGWDB ;
extern	OzoneDatabase	*g_theUVDB ;
extern	RadarMap	*g_radarMap;

#define N_X(d)		(d)
#define N_Y(d)		(d-1)
#define NE_X(d)		(d+1)
#define NE_Y(d)		(d-1)
#define E_X(d)		(d+1)
#define E_Y(d)		(d)
#define SE_X(d)		(d+1)
#define SE_Y(d)		(d+1)
#define S_X(d)		(d)
#define S_Y(d)		(d+1)
#define SW_X(d)		(d-1)
#define SW_Y(d)		(d+1)
#define W_X(d)		(d-1)
#define W_Y(d)		(d)
#define NW_X(d)		(d-1)
#define NW_Y(d)		(d-1)






















void World::RaiseWaters(void)
	{
	Cell	*c ;

	sint32	x, y,
			w, h ;

	w = GetWidth() ;
	h = GetHeight() ;
	for (y=0; y<h; y++)
		for (x=0; x<w; x++)
			{
			c = GetCell(x, y) ;
			if (c->GetScratch()<0 && !(c->GetEnv() & k_BIT_MOVEMENT_TYPE_WATER))
				FloodCell(x, y, c) ;

			}

	MapPoint cp(0,0), n;
	for(cp.x = 0; cp.x < m_size.x; cp.x++) {
		for(cp.y = 0; cp.y < m_size.y; cp.y++) {
			if(!IsRiver(cp.x,cp.y))
				continue;
			if(IsSurroundedByWater(cp.x,cp.y)) {
				GetCell(cp.x,cp.y)->m_env &= ~(k_BIT_ENV_RIV_CUR);
			} else {

				sint32 d;
				bool nextToRiver = false;
				for(d = 0; d < sint32(NOWHERE); d++) {
					if(cp.GetNeighborPosition((WORLD_DIRECTION)d, n)) {
						if(IsRiver(n.x, n.y)) {
							nextToRiver = true;
							break;
						}
					}
				}
				if(!nextToRiver) {
					GetCell(cp.x,cp.y)->m_env &= ~(k_BIT_ENV_RIV_CUR);
				}
			}
		}
	}
	}


void World::FloodCell(sint32 x, sint32 y, Cell *c)
	{

	Assert(c) ;

	FloodEverythingInCell(x, y, c) ;
	ConvertToShallowWater(x, y, c) ;
	}

void World::FloodEverythingInCell(sint32 x, sint32 y, Cell *c)
	{
	FloodArmies(c) ;
	FloodCity(c) ;
	FloodGoodyHut(c) ;
	FloodImprovements(x, y, c) ;
	}

void World::FloodImprovements(sint32 x, sint32 y, Cell *c)
{
	MapPoint pos ;

	pos.x = sint16(x) ;
	pos.y = (sint16)y ;
	Cell *cell = GetCell(x, y);

	if (cell->GetNumDBImprovements() > 0 || cell->GetNumImprovements() > 0)
		{
		g_theWorld->CutImprovements(pos);
		if(g_network.IsHost())
			{
			g_network.Enqueue(c, pos.x, pos.y) ;
			}

		static DynamicArray<Installation> instArray ;
		instArray.Clear() ;
		g_theInstallationTree->GetAt(pos, instArray) ;
		instArray.KillList() ;
		}
	if(GetCell(pos)->GetNumImprovements()) {

		sint32 i;
		Cell *cell = GetCell(pos);
		for(i = cell->GetNumImprovements() - 1; i >= 0; i--)
			cell->AccessImprovement(i).Kill();
	}
}

void World::FloodGoodyHut(Cell *c)
{
#if 0

	if(c->m_jabba)
	{

		delete c->m_jabba ;
		c->m_jabba = NULL ;
	}
#endif
}

void World::FloodCity(Cell *c)
	{
	Unit	u ;

	if (c->GetCity().m_id != (0))
		{

		u = c->GetCity() ;




		SlicObject *so = new SlicObject("04CitiesKilledByCalamity");
		so->AddCity(u);
		so->AddRecipient(u.GetOwner());
		g_slicEngine->Execute(so);




		u.KillUnit(CAUSE_REMOVE_ARMY_POLLUTION, -1);
		}

	}


void World::FloodArmies(Cell *c)
	{
	sint32	n,
			i ;

	Unit	u ;

	Assert(c) ;

	n = c->GetNumUnits() ;

	for (i=n-1; i>=0; i--)
		{

		if (!(c->AccessUnit(i).GetMovementTypeSea()))
			{

			u = c->AccessUnit(i) ;

			u.KillUnit(CAUSE_REMOVE_ARMY_POLLUTION, -1) ;
			}

		}

	}

void World::ConvertToShallowWater(sint32 x, sint32 y, Cell *c)
	{

	Assert(c) ;

	c->m_terrain_type = (sint8)TERRAIN_WATER_SHALLOW ;
	SetMovementType(x, y, k_MOVEMENT_TYPE_SHALLOW_WATER) ;
	c->m_search_count = 0 ;

	c->m_env &= ~k_MASK_ENV_RIV_CUR ;
	c->m_env &= ~k_MASK_ENV_CANAL_TUNNEL ;
	Assert(IsWater(x,y)) ;
	}

void World::ConvertToBeach(sint32 x, sint32 y, Cell *c)
{

	Assert(c) ;

	c->m_terrain_type = (sint8)TERRAIN_WATER_BEACH ;
	SetMovementType(x, y, k_MOVEMENT_TYPE_SHALLOW_WATER) ;
	c->m_search_count = 0 ;

	c->m_env &= ~k_MASK_ENV_RIV_CUR ;
	c->m_env &= ~k_MASK_ENV_CANAL_TUNNEL ;
}











void World::MakeBeaches()
{
	Cell	*c ;

	sint32	x, y,
			w, h ;

	w = GetWidth() ;
	h = GetHeight() ;

	for (y=0; y<h; y++) {
		for (x=0; x<w; x++) {
			c = GetCell(x, y) ;
			const TerrainRecord *rec = g_theTerrainDB->Get(c->GetTerrainType());
			if(rec->GetMovementTypeLand()) {
				SetMovementType(x,y,k_MOVEMENT_TYPE_LAND);
			} else if(rec->GetMovementTypeMountain()) {
				SetMovementType(x,y,k_MOVEMENT_TYPE_MOUNTAIN);
			} else if(rec->GetMovementTypeShallowWater()) {
				SetMovementType(x,y,k_MOVEMENT_TYPE_WATER | k_MOVEMENT_TYPE_SHALLOW_WATER);
				if(c->m_env & k_MASK_ENV_CANAL_TUNNEL) {
					c->m_env |= k_BIT_MOVEMENT_TYPE_LAND;
				}
			} else if(rec->GetMovementTypeSea()) {
				SetMovementType(x,y,k_MOVEMENT_TYPE_WATER);
				if(c->m_env & k_MASK_ENV_CANAL_TUNNEL) {
					c->m_env |= k_BIT_MOVEMENT_TYPE_LAND;
				}
			}

			if (IsWater(x,y)) {
				if(c->GetCity().m_id != 0) {
					if(!c->GetCity().GetDBRec()->GetMovementTypeSea()) {
						c->GetCity().Kill(CAUSE_REMOVE_ARMY_FLOOD, -1);
						Assert(c->GetCity().m_id==(0)) ;
					}
				}
				if (!IsSurroundedByWater(x, y)) {

					ConvertToBeach(x, y, c) ;
				}
			}
		}
	}
}











void World::RemoveBeaches()
	{
	sint32	x, y,
			w, h ;

	Cell	*c ;

	w = GetWidth() ;
	h = GetHeight() ;
	for (y=0; y<h; y++)
		for (x=0; x<w; x++)
			{
			c = GetCell(x, y) ;
			c->m_search_count = 0 ;
			if (c->m_terrain_type == TERRAIN_WATER_BEACH)
				{
				c->m_search_count = -3 ;
				c->m_terrain_type = (sint8)TERRAIN_WATER_SHALLOW ;
				SetMovementType(x, y, k_MOVEMENT_TYPE_WATER | k_MOVEMENT_TYPE_SHALLOW_WATER) ;
				}

			}

	}

void world_AddUnseenForHumans(sint32 x, sint32 y)
{
	sint32 i;
	for(i = 0; i < k_MAX_PLAYERS; i++) {
		if(!g_player[i]) continue;
		if(g_player[i]->GetPlayerType() != PLAYER_TYPE_ROBOT ||
		   (g_network.IsClient() && g_network.IsLocalPlayer(i))) {
			MapPoint pos(x, y);
			g_player[i]->m_vision->AddUnseen(pos);
		}
	}
}










void World::GWPhase(const sint32 phase)
{
	Cell	*c ;

	sint32	x, y,
			w, h ;

	TERRAIN_TYPES	newtype,
					terrain ;

	double	baseProbability = 0.0;

	w = GetWidth() ;
	h = GetHeight() ;
	for (y=0; y<h; y++) {
		for (x=0; x<w; x++) {
			c = GetCell(x, y) ;
			if(phase == 0) {
				if(c->m_terrain_type == TERRAIN_SWAMP) {

					ConvertToShallowWater(x, y, c);
				}
			} else {
				if (!IsWater(x,y)) {
					terrain = TERRAIN_TYPES(c->m_terrain_type) ;
					Assert(terrain != TERRAIN_WATER_SHALLOW) ;
					if (IsNextToWater(x, y) && !IsMountain(x, y))
						baseProbability = g_theGWDB->NextToWaterBonus() ;

					newtype = g_theGWDB->Change(baseProbability, TERRAIN_TYPES(terrain)) ;
					BOOL wasLand = IsLand(x,y) || IsMountain(x,y);

					if (newtype == TERRAIN_WATER_SHALLOW) {
						c->m_search_count = -3 ;

					} else {
						if(g_theTerrainDB->Get(newtype)->GetMovementTypeSea() &&
						   !g_theTerrainDB->Get(newtype)->GetMovementTypeShallowWater()) {
						    c->m_search_count = -3;
							newtype = TERRAIN_WATER_SHALLOW;
						}

						c->m_terrain_type = (sint8)newtype ;
						if(terrain != newtype) {
							if(newtype == TERRAIN_WATER_SHALLOW) {
								if(wasLand) {
									FloodEverythingInCell(x, y, c);
								}
							}
						}

					}

				}
			}
		}
	}

	if(phase != 0) {


		for(x = 0; x < m_size.x; x++) {
			for(y = 0; y < m_size.y; y++) {
				if(IsLand(x,y) && !IsMountain(x,y) &&
				   IsNextToWaterNotDiagonals(x,y)) {
					if(g_rand->Next(100) < sint32(g_theConstDB->GetFloodChangesCoastToWaterChance() * 100.0)) {
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
	MapPoint	pos,
				n ;

	if(g_network.IsHost()) {
		g_network.SyncRand();
		g_network.Enqueue(new NetInfo(NET_INFO_CODE_GLOBAL_WARMING, phase));
	}

	InformPlayersOfFloodingCatastrophe() ;
	RemoveBeaches() ;
	GWPhase(phase) ;
	RaiseWaters() ;
	MakeBeaches() ;
	RegenerateRivers();
	MakeBeaches() ;
	sint32 p;
	for(p = 0; p < k_MAX_PLAYERS; p++) {
		if(g_player[p]) {
			g_player[p]->RemoveEmptyCities(CAUSE_REMOVE_ARMY_FLOOD);
		}
	}

	sint32 x, y;
	for(x = 0; x < m_size.x; x++) {
		for(y = 0; y < m_size.y; y++) {
			Cell *theCell = GetCell(x,y);
			MapPoint pos(x,y);
			if(theCell->UnitArmy()) {
				if(!theCell->UnitArmy()->CanEnter(pos))
					theCell->UnitArmy()->KillList(CAUSE_REMOVE_ARMY_FLOOD, -1);
			}
			if(theCell->GetCity().m_id != 0) {
				if((IsWater(pos) || IsShallowWater(pos))) {
					if(!theCell->GetCity().GetMovementTypeSea()) {
						theCell->GetCity().Kill(CAUSE_REMOVE_ARMY_FLOOD, -1);
					}
				} else {
					if(!theCell->GetCity().GetMovementTypeLand()) {
						theCell->GetCity().Kill(CAUSE_REMOVE_ARMY_FLOOD, -1);
					}
				}
			}
		}
	}

	g_tiledMap->PostProcessMap() ;
	g_tiledMap->Refresh() ;
	g_radarMap->Update();
	g_radarMap->ShouldDraw();

	sint32 i;
	for(i = 0; i < k_MAX_PLAYERS; i++) {
		if(!g_player[i]) continue;
		g_player[i]->m_vision->ClearUnseen();
	}


	for(x = 0; x < m_size.x; x++) {
		for(y = 0; y < m_size.y; y++) {
			if(m_map[x][y]->GetCity().m_id != 0) {
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
	sint32	i ;

	for (i=0;i<k_MAX_PLAYERS; i++)
		{
		if (g_player[i] && !g_player[i]->IsDead())
			g_player[i]->IndicateTerrainPolluted() ;

		}

	}











void World::FloodRivers(void)
	{
	static	sint32	offset_pass1[4][2] = { { -1, +1 }, { -1, 0 }, { -1, -1 }, { 0, -1 } },
					offset_pass2[4][2] = { { +1, -1 }, { +1, 0 }, {+1, +1 }, { 0, +1 } } ;

	sint32	o,
			x, y,
			w, h ;

	h = GetHeight() ;
	w = GetWidth() ;

	for (y=0; y<h; y++)
		for (x=0; x<w; x++)
			{
			if (!IsMountain(x, y))
				{
				for (o=0; o<4; o++)
					{
					static MapPoint pnt;

					pnt.x = sint16(x+offset_pass1[o][0]);
					pnt.y = sint16(y+offset_pass1[o][1]);

					if(pnt.x < 0 || pnt.x >= m_size.x || pnt.y < 0 || pnt.y >= m_size.y)
						continue;
					if ((GetCell(pnt)->GetScratch()==-2) && IsRiver(x, y))
						{
						GetCell(x, y)->m_search_count = -3 ;
						}
					else if ((GetCell(pnt)->GetScratch()==-1) && IsRiver(x, y))
						{
						GetCell(x, y)->m_search_count = -2 ;
						}
					else if ((IsWater(pnt)))
						{
						GetCell(x, y)->m_search_count = -1 ;
						}

					}

				}

			}

	for (y=h-1; y>=0; y--)
		for (x=w-1;x>=0; x--)
			{
			if (!IsMountain(x, y))
				{
				for (o=0; o<4; o++)
					{
					static MapPoint pnt;

					pnt.x = sint16(x+offset_pass2[o][0]);
					pnt.y = sint16(y+offset_pass2[o][1]);

					if(pnt.x < 0 || pnt.x >= m_size.x || pnt.y < 0 || pnt.y >= m_size.y)
						continue;
					if ((GetCell(pnt)->GetScratch()==-2) && IsRiver(x, y))
						{
						GetCell(x, y)->m_search_count = -3 ;
						}
					else if ((GetCell(pnt)->GetScratch()==-1) && IsRiver(x, y))
						{
						GetCell(x, y)->m_search_count = -2 ;
						}
					else if ((IsWater(pnt)))
						{
						GetCell(x, y)->m_search_count = -1 ;
						}

					}

				}

			}

	}












void World::OzoneDepletion(void)
	{
	sint32	i,
			m,
			x, y,
			w, h ;

	Cell	*c ;

	sint32	phase = 0 ;

	if(g_network.IsHost()) {
		g_network.SyncRand();
		g_network.Enqueue(new NetInfo(NET_INFO_CODE_OZONE_DEPLETION,
									  phase));
	}

	for (i=0;i<k_MAX_PLAYERS; i++) {
		if (g_player[i])
			g_player[i]->IndicateTerrainPolluted() ;
	}

	RemoveBeaches() ;
	w = GetWidth() ;
	h = GetHeight() ;
    MapPoint pos;
	for (y=0; y<h; y++)
		for (x=0; x<w; x++)
			{
			c = GetCell(x, y) ;

			g_theConstDB->y2meridian(y, m) ;

			Assert(k_NUM_MERIDIANS == 6) ;
			Assert(m<=k_NUM_MERIDIANS) ;
			double flip_prob = g_theUVDB->GetProb(m) ;
			if ((c->GetCanDie()) && (g_rand->NextF() < flip_prob))
				{

                pos.Set(x, y);




				c->Kill() ;
				ClearGoods(x, y) ;
				CutImprovements(pos);





				}

			}

	MakeBeaches() ;
	SetAllMoveCost();
	g_tiledMap->PostProcessMap() ;
	g_tiledMap->Refresh() ;
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
#define k_GROUND_HEIGHT (k_BASE_GROUND_HEIGHT + (g_rand->Next(k_HEIGHT_RANGE) - ((k_HEIGHT_RANGE) / 2)))
#define k_HILL_HEIGHT (k_BASE_HILL_HEIGHT + (g_rand->Next(k_HEIGHT_RANGE) - ((k_HEIGHT_RANGE) / 2)))
#define k_MOUNTAIN_HEIGHT (k_BASE_MOUNTAIN_HEIGHT + (g_rand->Next(k_HEIGHT_RANGE) - ((k_HEIGHT_RANGE) / 2)))
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
			wetmap[y * m_size.x + x] += g_rand->Next(10) - 5;
		}
	}

	NewGenerateRivers(map, wetmap);
	delete [] map;
	delete [] wetmap;
}
