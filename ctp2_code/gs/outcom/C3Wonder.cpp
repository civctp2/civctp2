
#include "c3.h"


#include "C3Wonder.h"


#include "player.h"
#include "XY_Coordinates.h"
#include "World.h"
#include "civarchive.h"
#include "WonderRecord.h"

#include "StrDB.h"
#include "Unit.h"
#include "UnitData.h"
#include "citydata.h"
#include "Exclusions.h"

#include "BldQue.h"
#include "Exclusions.h"
#include "globals.h"

#include "WonderTracker.h"

#include "wonderutil.h"








STDMETHODIMP C3Wonder::QueryInterface(REFIID riid, void **obj)
	{
	*obj = NULL ;

	if(IsEqualIID(riid, IID_IUnknown))
		{
		*obj = (IUnknown *)this ;
		AddRef() ;

		return (S_OK) ;
		}
	else if(IsEqualIID(riid, CLSID_IC3Wonder))
		{
		*obj = (IC3Wonder*)this ;
		AddRef() ;
		return (S_OK) ;
		}

	return (E_NOINTERFACE) ;
	}









STDMETHODIMP_(ULONG) C3Wonder::AddRef()
	{
	return (++m_refCount) ;
	}








STDMETHODIMP_(ULONG) C3Wonder::Release()
	{
	if(--m_refCount)
		return m_refCount ;

	delete this ;

	return (0) ;
	}









C3Wonder::C3Wonder(sint32 idx)
	{
    m_refCount = 0 ;
    Assert(idx >= 0) ;
    Assert(idx < k_MAX_PLAYERS) ;

    m_owner = idx ;
    Assert(g_player[idx]) ;
    m_player = g_player[idx] ;
    Assert(m_player) ;
	}








C3Wonder::C3Wonder(CivArchive &archive)
	{
    Serialize(archive) ;
	}









void C3Wonder::Serialize(CivArchive &archive) 
	{

    CHECKSERIALIZE

    if (archive.IsStoring())
		{
        archive << m_refCount ;
		archive << m_owner ;
		}
	else
		{
        archive >> m_refCount ;
		archive >> m_owner ;
		m_player = g_player[m_owner] ;
		}

	}









sint32 C3Wonder::GetNumWonders(void)
	{
	return (g_theWonderDB->NumRecords());
	}










BOOL C3Wonder::HasBeenBuilt(sint32 wonder_id)
	{
	return (wonderutil_IsBuilt(wonder_id)) ;
	}

BOOL C3Wonder::WondersAvailable(sint32 player_idx)
{ 
#if 0
    sint32 wonder_type, wonder_num; 

    wonder_num = wonderutil_GetNumRec();

    for (wonder_type=0; wonder_type<wonder_num; wonder_type++) { 
	    if (wonderutil_IsBuilt(wonder_type)) continue; 

        if(wonderutil_IsAvailable(wonder_type, player_idx)) { 
            if(!g_exclusions->IsWonderExcluded(wonder_type)) {     
		        return TRUE;
            }
        }
    }
#endif

    return FALSE; 
}

double C3Wonder::MaxPecentageThisWonderDone(sint32 wonder_type) 
{
    sint32 player_idx; 
    sint32 city_idx, city_num; 
    CityData *the_city=NULL;
    BuildNode *node=NULL;
    double p, max_percent=0.0; 

    for (player_idx=0; player_idx<k_MAX_PLAYERS; player_idx++) { 
        if (!g_player[player_idx]) continue; 

        city_num = g_player[player_idx]->m_all_cities->Num(); 
        for (city_idx=0; city_idx<city_num; city_idx++) { 
            the_city = g_player[player_idx]->m_all_cities->Access(city_idx).GetData()->GetCityData();

            node = the_city->GetBuildQueue()->GetHead();
            if (!node) continue; 
            if (node->m_category != k_GAME_OBJ_TYPE_WONDER) continue;
            if (node->m_type != wonder_type) continue;

            p = the_city->GetStoredCityProduction() / node->m_cost;
            if (max_percent < p) { 
                max_percent = p; 
            }
        } 
    }

    return max_percent; 
}











sint32 C3Wonder::EnablingDiscovery(sint32 wonder_id)
	{
	return (wonderutil_Get(wonder_id)->GetEnableAdvanceIndex()) ;
	}













sint32 C3Wonder::ObsoleteDiscovery(sint32 wonder_id)
	{
	return (wonderutil_Get(wonder_id)->GetObsoleteAdvanceIndex(0)) ;
	}











sint32 C3Wonder::GetOwner(sint32 wonder_id)
	{
	return (wonderutil_GetOwner(wonder_id)) ;
	}


void C3Wonder::GetAllEffects(sint32 idx_wonder, double *effect) 
{ 
    Assert(0 <= idx_wonder); 
    Assert(idx_wonder < g_theWonderDB->NumRecords());
    Assert(effect); 

    sint32 idx_effect; 
    for (idx_effect=0; idx_effect<WONDER_EFFECT_MAX; idx_effect++) {
        effect[idx_effect] = 0.0; 
    } 

    uint64 bit = (uint64(0x1) << idx_wonder); 

	effect[WONDER_EFFECT_EMBASSY] = wonderutil_GetEmbassy(bit, 0);
	effect[WONDER_EFFECT_CLOSE_EMBASSY] = wonderutil_GetCloseEmbassies(bit);
	effect[WONDER_EFFECT_REFORM_CITIES] = wonderutil_GetReformCities(bit);
	effect[WONDER_EFFECT_FREE_SLAVES] = wonderutil_GetFreeSlaves(bit);
	effect[WONDER_EFFECT_GLOBAL_RADAR] = wonderutil_GetGlobalRadar(bit);
	effect[WONDER_EFFECT_SPIES_EVERYWHERE] = wonderutil_GetSpiesEverywhere(bit);
	effect[WONDER_EFFECT_STOP_BIOWAR] = wonderutil_GetProtectFromBiologicalWarfare(bit);
	effect[WONDER_EFFECT_ENABLE_PARKRANGER] = wonderutil_GetParkRangersEnabled(bit);
	effect[WONDER_EFFECT_ALL_POP_CONTENT] = wonderutil_GetAllCitizensContent(bit);
	effect[WONDER_EFFECT_DISABLE_NUKES] = wonderutil_GetNukesEliminated(bit);

	effect[WONDER_EFFECT_REDUCE_READINESS_COST] = wonderutil_GetReadinessCostReduction(bit);
	effect[WONDER_EFFECT_DECREASE_CRIME] = wonderutil_GetDecreaseCrimePercentage(bit);
	effect[WONDER_EFFECT_INCREASE_KNOWLEDGE] = wonderutil_GetIncreaseKnowledgePercentage(bit);
	effect[WONDER_EFFECT_DECREASE_EMPIRE_SIZE] = wonderutil_GetDecreaseEmpireSize(bit);
	effect[WONDER_EFFECT_INCREASE_HAPPY_EMPIRE] = wonderutil_GetIncreaseHappinessEmpire(bit);
	effect[WONDER_EFFECT_INCREASE_CONVERTED_CITY_FEE] = wonderutil_GetIncreaseConvertedCitiesFeePercentage(bit);
	effect[WONDER_EFFECT_TAX_WATER_ROUTES] = wonderutil_GetGoldPerWaterTradeRoute(bit);
	effect[WONDER_EFFECT_TAX_TELEVISION] = wonderutil_GetGoldPerTelevision(bit);
	effect[WONDER_EFFECT_TAX_INTERNATIONAL_ROUTE] = wonderutil_GetGoldPerInternationalTradeRoute(bit);
	effect[WONDER_EFFECT_POLLUTERS_TO_PARKS] = wonderutil_GetPollutersToParks(bit);
	effect[WONDER_EFFECT_REDUCE_WORLD_POLLUTION] = wonderutil_GetReduceWorldPollution(bit);
}

sint32 C3Wonder::GetProductionCost(sint32 wonder_id)
{
    return wonderutil_GetProductionCost(wonder_id);
}













char * C3Wonder::Wonder_Int_To_String(sint32 index)
{
#if 0
	StringId str_id;
	char * Wonder_string;

	
	if (wonderutil_GetNamedItemID(index, str_id))
	{
		
		Wonder_string = (char *) g_theStringDB->GetNameStr(str_id);

	} 

	return Wonder_string;
#else
	return "Mark's Hair Band";
#endif

}

sint32 C3Wonder::Wonder_String_To_Int(char *wonder_name)
{
#if 0
	StringId str_id;
	sint32 index;

	
	if (g_theStringDB->GetStringID(wonder_name, str_id))
	{
		
		if (wonderutil_GetNamedItem(str_id, index))
		{
			
			return index;

		} 

	} 

	Assert(false);
#endif

	return -1;

}

BOOL C3Wonder::CanBuild(sint32 wonder_type)
{

	if(g_exclusions->IsWonderExcluded(wonder_type))
		return FALSE;

	if(!wonderutil_IsAvailable(wonder_type, m_owner))
		return FALSE;

    return TRUE; 
}

inline BOOL C3Wonder::IsObsolete(sint32 wonder_type)
{
	return wonderutil_IsObsolete(wonder_type);
} 

BOOL C3Wonder::IsWormholeDetectorBuilt()
{ 
    return FALSE;
} 
