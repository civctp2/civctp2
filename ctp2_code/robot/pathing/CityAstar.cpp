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
// - Modified EntryCost by Martin Gühmann to allow:
//   - Bypassing of tiles without road improvement, e.g. polluted tiles
//   - Bypassing of unowend tiles and foreign tiles
//   - Bypassing of unexplored tiles
//   - Linking cities on different continents.
//   - Building of undersea tunnels
// - Added owner argument to to FindRoadPath function so that m_owner 
//   can be set in that function. The result is that the path finding
//   routine takes unexplored tiles into consideration, by Martin Gühmann.
//
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

#if !defined(ACTIVISION_ORIGINAL)
//Added by Martin Gühmann to acces the terrain improvement database
#include "TerrainImprovementRecord.h"
#include "terrainutil.h"
#include "AgreementMatrix.h" //Allow alliance checking
#endif

CityAstar g_city_astar; 


sint32 CityAstar::EntryCost(const MapPoint &prev, const MapPoint &pos,                           
                            float &cost, BOOL &is_zoc, ASTAR_ENTRY_TYPE &entry) 

{
#if defined(ACTIVISION_ORIGINAL)
//Removed by Martin Gühamnn
	if (m_pathRoad)
	{
		
		
		if (g_player[m_owner]->IsExplored(pos) == FALSE ||
			g_theWorld->IsWater(pos)) { //Cannot build undersea tunnels
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

	//Never executed
    if (g_theWorld->IsMoveZOC (m_owner, prev, pos, FALSE)) { 
        cost = k_ASTAR_BIG; 
		entry = ASTAR_BLOCKED;
        return FALSE; 
    } else { 
        cost = float(g_theWorld->GetMoveCost(pos)); 
		entry = ASTAR_CAN_ENTER;
        return TRUE; 
    }
#else
	if(m_pathRoad){
		
		const TerrainImprovementRecord *rec = terrainutil_GetBestRoad(m_owner, pos);
		if(g_player[m_owner]->IsExplored(pos) == FALSE 
		||(g_theWorld->AccessCell(pos)->GetOwner() >= 0
		&&(g_theWorld->AccessCell(pos)->GetOwner() != m_owner
		&& !AgreementMatrix::s_agreements.HasAgreement(m_owner, g_theWorld->AccessCell(pos)->GetOwner(), PROPOSAL_TREATY_ALLIANCE)))
		|| !rec
		){ 
			cost = k_ASTAR_BIG; 
			entry = ASTAR_BLOCKED; 
			return FALSE;
		}
		
		const TerrainImprovementRecord::Effect *effect = terrainutil_GetTerrainEffect(rec, pos);
		if(!effect){
			cost = k_ASTAR_BIG; 
			entry = ASTAR_BLOCKED; 
			return FALSE;
		}

        cost = float(g_theWorld->GetMoveCost(pos) * effect->GetProductionCost());
		if(g_theWorld->AccessCell(pos)->GetOwner() == -1){
			cost *= 10000;
		}

		entry = ASTAR_CAN_ENTER;
		return TRUE;
	}
	else{
		return TRUE;
	}
#endif
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
#if !defined(ACTIVISION_ORIGINAL)
							 PLAYER_INDEX owner,
#endif
							 Path & new_path,
							 float & total_cost)
{
    m_alliance_mask = 0x0; 
	m_pathRoad = true;
	total_cost = 0.0;

    sint32 cutoff = 2000000000; 
    sint32 nodes_opened=0;

#if !defined(ACTIVISION_ORIGINAL)
	m_owner = owner;
#endif

    if (FindPath(start, dest, new_path, total_cost,  FALSE, cutoff, nodes_opened)) { 
        return true;
    }
	return false;
}

