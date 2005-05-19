

#include "c3.h"
#include "globals.h"
#include "civarchive.h"
#include "FlatPtr.h"


#include "ic3GameState.h"
#include "ic3InstDB.h"
#include "aimain.h"

#include "CityAgent.h"

#include "InstBidList.h"


#include "GoalInst.h"
#include "GoalRoad.h"


InstallationBidList::InstallationBidList(sint32 ni)
{
	m_nInstallations = ni; 

	m_inst = new GoalInstallation*[ni]; 
	memset(m_inst, 0, ni * 4); 
    m_ngoals = new sint32[ni]; 
    memset(m_ngoals, 0, ni * sizeof(sint32)); 
}

InstallationBidList::InstallationBidList(BSet<CityAgent> *bs, CivArchive &archive)
{ 
    Serialize(bs, archive); 
} 

InstallationBidList::~InstallationBidList()
{
	Clear(); 

	
	delete[] m_inst;
    m_inst = NULL; 
	delete[] m_ngoals;
    m_ngoals = NULL; 

}

ZEROMEM(InstallationBidListFlat); 
FLATSERIALIZE(InstallationBidListFlat); 

void InstallationBidList::Serialize(BSet<CityAgent> *bs, CivArchive &archive)
{
	sint32 i, j; 
	GoalInstallation *ptr, **pptr; 

    CHECKSERIALIZE

    InstallationBidListFlat::Serialize(archive); 
    
    
    if (archive.IsStoring()) { 
        Clear(); 

        archive.Store((uint8*)m_ngoals, m_nInstallations * sizeof(sint32)); 

    	for (i=0; i<m_nInstallations; i++) { 		
            ptr = m_inst[i]; 
            for (j=0; j<m_ngoals[i]; j++, ptr = ptr->m_next) { 
                ptr->Serialize(bs, archive); 
            } 
	    }
    } else { 
        
        
        m_ngoals = new sint32[m_nInstallations]; 

        archive.Load((uint8*)m_ngoals, m_nInstallations * sizeof(sint32)); 

        m_inst = new GoalInstallation*[m_nInstallations]; 
       	for (i=0; i<m_nInstallations; i++) { 		
            pptr = &m_inst[i]; 
            *pptr = NULL; 
            for (j=0; j<m_ngoals[i]; j++, pptr = &((*pptr)->m_next)) { 
                *pptr = new GoalInstallation(bs, archive); 
            } 
            *pptr = NULL; 
	    }
    } 
}

void InstallationBidList::Clear()
{ 
    sint32 i; 
	GoalInstallation *ptr=NULL, *tmp=NULL; 

#ifdef _DEBUG
    sint32 finite_loop=0; 
#endif

	for (i=0; i<m_nInstallations; i++) { 		
		ptr = m_inst[i];
		while (ptr) { 
Assert(++finite_loop < 100000); 
			tmp = ptr; 
			ptr = ptr->m_next;

            switch (i) { 
			
            
            
            
            
            
            default:
                delete tmp; 
                break;
            }
		}
        m_inst[i]=NULL; 
        m_ngoals[i] = 0; 
	} 
}

void InstallationBidList::InsertGoal (GoalInstallation *goal_obj)
{
    sint32 t; 

    t = goal_obj->GetInstallationType(); 

    m_ngoals[t]++; 
    goal_obj->m_next = m_inst[t]; 
    m_inst[t] = goal_obj; 
}

void InstallationBidList::CalcDesiredPW(AiMain *ai)
{
    ai->m_desired_farm_pw = 0.0; 
    ai->m_desired_road_pw = 0.0; 
    ai->m_desired_mine_pw = 0.0; 
    ai->m_desired_transform_pw = 0.0; 
    ai->m_desired_inst_pw = 0.0; 
    
    double cost; 
    sint32 i, j; 
    GoalInstallation *ptr=NULL; 

    for (i=0; i<m_nInstallations; i++) { 
       for (ptr = m_inst[i], j=0; j<m_ngoals[i]; j++, ptr = ptr->m_next) { 
           cost = ptr->GetCost(); 
           switch (i) { 
               
				
#if 0
		   case TERRAIN_IMPROVEMENT_IRRIGATION_1:
               case TERRAIN_IMPROVEMENT_IRRIGATION_2:
               case TERRAIN_IMPROVEMENT_IRRIGATION_3:
                   ai->m_desired_farm_pw += cost; 
                   break; 

               case TERRAIN_IMPROVEMENT_ROAD_1:
               case TERRAIN_IMPROVEMENT_ROAD_2:
               case TERRAIN_IMPROVEMENT_ROAD_3:
               case TERRAIN_IMPROVEMENT_CANAL_TUNNEL:
                   ai->m_desired_road_pw += cost; 
                   break; 

               case TERRAIN_IMPROVEMENT_MINE_1:
               case TERRAIN_IMPROVEMENT_MINE_2:
               case TERRAIN_IMPROVEMENT_MINE_3:
                  ai->m_desired_mine_pw += cost; 
                  break; 


               case TERRAIN_IMPROVEMENT_TRANSFORM:
                   ai->m_desired_transform_pw += cost; 
                   break; 

               case TERRAIN_IMPROVEMENT_INSTALLATION: 
                   ai->m_desired_inst_pw += cost; 
                   break; 
#endif
                default:
                   ai->m_desired_inst_pw += cost; 
                   break; 
           } 
       }
    }
}


BOOL InstallationBidList::GetHighestProfit(AiMain *ai, 
    GoalInstallation *&max_goal)    
{
    sint32 i, j; 
    GoalInstallation *ptr=NULL; 
    double max_utility = -100.0; 
    double u; 

    for (i=0; i<m_nInstallations; i++) { 
        if (m_inst[i]) { 
           for (ptr = m_inst[i], j=0; j<m_ngoals[i]; j++, ptr = ptr->m_next) { 
              u = ptr->GetUtility();
              if (max_utility < u){ 
                  max_utility = u; 
                  max_goal = ptr; 
              } 
           }
        }
    } 

    if (max_utility <= 0.0) { 
        max_goal = NULL; 
        return FALSE; 
    } else { 
        max_goal->SetUtility(-101.0); 
        return TRUE; 
    } 
}


void InstallationBidList::RegisterMyRemoveCity(const uint32 remove_me)
{

#ifdef _DEBUG
    sint32 finite_loop=0; 
#endif

    GoalInstallation *old=NULL, **ptr=NULL;
    sint32 i; 
    CityAgent *city_ptr=NULL; 
    for (i=0; i<m_nInstallations; i++) { 
        if (m_inst[i]) { 
            ptr = &m_inst[i]; 
            do{                
                Assert(finite_loop++ < 100000);
                
                city_ptr = (*ptr)->GetHomeCity();
                if (NULL == city_ptr) { 
                    ptr = &((*ptr)->m_next); 
                    continue; 
                } 

                if (remove_me == city_ptr->GetID().GetVal()) { 
                    old = *ptr;
                    *ptr = (*ptr)->m_next; 
                    delete old; 
                    old = NULL; 
                    m_ngoals[i]--;
                    Assert(0 <= m_ngoals[i]);
                } else { 
                    ptr = &((*ptr)->m_next); 
                }
            } while (*ptr); 
        } 
    }
}

