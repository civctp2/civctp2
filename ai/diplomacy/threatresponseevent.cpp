












#include "c3.h"

#include "ThreatResponseEvent.h"

#include "Events.h"
#include "GameEventUser.h"
#include "Unit.h"
#include "StrDB.h"
#include "GameEventManager.h"

#include "AICause.h"
#include "Diplomat.h"
#include "MapAnalysis.h"
#include "AgreementMatrix.h"
#include "ProposalAnalysis.h"
#include "UnitPool.h"


#include "player.h"


STDEHANDLER(DefaultReject_ThreatResponseEvent)
{
	PLAYER_INDEX sender;
	PLAYER_INDEX receiver;

	
	if (!args->GetPlayer(0, sender))
		return GEV_HD_Continue;

	
	if (!args->GetPlayer(1, receiver))
		return GEV_HD_Continue;

	sint32 priority = 100;

	Diplomat & sender_diplomat = Diplomat::GetDiplomat(sender);
	Diplomat & receiver_diplomat = Diplomat::GetDiplomat(receiver);

	const NewProposal & proposal = sender_diplomat.GetMyLastNewProposal(receiver);
	const Response & sender_response = sender_diplomat.GetMyLastResponse(receiver);

	if (proposal.detail.first_type == PROPOSAL_REQUEST_GIVE_CITY ||
		proposal.detail.second_type == PROPOSAL_REQUEST_GIVE_CITY)
	{
		Player *receiver_ptr = g_player[receiver];
		if (receiver_ptr && receiver_ptr->GetNumCities() <= 2)
		{
			
			priority = 500;
		}
	}

	
	receiver_diplomat.ConsiderResponse(sender, RESPONSE_REJECT, priority);

	
	return GEV_HD_Continue;
}


STDEHANDLER(DestroyCityAccept_ThreatResponseEvent)
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
	const Response & sender_response = sender_diplomat.GetMyLastResponse(receiver);

	
	Assert(sender_response.type == RESPONSE_THREATEN);
	if (sender_response.threat.type != THREAT_DESTROY_CITY)
		return GEV_HD_Continue;

	
	DiplomacyResult sender_result;
	DiplomacyResult receiver_result;
	ProposalAnalysis::ComputeResult( sender_proposal,
									 sender_result,
									 receiver_result );

	sint32 value_requested = -1 *
		(receiver_result.gold + receiver_result.production + receiver_result.science);

	
	sint32 sender_nukes = MapAnalysis::GetMapAnalysis().GetNuclearWeaponsCount(sender);
	sint32 sender_nano  = MapAnalysis::GetMapAnalysis().GetNanoWeaponsCount(sender);
	sint32 receiver_nukes = MapAnalysis::GetMapAnalysis().GetNuclearWeaponsCount(receiver);
	sint32 receiver_nano  = MapAnalysis::GetMapAnalysis().GetNanoWeaponsCount(receiver);

	Unit city(sender_response.threat.arg.cityId);
	if (!g_theUnitPool->IsValid(city.m_id))
		return GEV_HD_Continue;

	sint32 value_at_risk = MapAnalysis::GetMapAnalysis().GetAlliedValue(receiver,city.RetPos());
	sint32 max_value = MapAnalysis::GetMapAnalysis().GetMaxAlliedValue(receiver);

	if (sender_nukes > 0 || sender_nano > 0)
	{

		bool fear_nukes_from = receiver_diplomat.FearNukesFrom(sender);
		bool fear_nano_from = receiver_diplomat.FearNanoAttackFrom(sender);
		bool lost_respect = (receiver_diplomat.GetTrust(sender) < HOTWAR_REGARD / 2);
		bool reasonable_demand = (((double)value_requested / max_value) < 0.33);

		if ((fear_nukes_from || fear_nano_from) && reasonable_demand && !lost_respect)
		{
			
			receiver_diplomat.ConsiderResponse(sender, RESPONSE_ACCEPT, 200);
			return GEV_HD_Continue;
		}
	}

	double risk = MapAnalysis::GetMapAnalysis().CityAtRiskRatio(city, sender);
	if (risk > 0.5)
	{
		bool reasonable_demand = ((risk * value_at_risk) > 5 * value_requested);

		bool lost_respect = (receiver_diplomat.GetTrust(sender) < HOTWAR_REGARD / 2);

		if (reasonable_demand && !lost_respect)
		{
			
			receiver_diplomat.ConsiderResponse(sender, RESPONSE_ACCEPT, 200);
			return GEV_HD_Continue;
		}
		
	}

	
	return GEV_HD_Continue;
}


STDEHANDLER(TradeEmbargoAccept_ThreatResponseEvent)
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
	const Response & sender_response = sender_diplomat.GetMyLastResponse(receiver);

	
	Assert(sender_response.type == RESPONSE_THREATEN);
	if (sender_response.threat.type != THREAT_TRADE_EMBARGO)
		return GEV_HD_Continue;

	
	DiplomacyResult sender_result;
	DiplomacyResult receiver_result;
	ProposalAnalysis::ComputeResult( sender_proposal,
									 sender_result,
									 receiver_result );

	sint32 value_at_stake = 
		(-1 * receiver_result.gold) + 
		(-1 * receiver_result.production) + 
		(-1 * receiver_result.science);

	sint32 trade_from = receiver_diplomat.GetTradeFrom(sender);
	sint32 receiver_trade_total =  MapAnalysis::GetMapAnalysis().GetTotalTrade(receiver);
	bool major_trading_partner = (receiver_trade_total > 0 && ((double) trade_from/receiver_trade_total) > 0.3);

	
	
	if (receiver_diplomat.GetPersonality()->GetDiscoveryEconomic())
	{
		if ((value_at_stake < trade_from/2) && 
			(value_at_stake > 0) && 
			major_trading_partner)
		{
			
			receiver_diplomat.ConsiderResponse(sender, RESPONSE_ACCEPT, 200);
			return GEV_HD_Continue;
		}
	}

	return GEV_HD_Continue;
}


STDEHANDLER(DeclareWarAccept_ThreatResponseEvent)
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
	const Response & sender_response = sender_diplomat.GetMyLastResponse(receiver);

	
	Assert(sender_response.type == RESPONSE_THREATEN);
	if (sender_response.threat.type != THREAT_DECLARE_WAR)
		return GEV_HD_Continue;

	
	DiplomacyResult sender_result;
	DiplomacyResult receiver_result;
	ProposalAnalysis::ComputeResult( sender_proposal,
									 sender_result,
									 receiver_result );

	sint32 max_value = MapAnalysis::GetMapAnalysis().GetMaxAlliedValue(receiver);

	sint32 value_at_stake = 
		(-1 * receiver_result.gold) + 
		(-1 * receiver_result.production) + 
		(-1 * receiver_result.science);

	sint32 turns_at_peace = (AgreementMatrix::s_agreements.TurnsSinceLastWar(sender,receiver));
	bool peace_time = (turns_at_peace < 0 || turns_at_peace > 5);
	bool lost_respect = (receiver_diplomat.GetTrust(sender) < HOTWAR_REGARD / 2);

	if (receiver_diplomat.GetPersonality()->GetConquestPassive() && 
		!receiver_diplomat.DesireWarWith(sender))
	{
		bool reasonable_demand = (value_at_stake < max_value * 0.1);
		if (reasonable_demand && !lost_respect && peace_time)
		{
			
			receiver_diplomat.ConsiderResponse(sender, RESPONSE_ACCEPT, 200);
			return GEV_HD_Continue;
		}
	}

 	else if (!receiver_diplomat.DesireWarWith(sender))
	{
		bool reasonable_demand = (value_at_stake < max_value * 0.05);
		if (reasonable_demand && !lost_respect && peace_time)
		{
			
			receiver_diplomat.ConsiderResponse(sender, RESPONSE_ACCEPT, 200);
			return GEV_HD_Continue;
		}
	}

	return GEV_HD_Continue;
}



void ThreatResponseEventCallbacks::AddCallbacks()
{
	
	g_gevManager->AddCallback(GEV_Threaten, 
							  GEV_PRI_Pre, 
							  &s_DefaultReject_ThreatResponseEvent);

	
	g_gevManager->AddCallback(GEV_Threaten, 
							  GEV_PRI_Pre, 
							  &s_DestroyCityAccept_ThreatResponseEvent);

	
	g_gevManager->AddCallback(GEV_Threaten, 
							  GEV_PRI_Pre, 
							  &s_TradeEmbargoAccept_ThreatResponseEvent);

	
	g_gevManager->AddCallback(GEV_Threaten, 
							  GEV_PRI_Pre, 
							  &s_DeclareWarAccept_ThreatResponseEvent);


}
