//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
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
// - For an AI transporter it is no more checked, whether the transporter cannot
//   unload its cargo, because the cargo has not enough move points.
// - A transporter's path does not not leave the target continent, once it
//   is on the target continent. (13-Jul-2009 Martin Gühmann)
//
//----------------------------------------------------------------------------

#include "c3.h"
#include "robotastar2.h"

#include "Path.h"
#include "World.h"          // g_theWorld
#include "ArmyData.h"
#include "Diplomat.h"
#include "ctpaidebug.h"

uint32 const    INCURSION_PERMISSION_ALL    = 0xffffffffu;

RobotAstar2 RobotAstar2::s_aiPathing;

RobotAstar2::RobotAstar2()
{
	m_pathType = PATH_TYPE_DEFAULT;
}

bool RobotAstar2::TransportPathCallback (const bool & can_enter,
                                         const MapPoint & prev,
                                         const MapPoint & pos,
                                         const bool & is_zoc,
                                         float & cost,
                                         ASTAR_ENTRY_TYPE & entry )
{
	if (can_enter)
	{
		sint32 cont     = g_theWorld->GetContinent(pos);
		bool is_land    = ( g_theWorld->IsLand(pos) || g_theWorld->IsMountain(pos) );
		bool wrong_cont = ( cont != m_transDestCont ) && (!g_theWorld->IsCity(pos));

		bool occupied = (m_army->HasCargo() && (!m_army.CanAtLeastOneCargoUnloadAt(pos, false, !m_is_robot)));

		if(occupied || wrong_cont)
		{
			if(  is_land
			   &&
			     (    g_theWorld->IsWater(prev)
			       || g_theWorld->IsShallowWater(prev)
			       || g_theWorld->IsCity(prev)
			     )
			  )
			{
				cost = k_ASTAR_BIG;
				entry = ASTAR_RETRY_DIRECTION;
				return false;
			}
		}

		if(g_theWorld->IsWater(pos) || g_theWorld->IsShallowWater(pos))
		{
			if
			  (
			       (
			           g_theWorld->IsLand(prev)
			        || g_theWorld->IsMountain(prev)
			       )
			    && g_theWorld->GetContinent(prev) == m_transDestCont
			  )
			{
				// Return invalid if we leave the target continent
				cost = k_ASTAR_BIG;
				entry = ASTAR_RETRY_DIRECTION;
				return false;
			}

			cost *= m_transMaxR;
		}

		if(g_theWorld->IsWater(prev) || g_theWorld->IsShallowWater(prev))
		{
			if
			  (
			       (
			           g_theWorld->IsLand(pos)
			        || g_theWorld->IsMountain(pos)
			       )
			    && (
			          ( g_theWorld->IsOccupiedByForeigner  (pos, m_owner) // If the target is a city
			        && !g_theWorld->IsSurroundedByWater(pos))
			        ||  g_theWorld->IsNextToForeignerOnLand(pos, m_owner)
			       )
			  )
			{
				cost += k_MOVE_ISDANGER_COST;
			}
		}

		return true;
	}
	else
	{
		cost = k_ASTAR_BIG;
		entry = ASTAR_BLOCKED;
		return false;
	}
}

bool RobotAstar2::AirliftPathCallback (const bool & can_enter,
                                       const MapPoint & prev,
                                       const MapPoint & pos,
                                       const bool & is_zoc,
                                       float & cost,
                                       ASTAR_ENTRY_TYPE & entry )
{
	if (can_enter)
	{
		if
		  (
		        pos == m_dest
		    &&
		   (
		        g_theWorld->GetCell(pos)->GetNumUnits() > 0
		    &&  g_theWorld->GetArmyPtr(pos)->GetOwner() != m_army->GetOwner()
		    && !m_army->CanFight(*g_theWorld->GetArmyPtr(pos))
		   )
		    ||
		   (
		        g_theWorld->HasCity(pos)
		    &&  g_theWorld->GetCity(pos).GetOwner() != m_army->GetOwner()
		    && !m_army->CanAtLeastOneCaptureCity()
		   )
		  )
		{
			bool occupied = (m_army->HasCargo() && !m_army.CanAtLeastOneCargoUnloadAt(pos, false, !m_is_robot));

			if(occupied)
			{
				if(!m_army.CanAtLeastOneCargoUnloadAt(prev, false, !m_is_robot))
				{
					cost = k_ASTAR_BIG;
					entry = ASTAR_RETRY_DIRECTION;
					return false;
				}
			}
		}

		return true;
	}
	else
	{
		cost = k_ASTAR_BIG;
		entry = ASTAR_BLOCKED;
		return false;
	}
}

bool RobotAstar2::DefensivePathCallback (const bool & can_enter,
                                         const MapPoint & prev,
                                         const MapPoint & pos,
                                         const bool & is_zoc,
                                         float & cost,
                                         ASTAR_ENTRY_TYPE & entry)
{
	PLAYER_INDEX pos_owner;
	PLAYER_INDEX prev_owner;

	pos_owner = g_theWorld->GetCell(pos)->GetOwner();
	if (can_enter)
	{
		if ((pos_owner < 0) || (m_incursionPermission & (0x1 << pos_owner)))
			return true;

		prev_owner = g_theWorld->GetCell(prev)->GetOwner();
		if ((prev_owner == pos_owner) &&
			!(m_incursionPermission & (0x1 << prev_owner)))
		{
			cost += k_MOVE_TREASPASSING_COST;
			return true;
		}
	}

	cost = k_ASTAR_BIG;
	entry = ASTAR_ENTRY_TYPE(0);
	return false;
}

bool RobotAstar2::FindPath( const PathType & pathType,
                            const Army & army,
                            const uint32 & army_move_type,
                            const MapPoint & start,
                            const MapPoint & dest,
                            const bool & check_dest,
                            const sint32 & trans_dest_cont,
                            const float & trans_max_r,
                            Path & new_path,
                            float & total_cost,
                            sint32 additionalUnits)
{
	sint32 cutoff = 20000;

	sint32 nodes_opened = 0;
	const bool no_straight_lines = false;
	bool is_broken_path = false;
	Path bad_path;

	m_pathType = pathType;
	m_transDestCont = trans_dest_cont;
	m_transMaxR = trans_max_r;
	m_owner = g_theWorld->GetOwner(start);

	sint32 nUnits;
	uint32 move_intersection;
	uint32 move_union;
	m_is_robot = true;

	bool isspecial, cancapture, haszoc, canbombard;
	bool isstealth;
	sint32 maxattack, maxdefense;
	army->CharacterizeArmy( isspecial,
	    isstealth,
	    maxattack,
	    maxdefense,
	    cancapture,
	    haszoc,
	    canbombard);

	if(isspecial && maxattack == 0 && !haszoc)
	{
		m_incursionPermission = INCURSION_PERMISSION_ALL;
	}
	else
	{
		m_incursionPermission =
			Diplomat::GetDiplomat(army.GetOwner()).GetIncursionPermission();
	}

	if (army_move_type != 0x0)
	{
		nUnits = 1;
		move_intersection = army_move_type;
		move_union = 0;
		m_army_minmax_move = 300.0;
		m_army_can_expel_stealth = false;
	}
	else
	{
		UnitAstar::InitArmy (army, nUnits, move_intersection, move_union,
			m_army_minmax_move);
	}

	AI_DPRINTF(k_DBG_ASTAR, -1, -1, -1,("\n"));
	if(!UnitAstar::FindPath(army,
	                        nUnits + additionalUnits,
	                        move_intersection,
	                        move_union,
	                        start,
	                        army.GetOwner(),
	                        dest, new_path,
	                        is_broken_path,
	                        bad_path,
	                        total_cost,
	                        true,
	                        cutoff,
	                        nodes_opened,
	                        check_dest,
	                        no_straight_lines)
	){
		return false;
	}

	return !is_broken_path;
}

bool RobotAstar2::EntryCost( const MapPoint &prev,
                               const MapPoint &pos,
                               float & cost,
                               bool &is_zoc,
                               ASTAR_ENTRY_TYPE &entry )
{
	if(m_pathType == PATH_TYPE_TRANSPORT || m_pathType == PATH_TYPE_AIRLIFT)
	{
		m_isTransporter = true;
	}
	else
	{
		m_isTransporter = false;
	}

	bool r = UnitAstar::EntryCost(prev, pos, cost, is_zoc, entry);

	if (r)
	{
		switch (m_pathType)
		{
		case PATH_TYPE_TRANSPORT:
			r = TransportPathCallback(r, prev, pos, is_zoc, cost, entry);
			break;
		case PATH_TYPE_AIRLIFT:
			r = AirliftPathCallback  (r, prev, pos, is_zoc, cost, entry);
			break;
		case PATH_TYPE_DEFENSIVE:
			r = DefensivePathCallback(r, prev, pos, is_zoc, cost, entry);
			break;
		}

		if(m_is_robot && pos != m_army->RetPos() && pos != m_dest && cost < k_ASTAR_BIG)
		{
			if(CheckIsDangerForPos(pos))
			{
				cost      += k_MOVE_ISDANGER_COST;
			}
		}

		DPRINTF(k_DBG_ASTAR,("\tCheckEnter, StartPos (%d, %d), DestPos (%d, %d), ThisPos (%d, %d), NextPos (%d, %d), IsZoc: %d, EntryType: %d, Cost: %f\n", m_start.x, m_start.y, m_dest.x, m_dest.y, prev.x, prev.y, pos.x, pos.y, is_zoc, entry, cost));

		if (cost < 1.0)
		{
			cost = 1.0;
		}
		else if ((k_ASTAR_BIG <= cost) && (entry != ASTAR_RETRY_DIRECTION))
		{
			return false;
		}
	}

	return r;
}

void RobotAstar2::RecalcEntryCost(AstarPoint *parent,
    AstarPoint *node, float &new_entry_cost,
    bool &new_is_zoc, ASTAR_ENTRY_TYPE &new_entry)
{
	new_entry = ASTAR_CAN_ENTER;
	UnitAstar::RecalcEntryCost(parent,
							   node,
							   new_entry_cost,
							   new_is_zoc,
							   new_entry);

	if ((new_entry_cost < k_ASTAR_BIG) && (new_entry == ASTAR_CAN_ENTER))
	{
		switch (m_pathType)
		{
			case PATH_TYPE_TRANSPORT:
				TransportPathCallback(true, parent->m_pos, node->m_pos, new_is_zoc,
									  new_entry_cost, new_entry);
				break;
			case PATH_TYPE_AIRLIFT:
				AirliftPathCallback  (true, parent->m_pos, node->m_pos, new_is_zoc,
									  new_entry_cost, new_entry);
				break;
			case PATH_TYPE_DEFENSIVE:
				DefensivePathCallback(true, parent->m_pos, node->m_pos, new_is_zoc,
									  new_entry_cost, new_entry);
				break;
		}
	}
}
