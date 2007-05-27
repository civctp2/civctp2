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
// - None
//
//----------------------------------------------------------------------------

#include "c3.h"
#include "robotastar2.h"

#include "c3math.h"
#include "c3errors.h"
#include "globals.h"




#include "dynarr.h"
#include "Path.h"
#include "UnitAstar.h"

#include "World.h"          // g_theWorld
#include "dynarr.h"
#include "player.h"
#include "RandGen.h"
#include "UnitRec.h"
#include "civarchive.h"
#include "UnitRecord.h"
#include "ArmyData.h"
#include "Cell.h"
#include "Diplomat.h"
#include "profileDB.h"      // g_theProfileDB


uint32 const    INCURSION_PERMISSION_ALL    = 0xffffffffu;

RobotAstar2 RobotAstar2::s_aiPathing;

RobotAstar2::RobotAstar2()
{
	m_pathType = PATH_TYPE_DEFAULT;
}

BOOL RobotAstar2::TransportPathCallback (const BOOL & can_enter, 
										 const MapPoint & prev,  
									     const MapPoint & pos, 
										 const BOOL & is_zoc, 
									     float & cost, 
									     ASTAR_ENTRY_TYPE & entry )
{ 
    if (can_enter) 
 		{ 
			
			
			sint32 cont; 
			BOOL is_land; 
			BOOL wrong_cont;
    
			cont = g_theWorld->GetContinent(pos);
			is_land = ( g_theWorld->IsLand(pos) || g_theWorld->IsMountain(pos) );

			
			
			wrong_cont = ( cont != m_transDestCont ) && 
				(g_theWorld->IsCity(pos) == false);

			
			
			

			BOOL occupied = FALSE; 

			
			occupied = (m_army->HasCargo() && (!m_army.CanAtLeastOneCargoUnloadAt(prev, pos, FALSE)));

			if (occupied || wrong_cont) 
				{
					
					if (is_land && 
						(g_theWorld->IsWater(prev) ||
						 g_theWorld->IsShallowWater(prev) ||
						 
						 g_theWorld->IsCity(prev) )) 
						{ 
							cost = k_ASTAR_BIG;
							entry = ASTAR_RETRY_DIRECTION; 
							return FALSE;
						}
				}
		
			if (g_theWorld->IsWater(pos) || g_theWorld->IsShallowWater(pos)) 
				{ 
					cost *= float(m_transMaxR);
				} 

			
			
			
			
			
            
			
			
			
		
			return TRUE; 
		} 
	else 
		{ 
			cost = k_ASTAR_BIG; 
			entry = ASTAR_BLOCKED; 
			return FALSE;
		} 
}

BOOL RobotAstar2::DefensivePathCallback (const BOOL & can_enter,  
									     const MapPoint & prev,  
									     const MapPoint & pos, 
										 const BOOL & is_zoc, 
									     float & cost, 
									     ASTAR_ENTRY_TYPE & entry)
{ 
	PLAYER_INDEX pos_owner;
	PLAYER_INDEX prev_owner;

	pos_owner = g_theWorld->GetCell(pos)->GetOwner();
    if (can_enter) { 
		
		if ((pos_owner < 0) || (m_incursionPermission & (0x1 << pos_owner)))
			return TRUE;
			
		
		
		prev_owner = g_theWorld->GetCell(prev)->GetOwner();
		if ((prev_owner == pos_owner) &&
			!(m_incursionPermission & (0x1 << prev_owner)))
		{
			cost += k_MOVE_TREASPASSING_COST;
			return TRUE; 
		} 
    }

	
	cost = k_ASTAR_BIG; 
	entry = ASTAR_ENTRY_TYPE(0); 
	return FALSE;
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
							float & total_cost ) 
						   
{ 
	sint32 cutoff;
	
	
	
	
	
	
		cutoff = 20000;

	sint32 nodes_opened = 0;
	const BOOL no_straight_lines = FALSE;
	const BOOL check_units_in_cell = TRUE;
    bool is_broken_path = false;
	const BOOL pretty_path = FALSE;
	Path bad_path;

    m_pathType = pathType; 
	m_transDestCont = trans_dest_cont;
	m_transMaxR = trans_max_r;
	m_owner = g_theWorld->GetOwner(start);

    sint32 nUnits; 
    uint32 move_intersection; 
    uint32 move_union; 
    m_is_robot = TRUE; 
	
	
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
	if (isspecial && maxattack == 0 && !haszoc)
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
		m_army_can_expel_stealth = FALSE; 
	}
	else
	{
		
		UnitAstar::InitArmy (army, nUnits, move_intersection, move_union,  
			m_army_minmax_move);        
	}
	
	if(!UnitAstar::FindPath(army, nUnits, move_intersection, move_union, 
		start, army.GetOwner(), dest, new_path, 
		is_broken_path, bad_path, total_cost, TRUE,
		false, pretty_path, cutoff, nodes_opened, 
		check_dest, no_straight_lines, 
		check_units_in_cell)){
		
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
	bool r = UnitAstar::EntryCost(prev, pos, cost, is_zoc, entry); 

	if (r)
	{
		switch (m_pathType) 
		{
		case PATH_TYPE_TRANSPORT:
			r = TransportPathCallback(r, prev, pos, is_zoc, cost, entry);
			break;
		case PATH_TYPE_DEFENSIVE:
			r = DefensivePathCallback(r, prev, pos, is_zoc, cost, entry);
			break;
		}

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
					TransportPathCallback(TRUE, parent->m_pos, node->m_pos, new_is_zoc, 
										  new_entry_cost, new_entry);
					break;
				case PATH_TYPE_DEFENSIVE:
					DefensivePathCallback(TRUE, parent->m_pos, node->m_pos, new_is_zoc, 
										  new_entry_cost, new_entry);
					break;
				}
		}
}

