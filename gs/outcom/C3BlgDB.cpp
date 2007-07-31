
#include "c3.h"



#include "C3BlgDB.h"

#include "Globals.h"
#include "C3GameState.h"


#include "Player.h"
#include "CivArchive.h"
#include "BuildingRecord.h"
#include "StrDB.h"
#include "buildingutil.h"

extern StringDB *g_theStringDB;


STDMETHODIMP C3BuildingDB::QueryInterface(REFIID riid, void **obj)
{
	*obj = NULL;

	if(IsEqualIID(riid, IID_IUnknown)) {
		*obj = (IUnknown *)this;
		AddRef();
		return S_OK;
	} else if(IsEqualIID(riid, CLSID_IC3BuildingDB)) {
		*obj = (IC3BuildingDB*)this;
		AddRef();
		return S_OK;
	}
	return E_NOINTERFACE;
}

STDMETHODIMP_(ULONG) C3BuildingDB::AddRef()
{
	return ++m_refCount;
}

STDMETHODIMP_(ULONG) C3BuildingDB::Release()
{
	if(--m_refCount)
		return m_refCount;
	delete this;
	return 0;
}

C3BuildingDB::C3BuildingDB(Player *p)
{
  	m_refCount = 0;
    m_player = p;
} 

C3BuildingDB::C3BuildingDB(Player *p, CivArchive &archive)
{
    m_player = p;
    Serialize(archive); 
} 

void C3BuildingDB::Serialize(CivArchive &archive) 
{
    CHECKSERIALIZE

    if (archive.IsStoring()) { 
        archive << m_refCount; 
    } else { 
        archive >> m_refCount; 
    } 
}


sint32 C3BuildingDB::GetNumBuildings ()
{
   return g_theBuildingDB->NumRecords();
}

sint32 C3BuildingDB::GetProductionCost(sint32 building_type)
{
   return buildingutil_GetProductionCost(building_type);
} 

sint32 C3BuildingDB::GetGoldUpkeep (sint32 building_type)
{
    return buildingutil_GetBlgUpkeep(building_type);
}

sint32 C3BuildingDB::GetGoldPerCitizen (sint32 building_type)
{
    return buildingutil_GetGoldPerCitizen(building_type);
}

BOOL C3BuildingDB::CityCanConstructBuilding(sint32 building_type)
{ 
#if 0
    sint32 e = buildingutil_GetEnabling(building_type); 
    sint32 o;
	BOOL isObsolete = FALSE;
	const ImproveRecord *rec = g_theBuildingDB->Get(building_type);
	for(o = 0; o < rec->GetNumObsolete(); o++) {
		if(m_player->HasAdvance(rec->GetObsolete(o))) {
			return FALSE;
		}
	}

    if (o == -1) {    
        if (e == -1) { 
            return TRUE; 
        } else { 
         	return m_player->HasAdvance(e);
        }
    } else { 
        if (e == -1) {  
            return TRUE; 
        } else { 
         	return m_player->HasAdvance(e);
        }
    }
#else
	return FALSE;
#endif
}














sint32 C3BuildingDB::GetHappyBonus(sint32 buildingType)
	{
	sint32	bonus ;

	buildingutil_GetHappinessIncrement((uint64)(1)<<buildingType, bonus, m_player->m_owner) ;

	return (bonus) ;
	}














double C3BuildingDB::GetScienceBonus(sint32 buildingType)
	{
	return 0;
	}













double C3BuildingDB::GetGoldBonus(sint32 buildingType)
	{
	return 0;
	}














double C3BuildingDB::GetDefenseBonus(sint32 buildingType)
	{
	return 0;
	}














double C3BuildingDB::GetProductionBonus(sint32 buildingType)
	{
	return 0;
	}













double C3BuildingDB::GetFoodBonus(sint32 buildingType)
	{
	return 0;
	}












sint32 C3BuildingDB::EnablingDiscovery(sint32 buildingType)
	{
	return 0;
	}

BOOL C3BuildingDB::GetIsAirport(sint32 buildingType)
{
    return 0;
}


BOOL C3BuildingDB::IsSpaceLauncher(sint32 buildingType)
{
    return 0;
}

BOOL C3BuildingDB::IsGranary(sint32 building_type)
{
	return FALSE;
}

BOOL C3BuildingDB::IsCapitolBuilding (sint32 buildingType)
{
    uint64 bit = uint64(0x1) << buildingType; 
     
    return buildingutil_GetDesignatesCapitol(bit);
}

double C3BuildingDB::GetIncreaseSciencePerPop(const sint32 &buildingType)
{
    uint64 bit = uint64(0x1) << buildingType; 
     
    return buildingutil_GetIncreaseSciencePerPop(bit);
}


void C3BuildingDB::GetAllEffects(sint32 blg, double *effect) 

{ 
#if 0
    sint32 idx_fx; 

    for (idx_fx = 0; idx_fx<BLG_EFFECT_MAX; idx_fx++){ 
        effect[idx_fx] = 0.0; 
    }

    uint64 bit = uint64(0x1) << uint64(blg); 

    sint32 tmpint;
    sint32 tmpint2; 
    double tmpdouble; 
    buildingutil_GetSilo(bit, tmpint2, tmpint);
    if (tmpint2) { 
        effect[BLG_EFFECT_SILO] = tmpint; 
    }






    
    effect[BLG_EFFECT_CAPITOL] = buildingutil_GetDesignatesCapitol(bit);
       
    buildingutil_GetDefendersBonus(bit, tmpdouble);
    effect[BLG_EFFECT_DEFENSE_BONUS] = tmpdouble; 
    effect[BLG_EFFECT_DEFENSE_BONUS_NO_CITY_WALLS_ATTACKER] = buildingutil_GetDefendersBonusNoCityWalls(bit);
    
    buildingutil_GetHappinessIncrement(bit, tmpint, m_player->m_owner);
    effect[BLG_EFFECT_HAPPY_INCREMENT] = tmpint; 

    buildingutil_GetContentAIF(bit, tmpint);
    effect[BLG_EFFECT_CONTENT_AIF]=tmpint; 



    buildingutil_GetDoubleTelevangelism(bit, tmpint);
    effect[BLG_EFFECT_DOUBLE_TELEVAN]=tmpint; 

    buildingutil_GetReduceWaste(bit, tmpdouble);
    effect[BLG_EFFECT_REDUCE_WASTE]=tmpdouble; 


    sint32 prod[PRODUCTION_TYPE_MAX];
    buildingutil_GetProductionIncrement(bit, prod);

    effect[BLG_EFFECT_PRODUCTION_TYPE_PRODUCTION_INCREMENT] = 
        prod[PRODUCTION_TYPE_PRODUCTION];
    effect[BLG_EFFECT_PRODUCTION_TYPE_FOOD_INCREMENT] = 
        prod[PRODUCTION_TYPE_FOOD];
    effect[BLG_EFFECT_PRODUCTION_TYPE_GOLD_INCREMENT] = 
        prod[PRODUCTION_TYPE_GOLD];
    effect[BLG_EFFECT_PRODUCTION_TYPE_KNOWLEDGE_INCREMENT] = 
        prod[PRODUCTION_TYPE_KNOWLEDGE];
    effect[BLG_EFFECT_PRODUCTION_TYPE_ENTERTAINMENT_INCREMENT] = 
        prod[PRODUCTION_TYPE_ENTERTAINMENT];

    double dprod[PRODUCTION_TYPE_MAX]; 
    buildingutil_GetProductionPercent(bit, dprod);

    effect[BLG_EFFECT_PRODUCTION_TYPE_PRODUCTION_PERCENT] =  
        dprod[PRODUCTION_TYPE_PRODUCTION];
    effect[BLG_EFFECT_PRODUCTION_TYPE_FOOD_PERCENT] =  
        dprod[PRODUCTION_TYPE_FOOD];
    effect[BLG_EFFECT_PRODUCTION_TYPE_GOLD_PERCENT] =  
        dprod[PRODUCTION_TYPE_GOLD];
    effect[BLG_EFFECT_PRODUCTION_TYPE_KNOWLEDGE_PERCENT] =  
        dprod[PRODUCTION_TYPE_KNOWLEDGE];
    effect[BLG_EFFECT_PRODUCTION_TYPE_ENTERTAINMENT_PERCENT] =  
        dprod[PRODUCTION_TYPE_ENTERTAINMENT];


    effect[BLG_EFFECT_POLLUTION_BONUS] = buildingutil_GetBuildingsPollutionBonus(bit);
    effect[BLG_EFFECT_POLLUTION_CLEANER] = buildingutil_GetBuildingsPollutionCleaner(bit);

    buildingutil_GetAffectivePopDec(bit, tmpint);
    effect[BLG_EFFECT_AFFECTIVE_POP_DEC]=tmpint;

    effect[BLG_EFFECT_NO_UNHAPPY_PEOPLE] = buildingutil_GetNoUnhappyPeople(bit);
    effect[BLG_EFFECT_NO_HAPPINESS] = buildingutil_GetNoHappiness(bit); 

    effect[BLG_EFFECT_LOWER_CRIME] = buildingutil_GetLowerCrime(bit);
    effect[BLG_EFFECT_PREVENT_CONVER] = buildingutil_GetPreventConversion(bit);
    effect[BLG_EFFECT_PREVENT_SLAVERY] = buildingutil_GetPreventSlavery(bit);
    effect[BLG_EFFECT_LOWER_OVERCROWDING] = buildingutil_GetLowerOvercrowding(bit);	
    effect[BLG_EFFECT_ALLOW_GRUNTS] = buildingutil_GetAllowGrunts(bit);
    effect[BLG_EFFECT_LOWER_PEACE_MOVE] = buildingutil_GetLowerPeaceMovement(bit);
    effect[BLG_EFFECT_GOLD_PER_CITIZEN] = buildingutil_GetGoldPerCitizen(bit);
    effect[BLG_EFFECT_PROTECT_FROM_NUKES] = buildingutil_GetProtectFromNukes(bit); 
    effect[BLG_EFFECT_PROTECT_FROM_BIO] = buildingutil_GetProtectFromBioAgents(bit);
    effect[BLG_EFFECT_PROTECT_FROM_NANO] = buildingutil_GetProtectFromNanoVirus(bit);
    effect[BLG_EFFECT_LOWER_POP_POLLUT] = buildingutil_GetLowerPollutionFromPopulation(bit); 
    effect[BLG_EFFECT_TELEVISION] = buildingutil_GetTelevision(bit);
    effect[BLG_EFFECT_CITY_WALL] = buildingutil_GetCityWalls(bit);
    effect[BLG_EFFECT_AIRPORT] = buildingutil_GetAirport(bit);    
    effect[BLG_EFFECT_SPACE_LAUNCH] = buildingutil_GetSpaceLaunch(bit);
#endif
}


char * C3BuildingDB::Improvementtype_Int_To_String(sint32 index)
{
#if 0
	StringId str_id;
	char * Improvementtype_string;

	
	if (buildingutil_GetNamedItemID(index, str_id))
	{
		
		Improvementtype_string = (char *) g_theStringDB->GetNameStr(str_id);

	} 

	return Improvementtype_string;
#endif
	return "";
}

sint32 C3BuildingDB::Improvementtype_String_To_Int(char *building_name)
{
#if 0
	StringId str_id;
	sint32 index;

	
	if (g_theStringDB->GetStringID(building_name, str_id))
	{
		
		if (buildingutil_GetNamedItem(str_id, index))
		{
			
			return index;

		} 

	} 

	Assert(false);

#endif
	return -1;
}


