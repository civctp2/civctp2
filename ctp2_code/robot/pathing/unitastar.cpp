//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : A* algorithm for units
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
// ACTIVISION_ORIGINAL		
// - When defined, generates the original Activision code.
// - When not defined, generates the modified Apolyton code.
//
//----------------------------------------------------------------------------
//
// Modifications from the original Activision code:
//
// - Corrected movement rate of ships above tunnels.
// - m_queue_index used.
// - Straight line corrected for worlds that do not have X-wrapping.
// - Standardised min/max usage.
// - Added method to check if there is a danger along the path (for civilian 
//   units). so units don't go near enemy cities (and can't be bombarded).
//   If no alternate path found, go on the first founded path. The method 
//   considers a danger if the owner is less than neutral - Calvitix
// - Disabled Calvitix check for danger. If an army encounter something on
//   its way the goal should be reconsidered. - Feb. 21st 2005 Martin Gühmann
//
//----------------------------------------------------------------------------

#include "c3.h"
#include "c3math.h"
#include "c3errors.h"
#include "Globals.h"

#include "DynArr.h"
#include "Path.h"
#include "UnitAstar.h"

#if !defined (ACTIVISION_ORIGINAL)
// Added by Calvitix
// Had to include those files to determine if the army encounters a possible 
// danger on its way. Of course the is only danger if an ennemy army is 
// encountered and not an ally.
#include "Diplomat.h"
#include "AgreementMatrix.h"
#include "ArmyData.h"
#endif //ACTIVISION_ORIGINAL


#include "Army.h"
#include "Unit.h"
#include "CellUnitList.h"
#include "XY_Coordinates.h"
#include "World.h"
extern World *g_theWorld; 
#include "Cell.h"

#include "DynArr.h"
#include "Player.h"
#include "RandGen.h"
#include "UnitRecord.h"
#include "ConstDB.h"
#include "WonderRecord.h"
#include "TerrainRecord.h"
#include "UnitData.h"


#include "UnseenCell.h"
#include "MoveFlags.h"
#include "wonderutil.h"

#include "A_Star_Heuristic_Cost.h"
#include "terrainutil.h"


DynamicArray<MapPoint> g_pixel; 

UnitAstar::UnitAstar() 
{ 
    ClearMem(); 
} 

#define GROUND_Z 0
#define SPACE_Z 1






#if defined(ACTIVISION_ORIGINAL)	// Unused stuff

extern MapPoint g_mp_size;

//----------------------------------------------------------------------------
//
// Name       : AddToPath
//
// Description: Add a step between 2 adjacent points to the path.
//
// Parameters : the_path		: the path to add the step to
//				oldpx, oldpy	: coordinates of the last point in the path
//				px, py			: coordinates of the new point in the path
//
// Globals    : -
//
// Returns    : the_path		: updated with the step
//
// Remark(s)  : All coordinates are in a "diagonal" coordinate system - i.e.
//              an orthogonal coordinate system with X and Y axis 45 degrees 
//              rotated from the usual horizontal and vertical axis.
//              These are converted to a map direction before adding to the 
//              path.
//
//              This function is not needed any more, 
//				because OldNormalizedSubtract is not used any more.
//
//----------------------------------------------------------------------------

void AddToPath(Path &the_path, sint32 &oldpx, sint32 &oldpy, 
               const sint32 px, const sint32 py) 
{ 
    sint32 dx = px - oldpx;
    sint32 dy = py - oldpy; 

    switch (dy) { 
    case 1:
        switch(dx) { 
        case 1: the_path.AddDir(NORTH); break; 
        case 0: the_path.AddDir(NORTHEAST); break;
        case -1: the_path.AddDir(EAST); break;
        default:
            Assert(0); 
        } 
        break; 
    case 0:
        switch(dx) { 
        case 1: the_path.AddDir(NORTHWEST); break; 
        case 0: Assert(0); break;
        case -1: the_path.AddDir(SOUTHEAST); break;
        default:
            Assert(0); 
        } 
        break; 
    case -1:
        switch(dx) { 
        case 1: the_path.AddDir(WEST); break; 
        case 0: the_path.AddDir(SOUTHWEST); break;
        case -1: the_path.AddDir(SOUTH); break;
        default:
            Assert(0); 
        } 
        break; 
    default:
        Assert(0); 
    }

    oldpx = px; 
    oldpy = py; 
} 

#endif // ACTIVISION_ORIGINAL

//----------------------------------------------------------------------------
//
// Name       : UnitAstar::StraightLine
//
// Description: Compute a straight path from start to destination.
//
// Parameters : start			: start
//				dest			: destination
//
// Globals    : -
//
// Returns    : sint32 (bool)	: a_path has meaning (i.e. we are not at the
//                                destination already).
//				a_path			: the computed straight line path
//
// Remark(s)  : Does not handle vertical (UP/DOWN) movements.
//
//----------------------------------------------------------------------------

#if defined(ACTIVISION_ORIGINAL)	// OldNormalizedSubtract requires X-wrap.

sint32 UnitAstar::StraightLine(const MapPoint &start, const MapPoint &dest, 
                               Path &a_path) const
                               
{
    
    if (start == dest) {
        a_path.Clear(); 
        return FALSE; 
    }
    
    sint32 dx,dy,sdx,sdy,absdx,absdy,num,den,px,py;
    
    
    px = 0;  
    py = 0;
    
    static MapPoint diff; 
    static MapPoint diff2;

    start.OldNormalizedSubtract(dest, diff); 





















  
    
    dx = diff.x; 
    dy = diff.y; 
        
    sdx = SGN(dx);
    sdy = SGN(dy);
    absdx = ABS(dx);
    absdy = ABS(dy) ;
    num = (absdx >> 1) + 1;
    den = (absdy >> 1) + 1;
    
    a_path.SetStart(start);

    sint32 oldpx, oldpy;
    oldpx = px; 
    oldpy = py; 

    if (absdx >= absdy) {	
        for (sint32 i=0;i < absdx; i++) {
            num += absdy;
            if (num > absdx){ 
                num -= absdx;
                py += sdy;
            }
            px += sdx;
            
            AddToPath(a_path, oldpx, oldpy, px, py);
        }
    }else {			
        for (int i=0;i < absdy;i++)	{
            den += absdx;
            if (den > absdy){   
                den -= absdy;
                px += sdx;
            }
            py += sdy;
            
            AddToPath(a_path, oldpx, oldpy, px, py);
        }
        
    }
    

    return TRUE;
}

#else	// ACTIVISION_ORIGINAL

sint32 UnitAstar::StraightLine
(
	const MapPoint &		start, 
	const MapPoint &		dest, 
    Path &					a_path
) const
{
    if (start == dest) 
	{
		a_path.Clear(); 
        return FALSE; 
    }
    
    a_path.SetStart(start);

	// Shortest distance vector from start to dest, using XY coordinates, and
	// taking world wrap properties into account.
	MapPoint				diff;	
	start.NormalizedSubtract(dest, diff);
	
	WORLD_DIRECTION	const	dirX		= (diff.x > 0) ? EAST  : WEST;
	WORLD_DIRECTION const	dirY		= (diff.y > 0) ? SOUTH : NORTH;
	WORLD_DIRECTION	const	dirDiagonal	= 
		(EAST == dirX) ? ((SOUTH == dirY) ? SOUTHEAST : NORTHEAST)
					   : ((SOUTH == dirY) ? SOUTHWEST : NORTHWEST);

	sint32					absdx		= ABS(diff.x);
	sint32					absdy		= ABS(diff.y);

	// Start with diagonal moves to make the path straight.
	for ( ;	(absdx > 0) && (absdy > 0); --absdx, --absdy)
	{
		a_path.AddDir(dirDiagonal);
	}

	// Pure X leftover - if any.
	for ( ; absdx > 0 ; absdx -=2)
	{
		a_path.AddDir(dirX);
	}

	// Pure Y leftover - if any.
	for ( ; absdy > 0 ; absdy -=2)
	{
		a_path.AddDir(dirY);
	}

    return TRUE;
}

#endif	// ACTIVISION_ORIGINAL





































































float UnitAstar::ComputeValidMovCost(const MapPoint &pos, Cell *the_pos_cell)
{
#if defined(ACTIVISION_ORIGINAL)
	static const float move_cost_without_tunnel = 
		(float) g_theTerrainDB->Access(TERRAIN_WATER_DEEP)->GetEnvBase()->GetMovement();

	bool is_tunnel_and_boat = g_theWorld->IsTunnel(pos) &&

		((m_move_intersection & k_Unit_MovementType_Sea_Bit) ||
		 (m_move_intersection & k_Unit_MovementType_ShallowWater_Bit));
		 
	
	
	
	if (is_tunnel_and_boat)
		return float(min(m_army_minmax_move, move_cost_without_tunnel));
	else
		return float(min(m_army_minmax_move, the_pos_cell->GetMoveCost()));   
#else
	bool const	is_tunnel_and_boat	= 
		g_theWorld->IsTunnel(pos) &&
		((m_move_intersection & k_Unit_MovementType_Sea_Bit) ||
		 (m_move_intersection & k_Unit_MovementType_ShallowWater_Bit)
		);
		 
	if (is_tunnel_and_boat)
	{
		sint32 icost_without_tunnel;
		(void) g_theTerrainDB->Access(TERRAIN_WATER_DEEP)->GetEnvBase()->
					GetMovement(icost_without_tunnel);
		return std::min(m_army_minmax_move, 
						static_cast<float>(icost_without_tunnel)
					   );
	}
	else
	{
		return std::min(m_army_minmax_move, 
						static_cast<float>(the_pos_cell->GetMoveCost())
					   );
	}
#endif
}

BOOL UnitAstar::CanMoveIntoTransports(const MapPoint &pos) 
{
	
	static CellUnitList tmp_transports;
    if (m_army.m_id == (0)) { 
        return FALSE;
    } else { 
        return m_army.CanMoveIntoTransport(m_dest, tmp_transports);
    }
}

BOOL UnitAstar::CanSpaceLaunch() 
    
{
	return FALSE;
}

BOOL UnitAstar::CanSpaceLand()     
{
	return FALSE;
}


BOOL UnitAstar::CheckUnexplored(const MapPoint &prev, const MapPoint &pos,   
     Cell *the_prev_cell, Cell *the_pos_cell,                                 
    float &cost, BOOL &is_zoc, ASTAR_ENTRY_TYPE &entry, BOOL &can_enter)
{
     if (m_no_bad_path) 
        return FALSE; 




     if (!m_is_robot) { 
        if ((g_player[m_owner]->IsExplored(prev) == FALSE) &&
            (g_player[m_owner]->IsExplored(pos) == FALSE)) { 
            cost = 100; 
            can_enter = TRUE;
            return TRUE; 
        }
     }









    return FALSE;
}


BOOL UnitAstar::CheckUnits(const MapPoint &prev, const MapPoint &pos,                           
    Cell* the_prev_cell, Cell* the_pos_cell, 
    float &cost, BOOL &is_zoc, BOOL &can_be_zoc, ASTAR_ENTRY_TYPE &entry, 
	BOOL &can_enter)
{     
    sint32 dest_owner; 

    if (the_pos_cell->GetCity().m_id != 0) { 
        if (the_pos_cell->GetCity().GetOwner() != m_owner) {
            return FALSE; 
        } 
    } 

    CellUnitList* dest_army = the_pos_cell->UnitArmy(); 

#if defined (ACTIVISION_ORIGINAL)
// Removed by Calvitix
    if (!dest_army)
        return FALSE; 
#endif

    if (0 < dest_army->Num()) {  
		if(m_is_robot || dest_army->IsVisible(m_owner)) {
			dest_owner = dest_army->GetOwner(); 
			if (dest_owner != m_owner) { 

				if (pos != m_dest) { 

                    if (m_is_robot && m_army_can_expel_stealth && dest_army->CanBeExpelled()) { 
                        return FALSE; 
                    } else { 
					    cost = k_ASTAR_BIG; 
					    can_enter = FALSE;
					    entry = ASTAR_BLOCKED;
					    
                        return TRUE; 
                    }
                } else  if (m_is_zero_attack) { 
					cost = k_ASTAR_BIG; 
					can_enter = FALSE;
					entry = ASTAR_BLOCKED;
					
                    return TRUE; 
                } else { 
					can_be_zoc = FALSE; 
				} 
			} else { 
				can_be_zoc = FALSE;
				
				if (pos == m_dest) { 
    				if (CanMoveIntoTransports(pos)) { 
	    				cost = k_MOVE_ENTER_TRANSPORT_COST; 
                        entry = ASTAR_CAN_ENTER; 
		    			can_enter = TRUE;
                        return TRUE; 
			    	}
				} 

				if ((m_check_dest || (!m_check_dest && (pos != m_dest)))&& (k_MAX_ARMY_SIZE - dest_army->Num()) < m_nUnits) {   
					cost = k_ASTAR_BIG; 
					can_enter = FALSE;
					entry = ASTAR_BLOCKED; 
                    return TRUE; 
				}
			}
        }
    }

#if 0 && !defined (ACTIVISION_ORIGINAL)
// Removed by Martin Gühmann should be reconsidered
// Maybe a special avoid danger astar
// Added by Calvitix
	MapPoint start;
	m_army->GetPos(start);
	if (pos != start && pos != m_dest)
	{
		if (CheckIsDangerForPos(pos,m_army->IsCivilian()))
		{
			if (cost < 1) cost = 1;
			cost *= k_MOVE_ISDANGER_COST; 
			can_enter = TRUE;
			entry = ASTAR_CAN_ENTER; 
			return TRUE; 
		}
	}
#endif
	
    return FALSE; 
}




BOOL UnitAstar::CheckHisCity(const MapPoint &prev, const MapPoint &pos,                           
    Cell *the_prev_cell, Cell *the_pos_cell, CityData *the_pos_city, 
    float &cost, BOOL &is_zoc, ASTAR_ENTRY_TYPE &entry, BOOL &can_enter)
{
    if (the_pos_city) { 
        if (the_pos_city->GetOwner() != m_owner) { 
            if (m_is_robot || the_pos_cell->GetCity().GetVisibility() & (0x01 << m_owner)) { 
                if (pos == m_dest) { 
					if ((m_move_intersection & k_Unit_MovementType_Air_Bit) ||
						(m_move_intersection & k_Unit_MovementType_Sea_Bit) ||
						(m_move_intersection & k_Unit_MovementType_ShallowWater_Bit)) {  
						cost = ComputeValidMovCost(pos, the_pos_cell);
						can_enter = TRUE; 
						return TRUE; 
					} else if (m_is_zero_attack) { 
						cost = k_ASTAR_BIG; 
						can_enter = FALSE;
						entry = ASTAR_BLOCKED; 
						return TRUE; 
					} else { 
						cost = ComputeValidMovCost(pos, the_pos_cell);
						can_enter = TRUE; 
						return TRUE; 
                    } 
                } else { 
                    cost = k_ASTAR_BIG; 
					can_enter = FALSE;
					entry = ASTAR_BLOCKED; 
                    return TRUE; 
                }
            }
        }
    }
    return FALSE; 
}


BOOL UnitAstar::CheckHeight(const MapPoint &prev, const MapPoint &pos, Cell *the_prev_cell, 
    Cell *the_pos_cell, float &cost, BOOL &is_zoc, ASTAR_ENTRY_TYPE &entry, BOOL &can_enter)
{


    return FALSE; 
}


BOOL UnitAstar::CheckMyCity(const MapPoint &prev, const MapPoint &pos,                           
    Cell *the_prev_cell, Cell *the_pos_cell, CityData *the_pos_city, 
    float &cost, BOOL &is_zoc, ASTAR_ENTRY_TYPE &entry, BOOL &can_enter)
{
    if (!the_pos_city) 
        return FALSE; 

    if (the_pos_city->GetOwner() == m_owner) {
		cost = ComputeValidMovCost(pos, the_pos_cell);
		can_enter = TRUE; 
        return TRUE; 
	}
    return FALSE; 
}





BOOL UnitAstar::CheckMoveUnion(const MapPoint &prev, const MapPoint &pos, Cell *the_prev_cell, 
    Cell *the_pos_cell, float &cost, BOOL &is_zoc, const BOOL can_be_zoc,  
    ASTAR_ENTRY_TYPE &entry, BOOL &can_enter)
{

    if (m_army.m_id != 0) { 
        if (m_army.CanEnter(pos)) { 
              cost = ComputeValidMovCost(pos, the_pos_cell);
              can_enter = TRUE; 
              return TRUE;               
        } else { 
              cost = k_ASTAR_BIG; 
              can_enter = FALSE; 
			  entry = ASTAR_BLOCKED; 
              return TRUE;
        }
    } else { 
        if ((m_move_union & k_Unit_MovementType_Land_Bit) &&
            (m_move_union & k_Unit_MovementType_Mountain_Bit)){ 

            if ((!the_pos_cell->CanEnter(k_Unit_MovementType_Land_Bit)) &&
                (!the_pos_cell->CanEnter(k_Unit_MovementType_Mountain_Bit))) { 
                cost = k_ASTAR_BIG; 
                can_enter = FALSE; 
			    entry = ASTAR_BLOCKED; 
                return TRUE;
            }
        } else if (m_move_union & k_Unit_MovementType_Land_Bit) { 

            if (!the_pos_cell->CanEnter(k_Unit_MovementType_Land_Bit)) { 
                cost = k_ASTAR_BIG; 
                can_enter = FALSE; 
			    entry = ASTAR_BLOCKED; 
                return TRUE;
            }
        } else if (m_move_union & k_Unit_MovementType_Mountain_Bit) { 

            if (!the_pos_cell->CanEnter(k_Unit_MovementType_Mountain_Bit)) { 
                cost = k_ASTAR_BIG; 
                can_enter = FALSE; 
			    entry = ASTAR_BLOCKED; 
                return TRUE;
            }
        }

    
        if ((m_move_union & k_Unit_MovementType_Sea_Bit) ||
            (m_move_union & k_Unit_MovementType_ShallowWater_Bit)) { 
            if ((!the_pos_cell->CanEnter(k_Unit_MovementType_Sea_Bit)) &&
                (!the_pos_cell->CanEnter(k_Unit_MovementType_ShallowWater_Bit))) { 
                cost = k_ASTAR_BIG; 
                can_enter = FALSE;
			    entry = ASTAR_BLOCKED; 
                return TRUE;
            }
        } else if (m_move_union & k_Unit_MovementType_Sea_Bit) { 
            if (!the_pos_cell->CanEnter(k_Unit_MovementType_Sea_Bit)) { 
                cost = k_ASTAR_BIG; 
                can_enter = FALSE;
			    entry = ASTAR_BLOCKED; 
                return TRUE;
            }
        } else if (m_move_union & k_Unit_MovementType_ShallowWater_Bit) { 
            if (!the_pos_cell->CanEnter(k_Unit_MovementType_ShallowWater_Bit)) { 
                cost = k_ASTAR_BIG; 
                can_enter = FALSE;
			    entry = ASTAR_BLOCKED; 
                return TRUE;
            }
        }

    
        if (m_move_union & k_Unit_MovementType_Space_Bit) { 
            if (!the_pos_cell->CanEnter(k_Unit_MovementType_Space_Bit)) { 
                cost = k_ASTAR_BIG; 
                can_enter = FALSE; 
			    entry = ASTAR_BLOCKED; 
                return TRUE; 
            }
        }
    
    
        if (m_move_union & k_Unit_MovementType_Trade_Bit) { 
            if (!the_pos_cell->CanEnter(k_Unit_MovementType_Trade_Bit)) { 
                cost = k_ASTAR_BIG; 
                can_enter = FALSE; 
			    entry = ASTAR_BLOCKED; 
                return TRUE; 
            }
        }
    
    
		
        if (can_be_zoc && g_theWorld->IsMoveZOC (m_owner, prev, pos, TRUE) &&
			!IsBeachLanding(prev,pos,m_move_intersection)) {
            is_zoc = TRUE; 
            cost = k_ASTAR_BIG; 
            can_enter = FALSE; 
		    entry = ASTAR_RETRY_DIRECTION; 
            return TRUE; 
        } else { 
            is_zoc = FALSE; 
            cost = ComputeValidMovCost(pos, the_pos_cell);
            can_enter = TRUE;
            return TRUE; 
        }
    }

    return FALSE; 
}


BOOL UnitAstar::CheckMoveIntersection(const MapPoint &prev, const MapPoint &pos,                           
    Cell *the_prev_cell, Cell *the_pos_cell, float &cost, BOOL &is_zoc, const BOOL can_be_zoc, 
    ASTAR_ENTRY_TYPE &entry, BOOL &can_enter)
{
	if (m_move_intersection & k_Unit_MovementType_Air_Bit) {
		cost = k_MOVE_AIR_COST; 
		can_enter = TRUE;
		return TRUE; 
	} else if (the_pos_cell->CanEnter(m_move_intersection)) { 
		
		if (can_be_zoc && g_theWorld->IsMoveZOC (m_owner, prev, pos, TRUE) &&
			!IsBeachLanding(prev,pos,m_move_intersection)) {  
			is_zoc = TRUE; 
			cost = k_ASTAR_BIG; 
			can_enter = FALSE; 
			entry = ASTAR_RETRY_DIRECTION; 
			return TRUE; 
		} else { 
			is_zoc = FALSE; 
			cost = ComputeValidMovCost(pos, the_pos_cell);
			can_enter = TRUE; 
			return TRUE; 
		}
	} else {
		cost = k_ASTAR_BIG; 
		can_enter = FALSE;
		entry = ASTAR_BLOCKED; 
		return TRUE;
	}
    return FALSE; 
}

extern sint32 g_ec_called; 
extern sint32 g_find_astar_bug;

sint32 g_check; 
sint32 UnitAstar::EntryCost(const MapPoint &prev, const MapPoint &pos,                           
    float &cost, BOOL &is_zoc, ASTAR_ENTRY_TYPE &entry) 
                            
{        







entry = ASTAR_CAN_ENTER; 

    BOOL can_be_zoc = TRUE; 
	if(m_ignore_zoc)
		can_be_zoc = FALSE;
	
	
	if(!g_theWorld->IsXwrap()) {
		sint16 w = (sint16)g_theWorld->GetXWidth();

		sint32 adjX1 = (prev.x + (prev.y / 2)) % w;
		sint32 adjX2 = (pos.x  + (pos.y  / 2)) % w;

		if((adjX1 == 0 && adjX2 == (w - 1)) ||
		   (adjX2 == 0 && adjX1 == (w - 1))) {
			cost = k_ASTAR_BIG;
			entry = ASTAR_BLOCKED;
			return FALSE;
		}
	}
	
	if (!m_check_dest  &&
		pos == m_dest)
	{
        cost = 10; 
		entry = ASTAR_CAN_ENTER; 
		return TRUE;
	}

    is_zoc = FALSE; 
    BOOL can_enter = FALSE; 
    Cell *the_prev_cell = g_theWorld->AccessCell(prev);
    Assert(the_prev_cell);
    Cell *the_pos_cell = g_theWorld->AccessCell(pos);
    Assert(the_pos_cell); 

	uint32 origEnv = the_pos_cell->GetEnv();
	sint16 origMove = sint16(the_pos_cell->GetMoveCost());

	UnseenCellCarton ucell;
	bool restore = false;
	if(!m_is_robot) {
		if(g_player[m_owner]->GetLastSeen(pos, ucell)) {
			the_pos_cell->SetEnvFast(ucell.m_unseenCell->m_env);
			the_pos_cell->SetMoveCost(double(ucell.m_unseenCell->m_move_cost));
			restore = true;
		}
	}

#define RESTORE if(restore) {the_pos_cell->SetEnvFast(origEnv); the_pos_cell->SetMoveCost(double(origMove)); }

    
    if (CheckUnexplored(prev, pos, the_prev_cell, the_pos_cell, cost, is_zoc, entry, can_enter)) return can_enter; 
    
    
    if (m_check_units_in_cell) { 
        if (CheckUnits(prev, pos, the_prev_cell, the_pos_cell, cost, is_zoc, can_be_zoc, entry, 
            can_enter)) { 
            RESTORE; 
            return can_enter; 
        }
    }

    
    CityData *the_pos_city=NULL;  
    if (the_pos_cell->GetCity().m_id !=  0) { 
        the_pos_city = the_pos_cell->GetCity().GetData()->GetCityData();
    }
    if (CheckHisCity(prev, pos, the_prev_cell, the_pos_cell, the_pos_city, 
					 cost, is_zoc, entry, can_enter)) {RESTORE; return can_enter; }

    
    if (CheckHeight(prev, pos, the_prev_cell, the_pos_cell, 
					cost, is_zoc, entry, can_enter)) {RESTORE; return can_enter; }

	
    if (CheckMyCity(prev, pos, the_prev_cell, the_pos_cell, the_pos_city, 
					cost, is_zoc, entry, can_enter)) {RESTORE; return can_enter; }

    if (m_move_union != 0) {
        
        
        
        
        if (CheckMoveUnion(prev, pos, the_prev_cell, the_pos_cell, cost, is_zoc, can_be_zoc, entry,
						   can_enter)) {RESTORE; return can_enter; }

    } else { 

        
        if (CheckMoveIntersection(prev, pos, the_prev_cell, the_pos_cell, cost, is_zoc, can_be_zoc, entry,
								  can_enter)) {RESTORE; return can_enter; }

    }

	RESTORE;

    
    cost = k_ASTAR_BIG;
	entry = ASTAR_BLOCKED; 
    return FALSE; 
}



sint32 g_test_it;
sint32 xxx;



sint32 UnitAstar::InitPoint(AstarPoint *parent, AstarPoint *point, 
                      const MapPoint &pos, 
                      const float pc, const MapPoint &dest) 

{ 

    BOOL is_ortho = FALSE; 
    AstarPoint *d = point; 
    BOOL is_zoc=FALSE; 
    ASTAR_ENTRY_TYPE entry=ASTAR_CAN_ENTER; 

    d->m_flags = 0; 
	d->SetEntry(ASTAR_CAN_ENTER);
    d->SetZoc(FALSE); 
    d->SetExpanded(FALSE); 
    d->m_pos = pos; 
    d->m_parent = parent;
#if !defined(ACTIVISION_ORIGINAL) || defined(_DEBUG)
    d->m_queue_idx = -1; 
#endif
    
	d->m_past_cost = pc;
    if (parent == NULL) { 
       d->m_entry_cost = 0.0;
       d->m_future_cost = EstimateFutureCost(d->m_pos, dest);
       d->m_total_cost = d->m_past_cost + d->m_entry_cost 
            + d->m_future_cost;
#ifdef PRINT_COSTS
			g_theWorld->SetColor(pos,  d->m_total_cost); 
#endif 
       return TRUE; 

    } else if (EntryCost(parent->m_pos, d->m_pos, d->m_entry_cost, is_zoc, entry)){

        Assert(entry != ASTAR_RETRY_DIRECTION);

#ifdef _DEBUG
if (entry == ASTAR_RETRY_DIRECTION) { 
    is_zoc=FALSE; 
    entry=ASTAR_CAN_ENTER; 

    d->m_flags = 0; 
    d->SetEntry(ASTAR_CAN_ENTER);
    d->SetZoc(FALSE); 
    d->SetExpanded(FALSE);  
    d->m_pos = pos; 
    d->m_parent = parent; 
    d->m_queue_idx = -1; 

    d->m_past_cost = pc;
    d->m_entry_cost = 0.0;
    d->m_future_cost = EstimateFutureCost(d->m_pos, dest);
    d->m_total_cost = d->m_past_cost + d->m_entry_cost 
            + d->m_future_cost;
    EntryCost(parent->m_pos, d->m_pos, d->m_entry_cost, is_zoc, entry);

}
#endif

        d->SetZoc(is_zoc); 
        d->SetEntry(entry); 

        if (m_pretty_path)
            Astar::DecayOrtho(parent, point, d->m_entry_cost);

        d->m_future_cost = EstimateFutureCost(d->m_pos, dest);
        d->m_total_cost = d->m_past_cost + d->m_entry_cost 
            + d->m_future_cost;

#ifdef PRINT_COSTS
			g_theWorld->SetColor(pos,  d->m_total_cost); 
#endif 






        return TRUE; 
    } else {  
            



        if (entry == ASTAR_RETRY_DIRECTION) { 
            d->SetEntry(ASTAR_RETRY_DIRECTION);
        } else { 
    		d->SetEntry(ASTAR_BLOCKED);
        }

        d->m_future_cost = k_ASTAR_BIG;
 
        d->m_total_cost = d->m_past_cost + d->m_entry_cost 
            + d->m_future_cost;
#ifdef PRINT_COSTS
			g_theWorld->SetColor(pos,  d->m_total_cost); 
#endif 
        return FALSE;
    }
}

float UnitAstar::EstimateFutureCost(const MapPoint &pos, const MapPoint &dest)
{

	if (m_move_intersection & k_Unit_MovementType_Air_Bit) { 
															
		float air_dist = 90.0f * pos.NormalizedDistance(dest); 

#ifdef SUPER_DEBUG_HEURISTIC
			g_theWorld->SetColor(pos, sint32(air_dist)); 
#endif

		return air_dist;

	} else { 
		return Astar::EstimateFutureCost(pos, dest); 
	}
}
  



void UnitAstar::RecalcEntryCost(AstarPoint *parent, AstarPoint *node, float &new_entry_cost, 
    BOOL &new_is_zoc, ASTAR_ENTRY_TYPE &new_entry)
{

    
    

   
    BOOL can_be_zoc = TRUE; 
    BOOL can_enter; 
    Cell *the_prev_cell = g_theWorld->AccessCell(parent->m_pos); 
    Cell *the_pos_cell = g_theWorld->AccessCell(node->m_pos);
    if (CheckMoveIntersection(parent->m_pos, node->m_pos, 
        the_prev_cell, the_pos_cell, new_entry_cost, new_is_zoc, 
          can_be_zoc, new_entry, can_enter)) return; 

    if (m_pretty_path)
        Astar::DecayOrtho(parent, node, new_entry_cost);
}


void UnitAstar::InitArmy(const Army &army, sint32 &nUnits, 
    uint32 &move_intersection,  uint32 &move_union, float &army_minmax_move)
{
    sint32 i; 

    move_intersection = 0xffffffff;     
    nUnits = army.Num(); 

    
    m_can_space_launch = TRUE; 
    m_can_space_land = TRUE; 
    m_is_zero_attack = TRUE; 
    m_army_can_expel_stealth = FALSE; 
    const UnitRecord *rec=NULL; 
    for (i=0; i<nUnits; i++) {         
        move_intersection &= army[i].GetMovementType();

        rec = g_theUnitDB->Get(army[i].GetType());
        if (!rec->GetSpaceLaunch()) { 
            m_can_space_launch = FALSE; 
        }

        if (!rec->GetSpaceLand()) { 
            m_can_space_land = FALSE; 
        }      

        if (rec->GetCanExpel()) { 
            m_army_can_expel_stealth = TRUE; 
        }      

        if (0 < army[i].GetAttack()) { 
            m_is_zero_attack = FALSE; 
        } 


    } 

    
    UnitDynamicArray* cargo_list;
    m_is_cargo_pod = FALSE;
    m_cargo_pod_intersection = 0xffffffff;
    sint32 j;
    sint32 num_carried;
    if (m_can_space_land) { 
        for (i=0; i<nUnits; i++) {  
            
            if (g_theUnitDB->Get(army[i].GetType())->GetCargoPod()) { 

                
                
                m_is_cargo_pod = TRUE;

                num_carried = army[i].GetNumCarried();
                cargo_list = army[i].GetData()->GetCargoList(); 

                for (j=0; j<num_carried; j++) { 
                    m_cargo_pod_intersection &= cargo_list->Get(j).GetMovementType();                    
                } 
            }
        } 
    }
    
    
    move_union = 0; 
    if (move_intersection == 0) {
        for (i=0; i<nUnits; i++) { 
            move_union |= army[i].GetMovementType();
        }
		if((move_union & k_Unit_MovementType_ShallowWater_Bit) &&
		   wonderutil_GetAllBoatsDeepWater(g_player[army.GetOwner()]->m_builtWonders)) {
			move_union |= k_Unit_MovementType_Sea_Bit;
		}
    } else {
		if((move_intersection & k_Unit_MovementType_ShallowWater_Bit) &&
		   wonderutil_GetAllBoatsDeepWater(g_player[army.GetOwner()]->m_builtWonders)) {
			move_intersection |= k_Unit_MovementType_Sea_Bit;
		}
	}
   
    army_minmax_move  = 1000000000; 
    float m;
    for (i=0; i<nUnits; i++) { 
        m = float(army[i].GetMaxMovePoints()); 
        if (m < army_minmax_move) { 
            army_minmax_move = m; 
        } 
    } 

    Assert (move_intersection || move_union); 
}

void UnitAstar::InitSearch(const MapPoint &start, const PLAYER_INDEX owner, 
    const MapPoint &dest, Path &good_path, sint32 &is_broken_path, Path &bad_path)
{

    g_ec_called = 0 ; 

    bad_path.Clear(); 
    good_path.Clear(); 
    
    is_broken_path = FALSE; 

    
    m_owner = owner; 
    m_is_robot = Player::IsThisPlayerARobot(owner);
    m_dest = dest; 
    m_start = start;

    m_mask_alliance = g_player[owner]->GetMaskAlliance(); 

    
    m_max_dir = NOWHERE; 
    
}

 

BOOL UnitAstar::EnterPathPoints(Path &a_path, MapPoint &old)
{
    static MapPoint pos; 
    float cost; 
    BOOL is_zoc; 
	ASTAR_ENTRY_TYPE entry;

    a_path.Start(old); 
    a_path.Next(pos);  
    for ( ; !a_path.IsEnd(); a_path.Next(pos)) { 
        if (EntryCost(old, pos, cost, is_zoc, entry)) {
            if (entry!= ASTAR_CAN_ENTER) { 
                return FALSE; 
            } 
           old = pos; 
        } else {
           return FALSE;
        }                 
    }      
    if (old != pos) { 
        if (EntryCost(old, pos, cost, is_zoc, entry)) {
            if (entry != ASTAR_CAN_ENTER) { 
                return FALSE; 
            } 
           old = pos; 
        } else {
            return FALSE;
        }
    }
    return TRUE; 
}

BOOL UnitAstar::FindVisionEdge(Path &a_path, MapPoint &old)
{
    static MapPoint pos; 

    a_path.Start(old); 
    a_path.Next(pos);  

    sint32 ao;
    CellUnitList *a = NULL;
    for ( ; !a_path.IsEnd(); a_path.Next(pos)) { 
        if (g_player[m_owner]->IsExplored(pos)) {
            a = g_theWorld->GetArmyPtr(pos); 
    
            if (a && (0 < a->Num())) {  
      			if(a->IsVisible(m_owner)) {
	    			ao = a->GetOwner(); 
		    		if (ao != m_owner) { 
                        continue; 
                    }
                }
            }

            old = pos; 
        } else {
           return FALSE;
        }                 
    }      
    if (old != pos) { 
        if (g_player[m_owner]->IsExplored(pos)) {
            a = g_theWorld->GetArmyPtr(pos); 
    
            if (a && (0 < a->Num())) {  
      			if(a->IsVisible(m_owner)) {
	    			ao = a->GetOwner(); 
		    		if (ao != m_owner) { 
                        return TRUE; 
                    }
                }
            }

            old = pos; 
        } else {
            return FALSE;
        }
    }
    return TRUE; 
}

sint32 UnitAstar::FindBrokenPath(const MapPoint &start, const MapPoint &dest, 
    Path &good_path, Path &bad_path, float &total_cost)

{           
    sint32 r; 
    static MapPoint no_enter_pos;
    static MapPoint local_start; 
    sint32 cutoff = 2000000000;
    sint32 nodes_opened=0; 

    StraightLine(start, dest, bad_path);

    Assert(0<bad_path.Num()); 
    if (bad_path.Num() < 1) { 
       return FALSE; 
    } 

    if (EnterPathPoints(bad_path, no_enter_pos)) { 
        good_path = bad_path; 
        bad_path.Clear(); 
        r = TRUE; 
    } else {        
        r = Astar::FindPath(start, no_enter_pos, good_path, total_cost,  FALSE, 
            cutoff, nodes_opened); 
        if (r) { 
            bad_path.ClipStartToCurrent();
        } else { 
            good_path.Clear(); 
        }
    } 

    return TRUE;
}










sint32 UnitAstar::GetMaxDir(MapPoint &pos) const
{   
	

    return SOUTH; 
} 



sint32 UnitAstar::FindStraightPath(const MapPoint &start, const MapPoint &dest, 
                           Path &good_path, 
                           sint32 &is_broken_path, Path &bad_path, 
                           float &total_cost, BOOL no_bad_path, 
                           const sint32 cutoff, sint32 &nodes_opened)
{            
    static MapPoint no_enter_pos;
    static MapPoint vision_edge; 
    static MapPoint tmp_point; 
    sint32 r; 
    
    StraightLine(start, dest, good_path);
    if (EnterPathPoints(good_path, no_enter_pos)) {   
       return TRUE; 
    } 

    if (g_player[m_owner]->IsExplored(dest) && g_theWorld->CanEnter(dest, m_move_intersection)) {
       
       if ((start.x == no_enter_pos.x) && (no_enter_pos.y == start.y)) { 
            
          r = Astar::FindPath(start, dest, good_path, total_cost, FALSE, 
              cutoff, nodes_opened); 
          if (r) { 
             return TRUE;
          } else if (no_bad_path) {
              return FALSE; 
          } else { 
               good_path.Clear();
               is_broken_path = TRUE; 
               StraightLine(start, dest, bad_path);
               return TRUE; 
          } 
        } else {
    
           StraightLine(start, no_enter_pos, good_path);
           if (EnterPathPoints(good_path, tmp_point)) { 
               r = Astar::FindPath(no_enter_pos, dest, bad_path, total_cost, 
                   FALSE, cutoff, nodes_opened); 
               if (r) { 
                   good_path.Concat(bad_path); 
                   bad_path.Clear(); 
                   return TRUE; 
               } else if (no_bad_path) {  
                   return FALSE; 
               } else { 
                   is_broken_path=TRUE; 
                   good_path.Clear(); 
                   StraightLine(start, dest, bad_path);
                   return TRUE; 
               }
           } else { 
               r = Astar::FindPath(start, dest, good_path, total_cost, FALSE,
                   cutoff, nodes_opened); 
               if (r) { 
                   bad_path.Clear(); 
                   return TRUE; 
               } else if (no_bad_path) {  
                   return FALSE; 
               } else { 
                   is_broken_path=TRUE; 
                   good_path.Clear(); 
                   StraightLine(start, dest, bad_path);
                   return TRUE; 
               }
           } 
        }
    } else if (no_bad_path) { 
            return FALSE;
    } else { 
        
  
       is_broken_path = TRUE;                    
       StraightLine(start, dest, bad_path);
       FindVisionEdge(bad_path, vision_edge); 
       EnterPathPoints(bad_path, no_enter_pos);

       if ((no_enter_pos.x == vision_edge.x) && (no_enter_pos.y == vision_edge.y)) { 
           if ((no_enter_pos.x == start.x) && (no_enter_pos.y == start.y)) {
               if (no_bad_path) { 
                   return FALSE; 
               } else { 
                    good_path.Clear(); 
                    StraightLine(start, dest, bad_path); 
                    return TRUE;
               }
           } else { 
                StraightLine(start, vision_edge, good_path);
                StraightLine(vision_edge, dest, bad_path); 
                if (EnterPathPoints(good_path, no_enter_pos)) { 
                    return TRUE; 
                } else { 
                    return Astar::FindPath(start, vision_edge, good_path, total_cost,  FALSE, 
                        cutoff, nodes_opened);  
                }
           }
       } else {                                
           StraightLine(start, no_enter_pos, good_path); 
           Path tmp_path;
           static MapPoint tmp_point;
           if (((no_enter_pos.x == start.x) && (no_enter_pos.y == start.y)) ||
               !EnterPathPoints(good_path, tmp_point)) { 
               r = Astar::FindPath(start, vision_edge, good_path, total_cost,  FALSE, 
                   cutoff, nodes_opened); 
               if (r) { 
                   StraightLine(vision_edge, dest, bad_path);
                   return TRUE; 
               } else if (no_bad_path) {  
                   return FALSE; 
               } else { 
                   good_path.Clear(); 
                   StraightLine(start, dest, bad_path);
                   return TRUE; 
               }

               return r;
           } else {
               r = Astar::FindPath(no_enter_pos, vision_edge, tmp_path, 
                   total_cost,  FALSE, cutoff, nodes_opened); 
               if (r) { 
                   good_path.Concat(tmp_path); 
                   StraightLine(vision_edge, dest, bad_path);
               } else if (no_bad_path) {  
                   return FALSE; 
               } else { 
                   good_path.Clear(); 
                   StraightLine(start, dest, bad_path);
                   return TRUE; 
               }
     
               return r;
           } 
       }
    }
}






    
BOOL UnitAstar::PretestDest_Enterable(const MapPoint &start, const MapPoint &dest)
{
	if (m_move_intersection) { 
         if(m_check_dest && !g_theWorld->HasCity(dest)) { 
            if(!g_theWorld->CanEnter(dest, m_move_intersection)) { 
                CellUnitList tmp_transports;
                if (m_army.m_id == (0)) { 
                    return FALSE;
                } else { 
                    return m_army.CanMoveIntoTransport(dest, tmp_transports);
                }
            } 
		}
    }

    return TRUE;
}

    
    
    
	
BOOL UnitAstar::PretestDest_HasRoom(const MapPoint &start, const MapPoint &dest)
{
    CellUnitList *dest_army = g_theWorld->GetArmyPtr(dest);

    if (dest_army && m_check_dest) {
		if (m_owner ==  dest_army ->GetOwner()) { 
			if (k_MAX_ARMY_SIZE < (m_nUnits + g_theWorld->GetArmyPtr(dest)->Num())) { 
                if (!CanMoveIntoTransports(dest)) { 
                    return FALSE;
			    }
			}
        } else if (m_is_zero_attack &&
            !g_theWorld->HasCity(dest) &&  
            dest_army->IsVisible(m_owner)) { 
            return FALSE; 
        } 
	}

    return TRUE; 
}

BOOL UnitAstar::PretestDest_SameLandContinent(const MapPoint &start, const MapPoint &dest)
{
    
     sint32 start_cont_number;
     BOOL start_is_land;
     sint32 dest_cont_number;
     BOOL dest_is_land;

    if (((m_move_intersection & k_Unit_MovementType_Land_Bit) ||
        (m_move_intersection & k_Unit_MovementType_Mountain_Bit)) 
        
        &&

        (FALSE == ((m_move_intersection & k_Unit_MovementType_Air_Bit) ||
            (m_move_intersection & k_Unit_MovementType_Space_Bit) ||
            (m_move_intersection & k_Unit_MovementType_Sea_Bit) ||
            (m_move_intersection & k_Unit_MovementType_ShallowWater_Bit))
         )
    )    
    { 
        
        g_theWorld->GetContinent(start, start_cont_number, start_is_land);

        g_theWorld->GetContinent(dest, dest_cont_number, dest_is_land);

        if (start_is_land && dest_is_land) { 
            if (start_cont_number != dest_cont_number) {
                return FALSE; 
            }
        }
    }
    return TRUE; 
}


BOOL UnitAstar::PretestDest_SameWaterContinent(const MapPoint &start, const MapPoint &dest)
{
    BOOL start_is_land; 
    sint32 start_cont_number;
    BOOL dest_is_land; 
    sint32 dest_cont_number;

    
    if	( 
			(
				(m_move_intersection & k_Unit_MovementType_Sea_Bit) || 
				(m_move_intersection & k_Unit_MovementType_Sea_Bit)
			)					  &&  
			(FALSE == 
				(
					(m_move_intersection & k_Unit_MovementType_Air_Bit) ||
					(m_move_intersection & k_Unit_MovementType_Space_Bit) ||
					(m_move_intersection & k_Unit_MovementType_Land_Bit) ||
					(m_move_intersection & k_Unit_MovementType_Mountain_Bit)
				)
			)
		)    
    { 
        
		
		
        g_theWorld->GetContinent(start, start_cont_number, start_is_land);

        g_theWorld->GetContinent(dest, dest_cont_number, dest_is_land);

		if((start_is_land == FALSE) && (dest_is_land == FALSE) && 
            (start_cont_number != dest_cont_number)) {
                return FALSE;
        }
    }
    return TRUE; 
}

BOOL UnitAstar::PretestDest_ZocEnterable(const MapPoint &start, const MapPoint &dest)
{
    sint32 i; 
    MapPoint neighbor;
    CellUnitList *the_army=NULL;

    
    if (m_ignore_zoc) return TRUE; 

    
    for (i=0; i <= SOUTH; i++) { 
            
       if (!dest.GetNeighborPosition(WORLD_DIRECTION(i), neighbor)) continue;

       
       if ((start.x == neighbor.x) && (start.y == neighbor.y)) { 
           return TRUE; 
       } 

       
       

        if (!PretestDest_Enterable(start, neighbor)) continue; 

       
       the_army = g_theWorld->GetArmyPtr(neighbor);
       if (the_army) {
    		if (m_owner !=  the_army->GetOwner()) continue; 

            if ((m_nUnits + the_army->Num()) <= k_MAX_ARMY_SIZE)  { 
                return TRUE; 
            } else { 
                continue; 
            }
 	    }

        
       if(g_theWorld->HasCity(neighbor)) { 
           if (g_theWorld->GetCity(neighbor).GetOwner() == m_owner) { 
                return TRUE; 
           } else { 
               continue; 
           } 

       }

       
       if (!g_theWorld->IsMoveZOC (m_owner, neighbor, dest, TRUE)) { 
           return TRUE; 
       } 

    }    

    return FALSE; 
}

BOOL UnitAstar::PretestDest(const MapPoint &start, const MapPoint &dest)
{
    if (!m_is_robot) {     
        if ((g_player[m_owner]->IsExplored(dest) == FALSE)) { 
            return FALSE; 
        }
     }

    if (!PretestDest_Enterable(start, dest)) return FALSE; 

    if (!PretestDest_HasRoom(start, dest)) return FALSE; 

    if (!PretestDest_SameLandContinent(start, dest)) return FALSE; 

    if (!PretestDest_SameWaterContinent(start, dest)) return FALSE; 

    if (!PretestDest_ZocEnterable(start, dest)) return FALSE;

    
    return TRUE; 
}


sint32 UnitAstar::FindPath(Army &army,  MapPoint &start,
    const PLAYER_INDEX owner, MapPoint &dest, Path &good_path, 
    sint32 &is_broken_path, Path &bad_path, float &total_cost)
                           
{
    sint32 nUnits; 
    uint32 move_intersection; 
    uint32 move_union; 
	sint32 result;

    InitArmy (army, nUnits, move_intersection, move_union, m_army_minmax_move); 

    sint32 cutoff=2000000000;
    sint32 nodes_opened=0;

	
	if (!Player::IsThisPlayerARobot(owner))
		m_pretty_path = true;

    result = FindPath(army, nUnits, move_intersection, move_union,  
       start, owner, dest, good_path, is_broken_path, bad_path, 
       total_cost, FALSE, FALSE, m_pretty_path, cutoff, nodes_opened, 
       TRUE, FALSE, TRUE);

	
	m_pretty_path = false;

	return result;

}

sint32 UnitAstar::FindPath(Army army, sint32 nUnits, 
    uint32 move_intersection, 
    uint32 move_union,  const MapPoint & start,
    const PLAYER_INDEX owner, const MapPoint & dest, Path &good_path, 
    sint32 &is_broken_path, Path &bad_path, 
    float &total_cost, const BOOL no_bad_path, 
    const BOOL check_rail_launcher, const BOOL pretty_path,     
    const sint32 cutoff, sint32 &nodes_opened,
	const BOOL &check_dest, const BOOL no_straight_lines, 
    const BOOL check_units_in_cell)

{
    if (start == dest) { 
        return FALSE; 
    }


    m_army = army; 
    m_nUnits = nUnits; 
    m_move_intersection = move_intersection;
    m_move_union = move_union;
    m_pretty_path = pretty_path; 
	m_check_dest = check_dest;
    m_check_units_in_cell = check_units_in_cell; 

	m_can_be_cargo_podded = FALSE;
    
    m_check_rail_launchers = check_rail_launcher; 
	m_ignore_zoc = (m_army.m_id != (0) && m_army.IsIgnoresZOC());
    if (!check_units_in_cell)
        m_ignore_zoc = TRUE; 

    m_no_bad_path = no_bad_path; 

    InitSearch(start, owner, dest, good_path, is_broken_path, bad_path);

    BOOL find_ok; 
    
    if (!PretestDest(start, dest)) { 

        if (no_bad_path) { 
            ClearMem(); 
            return FALSE; 
        } else { 
            find_ok =  FindBrokenPath(start, dest, good_path, bad_path, total_cost);
            is_broken_path = TRUE; 
            ClearMem();                         
            return find_ok; 
        }
    }


    static MapPoint pos, old; 
    static MapPoint tmpa, tmpb;
    static MapPoint no_enter_pos; 

    BOOL r; 

            
    
    Assert(VerifyMem());

    if (m_move_union != 0) { 
       
        if (Astar::FindPath(start, dest, good_path, total_cost, FALSE, cutoff, nodes_opened)) { 
            ClearMem(); 
            return TRUE; 
        } else if (no_bad_path) { 
            ClearMem(); 
            return FALSE;
        }

    } else {



        if ((m_move_intersection & k_Unit_MovementType_Air_Bit)) { 
            if (!no_straight_lines) { 
                r = FindStraightPath(start, dest, good_path, is_broken_path, 
                    bad_path, total_cost, no_bad_path, cutoff, nodes_opened);
                ClearMem(); 
                return r; 
            }
        }


        if (((m_move_intersection & k_Unit_MovementType_Sea_Bit)||
			 (m_move_intersection & k_Unit_MovementType_ShallowWater_Bit)) &&
			!(m_move_intersection & k_Unit_MovementType_Land_Bit)) {
			
			
			

            if (!m_check_dest || 
				(g_theWorld->CanEnter(dest, m_move_intersection) ||
				 (g_theWorld->HasCity(dest)
                 
				  
                  
                 ) )) { 
                
                if (!no_straight_lines) { 
                    StraightLine(start, dest, good_path);
                    if (EnterPathPoints(good_path, no_enter_pos)) {   
                        ClearMem(); 
                        return TRUE; 
                    } 
                }

                if (Astar::FindPath(start, dest, good_path, total_cost, FALSE, cutoff, nodes_opened)) { 
                    ClearMem();
                    return TRUE; 
                } else if (no_bad_path) { 
                    ClearMem(); 
                    return FALSE;
                }
            } else if (no_bad_path) { 
                ClearMem(); 
                return FALSE;
            }

            is_broken_path = TRUE; 
            r = FindBrokenPath(start, dest, good_path, bad_path, total_cost);
            ClearMem(); 
            return r; 
        }
    }




    
    if (!m_check_dest || 
		(
            (g_theWorld->CanEnter(dest, m_move_intersection)) ||
		    CanMoveIntoTransports(pos)
        )
       ) { 
    
        if (Astar::FindPath(start, dest, good_path, total_cost, FALSE, cutoff, nodes_opened)) { 
            ClearMem(); 
            return TRUE; 
        } else if (no_bad_path) { 
            ClearMem(); 
            return FALSE;
        }
    } else if (no_bad_path) { 
        ClearMem(); 
        return FALSE;
    }

    is_broken_path = TRUE;  
 

    r = FindBrokenPath(start, dest, good_path, bad_path, total_cost);
    is_broken_path = TRUE; 

    ClearMem(); 
    return r; 

    ClearMem(); 
    return FALSE;
}


BOOL UnitAstar::IsBeachLanding(const MapPoint &prev,
							   const MapPoint &pos,
							   const uint32 &m_move_intersection)
{
	
	if (m_move_intersection & k_Unit_MovementType_Air_Bit)
		return FALSE;

	uint32 prev_move;
	uint32 pos_move;

	
	
	if ((m_move_intersection & k_Unit_MovementType_Sea_Bit ||
		 m_move_intersection & k_Unit_MovementType_ShallowWater_Bit) &&
		(m_move_intersection & k_Unit_MovementType_Land_Bit ||
		 m_move_intersection & k_Unit_MovementType_Mountain_Bit))
		{
			prev_move = g_theWorld->GetMovementType(prev); 
			pos_move = g_theWorld->GetMovementType(pos);

			
			if ((prev_move & k_Unit_MovementType_Sea_Bit || 
				 prev_move & k_Unit_MovementType_ShallowWater_Bit) &&
				(pos_move & k_Unit_MovementType_Land_Bit ||
				 pos_move & k_Unit_MovementType_Mountain_Bit))
				return TRUE;
		}
	return FALSE;
}


void UnitAstar::ClearMem()
{
    m_move_union = 0xcdcdcdcd;
    m_move_intersection = 0xcdcdcdcd; 
    m_max_dir = 0xcdcdcdcd;
    m_mask_alliance = 0xcdcdcdcd;
    m_dest.x = sint16(0xcdcd); 
    m_dest.y = sint16(0xcdcd); 
	m_start.x = sint16(0xcdcd);
	m_start.y = sint16(0xcdcd); 
    m_owner = 0xcdcdcdcd; 
    m_nUnits = 0xcdcdcdcd;
    m_army.m_id = 0xcdcdcdcd;    
    m_army_minmax_move = -9999999.0f;
    m_can_space_launch = 0x0; 
    m_can_space_land = 0x0; 
	m_can_be_cargo_podded = 0xcdcdcdcd;
}


BOOL UnitAstar::VerifyMem() const
{
    if (m_move_union == 0xcdcdcdcd) return FALSE;
    if (m_move_intersection == 0xcdcdcdcd) return FALSE; 
    if (m_max_dir == 0xcdcdcdcd) return FALSE;
    if (m_mask_alliance == 0xcdcdcdcd) return FALSE;
    if (m_dest.x == 0xcdcdcdcd) return FALSE; 
    if (m_dest.y == 0xcdcdcdcd) return FALSE; 
    if (m_start.x == 0xcdcdcdcd) return FALSE; 
    if (m_start.y == 0xcdcdcdcd) return FALSE; 
    if (m_owner == 0xcdcdcdcd) return FALSE; 
    if (m_nUnits == 0xcdcdcdcd) return FALSE;
    if (m_army.m_id == 0xcdcdcdcd) return FALSE;    
    if (m_army_minmax_move == -99999999) return FALSE;
    if (m_can_space_launch == 0xcdcdcdcd) return FALSE; 
    if (m_can_space_land == 0xcdcdcdcd) return FALSE; 
	if (m_can_be_cargo_podded == 0xcdcdcdcd) return FALSE;

    return TRUE; 
}
#if !defined (ACTIVISION_ORIGINAL)
BOOL UnitAstar::CheckIsDangerForPos(const MapPoint & myPos, const BOOL IsCivilian)
{

	Cell* c = g_theWorld->GetCell(myPos);
    Diplomat & diplomat = Diplomat::GetDiplomat(m_owner);
    ai::Regard baseRegard = NEUTRAL_REGARD;

	PLAYER_INDEX owner;

	sint32 i; 
	MapPoint neighbor;
	MapPoint start;
	CellUnitList *the_army=NULL;
	Unit the_city;
	m_army->GetPos(start);


	for (i=0; i <= SOUTH; i++) 
	{ 		   
	   if (!myPos.GetNeighborPosition(WORLD_DIRECTION(i), neighbor)) continue;

	   if (neighbor == start || neighbor == m_dest)
	   { 
		   continue;
	   } 
		
	   //Check for hostile army
	   the_army = g_theWorld->GetArmyPtr(neighbor);
	   the_city = g_theWorld->GetCity(neighbor);
	   if (the_army || the_city.IsValid()) 
	   {
		    if (the_army) owner = the_army->GetOwner();
		    else owner = the_city.GetOwner();

            if (m_owner != owner)
	   {
				baseRegard = diplomat.GetBaseRegard(owner);
				sint32 turnsatwar = AgreementMatrix::s_agreements.TurnsAtWar(m_owner, owner);
    		    if (baseRegard <= NEUTRAL_REGARD || turnsatwar >= 0)
			{
					if (the_city.IsValid()) //TO DO : Add conditions (in danger only if the_army not civilian
				{
					/*	DPRINTF(k_DBG_MAPANALYSIS, 
						("\t Danger for Pos (%3d,%3d) : City (%3d,%3d)\n",
						myPos.x,
						myPos.y,
						neighbor.x,
						neighbor.y));*/
						return true;
				}
				
					if (the_army->Num() > g_theWorld->GetArmyPtr(start)->Num() || IsCivilian)
				{
					/*	DPRINTF(k_DBG_MAPANALYSIS, 
						("\t Danger for Pos (%3d,%3d) : Bigger Army at (%3d,%3d)\n",
						myPos.x,
						myPos.y,
						neighbor.x,
						neighbor.y));
						return true;					*/
			}
 		}
		   } 
	   }
	}
    /*DPRINTF(k_DBG_MAPANALYSIS, 
    ("\t No Danger for Pos (%3d,%3d)\n",
	myPos.x,
    myPos.y));*/
	return false;
}


#endif
