//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : part of the World class implementation
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
// - Added second World::GetGood method, usefull if you already have a Cell
//   pointer. - May 18th 2005 Martin Gühmann
//
//----------------------------------------------------------------------------
#ifdef HAVE_PRAGMA_ONCE
#pragma once
#endif
#ifndef __WORLD_H__
#define __WORLD_H__

#include "MapPoint.h"
#include "CityRadius.h"
#include "StartingPosition.h"
#include "XY_Coordinates.h"
#include "dbtypes.h"
#include "gstypes.h"
#include "Cell.h"
#include "MoveFlags.h"

class MapPoint;
class CivArchive ;
class TileInfo;
class Installation;
class TerrainImprovement;
class Unit;
class UnitDynamicArray;
class GoodyHut;
class CellUnitList;
class Army;
class A_Star_Heuristic_Cost;
class TerrainRecord;
enum WORLD_DIRECTION;

typedef Cell * CellPtr; 
typedef CellPtr * CellYarray;
typedef CellYarray * CellXarray;


typedef bool (FindDistanceCellCallback) (const MapPoint &pos, Cell *cell, void *cookie);

template <class T> class PQueue;
struct DistItem;

#ifndef USE_COM_REPLACEMENT
interface IMapGenerator;
#else
#include "IMapGen.h"
#include <ltdl.h>
#endif
#define k_WORLD_VERSION_MAJOR	0									
#define k_WORLD_VERSION_MINOR	0									

enum WORLD_RADIUS_OP {
	WORLD_RADIUS_OP_SET_RADIUS,
	WORLD_RADIUS_OP_IRRIGATION_CHECK,
	WORLD_RADIUS_OP_CITY_SEARCH,
};

enum MAP_GENERATOR {
	MAP_GENERATOR_ORIGINAL,
		MAP_GENERATOR_PLUGIN
};

#define GROUND_Z 0
#define SPACE_Z 1
struct MapPointNode;


template <class T> class DynamicArray; 
typedef DynamicArray<sint32> DAsint32; 

class World : public CityRadiusCallback { 
    sint32 m_isYwrap; 
    sint32 m_isXwrap;					
    BOOL m_continents_are_numbered; 

    MAP_GENERATOR m_mapGenerator;

    MapPoint m_size;
    CellXarray	m_map; 
    
    sint16 m_water_continent_max;
    sint16 m_land_continent_max; 
    DynamicArray<DAsint32> *m_water_next_too_land;
    DynamicArray<DAsint32> *m_land_next_too_water;
    DynamicArray<sint32> *m_water_size;
    DynamicArray<sint32> *m_land_size; 

    Cell		*m_cellArray;
    CellYarray	*m_tmpx;
    
    sint32			m_player_start_list[8][2];								
    TileInfo		*m_tileInfoStorage;
    double          *m_goodValue;

    
    
    
	
	
	sint32 m_num_civ_starts;
	StartingPosition m_civ_starts[k_MAX_START_POINTS];
	
	
	

    
    sint32			m_insertCityOwner; 
    WORLD_RADIUS_OP m_radiusOp;
    BOOL            m_tempIrrigation;
#ifndef USE_COM_REPLACEMENT
    HINSTANCE       m_current_plugin;
#else
    lt_dlhandle     m_current_plugin;
#endif
	BOOL            m_isInsideRadius;
	uint32          m_ignoreCity;
	sint32          m_cityOwnerCheck;
	PQueue<DistItem> *m_distanceQueue;
	
	
	
	
	sint32			m_playerLandArea[k_MAX_PLAYERS+1];
	
	
	uint32			m_capitolDistanceDirtyFlags;  
												

    
    
	friend class NetGameSettings;
	friend class NetWorld;

public:
	void SetCapitolDistanceDirtyFlags(uint32 flags) {m_capitolDistanceDirtyFlags |= flags;}
    
    
    XY_Coordinates XY_Coords;
	A_Star_Heuristic_Cost * A_star_heuristic;
    
    World(const MapPoint m, const int xw, const int yw);
    World(CivArchive &archive, BOOL fromMapFile = FALSE);
    void CreateTheWorld(MapPoint player_start_list[k_MAX_PLAYERS],
						sint32 player_start_score[k_MAX_PLAYERS]);
    virtual ~World();
    
    void FreeMap();
    void AllocateMap();
    void Reset(sint16 x, sint16 y, BOOL yWrap, BOOL xWrap);
    
    inline sint32 IsYwrap() const { return m_isYwrap; } 
    inline sint32 IsXwrap() const { return m_isXwrap; } 

	
	void SetXWrap(bool on) {m_isXwrap = on;};
	void SetYWrap(bool on) {m_isYwrap = on;};

    inline sint32 GetXWidth() const { return m_size.x; }
    inline sint32 GetYHeight() const { return m_size.y; } 
    
    Cell * GetCell(const MapPoint &p) const { 
        Assert(0 <= p.x);  Assert(p.x < m_size.x); 
        Assert(0 <= p.y);  Assert(p.y < m_size.y); 
        
        return m_map[p.x][p.y]; 
    }; 
    
    Cell * GetCell(const sint32 x, const sint32 y)  const { 
        Assert(0 <= x);  Assert(x < m_size.x); 
        Assert(0 <= y);  Assert(y < m_size.y); 
        return m_map[x][y]; 
    }; 
    
    Cell * AccessCell(const MapPoint &p)  { 
        Assert(0 <= p.x);  Assert(p.x < m_size.x); 
        Assert(0 <= p.y);  Assert(p.y < m_size.y); 
        
        return m_map[p.x][p.y]; 
    };     
    
    Cell * AccessCell(const sint32 x, const sint32 y)  { 
        Assert(0 <= x);  Assert(x < m_size.x); 
        Assert(0 <= y);  Assert(y < m_size.y); 
        return m_map[x][y]; 
    };  

    
    
    
    sint32 Verify1();
    sint32 Verify2();
    sint32 Verify3();
    
    void RandTilePoint(MapPoint &r);
    void FillBox (const MapPoint r, const sint32 radius, sint32 c, sint32 &landCount);
    sint32 IsNearCon(MapPoint r, const sint32 radius,  const sint32 conCount);
    void GenerateRandMap(MapPoint player_start_list[k_MAX_PLAYERS]); 
    void GenerateContinents(const sint32 tot); 
    void GenerateIslands(const sint32 tot);
    void GenerateHumidity();
    void GenerateHills(); 
    void GenerateSpace();
    
    sint32 IsInTheOcean (const MapPoint r, sint32 cr);
    void CalcRmap(sint32 ** m, sint32 rmapRadius, sint32 rmapCycle, 
        sint32 hmin[k_NUM_MERIDIANS+1], sint32 hmax[k_NUM_MERIDIANS+1]);
    
    void GenerateMountains(const sint32 totalLand); 
    void PostMountains();
    void PostMountains2();
    
    void GenerateHills(const sint32 totalHill); 
    void GenerateGlacier();
    void Mdump(FILE *fout);
    void Dump(FILE *fout);
	void MapDump(MBCHAR *mapName, sint8 *mapData, sint16 width, sint16 height);
	void TerrainDump (MBCHAR *mapName, sint8 *mapData, sint16 width, 
		sint16 height, sint32 waterLevel, sint32 mountainLevel, sint32 hillLevel);
    
    BOOL pick_start_cell(sint32 **c, sint32 xcell, sint32 ycell, sint32 cellsize, 
        sint32 &startxcell, sint32 &startycell,  MapPoint &pos);
    BOOL can_put_mt(const MapPoint &pos, sint32 cellsize, sint32 startxcell, sint32 startycell);
    void flip_dir (WORLD_DIRECTION &direction);
    void rand_dir(WORLD_DIRECTION &direction);
    void rand4dir(WORLD_DIRECTION &direction);
    void num_mountain (sint32 &nm);
    
    sint32 IsNextToLand(const sint32 i, const sint32 j);
    sint32 IsNextToWater(const sint32 x, const sint32 y) ;
    sint32 IsNextToWaterNotDiagonals(const sint32 x, const sint32 y);
    sint32 IsNextTo (const sint32 t, const sint32 i, const sint32 j);
    sint32 IsSurroundedByWater(const sint32 x, const sint32 y) ;
    void GenerateShallowWater();
    void GenerateDeepWater();
    void NewGenerateDeepWater();

    BOOL GetAdjacentLand(MapPoint &pos, MapPoint &land);

	
	BOOL GetAdjacentOcean(const MapPoint &pos, sint32 & water_cont) const;
    
    void GenerateTrenches();
    void GenerateVolcano();
    
    sint32 IsMinama(const sint32 i, const sint32 j);
    void GenerateGoods();
	void ComputeGoodsValues();
    void GenerateGoodyHuts();
    
    void CalcRawScore(const sint32 x, const sint32 y, float &score);
    void CalcCumScore(sint32 d, const sint32 x, const sint32 y, float &cum_score, float **raw_score);
    BOOL FindMaxCumScore(sint32 d, float **cum_score, sint32 &maxx, sint32 &maxy,
						 MapPoint player_start[k_MAX_PLAYERS], sint32 index,
						 sint32 minDist, sint32 maxDist,
						 BOOL ignoreTutorialRules);
    void FlattenCumScore(sint32 d, float **cum_score, const sint32 maxx, const sint32 maxy);
    
    sint32 CalcStartNiceness(const sint32 x, const sint32 y, const sint32 d, sint32 **con);
    
    void FindPlayerStart(MapPoint player_start[k_MAX_PLAYERS], 
						 sint32 &numStartsFound,
						 BOOL ignoreTutorialRules,
						 sint32 player_start_score[k_MAX_PLAYERS]);
    
    sint32 IsNextToLand4(const sint32 x, const sint32 y) const; 
    sint32 IsNextTo4(const sint32 t, const sint32 x, const sint32 y) const;
    
    sint32 PickNextRiver(sint32 v, MapPoint prev, MapPoint &pos);   
    void GenerateRivers(sint32 land);
    sint32 IsRiverClear(const sint32 x, const sint32 y);
    void SetAllMoveCost(); 
    
    
    sint32 GetTerrain(sint32 x, sint32 y);
    void SetTerrain(sint32 x, sint32 y, sint32 type);
    MapPoint *GetSize() { return &m_size; }
    inline sint32 GetWidth() { return m_size.x; }
    inline sint32 GetHeight() { return m_size.y; }
    
    
    
    
    sint16 GetContinent(const MapPoint & pos) const;
    
    sint16 GetMinWaterContinent() const; 
    sint16 GetMaxWaterContinent() const; 
    sint16 GetMinLandContinent() const; 
    sint16 GetMaxLandContinent() const; 
    
    void NumberContinents(); 
    void InitContinent(); 
    
    void GrowOceans();
    BOOL IsNewWater(MapPoint &p);
    void GrowWater(MapPoint &start);
    
    void AddToWaterSearch(MapPointNode *&search_list, 
        MapPoint &pos);
    
    BOOL NextPoint(MapPointNode *&search_list, MapPointNode *&finished_list, 
        MapPoint &p);
    
    void ResetCanalsTunnels();
    
    void GrowContinents();
    BOOL IsNewLand(MapPoint &p);
    void GrowLand(MapPoint &start);
    
    void FindContinentSize();
    sint32 GetLandContinentSize(sint32 cont_num)const; 
    sint32 GetWaterContinentSize(sint32 cont_num)const; 

    void AddToLandSearch(MapPointNode *&search_list, 
        MapPoint &pos);
    
    void GetContinent(const MapPoint &pos, sint32 &cont_number, 
        BOOL &is_land) const;
    
    void AllocateNeighborMem();

    void FindContinentNeighbors();
    
    void FindAContinentNeighbor(const sint32 center_cont, 
        MapPoint &test_point, const BOOL is_land);
    
    void InsertWaterNextToLand(const sint32 waterc, const sint32 landc);    
    void InsertLandNextToWater(const sint32 landc, const sint32 waterc); 
    

    BOOL IsWaterNextTooLand(const sint32 waterc, const sint32 landc) const;
    BOOL IsLandNextTooWater(const sint32 landc, const sint32 waterc) const;

	
	BOOL LandShareWater(const sint32 land1, const sint32 land2) const;

    
    BOOL GetIsChokePoint(MapPoint &pos) const;
    
    void CalcChokePoints();
    
    BOOL IsGFComputed(const BOOL is_choke_land, MapPoint &pos);
    void Grassfire8(const BOOL is_choke_land, sint16 **tmp_map);
    void ClipGF(sint16 **tmp_map); 
    
    void SaveGF(sint16 **water_map, sint16 **land_map);
    void DumpGF();
    
    
    
    const char * GetTerrainName (const MapPoint &pos);
    TERRAIN_TYPES GetTerrainType(const MapPoint &pos) const;
    
    sint32 IsGood(const MapPoint &pos) const;
    sint32 IsGood(const sint32 x, const sint32 y) const;
    BOOL GetGood(const MapPoint &pos, sint32 &good) const;
    BOOL GetGood(const Cell *c, sint32 &good) const;
    void SetGood(const sint32 x, const sint32 y, const sint32 g);
    void SetRandomGood(const sint32 x, const sint32 y);
    void ClearGoods(const sint32 x, const sint32 y) ;
    void SetMovementType(const sint32 x, const sint32 y, const sint32 t); 
    uint32 GetMovementType(const MapPoint &pos); 
    
    sint32 CanEnter(const MapPoint &pos, const uint32 flag) const;

	
	inline sint32 World::EnvIsWater(const uint32 env) const
	{
		return (env & (k_MASK_ENV_MOVEMENT_TYPE & (k_BIT_MOVEMENT_TYPE_WATER | k_BIT_MOVEMENT_TYPE_SHALLOW_WATER))) != 0; 
	}

	inline sint32 World::IsWater(const MapPoint &pos) const
	{ 
		return EnvIsWater(GetCell(pos)->m_env);
	}

	inline sint32 World::IsWater(const sint32 x, const sint32 y) const 
	{ 
		return EnvIsWater(m_map[x][y]->m_env);
	}
   
    sint32 EnvIsShallowWater(const uint32 env) const;
    sint32 IsShallowWater(const MapPoint &pos) const;
    sint32 IsShallowWater(const sint32 x, const sint32 y) const;
    
    sint32 EnvIsLand(const uint32 env) const;
    sint32 IsLand(const MapPoint &pos) const;
    sint32 IsLand(const sint32 i, const sint32 j) const;
    
    sint32 EnvIsSpace(const uint32 env) const;
    sint32 IsSpace(const MapPoint &pos) const;
    sint32 IsSpace(const sint32 i, const sint32 j) const;
    
    sint32 EnvIsMountain(const uint32 env) const;
    sint32 IsMountain(const sint32 x, const sint32 y) const;
    sint32 IsMountain(const MapPoint &pos) const;
    
    sint32 EnvIsHill(const uint32 env) const;
    sint32 IsHill(const MapPoint &pos) const ;
    sint32 IsHill(const sint32 x, const sint32 y) const ;
    
    sint32 IsNextToScratch(const sint32 prev, const sint32 i, const sint32 j);
    void CalcDistToWater(sint32 &maxd);
    sint32 IsCurrent(const MapPoint &pos); 
    sint32 IsCurrent(const sint32 x, const sint32 y); 
    
    sint32 EnvIsRiver(const uint32 env) const;
    sint32 IsRiver(const MapPoint &pos) const;
    sint32 IsRiver(const sint32 x, const sint32 y) const;
    
    void SetCurrent(const MapPoint &pos);
    void SetCurrent(const sint32 x, const sint32 y);
    void SetRiver(const MapPoint &pos); 
    void UnsetRiver(const sint32 x, const sint32 y);
    
    BOOL CanBeIrrigated(const MapPoint &pos);
    
	
	const TerrainRecord *GetTerrain(const MapPoint &pos);
    
    
    
    sint32 IsCity(const MapPoint &pos) const; 
	
    
    
    
    sint32 IsCanal(const MapPoint &pos) const;
    sint32 EnvIsTunnel(const uint32 env) const ;
    sint32 IsTunnel(const MapPoint &pos) const;
    void SetCanalTunnel(MapPoint &pos, sint32 level);

	
    
    
    
    sint32 EnvIsRoad(const sint32 i, const uint32 env) const;	
    sint32 IsRoad(const sint32 i, const MapPoint &pos) const ;
	sint32 IsAnyRoad(const MapPoint &pos) const;
	
    
    
    
    sint32 EnvIsIrrigation(const sint32 i, const uint32 env) const; 
    sint32 IsIrrigation(const sint32 i, const MapPoint &pos) const ; 
	
    
    
    
    sint32 EnvIsMine(const sint32 i, const uint32 env) const; 
    sint32 IsMine(const sint32 i, const MapPoint &pos) const; 
	
    
    
    BOOL IsInstallation(const MapPoint &pos) const;
    
    
    double CalcTerrainFreightCost(const MapPoint &pos);
    
    sint32 HasCity(const MapPoint &pos) const; 
    BOOL IsSupplyingTrade(const MapPoint &pos) const ;
    
    BOOL IsConnectedToCity(const MapPoint &pos, sint32 owner, uint8* array = NULL) const;
	BOOL IsOnOrNextToOwner(const MapPoint &pos, sint32 owner);

    BOOL IsContinentSharedWithOthers(const MapPoint &pnt, 
        sint32 owner, 
        uint8* array) const;
    BOOL IsContinentBiggerThan(uint32 size,
        const MapPoint &pnt,
        uint8 *array = NULL,
        uint32 *cursize = NULL) const;
#ifdef _DEBUG
    sint32 GetColor(const MapPoint &pos) const; 
#endif
    
    
    
    
    sint32 InsertUnit (const MapPoint &pos, Unit &id, UnitDynamicArray &revealedUnits);
    
    sint32 RemoveUnitReference(const MapPoint &pos, const Unit &id);
    
    double GetMoveCost(const MapPoint &pos) const;
    sint32 GetEnvFlags(const MapPoint &pos) const;
    
    sint32 GetEmptyTransports(const MapPoint pos, CellUnitList &transports); 
    
    void GetArmy(const MapPoint &pos, CellUnitList &army); 
    void GetNeighborUnits(const MapPoint &pos, UnitDynamicArray &n);
    void ClearArmies();
    
    BOOL IsCellZoc(const PLAYER_INDEX &owner, const MapPoint &pos, const BOOL is_check_only_visible); 
    BOOL IsMoveZOC(PLAYER_INDEX owner, const MapPoint &start, const MapPoint &dest, 
        const BOOL is_check_only_visible);
    
    
    
    
    CellUnitList * GetArmyPtr(const MapPoint &pos);
    
    
    void CityRadiusFunc(const MapPoint &pos);
    sint32 InsertCity(const MapPoint &pos, Unit u); 
    void MoveUnitToCitySlot(Unit newCity, const MapPoint &pos);
    Unit GetCity(const MapPoint &pos);
    
    void CityXOff(const MapPoint &pos, sint32 &startx, sint32 &endx);
    void CityYOff(sint32 x, const MapPoint &pos, sint32 &starty, sint32 &endy);
    
    sint32 GetFoodProduced(const MapPoint &pos) const;
    sint32 GetShieldsProduced(const MapPoint &pos) const;
    
    
    void InsertImprovement(const TerrainImprovement &imp, MapPoint &point);
    void RemoveImprovement(const TerrainImprovement &imp, const MapPoint &point);
    
    void InsertInstallation(Installation &inst, MapPoint &point);
    void RemoveInstallation(Installation &inst, MapPoint &point);
	sint32 CountImprovements(const MapPoint & pos);

	
	static sint32 GetMaxFoodFromTerrain();
	static sint32 GetMaxShieldsFromTerrain();
	static sint32 GetAvgFoodFromTerrain();
	static sint32 GetAvgShieldsFromTerrain();
	static sint32 GetAvgGoldFromTerrain();
    
    void GamestateDebug(); 
    void SetColor(const MapPoint &pos, sint32 c);
    
    sint32 GetTopVisibleUnit (const MapPoint &pos, Unit &top) const;
    sint32 GetTopVisibleUnitNotCity(const MapPoint &pos, Unit &top) const;
    sint32 GetTopVisibleUnit (const sint32 looking_player, const MapPoint &pos, Unit &top, BOOL includeCities = TRUE) const; 
    
    sint32 GetTopRadarUnit(const MapPoint &pos, Unit &top) const;
    
    sint32 GetSecondUnit (const MapPoint &pos, Unit &second) const;
    
    void GlobalWarming(const sint32 phase) ;
    void GWPhase0(void) ;
    void GWPhase1(void) ;
    void RaiseWaters(void) ;
    void MakeBeaches(void) ;
    void FloodCell(sint32 x, sint32 y, Cell *c) ;
    void FloodEverythingInCell(sint32 x, sint32 y, Cell *c) ;
    void FloodImprovements(sint32 x, sint32 y, Cell *c) ;
    void FloodGoodyHut(Cell *c) ;
    void FloodCity(Cell *c) ;
    void FloodPopulation(Cell *c) ;
    void FloodArmies(Cell *c) ;
    void ConvertToShallowWater(sint32 x, sint32 y, Cell *c) ;
    void ConvertToBeach(sint32 x, sint32 y, Cell *c) ;
    
    void RemoveBeaches(void) ;
    void FloodRivers(void) ;
    void OzoneDepletion(const sint32 phase) ;
    void InformPlayersOfFloodingCatastrophe(void) ;
    
    void ChangeOwner(const MapPoint &point, sint32 fromOwner, sint32 toOwner);
    void CutImprovements(const MapPoint &point);
    double GetDefenseBonus(const MapPoint &point);
    BOOL IsAirfield(const MapPoint &point);
    
    
    BOOL IsListeningPost(const MapPoint &point);


    BOOL IsSafeFromNukes(const MapPoint &point);
    BOOL IsRadar(const MapPoint &point);
    BOOL IsHealUnits(const MapPoint &point);
    BOOL IsFort(const MapPoint &point);

    
    void Serialize(CivArchive &archive) ;
    void SerializeJustMap(CivArchive &archive);
    
    
    TileInfo *GetTileInfo(const MapPoint &pos) { return GetTileInfoStoragePtr(pos); }
    
    
    
    GoodyHut *GetGoodyHut(const MapPoint & pos);
    
    void AllocateTileInfoStorage(void);
    void DisposeTileInfoStorage(void);
    TileInfo *GetTileInfoStoragePtr(const MapPoint &pos);
    
    PLAYER_INDEX GetOwner(const MapPoint &pos) const; 
    
    IMapGenerator *LoadMapPlugin(sint32 pass);
    void FreeMapPlugin();
	void GetHeightMap(IMapGenerator *gen, sint8 *&map,
					  const double *settings, sint32 numSettings);
	void GetHistogram(sint8 *&map, sint32 *&histogram);
    void GetMapAndHistogram(IMapGenerator *gen, sint8 *&map, sint32 *&histogram,
							const double *settings, sint32 numSettings);
    void FlatToIso(const MapPoint &flat, MapPoint &iso);
    BOOL IsNextToOldRiver(const MapPoint &newpoint, const MapPoint &lastpoint);
    void NewGenerateRivers(sint8 *map, sint8 *wetmap);
    
    sint32 IsNextToCity(const MapPoint &pos);

    void GWPhase(const sint32 phase);

    void GlobalWarming(void);

    void OzoneDepletion(void);

	void AddZOC(const MapPoint &pos, sint32 player);
	void RemoveZOC(const MapPoint &pos, sint32 player);
	void AddOtherArmyZOC(const MapPoint &pos, sint32 player, const Army &a,
						 const Unit &notThisCity);
	BOOL AdjacentToZOCUnit(const MapPoint &pos, sint32 player, const Army &notThisArmy,
						   const Unit &notThisCity);
	void GetAdjacentUnits(UnitDynamicArray &units, const MapPoint &pnt);
    void GetAdjacentCities(UnitDynamicArray &units, const MapPoint &cpos);

	void RegisterPlayerDead(sint32 owner);

	BOOL IsInsideCityRadiusOfPlayerOtherThan(const MapPoint &pos, sint32 owner, const Unit &ignoreThisCity);

	void FindCityDistances(sint32 player, const MapPoint &start);
	void CDMove(sint32 costSoFar, const sint32 x, const sint32 y,
				sint32 player, sint32 &numCitiesToVisit);
	void FindDistances(sint32 player, const MapPoint &start, sint32 numHits,
					   FindDistanceCellCallback *cb, void *cookie);
	void FDMove(sint32 costSoFar, const sint32 x, const sint32 y,
				sint32 player, sint32 &numHitsNeeded, FindDistanceCellCallback *cb,
				void *cookie);

	void ClearScratch();

	void RecalculateZOC();
	void RecalculateZOC(const MapPoint &p);

	void RegenerateRivers();
	void RemoveIsolatedWater();

	sint32 GetNumStartingPositions();
	MapPoint GetStartingPoint(sint32 index);
	sint32 GetStartingPointCiv(sint32 index);
	void SetStartingPoint(sint32 index, MapPoint &point, sint32 civ);

	void ClearStartingPoints();
	void AddStartingPoint(MapPoint &point, sint32 civ);
	void DeleteStartingPoint(sint32 index);
	
#ifdef _DEBUG
    void DebugUpdatePopColor(); 
    void ShowCellOwners();
#endif

	
	BOOL ExportMap(MBCHAR *filename);
	BOOL ImportMap(MBCHAR *filename);

	void SmartSetTerrain(const MapPoint &pos, sint32 terr, sint32 radius);
	void SmartSetOneCell(const MapPoint &pos, sint32 terr);

	double GetGoodValue(sint32 good);

#ifdef _DEBUG
	void WholePlayerLandArea(int *array) const;
#endif

}; 

extern World *g_theWorld;

uint32 World_World_GetVersion(void) ;
#else 

class World; 

#endif

