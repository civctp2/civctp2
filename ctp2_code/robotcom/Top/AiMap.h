#pragma once 

#ifndef __AI_MAP_H__
#define __AI_MAP_H__

#include "ctp2_enums.h"
#include "dynarr.h"
#include "aicell.h"
#include "priorityqueue.h"


class AiCell; 
class AiCellPtr; 
class MapPoint;

typedef AiCell * AiCellYarray ;
typedef AiCellYarray * AiCellXarray;

struct MapPointData; 

class AiMain; 
class CivArchive; 
class Bit_Table; 
enum GOAL_TYPE; 
enum GRADIENT_BITS; 

#define k_MINECELLX 15 
#define k_MINECELLY 15 

#include "TerrImproveData.h"

enum ERROR_INSERT_SETTLE { 
    ERROR_INSERT_SETTLE_OK, 
    ERROR_INSERT_SETTLE_EMPTY
};


interface IC3GameState; 


class AiMap { 

    MapPointData *m_size; 
  	

    Bit_Table *m_can_settle_ground; 
    Bit_Table *m_can_settle_space; 

    sint32 m_choke_land; 
	DynamicArray<MapPointData> m_stealthAttacks;
	DynamicArray<MapPointData> m_minedCells;
	DynamicArray<MapPointData> m_nukeSafeCities;

    sint32 m_cellx; 
    sint32 m_celly; 
    sint32 **m_mine_count;
    sint32 **m_fort_count;
    sint32 **m_airfield_count; 
    sint32 **m_guardtower_count; 

    DAPriorityQueue<AiCellPtr> m_settle_queue; 

    sint32 m_settler_xy_range; 
public:

	AiMap(sint16 x, sint16 y, sint16 z, IC3GameState *gs); 
    AiMap(CivArchive &archive); 
    void Serialize(CivArchive &archive); 
	~AiMap();

    void BeginTurn(); 







    MapPointData *GetSize() { return m_size; }
    

    void Wrap(const sint16 x, const sint16 y, const sint16 z, MapPointData &w) const;
    double WrappedDistance(const MapPointData &a, const MapPointData &b);

    BOOL CanSeeForeigner(AiMain *ai);
    void GetVisibleForeigners(AiMain *ai);
    
    void IncLandCityNeighbor (AiMain *ai, MapPointData &w, sint32 val[9], sint32 &num);
    void IncWaterCityNeighbor (AiMain *ai, MapPointData &w, sint32 val[9], sint32 &num);
    void IncCityNeighbors(AiMain *ai, const sint32 water, const sint32 land, 
        MapPointData &pos);
    BOOL IsNextToUnexplored(AiMain *ai, 
                               const MapPointData &pos);




    void FindContinentNeighbors(AiMain *ai);
    void FindAContinentNeighbor(AiMain *ai, MapPointData &pos, const sint32 v,
         MapPointData &w, const BOOL is_land, const BOOL is_water);

    BOOL HasPopFromThisCity(AiMain *ai, MapPointData &w, uint32 city_id) const;

	sint32 ExploredArea(AiMain *ai);

	
	void RegisterStealthAttack(const MapPointData &pos);
	void ClearStealthAttacks();
	DynamicArray<MapPointData> & GetStealthAttacks();
	void RegisterMinedCell(const MapPointData &pos);
	void ClearMinedCells();
	void RemoveMinedCell(const MapPointData &pos);
	DynamicArray<MapPointData> & GetMinedCells();
	void RegisterNukeSafeCity(const MapPointData &pos);
	BOOL IsNukeSafeCity(const MapPointData &pos);


 
    void IncMineCellCount(const MapPointData &pos);
    sint32 GetMineCellCount(const MapPointData &pos);

    void IncFortCellCount(const MapPointData &pos);
    sint32 GetFortCellCount(const MapPointData &pos);

    void IncAirfieldCellCount(const MapPointData &pos);
    sint32 GetAirfieldCellCount(const MapPointData &pos);

    void IncGuardtowerCellCount(const MapPointData &pos);
    sint32 GetGuardtowerCellCount(const MapPointData &pos);

    void GetCellMax(sint32 &maxx, sint32 &maxy) { maxx =  m_cellx; maxy = m_celly;  } 

    
    void GroundBuildSettleTree(AiMain *ai);    
    void SpaceBuildSettleTree(AiMain *ai);    

    void FlattenCanSettle(Bit_Table *can_settle, MapPointData &center, 
        sint16 radius);

    void FlattenStaticSettle(MapPointData &center, sint16 radius);

    void UnflattenSettle(MapPointData &pos, sint16 radius); 


    void InsertBestSettleGoal(AiMain *ai, ERROR_INSERT_SETTLE &err, 
        Bit_Table *can_settle, 
         sint32 *land_cont_count, sint32 &total_land_count, sint32 land_eval_nun,          
         sint32 *water_cont_count,  sint32 &total_water_count, sint32 water_eval_nun, 
        sint16 z_value);

    void InsertSomeSpaceCities(AiMain *ai);

    void InsertSettleGoals(AiMain *ai, const sint32 eval_num);
    void DumpSettleScore(AiMain *ai);

    void CleanSettleQueue();

};


BOOL IsEqual(const MapPointData &start, const MapPointData &pos);

#endif __AI_MAP_H__
