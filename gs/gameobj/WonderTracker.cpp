

#include "c3.h"
#include "WonderTracker.h"
#include "CivArchive.h"
#include "Player.h"
#include "network.h"
#include "net_info.h"
#include "Unit.h"
#include "UnitDynArr.h"
#include "UnitData.h"
#include "CityData.h"
#include "WonderRecord.h"
#include "wonderutil.h"
#include "UnitPool.h"

extern sint32 g_saveFileVersion;

WonderTracker *g_theWonderTracker = NULL;

WonderTracker::WonderTracker()
{
    sint32 i;

    m_builtWonders = 0;
    for(i=0; i<k_MAX_PLAYERS; i++)
        m_buildingWonders[i] = (uint64) 0x0;
	m_globeSatFlags = 0;
}

void WonderTracker::Serialize(CivArchive &archive)
{
	if(archive.IsStoring()) {
		archive << m_builtWonders;
        archive.Store((uint8*)m_buildingWonders, k_MAX_PLAYERS * sizeof(uint64));
		archive << m_globeSatFlags;
	} else {
		archive >> m_builtWonders;
		
		if (g_saveFileVersion >= 58) {
			archive.Load((uint8*)m_buildingWonders, k_MAX_PLAYERS * sizeof(uint64));
			if(g_saveFileVersion >= 63) {
				archive >> m_globeSatFlags;
			} else {
				m_globeSatFlags = 0;
			}
		}
		else {
			
			sint32 tmp[k_MAX_PLAYERS];
			archive.Load((uint8*)tmp, k_MAX_PLAYERS * sizeof(sint32));
		    for(int i=0; i<k_MAX_PLAYERS; i++)
		        m_buildingWonders[i] = (uint64) 0x0;
		}
	}
}

BOOL WonderTracker::HasWonderBeenBuilt(sint32 which)
{
	return (m_builtWonders & ((uint64)1 << (uint64)which)) != 0;
}

sint32 WonderTracker::WhoOwnsWonder(sint32 which)
{
	if(!HasWonderBeenBuilt(which))
		return -1;

	sint32 i;
	for(i = 0; i < k_MAX_PLAYERS; i++) {
		if(g_player[i]) {
			if(g_player[i]->m_builtWonders & ((uint64)1 << (uint64)which)) {
				return i;
			}
		}
	}
	return -1;
}

void WonderTracker::AddBuilt(sint32 which)
{
	m_builtWonders |= (uint64)1 << (uint64)which;
	if(g_network.IsHost()) {
		g_network.Enqueue(new NetInfo(NET_INFO_CODE_BUILT_WONDERS,
									  (uint32)(m_builtWonders & 0xffffffff),
									  (uint32)(m_builtWonders >> (uint64)32)));
	}
}

void WonderTracker::SetBuiltWonders(uint64 built)
{
	m_builtWonders = built;
}

BOOL WonderTracker::GetCityWithWonder(sint32 which, Unit &city)
{
	sint32 p;
	for(p = 0; p < k_MAX_PLAYERS; p++) {
		if(!g_player[p])
			continue;
		sint32 c;
		for(c = g_player[p]->m_all_cities->Num() - 1; c >= 0; c--) {
			if(g_player[p]->m_all_cities->Access(c).GetData()->GetCityData()->GetBuiltWonders() & ((uint64)1 << which)) {

				city = g_player[p]->m_all_cities->Access(c);
				return TRUE;
			}
		}
	}
	return FALSE;
}
void WonderTracker::SetBuildingWonder(sint32 which, PLAYER_INDEX who)
{
	
	
	m_buildingWonders[who] |= (uint64)1 << (uint64)which;
}


void WonderTracker::ClearBuildingWonder(sint32 which, PLAYER_INDEX who)
{
	m_buildingWonders[who] &= ~((uint64)1 << (uint64)which);
}


BOOL WonderTracker::IsBuildingWonder(sint32 which, PLAYER_INDEX who)
{
	
	
	return ((m_buildingWonders[who] & ((uint64)1 << (uint64)which)) != 0);
}


void WonderTracker::RecomputeIsBuilding(const PLAYER_INDEX who)
{
	
	
	
    m_buildingWonders[who] = (uint64) 0x0;

	Unit city;
	sint32 category;
	sint32 num_cities = g_player[who]->m_all_cities->Num();
	for (sint32 i = 0; i < num_cities; i++)
	{
		city = g_player[who]->m_all_cities->Access(i);
		Assert( g_theUnitPool->IsValid(city) );
		Assert( city->GetCityData() != NULL );
		
		if(city.CD() && city.CD()->GetBuildQueue()->GetHead())
		{
			category = city.CD()->GetBuildQueue()->GetHead()->m_category;
			if (category == k_GAME_OBJ_TYPE_WONDER)
			{
				SetBuildingWonder(city.CD()->GetBuildQueue()->GetHead()->m_type, who);
			}
		}
	}
}
