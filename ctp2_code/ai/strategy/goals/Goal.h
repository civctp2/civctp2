//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ header file
// Description  : the Goal motherclass header
// Id           : $Id$
//
//----------------------------------------------------------------------------
//
// Disclaimer
//
// THIS FILE IS NOT GENERATED OR SUPPORTED BY ACTIVISION.
//
// This material has been developed at apolyton.net by the Apolyton CtP2
// Source Code Project. Contact the authors at ctp2source@apolyton.net.
//
//----------------------------------------------------------------------------
//
// Compiler flags
//
// - None
//
//----------------------------------------------------------------------------
//
// Modifications from the original Activision code:
//
// - Removed MSVC specific code
// - Changes the const attribute for Compute_Matching_Value (Raw_Priority will 
//   be changed on wounded case) - Calvitix
//
//----------------------------------------------------------------------------

#ifndef __GOAL_H__
#define __GOAL_H__

#include <list>
#include <string>

class Goal;

#include "Plan.h"
#include "scheduler_types.h"
#include "squad_Strength.h"

class Goal
{
public:


    enum REMOVAL_TIME
    {
        REMOVE_WHEN_COMPLETE,
        DONT_REMOVE,


    };

    const static Utility BAD_UTILITY;

    const static Utility MAX_UTILITY;


protected:
	// Only part of derived classes
    Goal();


    Goal(const Goal & goal);

public:
    virtual ~Goal();


    virtual Goal & operator = (const Goal & goal);


    bool operator < (const Goal & goal) const;


    GOAL_TYPE Get_Goal_Type() const;


    void Set_Player_Index(const PLAYER_INDEX & player_index);


    PLAYER_INDEX Get_Player_Index() const;


    virtual bool Is_Satisfied() const;


    virtual bool Is_Goal_Undercommitted() const;


    sint16 Get_Agent_Count() const;


    bool Is_Single_Squad() const;


    bool Commit_Agent(const Agent_ptr & agent, Agent_List::const_iterator & agent_list_iter);


    const Agent_List & Goal::Get_Agent_List() const;


    Agent_ptr Rollback_Agent(Agent_List::const_iterator & agent_iter);


    virtual bool Is_Execute_Incrementally() const;


    virtual void Compute_Needed_Troop_Flow() = 0;

    virtual Utility Compute_Matching_Value(const Agent_ptr agent) const = 0;

    virtual Utility Compute_Raw_Priority() = 0;


    Utility Get_Raw_Priority() const;


    virtual GOAL_RESULT Execute_Task() = 0;


    virtual bool Get_Totally_Complete() const = 0;


    virtual bool Pretest_Bid(const Agent_ptr agent_ptr) const = 0;


    virtual void Set_Invalid(const bool & is_invalid);


    virtual bool Get_Invalid() const;


    virtual bool Get_Removal_Time() const;


    void Set_Removal_Time(const REMOVAL_TIME & removal_time);


    bool Can_Be_Executed() const;


    void Set_Can_Be_Executed(const bool & can_be_executed);


    virtual bool Validate() const;


    virtual void Log_Debug_Info(const int & log) const;


    void Add_Match_Reference(const Plan_List::iterator & plan_iter);


    void Remove_Match_Reference(const Plan_List::iterator & plan_iter);


    std::list < Plan_List::iterator > & Get_Match_References();


    void Set_Type(const GOAL_TYPE & type);


    void Set_Raw_Priority(const Utility & priority);


    bool Get_Is_Appropriate() const;


    bool Satisfied_By(const Squad_Strength & army_strength) const;


    bool Needs_Transport() const;


    const Squad_Strength Get_Strength_Needed() const;

protected:


    GOAL_TYPE m_goal_type;


    Utility m_raw_priority;


    REMOVAL_TIME m_removal_time;


    bool m_is_invalid;


    bool m_execute_incrementally;


    Squad_Strength m_current_needed_strength;


    Squad_Strength m_current_attacking_strength;


    std::list < Plan_List::iterator > m_match_references;


    Agent_List m_agents;


    PLAYER_INDEX m_playerId;


private:


};


#endif
