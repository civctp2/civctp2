










#ifndef __SQUAD_H__
#define __SQUAD_H__

#include "Plan.h"

#include "Scheduler_Types.h"







class Squad
{



public:

	
	
	
	
	

	
	
	
	
	

	
	
	
	
	
	
	Squad();

	
	
	
	
	
	
	Squad(const Squad &squad);

	
	
	
	
	
	
	virtual ~Squad ();

	
	
	
	
	
	
	
	virtual void Init();

	
	
	
	
	
	
	virtual Squad& operator= (const Squad &squad);

	
	virtual bool ContainsArmyIn(const Squad &squad) const;

	
	sint16 Remove_Dead_Agents();

	
	
	
	
	
	
	
	

	
	
	
	
	
	
	sint16 Get_Num_Agents() const; 

	
	
	
	
	
	
	
	

	
	
	
	
	
	
	Agent_ptr Get_Agent(const Agent_List::iterator & agent_iter) const;

	
	
	
	
	
    
    
    
    
	
	Agent_List::const_iterator Remove_Agent(const Agent_List::const_iterator & agent_iter,
                                      const bool & dealloc_agent = true);

	
	
	
	
	
	
	SQUAD_CLASS Compute_Squad_Class();

	
	
	
	
	
	
	SQUAD_CLASS Get_Squad_Class() const;

	
	
	
	
	
	
	
	void Add_Agent( Agent_ptr the_agent );

	
	
	
	
	
	
	
	void Remove_Agent( Agent_ptr the_agent );

	

	
	
	
	
	
	
	void Add_Match_Reference(const Plan_List::iterator &plan_iter);

	
	
	
	
	
	
	void Remove_Match_Reference(const Plan_List::iterator &plan_iter);

	
	
	
	
	
	
	std::list<Plan_List::iterator> & Get_Match_References();

    
    
    
    
    
    
    Agent_List & Get_Agent_List();
	const Agent_List & Get_Agent_List() const;

	
	void Set_Can_Be_Executed(const bool & can_be_executed);

	
	sint32 DisbandObsoleteArmies() const;

	
	
	

	
	void Log_Debug_Info(const int & log) const;

protected:

	
	
	
	
	

	
	bool m_is_committed;	

	
	SQUAD_CLASS m_squad_class;	

	
	Agent_List m_my_agents;		

	
	bool m_squad_changed;

	
    std::list<Plan_List::iterator> m_match_references;

	
	
	
	
	

private:

	
	
	
	
	

	
	
	
	
	
};






#endif __SQUAD_H__

