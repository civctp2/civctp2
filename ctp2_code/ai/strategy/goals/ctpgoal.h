//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ header file
// Description  : CTP2 specific goal handling
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
// - Set the SUB_TASK_TYPE global - Calvitix
// - Added an SUB_TASK_TYPE attribute (used for armytext display) - Calvitix
// - Added methods to Ungroup armies - Calvitix
// - Changes the const attribute for Compute_Matching_Value 
//   (Raw_Priority will be changed on wounded case) - Calvitix
// - Removed Microsoft specific code.
// - Fixed Goal subtask handling. (26-Jan-2008 Martin Gühmann)
// - Use more than one transporter if the goal needs more than one. (8-Feb-2008 Martin Gühmann)
// - Redesigned AI, so that the matching algorithm is now a greedy algorithm. (13-Aug-2008 Martin Gühmann)
// - Now the goals are used for the matching process, the goal match value
//   is the avarage match value of the matches needed for the goal.
//
//----------------------------------------------------------------------------

#ifdef HAVE_PRAGMA_ONCE
#pragma once
#endif
#ifndef __CTP_GOAL_H__
#define __CTP_GOAL_H__ 1

#include "Goal.h"
#include "MapPoint.h"
#include "Army.h"
#include "Unit.h"

class CTPGoal;
class CTPAgent;
typedef CTPGoal * CTPGoal_ptr;
typedef CTPAgent * CTPAgent_ptr;


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


class CTPGoal : public Goal
{
public:
    CTPGoal();
    virtual ~CTPGoal();


    bool operator == (const CTPGoal & rval) const;
    bool operator < (const CTPGoal & rval) const;

    void Set_Target_Pos(const MapPoint & pos);


    void Set_Target_Army(const Army & army);


    void Set_Target_City(const Unit & city);


    const MapPoint Get_Target_Pos(const Army & army) const;


    const MapPoint & Get_Target_Pos() const;


    const Army & Get_Target_Army() const;

    const SUB_TASK_TYPE & Get_Sub_Task() const;

    void Set_Sub_Task(const SUB_TASK_TYPE & sub_task);



    const Unit & Get_Target_City() const;


    sint32 Get_Target_Value() const;


    PLAYER_INDEX Get_Target_Owner() const;


    Agent_ptr Rollback_Agent(Agent_List::const_iterator & agent_iter);


    bool Is_Execute_Incrementally() const;


    void Compute_Needed_Troop_Flow();

    Utility Compute_Matching_Value (const Agent_ptr agent    ) const;

    Utility Compute_Raw_Priority();


    GOAL_RESULT Execute_Task();


    bool Get_Totally_Complete() const;


    bool Get_Invalid() const;


    bool Get_Removal_Time() const;


    bool Pretest_Bid(const Agent_ptr agent_ptr) const;


    bool Pretest_Bid(const Agent_ptr agent_ptr, const MapPoint & cache_pos) const;


    bool ArmiesAtGoal() const;


    sint32 GetThreatenBonus() const;


    bool ReferencesAgent(const CTPAgent * ctp_agent) const;


    void Log_Debug_Info(const int & log) const;
    void Log_Debug_Info_Full(const int & log) const;

    bool Can_Add_To_Goal(const Agent_ptr agent_ptr) const;


protected:


    bool NeededForGarrison(CTPAgent_ptr army,
                           const MapPoint & dest_pos,
                           sint8 & garrison_count,
                           double & garrison_strength) const;

    bool FindPathToTask(CTPAgent_ptr the_army,
                        const MapPoint & goal_pos,
                        Path & found_path);

    bool FollowPathToTask(CTPAgent_ptr first_army,
                          CTPAgent_ptr second_army,
                          const MapPoint & dest_pos,
                          const Path & path);


    bool GotoTransportTaskSolution(CTPAgent_ptr the_army, CTPAgent_ptr the_transport, MapPoint & pos);


    bool GotoGoalTaskSolution(CTPAgent_ptr the_army, const MapPoint & goal_pos);


    bool Ok_To_Rally() const;


    bool RallyComplete() const;

    MapPoint MoveOutOfCity(CTPAgent_ptr rallyAgent);
    CTPAgent_ptr GetRallyAgent() const;
    MapPoint GetFreeNeighborPos(MapPoint pos) const;
    bool RallyTroops();
    void GroupTroops();

    bool UnGroupTroops();

    bool UnGroupComplete() const;

    bool Goal_Too_Expensive() const;


    bool TryTransport(CTPAgent_ptr agent_ptr, const MapPoint & goal_pos);


    bool FindTransporters(const CTPAgent_ptr & agent_ptr, std::list< std::pair<Utility, CTPAgent_ptr> > & transporter_list);


    bool LoadTransporters(CTPAgent_ptr agent_ptr);

    MapPoint      m_target_pos;
    MapPoint      m_transport_pos;
    Unit          m_target_city;
    Army          m_target_army;
    SUB_TASK_TYPE m_sub_task;
};

#endif // __CTP_GOAL_H__
