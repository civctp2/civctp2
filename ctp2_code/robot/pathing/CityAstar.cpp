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
// - Road costs are now based on all the tile improvements a tile has,
//   including those that are under construction. (17-Jan-2008 Martin Gühmann)
// - A strategy option can also use to the base move costs of a tile. (17-Jan-2008 Martin Gühmann)
//
//----------------------------------------------------------------------------

#include "c3.h"
#include "CityAstar.h"

#include "AgreementMatrix.h"    // Allow alliance checking
#include "Astar.h"              // k_ASTAR_BIG, ASTAR_BLOCKED, ASTAR_CAN_ENTER
#include "c3errors.h"
#include "Diplomat.h"           // To be able to retrieve the current strategy
#include "Globals.h"
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

bool CityAstar::EntryCost
(
    MapPoint const &    prev,
    MapPoint const &    pos,
    float &             cost,
    bool &              is_zoc,
    ASTAR_ENTRY_TYPE &  entry
)
{
	if(m_pathRoad)
	{
		const TerrainImprovementRecord *rec = terrainutil_GetBestRoad(m_owner, pos);
		if (!rec || !g_player[m_owner]->IsExplored(pos))
		{
			cost = k_ASTAR_BIG;
			entry = ASTAR_BLOCKED;
			return false;
		}

		const TerrainImprovementRecord::Effect *effect = terrainutil_GetTerrainEffect(rec, pos);
		if(!effect)
		{
			cost = k_ASTAR_BIG;
			entry = ASTAR_BLOCKED;
			return false;
		}

		float prodCost = static_cast<float>(effect->GetProductionCost());

		const StrategyRecord & strategy = Diplomat::GetDiplomat(m_owner).GetCurrentStrategy();

		Cell *  entryCell   = g_theWorld->GetCell(pos);
		bool useBaseMovement = strategy.GetUseBaseMoveCostsForRoads() != 0;

		if(useBaseMovement)
		{
			cost = static_cast<float>(entryCell->GetBaseMoveCosts());
		}
		else
		{
			cost = static_cast<float>(entryCell->GetFutureTerrainMoveCost());
		}

		if(useBaseMovement
		|| static_cast<float>(entryCell->GetBaseMoveCosts()) > cost
		){
			// We want to follow existing roads even if they run through mountains
			prodCost = static_cast<float>(terrainutil_GetMinimumProductionCost(rec->GetIndex()));

			if(entryCell->HasTerrainImprovementOrInFuture(rec->GetIndex()))
			{
				float roadCostPercent = static_cast<float>(strategy.GetRoadAlreadyThereCostsCoefficient());

				// Make roads of the same type slightly cheaper
				prodCost *= roadCostPercent;
			}
		}

		cost += prodCost;

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

		entry = ASTAR_CAN_ENTER;
	}
	else
	{
		///ToDo: Cleanup this mess
		if(m_pathLand && g_theWorld->IsWater(pos))
		{
			cost = k_ASTAR_BIG;
			entry = ASTAR_BLOCKED;
			return false;
		}

		if(m_pathPenalizeCantEnter != 0)
		{
			Cell *  entryCell   = g_theWorld->GetCell(pos);

			cost  = static_cast<float>(entryCell->GetMoveCost());
			entry = ASTAR_CAN_ENTER;

			if(g_theWorld->CanEnter(pos, m_pathPenalizeCantEnter))
			{
				return true;
			}
			else
			{
				cost *= 10000.0;
				return true;
			}
		}

		if(m_pathLand || m_simpleDistance)
		{
			cost = sqrt(static_cast<float>(MapPoint::GetSquaredDistance(m_start, pos)))
				 + sqrt(static_cast<float>(MapPoint::GetSquaredDistance(m_dest,  pos)));
			entry = ASTAR_CAN_ENTER;
			return true;
		}

		Cell *  entryCell   = g_theWorld->GetCell(pos);

		cost  = static_cast<float>(entryCell->GetMoveCost());
		entry = ASTAR_CAN_ENTER;

		if(!g_player[m_owner]->IsExplored(pos))
		{
			cost *= 4.0F;
		}
		else if(entryCell->GetOwner() != m_owner)
		{
			cost *= 3.0F;
		}
		else if(entryCell->GetOwner() < 0)
		{
			cost *= 2.0F;
		}
	}

	return true;
}

sint32 CityAstar::GetMaxDir(MapPoint &pos) const
{
	return SOUTH;
}

bool CityAstar::IsLandConnected
(
    PLAYER_INDEX        owner,
    MapPoint const &    start,
    MapPoint const &    dest,
    float &             cost,
    sint32 &            distance,
    sint32              maxSquaredDistance
)
{
	m_pathRoad              = true;
	m_owner                 = owner;
	m_pathLand              = true;
	m_maxSquaredDistance    = maxSquaredDistance;
	m_simpleDistance        = false;
	m_start                 = start;
	m_dest                  = dest;
	m_pathPenalizeCantEnter = 0x0;
	m_alliance_mask         = g_player[m_owner]->GetMaskAlliance();

	Path    tmp_path;
	sint32  nodes_opened    = 0;

	bool r = FindPath(start, dest, tmp_path, cost, false, NODE_VISIT_COUNT_LIMIT, nodes_opened);
	distance = tmp_path.Num();
	return r;
}

void CityAstar::FindCityDist
(
    PLAYER_INDEX        owner,
    MapPoint const &    start,
    MapPoint const &    dest,
    float &             cost,
    sint32 &            distance
)
{
	m_pathRoad              = false;
	m_owner                 = owner;
	m_pathLand              = false;
	m_maxSquaredDistance    = -1;
	m_simpleDistance        = false;
	m_start                 = start;
	m_dest                  = dest;
	m_pathPenalizeCantEnter = 0x0;
	m_alliance_mask         = g_player[m_owner]->GetMaskAlliance();

	Path    tmp_path;
	sint32  nodes_opened    = 0;

	if (!FindPath(start, dest, tmp_path, cost, false, NODE_VISIT_COUNT_LIMIT, nodes_opened))
	{
		cost     = static_cast<float>(g_player[m_owner]->GetMaxEmpireDistance());
		distance = 0x7fffffff;
	}
	else
	{
		distance = tmp_path.Num();
	}
}

void CityAstar::FindCantEnterPenaltyDistance
(
    PLAYER_INDEX        owner,
    MapPoint const &    start,
    MapPoint const &    dest,
    float &             cost,
    sint32 &            distance,
    uint32 &            pathPenalizeCantEnter
)
{
	m_pathRoad              = false;
	m_owner                 = owner;
	m_pathLand              = false;
	m_maxSquaredDistance    = -1;
	m_simpleDistance        = false;
	m_start                 = start;
	m_dest                  = dest;
	m_pathPenalizeCantEnter = pathPenalizeCantEnter;
	m_alliance_mask         = g_player[m_owner]->GetMaskAlliance();

	Path    tmp_path;
	sint32  nodes_opened    = 0;

	if (!FindPath(start, dest, tmp_path, cost, false, NODE_VISIT_COUNT_LIMIT, nodes_opened))
	{
		cost     = static_cast<float>(g_player[m_owner]->GetMaxEmpireDistance());
		distance = 0x7fffffff;
	}
	else
	{
		distance = tmp_path.Num();
	}
}

bool CityAstar::FindSimpleDistancePath
(
    MapPoint const &    start,
    MapPoint const &    dest,
    PLAYER_INDEX        owner,
    Path &              new_path,
    float &             total_cost
)
{
	m_pathRoad              = false;
	m_owner                 = owner;
	m_pathLand              = false;
	m_maxSquaredDistance    = -1;
	m_simpleDistance        = true;
	m_start                 = start;
	m_dest                  = dest;
	m_pathPenalizeCantEnter = 0x0;
	m_alliance_mask         = 0x0;

	total_cost              = 0.0;

	sint32 nodes_opened     = 0;

	FindPath(start, dest, new_path, total_cost, false, NODE_VISIT_COUNT_LIMIT, nodes_opened);

	return total_cost > 0.0;
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
	m_pathRoad              = true;
	m_owner                 = owner;
	m_pathLand              = false;
	m_maxSquaredDistance    = -1;
	m_simpleDistance        = false;
	m_start                 = start;
	m_dest                  = dest;
	m_pathPenalizeCantEnter = 0x0;
	m_alliance_mask         = 0x0;

	total_cost              = 0.0;

	sint32 nodes_opened     = 0;

	FindPath(start, dest, new_path, total_cost, false, NODE_VISIT_COUNT_LIMIT, nodes_opened);

	return total_cost > 0.0;
}
