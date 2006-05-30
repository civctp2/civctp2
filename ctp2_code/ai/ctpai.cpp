//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : Main Ai File
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
//----------------------------------------------------------------------------
//
// Modifications from the original Activision code:
//
// - Add the same action to escort Transports as it exists for Settlers - Calvitix
// - Allow army groups (size > 1) to escort settlers or transports - Calvitix
// - Place Tile Improvement every turn instead of every 5 turns - Calvitix
// - Add goals (Chokepoint - Goodyhuts) if there is only a third of maxeval 
//   (and not just when there isn't anymore (if one goal remain and isn't 
//   satisfied, it can freeze all the goals of this type)) - Calvitix
// - AI now improves every turn its city tiles. - Calvitex
// - Set explore resolution (an Explore goal every 4 tiles is a good compromise)
// - Undid the last change as I still think it reduces game speed maybe 
//   something else has to be improved to avaoid the long pauses caused 
//   by some AIs. - Martin Gühmann
// - force to move the transport units out of city (12 units isn't much, and 
//   their is problems when a group want to enter in a transport that is in 
//   town(example : 5 - units group cannot enter transport if it is in a city 
//   and with 7 other garrison units(based on makeRoomForNewUnits code) - Calvitix
// - Add UngroupGarrison method (to ungroup units blocked by garrison 
//   (for example seige force) - Calvitix
// - Cleaned up data of dead player.
// - Improved Diplomat cleanup.
// - The explore resolution is now a constant. - Feb. 21st 2005 Martin Gühmann
// - Set back explore resolution to five and set roads are now every second
//   turn computed, tileimps every fifth turn. - Feb. 21st 2005 Martin Gühmann
// - UnGroupGarrisionUnits and MoveOutofCityTransportUnits disabled,
//   at least the later must be reconsidered as I got the feeling that we have
//   in the city units that are waiting for being moved out.
//    - Feb. 21st 2005 Martin Gühmann
// - Improved AI sliders optimization. - Jul 18th 2005 Martin Gühmann
// - Removed unused local variables. (Sep 9th 2005 Martin Gühmann)
// - Moved settle_water check inside the GetSettleTargets method. (May 20th 2006 Martin Gühmann)
//
//----------------------------------------------------------------------------

#include "c3.h"
#include "profileai.h"
#include "gfx_options.h"
#include "Diplomat.h"
#include "Scheduler.h"
#include "ctpagent.h"
#include "ctpgoal.h"
#include "ctpai.h"
#include "GoalRecord.h"
#include "Squad.h"
#include "mapanalysis.h"
#include "governor.h"
#include "AgreementMatrix.h"
#include "Civilisation.h" 
#include "DiplomacyProposalRecord.h"
#include "robotastar2.h"
#include "gaiacontroller.h"
#include "OrderRecord.h"
#include "AdvanceListRecord.h"
#include "AdvanceRecord.h"
#include "UnitRecord.h"
#include "EndGameObjectRecord.h"
#include "DiffDB.h"
#include "profileDB.h"
#include "RegardEvent.h"
#include "reactevent.h"
#include "ProposalResponseEvent.h"
#include "CounterResponseEvent.h"
#include "ThreatResponseEvent.h"
#include "RejectResponseEvent.h"
#include "MotivationEvent.h"
#include "NProposalEvent.h"
#include "ResponseEvent.h"
#include "SStateEvent.h"
#include "DStateEvent.h"
#include "player.h"
#include "newturncount.h"
#include "CTPDatabase.h"
#include "Army.h"
#include "ArmyData.h"
#include "ArmyPool.h"
#include "Unit.h"
#include "UnitRecord.h"
#include "UnitData.h"
#include "UnitPool.h"
#include "World.h"
#include "time.h"
#include "Cell.h"
#include "Gold.h"
#include "RandGen.h"
#include "GameSettings.h"
#include "SelItem.h"
#include "network.h"
#include "director.h"
#include "SlicEngine.h"
#include "tiledmap.h"
#include "radarmap.h"
extern SelectedItem *g_selected_item; 
extern Network g_network;
extern Director *g_director;
extern RadarMap		*g_radarMap;
#include "MoveFlags.h"
#include <list>
enum READINESS_LEVEL;


#include "Events.h"
#include "GameEventUser.h"
#include "Unit.h"
#include "settlemap.h"
#include "ctpaidebug.h"
#include "TurnCnt.h"
//Added by Martin Gühmann to access the ConstDB
#include "ConstDB.h"

extern TurnCount *g_turn;

extern CTPDatabase<GoalRecord> *g_theGoalDB;
extern ArmyPool		*g_theArmyPool;
extern World		*g_theWorld;

using namespace std;

PLAYER_INDEX CtpAi::s_maxPlayers = 0;

const OrderRecord * CtpAi::sm_unloadOrderRec = NULL;
const OrderRecord * CtpAi::sm_spaceLaunchOrderRec = NULL;
const OrderRecord * CtpAi::sm_disbandArmyOrderRec = NULL;
const OrderRecord * CtpAi::sm_moveOrderRec = NULL;
sint32 CtpAi::sm_goalDefendIndex = -1;
sint32 CtpAi::sm_goalSeigeIndex = -1;
sint32 CtpAi::sm_endgameWorldUnionIndex = -1;


namespace
{
	// Settings for periodic actions
	// These should be > 0 (otherwise % will crash). 
	// The original ACTIVISION values are 5 for all period.
	size_t const    PERIOD_COMPUTE_ROADS                = 2; // From 5
	size_t const    PERIOD_COMPUTE_TILE_IMPROVEMENTS    = 5;
	size_t const    EXPLORE_RESOLUTION                  = 5;

} // namespace


STDEHANDLER(CtpAi_CaptureCityEvent)
{
	Unit city;
	sint32 newOwner;
	sint32 cause;
	MapPoint pos;
	sint32 originalOwner;

	if(!args->GetCity(0, city))
		return GEV_HD_Continue;

	if(args->GetPlayer(0, newOwner))
		return GEV_HD_Continue;

	if(!args->GetInt(0, cause))
		return GEV_HD_Continue;

	originalOwner = city.GetOwner();
	
	
	
	

	
	CtpAi::AddOwnerGoalsForCity(city, newOwner);

	
	

	
	CtpAi::AddForeignerGoalsForCity(city, originalOwner);

	return GEV_HD_Continue;
}


STDEHANDLER(CtpAi_CreateCityEvent)
{
	PLAYER_INDEX playerId;
	MapPoint pos;
	sint32 cause;
	Unit city;

	
	if (!args->GetPlayer(0, playerId))
		return GEV_HD_Continue;

	
	if (!args->GetPos(0, pos))
		return GEV_HD_Continue;

	
	if (!args->GetInt(0, cause))
		return GEV_HD_Continue;

	
	if (!args->GetCity(0, city))
		return GEV_HD_Continue;

	
	for (PLAYER_INDEX foreignerId = 0; foreignerId < CtpAi::s_maxPlayers; foreignerId++)
	{
		
		if (foreignerId == city.GetOwner())
			continue;

		
		CtpAi::AddForeignerGoalsForCity(city, foreignerId);
	}
	CtpAi::AddOwnerGoalsForCity(city, city.GetOwner());

	
	SettleMap::s_settleMap.HandleCityGrowth(city);

	return GEV_HD_Continue;
}


void CtpAi::AddOwnerGoalsForCity(const Unit &city, const PLAYER_INDEX ownerId)
{
	CTPGoal_ptr goal_ptr;
	GOAL_TYPE goal_type;

	Assert(city.IsValid());
	
	for (goal_type = 0; goal_type < g_theGoalDB->NumRecords(); goal_type++)
	{
		
		if ( ! g_theGoalDB->Get(goal_type)->GetTargetOwnerSelf() )
			continue;
		
		
		if ( g_theGoalDB->Get(goal_type)->GetTargetTypeCity() == true ||
			g_theGoalDB->Get(goal_type)->GetTargetTypeTradeRoute() == true ||
			g_theGoalDB->Get(goal_type)->GetTargetTypeImprovement() == true)
		{
			goal_ptr = new CTPGoal;
			goal_ptr->Set_Type(goal_type);
			goal_ptr->Set_Player_Index(ownerId);
			goal_ptr->Set_Target_City(city);
			
			
			
			Scheduler::GetScheduler(ownerId).Add_New_Goal(goal_ptr);
		}
	}
}


void CtpAi::AddForeignerGoalsForCity(const Unit &city, const PLAYER_INDEX foreignerId)
{
	CTPGoal_ptr goal_ptr;
	GOAL_TYPE goal_type;

	Assert(city.IsValid());
	
	for (goal_type = 0; goal_type < g_theGoalDB->NumRecords(); goal_type++)
	{
		
		if ( g_theGoalDB->Get(goal_type)->GetTargetOwnerSelf() )
			continue;
		
		
		if ( g_theGoalDB->Get(goal_type)->GetTargetTypeCity() == true ||
			g_theGoalDB->Get(goal_type)->GetTargetTypeTradeRoute() == true ||
			g_theGoalDB->Get(goal_type)->GetTargetTypeImprovement() == true)
		{
			goal_ptr = new CTPGoal;
			goal_ptr->Set_Type(goal_type);
			goal_ptr->Set_Player_Index(foreignerId);
			goal_ptr->Set_Target_City(city);
			
			
			
			Scheduler::GetScheduler(foreignerId).Add_New_Goal(goal_ptr);
		}
	}
}


STDEHANDLER(CtpAi_SettleEvent)
{
	
	Army army;
	MapPoint pos;
	static sint32 last_settle;
	static sint32 last_player;

	
	if (!args->GetArmy(0, army))
		return GEV_HD_Continue;

	pos = army->RetPos();
	Player *player_ptr = g_player[army->GetOwner()];
	Assert(player_ptr != NULL);	
 	
	
	
	
	
	
	
	if(!g_network.IsActive()) {
		if (player_ptr->GetPlayerType() == PLAYER_TYPE_ROBOT &&
			!(g_network.IsClient() && g_network.IsLocalPlayer(army->GetOwner())) &&
			!(g_network.IsHost() && player_ptr->m_owner == g_selected_item->GetVisiblePlayer()) &&
			last_settle == NewTurnCount::GetCurrentRound() &&
			last_player == army->GetOwner())
		{
			
			return GEV_HD_Stop;
		}
	}

	
	last_settle = NewTurnCount::GetCurrentRound();
	last_player = army->GetOwner();

	return GEV_HD_Continue;
}



STDEHANDLER(CtpAi_GrowCityEvent)
{
	Unit city;

	if(!args->GetCity(0, city)) 
		return GEV_HD_Continue;

	
	SettleMap::s_settleMap.HandleCityGrowth(city);

	return GEV_HD_Continue;
}


STDEHANDLER(CtpAi_KillCityEvent)
{
	Unit u;
	CAUSE_REMOVE_ARMY cause;
	sint32 killer;

	if(!args->GetCity(0, u))
		return GEV_HD_Continue;

	if(!args->GetInt(0, (sint32&)cause))
		return GEV_HD_Continue;

	if(!args->GetPlayer(0, killer))
		killer = -1;

	
	SettleMap::s_settleMap.SetCanSettlePos(u.RetPos(), true);

	
	CTPGoal_ptr goal_ptr;
	GOAL_TYPE goal_type;
	for (sint32 playerId = 1; playerId < CtpAi::s_maxPlayers; playerId++)
	{
		
		if (playerId != killer && playerId != u.GetOwner())
			continue;

		Scheduler & scheduler = Scheduler::GetScheduler(playerId);

		
		for (goal_type = 0; goal_type < g_theGoalDB->NumRecords(); goal_type++)
		{
			
			if ( (g_theWorld->IsWater(u.RetPos()) == FALSE) && 
				(g_theGoalDB->Get(goal_type)->GetTargetTypeSettleLand()) ||
				(g_theWorld->IsWater(u.RetPos()) == TRUE) && 
				(g_theGoalDB->Get(goal_type)->GetTargetTypeSettleSea()))
			{
				goal_ptr = new CTPGoal();
				goal_ptr->Set_Type( goal_type );
				goal_ptr->Set_Player_Index( playerId );
				goal_ptr->Set_Target_Pos( u.RetPos() );
				
				scheduler.Add_New_Goal( goal_ptr );
				
				g_graphicsOptions->AddTextToCell(u.RetPos(), "KILLED", 0);
			}
		}
	}


	return GEV_HD_Continue;
}

	
STDEHANDLER(CtpAi_NukeCityUnit)
{
	Unit unit;
	Unit city;
	
	if (!args->GetUnit(0,unit))
		return GEV_HD_Continue;

	if (!args->GetCity(0,city))
		return GEV_HD_Continue;

	PLAYER_INDEX killer = unit.GetOwner();
	PLAYER_INDEX city_owner = city.GetOwner();

	sint32 population = 0;
	if (city.m_id && city->GetCityData())
	{
		city->GetCityData()->GetPop(population);
	}

	
	if (population <= 0)
	{	
		
		SettleMap::s_settleMap.SetCanSettlePos(city.RetPos(), true);
		
		
		CTPGoal_ptr goal_ptr;
		GOAL_TYPE goal_type;
		for (sint32 playerId = 1; playerId < CtpAi::s_maxPlayers; playerId++)
		{
			
			if (playerId != killer && playerId != city_owner)
				continue;
			
			Scheduler & scheduler = Scheduler::GetScheduler(playerId);
			
			
			for (goal_type = 0; goal_type < g_theGoalDB->NumRecords(); goal_type++)
			{
				
				if ( (g_theWorld->IsWater(city.RetPos()) == FALSE) && 
					(g_theGoalDB->Get(goal_type)->GetTargetTypeSettleLand()) ||
					(g_theWorld->IsWater(city.RetPos()) == TRUE) && 
					(g_theGoalDB->Get(goal_type)->GetTargetTypeSettleSea()))
				{
					goal_ptr = new CTPGoal();
					goal_ptr->Set_Type( goal_type );
					goal_ptr->Set_Player_Index( playerId );
					goal_ptr->Set_Target_Pos( city.RetPos() );
					
					scheduler.Add_New_Goal( goal_ptr );
					
					g_graphicsOptions->AddTextToCell(city.RetPos(), "NUKED", 0);
				}
			}
		}
	}

	return GEV_HD_Continue;
}


STDEHANDLER(CtpAi_AddUnitToArmyEvent)
{
	
	return GEV_HD_Continue;
}


STDEHANDLER(CtpAi_CreatedArmyEvent)
{
	Army army;

	if (!args->GetArmy(0, army))
		return GEV_HD_Continue;
	
	if (army->CanSettle() && 
		Diplomat::GetDiplomat(army.GetOwner()).ShouldEscortSettlers())
	{
		CtpAi::GroupWithEscort(army);
	}

	CtpAi::AddGoalsForArmy(army);

	return GEV_HD_Continue;
}


void CtpAi::GroupWithEscort(const Army & army)
{
	
	if (army.Num() > 1)
		return;

	
	PLAYER_INDEX playerId = army->GetOwner();

	
	sint32 min_strength = -1;
	sint32 tmp_strength;
	Army min_army;
	Army tmp_army;
	const UnitRecord *unit_rec;

	CellUnitList candidate_units;
	g_theWorld->GetArmy(army->RetPos(), candidate_units);
	for (sint32 i = 0; i < candidate_units.Num(); i++)
		{
			tmp_army = candidate_units[i].GetArmy();
			Assert( g_theArmyPool->IsValid(tmp_army) );

			
			if (tmp_army == army)
				continue;

			
			if (tmp_army.Num() > 1)
				continue;

			
			if ((tmp_army->GetMovementType() &
				 army->GetMovementType()) != army->GetMovementType())
				continue;

			unit_rec = g_theUnitDB->Get(tmp_army[0].GetType());
			tmp_strength = static_cast<sint32>
				(unit_rec->GetAttack() * 
				 unit_rec->GetDefense() *
				 unit_rec->GetFirepower() *
				 unit_rec->GetArmor()
				);

			if (min_strength < 0 || tmp_strength < min_strength)
				{
					min_strength = tmp_strength;
					min_army = tmp_army;
				}
		}
	
	
	if (min_army.m_id != 0x0)
	{
		
		g_gevManager->AddEvent(GEV_INSERT_Tail, GEV_GroupUnitOrder, 
			GEA_Army, army,
			GEA_Unit, min_army[0],
			GEA_End);
	}
}


void CtpAi::AddGoalsForArmy(const Army &army)
{	
	PLAYER_INDEX playerId;
	PLAYER_INDEX foreignerId;

	playerId = army.GetOwner();

	CTPGoal_ptr goal_ptr;
	GOAL_TYPE goal_type;

	
	
	

	
	CTPAgent *new_agent = new CTPAgent();
	new_agent->Set_Army(army);
	new_agent->Set_Player_Number(playerId);
		
	
	Squad *new_squad = new Squad();
	new_squad->Add_Agent(new_agent);
		
	
	
	Scheduler::GetScheduler(playerId).Add_New_Squad(new_squad);

	
	for (foreignerId = 0; foreignerId < CtpAi::s_maxPlayers; foreignerId++)
	{
		
		for (goal_type = 0; goal_type < g_theGoalDB->NumRecords(); goal_type++)
		{
			// Speed-up only, no functional change.
			GoalRecord const *	goal	= g_theGoalDB->Get(goal_type);

			if(goal
			&&(goal->GetTargetTypeAttackUnit() 
			|| goal->GetTargetTypeSpecialUnit())
			&&(goal->GetTargetOwnerSelf() == (foreignerId == playerId))
			){
				goal_ptr = new CTPGoal();
				goal_ptr->Set_Type(goal_type);
				goal_ptr->Set_Player_Index(foreignerId);
				goal_ptr->Set_Target_Army(army);
			
				Scheduler::GetScheduler(foreignerId).Add_New_Goal(goal_ptr);
			}
		}
	}

}


STDEHANDLER(CtpAi_StartNegotiationsEvent)
{
	time_t t1;
	time_t t2;

	PLAYER_INDEX playerId;

	
	if (!args->GetPlayer(0, playerId))
		return GEV_HD_Continue;

	sint32 round;
	if (g_player[playerId] != NULL)
		round = g_player[playerId]->GetCurRound();
	else
		return GEV_HD_Continue;

	t1 = GetTickCount();
	DPRINTF(k_DBG_AI, ("\n\n"));
	DPRINTF(k_DBG_AI, ("//\n"));
	DPRINTF(k_DBG_AI, ("// START NEGOTIATIONS -- Turn %d\n", round));
	DPRINTF(k_DBG_AI, ("//					     Player %d\n", playerId));

	
	if (g_player[playerId]->m_civilisation->GetCivilisation() == 0)
	{
		DPRINTF(k_DBG_AI, ("//	*** Barbarians do not conduct diplomacy.\n"));

		
		g_director->AddBeginScheduler(playerId);

		return GEV_HD_Continue;
	}

	

	
	
	if((g_turn->IsHotSeat() || g_turn->IsEmail()) && 
		g_player[playerId]->m_playerType != PLAYER_TYPE_ROBOT)
	{
		
		
		for (sint32 foreignerId = 1; foreignerId < CtpAi::s_maxPlayers; foreignerId++) 
		{
			if (g_player[foreignerId] &&
				g_player[foreignerId]->m_playerType == PLAYER_TYPE_ROBOT)
			{
				
				Diplomat::GetDiplomat(foreignerId).StartNegotiations(playerId);
			}
		}
	}

	
	bool found = Diplomat::GetDiplomat(playerId).StartNegotiations();
	
	if (found == false)
	{
		
		if(!g_network.IsClient()) {
			g_director->AddBeginScheduler(playerId);
		}
	}

	t2 = GetTickCount();
	DPRINTF(k_DBG_AI, ("//  elapsed time = %d ms\n", (t2 - t1)  ));

	return GEV_HD_Continue;
}




STDEHANDLER(CtpAi_ConsiderNuclearWar)
{
	PLAYER_INDEX playerId;

	
	if (!args->GetPlayer(0, playerId))
		return GEV_HD_Continue;

	Player *player_ptr = g_player[playerId];
	if (player_ptr == NULL)
		return GEV_HD_Continue;

	
	if(player_ptr->m_playerType != PLAYER_TYPE_ROBOT ||
	   (g_network.IsActive() && playerId == g_selected_item->GetVisiblePlayer())) {
		return GEV_HD_Continue;
	}

	Diplomat & diplomat = Diplomat::GetDiplomat(playerId);

	
	PLAYER_INDEX enemy_to_nuke = diplomat.ComputeNuclearLaunchTarget();

	
	bool fire_now;
	if (enemy_to_nuke != -1)
	{
		
		fire_now = (AgreementMatrix::s_agreements.TurnsAtWar(playerId, enemy_to_nuke) > 15);
	}
	else
	{
		
		fire_now = false;
	}
	diplomat.TargetNuclearAttack(enemy_to_nuke, fire_now);


	return GEV_HD_Continue;
}


STDEHANDLER(CtpAi_BeginSchedulerEvent)
{
	time_t t1;
	time_t t2;

	PLAYER_INDEX playerId;

	
	if (!args->GetPlayer(0, playerId))
		return GEV_HD_Continue;

	sint32 round;
	if (g_player[playerId] != NULL)
		round = g_player[playerId]->GetCurRound();
	else
		return GEV_HD_Continue;		

#ifdef _DEBUG
	static bool first=true;
	if (first) {
		int a;
		Assert(a=(playerId == g_selected_item->GetCurPlayer()));
		if (!a) first=false;
	}
#endif

	
	t1 = GetTickCount();
	DPRINTF(k_DBG_AI, ("\n\n"));
	DPRINTF(k_DBG_AI, ("//\n"));
	DPRINTF(k_DBG_AI, ("// PROCESS SQUAD CHANGES -- Turn %d\n", round));
	DPRINTF(k_DBG_AI, ("//                          Player %d\n", playerId));

	
	Scheduler::GetScheduler(playerId).Process_Squad_Changes();

	
	Scheduler::GetScheduler(playerId).Reset_Squad_Execution();

	t2 = GetTickCount();
	DPRINTF(k_DBG_AI, ("//  elapsed time = %d ms\n", (t2 - t1)  ));

	t1 = GetTickCount();
	DPRINTF(k_DBG_AI, ("\n"));
	DPRINTF(k_DBG_AI, ("\n"));
	DPRINTF(k_DBG_AI, ("//\n"));
	DPRINTF(k_DBG_AI, ("// PROCESS GOAL CHANGES -- Turn %d\n", round));
	DPRINTF(k_DBG_AI, ("//                         Player %d\n", playerId));
       
	
	Scheduler::GetScheduler(playerId).Process_Goal_Changes();
	t2 = GetTickCount();
	DPRINTF(k_DBG_AI, ("//  elapsed time = %d ms\n", (t2 - t1)  ));

	t1 = GetTickCount();
	DPRINTF(k_DBG_AI, ("\n\n"));
	DPRINTF(k_DBG_AI, ("//\n"));
	DPRINTF(k_DBG_AI, ("// SORT MATCHES -- Turn %d\n", round));
	DPRINTF(k_DBG_AI, ("//                 Player %d\n", playerId));

	
	Scheduler::GetScheduler(playerId).Sort_Matches();
	t2 = GetTickCount();
	DPRINTF(k_DBG_AI, ("//  elapsed time = %d ms\n", (t2 - t1)  ));

	
	g_gevManager->AddEvent(GEV_INSERT_Tail, GEV_ProcessMatches,
						   GEA_Player, playerId,
						   GEA_Int, 0, 
						   GEA_End);

	return GEV_HD_Continue;
}


STDEHANDLER(CtpAi_ProcessMatchesEvent)
{
	sint32 t1;
	sint32 t2;

	PLAYER_INDEX playerId;
	sint32 cycle;

	
	if (!args->GetPlayer(0, playerId))
		return GEV_HD_Continue;

	
	if (!args->GetInt(0, cycle))
		return GEV_HD_Continue;

	sint32 round;
	Player *player_ptr = g_player[playerId];
	if (g_player[playerId] != NULL)
		round = player_ptr->GetCurRound();
	else
		return GEV_HD_Continue;	
	



	t1 = GetTickCount();
	DPRINTF(k_DBG_AI, ("\n\n"));
	DPRINTF(k_DBG_AI, ("//\n"));
	DPRINTF(k_DBG_AI, ("// PROCESS SQUAD CHANGES -- Turn %d\n", round));
	DPRINTF(k_DBG_AI, ("//                          Player %d\n", playerId));
	
	
	Scheduler::GetScheduler(playerId).Process_Squad_Changes();
	t2 = GetTickCount();
	DPRINTF(k_DBG_AI, ("//  elapsed time = %d ms\n", (t2 - t1)  ));

    
	
	t1 = GetTickCount();
	DPRINTF(k_DBG_AI, ("\n\n"));
	DPRINTF(k_DBG_AI, ("//\n"));
	DPRINTF(k_DBG_AI, ("// PROCESS MATCHES -- Turn %d\n", round));
	DPRINTF(k_DBG_AI, ("//                    Cycle %d\n", cycle));
	DPRINTF(k_DBG_AI, ("//                    Player %d\n", playerId));

	
	Scheduler::GetScheduler(playerId).
		Match_Resources((player_ptr->m_playerType == PLAYER_TYPE_ROBOT));
	
	t2 = GetTickCount();
	DPRINTF(k_DBG_AI, ("//  elapsed time = %d ms\n", (t2 - t1)  ));

	
	
	cycle++;
	sint32 diff_cycles = 0;
	if (g_theGameSettings->GetDifficulty() == (LEVELS_OF_DIFFICULTY - 1))
		diff_cycles = 2;

	// Modified by Martin Gühmann so that this can be exposed to const.txt
	if ( cycle < g_theConstDB->GetMaxMatchListCycles() + diff_cycles)
		{
			g_gevManager->AddEvent(GEV_INSERT_Tail, GEV_ProcessMatches,
								   GEA_Player, playerId,
								   GEA_Int, cycle, 
								   GEA_End);
		}
	
	else 
	{
		
		if ( player_ptr->m_playerType == PLAYER_TYPE_ROBOT )
		{
			CtpAi::ExecuteOpportunityActions(playerId);

			
			if (player_ptr->GetGaiaController() && 
				player_ptr->GetGaiaController()->CanStartCountdown())
			{
				
				player_ptr->GetGaiaController()->StartCountdown();
			}

			const StrategyRecord & strategy = 
				Diplomat::GetDiplomat(playerId).GetCurrentStrategy();
		
			sint32 count;
			strategy.GetDisbandArmyCount(count);
		
			
			Scheduler::GetScheduler(playerId).DisbandObsoleteArmies(count);
		}
		
		
		
		sint32 i;
		if(player_ptr->m_playerType == PLAYER_TYPE_ROBOT &&
		   (!g_network.IsClient() || g_network.IsLocalPlayer(playerId))) {
			for(i = 0; i < player_ptr->m_all_armies->Num(); i++) {
//				g_director->IncrementPendingGameActions();

				g_gevManager->AddEvent(GEV_INSERT_Tail, GEV_BeginTurnExecute,
									   GEA_Army, player_ptr->m_all_armies->Access(i).m_id,
									   GEA_End);
			}
		}

		
		if (playerId != NewTurnCount::GetStopPlayer())
		{
			Assert(playerId == g_selected_item->GetCurPlayer());
			if (playerId == g_selected_item->GetCurPlayer()) {
				if(!g_network.IsActive() || g_network.IsLocalPlayer(playerId)) {
					if(!g_network.IsClient() || 
						(g_network.IsClient() && player_ptr->m_playerType == PLAYER_TYPE_ROBOT)) {
						DPRINTF(k_DBG_GAMESTATE, ("AI End turn, %d\n", playerId));
						g_director->AddEndTurn(); 
					}
				}
			}
		}
	}

	return GEV_HD_Continue;
}


STDEHANDLER(CtpAi_TransportLoaded)
{
	GameEventArgument *armyArg = args->GetArg(GEA_Army, 0);
	GameEventArgument *posArg = args->GetArg(GEA_MapPoint, 0);

	Army a;
	MapPoint pos;

	if(!armyArg || !armyArg->GetArmy(a))
		return GEV_HD_Continue;

	if(!posArg || !posArg->GetPos(pos))
		return GEV_HD_Continue;

	if( g_theArmyPool->IsValid(a) )
		a->ClearOrders();
	Assert(g_theArmyPool->IsValid(a) == TRUE);

	return GEV_HD_Continue;
}


STDEHANDLER(CtpAi_ImprovementComplete)
{
	sint32 owner;
	sint32 type;
	MapPoint pos;

	if(!args->GetPos(0, pos))
		return GEV_HD_Continue;

	if(!args->GetPlayer(0, owner))
		return GEV_HD_Continue;
	
	if(!args->GetInt(0, type))
		return GEV_HD_Continue;

	
	Player *owner_player = g_player[owner];
	if (owner_player == NULL)
		return GEV_HD_Continue;		

	if (GaiaController::sm_endgameImprovements & ((uint64)0x1 << (uint64)type))
		{
			GOAL_TYPE goal_type;
			const StrategyRecord::GoalElement *goal_element_ptr;
			sint16 goal_element;
			const GoalRecord *goal_rec;
				
			
			for (sint32 playerId = 1; playerId < CtpAi::s_maxPlayers; playerId++)
			{
				Scheduler & scheduler = 
					Scheduler::GetScheduler(playerId);
				const StrategyRecord & strategy = 
					Diplomat::GetDiplomat(playerId).GetCurrentStrategy();	

				for (goal_element = 0; goal_element < strategy.GetNumGoalElement(); goal_element++) 
				{
					goal_element_ptr = strategy.GetGoalElement(goal_element);
					goal_type = goal_element_ptr->GetGoalIndex();
					goal_rec = g_theGoalDB->Get(goal_type);
					
					
					if ( !goal_rec->GetTargetTypeEndgame() )
						continue;
					
					
					if (playerId != owner && goal_rec->GetTargetOwnerSelf())
						continue;
					
					
					if (playerId == owner && !goal_rec->GetTargetOwnerSelf())
						continue;
					
					CTPGoal_ptr goal_ptr = new CTPGoal();
					goal_ptr->Set_Player_Index( playerId );
					goal_ptr->Set_Type( goal_type );
					goal_ptr->Set_Target_Pos( pos );
					
					scheduler.Add_New_Goal( goal_ptr );
				}
			}
			
	}
	return GEV_HD_Continue;
}







void CtpAi::InitializeEvents()
{
	
	
	RegardEventCallbacks::AddCallbacks();
	ReactEventCallbacks::AddCallbacks();
	ProposalResponseEventCallbacks::AddCallbacks(); 
	CounterResponseEventCallbacks::AddCallbacks();
	ThreatResponseEventCallbacks::AddCallbacks();
	RejectResponseEventCallbacks::AddCallbacks();
	MotivationEventCallbacks::AddCallbacks();
	NewProposalEventCallbacks::AddCallbacks();
	ResponseEventCallbacks::AddCallbacks();
	StrategicStateEventCallbacks::AddCallbacks();
	DiplomaticStateEventCallbacks::AddCallbacks();
	
	
	CTPAgent::AssociateEventsWithOrdersDB();
	
	
	
	
	
	g_gevManager->AddCallback(GEV_StartNegotiations, 
		GEV_PRI_Primary, 
		&s_CtpAi_StartNegotiationsEvent);
	
	
	g_gevManager->AddCallback(GEV_BeginScheduler, 
		GEV_PRI_Pre, 
		&s_CtpAi_ConsiderNuclearWar);

	
	g_gevManager->AddCallback(GEV_BeginScheduler, 
		GEV_PRI_Primary, 
		&s_CtpAi_BeginSchedulerEvent);
	
	
	g_gevManager->AddCallback(GEV_ProcessMatches, 
		GEV_PRI_Primary, 
		&s_CtpAi_ProcessMatchesEvent);

	g_gevManager->AddCallback(GEV_CaptureCity, 
		GEV_PRI_Pre, 
		&s_CtpAi_CaptureCityEvent);
	
	g_gevManager->AddCallback(GEV_CreateCity, 
		GEV_PRI_Post, 
		&s_CtpAi_CreateCityEvent);

	g_gevManager->AddCallback(GEV_Settle, 
		GEV_PRI_Pre, 
		&s_CtpAi_SettleEvent);
	
	g_gevManager->AddCallback(GEV_CityInfluenceChanged, 
		GEV_PRI_Primary, 
		&s_CtpAi_GrowCityEvent);
	
	g_gevManager->AddCallback(GEV_CreatedArmy, 
		GEV_PRI_Post, 
		&s_CtpAi_CreatedArmyEvent);
	
	g_gevManager->AddCallback(GEV_AddUnitToArmy, 
		GEV_PRI_Post, 
		&s_CtpAi_AddUnitToArmyEvent);
	
	g_gevManager->AddCallback(GEV_MoveIntoTransport,
		GEV_PRI_Pre,
		&s_CtpAi_TransportLoaded);

	g_gevManager->AddCallback(GEV_ImprovementComplete,
		GEV_PRI_Post,
		&s_CtpAi_ImprovementComplete);

	g_gevManager->AddCallback(GEV_KillCity, 
		GEV_PRI_Pre, 
		&s_CtpAi_KillCityEvent);
	
	g_gevManager->AddCallback(GEV_NukeCityUnit,
		GEV_PRI_Post,
		&s_CtpAi_NukeCityUnit);

}




void CtpAi::Cleanup()
{
	SettleMap::s_settleMap.Cleanup();
    Scheduler::CleanupAll();
	Governor::Cleanup();
	Diplomat::CleanupAll();
}


void CtpAi::Initialize()
{
	
	Resize();

	
	SettleMap::s_settleMap.Initialize();

	
	GaiaController::InitializeStatics();

	for (PLAYER_INDEX player=0; player < s_maxPlayers; player++)
	{
		
		
		
		Player *player_ptr = g_player[player];
		if(player_ptr == NULL) continue;

		
		Scheduler::GetScheduler(player).Initialize();

		
		Diplomat::GetDiplomat(player).Initialize();

		
		
		Diplomat::GetDiplomat(player).InitStrategicState();

		
		Governor::GetGovernor(player).Resize( (sint16) g_theWorld->GetWidth(),
											  (sint16) g_theWorld->GetHeight(),
											  1 );
	}

	
	
	sint32 index;	
	g_theDiplomacyDB->GetNamedItem("DIPLOMACY_DEFAULT", index);
	const DiplomacyRecord *default_rec = g_theDiplomacyDB->Get(index);
	for (sint32 i = PROPOSAL_NONE+1; i < PROPOSAL_MAX; i++)
	{
		
		Diplomat::s_proposalTypeToElemIndex[i] = 0;

		for (index = 0; index < default_rec->GetNumProposalElement(); index++)
		{
			
			if (strcmp(s_proposalNames[i].c_str(),default_rec->GetProposalElement(index)->GetProposal()->GetType()) == 0)
			{
				Diplomat::s_proposalTypeToElemIndex[i] = index;
				break;
			}
		}
	}

	
	sm_unloadOrderRec = 
		g_theOrderDB->Get(g_theOrderDB->FindRecordNameIndex("ORDER_UNLOAD"));

	sm_spaceLaunchOrderRec = 
		g_theOrderDB->Get(g_theOrderDB->FindRecordNameIndex("ORDER_SPACE_LAUNCH"));

	sm_disbandArmyOrderRec =
		g_theOrderDB->Get(g_theOrderDB->FindRecordNameIndex("ORDER_DISBAND_ARMY"));

	sm_moveOrderRec =
		g_theOrderDB->Get(g_theOrderDB->FindRecordNameIndex("ORDER_MOVE"));

	sm_goalDefendIndex =
		g_theGoalDB->FindRecordNameIndex("GOAL_DEFEND");

	sm_goalSeigeIndex =
		g_theGoalDB->FindRecordNameIndex("GOAL_SEIGE");

	sm_endgameWorldUnionIndex =
		g_theEndGameObjectDB->FindRecordNameIndex("ENDGAME_WORLD_UNION");

	
#ifdef _DEBUG
	CellUnitList unit_list;
	
	CtpAiDebug::SetDebugPlayer(2); 
	CtpAiDebug::SetDebugGoalType(-1); 
	CtpAiDebug::SetDebugArmies(unit_list); 
#endif

}


void CtpAi::Load(CivArchive & archive)
{
	
	Initialize();

	
	
	
	Diplomat::LoadAll(archive);

	
	g_theWorld->ComputeGoodsValues();

	
	Player *player_ptr;
	sint32 num_armies;
	sint32 num_cities; 
	sint32 i;
	Army army;
	Unit city;
	for (PLAYER_INDEX playerId=0; playerId < s_maxPlayers; playerId++)
	{
		player_ptr = g_player[playerId];
		

		
		if (player_ptr == NULL)
			continue;

		num_armies = player_ptr->m_all_armies->Num();
		num_cities = player_ptr->m_all_cities->Num();

		for (i = 0; i < num_cities; i++)
		{
			city = player_ptr->m_all_cities->Access(i);
			Assert( g_theUnitPool->IsValid(city) );
			Assert( city->GetCityData() != NULL );

			
			for (PLAYER_INDEX foreignerId = 0; foreignerId < CtpAi::s_maxPlayers; foreignerId++)
			{
				
				if (foreignerId == city.GetOwner())
					continue;

				
				AddForeignerGoalsForCity(city, foreignerId);
			}
			AddOwnerGoalsForCity(city, city.GetOwner());

			
			SettleMap::s_settleMap.HandleCityGrowth(city);

			
			city.CD()->AdjustSizeIndices();
		}

		for (i = 0; i < num_armies; i++)
		{
			army = player_ptr->m_all_armies->Access(i);
			Assert( g_theArmyPool->IsValid(army) );

			
			AddGoalsForArmy(army);
		}
	}

	
	MapAnalysis::GetMapAnalysis().BeginTurn();
}


void CtpAi::Save(CivArchive & archive)
{
	
	
	
	Diplomat::SaveAll(archive);
}


//----------------------------------------------------------------------------
//
// Name       : CtpAi::RemovePlayer
//
// Description: Remove AI data of a dead player.
//
// Parameters : deadPlayerId	: index of the dead player
//
// Globals    : -
//
// Returns    : -
//
// Remark(s)  : -
//
//----------------------------------------------------------------------------
void CtpAi::RemovePlayer(const PLAYER_INDEX deadPlayerId)
{
	Assert(deadPlayerId < s_maxPlayers);

	Scheduler::GetScheduler(deadPlayerId).Cleanup();
    Governor::GetGovernor(deadPlayerId) = Governor::INVALID;

	for (PLAYER_INDEX player = 0; player < s_maxPlayers; ++player)
	{
		Diplomat::GetDiplomat(player).InitForeigner(deadPlayerId);
	}
	
	AgreementMatrix::s_agreements.ClearAgreementsInvolving(deadPlayerId);
	Diplomat::GetDiplomat(deadPlayerId).Cleanup();
	
	if (deadPlayerId + 1 >= s_maxPlayers)
    {
        // Last player in array
		Resize();
    }
}


void CtpAi::AddPlayer(const PLAYER_INDEX newPlayerId)
{
	Assert(g_player[newPlayerId]);
	
	if (newPlayerId >= s_maxPlayers)
    {
		Resize();
    }

	Scheduler::GetScheduler(newPlayerId).Initialize();
	Diplomat::GetDiplomat(newPlayerId).Initialize();
	Diplomat::GetDiplomat(newPlayerId).InitStrategicState();

	for (PLAYER_INDEX player = 0; player < s_maxPlayers; ++player)
	{
		Diplomat::GetDiplomat(player).InitForeigner(newPlayerId);
	}
}






void CtpAi::BeginTurn(const PLAYER_INDEX player) 
{
	uint32 t1;
	uint32 t2;

	
	if (s_maxPlayers <= 0)
		return;

	sint32 round;

	Assert(player < s_maxPlayers);
	Player *player_ptr = g_player[player];
	if (player_ptr != NULL)
		round = player_ptr->GetCurRound();
	else
		return;
	Assert(player == g_selected_item->GetCurPlayer());

	t1 = GetTickCount();
	DPRINTF(k_DBG_AI, ("\n\n"));
	DPRINTF(k_DBG_AI, ("//\n"));
	DPRINTF(k_DBG_AI, ("// CHANGE GOVERNMENT -- Turn %d\n", round));
	DPRINTF(k_DBG_AI, ("//					    Player %d\n", player));

	
	if (player_ptr != NULL && 
		player_ptr->m_playerType == PLAYER_TYPE_ROBOT &&
		!g_network.IsClient()) 
	{
		sint32 government_type = Governor::GetGovernor(player).ComputeBestGovernment();
		if (government_type >= 0 && government_type != player_ptr->GetGovernmentType())
			
			player_ptr->SetGovernmentType(government_type);
	}

	t2 = GetTickCount();
	DPRINTF(k_DBG_AI, ("//  elapsed time = %d ms\n", (t2 - t1)  ));

	t1 = GetTickCount();
	DPRINTF(k_DBG_AI, ("\n\n"));
	DPRINTF(k_DBG_AI, ("//\n"));
	DPRINTF(k_DBG_AI, ("// PROCESS STRATEGY -- Turn %d\n", round));
	DPRINTF(k_DBG_AI, ("//					   Player %d\n", player));

	const StrategyRecord & strategy = 
		Diplomat::GetDiplomat(player).GetCurrentStrategy();

	
	if (player_ptr &&
		player_ptr->m_playerType == PLAYER_TYPE_ROBOT &&
		!g_network.IsClient()) 
	{
		sint32 pw_percent;
		sint32 sci_percent;

		strategy.GetPublicWorksPercent(pw_percent);
		strategy.GetSciencePercent(sci_percent);

		player_ptr->SetMaterialsTax(pw_percent/100.0);
		player_ptr->SetTaxes(sci_percent/100.0);

		t2 = GetTickCount();
		DPRINTF(k_DBG_AI, ("//  elapsed time = %d ms\n", (t2 - t1)  ));
		
		t1 = GetTickCount();
		DPRINTF(k_DBG_AI, ("\n\n"));
		DPRINTF(k_DBG_AI, ("//\n"));
		DPRINTF(k_DBG_AI, ("// SET RESEARCH -- Turn %d\n", round));
		DPRINTF(k_DBG_AI, ("//                 Player %d\n", player));
		
		
		SetResearch(player);
		t2 = GetTickCount();
		DPRINTF(k_DBG_AI, ("//  elapsed time = %d ms\n", (t2 - t1)  ));
	}

	t2 = GetTickCount();
	DPRINTF(k_DBG_AI, ("//  elapsed time = %d ms\n", (t2 - t1)  ));

	t1 = GetTickCount();
	DPRINTF(k_DBG_AI, ("\n\n"));
	DPRINTF(k_DBG_AI, ("//\n"));
	DPRINTF(k_DBG_AI, ("// MAP ANALYSIS -- Turn %d\n", round));
	DPRINTF(k_DBG_AI, ("//                    Player %d\n", player));

	
	
	
	
	
	Diplomat::GetDiplomat(player).ClearEffectiveRegardCache();

	MapAnalysis::GetMapAnalysis().BeginTurn();
	t2 = GetTickCount();
	DPRINTF(k_DBG_AI, ("//  elapsed time = %d ms\n", (t2 - t1)  ));

	t1 = GetTickCount();
	DPRINTF(k_DBG_AI, ("\n\n"));
	DPRINTF(k_DBG_AI, ("//\n"));
	DPRINTF(k_DBG_AI, ("// BEGIN DIPLOMACY -- Turn %d\n", round));
	DPRINTF(k_DBG_AI, ("//                    Player %d\n", player));

	
	Diplomat::GetDiplomat(player).BeginTurn();
	t2 = GetTickCount();
	DPRINTF(k_DBG_AI, ("//  elapsed time = %d ms\n", (t2 - t1)  ));

    // update : Compute Road Tiles every turn instead of every 5 turns (Calvitix)
	// Road computation round now a constant 
	if (round % PERIOD_COMPUTE_ROADS == 0)
	{
		
		t1 = GetTickCount();
		DPRINTF(k_DBG_AI, ("\n\n"));
		DPRINTF(k_DBG_AI, ("//\n"));
		DPRINTF(k_DBG_AI, ("// COMPUTE ROAD PRIORITIES -- Turn %d\n", round));
		DPRINTF(k_DBG_AI, ("//                    Player %d\n", player));
		
		
		Governor::GetGovernor(player).ComputeRoadPriorities();
		t2 = GetTickCount();
		DPRINTF(k_DBG_AI, ("//  elapsed time = %d ms\n", (t2 - t1)  ));
	}

	
	GaiaController *gaia_controller = player_ptr->GetGaiaController();
	if (player_ptr->m_playerType == PLAYER_TYPE_ROBOT &&
		gaia_controller && gaia_controller->CanBuildTowers(false))
	{
		t1 = GetTickCount();
		DPRINTF(k_DBG_AI, ("\n\n"));
		DPRINTF(k_DBG_AI, ("//\n"));
		DPRINTF(k_DBG_AI, ("// PLACE ENDGAME INSTALLATIONS -- Turn %d\n", round));
		DPRINTF(k_DBG_AI, ("//							  Player %d\n", player));

		
		gaia_controller->BuildProcessingTowers();
		t2 = GetTickCount();
		DPRINTF(k_DBG_AI, ("//  elapsed time = %d ms\n", (t2 - t1)  ));
	}
	else
	{
		
// update : Place Tile Improvement every turn instead of every 5 turns (Calvitix)
	// Tile improment placement round now a constant
		if (round % PERIOD_COMPUTE_TILE_IMPROVEMENTS == 0)
		{
			t1 = GetTickCount();
			DPRINTF(k_DBG_AI, ("\n\n"));
			DPRINTF(k_DBG_AI, ("//\n"));
			DPRINTF(k_DBG_AI, ("// PLACE TILE IMPROVEMENTS -- Turn %d\n", round));
			DPRINTF(k_DBG_AI, ("//							  Player %d\n", player));
			
			
			Governor::GetGovernor(player).PlaceTileImprovements();
			t2 = GetTickCount();
			DPRINTF(k_DBG_AI, ("//  elapsed time = %d ms\n", (t2 - t1)  ));
		}
	}
	
	
	if (player_ptr->m_playerType == PLAYER_TYPE_ROBOT && !g_network.IsClient()) 
	{
		t1 = GetTickCount();
		DPRINTF(k_DBG_AI, ("\n\n"));
		DPRINTF(k_DBG_AI, ("//\n"));
		DPRINTF(k_DBG_AI, ("// SET MILITARY READINESS -- Turn %d\n", round));
		DPRINTF(k_DBG_AI, ("//						     Player %d\n", player));

		READINESS_LEVEL level = 
			(READINESS_LEVEL) Governor::GetGovernor(player).ComputeBestMilitaryReadiness();

		player_ptr->SetReadinessLevel(level, FALSE);
		
		t2 = GetTickCount();
		DPRINTF(k_DBG_AI, ("//  elapsed time = %d ms\n", (t2 - t1)  ));

		t1 = GetTickCount();
		DPRINTF(k_DBG_AI, ("\n\n"));
		DPRINTF(k_DBG_AI, ("//\n"));
		DPRINTF(k_DBG_AI, ("// ADJUST SLIDER SETTINGS -- Turn %d\n", round));
		DPRINTF(k_DBG_AI, ("//						     Player %d\n", player));

		Governor::SlidersSetting sliders_setting;
		
		
		Governor::GetGovernor(player).OptimizeSliders(sliders_setting);
		Governor::GetGovernor(player).SetSliders(sliders_setting, true);

		t2 = GetTickCount();
		DPRINTF(k_DBG_AI, ("//  elapsed time = %d ms\n", (t2 - t1)  ));
	
		t1 = GetTickCount();
		DPRINTF(k_DBG_AI, ("\n\n"));
		DPRINTF(k_DBG_AI, ("//\n"));
		DPRINTF(k_DBG_AI, ("// COMPUTE GOODS TRADE ROUTES -- Turn %d\n", round));
		DPRINTF(k_DBG_AI, ("//				 			     Player %d\n", player));
	
		
		Governor::GetGovernor(player).ManageGoodsTradeRoutes();
	
		t2 = GetTickCount();
		DPRINTF(k_DBG_AI, ("//  elapsed time = %d ms\n", (t2 - t1)  ));
	}
	

	AddSettleTargets(player);

	
	AddExploreTargets(player);

	
	AddMiscMapTargets(player);

	
	if (player == 0 && player_ptr->m_playerType == PLAYER_TYPE_ROBOT)
	{
		Unit city;
		Player * player_ptr = g_player[player];
		Assert(player_ptr != NULL);
		sint32 num_cities = player_ptr->m_all_cities->Num();

		for (sint32 i = 0; i < num_cities; i++)
		{
			city = player_ptr->m_all_cities->Access(i);
			Assert( g_theUnitPool->IsValid(city) );

			
			CtpAi::SellRandomBuildings(city, 0.3);
		}
	}
}

//----------------------------------------------------------------------------
//
// Name       : MoveOutofCityTransportUnits
//
// Description: Move the smallest transport army out of every city?
//
// Parameters : playerId	: index of - computer - player
//
// Globals    : g_player
//				g_theWorld
//				g_gevManager
//
// Returns    : -
//
// Remark(s)  : Assumption: playerId points to a valid computer player.
//
//----------------------------------------------------------------------------
void CtpAi::MoveOutofCityTransportUnits(const PLAYER_INDEX playerId)
{
	Player *		player_ptr	= g_player[playerId];
	sint32 const	num_cities	= player_ptr->m_all_cities->Num();
	CellUnitList	garrison;

	for (sint32 i = 0; i < num_cities; ++i)
	{
		Unit		city		= player_ptr->m_all_cities->Access(i);
		Assert(city.IsValid());
		Assert(city.GetCityData());

		MapPoint	pos(city.RetPos());
		g_theWorld->GetArmy(pos, garrison);
		
        Army		move_army;
        sint32		min_size	= k_MAX_ARMY_SIZE;

        for (sint32 j = 0; j < garrison.Num(); ++j)
        {
			Unit const &	candidate	= garrison[j];

            if (candidate.IsValid())
			{
				Army const &	candidateArmy	= candidate.GetArmy();
				
				if (candidateArmy.IsValid()				&& 
					candidateArmy.CanTransport()		&&
					(candidateArmy.Num() < min_size)
				   )
				{
					move_army	= candidateArmy;
					min_size	= candidateArmy.Num();
				}
			}
		}

        if (move_army.IsValid())
        {
	        bool found = false;

		    for (int dir = 0; !found && (dir < NOWHERE); ++dir)
			{
				MapPoint	dest;

				if (pos.GetNeighborPosition
						(static_cast<WORLD_DIRECTION>(dir), dest) &&
					move_army.CanEnter(dest)
				   )
				{
					Path * tmpPath = new Path;
					tmpPath->SetStart(pos);
					tmpPath->AddDir(static_cast<WORLD_DIRECTION>(dir));
					tmpPath->Start(pos);

					g_gevManager->AddEvent(GEV_INSERT_Tail, GEV_MoveOrder,
										   GEA_Army,		move_army,
										   GEA_Path,		tmpPath,
										   GEA_MapPoint,	dest,
										   GEA_Int,			FALSE,
										   GEA_End
										  );

#ifdef _DEBUG
                    uint8 const	magnitude	= 255;
                    g_graphicsOptions->AddTextToArmy
						(move_army, "Transport OutOfCity", magnitude);
#endif
                    found = true;
                }
			} // for dir
		}
	} // for i
}

//----------------------------------------------------------------------------
//
// Name       : UnGroupGarrisionUnits
//
// Description: -
//
// Parameters : playerId	: index of - computer - player
//
// Globals    : -
//
// Returns    : -
//
// Remark(s)  : Assumption: playerId points to a valid computer player.
//
//----------------------------------------------------------------------------
void CtpAi::UnGroupGarrisonUnits(const PLAYER_INDEX playerId)
{
	Player *		player_ptr = g_player[playerId];
	sint32 const	num_cities = player_ptr->m_all_cities->Num();

	CellUnitList	garrison;

	for (sint32 i = 0; i < num_cities; ++i)
	{
		Unit		city		= player_ptr->m_all_cities->Access(i);
		Assert(city.IsValid());
		Assert(city.GetCityData());

		MapPoint	pos(city.RetPos());
		g_theWorld->GetArmy(pos, garrison);
       
        sint32		min_size	= k_MAX_ARMY_SIZE;

		for (sint32 j = 0; j < garrison.Num(); ++j)
        {
			//there is a problem to determine if units are in garrison or just pathing through the city
			// If fear it will also ungroup all the units of the tile (garrison points on a cellunitlist that
			// cannot know which units is group tu another.
			// Don't really affect the game, as units with a real goal and rallyrfirst will regroup them to continue... (I hope...) - Calvitix
			Unit const &	candidate	= garrison[j];

            if (candidate.IsValid())
			{
				Army const &	candidateArmy	= candidate.GetArmy();
				
				if (candidateArmy.IsValid()				&& 
					(candidateArmy.Num() > 1)			&&
					(candidateArmy.IsEntrenched() || candidateArmy.IsEntrenching())
				   )
				{
					g_gevManager->AddEvent(GEV_INSERT_Tail, GEV_UngroupOrder, 
										   GEA_Army,		candidateArmy, 
										   GEA_End
										  );
				}
        	}
        } // for j
	} // for i
}


void CtpAi::MakeRoomForNewUnits(const PLAYER_INDEX playerId)
{
	
	Player * player_ptr = g_player[playerId];
	Assert(player_ptr != NULL);
	sint32 num_cities = player_ptr->m_all_cities->Num();
	
	CellUnitList garrison;
	Unit city;
	Army move_army;
	sint8 min_size;
	
	sint32 i,j;
	MapPoint pos,dest;
	for (i = 0; i < num_cities; i++)
	{
		city = player_ptr->m_all_cities->Access(i);
		Assert( g_theUnitPool->IsValid(city) );
		Assert( city->GetCityData() != NULL );
		
		pos = city.RetPos();
		g_theWorld->GetArmy(pos, garrison);
		if ((garrison.Num() >= k_MAX_ARMY_SIZE) &&
			city.CD()->GetBuildQueue() &&
			city.CD()->GetBuildQueue()->GetLen() > 0 &&
			city.CD()->GetBuildQueue()->GetHead() &&
			city.CD()->GetBuildQueue()->GetHead()->m_category == k_GAME_OBJ_TYPE_UNIT)
		{
			
			move_army.m_id = 0x0;
			min_size = k_MAX_ARMY_SIZE;
			for (j = 0; j < garrison.Num(); j++)
			{
				if (garrison.Access(j).GetArmy().Num() < min_size)
				{
					min_size = garrison.Access(j).GetArmy().Num();
					move_army = garrison.Access(j).GetArmy();
				}
			}
			
			
			if (move_army.m_id == 0x0)
			{
				Assert(0);
				continue;
			}
			
			
			bool found = false;
			for (j = 0; j < NOWHERE && !found; j++)
			{
				if(pos.GetNeighborPosition((WORLD_DIRECTION)j, dest)) {
					if (move_army.CanEnter(dest)) 
					{
						
						Path *tmpPath = new Path;
						tmpPath->SetStart(pos);
						tmpPath->AddDir((WORLD_DIRECTION)j);
						tmpPath->Start(pos); 

						
						g_gevManager->AddEvent(GEV_INSERT_Tail, GEV_MoveOrder, 
							GEA_Army, move_army,	
							GEA_Path, tmpPath,		
							GEA_MapPoint, dest,
							GEA_Int, FALSE, 
							GEA_End);

#ifdef _DEBUG
	uint8 magnitude = 255.0;
	g_graphicsOptions->AddTextToArmy(move_army, "MakeRoom", magnitude);
#endif

						found = true;
					}
				}
			}
			
		}
	}
}


void CtpAi::FinishBeginTurn(const PLAYER_INDEX player) 
{

	time_t t1;
	time_t t2;

	sint32 round;
	Assert(player < s_maxPlayers);
	if (g_player[player] != NULL)
		round = g_player[player]->GetCurRound();
	else
		return;

	if(g_network.IsActive() && !g_network.IsLocalPlayer(player))
		return;

	
	t1 = GetTickCount();
	DPRINTF(k_DBG_AI, ("\n\n"));
	DPRINTF(k_DBG_AI, ("//\n"));
	DPRINTF(k_DBG_AI, ("// FILL EMPTY BUILD QUEUES -- Turn %d\n", round));
	DPRINTF(k_DBG_AI, ("//                    Player %d\n", player));
	if (Player::IsThisPlayerARobot(player))
	{
		
   	   CtpAi::MakeRoomForNewUnits(player);
#if 0
	   // No idea if this should be done like this, 
	   // transport can also move out sleeping units
       //to execute the new action :
       CtpAi::MoveOutofCityTransportUnits(player);

       CtpAi::UnGroupGarrisonUnits(player);	
#endif
	}
	
	Governor::GetGovernor(player).FillEmptyBuildQueues();
	t2 = GetTickCount();
	DPRINTF(k_DBG_AI, ("//  elapsed time = %d ms\n", (t2 - t1)  ));

	t1 = GetTickCount();
	DPRINTF(k_DBG_AI, ("\n\n"));
	DPRINTF(k_DBG_AI, ("//\n"));
	DPRINTF(k_DBG_AI, ("// COMPUTE GARRISONS -- Turn %d\n", round));
	DPRINTF(k_DBG_AI, ("//                      Player %d\n", player));

	
	
	
	CtpAi::ComputeCityGarrisons(player);
	t2 = GetTickCount();
	DPRINTF(k_DBG_AI, ("//  elapsed time = %d ms\n", (t2 - t1)  ));

	t1 = GetTickCount();
	DPRINTF(k_DBG_AI, ("\n\n"));
	DPRINTF(k_DBG_AI, ("//\n"));
	DPRINTF(k_DBG_AI, ("// ASSIGN POPULATIONS -- Turn %d\n", round));
	DPRINTF(k_DBG_AI, ("//						 Player %d\n", player));
	Governor::GetGovernor(player).AssignPopulations();
	t2 = GetTickCount();
	DPRINTF(k_DBG_AI, ("//  elapsed time = %d ms\n", (t2 - t1)  ));
}

void CtpAi::NetworkClientBeginTurn(PLAYER_INDEX player)
{
	
	if(g_network.IsClient() && g_network.IsLocalPlayer(player) &&
	   Player::IsThisPlayerARobot(player)) {
		Player *player_ptr = g_player[player];

		sint32 government_type = Governor::GetGovernor(player).ComputeBestGovernment();
		if (government_type >= 0 && government_type != player_ptr->GetGovernmentType())
			
			player_ptr->SetGovernmentType(government_type);
		
		{
			sint32 pw_percent;
			sint32 sci_percent;
			
			const StrategyRecord & strategy = 
				Diplomat::GetDiplomat(player).GetCurrentStrategy();

			strategy.GetPublicWorksPercent(pw_percent);
			strategy.GetSciencePercent(sci_percent);
			
			player_ptr->SetMaterialsTax(pw_percent/100.0);
			player_ptr->SetTaxes(sci_percent/100.0);
			
			DPRINTF(k_DBG_AI, ("\n\n"));
			DPRINTF(k_DBG_AI, ("//\n"));
			DPRINTF(k_DBG_AI, ("// SET RESEARCH -- Turn %d\n", g_player[player]->m_current_round));
			DPRINTF(k_DBG_AI, ("//                 Player %d\n", player));
			
			
			SetResearch(player);
		}

		DPRINTF(k_DBG_AI, ("\n\n"));
		DPRINTF(k_DBG_AI, ("//\n"));
		DPRINTF(k_DBG_AI, ("// SET MILITARY READINESS -- Turn %d\n", g_player[player]->m_current_round));
		DPRINTF(k_DBG_AI, ("//						     Player %d\n", player));
		
		READINESS_LEVEL level = 
			(READINESS_LEVEL) Governor::GetGovernor(player).ComputeBestMilitaryReadiness();
		
		player_ptr->SetReadinessLevel(level, FALSE);
		
		DPRINTF(k_DBG_AI, ("\n\n"));
		DPRINTF(k_DBG_AI, ("//\n"));
		DPRINTF(k_DBG_AI, ("// ADJUST SLIDER SETTINGS -- Turn %d\n", g_player[player]->m_current_round));
		DPRINTF(k_DBG_AI, ("//						     Player %d\n", player));

		Governor::SlidersSetting sliders_setting;
		
		
		Governor::GetGovernor(player).OptimizeSliders(sliders_setting);
		Governor::GetGovernor(player).SetSliders(sliders_setting, true);
		
		DPRINTF(k_DBG_AI, ("\n\n"));
		DPRINTF(k_DBG_AI, ("//\n"));
		DPRINTF(k_DBG_AI, ("// COMPUTE GOODS TRADE ROUTES -- Turn %d\n", g_player[player]->m_current_round));
		DPRINTF(k_DBG_AI, ("//				 			     Player %d\n", player));
	
		
		Governor::GetGovernor(player).ManageGoodsTradeRoutes();
	}
}


void CtpAi::Resize() 
{
	
	sint32 old_player_count = s_maxPlayers;
	s_maxPlayers = 0;
	sint32 index;
	for(index = 0; index < k_MAX_PLAYERS; index++) {
		if (g_player[index] != NULL)
			{
				
				if (index + 1 > s_maxPlayers)
					s_maxPlayers = index + 1;
			}
	}

	
	Assert(s_maxPlayers > 0);
	Scheduler::ResizeAll(s_maxPlayers);

	
	AgreementMatrix::s_agreements.Resize(s_maxPlayers);

	
	Diplomat::ResizeAll(s_maxPlayers);

	
	
	sint32 resolution = 10;

	
	MapAnalysis::GetMapAnalysis().Resize( s_maxPlayers,
										  (sint16) g_theWorld->GetWidth(), 
										  (sint16) g_theWorld->GetHeight(), 
										  resolution);
	
	Governor::ResizeAll(s_maxPlayers);

	
}


void CtpAi::HandleMoveFailure(const Army & army, const MapPoint & pos)
{
	
	if ( army->CanAtLeastOneCargoUnloadAt(army->RetPos(), pos, false) )
	{
		
		g_gevManager->AddEvent(GEV_INSERT_Tail, GEV_UnloadOrder,
							   GEA_Army, army,
							   GEA_MapPoint, pos,
							   GEA_End);

	}
}


void CtpAi::AddExploreTargets(const PLAYER_INDEX playerId)
{
	
	Scheduler & scheduler = Scheduler::GetScheduler(playerId);
	const StrategyRecord & strategy = 
		Diplomat::GetDiplomat(playerId).GetCurrentStrategy();	

	Player *player_ptr = g_player[playerId];
	Assert(player_ptr);

	CTPGoal_ptr goal_ptr;

//Added by Martin Gühmann explore resolution is now constant
    sint16 explore_res = EXPLORE_RESOLUTION;
	GOAL_TYPE goal_type;
	const StrategyRecord::GoalElement *goal_element_ptr;
	sint16 goal_element;
	for (goal_element = 0; goal_element < strategy.GetNumGoalElement(); goal_element++) 
		{
			goal_element_ptr = strategy.GetGoalElement(goal_element);
			goal_type = goal_element_ptr->GetGoalIndex();

			
			Assert(goal_type >= 0);

			
			if ( !g_theGoalDB->Get(goal_type)->GetTargetTypeUnexplored() )
				continue;

//Add goals every turn (and not just when there isn't anymore (if one goal remain and isn't satisfied,
// it can freeze all the goals of this type) - Calvitix
			if (scheduler.CountGoalsOfType(goal_type) > (goal_element_ptr->GetMaxEval()/3))
				continue;
			
			
			if (g_player[playerId]->m_civilisation->GetCivilisation() == 0)
				continue;
			
			MapPoint pos;
			for ( pos.x = 0; pos.x <  g_theWorld->GetWidth(); pos.x += explore_res) 
				{
					for (pos.y = 0; pos.y <  g_theWorld->GetHeight(); pos.y += explore_res) 
						{
							
							if ( player_ptr->IsExplored(pos) == TRUE)
								continue;
							goal_ptr = new CTPGoal();
							goal_ptr->Set_Type( goal_type );
							goal_ptr->Set_Player_Index( playerId );
							goal_ptr->Set_Target_Pos( pos );

							scheduler.Add_New_Goal( goal_ptr );
						} 
				} 
		} 
}


void CtpAi::AddSettleTargets(const PLAYER_INDEX playerId)
{
	
	Scheduler & scheduler = Scheduler::GetScheduler(playerId);
	const StrategyRecord & strategy = 
		Diplomat::GetDiplomat(playerId).GetCurrentStrategy();

	Player *player_ptr = g_player[playerId];
	Assert(player_ptr);
	if (player_ptr == NULL)
		return;


	SettleMap::SettleTargetList targets;
	SettleMap::s_settleMap.GetSettleTargets(playerId, targets);

	
	SettleMap::SettleTarget settle_target;
	CTPGoal_ptr goal_ptr;

	SettleMap::SettleTargetList::iterator iter;
	sint32 desired_goals;
	sint32 max_desired_goals;
	uint8 magnitude;
	char buf[10];

	
	GOAL_TYPE goal_type;
	const StrategyRecord::GoalElement *goal_element_ptr;
	sint16 goal_element;
	for (goal_element = 0; goal_element < strategy.GetNumGoalElement(); goal_element++) 
		{
			goal_element_ptr = strategy.GetGoalElement(goal_element);
			goal_type = goal_element_ptr->GetGoalIndex();

			
			Assert(goal_type >= 0);

			
			if ( !g_theGoalDB->Get(goal_type)->GetTargetTypeSettleLand() &&
				 !g_theGoalDB->Get(goal_type)->GetTargetTypeSettleSea() )
				continue;

			max_desired_goals = goal_element_ptr->GetMaxEval() -
				scheduler.CountGoalsOfType(goal_type);

			desired_goals = max_desired_goals;

			
			for (iter = targets.begin(); 
				 iter != targets.end() && (desired_goals > 0); 
				 iter++)
				{
					settle_target = *iter;

					
					if ( (g_theWorld->IsWater(settle_target.m_pos) == FALSE) && 
						 (g_theGoalDB->Get(goal_type)->GetTargetTypeSettleLand()) ||
						 (g_theWorld->IsWater(settle_target.m_pos) == TRUE) && 
						 (g_theGoalDB->Get(goal_type)->GetTargetTypeSettleSea()))
						{
							goal_ptr = new CTPGoal();
							goal_ptr->Set_Type( goal_type );
							goal_ptr->Set_Player_Index( playerId );
							goal_ptr->Set_Target_Pos( settle_target.m_pos );

							scheduler.Add_New_Goal( goal_ptr );

							magnitude = (uint8) (((max_desired_goals - desired_goals)* 255.0) / max_desired_goals);
							sprintf(buf,"%4.0f",settle_target.m_value);
							g_graphicsOptions->AddTextToCell(settle_target.m_pos, buf, magnitude);
							
							
							desired_goals--;
						}
				}
	}
}


void CtpAi::AddMiscMapTargets(const PLAYER_INDEX playerId)
{
	
	Scheduler & scheduler = Scheduler::GetScheduler(playerId);
	const StrategyRecord & strategy = 
		Diplomat::GetDiplomat(playerId).GetCurrentStrategy();	

	Player *player_ptr = g_player[playerId];
	Assert(player_ptr);

	CTPGoal_ptr goal_ptr;

	
	GOAL_TYPE goal_type;
	const StrategyRecord::GoalElement *goal_element_ptr;
	sint16 goal_element;
	for (goal_element = 0; goal_element < strategy.GetNumGoalElement(); goal_element++) 
		{
			goal_element_ptr = strategy.GetGoalElement(goal_element);
			goal_type = goal_element_ptr->GetGoalIndex();

			
			Assert(goal_type >= 0);

			
			if ( !g_theGoalDB->Get(goal_type)->GetTargetTypeGoodyHut() &&
				 !g_theGoalDB->Get(goal_type)->GetTargetTypeChokePoint())
				 continue;

            //Add goals if there is only half or less goals remaining (and not just when there isn't anymore (if one goal remain and isn't satisfied,
            // it can freeze all the goals of this type) - Calvitix
			if (scheduler.CountGoalsOfType(goal_type) > (goal_element_ptr->GetMaxEval()/3))
				continue;					

			
			if (g_player[playerId]->m_civilisation->GetCivilisation() == 0)
				continue;

			MapPoint pos;
			for ( pos.x = 0; pos.x < g_theWorld->GetWidth(); pos.x++) 
				{
					for (pos.y = 0; pos.y < g_theWorld->GetHeight(); pos.y++) 
						{
							Cell *cell = g_theWorld->GetCell(pos);
							if (cell->GetIsChokePoint() &&
								g_theGoalDB->Get(goal_type)->GetTargetTypeChokePoint())
							{
								goal_ptr = new CTPGoal();
								goal_ptr->Set_Type( goal_type );
								goal_ptr->Set_Player_Index( playerId );
								goal_ptr->Set_Target_Pos( pos );

								scheduler.Add_New_Goal( goal_ptr );
							}

							if (cell->GetGoodyHut() != NULL)
							{
								goal_ptr = new CTPGoal();
								goal_ptr->Set_Type( goal_type );
								goal_ptr->Set_Player_Index( playerId );
								goal_ptr->Set_Target_Pos( pos );

								scheduler.Add_New_Goal( goal_ptr );
							}
						} 
				} 
		} 
}

	
void CtpAi::ComputeCityGarrisons(const PLAYER_INDEX playerId )
{
	sint32 committed_units = 0;

	Unit city;
	MapPoint pos;
	Army army;
	double prev_city_defense;
	double threat;
	double defense_bonus;
	sint32 slave_garrison;
	sint32 min_garrison;
	sint8 prev_garrison;
	sint32 offensive_garrison;
	sint32 defensive_garrison;
	sint32 ranged_garrison;
	Player *player_ptr = g_player[playerId];
	sint32 hp;
	sint32 defense_count;
	sint32 ranged_count;
	sint32 attack_strength;
	sint32 defense_strength;
	sint32 ranged_strength;
	sint32 total_value;
	
	Assert(player_ptr != NULL);
	sint32 num_armies = player_ptr->m_all_armies->Num();
	sint32 num_cities = player_ptr->m_all_cities->Num();
	
	
	const StrategyRecord & strategy = 
		Diplomat::GetDiplomat(playerId).GetCurrentStrategy();

	strategy.GetOffensiveGarrisonCount(offensive_garrison);
	strategy.GetDefensiveGarrisonCount(defensive_garrison);
	strategy.GetRangedGarrisonCount(ranged_garrison);
	min_garrison = offensive_garrison +
				   defensive_garrison +
				   ranged_garrison;

	sint32 i;
	for (i = 0; i < num_cities; i++)
	{
		city = player_ptr->m_all_cities->Access(i);
		Assert( g_theUnitPool->IsValid(city) );
		Assert( city->GetCityData() != NULL );

		pos = city.RetPos();

		
		slave_garrison = city->GetCityData()->SlaveCount() / 2;

		
		threat = MapAnalysis::GetMapAnalysis().GetThreat(playerId, pos);

		
		const StrategyRecord::ForceMatch *defense_force_match;
		strategy.GetDefensive(defense_force_match);
		threat *= defense_force_match->GetDefenseMatch();

		
		city->GetCityData()->SetNeededGarrisonStrength(threat);

		
		if ( slave_garrison > min_garrison )
			city->GetCityData()->SetNeededGarrison(slave_garrison);
		else
			city->GetCityData()->SetNeededGarrison(min_garrison);

		
		city->GetCityData()->SetCurrentGarrisonStrength(0.0);
		city->GetCityData()->SetCurrentGarrison(0);
	}

	
	for (i = 0; i < num_armies; i++)
	{
		army = player_ptr->m_all_armies->Access(i);
		Assert( g_theArmyPool->IsValid(army) );

		if (army->NumOrders() > 0)
			continue;

		
		pos = army->RetPos();
		city = g_theWorld->GetCity(pos);
		if (city.m_id == 0)
			continue;

		
		sint32 transports,max,empty;
		if (army->GetCargo(transports, max, empty))
			continue;

		
		defense_bonus = city.GetDefendersBonus();

		army->GetArmyStrength(hp,
			defense_count,
			ranged_count,
			attack_strength,
			defense_strength,
			ranged_strength,
			total_value);
		
		
		defense_strength += city->GetCityData()->GetDefendersBonus() * defense_count;

		
		prev_city_defense = city->GetCityData()->GetCurrentGarrisonStrength();
		city->GetCityData()->SetCurrentGarrisonStrength( prev_city_defense + defense_strength );
		prev_garrison = city->GetCityData()->GetCurrentGarrison();
		city->GetCityData()->SetCurrentGarrison( prev_garrison + defense_count );
	}
}

//PFT 12 apr 05: no longer used
void CtpAi::BombardAdjacentEnemies(const Army & army)
{
	if (!army->CanBombard())
		return;

	
	if (!army->CanPerformSpecialAction())
		return;

	sint8 dir;
	MapPoint adj;
	Unit city;
	PLAYER_INDEX playerId = army.GetOwner();
	MapPoint pos = army->RetPos();
	
	
	for (dir = 0; dir < (sint8) NOWHERE; dir++)
		{
			if(pos.GetNeighborPosition((WORLD_DIRECTION)dir, adj))
				{
					city = g_theWorld->GetCity(adj);
			
					if (city.m_id != 0x0 &&
						army->CanBombard(adj) &&
						AgreementMatrix::s_agreements.
						  HasAgreement(playerId, 
									   city.GetOwner(), 
									   PROPOSAL_TREATY_DECLARE_WAR))	
						{
							g_gevManager->AddEvent( GEV_INSERT_Tail, 
													GEV_BombardOrder,
													GEA_Army, army.m_id,
													GEA_MapPoint, adj,
													GEA_End);
							
							return;
						}
				}
		}

	
	CellUnitList enemy_army;
	for (dir = 0; dir < (sint8) NOWHERE; dir++)
		{
			if(pos.GetNeighborPosition((WORLD_DIRECTION)dir, adj))
				{
					g_theWorld->GetArmy(adj, enemy_army);
					if (enemy_army.Num() > 0 &&
						army->CanBombard(adj) &&
						
						enemy_army.IsVisible(army.GetOwner()) &&
						AgreementMatrix::s_agreements.
						  HasAgreement(playerId, 
									   enemy_army[0].GetOwner(), 
									   PROPOSAL_TREATY_DECLARE_WAR))
						{
							g_gevManager->AddEvent( GEV_INSERT_Tail, 
													GEV_BombardOrder,
													GEA_Army, army.m_id,
													GEA_MapPoint, adj,
													GEA_End);
	
							
							return;
						}
				}
		}
}



bool CtpAi::GetNearestAircraftCarrier(const Army & army, MapPoint & carrier_pos, double & distance)
{

	Player *player_ptr = g_player[army->GetOwner()];
	Assert(player_ptr);

	sint32 num_armies = player_ptr->m_all_armies->Num();
	sint32 max_squared_dist = (g_theWorld->GetWidth() * g_theWorld->GetHeight());
	max_squared_dist *= max_squared_dist;
	sint32 squared_distance = max_squared_dist;
	double tmp_squared_distance;
	Army tmp_army;

	
	
	for (sint16 i = 0; i < num_armies; i++)
	{
		tmp_army = player_ptr->m_all_armies->Access(i);
		Assert( g_theArmyPool->IsValid(tmp_army) );
		if ( !g_theArmyPool->IsValid(tmp_army) )
			continue;

		
		if (tmp_army->NumUnitsCanMoveIntoThisTransport(*tmp_army.GetData()) != tmp_army->Num())
			continue;
			
		tmp_squared_distance = MapPoint::GetSquaredDistance(tmp_army->RetPos(), army->RetPos());
		if (tmp_squared_distance < squared_distance)
		{
			squared_distance = tmp_squared_distance;
			carrier_pos = tmp_army->RetPos();
		}
	}

	return (squared_distance < max_squared_dist);
}


bool CtpAi::GetNearestRefuel(const Army & army, const MapPoint & start_pos, MapPoint & refueling_pos, sint32 & refueling_distance)
{
	Player *player = g_player[army->GetOwner()];
	Assert(player);

	sint32 num_tiles_to_half;
	sint32 num_tiles_to_empty;
	army->CalcRemainingFuel(num_tiles_to_half, num_tiles_to_empty);

	
	num_tiles_to_empty /= k_MOVE_AIR_COST;
	num_tiles_to_half /= k_MOVE_AIR_COST;

	
	refueling_distance = -1;

	MapPoint tmp_pos;
	double distance;
	Army carrier;
	bool found = false;

	
	Unit city;
	if (player->GetNearestCity(start_pos, city, distance))
	{
		refueling_distance = distance;
		refueling_pos = city.RetPos();
		found = (refueling_distance < num_tiles_to_empty);
	}

	
	if (!found && CtpAi::GetNearestAircraftCarrier(army, tmp_pos, distance))
	{
		distance = sqrt(distance);
		
		if (refueling_distance < 0 || distance < refueling_distance)
		{
			refueling_distance = distance;
			refueling_pos = tmp_pos;
			found = (refueling_distance < num_tiles_to_empty);
		}
	}

	
	if (!found && player->GetNearestAirfield(start_pos, tmp_pos))
	{
		distance = MapPoint::GetSquaredDistance(start_pos, tmp_pos);
		distance = sqrt(distance);
		if (refueling_distance < 0 || distance < refueling_distance)
		{
			refueling_distance = distance;
			refueling_pos = tmp_pos;
		}
	}

	
	if (refueling_distance < 0)
	{
		bool NO_REFUEL_DESTINATION = false;
		Assert(NO_REFUEL_DESTINATION);
		return false;
	}
	return true;
}


void CtpAi::RefuelAirplane(const Army & army)
{
	sint32 num_tiles_to_half;
	sint32 num_tiles_to_empty;
	army->CalcRemainingFuel(num_tiles_to_half, num_tiles_to_empty);
	MapPoint pos;

	
	num_tiles_to_empty /= k_MOVE_AIR_COST;
	num_tiles_to_half /= k_MOVE_AIR_COST;

	
	if (num_tiles_to_half > 0 && army->GetNextPathPoint(pos))
		return;
	
	
	Unit city = g_theWorld->GetCity(army->RetPos());
	if (city.m_id != 0)
		return;

	MapPoint start_pos = army->RetPos();
	MapPoint refueling_pos;
	sint32 refueling_distance;

	
	if (!CtpAi::GetNearestRefuel(army, start_pos, refueling_pos, refueling_distance))
		return;
	
	
	
	double trans_max_r = 0.8;
	Path new_path;
	float total_cost;
	if (! RobotAstar2::s_aiPathing.FindPath(RobotAstar2::PATH_TYPE_DEFAULT,
		army,
		army->GetMovementType(),
		start_pos,
		refueling_pos,
		true, 
		g_theWorld->GetContinent(refueling_pos),
		trans_max_r,
		new_path,
		total_cost))
	{
		
		bool NO_REFUEL_PATH = false;
		Assert(NO_REFUEL_PATH);
		return;
	}
	
	
	Path *tmpPath = new Path(new_path);
	MapPoint target_pos = tmpPath->GetEnd();
	
	
	g_gevManager->AddEvent(GEV_INSERT_Tail, GEV_MoveOrder, 
		GEA_Army, army,			
		GEA_Path, tmpPath,		
		GEA_MapPoint, target_pos,
		GEA_Int, FALSE, 
		GEA_End);
	
#ifdef _DEBUG
	uint8 magnitude = 255.0;
	g_graphicsOptions->AddTextToArmy(army, "Refuel", magnitude);
#endif
	
}



void CtpAi::ExecuteOpportunityActions(const PLAYER_INDEX player)
{
	Player *player_ptr = g_player[player];
	Assert(player_ptr);
	if (player_ptr == NULL)
		return;
	sint32 num_armies = player_ptr->m_all_armies->Num();
	sint32 num_cities = player_ptr->m_all_cities->Num();
	Army army;

	
	
	for (sint16 i = 0; i < num_armies; i++)
	{
		army = player_ptr->m_all_armies->Access(i);
		Assert( g_theArmyPool->IsValid(army) );
		if ( !g_theArmyPool->IsValid(army) )
			continue;

		//first get max bombard rge
		sint32 min_rge, max_rge = 0;
        army->GetBombardRange(min_rge, max_rge);

		if(max_rge && army->NumOrders() == 0){//army is available and can bombard, so find enemies

            CtpAi::BombardNearbyEnemies(army, max_rge);

		}


		
		if (army->GetMinFuel() != 0x7fffffff &&
			army->GetOrder(0) == NULL)
		{
				CtpAi::RefuelAirplane(army);
				continue;
		}

		if (army->NumOrders() > 0)
			continue;

		//we need to find something more interesting to do here
		g_gevManager->AddEvent( GEV_INSERT_Tail, 
								GEV_EntrenchOrder,
								GEA_Army, army.m_id,
								GEA_End);
	}
	CtpAi::SpendGoldToRushBuy(player);
}


const OrderRecord * CtpAi::GetUnloadOrder()
{
	return sm_unloadOrderRec;
}

const OrderRecord * CtpAi::GetSpaceLaunchOrder()
{
	return sm_spaceLaunchOrderRec;
}

const OrderRecord * CtpAi::GetDisbandArmyOrder()
{
	return sm_disbandArmyOrderRec;
}

const OrderRecord * CtpAi::GetMoveOrder()
{
	return sm_moveOrderRec;
}


const sint32 CtpAi::GetGoalDefendIndex()
{
	return sm_goalDefendIndex;
}

const sint32 CtpAi::GetGoalSeigeIndex()
{
	return sm_goalSeigeIndex;
}

const sint32 CtpAi::GetEndgameWorldUnionIndex()
{
	return sm_endgameWorldUnionIndex;
}


void CtpAi::SetResearch(const PLAYER_INDEX player)
{
	const StrategyRecord & strategy = 
		Diplomat::GetDiplomat(player).GetCurrentStrategy();
	Player *player_ptr = g_player[player];
	Assert(player_ptr);
	Assert(player_ptr->m_advances);
	if (player_ptr == NULL || player_ptr->m_advances == NULL)
		return;

	const AdvanceListRecord * advance_list = 
		strategy.GetResearchPtr();
	Assert(advance_list);
	if (advance_list == NULL)
		return;

	AdvanceType advance_index = -1;
	for (sint32 i = 0; i < advance_list->GetNumAdvance(); i++)
	{
		advance_index = advance_list->GetAdvance(i)->GetIndex();
		if (player_ptr->m_advances->CanResearch(advance_index))
		{

			
			bool stop_research = false;
			sint32 foreignerId;
			for (foreignerId = 1; foreignerId < CtpAi::s_maxPlayers; foreignerId++)
			{
				const ai::Agreement	& agreement = 
					AgreementMatrix::s_agreements.GetAgreement(player, foreignerId, PROPOSAL_OFFER_STOP_RESEARCH);
				
				
				if (agreement.start != -1 && agreement.end == -1)
				{
					if (agreement.proposal.first_arg.advanceType == advance_index)
					{
						stop_research = true;
						break;
					}
				}
			}
			
			
			if (stop_research)
			{
				sint32 duration = 
					AgreementMatrix::s_agreements.GetAgreementDuration(player, foreignerId, PROPOSAL_OFFER_STOP_RESEARCH);

				
				if (Diplomat::GetDiplomat(player).GetPersonality()->GetTrustworthinessChaotic())
					break;

				
				else if (Diplomat::GetDiplomat(player).GetPersonality()->GetTrustworthinessLawful() &&
					duration > 50)
					break;

				
				else if (duration > 20)
					break;
				
				continue;
			}
	
			
			break;
		}
	}

	if (advance_index < g_theAdvanceDB->NumRecords())
	{
		
		if (player_ptr->m_advances->GetResearching() != advance_index)
			player_ptr->SetResearching(advance_index);
	}
	else
	{
		
		player_ptr->SetTaxes(0.0);
	}
}


void CtpAi::SpendGoldToRushBuy(const PLAYER_INDEX player)
{
	
	list< pair<sint32, Unit> > rush_buy_list;
	pair<sint32, Unit> rush_buy;

	Player *player_ptr = g_player[player];
	Assert(player_ptr != NULL);
	sint32 num_cities = player_ptr->m_all_cities->Num();
	
	
	const StrategyRecord & strategy = 
		Diplomat::GetDiplomat(player).GetCurrentStrategy();

	
	sint32 threat_bonus = 0;
	(void) strategy.GetRushBuyThreatBonus(threat_bonus);

	double reserve_percent = 0.0;
	(void) strategy.GetRushBuyReservePercent(reserve_percent);

    sint32 lost_to_cleric;
	sint32 lost_to_crime;
	sint32 maintenance;
	sint32 wages; 
    sint32 science; 
	sint32 old_savings; 
	sint32 current_savings;
	sint32 income;
    player_ptr->m_gold->
		GetGoldLevels(&income, &lost_to_cleric, &lost_to_crime, &maintenance, 
					  &wages, &science, &old_savings, &current_savings); 

	sint32 reserve = sint32((lost_to_cleric + lost_to_crime + maintenance + wages +
		science) * reserve_percent);

	
	current_savings -= reserve;

	
	sint32 i;
	Unit city;
	for (i = 0; i < num_cities; i++)
	{
		city = player_ptr->m_all_cities->Access(i);
		Assert( g_theUnitPool->IsValid(city) );
		Assert( city->GetCityData() != NULL );

		rush_buy.first = city.CD()->HowMuchLonger();
		if (rush_buy.first > 1)
		{
			
			rush_buy.first += 
				(sint32) ceil(1.0 - MapAnalysis::GetMapAnalysis().GetThreatRank(city.CD()) * threat_bonus);
			rush_buy.second = city.m_id;

			
			rush_buy_list.push_back(rush_buy);
		}
	}
		
	
	rush_buy_list.sort();

	
	sint32 rush_buy_cost;
	list< pair<sint32, Unit> >::iterator iter;
	for (iter = rush_buy_list.begin(); iter != rush_buy_list.end(); iter++)
	{
		city = iter->second;

		rush_buy_cost = city.CD()->GetOvertimeCost();
		
		current_savings -= rush_buy_cost;

		
		if (current_savings < 0)
			break;

		
		g_gevManager->AddEvent(GEV_INSERT_Tail, GEV_BuyFront,
			GEA_City, city,
			GEA_End);
	}
}


void CtpAi::SellRandomBuildings(const Unit & city, const double chance)
{

	uint64 buildings = city.GetCityData()->GetEffectiveBuildings();
	for (sint32 which = 0; which < 64; which++)
	{
		if((buildings & ((uint64)1 << uint64(which)))) 
		{
			if(g_rand->Next(100) <= (100 * chance) ) 
			{
				g_gevManager->AddEvent(GEV_INSERT_Tail, GEV_SellBuilding,
					GEA_City, city,
					GEA_Int, which,
					GEA_End);
			}
		}
	}
}

//PFT 12 apr 05, replaces BombardAdjacentEnemies to accomodate bombarding from range
void CtpAi::BombardNearbyEnemies(Army army, sint32 max_rge)
{
    if (!army->CanBombard())
		return;

    if (!army->CanPerformSpecialAction())
		return;

    PLAYER_INDEX playerId = army->GetOwner();

    for (sint32 foreigner = 1; foreigner < CtpAi::s_maxPlayers; foreigner++) 
	{
		if (g_player[foreigner] 
			&& AgreementMatrix::s_agreements.HasAgreement(playerId, 
														  foreigner, 
														  PROPOSAL_TREATY_DECLARE_WAR))
		{  //try to bombard one of his armies or cities within max range      

			Player *foreigner_ptr = g_player[foreigner];
			Assert(foreigner_ptr);
			if (foreigner_ptr == NULL)
				return;

			sint32 num_armies = foreigner_ptr->m_all_armies->Num();

			MapPoint pos;
			army->GetPos(pos);

			Army def_army;
			MapPoint def_pos;
			sint32 i, dist = 0, min_dist = 0x7fffffff;
            //bombard the first enemy army within range
			for(i = 0; i < num_armies; i++){
				def_army = foreigner_ptr->m_all_armies->Access(i);

				//visibility check
				if( !def_army->IsVisible(playerId) )
					continue;
				
				def_army->GetPos(def_pos);

				dist = pos.NormalizedDistance(def_pos);
				if(dist < min_dist){
					min_dist = dist;
				    if(min_dist <= max_rge){

						g_gevManager->AddEvent( GEV_INSERT_Tail, 
												GEV_BombardOrder,
												GEA_Army, army.m_id,
												GEA_MapPoint,def_pos,
												GEA_End);
						return;
					}
				}
			}
	/*
	        if(min_dist <= max_rge){

				g_gevManager->AddEvent( GEV_INSERT_Tail, 
										GEV_BombardOrder,
										GEA_Army, army.m_id,
										GEA_MapPoint,def_pos,
										GEA_End);
				return;
			}
	*/		
            //bombard the first enemy city within range
			Unit def_city;
            sint32 num_cities = foreigner_ptr->m_all_cities ->Num();
            dist = 0;
			min_dist = 0x7fffffff;

            for(i = 0; i < num_cities; i++){

                def_city = foreigner_ptr->m_all_cities->Access(i);

               //should test if def_city is visible to player
                if(!(def_city->GetVisibility() & (1 << playerId)))
					continue;

				def_city->GetPos(def_pos);
                dist = pos.NormalizedDistance(def_pos);
				if(dist < min_dist){
					min_dist = dist;
				    if(min_dist <= max_rge){

						g_gevManager->AddEvent( GEV_INSERT_Tail, 
												GEV_BombardOrder,
												GEA_Army, army.m_id,
												GEA_MapPoint,def_pos,
												GEA_End);
						return;
					}
				}
			}
		}
    }
}
