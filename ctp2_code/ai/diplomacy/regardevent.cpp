//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : Regard Event
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
// _DEBUG
// - Generate debug version when set.
//
// _SLOW_BUT_SAFE
// - Define 2 other symbols (PROJECTED_CHECK_START and PROJECTED_CHECK_END)
//   when set. But the defined symbols are never used, so this doesn't do
//   anything at all. This makes preprocessing and compilation slower, but
//   should be safe.
//
//----------------------------------------------------------------------------
//
// Modifications from the original Activision code:
//
//  - Hidden Nationality check added to AfterBattle by E 18 Nov 2005 if unit is
//    not Hidden Nationality then Regard Event is Logged
//
//----------------------------------------------------------------------------

#include "c3.h"
#include "RegardEvent.h"

#include "AgreementMatrix.h"
#include "ctpai.h"
#include "mapanalysis.h"

#include "Events.h"
#include "GameEventUser.h"
#include "Unit.h"
#include "StrDB.h"
#include "GameEventManager.h"

#include "AICause.h"
#include "Diplomat.h"
#include "player.h"
#include "ArmyData.h"
#include "ArmyPool.h"
#include "dynarr.h"
#include "World.h"
#include "Cell.h"
#include "TradeRouteData.h"
#include "newturncount.h"
#include "UnitRecord.h"
#include "UnitData.h"
#include "SlicObject.h"
#include "SlicEngine.h"

STDEHANDLER(KillUnitRegardEvent)
{
	Unit u;
	StringId strId;

	CAUSE_REMOVE_ARMY cause;
	PLAYER_INDEX killer;

	if (!args->GetUnit(0,u))
		return GEV_HD_Continue;

	if (!args->GetInt(0, (sint32&) cause))
		return GEV_HD_Continue;

	if (!args->GetPlayer(0, killer))
		return GEV_HD_Continue;

	Diplomat & diplomat = Diplomat::GetDiplomat(u.GetOwner());

	if (cause != CAUSE_REMOVE_ARMY_DIED_IN_ATTACK_ON_TOP &&
		cause != CAUSE_REMOVE_ARMY_ATTACKED)
		return GEV_HD_Continue;

	if (u.Flag(k_UDF_IS_IN_TRANSPORT))
	{
		return GEV_HD_Continue;
	}

	DPRINTF(k_DBG_AI, ("//	Kill Unit regard event\n"));  //EMOD added

	CellUnitList army;
	g_theWorld->GetArmy(u.RetPos(), army);
	bool not_at_war = (AgreementMatrix::s_agreements.TurnsAtWar(killer, u.GetOwner()) < 1);

	if (u.GetDBRec()->GetCivilian())
	{
		sint32 cost;

		if (not_at_war
		&& army.Num() == 1
		&& diplomat.GetCurrentDiplomacy(killer).GetAttackCivilianRegardCost(cost)
		&& g_theWorld->GetCell(u.RetPos())->GetCity().m_id == 0x0
		){
			g_theStringDB->GetStringID("REGARD_EVENT_ATTACKED_CIVILIANS", strId);
			diplomat.LogRegardEvent( killer,
				cost,
				REGARD_EVENT_MILITARY_SAFETY,
				strId);

			sint32 trust_cost;
			if (diplomat.GetCurrentDiplomacy(killer).GetAttackCivilianTrustCost(trust_cost))
			{
				SlicObject *so = new SlicObject("TANoKillCivilian");
				so->AddRecipient(killer);
				so->AddUnitRecord(u.GetType());
				so->AddPlayer(u.GetOwner());
				g_slicEngine->Execute(so);

				Diplomat::ApplyGlobalTrustChange(killer, trust_cost, "Committed the war crime of killing a civilian unit.");
			}

			diplomat.UpdateRegard(killer);
		}
	}

	return GEV_HD_Continue;
}

STDEHANDLER(BorderIncursionRegardEvent)
{
	sint32 army_owner;
	sint32 border_owner;
	sint32 cost;
	StringId strId;

	if(!args->GetPlayer(0, border_owner)) return GEV_HD_Continue;
	if(!args->GetPlayer(1, army_owner)) return GEV_HD_Continue;

	Diplomat & owner_diplomat = Diplomat::GetDiplomat(border_owner);

	sint32 turns_since_withdraw_troops_agreement = 	AgreementMatrix::s_agreements.
		GetAgreementDuration(border_owner, army_owner,PROPOSAL_REQUEST_WITHDRAW_TROOPS);
	if ((turns_since_withdraw_troops_agreement >= 20) &&
		(!AgreementMatrix::s_agreements.HasAgreement(border_owner, army_owner, PROPOSAL_TREATY_ALLIANCE)) &&
		(!owner_diplomat.GetBorderIncursionBy(army_owner)))
	{

		owner_diplomat.GetCurrentDiplomacy(army_owner).GetIncursionRegardCost(cost);
		g_theStringDB->GetStringID("REGARD_EVENT_PEACETIME_BORDER_INCURSION", strId);
		owner_diplomat.LogRegardEvent( army_owner,
			cost,
			REGARD_EVENT_MILITARY_SAFETY,
			strId);

		owner_diplomat.SetBorderIncursionBy(army_owner);

		owner_diplomat.LogViolationEvent(army_owner, PROPOSAL_REQUEST_WITHDRAW_TROOPS);
	}

	return GEV_HD_Continue;
}

STDEHANDLER(InvaderMovementRegardEvent)
{
	Army a;
	MapPoint from, to;

	if(!args->GetArmy(0, a)) return GEV_HD_Continue;
	if(!args->GetPos(0, from)) return GEV_HD_Continue;
	if(!args->GetPos(1, to)) return GEV_HD_Continue;

	sint32 old_cell_owner = g_theWorld->GetCell(from)->GetOwner();
	sint32 new_cell_owner = g_theWorld->GetCell(to)->GetOwner();
	sint32 army_owner = a->GetOwner();

	if (old_cell_owner == -1 ||
		new_cell_owner == -1 ||
		old_cell_owner == army_owner ||
		old_cell_owner != new_cell_owner )
	{
		return GEV_HD_Continue;
	}

	sint32 invader_movement_cost;
	StringId strId;

	if (g_player[new_cell_owner] &&
		!g_player[new_cell_owner]->HasContactWith(army_owner))
		return GEV_HD_Continue;

	if (AgreementMatrix::s_agreements.HasAgreement(new_cell_owner, army_owner, PROPOSAL_REQUEST_WITHDRAW_TROOPS))
	{
		ai::Agreement agreement =
			AgreementMatrix::s_agreements.GetAgreement(new_cell_owner, army_owner, PROPOSAL_REQUEST_WITHDRAW_TROOPS);
		if (agreement.start + 5 > NewTurnCount::GetCurrentRound())
			return GEV_HD_Continue;
	}

	if (!a->PlayerCanSee(new_cell_owner) || a->IsCivilian() )
		return GEV_HD_Continue;

	Diplomat & owner_diplomat = Diplomat::GetDiplomat(new_cell_owner);
	owner_diplomat.GetCurrentDiplomacy(army_owner).GetInvaderMovementRegardCost(invader_movement_cost);

	g_theStringDB->GetStringID("REGARD_EVENT_INVADER_MOVEMENT", strId);
	owner_diplomat.LogRegardEvent( army_owner,
			invader_movement_cost,
			REGARD_EVENT_MILITARY_SAFETY,
			strId);

	return GEV_HD_Continue;
}

STDEHANDLER(BattleAftermathRegardEvent)
{
	Army army;
	MapPoint pos;

	Unit ta;
	Unit td;
	sint32 attack_owner, defense_owner;

	args->GetArmy(0, army);

	if(!args->GetPos(0, pos))
		return GEV_HD_Continue;

	args->GetUnit(0, ta);
	args->GetUnit(1, td);

	if(!args->GetPlayer(0, attack_owner))
		return GEV_HD_Continue;

	if(!args->GetPlayer(1, defense_owner))
		return GEV_HD_Continue;

	return GEV_HD_Continue;
}

STDEHANDLER(StopPiracyRegardEvent)
{
	TradeRoute route;
	Army army;

	if(!args->GetTradeRoute(0, route))
		return GEV_HD_Continue;

	if(!route.IsActive()) // skip deactivated routes (only exist for drawing until revisited, see #256)
		return GEV_HD_Continue;
	    
	if(!args->GetArmy(0, army))
		return GEV_HD_Continue;

	PLAYER_INDEX army_owner = army->GetOwner();
	PLAYER_INDEX route_owner = route->GetOwner();

	Diplomat & route_diplomat = Diplomat::GetDiplomat(route_owner);
	route_diplomat.LogViolationEvent(army_owner, PROPOSAL_REQUEST_STOP_PIRACY);

	return GEV_HD_Continue;
}

STDEHANDLER(NeighborHatredRegardEvent)
{
	PLAYER_INDEX playerId;
	PLAYER_INDEX foreignerId;

	if (!args->GetPlayer(0, playerId))
		return GEV_HD_Continue;

	if (playerId == 0)
		return GEV_HD_Continue;

	Diplomat & diplomat = Diplomat::GetDiplomat(playerId);

	Assert(g_player[playerId]);
	if (g_player[playerId] == NULL)
		return GEV_HD_Continue;

	for (foreignerId = 1; foreignerId < CtpAi::s_maxPlayers; foreignerId++)
	{
		if (foreignerId == playerId)
			continue;

		if (g_player[foreignerId] == NULL ||
			g_player[foreignerId]->HasContactWith(playerId) == FALSE)
			continue;

		if (g_player[playerId]->HasContactWith(foreignerId) == FALSE)
			continue;

		if (MapAnalysis::GetMapAnalysis().ShareContinent(playerId, foreignerId) == false)
			continue;

		if (AgreementMatrix::s_agreements.
			HasAgreement(playerId, foreignerId, PROPOSAL_TREATY_DECLARE_WAR))
			continue;

		if (AgreementMatrix::s_agreements.
				HasAgreement(playerId, foreignerId, PROPOSAL_TREATY_ALLIANCE) ||
			AgreementMatrix::s_agreements.
				HasAgreement(playerId, foreignerId, PROPOSAL_TREATY_MILITARY_PACT))
			continue;

		sint32 cost;
		diplomat.GetCurrentDiplomacy(foreignerId).GetShareContinentRegardCost(cost);

		if (AgreementMatrix::s_agreements.
			HasAgreement(playerId, foreignerId, PROPOSAL_TREATY_PEACE))
		{
			cost /= 2;
		}

		if (AgreementMatrix::s_agreements.
			HasAgreement(playerId, foreignerId, PROPOSAL_TREATY_TRADE_PACT))
		{
			cost /= 2;
		}

		if (AgreementMatrix::s_agreements.
			HasAgreement(playerId, foreignerId, PROPOSAL_TREATY_RESEARCH_PACT))
		{
			cost /= 2;
		}

		StringId strId;
		g_theStringDB->GetStringID("REGARD_EVENT_ENEMY_SHARES_CONTINENT", strId);
		diplomat.LogRegardEvent( foreignerId,
			cost,
			REGARD_EVENT_MILITARY_POWER,
			strId,
			50);
	}

	return GEV_HD_Continue;
}

STDEHANDLER(CaptureCityRegardEvent)
{
	Unit city;
	sint32 newOwner;
	sint32 cause;
	MapPoint pos;
	sint32 originalOwner;

	if(!args->GetCity(0, city))
		return GEV_HD_Continue;

	if(!args->GetPlayer(0, newOwner))
		return GEV_HD_Continue;

	if(!args->GetInt(0, cause))
		return GEV_HD_Continue;

	originalOwner = city.GetOwner();

	if ((CAUSE_REMOVE_CITY)cause != CAUSE_REMOVE_CITY_ATTACK)
		return GEV_HD_Continue;

	Diplomat & city_diplomat = Diplomat::GetDiplomat(originalOwner);

	StringId strId;
	sint32 cost;

	g_theStringDB->GetStringID("REGARD_EVENT_CAPTURED_CITY", strId);
	city_diplomat.GetCurrentDiplomacy(newOwner).GetTakeCityRegardCost(cost);
	city_diplomat.LogRegardEvent( newOwner, cost, REGARD_EVENT_MILITARY_POWER, strId);

	city_diplomat.LogViolationEvent(newOwner, PROPOSAL_TREATY_CEASEFIRE);

	return GEV_HD_Continue;
}

STDEHANDLER(InciteRevolution_RegardEvent)
{
	Unit unit;
	Unit city;

	if (!args->GetUnit(0,unit))
		return GEV_HD_Continue;

	if (!args->GetCity(0,city))
		return GEV_HD_Continue;

	PLAYER_INDEX attack_owner = unit.GetOwner();
	PLAYER_INDEX city_owner = city.GetOwner();

	Diplomat & city_diplomat = Diplomat::GetDiplomat(city_owner);

	city_diplomat.SetColdwarAttack(attack_owner, (sint16) NewTurnCount::GetCurrentRound());

	sint32 cost;
	city_diplomat.GetCurrentDiplomacy(attack_owner).GetInciteRevolutionRegardCost(cost);

	StringId strId;
	g_theStringDB->GetStringID("REGARD_EVENT_ENEMY_INCITED_REVOLUTION", strId);
	city_diplomat.LogRegardEvent( attack_owner,
			cost,
			REGARD_EVENT_MILITARY_SAFETY,
			strId);

	city_diplomat.LogViolationEvent(attack_owner, PROPOSAL_TREATY_MILITARY_PACT);
	city_diplomat.LogViolationEvent(attack_owner, PROPOSAL_TREATY_ALLIANCE);

	return GEV_HD_Continue;
}

STDEHANDLER(AssassinateRulerUnit_RegardEvent)
{
	Unit unit;
	Unit city;

	if (!args->GetUnit(0,unit))
		return GEV_HD_Continue;

	if (!args->GetCity(0,city))
		return GEV_HD_Continue;

	PLAYER_INDEX attack_owner = unit.GetOwner();
	PLAYER_INDEX city_owner = city.GetOwner();

	Diplomat & city_diplomat = Diplomat::GetDiplomat(city_owner);

	city_diplomat.SetColdwarAttack(attack_owner, (sint16) NewTurnCount::GetCurrentRound());

	sint32 cost;
	city_diplomat.GetCurrentDiplomacy(attack_owner).GetAssassinateRulerRegardCost(cost);

	StringId strId;
	g_theStringDB->GetStringID("REGARD_EVENT_ENEMY_ASSASSINATED_RULER", strId);
	city_diplomat.LogRegardEvent( attack_owner,
			cost,
			REGARD_EVENT_MILITARY_SAFETY,
			strId);

	city_diplomat.LogViolationEvent(attack_owner, PROPOSAL_TREATY_MILITARY_PACT);
	city_diplomat.LogViolationEvent(attack_owner, PROPOSAL_TREATY_ALLIANCE);

	return GEV_HD_Continue;
}

STDEHANDLER(MakeFranchise_RegardEvent)
{
	Unit unit;
	Unit city;

	if (!args->GetUnit(0,unit))
		return GEV_HD_Continue;

	if (!args->GetCity(0,city))
		return GEV_HD_Continue;

	PLAYER_INDEX attack_owner = unit.GetOwner();
	PLAYER_INDEX city_owner = city.GetOwner();

	Diplomat & city_diplomat = Diplomat::GetDiplomat(city_owner);

	city_diplomat.SetColdwarAttack(attack_owner, (sint16) NewTurnCount::GetCurrentRound());

	sint32 cost;
	city_diplomat.GetCurrentDiplomacy(attack_owner).GetFranchiseCityRegardCost(cost);

	StringId strId;
	g_theStringDB->GetStringID("REGARD_EVENT_ENEMY_FRANCHISED_CITY", strId);
	city_diplomat.LogRegardEvent( attack_owner,
			cost,
			REGARD_EVENT_GOLD,
			strId);


	return GEV_HD_Continue;
}

STDEHANDLER(PlantNukeUnit_RegardEvent)
{
	Unit unit;
	Unit city;

	if (!args->GetUnit(0,unit))
		return GEV_HD_Continue;

	if (!args->GetCity(0,city))
		return GEV_HD_Continue;

	PLAYER_INDEX attack_owner = unit.GetOwner();
	PLAYER_INDEX city_owner = city.GetOwner();

	Diplomat::GetDiplomat(attack_owner).SetHasLaunchedNukes(true);

	Diplomat & city_diplomat = Diplomat::GetDiplomat(city_owner);

	sint32 cost;
	city_diplomat.GetCurrentDiplomacy(attack_owner).GetPlantNukeRegardCost(cost);

	StringId strId;
	g_theStringDB->GetStringID("REGARD_EVENT_ENEMY_PLANTED_NUKE", strId);
	city_diplomat.LogRegardEvent( attack_owner,
			cost,
			REGARD_EVENT_MILITARY_SAFETY,
			strId);

	city_diplomat.LogViolationEvent(attack_owner, PROPOSAL_TREATY_CEASEFIRE);

	sint32 trust_cost;
	if(city_diplomat.GetCurrentDiplomacy(attack_owner).GetUsedNukesTrustCost(trust_cost)
	&&!city_diplomat.HasLaunchedNukes()
	){
		Diplomat::ApplyGlobalTrustChange(attack_owner, trust_cost*3, "Committed the war crime of planting a nuke in a city first.");
	}

	return GEV_HD_Continue;
}

STDEHANDLER(EnslaveSettler_RegardEvent)
{
	Army a;
	Unit slaver, settler;

	if(!args->GetArmy(0, a))
		return GEV_HD_Continue;

	if(!args->GetUnit(0, slaver))
		return GEV_HD_Continue;

	if(!args->GetUnit(1, settler))
		return GEV_HD_Continue;

	PLAYER_INDEX attack_owner = slaver.GetOwner();
	PLAYER_INDEX settler_owner = settler.GetOwner();

	Diplomat & settler_diplomat = Diplomat::GetDiplomat(settler_owner);

	settler_diplomat.SetColdwarAttack(attack_owner, (sint16)NewTurnCount::GetCurrentRound());

	sint32 cost;
	settler_diplomat.GetCurrentDiplomacy(attack_owner).GetEnslaveSettlerRegardCost(cost);

	StringId strId;
	g_theStringDB->GetStringID("REGARD_EVENT_ENEMY_ENSLAVED_SETTLER", strId);
	settler_diplomat.LogRegardEvent( attack_owner,
			cost,
			REGARD_EVENT_MILITARY_SAFETY,
			strId);

	settler_diplomat.LogViolationEvent(attack_owner, PROPOSAL_TREATY_MILITARY_PACT);
	settler_diplomat.LogViolationEvent(attack_owner, PROPOSAL_TREATY_TRADE_PACT);
	settler_diplomat.LogViolationEvent(attack_owner, PROPOSAL_TREATY_ALLIANCE);

	return GEV_HD_Continue;
}

STDEHANDLER(SlaveRaidCity_RegardEvent)
{
	Unit unit;
	Unit city;

	if (!args->GetUnit(0,unit))
		return GEV_HD_Continue;

	if (!args->GetCity(0,city))
		return GEV_HD_Continue;

	PLAYER_INDEX attack_owner = unit.GetOwner();
	PLAYER_INDEX city_owner = city.GetOwner();

	Diplomat & city_diplomat = Diplomat::GetDiplomat(city_owner);

	city_diplomat.SetColdwarAttack(attack_owner, (sint16)NewTurnCount::GetCurrentRound());

	// cost should be variable not standard each time
	sint32 cost;
	city_diplomat.GetCurrentDiplomacy(attack_owner).GetSlaveRaidRegardCost(cost);

	StringId strId;
	g_theStringDB->GetStringID("REGARD_EVENT_ENEMY_SLAVE_RAIDED_CITY", strId);
	city_diplomat.LogRegardEvent( attack_owner,
			cost,
			REGARD_EVENT_MILITARY_SAFETY,
			strId);

	city_diplomat.LogViolationEvent(attack_owner, PROPOSAL_TREATY_MILITARY_PACT);
	city_diplomat.LogViolationEvent(attack_owner, PROPOSAL_TREATY_TRADE_PACT);
	city_diplomat.LogViolationEvent(attack_owner, PROPOSAL_TREATY_ALLIANCE);

	return GEV_HD_Continue;
}

STDEHANDLER(EmbargoRegardEvent)
{
	PLAYER_INDEX playerId;
	PLAYER_INDEX foreignerId;

	if (!args->GetPlayer(0, playerId))
		return GEV_HD_Continue;

	if (playerId == 0)
		return GEV_HD_Continue;

	Diplomat & diplomat = Diplomat::GetDiplomat(playerId);

	Assert(g_player[playerId]);
	if (g_player[playerId] == NULL)
		return GEV_HD_Continue;

	for (foreignerId = 1; foreignerId < CtpAi::s_maxPlayers; foreignerId++)
	{

		if (foreignerId == playerId)
			continue;

		if (g_player[foreignerId] == NULL ||
			g_player[foreignerId]->HasContactWith(playerId) == FALSE)
			continue;

		if (g_player[playerId]->HasContactWith(foreignerId) == FALSE)
			continue;

		if (Diplomat::GetDiplomat(foreignerId).GetEmbargo(playerId) == false)
			continue;


		sint32 regard_cost = 0;
		if (diplomat.GetCurrentDiplomacy(foreignerId).GetEmbargoTradeRegardCost(regard_cost))
		{
			StringId strId;
			g_theStringDB->GetStringID("REGARD_EVENT_EMBARGO_TRADE", strId);
			diplomat.LogRegardEvent(foreignerId,
				regard_cost,
				REGARD_EVENT_GOLD,
				strId,
				1);
		}
	}

	return GEV_HD_Continue;
}

STDEHANDLER(Lawsuit_RegardEvent)
{
	Unit unit;
	MapPoint pos;
	Army army;

	if (!args->GetArmy(0,army))
		return GEV_HD_Continue;

	if (!args->GetUnit(0,unit))
		return GEV_HD_Continue;

	if (!args->GetPos(0,pos))
		return GEV_HD_Continue;

	PLAYER_INDEX attack_owner = unit.GetOwner();

	Cell *cell = g_theWorld->GetCell(pos);
	PLAYER_INDEX victim = cell->AccessUnit(0)->GetOwner();

	Diplomat & victim_diplomat = Diplomat::GetDiplomat(victim);

	victim_diplomat.SetColdwarAttack(victim, (sint16) NewTurnCount::GetCurrentRound());

	sint32 cost;
	victim_diplomat.GetCurrentDiplomacy(attack_owner).GetLawsuitRegardCost(cost);

	StringId strId;
	g_theStringDB->GetStringID("REGARD_EVENT_ENEMY_LAWSUIT", strId);
	victim_diplomat.LogRegardEvent( attack_owner,
			cost,
			REGARD_EVENT_PRODUCTION,
			strId);


	return GEV_HD_Continue;
}

STDEHANDLER(ExpelUnits_RegardEvent)
{
	Army army;
	MapPoint pos;

	if (!args->GetArmy(0,army))
		return GEV_HD_Continue;

	if (!args->GetPos(0,pos))
		return GEV_HD_Continue;

	PLAYER_INDEX attack_owner = army.GetOwner();

	Cell *cell = g_theWorld->GetCell(pos);
	if(!cell->UnitArmy())
		return GEV_HD_Continue;

	PLAYER_INDEX victim = cell->AccessUnit(0)->GetOwner();

	Diplomat & victim_diplomat = Diplomat::GetDiplomat(victim);

	sint32 cost;
	victim_diplomat.GetCurrentDiplomacy(attack_owner).GetExpelUnitsRegardCost(cost);

	StringId strId;
	g_theStringDB->GetStringID("REGARD_EVENT_ENEMY_EXPELLED_UNITS", strId);
	victim_diplomat.LogRegardEvent( attack_owner,
			cost,
			REGARD_EVENT_MILITARY_POWER,
			strId);

	return GEV_HD_Continue;
}

STDEHANDLER(NukeCityUnit_RegardEvent)
{
	Unit unit;
	Unit city;

	if (!args->GetUnit(0,unit))
		return GEV_HD_Continue;

	if (!args->GetCity(0,city))
		return GEV_HD_Continue;

	PLAYER_INDEX attack_owner = unit.GetOwner();
	PLAYER_INDEX city_owner = city.GetOwner();

	Diplomat::GetDiplomat(attack_owner).SetHasLaunchedNukes(true);

	Diplomat & city_diplomat = Diplomat::GetDiplomat(city_owner);

	sint32 cost;
	city_diplomat.GetCurrentDiplomacy(attack_owner).GetNukeCityRegardCost(cost);

	StringId strId;
	g_theStringDB->GetStringID("REGARD_EVENT_ENEMY_NUKED_CITY", strId);
	city_diplomat.LogRegardEvent( attack_owner,
			cost,
			REGARD_EVENT_MILITARY_SAFETY,
			strId);

	sint32 trust_cost = 0;
	if(city_diplomat.GetCurrentDiplomacy(attack_owner).GetUsedNukesTrustCost(trust_cost)
	&&!city_diplomat.HasLaunchedNukes()
	){
		Diplomat::ApplyGlobalTrustChange(attack_owner, trust_cost*2, "Committed the war crime of nuking a city first.");
	}

	city_diplomat.LogViolationEvent(attack_owner, PROPOSAL_TREATY_CEASEFIRE);

	return GEV_HD_Continue;
}

STDEHANDLER(NukeLocationUnit_RegardEvent)
{
	Unit unit;
	MapPoint pos;

	if(!args->GetUnit(0, unit)) return GEV_HD_Continue;
	if(!args->GetPos(0, pos)) return GEV_HD_Continue;

	PLAYER_INDEX attack_owner = unit.GetOwner();
	PLAYER_INDEX pos_owner;

	Diplomat::GetDiplomat(attack_owner).SetHasLaunchedNukes(true);

	CellUnitList army;
	g_theWorld->GetArmy(pos, army);
	if (army.Num() > 0)
		pos_owner = army.GetOwner();
	else
		pos_owner = g_theWorld->GetOwner(pos);

	if (pos_owner == -1)
		return GEV_HD_Continue;

	Diplomat & pos_diplomat = Diplomat::GetDiplomat(pos_owner);

	sint32 cost;
	pos_diplomat.GetCurrentDiplomacy(attack_owner).GetNukeCityRegardCost(cost);

	StringId strId;
	g_theStringDB->GetStringID("REGARD_EVENT_ENEMY_NUKED_CITY", strId);
	pos_diplomat.LogRegardEvent( attack_owner,
			cost,
			REGARD_EVENT_MILITARY_SAFETY,
			strId);

	pos_diplomat.LogViolationEvent(attack_owner, PROPOSAL_TREATY_CEASEFIRE);

	sint32 trust_cost = 0;
	if(pos_diplomat.GetCurrentDiplomacy(attack_owner).GetUsedNukesTrustCost(trust_cost)
	&&!pos_diplomat.HasLaunchedNukes()
	){
		Diplomat::ApplyGlobalTrustChange(attack_owner, trust_cost, "Committed the war crime of using nuclear weapons first.");
	}

	return GEV_HD_Continue;
}

STDEHANDLER(UndergroundRailwayUnit_RegardEvent)
{
	Unit unit;
	Unit city;

	if (!args->GetUnit(0,unit))
		return GEV_HD_Continue;

	if (!args->GetCity(0,city))
		return GEV_HD_Continue;

	PLAYER_INDEX attack_owner = unit.GetOwner();
	PLAYER_INDEX city_owner = city.GetOwner();

	Diplomat & city_diplomat = Diplomat::GetDiplomat(city_owner);

	city_diplomat.SetColdwarAttack(attack_owner, (sint16)NewTurnCount::GetCurrentRound());

	sint32 cost;
	city_diplomat.GetCurrentDiplomacy(attack_owner).GetUndergroundRailwayRegardCost(cost);

	StringId strId;
	g_theStringDB->GetStringID("REGARD_EVENT_ENEMY_FREED_SLAVES", strId);
	city_diplomat.LogRegardEvent( attack_owner,
			cost,
			REGARD_EVENT_MILITARY_SAFETY,
			strId);

	city_diplomat.LogViolationEvent(attack_owner, PROPOSAL_TREATY_MILITARY_PACT);
	city_diplomat.LogViolationEvent(attack_owner, PROPOSAL_TREATY_TRADE_PACT);
	city_diplomat.LogViolationEvent(attack_owner, PROPOSAL_TREATY_ALLIANCE);

	return GEV_HD_Continue;
}

STDEHANDLER(InciteUprisingUnit_RegardEvent)
{
	Unit unit;
	Unit city;

	if (!args->GetUnit(0,unit))
		return GEV_HD_Continue;

	if (!args->GetCity(0,city))
		return GEV_HD_Continue;

	PLAYER_INDEX attack_owner = unit.GetOwner();
	PLAYER_INDEX city_owner = city.GetOwner();

	Diplomat & city_diplomat = Diplomat::GetDiplomat(city_owner);

	city_diplomat.SetColdwarAttack(attack_owner, (sint16)NewTurnCount::GetCurrentRound());

	sint32 cost;
	city_diplomat.GetCurrentDiplomacy(attack_owner).GetInciteUprisingRegardCost(cost);

	StringId strId;
	g_theStringDB->GetStringID("REGARD_EVENT_ENEMY_INCITED_UPRISING", strId);
	city_diplomat.LogRegardEvent( attack_owner,
			cost,
			REGARD_EVENT_MILITARY_SAFETY,
			strId);

	city_diplomat.LogViolationEvent(attack_owner, PROPOSAL_TREATY_MILITARY_PACT);
	city_diplomat.LogViolationEvent(attack_owner, PROPOSAL_TREATY_TRADE_PACT);
	city_diplomat.LogViolationEvent(attack_owner, PROPOSAL_TREATY_ALLIANCE);

	return GEV_HD_Continue;
}

STDEHANDLER(BioInfectCityUnit_RegardEvent)
{
	Unit unit;
	Unit city;

	if (!args->GetUnit(0,unit))
		return GEV_HD_Continue;

	if (!args->GetCity(0,city))
		return GEV_HD_Continue;

	PLAYER_INDEX attack_owner = unit.GetOwner();
	PLAYER_INDEX city_owner = city.GetOwner();

	Diplomat & city_diplomat = Diplomat::GetDiplomat(city_owner);

	city_diplomat.SetColdwarAttack(attack_owner, (sint16)NewTurnCount::GetCurrentRound());

	sint32 cost;
	city_diplomat.GetCurrentDiplomacy(attack_owner).GetBioInfectedCityRegardCost(cost);

	StringId strId;
	g_theStringDB->GetStringID("REGARD_EVENT_ENEMY_BIO_INFECTED_CITY", strId);
	city_diplomat.LogRegardEvent( attack_owner,
			cost,
			REGARD_EVENT_MILITARY_SAFETY,
			strId);

	city_diplomat.LogViolationEvent(attack_owner, PROPOSAL_TREATY_MILITARY_PACT);
	city_diplomat.LogViolationEvent(attack_owner, PROPOSAL_TREATY_TRADE_PACT);
	city_diplomat.LogViolationEvent(attack_owner, PROPOSAL_TREATY_ALLIANCE);

	return GEV_HD_Continue;
}

STDEHANDLER(PlagueCityUnit_RegardEvent)
{
	Unit unit;
	Unit city;

	if (!args->GetUnit(0,unit))
		return GEV_HD_Continue;

	if (!args->GetCity(0,city))
		return GEV_HD_Continue;

	PLAYER_INDEX attack_owner = unit.GetOwner();
	PLAYER_INDEX city_owner = city.GetOwner();

	Diplomat & city_diplomat = Diplomat::GetDiplomat(city_owner);

	city_diplomat.SetColdwarAttack(attack_owner, (sint16) NewTurnCount::GetCurrentRound());

	sint32 cost;
	city_diplomat.GetCurrentDiplomacy(attack_owner).GetPlagueCityRegardCost(cost);

	StringId strId;
	g_theStringDB->GetStringID("REGARD_EVENT_ENEMY_PLAGUED_CITY", strId);
	city_diplomat.LogRegardEvent( attack_owner,
			cost,
			REGARD_EVENT_MILITARY_SAFETY,
			strId);

	city_diplomat.LogViolationEvent(attack_owner, PROPOSAL_TREATY_MILITARY_PACT);
	city_diplomat.LogViolationEvent(attack_owner, PROPOSAL_TREATY_TRADE_PACT);
	city_diplomat.LogViolationEvent(attack_owner, PROPOSAL_TREATY_ALLIANCE);

	return GEV_HD_Continue;
}

STDEHANDLER(NanoInfectCityUnit_RegardEvent)
{
	Unit unit;
	Unit city;

	if (!args->GetUnit(0,unit))
		return GEV_HD_Continue;

	if (!args->GetCity(0,city))
		return GEV_HD_Continue;

	PLAYER_INDEX attack_owner = unit.GetOwner();
	PLAYER_INDEX city_owner = city.GetOwner();

	Diplomat & city_diplomat = Diplomat::GetDiplomat(city_owner);

	city_diplomat.SetColdwarAttack(attack_owner, (sint16) NewTurnCount::GetCurrentRound());

	sint32 cost;
	city_diplomat.GetCurrentDiplomacy(attack_owner).GetNanoInfectCityRegardCost(cost);

	StringId strId;
	g_theStringDB->GetStringID("REGARD_EVENT_ENEMY_NANO_INFECTED_CITY", strId);
	city_diplomat.LogRegardEvent( attack_owner,
			cost,
			REGARD_EVENT_MILITARY_SAFETY,
			strId);

	city_diplomat.LogViolationEvent(attack_owner, PROPOSAL_TREATY_MILITARY_PACT);
	city_diplomat.LogViolationEvent(attack_owner, PROPOSAL_TREATY_TRADE_PACT);
	city_diplomat.LogViolationEvent(attack_owner, PROPOSAL_TREATY_ALLIANCE);

	return GEV_HD_Continue;
}

STDEHANDLER(ConvertCityUnit_RegardEvent)
{
	Unit unit;
	Unit city;

	if (!args->GetUnit(0,unit))
		return GEV_HD_Continue;

	if (!args->GetCity(0,city))
		return GEV_HD_Continue;

	PLAYER_INDEX attack_owner = unit.GetOwner();
	PLAYER_INDEX city_owner = city.GetOwner();

	Diplomat & city_diplomat = Diplomat::GetDiplomat(city_owner);

	city_diplomat.SetColdwarAttack(attack_owner, (sint16) NewTurnCount::GetCurrentRound());

	sint32 cost;
	city_diplomat.GetCurrentDiplomacy(attack_owner).GetConvertCityRegardCost(cost);

	StringId strId;
	g_theStringDB->GetStringID("REGARD_EVENT_ENEMY_CONVERTED_CITY", strId);
	city_diplomat.LogRegardEvent( attack_owner,
			cost,
			REGARD_EVENT_MILITARY_SAFETY,
			strId);

	city_diplomat.LogViolationEvent(attack_owner, PROPOSAL_TREATY_MILITARY_PACT);
	city_diplomat.LogViolationEvent(attack_owner, PROPOSAL_TREATY_TRADE_PACT);
	city_diplomat.LogViolationEvent(attack_owner, PROPOSAL_TREATY_ALLIANCE);

	return GEV_HD_Continue;
}

STDEHANDLER(IndulgenceSaleMade_RegardEvent)
{
	Unit unit;
	Unit city;

	if (!args->GetUnit(0,unit))
		return GEV_HD_Continue;

	if (!args->GetCity(0,city))
		return GEV_HD_Continue;

	PLAYER_INDEX attack_owner = unit.GetOwner();
	PLAYER_INDEX city_owner = city.GetOwner();

	Diplomat & city_diplomat = Diplomat::GetDiplomat(city_owner);

	city_diplomat.SetColdwarAttack(attack_owner, (sint16) NewTurnCount::GetCurrentRound());

	sint32 cost;
	city_diplomat.GetCurrentDiplomacy(attack_owner).GetConvertCityRegardCost(cost);

	StringId strId;
	g_theStringDB->GetStringID("REGARD_EVENT_ENEMY_CONVERTED_CITY", strId);
	city_diplomat.LogRegardEvent( attack_owner,
			cost,
			REGARD_EVENT_MILITARY_SAFETY,
			strId);

	city_diplomat.LogViolationEvent(attack_owner, PROPOSAL_TREATY_MILITARY_PACT);
	city_diplomat.LogViolationEvent(attack_owner, PROPOSAL_TREATY_TRADE_PACT);
	city_diplomat.LogViolationEvent(attack_owner, PROPOSAL_TREATY_ALLIANCE);

	return GEV_HD_Continue;
}

STDEHANDLER(CreateParkUnit_RegardEvent)
{
	Unit unit;
	Unit city;

	if (!args->GetUnit(0,unit))
		return GEV_HD_Continue;

	if (!args->GetCity(0,city))
		return GEV_HD_Continue;

	PLAYER_INDEX attack_owner = unit.GetOwner();
	PLAYER_INDEX city_owner = city.GetOwner();

	Diplomat::GetDiplomat(attack_owner).HasLaunchedNanoAttack(true);

	Diplomat & city_diplomat = Diplomat::GetDiplomat(city_owner);

	sint32 cost;
	city_diplomat.GetCurrentDiplomacy(attack_owner).GetCreateParkRegardCost(cost);

	StringId strId;
	g_theStringDB->GetStringID("REGARD_EVENT_ENEMY_CREATED_PARK", strId);
	city_diplomat.LogRegardEvent( attack_owner,
			cost,
			REGARD_EVENT_MILITARY_SAFETY,
			strId);

	sint32 trust_cost = 0;
	if(city_diplomat.GetCurrentDiplomacy(attack_owner).GetUsedNukesTrustCost(trust_cost)
	&&!city_diplomat.HasLaunchedNanoAttack()
	){
		Diplomat::ApplyGlobalTrustChange(attack_owner, trust_cost*2, "Committed the war crime of nano-attacking a city first.");
	}

	city_diplomat.LogViolationEvent(attack_owner, PROPOSAL_TREATY_CEASEFIRE);

	return GEV_HD_Continue;
}

STDEHANDLER(PillageUnit_RegardEvent)
{
	Unit unit;

	if (!args->GetUnit(0,unit))
		return GEV_HD_Continue;

	PLAYER_INDEX attack_owner = unit.GetOwner();
	PLAYER_INDEX victim = g_theWorld->GetOwner(unit.RetPos());

	if (victim == -1)
		return GEV_HD_Continue;

	if (victim == attack_owner)
		return GEV_HD_Continue;

	Diplomat & victim_diplomat = Diplomat::GetDiplomat(victim);

	sint32 cost;
	victim_diplomat.GetCurrentDiplomacy(attack_owner).GetPillageRegardCost(cost);

	StringId strId;
	g_theStringDB->GetStringID("REGARD_EVENT_ENEMY_PILLAGED", strId);
	victim_diplomat.LogRegardEvent( attack_owner,
			cost,
			REGARD_EVENT_MILITARY_SAFETY,
			strId);

	// Modified by E - Hidden Nationality check added by E 19 Nov 2005 - if unit is not Hidden Nationality then Regard Event is Logged
	if(!(unit.GetDBRec()->GetSneakPillage() == true))
	{
		//; || (!(td.GetDBRec()->GetHiddenNationality() == true));
		//original code
		victim_diplomat.LogViolationEvent(attack_owner, PROPOSAL_TREATY_CEASEFIRE);
		//  end original code
	}
	// end EMOD

	return GEV_HD_Continue;
}

STDEHANDLER(InjoinUnit_RegardEvent)
{
	Unit unit;
	Unit city;

	if (!args->GetUnit(0,unit))
		return GEV_HD_Continue;

	if (!args->GetCity(0,city))
		return GEV_HD_Continue;

	PLAYER_INDEX attack_owner = unit.GetOwner();
	PLAYER_INDEX city_owner = city.GetOwner();

	Diplomat & city_diplomat = Diplomat::GetDiplomat(city_owner);

	city_diplomat.SetColdwarAttack(attack_owner, (sint16) NewTurnCount::GetCurrentRound());

	sint32 cost;
	city_diplomat.GetCurrentDiplomacy(attack_owner).GetInjoinCityRegardCost(cost);

	StringId strId;
	g_theStringDB->GetStringID("REGARD_EVENT_ENEMY_INJOINED_CITY", strId);
	city_diplomat.LogRegardEvent( attack_owner,
			cost,
			REGARD_EVENT_PRODUCTION,
			strId);

	city_diplomat.LogViolationEvent(attack_owner, PROPOSAL_TREATY_TRADE_PACT);
	city_diplomat.LogViolationEvent(attack_owner, PROPOSAL_TREATY_RESEARCH_PACT);
	city_diplomat.LogViolationEvent(attack_owner, PROPOSAL_TREATY_ALLIANCE);

	return GEV_HD_Continue;
}

void RegardEventCallbacks::AddCallbacks()
{

	g_gevManager->AddCallback(GEV_KillUnit,
							  GEV_PRI_Pre,
							  &s_KillUnitRegardEvent);

	g_gevManager->AddCallback(GEV_BorderIncursion,
							  GEV_PRI_Pre,
							  &s_BorderIncursionRegardEvent);







	g_gevManager->AddCallback(GEV_MoveUnits,
							  GEV_PRI_Pre,
							  &s_InvaderMovementRegardEvent);

	g_gevManager->AddCallback(GEV_BattleAftermath,
							  GEV_PRI_Pre,
							  &s_BattleAftermathRegardEvent);

	g_gevManager->AddCallback(GEV_SetPiratingArmy,
							  GEV_PRI_Pre,
							  &s_StopPiracyRegardEvent);

	g_gevManager->AddCallback(GEV_CaptureCity,
							  GEV_PRI_Pre,
							  &s_CaptureCityRegardEvent);

	g_gevManager->AddCallback(GEV_BeginTurn,
							  GEV_PRI_Pre,
							  &s_NeighborHatredRegardEvent);

	g_gevManager->AddCallback(GEV_InciteRevolutionUnit,
							  GEV_PRI_Pre,
							  &s_InciteRevolution_RegardEvent);

	g_gevManager->AddCallback(GEV_AssassinateRulerUnit,
							  GEV_PRI_Pre,
							  &s_AssassinateRulerUnit_RegardEvent);

	g_gevManager->AddCallback(GEV_MakeFranchise,
							  GEV_PRI_Pre,
							  &s_MakeFranchise_RegardEvent);

	g_gevManager->AddCallback(GEV_PlantNukeUnit,
							  GEV_PRI_Pre,
							  &s_PlantNukeUnit_RegardEvent);

	g_gevManager->AddCallback(GEV_SlaveRaidCity,
							  GEV_PRI_Pre,
							  &s_SlaveRaidCity_RegardEvent);

	g_gevManager->AddCallback(GEV_Lawsuit,
							  GEV_PRI_Pre,
							  &s_Lawsuit_RegardEvent);

	g_gevManager->AddCallback(GEV_ExpelUnits,
							  GEV_PRI_Pre,
							  &s_ExpelUnits_RegardEvent);

	g_gevManager->AddCallback(GEV_NukeCityUnit,
							  GEV_PRI_Pre,
							  &s_NukeCityUnit_RegardEvent);

	g_gevManager->AddCallback(GEV_NukeLocationUnit,
							  GEV_PRI_Pre,
							  &s_NukeLocationUnit_RegardEvent);

	g_gevManager->AddCallback(GEV_EnslaveSettler,
							  GEV_PRI_Pre,
							  &s_EnslaveSettler_RegardEvent);

	g_gevManager->AddCallback(GEV_UndergroundRailwayUnit,
							  GEV_PRI_Pre,
							  &s_UndergroundRailwayUnit_RegardEvent);

	g_gevManager->AddCallback(GEV_InciteUprisingUnit,
							  GEV_PRI_Pre,
							  &s_InciteUprisingUnit_RegardEvent);

	g_gevManager->AddCallback(GEV_BioInfectCityUnit,
							  GEV_PRI_Pre,
							  &s_BioInfectCityUnit_RegardEvent);

	g_gevManager->AddCallback(GEV_PlagueCityUnit,
							  GEV_PRI_Pre,
							  &s_PlagueCityUnit_RegardEvent);

	g_gevManager->AddCallback(GEV_NanoInfectCityUnit,
							  GEV_PRI_Pre,
							  &s_NanoInfectCityUnit_RegardEvent);

	g_gevManager->AddCallback(GEV_ConvertCityUnit,
							  GEV_PRI_Pre,
							  &s_ConvertCityUnit_RegardEvent);

	g_gevManager->AddCallback(GEV_IndulgenceSaleMade,
							  GEV_PRI_Pre,
							  &s_IndulgenceSaleMade_RegardEvent);

	g_gevManager->AddCallback(GEV_CreateParkUnit,
							  GEV_PRI_Pre,
							  &s_CreateParkUnit_RegardEvent);

	g_gevManager->AddCallback(GEV_PillageUnit,
							  GEV_PRI_Pre,
							  &s_PillageUnit_RegardEvent);

	g_gevManager->AddCallback(GEV_InjoinUnit,
							  GEV_PRI_Pre,
							  &s_InjoinUnit_RegardEvent);

	g_gevManager->AddCallback(GEV_BeginTurn,
							  GEV_PRI_Pre,
							  &s_EmbargoRegardEvent);
}
