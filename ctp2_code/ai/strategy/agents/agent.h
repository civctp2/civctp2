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
//
//----------------------------------------------------------------------------

#ifdef HAVE_PRAGMA_ONCE
#pragma once
#endif

#ifndef __AGENT_H__
#define __AGENT_H__ 1

class Agent;

#include "c3.h"

#include "scheduler_types.h"
#include "squad_Strength.h"
#include "Army.h"               // Army
#include "MapPoint.h"           // MapPoint
#include "Path.h"
#include "player.h"             // PLAYER_INDEX
#include "scheduler_types.h"    // SQUAD_CLASS, Squad_Strength, etc.
#include "GameEventTypes.h"

class Agent
{
private:
	Agent();
public:
	explicit Agent(const Army & army);
	Agent(const Agent & an_Original);
	~Agent();

	Agent& operator= (const Agent & an_Original);

	sint16 Get_Type() const;

	void Set_Type(const sint16 & type);

	SQUAD_CLASS Get_Squad_Class() const;

	void     Set_Goal(Goal_ptr goal){ m_goal = goal; };
	Goal_ptr Get_Goal()             { return m_goal; };
	bool     Has_Goal()             { return m_goal != NULL; };

	bool Get_Is_Dead() const;

	void Set_Squad_Class(const SQUAD_CLASS & squad_class);

	SQUAD_CLASS Compute_Squad_Class();

	const Squad_Strength & Get_Squad_Strength() const;

	const Squad_Strength & Compute_Squad_Strength();

	void Set_Can_Be_Executed(const bool &can_be_executed);

	bool Get_Can_Be_Executed() const;

	void Set_Detached(const bool detached);

	bool Get_Detached() const;

	void Log_Debug_Info(const int & log, const Goal * const goal) const;

	void Set_Needs_Transporter(const bool needs_transporter) { m_needs_transporter = needs_transporter; };
	bool Get_Needs_Transporter() const                       { return m_needs_transporter; };

	const Army & Get_Army() const;


	PLAYER_INDEX Get_Player_Number() const { return m_playerId; };


	MapPoint Get_Pos() const;

	sint32 GetUnitsAtPos() const;
	bool IsArmyPosFilled() const;
	bool IsOneArmyAtPos() const;

	bool CanMove() const;


	bool FindPathToBoard( const uint32 & move_intersection, const MapPoint & dest_pos, const bool & check_dest, Path & found_path );


	static bool FindPath(const Army & army, const MapPoint & target_pos, const bool & check_dest, Path & found_path );
	static bool FindPath(const Army & army, const MapPoint & target_pos, const bool & check_dest, Path & found_path, float & total_cost);

	sint32 GetRounds(const MapPoint & pos, sint32 & cells) const;
	double GetRoundsPrecise(const MapPoint & pos, sint32 & cells) const;

	bool EstimateTransportUtility(const Agent_ptr transport, Utility & utility) const;

	void Set_Target_Order(const sint32 order_type);
	void Set_Target_Pos(const MapPoint &order_pos);
	sint32 Get_Target_Order() const;
	const MapPoint & Get_Target_Pos() const;

#if 0
	void Follow_Path(const Path & found_path, const sint32 & order_type);
#endif

	bool Can_Execute_Order(const sint32 & order_type) const;


	void Execute_Order(const sint32 & order_type, const MapPoint & order_pos);


	void Group_Order();


	void Group_With( Agent_ptr second_army );


	void Ungroup_Order();


	sint32 DisbandObsoleteUnits();


	void MoveIntoTransport();
	void PerformOrderHere(const OrderRecord * order_rec, const Path * path, GAME_EVENT_INSERT priority = GEV_INSERT_AfterCurrent);
	void PerformOrder(const OrderRecord * order_rec);
	void WaitHere(const MapPoint & goal_pos);
	void ClearOrders();
	bool HasMovePoints() const;

	bool     m_neededForGarrison;

private:

	SQUAD_CLASS    m_squad_class;
	sint16         m_agent_type;
	Goal_ptr       m_goal;
	Squad_Strength m_squad_strength;
	bool           m_can_be_executed;
	bool           m_detached;
	bool           m_needs_transporter;
	Army           m_army;
	sint32         m_targetOrder;
	sint32         m_playerId;
	MapPoint       m_targetPos;
};

#endif // __AGENT_H__
