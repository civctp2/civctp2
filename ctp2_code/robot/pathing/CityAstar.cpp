//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : A-star pathfinding for City
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
// - Added avoidList Param
// - Added method to build roads around dead tiles (using the avoidList)
//----------------------------------------------------------------------------
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

#if defined (ACTIVISION_ORIGINAL)
    if (!FindPath(start, dest, tmp_path, cost,  FALSE, cutoff, nodes_opened)) 
#else
	MapPoint_List avoidList;
    if (!FindPath(start, dest, tmp_path, cost,  FALSE, cutoff, nodes_opened, avoidList))
#endif
	{ 
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
#if defined (ACTIVISION_ORIGINAL)
    if (FindPath(start, dest, new_path, total_cost,  FALSE, cutoff, nodes_opened)) 
	{		
        return true;
    }
	return false;
#else
	const sint32 MAX_PATH_CYCLES = 5;
	sint32 cycles_count = 0;
	sint32 GoodPathFound = 0;
	Path last_good_path;
	bool Is_last_good_path_valid = false;
	MapPoint_List avoidList;
    while(!GoodPathFound && cycles_count < MAX_PATH_CYCLES)
	{	
		GoodPathFound = FindPath(start, dest, last_good_path, total_cost,  FALSE, cutoff, nodes_opened,avoidList);
		if (!GoodPathFound)
		{
			if(!Is_last_good_path_valid)
				break;
			else
			{	//Use the last_good_path - even if it has not been checked safe
				new_path = last_good_path;
				return TRUE; 
			}
		}
		else
		{ //A path has been founded. Check if it's safe
			if (CheckIsPollutionAlongPath(last_good_path, avoidList))
			{
				new_path = last_good_path;
				return TRUE; 
			}
			{
				GoodPathFound = 0; //to check another path with avoidList
			}
		}
	}
	if (!GoodPathFound && !Is_last_good_path_valid)
	{
	        new_path = last_good_path;
			return FALSE;
	}
	else
	{		//accept the last_good_path
			return TRUE; 
	}
#endif		
}

#if !defined (ACTIVISION_ORIGINAL)
BOOL CityAstar::CheckIsPollutionAlongPath(const Path & my_path, MapPoint_List & avoidList)
{
	BOOL IsPathClean = true;
	Path test_path = my_path;
	MapPoint myPos;
	test_path.GetStartPoint(myPos);
	if (myPos.x > 0 && myPos.y > 0)
	{
		test_path.Next(myPos); //the first point is not to check...
		while (myPos != test_path.GetEnd())
		{
			Cell* c = g_theWorld->GetCell(myPos);
			
			if (c->IsDead()) //cell is polluted (dead)
			{
				if (!Astar::IsInAvoidList(myPos,avoidList))
				{
					avoidList.push_back(myPos);
				}
				IsPathClean = false;
			}		
			test_path.Next(myPos);
			//myPos = test_path.GetCurrentPoint();
		}
		
	}
	return IsPathClean;
}

#endif