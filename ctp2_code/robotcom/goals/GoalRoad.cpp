




#include "c3.h"
#include "globals.h"
#include "IC3World.h"
#include "IC3InstDB.h"
#include "IC3Player.h"

#include "GoalRoad.h"
#include "FlatPtr.h"
#include "CivArchive.h"
#include "TerrImproveData.h"
#include "CityAgent.h"
#include "IC3RobotAstar.h"
#include "AiMain.h"
#include "AiCause.h"
#include "ScienceAgent.h"

extern double fz_inst_threat_threshold;
extern double fz_inst_road_coef;

#include "Scheduler.h"
#include "Planner.h"
#include "Grassfire.h"
#include "strategic_map.h"










GoalRoad::GoalRoad(
    AiMain *ai, 
    const sint32 rt, 
    CityAgent *sa, 
    CityAgent *da, 
    sint32 bufSize, 
    MapPointData *buffer, 
    sint32 nPoints
    )
{

    Assert(sa); 
    Assert(da); 
    Assert(sa != da); 
    Assert(3 <= rt);
    Assert(rt <= 5);
    Assert(buffer); 

    m_start = sa; 
    m_dest = da; 

    m_road_type = rt; 


    m_bufSize= bufSize; 
    m_buffer= buffer; 
    m_nPoints = nPoints; 
    m_total_cost = 0;

    
   sint32 i; 
   sint32 old_road_type; 
   for (i=0; i<m_nPoints; i++) { 
      if (ai->m_world->GetRoad(&m_buffer[i], &old_road_type)) {
         if (old_road_type < m_road_type) { 
             m_total_cost += ai->m_installationDB->GetCost(m_road_type, &m_buffer[i], 0); 
         }
      } else { 
           m_total_cost += ai->m_installationDB->GetCost(m_road_type, &m_buffer[i], 0); 
      }
   }

    m_utility = fz_inst_road_coef * (10000 - m_total_cost); 

    m_type = rt; 
    m_pos = new MapPointData; 
    sa->GetPos(*m_pos);
    m_home_city_agent = sa; 
    m_next = NULL; 
    m_cost = m_total_cost; 
}

GoalRoad::~GoalRoad() 
{ 
    
    if (m_buffer)
		delete[] m_buffer;
} 

sint32 GoalRoad::GetInstallationType() const
{
    return m_road_type;
}



sint32 GoalRoad::GetCost() const
{
    return m_total_cost; 
}

void GoalRoad::GetPos(MapPointData &pos)
{

    _ASSERTE(0); 
}
 








void GoalRoad::Construct(AiMain *ai, sint32 &stored)
{
   sint32 i; 
   sint32 cost=0; 
   BOOL is_broken = FALSE; 

   if (stored < m_total_cost) { 
       stored = 0;              
       return; 
   } 

   ERR_BUILD_INST err; 
   MapPoint XYpos; 
   double threat;
   for (i=1; i<(m_nPoints-1); i++) { 
       ai->m_player->Norm2XY(m_buffer[i], XYpos);
       threat = ai->m_planner->the_strategic_map->Get_Threat(XYpos);    
       if (fz_inst_threat_threshold < threat) {
           is_broken = TRUE;     
       } else if (ai->m_world->IsConnectedToACity(&m_buffer[i], ai->m_my_player_id)) { 
			
		   if (ai->m_science_agent->HasUnderseaTunnel() && ai->m_world->IsWater(&m_buffer[i])) { 
              ai->m_installationDB->CanBuildHere(0, 0, &m_buffer[i], &err); 
             if ((ERR_BUILD_INST_OK == err) || (ERR_BUILD_INST_UPGRADE == err)) { 
                 cost += ai->m_installationDB->GetCost(0 , &m_buffer[i], 0); 
                 if (cost <= stored) {                
                     if (FALSE == ai->m_installationDB->ConstructInstallation(0 , 0, &m_buffer[i])) { 
                         is_broken = TRUE; 
                     }

                 } else { 
                     is_broken = TRUE; 
                 }
             } else if (err != ERR_BUILD_INST_REDUNDENT) { 
                 is_broken = TRUE; 
             } 
          } else { 
             ai->m_installationDB->CanBuildHere(m_road_type, 0, &m_buffer[i], &err); 
             if ((ERR_BUILD_INST_OK == err) || (ERR_BUILD_INST_UPGRADE == err)) { 
                 cost += ai->m_installationDB->GetCost(m_road_type, &m_buffer[i], 0); 
                 if (cost <= stored) {                
                     if (FALSE == ai->m_installationDB->ConstructInstallation(m_road_type, 0, &m_buffer[i])) { 
                         is_broken = TRUE;
                     }
                 } else { 
                     is_broken = TRUE; 
                 }
             } else if (err != ERR_BUILD_INST_REDUNDENT) { 
                 is_broken = TRUE;
             }
          }
      } else { 
          is_broken = TRUE; 
      }
   }
 
   stored -= cost; 

   if (!is_broken) { 
       m_start->InsertRoad(m_dest, m_road_type); 
       m_dest->InsertRoad(m_start, m_road_type);  
   }
}



