










#pragma once

#ifndef __C3InstDB_H__
#define __C3InstDB_H__ 1

#include "ic3InstDB.h"

class Player; 
class CivArchive; 
enum ERR_BUILD_INST;

class C3InstDB : public IC3InstDB
{
	ULONG m_refCount;
	
	
    Player *m_player; 

public:
	
	STDMETHODIMP QueryInterface(REFIID, void **obj);
	STDMETHODIMP_(ULONG) AddRef();
	STDMETHODIMP_(ULONG) Release();

    C3InstDB(Player *p); 
    C3InstDB(Player *p, CivArchive &archive);
    void Serialize(CivArchive &archive);

    STDMETHODIMP_ (sint32) GetNumInstallations ();
    STDMETHODIMP_ (BOOL) IsEnabled (sint32 type_inst); 
	STDMETHODIMP_ (sint32) EnablingDiscovery(sint32 type_inst) ;
    STDMETHODIMP_ (BOOL) ConstructInstallation(sint32 inst_type, sint32 extra_data, MapPointData *pos);
    STDMETHODIMP_ (sint32) GetCost(sint32 type_inst, MapPointData *pos, sint32 extra_data);
    STDMETHODIMP_ (BOOL) CanBuildHere(sint32 type_inst, sint32 extra_data, MapPointData *pos, 
        ERR_BUILD_INST *err);
    STDMETHODIMP_ (BOOL) GetOverwritableInstallation(MapPointData *w, sint32 *old_inst); 

    STDMETHODIMP_ (BOOL) HasInstallationHere (sint32 type_inst, sint32 extra_data, MapPointData *pos);
    STDMETHODIMP_ (BOOL) IsInstallationHere(); 
    STDMETHODIMP_ (BOOL) InstallationWillReplace(sint32 type_inst);

    STDMETHODIMP_ (double) GetFoodBonus(sint32 type_inst, MapPointData *pos);
    STDMETHODIMP_ (double) GetProductionBonus(sint32 type_inst, MapPointData *pos);
    STDMETHODIMP_ (double) GetFoodBonus(sint32 improveType, sint32 terrainType);
    STDMETHODIMP_ (double) GetProductionBonus(sint32 installation_type, sint32 terrainType);

    STDMETHODIMP_ (double) GetProductionDelta(sint32 type_inst, MapPointData *pos);
    STDMETHODIMP_ (double) GetFoodDelta(sint32 type_inst, MapPointData *pos);
	STDMETHODIMP_ (double) GetInstFoodDelta(sint32 instType, sint32 terrainType) ;
    STDMETHODIMP_ (double) GetMovementDelta(sint32 type_inst, MapPointData *pos);
    STDMETHODIMP_ (double) GetInstMovement(sint32 type_inst); 
    STDMETHODIMP_ (double) GetDefenseDelta(sint32 type_inst, MapPointData *pos);
	STDMETHODIMP_ (double) GetInstDefenseDelta(sint32 type_inst) ;
    
    STDMETHODIMP_ (sint32) GetVisionRange(sint32 type_inst, MapPointData *pos);
    STDMETHODIMP_ (sint32) GetInstVisionRange(sint32 type_inst) ;
    STDMETHODIMP_ (uint32) GetVisibilityClasses(sint32 type_inst, MapPointData *pos);
    STDMETHODIMP_ (BOOL) GetIsAirfield(sint32 type_inst, MapPointData *pos);
    STDMETHODIMP_ (BOOL) GetIsFort(sint32 type_inst, MapPointData *pos);
    STDMETHODIMP_ (sint32) GetAttackValue(sint32 type_inst, MapPointData *pos); 
    STDMETHODIMP_ (sint32) GetFirepowerValue(sint32 type_inst, MapPointData *pos); 

    STDMETHODIMP_ (BOOL) IsUnderseaTunnel(sint32 inst_type); 

};

#endif __C3InstDB_H__

