
#pragma once

#ifndef __AGENT_H__
#define __AGENT_H__ 1

#include "BSet.h"





#include "common.h"
#include "linked_list.h"


#include "unitflow.h"
#include "squad.h"

#include "dr_debug.h"

#include "AILog.h"




#include "Strategic_AI_Forward.h"

enum AGENT_TYPE { 
    AGENT_TYPE_UNKNOWN,
    AGENT_TYPE_ARMY, 
    AGENT_TYPE_CITY,
    AGENT_TYPE_FOREIGN_ARMY, 
    AGENT_TYPE_FOREIGN_CITY
};
class ArmyGoal; 
class AiMain; 
class Squad_Strength; 

class Agent { 

protected:
	AGENT_TYPE m_agent_type; 

	
	


    Squad_Strength *m_agent_strength; 

public:
    
    BSetID m_id; 

    BOOL m_has_been_executed; 


	list_cell_ptr squads_list_cell_ptr; 
	list_cell_ptr agents_list_cell_ptr; 
	squad_ptr the_squad;				
	Planner *my_planner;				

    Agent(); 

	~Agent();

    void Serialize(CivArchive &archive); 

    virtual BOOL Validate(AiMain *ai); 

    BSetID GetID() const;
    
    AGENT_TYPE GetType() const; 

	void Log_Type(AiMain *ai);

    virtual double EstimateUtility(AiMain *ai, ArmyGoal *goal_obj);

    virtual void DumpStr(AiMain *ai, char *str);

    Squad_Strength * GetSquadStrength() { Assert(m_agent_strength); return m_agent_strength; }  

	
	
	
	SQUAD_CLASS Classify_Agents_Squad_Type(AiMain *ai);

};

#endif __AGENT_H__
