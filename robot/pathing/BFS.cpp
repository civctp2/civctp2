
#include "C3.h"

#include "BFS.h"

#include "AstarPnt.h"
#include "AVLHeap.h"
extern AVLHeap g_astar_mem; 

#include "MapPoint.h"

#include "Player.h"
extern Player **g_player;

#include "Cell.h"
#include "XY_Coordinates.h"
#include "World.h"
extern World *g_theWorld; 

#include "Unit.h"
#include "UnitData.h"
#include "CityData.h"
#include "Happy.h"
#include "ArmyData.h"
#include "GovernmentRecord.h"


extern uint32 g_search_count; 

BestFirstSearch::BestFirstSearch()
{
}

BestFirstSearch::~BestFirstSearch()
{
}


sint32  BestFirstSearch::FindNumCitiesAtHeight(const sint32 player_idx, const sint32 z_height)
{
    sint32 count=0; 
    MapPoint city_pos; 
    MapPoint capitol_pos; 
    g_player[player_idx]->GetCapitolPos(capitol_pos);
    
    sint32 city_idx; 
    sint32 city_num=g_player[player_idx]->m_all_cities->Num(); 
    for (city_idx=0; city_idx<city_num; city_idx++) {
        g_player[player_idx]->m_all_cities->Access(city_idx).GetPos(city_pos); 
        if (z_height != city_pos.z) continue;  

        if ((capitol_pos.x == city_pos.x) &&
            (capitol_pos.y == city_pos.y) &&
            (capitol_pos.z == city_pos.z)) continue; 

            count++; 
       
    }

    return count; 
}

BOOL BestFirstSearch::InitPoint(const sint32 player_idx, AstarPoint *parent, AstarPoint *&node, 
    MapPoint &pos, const double past_cost, const double max_cost)
{   

    Cell *the_cell = g_theWorld->GetCell(pos);
    double entry_cost = the_cell->GetMoveCost();

  


























    

    node = g_astar_mem.GetNew();            

    node->m_is_zoc = FALSE; 
    node->m_parent = parent; 
    node->m_past_cost = past_cost; 
    node->m_entry_cost = entry_cost;
    node->m_future_cost =0.0; 

    node->m_total_cost = node->m_past_cost + node->m_entry_cost + node->m_future_cost; 

    node->m_pos = pos; 
    node->m_is_expanded = FALSE; 
    node->m_queue_idx = -1; 

    return TRUE; 
}


void BestFirstSearch::FindMoveCostToCitiesZ(const sint32 player_idx, const sint32 z_height, 
    const double max_cost, const double min_cost)
{	
    g_search_count++; 

    
    BOOL searching = TRUE; 
    sint32 num_cities_found = 0; 
    sint32 num_cities_out_there = FindNumCitiesAtHeight(player_idx, z_height); 
    if (num_cities_out_there < 1) return; 

    
    m_priority_queue.Clear(); 

    
    MapPoint start_pos;
    MapPoint neighbor_pos; 
    double start_cost;
	g_player[player_idx]->GetCapitolPos(start_pos); 
    start_cost = -g_theWorld->GetCell(start_pos)->GetMoveCost() + ((start_pos.z != z_height) ? 1000.0 : 0) ;
    start_pos.z = z_height; 	
    Cell *neighbor_cell = g_theWorld->GetCell(start_pos);              
    neighbor_cell->m_search_count = g_search_count; 
    InitPoint(player_idx, NULL, neighbor_cell->m_point, start_pos, start_cost, max_cost);
    sint32 nodes_opened = 1;
    AstarPoint* best = neighbor_cell->m_point; 
    double past_cost;

    sint32 i; 
    sint32 bfs_loop_count =0; 
    Unit a_city;
    double dist_cost; 
    do {  
        Assert(bfs_loop_count++ < 140000); 
                
        best->m_is_expanded = TRUE;  

        past_cost = best->m_total_cost; 
        
        
        for (i=0; i <= 7; i++) { 
            
            
            if (!best->m_pos.GetNeighborPosition(WORLD_DIRECTION(i), neighbor_pos)) continue;	            
            neighbor_cell = g_theWorld->GetCell(neighbor_pos);     

            if (neighbor_cell->m_search_count == g_search_count)  
                continue; 

            
            if (InitPoint(player_idx, best, neighbor_cell->m_point, neighbor_pos, past_cost, max_cost)) {  

g_theWorld->SetColor(neighbor_pos, int(past_cost)); 
                nodes_opened++;
                neighbor_cell->m_search_count = g_search_count;    
                a_city = neighbor_cell->GetCity(); 
                if (a_city.m_id != 0) { 
                    if (a_city.GetOwner() == player_idx) { 

                        dist_cost = max(0.0, neighbor_cell->m_point->m_total_cost - min_cost); 
                        a_city.AccessData()->GetCityData()->GetHappy()->SetDistToCapitol(dist_cost); 
                        num_cities_found++;
                        if (num_cities_found == num_cities_out_there) { 
                            searching = FALSE; 
                            break; 
                        } 
                    } 
                } 
                m_priority_queue.Insert(neighbor_cell->m_point); 
            } 
        } 

        if (!searching || (m_priority_queue.Len() < 1)) { 
            break; 
        } else {                       
            best = m_priority_queue.Remove(1);                
        }        

    } while (searching);      

    
    
    g_astar_mem.MassDelete(FALSE);   
}

void BestFirstSearch::CalcDistToCapitol(const sint32 player_idx)
{ 

MapPoint pos; 
MapPoint *size; 

pos.z=0; 
size = g_theWorld->GetSize(); 
for (pos.x=0; pos.x<size->x; pos.x++) { 
    for (pos.y=0; pos.y<size->y; pos.y++) { 
        g_theWorld->SetColor(pos, 0); 
    }
}

    Assert(0 <= player_idx); 
    Assert(player_idx < k_MAX_PLAYERS); 
    Assert(g_player[player_idx]);
    if (!g_player[player_idx]) return;
   
    
    
    double raw_max_cost = g_player[player_idx]->GetMaxEmpireDistance();
    double raw_min_cost = g_theGovernmentDB->Get(g_player[player_idx]->GetGovernmentType())->GetMinEmpireDistance();
    double dist_cost = max(0.0, raw_max_cost - raw_min_cost); 

    sint32 city_idx; 
    sint32 city_num;
    city_num = g_player[player_idx]->m_all_cities->Num(); 

    BOOL searching; 
    MapPoint start; 
    if (g_player[player_idx]->GetCapitolPos(start)) {  
        searching = TRUE; 
    } else { 
        searching = FALSE; 
        start.x=-1;
        start.y=-1; 
        start.z=-1;       
    }

    MapPoint city_pos; 
    for (city_idx=0; city_idx<city_num; city_idx++) {

        g_player[player_idx]->m_all_cities->Access(city_idx).GetPos(city_pos); 
        if ((city_pos.x == start.x) && (city_pos.y == start.y) && (city_pos.z == start.z)) { 
            g_player[player_idx]->m_all_cities->Access(city_idx).AccessData()->GetCityData()->GetHappy()->SetDistToCapitol(0); 
        } else { 
            g_player[player_idx]->m_all_cities->Access(city_idx).AccessData()->GetCityData()->GetHappy()->SetDistToCapitol(dist_cost); 
        }
    }    

 
    if (searching) { 
        FindMoveCostToCitiesZ(player_idx, GROUND_Z, raw_max_cost, raw_min_cost);    
        FindMoveCostToCitiesZ(player_idx, SPACE_Z, raw_max_cost, raw_min_cost);
    }    
}

