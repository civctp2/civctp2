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
// - Set the SUB_TASK_TYPE global - Calvitix
// - Added an SUB_TASK_TYPE attribute (used for armytext display) - Calvitix
// - Added methods to Ungroup armies - Calvitix
// - Changes the const attribute for Compute_Matching_Value (Raw_Priority will 
//   be changed on wounded case) - Calvitix
// - Fixed Goal subtask handling. (26-Jan-2008 Martin Gühmann)
// - Use more than one transporter if the goal needs more than one. (8-Feb-2008 Martin Gühmann)
// - Redesigned AI, so that the matching algorithm is now a greedy algorithm. (13-Aug-2008 Martin Gühmann)
// - Now the goals are used for the matching process, the goal match value
//   is the avarage match value of the matches needed for the goal.
// - Merged in CTPGoal, removed virtual functions, for design and speed
//   improvement. (28-Jan-2009 Martin Gühmann)
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
#include "MapPoint.h"
#include "Army.h"
#include "Unit.h"

class Agent;

//Now the enum is global and can be accessed by other objects
enum SUB_TASK_TYPE
{
    SUB_TASK_GOAL,
    SUB_TASK_RALLY,
    SUB_TASK_TRANSPORT_TO_BOARD,
    SUB_TASK_TRANSPORT_TO_GOAL,
    SUB_TASK_CARGO_TO_BOARD,
    SUB_TASK_AIRLIFT,
    SUB_TASK_UNGROUP
};

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

    Goal();
    Goal(const Goal & goal);
    ~Goal();


    Goal & operator = (const Goal & goal);
    bool operator ==  (const Goal & rval) const;
    bool operator <   (const Goal & goal) const { return (m_raw_priority < goal.m_raw_priority); };


    GOAL_TYPE Get_Goal_Type() const { return m_goal_type; };


    void Set_Player_Index(const PLAYER_INDEX & playerId) { m_playerId = playerId; };


    PLAYER_INDEX Get_Player_Index() const { return m_playerId; };


    bool Is_Satisfied() const;


    bool Is_Goal_Undercommitted() const { return (!Is_Satisfied() && m_agents.size() > 0); };


    sint16 Get_Agent_Count() const { return m_agents.size(); };


    bool Is_Single_Agent() const { return m_agents.size() == 1; };


    void Commit_Agent(const Agent_ptr & agent);


    void Rollback_Agent(Agent_ptr agent_ptr);
    void Rollback_Emptied_Transporters();

    bool Is_Execute_Incrementally() const;


    void Compute_Needed_Troop_Flow();

    Utility Compute_Agent_Matching_Value(const Agent_ptr agent_ptr) const;

    Utility Get_Initial_Priority() const;
    Utility Compute_Raw_Priority();


    Utility Get_Raw_Priority() const { return m_raw_priority; };


    GOAL_RESULT Execute_Task();


    bool Get_Invalid() const;


    bool Get_Removal_Time() const;


    void Set_Removal_Time(const REMOVAL_TIME & removal_time) { m_removal_time = removal_time; };


    bool Can_Be_Executed() const;


    void Set_Can_Be_Executed(const bool & can_be_executed);


    void Set_Type(const GOAL_TYPE & type);


    void Set_Raw_Priority(const Utility & priority);


    bool Get_Is_Appropriate() const;


    bool Satisfied_By(const Squad_Strength & army_strength) const;


    bool Needs_Transporter() const;


    const Squad_Strength Get_Strength_Needed() const;

    Utility Compute_Matching_With_Generic_Matches(Goal_ptr genric_goal, const bool update = true) { return Compute_Matching_Value(genric_goal->m_matches, update); };
    Utility Compute_Matching_Value(Plan_List & matches, const bool update = true);
    Utility Compute_Matching_Value(const bool update = true) { return Compute_Matching_Value(m_matches, update); };
    Utility Recompute_Matching_Value(Plan_List & matches, const bool update = true, const bool show_strength = true);
    Utility Recompute_Matching_Value(const bool update = true, const bool show_strength = true) { return Recompute_Matching_Value(m_matches, update, show_strength); };
    Utility Get_Matching_Value() const;
    void    Set_Matching_Value(Utility combinedUtility);

    bool Add_Match(const Agent_ptr & agent, const bool update_match_value = true, const bool needsCargo = false);
    bool Add_Transport_Match(const Agent_ptr & agent) { return Add_Match(agent, true, true); };

    bool CanGoalBeReevaluated() const;
    bool Commited_Agents_Need_Orders() const;
    void Rollback_All_Agents();
    void Commit_Agents();
    void Commit_Transport_Agents();
    void Remove_Matches();
    void Remove_Match(const Agent_ptr & agent);
    bool Has_Agent_And_Set_Needs_Cargo(Agent* agent);
    bool Needs_Cargo(Agent* agent);
    bool Cannot_Be_Used(Agent* agent);
    void Set_Cannot_Be_Used(Agent* agent, bool cannotBeUsed);
    size_t Get_Matches_Num() const { return m_matches.size(); }
    void Set_Needs_Transporter(const bool needs_transporter);
    void Set_Needs_Transporter(Agent_ptr agent_ptr);
    sint16 Get_Transporters_Num() const { return m_current_attacking_strength.Get_Transport(); }

    void Recompute_Current_Attacking_Strength();
    Squad_Strength Compute_Current_Strength();

    void Sort_Matches_If_Necessary();
    void Set_Target_Pos(const MapPoint & pos);


    void Set_Target_Army(const Army & army);


    void Set_Target_City(const Unit & city);


    const MapPoint & Get_Target_Pos() const;
    const MapPoint Get_Target_Pos(const Army & army) const;

    const Army & Get_Target_Army() const;

    const SUB_TASK_TYPE & Get_Sub_Task() const;

    void Set_Sub_Task(const SUB_TASK_TYPE & sub_task);



    const Unit & Get_Target_City() const;


    sint32 Get_Target_Value() const;


    PLAYER_INDEX Get_Target_Owner() const;


    bool Pretest_Bid(const Agent_ptr agent_ptr, const MapPoint & target_pos) const;


    bool ArmiesAtGoal() const;


    sint32 GetThreatenBonus() const;


    void Log_Debug_Info(const int & log) const;
    void Log_Debug_Info_Full(const int & log) const;

    void Copy_Insert_Matches(Goal_ptr generic_goal)
    {
        m_matches = generic_goal->m_matches;
        this->Compute_Matching_Value();
    };

    bool CanReachTargetContinent(Agent_ptr agent_ptr) const;

private:

    bool Get_Totally_Complete() const;

    bool FindPathToTask(Agent_ptr the_army,
                        const MapPoint & goal_pos,
                        Path & found_path);

    bool FollowPathToTask(Agent_ptr first_army,
                          Agent_ptr second_army,
                          MapPoint & dest_pos,
                          Path & path);


    bool GotoTransportTaskSolution(Agent_ptr the_army, Agent_ptr the_transport, MapPoint & pos);


    bool GotoGoalTaskSolution(Agent_ptr the_army, MapPoint & goal_pos);


    bool Ok_To_Rally() const;


    bool RallyComplete() const;

    MapPoint MoveToTarget(Agent_ptr rallyAgent);
    MapPoint MoveOutOfCity(Agent_ptr rallyAgent);
    Agent_ptr GetRallyAgent() const;
    MapPoint GetFreeNeighborPos(MapPoint pos) const;
    bool RallyTroops();
    void GroupTroops();

    bool UnGroupTroops();

    bool UnGroupComplete() const;

    bool Goal_Too_Expensive() const;


    bool TryTransport(Agent_ptr agent_ptr, const MapPoint & goal_pos);


    bool FindTransporters(const Agent_ptr & agent_ptr, std::list< std::pair<Utility, Agent_ptr> > & transporter_list);


    bool LoadTransporters(Agent_ptr agent_ptr);

    MapPoint GetClosestTransporterPos(const Agent_ptr agent_ptr) const;
    MapPoint GetClosestCargoPos      (const Agent_ptr agent_ptr) const;

    inline void Sort_Matches(Plan_List & matches);

    Squad_Strength                    m_current_needed_strength;
    Squad_Strength                    m_current_attacking_strength;
    Plan_List                         m_matches;
    Agent_List                        m_agents;
    REMOVAL_TIME                      m_removal_time;
    PLAYER_INDEX                      m_playerId;
    Utility                           m_raw_priority;
    Utility                           m_combinedUtility;
    MapPoint                          m_target_pos;
    Unit                              m_target_city;
    Army                              m_target_army;
    SUB_TASK_TYPE                     m_sub_task;
    GOAL_TYPE                         m_goal_type;
    bool                              m_needs_sorting;
};

namespace std {
template<>
struct greater<Goal_ptr> : binary_function<Goal_ptr, Goal_ptr, bool>
{
	bool operator()(const Goal_ptr _X, const Goal_ptr _Y) const
	{
		return _X->Get_Matching_Value() > _Y->Get_Matching_Value();
	}
};
}

#endif
