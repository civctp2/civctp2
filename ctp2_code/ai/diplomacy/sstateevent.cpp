













#include "c3.h"

#include "SStateEvent.h"

#include "Events.h"
#include "GameEventUser.h"
#include "Unit.h"
#include "StrDB.h"
#include "GameEventManager.h"

#include "AICause.h"

#include "Diplomat.h"
#include "MapAnalysis.h"
#include "Player.h"
#include "Governor.h"
#include "StrategyRecord.h"
#include "NewTurnCount.h"
#include "AgreementMatrix.h"
#include "CtpAi.h"







STDEHANDLER(InitSStateEvent)
{	
	PLAYER_INDEX playerId;

	
	if (!args->GetPlayer(0, playerId))
		return GEV_HD_Continue;

	Diplomat & diplomat = Diplomat::GetDiplomat(playerId);
	sint32 index = 0;

    if (diplomat.GetPersonality()->GetDiscoveryScientist())
	{
		g_theStrategyDB->GetNamedItem("STRATEGY_SCIENTIST_DEFAULT", index);
	}
	else if (diplomat.GetPersonality()->GetDiscoveryMilitary())
	{
		g_theStrategyDB->GetNamedItem("STRATEGY_MILITARIST_DEFAULT", index);
	}
	else if (diplomat.GetPersonality()->GetDiscoveryEconomic())
	{
		g_theStrategyDB->GetNamedItem("STRATEGY_ECONOMIC_DEFAULT", index);
	}
	else if (diplomat.GetPersonality()->GetDiscoveryEcotopian())
	{
		g_theStrategyDB->GetNamedItem("STRATEGY_ECOTOPIAN_DEFAULT", index);
	}
	else if (diplomat.GetPersonality()->GetDiscoveryDiplomatic())
	{
		g_theStrategyDB->GetNamedItem("STRATEGY_DIPLOMATIC_DEFAULT", index);
	}
	else
	{
		g_theStrategyDB->GetNamedItem("STRATEGY_DEFAULT", index);
	}

	
	diplomat.SetStrategy(index);

	DPRINTF(k_DBG_AI, ("Player %d initialized strategy to %s.\n",
		playerId,
		g_theStrategyDB->Get(index)->GetNameText()));

	return GEV_HD_Continue;
}







STDEHANDLER(NextSStateEvent)
{
	PLAYER_INDEX playerId;

	
	if (!args->GetPlayer(0, playerId))
		return GEV_HD_Continue;

	Diplomat & diplomat = Diplomat::GetDiplomat(playerId);

	diplomat.ComputeCurrentStrategy();

	return GEV_HD_Continue;
}










STDEHANDLER(FullAttack_NextSStateEvent)
{
	PLAYER_INDEX playerId;
	
	
	if (!args->GetPlayer(0, playerId))
		return GEV_HD_Continue;

	Player *player = g_player[playerId];
	Assert(player);

	Diplomat & diplomat = Diplomat::GetDiplomat(playerId);
	
	AiState state;

	if (
		
		(diplomat.AtWarCount() > 0) &&

		
		

		
		

		
		( Governor::GetGovernor(playerId).PercentUnbuilt( Governor::BUILD_UNIT_LIST_OFFENSE ) <= 0.50) &&

		
		( Governor::GetGovernor(playerId).PercentUnbuilt( Governor::BUILD_UNIT_LIST_DEFENSE ) <= 0.50)
		) 
	{
		BOOL invaded = FALSE;

		
		for (sint32 i=1; i<AgreementMatrix::s_agreements.GetMaxPlayers(); i++) {
			
			if (AgreementMatrix::s_agreements.HasAgreement(playerId, i, PROPOSAL_TREATY_DECLARE_WAR)) {
				
				if ((NewTurnCount::GetCurrentRound() - diplomat.GetLastBorderIncursionBy(i)) < 15) {
					invaded = TRUE;
				}
			}
		}

		if (invaded) {
			
			state.priority = 250;		   
			g_theStrategyDB->GetNamedItem("STRATEGY_ATTACK", state.dbIndex);

			
			g_theStringDB->GetStringID("SPY_FULL_ATTACK_SS",state.spyStrId);
			g_theStringDB->GetStringID("ADVICE_FULL_ATTACK_SS",state.adviceStrId);
			g_theStringDB->GetStringID("NEWS_FULL_ATTACK_SS",state.newsStrId);

			
			diplomat.ConsiderStrategicState(state);
		}
	}
	
	return GEV_HD_Continue;
}










STDEHANDLER(BuildupStrength_NextSStateEvent)
{
	PLAYER_INDEX playerId;

	
	if (!args->GetPlayer(0, playerId))
		return GEV_HD_Continue;

	Diplomat & diplomat = Diplomat::GetDiplomat(playerId);
	
	AiState state;

	Player *player = g_player[playerId];
	Assert(player);

	
	if (diplomat.AtWarCount() <= 0)
		return GEV_HD_Continue;

	
	if ( Governor::GetGovernor(playerId).PercentUnbuilt( Governor::BUILD_UNIT_LIST_OFFENSE ) < 0.50)
		return GEV_HD_Continue;

	
	if ( Governor::GetGovernor(playerId).PercentUnbuilt( Governor::BUILD_UNIT_LIST_DEFENSE ) < 0.50)
		return GEV_HD_Continue;

	
	state.priority = 75;		   
	g_theStrategyDB->GetNamedItem("STRATEGY_DEFEND", state.dbIndex);

	
	g_theStringDB->GetStringID("SPY_REGROUP_SS",state.spyStrId);
	g_theStringDB->GetStringID("ADVICE_REGROUP_SS",state.spyStrId);
	g_theStringDB->GetStringID("NEWS_REGROUP_SS",state.newsStrId);

	
	diplomat.ConsiderStrategicState(state);

	
	return GEV_HD_Continue;
}

STDEHANDLER(SeigeCities_NextSStateEvent)
{
	PLAYER_INDEX playerId;

	
	if (!args->GetPlayer(0, playerId))
		return GEV_HD_Continue;

	Diplomat & diplomat = Diplomat::GetDiplomat(playerId);
	

	Player *player = g_player[playerId];
	Assert(player);

	if (
		
		(diplomat.AtWarCount() > 0) &&

		
		

		
		( Governor::GetGovernor(playerId).PercentUnbuilt( Governor::BUILD_UNIT_LIST_OFFENSE ) <= 0.50) &&

		
		( Governor::GetGovernor(playerId).PercentUnbuilt( Governor::BUILD_UNIT_LIST_RANGED ) <= 0.50) &&

		
		( Governor::GetGovernor(playerId).PercentUnbuilt( Governor::BUILD_UNIT_LIST_DEFENSE ) <= 0.50)
		) 
	{
		AiState state;

		
		state.priority = 200;		   
		g_theStrategyDB->GetNamedItem("STRATEGY_SEIGE", state.dbIndex);

		
		g_theStringDB->GetStringID("SPY_REGROUP_SS",state.spyStrId);
		g_theStringDB->GetStringID("ADVICE_REGROUP_SS",state.spyStrId);
		g_theStringDB->GetStringID("NEWS_REGROUP_SS",state.newsStrId);

		
		diplomat.ConsiderStrategicState(state);
	}
	
	return GEV_HD_Continue;
}

STDEHANDLER(OpeningGambit_NextSStateEvent)
{
	PLAYER_INDEX playerId;

	
	if (!args->GetPlayer(0, playerId))
		return GEV_HD_Continue;

	
	if (NewTurnCount::GetCurrentRound() > 75)
		return GEV_HD_Continue;

	Diplomat & diplomat = Diplomat::GetDiplomat(playerId);
	AiState state;

	if (diplomat.GetPersonality()->GetExpansionMaximum() ||
		diplomat.GetPersonality()->GetExplorationWide())
	{
		state.priority = 200;		   
		g_theStrategyDB->GetNamedItem("STRATEGY_AGRESSIVE_START", state.dbIndex);
		
	}
	else
	{
		state.priority = 200;		   
		g_theStrategyDB->GetNamedItem("STRATEGY_CAREFUL_START", state.dbIndex);
		
	}

	
	diplomat.ConsiderStrategicState(state);

	return GEV_HD_Continue;
}

STDEHANDLER(NuclearStrike_NextSStateEvent)
{
	PLAYER_INDEX playerId;

	
	if (!args->GetPlayer(0, playerId))
		return GEV_HD_Continue;

	Diplomat & diplomat = Diplomat::GetDiplomat(playerId);
	AiState state;

	if ((diplomat.GetPersonality()->GetDiscoveryMilitary() ||
		 diplomat.GetPersonality()->GetDiscoveryScientist()) &&
		(diplomat.GetPersonality()->GetTrustworthinessChaotic() &&
		 diplomat.GetPersonality()->GetAlignmentEvil()))
	{
		state.priority = 150;		   
		g_theStrategyDB->GetNamedItem("STRATEGY_LAUNCH_NUKES", state.dbIndex);
		

		
		diplomat.ConsiderStrategicState(state);
	}

	return GEV_HD_Continue;
}

STDEHANDLER(NuclearReadiness_NextSStateEvent)
{
	PLAYER_INDEX playerId;

	
	if (!args->GetPlayer(0, playerId))
		return GEV_HD_Continue;

	Diplomat & diplomat = Diplomat::GetDiplomat(playerId);
	AiState state;

	
	sint32 our_nukes = MapAnalysis::GetMapAnalysis().GetNuclearWeaponsCount(playerId);

	
	if (our_nukes) {
		
		sint32 max_nukes = 0;

		for (sint32 id = 1; id < CtpAi::s_maxPlayers; id++) 
		{
			if ((id != playerId) && (g_player[id] != NULL)) 
			{
				sint32 nukes = MapAnalysis::GetMapAnalysis().GetNuclearWeaponsCount(id);

				if (nukes > max_nukes) max_nukes = nukes;
			}
		}

		
		if (max_nukes > 75) {
			state.priority = 100;		   
			g_theStrategyDB->GetNamedItem("STRATEGY_MAXIMUM_NUKES", state.dbIndex);

			
			diplomat.ConsiderStrategicState(state);
		} 
		else if (max_nukes > 50) {
			if (diplomat.GetPersonality()->GetConquestAgressive())
			{
				state.priority = 100;		   
				g_theStrategyDB->GetNamedItem("STRATEGY_MAXIMUM_NUKES", state.dbIndex);

				
				diplomat.ConsiderStrategicState(state);
			}
			else
			{
				state.priority = 100;		   
				g_theStrategyDB->GetNamedItem("STRATEGY_AVERAGE_NUKES", state.dbIndex);

				
				diplomat.ConsiderStrategicState(state);
			}
		}
		else if (max_nukes > 25) {
			if (diplomat.GetPersonality()->GetConquestAgressive())
			{
				state.priority = 100;		   
				g_theStrategyDB->GetNamedItem("STRATEGY_AVERAGE_NUKES", state.dbIndex);

				
				diplomat.ConsiderStrategicState(state);
			}
			else
			{
				state.priority = 100;		   
				g_theStrategyDB->GetNamedItem("STRATEGY_MINIMAL_NUKES", state.dbIndex);

				
				diplomat.ConsiderStrategicState(state);
			}
		}
		else if (max_nukes > 10) {
			if (diplomat.GetPersonality()->GetConquestAgressive())
			{
				state.priority = 100;		   
				g_theStrategyDB->GetNamedItem("STRATEGY_MINIMAL_NUKES", state.dbIndex);

				
				diplomat.ConsiderStrategicState(state);
			}
		}
	}

	return GEV_HD_Continue;
}

STDEHANDLER(SetExpansion_NextSStateEvent)
{
	PLAYER_INDEX playerId;

	
	if (!args->GetPlayer(0, playerId))
		return GEV_HD_Continue;

	Diplomat & diplomat = Diplomat::GetDiplomat(playerId);
	AiState state;

	if (diplomat.GetPersonality()->GetExpansionMinimum())
	{
		state.priority = 1100;
		g_theStrategyDB->GetNamedItem("STRATEGY_SETTLE_COMPACT",state.dbIndex);
		

		
		diplomat.ConsiderStrategicState(state);
	}
	else if (diplomat.GetPersonality()->GetExpansionMaximum())
	{
		state.priority = 1100;
		g_theStrategyDB->GetNamedItem("STRATEGY_SETTLE_LARGE",state.dbIndex);
		

		
		diplomat.ConsiderStrategicState(state);
	}
	else 
	{
		state.priority = 1100;
		g_theStrategyDB->GetNamedItem("STRATEGY_SETTLE_COMPACT",state.dbIndex);
		

		
		diplomat.ConsiderStrategicState(state);
	} 

	return GEV_HD_Continue;
}

STDEHANDLER(SetExploration_NextSStateEvent)
{
	PLAYER_INDEX playerId;

	
	if (!args->GetPlayer(0, playerId))
		return GEV_HD_Continue;

	Diplomat & diplomat = Diplomat::GetDiplomat(playerId);
	AiState state;

	if (diplomat.GetPersonality()->GetExplorationMinimal())
	{
		state.priority = 1000;
		g_theStrategyDB->GetNamedItem("STRATEGY_EXPLORE_NEAR",state.dbIndex);
		

		
		diplomat.ConsiderStrategicState(state);
	}
	else if (diplomat.GetPersonality()->GetExplorationMedium())
	{
		state.priority = 1000;
		g_theStrategyDB->GetNamedItem("STRATEGY_EXPLORE_FAR",state.dbIndex);
		

		
		diplomat.ConsiderStrategicState(state);
	}
	else if (diplomat.GetPersonality()->GetExplorationWide())
	{
		state.priority = 1000;
		g_theStrategyDB->GetNamedItem("STRATEGY_EXPLORE_WIDE",state.dbIndex);
		

		
		diplomat.ConsiderStrategicState(state);
	}

	return GEV_HD_Continue;
}

STDEHANDLER(IslandNation_NextSStateEvent)
{
	PLAYER_INDEX playerId;

	
	if (!args->GetPlayer(0, playerId))
		return GEV_HD_Continue;

	Diplomat & diplomat = Diplomat::GetDiplomat(playerId);
	AiState state;

	sint32 avg_cont_size = MapAnalysis::GetMapAnalysis().AverageSettledContinentSize(playerId);

	
	if (avg_cont_size < 100 && avg_cont_size > 0)
	{
		state.priority = 150;		   
		g_theStrategyDB->GetNamedItem("STRATEGY_ISLAND_NATION", state.dbIndex);
		

		
		diplomat.ConsiderStrategicState(state);
	}

	return GEV_HD_Continue;
}

STDEHANDLER(DefenseLevel_NextSStateEvent)
{
	PLAYER_INDEX playerId;

	
	if (!args->GetPlayer(0, playerId))
		return GEV_HD_Continue;

	Diplomat & diplomat = Diplomat::GetDiplomat(playerId);
	
	diplomat.ClearEffectiveRegardCache();

	AiState state;

	
	sint16 max_threat = MapAnalysis::GetMapAnalysis().GetMaxThreat(playerId);

#define MAXIMUM_DEFENSE_LEVEL	50000
#define HIGH_DEFENSE_LEVEL		25000
#define MEDIUM_DEFENSE_LEVEL	10000
#define LOW_DEFENSE_LEVEL		5000
#define MINIMUM_DEFENSE_LEVEL	1000
	Player *player_ptr = g_player[playerId];

	
	if (NewTurnCount::GetCurrentRound() < 75)
	{
		if (max_threat > MEDIUM_DEFENSE_LEVEL)
		{
			state.priority = 800;		   
			g_theStrategyDB->GetNamedItem("STRATEGY_DEFENSE_VERY_LOW", state.dbIndex);
			
		}
		else 
		{
			state.priority = 800;		   
			g_theStrategyDB->GetNamedItem("STRATEGY_DEFENSE_NONE", state.dbIndex);
			
		}
	}
	else
	{
		if (max_threat > MAXIMUM_DEFENSE_LEVEL)
		{
			if (!diplomat.GetPersonality()->GetExplorationMinimal() ||
				diplomat.GetPersonality()->GetConquestAgressive())
			{
				state.priority = 800;		   
				g_theStrategyDB->GetNamedItem("STRATEGY_DEFENSE_HIGH", state.dbIndex);
				
			}
			else
			{
				state.priority = 800;		   
				g_theStrategyDB->GetNamedItem("STRATEGY_DEFENSE_VERY_HIGH", state.dbIndex);
				
			}
		}
		else if (max_threat > HIGH_DEFENSE_LEVEL)
		{
			if (!diplomat.GetPersonality()->GetExplorationMinimal() ||
				diplomat.GetPersonality()->GetConquestAgressive())
			{
				state.priority = 800;		   
				g_theStrategyDB->GetNamedItem("STRATEGY_DEFENSE_MEDIUM", state.dbIndex);
				
			}
			else
			{
				state.priority = 800;		   
				g_theStrategyDB->GetNamedItem("STRATEGY_DEFENSE_HIGH", state.dbIndex);
				
			}
		}
		else if (max_threat > MEDIUM_DEFENSE_LEVEL)
		{
			if (!diplomat.GetPersonality()->GetExplorationMinimal() ||
				diplomat.GetPersonality()->GetConquestAgressive())
			{
				state.priority = 800;		   
				g_theStrategyDB->GetNamedItem("STRATEGY_DEFENSE_LOW", state.dbIndex);
				
			}
			else
			{
				state.priority = 800;		   
				g_theStrategyDB->GetNamedItem("STRATEGY_DEFENSE_MEDIUM", state.dbIndex);
				
			}
		}
		else if (max_threat > LOW_DEFENSE_LEVEL)
		{
			if (!diplomat.GetPersonality()->GetExplorationMinimal() ||
				diplomat.GetPersonality()->GetConquestAgressive())
			{
				state.priority = 800;		   
				g_theStrategyDB->GetNamedItem("STRATEGY_DEFENSE_VERY_LOW", state.dbIndex);
				
			}
			else
			{
				state.priority = 800;		   
				g_theStrategyDB->GetNamedItem("STRATEGY_DEFENSE_LOW", state.dbIndex);
				
			}
		}
		else if (max_threat > MINIMUM_DEFENSE_LEVEL)
		{
			if (!diplomat.GetPersonality()->GetExplorationMinimal() ||
				diplomat.GetPersonality()->GetConquestAgressive())
			{
				state.priority = 800;		   
				g_theStrategyDB->GetNamedItem("STRATEGY_DEFENSE_NONE", state.dbIndex);
				
			}
			else
			{
				state.priority = 800;		   
				g_theStrategyDB->GetNamedItem("STRATEGY_DEFENSE_VERY_LOW", state.dbIndex);
				
			}
		}
		else 
		{
			state.priority = 800;		   
			g_theStrategyDB->GetNamedItem("STRATEGY_DEFENSE_NONE", state.dbIndex);
			
		}
	}

	
	diplomat.ConsiderStrategicState(state);

	return GEV_HD_Continue;
}


void StrategicStateEventCallbacks::AddCallbacks()
{
    
	g_gevManager->AddCallback(GEV_InitStrategicState, 
							  GEV_PRI_Primary, 
							  &s_InitSStateEvent);

    
	g_gevManager->AddCallback(GEV_NextStrategicState, 
							  GEV_PRI_Primary, 
							  &s_NextSStateEvent);

	g_gevManager->AddCallback(GEV_NextStrategicState, 
							  GEV_PRI_Pre, 
							  &s_InitSStateEvent);

	g_gevManager->AddCallback(GEV_NextStrategicState, 
							  GEV_PRI_Pre, 
							  &s_FullAttack_NextSStateEvent);

	g_gevManager->AddCallback(GEV_NextStrategicState, 
							  GEV_PRI_Pre, 
							  &s_BuildupStrength_NextSStateEvent);

	g_gevManager->AddCallback(GEV_NextStrategicState, 
							  GEV_PRI_Pre, 
							  &s_SeigeCities_NextSStateEvent);
	
	g_gevManager->AddCallback(GEV_NextStrategicState, 
							  GEV_PRI_Pre, 
							  &s_NuclearReadiness_NextSStateEvent);

	g_gevManager->AddCallback(GEV_NextStrategicState, 
							  GEV_PRI_Pre, 
							  &s_NuclearStrike_NextSStateEvent);

	g_gevManager->AddCallback(GEV_NextStrategicState, 
							  GEV_PRI_Pre, 
							  &s_OpeningGambit_NextSStateEvent);

	g_gevManager->AddCallback(GEV_NextStrategicState, 
							  GEV_PRI_Pre, 
							  &s_IslandNation_NextSStateEvent);

	g_gevManager->AddCallback(GEV_NextStrategicState, 
							  GEV_PRI_Pre, 
							  &s_SetExpansion_NextSStateEvent);

	g_gevManager->AddCallback(GEV_NextStrategicState, 
							  GEV_PRI_Pre, 
							  &s_SetExploration_NextSStateEvent);

	g_gevManager->AddCallback(GEV_NextStrategicState, 
							  GEV_PRI_Pre, 
							  &s_DefenseLevel_NextSStateEvent);
}
