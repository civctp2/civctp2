














#include "c3.h"

#include "NProposalEvent.h"

#include "Events.h"
#include "GameEventUser.h"
#include "Unit.h"
#include "StrDB.h"
#include "GameEventManager.h"

#include "Player.h"
#include "Strengths.h"

#include "Diplomat.h"
#include "MapAnalysis.h" 
#include "AdvanceRecord.h"
#include "AgreementMatrix.h"
#include "NewTurnCount.h"
#include "SelItem.h"
#include "CtpAi.h"
#include "ProposalAnalysis.h"
#include "c3math.h"
#include "UnitData.h"
#include "UnitPool.h"
#include "network.h"
#include "Pollution.h"
#include "Director.h"
#include "Gold.h"

extern SelectedItem *g_selected_item;
extern UnitPool *g_theUnitPool;


STDEHANDLER(NewProposalEvent)
{
	PLAYER_INDEX sender;
	PLAYER_INDEX receiver;	

	
	if (!args->GetPlayer(0, sender))
		return GEV_HD_Continue;

	
	if (!args->GetPlayer(1, receiver))
		return GEV_HD_Continue;

	Diplomat & sender_diplomat = Diplomat::GetDiplomat(sender);
	Diplomat & receiver_diplomat = Diplomat::GetDiplomat(receiver);

	const NewProposal & newProposal = sender_diplomat.GetMyLastNewProposal(receiver);

	
	if ( newProposal == Diplomat::s_badNewProposal ) {
		
		
		sender_diplomat.ContinueDiplomacy(receiver);
	} 
	else {
		

		
		
		
		
		
		g_gevManager->AddEvent(GEV_INSERT_Tail, GEV_NewProposalReady,
						   GEA_Player, sender,
						   GEA_Player, receiver,
						   GEA_End);

		
		
		
		bool execute = false;
		if(sender != g_selected_item->GetVisiblePlayer()) {
			if(g_network.IsActive() && !g_network.IsLocalPlayer(sender)) {
				execute = false;
			} else if(g_network.IsActive()) {
				execute = g_player[sender]->m_playerType == PLAYER_TYPE_ROBOT && g_network.IsLocalPlayer(sender);
			} else {
				execute = true;
			}
		}
		if((!execute) && (g_player[sender]->m_playerType == PLAYER_TYPE_ROBOT)) {
			if(!g_network.IsActive() || g_network.IsLocalPlayer(sender)) {
				execute = true;
			}
		}

		if(execute) {
			sender_diplomat.ExecuteNewProposal(receiver);
		}
		
		
		else {
			if(!g_network.IsClient()) {
				g_director->AddBeginScheduler(sender);
			}
		}

	}

	return GEV_HD_Continue;
}






STDEHANDLER(Scenario_NewProposalEvent) {
	sint32 motivation_type;
	PLAYER_INDEX sender;
	PLAYER_INDEX receiver;

	if (!args->GetInt(0, motivation_type))
		return GEV_HD_Continue;

	
	if (gameEventType == GEV_FearMotivation) {
		if (motivation_type != MOTIVATION_FEAR_SCENARIO)
			return GEV_HD_Continue;
	}
	else if (gameEventType == GEV_DesireMotivation) {
		if (motivation_type != MOTIVATION_DESIRE_SCENARIO)
			return GEV_HD_Continue;
	} 
	else
		
		Assert(false);

	if (!args->GetPlayer(0, sender))
		return GEV_HD_Continue;

	if (!args->GetPlayer(1, receiver))
		return GEV_HD_Continue;

	Diplomat & sender_diplomat = Diplomat::GetDiplomat(sender);

	
	

	
	
	sint32 priority = sender_diplomat.GetNewProposalPriority(receiver, PROPOSAL_TREATY_PEACE);

	if (priority <= 0)
		return GEV_HD_Continue;

	NewProposal new_proposal;
	
	
	new_proposal.priority = priority;				  
	
	
	new_proposal.detail.first_type = PROPOSAL_TREATY_PEACE; 
	new_proposal.detail.tone = DIPLOMATIC_TONE_EQUAL;	   
	
	new_proposal.receiverId = sender;
	new_proposal.senderId = receiver;
	
	
	
	g_theStringDB->GetStringID("EXPLAIN_SCENARIO_FORCE_PEACE",new_proposal.explainStrId);
	g_theStringDB->GetStringID("ADVICE_SCENARIO_FORCE_PEACE",new_proposal.adviceStrId);
	g_theStringDB->GetStringID("NEWS_SCENARIO_FORCE_PEACE",new_proposal.newsStrId);
	
	
	if (sender_diplomat.GetNewProposalTimeout( new_proposal, 25 ) )
	{
		
		return GEV_HD_Continue;
	}
	
	
	sender_diplomat.ConsiderNewProposal(receiver, new_proposal);
	
	
	return GEV_HD_Continue;
}


STDEHANDLER(LeaveOurBorders_NewProposalEvent) {
	PLAYER_INDEX sender;
	PLAYER_INDEX receiver;

	
	if (gameEventType != GEV_ReactionMotivation)
		return GEV_HD_Continue;

	if (!args->GetPlayer(0, sender))
		return GEV_HD_Continue;
	
	if (!args->GetPlayer(1, receiver))
		return GEV_HD_Continue;
	
	Diplomat & sender_diplomat = Diplomat::GetDiplomat(sender);
	Diplomat & receiver_diplomat = Diplomat::GetDiplomat(receiver);
	
	
	if (! sender_diplomat.GetBorderIncursionBy(receiver))
		return GEV_HD_Continue;
	
	sint16 duration = 
		AgreementMatrix::s_agreements.GetAgreementDuration(sender, receiver, PROPOSAL_REQUEST_WITHDRAW_TROOPS);
	
	
	if (duration >= 0 && duration < 25)
		return GEV_HD_Continue;
	
	
	
	sint32 priority = 
		sender_diplomat.GetNewProposalPriority(receiver, PROPOSAL_REQUEST_WITHDRAW_TROOPS);
	
	if (priority <= 0)
		return GEV_HD_Continue;
	
	NewProposal new_proposal;
	
	
	new_proposal.priority = priority;				
	
	
	new_proposal.detail.first_type = PROPOSAL_REQUEST_WITHDRAW_TROOPS;	
	new_proposal.detail.tone = DIPLOMATIC_TONE_EQUAL;	   
	
	new_proposal.receiverId = receiver;
	new_proposal.senderId = sender;

	
	
	sint32 at_risk_value_percent = 
		MapAnalysis::GetMapAnalysis().AtRiskCitiesValue(sender, receiver);
	if (at_risk_value_percent > 75)
	{
		sint32 next_advance = receiver_diplomat.GetNextAdvance();
		if (next_advance >= 0)
		{
			sint32 max_cost = g_theAdvanceDB->Get(next_advance)->GetCost();
			sint32 desired_advance = receiver_diplomat.GetDesiredAdvanceFrom( sender, 0, max_cost);
			
			
			if (desired_advance >= 0)
			{
				new_proposal.detail.second_type = PROPOSAL_OFFER_GIVE_ADVANCE;
				new_proposal.detail.second_arg.advanceType = desired_advance;
				
				new_proposal.detail.tone = DIPLOMATIC_TONE_MEEK;
			}
		}
	}

		
	if (sender_diplomat.GetNewProposalTimeout( new_proposal, 25 ) )
	{
		
		return GEV_HD_Continue;
	}
	
	
	sender_diplomat.ConsiderNewProposal(receiver, new_proposal);

	return GEV_HD_Continue;
}








STDEHANDLER(ExchangeMaps_NewProposalEvent) {

	sint32 desireType;
	PLAYER_INDEX sender;
	PLAYER_INDEX receiver;

	
	if (gameEventType != GEV_DesireMotivation)
		return GEV_HD_Continue;

	if (!args->GetInt(0, desireType))
		return GEV_HD_Continue;

	if ((MOTIVATION_TYPE) desireType != MOTIVATION_DESIRE_MAKE_FRIEND)
		return GEV_HD_Continue;

	if (!args->GetPlayer(0, sender))
		return GEV_HD_Continue;

	if (!args->GetPlayer(1, receiver))
		return GEV_HD_Continue;

	Diplomat & sender_diplomat = Diplomat::GetDiplomat(sender);

	
	if (sender_diplomat.GetTrust(receiver) < NEUTRAL_REGARD)
		return GEV_HD_Continue;

	Player *player_ptr = g_player[sender];
	Assert(player_ptr != NULL);

	
	
	sint32 offer_priority = 
		sender_diplomat.GetNewProposalPriority(receiver, PROPOSAL_OFFER_MAP);

	sint32 request_priority = 
		sender_diplomat.GetNewProposalPriority(receiver, PROPOSAL_REQUEST_MAP);

	
	if (sender_diplomat.TestPublicRegard(receiver, COLDWAR_REGARD))
		return GEV_HD_Continue;

	
	if (AgreementMatrix::s_agreements.HasAgreement(sender,receiver,PROPOSAL_TREATY_DECLARE_WAR) )
		return GEV_HD_Continue;

	
	sint32 want_map_turns;
	sender_diplomat.GetCurrentDiplomacy(receiver).GetWantMapTurns(want_map_turns);

	ai::Agreement found;
	found = AgreementMatrix::s_agreements.GetAgreement(sender,receiver, PROPOSAL_OFFER_MAP);

	
	sint32 sender_saw_map =
		AgreementMatrix::s_agreements.GetAgreementDuration(sender, receiver, PROPOSAL_REQUEST_MAP);

	
	
	if (sender_saw_map == -1 ||	sender_saw_map > want_map_turns)
		{
			NewProposal new_proposal;

			
			new_proposal.priority = (offer_priority + request_priority)/2;		

			
			new_proposal.detail.first_type = PROPOSAL_REQUEST_MAP;	
			new_proposal.detail.second_type = PROPOSAL_OFFER_MAP;	
			new_proposal.detail.tone = DIPLOMATIC_TONE_EQUAL;	    
			
			new_proposal.receiverId = receiver;
			new_proposal.senderId = sender;

			

			g_theStringDB->GetStringID("EXPLAIN_EXCHANGE_MAPS_0001",new_proposal.explainStrId);
			g_theStringDB->GetStringID("ADVICE_EXCHANGE_MAPS_0001",new_proposal.adviceStrId);
			g_theStringDB->GetStringID("NEWS_EXCHANGE_MAPS_0001",new_proposal.newsStrId);

			
			if (sender_diplomat.GetNewProposalTimeout( new_proposal, want_map_turns ) )
				{
					
					return GEV_HD_Continue;
				}

			
			sender_diplomat.ConsiderNewProposal(receiver, new_proposal);
		}
	
	return GEV_HD_Continue;
}


STDEHANDLER(MakePeace_NewProposalEvent)
{
	sint32 desireType;
	PLAYER_INDEX sender;
	PLAYER_INDEX receiver;

	
	if (gameEventType != GEV_DesireMotivation)
		return GEV_HD_Continue;

	if (!args->GetInt(0, desireType))
		return GEV_HD_Continue;

	if ((MOTIVATION_TYPE) desireType != MOTIVATION_DESIRE_MAKE_FRIEND)
		return GEV_HD_Continue;

	if (!args->GetPlayer(0, sender))
		return GEV_HD_Continue;

	if (!args->GetPlayer(1, receiver))
		return GEV_HD_Continue;

	Diplomat & sender_diplomat = Diplomat::GetDiplomat(sender);
	Diplomat & receiver_diplomat = Diplomat::GetDiplomat(receiver);

	
	
	sint32 priority =
		sender_diplomat.GetNewProposalPriority(receiver, PROPOSAL_TREATY_PEACE);

	if (priority <= 0)
		return GEV_HD_Continue;

	
	if (!sender_diplomat.TestEffectiveRegard(receiver, NEUTRAL_REGARD))
		return GEV_HD_Continue;

	
	if (sender_diplomat.GetLastHotwarAttack(receiver) < 10)
		return GEV_HD_Continue;

	
	if (sender_diplomat.GetLastColdwarAttack(receiver) < 5)
		return GEV_HD_Continue;

	
	if (sender_diplomat.GetLastBorderIncursionBy(receiver) + 20 < NewTurnCount::GetCurrentRound())
		return GEV_HD_Continue;

	
	if (sender_diplomat.GetTrust(receiver) < COLDWAR_REGARD)
		return GEV_HD_Continue;

	
	if (sender_diplomat.DesireWarWith(receiver))
		return GEV_HD_Continue;
	
	NewProposal new_proposal;
	
	
	new_proposal.priority = priority;	
	new_proposal.senderId = sender;		
	new_proposal.receiverId = receiver;	
	new_proposal.detail.first_type = PROPOSAL_TREATY_PEACE; 
	new_proposal.detail.tone = DIPLOMATIC_TONE_KIND;   
	
	
	g_theStringDB->GetStringID("EXPLAIN_PEACE_TREATY",new_proposal.explainStrId);
	g_theStringDB->GetStringID("ADVICE_PEACE_TREATY",new_proposal.adviceStrId);
	g_theStringDB->GetStringID("NEWS_PEACE_TREATY",new_proposal.newsStrId);
	
	
	if (sender_diplomat.GetNewProposalTimeout( new_proposal, 25 ) )
	{
		
		return GEV_HD_Continue;
	}
	
	
	sender_diplomat.ConsiderNewProposal( receiver, new_proposal);

	return GEV_HD_Continue;
}


STDEHANDLER(CeaseFire_NewProposalEvent)
{
	sint32 fearType;
	PLAYER_INDEX sender;
	PLAYER_INDEX receiver;

	
	if (gameEventType != GEV_FearMotivation)
		return GEV_HD_Continue;

	if (!args->GetInt(0, fearType))
		return GEV_HD_Continue;

	
	if ((MOTIVATION_TYPE) fearType != MOTIVATION_FEAR_INVASION &&
		(MOTIVATION_TYPE) fearType != MOTIVATION_FEAR_CITY_DEFENSE &&
		(MOTIVATION_TYPE) fearType != MOTIVATION_FEAR_MILITARY_RANK )
		return GEV_HD_Continue;

	if (!args->GetPlayer(0, sender))
		return GEV_HD_Continue;

	if (!args->GetPlayer(1, receiver))
		return GEV_HD_Continue;

	Diplomat & sender_diplomat = Diplomat::GetDiplomat(sender);
	Diplomat & receiver_diplomat = Diplomat::GetDiplomat(receiver);

	
	
	sint32 priority =
		sender_diplomat.GetNewProposalPriority(receiver, PROPOSAL_TREATY_CEASEFIRE);

	if (priority <= 0)
		return GEV_HD_Continue;

	DIPLOMATIC_STRENGTH sender_strength = 
		g_player[sender]->GetRelativeStrength(receiver);

	
	if (sender_strength > DIPLOMATIC_STRENGTH_AVERAGE )
		return GEV_HD_Continue;

	sint32 at_risk_value_percent = 
		MapAnalysis::GetMapAnalysis().AtRiskCitiesValue(sender, receiver);

	
	if (at_risk_value_percent < 75 && sender_diplomat.GetLastHotwarAttack(receiver) < 5)
		return GEV_HD_Continue;

	
	if (sender_diplomat.GetTrust(receiver) < HOTWAR_REGARD)
		return GEV_HD_Continue;

	
	if (sender_diplomat.DesireWarWith(receiver))
		return GEV_HD_Continue;

	NewProposal new_proposal;
	
	
	new_proposal.priority = priority;	
	new_proposal.senderId = sender;		
	new_proposal.receiverId = receiver;	
	new_proposal.detail.first_type = PROPOSAL_TREATY_CEASEFIRE; 
	new_proposal.detail.tone = DIPLOMATIC_TONE_EQUAL;	   

	
	
	if (at_risk_value_percent > 75)
	{
		
		new_proposal.detail.tone = DIPLOMATIC_TONE_MEEK;

		sint32 max_can_pay_gold = 
		MIN(g_player[receiver]->m_gold->GetIncome() * 2, g_player[sender]->GetGold());

		
		max_can_pay_gold = ProposalAnalysis::RoundGold(max_can_pay_gold);
		sint32 next_advance = receiver_diplomat.GetNextAdvance();

		
		if (max_can_pay_gold > 0)
		{
			new_proposal.detail.second_type = PROPOSAL_OFFER_GIVE_GOLD;
			new_proposal.detail.second_arg.gold = max_can_pay_gold;
	
		}
		
		else if (next_advance >= 0)
		{
			sint32 max_cost = g_theAdvanceDB->Get(next_advance)->GetCost();
			sint32 desired_advance = receiver_diplomat.GetDesiredAdvanceFrom( sender, 0, max_cost);
			
			
			if (desired_advance >= 0)
			{
				new_proposal.detail.second_type = PROPOSAL_OFFER_GIVE_ADVANCE;
				new_proposal.detail.second_arg.advanceType = desired_advance;
			}
		}
		
		else
		{
			
			new_proposal.detail.second_type = PROPOSAL_OFFER_MAP;
		}
	}

	
	if (sender_diplomat.GetNewProposalTimeout( new_proposal, 25 ) )
	{
		
		return GEV_HD_Continue;
	}
	
	
	sender_diplomat.ConsiderNewProposal( receiver, new_proposal);

	return GEV_HD_Continue;
}

STDEHANDLER(ReducePollution_NewProposalEvent)
{
	sint32 fearType;
	PLAYER_INDEX sender;
	PLAYER_INDEX receiver;

	
	if (gameEventType != GEV_FearMotivation)
		return GEV_HD_Continue;

	if (!args->GetInt(0, fearType))
		return GEV_HD_Continue;

	
	if ((MOTIVATION_TYPE) fearType != MOTIVATION_FEAR_POLLUTION)
		return GEV_HD_Continue;

	if (!args->GetPlayer(0, sender))
		return GEV_HD_Continue;

	if (!args->GetPlayer(1, receiver))
		return GEV_HD_Continue;

	Diplomat & sender_diplomat = Diplomat::GetDiplomat(sender);
	Diplomat & receiver_diplomat = Diplomat::GetDiplomat(receiver);

	
	
	sint32 priority =
		sender_diplomat.GetNewProposalPriority(receiver, PROPOSAL_REQUEST_REDUCE_POLLUTION);

	if (priority <= 0)
		return GEV_HD_Continue;

	uint32 sender_pollution = g_player[sender]->GetPollutionLevel();
	uint32 receiver_pollution = g_player[receiver]->GetPollutionLevel();
	double pollution_ratio = (double) sender_pollution / receiver_pollution;

	
	if (pollution_ratio > 1.5)
		return GEV_HD_Continue;

	double reduce_percent = 0.05; 
	DIPLOMATIC_TONE tone = DIPLOMATIC_TONE_EQUAL;

	
	if (sender_diplomat.GetPersonality()->GetDiscoveryEcotopian() &&
		pollution_ratio < 1.0)
	{
		reduce_percent = (double)(receiver_pollution - sender_pollution) / receiver_pollution;
		reduce_percent += 0.05;
		tone = DIPLOMATIC_TONE_ANGRY;
	}
	else if (sender_diplomat.GetPersonality()->GetDiscoveryDiplomatic())
	{
		reduce_percent = 0.1;
		tone = DIPLOMATIC_TONE_INDIGNANT;
	}

	
	reduce_percent = ProposalAnalysis::RoundPercentReduction(reduce_percent);
	sint32 target_pollution = ((1.0 - reduce_percent) * receiver_pollution);
	target_pollution = ProposalAnalysis::RoundGold(target_pollution);
	if (target_pollution <= 0)
		return GEV_HD_Continue;

	NewProposal new_proposal;
	
	
	new_proposal.priority = priority;	
	new_proposal.senderId = sender;		
	new_proposal.receiverId = receiver;	
	new_proposal.detail.first_type = PROPOSAL_REQUEST_REDUCE_POLLUTION; 
	new_proposal.detail.first_arg.pollution = target_pollution;
	new_proposal.detail.tone = DIPLOMATIC_TONE_EQUAL;	   
	
	
	if (sender_diplomat.GetNewProposalTimeout( new_proposal, 20 ) )
	{
		
		return GEV_HD_Continue;
	}
	
	
	sender_diplomat.ConsiderNewProposal( receiver, new_proposal);

	return GEV_HD_Continue;
}


STDEHANDLER(HonorPollutionAgreement_NewProposalEvent)
{
	sint32 fearType;
	PLAYER_INDEX sender;
	PLAYER_INDEX receiver;

	
	if (gameEventType != GEV_FearMotivation)
		return GEV_HD_Continue;

	if (!args->GetInt(0, fearType))
		return GEV_HD_Continue;

	
	if ((MOTIVATION_TYPE) fearType != MOTIVATION_FEAR_POLLUTION)
		return GEV_HD_Continue;

	if (!args->GetPlayer(0, sender))
		return GEV_HD_Continue;

	if (!args->GetPlayer(1, receiver))
		return GEV_HD_Continue;

	Diplomat & sender_diplomat = Diplomat::GetDiplomat(sender);
	Diplomat & receiver_diplomat = Diplomat::GetDiplomat(receiver);

	
	
	sint32 priority =
		sender_diplomat.GetNewProposalPriority(receiver, PROPOSAL_REQUEST_HONOR_POLLUTION_AGREEMENT);

	if (priority <= 0)
		return GEV_HD_Continue;

	
	ai::Agreement agreement = 
		AgreementMatrix::s_agreements.GetAgreement(sender, receiver, PROPOSAL_TREATY_POLLUTION_PACT);

	if (agreement == AgreementMatrix::s_badAgreement)
		agreement = AgreementMatrix::s_agreements.GetAgreement(sender, receiver, PROPOSAL_REQUEST_REDUCE_POLLUTION);

	if (agreement == AgreementMatrix::s_badAgreement)
		return GEV_HD_Continue;

	uint32 receiver_pollution = g_player[receiver]->GetPollutionLevel();
	sint32 promised_pollution = agreement.proposal.first_arg.pollution;;
	sint32 half_promised_pollution = promised_pollution / 2;
	double pollution_ratio = (double) receiver_pollution / promised_pollution;

	sint32 turns_since_start = NewTurnCount::GetCurrentRound() - agreement.start;

	
	if (pollution_ratio <= 1.0)
		return GEV_HD_Continue;

	
	if (sender_diplomat.GetPersonality()->GetDiscoveryEcotopian() &&
		turns_since_start < 10)
	{
		return GEV_HD_Continue;
	}
	
	else 
	{
		
		if (turns_since_start < 20) 
			return GEV_HD_Continue;
		
		
		if (turns_since_start > 20 && turns_since_start < 50 &&
			receiver_pollution <= half_promised_pollution)
			return GEV_HD_Continue;
	}

	DIPLOMATIC_TONE tone = DIPLOMATIC_TONE_EQUAL;

	NewProposal new_proposal;
	
	
	new_proposal.priority = priority;	
	new_proposal.senderId = sender;		
	new_proposal.receiverId = receiver;	
	new_proposal.detail.first_type = PROPOSAL_REQUEST_HONOR_POLLUTION_AGREEMENT; 
	new_proposal.detail.first_arg.pollution = promised_pollution;
	new_proposal.detail.tone = DIPLOMATIC_TONE_INDIGNANT;	   
	
	
	if (sender_diplomat.GetNewProposalTimeout( new_proposal, 20 ) )
	{
		
		return GEV_HD_Continue;
	}
	
	
	sender_diplomat.ConsiderNewProposal( receiver, new_proposal);

	return GEV_HD_Continue;
}


STDEHANDLER(TradePact_NewProposalEvent)
{
	sint32 desireType;
	PLAYER_INDEX sender;
	PLAYER_INDEX receiver;

	
	if (gameEventType != GEV_DesireMotivation)
		return GEV_HD_Continue;

	if (!args->GetInt(0, desireType))
		return GEV_HD_Continue;

	if ((MOTIVATION_TYPE) desireType != MOTIVATION_DESIRE_MAKE_FRIEND ||
		(MOTIVATION_TYPE) desireType != MOTIVATION_DESIRE_GOLD)
		return GEV_HD_Continue;

	if (!args->GetPlayer(0, sender))
		return GEV_HD_Continue;

	if (!args->GetPlayer(1, receiver))
		return GEV_HD_Continue;

	Diplomat & sender_diplomat = Diplomat::GetDiplomat(sender);
	Diplomat & receiver_diplomat = Diplomat::GetDiplomat(receiver);

	
	if (AgreementMatrix::s_agreements.HasAgreement(sender, receiver, PROPOSAL_TREATY_TRADE_PACT))
		return GEV_HD_Continue;

	
	
	sint32 priority =
		sender_diplomat.GetNewProposalPriority(receiver, PROPOSAL_TREATY_TRADE_PACT);

	if (priority <= 0)
		return GEV_HD_Continue;

	
	if (!sender_diplomat.TestEffectiveRegard(receiver, FRIEND_REGARD))
			return GEV_HD_Continue;

	
	if (sender_diplomat.GetLastHotwarAttack(receiver) < 10)
		return GEV_HD_Continue;

	
	if (MapAnalysis::GetMapAnalysis().GetPiracyIncomeByPlayer(receiver, sender) > 0)
		return GEV_HD_Continue;

	
	if (sender_diplomat.GetBorderIncursionBy(receiver))
		return GEV_HD_Continue;

	
	if (sender_diplomat.GetTrust(receiver) < NEUTRAL_REGARD)
		return GEV_HD_Continue;

	sint32 sender_trade_from = sender_diplomat.GetTradeFrom(receiver);
	sint32 receiver_trade_from = receiver_diplomat.GetTradeFrom(sender);
	sint32 sender_trade_total = MapAnalysis::GetMapAnalysis().GetTotalTrade(sender);

	
	if ( ((double) sender_trade_from / sender_trade_total) < 0.10)
		return GEV_HD_Continue;

	
	
	if ( receiver_trade_from > sender_trade_from * 2)
		return GEV_HD_Continue;

	NewProposal new_proposal;
	
	
	new_proposal.priority = priority;	
	new_proposal.senderId = sender;		
	new_proposal.receiverId = receiver;	
	new_proposal.detail.first_type = PROPOSAL_TREATY_TRADE_PACT; 
	new_proposal.detail.tone = DIPLOMATIC_TONE_EQUAL;   
	
	
	if (sender_diplomat.GetNewProposalTimeout( new_proposal, 22 ) )
	{
		
		return GEV_HD_Continue;
	}
	
	
	sender_diplomat.ConsiderNewProposal( receiver, new_proposal);

	return GEV_HD_Continue;
}


STDEHANDLER(ResearchPact_NewProposalEvent)
{
	PLAYER_INDEX sender;
	PLAYER_INDEX receiver;

	
	if (gameEventType == GEV_DesireMotivation) 
	{
		sint32 desireType;
		
		if (!args->GetInt(0, desireType))
			return GEV_HD_Continue;
		
		if ((MOTIVATION_TYPE) desireType != MOTIVATION_DESIRE_MAKE_FRIEND)
			return GEV_HD_Continue;
	}
	else if (gameEventType == GEV_FearMotivation) 
	{
		sint32 fearType;
		
		if (!args->GetInt(0, fearType))
			return GEV_HD_Continue;
		
		if ((MOTIVATION_TYPE) fearType != MOTIVATION_FEAR_SCIENCE_RANK)
			return GEV_HD_Continue;
	}
	else
		return GEV_HD_Continue;

	if (!args->GetPlayer(0, sender))
		return GEV_HD_Continue;

	if (!args->GetPlayer(1, receiver))
		return GEV_HD_Continue;

	Diplomat & sender_diplomat = Diplomat::GetDiplomat(sender);
	Diplomat & receiver_diplomat = Diplomat::GetDiplomat(receiver);

	
	
	sint32 priority =
		sender_diplomat.GetNewProposalPriority(receiver, PROPOSAL_TREATY_RESEARCH_PACT);

	if (priority <= 0)
		return GEV_HD_Continue;

	
	if (!sender_diplomat.TestEffectiveRegard(receiver, FRIEND_REGARD))
			return GEV_HD_Continue;

	
	if (sender_diplomat.GetLastHotwarAttack(receiver) < 10)
		return GEV_HD_Continue;

	
	if (sender_diplomat.GetBorderIncursionBy(receiver))
		return GEV_HD_Continue;

	
	if (sender_diplomat.GetTrust(receiver) < NEUTRAL_REGARD)
		return GEV_HD_Continue;

	sint32 sender_science = MapAnalysis::GetMapAnalysis().GetProjectedScience(sender);
	sint32 receiver_science = MapAnalysis::GetMapAnalysis().GetProjectedScience(receiver);

	
	if ( sender_science > receiver_science && 
		 ((double) receiver_science / sender_science < 0.8))
		return GEV_HD_Continue;

	NewProposal new_proposal;
	
	
	new_proposal.priority = priority;	
	new_proposal.senderId = sender;		
	new_proposal.receiverId = receiver;	
	new_proposal.detail.first_type = PROPOSAL_TREATY_RESEARCH_PACT; 

    
	if (sender_science < receiver_science)
		new_proposal.detail.tone = DIPLOMATIC_TONE_MEEK; 
	else
		new_proposal.detail.tone = DIPLOMATIC_TONE_EQUAL;
	
	
	if (sender_diplomat.GetNewProposalTimeout( new_proposal, 20 ) )
	{
		
		return GEV_HD_Continue;
	}
	
	
	sender_diplomat.ConsiderNewProposal( receiver, new_proposal);

	return GEV_HD_Continue;
}


STDEHANDLER(MilitaryPact_NewProposalEvent)
{
	PLAYER_INDEX sender;
	PLAYER_INDEX receiver;

	
	if (gameEventType == GEV_DesireMotivation) 
	{
		sint32 desireType;
		
		if (!args->GetInt(0, desireType))
			return GEV_HD_Continue;
		
		if ((MOTIVATION_TYPE) desireType != MOTIVATION_DESIRE_MAKE_FRIEND ||
			(MOTIVATION_TYPE) desireType != MOTIVATION_DESIRE_ENLIST_FRIEND)
			return GEV_HD_Continue;
	}
	else if (gameEventType == GEV_FearMotivation) 
	{
		sint32 fearType;
		
		if (!args->GetInt(0, fearType))
			return GEV_HD_Continue;
		
		if ((MOTIVATION_TYPE) fearType != MOTIVATION_FEAR_MILITARY_RANK)
			return GEV_HD_Continue;
	}
	else
		return GEV_HD_Continue;

	if (!args->GetPlayer(0, sender))
		return GEV_HD_Continue;

	if (!args->GetPlayer(1, receiver))
		return GEV_HD_Continue;

	Diplomat & sender_diplomat = Diplomat::GetDiplomat(sender);
	Diplomat & receiver_diplomat = Diplomat::GetDiplomat(receiver);

	
	if (AgreementMatrix::s_agreements.HasAgreement(sender, receiver, PROPOSAL_TREATY_MILITARY_PACT))
		return GEV_HD_Continue;

	
	
	sint32 priority =
		sender_diplomat.GetNewProposalPriority(receiver, PROPOSAL_TREATY_MILITARY_PACT);

	if (priority <= 0)
		return GEV_HD_Continue;

	
	if (!sender_diplomat.TestEffectiveRegard(receiver, FRIEND_REGARD))
			return GEV_HD_Continue;

	
	if (sender_diplomat.GetLastHotwarAttack(receiver) < 10)
		return GEV_HD_Continue;

	
	if (sender_diplomat.GetBorderIncursionBy(receiver))
		return GEV_HD_Continue;

	
	if (sender_diplomat.GetTrust(receiver) < NEUTRAL_REGARD)
		return GEV_HD_Continue;

	
	if (!sender_diplomat.CanFormAlliance(receiver))
		return GEV_HD_Continue;

	NewProposal new_proposal;
	
	
	new_proposal.priority = priority;	
	new_proposal.senderId = sender;		
	new_proposal.receiverId = receiver;	
	new_proposal.detail.first_type = PROPOSAL_TREATY_MILITARY_PACT; 
	new_proposal.detail.tone = DIPLOMATIC_TONE_EQUAL;   
	
	
	if (sender_diplomat.GetNewProposalTimeout( new_proposal, 20 ) )
	{
		
		return GEV_HD_Continue;
	}
	
	
	sender_diplomat.ConsiderNewProposal( receiver, new_proposal);

	return GEV_HD_Continue;
}


STDEHANDLER(PollutionPact_NewProposalEvent)
{
	sint32 fearType;
	PLAYER_INDEX sender;
	PLAYER_INDEX receiver;

	
	if (gameEventType != GEV_FearMotivation)
		return GEV_HD_Continue;

	if (!args->GetInt(0, fearType))
		return GEV_HD_Continue;

	if ((MOTIVATION_TYPE) fearType != MOTIVATION_FEAR_POLLUTION)
		return GEV_HD_Continue;

	if (!args->GetPlayer(0, sender))
		return GEV_HD_Continue;

	if (!args->GetPlayer(1, receiver))
		return GEV_HD_Continue;

	Diplomat & sender_diplomat = Diplomat::GetDiplomat(sender);
	Diplomat & receiver_diplomat = Diplomat::GetDiplomat(receiver);

	
	if (AgreementMatrix::s_agreements.HasAgreement(sender, receiver, PROPOSAL_TREATY_POLLUTION_PACT))
		return GEV_HD_Continue;

	
	sint32 priority =
		sender_diplomat.GetNewProposalPriority(receiver, PROPOSAL_TREATY_POLLUTION_PACT);

	if (priority <= 0)
		return GEV_HD_Continue;

	
	if (!sender_diplomat.TestEffectiveRegard(receiver, NEUTRAL_REGARD))
			return GEV_HD_Continue;

	
	if (sender_diplomat.GetLastHotwarAttack(receiver) < 10)
		return GEV_HD_Continue;

	
	if (MapAnalysis::GetMapAnalysis().GetPiracyIncomeByPlayer(receiver, sender) > 0)
		return GEV_HD_Continue;

	
	if (sender_diplomat.GetBorderIncursionBy(receiver))
		return GEV_HD_Continue;

	
	if (sender_diplomat.GetTrust(receiver) < NEUTRAL_REGARD)
		return GEV_HD_Continue;

	uint32 sender_pollution = g_player[sender]->GetPollutionLevel();
	uint32 receiver_pollution = g_player[receiver]->GetPollutionLevel();
	double pollution_ratio = (double) sender_pollution / receiver_pollution;

	
	if (pollution_ratio > 1.5)
		return GEV_HD_Continue;

	double reduce_percent = 0.05; 
	DIPLOMATIC_TONE tone = DIPLOMATIC_TONE_EQUAL;

	
	if (sender_diplomat.GetPersonality()->GetDiscoveryEcotopian() &&
		pollution_ratio < 1.0)
	{
		reduce_percent = (double)(receiver_pollution - sender_pollution) / receiver_pollution;
		reduce_percent += 0.05;
		tone = DIPLOMATIC_TONE_ANGRY;
	}
	else if (sender_diplomat.GetPersonality()->GetDiscoveryDiplomatic())
	{
		reduce_percent = 0.1;
		tone = DIPLOMATIC_TONE_EQUAL;
	}

	
	reduce_percent = ProposalAnalysis::RoundPercentReduction(reduce_percent);
	sint32 target_pollution = (reduce_percent * receiver_pollution);
	target_pollution = ProposalAnalysis::RoundGold(target_pollution);
	if (target_pollution <= 0)
		return GEV_HD_Continue;

	NewProposal new_proposal;
	
	
	new_proposal.priority = priority;	
	new_proposal.senderId = sender;		
	new_proposal.receiverId = receiver;	
	new_proposal.detail.first_type = PROPOSAL_TREATY_POLLUTION_PACT; 
	new_proposal.detail.first_arg.pollution = (1.0 - reduce_percent) * receiver_pollution;
	new_proposal.detail.tone = DIPLOMATIC_TONE_EQUAL;   
	
	
	if (sender_diplomat.GetNewProposalTimeout( new_proposal, 20 ) )
	{
		
		return GEV_HD_Continue;
	}
	
	
	sender_diplomat.ConsiderNewProposal( receiver, new_proposal);

	return GEV_HD_Continue;
}


STDEHANDLER(Alliance_NewProposalEvent)
{
	sint32 desireType;
	PLAYER_INDEX sender;
	PLAYER_INDEX receiver;

	
	if (gameEventType != GEV_DesireMotivation)
		return GEV_HD_Continue;

	if (!args->GetInt(0, desireType))
		return GEV_HD_Continue;

	if ((MOTIVATION_TYPE) desireType != MOTIVATION_DESIRE_MAKE_FRIEND)
		return GEV_HD_Continue;

	if (!args->GetPlayer(0, sender))
		return GEV_HD_Continue;

	if (!args->GetPlayer(1, receiver))
		return GEV_HD_Continue;

	Diplomat & sender_diplomat = Diplomat::GetDiplomat(sender);
	Diplomat & receiver_diplomat = Diplomat::GetDiplomat(receiver);

	
	if (AgreementMatrix::s_agreements.HasAgreement(sender, receiver, PROPOSAL_TREATY_ALLIANCE))
		return GEV_HD_Continue;

	
	
	sint32 priority =
		sender_diplomat.GetNewProposalPriority(receiver, PROPOSAL_TREATY_ALLIANCE);

	if (priority <= 0)
		return GEV_HD_Continue;

	
	if (!sender_diplomat.TestEffectiveRegard(receiver, ALLIED_REGARD))
			return GEV_HD_Continue;

	
	if (sender_diplomat.GetLastHotwarAttack(receiver) < 25)
		return GEV_HD_Continue;

	
	if (sender_diplomat.GetLastColdwarAttack(receiver) < 15)
		return GEV_HD_Continue;

	
	if (sender_diplomat.GetBorderIncursionBy(receiver))
		return GEV_HD_Continue;

	
	if (sender_diplomat.GetTrust(receiver) < FRIEND_REGARD)
		return GEV_HD_Continue;

	
	if (!sender_diplomat.CanFormAlliance(receiver))
		return GEV_HD_Continue;

	NewProposal new_proposal;
	
	
	new_proposal.priority = priority;	
	new_proposal.senderId = sender;		
	new_proposal.receiverId = receiver;	
	new_proposal.detail.first_type = PROPOSAL_TREATY_ALLIANCE; 
	new_proposal.detail.tone = DIPLOMATIC_TONE_KIND;   
	
	
	if (sender_diplomat.GetNewProposalTimeout( new_proposal, 50 ) )
	{
		
		return GEV_HD_Continue;
	}
	
	
	sender_diplomat.ConsiderNewProposal( receiver, new_proposal);

	return GEV_HD_Continue;
}


STDEHANDLER(RequestAdvance_NewProposalEvent)
{
	sint32 fearType;
	PLAYER_INDEX sender;
	PLAYER_INDEX receiver;

	
	if (gameEventType != GEV_FearMotivation)
		return GEV_HD_Continue;

	if (!args->GetInt(0, fearType))
		return GEV_HD_Continue;

	
	
	if ((MOTIVATION_TYPE) fearType != MOTIVATION_FEAR_SCIENCE_RANK)
		return GEV_HD_Continue;

	if (!args->GetPlayer(0, sender))
		return GEV_HD_Continue;

	if (!args->GetPlayer(1, receiver))
		return GEV_HD_Continue;

	sint32 sender_science_rank = g_player[sender]->GetRank(STRENGTH_CAT_KNOWLEDGE);
	sint32 receiver_science_rank = g_player[receiver]->GetRank(STRENGTH_CAT_KNOWLEDGE);

	Diplomat & sender_diplomat = Diplomat::GetDiplomat(sender);
	
	if (
		
		sender_diplomat.TestEffectiveRegard(receiver, NEUTRAL_REGARD) &&

		
		
		( receiver_science_rank > sender_science_rank + 20) )
	{
		
		NewProposal new_proposal;

		sint32 next_advance = sender_diplomat.GetNextAdvance();
		sint32 max_cost = 0;
		if (next_advance >= 0)
		{
			max_cost = g_theAdvanceDB->Get(next_advance)->GetCost();
		}
		else
		{
			return GEV_HD_Continue;
		}

		
		max_cost = (sint32) ((double) max_cost * 1.20);
		sint32 min_cost = (sint32) ((double) max_cost * 0.50);

		sint32 desired_advance = sender_diplomat.GetDesiredAdvanceFrom( receiver,  min_cost, max_cost);
		
		if (desired_advance < 0)
			{
				return GEV_HD_Continue;
			}

		
		sint32 priority =
			sender_diplomat.GetNewProposalPriority(receiver, PROPOSAL_REQUEST_GIVE_ADVANCE);

		new_proposal.priority = priority;				
		new_proposal.senderId = sender;			
		new_proposal.receiverId = receiver;		
		new_proposal.detail.first_type = PROPOSAL_REQUEST_GIVE_ADVANCE;	
		new_proposal.detail.first_arg.advanceType = desired_advance;
		new_proposal.detail.second_type = PROPOSAL_NONE;	

		new_proposal.detail.tone = DIPLOMATIC_TONE_EQUAL;	   

		
		g_theStringDB->GetStringID("EXPLAIN_REQUEST_ADVANCE",new_proposal.explainStrId);
		g_theStringDB->GetStringID("ADVICE_REQUEST_ADVANCE",new_proposal.adviceStrId);
		g_theStringDB->GetStringID("NEWS_REQUEST_ADVANCE",new_proposal.newsStrId);

		
		if (sender_diplomat.GetNewProposalTimeout( new_proposal, 20 ) )
			{
				
				return GEV_HD_Continue;
			}

		
		sender_diplomat.ConsiderNewProposal( receiver, new_proposal);
	}
	
	return GEV_HD_Continue;
}



STDEHANDLER(StopPiracy_NewProposalEvent)
{
	sint32 motivation_type;
	PLAYER_INDEX sender;
	PLAYER_INDEX receiver;

	
	if (gameEventType != GEV_FearMotivation)
		return GEV_HD_Continue;

	if (!args->GetInt(0, motivation_type))
		return GEV_HD_Continue;

	
	if ((MOTIVATION_TYPE) motivation_type != MOTIVATION_FEAR_PIRACY )
		return GEV_HD_Continue;

	if (!args->GetPlayer(0, sender))
		return GEV_HD_Continue;

	if (!args->GetPlayer(1, receiver))
		return GEV_HD_Continue;

	Diplomat & sender_diplomat = Diplomat::GetDiplomat(sender);
	Diplomat & receiver_diplomat = Diplomat::GetDiplomat(receiver);
	const MapAnalysis & map = MapAnalysis::GetMapAnalysis();

	
	
	sint32 priority =
		sender_diplomat.GetNewProposalPriority(receiver, PROPOSAL_REQUEST_STOP_PIRACY);

	
	
	sint32 turns_since_last_war = AgreementMatrix::s_agreements.TurnsSinceLastWar(sender, receiver);
	if ( (map.GetPiracyIncomeByPlayer(receiver, sender) > 0) &&
		  ((turns_since_last_war > 0) || (turns_since_last_war == -1)))
	{
		
		NewProposal new_proposal;

		
		new_proposal.priority = priority;	
		new_proposal.senderId = sender;		
		new_proposal.receiverId = receiver;	
		new_proposal.detail.first_type = PROPOSAL_REQUEST_STOP_PIRACY; 
		new_proposal.detail.tone = DIPLOMATIC_TONE_EQUAL;	   

		
		g_theStringDB->GetStringID("EXPLAIN_PEACE_TREATY",new_proposal.explainStrId);
		g_theStringDB->GetStringID("ADVICE_PEACE_TREATY",new_proposal.adviceStrId);
		g_theStringDB->GetStringID("NEWS_PEACE_TREATY",new_proposal.newsStrId);

		
		if (sender_diplomat.GetNewProposalTimeout( new_proposal, 20 ) )
			{
				
				return GEV_HD_Continue;
			}

		
		sender_diplomat.ConsiderNewProposal( receiver, new_proposal );
	}
	
	return GEV_HD_Continue;
}


STDEHANDLER(BegForGold_NewProposalEvent)
{
	PLAYER_INDEX sender;
	PLAYER_INDEX receiver;

	
	if (gameEventType != GEV_DesireMotivation)
		return GEV_HD_Continue;

	sint32 desireType;
	if (!args->GetInt(0, desireType))
		return GEV_HD_Continue;

	
	if ((MOTIVATION_TYPE) desireType != MOTIVATION_DESIRE_GOLD &&
		(MOTIVATION_TYPE) desireType != MOTIVATION_DESIRE_ENLIST_FRIEND)
		return GEV_HD_Continue;

	if (!args->GetPlayer(0, sender))
		return GEV_HD_Continue;

	if (!args->GetPlayer(1, receiver))
		return GEV_HD_Continue;

	Diplomat & receiver_diplomat = Diplomat::GetDiplomat(receiver);
	Diplomat & sender_diplomat = Diplomat::GetDiplomat(receiver);

	bool allied = 
		AgreementMatrix::s_agreements.HasAgreement(sender,receiver,PROPOSAL_TREATY_MILITARY_PACT) ||
		AgreementMatrix::s_agreements.HasAgreement(sender,receiver,PROPOSAL_TREATY_ALLIANCE);

	
	if (!receiver_diplomat.TestPublicRegard(sender, FRIEND_REGARD) && !allied)
		return GEV_HD_Continue;

	DIPLOMATIC_STRENGTH receiver_strength = 
		g_player[receiver]->GetRelativeStrength(sender);

	
	if (receiver_strength < DIPLOMATIC_STRENGTH_STRONG )
		return GEV_HD_Continue;

	
	if (MapAnalysis::GetMapAnalysis().AtRiskCitiesValue(sender) < 60)
		return GEV_HD_Continue;

	
	sint32 gold = 0;

	
	gold = 5 * g_player[sender]->m_gold->GetIncome();

	
	gold = ProposalAnalysis::RoundGold(gold);

	if (gold <= 0)
		return GEV_HD_Continue;
	
	NewProposal new_proposal;

	
	
	sint32 priority =
		sender_diplomat.GetNewProposalPriority(receiver, PROPOSAL_REQUEST_GIVE_GOLD);
	
	
	new_proposal.priority = priority;				
	
	
	new_proposal.detail.first_type = PROPOSAL_REQUEST_GIVE_GOLD;	
	new_proposal.detail.first_arg.gold = gold; 

	if (allied)
		
		new_proposal.detail.tone = DIPLOMATIC_TONE_KIND;	   
	else
		
		new_proposal.detail.tone = DIPLOMATIC_TONE_MEEK;	   
	
	new_proposal.receiverId = receiver;
	new_proposal.senderId = sender;
	
	
	if (sender_diplomat.GetNewProposalTimeout( new_proposal, 50 ) )
	{
		
		return GEV_HD_Continue;
	}
	
	
	sender_diplomat.ConsiderNewProposal(receiver, new_proposal);
	
	
	return GEV_HD_Continue;
}


STDEHANDLER(BlackmailGold_NewProposalEvent)
{
	PLAYER_INDEX sender;
	PLAYER_INDEX receiver;

	
	if (gameEventType != GEV_DesireMotivation)
		return GEV_HD_Continue;

	sint32 desireType;
	if (!args->GetInt(0, desireType))
		return GEV_HD_Continue;

	
	if ((MOTIVATION_TYPE) desireType != MOTIVATION_DESIRE_GOLD &&
		(MOTIVATION_TYPE) desireType != MOTIVATION_DESIRE_ATTACK)
		return GEV_HD_Continue;

	if (!args->GetPlayer(0, sender))
		return GEV_HD_Continue;

	if (!args->GetPlayer(1, receiver))
		return GEV_HD_Continue;

	Diplomat & sender_diplomat = Diplomat::GetDiplomat(sender);

	
	if (!sender_diplomat.GetPersonality()->GetConquestAgressive() &&
		!sender_diplomat.GetPersonality()->GetDiscoveryMilitary())
		return GEV_HD_Continue;

	
	if (!sender_diplomat.TestEffectiveRegard(receiver, COLDWAR_REGARD))
		return GEV_HD_Continue;

	Assert(g_player[sender]);
	Assert(g_player[receiver]);

	DIPLOMATIC_STRENGTH sender_strength = 
		g_player[sender]->GetRelativeStrength(receiver);

	
	if (sender_strength < DIPLOMATIC_STRENGTH_STRONG )
		return GEV_HD_Continue;

	sint32 at_risk_value_percent = 
		MapAnalysis::GetMapAnalysis().AtRiskCitiesValue(receiver,sender);

	
	
	if (at_risk_value_percent < 30)
		return GEV_HD_Continue;

	
	sint32 gold = 0;

	
	gold = (2 * g_player[sender]->m_gold->GetIncome());

	
	gold = ProposalAnalysis::RoundGold(gold);

	
	
	sint32 priority =
		sender_diplomat.GetNewProposalPriority(receiver, PROPOSAL_REQUEST_GIVE_GOLD);
	
	if (gold <= 0)
		return GEV_HD_Continue;
	
	NewProposal new_proposal;
	
	
	new_proposal.priority = priority;				
	
	
	new_proposal.detail.first_type = PROPOSAL_REQUEST_GIVE_GOLD;	
	new_proposal.detail.first_arg.gold = gold; 

	
	new_proposal.detail.tone = DIPLOMATIC_TONE_ANGRY;	   
	
	new_proposal.receiverId = receiver;
	new_proposal.senderId = sender;
	
	
	if (sender_diplomat.GetNewProposalTimeout( new_proposal, 50 ) )
	{
		
		return GEV_HD_Continue;
	}
	
	
	sender_diplomat.ConsiderNewProposal(receiver, new_proposal);
	
	
	return GEV_HD_Continue;
}


STDEHANDLER(BreakAgreementWithEnemy_NewProposalEvent)
{
	
	if (gameEventType != GEV_DesireMotivation)
		return GEV_HD_Continue;

	sint32 motivation_type;
	if (!args->GetInt(0, motivation_type))
		return GEV_HD_Continue;

	
	if ((MOTIVATION_TYPE) motivation_type != MOTIVATION_DESIRE_ENLIST_FRIEND )
		return GEV_HD_Continue;

	PLAYER_INDEX sender;
	PLAYER_INDEX receiver;

	if (!args->GetPlayer(0, sender))
		return GEV_HD_Continue;

	if (!args->GetPlayer(1, receiver))
		return GEV_HD_Continue;

	Diplomat & sender_diplomat = Diplomat::GetDiplomat(sender);

	
	if (sender_diplomat.TestPublicRegard(receiver, FRIEND_REGARD) == false)
		return GEV_HD_Continue;

	const Diplomat & receiver_diplomat = Diplomat::GetDiplomat(receiver);
	ai::Agreement agreement;

	for (PLAYER_INDEX cold_war_enemy = 1; cold_war_enemy < CtpAi::s_maxPlayers; cold_war_enemy++)
	{
		if (g_player[cold_war_enemy] == NULL)
			continue;

		
		if (receiver_diplomat.GetPublicRegard(cold_war_enemy) < FRIEND_REGARD)
			continue;

		agreement = AgreementMatrix::s_agreements.GetAgreement(receiver, cold_war_enemy, PROPOSAL_TREATY_TRADE_PACT);

		if (agreement == AgreementMatrix::s_badAgreement)
			agreement = AgreementMatrix::s_agreements.GetAgreement(receiver, cold_war_enemy, PROPOSAL_TREATY_RESEARCH_PACT);

		if (agreement == AgreementMatrix::s_badAgreement)
			agreement = AgreementMatrix::s_agreements.GetAgreement(receiver, cold_war_enemy, PROPOSAL_TREATY_MILITARY_PACT);

		
		if (agreement == AgreementMatrix::s_badAgreement)
			continue;
		
		break;
	}

	
	if (cold_war_enemy == CtpAi::s_maxPlayers)
		return GEV_HD_Continue;

	
	
	sint32 priority =
		sender_diplomat.GetNewProposalPriority(receiver, PROPOSAL_REQUEST_BREAK_AGREEMENT);
	
	if (priority > 0) {
		NewProposal new_proposal;

		
		new_proposal.priority = priority;				

		
		new_proposal.detail.first_type = PROPOSAL_REQUEST_BREAK_AGREEMENT;	
		new_proposal.detail.first_arg.playerId = cold_war_enemy; 
		
		new_proposal.detail.tone = DIPLOMATIC_TONE_ANGRY;	   

		new_proposal.receiverId = receiver;
		new_proposal.senderId = sender;

		

		g_theStringDB->GetStringID("EXPLAIN_WITHDRAW_TROOPS_0001",new_proposal.explainStrId);
		g_theStringDB->GetStringID("ADVICE_WITHDRAW_TROOPS_0001",new_proposal.adviceStrId);
		g_theStringDB->GetStringID("NEWS_WITHDRAW_TROOPS_0001",new_proposal.newsStrId);

		
		if (sender_diplomat.GetNewProposalTimeout( new_proposal, 20 ) )
			{
				
				return GEV_HD_Continue;
			}

		
		sender_diplomat.ConsiderNewProposal(receiver, new_proposal);
	}
	
	return GEV_HD_Continue;
}


STDEHANDLER(StopResearch_NewProposalEvent)
{
	
	if (gameEventType != GEV_FearMotivation)
		return GEV_HD_Continue;

	sint32 motivation_type;
	if (!args->GetInt(0, motivation_type))
		return GEV_HD_Continue;

	
	if ((MOTIVATION_TYPE) motivation_type != MOTIVATION_FEAR_SCIENCE_RANK )
		return GEV_HD_Continue;

	PLAYER_INDEX sender;
	PLAYER_INDEX receiver;

	if (!args->GetPlayer(0, sender))
		return GEV_HD_Continue;

	if (!args->GetPlayer(1, receiver))
		return GEV_HD_Continue;

	Diplomat & sender_diplomat = Diplomat::GetDiplomat(sender);

	
	if (sender_diplomat.TestEffectiveRegard(receiver, COLDWAR_REGARD) == false)
		return GEV_HD_Continue;

	
	Assert(g_player[sender])
	if (g_player[sender]->HasEmbassyWith(receiver) == FALSE)
		return GEV_HD_Continue;

	AdvanceType adv_type = sender_diplomat.GetStopResearchingAdvance(receiver);
	
	
	if (adv_type <= -1)
		return GEV_HD_Continue;

	
	sint32 priority =
		sender_diplomat.GetNewProposalPriority(receiver, PROPOSAL_REQUEST_STOP_RESEARCH);

	if (priority <= 0) 
		return GEV_HD_Continue;

	NewProposal new_proposal;
	
	
	new_proposal.priority = priority;				
	
	
	new_proposal.detail.first_type = PROPOSAL_REQUEST_STOP_RESEARCH;	
	new_proposal.detail.first_arg.advanceType = adv_type; 
	new_proposal.detail.tone = DIPLOMATIC_TONE_INDIGNANT;	

	new_proposal.receiverId = receiver;
	new_proposal.senderId = sender;
	
	
	
	g_theStringDB->GetStringID("EXPLAIN_WITHDRAW_TROOPS_0001",new_proposal.explainStrId);
	g_theStringDB->GetStringID("ADVICE_WITHDRAW_TROOPS_0001",new_proposal.adviceStrId);
	g_theStringDB->GetStringID("NEWS_WITHDRAW_TROOPS_0001",new_proposal.newsStrId);
	
	
	if (sender_diplomat.GetNewProposalTimeout( new_proposal, 20 ) )
	{
		
		return GEV_HD_Continue;
	}
	
	
	sender_diplomat.ConsiderNewProposal(receiver, new_proposal);

	return GEV_HD_Continue;
}


STDEHANDLER(ReduceWeapons_NewProposalEvent)
{
	
	if (gameEventType != GEV_FearMotivation)
		return GEV_HD_Continue;

	sint32 motivation_type;
	if (!args->GetInt(0, motivation_type))
		return GEV_HD_Continue;

	
	if ((MOTIVATION_TYPE) motivation_type != MOTIVATION_FEAR_SCIENCE_RANK )
		return GEV_HD_Continue;

	PLAYER_INDEX sender;
	PLAYER_INDEX receiver;

	if (!args->GetPlayer(0, sender))
		return GEV_HD_Continue;

	if (!args->GetPlayer(1, receiver))
		return GEV_HD_Continue;

	Diplomat & sender_diplomat = Diplomat::GetDiplomat(sender);

	
	if (sender_diplomat.TestEffectiveRegard(receiver, COLDWAR_REGARD) == false)
		return GEV_HD_Continue;

	
	Assert(g_player[sender]);
	sint32 sender_city_count = g_player[sender]->GetNumCities();
	Assert(g_player[receiver]);
	sint32 receiver_city_count = g_player[receiver]->GetNumCities();
	
	
	sint16 sender_nukes_count = MapAnalysis::GetMapAnalysis().GetNanoWeaponsCount(sender);
	sint16 sender_bio_count = MapAnalysis::GetMapAnalysis().GetBioWeaponsCount(sender);
	sint16 sender_nano_count = MapAnalysis::GetMapAnalysis().GetNuclearWeaponsCount(sender);

	
	sint16 receiver_nukes_count = MapAnalysis::GetMapAnalysis().GetNanoWeaponsCount(receiver);
	sint16 receiver_bio_count = MapAnalysis::GetMapAnalysis().GetBioWeaponsCount(receiver);
	sint16 receiver_nano_count = MapAnalysis::GetMapAnalysis().GetNuclearWeaponsCount(receiver);

	
	double sender_losses;
	double receiver_losses;
	
	PROPOSAL_TYPE proposal_type;
	if (receiver_nano_count > 0)
	{
		proposal_type = PROPOSAL_REQUEST_REDUCE_NANO_WEAPONS;
		sender_losses = ((double)receiver_nano_count / sender_city_count);
		receiver_losses = ((double)sender_nano_count / receiver_city_count);
	}
	else if (receiver_bio_count > 0)
	{
		proposal_type = PROPOSAL_REQUEST_REDUCE_BIO_WEAPONS;
		sender_losses = ((double)receiver_bio_count / sender_city_count);
		receiver_losses = ((double)sender_bio_count / receiver_city_count);
	}
	else if (receiver_nukes_count > 0)
	{
		proposal_type = PROPOSAL_REQUEST_REDUCE_NUCLEAR_WEAPONS;
		sender_losses = ((double)receiver_nukes_count / sender_city_count);
		receiver_losses = ((double)sender_nukes_count / receiver_city_count);
	}
	else
		
		return GEV_HD_Continue;

	
	double reduce_percent;
	if (receiver_losses <= 0.0)
		reduce_percent = 1.0;
	else if (sender_losses > 0.5)
		reduce_percent = (sender_losses - 0.5) / sender_losses;
	else 
		reduce_percent = (sender_losses - (receiver_losses / 2)) / sender_losses;

	if (reduce_percent <= 0.0)
		
		return GEV_HD_Continue;
	
	
	sint32 priority =
		sender_diplomat.GetNewProposalPriority(receiver, proposal_type);

	if (priority <= 0) 
		return GEV_HD_Continue;

	NewProposal new_proposal;

	reduce_percent = ProposalAnalysis::RoundPercentReduction(reduce_percent);
	if (reduce_percent <= 0)
		return GEV_HD_Continue;
	
	
	new_proposal.priority = priority;				
	
	
	new_proposal.detail.first_type = proposal_type;	
	new_proposal.detail.first_arg.percent = reduce_percent; 
	new_proposal.detail.tone = DIPLOMATIC_TONE_INDIGNANT;	

	new_proposal.receiverId = receiver;
	new_proposal.senderId = sender;
	
	
	
	g_theStringDB->GetStringID("EXPLAIN_WITHDRAW_TROOPS_0001",new_proposal.explainStrId);
	g_theStringDB->GetStringID("ADVICE_WITHDRAW_TROOPS_0001",new_proposal.adviceStrId);
	g_theStringDB->GetStringID("NEWS_WITHDRAW_TROOPS_0001",new_proposal.newsStrId);
	
	
	if (sender_diplomat.GetNewProposalTimeout( new_proposal, 20 ) )
	{
		
		return GEV_HD_Continue;
	}
	
	
	sender_diplomat.ConsiderNewProposal(receiver, new_proposal);

	return GEV_HD_Continue;
}


STDEHANDLER(RequestTribute_NewProposalEvent)
{
	PLAYER_INDEX sender;
	PLAYER_INDEX receiver;

	
	if (gameEventType != GEV_DesireMotivation)
		return GEV_HD_Continue;

	sint32 motivation_type;
	if (!args->GetInt(0, motivation_type))
		return GEV_HD_Continue;

	if (!args->GetPlayer(0, sender))
		return GEV_HD_Continue;

	if (!args->GetPlayer(1, receiver))
		return GEV_HD_Continue;

	Diplomat & sender_diplomat = Diplomat::GetDiplomat(sender);

	DIPLOMATIC_TONE tone;
	sint32 gold = 0;
	PROPOSAL_TYPE type = PROPOSAL_REQUEST_GIVE_GOLD;

	Assert(g_player[sender]);
	Assert(g_player[receiver]);
	sint32 sender_income = g_player[sender]->m_gold->GetIncome();
	sint32 receiver_income = g_player[receiver]->m_gold->GetIncome();
	sint32 sender_savings = g_player[receiver]->m_gold->GetLevel();

	DIPLOMATIC_STRENGTH sender_strength = 
		g_player[sender]->GetRelativeStrength(receiver);

	
	if ((MOTIVATION_TYPE) motivation_type == MOTIVATION_DESIRE_GOLD )
	{
		
		if (!sender_diplomat.TestEffectiveRegard(receiver, NEUTRAL_REGARD) ||
			sender_strength < DIPLOMATIC_STRENGTH_STRONG)
		{
			return GEV_HD_Continue;
		}
		
		sint32 at_risk_value_percent = 
			MapAnalysis::GetMapAnalysis().AtRiskCitiesValue(receiver,sender);
		
		
		if (at_risk_value_percent < 30)
		{
			return GEV_HD_Continue;
		}
		sint32 at_risk_value = ((double)at_risk_value_percent / 100.0) *
			MapAnalysis::GetMapAnalysis().TotalValue(receiver);
		
		type = PROPOSAL_REQUEST_GIVE_GOLD;
		tone = DIPLOMATIC_TONE_ANGRY;
		gold = MIN(at_risk_value, sender_income);
	}
	
	else if ((MOTIVATION_TYPE) motivation_type == MOTIVATION_DESIRE_ENLIST_FRIEND)
	{
		
		if (!sender_diplomat.TestPublicRegard(receiver, FRIEND_REGARD) ||
			sender_strength < DIPLOMATIC_STRENGTH_STRONG)
		{
			return GEV_HD_Continue;
		}

		
		if (sender_diplomat.GetPersonality()->GetDiscoveryMilitary() ||
			sender_diplomat.GetPersonality()->GetAlignmentEvil())
		{
			return GEV_HD_Continue;
		}

		
		if (sender_income < 3 * receiver_income)
		{
			return GEV_HD_Continue;
		}

		type = PROPOSAL_OFFER_GIVE_GOLD;
		tone = DIPLOMATIC_TONE_EQUAL;
		gold = MAX(receiver_income / 2, 100);
		gold = MIN(gold, sender_savings);
	}
	else
		
		return GEV_HD_Continue;

	
	gold = ProposalAnalysis::RoundGold(gold);

	
	
	sint32 priority =
		sender_diplomat.GetNewProposalPriority(receiver, type);

	if (priority <= 0)
		return GEV_HD_Continue;

	if (gold <= 0)
		return GEV_HD_Continue;

	NewProposal new_proposal;
	
	
	new_proposal.priority = priority;				
	
	
	new_proposal.detail.first_type = type;	
	new_proposal.detail.first_arg.gold = gold; 
	new_proposal.detail.tone = tone;	   
	
	new_proposal.receiverId = receiver;
	new_proposal.senderId = sender;
	
	
	if (sender_diplomat.GetNewProposalTimeout( new_proposal, 50 ) )
	{
		
		return GEV_HD_Continue;
	}
	
	
	sender_diplomat.ConsiderNewProposal(receiver, new_proposal);
	
	
	return GEV_HD_Continue;
}


STDEHANDLER(RequestCity_NewProposalEvent)
{
	Unit city;
	NewProposal proposal;
	PLAYER_INDEX sender;
	PLAYER_INDEX receiver;

	
	if (gameEventType != GEV_DesireMotivation)
		return GEV_HD_Continue;

	sint32 motivation_type;
	if (!args->GetInt(0, motivation_type))
		return GEV_HD_Continue;

	if (!args->GetPlayer(0, sender))
		return GEV_HD_Continue;

	if (!args->GetPlayer(1, receiver))
		return GEV_HD_Continue;

	Diplomat & sender_diplomat = Diplomat::GetDiplomat(sender);

	DIPLOMATIC_TONE tone = DIPLOMATIC_TONE_ANGRY;
	sint32 gold = 0;

	
	Diplomat & receiver_diplomat = Diplomat::GetDiplomat(receiver);

	
	
	sint32 priority =
		sender_diplomat.GetNewProposalPriority(receiver, PROPOSAL_REQUEST_GIVE_CITY);

	
	if (priority <= 0)
		return GEV_HD_Continue;		

	
	if (sender_diplomat.GetPublicRegard(receiver) >= COLDWAR_REGARD)
		return GEV_HD_Continue;

	
	if (AgreementMatrix::s_agreements.HasAgreement(sender,receiver,PROPOSAL_TREATY_DECLARE_WAR) )
		return GEV_HD_Continue;

	
	if (!sender_diplomat.DesireWarWith(receiver))
		return GEV_HD_Continue;

	sint32 at_risk_value_percent = 
		MapAnalysis::GetMapAnalysis().AtRiskCitiesValue(receiver,sender);

	sint32 at_risk_value = ((double)at_risk_value_percent / 100.0) *
		MapAnalysis::GetMapAnalysis().TotalValue(receiver);

	
	
	if (at_risk_value_percent < 75)
		return GEV_HD_Continue;		

	
	
	if ((double) MapAnalysis::GetMapAnalysis().TotalThreat(receiver) /
		MapAnalysis::GetMapAnalysis().TotalThreat(sender) > 1.25)
		return GEV_HD_Continue;

	NewProposal new_proposal;

	
	new_proposal.priority = priority;				

	
	new_proposal.detail.first_type = PROPOSAL_REQUEST_GIVE_CITY;	
	new_proposal.detail.tone = tone;	   
	
	new_proposal.receiverId = receiver;
	new_proposal.senderId = sender;

	
	if (!sender_diplomat.CanExtortCityFrom(receiver, at_risk_value, city))
		return GEV_HD_Continue;

	new_proposal.detail.first_arg.cityId = city.m_id;

	
	if (sender_diplomat.GetNewProposalTimeout( new_proposal, 20 ))
		GEV_HD_Continue;

	
	sender_diplomat.ConsiderNewProposal(receiver, new_proposal);

	return GEV_HD_Continue;
}


STDEHANDLER(RequestHonorMilitaryAgeement_NewProposalEvent)
{
	
	if (gameEventType != GEV_DesireMotivation)
		return GEV_HD_Continue;

	sint32 motivation_type;
	if (!args->GetInt(0, motivation_type))
		return GEV_HD_Continue;

	
	if ((MOTIVATION_TYPE) motivation_type != MOTIVATION_DESIRE_ENLIST_FRIEND )
		return GEV_HD_Continue;

	PLAYER_INDEX sender;
	PLAYER_INDEX receiver;

	if (!args->GetPlayer(0, sender))
		return GEV_HD_Continue;

	if (!args->GetPlayer(1, receiver))
		return GEV_HD_Continue;

	
	if (!AgreementMatrix::s_agreements.HasAgreement(sender, receiver, PROPOSAL_TREATY_MILITARY_PACT) &&
		!AgreementMatrix::s_agreements.HasAgreement(sender, receiver, PROPOSAL_TREATY_ALLIANCE))
		return GEV_HD_Continue;
	
	Diplomat & sender_diplomat = Diplomat::GetDiplomat(sender);
	ai::Agreement agreement;

	
	
	sint32 priority =
		sender_diplomat.GetNewProposalPriority(receiver, PROPOSAL_REQUEST_HONOR_MILITARY_AGREEMENT);
	
	
	if (priority <= 0)
		return GEV_HD_Continue;

	
	Assert(g_player[receiver]);
	if (g_player[receiver] == NULL)
		return GEV_HD_Continue;

	
	for (PLAYER_INDEX hot_war_enemy = 1; hot_war_enemy < CtpAi::s_maxPlayers; hot_war_enemy++)
	{
		if (g_player[hot_war_enemy] == NULL)
			continue;

		
		if (AgreementMatrix::s_agreements.TurnsAtWar(sender, hot_war_enemy) < 10)
			continue;

		
		if (AgreementMatrix::s_agreements.HasAgreement(receiver, hot_war_enemy, PROPOSAL_TREATY_DECLARE_WAR))
			continue;

		
		if (!g_player[receiver]->HasContactWith(hot_war_enemy))
			continue;
		
		break;
	}

	
	if (hot_war_enemy == CtpAi::s_maxPlayers)
		return GEV_HD_Continue;

	NewProposal new_proposal;
	
	
	new_proposal.priority = priority;				
	
	
	new_proposal.detail.first_type = PROPOSAL_REQUEST_HONOR_MILITARY_AGREEMENT;	
	new_proposal.detail.first_arg.playerId = hot_war_enemy; 
	
	new_proposal.detail.tone = DIPLOMATIC_TONE_ANGRY;	   
	
	new_proposal.receiverId = receiver;
	new_proposal.senderId = sender;
	
	
	if (sender_diplomat.GetNewProposalTimeout( new_proposal, 20 ) )
	{
		
		return GEV_HD_Continue;
	}
	
	
	sender_diplomat.ConsiderNewProposal(receiver, new_proposal);

	
	return GEV_HD_Continue;
}


STDEHANDLER(RequestEndEmbargo_NewProposalEvent)
{
	DIPLOMATIC_TONE tone;

	
	if (gameEventType == GEV_DesireMotivation)
	{
		sint32 motivation_type;
		if (!args->GetInt(0, motivation_type))
			return GEV_HD_Continue;

		
		if ((MOTIVATION_TYPE) motivation_type != MOTIVATION_DESIRE_GOLD)
			return GEV_HD_Continue;

		tone = DIPLOMATIC_TONE_EQUAL;
	}

	PLAYER_INDEX sender;
	PLAYER_INDEX receiver;

	if (!args->GetPlayer(0, sender))
		return GEV_HD_Continue;

	if (!args->GetPlayer(1, receiver))
		return GEV_HD_Continue;

	Diplomat & sender_diplomat = Diplomat::GetDiplomat(sender);
	Diplomat & receiver_diplomat = Diplomat::GetDiplomat(receiver);

	
	if (receiver_diplomat.GetEmbargo(sender) == false)
		return GEV_HD_Continue;

	
	if (gameEventType == GEV_ReactionMotivation)
	{
		if (sender_diplomat.GetEmbargo(receiver) == true)
			return GEV_HD_Continue;
		tone = DIPLOMATIC_TONE_ANGRY;
	}

	
	
	sint32 priority =
		sender_diplomat.GetNewProposalPriority(receiver, PROPOSAL_REQUEST_END_EMBARGO);
	
	
	if (priority <= 0)
		return GEV_HD_Continue;

	NewProposal new_proposal;
	
	
	new_proposal.priority = priority;				
	
	
	new_proposal.detail.first_type = PROPOSAL_REQUEST_END_EMBARGO;	
	new_proposal.detail.first_arg.playerId = receiver; 
	
	new_proposal.detail.tone = tone;	   
	
	new_proposal.receiverId = receiver;
	new_proposal.senderId = sender;
	
	
	if (sender_diplomat.GetNewProposalTimeout( new_proposal, 20 ) )
	{
		
		return GEV_HD_Continue;
	}
	
	
	sender_diplomat.ConsiderNewProposal(receiver, new_proposal);

	
	return GEV_HD_Continue;
}


void NewProposalEventCallbacks::AddCallbacks() 
{

	
	g_gevManager->AddCallback(GEV_ReactionMotivation,
							  GEV_PRI_Primary, 
							  &s_NewProposalEvent);

	

	
	
	g_gevManager->AddCallback(GEV_ReactionMotivation,
							  GEV_PRI_Pre, 
							  &s_LeaveOurBorders_NewProposalEvent);

	

	
	g_gevManager->AddCallback(GEV_FearMotivation,
							  GEV_PRI_Pre, 
							  &s_Scenario_NewProposalEvent);

	
	g_gevManager->AddCallback(GEV_FearMotivation,
							  GEV_PRI_Pre,
							  &s_StopPiracy_NewProposalEvent);

	
	g_gevManager->AddCallback(GEV_FearMotivation,
							  GEV_PRI_Pre,
							  &s_StopResearch_NewProposalEvent);

	
	g_gevManager->AddCallback(GEV_FearMotivation,
							  GEV_PRI_Pre,
							  &s_ReduceWeapons_NewProposalEvent);

	
	g_gevManager->AddCallback(GEV_FearMotivation,
							  GEV_PRI_Pre, 
							  &s_CeaseFire_NewProposalEvent);

	
	g_gevManager->AddCallback(GEV_FearMotivation,
							  GEV_PRI_Pre,
							  &s_ReducePollution_NewProposalEvent);

	
	g_gevManager->AddCallback(GEV_FearMotivation,
							  GEV_PRI_Pre,
							  &s_HonorPollutionAgreement_NewProposalEvent);


	

    
	g_gevManager->AddCallback(GEV_DesireMotivation,
							  GEV_PRI_Pre, 
							  &s_Scenario_NewProposalEvent);


	
	g_gevManager->AddCallback(GEV_DesireMotivation,
							  GEV_PRI_Pre, 
							  &s_Alliance_NewProposalEvent);

	
	g_gevManager->AddCallback(GEV_DesireMotivation,
							  GEV_PRI_Pre, 
							  &s_TradePact_NewProposalEvent);

	
	g_gevManager->AddCallback(GEV_DesireMotivation,
							  GEV_PRI_Pre, 
							  &s_ResearchPact_NewProposalEvent);

	
	g_gevManager->AddCallback(GEV_FearMotivation,
							  GEV_PRI_Pre, 
							  &s_PollutionPact_NewProposalEvent);

	
	g_gevManager->AddCallback(GEV_FearMotivation,
							  GEV_PRI_Pre, 
							  &s_RequestAdvance_NewProposalEvent);

	
	g_gevManager->AddCallback(GEV_DesireMotivation,
							  GEV_PRI_Pre, 
							  &s_ExchangeMaps_NewProposalEvent);

	
	g_gevManager->AddCallback(GEV_DesireMotivation,
							  GEV_PRI_Pre, 
							  &s_MakePeace_NewProposalEvent);

	
	g_gevManager->AddCallback(GEV_DesireMotivation,
							  GEV_PRI_Pre, 
							  &s_BegForGold_NewProposalEvent);

	
	g_gevManager->AddCallback(GEV_DesireMotivation,
							  GEV_PRI_Pre, 
							  &s_BlackmailGold_NewProposalEvent);

	
	g_gevManager->AddCallback(GEV_DesireMotivation,
							  GEV_PRI_Pre,
							  &s_RequestTribute_NewProposalEvent);

	
	g_gevManager->AddCallback(GEV_DesireMotivation,
							  GEV_PRI_Pre,
							  &s_BreakAgreementWithEnemy_NewProposalEvent);

	
	g_gevManager->AddCallback(GEV_DesireMotivation,
							  GEV_PRI_Pre,
							  &s_RequestCity_NewProposalEvent);

	
	g_gevManager->AddCallback(GEV_DesireMotivation,
							  GEV_PRI_Pre,
							  &s_RequestHonorMilitaryAgeement_NewProposalEvent);

	
	g_gevManager->AddCallback(GEV_DesireMotivation,
							  GEV_PRI_Pre,
							  &s_RequestEndEmbargo_NewProposalEvent);

}
