













#pragma once
#ifndef __IC3WORLD_H__
#define __IC3WORLD_H__

#include <OBJBASE.H>  

#undef INTERFACE
#define INTERFACE IC3World

DEFINE_GUID(CLSID_IC3World, 
  0x61a93720,
  0xc87a,
  0x11d1,
  0x85, 0xa7, 0x00, 0x60, 0x08, 0x3a, 0x40, 0x82
);

typedef sint32 PLAYER_INDEX; 
class Player;

struct MapPointData;
enum TERRAIN_TYPES; 
enum TERRAIN_IMPROVEMENT;

#define CELL_COLOR 1

typedef struct { 
    sint8 m_settle_prod; 
    sint8 m_settle_gold; 
    sint8 m_settle_food; 
    sint8 m_good_index; 
} WorkingMapValueStruct;

typedef struct { 
    sint16 m_settle_value;
   
} MapValueStruct;

DECLARE_INTERFACE_(IC3World, IUnknown)
{
 	
   
    STDMETHOD_ (int, Test) (THIS)  PURE; 

    
    STDMETHOD_ (sint16, GetXWidth) (THIS_) PURE; 
    STDMETHOD_ (sint16, GetYHeight) (THIS_) PURE; 
    STDMETHOD_ (sint16, GetZHeight) (THIS_) PURE; 

    STDMETHOD_ (double, GetMoveCost)(THIS_ MapPointData *pos) PURE; 
    STDMETHOD_ (BOOL, GetTileType) (THIS_  MapPointData *pos, 
        TERRAIN_TYPES *t)  PURE; 
                                 
    STDMETHOD_ (BOOL, IsVisible) (THIS_ MapPointData *pos) PURE;
    STDMETHOD_ (BOOL, IsExplored) (THIS_ MapPointData *pos) PURE;
    STDMETHOD_ (BOOL, IOwnThis) (THIS_ MapPointData *pos) PURE;
    STDMETHOD_ (BOOL, IsRiver) (THIS_ MapPointData *pos) PURE; 
    STDMETHOD_ (BOOL, CanBeIrrigated)(THIS_ MapPointData *pos) PURE;

    STDMETHOD_ (BOOL, IsTradeGood) (THIS_ MapPointData *pos) PURE; 
    STDMETHOD_ (BOOL, IsWater) (THIS_ MapPointData *pos) PURE; 
    STDMETHOD_ (double, GetDefenseBonus) (THIS_ MapPointData *pos) PURE; 
    STDMETHOD_ (double, GetTerrainDefenseBonus) (THIS_ MapPointData *pos) PURE; 

    STDMETHOD_ (BOOL, IsVisibleEnemyArmyThere) (THIS_ MapPointData *pos) PURE; 
    STDMETHOD_ (BOOL, IsMyArmyThere) (THIS_ MapPointData *pos) PURE; 

    STDMETHOD_ (BOOL, IsCellOwned) (THIS_ MapPointData *pos) PURE; 
    STDMETHOD_ (PLAYER_INDEX, GetCellOwner) (THIS_ MapPointData *pos) PURE; 
    STDMETHOD_ (BOOL, IsCityHere) (THIS_ MapPointData *pos) PURE; 
    STDMETHOD_ (PLAYER_INDEX, GetCityOwner) (THIS_ MapPointData *pos) PURE;  
    STDMETHOD_ (BOOL, IsArmyHere) (THIS_ MapPointData *pos) PURE; 
    STDMETHOD_ (PLAYER_INDEX, GetArmyOwner) (THIS_ MapPointData *pos) PURE;
    
    STDMETHOD_ (BOOL, GetArmyId) (THIS_ MapPointData *pos, sint32 *player_id,
       uint32 *a_id, sint32 *top_unit_type, sint32 *unit_num) PURE; 
    STDMETHOD_ (BOOL, GetCityId) (THIS_ MapPointData *pos, sint32 *player_id, 
        uint32 *c_id) PURE; 

    STDMETHOD_ (BOOL, IsWatchfulCityHere) (THIS_ MapPointData *pos) PURE; 

	STDMETHOD_ (void, GetCityAndArmyId) (THIS_ MapPointData* pos, sint32* player_id, 
							   uint32* a_id, sint32* top_unit_type, 
							   sint32* unit_num, uint32* c_id,
							   BOOL* unitVis, BOOL* cityVis, BOOL* mineVis ) PURE;

    
    STDMETHOD_ (void, GetCellContents) (THIS_ MapPointData* pos, sint32* player_id, 
							   uint32* a_id, sint32* top_unit_type, 
							   sint32* unit_num, uint32* c_id,
							   BOOL* unitVis, BOOL* cityVis, BOOL* mineVis, 
                               BOOL *can_be_expelled ) PURE;



    STDMETHOD_ (BOOL, GetUnitData)(MapPointData *pos, PLAYER_INDEX *owner, 
        sint32 *unit_num, sint32 unit_type[k_MAX_ARMY_SIZE], 
        sint32 unit_hp[k_MAX_ARMY_SIZE], BOOL *is_entrenched) PURE; 

    STDMETHOD_ (void, GetTerrainCombatBonus) (MapPointData *pos,
        double *terrain_bonus, double *fort_bonus, double *wall_bonus) PURE; 

	STDMETHOD_ (BOOL, GetMoveType) (MapPointData *p, uint32 *moveType) PURE;
    STDMETHOD_ (BOOL, IsMoveTypeLand) (THIS_ MapPointData *p) PURE;
    STDMETHOD_ (BOOL, IsMoveTypeMountain) (THIS_ MapPointData *p) PURE;
    STDMETHOD_ (BOOL, IsMoveTypeWater) (THIS_ MapPointData *p) PURE;
    STDMETHOD_ (BOOL, IsMoveTypeShallowWater) (THIS_ MapPointData *p)PURE;
    STDMETHOD_ (BOOL, IsMoveTypeSpace) (THIS_ MapPointData *p)PURE;
    STDMETHOD_ (BOOL, IsMoveTypeTrade) (THIS_ MapPointData *p)PURE;
    STDMETHOD_ (BOOL, IsMyPopThere) (THIS_ MapPointData *p)PURE;
    STDMETHOD_ (BOOL, GetCityIdOfMyPop) (THIS_ MapPointData *w, uint32 *u_id) PURE; 

    STDMETHOD_ (BOOL, GetTerrainType) (THIS_ MapPointData *p, sint32 *type_terrain) PURE; 
	STDMETHOD_ (void, GetOpenTileValues)(THIS_ sint32 player, MapPointData *p, sint32 *tiles, MapPointData *openList) PURE ;
	STDMETHOD_ (BOOL, ProvidesTrade)(THIS_ MapPointData *p) PURE ;

    
    STDMETHOD_ (BOOL, GetGood) (THIS_ MapPointData *p, sint32 *type_good) PURE; 
    STDMETHOD_ (BOOL, GetFood) (THIS_ MapPointData *pos, sint32 *food) PURE;
    STDMETHOD_ (BOOL, GetProd) (THIS_ MapPointData *pos, sint32 *prod) PURE;

    STDMETHOD_ (BOOL, IsNextTo) (THIS_ MapPointData *a, MapPointData *b) PURE; 
 
    STDMETHOD_ (BOOL, GetRoad) (THIS_ MapPointData *pos, sint32 *road_type) PURE; 
    STDMETHOD_ (BOOL, GetConstructingRoad) (THIS_ MapPointData *pos, sint32 *road_type) PURE;

    STDMETHOD_ (BOOL, IsConnectedToACity) (THIS_ MapPointData *pos, sint32 player_id) PURE;
    STDMETHOD_ (sint32, Get_XY_Travel_Distance) (THIS_ const MapPointData &A, const MapPointData &B) PURE;
    STDMETHOD_ (double, Get_XY_Euclidean_Distance_Squared) (THIS_ const MapPointData &A, const MapPointData &B) PURE;
    STDMETHOD_ (void, Get_Army_Types) (THIS_ MapPointData *pos, sint32 types_in_army[k_MAX_ARMY_SIZE], sint32 &num_in_army) PURE;
	STDMETHOD_ (BOOL, HasBonusFoodUnit) (THIS_ MapPointData *pos) PURE;
	STDMETHOD_ (sint32, GetConvertedTo)(THIS_ uint32 u_id, BOOL *is_unknown_id, MapPointData *target_pos) PURE;

	STDMETHOD_ (BOOL, GetSafeFromNukes)(THIS_ MapPointData *target_pos) PURE;
	STDMETHOD_ (BOOL, GetHasCityAirport)(THIS_ MapPointData *target_pos) PURE;
	STDMETHOD_ (BOOL, GetHasForceField)(THIS_ MapPointData *target_pos) PURE;
	STDMETHOD_ (BOOL, GetHasCityWalls)(THIS_ MapPointData *target_pos) PURE;

	STDMETHOD_ (sint32, GetFranchiseOwner)(uint32 u_id, BOOL *is_unknown_id, MapPointData *target_pos) PURE;

	STDMETHOD_ (uint32, GetNumImprovements)(THIS_ const MapPointData &pos) PURE;
	STDMETHOD_ (TERRAIN_IMPROVEMENT, GetImprovementType) (THIS_ const MapPointData &pos, const uint32 & index) PURE;
	STDMETHOD_ (PLAYER_INDEX, GetImprovementOwner) (THIS_ const MapPointData & pos, const uint32 & index) PURE;
	STDMETHOD_ (BOOL, IsSafeFromNukes) (THIS_ const MapPointData & pos) PURE;
	STDMETHOD_ (uint32, GetNumTradeRoutes) (THIS_ const MapPointData & pos) PURE;
	STDMETHOD_ (PLAYER_INDEX, GetTradeRouteSource) (THIS_ const MapPointData & pos, const uint32 index) PURE;
	STDMETHOD_ (PLAYER_INDEX, GetTradeRouteDestination) (THIS_ const MapPointData & pos, const uint32 index) PURE;
	STDMETHOD_ (sint32, GetTradeRouteGoldInReturn) (THIS_ const MapPointData & pos, const uint32 index) PURE;
	STDMETHOD_ (BOOL, IsGoodyHut) (THIS_ const MapPointData & pos) PURE;

    






 

    STDMETHOD_ (BOOL, GetIsChokePoint)(THIS_ const MapPointData &pos) PURE;







 
    STDMETHOD_ (sint16, GetContinent)(THIS_ const MapPointData &pos) PURE;
    
    STDMETHOD_ (sint16, GetMinWaterContinent)(THIS_) PURE;
    STDMETHOD_ (sint16, GetMaxWaterContinent)(THIS_) PURE; 
    STDMETHOD_ (sint16, GetMinLandContinent)(THIS_) PURE; 
    STDMETHOD_ (sint16, GetMaxLandContinent)(THIS_) PURE; 

    STDMETHOD_ (sint32, IsXwrap)(THIS_) PURE; 
    STDMETHOD_ (sint32, IsYwrap)(THIS_) PURE; 

#ifdef CELL_COLOR
    STDMETHOD_ (BOOL, ClearColor) (THIS_) PURE; 
    STDMETHOD_ (BOOL, SetColor) (THIS_  MapPointData *pos, sint32 r, sint32 g, sint32 b) PURE; 
    STDMETHOD_ (BOOL, SetColor) (THIS_ MapPointData *pos, sint32 c) PURE;
#endif 

    STDMETHOD_ (BOOL, CanBuildUnderseaTunnel) (THIS_ MapPointData *pos) PURE;
    STDMETHOD_ (BOOL, HasUnderseaTunnel) (THIS_ MapPointData *pos) PURE;

    STDMETHOD_ (sint32, GetLandContinentSize)(THIS_ sint32 cont_num) PURE;
    STDMETHOD_ (sint32, GetWaterContinentSize)(THIS_ sint32 cont_num) PURE; 

    STDMETHOD_ (BOOL, IsWaterNextTooLand)(THIS_ const sint32 waterc, const sint32 landc) PURE;
    STDMETHOD_ (BOOL, IsLandNextTooWater)(THIS_ const sint32 landc, const sint32 waterc) PURE;
	STDMETHOD_ (sint32, GetCitySlaveCount) (THIS_ MapPointData *pos) PURE;

    STDMETHOD_ (MapValueStruct ***, GetMapValue) (THIS_) PURE; 
    STDMETHOD_ (sint16, GetSettleValue) (THIS_ sint16 x, sint16 y, sint16 z) PURE; 

	STDMETHOD_ (BOOL, IsWormholeVisible)(const sint32 &player_id) PURE;
	STDMETHOD_ (void, GetWormholePos)(MapPointData &pos)PURE;
	STDMETHOD_ (BOOL, HasWormholeProbeEntered)(const MapPointData &pos) PURE;
	STDMETHOD_ (BOOL, HasWormholeProbeReturned)(const MapPointData &pos) PURE;
	STDMETHOD_ (sint32, GetNumCities)(const PLAYER_INDEX &player_index) PURE;
	STDMETHOD_ (sint32, GetNumArmies)(const PLAYER_INDEX &player_index) PURE;
};

#endif
