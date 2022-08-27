//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : World generator
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
// - Generate debug version
//
// DUMP_TERRAIN_HEIGHT_MAPS
// USE_COM_REPLACEMENT
// CELL_COLOR
//----------------------------------------------------------------------------
//
// Modifications from the original Activision code:
//
// - Implemented CalcTerrainFreightCost by Martin Gühmann
// - Resolved ambiguous sqrt call.
// - Standardised min/max usage.
// - Repaired memory leaks.
// - Force a good value recalculation on reload if the ressouce database was
//   modified (goods added removed). - May 19th 2005 Martin Gühmann
// - Wrap handling improved
// - Using /importmap to import a text map no longer causes the river mouths
//   to be deleted - 2005-07-01 Shaun Dove
// - Initialized local variables. (Sep 9th 2005 Martin Gühmann)
// - Replaced old map database by new one. (27-Mar-2007 Martin Gühmann)
// - Replaced old const database by new one. (5-Aug-2007 Martin Gühmann)
// - Added a no goody huts option (20-Mar-2009 Maq)
//
//----------------------------------------------------------------------------

#include "c3.h"                     // pre-compiled
#include "World.h"                  // own declarations, g_theWorld
#include "worldutils.h"

#include "AICause.h"
#include <algorithm>
#if defined(DUMP_TERRAIN_HEIGHT_MAPS)
#include "bmp_io.h"
#endif
#include "c3errors.h"
#include "Cell.h"
#include "cellunitlist.h"
#include "citydata.h"
#include "civarchive.h"
#include "ConstRecord.h"            // g_theConstDB
#include "dynarr.h"
#include "gamefile.h"
#include "Globals.h"
#include "GoodyHuts.h"
#include "MoveFlags.h"
#include "network.h"
#include "player.h"                 // g_player
#include "PQueue.h"
#include "profileDB.h"              // g_theProfileDB
#include "RandGen.h"                // g_rand
#include "ResourceRecord.h"
#include "StartingPosition.h"
#include "TerrainRecord.h"
#include "terrainutil.h"
#include "tiledmap.h"               // g_tiledMap
#include "TileInfo.h"
#include <vector>
#include "WorldDistance.h"
#include "tradeutil.h"              // constutil_GetMapSizeMapPoint
#include "StrDB.h"
#include "MessageBoxDialog.h"

#if defined(USE_COM_REPLACEMENT)
#include <dlfcn.h>
#else
#include <initguid.h>
#endif
#include "C3Rand.h"
#include "IMapGen.h"

extern MapPoint g_mp_size;




static C3Rand *                 s_randomGenerator   = NULL;
static DynamicArray<MapPoint> * s_visited           = NULL;

void TemperatureFilter(sint8 *map, sint32 *histogram);










World::World(const MapPoint & m, const int xw, const int yw)
:
    CityRadiusCallback      (),
    m_isYwrap               (yw),
    m_isXwrap               (xw),
    m_mapGenerator          (MAP_GENERATOR_PLUGIN),
    m_size                  (m),
    m_map                   (NULL),
    m_water_next_too_land   (NULL),
    m_land_next_too_water   (NULL),
    m_water_size            (NULL),
    m_land_size             (NULL),
    m_cellArray             (NULL),
    m_tmpx                  (NULL),
    m_tileInfoStorage       (NULL),
    m_goodValue             (NULL),
    m_num_civ_starts        (0),
    m_current_plugin        (NULL),
    m_distanceQueue         (NULL)
{
    Assert(0 < m_size.x);
    Assert(0 < m_size.y);
	AllocateMap();

    // Ugly
    g_mp_size = m_size;
    if (!s_randomGenerator)
    {
        s_randomGenerator   = new C3Rand();
    }
    (void) s_randomGenerator->AddRef();
}

void World::CreateTheWorld(MapPoint player_start_list[k_MAX_PLAYERS],
						   sint32 player_start_score[k_MAX_PLAYERS])
{
	bool ignoreTutorialRules    = false;
	bool worldIsGood            = true;

	do {

		GenerateRandMap(player_start_list);

		SetAllMoveCost();

		m_continents_are_numbered= FALSE;

		NumberContinents();

		CalcChokePoints();

		sint32 numFound;
		FindPlayerStart(player_start_list, numFound, ignoreTutorialRules, player_start_score);

		worldIsGood = true;

		if(numFound < 3 ||
		   (g_network.IsNetworkLaunch() && g_network.IsLaunchHost() &&
			numFound < g_network.GetNumHumanPlayers())) {
			if(g_theProfileDB->IsTutorialAdvice() && !ignoreTutorialRules) {
				Reset(m_size.x, m_size.y, m_isYwrap, m_isXwrap);
				ignoreTutorialRules = true;
				worldIsGood = false;
			} else {
				Assert(g_theProfileDB->PercentLand() < 1.0);
				if (g_theProfileDB->PercentLand() < 1.0)
                {
					g_theProfileDB->SetPercentLand
                        (std::min<double>(1.0,
                                          g_theProfileDB->PercentLand() + 0.1
                                         )
                        );

                    Reset(m_size.x, m_size.y, m_isYwrap, m_isXwrap);
					ignoreTutorialRules = false;
					worldIsGood = false;
				}
			}
		}

		if (worldIsGood)
        {
            if (0 == s_randomGenerator->Release())
            {
                s_randomGenerator = NULL;
            }
        }
        else
        {
			for (sint32 x = 0; x < m_size.x; x++) {
				for (sint32 y = 0; y < m_size.y; y++) {
					m_map[x][y]->m_env = 0;
				}
			}
		}
	} while(!worldIsGood);

	ClearScratch();
}

World::World(CivArchive &archive, BOOL fromMapFile)
:
    m_mapGenerator          (MAP_GENERATOR_PLUGIN),
    m_map                   (NULL),
    m_water_next_too_land   (NULL),
    m_land_next_too_water   (NULL),
    m_water_size            (NULL),
    m_land_size             (NULL),
    m_cellArray             (NULL),
    m_tmpx                  (NULL),
    m_tileInfoStorage       (NULL),
    m_goodValue             (NULL),
    m_num_civ_starts        (0),
    m_current_plugin        (NULL),
    m_distanceQueue         (NULL)
//  m_isXwrap, m_isYwrap, m_size: set by Serialize
{
	if ( fromMapFile )
		SerializeJustMap(archive);
	else
	    Serialize(archive) ;

	g_mp_size = m_size;
}

World::~World()
{
	FreeMap();  // m_map, m_tmpx, m_cellArray, m_water_next_too_land,
                // m_land_next_too_water, m_water_size, m_land_size,
                // m_tileInfoStorage
	delete m_distanceQueue;

	delete [] m_goodValue;

	if (m_current_plugin)
	{
#ifndef USE_COM_REPLACEMENT
		FreeLibrary(m_current_plugin);
#else
		dlclose(m_current_plugin);
#endif
	}
}

void World::FreeMap()
{
	for (int x = 0; x < m_size.x; x++)
    {
		if (m_isYwrap)
        {
			delete [] (CellPtr*)(m_map[x] - k_MAP_WRAPAROUND);
        }
		else
        {
			delete [] m_map[x];
        }
	}

	delete [] m_tmpx;
    m_tmpx      = NULL;
	delete [] m_cellArray;
	m_cellArray = NULL;

	DisposeTileInfoStorage();
	m_map       = NULL;

    delete m_water_next_too_land;
    m_water_next_too_land = NULL;
    delete m_land_next_too_water;
    m_land_next_too_water = NULL;
    delete m_water_size;
    m_water_size = NULL;
    delete m_land_size;
    m_land_size = NULL;
}

void World::Reset(sint16 sx, sint16 sy, BOOL yWrap, BOOL xWrap)
{
	FreeMap();
	m_size.x = sx;
	m_size.y = sy;
    g_mp_size = m_size;
	bool yWrapOk = sy % sx == 0;
	m_isYwrap = yWrapOk ? yWrap : FALSE;
	m_isXwrap = xWrap;
	AllocateMap();
}

void World::AllocateMap()
{
    AllocateTileInfoStorage();

    delete [] m_cellArray;
	m_cellArray = new Cell[m_size.x * m_size.y];

    Assert (2 * k_MAP_WRAPAROUND <= m_size.x);
    Assert (2 * k_MAP_WRAPAROUND <= m_size.y);

    CellYarray *    tmpx = new CellYarray[m_size.x + 2 * k_MAP_WRAPAROUND];
	m_tmpx = tmpx;

	m_map = &(tmpx[k_MAP_WRAPAROUND]);
	Assert(m_map);

    sint32 x;
	for (x = 0; x < m_size.x; x++)
    {
		if (m_isYwrap)
        {
			CellPtr * tmpy = new CellPtr[m_size.y + 2 * k_MAP_WRAPAROUND];
			m_map[x] = &(tmpy[k_MAP_WRAPAROUND]);
		}
        else
        {
			m_map[x] = new CellPtr[m_size.y];
		}

		for (sint32 y = 0; y < m_size.y; y++)
        {
			m_map[x][y] = &m_cellArray[y * m_size.x + x];
		}
    }

    std::copy(m_map, m_map + k_MAP_WRAPAROUND, m_map + m_size.x);
    sint32 i;
	for (i = -1; - k_MAP_WRAPAROUND <= i; i--)
    {
		m_map[i] = m_map[m_size.x + i];
	}

	if (m_isYwrap) {
		for (x=0; x<m_size.x; x++)
        {
            std::copy(m_map[x], m_map[x] + k_MAP_WRAPAROUND, m_map[x] + m_size.y);

			for (i=-1; - k_MAP_WRAPAROUND <= i; i--) {
				m_map[x][i] = m_map[x][m_size.y + i];
			}
		}
	}

    m_water_next_too_land = new DynamicArray<DAsint32>;
    m_land_next_too_water = new DynamicArray<DAsint32>;
    m_water_size = new DynamicArray<sint32>;
    m_land_size = new DynamicArray<sint32>;
}










void World::GenerateRandMap(MapPoint player_start_list[k_MAX_PLAYERS])

{
	IMapGenerator *firstPass = LoadMapPlugin(0);
	Assert(firstPass);
	if(!firstPass) {
		return;
	}

	sint8 *map = new sint8[m_size.y * m_size.x];
	sint32 histogramarray[256];
	sint32 *histogram = &histogramarray[128];

	sint32 numSettings;
	const MapRecord *mapRec     = worldutils_FindBestMapSizeMatch(m_size.x, m_size.y);
	sint32          whichSet    = static_cast<sint32>(g_theProfileDB->PercentContinent() * 10);
	double *        settings    = worldutils_CreateSettings(mapRec, whichSet, numSettings);

	GetHeightMap(firstPass, map, settings, numSettings);
	firstPass->Release();
	FreeMapPlugin();
	worldutils_DeleteSettings(settings);

#ifdef DUMP_TERRAIN_HEIGHT_MAPS
	MapDump ("logs" FILE_SEP "HeightMap.bmp", map, m_size.x, m_size.y);
#endif

	IMapGenerator *filterPass = LoadMapPlugin(1);
	if(filterPass) {

		double pass1Settings[2] = {100.0, 5.0};
		filterPass->Generate(map, m_size.x, m_size.y, s_randomGenerator,
							 &pass1Settings[0], 2);
		filterPass->Release();
		FreeMapPlugin();

#ifdef DUMP_TERRAIN_HEIGHT_MAPS
		MapDump ("logs" FILE_SEP "FilterMap.bmp", map, m_size.x, m_size.y);
#endif
	}
	GetHistogram(map, histogram);
	sint32 landPercent = sint32(g_theProfileDB->PercentLand() * 100);
	sint32 waterPercent = 100 - landPercent;
	sint32 mountainPercent = sint32(g_theConstDB->Get(0)->GetPercentMountain());
	sint32 hillPercent = sint32(g_theConstDB->Get(0)->GetPercentHills());
	hillPercent += mountainPercent;

	sint32 waterLevel=0, hillLevel=80, mountainLevel=100;

	sint32 totalCells = m_size.y * m_size.x;
	sint32 count = 0;
    sint32 h;
	for(h = -128; h < 128; h++) {
		count += histogram[h];
		if(count >= (totalCells * waterPercent) / 100) {
			waterLevel = h;
			break;
		}
	}

	for(h = 127, count = 0; h > waterLevel; h--) {
		count += histogram[h];
		if(count > ((totalCells * landPercent) * mountainPercent) / 10000) {
			mountainLevel = h;
			break;
		}
	}
	for(; h > waterLevel ; h --) {
		count += histogram[h];
		if(count > ((totalCells * landPercent) * hillPercent ) / 10000) {
			hillLevel = h;
			break;
		}
	}

#ifdef DUMP_TERRAIN_HEIGHT_MAPS
	TerrainDump ("logs" FILE_SEP "TerrainMap.bmp", map, m_size.x, m_size.y, waterLevel, mountainLevel, hillLevel);
#endif

	sint32 x, y;
	for(y = 0; y < m_size.y; y++) {
		for(x = 0; x < m_size.x; x++) {
			MapPoint from(x,y);
			MapPoint to;
            FlatToIso(from, to);

			m_map[to.x][to.y]->m_env = 0;
			if(map[from.y * m_size.x + from.x] >= mountainLevel) {
				m_map[to.x][to.y]->m_terrain_type = TERRAIN_MOUNTAIN;
				SetMovementType(to.x, to.y, k_MOVEMENT_TYPE_MOUNTAIN);
			} else if(map[from.y * m_size.x + from.x] >= hillLevel) {
				m_map[to.x][to.y]->m_terrain_type = TERRAIN_HILL;
				SetMovementType(to.x, to.y, k_MOVEMENT_TYPE_LAND);
			} else if(map[from.y * m_size.x + from.x] > waterLevel) {
				m_map[to.x][to.y]->m_terrain_type = TERRAIN_PLAINS;
				SetMovementType(to.x, to.y, k_MOVEMENT_TYPE_LAND);
			} else {
				m_map[to.x][to.y]->m_terrain_type = TERRAIN_WATER_SHALLOW;
				SetMovementType(to.x, to.y, k_MOVEMENT_TYPE_WATER | k_MOVEMENT_TYPE_SHALLOW_WATER);
			}
		}
	}


	IMapGenerator *secondPass = LoadMapPlugin(2);
	if(!secondPass) {
		delete [] map;
		m_mapGenerator = MAP_GENERATOR_ORIGINAL;
		GenerateRandMap(player_start_list);
		return;
	}

	sint8 *wetmap = new sint8[m_size.y * m_size.x];
	sint32 wethistogramarray[256];
	sint32 *wethistogram = &wethistogramarray[128];

	double homogeneity = 0.9 * g_theProfileDB->PercentHomogenous() + 0.05;
	GetMapAndHistogram(secondPass, wetmap, wethistogram,
					   &homogeneity, 1);

#ifdef DUMP_TERRAIN_HEIGHT_MAPS
	MapDump ("logs" FILE_SEP "WetMap.bmp", wetmap, m_size.x, m_size.y);
#endif

	sint32 totalLandCells = 0;
	for(h = waterLevel + 1; h < 128; h++) {
		totalLandCells += histogram[h];
	}

	for(h = -128; h < 128; h++) {
		wethistogram[h] = 0;
	}
	for(y = 0; y < m_size.y; y++) {
		for(x = 0; x < m_size.x; x++) {
			if(map[y * m_size.x + x] <= waterLevel)
				continue;
			wethistogram[wetmap[y * m_size.x + x]]++;
		}
	}


	sint32 forestPercent = g_theProfileDB->PercentForest();
	sint32 grassPercent  = g_theProfileDB->PercentGrass();
	sint32 plainsPercent = g_theProfileDB->PercentPlains();
	sint32 desertPercent = g_theProfileDB->PercentDesert();

#define k_INVALID_LEVEL -1000
	sint32 forestLevel = k_INVALID_LEVEL,
		grassLevel = k_INVALID_LEVEL,
		plainsLevel = k_INVALID_LEVEL,
		desertLevel = k_INVALID_LEVEL;

	for(h = -128, count = 0; h < 128; h++) {
		count += wethistogram[h];
		if(count > (totalLandCells * desertPercent) / 100) {
			desertLevel = h;
			break;
		}
	}
	for(count = 0; h < 128; h++) {
		count += wethistogram[h];
		if(count > (totalLandCells * plainsPercent) / 100) {
			plainsLevel = h;
			break;
		}
	}
	for(count = 0; h < 128; h++) {
		count += wethistogram[h];
		if(count > (totalLandCells * grassPercent) / 100) {
			grassLevel = h;
			break;
		}
	}
	for(count = 0; h < 128; h++) {
		count += wethistogram[h];
		if(count > (totalLandCells * forestPercent) / 100) {
			forestLevel = h;
			break;
		}
	}
	Assert(forestLevel != k_INVALID_LEVEL);
	Assert(grassLevel != k_INVALID_LEVEL);
	Assert(plainsLevel != k_INVALID_LEVEL);
	Assert(desertLevel != k_INVALID_LEVEL);

	for(y = 0; y < m_size.y; y++) {
		for(x = 0; x < m_size.x; x++) {
			MapPoint from(x,y);
			MapPoint to;
            FlatToIso(from, to);

			if(m_map[to.x][to.y]->m_terrain_type == TERRAIN_PLAINS) {
				if(wetmap[from.y * m_size.x + from.x] >= forestLevel) {
					m_map[to.x][to.y]->m_terrain_type = TERRAIN_SWAMP;
				} else if(wetmap[from.y * m_size.x + from.x] >= grassLevel) {
					m_map[to.x][to.y]->m_terrain_type = TERRAIN_FOREST;
				} else if(wetmap[from.y * m_size.x + from.x] >= plainsLevel) {
					m_map[to.x][to.y]->m_terrain_type = TERRAIN_GRASSLAND;
				} else if(wetmap[from.y * m_size.x + from.x] >= desertLevel) {
					m_map[to.x][to.y]->m_terrain_type = TERRAIN_PLAINS;
				} else {
					m_map[to.x][to.y]->m_terrain_type = TERRAIN_DESERT;
				}
			} else if(m_map[to.x][to.y]->m_terrain_type == TERRAIN_WATER_SHALLOW) {
				if(wetmap[from.y * m_size.x + from.x] >= grassLevel) {
					m_map[to.x][to.y]->m_terrain_type = TERRAIN_WATER_KELP;
				} else if(wetmap[from.y * m_size.x + from.x] < desertLevel) {
					m_map[to.x][to.y]->m_terrain_type = TERRAIN_WATER_REEF;
				}
			}
		}
	}
	secondPass->Release();
	FreeMapPlugin();

	IMapGenerator *thirdPass = LoadMapPlugin(3);
	if(!thirdPass) {
		delete [] map;
		delete [] wetmap;
		m_mapGenerator = MAP_GENERATOR_ORIGINAL;
		GenerateRandMap(player_start_list);
		return;
	}

	sint8* temperatureMap = new sint8[m_size.y * m_size.x];
	sint32 temperatureHistArray[256];
	sint32 *temperatureHist = &temperatureHistArray[128];

	double pass3Settings[1] = { 0.5 };
	GetMapAndHistogram(thirdPass, temperatureMap, temperatureHist,
					   &pass3Settings[0], 1);
	TemperatureFilter(temperatureMap, temperatureHist);

#ifdef DUMP_TERRAIN_HEIGHT_MAPS
	MapDump ("logs" FILE_SEP "TempMap.bmp", temperatureMap, m_size.x, m_size.y);
#endif

	sint32 whitePercent = g_theProfileDB->PercentWhite();
	sint32 brownPercent = g_theProfileDB->PercentBrown();
	sint32 whiteLevel = k_INVALID_LEVEL;
	sint32 brownLevel = k_INVALID_LEVEL;

	for(h = -128, count = 0; h < 128; h++) {
		count += temperatureHist[h];
		if(count > (totalCells * whitePercent) / 100) {
			whiteLevel = h;
			break;
		}
	}

	for(h = 127, count = 0; h > -128; h--) {
		count += temperatureHist[h];
		if(count > (totalCells * brownPercent) / 100) {
			brownLevel = h;
			break;
		}
	}

	Assert(whiteLevel != k_INVALID_LEVEL);
	Assert(brownLevel != k_INVALID_LEVEL);
	Assert(whiteLevel < brownLevel);

	for(x = 0; x < m_size.x; x++) {
		for(y = 0; y < m_size.y; y++) {
			MapPoint from(x, y);
			MapPoint to;
            FlatToIso(from, to);

			if(temperatureMap[y * m_size.x + x] <= whiteLevel) {
				sint32 tt = m_map[to.x][to.y]->m_terrain_type;
				if((tt == TERRAIN_PLAINS) || (tt == TERRAIN_DESERT)) {
					m_map[to.x][to.y]->m_terrain_type = TERRAIN_GLACIER;
				} else if((tt == TERRAIN_SWAMP) || (tt == TERRAIN_GRASSLAND)) {
					m_map[to.x][to.y]->m_terrain_type = TERRAIN_TUNDRA;
				} else if(tt == TERRAIN_HILL) {
					m_map[to.x][to.y]->m_terrain_type = TERRAIN_WHITE_HILL;
				} else if(tt == TERRAIN_MOUNTAIN) {
					m_map[to.x][to.y]->m_terrain_type = TERRAIN_WHITE_MOUNTAIN;
				}
			} else if(temperatureMap[y * m_size.x + x] >= brownLevel) {
				sint32 tt = m_map[to.x][to.y]->m_terrain_type;
				if(tt == TERRAIN_FOREST) {
					m_map[to.x][to.y]->m_terrain_type = TERRAIN_JUNGLE;
				} else if(tt == TERRAIN_GRASSLAND) {
					m_map[to.x][to.y]->m_terrain_type = TERRAIN_PLAINS;
				} else if(tt == TERRAIN_PLAINS) {
					m_map[to.x][to.y]->m_terrain_type = TERRAIN_DESERT;
				} else if(tt == TERRAIN_HILL) {
					m_map[to.x][to.y]->m_terrain_type = TERRAIN_BROWN_HILL;
				} else if(tt == TERRAIN_MOUNTAIN) {
					m_map[to.x][to.y]->m_terrain_type = TERRAIN_BROWN_MOUNTAIN;
				}
			} else {

				if(m_map[to.x][to.y]->m_terrain_type == TERRAIN_DESERT) {
					m_map[to.x][to.y]->m_terrain_type = TERRAIN_PLAINS;
				}
			}
		}
	}

	RemoveIsolatedWater();

#define IS_SHALLOW(x) ( ((x) == TERRAIN_WATER_SHALLOW) || ((x) == TERRAIN_WATER_KELP) || ((x) == TERRAIN_WATER_REEF) )

	for(x = 0; x < m_size.x; x++) {
		for(y = 0; y < m_size.y; y++) {
			if(IS_SHALLOW(m_map[x][y]->m_terrain_type) && IsNextToLand(x,y)) {
				m_map[x][y]->m_terrain_type = TERRAIN_WATER_BEACH;
			}
		}
	}

	MapPoint neighbor;

	for(x = 0; x < m_size.x; x++) {
		for(y = 0; y < m_size.y; y++) {
			if (IS_SHALLOW(m_map[x][y]->m_terrain_type) &&
			    !IsNextTo(TERRAIN_WATER_BEACH, x, y)
               )
            {
				MapPoint curpos(x, y);
				bool skip = false;
				for (sint32 d = 0; d < 8; d++)
                {
					if(curpos.GetNeighborPosition((WORLD_DIRECTION) d, neighbor)) {
						if(IsNextTo(TERRAIN_WATER_BEACH, neighbor.x, neighbor.y)) {
							skip = true;
							break;
						}
					}
				}
				if(skip)
					continue;
				m_map[x][y]->m_terrain_type = TERRAIN_WATER_DEEP;
				SetMovementType(x, y, k_MOVEMENT_TYPE_WATER);
			}
		}
	}

	for(x = 0; x < m_size.x; x++) {
		for(y = 0; y < m_size.y; y++) {
			if(IS_SHALLOW(m_map[x][y]->m_terrain_type) &&
			   IsNextTo(TERRAIN_WATER_DEEP, x, y))
				m_map[x][y]->m_terrain_type = TERRAIN_WATER_SHELF;
		}
	}

	NewGenerateRivers(map, wetmap);
	GenerateDeepWater();
	GenerateVolcano();
	GenerateTrenches();

	thirdPass->Release();
	FreeMapPlugin();

	delete [] map;
	delete [] wetmap;
	delete [] temperatureMap;

	GenerateGoodyHuts();
	GenerateGoods();
}

void World::GenerateGoods()
{
	ClearScratch();

	sint32 x, y;
	for (x = 0; x < m_size.x; x++)
	{
		for (y = 0; y < m_size.y; y++)
		{
			MapPoint center(x, y);
			Cell *centerCell = GetCell(center);

			SquareIterator it(center, 1);
			for(it.Start(); !it.End(); it.Next()) {
				if(it.Pos() == center)
					continue;

				if(GetCell(it.Pos())->GetTerrain() == centerCell->GetTerrain()) {
					centerCell->SetScratch(centerCell->GetScratch() + 1);
				}
			}
		}
	}

	sint32 b, a;
	if (90 < g_theProfileDB->PercentRichness()) {
		b = 1;
		a = 2;
	} else if (80 < g_theProfileDB->PercentRichness()) {
		b = 2;
		a = 3;
	} else if (65 < g_theProfileDB->PercentRichness()) {
		b = 2;
		a = 4;
	} else if (45 < g_theProfileDB->PercentRichness()) {
		b = 2;
		a = 5;
	} else if (35 < g_theProfileDB->PercentRichness()) {
		b = 3;
		a = 6;
	} else if (20 < g_theProfileDB->PercentRichness()) {
		b = 3;
		a = 7;
	} else if (2 < g_theProfileDB->PercentRichness()){
		b = 4;
		a = 8;
	} else {
		return;
	}

	sint32 c = b + a;

	sint32 minAdjacent = g_theConstDB->Get(0)->GetMinLikeTilesForGood();

    sint32 ox, oy;
	for (ox = b; ox + a < m_size.x ; ox += c) {
		for (oy = b; oy + a < m_size.y ; oy += c) {
			x = ox + g_rand->Next(a);
			y = oy + g_rand->Next(a);

			if(GetCell(x, y)->GetScratch() >= minAdjacent) {
				SetRandomGood(x, y);
			}
		}

		if (oy < m_size.y) {
			x = ox + g_rand->Next(a);
			y = oy + g_rand->Next(m_size.y - oy);
			if(GetCell(x, y)->GetScratch() >= minAdjacent) {
				SetRandomGood(x, y);
			}
		}
	}

	sint32 r = m_size.x - ox;
	if (0 < r)
    {
		for (oy = b; oy + a < m_size.y ; oy += c) {
			x = ox + g_rand->Next(r);
			y = oy + g_rand->Next(a);
			if(GetCell(x, y)->GetScratch() >= minAdjacent) {
				SetRandomGood(x, y);
			}
		}

		if (oy < m_size.y) {
			x = ox + g_rand->Next(r);
			y = oy + g_rand->Next(m_size.y - oy);
			if(GetCell(x, y)->GetScratch() >= minAdjacent) {
				SetRandomGood(x, y);
			}
		}
	}

	ComputeGoodsValues();
}

void World::ComputeGoodsValues()
{
    sint32 newGoodCount = g_theResourceDB->NumRecords();

    delete [] m_goodValue;
    if (newGoodCount > 0) {
        m_goodValue = new double[newGoodCount];
    }
    else {
        m_goodValue = NULL;
        return;
    }

    std::vector<sint32> goodCounts(newGoodCount, 0);
	MapPoint pos;
	sint32 good;
	sint32 totalGoods = 0;
	for(pos.x = 0; pos.x < m_size.x; pos.x++) {
		for(pos.y = 0; pos.y < m_size.y; pos.y++) {
			if(GetGood(pos, good)) {
				Assert((good >= 0) && (good < newGoodCount));

				totalGoods++;
				goodCounts[good]++;
			}
		}
	}

	sint32 maxGood = -1;
	sint32 maxCount = 0;
	sint32 minGood = -1;
	sint32 minCount = 0x7fffffff;

	sint32 i;
	for(i = 0; i < newGoodCount; i++) {
		if (goodCounts[i] > maxCount) {
			maxCount = goodCounts[i];
			maxGood = i;
		}

		if ((goodCounts[i] > 0) && (goodCounts[i] < minCount)) {
			minCount = goodCounts[i];
			minGood = i;
		}
	}

	double valueDiff = g_theConstDB->Get(0)->GetMaxGoodValue() - g_theConstDB->Get(0)->GetMinGoodValue();

	for (i = 0; i < newGoodCount; i++) {
		if (goodCounts[i] <= 0) {
			m_goodValue[i] = g_theConstDB->Get(0)->GetMaxGoodValue() + 1;
		}
		else {
		    // goodCounts[i] > 0 => maxCount > 0, so division by maxCount is OK
		    double percent = double(goodCounts[i]) / double(maxCount);

		    m_goodValue[i] = g_theConstDB->Get(0)->GetMinGoodValue() + ((1.0 - percent) * valueDiff);
		}

		DPRINTF(k_DBG_GAMESTATE, ("Good %s has a base value of %lf\n", g_theResourceDB->Get(i)->GetNameText(), m_goodValue[i]));
	}
}

bool World::IsNextToLand(const sint32 i, const sint32 j)
{
	MapPoint pos(i, j);
	MapPoint n;

	return GetAdjacentLand(pos, n);
}

bool World::GetAdjacentLand(MapPoint const & pos, MapPoint & land) const
{
	for(sint16 dir = 0; dir < NOWHERE; ++dir)
	{
		if(pos.GetNeighborPosition(static_cast<WORLD_DIRECTION>(dir), land))
		{
			if(IsLand(land) || IsMountain(land))
			{
				return true;
			}
		}
	}

	return false;
}

bool World::IsNextToForeignerOnLand(const MapPoint &pos, PLAYER_INDEX owner) const
{
	MapPoint next;
	for(sint16 dir = 0; dir < NOWHERE; ++dir)
	{
		if(pos.GetNeighborPosition(static_cast<WORLD_DIRECTION>(dir), next))
		{
			if(IsWater(next))
				continue;

			Cell * cell = g_theWorld->GetCell(next);

			if(cell->GetNumUnits() > 0
			&& cell->UnitArmy()->GetOwner() != owner)
			{
				return true;
			}
		}
	}

	return false;
}

bool World::IsOccupiedByForeigner(const MapPoint &pos, PLAYER_INDEX owner) const
{
	Cell * cell = g_theWorld->GetCell(pos);

	return (cell->GetNumUnits() > 0)
		&& (cell->UnitArmy()->GetOwner() != owner);
}

bool World::GetAdjacentOcean(const MapPoint &pos, sint32 & water_cont) const
{
	MapPoint water;

	for(sint16 dir = 0; dir < NOWHERE; dir++)
	{
		if(pos.GetNeighborPosition((WORLD_DIRECTION) dir, water))
		{
			if(IsWater(water))
			{
				water_cont = GetCell(water)->GetContinent();

				if(GetWaterContinentSize(water_cont) > 25)
					return true;
			}
		}
	}

	return false;
}
bool World::IsSurroundedByWater(const sint32 x, const sint32 y)
{
	MapPoint	pos(x, y);
	return IsSurroundedByWater(pos);
}

bool World::HasAdjacentFreeLand(MapPoint const & pos, const sint32 player) const
{
	MapPoint neighbor;
	for(sint16 dir = 0; dir < NOWHERE; ++dir)
	{
		if(pos.GetNeighborPosition(static_cast<WORLD_DIRECTION>(dir), neighbor))
		{
			if
			  (
			   (
			        g_theWorld->IsLand(neighbor)
			     || g_theWorld->IsMountain(neighbor)
			   )
			   &&
			   (
			        g_theWorld->GetCell(neighbor)->GetNumUnits() == 0
			     || g_theWorld->GetCell(neighbor)->AccessUnit(0).GetOwner() == player
			   )
			  )
			{
				return true;
			}
		}
	}

	return false;
}

bool World::IsSurroundedByWater(MapPoint const & pos)
{
	MapPoint	n ;

	if(pos.GetNeighborPosition(NORTH, n))
	{
		if (!IsWater(n))
			return false;
	}
	if(pos.GetNeighborPosition(NORTHWEST, n))
	{
		if (!IsWater(n))
			return false;
	}
	if(pos.GetNeighborPosition(NORTHEAST, n))
	{
		if (!IsWater(n))
			return false;
	}
	if(pos.GetNeighborPosition(SOUTH, n))
	{
		if (!IsWater(n))
			return false;
	}
	if(pos.GetNeighborPosition(SOUTHWEST, n))
	{
		if (!IsWater(n))
			return false;
	}
	if(pos.GetNeighborPosition(SOUTHEAST, n))
	{
		if (!IsWater(n))
			return false;
	}
	if(pos.GetNeighborPosition(WEST, n))
	{
		if (!IsWater(n))
			return false;
	}
	if(pos.GetNeighborPosition(EAST, n))
	{
		if (!IsWater(n))
			return false;
	}
	return true;
}

bool World::IsNextTo (const sint32 t, const sint32 i, const sint32 j)
{
	MapPoint pos (i, j);
	MapPoint n;

	if(pos.GetNeighborPosition(NORTH, n))
		if (GetCell(n)->m_terrain_type == t) return true;

	if(pos.GetNeighborPosition(NORTHWEST, n))
		if (GetCell(n)->m_terrain_type == t) return true;

	if(pos.GetNeighborPosition(NORTHEAST, n))
		if (GetCell(n)->m_terrain_type == t) return true;

	if(pos.GetNeighborPosition(SOUTH, n))
		if (GetCell(n)->m_terrain_type == t) return true;

	if(pos.GetNeighborPosition(SOUTHWEST, n))
		if (GetCell(n)->m_terrain_type == t) return true;

	if(pos.GetNeighborPosition(SOUTHEAST, n))
		if (GetCell(n)->m_terrain_type == t) return true;

	if(pos.GetNeighborPosition(WEST, n))
		if (GetCell(n)->m_terrain_type == t) return true;

	if(pos.GetNeighborPosition(EAST, n))
		if (GetCell(n)->m_terrain_type == t) return true;

	return false;
}

bool World::IsNextToWater(const sint32 i, const sint32 j)
{
	MapPoint pos (i, j);
	MapPoint n;

	if(pos.GetNeighborPosition(NORTH, n))
		if (IsWater(n)) return true;

	if(pos.GetNeighborPosition(NORTHWEST, n))
		if (IsWater(n)) return true;

	if(pos.GetNeighborPosition(NORTHEAST, n))
		if (IsWater(n)) return true;

	if(pos.GetNeighborPosition(SOUTH, n))
		if (IsWater(n)) return true;

	if(pos.GetNeighborPosition(SOUTHWEST, n))
		if (IsWater(n)) return true;

	if(pos.GetNeighborPosition(SOUTHEAST, n))
		if (IsWater(n)) return true;

	if(pos.GetNeighborPosition(WEST, n))
		if (IsWater(n)) return true;

	if(pos.GetNeighborPosition(EAST, n))
		if (IsWater(n)) return true;

	return false;
}

bool World::IsNextToWaterNotDiagonals(const sint32 i, const sint32 j)
{
	MapPoint pos (i, j);
	MapPoint n;

	if(pos.GetNeighborPosition(NORTHWEST, n))
		if (IsWater(n)) return true;

	if(pos.GetNeighborPosition(NORTHEAST, n))
		if (IsWater(n)) return true;

	if(pos.GetNeighborPosition(SOUTHWEST, n))
		if (IsWater(n)) return true;

	if(pos.GetNeighborPosition(SOUTHEAST, n))
		if (IsWater(n)) return true;

	return false;
}

void World::GenerateDeepWater()

{
	sint32 i, j;
	MapPoint tmp;
	sint32 minx = 0, miny = 0, rmin, ocount, dcount, k;
	sint32 radius       = 2;
	sint32 delta        = 1;
	sint32 cellWidth    = g_theConstDB->Get(0)->GetRiverCellWidth();
    sint32 cellHeight   = g_theConstDB->Get(0)->GetRiverCellHeight();

	for(i = 0; i < cellWidth; i++) {
		for(j = 0; j < cellHeight; j++) {
			minx = i * (m_size.x / cellWidth);
			miny = j * (m_size.y / cellHeight);

			minx += g_rand->Next(cellWidth);
			miny += g_rand->Next(cellHeight);
			if (m_map[minx][miny]->m_terrain_type == TERRAIN_WATER_DEEP)  {
				m_map[minx][miny]->m_terrain_type = TERRAIN_WATER_RIFT;
			}
		}
	}

	ClearScratch();

	BOOL    find = TRUE;
	sint32  rcount = 0;
	sint32  x, y;
	BOOL hot;
	sint32 oldval;
	MapPoint pos;

	for (k=0; k<500 && find; k++) {

		find = FALSE;
		rmin = -2;

		for (i=0; i<m_size.x; i++) {
			for (j=0; j<m_size.y; j++) {
				if (m_map[i][j]->m_terrain_type == TERRAIN_WATER_DEEP) {

					hot = TRUE;
					for (x = std::max<sint32>(i-(radius-delta), 0);
						 hot &&  x <= std::min<sint32>(m_size.x-1, (i+(radius-delta)));
						 ++x
						)
					{
						for (y = std::max<sint32>(j-(radius-delta), 0);
							 hot && y <= std::min<sint32>(m_size.y-1, (j+(radius-delta)));
							 ++y
							)
						{

							if (m_map[x][y]->m_search_count != m_map[i][j]->m_search_count) {
								hot = FALSE;
								break;
							}
						}
					}
					if (!hot) {
						continue;
					}
					pos.Set(i, j);

					ocount=0;
					dcount = 0;
					if(pos.GetNeighborPosition(NORTH, tmp))
						if (GetCell(tmp)->m_terrain_type == TERRAIN_WATER_RIFT) dcount++;
					if(pos.GetNeighborPosition(SOUTH, tmp))
						if (GetCell(tmp)->m_terrain_type == TERRAIN_WATER_RIFT) dcount++;
					if(pos.GetNeighborPosition(WEST, tmp))
						if (GetCell(tmp)->m_terrain_type == TERRAIN_WATER_RIFT) dcount++;
					if(pos.GetNeighborPosition(EAST, tmp))
						if (GetCell(tmp)->m_terrain_type == TERRAIN_WATER_RIFT) dcount++;

					if(pos.GetNeighborPosition(NORTHEAST, tmp))
						if (GetCell(tmp)->m_terrain_type == TERRAIN_WATER_RIFT) ocount++;
					if(pos.GetNeighborPosition(SOUTHEAST, tmp))
						if (GetCell(tmp)->m_terrain_type == TERRAIN_WATER_RIFT) ocount++;
					if(pos.GetNeighborPosition(NORTHWEST, tmp))
						if (GetCell(tmp)->m_terrain_type == TERRAIN_WATER_RIFT) ocount++;
					if(pos.GetNeighborPosition(SOUTHWEST, tmp))
						if (GetCell(tmp)->m_terrain_type == TERRAIN_WATER_RIFT) ocount++;

					if ((ocount == 1) && (dcount < 2)){
						rmin = m_map[i][j]->m_terrain_type;
						minx = i;
						miny = j;
						find = TRUE;
					}

				}
			}
		}

		if (find) {
			m_map[minx][miny]->m_terrain_type = TERRAIN_WATER_RIFT;

			oldval =   m_map[minx][miny]->m_search_count;

			rcount++;
			for (i = std::max<sint32>(minx-(radius), 0);
				 i <= std::min<sint32>(m_size.x-1, (minx+(radius)));
				 ++i
				)
			{
				for (j = std::max<sint32>(miny-(radius), 0);
					 j <= std::min<sint32>(m_size.y-1, (miny+(radius)));
					 ++j
					)
				{
					if ((m_map[i][j]->m_search_count == oldval) ||
						(m_map[i][j]->m_search_count == 0)) {
						m_map[i][j]->m_search_count = rcount;
					}       else if (g_rand->Next(100) < 95) {
						m_map[i][j]->m_search_count = rcount;
					}
				}
			}
		}
	}
}

void World::GenerateTrenches()
{
	sint32 trench = g_theConstDB->Get(0)->GetPercentTrench();

	if(trench <= 0)
	{
		return;
	}

	sint32 i, j;
	sint32 tcount = 0;
	sint32 m = 0;

	for(i=0; i<m_size.x; i++)
	{
		for(j=0; j<m_size.y; j++)
		{
			if
			  (
			       (m_map[i][j]->m_terrain_type == TERRAIN_WATER_SHELF)
			    && IsNextTo(TERRAIN_WATER_DEEP, i, j)
			  )
			{
				++m;
			}
		}
	}

	m *= trench;
	m *= 0.01;

	if (m < 1)
		return;

	MapPoint pos;
	sint32 searching = TRUE;
	MapPoint tmp;

	sint32 ocount;
	sint32 sdcount, socount;

	for (i=0; i<m_size.x; i++)
	{
		for (j=0; j<m_size.y; j++)
		{
			if
			  (
			       (m_map[i][j]->m_terrain_type == TERRAIN_WATER_SHELF)
			    && IsNextTo(TERRAIN_WATER_DEEP, i, j)
			  )
			{
				if(g_rand->Next(100) < trench)
				{
					pos.Set(i, j);
					sdcount= 0;
					socount= 0;
					if(pos.GetNeighborPosition(NORTHWEST, tmp))
						if(GetCell(tmp)->m_terrain_type == TERRAIN_WATER_SHELF) socount++;

					if(pos.GetNeighborPosition(NORTHEAST,tmp))
						if (GetCell(tmp)->m_terrain_type == TERRAIN_WATER_SHELF) socount++;

					if(pos.GetNeighborPosition(SOUTHEAST, tmp))
						if (GetCell(tmp)->m_terrain_type == TERRAIN_WATER_SHELF) socount++;

					if(pos.GetNeighborPosition(SOUTHWEST,tmp))
						if (GetCell(tmp)->m_terrain_type == TERRAIN_WATER_SHELF) socount++;

					if(pos.GetNeighborPosition(SOUTH,tmp))
						if (GetCell(tmp)->m_terrain_type == TERRAIN_WATER_SHELF) sdcount++;

					if(pos.GetNeighborPosition(NORTH,tmp))
						if (GetCell(tmp)->m_terrain_type == TERRAIN_WATER_SHELF) sdcount++;

					if(pos.GetNeighborPosition(WEST,tmp))
						if (GetCell(tmp)->m_terrain_type == TERRAIN_WATER_SHELF) sdcount++;

					if(pos.GetNeighborPosition(EAST,tmp))
						if (GetCell(tmp)->m_terrain_type == TERRAIN_WATER_SHELF) sdcount++;

					if
					  (
					       ((sdcount == 2) && (socount == 0))
					    || ((sdcount == 0) && (socount == 2))
					  )
					{
						m_map[i][j]->m_terrain_type = TERRAIN_WATER_TRENCH;
						tcount++;
						if(m <= tcount)
							break;
					}
				}
			}
		}

		if(m <= tcount)
			break;
	}

	searching = TRUE;

    while (searching && ( tcount < m)) {
        searching = FALSE;
        for (i=0; i<m_size.x; i++) {
            for (j=0; j<m_size.y; j++) {
                if ((m_map[i][j]->m_terrain_type == TERRAIN_WATER_SHELF) &&
                IsNextTo(TERRAIN_WATER_DEEP, i, j)){

                    pos.Set(i, j);
                    ocount = 0;
                    sdcount = 0;
                    socount = 0;
                    if(pos.GetNeighborPosition(NORTHWEST, tmp)) {
						if (GetCell(tmp)->m_terrain_type == TERRAIN_WATER_TRENCH) ocount++;
						if (GetCell(tmp)->m_terrain_type == TERRAIN_WATER_SHELF) socount++;
					}

                    if(pos.GetNeighborPosition(NORTHEAST,tmp)) {
						if (GetCell(tmp)->m_terrain_type == TERRAIN_WATER_TRENCH) ocount++;
						if (GetCell(tmp)->m_terrain_type == TERRAIN_WATER_SHELF) socount++;
					}

                    if(pos.GetNeighborPosition(SOUTHEAST, tmp)) {
						if (GetCell(tmp)->m_terrain_type == TERRAIN_WATER_TRENCH) ocount++;
						if (GetCell(tmp)->m_terrain_type == TERRAIN_WATER_SHELF) socount++;
					}

                    if(pos.GetNeighborPosition(SOUTHWEST,tmp)) {
						if (GetCell(tmp)->m_terrain_type == TERRAIN_WATER_TRENCH) ocount++;
						if (GetCell(tmp)->m_terrain_type == TERRAIN_WATER_SHELF) socount++;
					}

                    if(pos.GetNeighborPosition(SOUTH,tmp)) {
						if (GetCell(tmp)->m_terrain_type == TERRAIN_WATER_SHELF) sdcount++;
					}
                    if(pos.GetNeighborPosition(NORTH,tmp)) {
						if (GetCell(tmp)->m_terrain_type == TERRAIN_WATER_SHELF) sdcount++;
					}
                    if(pos.GetNeighborPosition(WEST,tmp)) {
						if (GetCell(tmp)->m_terrain_type == TERRAIN_WATER_SHELF) sdcount++;
					}
                    if(pos.GetNeighborPosition(EAST,tmp)) {
						if (GetCell(tmp)->m_terrain_type == TERRAIN_WATER_SHELF) sdcount++;
					}

                    if ((0 < ocount) && (socount == 1)  && ((sdcount == 0) || (sdcount == 1))){
                        searching = TRUE;

                        m_map[i][j]->m_terrain_type = TERRAIN_WATER_TRENCH;
                        tcount++;
                    }
                }
            }
        }
    }

    sint32 dcount;
    searching = TRUE;
    while (searching) {
        searching = FALSE;
        for (i=0; i<m_size.x; i++) {
            for (j=0; j<m_size.y; j++) {
                if  (m_map[i][j]->m_terrain_type == TERRAIN_WATER_TRENCH) {
                    pos.Set(i, j);
                    dcount=0;
                    ocount=0;
                    if(pos.GetNeighborPosition(NORTH, tmp))
						if (GetCell(tmp)->m_terrain_type == TERRAIN_WATER_TRENCH) dcount++;
                    if(pos.GetNeighborPosition(SOUTH, tmp))
						if (GetCell(tmp)->m_terrain_type == TERRAIN_WATER_TRENCH) dcount++;
                    if(pos.GetNeighborPosition(EAST, tmp))
						if (GetCell(tmp)->m_terrain_type == TERRAIN_WATER_TRENCH) dcount++;
                    if(pos.GetNeighborPosition(WEST, tmp))
						if (GetCell(tmp)->m_terrain_type == TERRAIN_WATER_TRENCH) dcount++;

                    if(pos.GetNeighborPosition(SOUTHWEST, tmp))
						if (GetCell(tmp)->m_terrain_type == TERRAIN_WATER_TRENCH) ocount++;
                    if(pos.GetNeighborPosition(SOUTHEAST, tmp))
						if (GetCell(tmp)->m_terrain_type == TERRAIN_WATER_TRENCH) ocount++;
                    if(pos.GetNeighborPosition(NORTHWEST, tmp))
						if (GetCell(tmp)->m_terrain_type == TERRAIN_WATER_TRENCH) ocount++;
                    if(pos.GetNeighborPosition(NORTHEAST, tmp))
						if (GetCell(tmp)->m_terrain_type == TERRAIN_WATER_TRENCH) ocount++;

                    switch(ocount) {
                    case 0:
                        m_map[i][j]->m_terrain_type = TERRAIN_WATER_SHELF;
                        searching = TRUE;
                        break;
                    case 1:
                    case 2:
                        if (3 <= dcount) {
                            m_map[i][j]->m_terrain_type = TERRAIN_WATER_SHELF;

                            searching = TRUE;
                        }
                        break;

                    case 3:
                    case 4:
                        m_map[i][j]->m_terrain_type = TERRAIN_WATER_SHELF;
                        searching = TRUE;
                        break;
                    default:
                        break;
                    }
                }
            }
        }
	}
}








void World::GenerateVolcano()

{
	sint32 i, j;

	for (i=0; i<m_size.x; i++) {
		for (j=0; j<m_size.y; j++) {
			if(m_map[i][j]->m_terrain_type == TERRAIN_WATER_DEEP) {
				if (g_rand->Next(100) < g_theConstDB->Get(0)->GetPercentVolcano()) {
					m_map[i][j]->m_terrain_type = TERRAIN_WATER_VOLCANO;
				}
			}
		}
	}

    for (i=0; i<m_size.x; i++) {
		for (j=0; j<m_size.y; j++) {
			if (m_map[i][j]->m_terrain_type == TERRAIN_WATER_RIFT) {

				if (IsNextTo(TERRAIN_WATER_SHELF, i, j) ||
					IsNextTo(TERRAIN_WATER_TRENCH, i, j)) {
					m_map[i][j]->m_terrain_type = TERRAIN_WATER_DEEP;
				}
			}
		}
    }

    sint32 ocount;
    sint32 searching = TRUE;
    MapPoint pos, tmp, n, e, s, w, ne, nw, sw, se;

	while (searching) {
		searching = FALSE;
		for (i=0; i<m_size.x; i++) {
			for (j=0; j<m_size.y; j++) {
				if  (m_map[i][j]->m_terrain_type == TERRAIN_WATER_RIFT) {
					pos.Set(i, j);
					ocount=0;

					if(pos.GetNeighborPosition(SOUTHWEST, sw))
						if (GetCell(sw)->m_terrain_type == TERRAIN_WATER_RIFT) ocount++;

					if(pos.GetNeighborPosition(SOUTHEAST, se))
						if (GetCell(se)->m_terrain_type == TERRAIN_WATER_RIFT) ocount++;

					if(pos.GetNeighborPosition(NORTHWEST, nw))
						if (GetCell(nw)->m_terrain_type == TERRAIN_WATER_RIFT) ocount++;

					if(pos.GetNeighborPosition(NORTHEAST, ne))
						if (GetCell(ne)->m_terrain_type == TERRAIN_WATER_RIFT) ocount++;

					if (ocount == 0) {
						m_map[i][j]->m_terrain_type = TERRAIN_WATER_DEEP;
						searching = TRUE;
						continue;
					}

					if(pos.GetNeighborPosition(NORTH, n)) {
						if (GetCell(n)->m_terrain_type == TERRAIN_WATER_RIFT) {
							if ((GetCell(ne)->m_terrain_type == TERRAIN_WATER_RIFT) &&
								(GetCell(nw)->m_terrain_type == TERRAIN_WATER_RIFT)) {
								m_map[i][j]->m_terrain_type = TERRAIN_WATER_DEEP;
								searching = TRUE;
								continue;
							}
						}
					}
					if(pos.GetNeighborPosition(SOUTH, s)) {
						if (GetCell(s)->m_terrain_type == TERRAIN_WATER_RIFT) {
							if ((GetCell(se)->m_terrain_type == TERRAIN_WATER_RIFT) &&
								(GetCell(sw)->m_terrain_type == TERRAIN_WATER_RIFT)) {
								m_map[i][j]->m_terrain_type = TERRAIN_WATER_DEEP;
								searching = TRUE;
								continue;
							}
						}
					}
					if(pos.GetNeighborPosition(EAST, e)) {
						if (GetCell(e)->m_terrain_type == TERRAIN_WATER_RIFT) {
							if ((GetCell(ne)->m_terrain_type == TERRAIN_WATER_RIFT) &&
								(GetCell(se)->m_terrain_type == TERRAIN_WATER_RIFT)) {
								m_map[i][j]->m_terrain_type = TERRAIN_WATER_DEEP;
								searching = TRUE;
								continue;
							}
						}
					}
					if(pos.GetNeighborPosition(WEST, w)) {
						if (GetCell(w)->m_terrain_type == TERRAIN_WATER_RIFT) {
							if ((GetCell(sw)->m_terrain_type == TERRAIN_WATER_RIFT) &&
								(GetCell(nw)->m_terrain_type == TERRAIN_WATER_RIFT)) {
								m_map[i][j]->m_terrain_type = TERRAIN_WATER_DEEP;
								searching = TRUE;
								continue;
							}
						}
					}
				}
			}
		}
	}
}

extern uint16 myRGB(sint32 r,  sint32 g, sint32 b);








void World::CalcRawScore(const sint32 x, const sint32 y, float &score)
{
    if (!IsLand(x, y)) {
       score = 0.0;
    } else {
		score = (float)m_map[x][y]->GetScore();
#if 0
       if (m_map[x][y]->m_terrain_type == TERRAIN_GRASSLAND)
            score += 1.0;

	   if(m_map[x][y]->m_terrain_type == TERRAIN_FOREST ||
		  m_map[x][y]->m_terrain_type == TERRAIN_JUNGLE)
		   score += 1.0;

	   if(m_map[x][y]->m_terrain_type == TERRAIN_PLAINS)
		   score += 0.5;

       if (IsGood(x,y, 0))
            score += 5.0;

       if (IsRiver(x,y))
            score += 1.5;
#endif
    }
}

sint32 s_actualMinContinentStartSize = 0;

void World::CalcCumScore(sint32 d, const sint32 x, const sint32 y,
    float &cum_score, float **raw_score)
{
	sint32 numCounted[TERRAIN_MAX];
	for(sint32 i = 0; i < TERRAIN_MAX; i++) {
		numCounted[i] = 0;
	}
	sint32 maxToCount = g_theConstDB->Get(0)->GetMaxSameTiles();

	MapPoint pos(x, y);
	sint16 cont;
	bool is_land;
	GetContinent(pos, cont, is_land);
    cum_score   = 0.0;
	if (is_land &&
        (m_land_size->Access(cont) >= s_actualMinContinentStartSize)
       )
    {
		MapPoint        start   (x, y);
        RadiusIterator  it      (start, d);

		for (it.Start(); !it.End(); it.Next())
        {
            MapPoint pos     = it.Pos();
			Cell *cell = m_map[pos.x][pos.y];
			if ((numCounted[cell->m_terrain_type] < maxToCount) ||
			    IsRiver(pos) || IsGood(pos)
               )
            {
				cum_score += raw_score[pos.x][pos.y];
				numCounted[cell->m_terrain_type]++;
			}
		}
	}
	m_map[x][y]->m_color = sint32(cum_score);
}

BOOL World::FindMaxCumScore(sint32 d, float **cum_score, sint32 &maxx, sint32 &maxy,
							MapPoint player_start[k_MAX_PLAYERS], sint32 index,
							sint32 minDist, sint32 maxDist,
							BOOL ignoreTutorialRules)

{
    float max = -1.0;
    sint32 searching;
	MapPoint chk;
	sint32 s;
	sint32 badx = 0, bady = 0;
	float badmax = -1.0;

    maxx = 0;
    maxy = 0;
    searching = TRUE;
	sint32 topY = 0, botY = m_size.y;
	if(!m_isYwrap) {
		topY += 4;
		botY -= 4;
	}
	sint16 playerContinent;
	if(index > 0) {
		bool is_land;
		GetContinent(player_start[0], playerContinent, is_land);
	} else {
		playerContinent = 0;
	}
	for (chk.x=0; chk.x<m_size.x; chk.x++) {
		for (chk.y=sint16(topY); chk.y<botY; chk.y++) {
			BOOL dontUse = FALSE;
			sint32 minrel = maxDist;

			if(g_theProfileDB->IsTutorialAdvice() && !ignoreTutorialRules && index != 0) {
				sint16 cont;
				bool is_land;
				GetContinent(chk, cont, is_land);
				if(is_land && cont == playerContinent)
					continue;
			}

			for(s = 0; s < index; s++) {
				sint32 dist = player_start[s].NormalizedDistance(chk);
				if(dist < minrel) {
					minrel = dist;
				}
				if(dist < minDist) {
					dontUse = TRUE;
				}
				if(index == 1 && !g_network.IsNetworkLaunch() && !g_network.IsActive()) {
					if(dist > maxDist) {
						dontUse = TRUE;
					}
				} else {
					if(dist < maxDist) {
						dontUse = TRUE;
					}
				}
			}
			if(dontUse) {
				if(minrel >= minDist) {
					if(badmax < cum_score[chk.x][chk.y]) {
						badmax = cum_score[chk.x][chk.y];
						badx = chk.x;
						bady = chk.y;
					}
				}

				continue;
			}
            if (max < cum_score[chk.x][chk.y]) {
                max = cum_score[chk.x][chk.y];
                maxx = chk.x;
                maxy = chk.y;
                searching = FALSE;
            }
        }
    }

	if(max < 0 && badmax > 0) {
		max = badmax;
		maxx = badx;
		maxy = bady;
		searching = FALSE;
	}
	if(max < 0.00001) {

		searching = TRUE;
	}
	return !searching;
}

#if 0
// Not a clue what this - apparenly unused - function is supposed to do.
// Just replaced the flawed iterator with a working one.
void World::FlattenCumScore(sint32 d, float **cum_score,
    const sint32 maxx, const sint32 maxy)

{
    MapPoint    start(maxx, maxy);

    float s;
    float r = 2.0f * float(d) + 1.0f;   // unused?

	SquareIterator  it  (start, d);
    for (it.Start(); !it.End(); it.Next())
    {
        MapPoint pos = it.Pos();
		sint32 dist = start.NormalizedDistance(pos);
		s = float(d - dist) / (float(d) +1.0f);   // unused?
		cum_score[pos.x][pos.y] = -50000.0f;
    }

    cum_score[start.x][start.y] = -60000.0f;
}
#endif

uint16 myRGB(sint32 r,  sint32 g, sint32 b)
{
    return static_cast<uint16>
            (   ((r & 0x1f) << 11)
              | ((g & 0x1f) <<  5)
              | ((b & 0x1f))
            );
}

void World::FindPlayerStart(MapPoint player_start[k_MAX_PLAYERS],
							sint32 &numStartsFound,
							BOOL ignoreTutorialRules,
							sint32 player_start_score[k_MAX_PLAYERS])

{
    sint32 x, y, i, j;
    sint32 maxx, maxy;

	sint32 maxContinentSize = 0;
	for(i = 0; i < m_land_size->Num(); i++)
    {
        maxContinentSize = std::max(m_land_size->Access(i), maxContinentSize);
	}

	if (maxContinentSize < g_theConstDB->Get(0)->GetMinContinentStartSize()) {
		s_actualMinContinentStartSize = 2;
	} else {
		s_actualMinContinentStartSize = g_theConstDB->Get(0)->GetMinContinentStartSize();
	}

    float ** raw_score = new float*[m_size.x];
    float ** cum_score = new float*[m_size.x];

    for (x=0; x<m_size.x; x++) {
        raw_score[x] = new float [m_size.y];
        cum_score[x] = new float [m_size.y];
        for (y=0; y<m_size.y; y++) {
            CalcRawScore(x, y, raw_score[x][y]);
        }
    }

    sint32 d=2;

    for (x=0; x<m_size.x; x++) {
        for (y=0; y<m_size.y; y++) {
            CalcCumScore(d, x, y, cum_score[x][y], raw_score);
        }
    }

    for (x=0; x<m_size.x; x++) {
        for (y=0; y<m_size.y; y++) {
             if (!IsLand(x, y)) {
                  cum_score[x][y] = -100000.0;
             }
        }
    }

    sint32 maxSize = std::max(m_size.x, m_size.y);
	double const someNumber =
		sqrt(static_cast<double>(maxSize * maxSize) /
			 (2 * g_theProfileDB->GetNPlayers())
			);
	sint32 minDistance = sint32(g_theConstDB->Get(0)->GetMinStartDistanceCoefficient() * someNumber);
	sint32 maxDistance = sint32(g_theConstDB->Get(0)->GetMaxStartDistanceCoefficient() * someNumber);
	if(minDistance < g_theConstDB->Get(0)->GetMinAbsoluteStartDistance())
		minDistance = g_theConstDB->Get(0)->GetMinAbsoluteStartDistance();
	if(maxDistance < minDistance)
		maxDistance = minDistance;

    double third = std::min(m_size.x, m_size.y) * 0.33;
    if (third < minDistance) {
        minDistance = (sint32)third;
    }


    if (15 < g_theProfileDB->GetNPlayers()) {
        if (8 < minDistance)  {
            minDistance = 8;
        }
    } else if (9 < g_theProfileDB->GetNPlayers()) {
        if (12 < minDistance)  {
            minDistance = 12;
        }
    }

    d = g_theProfileDB->SetupRadius() * 2 - 1;
	numStartsFound = 0;
    for (i=0; i<k_MAX_PLAYERS; i++) {
        if(!FindMaxCumScore(d, cum_score, maxx, maxy,
							player_start, i,
							minDistance, maxDistance,
							ignoreTutorialRules)) {
			numStartsFound = i - 1;
			player_start[i].Set(-1,-1);
			break;
		}
		player_start_score[i] = sint32(cum_score[maxx][maxy]);
        player_start[i].Set(maxx, maxy);
		numStartsFound++;

    }

    for (i=0; i<numStartsFound; i++) {
        for (j=i+1; j<numStartsFound; j++) {
            if (player_start[i] == player_start[j]) {
                c3errors_FatalDialog("WldGen", "Duplicate player start positions %d %d", i, j);
            }
        }
    }

    for (x=0; x<m_size.x; x++) {
        delete[] cum_score[x];
        delete[] raw_score[x];
    }
    delete[] cum_score;
    delete[] raw_score;
}

double World::CalcTerrainFreightCost(const MapPoint &pos)
{
	return AccessCell(pos)->CalcTerrainFreightCost();
}

void World::SetAllMoveCost()
{
	MapPoint pos;

	for(pos.x = 0; pos.x < m_size.x; pos.x++)
	{
		for(pos.y = 0; pos.y < m_size.y; pos.y++)
		{
			Cell *cell = GetCell(pos);
			cell->CalcTerrainMoveCost();
			cell->CalcMovementType();
		}
	}
}

void World::ResetAllTmpFutureMoveCosts()
{
	MapPoint pos;

	for(pos.x = 0; pos.x < m_size.x; pos.x++)
	{
		for(pos.y = 0; pos.y < m_size.y; pos.y++)
		{
			GetCell(pos)->ResetTmpFutureMoveCosts();
		}
	}
}

void World::MapDump(MBCHAR *mapName, sint8 *mapData, sint16 width, sint16 height)
{
#ifdef DUMP_TERRAIN_HEIGHT_MAPS
	int y_scale = 2;
	int x_scale = 4;

	int *mapColor = new int[(width*x_scale) * (height*y_scale)];

	for (int y=0; y<height*y_scale; y+=y_scale) {
		for (int a=0; a<y_scale; a++) {
			for (int x=0; x<width*x_scale; x+=x_scale) {
				for (int b=0; b<x_scale; b++) {
					mapColor[((y+a)*(width*x_scale))+(x+b)] =
						mapData[(((height-1)-(y/y_scale))*width)+(x/x_scale)] + 127;
				}
			}
		}
	}

	bmp_write (mapName, width*x_scale, height*y_scale, mapColor, mapColor, mapColor);

	delete [] mapColor;
#endif
}

void World::TerrainDump (MBCHAR *mapName, sint8 *mapData, sint16 width,
						 sint16 height, sint32 waterLevel,
						 sint32 mountainLevel, sint32 hillLevel)
{
#ifdef DUMP_TERRAIN_HEIGHT_MAPS
	int y_scale = 2;
	int x_scale = 4;

	int *mapColor = new int[(width*x_scale) * (height*y_scale)];

	for (int y=0; y<height*y_scale; y+=y_scale) {
		for (int a=0; a<y_scale; a++) {
			for (int x=0; x<width*x_scale; x+=x_scale) {
				for (int b=0; b<x_scale; b++) {
					if (mapData[(((height-1)-(y/y_scale))*width)+(x/x_scale)] > mountainLevel)
						mapColor[((y+a)*(width*x_scale))+(x+b)] = 255;
					else if (mapData[(((height-1)-(y/y_scale))*width)+(x/x_scale)] > hillLevel)
						mapColor[((y+a)*(width*x_scale))+(x+b)] = 194;
					else if (mapData[(((height-1)-(y/y_scale))*width)+(x/x_scale)] > waterLevel)
						mapColor[((y+a)*(width*x_scale))+(x+b)] = 128;
					else
						mapColor[((y+a)*(width*x_scale))+(x+b)] = 0;
				}
			}
		}
	}

	bmp_write (mapName, width*x_scale, height*y_scale, mapColor, mapColor, mapColor);

	delete [] mapColor;
#endif
}

void World::Dump(FILE *fout)
{
#if 0
   sint32 i, j, k;

   fprintf (fout, "dsafdsfdsfddfdsfdddddd\n");
   fprintf (fout, "F - forest\n");
   fprintf (fout, "p - plains\n");
   fprintf (fout, "t - tundra\n");
   fprintf (fout, "G - grassland\n");
   fprintf (fout, "d - deasert\n");
   fprintf (fout, "S - swamp\n");
   fprintf (fout, "j - jungle\n");
   fprintf (fout, "h - hill\n");
   fprintf (fout, "g - glacier\n");
   fprintf (fout, ". - shallow water\n");
   fprintf (fout, "; - deep water\n");

   fprintf (fout, "u - water rift\n");
   fprintf (fout, "# - water trench\n");
   fprintf (fout, "X - water volcano\n");
   fprintf (fout, "/ - water current\n");
   fprintf (fout, "? - error unknown tile\n");
   fprintf (fout, "+ - resources");
   fprintf (fout, "* - river");
   fprintf (fout, "XX - g_player start point\n");

   sint32 found, u;
   for (i=0; i<m_size.y; i++){

      if (i % 2)
         fprintf (fout, " ");

      for (j=0; j<m_size.x; j++) {
         k = sint32(m_size.y - ceil(i/2) + j) % m_size.x;

         found = FALSE;
         for (u=0; u<8; u++) {
            if ((m_player_start_list[u][0] == k) && (m_player_start_list[u][1] == i))  {
               found = TRUE;
            }
         }

         if (found) {
            fprintf (fout, "XX");
         } else {

            if (IsGood(k,i, 0)) {
               fprintf(fout, "+");
            } else {

               if (IsRiver(k,i)) {
                  fprintf (fout, "*");
               } else {

                  switch (m_map[k][i]->m_terrain_type) {
                  case TERRAIN_NULL:
                     fprintf (fout, "@");
                     break;

                  case TERRAIN_FOREST:
                     fprintf (fout , "F");
                     break;
                  case TERRAIN_PLAINS:
                     fprintf (fout, "p");
                     break;
                  case TERRAIN_TUNDRA:
                     fprintf (fout, "t");
                     break;
                  case TERRAIN_GRASSLAND:
                     fprintf (fout, "G");
                     break;
                  case TERRAIN_DESERT:
                     fprintf (fout, "d");
                     break;
                  case TERRAIN_SWAMP:
                     fprintf (fout, "S");
                     break;
                  case TERRAIN_JUNGLE:
                     fprintf (fout, "J");
                     break;
                  case TERRAIN_MOUNTAIN:
                     fprintf (fout, "M");
                     break;
                  case TERRAIN_HILL:
                     fprintf (fout, "h");
                     break;
                  case TERRAIN_GLACIER:
                     fprintf (fout, "g");
                     break;

                  case TERRAIN_WATER_SHALLOW:
                     fprintf (fout, ".");
                     break;
                  case TERRAIN_WATER_DEEP:
                     if (IsCurrent(k,i)) {
                        fprintf (fout, "/");
                     } else {
                        fprintf (fout, ";");
                     }
                     break;
                  case TERRAIN_WATER_RIFT:
                     fprintf (fout, "u");
                     break;
                  case TERRAIN_WATER_TRENCH:
                     fprintf (fout, "#");
                     break;
                  case TERRAIN_WATER_VOLCANO:
                     fprintf (fout, "v");
                     break;
                  default:
                     fprintf (fout, "%d", m_map[j][i]->m_terrain_type);
                     break;
                  }

               }
            }
            fprintf(fout, " ");
         }

      }
      fprintf (fout, "\n");
   }

   sint32 count[17];
   double total1=0.0f, total2=0.0f, total3=0.0f;
   for (i=0; i<17; i++) {
      count[i] = 0;
   }

   sint32 mc[7][17];
   double mtot[7];

   for (i=0; i<7; i++) {
      mtot[i] = 0.0f;
      for (j=0; j<17; j++) {
         mc[i][j] = 0;
      }
   }

   sint32 t;
   for (i=0; i<m_size.x; i++) {
      for (j=0; j<m_size.y; j++) {
         t = m_map[i][j]->m_terrain_type;
         Assert(0 <= t);
         Assert(t < 17);
         count[t]++;
         total1 += 1.0f;
         if (t <= TERRAIN_HILL) {
            total2 += 1.0f;
         } else {
            total3 += 1.0f;
         }

         g_theConstDB->y2meridian(j, k);
         if (t <= TERRAIN_HILL) {
            mtot[k] += 1.0f;
         }

         mc[k][t]++;

      }
   }

   total1 = total1/100.0f;
   total2 = total2/100.0f;
   total3 = total3/100.0f;

   for (i=0; i<7; i++) {
      mtot[i] = mtot[i]/100.0f;
   }

   fprintf (fout, "\n");
   fprintf (fout, "                      tot  totp  landp   npol   mil   dry   eqa   dry   mil   spol\n");
   fprintf (fout, "TERRAIN_NULL          %4d %5.2f %5.2f ", count[0], float(count[0])/total1, float(count[0])/total2);

   for (i=0; i<7; i++) {
      fprintf (fout, " %5.2f", mc[i][0]/mtot[i]);
   }
   fprintf (fout, "\n");

   fprintf (fout, "TERRAIN_GLACIER       %4d %5.2f %5.2f ", count[4], float(count[4])/total1, float(count[4])/total2);
   for (i=0; i<7; i++) {
      fprintf (fout, " %5.2f", mc[i][1]/mtot[i]);
   }
   fprintf (fout, "\n");

   fprintf (fout, "\n");
   fprintf (fout, "TERRAIN_FOREST        %4d %5.2f %5.2f ", count[1], float(count[1])/total1, float(count[1])/total2);
   for (i=0; i<7; i++) {
      fprintf (fout, " %5.2f", mc[i][2]/mtot[i]);
   }
   fprintf (fout, "\n");

   fprintf (fout, "TERRAIN_PLAINS        %4d %5.2f %5.2f ", count[2], float(count[2])/total1, float(count[2])/total2);
   for (i=0; i<7; i++) {
      fprintf (fout, " %5.2f", mc[i][3]/mtot[i]);
   }
   fprintf (fout, "\n");

   fprintf (fout, "TERRAIN_TUNDRA        %4d %5.2f %5.2f ", count[3], float(count[3])/total1, float(count[3])/total2);
   for (i=0; i<7; i++) {
      fprintf (fout, " %5.2f", mc[i][4]/mtot[i]);
   }
   fprintf (fout, "\n");

   fprintf (fout, "TERRAIN_GRASSLAND     %4d %5.2f %5.2f ", count[5], float(count[5])/total1, float(count[5])/total2);
   for (i=0; i<7; i++) {
      fprintf (fout, " %5.2f", mc[i][5]/mtot[i]);
   }
   fprintf (fout, "\n");

   fprintf (fout, "TERRAIN_DESERT        %4d %5.2f %5.2f ", count[6], float(count[6])/total1, float(count[6])/total2);
   for (i=0; i<7; i++) {
      fprintf (fout, " %5.2f", mc[i][6]/mtot[i]);
   }
   fprintf (fout, "\n");

   fprintf (fout, "TERRAIN_SWAMP         %4d %5.2f %5.2f ", count[7], float(count[7])/total1, float(count[7])/total2);
   for (i=0; i<7; i++) {
      fprintf (fout, " %5.2f", mc[i][7]/mtot[i]);
   }
   fprintf (fout, "\n");

   fprintf (fout, "TERRAIN_JUNGLE        %4d %5.2f %5.2f ", count[8], float(count[8])/total1, float(count[8])/total2);
   for (i=0; i<7; i++) {
      fprintf (fout, " %5.2f", mc[i][8]/mtot[i]);
   }
   fprintf (fout, "\n");

   fprintf (fout, "\n");
   fprintf (fout, "TERRAIN_MOUNTAIN      %4d %5.2f %5.2f ", count[9], float(count[9])/total1, float(count[9])/total2);
   for (i=0; i<7; i++) {
      fprintf (fout, " %5.2f", mc[i][9]/mtot[i]);
   }
   fprintf (fout, "\n");

   fprintf (fout, "TERRAIN_HILL          %4d %5.2f %5.2f ", count[10], float(count[10])/total1, float(count[10])/total2);
   for (i=0; i<7; i++) {
      fprintf (fout, " %5.2f", mc[i][10]/mtot[i]);
   }
   fprintf (fout, "\n");
   fprintf (fout, "\n");
   fprintf (fout, "TERRAIN_WATER_SHALLOW %4d %5.2f %5.2f\n", count[11], float(count[11])/total1, float(count[11])/total3);
   fprintf (fout, "TERRAIN_WATER_DEEP    %4d %5.2f %5.2f\n", count[12], float(count[12])/total1, float(count[12])/total3);
   fprintf (fout, "TERRAIN_WATER_RIFT    %4d %5.2f %5.2f\n", count[13], float(count[13])/total1, float(count[13])/total3);
   fprintf (fout, "TERRAIN_WATER_TRENCH  %4d %5.2f %5.2f\n", count[14], float(count[14])/total1, float(count[14])/total3);
   fprintf (fout, "TERRAIN_WATER_VOLCANO %4d %5.2f %5.2f\n", count[15], float(count[15])/total1, float(count[15])/total3);
   fprintf (fout, "TERRAIN_WATER_CURRENT %4d %5.2f %5.2f\n", count[16], float(count[16])/total1, float(count[16])/total3);
#endif
}

void World::Mdump(FILE *fout)
{
#if 0
	sint32 i, j, k;
	sint32 found, u;

	Assert(0 < m_size.x);
	for (i=0; i<m_size.y; i++){

		if (i & 0x01)
			fprintf (fout, " ");

		for (j=0; j<m_size.x; j++) {
			k = sint32(m_size.y - ceil(i/2) + j) % m_size.x;

			found = FALSE;
			for (u=0; u<8; u++) {
				if ((m_player_start_list[u][0] == k) && (m_player_start_list[u][1] == i))  {
					found = TRUE;
				}
			}

			if (found) {
				fprintf (fout, "XX");
			} else {

				if (m_map[k][i]->IsAnyUnitInCell()) {
					fprintf (fout, "@");
				} else {
					if (IsGood(k,i, 0)) {
						fprintf(fout, "+");
					} else {

						if (IsRiver(k,i)) {
							fprintf (fout, "*");
						} else {

							switch (m_map[k][i]->m_terrain_type) {
							case TERRAIN_NULL:
								fprintf (fout, "@");
								break;

							case TERRAIN_FOREST:
								fprintf (fout , "F");
								break;
							case TERRAIN_PLAINS:
								fprintf (fout, "p");
								break;
							case TERRAIN_TUNDRA:
								fprintf (fout, "t");
								break;
							case TERRAIN_GRASSLAND:
								fprintf (fout, "G");
								break;
							case TERRAIN_DESERT:
								fprintf (fout, "d");
								break;
							case TERRAIN_SWAMP:
								fprintf (fout, "S");
								break;
							case TERRAIN_JUNGLE:
								fprintf (fout, "J");
								break;
							case TERRAIN_MOUNTAIN:
								fprintf (fout, "M");
								break;
							case TERRAIN_HILL:
								fprintf (fout, "h");
								break;
							case TERRAIN_GLACIER:
								fprintf (fout, "g");
								break;

							case TERRAIN_WATER_SHALLOW:
								fprintf (fout, ".");
								break;
							case TERRAIN_WATER_DEEP:
								if (IsCurrent(k,i)) {
									fprintf (fout, "/");
								} else {
									fprintf (fout, ";");
								}
								break;
							case TERRAIN_WATER_RIFT:
								fprintf (fout, "u");
								break;
							case TERRAIN_WATER_TRENCH:
								fprintf (fout, "#");
								break;
							case TERRAIN_WATER_VOLCANO:
								fprintf (fout, "v");
								break;
							case TERRAIN_WATER_BEACH :
								fprintf (fout, "B");
								break;
							case TERRAIN_WATER_SHELF :
								fprintf (fout, "S");
								break;
							case TERRAIN_DEAD :
								fprintf (fout, "D");
								break;
							default:
								fprintf (fout, "%d", m_map[j][i]->m_terrain_type);
								break;
							}

						}
					}
				}
				fprintf(fout, " ");

			}
		}
		fprintf (fout, "\n");
	}
#endif
}

// Need a global to fix number of goods.
sint32 g_numGoods = 0;

//----------------------------------------------------------------------------
//
// Name       : World::Serialize
//
// Description: Store/Load CityData
//
// Parameters : CivArchive &archive       :
//
// Globals    : -
//
// Returns    : -
//
// Remark(s)  : -
//
//----------------------------------------------------------------------------
void World::Serialize(CivArchive &archive)
{
#define WORLD_MAGIC 0x48A8A848

	CHECKSERIALIZE

	if (archive.IsStoring())
	{
		archive.PerformMagic(WORLD_MAGIC);
		archive << m_isXwrap;
		archive << m_isYwrap;
		archive.PutSINT32(m_continents_are_numbered);
		archive << m_water_continent_max;
		archive << m_land_continent_max;
		m_size.Serialize(archive) ;

		sint32 const    w   = GetWidth();
		sint32 const    h   = GetHeight();
		sint32 const	len = w * h;
		for (sint32 i = 0; i < len; ++i)
        {
			m_tileInfoStorage[i].Serialize(archive);
		}

		for (sint32 x = 0; x < w; ++x)
        {
			for (sint32 y = 0; y < h; ++y)
            {
				GetCell(x, y)->Serialize(archive);
			}
		}

		archive << m_num_civ_starts;
		archive.Store((uint8*)m_civ_starts, sizeof(m_civ_starts));

		archive << g_theResourceDB->NumRecords();
		archive.Store((uint8*)m_goodValue, sizeof(double) * g_theResourceDB->NumRecords());
	}
    else
    {
		archive.TestMagic(WORLD_MAGIC);

		FreeMap();

		archive >> m_isXwrap;
		archive >> m_isYwrap;
		m_continents_are_numbered = archive.GetSINT32();
		archive >> m_water_continent_max;
		archive >> m_land_continent_max;

		m_size.Serialize(archive) ;

		AllocateMap();

		sint32 const    w   = GetWidth();
		sint32 const    h   = GetHeight();
		sint32 const    len = w * h;

		for (sint32 i = 0; i < len; i++)
        {
			m_tileInfoStorage[i].Serialize(archive);
		}

		for (sint32 x = 0; x < w; ++x)
        {
			for (sint32 y = 0; y < h; ++y)
            {
				MapPoint	pos(x,y);
				Cell *      theCell = GetCell(x, y);
				theCell->Serialize(archive);
			}
		}

		archive >> m_num_civ_starts;
		archive.Load((uint8*)m_civ_starts, sizeof(m_civ_starts));

		archive >> g_numGoods;
		if(g_numGoods == g_theResourceDB->NumRecords()){
			m_goodValue = new double[g_numGoods];
			archive.Load((uint8 *)m_goodValue, sizeof(double) * g_numGoods);
		}
		else{ // Handle sized up good database
			// Remove good values from archieve and recalculate them
			double *tmpGoodValue = new double[g_numGoods];
			archive.Load((uint8 *)tmpGoodValue, sizeof(double) * g_numGoods);
			delete[] tmpGoodValue;
			ComputeGoodsValues();
		}
	}
}

void World::SerializeJustMap(CivArchive &archive)
{
	Serialize(archive);
	if(!archive.IsStoring()) {
		sint32 x, y;
		for(x = 0; x < m_size.x; x++) {
			for(y = 0; y < m_size.y; y++) {
				m_map[x][y]->ClearUnitsNStuff();
			}
		}
	}
}













uint32 World_World_GetVersion(void)
{
	return (k_WORLD_VERSION_MAJOR<<16 | k_WORLD_VERSION_MINOR) ;
}

void World::AllocateTileInfoStorage(void)
{
	sint32			width = m_size.x;
	sint32			height = m_size.y;

	delete [] m_tileInfoStorage;
	m_tileInfoStorage = new TileInfo[width*height];
}

void World::DisposeTileInfoStorage(void)
{
	delete [] m_tileInfoStorage;
	m_tileInfoStorage = NULL;
}

TileInfo *World::GetTileInfoStoragePtr(const MapPoint &pos)
{
	Assert(m_tileInfoStorage);
	if (!m_tileInfoStorage)
	{
		return NULL;
	}

	sint32 const    width = m_size.x;
	return &m_tileInfoStorage[pos.x + pos.y * width];
}

void World::GenerateGoodyHuts()
{
	if(g_theProfileDB->IsTutorialAdvice()) {

		return;
	}

	if (g_theProfileDB->IsNoGoodyHuts()) {

		return;
	}

	MapPoint hutBoxSize;
	hutBoxSize.x = sint16(g_theConstDB->Get(0)->GetRuinsBoxWidth());
	hutBoxSize.y = sint16(g_theConstDB->Get(0)->GetRuinsBoxHeight());
	double hutChance = g_theConstDB->Get(0)->GetRuinsChancePerBox();

	sint32 x, y;
	sint32 w = m_size.x / hutBoxSize.x;
	sint32 h = m_size.y / hutBoxSize.y;

	for(x = 0; x < w; x++) {
		for(y = 0; y < h; y++) {
			if(g_rand->Next(100) < sint32(hutChance * 100.0)) {
				sint32 coffx = g_rand->Next(hutBoxSize.x);
				sint32 coffy = g_rand->Next(hutBoxSize.y);
				sint32 cellx = x * hutBoxSize.x + coffx;
				sint32 celly = y * hutBoxSize.y + coffy;
				Cell * cell = GetCell(cellx, celly);
				if(cell->m_env & (k_BIT_MOVEMENT_TYPE_LAND)) {

					cell->m_jabba = new GoodyHut();

				}
			}
		}
	}
}

IMapGenerator *World::LoadMapPlugin(sint32 pass)
{
#ifndef USE_COM_REPLACEMENT
	HINSTANCE plugin;
#else
	void* plugin;
#endif
	const char *name = g_theProfileDB->MapPluginName(pass);
	if(stricmp(name, "none") == 0)
		return NULL;
#ifndef USE_COM_REPLACEMENT
	plugin = LoadLibrary(name);
#else
	char* name_so = strdup(name);
	char *p = strrchr(name_so, '.');
	if(p) strcpy(p, ".so");	// quickly change .dll to .so
	//printf("DLL, open %s => %s\n", name, CI_FixName(name_so));
	plugin = dlopen(CI_FixName(name_so), RTLD_LAZY);
	free(name_so);
#endif
	if(plugin == NULL) {
#ifndef USE_COM_REPLACEMENT
		c3errors_ErrorDialog("Map Generator", "Could not load library %s, using builtin map generator", name);
#else
		fprintf(stderr, "Could not load library %s, using builtin map generator: %s", name, dlerror());
#endif
		return NULL;
	}
#ifndef USE_COM_REPLACEMENT
	CreateMapGenerator creator = (CreateMapGenerator)GetProcAddress(plugin, "CoCreateMapGenerator");
#else
	CreateMapGenerator creator = (CreateMapGenerator)dlsym(plugin, "CoCreateMapGenerator");
#endif
	if(creator == NULL) {
#ifndef USE_COM_REPLACEMENT
		FreeLibrary(plugin);
#else
		dlclose(plugin);
#endif
		c3errors_ErrorDialog("Map Generator", "Plugin %s is not a valid map generator", name);
		return NULL;
	}
#ifndef USE_COM_REPLACEMENT
	IUnknown *unknown;
	if(creator(&unknown) != S_OK) {
#else
	IMapGenerator *generator = creator();
	if (!generator) {
#endif
		c3errors_ErrorDialog("Map Generator", "Plugin creation failed");
#ifndef USE_COM_REPLACEMENT
		FreeLibrary(plugin);
#else
		dlclose(plugin);
#endif
		return NULL;
	}
#ifndef USE_COM_REPLACEMENT
	IMapGenerator *generator;
	if(unknown->QueryInterface(CLSID_MapGenerator, (void **)&generator) != S_OK) {
		c3errors_ErrorDialog("Map Generator", "Plugin does not have map generator interface");
		unknown->Release();
		FreeLibrary(plugin);
		return NULL;
	}
#endif
	m_current_plugin = plugin;
#ifndef USE_COM_REPLACEMENT
	unknown->Release();
#endif
	return generator;
}

void World::FreeMapPlugin()
{
#ifndef USE_COM_REPLACEMENT
	FreeLibrary(m_current_plugin);
#else
	dlclose(m_current_plugin);
#endif
	m_current_plugin = NULL;
}

void World::GetHeightMap(IMapGenerator *mapgen,
						 sint8 *&map,
						 const double *settings, sint32 numSettings)
{
	sint32 totalsize = m_size.x * m_size.y;
	for(sint32 i = 0; i < totalsize; i++) {
		map[i] = 0;
	}
	mapgen->Generate(map, m_size.x, m_size.y, s_randomGenerator,
					 settings, numSettings);
}

void World::GetHistogram(sint8 *&map,
						 sint32 *&histogram)
{
	for(sint32 h = -128; h < 128; h++) {
		histogram[h] = 0;
	}

	sint32 y, x;
	for(y = 0; y < m_size.y; y++) {
		for(x = 0; x < m_size.x; x++) {
			histogram[map[y * m_size.x + x]]++;
		}
	}
}

void World::GetMapAndHistogram(IMapGenerator *mapgen,
							   sint8 *&map,
							   sint32 *&histogram,
							   const double *settings, sint32 numSettings)
{
	GetHeightMap(mapgen, map, settings, numSettings);
	GetHistogram(map, histogram);
}

void World::FlatToIso(const MapPoint &flat, MapPoint &iso)
{
	iso.y = flat.y;
	iso.x = flat.x - flat.y / 2;
	while (iso.x < 0)
    {
		iso.x += m_size.x;
    }
}


bool World::IsNextToOldRiver
(
    const MapPoint &    newpoint,
    const MapPoint &    lastpoint
) const
{
    sint32 left = (newpoint.y & 1) ? newpoint.x : (newpoint.x - 1);

    for (sint32 xc = left; xc < left+2; xc++)
    {
		if(xc < 0 || xc >= m_size.x)
			continue;

		for (sint32 yc = newpoint.y-1; yc < newpoint.y + 2; yc+=2)
        {
			if(yc < 0 || yc >= m_size.y)
				continue;

			MapPoint chk(xc, yc);
			if(chk == lastpoint)
				continue;
			if(s_visited->IsPresent(chk))
				return true;
		}
	}

	return false;
}

void World::NewGenerateRivers(sint8 *map, sint8 *wetmap)
{
	s_visited = new DynamicArray<MapPoint>;
	sint32 x, y;
	sint32 maxheight;
	sint32  cellwidth   = g_theConstDB->Get(0)->GetRiverCellWidth();
	sint32  cellheight  = g_theConstDB->Get(0)->GetRiverCellHeight();
	sint32  maxx        = 0;
    sint32  maxy        = 0;

#define CHKPOINT(p) Assert(p.x >= 0 && p.x < m_size.x && p.y >= 0 && p.y < m_size.y)
#define CHKCOORD(xxx,yyy) Assert(xxx >= 0 && xxx < m_size.x && yyy >= 0 && yyy < m_size.y)

	for (sint32 xcell = 0; xcell < cellwidth; xcell++)
    {
		for (sint32 ycell = 0; ycell < cellheight; ycell++)
        {
			sint32  xsrch   = xcell * (m_size.x / cellwidth);
			sint32  ysrch   = ycell * (m_size.y / cellheight);
			sint32  xend    = (xcell + 1) * (m_size.x / cellwidth);
			sint32  yend    = (ycell + 1) * (m_size.y / cellheight);
			maxheight = -1000;
			for(; xsrch < xend; xsrch++) {
				if(xsrch >= m_size.x)
					continue;
				for(; ysrch < yend; ysrch++) {
					CHKCOORD(xsrch, ysrch);
					if(ysrch >= m_size.y)
						continue;
					if(map[ysrch * m_size.x + xsrch] > maxheight) {
						MapPoint from(xsrch, ysrch);
						MapPoint to;
						FlatToIso(from, to);
						CHKPOINT(to);
						if(IsNextToWater(to.x, to.y) || !IsLand(to) || IsRiver(to))
							continue;
						maxheight = map[ysrch * m_size.x + xsrch];
						maxx = xsrch;
						maxy = ysrch;
					}
				}
			}
			if(maxheight == -1000)
				continue;

			x = maxx;
			y = maxy;
			MapPoint from(x,y);
			MapPoint to;

			FlatToIso(from, to);

			CHKPOINT(from);
			CHKPOINT(to);
			CHKCOORD(x, y);

			if(IsRiver(to) || !IsLand(to) || IsNextToWater(to.x,to.y)) {
				continue;
			}

			SetRiver(to);
			bool atMouth = false;

			s_visited->Clear();

			while(!atMouth) {
				sint32 lowest   = 0x7fffffff;
				sint32 lowx     = 0x7fffffff;
                sint32 lowy     = 0x7fffffff;
				CHKCOORD(x, y);

                sint32 left = (y & 1) ? x : x - 1;

                for (sint32 xc = left; xc < left+2; xc++)
                {
					if(xc < 0 || xc >= m_size.x)
						continue;

					for(sint32 yc = y-1; yc < y + 2; yc+=2) {
						if(yc < 0 || yc >= m_size.y)
							continue;

						MapPoint c(xc, yc);
						MapPoint from(x,y);
						CHKPOINT(c);
						CHKPOINT(from);

						if(s_visited->IsPresent(c))
							continue;
						if(IsNextToOldRiver(c, from))
							continue;
						sint32 ax = xc;
						sint32 ay = yc;
						if(ax < 0) ax += m_size.x;
						if(ax >= m_size.x) ax -= m_size.x;
						if(ay < 0) ay += m_size.y;
						if(ay >= m_size.y) ay -= m_size.y;

						MapPoint flat(ax,ay);
						MapPoint iso;
						FlatToIso(flat,iso);

						sint32 checkheight = map[ay * m_size.x + ax];
						if(checkheight > lowest)
							continue;
						lowest = checkheight;
						lowx = ax;
                        lowy = ay;
					}
				}
				if(lowest >= 0x7fffffff) {
					lowx = x;
                    lowy = y;
				}
				MapPoint v(lowx, lowy);
				CHKPOINT(v);
				s_visited->Insert(v);
				x = lowx;
                y = lowy;
				MapPoint checkfrom(lowx, lowy);
				MapPoint checkto;
				FlatToIso(checkfrom, checkto);

				CHKPOINT(checkto);
				CHKPOINT(checkfrom);
				if(lowest < 0x7fffffff) {
					if(m_map[checkto.x][checkto.y]->m_terrain_type == TERRAIN_WATER_BEACH || IsRiver(checkto)) {
						atMouth = true;
					}
					SetRiver(checkto);
				} else {
					atMouth = true;
					m_map[checkto.x][checkto.y]->m_terrain_type = TERRAIN_WATER_BEACH;
					SetMovementType(checkto.x, checkto.y, k_MOVEMENT_TYPE_WATER | k_MOVEMENT_TYPE_SHALLOW_WATER);
				}
			}
		}
	}

	s_visited->Clear();


	for(x = 0; x < m_size.x; x++) {
		for(y = 0; y < m_size.y; y++) {
			if(!IsRiver(x,y))
				continue;
			if(IsSurroundedByWater(x,y)) {
				GetCell(x,y)->m_env &= ~(k_BIT_ENV_RIV_CUR);
			} else {
				bool nextToRiver = false;
				MapPoint cp(x,y);
                MapPoint n;
				for (sint32 d = 0; d < sint32(NOWHERE); d++) {
					if(cp.GetNeighborPosition((WORLD_DIRECTION)d, n)) {
						if(IsRiver(n.x, n.y)) {
							nextToRiver = true;
							break;
						}
					}
				}
				if(!nextToRiver) {
					GetCell(x,y)->m_env &= ~(k_BIT_ENV_RIV_CUR);
				}
			}
		}
	}

	delete s_visited;
}

void TemperatureFilter(sint8 *map, sint32 *histogram)
{
	sint32 x, y;


	MapPoint	mapSize;
	constutil_GetMapSizeMapPoint(g_theProfileDB->GetMapSize(), mapSize);


	sint32 range = g_theProfileDB->TemperatureRangeAdjust();
	sint32 val;
	sint32 dist;
	sint32 quart = mapSize.y / 4;
	sint32 bottomy;
	sint32 half = mapSize.y / 2;
	sint32 adj;

	for(y = 0; y < quart; y++) {
		dist = quart - y;
		adj = (dist * range)/ quart;
		bottomy = mapSize.y - y - 1;
		for(x = 0; x < mapSize.x; x++) {
			val = map[y * mapSize.x + x];
			histogram[val]--;
			val -= adj;
			if(val < -127)
				val = -127;
			histogram[val]++;
			map[y * mapSize.x +x] = (sint8)val;

			val = map[bottomy * mapSize.x + x];
			histogram[val]--;
			val -= adj;
			if(val < -127)
				val = -127;
			histogram[val]++;
			map[bottomy * mapSize.x + x] = (sint8)val;
		}
	}

	for(y = quart + 1; y < half; y++) {
		dist = y - quart;
		adj = (dist * range) / quart;
		bottomy = mapSize.y - y - 1;
		for(x = 0; x < mapSize.x; x++) {
			val = map[y * mapSize.x + x];
			histogram[val]--;
			val += adj;
			if(val > 127)
				val = 127;
			histogram[val]++;
			map[y * mapSize.x +x] = (sint8)val;

			val = map[bottomy * mapSize.x + x];
			histogram[val]--;
			val += adj;
			if(val > 127)
				val = 127;
			histogram[val]++;
			map[bottomy * mapSize.x + x] = (sint8)val;
		}
	}
}

void World::ClearScratch()
{
	sint32 size = m_size.x * m_size.y;
	Cell *pCell = &m_cellArray[0];
	do
	{
#if defined(_DEBUG) && defined(CELL_COLOR)
		pCell->m_color = pCell->m_search_count & 0x7fffffff;
#endif
		pCell->m_search_count = 0;
		pCell++;
	} while (--size);
}

void World::RemoveIsolatedWater()
{
	for (sint32 x = 0; x < m_size.x; x++)
    {
		for (sint32 y = 0; y < m_size.y; y++)
        {
			if(IS_SHALLOW(m_map[x][y]->m_terrain_type) &&
			   !IsNextToWaterNotDiagonals(x,y))
            {
				MapPoint n, p;
				WORLD_DIRECTION d;
				sint32 count = 0;
				do {
					sint32 dn = g_rand->Next(4);
					switch(dn) {
						case 0: d = NORTHWEST; break;
						case 1: d = NORTHEAST; break;
						case 2: d = SOUTHWEST; break;
						case 3: d = SOUTHEAST; break;
						default:
							Assert(FALSE);
							d = NORTHWEST;
							break;
					}

					p.Set(x,y);
					count++;
				} while (!p.GetNeighborPosition((WORLD_DIRECTION)d, n) && count < 10);

				if(count < 10) {
					m_map[x][y]->m_terrain_type = m_map[n.x][n.y]->m_terrain_type;
					m_map[x][y]->m_env &= ~(k_MASK_ENV_MOVEMENT_TYPE);
					m_map[x][y]->m_env |= (m_map[n.x][n.y]->m_env & k_MASK_ENV_MOVEMENT_TYPE);
				}
			}
		}
	}
}

sint32 World::GetNumStartingPositions()
{
	return m_num_civ_starts;
}

MapPoint World::GetStartingPoint(sint32 index)
{
	Assert(index >= 0);
	Assert(index < m_num_civ_starts);
	return m_civ_starts[index].point;
}

sint32 World::GetStartingPointCiv(sint32 index)
{
	Assert(index >= 0);
	Assert(index < m_num_civ_starts);
	return m_civ_starts[index].civIndex;
}

void World::SetStartingPoint(sint32 index, MapPoint &point, sint32 civ)
{
	Assert(index >= 0);
	Assert(index < m_num_civ_starts);

	m_civ_starts[index].point = point;
	m_civ_starts[index].civIndex = civ;
}

void World::ClearStartingPoints()
{
	m_num_civ_starts = 0;
}

void World::AddStartingPoint(MapPoint &point, sint32 civ)
{
	Assert(m_num_civ_starts < k_MAX_START_POINTS);
	if(m_num_civ_starts < k_MAX_START_POINTS) {
		m_civ_starts[m_num_civ_starts].point = point;
		m_civ_starts[m_num_civ_starts].civIndex = civ;
		m_num_civ_starts++;
	}
}

void World::DeleteStartingPoint(sint32 index)
{
	Assert(index >= 0);
	Assert(index < m_num_civ_starts);
	if(index >= 0 && index < m_num_civ_starts) {
		if(index < m_num_civ_starts - 1) {
			memmove(&m_civ_starts[index], &m_civ_starts[index + 1],
					sizeof(StartingPosition) * m_num_civ_starts - index - 1);
		}
		m_num_civ_starts--;
	}
}





bool World::ExportMap(MBCHAR const *filename)
{
	Assert(filename && (strlen(filename) > 0));
	if (!filename || (0 == strlen(filename))) return false;

	FILE * outfile = fopen(filename, "wt");
	Assert(outfile);
	if (!outfile) return false;

	fprintf(outfile, "%d,%d\n", m_size.x, m_size.y);

	for (sint16 y = 0; y < m_size.y; y++)
    {
		for (sint16 x = 0; x < m_size.x; x++)
        {
			MapPoint    pos(x, y);
			Cell *      cell = GetCell(pos);
			sint32	    good;

			BOOL hasHut = (cell->GetGoodyHut() != NULL);
			BOOL hasRiver = IsRiver(pos);
			BOOL hasGood = GetGood(pos, good);

			fprintf(outfile, "%d,%d,%d,%d,%ld\t",
					cell->GetTerrain(),
					hasHut,
					hasRiver,
					hasGood,
					cell->GetEnv()
                   );
		}
		fprintf(outfile, "\n");
	}

	fclose(outfile);

	return true;
}

bool World::ImportMap(MBCHAR const * filename)
{
	Assert(filename && (strlen(filename) > 0));
	if (!filename || (0 == strlen(filename))) return false;

	FILE * infile = fopen(filename, "rt");
	Assert(infile);
	// Add here a file does not exist message
	if (!infile)
	{
		const char *str = g_theStringDB->GetNameStr("str_FileDoesNotExist");
		if(str == NULL)
		{
			str = "File does not exist: ";
		}

		char buff[1024];
		sprintf(buff, "%s\n%s", str, filename);
		MessageBoxDialog::Information(buff, "FileDoesNotExist");
		return false;
	}

	sint32 width,height;
	fscanf(infile, "%d,%d\n", &width, &height);
	MapPoint size (width, height);

	if (size != m_size) {
		const char *str = g_theStringDB->GetNameStr("str_MapSizesDiffer");
		if(str == NULL)
		{
			str = "The new map does not have the same size as the loaded map.\n The map sizes must fit. Map size old/new: ";
		}

		char buff[1024];
		sprintf(buff, "%s(%i,%i)/(%i,%i)", str, m_size.x, m_size.y, size.x, size.y);
		MessageBoxDialog::Information(buff, "MapSizesDiffer");
		fclose(infile);
		return false;
	}

	sint32		x, y;

	Reset(m_size.x, m_size.y, m_isYwrap, m_isXwrap);

	for(x = 0; x < m_size.x; x++) {
		for(y = 0; y < m_size.y; y++) {
			m_map[x][y]->m_env = 0;
		}
	}

	for (y=0; y<m_size.y; y++) {
		for (x=0; x<m_size.x; x++) {

			BOOL hasHut;
			BOOL hasRiver;
			BOOL hasGood;
			sint32 terrainType;
			uint32 env;

			fscanf(infile, "%d,%d,%d,%d,%ld\t",
					&terrainType,
					&hasHut,
					&hasRiver,
					&hasGood,
					&env);







			if(terrainType < 0 || terrainType >= g_theTerrainDB->NumRecords())
				terrainType = 0;

			MapPoint	pos     (x, y);
			Cell *      cell    = GetCell(pos);
			cell->SetEnvFast(env);
			cell->SetTerrain(terrainType);

			cell->DeleteGoodyHut();
			if (hasHut) {
				cell->CreateGoodyHut();
			}

			if (hasRiver) {
				SetRiver(pos);
			}

			if (hasGood) {
				SetRandomGood(pos.x, pos.y);
			}

		}
		fscanf(infile, "\n");
	}

	fclose(infile);

	for(x = 0; x < m_size.x; x++) {
		for(y = 0; y < m_size.y; y++) {
			if(IsWater(x,y) && !IsSurroundedByWater(x,y)) {
				// ConvertToBeach(x, y, GetCell(x,y));
				GetCell(x,y)->SetTerrain(TERRAIN_WATER_BEACH);
			} else if(GetCell(x,y)->GetTerrainType() == TERRAIN_WATER_BEACH && IsSurroundedByWater(x,y)) {
				GetCell(x,y)->SetTerrain(TERRAIN_WATER_SHALLOW);
			}
		}
	}

	SetAllMoveCost();
	m_continents_are_numbered= FALSE;
	NumberContinents();
	CalcChokePoints();
	ClearScratch();

	return true;
}

#define k_SMART_SET_HIT 1
#define k_SMART_SET_POST_PROCESS 2

void World::SmartSetTerrain(const MapPoint &pos, sint32 terr, sint32 radius)
{
	ClearScratch();
	sint32 x, y;

	if(radius > 0) {
		SquareIterator it(pos, radius);
		for(it.Start(); !it.End(); it.Next()) {




			for(x = 0; x < GetXWidth(); x++) {
				for(y = 0; y < GetYHeight(); y++) {
					MapPoint pos(x,y);
					Cell *cell = GetCell(pos);
					cell->SetScratch(cell->GetScratch() & (~(k_SMART_SET_HIT)));
				}
			}

			SmartSetOneCell(it.Pos(), terr);
		}
	} else {
		SmartSetOneCell(pos, terr);
	}

	for(x = 0; x < GetXWidth(); x++) {
		for(y = 0; y < GetYHeight(); y++) {
			MapPoint pos(x,y);
			Cell *cell = GetCell(pos);
			if(cell->GetScratch() & k_SMART_SET_POST_PROCESS) {
				g_tiledMap->PostProcessTile(pos, GetTileInfo(pos));
				g_tiledMap->RedrawTile(&pos);
			}
		}
	}

}

void World::SmartSetOneCell(const MapPoint &pos, sint32 terr)
{
	Cell *  thisCell = GetCell(pos);
	Assert(thisCell);
	if(!thisCell) return;

	if(thisCell->GetScratch() & k_SMART_SET_HIT)
		return;

	thisCell->SetScratch(thisCell->GetScratch() | k_SMART_SET_HIT);

	const TerrainRecord * rec = g_theTerrainDB->Get(terr);
	Assert(rec);
	if(!rec)
		return;

	sint32 oldTerr = thisCell->GetTerrainType();

	thisCell->SetTerrain(terr);

	const TerrainRecord *neighborRec;
	if(rec->GetMovementTypeLand() || rec->GetMovementTypeMountain()) {

		RadiusIterator it(pos, 1, 2.0);
		for(it.Start(); !it.End(); it.Next()) {
			if(it.Pos() == pos) continue;

			Cell *cell = GetCell(it.Pos());

			Assert(cell);
			if(!cell) continue;

			neighborRec = g_theTerrainDB->Get(cell->GetTerrainType());
			if(!neighborRec->GetMovementTypeLand() && !neighborRec->GetMovementTypeMountain()) {
				SmartSetOneCell(it.Pos(), TERRAIN_WATER_BEACH);
			}
		}
	} else if(rec->GetMovementTypeShallowWater()) {

		RadiusIterator it(pos, 1, 2.0);
		bool nextToLand = false;
		for(it.Start(); !it.End(); it.Next()) {
			if(it.Pos() == pos) continue;

			Cell *cell = GetCell(it.Pos());
			Assert(cell);
			if(!cell) continue;

			sint32 ntype = cell->GetTerrainType();
			neighborRec = g_theTerrainDB->Get(ntype);
			if(!nextToLand && (neighborRec->GetMovementTypeLand() || neighborRec->GetMovementTypeMountain())) {
				nextToLand = true;
				thisCell->SetTerrain(TERRAIN_WATER_BEACH);
				terr = TERRAIN_WATER_BEACH;
				break;
			}
		}

		if(oldTerr != terr) {
			for(it.Start(); !it.End(); it.Next()) {
				if(it.Pos() == pos) continue;

				Cell *cell = GetCell(it.Pos());
				Assert(cell);
				if(!cell) continue;

				sint32 ntype = cell->GetTerrainType();
				neighborRec = g_theTerrainDB->Get(ntype);

				if(neighborRec->GetMovementTypeShallowWater() && oldTerr != terr) {

					if (ntype != TERRAIN_WATER_KELP && ntype != TERRAIN_WATER_REEF) {
						SmartSetOneCell(it.Pos(), TERRAIN_WATER_SHALLOW);
					}

				} else if(neighborRec->GetMovementTypeSea()) {
					if((ntype != TERRAIN_WATER_SHELF) &&
					   (ntype != TERRAIN_WATER_TRENCH)) {

						cell->SetTerrain(TERRAIN_WATER_SHELF);
					}
				}
			}
		}
		if(terr == TERRAIN_WATER_BEACH && !nextToLand) {
			thisCell->SetTerrain(TERRAIN_WATER_SHALLOW);
			terr = TERRAIN_WATER_SHALLOW;
		}
	} else if(rec->GetMovementTypeSea()) {
		RadiusIterator it(pos, 1, 2.0);
		bool nextToLand = false;
		bool nextToShallow = false;
		bool nextToBeach = false;
		bool nextToShelf = false;

		for(it.Start(); !it.End(); it.Next()) {
			if(it.Pos() == pos) continue;

			Cell *cell = GetCell(it.Pos());
			Assert(cell);
			if(!cell) continue;

			neighborRec = g_theTerrainDB->Get(cell->GetTerrainType());
			if(!nextToLand && (neighborRec->GetMovementTypeLand() || neighborRec->GetMovementTypeMountain())) {
				nextToLand = true;
				thisCell->SetTerrain(TERRAIN_WATER_BEACH);
				terr = TERRAIN_WATER_BEACH;
				break;
			}
		}

		if(!nextToLand) {

			for(it.Start(); !it.End(); it.Next()) {
				if(it.Pos() == pos) continue;

				Cell *cell = GetCell(it.Pos());
				Assert(cell);
				if(!cell) continue;

				sint32 ntype = cell->GetTerrainType();
				neighborRec = g_theTerrainDB->Get(ntype);
				if(neighborRec->GetMovementTypeShallowWater()) {
					nextToShallow = true;
					if(ntype == TERRAIN_WATER_BEACH) {
						nextToBeach = true;
					}
				} else if(ntype == TERRAIN_WATER_SHELF || ntype == TERRAIN_WATER_TRENCH) {
					nextToShelf = true;
				}
			}

			if(!nextToShallow && ((terr == TERRAIN_WATER_SHELF) ||
								  (terr == TERRAIN_WATER_TRENCH))) {

				thisCell->SetTerrain(TERRAIN_WATER_DEEP);
				terr = TERRAIN_WATER_DEEP;
			}

			if(nextToBeach) {
				thisCell->SetTerrain(TERRAIN_WATER_SHELF);
				terr = TERRAIN_WATER_SHELF;
				for(it.Start(); !it.End(); it.Next()) {
					if(it.Pos() == pos) continue;
					if(GetCell(it.Pos())->GetTerrainType() == TERRAIN_WATER_BEACH) {
						SmartSetOneCell(it.Pos(), TERRAIN_WATER_BEACH);
					}
				}
			} else if(nextToShallow) {
				if((terr != TERRAIN_WATER_SHELF) && (terr != TERRAIN_WATER_TRENCH)) {
					thisCell->SetTerrain(TERRAIN_WATER_SHELF);
					terr = TERRAIN_WATER_SHELF;
				}
			}

			if(nextToShelf) {
				for(it.Start(); !it.End(); it.Next()) {
					if(it.Pos() == pos) continue;
					sint32 ntype = GetCell(it.Pos())->GetTerrainType();
					if(ntype == TERRAIN_WATER_SHELF ||
					   ntype == TERRAIN_WATER_TRENCH) {
						SmartSetOneCell(it.Pos(), ntype);
					}
				}
			}

		} else {

			for(it.Start(); !it.End(); it.Next()) {
				if(it.Pos() == pos) continue;
				Cell *cell = GetCell(it.Pos());
				Assert(cell);
				if(!cell) continue;

				sint32 ntype = cell->GetTerrainType();
				neighborRec = g_theTerrainDB->Get(ntype);

				if(neighborRec->GetMovementTypeShallowWater()) {
					SmartSetOneCell(it.Pos(), TERRAIN_WATER_SHALLOW);
				} else if(neighborRec->GetMovementTypeSea()) {
					if((ntype != TERRAIN_WATER_SHELF) &&
					   (ntype != TERRAIN_WATER_TRENCH)) {

						SmartSetOneCell(it.Pos(), TERRAIN_WATER_SHELF);
					}
				}
			}
		}
	}

	if(terr != oldTerr) {

		bool terrIsLand = g_theTerrainDB->Get(terr)->GetMovementTypeLand() || g_theTerrainDB->Get(terr)->GetMovementTypeMountain();
		bool oldTerrIsLand = g_theTerrainDB->Get(oldTerr)->GetMovementTypeLand() || g_theTerrainDB->Get(oldTerr)->GetMovementTypeMountain();
		if(oldTerrIsLand != terrIsLand) {
			if(thisCell->UnitArmy()) {
				thisCell->UnitArmy()->KillList(CAUSE_REMOVE_ARMY_ILLEGAL_CELL, -1);
			}

			if(thisCell->GetCity().IsValid()) {
				thisCell->GetCity().CD()->UpdateSprite();
			}
		}


		RadiusIterator it(pos, 1, 2.0);
		for(it.Start(); !it.End(); it.Next()) {
			Cell *cell = GetCell(it.Pos());
			if(cell) {
				if(!(cell->GetScratch() & k_SMART_SET_POST_PROCESS)) {
					cell->SetScratch(cell->GetScratch() | k_SMART_SET_POST_PROCESS);
				}
			}
		}
	}
}

#ifdef _DEBUG

void World::WholePlayerLandArea(int * a_Array) const
{
    std::fill(a_Array, a_Array + k_MAX_PLAYERS + 1, 0);

	Cell* const pLastCell = &m_cellArray[m_size.x * m_size.y - 1];
	for (Cell * pCurCell = m_cellArray; pCurCell <= pLastCell; ++pCurCell)
    {
		a_Array[pCurCell->GetOwner() + 1]++;
	}
}

#endif
