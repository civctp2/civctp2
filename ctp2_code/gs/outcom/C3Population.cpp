
#include "c3.h"


#include "C3Population.h"


#include "player.h"
#include "XY_Coordinates.h"
#include "World.h"
#include "civarchive.h"
#include "DB.h"

	extern	enum POPTYPE ;

	extern	World	*g_theWorld ;

	extern	Player	**g_player ;


#include "bset.h"
#include "UnitData.h"
#include "citydata.h"
#include "WonderRecord.h"
#include "Happy.h"








STDMETHODIMP C3Population::QueryInterface(REFIID riid, void **obj)
	{
	*obj = NULL;

	if(IsEqualIID(riid, IID_IUnknown))
		{
		*obj = (IUnknown *)this;
		AddRef();
		return S_OK;
		}
	else if(IsEqualIID(riid, CLSID_IC3Population))
		{
		*obj = (IC3Population*)this;
		AddRef();
		return S_OK;
		}

	return E_NOINTERFACE;
	}









STDMETHODIMP_(ULONG) C3Population::AddRef()
	{
	return ++m_refCount;
	}









STDMETHODIMP_(ULONG) C3Population::Release()
	{
	if (--m_refCount)
		return (m_refCount) ;
	
	delete this ;

	return (0) ;
	}









C3Population::C3Population(sint32 idx)
{
    m_refCount = 0 ;
    Assert(idx >= 0) ;
    Assert(idx < k_MAX_PLAYERS) ;

    m_owner = idx ;
    Assert(g_player[idx]) ;
    m_player = g_player[idx] ;
    Assert(m_player) ;

}








C3Population::C3Population(CivArchive &archive)
{

    Serialize(archive) ;
}









void C3Population::Serialize(CivArchive &archive) 
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









BOOL C3Population::HasPopChanged(uint32 city_id, BOOL *is_unknown_id)
	{
	return (m_player->HasCityPopStarved(city_id, *is_unknown_id) || m_player->HasCityPopGrown(city_id, *is_unknown_id)) ;
	}









BOOL C3Population::HasPopGrown(uint32 city_id, BOOL *is_unknown_id)
	{
	return (m_player->HasCityPopGrown(city_id, *is_unknown_id)) ;
	}









BOOL C3Population::HasPopStarved(uint32 city_id, BOOL *is_unknown_id)
	{
	return (m_player->HasCityPopStarved(city_id, *is_unknown_id)) ;
	}









BOOL C3Population::WasPopExpelled(uint32 city_id, BOOL *is_unknown_id)
	{
	return FALSE;
	}









BOOL C3Population::WasImprovementBuilt(uint32 city_id, BOOL *is_unknown_id)
	{
	return (m_player->WasCityImprovementBuilt(city_id, *is_unknown_id)) ;
	}









BOOL C3Population::WasTerrainImprovementBuilt(uint32 city_id, BOOL *is_unknown_id)
	{
	return (m_player->WasTerrainImprovementBuilt(city_id, *is_unknown_id)) ;
	}









BOOL C3Population::WasHappinessAttacked(uint32 city_id, BOOL *is_unknown_id)
	{
	return (m_player->WasCityHappinessAttacked(city_id, *is_unknown_id)) ;
	}









BOOL C3Population::WasTerrainPolluted(void)
	{
	return (m_player->WasTerrainPolluted()) ;
	}














sint32 C3Population::GetCityPopCount(uint32 city_id, BOOL *is_unknown_id)
	{
	return (m_player->GetCityPopCount(city_id, *is_unknown_id)) ;
	}









sint32 C3Population::GetCitySlaveCount(uint32 city_id, BOOL *is_unknown_id)
	{
	return (m_player->GetCitySlaveCount(city_id, *is_unknown_id)) ;
	}









void C3Population::SetCityPopInCity(uint32 city_id, uint32 pop_idx, BOOL inCity, BOOL *is_unknown_id)
{
}









BOOL C3Population::GetCityPopIsSlave(uint32 city_id, uint32 pop_idx, BOOL *is_unknown_id)
{
	return FALSE;
}









void C3Population::SetCityPopType(uint32 city_id, uint32 pop_idx, uint32 popType, BOOL *is_unknown_id)
{
}

















void C3Population::GetRawHappiness(uint32 city_id, BOOL *is_unknown_id, 
    double *raw_happiness, double *happy_per_entertainer)

{ 
    BSetID *idx = m_player->m_bset_cities_index->Find(city_id) ;

	
	*is_unknown_id = TRUE ;
	if (idx == NULL)
		return;

	*is_unknown_id = FALSE ;

    CityData *the_city = m_player->m_all_cities->Get(idx->GetVal()).GetData()->GetCityData();
    Assert(the_city); 

    *raw_happiness = the_city->m_happy->GetGreedyPopHappiness(*the_city);
    
    *happy_per_entertainer = 0;

}








sint32 C3Population::GetTileFood(uint32 city_id, MapPointData *pos, BOOL *is_unknown_id)
	{
	MapPoint	ipos ;
	
	ipos.Norm2Iso(*pos) ;

	return (m_player->GetTileFood(city_id, ipos, *is_unknown_id)) ;
	}









sint32 C3Population::GetTileProduction(uint32 city_id, MapPointData *pos, BOOL *is_unknown_id)
	{
	MapPoint	ipos ;
	
	ipos.Norm2Iso(*pos) ;

	return (m_player->GetTileProduction(city_id, ipos, *is_unknown_id)) ;
	}









sint32 C3Population::GetTileResource(uint32 city_id, MapPointData *pos, BOOL *is_unknown_id)
	{
	MapPoint	ipos ;
	
	ipos.Norm2Iso(*pos) ;

	return (m_player->GetTileResource(city_id, ipos, *is_unknown_id)) ;
	}









void C3Population::GetCityProjectedFood(uint32 city_id, sint32 *food, BOOL *is_unknown_id)
	{
	m_player->GetCityProjectedFood(city_id, *food, *is_unknown_id) ;
	}









void C3Population::GetCityProjectedTrade(uint32 city_id,  
     BOOL *is_unknown_id, sint32 *projected_gross_gold, sint32 *projected_net_gold)
{
	m_player->GetCityProjectedTrade(city_id, *is_unknown_id, 
        *projected_gross_gold, *projected_net_gold) ;
}









void C3Population::GetCityProjectedTradeFromCell(uint32 city_id, MapPointData *p, sint32 *trade, BOOL *is_unknown_id)
	{
	MapPoint ipos ;
	ipos.Norm2Iso(*p) ;

	m_player->GetProjectedTradeFromCell(city_id, ipos, *trade, *is_unknown_id) ;
	}









void C3Population::GetCityProjectedProduction(uint32 city_id, sint32 *production, BOOL *is_unknown_id)
	{
	m_player->GetCityProjectedProduction(city_id, *production, *is_unknown_id) ;
	}









void C3Population::GetCityRequiredFood(uint32 city_id, sint32 *food, BOOL *is_unknown_id)
	{
	m_player->GetCityRequiredFood(city_id, *food, *is_unknown_id) ;
	}









BOOL C3Population::TryPlacePop(uint32 city_id, uint32 pop_idx, sint32 player_id, MapPointData *p, BOOL *is_unknown_id)
	{
if (
    (p->x < 0) || 
   (g_theWorld->GetXWidth() <= p->x) ||
    (p->y < 0) || 
   (g_theWorld->GetYHeight() <= p->y) ||
    (p->z < 0) || 
   (g_theWorld->GetZHeight() <= p->z))
{
    sint32 TryPlacePop_out_of_bounds=0; 
    Assert(TryPlacePop_out_of_bounds); 
    return FALSE; 
} 

    

		{
		MapPoint ipos ;
		ipos.Norm2Iso(*p) ;

        Assert(m_player); 
        if (NULL == m_player) 
            return FALSE; 

		return FALSE;
		}

	return (FALSE) ;
	}
 








BOOL C3Population::IsPopAllowed(uint32 city_id, uint32 popType, BOOL *is_unknown_id)
	{
	return (m_player->IsPopAllowed(city_id, (POPTYPE)(popType), *is_unknown_id)) ;
	}

double C3Population::GetCityScientistOutput(uint32 city_id, BOOL *is_unknown_id)  
{
    return 0;
}

double C3Population::GetCityGruntOutput(uint32 city_id, BOOL *is_unknown_id)
{
    return 0;
}

double C3Population::GetCityMusicianOutput(uint32 city_id, BOOL *is_unknown_id)
{
    return 0;
}







void C3Population::GetCityScience(uint32 city_id, sint32 *science, BOOL *is_unknown_id)
	{
	m_player->GetCityScience(city_id, *science, *is_unknown_id) ;
	}

double C3Population::GetSlaveHunger ()
{
    return 0.0; 
}

double C3Population::GetCitizensHunger ()
{
    return 0.0; 
}

