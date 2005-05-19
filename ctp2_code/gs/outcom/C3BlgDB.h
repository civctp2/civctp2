










#pragma once

#ifndef __C3BuildingDB_H__
#define __C3BuildingDB_H__ 1

#include "ic3BlgDB.h"

class Player; 
class CivArchive; 

class C3BuildingDB : public IC3BuildingDB
{
	ULONG m_refCount;
	
	
    Player *m_player; 

public:
	
	STDMETHODIMP QueryInterface(REFIID, void **obj);
	STDMETHODIMP_(ULONG) AddRef();
	STDMETHODIMP_(ULONG) Release();

    C3BuildingDB(Player *p); 
    C3BuildingDB(Player *p, CivArchive &archive);
    void Serialize(CivArchive &archive);

    STDMETHODIMP_ (sint32) GetNumBuildings (); 
 
    STDMETHODIMP_(sint32) GetProductionCost (sint32 building_type); 
    STDMETHODIMP_(sint32) GetGoldUpkeep (sint32 building_type);
    STDMETHODIMP_(sint32) GetGoldPerCitizen (sint32 building_type);
    
    STDMETHODIMP_ (BOOL)  CityCanConstructBuilding(sint32 building_type);
	STDMETHODIMP_ (sint32) EnablingDiscovery(sint32 buildingType) ;
	STDMETHODIMP_ (sint32) GetHappyBonus(sint32 buildingType) ;
	STDMETHODIMP_ (double) GetScienceBonus(sint32 buildingType) ;
	STDMETHODIMP_ (double) GetProductionBonus(sint32 buildingType) ;
	STDMETHODIMP_ (double) GetFoodBonus(sint32 buildingType) ;
	STDMETHODIMP_ (double) GetGoldBonus(sint32 buildingType) ;
	STDMETHODIMP_ (double) GetDefenseBonus(sint32 buildingType) ;
    STDMETHODIMP_ (BOOL) GetIsAirport(sint32 buildingType); 
    STDMETHODIMP_ (BOOL) IsSpaceLauncher(sint32 buildingType); 
    STDMETHODIMP_ (BOOL) IsGranary(sint32 building_type); 
    STDMETHODIMP_ (BOOL) IsCapitolBuilding (sint32 buildingType); 
	STDMETHODIMP_ (double) GetIncreaseSciencePerPop(const sint32 &buildingType);

    STDMETHODIMP_ (void) GetAllEffects(sint32 blg, double *effect); 
    STDMETHODIMP_ (char *) Improvementtype_Int_To_String(sint32 index); 
    STDMETHODIMP_ (sint32) Improvementtype_String_To_Int(char *building_name); 

};

#endif __C3BuildingDB_H__

