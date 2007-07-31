

#include "c3.h"

#include "Cont.h"


#include "globals.h"
#include "CivArchive.h"
#include "IC3World.h"

#include "BSet.h"
#include "DynArr.h"
#include "FlatPtr.h"


#include "IC3GameState.h"
#include "AiMain.h"
#include "AiCell.h"


#include "ArmyAgent.h"
#include "CityAgent.h"

#include "CityTree.h"
#include "AiMap.h"





#define MINIMUM_OCEAN_SIZE 25

Continents::Continents(AiMain *ai)
{ 
    InitMemory(ai);
} 

void Continents::InitMemory(AiMain *ai)
{
    m_min_land_cont = ai->m_world->GetMinLandContinent(); 
    m_num_land_cont = ai->m_world->GetMaxLandContinent() - m_min_land_cont;

    if (m_num_land_cont < 1){
        m_num_land_cont = 1; 
    }
    m_land_size = new sint32[m_num_land_cont];
    m_land_cities = new sint32[m_num_land_cont];
    m_land_num_next_to_edge_cities = new sint32[m_num_land_cont];
    m_land_armies = new sint32[m_num_land_cont];
    m_land_settlers = new sint32[m_num_land_cont]; 
    m_land_space_launchers = new sint32[m_num_land_cont];
 
    m_min_water_cont = ai->m_world->GetMinWaterContinent(); 
    m_num_water_cont = ai->m_world->GetMaxWaterContinent() - m_min_water_cont;   
    if (m_num_water_cont < 1) { 
        m_num_water_cont = 1; 
    } 

    m_water_next_too_land = new DynamicArray<DAsint32>(m_num_water_cont);
    sint32 idx_water;
    for(idx_water=0; idx_water<m_num_water_cont; idx_water++) {
        m_water_next_too_land->ExtendByOne(); 
    }
        
    m_water_size = new sint32[m_num_water_cont];
    m_water_cities = new sint32[m_num_water_cont];
    m_water_num_next_to_edge_cities  = new sint32[m_num_water_cont];
    m_water_armies = new sint32[m_num_water_cont];
    m_water_space_launchers  = new sint32[m_num_water_cont];
   
    m_land_next_too_water = new DynamicArray<DAsint32>(m_num_land_cont);
    sint32 idx_land; 

    for(idx_land=0; idx_land<m_num_land_cont; idx_land++) {
        m_land_next_too_water->ExtendByOne(); 
    }

    m_mst = new CityTree*[m_num_land_cont];
    for (idx_land=0; idx_land<m_num_land_cont; idx_land++) { 
        m_mst[idx_land]=NULL;
    } 

	Clear(); 
}

Continents::Continents(CivArchive &archive)  
{
    Serialize(archive); 

	m_mst = new CityTree *[m_num_land_cont]; 
    sint32 idx_land; 
    for (idx_land=0; idx_land<m_num_land_cont; idx_land++) { 
        m_mst[idx_land]=NULL;
    } 
} 

Continents::~Continents()
{
    CleanupMemory(); 
} 

void Continents::CleanupMemory()
{ 
    delete[] m_land_size;
    m_land_size = NULL; 
    delete[] m_land_cities; 
    m_land_cities = NULL; 
    delete[] m_land_num_next_to_edge_cities;
    m_land_num_next_to_edge_cities = NULL; 
    delete[] m_land_armies; 
    m_land_armies = NULL; 
    delete[] m_land_settlers; 
    m_land_settlers = NULL; 
    delete[] m_land_space_launchers; 
    m_land_space_launchers = NULL; 
 
    delete[] m_water_size;
    m_water_size = NULL; 
    delete[] m_water_cities;
    m_water_cities = NULL; 
    delete[] m_water_num_next_to_edge_cities;
    m_water_num_next_to_edge_cities = NULL; 
    delete[] m_water_armies; 
    m_water_armies = NULL; 
    delete[] m_water_space_launchers; 
    m_water_space_launchers = NULL; 

    delete m_water_next_too_land; 
    m_water_next_too_land = NULL; 
    delete m_land_next_too_water; 
    m_land_next_too_water = NULL; 

    delete[] m_mst; 
    m_mst = NULL; 
}


void Continents::Store(CivArchive &archive)
{
    archive << m_num_land_cont;
    archive << m_min_land_cont; 

    sint32 idx;
    for (idx = 0; idx< m_num_land_cont; idx++) { 
        archive << m_land_size[idx]; 
    }


    for (idx = 0; idx< m_num_land_cont; idx++) { 
        archive << m_land_cities[idx]; 
    }

    for (idx = 0; idx< m_num_land_cont; idx++) { 
        archive << m_land_num_next_to_edge_cities[idx];
    }

    for (idx = 0; idx< m_num_land_cont; idx++) { 
        archive << m_land_armies[idx];
    }

    for (idx = 0; idx< m_num_land_cont; idx++) { 
        archive << m_land_settlers[idx]; 
    }

    for (idx = 0; idx< m_num_land_cont; idx++) { 
        archive << m_land_space_launchers[idx];
    }
 
    archive << m_num_water_cont; 
    archive << m_min_water_cont;

    for (idx = 0; idx< m_num_water_cont; idx++) { 
        archive << m_water_size[idx];
    }

    for (idx = 0; idx < m_num_water_cont; idx++) { 
        archive << m_water_cities[idx];
    }
    
    for (idx = 0; idx < m_num_water_cont; idx++) { 
        archive << m_water_num_next_to_edge_cities[idx];
    }

    for (idx = 0; idx < m_num_water_cont; idx++) { 
        archive << m_water_armies[idx]; 
    }

    for (idx = 0; idx < m_num_water_cont; idx++) { 
        archive << m_water_space_launchers[idx]; 
    }

    sint32 num_water; 
    sint32 max_water;
    sint32 idx_water; 
    
    num_water = m_water_next_too_land->Num(); 
    max_water = m_water_next_too_land->ArraySize(); 

    archive << num_water; 
    archive << max_water; 
   
    sint32 num_land; 
    sint32 max_land, idx_land; 
    for (idx_water=0; idx_water<num_water; idx_water++) {        
        num_land = m_water_next_too_land->Get(idx_water).Num(); 
        max_land = m_water_next_too_land->Get(idx_water).ArraySize(); 

        archive << num_land; 
        archive << max_land; 

        for (idx_land=0; idx_land<num_land; idx_land++) { 
            archive <<  m_water_next_too_land->Get(idx_water).Get(idx_land); 
        }
    }


    num_land = m_land_next_too_water->Num(); 
    max_land = m_land_next_too_water->ArraySize(); 

    archive << num_land; 
    archive << max_land; 
   
    for (idx_land=0; idx_land<num_land; idx_land++) {        
        num_water = m_land_next_too_water->Get(idx_land).Num(); 
        max_water = m_land_next_too_water->Get(idx_land).ArraySize(); 

        archive << num_water; 
        archive << max_water; 

        for (idx_water=0; idx_water<num_water; idx_water++) { 
            archive <<  m_land_next_too_water->Get(idx_land).Get(idx_water); 
        }
    }
}

void Continents::Load(CivArchive &archive)
{

    archive >> m_num_land_cont;
    archive >> m_min_land_cont; 

    sint32 idx;
    m_land_size = new sint32[m_num_land_cont]; 
    for (idx = 0; idx< m_num_land_cont; idx++) { 
        archive >> m_land_size[idx]; 
    }

    m_land_cities = new sint32[m_num_land_cont]; 
    for (idx = 0; idx< m_num_land_cont; idx++) { 
        archive >> m_land_cities[idx]; 
    }

    m_land_num_next_to_edge_cities = new sint32[m_num_land_cont]; 
    for (idx = 0; idx< m_num_land_cont; idx++) { 
        archive >> m_land_num_next_to_edge_cities[idx];
    }

    m_land_armies = new sint32[m_num_land_cont]; 
    for (idx = 0; idx< m_num_land_cont; idx++) { 
        archive >> m_land_armies[idx];
    }

    m_land_settlers = new sint32[m_num_land_cont]; 
    for (idx = 0; idx< m_num_land_cont; idx++) { 
        archive >> m_land_settlers[idx]; 
    }

    m_land_space_launchers = new sint32[m_num_land_cont]; 
    for (idx = 0; idx< m_num_land_cont; idx++) { 
        archive >> m_land_space_launchers[idx];
    }
 
    archive >> m_num_water_cont; 
    archive >> m_min_water_cont;

    m_water_size = new sint32[m_num_water_cont]; 
    for (idx = 0; idx< m_num_water_cont; idx++) { 
        archive >> m_water_size[idx];
    }

    m_water_cities = new sint32[m_num_water_cont]; 
    for (idx = 0; idx < m_num_water_cont; idx++) { 
        archive >> m_water_cities[idx];
    }
    
    m_water_num_next_to_edge_cities = new sint32[m_num_water_cont]; 
    for (idx = 0; idx < m_num_water_cont; idx++) { 
        archive >> m_water_num_next_to_edge_cities[idx];
    }

    m_water_armies = new sint32[m_num_water_cont]; 
    for (idx = 0; idx < m_num_water_cont; idx++) { 
        archive >> m_water_armies[idx]; 
    }

    m_water_space_launchers = new sint32[m_num_water_cont]; 
    for (idx = 0; idx < m_num_water_cont; idx++) { 
        archive >> m_water_space_launchers[idx]; 
    }

    sint32 num_water; 
    sint32 max_water;
    sint32 idx_water; 

    archive >> num_water; 
    archive >> max_water; 
    m_water_next_too_land = new DynamicArray<DAsint32>(max_water); 
    for(idx_water=0; idx_water<m_num_water_cont; idx_water++) {
        m_water_next_too_land->ExtendByOne(); 
    }

    sint32 num_land; 
    sint32 max_land;
    sint32 idx_land; 
    for (idx_water=0; idx_water<num_water; idx_water++) {        
        archive >> num_land; 
        archive >> max_land; 

        m_water_next_too_land->Access(idx_water).ResizeFlat(max_land); 
        for (idx_land=0; idx_land<num_land; idx_land++) { 
            m_water_next_too_land->Access(idx_water).ExtendByOneFlat(); 
            archive >> m_water_next_too_land->Access(idx_water).Access(idx_land); 
        }
    }


    archive >> num_land; 
    archive >> max_land; 
    m_land_next_too_water = new DynamicArray<DAsint32>(max_land); 
    for(idx_land=0; idx_land<m_num_land_cont; idx_land++) {
        m_land_next_too_water->ExtendByOne(); 
    }
    
    for (idx_land=0; idx_land<num_land; idx_land++) {        

        archive >> num_water; 
        archive >> max_water; 

        m_land_next_too_water->Access(idx_land).ResizeFlat(max_water); 
        for (idx_water=0; idx_water<num_water; idx_water++) { 
            m_land_next_too_water->Access(idx_land).ExtendByOneFlat(); 
            archive >>  m_land_next_too_water->Access(idx_land).Access(idx_water); 
        }
    }
}

void Continents::Serialize(CivArchive &archive)
{
    CHECKSERIALIZE 

    if (archive.IsStoring()) { 
        Store(archive);
    } else {         
        Load(archive);
    } 
} 

void Continents::Clear()
{
	sint32 i; 

    for (i=0; i<m_num_land_cont; i++) { 
		m_land_size[i]=0;
        m_land_cities[i]=0; 
        m_land_num_next_to_edge_cities[i]=0; 
        m_land_armies[i]=0;
        m_land_settlers[i]=0;
        m_land_space_launchers[i]=0; 
    }

    sint32 n = m_land_next_too_water->Num(); 
    for (i=0; i<n; i++) { 
        m_land_next_too_water->Access(i).Clear();
    }
	
	for(i=0; i<m_num_water_cont; i++) { 
		m_water_size[i]=0;
        m_water_cities[i]=0; 
        m_water_num_next_to_edge_cities[i]=0; 
        m_water_armies[i]=0;
        m_water_space_launchers[i]=0; 
	}

    n = m_water_next_too_land->Num(); 
    for (i=0; i<n; i++) { 
        m_water_next_too_land->Access(i).Clear();
    }

}

void Continents::Update(AiMain *ai)
{
    Clear(); 

    MapPointData *size = ai->m_map->GetSize();
    MapPointData pos;
    BOOL is_land; 
    sint32 cont; 















    ArmyAgent *nth_army=NULL; 
    BSetID id; 
    for (nth_army = ai->m_army_set->First(id); ai->m_army_set->Last(); nth_army = ai->m_army_set->Next(id)) { 
        switch(nth_army->GetState()) { 
        case AGENT_STATE_UNBUILT:
        case AGENT_STATE_BUILDING: 
        case AGENT_STATE_MARKED_DEAD:
            continue;
        default:
            nth_army->GetPos(ai, pos); 

            if (SPACE_Z == pos.z) continue; 

            if (GetContinent(ai, pos, cont, is_land)) {
                if (is_land) { 
                    m_land_armies[cont]++; 

                    if (nth_army->GetArmyContainsSettler()) { 
                        m_land_settlers[cont]++; 
                    }
                } else { 
                    m_water_armies[cont]++; 
                    
                       
                    
                } 
            } else { 
                INSANE(CONT_COULD_NOT_FIND_ARMY); 
            }
        }
    } 

    CityAgent *nth_city=NULL;
    for (nth_city = ai->m_city_set->First(id); ai->m_city_set->Last(); nth_city  = ai->m_city_set->Next(id)) { 
        nth_city->GetPos( pos); 

        if (SPACE_Z == pos.z) continue; 

        if (GetContinent(ai, pos, cont, is_land)) {
            if (is_land) { 
                m_land_cities[cont]++; 
            } else { 
                m_water_cities[cont]++; 
            }
        } else { 
            INSANE(CONT_COULD_NOT_FIND_CITY); 
        } 
    }

    
}

BOOL Continents::GetContinent(AiMain *ai, MapPointData &pos, sint32 &cont, BOOL &is_land)
{
    sint32 foo;

    if (SPACE_Z == pos.z) { 
        return FALSE; 
    } else { 

        cont = ai->m_world->GetContinent(pos); 
#ifdef _DEBUG
        if (cont < 0) { 
            INSANE(CONTINENT_NUMBER_INVALID); 
            return FALSE; 
        } 

        if ((m_num_water_cont <= cont) && (cont < m_min_land_cont)) {             
            INSANE(CONTINENT_NUMBER_INVALID); 
            return FALSE; 
        } 

        if ((m_min_land_cont + m_num_land_cont) <= cont) {            
            INSANE(CONTINENT_NUMBER_INVALID); 
            cont = ai->m_world->GetContinent(pos); 
            return FALSE; 
        } 
#endif

        if (m_min_land_cont <= cont) { 
            cont -= m_min_land_cont;
            is_land = TRUE;

            Assert(0<=cont); 
            Assert(cont<ai->m_continents->GetNumLandCont()); 
if (!((0<=cont) ||
    (cont<ai->m_continents->GetNumLandCont()))) { 
   foo = 0; 
} 

        } else { 
            is_land = FALSE; 

            Assert(0<=cont); 
            Assert(cont<ai->m_continents->GetNumWaterCont()); 

if (!((0<=cont) ||
    (cont<ai->m_continents->GetNumWaterCont()))) { 
   foo = 0; 
} 

        } 
        return TRUE; 
    } 
}

void Continents::IncLandCities(const sint32 idx)
{
    m_land_cities[idx]++; 
}

sint32 Continents::GetLandCities(const sint32 idx) const
{
    return m_land_cities[idx];
} 

void Continents::IncLandNumNextToEdgeCities(const sint32 idx)
{
    m_land_num_next_to_edge_cities[idx]++; 
}

sint32 Continents::GetLandNumNextToEdgeCities(const sint32 idx) const
{
    return m_land_num_next_to_edge_cities[idx];
} 

void Continents::IncLandArmies(AiMain *ai, const sint32 idx, const uint32 a_id) 
{
    m_land_armies[idx]++;

    BSetID id(a_id); 
    ArmyAgent *aa = ai->m_army_set->Find(id);
	Assert(aa);

    if (aa->GetArmyContainsSettler()) { 
        m_land_settlers[idx]++; 
    }
}
sint32 Continents::GetLandArmies(const sint32 idx) const
{
    return m_land_armies[idx];
} 

sint32 Continents::GetLandSettlers(const sint32 idx) const 
{ 
    return m_land_settlers[idx];
} 

void Continents::IncWaterArmies(AiMain *ai, const sint32 idx, const uint32 a_id)
{ 
    m_water_armies[idx]++; 
} 

sint32 Continents::GetWaterArmies(const sint32 idx) const
{
   return m_water_armies[idx]; 
}

void Continents::IncWaterCities(const sint32 idx)
{
    m_water_cities[idx]++; 
}
sint32 Continents::GetWaterCities(const sint32 idx) const
{
    return m_water_cities[idx];
} 

void Continents::IncWaterNumNextToEdgeCities(const sint32 idx)
{
    m_water_num_next_to_edge_cities[idx]++; 
}
sint32 Continents::GetWaterNumNextToEdgeCities(const sint32 idx) const
{
    return m_water_num_next_to_edge_cities[idx];
} 


BOOL Continents::IsOcean(AiMain *ai, sint16 x, sint16 y, sint16 z)
{
    sint32 cont = 0;
    BOOL is_land = TRUE;
    MapPointData pos;

    if (z != 0)
        return FALSE; 

    pos.x = x;
    pos.y = y;
    pos.z = z;
    GetContinent(ai, pos, cont, is_land);
    if (is_land)
        return(FALSE);

    if (ai->m_world->GetWaterContinentSize(cont) < MINIMUM_OCEAN_SIZE)
        return(FALSE);

	return(TRUE);
}

void Continents::InsertWaterNextToLand(const sint32 waterc, const sint32 landc)
{
    sint32 idx_next_too, num_land;
    DynamicArray<sint32> *next=NULL;
   
    next = &(m_water_next_too_land->Access(waterc));
    num_land = next->Num(); 

    for (idx_next_too=0; idx_next_too<num_land; idx_next_too++) {
        if (next->Get(idx_next_too) == landc) { 
            return; 
        }
    }

   next->InsertFlat(landc);

}

void Continents::InsertLandNextToWater(const sint32 landc, const sint32 waterc)
{
    
    sint32 idx_next_too, num_water;
    DynamicArray<sint32> *next=NULL;
   
    next = &(m_land_next_too_water->Access(landc));
    num_water = next->Num(); 

    for (idx_next_too=0; idx_next_too<num_water; idx_next_too++) {
        if (next->Get(idx_next_too) == waterc) { 
            return; 
        }
    }

   next->InsertFlat(waterc);

}


BOOL Continents::LandShareWater(AiMain *ai, const sint32 land1, const sint32 land2) const
{
    sint32 water_idx; 
    for (water_idx=0; water_idx<m_num_water_cont; water_idx++) { 
        if (ai->m_world->IsLandNextTooWater(land1, water_idx) &&
            ai->m_world->IsLandNextTooWater(land2, water_idx)) { 
            return TRUE; 
        } 
    }

    return FALSE; 
}

CityTree * Continents::GetMst(const sint32 idx_cont)
{
    return m_mst[idx_cont]; 
}

void Continents::SetMst(const sint32 idx_cont, CityTree *mst)
{
    Assert(0 <= idx_cont); 
    Assert(idx_cont < m_num_land_cont); 

    m_mst[idx_cont] = mst; 
}


void Continents::RemoveCityReferance(CityAgent *remove_me)
{
    sint32 idx_cont; 
    for (idx_cont=0; idx_cont<m_num_land_cont; idx_cont++) {
        if (m_mst[idx_cont]) { 
            m_mst[idx_cont]->RemoveCityReferance(remove_me); 
        }
    }
}

void Continents::IncLandSpaceLaunchers(const sint32 idx_cont)
{
    Assert(0 <= idx_cont); 
    Assert(idx_cont < m_num_land_cont); 

    m_land_space_launchers[idx_cont]++; 
}

void Continents::IncWaterSpaceLaunchers(const sint32 idx_cont)
{
    Assert(0 <= idx_cont); 
    Assert(idx_cont < m_num_water_cont); 

    m_water_space_launchers[idx_cont]++; 
}

BOOL Continents::LandGotSpaceLauncher(const sint32 idx_cont)
{
    Assert(0 <= idx_cont); 
    Assert(idx_cont < m_num_land_cont); 

    return 0 < m_land_space_launchers[idx_cont]; 
}

BOOL Continents::WaterGotSpaceLauncher(const sint32 idx_cont)
{
    Assert(0 <= idx_cont); 
    Assert(idx_cont < m_num_land_cont); 

    return 0 < m_water_space_launchers[idx_cont]; 
}


void Continents::FindContinentNeighbors(AiMain *ai)
{
    sint32 v; 
    MapPointData pos; 
    MapPointData w; 
    BOOL is_water=FALSE, is_land=FALSE; 

    pos.z = GROUND_Z; 
    
    MapPointData *size = ai->m_map->GetSize(); 

    for(pos.x=0; pos.x< size->x; pos.x++) { 
        for(pos.y=0; pos.y< size->y; pos.y++) { 

            if (GetContinent(ai, pos, v, is_land)) {
                ai->m_map->Wrap(pos.x+1, pos.y-1, pos.z, w); 
                FindAContinentNeighbor(ai, pos, v, w, is_land, is_water); 

                ai->m_map->Wrap(pos.x+1, pos.y, pos.z, w); 
                FindAContinentNeighbor(ai, pos, v, w, is_land, is_water); 

                ai->m_map->Wrap(pos.x+1, pos.y+1, pos.z, w); 
                FindAContinentNeighbor(ai, pos, v, w, is_land, is_water); 

                ai->m_map->Wrap(pos.x, pos.y+1, pos.z, w); 
                FindAContinentNeighbor(ai, pos, v, w, is_land, is_water); 
            }
        }
    }
}

void Continents::FindAContinentNeighbor(AiMain *ai, MapPointData &pos, const sint32 v, 
      MapPointData &w, const BOOL is_land, const BOOL is_water)                                    
{
    BOOL is_neighbor_land = FALSE; 
    BOOL is_neighbor_water = FALSE; 
    sint32 v2;
    
    if (GetContinent(ai, w, v2, is_neighbor_land)) { 

        is_neighbor_water = !is_neighbor_land; 

        if (is_land && is_neighbor_water) { 
            InsertLandNextToWater(v, v2); 
            InsertWaterNextToLand(v2, v); 
        } else if (is_water && is_neighbor_land) { 
            InsertWaterNextToLand(v, v2); 
            InsertLandNextToWater(v2, v); 
        }
    }
}


void Continents::RegisterNewContinents(AiMain *ai)
{ 

    if ((m_min_land_cont != ai->m_world->GetMinLandContinent()) || 
        (m_num_land_cont != (ai->m_world->GetMaxLandContinent() - m_min_land_cont)) || 
        (m_min_water_cont != ai->m_world->GetMinWaterContinent()) ||  
        (m_num_water_cont != (ai->m_world->GetMaxWaterContinent() - m_min_water_cont))
        ) { 

        CleanupMemory(); 
        InitMemory(ai);
        Update(ai); 
    }
}
