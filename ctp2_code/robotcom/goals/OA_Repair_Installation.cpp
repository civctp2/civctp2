


#include "c3.h"
#include "globals.h"
#include "IC3World.h"
#include "IC3Player.h"
#include "IC3Science.h"
#include "IC3InstDB.h"
#include "IC3Rand.h"

#include "OA_Repair_Installation.h"
#include "CivArchive.h"
#include "AiMain.h"
#include "AiRndCnt.h"
#include "TerrImproveData.h"
#include "ScienceAgent.h"
#include "AiCause.h"

extern double fz_min_num_dirty_tiles;

Pillaged_Node::Pillaged_Node(MapPointData &pos, sint32 t, sint32 i)
{
    m_pos = pos; 
    m_fix_me_by = t; 
    m_inst = i; 
    m_attempt_count = 0; 
    m_next = NULL; 
} 

Pillaged_Node::~Pillaged_Node()
{
}

Pillaged_Node::Pillaged_Node(CivArchive &archive)
{
    m_next = NULL; 
    Serialize(archive); 
}

void Pillaged_Node::Serialize(CivArchive &archive)
{
    if (archive.IsStoring()) { 
        archive << m_pos.x; 
        archive << m_pos.y; 
        archive << m_pos.z; 
        archive << m_fix_me_by ; 
        archive << m_inst; 
        archive << m_attempt_count;
    } else { 
        archive >> m_pos.x; 
        archive >> m_pos.y; 
        archive >> m_pos.z; 
        archive >> m_fix_me_by ; 
        archive >> m_inst; 
        archive >> m_attempt_count;
    } 
} 

Opportunity_Action_Repair_Installation::Opportunity_Action_Repair_Installation()
{
   m_dirty_tile_count = 0; 
   m_queue_len=0;

   m_head = NULL; 
   m_tail = NULL; 
}


Opportunity_Action_Repair_Installation::~Opportunity_Action_Repair_Installation()
{
    Pillaged_Node *nth_node=NULL; 
    Pillaged_Node *del_me; 

#ifdef _DEBUG
sint32 finite_count_del_oa_repair=0;
#endif

    nth_node = m_head; 
    while (nth_node) { 
Assert(finite_count_del_oa_repair++ < 1000); 
        del_me = nth_node; 
        nth_node = nth_node->m_next; 
        delete del_me; 
        del_me = NULL; 
    } 
}

void Opportunity_Action_Repair_Installation::Serialize (CivArchive &archive)
{
    Pillaged_Node *nth_node=NULL; 

#ifdef _DEBUG
    sint32 finite_count_oa_repair_serialize=0; 
#endif

    sint32 node_idx, num_stored_nodes; 
    if (archive.IsStoring()) { 

         archive << m_queue_len;
         for (nth_node = m_head; nth_node; nth_node = nth_node->m_next) { 
            Assert(finite_count_oa_repair_serialize++ < m_queue_len); 
            nth_node->Serialize(archive); 
         }
    }  else { 

        archive >> num_stored_nodes; 
        if (num_stored_nodes < 1) { 
            m_queue_len = 0; 
            m_head = NULL; 
            m_tail = NULL; 
        } else { 
            for  (node_idx=0; node_idx<num_stored_nodes; node_idx++) {
                nth_node = new Pillaged_Node(archive); 
                Enqueue(nth_node); 
            } 



m_dirty_tile_count = 0;
for  (node_idx=0, nth_node = m_head; node_idx<num_stored_nodes; node_idx++, nth_node = nth_node->m_next) {
	
	if (0  == nth_node->m_inst) {
      m_dirty_tile_count++; 
  }
} 


            Assert(num_stored_nodes == m_queue_len); 
        }
    } 
}

void Opportunity_Action_Repair_Installation::Execute(AiMain *ai)
{

    sint32 node_idx, node_num; 
    
    node_num = m_queue_len; 
    
    Pillaged_Node *p=NULL;
    BOOL best_road_ok; 
    sint32 road_type;
    BOOL build_success; 
    sint32 stored = ai->m_player->GetMaterialsStored(); 
    sint32 cost;
    ERR_BUILD_INST err;
    sint32 o; 
    BOOL make_grass; 
    sint32 extra_data; 

    for (node_idx=0; node_idx<node_num; node_idx++) { 
        p = Dequeue(); 
        Assert(p); 
        build_success = FALSE; 

        if (p->m_fix_me_by <= ai->m_round_count->GetRound()) { 

			
            if (0  == p->m_inst) {
                
                if (fz_min_num_dirty_tiles < m_dirty_tile_count) { 
                    if (ai->m_world->CanBeIrrigated(&(p->m_pos))) { 
                        make_grass = ai->m_rand->Next(100) < 70; 
                    } else { 
                        make_grass = ai->m_rand->Next(100) < 30; 
                    } 

                    if (make_grass) { 
                        extra_data = TERRAIN_GRASSLAND; 
                    } else { 
                        extra_data = TERRAIN_HILL; 
                    } 

                     cost = ai->m_installationDB->GetCost(0 , &(p->m_pos), extra_data); 
                     o = ai->m_world->GetArmyOwner(&(p->m_pos));
                     if (((cost + 2000) <= stored) && ((o == ai->m_my_player_id) || (o == -1))) { 
                         ai->m_installationDB->CanBuildHere(0 , extra_data, &(p->m_pos), &err); 
                         if ((ERR_BUILD_INST_OK == err) || (ERR_BUILD_INST_UPGRADE == err)) {                
                             build_success = ai->m_installationDB->ConstructInstallation(0 , extra_data,
                                 &(p->m_pos)); 
                             if (build_success) { 
                                 m_dirty_tile_count--; 
                                 stored -= cost; 
                             }
                         }
                     }
                }

            } else { 
				
                if (0  == p->m_inst) { 
                    road_type = 0 ;
                } else { 
                    best_road_ok = ai->m_science_agent->GetBestRoadType(road_type);
                    Assert(best_road_ok); 
                }

                 cost = ai->m_installationDB->GetCost(road_type, &(p->m_pos), 0); 
                 if (cost <= stored) { 

                     o = ai->m_world->GetArmyOwner(&(p->m_pos));
                     if ((o == ai->m_my_player_id) || (o == -1)) { 
                         ai->m_installationDB->CanBuildHere(road_type, 0, &(p->m_pos), &err); 
                         if ((ERR_BUILD_INST_OK == err) || (ERR_BUILD_INST_UPGRADE == err)) {                
                             build_success = ai->m_installationDB->ConstructInstallation(road_type, 0,
                                 &(p->m_pos)); 
                             if (build_success) { 
                                 stored -= cost; 
                             }
                         }
                     } 
             
                     if (!build_success) { 
                        p->m_fix_me_by += 5;
                        p->m_attempt_count++;
                     }
                 } 
            }

            if (!build_success && (p->m_attempt_count<4)) { 
                Enqueue(p); 
            } else { 
                delete p; 
                p=NULL;
            } 
        }
    } 
}

Pillaged_Node* Opportunity_Action_Repair_Installation::Dequeue()
{ 
    Pillaged_Node *ret; 

    Assert(m_head); 
    Assert(m_tail); 
    Assert(0 < m_queue_len); 

    ret = m_head; 
    m_queue_len--; 
    m_head = m_head->m_next; 

    if (m_head == NULL) { 
        m_tail = NULL; 
    } 

    ret->m_next = NULL; 
    return ret; 
} 

void Opportunity_Action_Repair_Installation::Enqueue(Pillaged_Node *add_me)
{
    if (NULL == m_head) {
        Assert(NULL == m_tail); 
        Assert(0 == m_queue_len); 

        m_head = add_me; 
        m_tail = add_me; 
    } else { 
        m_tail->m_next = add_me; 
        m_tail = add_me; 
    } 

    m_queue_len++; 
} 

void Opportunity_Action_Repair_Installation::RegisterHostileAction(AiMain *ai, MapPointData &pos)
{

    sint32 old_road_type; 
    sint32 t;
    Pillaged_Node *p=NULL;

    if (ai->m_world->GetRoad(&pos, &old_road_type)) {
        t = ai->m_round_count->GetRound() + 10;
        p = new Pillaged_Node(pos, t, old_road_type);
        Enqueue(p); 
    } else  if (ai->m_world->HasUnderseaTunnel(&pos))  {
        t = ai->m_round_count->GetRound() + 10;
        p = new Pillaged_Node(pos, t, 0 );
        Enqueue(p); 
    }
}


void Opportunity_Action_Repair_Installation::RegisterPollution(AiMain *ai, MapPointData &pos)
{
     sint32 t;

     Pillaged_Node *p=NULL;

     m_dirty_tile_count++; 

     t = ai->m_round_count->GetRound();
     p = new Pillaged_Node(pos, t, 0 );
     Enqueue(p); 

}

