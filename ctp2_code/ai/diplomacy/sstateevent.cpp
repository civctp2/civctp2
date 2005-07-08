//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : AI startegy control
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
// - Exposed startegies to personalities.txt by Martin Gühmann.
// - Fixed advice string for BuildupStrength and SeigeCities 
//   strategies by Martin Gühmann.
// - Added over city limit strategy by Martin Gühmann.
// - Restored backward compartibility with old personalities.txts 
//   by Martin Gühmann
//
//----------------------------------------------------------------------------

#include "c3.h"

#include "SStateEvent.h"

#include "Events.h"
#include "GameEventUser.h"
#include "Unit.h"
#include "StrDB.h"
#include "GameEventManager.h"

#include "AICause.h"

#include "Diplomat.h"
#include "mapanalysis.h"
#include "player.h"
#include "governor.h"
#include "StrategyRecord.h"
#include "newturncount.h"
#include "AgreementMatrix.h"
#include "ctpai.h"

#include "GovernmentRecord.h"
#include "wonderutil.h"





//----------------------------------------------------------------------------
//
// Name       : InitSStateEvent
//
// Description: Initializes the default player's strategy 
//
// Parameters : -
//
// Globals    : -
//
// Returns    : -
//
// Remark(s)  : -
//
//----------------------------------------------------------------------------
STDEHANDLER(InitSStateEvent)
{	
	PLAYER_INDEX playerId;

	
	if (!args->GetPlayer(0, playerId))
		return GEV_HD_Continue;

	Diplomat & diplomat = Diplomat::GetDiplomat(playerId);

//Added by Martin Gühmann to expose the 
//default strategy to personalities.txt
	sint32 index = diplomat.GetPersonality()->GetDefaultStrategyIndex();
	if(index < 0){
		if (diplomat.GetPersonality()->GetDiscoveryScientist()){
			g_theStrategyDB->GetNamedItem("STRATEGY_SCIENTIST_DEFAULT", index);
		}
		else if(diplomat.GetPersonality()->GetDiscoveryMilitary()){
			g_theStrategyDB->GetNamedItem("STRATEGY_MILITARIST_DEFAULT", index);
		}
		else if(diplomat.GetPersonality()->GetDiscoveryEconomic()){
			g_theStrategyDB->GetNamedItem("STRATEGY_ECONOMIC_DEFAULT", index);
		}
		else if(diplomat.GetPersonality()->GetDiscoveryEcotopian()){
			g_theStrategyDB->GetNamedItem("STRATEGY_ECOTOPIAN_DEFAULT", index);
		}
		else if(diplomat.GetPersonality()->GetDiscoveryDiplomatic()){
			g_theStrategyDB->GetNamedItem("STRATEGY_DIPLOMATIC_DEFAULT", index);
		}
		else{
			g_theStrategyDB->GetNamedItem("STRATEGY_DEFAULT", index);
		}
	}
	
	diplomat.SetStrategy(index);

	DPRINTF(k_DBG_AI, ("Player %d initialized strategy to %s.\n",
		playerId,
		g_theStrategyDB->Get(index)->GetNameText()));

	return GEV_HD_Continue;
}

//----------------------------------------------------------------------------
//
// Name       : NextSStateEvent
//
// Description: Computes the current player's strategy 
//
// Parameters : -
//
// Globals    : -
//
// Returns    : -
//
// Remark(s)  : -
//
//----------------------------------------------------------------------------
STDEHANDLER(NextSStateEvent)
{
	PLAYER_INDEX playerId;

	
	if (!args->GetPlayer(0, playerId))
		return GEV_HD_Continue;

	Diplomat & diplomat = Diplomat::GetDiplomat(playerId);

	diplomat.ComputeCurrentStrategy();

	return GEV_HD_Continue;
}

//----------------------------------------------------------------------------
//
// Name       : FullAttack_NextSStateEvent
//
// Description: Checks whether the player should use the full attack startegy
//              and sets it if necessary. 
//
// Parameters : -
//
// Globals    : -
//
// Returns    : -
//
// Remark(s)  : -
//
//----------------------------------------------------------------------------
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

			if(diplomat.GetPersonality()->GetFullAttackStrategy()){
				state.priority = diplomat.GetPersonality()->GetFullAttackStrategyPtr()->GetPriority();
				state.dbIndex = diplomat.GetPersonality()->GetFullAttackStrategyPtr()->GetStrategyIndex();
			}
			else{
				state.priority = 250;		   
				g_theStrategyDB->GetNamedItem("STRATEGY_ATTACK", state.dbIndex);
			}
			g_theStringDB->GetStringID("SPY_FULL_ATTACK_SS",state.spyStrId);
			g_theStringDB->GetStringID("ADVICE_FULL_ATTACK_SS",state.adviceStrId);
			g_theStringDB->GetStringID("NEWS_FULL_ATTACK_SS",state.newsStrId);

			
			diplomat.ConsiderStrategicState(state);
		}
	}
	
	return GEV_HD_Continue;
}

//----------------------------------------------------------------------------
//
// Name       : BuildupStrength_NextSStateEvent
//
// Description: Checks whether the player should use the defend startegy 
//              to build up its strength and sets it if necessary. 
//
// Parameters : -
//
// Globals    : -
//
// Returns    : -
//
// Remark(s)  : -
//
//----------------------------------------------------------------------------
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

	if(diplomat.GetPersonality()->GetBuildupStrengthStrategy()){
		state.priority = diplomat.GetPersonality()->GetBuildupStrengthStrategyPtr()->GetPriority();		   
		state.dbIndex = diplomat.GetPersonality()->GetBuildupStrengthStrategyPtr()->GetStrategyIndex();
	}
	else{
		state.priority = 75;		   
		g_theStrategyDB->GetNamedItem("STRATEGY_DEFEND", state.dbIndex);
	}

	
	g_theStringDB->GetStringID("SPY_REGROUP_SS",state.spyStrId);
	g_theStringDB->GetStringID("ADVICE_REGROUP_SS",state.adviceStrId);
	g_theStringDB->GetStringID("NEWS_REGROUP_SS",state.newsStrId);
	
	diplomat.ConsiderStrategicState(state);

	
	return GEV_HD_Continue;
}

//----------------------------------------------------------------------------
//
// Name       : SeigeCities_NextSStateEvent
//
// Description: Checks whether the player should use the siege cities startegy
//              and sets it if necessary. 
//
// Parameters : -
//
// Globals    : -
//
// Returns    : -
//
// Remark(s)  : -
//
//----------------------------------------------------------------------------
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

		if(diplomat.GetPersonality()->GetSeigeCitiesStrategy()){
			state.priority = diplomat.GetPersonality()->GetSeigeCitiesStrategyPtr()->GetPriority();
			state.dbIndex = diplomat.GetPersonality()->GetSeigeCitiesStrategyPtr()->GetStrategyIndex();
		}
		else{
			state.priority = 200;		   
			g_theStrategyDB->GetNamedItem("STRATEGY_SEIGE", state.dbIndex);
		}

		
		g_theStringDB->GetStringID("SPY_REGROUP_SS",state.spyStrId);
		g_theStringDB->GetStringID("ADVICE_REGROUP_SS",state.adviceStrId);
		g_theStringDB->GetStringID("NEWS_REGROUP_SS",state.newsStrId);
		
		diplomat.ConsiderStrategicState(state);
	}
	
	return GEV_HD_Continue;
}

//----------------------------------------------------------------------------
//
// Name       : OpeningGambit_NextSStateEvent
//
// Description: Sets the strategy for the start in the first turns. The number
//              of turns is set in personalities.txt. 
//
// Parameters : -
//
// Globals    : -
//
// Returns    : -
//
// Remark(s)  : -
//
//----------------------------------------------------------------------------
STDEHANDLER(OpeningGambit_NextSStateEvent)
{
	PLAYER_INDEX playerId;

	
	if (!args->GetPlayer(0, playerId))
		return GEV_HD_Continue;

	Diplomat & diplomat = Diplomat::GetDiplomat(playerId);
	if (NewTurnCount::GetCurrentRound() > diplomat.GetPersonality()->GetLastStartTurn())
		return GEV_HD_Continue;

	AiState state;
	if(diplomat.GetPersonality()->GetStartStrategy()){
		state.priority = diplomat.GetPersonality()->GetStartStrategyPtr()->GetPriority();
		state.dbIndex = diplomat.GetPersonality()->GetStartStrategyPtr()->GetStrategyIndex();
	}
	else{
		state.priority = 200;
		if(diplomat.GetPersonality()->GetExpansionMaximum()
		|| diplomat.GetPersonality()->GetExplorationWide()){
			g_theStrategyDB->GetNamedItem("STRATEGY_AGRESSIVE_START", state.dbIndex);
		}
		else{
			g_theStrategyDB->GetNamedItem("STRATEGY_CAREFUL_START", state.dbIndex);		
		}
	}
	
	diplomat.ConsiderStrategicState(state);

	return GEV_HD_Continue;
}

//----------------------------------------------------------------------------
//
// Name       : NuclearStrike_NextSStateEvent
//
// Description: Checks whether the players's personality has a lunch nukes 
//              strategy and sets it if the player's personalty has such a
//              startegy. 
//
// Parameters : -
//
// Globals    : -
//
// Returns    : -
//
// Remark(s)  : -
//
//----------------------------------------------------------------------------
STDEHANDLER(NuclearStrike_NextSStateEvent)
{
	PLAYER_INDEX playerId;

	
	if (!args->GetPlayer(0, playerId))
		return GEV_HD_Continue;

	Diplomat & diplomat = Diplomat::GetDiplomat(playerId);
	AiState state;

#if defined(ACTIVISION_DEFAULT)
	if ((diplomat.GetPersonality()->GetDiscoveryMilitary() ||
		 diplomat.GetPersonality()->GetDiscoveryScientist()) &&
		(diplomat.GetPersonality()->GetTrustworthinessChaotic() &&
		 diplomat.GetPersonality()->GetAlignmentEvil()))
	{
		state.priority = 150;		   
		g_theStrategyDB->GetNamedItem("STRATEGY_LAUNCH_NUKES", state.dbIndex);
		

		
		diplomat.ConsiderStrategicState(state);
	}
#else
	if(diplomat.GetPersonality()->GetNuclearStrikeStrategy()){
		state.priority = diplomat.GetPersonality()->GetNuclearStrikeStrategyPtr()->GetPriority();
		state.dbIndex = diplomat.GetPersonality()->GetNuclearStrikeStrategyPtr()->GetStrategyIndex();
	}
#endif

	return GEV_HD_Continue;
}

//----------------------------------------------------------------------------
//
// Name       : NuclearReadiness_NextSStateEvent
//
// Description: Determines which nuklear readiness strategy should be used. 
//              This is dependent on the nuclear threat level. 
//
// Parameters : -
//
// Globals    : -
//
// Returns    : -
//
// Remark(s)  : -
//
//----------------------------------------------------------------------------
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

		if(diplomat.GetPersonality()->GetMaximumNukesStrategy()
		&& diplomat.GetPersonality()->GetAverageNukesStrategy()
		&& diplomat.GetPersonality()->GetLowNukesStrategy()
		&& diplomat.GetPersonality()->GetMinimumNukesStrategy()){
			if(max_nukes > 75){
				state.priority = diplomat.GetPersonality()->GetMaximumNukesStrategyPtr()->GetPriority();		   
				state.dbIndex = diplomat.GetPersonality()->GetMaximumNukesStrategyPtr()->GetStrategyIndex();
			} 
			else if(max_nukes > 50){
				state.priority = diplomat.GetPersonality()->GetAverageNukesStrategyPtr()->GetPriority();		   
				state.dbIndex = diplomat.GetPersonality()->GetAverageNukesStrategyPtr()->GetStrategyIndex();
			}
			else if(max_nukes > 25){
				state.priority = diplomat.GetPersonality()->GetLowNukesStrategyPtr()->GetPriority();		   
				state.dbIndex = diplomat.GetPersonality()->GetLowNukesStrategyPtr()->GetStrategyIndex();
			}
			else if(max_nukes > 10){
				state.priority = diplomat.GetPersonality()->GetMinimumNukesStrategyPtr()->GetPriority();		   
				state.dbIndex = diplomat.GetPersonality()->GetMinimumNukesStrategyPtr()->GetStrategyIndex();
			}
		}
		else{
			state.priority = 100;		   
			if(max_nukes > 75){
				g_theStrategyDB->GetNamedItem("STRATEGY_MAXIMUM_NUKES", state.dbIndex);
			} 
			else if(max_nukes > 50){
				if(diplomat.GetPersonality()->GetConquestAgressive()){
					g_theStrategyDB->GetNamedItem("STRATEGY_MAXIMUM_NUKES", state.dbIndex);
				}
				else{
					g_theStrategyDB->GetNamedItem("STRATEGY_AVERAGE_NUKES", state.dbIndex);
				}
			}
			else if(max_nukes > 25){
				if(diplomat.GetPersonality()->GetConquestAgressive()){
					g_theStrategyDB->GetNamedItem("STRATEGY_AVERAGE_NUKES", state.dbIndex);
				}
				else{
					g_theStrategyDB->GetNamedItem("STRATEGY_MINIMAL_NUKES", state.dbIndex);
				}
			}
			else if(max_nukes > 10){
				if(diplomat.GetPersonality()->GetConquestAgressive()){
					g_theStrategyDB->GetNamedItem("STRATEGY_MINIMAL_NUKES", state.dbIndex);
				}
			}
		}

		diplomat.ConsiderStrategicState(state);
	}

	return GEV_HD_Continue;
}

//----------------------------------------------------------------------------
//
// Name       : SetExpansion_NextSStateEvent
//
// Description: Loads the player's personality's expansion startegy.
//
// Parameters : -
//
// Globals    : -
//
// Returns    : -
//
// Remark(s)  : -
//
//----------------------------------------------------------------------------
STDEHANDLER(SetExpansion_NextSStateEvent)
{
	PLAYER_INDEX playerId;

	
	if (!args->GetPlayer(0, playerId))
		return GEV_HD_Continue;

	Diplomat & diplomat = Diplomat::GetDiplomat(playerId);
	AiState state;

	if(diplomat.GetPersonality()->GetExpansionStrategy()){
		state.priority = diplomat.GetPersonality()->GetExpansionStrategyPtr()->GetPriority();		   
		state.dbIndex = diplomat.GetPersonality()->GetExpansionStrategyPtr()->GetStrategyIndex();
	}
	else{
		state.priority = 1100;
		if(diplomat.GetPersonality()->GetExpansionMinimum()){
			g_theStrategyDB->GetNamedItem("STRATEGY_SETTLE_COMPACT",state.dbIndex);
		}
		else if(diplomat.GetPersonality()->GetExpansionMaximum()){
			g_theStrategyDB->GetNamedItem("STRATEGY_SETTLE_LARGE",state.dbIndex);
		}
		else{
			g_theStrategyDB->GetNamedItem("STRATEGY_SETTLE_COMPACT",state.dbIndex);
		}
	}

	diplomat.ConsiderStrategicState(state);

	return GEV_HD_Continue;
}

//----------------------------------------------------------------------------
//
// Name       : SetExploration_NextSStateEvent
//
// Description: Loads the player's personality's exploration strategy. 
//
// Parameters : -
//
// Globals    : -
//
// Returns    : -
//
// Remark(s)  : -
//
//----------------------------------------------------------------------------
STDEHANDLER(SetExploration_NextSStateEvent)
{
	PLAYER_INDEX playerId;

	
	if (!args->GetPlayer(0, playerId))
		return GEV_HD_Continue;

	Diplomat & diplomat = Diplomat::GetDiplomat(playerId);
	AiState state;

	if(diplomat.GetPersonality()->GetExplorationStrategy()){
		state.priority = diplomat.GetPersonality()->GetExplorationStrategyPtr()->GetPriority();		   
		state.dbIndex = diplomat.GetPersonality()->GetExplorationStrategyPtr()->GetStrategyIndex();
	}
	else{
		state.priority = 1000;
		if(diplomat.GetPersonality()->GetExplorationMinimal()){
			g_theStrategyDB->GetNamedItem("STRATEGY_EXPLORE_NEAR",state.dbIndex);
		}
		else if(diplomat.GetPersonality()->GetExplorationMedium()){
			g_theStrategyDB->GetNamedItem("STRATEGY_EXPLORE_FAR",state.dbIndex);
		}
		else if(diplomat.GetPersonality()->GetExplorationWide()){
			g_theStrategyDB->GetNamedItem("STRATEGY_EXPLORE_WIDE",state.dbIndex);
		}
	}

	diplomat.ConsiderStrategicState(state);

	return GEV_HD_Continue;
}

//----------------------------------------------------------------------------
//
// Name       : IslandNation_NextSStateEvent
//
// Description: Loads the player's personality's island nation startegy
//              if the players start position is on a continent with less
//              x squares. x is defined in personalities.txt.
//
// Parameters : -
//
// Globals    : -
//
// Returns    : -
//
// Remark(s)  : -
//
//----------------------------------------------------------------------------
STDEHANDLER(IslandNation_NextSStateEvent)
{
	PLAYER_INDEX playerId;

	
	if (!args->GetPlayer(0, playerId))
		return GEV_HD_Continue;

	Diplomat & diplomat = Diplomat::GetDiplomat(playerId);
	AiState state;

	sint32 avg_cont_size = MapAnalysis::GetMapAnalysis().AverageSettledContinentSize(playerId);

	if(avg_cont_size < diplomat.GetPersonality()->GetMaxIslandSize() && avg_cont_size > 0){
		if(diplomat.GetPersonality()->GetIslandNationStrategy()){
			state.priority = diplomat.GetPersonality()->GetIslandNationStrategyPtr()->GetPriority();		   
			state.dbIndex = diplomat.GetPersonality()->GetIslandNationStrategyPtr()->GetStrategyIndex();
		}
		else{
			state.priority = 150;		   
			g_theStrategyDB->GetNamedItem("STRATEGY_ISLAND_NATION", state.dbIndex);
		}

		diplomat.ConsiderStrategicState(state);
	}

	return GEV_HD_Continue;
}

//----------------------------------------------------------------------------
//
// Name       : DefenseLevel_NextSStateEvent
//
// Description: Loads the defense strategy to threat level.
//
// Parameters : -
//
// Globals    : -
//
// Returns    : -
//
// Remark(s)  : -
//
//----------------------------------------------------------------------------
STDEHANDLER(DefenseLevel_NextSStateEvent)
{
	PLAYER_INDEX playerId;

	
	if (!args->GetPlayer(0, playerId))
		return GEV_HD_Continue;

	Diplomat & diplomat = Diplomat::GetDiplomat(playerId);
	
	diplomat.ClearEffectiveRegardCache();

	AiState state;

	
	sint32 max_threat = MapAnalysis::GetMapAnalysis().GetMaxThreat(playerId);

#define MAXIMUM_DEFENSE_LEVEL	50000
#define HIGH_DEFENSE_LEVEL		25000
#define MEDIUM_DEFENSE_LEVEL	10000
#define LOW_DEFENSE_LEVEL		5000
#define MINIMUM_DEFENSE_LEVEL	1000
	Player *player_ptr = g_player[playerId];

	if(diplomat.GetPersonality()->GetDefenceVeryHighStrategy()
	&& diplomat.GetPersonality()->GetDefenceHighStrategy()
	&& diplomat.GetPersonality()->GetDefenceMediumStrategy()
	&& diplomat.GetPersonality()->GetDefenceLowStrategy()
	&& diplomat.GetPersonality()->GetDefenceVeryLowStrategy()
	&& diplomat.GetPersonality()->GetDefenceNoneStrategy()
	){
		if(NewTurnCount::GetCurrentRound() < diplomat.GetPersonality()->GetLastStartTurn()){
			if(max_threat > MEDIUM_DEFENSE_LEVEL){
				state.priority = diplomat.GetPersonality()->GetStartHighDefenceStrategyPtr()->GetPriority();
				state.dbIndex  = diplomat.GetPersonality()->GetStartHighDefenceStrategyPtr()->GetStrategyIndex();
			}
			else{
				state.priority = diplomat.GetPersonality()->GetStartLowDefenceStrategyPtr()->GetPriority();
				state.dbIndex  = diplomat.GetPersonality()->GetStartLowDefenceStrategyPtr()->GetStrategyIndex();
			}
		}
		else{
			if(max_threat > MAXIMUM_DEFENSE_LEVEL){
				state.priority = diplomat.GetPersonality()->GetDefenceVeryHighStrategyPtr()->GetPriority();
				state.dbIndex  = diplomat.GetPersonality()->GetDefenceVeryHighStrategyPtr()->GetStrategyIndex();
			}
			else if(max_threat > HIGH_DEFENSE_LEVEL){
				state.priority = diplomat.GetPersonality()->GetDefenceHighStrategyPtr()->GetPriority();
				state.dbIndex  = diplomat.GetPersonality()->GetDefenceHighStrategyPtr()->GetStrategyIndex();
			}
			else if(max_threat > MEDIUM_DEFENSE_LEVEL){
				state.priority = diplomat.GetPersonality()->GetDefenceMediumStrategyPtr()->GetPriority();
				state.dbIndex  = diplomat.GetPersonality()->GetDefenceMediumStrategyPtr()->GetStrategyIndex();
			}
			else if(max_threat > LOW_DEFENSE_LEVEL){
				state.priority = diplomat.GetPersonality()->GetDefenceLowStrategyPtr()->GetPriority();
				state.dbIndex  = diplomat.GetPersonality()->GetDefenceLowStrategyPtr()->GetStrategyIndex();
			}
			else if(max_threat > MINIMUM_DEFENSE_LEVEL){
				state.priority = diplomat.GetPersonality()->GetDefenceVeryLowStrategyPtr()->GetPriority();
				state.dbIndex  = diplomat.GetPersonality()->GetDefenceVeryLowStrategyPtr()->GetStrategyIndex();
			}
			else{
				state.priority = diplomat.GetPersonality()->GetDefenceNoneStrategyPtr()->GetPriority();
				state.dbIndex  = diplomat.GetPersonality()->GetDefenceNoneStrategyPtr()->GetStrategyIndex();
			}
		}
	}
	else{
		state.priority = 800;
		if(NewTurnCount::GetCurrentRound() < diplomat.GetPersonality()->GetLastStartTurn()){
			if(max_threat > MEDIUM_DEFENSE_LEVEL){
				g_theStrategyDB->GetNamedItem("STRATEGY_DEFENSE_VERY_LOW", state.dbIndex);
			}
			else{
				g_theStrategyDB->GetNamedItem("STRATEGY_DEFENSE_NONE", state.dbIndex);
			}
		}
		else{
			if(max_threat > MAXIMUM_DEFENSE_LEVEL){
				if(!diplomat.GetPersonality()->GetExplorationMinimal()
				||  diplomat.GetPersonality()->GetConquestAgressive()
				){
					g_theStrategyDB->GetNamedItem("STRATEGY_DEFENSE_HIGH", state.dbIndex);
				}
				else{
					g_theStrategyDB->GetNamedItem("STRATEGY_DEFENSE_VERY_HIGH", state.dbIndex);
				}
			}
			else if(max_threat > HIGH_DEFENSE_LEVEL){
				if(!diplomat.GetPersonality()->GetExplorationMinimal()
				||  diplomat.GetPersonality()->GetConquestAgressive()
				){
					g_theStrategyDB->GetNamedItem("STRATEGY_DEFENSE_MEDIUM", state.dbIndex);
				}
				else{
					g_theStrategyDB->GetNamedItem("STRATEGY_DEFENSE_HIGH", state.dbIndex);
				}
			}
			else if(max_threat > MEDIUM_DEFENSE_LEVEL){
				if(!diplomat.GetPersonality()->GetExplorationMinimal()
				||  diplomat.GetPersonality()->GetConquestAgressive()
				){
					g_theStrategyDB->GetNamedItem("STRATEGY_DEFENSE_LOW", state.dbIndex);
				}
				else{
					g_theStrategyDB->GetNamedItem("STRATEGY_DEFENSE_MEDIUM", state.dbIndex);
				}
			}
			else if(max_threat > LOW_DEFENSE_LEVEL){
				if(!diplomat.GetPersonality()->GetExplorationMinimal()
				|| diplomat.GetPersonality()->GetConquestAgressive()
				){
					g_theStrategyDB->GetNamedItem("STRATEGY_DEFENSE_VERY_LOW", state.dbIndex);
				}
				else{
					g_theStrategyDB->GetNamedItem("STRATEGY_DEFENSE_LOW", state.dbIndex);
				}
			}
			else if(max_threat > MINIMUM_DEFENSE_LEVEL){
				if(!diplomat.GetPersonality()->GetExplorationMinimal()
				||  diplomat.GetPersonality()->GetConquestAgressive()){
					g_theStrategyDB->GetNamedItem("STRATEGY_DEFENSE_NONE", state.dbIndex);
				}
				else{
					g_theStrategyDB->GetNamedItem("STRATEGY_DEFENSE_VERY_LOW", state.dbIndex);
				}
			}
			else{
				g_theStrategyDB->GetNamedItem("STRATEGY_DEFENSE_NONE", state.dbIndex);
			}
		}
	}
	diplomat.ConsiderStrategicState(state);

	return GEV_HD_Continue;
}

//----------------------------------------------------------------------------
//
// Name       : CheckCityLimit_NextSStateEvent
//
// Description: Loads the over city limit strategy 
//
// Parameters : -
//
// Globals    : -
//
// Returns    : -
//
// Remark(s)  : -
//
//----------------------------------------------------------------------------
STDEHANDLER(CheckCityLimit_NextSStateEvent)
{
	PLAYER_INDEX playerId;

	
	if (!args->GetPlayer(0, playerId))
		return GEV_HD_Continue;

	Diplomat & diplomat = Diplomat::GetDiplomat(playerId);
	AiState state;

	const GovernmentRecord *government = 
	g_theGovernmentDB->Get(g_player[playerId]->GetGovernmentType());

	int acceptedCityMaximum = diplomat.GetPersonality()->GetCitiesOverLimit() + government->GetTooManyCitiesThreshold();

	if(g_player[playerId]->GetNumCities() > acceptedCityMaximum
	&& diplomat.GetPersonality()->GetOverCityLimitStrategy()
	){
		state.priority = diplomat.GetPersonality()->GetOverCityLimitStrategyPtr()->GetPriority();
		state.dbIndex = diplomat.GetPersonality()->GetOverCityLimitStrategyPtr()->GetStrategyIndex();
		diplomat.ConsiderStrategicState(state);
		if(wonderutil_GetRevoltingCitiesJoinPlayer(g_player[playerId]->m_builtWonders)
		&& diplomat.GetPersonality()->GetNoRevolutionStrategy()
		){
			state.priority = diplomat.GetPersonality()->GetNoRevolutionStrategyPtr()->GetPriority();
			state.dbIndex = diplomat.GetPersonality()->GetNoRevolutionStrategyPtr()->GetStrategyIndex();
			diplomat.ConsiderStrategicState(state);
		}
	}

	return GEV_HD_Continue;
}

//----------------------------------------------------------------------------
//
// Name       : StrategicStateEventCallbacks::AddCallbacks
//
// Description: Adds the functions above to the event manager callback,
//              so that all these functions are executed on the 
//              InitStrategicState or NextStrategicState event.
//
// Parameters : -
//
// Globals    : -
//
// Returns    : -
//
// Remark(s)  : -
//
//----------------------------------------------------------------------------
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

	g_gevManager->AddCallback(GEV_NextStrategicState, 
							  GEV_PRI_Pre, 
							  &s_CheckCityLimit_NextSStateEvent);
}
