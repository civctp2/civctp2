













#include "c3.h"

#include "c3math.h"

#include "ProposalAnalysis.h"

#include "ProposalResponseEvent.h"

#include "Events.h"
#include "GameEventUser.h"
#include "Unit.h"
#include "UnitData.h"
#include "StrDB.h"
#include "GameEventManager.h"

#include "AICause.h"

#include "Diplomat.h"
#include "mapanalysis.h"
#include "AgreementMatrix.h"
#include "player.h"
#include "AdvanceRecord.h"
#include "Strengths.h"
#include "pollution.h"
#include "newturncount.h"
#include "Sci.h"
#include "Gold.h"








STDEHANDLER(LandForPeace_ProposalResponseEvent)
{
	sint32 cityId;
	NewProposal proposal;
	PLAYER_INDEX sender;
	PLAYER_INDEX receiver;

	
	if (!args->GetPlayer(0, sender))
		return GEV_HD_Continue;

	
	if (!args->GetPlayer(1, receiver))
		return GEV_HD_Continue;

	
	Diplomat & sender_diplomat = Diplomat::GetDiplomat(sender);

	proposal = sender_diplomat.GetMyLastNewProposal(receiver);

	
	if (proposal.detail.first_type != PROPOSAL_REQUEST_GIVE_CITY)
		return GEV_HD_Continue;

	cityId = proposal.detail.first_arg.cityId;

	
	Diplomat & receiver_diplomat = Diplomat::GetDiplomat(receiver);
	

	sint32 accept_priority =
		receiver_diplomat.GetAcceptPriority( sender, proposal.detail.first_type);

	sint32 at_risk_value_percent = 
		MapAnalysis::GetMapAnalysis().AtRiskCitiesValue(receiver,sender);

	sint32 at_risk_value = (sint32) ((double)at_risk_value_percent / 100.0) *
		MapAnalysis::GetMapAnalysis().TotalValue(receiver);

	
	
	

	
	if (receiver_diplomat.GetTrust(sender) < COLDWAR_REGARD)
		return GEV_HD_Continue;

	
	
	if (at_risk_value_percent < 75)
		return GEV_HD_Continue;

	
	
	if ( at_risk_value < Unit(cityId)->GetCityData()->GetValue())
		return GEV_HD_Continue;

	
	
	if (((double) MapAnalysis::GetMapAnalysis().TotalThreat(receiver) /
		MapAnalysis::GetMapAnalysis().TotalThreat(sender)) > 1.25)
		return GEV_HD_Continue;
	
	
	if (AgreementMatrix::s_agreements.HasAgreement(sender, receiver, PROPOSAL_TREATY_DECLARE_WAR))
	{
		DiplomacyArg arg;
		receiver_diplomat.ConsiderCounterResponse(sender, PROPOSAL_TREATY_CEASEFIRE, 
			arg, accept_priority, DIPLOMATIC_TONE_EQUAL);
	}
	else if (!AgreementMatrix::s_agreements.HasAgreement(sender, receiver, PROPOSAL_TREATY_PEACE))
	{
		
		DiplomacyArg arg;
		receiver_diplomat.ConsiderCounterResponse(sender, PROPOSAL_TREATY_PEACE, 
			arg, accept_priority, DIPLOMATIC_TONE_EQUAL);
	}

	
	
	
	

	return GEV_HD_Continue;
}








STDEHANDLER(Accept_ProposalResponseEvent)
{
	PLAYER_INDEX sender;
	PLAYER_INDEX receiver;

	
	if (!args->GetPlayer(0, sender))
		return GEV_HD_Continue;

	
	if (!args->GetPlayer(1, receiver))
		return GEV_HD_Continue;

	Diplomat & sender_diplomat = Diplomat::GetDiplomat(sender);
	Diplomat & receiver_diplomat = Diplomat::GetDiplomat(receiver);

	const NewProposal & proposal = sender_diplomat.GetMyLastNewProposal(receiver);
	const Response & receiver_response = receiver_diplomat.GetMyLastResponse(sender);

	sint32 accept_priority =
		receiver_diplomat.GetAcceptPriority( sender, proposal.detail.first_type);

	
	if (accept_priority > 0) {
		
		Response response;

		
		response.priority = 50;    
		response.type = RESPONSE_ACCEPT;		
		response.counter.tone = DIPLOMATIC_TONE_EQUAL;	   

		
		response.senderId = sender;				
		response.receiverId = receiver;			

		
		g_theStringDB->GetStringID("EXPLAIN_GENERIC_ACCEPT",response.explainStrId);
		g_theStringDB->GetStringID("ADVICE_GENERIC_ACCEPT",response.adviceStrId);
		g_theStringDB->GetStringID("NEWS_GENERIC_ACCEPT",response.newsStrId);

		
		receiver_diplomat.ConsiderResponse(sender, response);
	}

	return GEV_HD_Continue;
}







STDEHANDLER(Reject_ProposalResponseEvent)
{
	PLAYER_INDEX sender;
	PLAYER_INDEX receiver;

	
	if (!args->GetPlayer(0, sender))
		return GEV_HD_Continue;

	
	if (!args->GetPlayer(1, receiver))
		return GEV_HD_Continue;

	Diplomat & sender_diplomat = Diplomat::GetDiplomat(sender);
	Diplomat & receiver_diplomat = Diplomat::GetDiplomat(receiver);

	const NewProposal & proposal = sender_diplomat.GetMyLastNewProposal(receiver);

	sint32 reject_priority =
		receiver_diplomat.GetRejectPriority( sender, proposal.detail.first_type);

	if (reject_priority > 0) {
		receiver_diplomat.ConsiderResponse(sender, RESPONSE_REJECT, reject_priority);
	}
	return GEV_HD_Continue;
}



STDEHANDLER(PayTribute_ProposalResponseEvent)
{
	PLAYER_INDEX sender;
	PLAYER_INDEX receiver;

	
	if (!args->GetPlayer(0, sender))
		return GEV_HD_Continue;

	
	if (!args->GetPlayer(1, receiver))
		return GEV_HD_Continue;

	Diplomat & sender_diplomat = Diplomat::GetDiplomat(sender);
	Diplomat & receiver_diplomat = Diplomat::GetDiplomat(receiver);

	const NewProposal & proposal = sender_diplomat.GetMyLastNewProposal(receiver);

	
	if (proposal.detail.first_type != PROPOSAL_REQUEST_WITHDRAW_TROOPS)
		return GEV_HD_Continue;

	sint32 accept_priority =
		receiver_diplomat.GetAcceptPriority( sender, proposal.detail.first_type);

	if (accept_priority <= 0)
		return GEV_HD_Continue;

	
	if (receiver_diplomat.GetBorderIncursionBy(sender))
		return GEV_HD_Continue;

	
	if (!receiver_diplomat.DesireWarWith(sender))
	{
		receiver_diplomat.ConsiderResponse(sender, RESPONSE_ACCEPT, accept_priority);
		return GEV_HD_Continue;
	}

	
	if ((double) MapAnalysis::GetMapAnalysis().TotalThreat(sender) /
		MapAnalysis::GetMapAnalysis().TotalThreat(receiver) < 1.25)
		return GEV_HD_Continue;

	sint32 extort_gold = 
		sint32(MIN(3 * g_player[receiver]->m_gold->GetIncome(), g_player[receiver]->GetGold()) * 0.85);

	extort_gold = ProposalAnalysis::RoundGold(extort_gold);
	if (extort_gold <= 0)
	{
		return GEV_HD_Continue;
	}

	DiplomacyArg arg;
	arg.gold = extort_gold;
	receiver_diplomat.ConsiderCounterResponse(sender, PROPOSAL_OFFER_GIVE_GOLD, 
		arg, accept_priority, DIPLOMATIC_TONE_EQUAL);

	
	
	
	

	return GEV_HD_Continue;
}


STDEHANDLER(MutualWithdraw_ProposalResponseEvent)
{
	PLAYER_INDEX sender;
	PLAYER_INDEX receiver;

	
	if (!args->GetPlayer(0, sender))
		return GEV_HD_Continue;

	
	if (!args->GetPlayer(1, receiver))
		return GEV_HD_Continue;

	Diplomat & sender_diplomat = Diplomat::GetDiplomat(sender);
	Diplomat & receiver_diplomat = Diplomat::GetDiplomat(receiver);

	const NewProposal & proposal = sender_diplomat.GetMyLastNewProposal(receiver);

	
	if (proposal.detail.first_type != PROPOSAL_REQUEST_WITHDRAW_TROOPS)
		return GEV_HD_Continue;

	sint32 accept_priority =
		receiver_diplomat.GetAcceptPriority( sender, proposal.detail.first_type);

	if (accept_priority <= 0)
		return GEV_HD_Continue;

	
	if (!receiver_diplomat.GetBorderIncursionBy(sender))
		return GEV_HD_Continue;

	
	if (receiver_diplomat.DesireWarWith(sender))
		return GEV_HD_Continue;

	DiplomacyArg arg;
	receiver_diplomat.ConsiderCounterResponse(sender, PROPOSAL_OFFER_WITHDRAW_TROOPS, 
		arg, accept_priority, DIPLOMATIC_TONE_EQUAL);

	return GEV_HD_Continue;
}



STDEHANDLER(PayForAdvance_ProposalResponseEvent)
{
	PLAYER_INDEX sender;
	PLAYER_INDEX receiver;

	
	if (!args->GetPlayer(0, sender))
		return GEV_HD_Continue;

	
	if (!args->GetPlayer(1, receiver))
		return GEV_HD_Continue;

	Diplomat & sender_diplomat = Diplomat::GetDiplomat(sender);
	Diplomat & receiver_diplomat = Diplomat::GetDiplomat(receiver);

	const NewProposal & sender_proposal = 
		sender_diplomat.GetMyLastNewProposal(receiver);

	
	if (sender_proposal.detail.first_type != PROPOSAL_REQUEST_GIVE_ADVANCE)
		return GEV_HD_Continue;

	
	DiplomacyResult proposal_sender_result;
	DiplomacyResult proposal_receiver_result;
	ProposalAnalysis::ComputeResult( sender_proposal,
									 proposal_sender_result,
									 proposal_receiver_result );
#ifdef _DEBUG
	char name[100];
	GetDescription(name,100);
	DPRINTF(k_DBG_DIPLOMACY, ("Executing %s\n",name));
	ProposalAnalysis::DebugResult(sender_proposal);
#endif // _DEBUG

	

	sint32 regard = receiver_diplomat.GetEffectiveRegard(sender);
	sint32 desired_gold = 0;

	
	if (regard >= NEUTRAL_REGARD)
		desired_gold = sint32(proposal_sender_result.science * 1.5);
	else if (regard >= FRIEND_REGARD)
		desired_gold = sint32(proposal_sender_result.science * 1.0);
	else if (regard >= ALLIED_REGARD)
		desired_gold = sint32(proposal_sender_result.science * 0.8);
	else 
		return GEV_HD_Continue;
	
	
	desired_gold = ProposalAnalysis::RoundGold(desired_gold);
	if (desired_gold <= 0)
		return GEV_HD_Continue;
	
	sint32 accept_priority =
		receiver_diplomat.GetAcceptPriority( sender, sender_proposal.detail.first_type);
	
	
	if (proposal_receiver_result.gold < desired_gold) {

		DiplomacyArg arg;
		arg.gold = desired_gold;
		receiver_diplomat.ConsiderCounterResponse(sender, PROPOSAL_OFFER_GIVE_GOLD, 
			arg, accept_priority, DIPLOMATIC_TONE_EQUAL);

		
		
		
		
	}
	
	else if (proposal_receiver_result.gold >= desired_gold) 
	{
		receiver_diplomat.ConsiderResponse(sender, RESPONSE_ACCEPT, accept_priority);
	}

	return GEV_HD_Continue;
}








STDEHANDLER(AdvanceForGold_ProposalResponseEvent)
{
	PLAYER_INDEX sender;
	PLAYER_INDEX receiver;

	
	if (!args->GetPlayer(0, sender))
		return GEV_HD_Continue;

	
	if (!args->GetPlayer(1, receiver))
		return GEV_HD_Continue;

	Diplomat & sender_diplomat = Diplomat::GetDiplomat(sender);
	Diplomat & receiver_diplomat = Diplomat::GetDiplomat(receiver);

	const NewProposal & sender_proposal = 
		sender_diplomat.GetMyLastNewProposal(receiver);

	
	if (sender_proposal.detail.first_type != PROPOSAL_REQUEST_GIVE_GOLD)
		return GEV_HD_Continue;

	
	DiplomacyResult proposal_sender_result;
	DiplomacyResult proposal_receiver_result;
	ProposalAnalysis::ComputeResult( sender_proposal,
									 proposal_sender_result,
									 proposal_receiver_result );
#ifdef _DEBUG
	char name[100];
	GetDescription(name,100);
	DPRINTF(k_DBG_DIPLOMACY, ("Executing %s\n",name));
	ProposalAnalysis::DebugResult(sender_proposal);
#endif // _DEBUG

	

	sint32 regard = receiver_diplomat.GetEffectiveRegard(sender);

	sint32 min_cost = 0;
	sint32 max_cost = 0;

	
	if (regard >= NEUTRAL_REGARD)
		max_cost  = sint32(proposal_sender_result.science * 0.8);
	else if (regard >= FRIEND_REGARD)
		max_cost = sint32(proposal_sender_result.science * 1.0);
	else if (regard >= ALLIED_REGARD)
		max_cost = sint32(proposal_sender_result.science * 3.0);
	else 
		return GEV_HD_Continue;

	if (AgreementMatrix::s_agreements.HasAgreement(sender, receiver, PROPOSAL_TREATY_RESEARCH_PACT) ||
		AgreementMatrix::s_agreements.HasAgreement(sender, receiver, PROPOSAL_TREATY_ALLIANCE) ||
		receiver_diplomat.GetPersonality()->GetDiscoveryScientist())
	{
		
		min_cost = sint32(proposal_sender_result.science * 0.5);
		max_cost = sint32(MIN(max_cost, proposal_sender_result.science * 3.0));
	}
	else
	{
		
		min_cost = sint32(proposal_sender_result.science * 0.8);
		max_cost = sint32(MIN(max_cost, proposal_sender_result.science * 2.0));
	}
	sint32 desired_advance = receiver_diplomat.GetDesiredAdvanceFrom(sender, min_cost, max_cost);
	
	
	if (desired_advance >= 0)
	{
		sint32 accept_priority = receiver_diplomat.GetAcceptPriority(sender, PROPOSAL_OFFER_GIVE_ADVANCE);
		
		DiplomacyArg arg;
		arg.advanceType = desired_advance;
		receiver_diplomat.ConsiderCounterResponse(sender, PROPOSAL_OFFER_GIVE_ADVANCE, 
			arg, accept_priority, DIPLOMATIC_TONE_EQUAL);
		
		
		
		
		
	}
		
	return GEV_HD_Continue;
}




STDEHANDLER(AdvanceExchange_ProposalResponseEvent)
{
	PLAYER_INDEX sender;
	PLAYER_INDEX receiver;

	
	if (!args->GetPlayer(0, sender))
		return GEV_HD_Continue;

	
	if (!args->GetPlayer(1, receiver))
		return GEV_HD_Continue;

	Diplomat & sender_diplomat = Diplomat::GetDiplomat(sender);
	Diplomat & receiver_diplomat = Diplomat::GetDiplomat(receiver);

	const NewProposal & sender_proposal = 
		sender_diplomat.GetMyLastNewProposal(receiver);

	
	if (sender_proposal.detail.first_type != PROPOSAL_REQUEST_GIVE_ADVANCE)
		return GEV_HD_Continue;

	
	DiplomacyResult proposal_sender_result;
	DiplomacyResult proposal_receiver_result;
	ProposalAnalysis::ComputeResult( sender_proposal,
									 proposal_sender_result,
									 proposal_receiver_result );
#ifdef _DEBUG
	char name[100];
	GetDescription(name,100);
	DPRINTF(k_DBG_DIPLOMACY, ("Executing %s\n",name));
	ProposalAnalysis::DebugResult(sender_proposal);
#endif // _DEBUG

	
	sint32 regard = receiver_diplomat.GetEffectiveRegard(sender);

	sint32 min_cost = 0;
	sint32 max_cost = 0;

	
	if (regard < NEUTRAL_REGARD)
		return GEV_HD_Continue;

	if (AgreementMatrix::s_agreements.HasAgreement(sender, receiver, PROPOSAL_TREATY_RESEARCH_PACT) ||
		AgreementMatrix::s_agreements.HasAgreement(sender, receiver, PROPOSAL_TREATY_ALLIANCE) ||
		receiver_diplomat.GetPersonality()->GetDiscoveryScientist())
	{
		
		min_cost = sint32(proposal_sender_result.science * 0.5);
		max_cost = proposal_sender_result.science * 2;
	}
	else
	{
		
		min_cost = sint32(proposal_sender_result.science * 0.8);
		max_cost = proposal_sender_result.science * 3;
	}

	

	sint32 desired_advance = receiver_diplomat.GetDesiredAdvanceFrom(sender, min_cost, max_cost);
	
	sint32 accept_priority = 
		receiver_diplomat.GetAcceptPriority(sender, PROPOSAL_OFFER_GIVE_ADVANCE);

	ai::Agreement agreement;
	if (AgreementMatrix::s_agreements.HasAgreement(sender, receiver, PROPOSAL_TREATY_RESEARCH_PACT))
		agreement =	AgreementMatrix::s_agreements.GetAgreement(sender, receiver, PROPOSAL_TREATY_RESEARCH_PACT);
	else if (AgreementMatrix::s_agreements.HasAgreement(sender, receiver, PROPOSAL_TREATY_ALLIANCE))
		agreement =	AgreementMatrix::s_agreements.GetAgreement(sender, receiver, PROPOSAL_TREATY_ALLIANCE);
	
	
	if (desired_advance >= 0)
	{
		DiplomacyArg arg;
		arg.advanceType = desired_advance;
		receiver_diplomat.ConsiderCounterResponse(sender, PROPOSAL_OFFER_GIVE_ADVANCE, 
			arg, accept_priority, DIPLOMATIC_TONE_EQUAL);
	}
	
	else if (proposal_receiver_result.science > proposal_sender_result.science)
	{
		receiver_diplomat.ConsiderResponse(sender, RESPONSE_ACCEPT, accept_priority);
	}
	
	else if (agreement != Diplomat::s_badAgreement && 
			 agreement.end == -1 &&
			 agreement.start + 20 <= NewTurnCount::GetCurrentRound())
	{
		receiver_diplomat.ConsiderResponse(sender, RESPONSE_ACCEPT, accept_priority);
	}


	return GEV_HD_Continue;
}


STDEHANDLER(StopPiracy_ProposalResponseEvent)
{
	PLAYER_INDEX sender;
	PLAYER_INDEX receiver;

	
	if (!args->GetPlayer(0, sender))
		return GEV_HD_Continue;

	
	if (!args->GetPlayer(1, receiver))
		return GEV_HD_Continue;

	Diplomat & sender_diplomat = Diplomat::GetDiplomat(sender);
	Diplomat & receiver_diplomat = Diplomat::GetDiplomat(receiver);

	const NewProposal & sender_proposal = 
		sender_diplomat.GetMyLastNewProposal(receiver);

	
	if (sender_proposal.detail.first_type != PROPOSAL_REQUEST_STOP_PIRACY)
		return GEV_HD_Continue;

	
	DiplomacyResult proposal_sender_result;
	DiplomacyResult proposal_receiver_result;
	ProposalAnalysis::ComputeResult( sender_proposal,
									 proposal_sender_result,
									 proposal_receiver_result );
#ifdef _DEBUG
	char name[100];
	GetDescription(name,100);
	DPRINTF(k_DBG_DIPLOMACY, ("Executing %s\n",name));
	ProposalAnalysis::DebugResult(sender_proposal);
#endif // _DEBUG

	

	const MapAnalysis & map = MapAnalysis::GetMapAnalysis();
	sint32 piracy_lost_to_sender = map.GetPiracyIncomeByPlayer(sender, receiver);
	sint32 piracy_income_from_sender = map.GetPiracyIncomeByPlayer(receiver, sender);
	Response response;

	
	piracy_lost_to_sender = ProposalAnalysis::RoundGold(piracy_lost_to_sender);
	piracy_income_from_sender = ProposalAnalysis::RoundGold(piracy_income_from_sender);

	if (piracy_income_from_sender <= 0)
		return GEV_HD_Continue;

	sint32 accept_priority = 
		receiver_diplomat.GetAcceptPriority(sender, PROPOSAL_REQUEST_STOP_PIRACY);

	if (accept_priority <= 0)
		return GEV_HD_Continue;		
	
	
	if ( piracy_lost_to_sender == 0 && piracy_income_from_sender > 0)
	{
		DiplomacyArg arg;
		arg.gold = piracy_income_from_sender * 3;
		receiver_diplomat.ConsiderCounterResponse(sender, PROPOSAL_OFFER_GIVE_GOLD, 
			arg, accept_priority, DIPLOMATIC_TONE_EQUAL);

		
		
		
		
	}
	
	else if ( piracy_lost_to_sender >= piracy_income_from_sender )
	{
		DiplomacyArg arg;
		receiver_diplomat.ConsiderCounterResponse(sender, PROPOSAL_OFFER_STOP_PIRACY, 
			arg, accept_priority, DIPLOMATIC_TONE_INDIGNANT);

		
		
		
		
	}

	return GEV_HD_Continue;
}


STDEHANDLER(GiveMap_ProposalResponseEvent)
{
	PLAYER_INDEX sender;
	PLAYER_INDEX receiver;

	
	if (!args->GetPlayer(0, sender))
		return GEV_HD_Continue;

	
	if (!args->GetPlayer(1, receiver))
		return GEV_HD_Continue;

	Diplomat & sender_diplomat = Diplomat::GetDiplomat(sender);
	Diplomat & receiver_diplomat = Diplomat::GetDiplomat(receiver);

	const NewProposal & sender_proposal = 
		sender_diplomat.GetMyLastNewProposal(receiver);

	
	if (sender_proposal.detail.first_type != PROPOSAL_REQUEST_MAP)
		return GEV_HD_Continue;

	sint32 accept_priority =
		receiver_diplomat.GetAcceptPriority( sender, sender_proposal.detail.first_type);

	
	if (AgreementMatrix::s_agreements.HasAgreement(sender, receiver, PROPOSAL_TREATY_ALLIANCE) ||
		AgreementMatrix::s_agreements.HasAgreement(sender, receiver, PROPOSAL_TREATY_TRADE_PACT) ||
		AgreementMatrix::s_agreements.HasAgreement(sender, receiver, PROPOSAL_TREATY_MILITARY_PACT))
	{
		receiver_diplomat.ConsiderResponse(sender, RESPONSE_ACCEPT, accept_priority);
		return GEV_HD_Continue;
	}

	
	sint32 turns_since_last_war = AgreementMatrix::s_agreements.TurnsSinceLastWar(sender, receiver);
	if (( (turns_since_last_war >= 0) && (turns_since_last_war < 5)) ||
		receiver_diplomat.TestPublicRegard(sender, COLDWAR_REGARD))
	{
		return GEV_HD_Continue;
	}

	
	sint32 receiver_saw_map =
		AgreementMatrix::s_agreements.GetAgreementDuration(receiver, sender, PROPOSAL_REQUEST_MAP);
	
	sint32 want_map_turns;
	receiver_diplomat.GetCurrentDiplomacy(sender).GetWantMapTurns(want_map_turns);

	if (receiver_saw_map != -1 &&
		receiver_saw_map < want_map_turns)
	{
		
		
		DiplomacyResult proposal_sender_result;
		DiplomacyResult proposal_receiver_result;
		ProposalAnalysis::ComputeResult( sender_proposal,
			proposal_sender_result,
			proposal_receiver_result );
		
		Player * receiver_ptr = g_player[receiver];
		sint32 income = receiver_ptr->GetGold();
		sint32 science_level = receiver_ptr->m_science->GetLevel();
		if (proposal_receiver_result.gold > 3 * income ||
			proposal_receiver_result.science > science_level * 0.10)
		{
			receiver_diplomat.ConsiderResponse(sender, RESPONSE_ACCEPT, accept_priority);
		}
		return GEV_HD_Continue;
	}

	
	DiplomacyArg arg;
	receiver_diplomat.ConsiderCounterResponse(
		sender, 
		PROPOSAL_OFFER_MAP, 
		arg, 
		accept_priority,
		DIPLOMATIC_TONE_EQUAL);

	return GEV_HD_Continue;
}

							  

STDEHANDLER(PeaceTreaty_ProposalResponseEvent)
{
	PLAYER_INDEX sender;
	PLAYER_INDEX receiver;

	
	if (!args->GetPlayer(0, sender))
		return GEV_HD_Continue;

	
	if (!args->GetPlayer(1, receiver))
		return GEV_HD_Continue;

	Diplomat & sender_diplomat = Diplomat::GetDiplomat(sender);
	Diplomat & receiver_diplomat = Diplomat::GetDiplomat(receiver);

	const NewProposal & sender_proposal = 
		sender_diplomat.GetMyLastNewProposal(receiver);

	
	if (sender_proposal.detail.first_type != PROPOSAL_TREATY_PEACE)
		return GEV_HD_Continue;

	sint32 accept_priority =
		receiver_diplomat.GetAcceptPriority( sender, sender_proposal.detail.first_type);

	sint32 reject_priority =
		receiver_diplomat.GetRejectPriority( sender, sender_proposal.detail.first_type);

	
	if (accept_priority <= 0)
		return GEV_HD_Continue;
	
	
	
	
	if ((!receiver_diplomat.GetPersonality()->GetConquestAgressive() ||
		 receiver_diplomat.GetPersonality()->GetDiscoveryDiplomatic()) &&
		 AgreementMatrix::s_agreements.TurnsSinceLastWar(sender,receiver) == -1)
	{
		
		if (receiver_diplomat.TestEffectiveRegard(sender, COLDWAR_REGARD))
			return GEV_HD_Continue;
	}

	
	else if (!receiver_diplomat.TestEffectiveRegard(sender, NEUTRAL_REGARD))
		return GEV_HD_Continue;

	
	if (receiver_diplomat.GetLastHotwarAttack(sender) <= 10)
		return GEV_HD_Continue;
	
	
	if (receiver_diplomat.GetLastColdwarAttack(sender) <= 5)
		return GEV_HD_Continue;
	
	
	if (receiver_diplomat.GetTrust(sender) < COLDWAR_REGARD)
		return GEV_HD_Continue;
	
	
	if (receiver_diplomat.DesireWarWith(sender))
		return GEV_HD_Continue;

	
	
	if (receiver_diplomat.GetBorderIncursionBy(sender) &&
		AgreementMatrix::s_agreements.HasAgreement(sender,receiver, PROPOSAL_OFFER_WITHDRAW_TROOPS))
	{
		
		DiplomacyArg arg;
		receiver_diplomat.ConsiderCounterResponse(sender, PROPOSAL_OFFER_WITHDRAW_TROOPS, arg, accept_priority, DIPLOMATIC_TONE_INDIGNANT);

		return GEV_HD_Continue;
	}
	else
	{
		
		receiver_diplomat.ConsiderResponse(sender, RESPONSE_ACCEPT, accept_priority);
	}
	
	return GEV_HD_Continue;
}


STDEHANDLER(AcceptAlliance_ProposalResponseEvent)
{
	PLAYER_INDEX sender;
	PLAYER_INDEX receiver;

	
	if (!args->GetPlayer(0, sender))
		return GEV_HD_Continue;

	
	if (!args->GetPlayer(1, receiver))
		return GEV_HD_Continue;

	Diplomat & sender_diplomat = Diplomat::GetDiplomat(sender);
	Diplomat & receiver_diplomat = Diplomat::GetDiplomat(receiver);

	const NewProposal & sender_proposal = 
		sender_diplomat.GetMyLastNewProposal(receiver);

	
	if (sender_proposal.detail.first_type != PROPOSAL_TREATY_ALLIANCE)
		return GEV_HD_Continue;

	sint32 accept_priority =
		receiver_diplomat.GetAcceptPriority( sender, sender_proposal.detail.first_type);

	if (accept_priority <= 0)
		return GEV_HD_Continue;

	
	if (!receiver_diplomat.TestEffectiveRegard(sender, ALLIED_REGARD))
		return GEV_HD_Continue;

	
	if (receiver_diplomat.GetLastHotwarAttack(sender) < 25)
		return GEV_HD_Continue;

	
	if (receiver_diplomat.GetLastColdwarAttack(sender) < 15)
		return GEV_HD_Continue;

	
	if (receiver_diplomat.GetBorderIncursionBy(sender))
		return GEV_HD_Continue;

	
	if (receiver_diplomat.GetTrust(sender) < FRIEND_REGARD)
		return GEV_HD_Continue;

	
	if (!receiver_diplomat.CanFormAlliance(sender))
		return GEV_HD_Continue;
	
	
	receiver_diplomat.ConsiderResponse(sender, RESPONSE_ACCEPT, accept_priority);

	return GEV_HD_Continue;
}




STDEHANDLER(CeasefireForGold_ProposalResponseEvent)
{
	PLAYER_INDEX sender;
	PLAYER_INDEX receiver;

	
	if (!args->GetPlayer(0, sender))
		return GEV_HD_Continue;

	
	if (!args->GetPlayer(1, receiver))
		return GEV_HD_Continue;

	Diplomat & sender_diplomat = Diplomat::GetDiplomat(sender);
	Diplomat & receiver_diplomat = Diplomat::GetDiplomat(receiver);

	const NewProposal & sender_proposal = 
		sender_diplomat.GetMyLastNewProposal(receiver);

	
	if (sender_proposal.detail.first_type != PROPOSAL_REQUEST_GIVE_GOLD)
		return GEV_HD_Continue;

	sint32 accept_priority =
		receiver_diplomat.GetAcceptPriority( sender, sender_proposal.detail.first_type);

	
	if (!AgreementMatrix::s_agreements.HasAgreement(sender, receiver, PROPOSAL_TREATY_DECLARE_WAR))
		return GEV_HD_Continue;

	
	if (receiver_diplomat.GetTrust(sender) < COLDWAR_REGARD)
		return GEV_HD_Continue;

	
	if (receiver_diplomat.DesireWarWith(sender))
		return GEV_HD_Continue;

	
	sint32 extort_gold = 
		sint32(MIN(3 * g_player[receiver]->m_gold->GetIncome(), g_player[receiver]->GetGold()) * 0.85);

	
	if (sender_proposal.detail.first_arg.gold > extort_gold)
		return GEV_HD_Continue;

	DiplomacyArg arg;
	receiver_diplomat.ConsiderCounterResponse(sender, PROPOSAL_TREATY_CEASEFIRE, 
		arg, accept_priority, DIPLOMATIC_TONE_EQUAL);

	return GEV_HD_Continue;
}


STDEHANDLER(AcceptGifts_ProposalResponseEvent)
{
	PLAYER_INDEX sender;
	PLAYER_INDEX receiver;

	
	if (!args->GetPlayer(0, sender))
		return GEV_HD_Continue;

	
	if (!args->GetPlayer(1, receiver))
		return GEV_HD_Continue;

	Diplomat & sender_diplomat = Diplomat::GetDiplomat(sender);
	Diplomat & receiver_diplomat = Diplomat::GetDiplomat(receiver);

	const NewProposal & sender_proposal = 
		sender_diplomat.GetMyLastNewProposal(receiver);

	sint32 accept_priority =
		receiver_diplomat.GetAcceptPriority( sender, sender_proposal.detail.first_type);
	
	if (!ProposalAnalysis::IsSimpleGift( sender_proposal ))
		return GEV_HD_Continue;

	
	receiver_diplomat.ConsiderResponse(sender, RESPONSE_ACCEPT, accept_priority);
	
	return GEV_HD_Continue;
}


STDEHANDLER(StopResearch_ProposalResponseEvent)
{
	PLAYER_INDEX sender;
	PLAYER_INDEX receiver;

	
	if (!args->GetPlayer(0, sender))
		return GEV_HD_Continue;

	
	if (!args->GetPlayer(1, receiver))
		return GEV_HD_Continue;

	Diplomat & sender_diplomat = Diplomat::GetDiplomat(sender);
	Diplomat & receiver_diplomat = Diplomat::GetDiplomat(receiver);

	const NewProposal & sender_proposal = 
		sender_diplomat.GetMyLastNewProposal(receiver);

	
	if (sender_proposal.detail.first_type != PROPOSAL_REQUEST_STOP_RESEARCH)
		return GEV_HD_Continue;

	Player *sender_ptr = g_player[sender];
	Player *receiver_ptr = g_player[receiver];
	Assert(sender_ptr);
	Assert(receiver_ptr);
	if (sender_ptr == NULL || receiver_ptr == NULL)
		return GEV_HD_Continue;

	sint32 accept_priority =
		receiver_diplomat.GetAcceptPriority( sender, sender_proposal.detail.first_type);

	sint32 reject_priority =
		receiver_diplomat.GetRejectPriority( sender, sender_proposal.detail.first_type);

	sint32 stop_advance_type = sender_proposal.detail.first_arg.advanceType;
	Assert(stop_advance_type  >= 0 && stop_advance_type < g_theAdvanceDB->NumRecords());

	DIPLOMATIC_STRENGTH sender_strength = 
		g_player[sender]->GetRelativeStrength(receiver);

	
	bool personality_fails =
		((receiver_diplomat.GetPersonality()->GetDiscoveryScientist() &&
		  receiver_diplomat.GetPersonality()->GetConquestAgressive()) ||
		 (receiver_diplomat.GetPersonality()->GetAlignmentEvil() &&
		  receiver_diplomat.GetPersonality()->GetTrustworthinessLawful()));

	
	bool appropriate_advance = (sender_diplomat.GetStopResearchingAdvance(receiver) == stop_advance_type );

	
	if (personality_fails || 
		(sender_strength > DIPLOMATIC_STRENGTH_AVERAGE) || 
		!appropriate_advance)
	{
		
		receiver_diplomat.ConsiderResponse(sender, RESPONSE_REJECT, reject_priority);
		return GEV_HD_Continue;
	}

	
	if (sender_strength > DIPLOMATIC_STRENGTH_AVERAGE)
	{
		bool researching_advance = (sender_ptr->m_advances->GetResearching() == stop_advance_type);
		bool has_advance = (sender_ptr->HasAdvance(stop_advance_type) == TRUE);

		if (researching_advance)
		{
			
			DiplomacyArg arg;
			arg.advanceType = stop_advance_type;
			receiver_diplomat.ConsiderCounterResponse(
				sender, 
				PROPOSAL_OFFER_STOP_RESEARCH, 
				arg, 
				accept_priority,
				DIPLOMATIC_TONE_MEEK);
		}
		else if (has_advance && !AgreementMatrix::s_agreements.HasAgreement(sender,receiver,PROPOSAL_TREATY_PEACE))
		{
			
			DiplomacyArg arg;
			receiver_diplomat.ConsiderCounterResponse(
				sender, 
				PROPOSAL_TREATY_PEACE, 
				arg, 
				accept_priority,
				DIPLOMATIC_TONE_MEEK);
		}
		else
		{
			
			receiver_diplomat.ConsiderResponse(sender, RESPONSE_ACCEPT, accept_priority);
			return GEV_HD_Continue;
		}
	}

	return GEV_HD_Continue;
}

STDEHANDLER(ReduceWeapons_ProposalResponseEvent)
{
	PLAYER_INDEX sender;
	PLAYER_INDEX receiver;

	
	if (!args->GetPlayer(0, sender))
		return GEV_HD_Continue;

	
	if (!args->GetPlayer(1, receiver))
		return GEV_HD_Continue;

	Diplomat & sender_diplomat = Diplomat::GetDiplomat(sender);
	Diplomat & receiver_diplomat = Diplomat::GetDiplomat(receiver);

	const NewProposal & sender_proposal = 
		sender_diplomat.GetMyLastNewProposal(receiver);

	
	if (sender_proposal.detail.first_type != PROPOSAL_REQUEST_REDUCE_NANO_WEAPONS &&
		sender_proposal.detail.first_type != PROPOSAL_REQUEST_REDUCE_BIO_WEAPONS &&
		sender_proposal.detail.first_type != PROPOSAL_REQUEST_REDUCE_NUCLEAR_WEAPONS)
		return GEV_HD_Continue;

	Player *sender_ptr = g_player[sender];
	Player *receiver_ptr = g_player[receiver];
	Assert(sender_ptr);
	Assert(receiver_ptr);
	if (sender_ptr == NULL || receiver_ptr == NULL)
		return GEV_HD_Continue;

	sint32 accept_priority =
		receiver_diplomat.GetAcceptPriority( sender, sender_proposal.detail.first_type);

	sint32 reject_priority =
		receiver_diplomat.GetRejectPriority( sender, sender_proposal.detail.first_type);

	
	if (receiver_diplomat.GetTrust(sender) < NEUTRAL_REGARD)
		return GEV_HD_Continue;

	
	sint32 sender_city_count = g_player[sender]->GetNumCities();
	sint32 receiver_city_count = g_player[receiver]->GetNumCities();
	
	
	sint16 sender_nukes_count = MapAnalysis::GetMapAnalysis().GetNuclearWeaponsCount(sender);
	sint16 sender_bio_count = MapAnalysis::GetMapAnalysis().GetBioWeaponsCount(sender);
	sint16 sender_nano_count = MapAnalysis::GetMapAnalysis().GetNanoWeaponsCount(sender);

	
	sint16 receiver_nukes_count = MapAnalysis::GetMapAnalysis().GetNuclearWeaponsCount(receiver);
	sint16 receiver_bio_count = MapAnalysis::GetMapAnalysis().GetBioWeaponsCount(receiver);
	sint16 receiver_nano_count = MapAnalysis::GetMapAnalysis().GetNanoWeaponsCount(receiver);

	
	double sender_losses;
	double receiver_losses;
	
	PROPOSAL_TYPE proposal_type;
	if (sender_nano_count > 0)
	{
		proposal_type = PROPOSAL_OFFER_REDUCE_NANO_WEAPONS;
		sender_losses = ((double)receiver_nano_count / sender_city_count);
		receiver_losses = ((double)sender_nano_count / receiver_city_count);
	}
	else if (sender_bio_count > 0)
	{
		proposal_type = PROPOSAL_OFFER_REDUCE_BIO_WEAPONS;
		sender_losses = ((double)receiver_bio_count / sender_city_count);
		receiver_losses = ((double)sender_bio_count / receiver_city_count);
	}
	else if (sender_nukes_count > 0)
	{
		proposal_type = PROPOSAL_OFFER_REDUCE_NUCLEAR_WEAPONS;
		sender_losses = ((double)receiver_nukes_count / sender_city_count);
		receiver_losses = ((double)sender_nukes_count / receiver_city_count);
	}
	else
	{
		
		receiver_diplomat.ConsiderResponse(sender, RESPONSE_REJECT, reject_priority);

		return GEV_HD_Continue;
	}

	
	double reduce_percent;
	if (sender_losses <= 0.0)
		reduce_percent = 1.0;
	else if (receiver_losses > 0.5)
		reduce_percent = (receiver_losses - 0.5) / receiver_losses;
	else 
		reduce_percent = (receiver_losses - (sender_losses / 2)) / receiver_losses;

	if (reduce_percent <= 0.0)
	{
		
		receiver_diplomat.ConsiderResponse(sender, RESPONSE_REJECT, reject_priority);

		return GEV_HD_Continue;
	}

	
	reduce_percent = ProposalAnalysis::RoundPercentReduction(reduce_percent);

	if (reduce_percent <= 0)
		return GEV_HD_Continue;

	
	DiplomacyArg arg;
	arg.percent = reduce_percent;
	receiver_diplomat.ConsiderCounterResponse(
		sender, 
		proposal_type, 
		arg, 
		accept_priority,
		DIPLOMATIC_TONE_EQUAL);
	
	return GEV_HD_Continue;
}


STDEHANDLER(ReducePollution_ProposalResponseEvent)
{
	PLAYER_INDEX sender;
	PLAYER_INDEX receiver;

	
	if (!args->GetPlayer(0, sender))
		return GEV_HD_Continue;

	
	if (!args->GetPlayer(1, receiver))
		return GEV_HD_Continue;

	Diplomat & sender_diplomat = Diplomat::GetDiplomat(sender);
	Diplomat & receiver_diplomat = Diplomat::GetDiplomat(receiver);

	const NewProposal & sender_proposal = 
		sender_diplomat.GetMyLastNewProposal(receiver);

	
	if (sender_proposal.detail.first_type != PROPOSAL_REQUEST_REDUCE_POLLUTION)
		return GEV_HD_Continue;

	Player *sender_ptr = g_player[sender];
	Player *receiver_ptr = g_player[receiver];
	Assert(sender_ptr);
	Assert(receiver_ptr);
	if (sender_ptr == NULL || receiver_ptr == NULL)
		return GEV_HD_Continue;

	sint32 accept_priority =
		receiver_diplomat.GetAcceptPriority( sender, sender_proposal.detail.first_type);

	sint32 reject_priority =
		receiver_diplomat.GetRejectPriority( sender, sender_proposal.detail.first_type);

	uint32 sender_pollution = g_player[sender]->GetPollutionLevel();
	uint32 receiver_pollution = g_player[receiver]->GetPollutionLevel();
	double pollution_ratio = (double) sender_pollution / receiver_pollution;

	
	if (pollution_ratio > 1.5)
	{
		receiver_diplomat.ConsiderResponse(sender, RESPONSE_REJECT, reject_priority);

		return GEV_HD_Continue;
	}

	double reduce_percent = 0.05; 
	DIPLOMATIC_TONE tone = DIPLOMATIC_TONE_EQUAL;

	
	if (receiver_diplomat.GetPersonality()->GetDiscoveryEcotopian() &&
		pollution_ratio > 1.0)
	{
		reduce_percent = (double)(sender_pollution - receiver_pollution) / sender_pollution;
		reduce_percent += 0.05;
		tone = DIPLOMATIC_TONE_ANGRY;
	}
	else if (sender_diplomat.GetPersonality()->GetDiscoveryDiplomatic())
	{
		if (!AgreementMatrix::s_agreements.HasAgreement(sender,receiver,PROPOSAL_TREATY_PEACE))
		{
			receiver_diplomat.ConsiderResponse(
				sender, 
				RESPONSE_REJECT, 
				reject_priority);
			return GEV_HD_Continue;
		}
		reduce_percent = 0.1;
		tone = DIPLOMATIC_TONE_INDIGNANT;
	}

	
	reduce_percent = ProposalAnalysis::RoundPercentReduction(reduce_percent);

	
	DiplomacyArg arg;
	arg.pollution = sint32((1.0 - reduce_percent) * sender_pollution);
	receiver_diplomat.ConsiderCounterResponse(
		sender, 
		PROPOSAL_OFFER_REDUCE_POLLUTION, 
		arg, 
		accept_priority,
		tone);

	return GEV_HD_Continue;
}

STDEHANDLER(HonorMilitaryAgreement_ProposalResponseEvent)
{
	PLAYER_INDEX sender;
	PLAYER_INDEX receiver;

	
	if (!args->GetPlayer(0, sender))
		return GEV_HD_Continue;

	
	if (!args->GetPlayer(1, receiver))
		return GEV_HD_Continue;

	Diplomat & sender_diplomat = Diplomat::GetDiplomat(sender);
	Diplomat & receiver_diplomat = Diplomat::GetDiplomat(receiver);

	const NewProposal & sender_proposal = 
		sender_diplomat.GetMyLastNewProposal(receiver);

	
	if (sender_proposal.detail.first_type != PROPOSAL_REQUEST_HONOR_MILITARY_AGREEMENT)
		return GEV_HD_Continue;

	Assert(sender_proposal.detail.first_arg.playerId != -1);
	PLAYER_INDEX foreigner = sender_proposal.detail.first_arg.playerId;

	Player *sender_ptr = g_player[sender];
	Player *receiver_ptr = g_player[receiver];
	Player *foreigner_ptr = g_player[foreigner];
	Assert(sender_ptr);
	Assert(receiver_ptr);
	if (sender_ptr == NULL || receiver_ptr == NULL)
		return GEV_HD_Continue;

	sint32 accept_priority =
		receiver_diplomat.GetAcceptPriority( sender, sender_proposal.detail.first_type);

	sint32 reject_priority =
		receiver_diplomat.GetRejectPriority( sender, sender_proposal.detail.first_type);

	
	if (foreigner == 0 ||
		AgreementMatrix::s_agreements.HasAgreement(receiver, foreigner, PROPOSAL_TREATY_DECLARE_WAR))
	{
		receiver_diplomat.ConsiderResponse(sender, RESPONSE_ACCEPT, accept_priority);
		return GEV_HD_Continue;
	}

	
	if (AgreementMatrix::s_agreements.HasAgreement(receiver, sender, PROPOSAL_TREATY_ALLIANCE) ||
		receiver_diplomat.GetPersonality()->GetTrustworthinessLawful())
	{
		receiver_diplomat.ConsiderResponse(sender, RESPONSE_ACCEPT, accept_priority);
		return GEV_HD_Continue;
	}

	DIPLOMATIC_STRENGTH sender_strength = 
		MAX(g_player[sender]->GetRelativeStrength(foreigner),
			g_player[sender]->GetRelativeStrength(receiver));

	
	if (sender_strength > DIPLOMATIC_STRENGTH_VERY_STRONG)
	{
		receiver_diplomat.ConsiderResponse(sender, RESPONSE_ACCEPT, accept_priority);
		return GEV_HD_Continue;
	}

	DIPLOMATIC_STRENGTH foreigner_strength = 
		g_player[foreigner]->GetRelativeStrength(sender);

	
	if (foreigner_strength > DIPLOMATIC_STRENGTH_VERY_STRONG)
	{
		receiver_diplomat.ConsiderResponse(sender, RESPONSE_REJECT, reject_priority);
		return GEV_HD_Continue;
	}

	
	if (!receiver_diplomat.TestEffectiveRegard(sender, FRIEND_REGARD))
	{
		receiver_diplomat.ConsiderResponse(sender, RESPONSE_REJECT, reject_priority);
		return GEV_HD_Continue;
	}

	
	if (!receiver_diplomat.TestEffectiveRegard(foreigner, FRIEND_REGARD))
	{
		receiver_diplomat.ConsiderResponse(sender, RESPONSE_ACCEPT, accept_priority);
		return GEV_HD_Continue;
	}

	
	return GEV_HD_Continue;
}

STDEHANDLER(HonorPollutionAgreement_ProposalResponseEvent)
{
	PLAYER_INDEX sender;
	PLAYER_INDEX receiver;

	
	if (!args->GetPlayer(0, sender))
		return GEV_HD_Continue;

	
	if (!args->GetPlayer(1, receiver))
		return GEV_HD_Continue;

	Diplomat & sender_diplomat = Diplomat::GetDiplomat(sender);
	Diplomat & receiver_diplomat = Diplomat::GetDiplomat(receiver);

	const NewProposal & sender_proposal = 
		sender_diplomat.GetMyLastNewProposal(receiver);

	
	if (sender_proposal.detail.first_type != PROPOSAL_REQUEST_HONOR_POLLUTION_AGREEMENT)
		return GEV_HD_Continue;

	Player *sender_ptr = g_player[sender];
	Player *receiver_ptr = g_player[receiver];
	Assert(sender_ptr);
	Assert(receiver_ptr);
	if (sender_ptr == NULL || receiver_ptr == NULL)
		return GEV_HD_Continue;

	sint32 accept_priority =
		receiver_diplomat.GetAcceptPriority( sender, sender_proposal.detail.first_type);

	sint32 reject_priority =
		receiver_diplomat.GetRejectPriority( sender, sender_proposal.detail.first_type);

	
	ai::Agreement receiver_agreement = 
		AgreementMatrix::s_agreements.GetAgreement(sender, receiver, PROPOSAL_TREATY_POLLUTION_PACT);

	ai::Agreement sender_agreement = receiver_agreement;

	if (receiver_agreement == AgreementMatrix::s_badAgreement)
	{
		receiver_agreement = AgreementMatrix::s_agreements.GetAgreement(sender, receiver, PROPOSAL_REQUEST_REDUCE_POLLUTION);
		sender_agreement = AgreementMatrix::s_agreements.GetAgreement(receiver, sender, PROPOSAL_REQUEST_REDUCE_POLLUTION);
	}

	if (sender_agreement == AgreementMatrix::s_badAgreement)
	{
		
		Assert(0);
		receiver_diplomat.ConsiderResponse(sender, RESPONSE_REJECT, reject_priority);
		return GEV_HD_Continue;
	}

	uint32 receiver_pollution = receiver_ptr->GetPollutionLevel();
	uint32 sender_pollution = sender_ptr->GetPollutionLevel();

	
	sint32 receiver_promised_pollution = receiver_agreement.proposal.first_arg.pollution;
	double receiver_pollution_ratio = (double) receiver_pollution / receiver_promised_pollution;
	sint32 requested_pollution = sender_proposal.detail.first_arg.pollution;

	
	if (requested_pollution > receiver_promised_pollution * 1.3)
	{
		receiver_diplomat.ConsiderResponse(sender, RESPONSE_REJECT, reject_priority);
		return GEV_HD_Continue;
	}

	
	if (receiver_agreement == AgreementMatrix::s_badAgreement)
	{
		sint32 sender_promised_pollution = sender_agreement.proposal.first_arg.pollution;
		double sender_pollution_ratio = (double) sender_pollution / sender_promised_pollution;
		uint32 half_sender_promised_pollution = sender_promised_pollution / 2;
		
		
		if (sender_pollution_ratio > 1.0)
		{
			
			DiplomacyArg arg;
			double percent = ((double)sender_pollution - sender_promised_pollution) / sender_pollution;
			percent = ProposalAnalysis::RoundPercentReduction(percent);

			arg.pollution = sint32((1.0 - percent) * sender_pollution);
			arg.pollution = ProposalAnalysis::RoundGold(arg.pollution);
			if (arg.pollution <= 0)
				return GEV_HD_Continue;
			
			sint32 turns_since_start = NewTurnCount::GetCurrentRound() - sender_agreement.start;

			
			if (receiver_diplomat.GetPersonality()->GetDiscoveryEcotopian() &&
				turns_since_start < 10)
			{
				receiver_diplomat.ConsiderCounterResponse(
					sender, 
					PROPOSAL_OFFER_HONOR_POLLUTION_AGREEMENT, 
					arg, 
					accept_priority,
					DIPLOMATIC_TONE_ANGRY);
			}
			
			else if (turns_since_start < 20 )
			{
				receiver_diplomat.ConsiderResponse(sender, RESPONSE_ACCEPT, accept_priority);
				return GEV_HD_Continue;
			}
			
			
			else if (turns_since_start > 20 && turns_since_start < 50 &&
				     sender_pollution <= half_sender_promised_pollution)
			{
				receiver_diplomat.ConsiderResponse(sender, RESPONSE_ACCEPT, accept_priority);
				return GEV_HD_Continue;
			}
			else
			{
				
				receiver_diplomat.ConsiderCounterResponse(
					sender, 
					PROPOSAL_OFFER_HONOR_POLLUTION_AGREEMENT, 
					arg, 
					accept_priority,
					DIPLOMATIC_TONE_INDIGNANT);
				return GEV_HD_Continue;
			}
		}
	}
	
	else if ((receiver_pollution / sender_pollution) < 1.0)
	{
		DiplomacyArg arg;
		double percent;
		DIPLOMATIC_TONE tone = DIPLOMATIC_TONE_EQUAL;
		
		if (receiver_diplomat.GetPersonality()->GetDiscoveryEcotopian())
		{
			percent = (double)(sender_pollution - receiver_pollution) / sender_pollution;
			percent -= 0.05;
			tone = DIPLOMATIC_TONE_ANGRY;
		}
		else if (sender_diplomat.GetPersonality()->GetDiscoveryDiplomatic())
		{
			percent = 0.1;
			tone = DIPLOMATIC_TONE_INDIGNANT;
		}
		
		
		percent = ProposalAnalysis::RoundPercentReduction(percent);

		arg.pollution = sint32(percent * sender_pollution);
		arg.pollution = ProposalAnalysis::RoundGold(arg.pollution);
		if (arg.pollution <= 0)
			return GEV_HD_Continue;

		receiver_diplomat.ConsiderCounterResponse(
			sender, 
			PROPOSAL_OFFER_REDUCE_POLLUTION,
			arg, accept_priority, tone);
		return GEV_HD_Continue;
	}

	
	
	if (receiver_diplomat.GetPersonality()->GetDiscoveryEcotopian() ||
		receiver_diplomat.GetPersonality()->GetTrustworthinessLawful() ||
		receiver_diplomat.GetPersonality()->GetAlignmentGood())
	{
		receiver_diplomat.ConsiderResponse(sender, RESPONSE_ACCEPT, accept_priority);
	}
	else
	{
		
		receiver_diplomat.ConsiderResponse(sender, RESPONSE_REJECT, reject_priority);
	}
	return GEV_HD_Continue;
}


STDEHANDLER(EndEmbargo_ProposalResponseEvent)
{
	PLAYER_INDEX sender;
	PLAYER_INDEX receiver;

	
	if (!args->GetPlayer(0, sender))
		return GEV_HD_Continue;

	
	if (!args->GetPlayer(1, receiver))
		return GEV_HD_Continue;

	Diplomat & sender_diplomat = Diplomat::GetDiplomat(sender);
	Diplomat & receiver_diplomat = Diplomat::GetDiplomat(receiver);

	const NewProposal & sender_proposal = 
		sender_diplomat.GetMyLastNewProposal(receiver);

	
	if (sender_proposal.detail.first_type != PROPOSAL_REQUEST_END_EMBARGO)
		return GEV_HD_Continue;

	Player *sender_ptr = g_player[sender];
	Player *receiver_ptr = g_player[receiver];
	Assert(sender_ptr);
	Assert(receiver_ptr);
	if (sender_ptr == NULL || receiver_ptr == NULL)
		return GEV_HD_Continue;

	sint32 accept_priority =
		receiver_diplomat.GetAcceptPriority( sender, sender_proposal.detail.first_type);

	sint32 reject_priority =
		receiver_diplomat.GetRejectPriority( sender, sender_proposal.detail.first_type);

	
	if (!receiver_diplomat.GetEmbargo(sender))
	{
		receiver_diplomat.ConsiderResponse(sender, RESPONSE_REJECT, reject_priority);
		return GEV_HD_Continue;
	}

	sint32 receiver_trade_total =  MapAnalysis::GetMapAnalysis().GetTotalTrade(receiver);
	sint32 sender_trade_total = MapAnalysis::GetMapAnalysis().GetTotalTrade(sender);

	
	
	if (receiver_diplomat.TestPublicRegard(sender, FRIEND_REGARD) &&
		(sender_trade_total > 1.5 * receiver_trade_total ))
	{
		DiplomacyArg arg;
		arg.gold = (sender_trade_total - receiver_trade_total) * 2;
		arg.gold = ProposalAnalysis::RoundGold(arg.gold);
		if (arg.gold <= 0)
			return GEV_HD_Continue;
		
		
		
		receiver_diplomat.ConsiderCounterResponse(
			sender, 
			PROPOSAL_OFFER_GIVE_GOLD, 
			arg, accept_priority, DIPLOMATIC_TONE_MEEK);
		
		return GEV_HD_Continue;
	}

	
	if (sender_diplomat.GetEmbargo(receiver))
	{
		DiplomacyArg arg;
		arg.gold = (sender_trade_total - receiver_trade_total) * 2;
		arg.gold = ProposalAnalysis::RoundGold(arg.gold);
		if (arg.gold <= 0)
			return GEV_HD_Continue;

		
		receiver_diplomat.ConsiderCounterResponse(
			sender, 
			PROPOSAL_OFFER_END_EMBARGO, 
			arg, accept_priority, DIPLOMATIC_TONE_MEEK);
		return GEV_HD_Continue;
	}

	
	
	receiver_diplomat.ConsiderResponse(sender, RESPONSE_ACCEPT, accept_priority);

	return GEV_HD_Continue;
}

STDEHANDLER(DefaultTreatyResponse_ProposalResponseEvent)
{
	PLAYER_INDEX sender;
	PLAYER_INDEX receiver;

	
	if (!args->GetPlayer(0, sender))
		return GEV_HD_Continue;

	
	if (!args->GetPlayer(1, receiver))
		return GEV_HD_Continue;

	Diplomat & sender_diplomat = Diplomat::GetDiplomat(sender);
	Diplomat & receiver_diplomat = Diplomat::GetDiplomat(receiver);

	const NewProposal & sender_proposal = 
		sender_diplomat.GetMyLastNewProposal(receiver);

	
	PROPOSAL_TYPE gift_type;
	DiplomacyArg gift_arg;
	PROPOSAL_TYPE desire_type;
	DiplomacyArg desire_arg;
	PROPOSAL_TYPE treaty_type;
	DiplomacyArg treaty_arg;
	bool includes_gift = false;
	bool is_treaty = false;

	
	is_treaty = ProposalAnalysis::GetTreatyFromProposal(sender_proposal.detail, 
		treaty_type, treaty_arg, includes_gift, gift_type, gift_arg);

	if (is_treaty)
	{
		
		DiplomacyResult sender_result;
		DiplomacyResult receiver_result;
		ProposalAnalysis::ComputeResult( sender_proposal,
			sender_result,
			receiver_result );

		sint32 accept_priority = 
			receiver_diplomat.GetAcceptPriority( sender, treaty_type);
		
		sint32 reject_priority =
			receiver_diplomat.GetRejectPriority( sender, treaty_type);

		
		if (receiver_result.gold < 0 ||
			receiver_result.science < 0 ||
			receiver_result.production < 0)
		{
			return GEV_HD_Continue;
		}

		
		ProposalAnalysis::AcceptTreatyCondition(receiver, sender, treaty_type, treaty_arg, desire_type, desire_arg );

		
		if (desire_type == PROPOSAL_NONE)
		{
			receiver_diplomat.ConsiderResponse(sender, RESPONSE_ACCEPT, accept_priority);
			return GEV_HD_Continue;
		}
		
		
		if (!includes_gift)
		{
			return GEV_HD_Continue;
		}

		switch (desire_type)
		{
		case PROPOSAL_MAX:
			
			receiver_diplomat.ConsiderResponse(sender, RESPONSE_REJECT, reject_priority);
			break;
		case PROPOSAL_OFFER_END_EMBARGO:
			
			if (gift_type == PROPOSAL_OFFER_END_EMBARGO ||
				gift_type == PROPOSAL_OFFER_REDUCE_NUCLEAR_WEAPONS ||
				gift_type == PROPOSAL_OFFER_REDUCE_BIO_WEAPONS ||
				gift_type == PROPOSAL_OFFER_REDUCE_NANO_WEAPONS)
			{
				receiver_diplomat.ConsiderResponse(sender, RESPONSE_ACCEPT, accept_priority);
			}
			
			else if (gift_type == PROPOSAL_OFFER_GIVE_GOLD && 
					 (receiver_result.gold > desire_arg.gold))
			{
				receiver_diplomat.ConsiderResponse(sender, RESPONSE_ACCEPT, accept_priority);
			}
			
			else if (gift_type == PROPOSAL_OFFER_GIVE_ADVANCE && 
				receiver_result.science > desire_arg.gold)
			{
				receiver_diplomat.ConsiderResponse(sender, RESPONSE_ACCEPT, accept_priority);
			}
			
			else if (gift_type == PROPOSAL_OFFER_GIVE_CITY &&
				receiver_result.science + receiver_result.gold > desire_arg.gold)
			{
				receiver_diplomat.ConsiderResponse(sender, RESPONSE_ACCEPT, accept_priority);
			}

			break;
		case PROPOSAL_OFFER_STOP_PIRACY:
			
			if (gift_type == PROPOSAL_OFFER_STOP_PIRACY ||
				gift_type == PROPOSAL_OFFER_END_EMBARGO ||
				gift_type == PROPOSAL_OFFER_WITHDRAW_TROOPS ||
				gift_type == PROPOSAL_OFFER_REDUCE_NUCLEAR_WEAPONS ||
				gift_type == PROPOSAL_OFFER_REDUCE_BIO_WEAPONS ||
				gift_type == PROPOSAL_OFFER_REDUCE_NANO_WEAPONS)
			{
				receiver_diplomat.ConsiderResponse(sender, RESPONSE_ACCEPT, accept_priority);
			}
			
			else if (gift_type == PROPOSAL_OFFER_GIVE_GOLD && 
					 (receiver_result.gold > desire_arg.gold))
			{
				receiver_diplomat.ConsiderResponse(sender, RESPONSE_ACCEPT, accept_priority);
			}
			
			else if (gift_type == PROPOSAL_OFFER_GIVE_ADVANCE && 
				receiver_result.science > desire_arg.gold)
			{
				receiver_diplomat.ConsiderResponse(sender, RESPONSE_ACCEPT, accept_priority);
			}
			
			else if (gift_type == PROPOSAL_OFFER_GIVE_CITY &&
				receiver_result.science + receiver_result.gold > desire_arg.gold)
			{
				receiver_diplomat.ConsiderResponse(sender, RESPONSE_ACCEPT, accept_priority);
			}
			break;
		case PROPOSAL_OFFER_WITHDRAW_TROOPS:
			
			if (gift_type == PROPOSAL_OFFER_WITHDRAW_TROOPS ||
				gift_type == PROPOSAL_OFFER_STOP_PIRACY ||
				gift_type == PROPOSAL_OFFER_REDUCE_NUCLEAR_WEAPONS ||
				gift_type == PROPOSAL_OFFER_REDUCE_BIO_WEAPONS ||
				gift_type == PROPOSAL_OFFER_REDUCE_NANO_WEAPONS ||
				gift_type == PROPOSAL_OFFER_MAP)
			{
				receiver_diplomat.ConsiderResponse(sender, RESPONSE_ACCEPT, accept_priority);
			}
			
			else if (gift_type == PROPOSAL_OFFER_GIVE_CITY &&
				receiver_result.science + receiver_result.gold > desire_arg.gold)
			{
				receiver_diplomat.ConsiderResponse(sender, RESPONSE_ACCEPT, accept_priority);
			}

			break;
		case PROPOSAL_OFFER_GIVE_GOLD:
			
			if (gift_type == PROPOSAL_OFFER_GIVE_GOLD && 
				(receiver_result.gold > desire_arg.gold))
			{
				receiver_diplomat.ConsiderResponse(sender, RESPONSE_ACCEPT, accept_priority);
			}
			
			else if (gift_type == PROPOSAL_OFFER_GIVE_ADVANCE && 
				receiver_result.science > desire_arg.gold)
			{
				receiver_diplomat.ConsiderResponse(sender, RESPONSE_ACCEPT, accept_priority);
			}
			
			else if (gift_type == PROPOSAL_OFFER_GIVE_CITY &&
				receiver_result.science + receiver_result.gold > desire_arg.gold)
			{
				receiver_diplomat.ConsiderResponse(sender, RESPONSE_ACCEPT, accept_priority);
			}
			break;
		case PROPOSAL_OFFER_GIVE_ADVANCE:
			
			if (gift_type == PROPOSAL_OFFER_GIVE_GOLD && 
					 (receiver_result.science > desire_arg.gold))
			{
				receiver_diplomat.ConsiderResponse(sender, RESPONSE_ACCEPT, accept_priority);
			}
			
			else if (gift_type == PROPOSAL_OFFER_GIVE_ADVANCE && 
				receiver_result.science > desire_arg.gold)
			{
				receiver_diplomat.ConsiderResponse(sender, RESPONSE_ACCEPT, accept_priority);
			}
			
			else if (gift_type == PROPOSAL_OFFER_GIVE_CITY &&
				receiver_result.science + receiver_result.gold > desire_arg.gold)
			{
				receiver_diplomat.ConsiderResponse(sender, RESPONSE_ACCEPT, accept_priority);
			}
			break;
		case PROPOSAL_OFFER_GIVE_CITY:
			
			if (gift_type == PROPOSAL_OFFER_GIVE_CITY &&
				receiver_result.science + receiver_result.gold > desire_arg.gold)
			{
				receiver_diplomat.ConsiderResponse(sender, RESPONSE_ACCEPT, accept_priority);
			}
			break;
		case PROPOSAL_OFFER_REDUCE_POLLUTION:
			
			if (gift_type == PROPOSAL_OFFER_REDUCE_POLLUTION &&
				gift_arg.pollution >= desire_arg.pollution) 
			{
				receiver_diplomat.ConsiderResponse(sender, RESPONSE_ACCEPT, accept_priority);
			}
			break;
		}
	}
	return GEV_HD_Continue;
}


void ProposalResponseEventCallbacks::AddCallbacks()
{
	
	g_gevManager->AddCallback(GEV_NewProposal, 
							  GEV_PRI_Pre, 
							  &s_LandForPeace_ProposalResponseEvent);

	
	g_gevManager->AddCallback(GEV_NewProposal, 
							  GEV_PRI_Pre, 
							  &s_Accept_ProposalResponseEvent);

	
	g_gevManager->AddCallback(GEV_NewProposal, 
							  GEV_PRI_Pre, 
							  &s_Reject_ProposalResponseEvent);

	
	g_gevManager->AddCallback(GEV_NewProposal, 
							  GEV_PRI_Pre, 
							  &s_PayTribute_ProposalResponseEvent);

	
	g_gevManager->AddCallback(GEV_NewProposal, 
							  GEV_PRI_Pre, 
							  &s_PayForAdvance_ProposalResponseEvent);

	g_gevManager->AddCallback(GEV_NewProposal, 
							  GEV_PRI_Pre, 
							  &s_AdvanceForGold_ProposalResponseEvent);

	g_gevManager->AddCallback(GEV_NewProposal,
							  GEV_PRI_Pre, 
							  &s_AdvanceExchange_ProposalResponseEvent);

	g_gevManager->AddCallback(GEV_NewProposal,
							  GEV_PRI_Pre, 
							  &s_StopPiracy_ProposalResponseEvent);

	g_gevManager->AddCallback(GEV_NewProposal, 
							  GEV_PRI_Pre, 
							  &s_GiveMap_ProposalResponseEvent);

	g_gevManager->AddCallback(GEV_NewProposal, 
							  GEV_PRI_Pre, 
							  &s_PeaceTreaty_ProposalResponseEvent);

	g_gevManager->AddCallback(GEV_NewProposal, 
							  GEV_PRI_Pre, 
							  &s_AcceptAlliance_ProposalResponseEvent);

	g_gevManager->AddCallback(GEV_NewProposal, 
							  GEV_PRI_Pre, 
							  &s_CeasefireForGold_ProposalResponseEvent);

	g_gevManager->AddCallback(GEV_NewProposal,
							  GEV_PRI_Pre, 
							  &s_AcceptGifts_ProposalResponseEvent);

	g_gevManager->AddCallback(GEV_NewProposal,
							  GEV_PRI_Pre, 
							  &s_StopResearch_ProposalResponseEvent);

	g_gevManager->AddCallback(GEV_NewProposal,
							  GEV_PRI_Pre, 
							  &s_ReduceWeapons_ProposalResponseEvent);

	g_gevManager->AddCallback(GEV_NewProposal,
							  GEV_PRI_Pre, 
							  &s_ReducePollution_ProposalResponseEvent);

	g_gevManager->AddCallback(GEV_NewProposal,
							  GEV_PRI_Pre, 
							  &s_HonorMilitaryAgreement_ProposalResponseEvent);

	g_gevManager->AddCallback(GEV_NewProposal,
							  GEV_PRI_Pre,
							  &s_HonorPollutionAgreement_ProposalResponseEvent);

	g_gevManager->AddCallback(GEV_NewProposal,
							  GEV_PRI_Pre,
							  &s_EndEmbargo_ProposalResponseEvent);

	g_gevManager->AddCallback(GEV_NewProposal,
							  GEV_PRI_Pre,
							  &s_DefaultTreatyResponse_ProposalResponseEvent);

	g_gevManager->AddCallback(GEV_NewProposal,
							  GEV_PRI_Pre,
							  &s_MutualWithdraw_ProposalResponseEvent);


}
