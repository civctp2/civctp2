
#include "c3.h"
#include "globals.h"
#include "IC3World.h"

#include "Bset.h"
#include "AiMain.h"

#include "ArmyAgent.h"
#include "CityAgent.h"

#include "OA_Refuel.h"

#include "AiMap.h"







Opportunity_Action_Refuel::Opportunity_Action_Refuel ()
{
}







Opportunity_Action_Refuel::~Opportunity_Action_Refuel ()
{
}







void Opportunity_Action_Refuel::Serialize (CivArchive &archive)
{
    return; 
}









void Opportunity_Action_Refuel::Execute(AiMain *ai)
{ 
    
    BSetID id; 

    ArmyAgent *current_agent=NULL;
    CityAgent *tmp_city=NULL;

    MapPointData dest_pos; 

    BOOL searching; 

    sint32 army_num = ai->m_army_set->GetNum();
    sint32 army_idx=0; 

    if (army_num < 1) return; 

    
    
    ArmyAgent **plane_table; 
    plane_table = new ArmyAgent*[army_num]; 
    for (current_agent = ai->m_army_set->First(id); ai->m_army_set->Last(); current_agent = ai->m_army_set->Next(id)) {
        plane_table[army_idx++] = current_agent; 
    }

    
    for (army_idx=0; army_idx < army_num; army_idx++) { 
        current_agent = plane_table[army_idx]; 

        switch (current_agent->GetState()) { 
        case AGENT_STATE_MARKED_DEAD:
        case AGENT_STATE_UNBUILT:
        case AGENT_STATE_BUILDING: 
            continue; 
        default:
            if (current_agent->OverFuelSafety(ai)) { 
                
                if (current_agent->IsInCity(ai, tmp_city)) {
					
					continue;
                    
                }
                searching = TRUE; 

                
                FindAircraftCarriers(ai, current_agent, searching, dest_pos);
   
                
                FindCityAirports(ai, current_agent, searching, dest_pos);

                
                FindAirfields(ai, current_agent, searching, dest_pos);

                if (!searching) { 
                    SendPlaneToCity(ai, current_agent, dest_pos); 

                    if (ai->m_i_am_dead) { 
                        delete plane_table;
                        plane_table = NULL; 
                        return; 	
                    } 
                }
            }
        }
    }

    delete plane_table;
}

void Opportunity_Action_Refuel::FindAircraftCarriers(AiMain *ai, ArmyAgent *current_agent, BOOL &searching, MapPointData &dest_pos)
{

    ArmyAgent *transport_agent=NULL; 
    BSetID id; 
    MapPointData test_pos;

    for (transport_agent = ai->m_army_set->First(id); searching && ai->m_army_set->Last(); transport_agent = ai->m_army_set->Next(id)) {
        if (transport_agent != current_agent) { 
           if (transport_agent->CanTransport(ai, current_agent)) {
                switch (transport_agent->GetState()) { 
                case AGENT_STATE_MARKED_DEAD:
                case AGENT_STATE_UNBUILT:
                case AGENT_STATE_BUILDING: 
                    continue; 
                default:
                    transport_agent->GetPos(ai, test_pos); 
                    if (current_agent->IsInFuelRange(ai, test_pos)) {
                        dest_pos = test_pos; 
                        searching = FALSE; 
                    }
                }
           } 
        }
    }
}

void Opportunity_Action_Refuel::FindCityAirports(AiMain *ai, ArmyAgent *current_agent, BOOL &searching, MapPointData &dest_pos)
{
    CityAgent *current_city=NULL; 
    BSetID id; 
    sint32 capacity;
    sint32 max_capacity = 1; 
    MapPointData test_pos; 
    BOOL local_searching= TRUE; 

    for (current_city = ai->m_city_set->First(id); searching && ai->m_city_set->Last(); current_city = ai->m_city_set->Next(id)) {

		
        

        capacity = k_MAX_ARMY_SIZE - current_city->GetNumUnitsInCity(ai); 

        if (max_capacity <= capacity) { 

            current_city->GetPos(test_pos);  
        
            if (current_agent->IsInFuelRange(ai, test_pos)) { 
                dest_pos = test_pos;
                max_capacity = capacity; 
                local_searching = FALSE; 
            } 
        }
    }

    if (!local_searching) { 
        searching = FALSE; 
    }
}

void Opportunity_Action_Refuel::FindAirfields(AiMain *ai, ArmyAgent *current_agent, BOOL &searching, MapPointData &dest_pos)
{
    sint32 player_id;
    uint32 a_id; 
    sint32 top_unit_type;
    sint32 unit_num; 

    sint32 air_num; 
    sint32 air_idx; 
    MapPointData test_pos; 
    sint32 capacity; 
    sint32 max_capacity = 1; 

    BOOL local_searching = TRUE; 
    air_num = ai->m_airfield_list->Num(); 
    for (air_idx=0; searching && air_idx<air_num; air_idx++) { 

        test_pos = ai->m_airfield_list->Access(air_idx); 
          if (ai->m_world->GetArmyId (&test_pos, 
              &player_id, &a_id, &top_unit_type, &unit_num)) { 

              if (player_id != ai->m_my_player_id) continue; 

              capacity = k_MAX_ARMY_SIZE - unit_num; 
          } else { 
                capacity = k_MAX_ARMY_SIZE; 
          }

        if (max_capacity <= capacity) { 
        
            if (current_agent->IsInFuelRange(ai, test_pos)) { 
                dest_pos = test_pos;
                max_capacity = capacity; 
                local_searching = FALSE; 
            } 
        }
    }

    if (!local_searching) { 
        searching = FALSE; 
    }
}








void Opportunity_Action_Refuel::SendPlaneToCity(AiMain *ai,
    ArmyAgent *the_army, MapPointData &dest_pos)
{

    sint32 pathtime; 
    BOOL did_move; 
    sint32 cutoff = 200000; 
    sint32 nodes_opened; 

    if (the_army->FindPath(ai, dest_pos, pathtime, FALSE, cutoff, 
        nodes_opened, TRUE, GOAL_TYPE_NULL)) { 

        while (1) { 
            if (the_army->AtEndOfPath(ai))  { 
                return; 
            }
    
            if (the_army->CanEnterNextPathPoint(ai)) { 
      
                the_army->GotoNextPathPoint(ai, did_move); 
                   	
                if (ai->m_i_am_dead) { 
                    return; 	
                } 
            
        
                if (AGENT_STATE_MARKED_DEAD == the_army->GetState()){
                    return; 
                }

                if (!did_move) { 
                    return;
                } 
        
            } else { 
                
                return;
            }
        }
    }
}



