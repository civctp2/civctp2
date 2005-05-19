//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : Handling of diplomatic countering
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
// - Generates debug information when set.
//
//----------------------------------------------------------------------------
//
// Modifications from the original Activision code:
//
// - AI now considers more before stopping pirating
//
//----------------------------------------------------------------------------

#include "c3.h"


#include "player.h"
#include "c3math.h"

#include "CounterResponseEvent.h"

#include "Events.h"
#include "GameEventUser.h"
#include "Unit.h"
#include "StrDB.h"
#include "GameEventManager.h"

#include "aicause.h"
#include "Diplomat.h"
#include "mapanalysis.h"
#include "ProposalAnalysis.h"
#include "AgreementMatrix.h"
#include "Strengths.h"



STDEHANDLER(ThreatenAttackCity_CounterResponseEvent)
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
	const Response & response = receiver_diplomat.GetMyLastResponse(sender);

	
	if ( AgreementMatrix::s_agreements.HasAgreement(sender,
													receiver,
													PROPOSAL_TREATY_DECLARE_WAR) )
		return GEV_HD_Continue;

	
	if ( AgreementMatrix::s_agreements.HasAgreement(sender,
													receiver,
													PROPOSAL_TREATY_PEACE) )
		return GEV_HD_Continue;

	
	if ( !sender_diplomat.GetPersonality()->GetConquestAgressive() &&
		 !sender_diplomat.GetPersonality()->GetDiscoveryMilitary())
		return GEV_HD_Continue;

	
	if (!sender_diplomat.TestEffectiveRegard(receiver, HOTWAR_REGARD))
		return GEV_HD_Continue; 

	Response threat_response;

	threat_response.priority =	sender_diplomat.
		GetRejectPriority( receiver, response.counter.second_type );

	
	threat_response.type = RESPONSE_THREATEN;				
	threat_response.counter.tone = DIPLOMATIC_TONE_ANGRY;	
	
	
	threat_response.threat.type = THREAT_DESTROY_CITY;	
	threat_response.receiverId = receiver;			
	threat_response.senderId = sender;				

	
	Unit most_threatened_city;
	sint32 percent_threatened =
		MapAnalysis::GetMapAnalysis().MostAtRiskCity(receiver, most_threatened_city, sender);

	
	if (percent_threatened > 65)
		threat_response.threat.arg.cityId = most_threatened_city.m_id; 
	else 
		return GEV_HD_Continue;

	
	sender_diplomat.ConsiderResponse(receiver, threat_response);

	return GEV_HD_Continue;
}



STDEHANDLER(PayForCeasefire_CounterResponseEvent)
{
	PLAYER_INDEX sender;
	PLAYER_INDEX receiver;

	
	if (!args->GetPlayer(0, sender))
		return GEV_HD_Continue;

	
	if (!args->GetPlayer(1, receiver))
		return GEV_HD_Continue;

	Diplomat & sender_diplomat = Diplomat::GetDiplomat(sender);
	Diplomat & receiver_diplomat = Diplomat::GetDiplomat(receiver);

	const NewProposal & proposal = 
		sender_diplomat.GetMyLastNewProposal(receiver);
	const Response & response = 
		receiver_diplomat.GetMyLastResponse(sender);

	
	if ( proposal.detail.first_type != PROPOSAL_TREATY_CEASEFIRE ||
		 response.type != RESPONSE_COUNTER ||
		 response.counter.first_type != PROPOSAL_TREATY_CEASEFIRE)
		return GEV_HD_Continue;

	
	sint32 at_risk_value_percent = 
		MapAnalysis::GetMapAnalysis().AtRiskCitiesValue(sender, receiver);

	sint32 at_risk_value = (sint32) ((double)at_risk_value_percent / 100.0) *
		MapAnalysis::GetMapAnalysis().TotalValue(sender);

	DiplomacyResult sender_result;
	DiplomacyResult receiver_result;
	ProposalAnalysis::ComputeResult(response, sender_result, receiver_result);

	sint32 accept_priority = 
		sender_diplomat.GetAcceptPriority(receiver, response.counter.second_type);

	
	
	if ( response.counter.second_type == PROPOSAL_OFFER_GIVE_GOLD &&
		 receiver_result.gold < at_risk_value * 1.6 &&
		 receiver_result.gold < g_player[sender]->GetGold())
	{
		sender_diplomat.ConsiderResponse(receiver, RESPONSE_ACCEPT, accept_priority);
		return GEV_HD_Continue;
	}

	
	
	if ( response.counter.second_type == PROPOSAL_OFFER_GIVE_ADVANCE && 
		 receiver_result.science < at_risk_value * 1.2 )
	{
		sender_diplomat.ConsiderResponse(receiver, RESPONSE_ACCEPT, accept_priority);
		return GEV_HD_Continue;
	}

	
	
	if ( response.counter.second_type == PROPOSAL_OFFER_MAP ||
		 response.counter.second_type == PROPOSAL_OFFER_WITHDRAW_TROOPS ||
		 response.counter.second_type == PROPOSAL_OFFER_STOP_PIRACY ||
		 response.counter.second_type == PROPOSAL_OFFER_END_EMBARGO)
	{
		sender_diplomat.ConsiderResponse(receiver, RESPONSE_ACCEPT, accept_priority);
		return GEV_HD_Continue;
	}

	return GEV_HD_Continue;
}



STDEHANDLER(PayForGiveCity_CounterResponseEvent)
{
	PLAYER_INDEX sender;
	PLAYER_INDEX receiver;

	
	if (!args->GetPlayer(0, sender))
		return GEV_HD_Continue;

	
	if (!args->GetPlayer(1, receiver))
		return GEV_HD_Continue;

	Diplomat & sender_diplomat = Diplomat::GetDiplomat(sender);
	Diplomat & receiver_diplomat = Diplomat::GetDiplomat(receiver);

	const NewProposal & proposal = 
		sender_diplomat.GetMyLastNewProposal(receiver);
	const Response & response = 
		receiver_diplomat.GetMyLastResponse(sender);

	
	if ( proposal.detail.first_type != PROPOSAL_REQUEST_GIVE_CITY ||
		 response.type != RESPONSE_COUNTER ||
		 response.counter.first_type != PROPOSAL_REQUEST_GIVE_CITY )
		return GEV_HD_Continue;

	
	

	sint32 at_risk_value_percent = 
		MapAnalysis::GetMapAnalysis().AtRiskCitiesValue(receiver,sender);

	sint32 at_risk_value = (sint32) ((double)at_risk_value_percent / 100.0) *
		MapAnalysis::GetMapAnalysis().TotalValue(receiver);

	sint32 accept_priority = 
		sender_diplomat.GetAcceptPriority(receiver, response.counter.second_type);

	DiplomacyResult sender_result;
	DiplomacyResult receiver_result;
	ProposalAnalysis::ComputeResult(response, sender_result, receiver_result);
	Response threat_response;
	
	
	switch (response.counter.second_type)
	{
		case PROPOSAL_OFFER_WITHDRAW_TROOPS:
		case PROPOSAL_OFFER_STOP_PIRACY:
		case PROPOSAL_OFFER_GIVE_ADVANCE:
		case PROPOSAL_OFFER_GIVE_GOLD:
		case PROPOSAL_OFFER_MAP:
		case PROPOSAL_OFFER_END_EMBARGO:
			break;
		case PROPOSAL_TREATY_CEASEFIRE:
		case PROPOSAL_TREATY_PEACE:
			
			
			if (at_risk_value_percent < 85)
			{
				sender_diplomat.ConsiderResponse(receiver, RESPONSE_ACCEPT, accept_priority);		
			}
			break;
		case PROPOSAL_TREATY_TRADE_PACT:
			if ((sender_diplomat.TestEffectiveRegard(receiver, COLDWAR_REGARD) &&
				sender_diplomat.GetTrust(receiver) >= COLDWAR_REGARD) ||
				sender_diplomat.GetPersonality()->GetDiscoveryDiplomatic() ||
				sender_diplomat.GetPersonality()->GetDiscoveryEconomic())
			{
				sender_diplomat.ConsiderResponse(receiver, RESPONSE_ACCEPT, accept_priority);
			}
			break;
		case PROPOSAL_TREATY_RESEARCH_PACT:
			if ((sender_diplomat.TestEffectiveRegard(receiver, COLDWAR_REGARD) &&
				sender_diplomat.GetTrust(receiver) >= COLDWAR_REGARD) ||
				sender_diplomat.GetPersonality()->GetDiscoveryDiplomatic() ||
				sender_diplomat.GetPersonality()->GetDiscoveryScientist() ||
				sender_diplomat.GetPersonality()->GetDiscoveryEcotopian())
			{
				sender_diplomat.ConsiderResponse(receiver, RESPONSE_ACCEPT, accept_priority);
			}
			break;
		case PROPOSAL_TREATY_MILITARY_PACT:
			if ((sender_diplomat.TestEffectiveRegard(receiver, NEUTRAL_REGARD) &&
				sender_diplomat.GetTrust(receiver) >= NEUTRAL_REGARD) &&
				sender_diplomat.GetPersonality()->GetAlignmentGood() &&
				sender_diplomat.GetPersonality()->GetConquestAgressive())
			{
				sender_diplomat.ConsiderResponse(receiver, RESPONSE_ACCEPT, accept_priority);
			}
			break;
		case PROPOSAL_TREATY_POLLUTION_PACT:
			break;
		case PROPOSAL_TREATY_ALLIANCE:
			if ((sender_diplomat.TestEffectiveRegard(receiver, ALLIED_REGARD) &&
				sender_diplomat.GetTrust(receiver) >= FRIEND_REGARD) &&
				sender_diplomat.CanFormAlliance(receiver))
			{
				sender_diplomat.ConsiderResponse(receiver, RESPONSE_ACCEPT, accept_priority);
			}
		default:
			
			if (sender_diplomat.DesireWarWith(receiver))
			{
		
				
				if (sender_diplomat.ComputeThreatResponse(receiver, threat_response))
				{
					
					sender_diplomat.ConsiderResponse(receiver, threat_response);
				}
				
				return GEV_HD_Continue;
			}
	}

	
	return GEV_HD_Continue;
}




STDEHANDLER(PayForAdvance_CounterResponseEvent)
{
	PLAYER_INDEX sender;
	PLAYER_INDEX receiver;

	
	if (!args->GetPlayer(0, sender))
		return GEV_HD_Continue;

	
	if (!args->GetPlayer(1, receiver))
		return GEV_HD_Continue;

	Diplomat & diplomat = Diplomat::GetDiplomat(sender);

	const NewProposal & sender_proposal = diplomat.GetMyLastNewProposal(receiver);
	const Response & receiver_response = 
		Diplomat::GetDiplomat(receiver).GetMyLastResponse(sender);

	
	if (  receiver_response.type != RESPONSE_COUNTER ||
		  sender_proposal.detail.first_type != PROPOSAL_REQUEST_GIVE_ADVANCE ||
		  receiver_response.counter.first_type != PROPOSAL_REQUEST_GIVE_ADVANCE)
	{
		return GEV_HD_Continue;
	}

	
	if (!ProposalAnalysis::PlayerGetsEnoughValue(sender, sender, receiver, receiver_response.counter))
	{
		return GEV_HD_Continue;
	}

	
	if ( receiver_response.counter.second_type == PROPOSAL_OFFER_GIVE_CITY)
	{
		return GEV_HD_Continue;
	}
	
	sint32 accept_priority =
			diplomat.GetAcceptPriority(receiver, receiver_response.counter.second_type);
				
	
	diplomat.ConsiderResponse(receiver, RESPONSE_ACCEPT, accept_priority);
	
	
	return GEV_HD_Continue;
}



STDEHANDLER(AdvanceForGold_CounterResponseEvent)
{
	PLAYER_INDEX sender;
	PLAYER_INDEX receiver;
	
	
	if (!args->GetPlayer(0, sender))
		return GEV_HD_Continue;
	
	
	if (!args->GetPlayer(1, receiver))
		return GEV_HD_Continue;
	
	Diplomat & sender_diplomat = Diplomat::GetDiplomat(sender);
	Diplomat & receiver_diplomat = Diplomat::GetDiplomat(receiver);
	
	const NewProposal & sender_proposal = sender_diplomat.GetMyLastNewProposal(receiver);
	const Response & receiver_response = receiver_diplomat.GetMyLastResponse(sender);
	
	
	if ( receiver_response.type != RESPONSE_COUNTER ||
		receiver_response.counter.first_type != PROPOSAL_REQUEST_GIVE_GOLD)
	{
		return GEV_HD_Continue;
	}
	
	
	DiplomacyResult sender_result;
	DiplomacyResult receiver_result;
	ProposalAnalysis::ComputeResult( receiver_response,
		sender_result,
		receiver_result );
#ifdef _DEBUG
	char name[100];
	GetDescription(name,100);
	DPRINTF(k_DBG_DIPLOMACY, ("Executing %s\n",name));
	ProposalAnalysis::DebugResult(sender_proposal);
	ProposalAnalysis::DebugResult(receiver_response);
#endif _DEBUG
	
	sint32 accept_priority = 
		sender_diplomat.GetAcceptPriority(receiver, PROPOSAL_REQUEST_GIVE_GOLD);
	
	if (receiver_response.counter.second_type == PROPOSAL_OFFER_GIVE_ADVANCE)
	{
		
		sint32 stop_advance = sender_diplomat.GetStopResearchingAdvance( receiver );
		if (receiver_response.counter.second_arg.advanceType == stop_advance)
		{
			return GEV_HD_Continue;		
		}
		
		
		
		if (sender_result.gold < receiver_result.science * 1.2)
			return GEV_HD_Continue;
		
		
		sender_diplomat.ConsiderResponse(receiver, RESPONSE_ACCEPT, accept_priority);
	}
	
	
	if (receiver_response.counter.second_type == PROPOSAL_TREATY_CEASEFIRE)
	{
		if (!sender_diplomat.DesireWarWith(receiver))
		{
			
			sender_diplomat.ConsiderResponse(receiver, RESPONSE_ACCEPT, accept_priority);
		}
	}
	
	
	
	if (receiver_response.counter.second_type == PROPOSAL_TREATY_CEASEFIRE)
	{
		
		if (sender_diplomat.GetBorderIncursionBy(receiver))
			return GEV_HD_Continue;
		
		
		if (sender_diplomat.GetTrust(receiver) < COLDWAR_REGARD)
			return GEV_HD_Continue;
		
		
		if (sender_diplomat.DesireWarWith(receiver))
			return GEV_HD_Continue;
		
		
		sender_diplomat.ConsiderResponse(receiver, RESPONSE_ACCEPT, accept_priority);
	}

	return GEV_HD_Continue;
}



STDEHANDLER(ActionForValue_CounterResponseEvent)
{
	PLAYER_INDEX sender;
	PLAYER_INDEX receiver;

	
	if (!args->GetPlayer(0, sender))
		return GEV_HD_Continue;

	
	if (!args->GetPlayer(1, receiver))
		return GEV_HD_Continue;

	Diplomat & sender_diplomat = Diplomat::GetDiplomat(sender);
	Diplomat & receiver_diplomat = Diplomat::GetDiplomat(receiver);

	const NewProposal & sender_proposal = sender_diplomat.GetMyLastNewProposal(receiver);
	const Response & receiver_response = receiver_diplomat.GetMyLastResponse(sender);

	
	sint32 turns_since_last_war = AgreementMatrix::s_agreements.TurnsSinceLastWar(sender, receiver);
	if (turns_since_last_war >= 0 && turns_since_last_war < 10)
		return GEV_HD_Continue;		

	
	if (sender_diplomat.TestEffectiveRegard(receiver, HOTWAR_REGARD))
		return GEV_HD_Continue;

	
	if ( receiver_response.type != RESPONSE_COUNTER ||
		 ( receiver_response.counter.first_type != PROPOSAL_REQUEST_GIVE_GOLD &&
		   receiver_response.counter.first_type != PROPOSAL_REQUEST_GIVE_ADVANCE) )
	{
		return GEV_HD_Continue;
	}

	
	DiplomacyResult sender_result;
	DiplomacyResult receiver_result;
	ProposalAnalysis::ComputeResult( receiver_response,
									 sender_result,
									 receiver_result );
#ifdef _DEBUG
	char name[100];
	GetDescription(name,100);
	DPRINTF(k_DBG_DIPLOMACY, ("Executing %s\n",name));
	ProposalAnalysis::DebugResult(sender_proposal);
	ProposalAnalysis::DebugResult(receiver_response);
#endif _DEBUG

	const MapAnalysis & map_analysis = MapAnalysis::GetMapAnalysis();
	sint32 receiver_trade_total = map_analysis.GetTotalTrade(receiver);
	sint32 sender_trade_total = map_analysis.GetTotalTrade(sender);
	sint32 receiver_piracy = map_analysis.GetPiracyIncomeByPlayer(receiver,sender);
	sint32 sender_piracy = map_analysis.GetPiracyIncomeByPlayer(sender, receiver);
	sint32 sender_result_value = MAX(sender_result.science, sender_result.gold);

	sint32 accept_priority = 
		sender_diplomat.GetAcceptPriority(receiver, receiver_response.counter.second_type);

	
	switch (receiver_response.counter.second_type)
		{
		case PROPOSAL_OFFER_WITHDRAW_TROOPS:
			
			
			if (!sender_diplomat.GetBorderIncursionBy(receiver) &&
				!sender_diplomat.DesireWarWith(receiver))
			{
				sender_diplomat.ConsiderResponse(receiver, RESPONSE_ACCEPT, accept_priority);
			}
			break;
		case PROPOSAL_OFFER_STOP_PIRACY:
			
			// made AI consider more - tombom
			if ((receiver_piracy <= 0 &&
				(sender_result_value > sender_piracy * 4)) && 
				(!sender_diplomat.DesireWarWith(receiver)||!receiver_diplomat.DesireWarWith(sender)))
			{
				sender_diplomat.ConsiderResponse(receiver, RESPONSE_ACCEPT, accept_priority);
			}
			break;
		case PROPOSAL_OFFER_END_EMBARGO:
			
			
			
			
			if (!receiver_diplomat.GetEmbargo(sender) &&
				(sender_result_value > (receiver_trade_total - sender_trade_total) * 3))
			{
				sender_diplomat.ConsiderResponse(receiver, RESPONSE_ACCEPT, accept_priority);
			}
			break;
		default:
			
			return GEV_HD_Continue;
		}

	
	if (sender_diplomat.DesireWarWith(receiver))
	{
		Response threat_response;
		
		
		if (sender_diplomat.ComputeThreatResponse(receiver, threat_response))
		{
			
			sender_diplomat.ConsiderResponse(receiver, threat_response);
		}
		
		return GEV_HD_Continue;
	}

	
	return GEV_HD_Continue;
}



STDEHANDLER(ReciprocateAction_CounterResponseEvent)
{
	PLAYER_INDEX sender;
	PLAYER_INDEX receiver;

	
	if (!args->GetPlayer(0, sender))
		return GEV_HD_Continue;

	
	if (!args->GetPlayer(1, receiver))
		return GEV_HD_Continue;

	Diplomat & sender_diplomat = Diplomat::GetDiplomat(sender);
	Diplomat & receiver_diplomat = Diplomat::GetDiplomat(receiver);

	const NewProposal & sender_proposal = sender_diplomat.GetMyLastNewProposal(receiver);
	const Response & receiver_response = receiver_diplomat.GetMyLastResponse(sender);

	
	if ( receiver_response.type != RESPONSE_COUNTER ||
		 (receiver_response.counter.first_type != PROPOSAL_REQUEST_WITHDRAW_TROOPS) ||
		 (receiver_response.counter.first_type != PROPOSAL_REQUEST_STOP_PIRACY) ||
		 (receiver_response.counter.first_type != PROPOSAL_REQUEST_END_EMBARGO) )
	{
		return GEV_HD_Continue;
	}

	
	if ( receiver_response.type != RESPONSE_COUNTER ||
		 (receiver_response.counter.first_type != PROPOSAL_OFFER_WITHDRAW_TROOPS) ||
		 (receiver_response.counter.first_type != PROPOSAL_OFFER_STOP_PIRACY) ||
		 (receiver_response.counter.first_type != PROPOSAL_OFFER_END_EMBARGO) )
	{
		return GEV_HD_Continue;
	}

	
	if (sender_diplomat.DesireWarWith(receiver))
		{
			Response threat_response;
			
			
			if (sender_diplomat.ComputeThreatResponse(receiver, threat_response))
				{
					
					sender_diplomat.ConsiderResponse(receiver, threat_response);
				}
			
			return GEV_HD_Continue;
		}

	const MapAnalysis & map_analysis = MapAnalysis::GetMapAnalysis();
	sint32 receiver_trade_total = map_analysis.GetTotalTrade(receiver);
	sint32 sender_trade_total = map_analysis.GetTotalTrade(sender);
	sint32 receiver_piracy = map_analysis.GetPiracyIncomeByPlayer(receiver,sender);
	sint32 sender_piracy = map_analysis.GetPiracyIncomeByPlayer(sender, receiver);

	DIPLOMATIC_STRENGTH sender_strength = 
		g_player[sender]->GetRelativeStrength(receiver);

	if (receiver_response.counter.first_type == PROPOSAL_REQUEST_WITHDRAW_TROOPS)
	{
		if (receiver_response.counter.second_type == PROPOSAL_OFFER_WITHDRAW_TROOPS)
		{
			
			if ( sender_strength > DIPLOMATIC_STRENGTH_VERY_STRONG)
				return GEV_HD_Continue;
		}
	}
	else if (receiver_response.counter.first_type == PROPOSAL_REQUEST_STOP_PIRACY)
	{
		
		if (!sender_diplomat.TestEffectiveRegard(receiver, NEUTRAL_REGARD))
		{
			
			
			if (receiver_response.counter.second_type == PROPOSAL_OFFER_END_EMBARGO &&
				(receiver_diplomat.GetEmbargo(sender) ||
				((double) receiver_piracy / sender_trade_total) < 0.5))
				return GEV_HD_Continue;
			
			
			
			if (receiver_response.counter.second_type == PROPOSAL_OFFER_STOP_PIRACY &&
				sender_piracy > (receiver_piracy * 2))
				return GEV_HD_Continue;
		}
	}
	else if (receiver_response.counter.first_type == PROPOSAL_REQUEST_END_EMBARGO)
	{
		
		if (!sender_diplomat.TestEffectiveRegard(receiver, NEUTRAL_REGARD))
		{
			
			
			if (receiver_response.counter.second_type == PROPOSAL_OFFER_END_EMBARGO &&
				receiver_trade_total > (sender_trade_total * 2))
				return GEV_HD_Continue;
			
			
			if (receiver_response.counter.second_type == PROPOSAL_OFFER_STOP_PIRACY &&
				sender_piracy > (sender_trade_total * 0.25))
				return GEV_HD_Continue;
		}
	}
	else
	{
		
		return GEV_HD_Continue;
	}

	
	sint32 accept_priority = 
		sender_diplomat.GetNewProposalPriority(receiver, receiver_response.counter.second_type);
	
	Response counter_response;
	
	counter_response.priority = accept_priority;
	counter_response.type = RESPONSE_ACCEPT; 
	counter_response.counter.tone = DIPLOMATIC_TONE_EQUAL;	
	counter_response.senderId = sender;
	counter_response.receiverId = receiver;
	
	
	sender_diplomat.ConsiderResponse(receiver, counter_response);
	
	return GEV_HD_Continue;
}


STDEHANDLER(Default_CounterResponseEvent)
{
	PLAYER_INDEX sender;
	PLAYER_INDEX receiver;

	
	if (!args->GetPlayer(0, sender))
		return GEV_HD_Continue;

	
	if (!args->GetPlayer(1, receiver))
		return GEV_HD_Continue;

	Diplomat & sender_diplomat = Diplomat::GetDiplomat(sender);
	Diplomat & receiver_diplomat = Diplomat::GetDiplomat(receiver);

	const Response & receiver_response = receiver_diplomat.GetMyLastResponse(sender);

	
	if (ProposalAnalysis::IsAcceptResponse(receiver_response))
	{
		sint32 accept_priority = 
			sender_diplomat.GetNewProposalPriority(receiver, receiver_response.counter.second_type);

		sender_diplomat.ConsiderResponse(receiver, RESPONSE_ACCEPT, accept_priority);
	}

	
	sender_diplomat.ConsiderResponse(receiver, RESPONSE_REJECT, 1);			

	
	return GEV_HD_Continue;
}


void CounterResponseEventCallbacks::AddCallbacks() 
{

	
	g_gevManager->AddCallback(GEV_Counter, 
							  GEV_PRI_Pre, 
							  &s_ThreatenAttackCity_CounterResponseEvent);

	
	g_gevManager->AddCallback(GEV_Counter, 
							  GEV_PRI_Pre, 
							  &s_PayForCeasefire_CounterResponseEvent);

	
	g_gevManager->AddCallback(GEV_Counter, 
							  GEV_PRI_Pre, 
							  &s_PayForGiveCity_CounterResponseEvent);

	
	g_gevManager->AddCallback(GEV_Counter, 
							  GEV_PRI_Pre, 
							  &s_PayForAdvance_CounterResponseEvent);

	
	g_gevManager->AddCallback(GEV_Counter, 
							  GEV_PRI_Pre, 
							  &s_AdvanceForGold_CounterResponseEvent);

	
	g_gevManager->AddCallback(GEV_Counter, 
							  GEV_PRI_Pre, 
							  &s_ActionForValue_CounterResponseEvent);

	
	g_gevManager->AddCallback(GEV_Counter, 
							  GEV_PRI_Pre, 
							  &s_ReciprocateAction_CounterResponseEvent);

	
	g_gevManager->AddCallback(GEV_Counter, 
							  GEV_PRI_Pre, 
							  &s_Default_CounterResponseEvent);

}
