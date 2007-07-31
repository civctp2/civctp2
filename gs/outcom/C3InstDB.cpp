
#include "c3.h"


#include "C3InstDB.h"


#include "Player.h"
#include "XY_Coordinates.h"
#include "World.h"
#include "CivArchive.h"
#include "TerrainRecord.h"
#include "InstDB.h"
#include "TerrImproveData.h"
#include "AdvanceRecord.h"
#include "XY_Coordinates.h"
#include "World.h"
#include "TerrImprovePool.h"
#include "Cell.h"

extern InstallationDatabase *g_theInstallationDB;
extern World *g_theWorld; 
extern TerrainImprovementPool      *g_theTerrainImprovementPool;

STDMETHODIMP C3InstDB::QueryInterface(REFIID riid, void **obj)
{
	*obj = NULL;

	if(IsEqualIID(riid, IID_IUnknown)) {
		*obj = (IUnknown *)this;
		AddRef();
		return S_OK;
	} else if(IsEqualIID(riid, CLSID_IC3InstDB)) {
		*obj = (IC3InstDB*)this;
		AddRef();
		return S_OK;
	}
	return E_NOINTERFACE;
}

STDMETHODIMP_(ULONG) C3InstDB::AddRef()
{
	return ++m_refCount;
}

STDMETHODIMP_(ULONG) C3InstDB::Release()
{
	if(--m_refCount)
		return m_refCount;
	delete this;
	return 0;
}


C3InstDB::C3InstDB(Player *p)
{
   	m_refCount = 0;
    m_player = p;
} 

C3InstDB::C3InstDB(Player *p, CivArchive &archive)
{
    m_player = p;
    Serialize(archive); 
} 

void C3InstDB::Serialize(CivArchive &archive) 
{
    CHECKSERIALIZE

    if (archive.IsStoring()) { 
        archive << m_refCount; 
    } else { 
        archive >> m_refCount; 
    } 
}

sint32 C3InstDB::GetNumInstallations ()
{
	return  1; 
	
}

BOOL C3InstDB::IsEnabled (sint32 type_inst)
{
    BOOL advanceEnable, advanceObsolete; 
    
    if (0 ) { 
  		advanceEnable = g_theInstallationDB->EnableInstallation(type_inst - 0 );
		advanceObsolete = g_theInstallationDB->Get(type_inst - 0 )->GetObsolete(0);
    } else {
		advanceEnable = 0; 
		advanceObsolete = -1;
    }

    if(advanceEnable >= 0 && !m_player->HasAdvance(advanceEnable))
		return FALSE;

    if(advanceObsolete >= 0 && m_player->HasAdvance(advanceObsolete))
		return FALSE;

    return TRUE; 
}

BOOL C3InstDB::CanBuildHere(sint32 type_inst, sint32 extra_data, MapPointData *pos, 
                            ERR_BUILD_INST *err) 
{ 
	
	return FALSE;
#if 0
    MapPoint ipos; 
    ipos.Norm2Iso(*pos); 


    if (TERRAIN_IMPROVEMENT_INSTALLATION <= type_inst) { 
        if (m_player->CanCreateImprovement(TERRAIN_IMPROVEMENT_INSTALLATION, 
           ipos, type_inst - TERRAIN_IMPROVEMENT_INSTALLATION, FALSE, *err) ==FALSE) { 
            return FALSE; 
        } 
    } else { 
        if (m_player->CanCreateImprovement(TERRAIN_IMPROVEMENT(type_inst), 
           ipos,TERRAIN_TYPES(extra_data), 
           FALSE, *err) == FALSE) { 
            return FALSE; 
        } 
    }

    




    
    if (HasInstallationHere(type_inst, extra_data, pos)) 
        return FALSE; 

    if (TERRAIN_IMPROVEMENT_INSTALLATION <= type_inst) { 
        if (!g_theTerrainImprovementPool->CanHaveImprovement(ipos, TERRAIN_IMPROVEMENT_INSTALLATION,
            type_inst - TERRAIN_IMPROVEMENT_INSTALLATION)) { 
            return FALSE; 
        } 
    } else { 
        if (!g_theTerrainImprovementPool->CanHaveImprovement(ipos, TERRAIN_IMPROVEMENT(type_inst),0)) { 
            return FALSE; 
        } 
    }

    return TRUE; 
#endif
}

BOOL C3InstDB::GetOverwritableInstallation(MapPointData *pos, sint32 *old_inst)
{
    
	return TRUE;
}

BOOL C3InstDB::ConstructInstallation(sint32 type_inst, sint32 extra_data, MapPointData *pos)
{
    MapPoint ipos; 
    ipos.Norm2Iso(*pos); 

#if 0 
    if (TERRAIN_IMPROVEMENT_INSTALLATION <= type_inst) { 
        return m_player->CreateImprovement(TERRAIN_IMPROVEMENT_INSTALLATION, 
           ipos, type_inst - TERRAIN_IMPROVEMENT_INSTALLATION) != TerrainImprovement(0);
    } else 
#endif
	{ 
        return  m_player->CreateImprovement(TERRAIN_IMPROVEMENT(type_inst), 
           ipos,TERRAIN_TYPES(extra_data)) != TerrainImprovement(0);
    }
}


sint32 C3InstDB::GetCost(sint32 type_inst, MapPointData *pos, sint32 extra_data)
{ 
	
	return 10;
}

BOOL C3InstDB::HasInstallationHere(sint32 type_inst, sint32  extra_data, MapPointData *pos) 
{
    MapPoint ipos; 
    ipos.Norm2Iso(*pos); 

#if 0
    if (TERRAIN_IMPROVEMENT_INSTALLATION <= type_inst) { 
        return g_theTerrainImprovementPool->HasImprovement(ipos, 
            TERRAIN_IMPROVEMENT_INSTALLATION, type_inst - TERRAIN_IMPROVEMENT_INSTALLATION);
    } else 
#endif
	{ 
        return g_theTerrainImprovementPool->HasImprovement(ipos, 
            TERRAIN_IMPROVEMENT(type_inst),	TERRAIN_TYPES(extra_data));
    }
}

BOOL C3InstDB::IsInstallationHere() 
{ 
    return FALSE; 
} 

BOOL C3InstDB::InstallationWillReplace(sint32 type_inst) 
{
    return FALSE; 
}

double C3InstDB::GetProductionDelta(sint32 type_inst, MapPointData *pos)
{
	
	return 10;
}

double C3InstDB::GetFoodDelta(sint32 type_inst, MapPointData *pos)
{
	
	return 10;
}


double C3InstDB::GetFoodBonus(sint32 type_inst, MapPointData *pos)
{

    MapPoint ipos; 
    ipos.Norm2Iso(*pos); 

    sint32 tt = g_theWorld->GetTerrainType(ipos); 

    return GetFoodBonus(type_inst, tt);
}

double C3InstDB::GetProductionBonus(sint32 type_inst, MapPointData *pos)
{
    MapPoint ipos; 
    ipos.Norm2Iso(*pos); 

    TERRAIN_TYPES tt = g_theWorld->GetTerrainType(ipos); 

    return GetProductionBonus(type_inst, tt); 
}








double C3InstDB::GetFoodBonus(sint32 installation_type, sint32 terrainType)
{
	
	return 10;
}









double C3InstDB::GetProductionBonus(sint32 installation_type, sint32 terrainType)
{
    
	
	return 0;
}









double C3InstDB::GetInstFoodDelta(sint32 instType, sint32 terrainType)
{
	
	return 0;
}

	
double C3InstDB::GetMovementDelta(sint32 type_inst, MapPointData *pos)
{
	
	return 0;
}

double C3InstDB::GetInstMovement(sint32 type_inst)
{
    switch (type_inst) { 
    case 3: return 33;
    case 4: return 20; 
    case 5: return 10; 
    default:
        return 100; 
    }
}

double C3InstDB::GetDefenseDelta(sint32 type_inst, MapPointData *pos)
{
	
	return 0;
}














double C3InstDB::GetInstDefenseDelta(sint32 type_inst)
	{
    
	

	return (0.0) ;
	}


sint32 C3InstDB::GetVisionRange(sint32 type_inst, MapPointData *pos)
{
   MapPoint ipos; 
   ipos.Norm2Iso(*pos); 

#if 0 
    if (TERRAIN_IMPROVEMENT_INSTALLATION <= type_inst) { 
        return sint32(g_theInstallationDB->GetVisionRange(type_inst - TERRAIN_IMPROVEMENT_INSTALLATION));
     else 
#endif
	{
        return 0;
    }
}














sint32 C3InstDB::GetInstVisionRange(sint32 type_inst)
	{
#if 0 
    if (type_inst >= TERRAIN_IMPROVEMENT_INSTALLATION)
        return ((sint32)(g_theInstallationDB->GetVisionRange(type_inst - TERRAIN_IMPROVEMENT_INSTALLATION))) ;
#endif

	return (0) ;
	}













sint32 C3InstDB::EnablingDiscovery(sint32 type_inst)
	{
#if 0
    if (TERRAIN_IMPROVEMENT_INSTALLATION <= type_inst)
		return (g_theInstallationDB->Get(type_inst - TERRAIN_IMPROVEMENT_INSTALLATION)->GetEnabling()) ;
	else
#endif
		return 0; 
	

	}


uint32 C3InstDB::GetVisibilityClasses(sint32 type_inst, MapPointData *pos)
{
   MapPoint ipos; 
   ipos.Norm2Iso(*pos); 

#if 0
   if (TERRAIN_IMPROVEMENT_INSTALLATION <= type_inst) { 
        return g_theInstallationDB->GetVisibilityClasses(type_inst - TERRAIN_IMPROVEMENT_INSTALLATION);
    } else 
#endif
   { 
        return 0;
    }
}

BOOL C3InstDB::GetIsAirfield(sint32 type_inst, MapPointData *pos)
{
#if 0
    if (TERRAIN_IMPROVEMENT_INSTALLATION <= type_inst) { 
        return g_theInstallationDB->IsAirfield(type_inst - TERRAIN_IMPROVEMENT_INSTALLATION);
    } else 
#endif
	{ 
        return FALSE;
    }
}

BOOL C3InstDB::GetIsFort(sint32 type_inst, MapPointData *pos)
{
#if 0
    if (TERRAIN_IMPROVEMENT_INSTALLATION <= type_inst) { 
        return g_theInstallationDB->IsFort(type_inst - TERRAIN_IMPROVEMENT_INSTALLATION);
    } else 
#endif
	{ 
        return FALSE;
    }
}













sint32 C3InstDB::GetAttackValue(sint32 type_inst, MapPointData *pos)
{
#if 0
    if (TERRAIN_IMPROVEMENT_INSTALLATION <= type_inst) { 
        return g_theInstallationDB->GetAttack(type_inst - TERRAIN_IMPROVEMENT_INSTALLATION);
    } else 
#endif
	{ 
        return FALSE;
    }
}

sint32 C3InstDB::GetFirepowerValue(sint32 type_inst, MapPointData *pos)
{

#if 0
    if (TERRAIN_IMPROVEMENT_INSTALLATION <= type_inst) { 
        return g_theInstallationDB->GetFirepower(type_inst - TERRAIN_IMPROVEMENT_INSTALLATION);
    } else 
#endif
	{ 
        return FALSE;
    }
}


BOOL C3InstDB::IsUnderseaTunnel(sint32 type_inst)
{

#if 0
    if (TERRAIN_IMPROVEMENT_INSTALLATION <= type_inst) { 
        return FALSE;
    } else 
#endif
	{ 
        return FALSE; 
    }
}


