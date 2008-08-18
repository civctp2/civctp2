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
// - Redesigned AI, so that the matching algorithm is now a greedy algorithm. (13-Aug-2008 Martin Gühmann)
// - Now the goals are used for the matching process, the goal match value
//   is the avarage match value of the matches needed for the goal.
//
//----------------------------------------------------------------------------

#ifndef __GOAL_H__
#define __GOAL_H__

#include <list>
#include <string>

class Goal;

#include "scheduler_types.h"
#include "Plan.h"
#include "squad_Strength.h"
#include "Squad.h"

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


    bool Commit_Agent(const Agent_ptr & agent);


    const Agent_List & Get_Agent_List() const;


    void Rollback_Agent(Agent_ptr agent_ptr);
    sint32 Rollback_Emptied_Transporters();

    virtual bool Is_Execute_Incrementally() const = 0;


    virtual void Compute_Needed_Troop_Flow() = 0;

    virtual Utility Compute_Matching_Value (const Agent_ptr agent) const = 0;

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


    virtual void Log_Debug_Info(const int & log) const = 0;


    void Set_Type(const GOAL_TYPE & type);


    void Set_Raw_Priority(const Utility & priority);


    bool Get_Is_Appropriate() const;


    bool Satisfied_By(const Squad_Strength & army_strength) const;


    bool Needs_Transporter() const;


    const Squad_Strength Get_Strength_Needed() const;
    virtual const MapPoint & Get_Target_Pos() const = 0;


    Utility Compute_Matching_Value(const bool update = true);
    Utility Recompute_Matching_Value(const bool update = true, const bool show_strength = true);
    Utility Get_Matching_Value() const;
    void    Set_Matching_Value(Utility combinedUtility);

    bool Add_Match(const Squad_ptr & squad, const bool update_match_value = true, const bool needsCargo = false);
    bool Add_Transport_Match(const Squad_ptr & squad) { return Add_Match(squad, true, true); };

    bool CanGoalBeReevaluated() const;
    bool Commited_Agents_Need_Orders() const;
    sint32 Rollback_All_Agents();
    sint32 Commit_Agents();
    sint32 Commit_Transport_Agents();
    sint32 Remove_Matches();
    sint32 Remove_Match(Plan_List::iterator match);
    bool Has_Squad(Squad* squad);
    size_t Get_Matches_Num() const { return m_matches.size(); }
    void Set_Needs_Transporter(const bool needs_transporter);
    sint16 Get_Transporters_Num() const { return m_current_attacking_strength.Get_Transport(); }

    void Recompute_Current_Attacking_Strength();
    Squad_Strength Compute_Current_Strength();

    virtual bool Can_Add_To_Goal(const Agent_ptr agent_ptr) const = 0;

    void Sort_Matches_If_Necessary();

protected:

    inline void Sort_Matches();

    GOAL_TYPE                         m_goal_type;
    Utility                           m_raw_priority;
    REMOVAL_TIME                      m_removal_time;
    bool                              m_is_invalid;
    Squad_Strength                    m_current_needed_strength;
    Squad_Strength                    m_current_attacking_strength;
    Squad_Strength                    m_current_projected_strength;
    Plan_List                         m_matches;
    Agent_List                        m_agents;
    PLAYER_INDEX                      m_playerId;
    Utility                           m_combinedUtility;
    bool                              m_needs_sorting;
};

template<>
struct std::greater<Goal_ptr> : std::binary_function<Goal_ptr, Goal_ptr, bool>
{
	bool operator()(const Goal_ptr _X, const Goal_ptr _Y) const
	{
		return _X->Get_Matching_Value() > _Y->Get_Matching_Value();
	}
};

#endif
