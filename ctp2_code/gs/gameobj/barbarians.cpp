//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : Barbarian placement and generation
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
// - Alter the algorithm used to place barbarians in single player games,
//   making it the same as that used in multiplayer games - JJB 2004/12/13
// - Add a NoBarbarian flag which makes the unit not appear as barbarian
//
//----------------------------------------------------------------------------


#include "c3.h"
#include "Barbarians.h"
#include "MapPoint.h"
#include "RandGen.h"
#include "Player.h"
#include "Globals.h"
#include "Unit.h"
#include "AICause.h"
#include "Advances.h"
#include "XY_Coordinates.h"
#include "World.h"
#include "RiskDB.h"
#include "ProfileDB.h"
#include "network.h"
#include "UnitDynArr.h"
#include "Cell.h"
#include "GameSettings.h"
#include "TurnCnt.h"
#include "UnitRecord.h"
#include "Exclusions.h"

extern Player **g_player;
extern RandomGenerator *g_rand;
extern World *g_theWorld;
extern ProfileDB *g_theProfileDB;
extern TurnCount *g_turn;

struct BestUnit {
	sint32 index;
	sint32 attack;
};

BOOL SomeoneCanHave(const UnitRecord *rec)
{
	sint32 p;

	for(p = 0; p < k_MAX_PLAYERS; p++) {
		
		
		if(g_player[p] && g_player[p]->m_advances->HasAdvance(rec->GetEnableAdvanceIndex()))
			return TRUE;
	}
	return FALSE;
}

sint32 Barbarians::ChooseUnitType()
{
	const RiskRecord *risk = g_theRiskDB->Get(g_theGameSettings->GetRisk());
	sint32 num_best_units = risk->m_barbarianUnitRankMin;
	BestUnit *best = new BestUnit[num_best_units];
	sint32 i, j, k;
	sint32 count = 0;

	for(i = 0; i < num_best_units; i++) {
		best[i].index = -1;
		best[i].attack = -1;
	}

	for(i = 0; i < g_theUnitDB->NumRecords(); i++) {
		const UnitRecord *rec = g_theUnitDB->Get(i);
		if(rec->GetCantBuild())
			continue;

		if(!rec->GetMovementTypeLand())
			continue;
		
		if(rec->GetAttack() < 1)
			continue;

		if(g_exclusions && g_exclusions->IsUnitExcluded(i))
			continue;
		if(rec->GetNoBarbarian())
			continue;
	
		
		

		if(SomeoneCanHave(rec)) {
			for(j = 0; j < num_best_units; j++) {
				if(rec->GetAttack() > best[j].attack) {
					for(k = num_best_units - 1; k >= j+1; k--) {
						best[k] = best[k-1];
					}
					best[j].index = i;
					best[j].attack = (sint32)rec->GetAttack();
					count++;
					break;
				}
			}
		}
	}

	Assert(count > 0);

	if(count > num_best_units)
		count = num_best_units;

	sint32 rankMax;
	if(risk->m_barbarianUnitRankMax >= count) {
		rankMax = count - 1;
	} else {
		rankMax = risk->m_barbarianUnitRankMax;
	}
	sint32 whichbest = g_rand->Next(count - rankMax) + rankMax;
	if(whichbest >= count)
		whichbest = count - 1;

	sint32 ret = best[whichbest].index;
	delete [] best;
	return ret;
}

BOOL Barbarians::AddBarbarians(const MapPoint &point, PLAYER_INDEX meat,
							   BOOL fromGoodyHut)
{
	if(g_network.IsClient() && !g_network.IsLocalPlayer(meat))
		return FALSE;


	if(g_turn->GetRound() < g_theRiskDB->Get(g_theGameSettings->GetRisk())->m_firstBarbarianTurn ||
	   g_turn->GetRound() >= g_theRiskDB->Get(g_theGameSettings->GetRisk())->m_lastBarbarianTurn) {
		return FALSE;
	}

	sint32 unitIndex = Barbarians::ChooseUnitType();
	sint32 d;
	MapPoint neighbor;
	BOOL tried[NOWHERE];
	sint32 triedCount = 0;

	sint32 maxBarbarians;
	if(fromGoodyHut) {
		maxBarbarians = g_rand->Next(g_theRiskDB->Get(g_theGameSettings->GetRisk())->m_maxHutBarbarians - 1) + 1;
	} else {
		maxBarbarians = g_rand->Next(g_theRiskDB->Get(g_theGameSettings->GetRisk())->m_maxSpontaneousBarbarians - 1) + 1;
	}

	sint32 count = 0;
	for(d = sint32(NORTH); d < sint32(NOWHERE); d++) {
		tried[d] = FALSE;
	}

	for(count = 0; count < maxBarbarians && triedCount < 8;) {
		sint32 use = g_rand->Next(NOWHERE);
		while(tried[use]) {
			use++;
			if(use >= NOWHERE)
				use = NORTH;
		}
	
		tried[use] = TRUE;
		triedCount++;
		if(point.GetNeighborPosition((WORLD_DIRECTION)use, neighbor)) {
			if(g_theWorld->IsLand(neighbor) &&
			   !g_theWorld->IsCity(neighbor)) {
				count++;
				Unit u = g_player[PLAYER_INDEX_VANDALS]->CreateUnit(unitIndex,
														   neighbor,
														   Unit(0),
														   FALSE,
														   CAUSE_NEW_ARMY_INITIAL);
				if(u.m_id == 0)
					count--;
			}
		}
	}
	return count != 0;
}

// The new barb placement method (adapted from the above,
// using only the algorithm previously used in MP):

#define k_MAX_BARBARIAN_TRIES 400

void Barbarians::BeginYear()
{
	const RiskRecord *risk = g_theRiskDB->Get(g_theGameSettings->GetRisk());
	if(g_turn->GetRound() < risk->m_firstBarbarianTurn)
		return;

	if(g_rand->Next(10000) < risk->m_barbarianChance * 10000) {
		MapPoint point;
		sint32 tries;
		sint32 p;

		for(tries = 0; tries < k_MAX_BARBARIAN_TRIES; tries++) {
			point.x = sint16(g_rand->Next(g_theWorld->GetXWidth()));
			point.y = sint16(g_rand->Next(g_theWorld->GetYHeight()));
			
			if (!g_theWorld->IsLand(point)) {
				continue;
			}
			
			for(p = 1; p < k_MAX_PLAYERS; p++) {
				if(g_player[p] && g_player[p]->IsVisible(point))
					break;
			}
			if(p >= k_MAX_PLAYERS) {
				break;
			}
		}
		if(tries < k_MAX_BARBARIAN_TRIES) {
			AddBarbarians(point, -1, FALSE);
		}
	}
}
