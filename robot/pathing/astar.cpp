

























#include "c3.h"
#include "c3errors.h"
#include "Globals.h"


#include "AstarPnt.h"
#include "Astar.h"
#include "AVLHeap.h"

#include "DynArr.h"
#include "Path.h"

#include "XY_Coordinates.h"
#include "World.h"
#include "RandGen.h"
#include "Cell.h"
#include "ConstDB.h"
#include "PriorityQueue.h"
#include "A_Star_Heuristic_Cost.h"


extern World *g_theWorld; 
extern RandomGenerator *g_rand; 
extern ConstDB *g_theConstDB; 

sint32 g_search_count; 

AVLHeap g_astar_mem; 

void Astar_Init()

{ 
    g_astar_mem.InitHeap(); 
    g_search_count = 1; 
} 

void Astar_Cleanup()

{ 
    g_astar_mem.CleanUp(); 
}
















#define k_MIN_MOVE_COST 10.0 

float g_cost_factor = k_MIN_MOVE_COST;

#ifdef _DEBUG



#endif 

#ifdef TRACK_ASTAR_NODES

float g_nodes_opened = 0;
float g_nodes_inserted = 0;
int g_paths_found = 0;
long int g_paths_lengths = 0;
long int g_closed_nodes = 0;
#endif 

extern void WhackScreen(); 
#ifdef _DEBUG
extern bool g_old_heuristic;
#else
bool g_old_heuristic = false;
#endif

float Astar::EstimateFutureCost(const MapPoint &pos, const MapPoint &dest)
{

	float best_heuristic;













    sint32 dist = pos.NormalizedDistance(dest); 

	{ 
		
		best_heuristic = (float)g_theWorld->A_star_heuristic->Get_Minimum_Nearby_Movement_Cost(
				MapPointData(pos));

		Assert(best_heuristic > 0);
		Assert(best_heuristic < 500);

#ifdef TEST_DISTANCE
		
		sint32 idist;

		
		idist = g_theWorld->XY_Coords.Get_RC_Travel_Distance(pos, dest);


#ifdef SUPER_DEBUG_HEURISTIC
		if (dist == idist)
			g_theWorld->SetColor(pos,  dist); 
		else
			g_theWorld->SetColor(pos,  dist*100 + idist); 
#endif
#endif 

        

		
		
		
		
		


#ifdef _PLAYTEST
		if (m_pretty_path && g_old_heuristic) { 
			 float ground_dist =  0.7f * best_heuristic * dist;


#ifdef SUPER_DEBUG_HEURISTIC
			g_theWorld->SetColor(pos, sint32(ground_dist)); 
#endif
			return ground_dist; 
		} else
#endif
			return best_heuristic * dist;
    }
}
  
void Astar::DecayOrtho(AstarPoint *parent, AstarPoint *point, 
      float &new_entry_cost)
{  
    BOOL is_ortho = FALSE; 

    
    if (parent->m_pos.x == point->m_pos.x) { 
      is_ortho = TRUE; 
    } else if (parent->m_pos.x < point->m_pos.x) {
       is_ortho = ((parent->m_pos.y-1) == point->m_pos.y);
    } else { 
      is_ortho = ((parent->m_pos.y+1) == point->m_pos.y);
    }

    if (is_ortho) { 
       new_entry_cost = point->m_entry_cost * 0.95f; 
    } 
}













sint32 Astar::InitPoint(AstarPoint *parent, AstarPoint *point, 
    const MapPoint &pos, const float pc, const MapPoint &dest) 

{ 
    BOOL is_ortho = FALSE; 
    AstarPoint *d = point;
    ASTAR_ENTRY_TYPE entry=ASTAR_CAN_ENTER; 
    BOOL is_zoc=FALSE;

    d->m_flags = 0; 
	d->SetEntry(ASTAR_CAN_ENTER); 
    d->SetZoc(FALSE); 
    d->SetExpanded(FALSE); 
    d->m_pos = pos; 
    d->m_parent = parent; 
#if _DEBUG
    d->m_queue_idx = -1; 
#endif
    
	d->m_past_cost = pc;
    if (parent == NULL) { 
       d->m_entry_cost = 0.0;
       d->m_future_cost = EstimateFutureCost(d->m_pos, dest);
       d->m_total_cost = d->m_past_cost + d->m_entry_cost 
            + d->m_future_cost;

#ifdef PRINT_COSTS
			g_theWorld->SetColor(pos,  d->m_total_cost); 
#endif 

       return TRUE; 
    } else if (EntryCost(parent->m_pos, d->m_pos, d->m_entry_cost, is_zoc, entry)){         

        d->SetEntry(entry); 
        d->SetZoc(is_zoc); 

        DecayOrtho(parent, point, d->m_entry_cost);

        d->m_future_cost = EstimateFutureCost(d->m_pos, dest);
        d->m_total_cost = d->m_past_cost + d->m_entry_cost 
            + d->m_future_cost;

#ifdef PRINT_COSTS
			g_theWorld->SetColor(pos,  d->m_total_cost); 
#endif 

        return TRUE; 
    } else {  
            
        d->SetExpanded(TRUE); 
        d->SetEntry(entry); 
        d->SetZoc(is_zoc); 

	
        if (entry == ASTAR_RETRY_DIRECTION) { 
            d->SetEntry(ASTAR_RETRY_DIRECTION);
        } else { 
    		d->SetEntry(ASTAR_BLOCKED);
        }

        d->m_future_cost = (d->m_entry_cost + k_ASTAR_BIG); 
        d->m_total_cost = d->m_past_cost + d->m_entry_cost 
            + d->m_future_cost;
 #ifdef PRINT_COSTS
			g_theWorld->SetColor(pos,  d->m_total_cost); 
#endif 
       return FALSE;
    }
}

void Astar::RecalcEntryCost(AstarPoint *parent, AstarPoint *node, float &new_entry_cost, 
    BOOL &new_is_zoc, ASTAR_ENTRY_TYPE &new_entry)
{

    Assert(parent); 
    Assert(node); 

    EntryCost(parent->m_pos, node->m_pos, 
        new_entry_cost, new_is_zoc, new_entry);

	Assert(new_entry_cost < 10000.0); 

    DecayOrtho(parent, node, new_entry_cost);
}
























#ifdef _DEBUG
sint32 finite_stack; 
#endif _DEUBG




#if 0
sint32 g_propagate_depth;  
sint32 g_propagate_been_seen;

void Astar::PropagatePathCost(AstarPoint *node, AstarPoint *parent, 
   const MapPoint &start, const MapPoint &dest, AstarPoint **cost_tree)
                              
{  
#ifdef _DEBUG
    if ((100 < g_propagate_depth) && (!g_propagate_been_seen)){ 
        g_propagate_been_seen=1;
        sint32 DONT_hit_I___call_KARL_at_x4646=0;
        Assert(DONT_hit_I___call_KARL_at_x4646);
    }
#endif
     

    AstarPoint *dead = NULL;
    float new_past_cost, new_entry_cost, new_total_cost;
    sint32 reset = FALSE;
    sint32 d=0; 
    
    if (node->m_parent == NULL) { 
        return;  
    }

    if (node->GetEntry() != ASTAR_CAN_ENTER) { 
		return; 
	} 
    
    
    
    
    BOOL new_is_zoc; 
	ASTAR_ENTRY_TYPE new_entry; 
    if (node->m_parent != parent) { 
        
        new_past_cost = parent->m_past_cost + parent->m_entry_cost;
        new_is_zoc = FALSE;
        new_entry = ASTAR_CAN_ENTER; 

        RecalcEntryCost(parent, node, new_entry_cost, new_is_zoc, new_entry); 
        new_total_cost = new_past_cost + new_entry_cost + node->m_future_cost;
        
        
		
		
        if ((new_entry == ASTAR_CAN_ENTER) && (new_total_cost < k_ASTAR_BIG) && ((new_total_cost+0.8) < node->m_total_cost) && (new_is_zoc == FALSE))
        { 
            reset = TRUE; 
            node->m_parent = parent;
        }      
    }
    
    static MapPoint next_pos; 
    Cell *c; 
    
    if (node->m_parent == parent) {
       
        if (!(node->GetExpanded())) { 
			sint32 del_idx = node->m_queue_idx; 
            AstarPoint *node_actually_removed = m_priority_queue.Remove(del_idx); 
			Assert(node_actually_removed == node); 
        }
        
        node->m_past_cost = parent->m_past_cost + 
            parent->m_entry_cost;
        
        if (reset) {
            node->m_entry_cost = new_entry_cost; 
            node->SetZoc(new_is_zoc); 
			node->SetEntry(new_entry); 
        } 
        
        node->m_total_cost = node->m_past_cost + 
            node->m_entry_cost + node->m_future_cost; 
        
        if (!(node->GetExpanded())) {                 
            
			if (node->GetEntry() == ASTAR_CAN_ENTER)  { 
				m_priority_queue.Insert(node); 
			}

        }
        
		if ((node->GetEntry() == ASTAR_CAN_ENTER) && (node->m_total_cost  < k_ASTAR_BIG)) { 
			for (d=0; d<GetMaxDir(node->m_pos); d++) {
				if (!node->m_pos.GetNeighborPosition(WORLD_DIRECTION(d), next_pos))  continue;
            
				c = g_theWorld->GetCell(next_pos); 
            
				if (c->m_point != NULL)  { 
					if ((c->m_search_count == g_search_count) && 
						(c->m_point->GetZoc() == FALSE)) {

						g_propagate_depth++;

						
						
						if (g_propagate_depth > 100)
							return;

						PropagatePathCost(c->m_point, node, start, dest, cost_tree);

						g_propagate_depth--;
					}
				}
			} 
		}
    } 
    
}
#endif	





















#ifdef A_STAR_TRACK_CALLSTACK
#include "debugcallstack.h"

cDebugCallStackSet g_astarCallStackSet(5);
#endif


sint32 Astar::FindPath(const MapPoint &start, const MapPoint &dest, 
                       Path &a_path, float &total_cost, const sint32 isunit, 
                       const sint32 cutoff, sint32 &nodes_opened)
                       
{   
#ifdef A_STAR_TRACK_CALLSTACK
	g_astarCallStackSet.Add();
#endif

    sint32 i;
    AstarPoint *best = NULL; 
    AstarPoint *cost_tree = NULL;

    m_priority_queue.Clear(); 

#ifdef SUPER_DEBUG_HEURISTIC
MapPoint tmp; 
MapPoint *size; 

size = g_theWorld->GetSize(); 
for  (tmp.x=0; tmp.x<size->x; tmp.x++) { 
    for (tmp.y=0; tmp.y<size->y; tmp.y++) { 
        g_theWorld->SetColor(tmp, 0); 
    } 
} 
#endif

    static MapPoint next_pos;
    float past_cost; 
    sint32 count;
    sint32 r;
    BOOL searching;

    g_search_count++; 
    
    if (start == dest) {
        return Cleanup (dest, a_path, total_cost,  isunit, best, cost_tree);
    } 
    
    
    
    Cell *c = g_theWorld->GetCell(start); 
    
    Assert(c);
    if (!c) 
        return FALSE; 
    
    nodes_opened++;

#ifdef TRACK_ASTAR_NODES
	g_nodes_inserted++;
	g_nodes_opened++;
#endif 

    c->m_point = g_astar_mem.GetNew();
    c->m_search_count = g_search_count; 
    
    if (InitPoint(NULL, c->m_point, start, 0.0, dest) == FALSE) { 
        r =  Cleanup (dest, a_path, total_cost,  isunit, best, cost_tree);
        return r; 
    }
    
    
    best = c->m_point; 
    
    
    
    
    count = 0;
    
    sint32 loop_count =0; 
    do {  
        
        loop_count++; 
        
        Assert(loop_count < 140000); 
        
        past_cost = best->m_past_cost + best->m_entry_cost; 
        best->SetExpanded(TRUE); 
        
        sint32 max_dir = GetMaxDir(best->m_pos);

        for (i=0; i <= max_dir; i++) { 
            
            if (!best->m_pos.GetNeighborPosition(WORLD_DIRECTION(i), next_pos)) continue;	
            

            c = g_theWorld->GetCell(next_pos);  
            
            if (c->m_search_count == g_search_count) {  
                
                
#if 0               
				if (c->m_point->GetEntry() == ASTAR_CAN_ENTER) {
				   if (past_cost < c->m_point->m_past_cost) 
					{ 

						g_propagate_depth=0;

						

						
						if (g_full_propagate_path)
						{
	                        PropagatePathCost(c->m_point, best, start, dest, &cost_tree);

						} 
                   }                
			   
			   } else 
#endif				   
				if (c->m_point->GetEntry() == ASTAR_RETRY_DIRECTION) { 
                        
				   if (InitPoint(best, c->m_point, next_pos, past_cost, dest)) { 
                       Assert(c->m_point->m_queue_idx < 0); 

					   if (c->m_point->GetEntry() == ASTAR_CAN_ENTER)  { 
	                       m_priority_queue.Insert(c->m_point); 
					   }

                   }
               }   
           } else { 
                
                nodes_opened++;

#ifdef TRACK_ASTAR_NODES
				g_nodes_opened++;

#endif 
                c->m_point = g_astar_mem.GetNew(); 
                c->m_search_count = g_search_count;               

                if (InitPoint(best, c->m_point, next_pos, past_cost, dest)) {  


#ifdef TRACK_ASTAR_NODES
					g_nodes_inserted++;
#endif 

					if (c->m_point->GetEntry() == ASTAR_CAN_ENTER) { 
	                    m_priority_queue.Insert(c->m_point); 
					}

				} 
            } 
        } 
        count++;
        
        if (m_priority_queue.Len() < 1) { 
            break; 
        } else { 
            
            if (0 < m_priority_queue.Len()) { 


				
				
				
				
				
				
				
				
				
				
				
				
				
				
                best = m_priority_queue.Remove(1); 


#ifdef TRACK_ASTAR_NODES
				g_closed_nodes++;
#endif 


                Assert(best->m_queue_idx < 0); 
            } else { 
                best = NULL; 
            }
        }
        
        
        if (best) { 
            searching = TRUE; 
            float cost; 
            BOOL is_zoc = FALSE; 
            ASTAR_ENTRY_TYPE entry = ASTAR_CAN_ENTER;
            if (best->m_pos ==  dest) { 
                if (EntryCost(best->m_parent->m_pos, best->m_pos,                           
                    cost, is_zoc, entry)) { 
                    if (entry == ASTAR_CAN_ENTER) { 
                       searching = FALSE; 
                    }
                } 
            } 
        } else { 
            searching = FALSE; 
        }
        
    } while  (searching || (best && (k_ASTAR_BIG <= best->m_entry_cost )) && (nodes_opened < cutoff));   

#ifdef SUPER_DEBUG_HEURISTIC

   WhackScreen();  
#endif

    r =  Cleanup (dest, a_path, total_cost, isunit, best, cost_tree);


#ifdef TRACK_ASTAR_NODES
	
	g_paths_found++;
	g_paths_lengths += a_path.Num();
#endif 


    
    return r; 
    
}

sint32 Astar::Cleanup (const MapPoint &dest, Path &a_path, 
                       float &total_cost, const sint32 isunit, 
                       AstarPoint *best, AstarPoint *cost_tree)
{
    if ((best == NULL) ||
        (best->m_pos != dest)) {
       total_cost = 0.0;
       
       a_path.Clear(); 
       g_astar_mem.MassDelete(isunit);
       return FALSE; 
    }  else { 
        
       total_cost = best->m_past_cost + best->m_entry_cost; 
       a_path.FlattenAstarList(best); 
       g_astar_mem.MassDelete(isunit);   

       return TRUE;
    }
}


