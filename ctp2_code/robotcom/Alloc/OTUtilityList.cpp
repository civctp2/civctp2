#include "c3.h"

#include "OTUtilityList.h"

#include "BSet.h"
#include "CivArchive.h"

#include "IC3GameState.h"
#include "AiMain.h"

#include "CityAgent.h"

extern double fz_rushbuy_max_utility;

OvertimeUtilityList::OvertimeUtilityList()
{
	m_num_goals = 0; 
	m_max_goals = 4; 

	m_goal = new GoalOvertime*[m_max_goals]; 
    sint32 i; 
    for (i=0; i<m_max_goals; i++) { 
        m_goal[i] = NULL; 
    } 
}

OvertimeUtilityList::OvertimeUtilityList(BSet<CityAgent>*bs, CivArchive &archive)
{
	Serialize(bs, archive); 
}

OvertimeUtilityList::~OvertimeUtilityList()
{
	sint32 i; 

	for (i=0; i<m_num_goals; i++) { 
		NDELETE (m_goal[i]); 
	}
	delete[] m_goal; 
    m_goal = NULL; 
}

void OvertimeUtilityList::Serialize(BSet<CityAgent> *bs, CivArchive &archive)
{
	sint32 i; 

    CHECKSERIALIZE

	if (archive.IsStoring()) { 

Clear(); 

		archive << m_num_goals; 
		archive << m_max_goals; 
		for (i=0; i<m_num_goals; i++) { 
			m_goal[i]->Serialize(bs, archive);
		}
	} else { 
		archive >> m_num_goals; 
		archive >> m_max_goals; 
        m_goal = new GoalOvertime*[m_max_goals]; 
		for (i=0; i<m_num_goals; i++) { 
            m_goal[i] = new GoalOvertime(bs, archive); 
		}
        for (i=m_num_goals; i<m_max_goals; i++) { 
            m_goal[i] = NULL; 
        } 
	}
}

void OvertimeUtilityList::DoubleGoal()
{
    GoalOvertime **tmp; 

    tmp = new GoalOvertime*[2*m_max_goals]; 
    memcpy(tmp, m_goal, m_num_goals * sizeof(GoalOvertime*)); 
    delete[] m_goal; 
    m_goal = tmp; 
    m_max_goals *= 2; 
}



void OvertimeUtilityList::Insert(GoalOvertime *goal_obj)
{
    if (m_max_goals <= m_num_goals) { 
        DoubleGoal(); 
    } 

    m_goal[m_num_goals++] = goal_obj; 
}


void OvertimeUtilityList::GetMax(GoalOvertime *&max_goal, double &max_utility, 
   double &min_cost)
{ 
    GoalOvertime *goal_obj=NULL;
    double utility; 
    sint32 i; 
  
    min_cost = 10000000.0;
    max_utility = NULL; 
    max_goal = NULL;
	
    max_utility = fz_rushbuy_max_utility; 

    
    for (i=0; i<m_num_goals; i++) { 

        if (!m_goal[i]->GetIsSatisfied()) { 

            utility = m_goal[i]->GetUtility(); 

            if ((sint32(max_utility) < sint32(utility)) ||
                ((sint32(max_utility) == sint32(utility)) && 
                (m_goal[i]->GetCost() < min_cost))) 
			{   
                max_utility = utility; 
                min_cost = m_goal[i]->GetCost();
                max_goal = m_goal[i]; 
            }
        }
   }

    
   if (max_goal) { 
	   
	   
       
       
       
       

       Assert(0.0 < max_goal->GetCost()); 
       if (max_goal->GetCost() < 0.0) { 
            max_goal = NULL; 
            return; 
       } 
   }
} 

void OvertimeUtilityList::Clear()

{
    sint32 i; 

    for (i=0; i<m_num_goals; i++) { 
        NDELETE(m_goal[i]); 
    } 
    m_num_goals = 0; 
}


void OvertimeUtilityList::RegisterMyRemoveCity(const uint32 del_me)
{

    
    if (NULL == m_goal)
        return; 

    
    sint32 del_count=0; 
    sint32 goal_idx; 
    for (goal_idx=0; goal_idx<m_num_goals; goal_idx++) { 
        if (m_goal[goal_idx]->GetHomeCity()->GetID().GetVal() == del_me) { 
            delete m_goal[goal_idx]; 
            m_goal[goal_idx] = NULL; 
            del_count++; 
        }
    } 

    if (del_count < 1) {
        return; 
    }

    
    for (goal_idx=0; goal_idx<m_num_goals; goal_idx++) { 
        if (NULL == m_goal[goal_idx]) { 
           break; 
        } 
    } 

    
    sint32 dest, src; 
    dest = goal_idx; 
    src = goal_idx+1;


    
#ifdef _DEBUG
    sint32 finite_loop=0; 
#endif
    while (src <m_num_goals) { 

        while ((src <m_num_goals) && (NULL == m_goal[src])) { 
            Assert(finite_loop++ < 10000); 
        }

        if (src < m_num_goals) { 
            m_goal[dest] = m_goal[src]; 
            dest++; 
            src++; 
        }
    }

    Assert(del_count == (m_num_goals - dest)); 

    
    for (goal_idx = dest; goal_idx < m_num_goals; goal_idx++) { 
        m_goal[goal_idx] = NULL; 
    } 

    
    m_num_goals -= del_count; 
}

