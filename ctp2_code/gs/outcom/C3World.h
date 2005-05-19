












#pragma once
#ifndef __C3World_H__
#define __C3World_H__

#include "ic3world.h"

class World; 

class MapPoint; 
enum TERRAIN_TYPES; 
class CivArchive; 
typedef sint32 TERRAIN_IMPROVEMENT;

class C3World : public IC3World
{
	ULONG m_refCount;
	
	
	World *m_ptr;
    Player *m_player; 

public:
	
	STDMETHODIMP QueryInterface(REFIID, void **obj);
	STDMETHODIMP_(ULONG) AddRef();
	STDMETHODIMP_(ULONG) Release();

    	
    STDMETHODIMP_(int) Test() { return 2; } 
    
    C3World(Player *p); 
    C3World(Player *p, CivArchive &archive); 
    void Serialize(CivArchive &archive);
	void GetOpenTileValuesRow(sint32 player, sint32 len, MapPoint &pos, sint32 &tiles, MapPointData *openList) ;

    
    STDMETHODIMP_ (sint16) GetXWidth();
    STDMETHODIMP_ (sint16) GetYHeight(); 
    STDMETHODIMP_ (sint16) GetZHeight();

    STDMETHODIMP_ (double) GetMoveCost(MapPointData *pos); 
    STDMETHODIMP_ (BOOL) GetTileType (MapPointData *pos, TERRAIN_TYPES *t);
    STDMETHODIMP_ (BOOL) IsVisible (MapPointData *pos);
    STDMETHODIMP_ (BOOL) IsExplored (MapPointData *pos);
    STDMETHODIMP_ (BOOL) IOwnThis (MapPointData *pos);
    STDMETHODIMP_ (BOOL) IsRiver (MapPointData *pos); 
    STDMETHODIMP_ (BOOL) CanBeIrrigated(MapPointData *pos);

    STDMETHODIMP_ (BOOL) IsTradeGood (MapPointData *pos); 
    STDMETHODIMP_ (BOOL) IsWater (MapPointData *pos); 
    STDMETHODIMP_ (double) GetDefenseBonus (MapPointData *pos); 
    STDMETHODIMP_ (double) GetTerrainDefenseBonus (MapPointData *pos); 

    STDMETHODIMP_ (BOOL) IsVisibleEnemyArmyThere (MapPointData *pos); 
    STDMETHODIMP_ (BOOL) IsMyArmyThere (MapPointData *pos); 

    STDMETHODIMP_ (BOOL) IsCellOwned (MapPointData *pos); 
    STDMETHODIMP_ (PLAYER_INDEX) GetCellOwner (MapPointData *pos); 
    STDMETHODIMP_ (BOOL) IsCityHere (MapPointData *pos); 
    STDMETHODIMP_ (PLAYER_INDEX) GetCityOwner (MapPointData *pos);  
    STDMETHODIMP_ (BOOL) IsArmyHere (MapPointData *pos); 
    STDMETHODIMP_ (PLAYER_INDEX) GetArmyOwner (MapPointData *pos); 
    STDMETHODIMP_ (BOOL) GetArmyId (MapPointData *pos, sint32 *player_id, 
        uint32 *a_id, sint32 *top_unit_type, sint32 *unit_num); 
    STDMETHODIMP_ (BOOL) GetCityId (MapPointData *pos, sint32 *player_id, 
        uint32 *c_id); 

    STDMETHODIMP_ (BOOL) IsWatchfulCityHere(MapPointData *pos); 

    STDMETHODIMP_ (MapValueStruct ***) GetMapValue (); 
    STDMETHODIMP_ (sint16) GetSettleValue(sint16 x, sint16 y, sint16 z); 


	
	STDMETHODIMP_ (void) GetCityAndArmyId( MapPointData* pos, sint32* player_id, 
							   uint32* a_id, sint32* top_unit_type, 
							   sint32* unit_num, uint32* c_id,
							   BOOL* unitVis, BOOL* cityVis, BOOL* mineVis );

    STDMETHODIMP_ (void) GetCellContents( MapPointData* pos, sint32* player_id, 
							   uint32* a_id, sint32* top_unit_type, 
							   sint32* unit_num, uint32* c_id,
							   BOOL* unitVis, BOOL* cityVis, BOOL* mineVis, 
                               BOOL *can_be_expelled );

 
    STDMETHODIMP_ (BOOL) GetUnitData(MapPointData *pos, PLAYER_INDEX *owner, 
        sint32 *unit_num, sint32 unit_type[k_MAX_ARMY_SIZE], 
        sint32 unit_hp[k_MAX_ARMY_SIZE], BOOL *is_entrenched); 

    STDMETHODIMP_ (void) GetTerrainCombatBonus (MapPointData *pos,
        double *terrain_bonus, double *fort_bonus, double *wall_bonus); 

	STDMETHODIMP_ (BOOL) GetMoveType(MapPointData *p, uint32 *moveType);
    STDMETHODIMP_ (BOOL) IsMoveTypeLand (MapPointData *p);
    STDMETHODIMP_ (BOOL) IsMoveTypeMountain (MapPointData *p);
    STDMETHODIMP_ (BOOL) IsMoveTypeWater (MapPointData *p);
    STDMETHODIMP_ (BOOL) IsMoveTypeShallowWater (MapPointData *p);
    STDMETHODIMP_ (BOOL) IsMoveTypeSpace (MapPointData *p);
    STDMETHODIMP_ (BOOL) IsMoveTypeTrade (MapPointData *p);

    STDMETHODIMP_ (BOOL) GetTerrainType(MapPointData *p, sint32 *type_terrain); 
    STDMETHODIMP_ (BOOL) GetGood (MapPointData *p, sint32 *type_good); 
    STDMETHODIMP_ (BOOL) GetFood(MapPointData *pos, sint32 *food);
    STDMETHODIMP_ (BOOL) GetProd(MapPointData *pos, sint32 *prod);

    STDMETHODIMP_ (BOOL) IsMyPopThere(MapPointData *pos);

	STDMETHODIMP_ (void) GetOpenTileValues(sint32 player, MapPointData *p, sint32 *tiles, MapPointData *openList) ;
	STDMETHODIMP_ (BOOL) ProvidesTrade(MapPointData *p) ;

    STDMETHODIMP_ (BOOL) IsNextTo (MapPointData *a, MapPointData *b); 

    STDMETHODIMP_ (BOOL) GetRoad (MapPointData *pos, sint32 *road_type); 
    STDMETHODIMP_ (BOOL) GetConstructingRoad(MapPointData *pos, sint32 *road_type);

    STDMETHODIMP_ (BOOL) IsConnectedToACity(MapPointData *pos, sint32 player_id);
    STDMETHODIMP_ (sint32) Get_XY_Travel_Distance(const MapPointData &A, const MapPointData &B);
    STDMETHODIMP_ (double) Get_XY_Euclidean_Distance_Squared(const MapPointData &A, const MapPointData &B);
    STDMETHODIMP_ (void) Get_Army_Types(MapPointData *pos, sint32 types_in_army[k_MAX_ARMY_SIZE], sint32 &num_in_army);

	STDMETHODIMP_ (BOOL) HasBonusFoodUnit(MapPointData *pos);

	STDMETHODIMP_ (sint32) GetConvertedTo(uint32 u_id, 
		BOOL *is_unknown_id, 
		MapPointData *target_pos);

	STDMETHODIMP_ (BOOL) GetSafeFromNukes(MapPointData *target_pos);
	STDMETHODIMP_ (BOOL) GetHasCityAirport(MapPointData *target_pos);
	STDMETHODIMP_ (BOOL) GetHasForceField(MapPointData *target_pos);
	STDMETHODIMP_ (BOOL) GetHasCityWalls(MapPointData *target_pos);

	STDMETHODIMP_ (sint32) GetFranchiseOwner(uint32 u_id, 
		BOOL *is_unknown_id, 
		MapPointData *target_pos);
	STDMETHODIMP_ (uint32) GetNumImprovements (const MapPointData &pos);
	STDMETHODIMP_ (TERRAIN_IMPROVEMENT) GetImprovementType (const MapPointData &pos, const uint32 & index);
	STDMETHODIMP_ (PLAYER_INDEX) GetImprovementOwner (const MapPointData & pos, const uint32 & index);



	STDMETHODIMP_ (BOOL) IsSafeFromNukes (const MapPointData & pos);

	STDMETHODIMP_ (uint32) GetNumTradeRoutes (const MapPointData & pos);
	STDMETHODIMP_ (PLAYER_INDEX) GetTradeRouteSource (const MapPointData & pos, const uint32 index);
	STDMETHODIMP_ (PLAYER_INDEX) GetTradeRouteDestination(const MapPointData & pos, const uint32 index);
	STDMETHODIMP_ (sint32) GetTradeRouteGoldInReturn (const MapPointData & pos, const uint32 index);
	STDMETHODIMP_ (BOOL) IsGoodyHut (const MapPointData & pos);

    STDMETHODIMP_ (BOOL) GetIsChokePoint(const MapPointData &pos);
    STDMETHODIMP_ (sint16) GetContinent(const MapPointData &pos);

    STDMETHODIMP_ (sint16) GetMinWaterContinent();
    STDMETHODIMP_ (sint16) GetMaxWaterContinent(); 
    STDMETHODIMP_ (sint16) GetMinLandContinent(); 
    STDMETHODIMP_ (sint16) GetMaxLandContinent(); 

    STDMETHODIMP_ (sint32) IsXwrap(); 
    STDMETHODIMP_ (sint32) IsYwrap(); 

#ifdef CELL_COLOR
     STDMETHODIMP_ (BOOL) ClearColor (THIS_); 
     STDMETHODIMP_ (BOOL) SetColor (MapPointData *pos, sint32 r, sint32 g, sint32 b);
     STDMETHODIMP_ (BOOL) SetColor (MapPointData *pos, sint32 c);

#endif
     
    STDMETHODIMP_ (BOOL) CanBuildUnderseaTunnel(MapPointData *pos);
    STDMETHODIMP_ (BOOL) HasUnderseaTunnel(MapPointData  *pos);
    
    STDMETHODIMP_ (sint32) GetLandContinentSize(sint32 cont_num);
    STDMETHODIMP_ (sint32) GetWaterContinentSize(sint32 cont_num); 

    STDMETHODIMP_ (BOOL) IsWaterNextTooLand(const sint32 waterc, const sint32 landc);
    STDMETHODIMP_ (BOOL) IsLandNextTooWater(const sint32 landc, const sint32 waterc);
	STDMETHODIMP_ (sint32) GetCitySlaveCount (MapPointData *pos);

	STDMETHODIMP_ (BOOL) IsWormholeVisible(const sint32 &player_id);
	STDMETHODIMP_ (void) GetWormholePos(MapPointData &pos);
	STDMETHODIMP_ (BOOL) HasWormholeProbeEntered(const MapPointData &pos);
	STDMETHODIMP_ (BOOL) HasWormholeProbeReturned(const MapPointData &pos);
	STDMETHODIMP_ (sint32) GetNumCities(const PLAYER_INDEX &player_index);
	STDMETHODIMP_ (sint32) GetNumArmies(const PLAYER_INDEX &player_index);
};

#endif
