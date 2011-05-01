//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ header
// Description  : Goal handling
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
// - Improved design
// - Made FindPath and GetRounds methods more flexible. (25-Jan-2008 Martin Gühmann)
// - Redesigned AI, so that the matching algorithm is now a greedy algorithm. (13-Aug-2008 Martin Gühmann)
// - Fixed unit garrison assignment. (23-Jan-2009 Martin Gühmann)
// - Merged in CTPAgent, removed virtual functions, for design and speed
//   improvement. (29-Jan-2009 Martin Gühmann)
// - Merged in Squad, no need for an additional class, just wastes space. (29-Jan-2009 Martin Gühmann)
// - Removed last Squad remainings, no need to allocate all that memory. (14-Feb-2009 Martin Gühmann)
//
//----------------------------------------------------------------------------

#ifdef HAVE_PRAGMA_ONCE
#pragma once
#endif

#ifndef __AGENT_H__
#define __AGENT_H__ 1

class Agent;

#include "c3.h"

#include "Goal.h"               // Needed here to instantaite std::greater<Goal_ptr> correctly
#include "scheduler_types.h"
#include "squad_Strength.h"
#include "Army.h"               // Army
#include "MapPoint.h"           // MapPoint
#include "Path.h"
#include "player.h"             // PLAYER_INDEX
#include "scheduler_types.h"    // SQUAD_CLASS, Squad_Strength, etc.
#include "GameEventTypes.h"
#include "c3debugstl.h"
#include "Army.h"
#include "ArmyData.h"
#include "World.h"          // g_theWorld

class Agent
{
public:
#if defined(_DEBUG)
	typedef std::list<Goal_ptr, dbgallocator<Goal_ptr> > Goal_Ref_List;
#else
	typedef std::list<Goal_ptr> Goal_Ref_List;
#endif

private:
	Agent();
public:
	explicit Agent(const Army & army);
	Agent(const Agent & an_Original);
	~Agent();

	Agent& operator= (const Agent & an_Original);

	sint16 Get_Type() const { return m_agent_type; };

	void Set_Type(const sint16 & type) { m_agent_type = type; };

	SQUAD_CLASS Get_Squad_Class() const { return m_squad_class; };

	void     Set_Goal(Goal_ptr goal)                 { m_goal = goal; };
	Goal_ptr Get_Goal()                        const { return m_goal; };
	bool     Has_Any_Goal()                    const { return m_goal != NULL; };
	bool     Has_Goal(const Goal * const goal) const { return m_goal == goal; };
	sint32   Get_Goal_Type()                   const { return Has_Any_Goal() ? m_goal->Get_Goal_Type() : -1; }

	bool Get_Is_Dead() const { return !m_army.IsValid() || m_army->GetOwner() != m_playerId; }
;

	void Set_Squad_Class(const SQUAD_CLASS & squad_class) { m_squad_class = squad_class; };

	SQUAD_CLASS Compute_Squad_Class();

	const Squad_Strength & Get_Squad_Strength() const { return m_squad_strength; };

	const Squad_Strength & Compute_Squad_Strength();

	void Set_Can_Be_Executed(const bool &can_be_executed) { m_can_be_executed = can_be_executed; };

	bool Get_Can_Be_Executed() const { return m_can_be_executed && !m_detached; };

	void Set_Detached(const bool detached) { m_detached = detached; };

	bool Get_Detached() const { return m_detached; };

	void Log_Debug_Info(const int & log, const Goal * const goal) const;

	const Army & Get_Army() const { return m_army; };


	PLAYER_INDEX Get_Player_Number() const { return m_playerId; };


	MapPoint Get_Pos() const { return m_army.IsValid() ? m_army->RetPos() : MapPoint(); };

	sint32 GetUnitsAtPos() const { return g_theWorld->GetCell(Get_Pos())->GetNumUnits(); };
	bool IsArmyPosFilled() const { return g_theWorld->GetCell(Get_Pos())->GetNumUnits() >= k_MAX_ARMY_SIZE; };
	bool IsOneArmyAtPos () const { return g_theWorld->GetCell(Get_Pos())->GetNumUnits() == Get_Army()->Num(); };

	bool CanMove() const { return m_army.IsValid() && m_army->CanMove(); };


	bool FindPathToBoard( const uint32 & move_intersection, const MapPoint & dest_pos, const bool & check_dest, Path & found_path );


	static bool FindPath(const Army & army, const MapPoint & target_pos, const bool & check_dest, Path & found_path );
	static bool FindPath(const Army & army, const MapPoint & target_pos, const bool & check_dest, Path & found_path, float & total_cost);

	sint32 GetRounds(const MapPoint & pos, sint32 & cells) const;
	double GetRoundsPrecise(const MapPoint & pos, sint32 & cells) const;

	bool CanReachTargetContinent(const MapPoint & pos) const;
	bool EstimateTransportUtility(const Agent_ptr transport, Utility & utility) const;

	void Set_Target_Order(const sint32   &target_order) { m_targetOrder = target_order; };
	void Set_Target_Pos  (const MapPoint &target_pos)   { m_targetPos   = target_pos;   };

	sint32           Get_Target_Order() const { return m_targetOrder; };
	const MapPoint & Get_Target_Pos  () const { return m_targetPos;   };

#if 0
	void Follow_Path(const Path & found_path, const sint32 & order_type);
#endif

	bool Can_Execute_Order(const sint32 & order_type) const;


	void Execute_Order(const sint32 & order_type, const MapPoint & order_pos);


	void Group_Order();


	void Group_With( Agent_ptr second_army );


	void Ungroup_Order();

	sint32 DisbandObsoleteArmies();
	inline sint32 DisbandObsoleteUnits();

	void MoveIntoTransport();
	void UnloadCargo();
	void PerformOrderHere(const OrderRecord * order_rec, const Path * path, GAME_EVENT_INSERT priority = GEV_INSERT_AfterCurrent);
	void PerformOrder(const OrderRecord * order_rec);
	void WaitHere(const MapPoint & goal_pos);
	void ClearOrders();
	bool HasMovePoints() const;

	bool IsNeededForGarrison() const { return m_neededForGarrison; };
	void SetIsNeededForGarrison(bool neededForGarrison) { m_neededForGarrison = neededForGarrison; };

#ifdef _DEBUG_SCHEDULER
	bool ContainsArmyIn(const Agent_ptr agent) const { return Get_Army() == agent->Get_Army(); };
#endif

private:

	Squad_Strength m_squad_strength;
	Goal_ptr       m_goal;
	Army           m_army;
	sint32         m_playerId;
	sint32         m_targetOrder;
	MapPoint       m_targetPos;
	SQUAD_CLASS    m_squad_class;
	sint16         m_agent_type;
	bool           m_can_be_executed;
	bool           m_detached;
	bool           m_neededForGarrison;
};

#endif // __AGENT_H__
