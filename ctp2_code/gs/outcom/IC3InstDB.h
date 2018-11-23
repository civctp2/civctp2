#ifdef HAVE_PRAGMA_ONCE
#pragma once
#endif
#ifndef __IC3InstDB_H__
#define __IC3InstDB_H__ 1

#include <OBJBASE.H>

#include "TerrImproveData.h"

DEFINE_GUID(CLSID_IC3InstDB,
    0x08420cc0,
    0xfee4,
    0x11d1,
    0x85, 0xa7, 0x00, 0x60, 0x08, 0x3a, 0x40, 0x82
);

#undef INTERFACE
#define INTERFACE IC3InstDB

struct MapPointData;
enum ERR_BUILD_INST;

DECLARE_INTERFACE_(IC3InstDB, IUnknown)
{


    STDMETHOD_ (sint32, GetNumInstallations) (THIS_) PURE;
    STDMETHOD_ (BOOL, IsEnabled) (THIS_ sint32 type_inst) PURE;
	STDMETHOD_ (sint32, EnablingDiscovery)(THIS_ sint32 type_inst) PURE ;
    STDMETHOD_ (BOOL, ConstructInstallation) (THIS_ sint32 inst_type, sint32 extra_data,
        MapPointData *pos) PURE;
    STDMETHOD_ (sint32, GetCost) (THIS_  sint32 type_inst, MapPointData *pos, sint32 extra_data) PURE;
    STDMETHOD_ (BOOL, CanBuildHere) (THIS_ sint32 type_inst, sint32 extra_data, MapPointData *pos,
        ERR_BUILD_INST *err)PURE;
    STDMETHOD_ (BOOL, GetOverwritableInstallation) (THIS_ MapPointData *w, sint32 *old_inst) PURE;

    STDMETHOD_ (BOOL, HasInstallationHere) (sint32 type_inst, sint32 extra_data, MapPointData *pos) PURE;
    STDMETHOD_ (BOOL, IsInstallationHere) () PURE;
    STDMETHOD_ (BOOL, InstallationWillReplace)(THIS_ sint32 type_inst) PURE;

    STDMETHOD_ (double, GetFoodBonus)(THIS_ sint32 type_inst, MapPointData *pos) PURE;
    STDMETHOD_ (double, GetProductionBonus)( THIS_ sint32 type_inst, MapPointData *pos)PURE;
    STDMETHOD_ (double, GetFoodBonus) (THIS_ sint32 improveType, sint32 terrainType) PURE;
    STDMETHOD_ (double, GetProductionBonus) (THIS_ sint32 installation_type, sint32 terrainType) PURE;

    STDMETHOD_ (double, GetProductionDelta) (THIS_ sint32 type_inst, MapPointData *pos) PURE;
    STDMETHOD_ (double, GetFoodDelta) (THIS_ sint32 type_inst, MapPointData *pos) PURE;
	STDMETHOD_ (double, GetInstFoodDelta)(THIS_ sint32 instType, sint32 terrainType) PURE ;
    STDMETHOD_ (double, GetMovementDelta) (THIS_ sint32 type_inst, MapPointData *pos) PURE;
    STDMETHOD_ (double, GetInstMovement) (THIS_ sint32 type_inst) PURE;
    STDMETHOD_ (double, GetDefenseDelta)(THIS_ sint32 type_inst, MapPointData *pos) PURE;
	STDMETHOD_ (double, GetInstDefenseDelta)(THIS_ sint32 type_inst) PURE ;

    STDMETHOD_ (sint32, GetVisionRange) (THIS_ sint32 type_inst, MapPointData *pos) PURE;
    STDMETHOD_ (sint32, GetInstVisionRange) (THIS_ sint32 type_inst) PURE ;
    STDMETHOD_ (uint32, GetVisibilityClasses) (THIS_ sint32 type_inst, MapPointData *pos) PURE;
    STDMETHOD_ (BOOL, GetIsAirfield) (THIS_ sint32 type_inst, MapPointData *pos) PURE;
    STDMETHOD_ (BOOL, GetIsFort) (THIS_ sint32 type_inst, MapPointData *pos) PURE;

    STDMETHOD_ (sint32, GetAttackValue)(THIS_ sint32 type_inst, MapPointData *pos) PURE;
    STDMETHOD_ (sint32, GetFirepowerValue)(THIS_ sint32 type_inst, MapPointData *pos) PURE;

    STDMETHOD_ (BOOL, IsUnderseaTunnel) (THIS_ sint32 inst_type) PURE;

};

#endif __IC3InstDB_H__
