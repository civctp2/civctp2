













#include "c3.h"

#include "RejectResponseEvent.h"

#include "Events.h"
#include "GameEventUser.h"
#include "Unit.h"
#include "StrDB.h"
#include "GameEventManager.h"
#include "CtpAi.h"
#include "AgreementMatrix.h"

#include "AICause.h"

#include "Diplomat.h"
#include "MapAnalysis.h" 
#include "Player.h"








STDEHANDLER(NoThreat_RejectResponseEvent)
{
	PLAYER_INDEX sender;
	PLAYER_INDEX receiver;

	
	if (!args->GetPlayer(0, sender))
		return GEV_HD_Continue;

	
	if (!args->GetPlayer(1, receiver))
		return GEV_HD_Continue;

	Diplomat & sender_diplomat = Diplomat::GetDiplomat(sender);
	Diplomat & receiver_diplomat = Diplomat::GetDiplomat(receiver);

	if (1) {
		Response response;

		
		response.priority = 50;						
		response.type = RESPONSE_REJECT;			
		response.counter.tone = DIPLOMATIC_TONE_EQUAL;	   

		
		response.receiverId = receiver;			
		response.senderId = sender;				

		
		g_theStringDB->GetStringID("EXPLAIN_NO_THREAT",response.explainStrId);
		g_theStringDB->GetStringID("ADVICE_NO_THREAT",response.adviceStrId);
		g_theStringDB->GetStringID("NEWS_NO_THREAT",response.newsStrId);

		
		sender_diplomat.ConsiderResponse(receiver, response);
	}
	
	return GEV_HD_Continue;
}




STDEHANDLER(ThreatenAlliedInvasion_RejectResponseEvent)
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

	PLAYER_INDEX foreignerId;
	PLAYER_INDEX allyId = -1;
	for (foreignerId = 0; foreignerId < CtpAi::s_maxPlayers; foreignerId++) 
	{
		if (AgreementMatrix::s_agreements.HasAgreement(sender, foreignerId,
				PROPOSAL_TREATY_ALLIANCE) )
				allyId = foreignerId;
	}

	
	
	if (allyId != -1) {
		Response response;

		
		response.priority = 100;					
		response.type = RESPONSE_THREATEN;			
		response.counter.tone = DIPLOMATIC_TONE_INDIGNANT;	   

		
		response.receiverId = receiver;			
		response.senderId = sender;				

		response.threat.type = THREAT_DECLARE_WAR;

		
		g_theStringDB->GetStringID("EXPLAIN_NO_THREAT",response.explainStrId);
		g_theStringDB->GetStringID("ADVICE_NO_THREAT",response.adviceStrId);
		g_theStringDB->GetStringID("NEWS_NO_THREAT",response.newsStrId);

		
		sender_diplomat.ConsiderResponse(receiver, response);
	}
	
	return GEV_HD_Continue;
}









STDEHANDLER(ThreatenAttackCity_RejectResponseEvent)
{
	PLAYER_INDEX sender;
	PLAYER_INDEX receiver;

	
	if (!args->GetPlayer(0, sender))
		return GEV_HD_Continue;

	
	if (!args->GetPlayer(1, receiver))
		return GEV_HD_Continue;

	Diplomat & diplomat = Diplomat::GetDiplomat(sender);

	const NewProposal & proposal = diplomat.GetMyLastNewProposal(receiver);
	const Response & response = 
		Diplomat::GetDiplomat(receiver).GetMyLastResponse(sender);

	
	
	if ( proposal.detail.first_type == PROPOSAL_REQUEST_GIVE_CITY &&
		 response.type == RESPONSE_REJECT)
	{
		
		
		
		if (
			
			(AgreementMatrix::s_agreements.HasAgreement(
				sender,
				receiver,
				PROPOSAL_TREATY_DECLARE_WAR)) &&
			
			
			(diplomat.GetTrust(receiver) < NEUTRAL_REGARD) &&

			
			
			(MapAnalysis::GetMapAnalysis().TotalThreat(sender) /
			MapAnalysis::GetMapAnalysis().TotalThreat(receiver) >= 1.30)) {
			
			Response threat_response;
			
			
			threat_response.priority = 75;							
			threat_response.type = RESPONSE_THREATEN;				
			threat_response.counter.tone = DIPLOMATIC_TONE_ANGRY;	
			
			
			threat_response.threat.type = THREAT_DESTROY_CITY;	
			threat_response.receiverId = receiver;			
			threat_response.senderId = sender;				
			if(g_player[receiver] && g_player[receiver]->m_all_cities->Num() > 0) {
				threat_response.threat.arg.cityId = g_player[receiver]->m_all_cities->Access(0).m_id;  
			} else {
				threat_response.threat.arg.cityId = 0;
			}
			
			
			g_theStringDB->GetStringID("EXPLAIN_THREATEN_ATTACK_CITY",threat_response.explainStrId);
			g_theStringDB->GetStringID("ADVICE_THREATEN_ATTACK_CITY",threat_response.adviceStrId);
			g_theStringDB->GetStringID("NEWS_THREATEN_ATTACK_CITY",threat_response.newsStrId);
			
			
			diplomat.ConsiderResponse(receiver, threat_response);
		}
	}

	
	if ( proposal.detail.first_type == PROPOSAL_REQUEST_WITHDRAW_TROOPS &&
		 response.type == RESPONSE_REJECT )
	{
		if (diplomat.GetPersonality()->GetConquestAgressive() &&
			diplomat.GetPersonality()->GetTrustworthinessChaotic())
		{
			Response response;
			
			
			response.priority = 75;					
			response.type = RESPONSE_THREATEN;		
			response.counter.tone = DIPLOMATIC_TONE_ANGRY;	
			
			
			response.threat.type = THREAT_DESTROY_CITY;	
			response.receiverId = receiver;				
			response.senderId = sender;					
			if(g_player[receiver] && g_player[receiver]->m_all_cities->Num() > 0) {
				response.threat.arg.cityId = g_player[receiver]->m_all_cities->Access(0).m_id;  
			} else {
				response.threat.arg.cityId = 0;
			}
			
			
			g_theStringDB->GetStringID("EXPLAIN_THREATEN_ATTACK_CITY2",response.explainStrId);
			g_theStringDB->GetStringID("ADVICE_THREATEN_ATTACK_CITY2",response.adviceStrId);
			g_theStringDB->GetStringID("NEWS_THREATEN_ATTACK_CITY2",response.newsStrId);
			
			
			diplomat.ConsiderResponse(receiver, response);
		}
	}

	
	return GEV_HD_Continue;
}


void RejectResponseEventCallbacks::AddCallbacks()
{
	
	g_gevManager->AddCallback(GEV_Reject, 
							  GEV_PRI_Pre, 
							  &s_NoThreat_RejectResponseEvent);

	
	g_gevManager->AddCallback(GEV_Reject, 
							  GEV_PRI_Pre, 
							  &s_ThreatenAlliedInvasion_RejectResponseEvent);

	
	g_gevManager->AddCallback(GEV_Reject, 
							  GEV_PRI_Pre, 
							  &s_ThreatenAttackCity_RejectResponseEvent);

}

