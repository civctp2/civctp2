












#include "c3.h"
#include "c3errors.h"
#include "Globals.h"


#include "CityAstar.h"
#include "DynArr.h"
#include "Path.h"

#include "XY_Coordinates.h"
#include "World.h"
#include "DynArr.h"
#include "Player.h"

#include "MoveFlags.h"


CityAstar g_city_astar; 


sint32 CityAstar::EntryCost(const MapPoint &prev, const MapPoint &pos,                           
                            float &cost, BOOL &is_zoc, ASTAR_ENTRY_TYPE &entry) 

{
    
	if (m_pathRoad)
	{
		
		
		if (g_player[m_owner]->IsExplored(pos) == FALSE ||
			g_theWorld->IsWater(pos)) { 
			cost = k_ASTAR_BIG; 
			entry = ASTAR_BLOCKED; 
			return FALSE;
		}
        cost = float(g_theWorld->GetMoveCost(pos)); 
		entry = ASTAR_CAN_ENTER;
		return TRUE;
	}
	else
	{

		return true;
	}

    if (g_theWorld->IsMoveZOC (m_owner, prev, pos, FALSE)) { 
        cost = k_ASTAR_BIG; 
		entry = ASTAR_BLOCKED;
        return FALSE; 
    } else { 
        cost = float(g_theWorld->GetMoveCost(pos)); 
		entry = ASTAR_CAN_ENTER;
        return TRUE; 
    }
}

sint32 CityAstar::GetMaxDir(MapPoint &pos) const
{
    return SOUTH; 
}
    


void CityAstar::FindCityDist(PLAYER_INDEX owner, const MapPoint &start, const MapPoint &dest, 
      float &cost)

{
    Path tmp_path; 

    m_owner = owner; 
    m_alliance_mask = g_player[m_owner]->GetMaskAlliance(); 
	m_pathRoad = false;

    sint32 cutoff = 2000000000; 
    sint32 nodes_opened=0;

    if (!FindPath(start, dest, tmp_path, cost,  FALSE, cutoff, nodes_opened)) { 
         cost = float(g_player[m_owner]->GetMaxEmpireDistance()); 
    }
}

bool CityAstar::FindRoadPath(const MapPoint & start, 
							 const MapPoint & dest,
							 Path & new_path,
							 float & total_cost)
{
    m_alliance_mask = 0x0; 
	m_pathRoad = true;
	total_cost = 0.0;

    sint32 cutoff = 2000000000; 
    sint32 nodes_opened=0;

    if (FindPath(start, dest, new_path, total_cost,  FALSE, cutoff, nodes_opened)) { 
        return true;
    }
	return false;
}

