







 
#ifndef __PLAN_H__
#define __PLAN_H__ 1



#include "scheduler_types.h"
#include <list-fixed>

class Plan
{ 

public:

  	
  	
  	
  	
  	
    struct Agent_Match {
        Utility value;
        Agent_List::const_iterator squad_index;
        Agent_List::const_iterator goal_index;
        bool committed;
        bool operator<(const Agent_Match &match) const { 
            return value < match.value; 
        }
        bool operator>(const Agent_Match &match) const { 
            return value > match.value; 
        }
    };
    
    typedef std::list<Agent_Match> Agent_Match_List;


  	
  	
  	
  	
  	

	
	
	
	
	

	
	
	

	
	
	
	
	
	
    Plan();

	
	
	
	
	
	
    virtual ~Plan(); 

	
	
	
	
	
	
	Plan(const Plan &plan);
	
	
	
	
	
	
	
	Plan& operator= (const Plan &plan);

	
	
	
	
	
	
	bool operator< (const Plan &plan) const;

    
	
	
	
	
	
	bool operator> (const Plan &plan) const;

	
	bool operator== (const Plan &plan) const;

	
	bool operator!= (const Plan &plan) const;


	
	
	
	
	
	
	void Init();

    

	
	
	
	
	
	
	GOAL_TYPE Get_Goal_Type() const;

	
	
	
	
	
	
    bool Plan_Is_Needed_And_Valid() const;

	
	
	
	
	
	
    Utility Compute_Matching_Value();


	
	
	
	
	
	
    Utility Get_Matching_Value() const;

	
	
	
	
	
	
    void Set_Goal(Goal_ptr goal);

	
	
	
	
	
	
    Goal_ptr Get_Goal() const;

	
	
	
	
	
	
    void Set_Squad(Squad_ptr squad);

	
	
	
	
	
	
    Squad_ptr Get_Squad() const;

	
	
	

	
	
	
	
	
	
	
	
	
	sint16 Commit_Agents();
	
	
	
	
	
	
	
	
	
	
	
	
    GOAL_RESULT Execute_Task();

	
	
	
	
	
	
	
	
	
	sint16 Rollback_Invalid_Agents();

	
	
	
	
	
	
	
	
	
	sint16 Rollback_All_Agents();

 	
 	bool Commited_Agents_Need_Orders() const;

	
	
	
	
	
	
	
	
	
	
	
	
	sint16 Move_All_Agents(Squad_ptr new_squad);

	
	
	
	
	
    
    
	
    bool Remove_Agent_Reference(const Agent_List::const_iterator & agent_iter);

	
	bool Agent_Committed(const Agent_ptr agent_ptr) const;


protected:

	
	
	
	
	

	
	Utility m_matching_value;

	
	Squad_ptr m_the_squad;

	
	Goal_ptr m_the_goal;

	
	Agent_Match_List m_matches;

	
	
	
	
	
};





#endif __PLAN_H__
