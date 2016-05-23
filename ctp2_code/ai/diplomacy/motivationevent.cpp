//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : Diplomacy motivation events
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
// - None
//
//----------------------------------------------------------------------------

#include "c3.h"

#include "profileai.h"

#include "Scheduler.h"

#include "MotivationEvent.h"

#include "Events.h"
#include "GameEventUser.h"
#include "Unit.h"
#include "player.h"
#include "Strengths.h"
#include "StrDB.h"
#include "GameEventManager.h"

#include "Diplomat.h"
#include "mapanalysis.h"
#include "GoalRecord.h"
#include "Goal.h"
#include "gold.h"
#include "stringutils.h"
#include "ctpai.h"

#include "pollution.h"
extern Pollution *g_thePollution;

STDEHANDLER(MotivationEvent)
{
	sint32 playerId;

	if (!args->GetPlayer(0, playerId))
		return GEV_HD_Continue;

	Diplomat & diplomat = Diplomat::GetDiplomat(playerId);

	diplomat.SortMotivations();

	g_gevManager->AddEvent(GEV_INSERT_Tail, GEV_StartNegotiations,
						   GEA_Player, playerId,
						   GEA_End);

	return GEV_HD_Continue;
}

STDEHANDLER(ThreatenedCity_MotivationEvent)
{
	static StringId adviceId = -1;
	stringutils_SetStaticStringId(adviceId, "MOTIVATION_FEAR_CITY_DEFENSE_ADVICE");

	PLAYER_INDEX playerId;

	sint32 defend_goal_type = CtpAi::GetGoalDefendIndex();
	Assert(defend_goal_type >= 0);
	if (defend_goal_type < 0)
		return GEV_HD_Continue;

	if (!args->GetPlayer(0, playerId))
		return GEV_HD_Continue;

	Diplomat & diplomat = Diplomat::GetDiplomat(playerId);
	const Scheduler & scheduler = Scheduler::GetScheduler(playerId);
	Motivation motivation;

	sint32 at_risk_value =
		scheduler.GetValueUnsatisfiedGoals(static_cast<GOAL_TYPE>(defend_goal_type));
	Goal_ptr goal_ptr = scheduler.GetHighestPriorityGoal(static_cast<GOAL_TYPE>(defend_goal_type), false);

	sint32 cityId = 0;
	if (goal_ptr != NULL)
		cityId = goal_ptr->Get_Target_City().m_id;

	sint32 total_value = MapAnalysis::GetMapAnalysis().TotalValue(playerId);


	if (at_risk_value > total_value * 0.10)
	{
		sint32 priority;

		diplomat.GetCurrentStrategy().GetFearCityDefense(priority);
		motivation.priority = static_cast<sint16>(priority);
		motivation.type = MOTIVATION_FEAR_CITY_DEFENSE;
		motivation.arg.cityId = cityId;
		motivation.adviceStrId = adviceId;
		diplomat.ConsiderMotivation(motivation);
	}

	return GEV_HD_Continue;
}

STDEHANDLER(DesireGold_MotivationEvent)
{
	static StringId adviceId = -1;
	if (adviceId < 0)
	{
		char motivation_name[] = "MOTIVATION_DESIRE_GOLD_ADVICE";
		BOOL found =
			g_theStringDB->GetStringID(motivation_name, adviceId);
		Assert(found);
	}

	PLAYER_INDEX playerId;
	if (!args->GetPlayer(0, playerId))
		return GEV_HD_Continue;

	Diplomat & diplomat = Diplomat::GetDiplomat(playerId);

	sint32 rank = g_player[playerId]->GetRank(STRENGTH_CAT_GOLD);
	sint32 needed_reserves = g_player[playerId]->m_gold->GetIncome() * 2;
	bool low_reserves = (needed_reserves < g_player[playerId]->GetGold());
	bool capitalist_personality = (diplomat.GetPersonality()->GetDiscoveryEconomic());

	if ( rank < 75 || low_reserves || capitalist_personality)
	{
		sint32 priority;
		diplomat.GetCurrentStrategy().GetDesireGold(priority);

		Motivation motivation;
		motivation.priority = (sint16) priority;
		motivation.type = MOTIVATION_DESIRE_GOLD;
		motivation.arg.gold = needed_reserves;
		motivation.adviceStrId = adviceId;
		diplomat.ConsiderMotivation(motivation);
	}

	return GEV_HD_Continue;
}

STDEHANDLER(DesireMakeFriend_MotivationEvent)
{
	static StringId adviceId = -1;
	stringutils_SetStaticStringId(adviceId, "MOTIVATION_MAKE_FRIEND_ADVICE");

	PLAYER_INDEX playerId;

	if (!args->GetPlayer(0, playerId))
		return GEV_HD_Continue;

	if (g_player[playerId] == NULL)
		return GEV_HD_Continue;

	Diplomat & diplomat = Diplomat::GetDiplomat(playerId);
	Motivation motivation;

	sint8 friends = diplomat.GetFriendCount();
	sint32 friend_power = diplomat.GetFriendPower();
	sint32 our_power = MapAnalysis::GetMapAnalysis().TotalThreat(playerId);
	sint32 enemy_threat = diplomat.GetEnemyThreat();
	bool friendly_personality =
		diplomat.GetPersonality()->GetDiscoveryDiplomatic() ||
		diplomat.GetPersonality()->GetConquestPassive();

	if ( (friendly_personality && friends == 0) || (((friend_power + our_power) * 1.5 ) < enemy_threat))
	{
		sint32 priority;

		diplomat.GetCurrentStrategy().GetDesireMakeFriend(priority);
		motivation.priority = (sint16) priority;
		motivation.type = MOTIVATION_DESIRE_MAKE_FRIEND;
		motivation.adviceStrId = adviceId;
		diplomat.ConsiderMotivation(motivation);
	}

	return GEV_HD_Continue;
}

STDEHANDLER(StopPiracy_MotivationEvent)
{
	static StringId adviceId = -1;
	stringutils_SetStaticStringId(adviceId, "MOTIVATION_STOP_PIRACY_ADVICE");

	PLAYER_INDEX playerId;

	if (!args->GetPlayer(0, playerId))
		return GEV_HD_Continue;

	if (g_player[playerId] == NULL)
		return GEV_HD_Continue;

	Diplomat & diplomat = Diplomat::GetDiplomat(playerId);
	const MapAnalysis & map = MapAnalysis::GetMapAnalysis();
	Motivation motivation;

	Player *player_ptr = g_player[playerId];
	Assert(player_ptr != NULL);

	sint32 piracy_loss = map.TotalPiracyLoss(playerId);
	sint32 income = player_ptr->m_gold->GetIncome();

	if ( piracy_loss > income * 0.2)
	{
		sint32 priority;

		diplomat.GetCurrentStrategy().GetFearPiracy(priority);
		motivation.priority = (sint16) priority;
		motivation.type =MOTIVATION_FEAR_PIRACY;
		motivation.adviceStrId = adviceId;
		diplomat.ConsiderMotivation(motivation);
	}

	return GEV_HD_Continue;
}

STDEHANDLER(EnlistFriends_MotivationEvent)
{
	static StringId adviceId = -1;
	stringutils_SetStaticStringId(adviceId, "MOTIVATION_ENLIST_FRIENDS_ADVICE");

	PLAYER_INDEX playerId;

	if (!args->GetPlayer(0, playerId))
		return GEV_HD_Continue;

	if (g_player[playerId] == NULL)
		return GEV_HD_Continue;

	Diplomat & diplomat = Diplomat::GetDiplomat(playerId);
	sint32 priority;

	sint32 friends = diplomat.GetFriendCount();
	sint32 enemies = diplomat.GetEnemyCount();

	if ( friends > 0 && enemies > 0 )
	{
		Motivation motivation;

		diplomat.GetCurrentStrategy().GetDesireEnlistFriend(priority);
		motivation.priority = (sint16) priority;
		motivation.type = MOTIVATION_DESIRE_ENLIST_FRIEND;
		motivation.adviceStrId = adviceId;
		diplomat.ConsiderMotivation(motivation);
	}

	return GEV_HD_Continue;
}

STDEHANDLER(PressAdvantage_MotivationEvent)
{
	static StringId adviceId = -1;
	stringutils_SetStaticStringId(adviceId, "MOTIVATION_ENLIST_FRIENDS_ADVICE");

	PLAYER_INDEX playerId;

	if (!args->GetPlayer(0, playerId))
		return GEV_HD_Continue;

	if (g_player[playerId] == NULL)
		return GEV_HD_Continue;

	Diplomat & diplomat = Diplomat::GetDiplomat(playerId);
	Motivation motivation;

	sint32 friend_power = diplomat.GetFriendPower();
	sint32 our_power = MapAnalysis::GetMapAnalysis().TotalThreat(playerId);
	sint32 enemy_threat = diplomat.GetEnemyThreat();
	bool agressive_personality =
		diplomat.GetPersonality()->GetAlignmentEvil() ||
		diplomat.GetPersonality()->GetDiscoveryMilitary() ||
		diplomat.GetPersonality()->GetConquestAgressive();

	bool friendly_personality =
		diplomat.GetPersonality()->GetDiscoveryDiplomatic() ||
		diplomat.GetPersonality()->GetConquestPassive();

	double ratio = (agressive_personality ? 1.2 : 1.5);

	if (!friendly_personality && ((friend_power + our_power) > (enemy_threat * ratio)))
	{
		sint32 priority;

		diplomat.GetCurrentStrategy().GetDesireAttack(priority);
		motivation.priority = (sint16) priority;
		motivation.type = MOTIVATION_DESIRE_ATTACK;
		motivation.adviceStrId = adviceId;
		diplomat.ConsiderMotivation(motivation);
	}

	return GEV_HD_Continue;
}

STDEHANDLER(FearRank_MotivationEvent)
{
	static StringId adviceScienceId = -1;
	static StringId adviceMilitaryId = -1;
	static StringId adviceTradeId = -1;

	stringutils_SetStaticStringId(adviceScienceId, "MOTIVATION_FEAR_SCIENCE_ADVICE");
	stringutils_SetStaticStringId(adviceMilitaryId, "MOTIVATION_FEAR_MILITARY_ADVICE");
	stringutils_SetStaticStringId(adviceTradeId, "MOTIVATION_FEAR_TRADE_ADVICE");

	PLAYER_INDEX playerId;

	if (!args->GetPlayer(0, playerId))
		return GEV_HD_Continue;

	if (g_player[playerId] == NULL)
		return GEV_HD_Continue;

	Diplomat & diplomat = Diplomat::GetDiplomat(playerId);

	sint32 enemies = diplomat.GetEnemyCount();
	sint32 at_war_count = diplomat.AtWarCount();
	sint32 rank = g_player[playerId]->GetRank(STRENGTH_CAT_KNOWLEDGE);
	sint32 priority;
	Motivation motivation;

	if ( enemies > 0 || rank < 75  ||
		 (rank < 50 && diplomat.GetPersonality()->GetDiscoveryScientist()) )
	{
		diplomat.GetCurrentStrategy().GetFearScienceRank(priority);
		motivation.priority = (sint16) priority;
		motivation.type = MOTIVATION_FEAR_SCIENCE_RANK;
		motivation.adviceStrId = adviceScienceId ;
		diplomat.ConsiderMotivation(motivation);
	}

	rank = g_player[playerId]->GetRank(STRENGTH_CAT_MILITARY);
	if ( at_war_count > 0 || rank < 75 ||
		 (rank < 50 && diplomat.GetPersonality()->GetDiscoveryMilitary()) )
	{
		diplomat.GetCurrentStrategy().GetFearMilitaryRank(priority);
		motivation.priority = (sint16) priority;
		motivation.type = MOTIVATION_FEAR_MILITARY_RANK;
		motivation.adviceStrId = adviceMilitaryId ;
		diplomat.ConsiderMotivation(motivation);
	}

	rank = g_player[playerId]->GetRank(STRENGTH_CAT_TRADE);

	if ( rank < 75 ||
		 (rank < 50 && diplomat.GetPersonality()->GetDiscoveryEconomic()) )
	{
		diplomat.GetCurrentStrategy().GetFearTradeRank(priority);
		motivation.priority = (sint16) priority;
		motivation.type = MOTIVATION_FEAR_TRADE_RANK;
		motivation.adviceStrId = adviceMilitaryId ;
		diplomat.ConsiderMotivation(motivation);
	}

	return GEV_HD_Continue;
}

STDEHANDLER(FearPollution_MotivationEvent)
{
	static StringId adviceId = -1;
	stringutils_SetStaticStringId(adviceId, "MOTIVATION_FEAR_POLLUTION_ADVICE");

	PLAYER_INDEX playerId;

	if (!args->GetPlayer(0, playerId))
		return GEV_HD_Continue;

	if (g_player[playerId] == NULL)
		return GEV_HD_Continue;

	Diplomat & diplomat = Diplomat::GetDiplomat(playerId);

	sint32 next_disaster = g_thePollution->GetRoundsToNextDisaster();

	if (diplomat.GetPersonality()->GetDiscoveryEcotopian() &&
		next_disaster > 200)
		return GEV_HD_Continue;

	else if (diplomat.GetPersonality()->GetDiscoveryDiplomatic() &&
		next_disaster > 150)
		return GEV_HD_Continue;

	else if (diplomat.GetPersonality()->GetDiscoveryScientist() &&
		next_disaster > 100)
		return GEV_HD_Continue;

	else if (next_disaster > 50)
		return GEV_HD_Continue;

	Motivation motivation;

	sint32 priority;
	diplomat.GetCurrentStrategy().GetFearPollution(priority);
	motivation.priority = (sint16) priority;
	motivation.type = MOTIVATION_FEAR_POLLUTION;
	motivation.adviceStrId = adviceId;
	diplomat.ConsiderMotivation(motivation);

	return GEV_HD_Continue;
}

void MotivationEventCallbacks::AddCallbacks()
{
	g_gevManager->AddCallback(GEV_ComputeMotivations,
							  GEV_PRI_Primary,
							  &s_MotivationEvent);

	g_gevManager->AddCallback(GEV_ComputeMotivations,
							  GEV_PRI_Pre,
							  &s_ThreatenedCity_MotivationEvent);

	g_gevManager->AddCallback(GEV_ComputeMotivations,
							  GEV_PRI_Pre,
							  &s_DesireGold_MotivationEvent);

	g_gevManager->AddCallback(GEV_ComputeMotivations,
							  GEV_PRI_Pre,
							  &s_DesireMakeFriend_MotivationEvent);

	g_gevManager->AddCallback(GEV_ComputeMotivations,
							  GEV_PRI_Pre,
							  &s_StopPiracy_MotivationEvent);

	g_gevManager->AddCallback(GEV_ComputeMotivations,
							  GEV_PRI_Pre,
							  &s_EnlistFriends_MotivationEvent);

	g_gevManager->AddCallback(GEV_ComputeMotivations,
							  GEV_PRI_Pre,
							  &s_PressAdvantage_MotivationEvent);

	g_gevManager->AddCallback(GEV_ComputeMotivations,
							  GEV_PRI_Pre,
							  &s_FearRank_MotivationEvent);

	g_gevManager->AddCallback(GEV_ComputeMotivations,
							  GEV_PRI_Pre,
							  &s_FearPollution_MotivationEvent);
}
