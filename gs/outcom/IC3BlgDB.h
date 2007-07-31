
#pragma once

#ifndef __IC3BuildingDB_H__
#define __IC3BuildingDB_H__ 1

#include <OBJBASE.H>  


enum BLG_EFFECT { 
    BLG_EFFECT_SILO, 
    BLG_EFFECT_CAPITOL,
    BLG_EFFECT_DEFENSE_BONUS,

    BLG_EFFECT_DEFENSE_BONUS_NO_CITY_WALLS_ATTACKER,
    
    BLG_EFFECT_HAPPY_INCREMENT,
    BLG_EFFECT_CONTENT_AIF,

    BLG_EFFECT_DOUBLE_TELEVAN,
    BLG_EFFECT_REDUCE_WASTE,

    BLG_EFFECT_PRODUCTION_TYPE_PRODUCTION_INCREMENT,
    BLG_EFFECT_PRODUCTION_TYPE_FOOD_INCREMENT,
    BLG_EFFECT_PRODUCTION_TYPE_GOLD_INCREMENT,
    BLG_EFFECT_PRODUCTION_TYPE_KNOWLEDGE_INCREMENT,
    BLG_EFFECT_PRODUCTION_TYPE_ENTERTAINMENT_INCREMENT,
    
    BLG_EFFECT_PRODUCTION_TYPE_PRODUCTION_PERCENT,
    BLG_EFFECT_PRODUCTION_TYPE_FOOD_PERCENT,
    BLG_EFFECT_PRODUCTION_TYPE_GOLD_PERCENT,
    BLG_EFFECT_PRODUCTION_TYPE_KNOWLEDGE_PERCENT,
    BLG_EFFECT_PRODUCTION_TYPE_ENTERTAINMENT_PERCENT,

    BLG_EFFECT_POLLUTION_BONUS,
    BLG_EFFECT_POLLUTION_CLEANER,

    BLG_EFFECT_AFFECTIVE_POP_DEC,
    BLG_EFFECT_NO_UNHAPPY_PEOPLE,
    BLG_EFFECT_NO_HAPPINESS,

    BLG_EFFECT_LOWER_CRIME,
    BLG_EFFECT_PREVENT_CONVER,
    BLG_EFFECT_PREVENT_SLAVERY,
    BLG_EFFECT_LOWER_OVERCROWDING,
    BLG_EFFECT_ALLOW_GRUNTS,
    BLG_EFFECT_LOWER_PEACE_MOVE,
    BLG_EFFECT_GOLD_PER_CITIZEN,
    BLG_EFFECT_PROTECT_FROM_NUKES,
    BLG_EFFECT_PROTECT_FROM_BIO,
    BLG_EFFECT_PROTECT_FROM_NANO,
    BLG_EFFECT_LOWER_POP_POLLUT,
    BLG_EFFECT_TELEVISION,
    BLG_EFFECT_CITY_WALL,
    BLG_EFFECT_AIRPORT,
    BLG_EFFECT_SPACE_LAUNCH,

    BLG_EFFECT_MAX
};


DEFINE_GUID(CLSID_IC3BuildingDB, 
    0xd9db2c40,
    0xfc5e,
    0x11d1,
    0x85, 0xa7, 0x00, 0x60, 0x08, 0x3a, 0x40, 0x82    
);

#undef INTERFACE
#define INTERFACE IC3BuildingDB

DECLARE_INTERFACE_(IC3BuildingDB, IUnknown)
{
 	

    STDMETHOD_ (sint32, GetNumBuildings) (THIS_) PURE; 

    STDMETHOD_(sint32, GetProductionCost) (THIS_ sint32 building_type) PURE;
    STDMETHOD_ (sint32, GetGoldUpkeep) (THIS_ sint32 building_type) PURE;

    STDMETHOD_ (sint32, GetGoldPerCitizen) (THIS_ sint32 building_type) PURE;

	STDMETHOD_ (BOOL, CityCanConstructBuilding) (THIS_ sint32 building_type) PURE;
	STDMETHOD_ (sint32, EnablingDiscovery)(THIS_ sint32 buildingType) PURE ;
	STDMETHOD_ (sint32, GetHappyBonus)(THIS_ sint32 buildingType) PURE ;

	STDMETHOD_ (double, GetScienceBonus)(THIS_ sint32 buildingType) PURE ;
	STDMETHOD_ (double, GetProductionBonus)(THIS_ sint32 buildingType) PURE ;
	STDMETHOD_ (double, GetFoodBonus)(THIS_ sint32 buildingType) PURE ;
	STDMETHOD_ (double, GetGoldBonus)(THIS_ sint32 buildingType) PURE ;
	STDMETHOD_ (double, GetDefenseBonus)(THIS_ sint32 buildingType) PURE ;
    STDMETHOD_ (BOOL, GetIsAirport) (sint32 buildingType) PURE;
    STDMETHOD_ (BOOL, IsSpaceLauncher)(sint32 buildingType) PURE; 
    STDMETHOD_ (BOOL, IsGranary) (sint32 building_type) PURE; 

    STDMETHOD_ (BOOL, IsCapitolBuilding) (THIS_ sint32 buildingType) PURE; 
	STDMETHOD_ (double, GetIncreaseSciencePerPop)(THIS_ const sint32 &buildingType) PURE;

    STDMETHOD_ (void, GetAllEffects) (sint32 blg, double *effect) PURE; 
    STDMETHOD_ (char *, Improvementtype_Int_To_String) (sint32 index) PURE; 
    STDMETHOD_ (sint32, Improvementtype_String_To_Int) (char *building_name) PURE; 

};

#endif __IC3BuildingDB_H__
