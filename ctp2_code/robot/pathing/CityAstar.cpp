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
// - Road path generation is no more dependent on tile move costs for units,
//   but on pw costs per tile.
// - Road path may go through foreign territory but is even more expensive 
//   in comparision to unexplored territory. - Oct. 6th 2004 Martin Gühmann
//
//----------------------------------------------------------------------------

#include "c3.h"
#include "c3errors.h"
#include "globals.h"


#include "CityAstar.h"
#include "dynarr.h"
#include "Path.h"

#include "XY_Coordinates.h"
#include "World.h"
#include "dynarr.h"
#include "player.h"

#include "MoveFlags.h"

//Added by Martin Gühmann to acces the terrain improvement database
#include "TerrainImprovementRecord.h"
#include "terrainutil.h"
#include "TerrImprove.h"
#include "AgreementMatrix.h" // Allow alliance checking
#include "StrategyRecord.h"  // For accessing the strategy database
#include "Diplomat.h"        // To be able to retrieve the current strategy

CityAstar g_city_astar; 


sint32 CityAstar::EntryCost(const MapPoint &prev, const MapPoint &pos,                           
                            float &cost, BOOL &is_zoc, ASTAR_ENTRY_TYPE &entry) 

{
	sint32 i;

	if(m_pathRoad){
		
		const TerrainImprovementRecord *rec = terrainutil_GetBestRoad(m_owner, pos);
		if(g_player[m_owner]->IsExplored(pos) == FALSE 
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

//		cost = float(g_theWorld->GetMoveCost(pos) * effect->GetProductionCost());
		cost = static_cast<float>(effect->GetProductionCost());
		if(g_theWorld->AccessCell(pos)->GetOwner() == -1){
			cost *= 10000.0; // Should be calculated from most expensive tile improvement + 20 percent
		}
		else if((g_theWorld->AccessCell(pos)->GetOwner() >= 0
		&&      (g_theWorld->AccessCell(pos)->GetOwner() != m_owner
		&&       !AgreementMatrix::s_agreements.HasAgreement(m_owner, g_theWorld->AccessCell(pos)->GetOwner(), PROPOSAL_TREATY_ALLIANCE)))
		){
			cost *= 20000.0; // Should be calculated from most expensive tile improvement + 20 percent and everything times 2
		}

		sint32 type = rec->GetIndex();

		const StrategyRecord & strategy = Diplomat::GetDiplomat(m_owner).GetCurrentStrategy();
		double roadCostsPercent;
		strategy.GetRoadAlreadyThereCostsCoefficient(roadCostsPercent);


		for(i = 0; i < g_theWorld->AccessCell(pos)->GetNumDBImprovements(); ++i){
			rec = g_theTerrainImprovementDB->Get(g_theWorld->AccessCell(pos)->GetDBImprovement(i));
			if(rec){
				effect = terrainutil_GetTerrainEffect(rec, pos);
				if(effect && effect->GetMoveCost()){
					cost *= static_cast<float>(roadCostsPercent);
					break;
				}
			}
		}

		if(i == g_theWorld->AccessCell(pos)->GetNumDBImprovements()){
			for(i = 0; i < g_theWorld->AccessCell(pos)->GetNumImprovements(); ++i){
				if(g_theWorld->AccessCell(pos)->AccessImprovement(i).GetType() == type){
					cost *= static_cast<float>(roadCostsPercent);
					break;
				}
			}
		}

		entry = ASTAR_CAN_ENTER;
		return TRUE;
	}
	else{
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
							 PLAYER_INDEX owner,
							 Path & new_path,
							 float & total_cost)
{
    m_alliance_mask = 0x0; 
	m_pathRoad = true;
	total_cost = 0.0;

    sint32 cutoff = 2000000000; 
    sint32 nodes_opened=0;

	m_owner = owner;

    if (FindPath(start, dest, new_path, total_cost,  FALSE, cutoff, nodes_opened)) { 
        return true;
    }
	return false;
}

