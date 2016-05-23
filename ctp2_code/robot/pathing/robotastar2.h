//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ header
// Description  : A-star AI pathing algorithm
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
// - None
//
//----------------------------------------------------------------------------

#pragma once
#ifndef __ROBOT_ASTAR2_H__
#define __ROBOT_ASTAR2_H__ 1

#include "UnitAstar.h"

class RobotAstar2 : public UnitAstar
{
public:

	enum PathType {
		PATH_TYPE_DEFAULT,
		PATH_TYPE_TRANSPORT,
		PATH_TYPE_AIRLIFT,
		PATH_TYPE_DEFENSIVE
	};

	static RobotAstar2 s_aiPathing;

	RobotAstar2();

	bool FindPath(const PathType & pathType,
				  const Army & army,
				  const uint32 & army_move_type,
		          const MapPoint & start,
				  const MapPoint & dest,
				  const bool & check_dest,
				  const sint32 & trans_dest_cont,
				  const float & trans_max_r,
				  Path & new_path,
				  float & total_cost);
private:

	bool TransportPathCallback (const bool & can_enter,
								const MapPoint & prev,
								const MapPoint & pos,
								const bool & is_zoc,
								float & cost,
								ASTAR_ENTRY_TYPE & entry);

	bool AirliftPathCallback   (const bool & can_enter,
								const MapPoint & prev,
								const MapPoint & pos,
								const bool & is_zoc,
								float & cost,
								ASTAR_ENTRY_TYPE & entry);

	bool DefensivePathCallback (const bool & can_enter,
								const MapPoint & prev,
								const MapPoint & pos,
								const bool & is_zoc,
								float & cost,
								ASTAR_ENTRY_TYPE & entry);




	bool EntryCost(const MapPoint & prev, const MapPoint & pos,
					 float &cost, bool & is_zoc, ASTAR_ENTRY_TYPE & entry);

	void RecalcEntryCost(AstarPoint *parent, AstarPoint *node,
						 float &new_entry_cost, bool &new_is_zoc,
						 ASTAR_ENTRY_TYPE &new_entry);

	PathType m_pathType;
	sint32 m_transDestCont;
	float m_transMaxR;
	uint32 m_incursionPermission;
	PLAYER_INDEX m_owner;
};

#endif __ROBOT_ASTAR2_H__
