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
//
//----------------------------------------------------------------------------

#ifdef HAVE_PRAGMA_ONCE
#pragma once
#endif

#ifndef __CTP_AGENT_H__
#define __CTP_AGENT_H__ 1

#include <vector>

class CTPAgent;
typedef CTPAgent *  CTPAgent_ptr;

#include "agent.h"              // Agent
#include "Army.h"               // Army
#include "MapPoint.h"           // MapPoint
#include "Path.h"
#include "player.h"             // PLAYER_INDEX
#include "scheduler_types.h"    // SQUAD_CLASS, Squad_Strength, etc.
#include "GameEventTypes.h"

class CTPAgent : public Agent
{
protected:
	CTPAgent();
public:
	explicit CTPAgent(const Army & army);
	CTPAgent(CTPAgent const & an_Original);


	virtual ~CTPAgent();


	virtual CTPAgent & operator= (CTPAgent const & an_Original);


	const Army & Get_Army() const;


	PLAYER_INDEX Get_Player_Number() const { return m_playerId; };

	bool Get_Is_Dead() const;


	SQUAD_CLASS Compute_Squad_Class();


	MapPoint Get_Pos() const;

	sint32 GetUnitsAtPos() const;
	bool IsArmyPosFilled() const;
	bool IsOneArmyAtPos() const;

	bool CanMove() const;


	const Squad_Strength & Compute_Squad_Strength();




	virtual void Log_Debug_Info(const int & log, const Goal * const goal) const;

	bool FindPathToBoard( const uint32 & move_intersection, const MapPoint & dest_pos, const bool & check_dest, Path & found_path );


	static bool FindPath(const Army & army, const MapPoint & target_pos, const bool & check_dest, Path & found_path );
	static bool FindPath(const Army & army, const MapPoint & target_pos, const bool & check_dest, Path & found_path, float & total_cost);

	sint32 GetRounds(const MapPoint & pos, sint32 & cells) const;
	double GetRoundsPrecise(const MapPoint & pos, sint32 & cells) const;

	bool EstimateTransportUtility(const CTPAgent_ptr transport, Utility & utility) const;

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


	void Group_With( CTPAgent_ptr second_army );


	void Ungroup_Order();


	sint32 DisbandObsoleteUnits();


	void MoveIntoTransport();
	void PerformOrderHere(const OrderRecord * order_rec, const Path * path, GAME_EVENT_INSERT priority = GEV_INSERT_AfterCurrent);
	void PerformOrder(const OrderRecord * order_rec);
	void WaitHere(const MapPoint & goal_pos);
	void ClearOrders();
	bool HasMovePoints() const;

protected:

	Army     m_army;
	sint32   m_targetOrder;
	sint32   m_playerId;
	MapPoint m_targetPos;
};

#endif // __CTP_AGENT_H__
