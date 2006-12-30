//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : A-star pathfinding for City
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
// - Initialised roadCostsPercent, so it will have a proper value when not 
//   set in the strategy.
// - removed bool fix attempt; - E 12.27.2006 note: MArtin G reports that
//   fixing this defect made the AI act "weird" even though the bool in ln198 
//   causes an error.
//
//----------------------------------------------------------------------------

#include "c3.h"
#include "CityAstar.h"

#include "AgreementMatrix.h"    // Allow alliance checking
#include "Astar.h"              // k_ASTAR_BIG, ASTAR_BLOCKED, ASTAR_CAN_ENTER
#include "c3errors.h"
#include "Diplomat.h"           // To be able to retrieve the current strategy
#include "globals.h"
#include "MoveFlags.h"
#include "Path.h"
#include "player.h"             // g_player
#include "StrategyRecord.h"     // For accessing the strategy database
#include "TerrainImprovementRecord.h"
#include "terrainutil.h"
#include "TerrImprove.h"
#include "World.h"              // g_theWorld

CityAstar g_city_astar; 

namespace
{
	sint32 const    NODE_VISIT_COUNT_LIMIT  = 2000000000;
}

sint32 CityAstar::EntryCost
(
    MapPoint const &    prev,
    MapPoint const &    pos,
    float &             cost,
    BOOL &              is_zoc,
    ASTAR_ENTRY_TYPE &  entry
) 
{
	if (m_pathRoad)
	{
		const TerrainImprovementRecord *rec = terrainutil_GetBestRoad(m_owner, pos);
		if (!rec || !g_player[m_owner]->IsExplored(pos))
		{
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

		Cell *  entryCell   = g_theWorld->GetCell(pos);
		
		if (entryCell->GetOwner() < 0)
		{
			// Unowned territory
			cost *= 10000.0; // Should be calculated from most expensive tile improvement + 20 percent
		}
		else if (  (entryCell->GetOwner() != m_owner)
		        && !AgreementMatrix::s_agreements.HasAgreement(m_owner, entryCell->GetOwner(), PROPOSAL_TREATY_ALLIANCE)
		        )
		{
			// "Hostile" territory
			cost *= 20000.0; // Should be calculated from most expensive tile improvement + 20 percent and everything times 2
		}

		sint32 type = rec->GetIndex();

		const StrategyRecord & strategy = Diplomat::GetDiplomat(m_owner).GetCurrentStrategy();
		double roadCostsPercent = strategy.GetRoadAlreadyThereCostsCoefficient();

		// Reconsider this. This doesn't look right
		sint32 i;
		for(i = 0; i < entryCell->GetNumDBImprovements(); ++i)
		{
			rec = g_theTerrainImprovementDB->Get(entryCell->GetDBImprovement(i));
			if(rec){
				effect = terrainutil_GetTerrainEffect(rec, pos);
				if(effect && effect->HasMoveCost()){
					// roadCostsPercent < 1.0 so that new roads are planned along old roads
					cost *= static_cast<float>(roadCostsPercent);
					break;
				}
			}
		}

		if(i == entryCell->GetNumDBImprovements()){
			for(i = 0; i < entryCell->GetNumImprovements(); ++i)
			{
				// Double the bonus for the same road type
				if (entryCell->AccessImprovement(i).GetType() == type){
					cost *= static_cast<float>(roadCostsPercent);
					break;
				}
			}
		}

		entry = ASTAR_CAN_ENTER;
	}

	return TRUE;
}

sint32 CityAstar::GetMaxDir(MapPoint &pos) const
{
	return SOUTH;
}



void CityAstar::FindCityDist
(
    PLAYER_INDEX        owner,
    MapPoint const &    start,
    MapPoint const &    dest,
    float &             cost
)
{
	m_alliance_mask     = g_player[m_owner]->GetMaskAlliance(); 
	m_pathRoad          = false;
	m_owner             = owner; 

	Path    tmp_path; 
	sint32  nodes_opened    = 0;

	if (!FindPath(start, dest, tmp_path, cost, FALSE, NODE_VISIT_COUNT_LIMIT, nodes_opened)) 
	{
		cost = static_cast<float>(g_player[m_owner]->GetMaxEmpireDistance()); 
	}
}

bool CityAstar::FindRoadPath
(
	MapPoint const &    start,
	MapPoint const &    dest,
	PLAYER_INDEX        owner,
	Path &              new_path,
	float &             total_cost
)
{
	m_alliance_mask     = 0x0;
	m_pathRoad          = true;
	m_owner             = owner;

	total_cost          = 0.0;

	sint32  nodes_opened    = 0;

	return FindPath(start, dest, new_path, total_cost, false, NODE_VISIT_COUNT_LIMIT, nodes_opened);
}

