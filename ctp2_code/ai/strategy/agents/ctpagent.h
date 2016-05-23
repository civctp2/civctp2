#ifdef HAVE_PRAGMA_ONCE
#pragma once
#endif
#ifndef __CTP_AGENT_H__
#define __CTP_AGENT_H__ 1

#include <vector>

#include "scheduler_types.h"
#include "agent.h"

#include "c3debugstl.h"
#include "Army.h"
#include "Path.h"

class CTPAgent;
typedef CTPAgent* CTPAgent_ptr;

class CTPAgent : public Agent {

public:












	static std::vector<sint32, dbgallocator<sint32> > s_orderDBToEventMap;
	static void AssociateEventsWithOrdersDB();












	CTPAgent();

	CTPAgent(const CTPAgent &agent);

  	virtual ~CTPAgent();


	virtual void Init();

	virtual CTPAgent & operator= (const CTPAgent & agent);

    const Army & Get_Army() const;

    void Set_Army(const Army & army);

    PLAYER_INDEX Get_Player_Number() const;

    void Set_Player_Number(const PLAYER_INDEX & playerId);

	bool Get_Is_Dead() const;

	SQUAD_CLASS Compute_Squad_Class();

	MapPoint Get_Pos() const;

	bool IsArmyPosFilled() const;

	bool CanMove() const;

    const Squad_Strength & Compute_Squad_Strength();






    virtual void Log_Debug_Info(const int & log) const;






	bool FindPathToBoard( const uint32 & move_intersection, const MapPoint & dest_pos, const bool & check_dest, Path & found_path );

	static bool FindPath(const Army & army, const MapPoint & target_pos, const bool & check_dest, Path & found_path );

	sint32 GetRounds(const MapPoint & pos, sint32 & cells) const;


	bool EstimateTransportUtility(const CTPAgent_ptr transport, double & utility) const;






	void Set_Target_Order(const sint32 order_type);
	void Set_Target_Pos(const MapPoint &order_pos);
	sint32 Get_Target_Order() const;
	const MapPoint & Get_Target_Pos() const;

	bool Follow_Path(const Path & found_path, const sint32 & order_type);

	bool Can_Execute_Order(const sint32 & order_type) const;

	void Execute_Order(const sint32 & order_type, const MapPoint & order_pos);

	void Group_Order();

	void Group_With( CTPAgent_ptr second_army );

	void Ungroup_Order();

	sint32 DisbandObsoleteUnits();

	void MoveIntoTransport();

protected:








    Army m_army;

    sint16 m_playerId;

	sint32 m_targetOrder;
	MapPoint m_targetPos;
};

#endif // __CTP_AGENT_H__
