











#include "c3.h"
#include "Unit.h"
#include "SlicFuncAI.h"
#include "AgreementMatrix.h"
#include "Army.h"
#include "Scheduler.h"
#include "DiplomatTypes.h"

#include "slicif.h"
#include "StrDB.h"
extern StringDB *g_theStringDB;




#include "Diplomat.h"








bool ParseProposalDataSlicArgs(SlicArgList *args, sint32 &argNum, ProposalData &data) {

	sint32 first_type;
	sint32 second_type;

	if(!args->GetInt(argNum++, first_type))
		return false;


	Assert(first_type >= PROPOSAL_NONE);
	Assert(first_type < PROPOSAL_MAX);
	data.first_type = static_cast<PROPOSAL_TYPE>(first_type);

	
	switch (data.first_type) {
	case PROPOSAL_NONE:
		break;
	









	}

	if(!args->GetInt(argNum++, second_type))
		return false;
	Assert(second_type >= PROPOSAL_NONE);
	Assert(second_type < PROPOSAL_MAX);
	data.second_type = static_cast<PROPOSAL_TYPE>(second_type);

	switch (data.second_type) {
	case PROPOSAL_NONE:
		break;
	









	}

	return true;
}


bool ParseResponseSlicArgs(SlicArgList *args, sint32 &argNum, Response &response) {

	sint32 type;
	sint32 threat_type;
	sint32 priority;
	sint32 value;
	if(!args->GetPlayer(argNum++, response.senderId))
		return false;
	Assert(response.senderId > 0);
	Assert(response.senderId < k_MAX_PLAYERS);

	if(!args->GetPlayer(argNum++, response.receiverId))
		return false;
	Assert(response.receiverId > 0);
	Assert(response.receiverId < k_MAX_PLAYERS);

	if(!args->GetInt(argNum++, priority))
		return false;
	Assert( priority >= 0);
	response.priority = static_cast<sint16>(priority);

	if(!args->GetInt(argNum++, type))
		return false;
	Assert(type > RESPONSE_INVALID);
	Assert(type < RESPONSE_MAX);
	response.type = static_cast<RESPONSE_TYPE>(type);

	
	if ( type == RESPONSE_COUNTER ) {
		
		if (!ParseProposalDataSlicArgs(args, argNum, response.counter))
			return false;
	} 
	
	else if ( type ==  RESPONSE_THREATEN ) {
		
		if(!args->GetInt(argNum++, threat_type))
			return false;
		Assert(threat_type > THREAT_NONE);
		Assert(threat_type < THREAT_MAX);
		response.threat.type = static_cast<THREAT_TYPE>(threat_type);
		switch(response.threat.type) {
			
		case THREAT_TRADE_EMBARGO:
		case THREAT_DECLARE_WAR:
			break;
		
		case THREAT_DESTROY_CITY:
			if(!args->GetInt(argNum++, value))
				return false;			
			response.threat.arg.cityId = value;
			break;
		}
	}

	
	if(!args->GetStringId(argNum++, response.explainStrId))
		return false;

	
	if(!args->GetStringId(argNum++, response.adviceStrId))
		return false;

	
	if(!args->GetStringId(argNum++, response.newsStrId))
		return false;

	return true;
}


bool ParseNewProposalSlicArgs(SlicArgList *args, sint32 &argNum, NewProposal &new_proposal) {
	sint32 priority;
	if(!args->GetPlayer(argNum++, new_proposal.senderId))
		return false;
	Assert(new_proposal.senderId > 0);
	Assert(new_proposal.senderId < k_MAX_PLAYERS);

	if(!args->GetPlayer(argNum++, new_proposal.receiverId))
		return false;
	Assert(new_proposal.receiverId > 0);
	Assert(new_proposal.receiverId < k_MAX_PLAYERS);

	if(!args->GetInt(argNum++, priority))
		return false;
	Assert( priority >= 0);
	new_proposal.priority = static_cast<sint16>(priority);

	if (!ParseProposalDataSlicArgs(args, argNum, new_proposal.detail))
		return false;
	
	
	if(!args->GetStringId(argNum++, new_proposal.explainStrId))
		return false;

	
	if(!args->GetStringId(argNum++, new_proposal.adviceStrId))
		return false;

	
	if(!args->GetStringId(argNum++, new_proposal.newsStrId))
		return false;

	return true;
}






SFN_ERROR Slic_LogRegardEvent::Call(SlicArgList *args)
{
	if(args->m_numArgs < 5)
		return SFN_ERROR_NUM_ARGS;

	sint32 argNum = 0;
	PLAYER_INDEX player;
	PLAYER_INDEX foreigner;
	sint32 delta;
	sint32 type;
	sint32 duration;
	StringId explain;
	if(!args->GetPlayer(argNum++, player))
		return SFN_ERROR_TYPE_ARGS;
	Assert(player > 0);
	Assert(player < k_MAX_PLAYERS);
	
	if(!args->GetPlayer(argNum++, foreigner))
		return SFN_ERROR_TYPE_ARGS;
	Assert(foreigner > 0);
	Assert(foreigner < k_MAX_PLAYERS);

	if(!args->GetInt(argNum++, delta))
		return SFN_ERROR_TYPE_ARGS;

	if(!args->GetInt(argNum++, type))
		return SFN_ERROR_TYPE_ARGS;

	if(!args->GetStringId(argNum++, explain))
		return SFN_ERROR_TYPE_ARGS;

	
	if(!args->GetInt(argNum++, duration))
		duration = -1;

	Diplomat::GetDiplomat(player).LogRegardEvent( foreigner, 
		delta, 
		static_cast<REGARD_EVENT_TYPE>(type), 
		explain,
		duration);

	return SFN_ERROR_OK;
}


SFN_ERROR Slic_GetPublicRegard::Call(SlicArgList *args)
{
	if(args->m_numArgs < 3)
		return SFN_ERROR_NUM_ARGS;

	sint32 argNum = 0;
	PLAYER_INDEX player;
	PLAYER_INDEX foreigner;
	sint32 type;

	if(!args->GetPlayer(argNum++, player))
		return SFN_ERROR_TYPE_ARGS;
	Assert(player > 0);
	Assert(player < k_MAX_PLAYERS);
	
	if(!args->GetPlayer(argNum++, foreigner))
		return SFN_ERROR_TYPE_ARGS;
	Assert(foreigner > 0);
	Assert(foreigner < k_MAX_PLAYERS);

	if(!args->GetInt(argNum++, type))
		return SFN_ERROR_TYPE_ARGS;

	m_result.m_int = 
		Diplomat::GetDiplomat(player).
			GetPublicRegard( foreigner, static_cast<REGARD_EVENT_TYPE>(type));
	
	return SFN_ERROR_OK;
}


SFN_ERROR Slic_GetEffectiveRegard::Call(SlicArgList *args)
{
	if(args->m_numArgs < 2)
		return SFN_ERROR_NUM_ARGS;

	sint32 argNum = 0;
	PLAYER_INDEX player;
	PLAYER_INDEX foreigner;

	if(!args->GetPlayer(argNum++, player))
		return SFN_ERROR_TYPE_ARGS;
	Assert(player > 0);
	Assert(player < k_MAX_PLAYERS);
	
	if(!args->GetPlayer(argNum++, foreigner))
		return SFN_ERROR_TYPE_ARGS;
	Assert(foreigner > 0);
	Assert(foreigner < k_MAX_PLAYERS);

	m_result.m_int = 
		Diplomat::GetDiplomat(player).GetEffectiveRegard( foreigner );
	
	return SFN_ERROR_OK;
}


SFN_ERROR Slic_GetTrust::Call(SlicArgList *args)
{
	if(args->m_numArgs < 2)
		return SFN_ERROR_NUM_ARGS;

	sint32 argNum = 0;
	PLAYER_INDEX player;
	PLAYER_INDEX foreigner;

	if(!args->GetPlayer(argNum++, player))
		return SFN_ERROR_TYPE_ARGS;
	Assert(player > 0);
	Assert(player < k_MAX_PLAYERS);
	
	if(!args->GetPlayer(argNum++, foreigner))
		return SFN_ERROR_TYPE_ARGS;
	Assert(foreigner > 0);
	Assert(foreigner < k_MAX_PLAYERS);

	m_result.m_int = 
		Diplomat::GetDiplomat(player).GetTrust( foreigner );
	
	return SFN_ERROR_OK;
}


SFN_ERROR Slic_SetTrust::Call(SlicArgList *args)
{
	if(args->m_numArgs < 3)
		return SFN_ERROR_NUM_ARGS;

	sint32 argNum = 0;
	PLAYER_INDEX player;
	PLAYER_INDEX foreigner;
	sint32 trustDelta;

	if(!args->GetPlayer(argNum++, player))
		return SFN_ERROR_TYPE_ARGS;
	Assert(player > 0);
	Assert(player < k_MAX_PLAYERS);
	
	if(!args->GetPlayer(argNum++, foreigner))
		return SFN_ERROR_TYPE_ARGS;
	Assert(foreigner > 0);
	Assert(foreigner < k_MAX_PLAYERS);

	if(!args->GetInt(argNum++, trustDelta))
		return SFN_ERROR_TYPE_ARGS;

	Diplomat::GetDiplomat(player).SetTrust( foreigner, static_cast<sint16>(trustDelta) );

	return SFN_ERROR_OK;
}


SFN_ERROR Slic_RecomputeRegard::Call(SlicArgList *args)
{
	if(args->m_numArgs < 1)
		return SFN_ERROR_NUM_ARGS;

	sint32 argNum = 0;
	PLAYER_INDEX player;

	if(!args->GetPlayer(argNum++, player))
		return SFN_ERROR_TYPE_ARGS;
	Assert(player > 0);
	Assert(player < k_MAX_PLAYERS);
	
	Diplomat::GetDiplomat(player).RecomputeRegard();

	return SFN_ERROR_OK;
}






SFN_ERROR Slic_HasAgreementWithAnyone::Call(SlicArgList *args)
{
	if(args->m_numArgs < 2)
		return SFN_ERROR_NUM_ARGS;

	sint32 argNum = 0;
	PLAYER_INDEX player;
	sint32 agreement_type;

	if(!args->GetPlayer(argNum++, player))
		return SFN_ERROR_TYPE_ARGS;
	Assert(player > 0);
	Assert(player < k_MAX_PLAYERS);
	
	if(!args->GetPlayer(argNum++, agreement_type))
		return SFN_ERROR_TYPE_ARGS;
	Assert(agreement_type > 0);
	Assert(agreement_type < PROPOSAL_MAX);

	m_result.m_int = 
		AgreementMatrix::s_agreements.HasAgreement( player, (PROPOSAL_TYPE) agreement_type);
	
	return SFN_ERROR_OK;
}


SFN_ERROR Slic_HasAgreement::Call(SlicArgList *args)
{
	if(args->m_numArgs < 3)
		return SFN_ERROR_NUM_ARGS;

	sint32 argNum = 0;
	PLAYER_INDEX player;
	PLAYER_INDEX foreigner;
	sint32 agreement_type;

	if(!args->GetPlayer(argNum++, player))
		return SFN_ERROR_TYPE_ARGS;
	Assert(player > 0);
	Assert(player < k_MAX_PLAYERS);
	
	if(!args->GetPlayer(argNum++, foreigner))
		return SFN_ERROR_TYPE_ARGS;
	Assert(foreigner > 0);
	Assert(foreigner < k_MAX_PLAYERS);

	if(!args->GetPlayer(argNum++, agreement_type))
		return SFN_ERROR_TYPE_ARGS;
	Assert(agreement_type > 0);
	Assert(foreigner < PROPOSAL_MAX);

	m_result.m_int = AgreementMatrix::s_agreements.HasAgreement( player, foreigner, 
		(PROPOSAL_TYPE) agreement_type);
	
	return SFN_ERROR_OK;
}




SFN_ERROR Slic_CancelAgreement::Call(SlicArgList *args)
{
	if(args->m_numArgs < 3)
		return SFN_ERROR_NUM_ARGS;

	sint32 argNum = 0;
	PLAYER_INDEX player;
	PLAYER_INDEX foreigner;
	sint32 agreement_type;

	if(!args->GetPlayer(argNum++, player))
		return SFN_ERROR_TYPE_ARGS;
	Assert(player > 0);
	Assert(player < k_MAX_PLAYERS);
	
	if(!args->GetPlayer(argNum++, foreigner))
		return SFN_ERROR_TYPE_ARGS;
	Assert(foreigner > 0);
	Assert(foreigner < k_MAX_PLAYERS);

	if(!args->GetPlayer(argNum++, agreement_type))
		return SFN_ERROR_TYPE_ARGS;
	Assert(agreement_type > 0);
	Assert(foreigner < PROPOSAL_MAX);

	AgreementMatrix::s_agreements.CancelAgreement( player, foreigner, (PROPOSAL_TYPE) agreement_type);
	
	return SFN_ERROR_OK;
}


SFN_ERROR Slic_TurnsSinceLastWar::Call(SlicArgList *args)
{
	if(args->m_numArgs < 2)
		return SFN_ERROR_NUM_ARGS;

	sint32 argNum = 0;
	PLAYER_INDEX player;
	PLAYER_INDEX foreigner;

	if(!args->GetPlayer(argNum++, player))
		return SFN_ERROR_TYPE_ARGS;
	Assert(player > 0);
	Assert(player < k_MAX_PLAYERS);
	
	if(!args->GetPlayer(argNum++, foreigner))
		return SFN_ERROR_TYPE_ARGS;
	Assert(foreigner > 0);
	Assert(foreigner < k_MAX_PLAYERS);

	m_result.m_int = 
		AgreementMatrix::s_agreements.TurnsSinceLastWar( player, foreigner);
	
	return SFN_ERROR_OK;
}


SFN_ERROR Slic_TurnsAtWar::Call(SlicArgList *args)
{
	if(args->m_numArgs < 2)
		return SFN_ERROR_NUM_ARGS;

	sint32 argNum = 0;
	PLAYER_INDEX player;
	PLAYER_INDEX foreigner;

	if(!args->GetPlayer(argNum++, player))
		return SFN_ERROR_TYPE_ARGS;
	Assert(player > 0);
	Assert(player < k_MAX_PLAYERS);
	
	if(!args->GetPlayer(argNum++, foreigner))
		return SFN_ERROR_TYPE_ARGS;
	Assert(foreigner > 0);
	Assert(foreigner < k_MAX_PLAYERS);

	m_result.m_int = 
		AgreementMatrix::s_agreements.TurnsAtWar( player, foreigner);
	
	return SFN_ERROR_OK;
}




SFN_ERROR Slic_GetLastHotwarAttack::Call(SlicArgList *args)
{
	if(args->m_numArgs < 2)
		return SFN_ERROR_NUM_ARGS;

	sint32 argNum = 0;
	PLAYER_INDEX player;
	PLAYER_INDEX foreigner;

	if(!args->GetPlayer(argNum++, player))
		return SFN_ERROR_TYPE_ARGS;
	Assert(player > 0);
	Assert(player < k_MAX_PLAYERS);
	
	if(!args->GetPlayer(argNum++, foreigner))
		return SFN_ERROR_TYPE_ARGS;
	Assert(foreigner > 0);
	Assert(foreigner < k_MAX_PLAYERS);

	m_result.m_int = 
		Diplomat::GetDiplomat(player).GetLastHotwarAttack(foreigner);
	
	return SFN_ERROR_OK;
}




SFN_ERROR Slic_GetLastColdwarAttack::Call(SlicArgList *args)
{
	if(args->m_numArgs < 2)
		return SFN_ERROR_NUM_ARGS;

	sint32 argNum = 0;
	PLAYER_INDEX player;
	PLAYER_INDEX foreigner;

	if(!args->GetPlayer(argNum++, player))
		return SFN_ERROR_TYPE_ARGS;
	Assert(player > 0);
	Assert(player < k_MAX_PLAYERS);
	
	if(!args->GetPlayer(argNum++, foreigner))
		return SFN_ERROR_TYPE_ARGS;
	Assert(foreigner > 0);
	Assert(foreigner < k_MAX_PLAYERS);

	m_result.m_int = 
		Diplomat::GetDiplomat(player).GetLastColdwarAttack(foreigner);
	
	return SFN_ERROR_OK;
}










SFN_ERROR Slic_ConsiderResponse::Call(SlicArgList *args)
{
	if(args->m_numArgs < 7)
		return SFN_ERROR_NUM_ARGS;

	sint32 argNum = 0;
	Response response;

	if (!ParseResponseSlicArgs(args, argNum, response))
		SFN_ERROR_TYPE_ARGS;

	
	if (Diplomat::GetDiplomat(response.senderId).GetReceiverHasInitiative(response.receiverId))
		Diplomat::GetDiplomat(response.receiverId).
			ConsiderResponse(response.senderId, response);
	else
		Diplomat::GetDiplomat(response.senderId).
			ConsiderResponse(response.receiverId, response);

	return SFN_ERROR_OK;
}


SFN_ERROR Slic_SetResponse::Call(SlicArgList *args)
{
	if(args->m_numArgs < 7)
		return SFN_ERROR_NUM_ARGS;

	sint32 argNum = 0;
	Response response;

	if (!ParseResponseSlicArgs(args, argNum, response))
		SFN_ERROR_TYPE_ARGS;

	
	if (Diplomat::GetDiplomat(response.senderId).GetReceiverHasInitiative(response.receiverId))
		Diplomat::GetDiplomat(response.receiverId).
			SetMyLastResponse(response.senderId, response);
	else
		Diplomat::GetDiplomat(response.senderId).
			SetMyLastResponse(response.receiverId, response);

	return SFN_ERROR_OK;
}






SFN_ERROR Slic_ConsiderMotivation::Call(SlicArgList *args)
{
	if(args->m_numArgs < 3)
		return SFN_ERROR_NUM_ARGS;

	sint32 argNum = 0;
	PLAYER_INDEX player;
	sint32 priority;
	sint32 type;
	sint32 value;
	Unit unit;
	Motivation motivation;

	if(!args->GetPlayer(argNum++, player))
		return SFN_ERROR_TYPE_ARGS;
	Assert(player > 0);
	Assert(player < k_MAX_PLAYERS);
	
	if(!args->GetInt(argNum++, priority))
		return SFN_ERROR_TYPE_ARGS;
	Assert( priority >= 0);
	motivation.priority = static_cast<sint16>(priority);

	if(!args->GetInt(argNum++, type))
		return SFN_ERROR_TYPE_ARGS;
	Assert(type > MOTIVATION_NONE);
	Assert(type < MOTIVATION_MAX);
	motivation.type = static_cast<MOTIVATION_TYPE>(type);

	switch (motivation.type) {
	
	case MOTIVATION_FEAR_SCENARIO:
	case MOTIVATION_FEAR_SCIENCE_RANK:
	case MOTIVATION_FEAR_MILITARY_RANK:
	case MOTIVATION_FEAR_TRADE_RANK:
	case MOTIVATION_DESIRE_SCENARIO:
	case MOTIVATION_DESIRE_TRADE:
	case MOTIVATION_DESIRE_GROWTH:
	case MOTIVATION_DESIRE_GOLD:
		break;
	
	case MOTIVATION_FEAR_INVASION:
	case MOTIVATION_FEAR_PIRACY:
	case MOTIVATION_FEAR_SPECIAL_ATTACKS:
	case MOTIVATION_FEAR_SPYING:
	case MOTIVATION_DESIRE_ATTACK:
	case MOTIVATION_DESIRE_INTIMIDATE:
	case MOTIVATION_DESIRE_MAKE_FRIEND:
		if(!args->GetPlayer(argNum++, value))
			return SFN_ERROR_TYPE_ARGS;
		Assert(player > 0);
		Assert(player < k_MAX_PLAYERS);
		motivation.arg.playerId = player;
		break;
	
	case MOTIVATION_FEAR_CITY_DEFENSE:
	case MOTIVATION_DESIRE_ATTACK_CITY:
	if(!args->GetCity(argNum++, unit))
		return SFN_ERROR_TYPE_ARGS;
		motivation.arg.cityId = unit.m_id;
		break;
	
	case MOTIVATION_FEAR_ADVANCE:
	case MOTIVATION_DESIRE_ADVANCE:
		if(!args->GetInt(argNum++, value))
			return SFN_ERROR_TYPE_ARGS;
		motivation.arg.advanceType = value;
		break;
	}

	Diplomat::GetDiplomat(player).ConsiderMotivation( motivation );

	return SFN_ERROR_OK;
}






SFN_ERROR Slic_ConsiderNewProposal::Call(SlicArgList *args)
{
	if(args->m_numArgs < 7)
		return SFN_ERROR_NUM_ARGS;

	sint32 argNum = 0;
	NewProposal new_proposal;

	if (!ParseNewProposalSlicArgs(args, argNum, new_proposal))
		return SFN_ERROR_TYPE_ARGS;

	Diplomat::GetDiplomat(new_proposal.senderId).
		ConsiderNewProposal(new_proposal.receiverId, new_proposal);

	return SFN_ERROR_OK;
}


SFN_ERROR Slic_SetNewProposal::Call(SlicArgList *args)
{
	if(args->m_numArgs < 7)
		return SFN_ERROR_NUM_ARGS;

	sint32 argNum = 0;
	NewProposal new_proposal;

	if (!ParseNewProposalSlicArgs(args, argNum, new_proposal))
		return SFN_ERROR_TYPE_ARGS;

	Diplomat::GetDiplomat(new_proposal.senderId).
		SetMyLastNewProposal(new_proposal.receiverId, new_proposal);

	return SFN_ERROR_OK;
}






SFN_ERROR Slic_ConsiderStrategicState::Call(SlicArgList *args)
{
	if(args->m_numArgs < 3)
		return SFN_ERROR_NUM_ARGS;

	sint32 player;
	sint32 priority;
	sint32 strategy_index;
	sint32 argNum = 0;

	if(!args->GetPlayer(argNum++, player))
		return SFN_ERROR_TYPE_ARGS;

	if(!args->GetInt(argNum++, priority))
		return SFN_ERROR_TYPE_ARGS;

	if(!args->GetInt(argNum++, strategy_index))
		return SFN_ERROR_TYPE_ARGS;

	AiState state;

	
	sint32 spystr_id;
	sint32 advicestr_id;
	sint32 newsstr_id;

	if (args->m_numArgs > argNum && args->GetInt(argNum++, spystr_id))
		state.spyStrId = spystr_id;
	if (args->m_numArgs > argNum && args->GetInt(argNum++, advicestr_id))
		state.spyStrId = advicestr_id;
	if (args->m_numArgs > argNum && args->GetInt(argNum++, newsstr_id))
		state.spyStrId = newsstr_id;

	Diplomat & diplomat = Diplomat::GetDiplomat(player);

	
	state.priority = priority;		   
	state.dbIndex = strategy_index;
	
	
	diplomat.ConsiderStrategicState(state);
	

	return SFN_ERROR_OK;
}


SFN_ERROR Slic_ComputeCurrentStrategy::Call(SlicArgList *args)
{
	if(args->m_numArgs < 1)
		return SFN_ERROR_NUM_ARGS;

	sint32 player;
	sint32 argNum = 0;

	if(!args->GetPlayer(argNum++, player))
		return SFN_ERROR_TYPE_ARGS;

	Diplomat & diplomat = Diplomat::GetDiplomat(player);

	
	diplomat.ComputeCurrentStrategy();

	return SFN_ERROR_OK;
}






SFN_ERROR Slic_ConsiderDiplomaticState::Call(SlicArgList *args)
{
	if(args->m_numArgs < 4)
		return SFN_ERROR_NUM_ARGS;

	sint32 player;
	sint32 foreigner;
	sint32 priority;
	sint32 diplomacy_index;
	sint32 argNum = 0;

	if(!args->GetPlayer(argNum++, player))
		return SFN_ERROR_TYPE_ARGS;

	if(!args->GetPlayer(argNum++, foreigner))
		return SFN_ERROR_TYPE_ARGS;

	if(!args->GetInt(argNum++, priority))
		return SFN_ERROR_TYPE_ARGS;

	if(!args->GetInt(argNum++, diplomacy_index))
		return SFN_ERROR_TYPE_ARGS;

	AiState state;

	
	sint32 spystr_id;
	sint32 advicestr_id;
	sint32 newsstr_id;
	if (args->m_numArgs > argNum && args->GetInt(argNum++, spystr_id))
		state.spyStrId = spystr_id;
	if (args->m_numArgs > argNum && args->GetInt(argNum++, advicestr_id))
		state.spyStrId = advicestr_id;
	if (args->m_numArgs > argNum && args->GetInt(argNum++, newsstr_id))
		state.spyStrId = newsstr_id;

	Diplomat & diplomat = Diplomat::GetDiplomat(player);

	
	state.priority = priority;		   
	state.dbIndex = diplomacy_index;
	
	
	diplomat.ConsiderDiplomaticState(foreigner, state);

	return SFN_ERROR_OK;
}


SFN_ERROR Slic_ChangeDiplomaticState::Call(SlicArgList *args)
{
	if(args->m_numArgs < 2)
		return SFN_ERROR_NUM_ARGS;

	sint32 player;
	sint32 foreigner;
	sint32 argNum = 0;

	if(!args->GetPlayer(argNum++, player))
		return SFN_ERROR_TYPE_ARGS;

	if(!args->GetPlayer(argNum++, foreigner))
		return SFN_ERROR_TYPE_ARGS;

	Diplomat & diplomat = Diplomat::GetDiplomat(player);

	
	sint32 diplomacy_index = diplomat.GetCurrentDiplomacy(foreigner).GetIndex();
	diplomat.ChangeDiplomacy(foreigner, diplomacy_index);

	return SFN_ERROR_OK;
}






SFN_ERROR Slic_GetTradeFrom::Call(SlicArgList *args)
{
	
	return SFN_ERROR_OK;
}


SFN_ERROR Slic_GetTributeFrom::Call(SlicArgList *args)
{
	
	return SFN_ERROR_OK;
}


SFN_ERROR Slic_GetGoldSurplusPercent::Call(SlicArgList *args)
{
	
	return SFN_ERROR_OK;
}


SFN_ERROR Slic_CanBuySurplus::Call(SlicArgList *args)
{
	
	return SFN_ERROR_OK;
}


SFN_ERROR Slic_GetAdvanceLevelPercent::Call(SlicArgList *args)
{
	
	return SFN_ERROR_OK;
}


SFN_ERROR Slic_AtWarCount::Call(SlicArgList *args)
{
	if(args->m_numArgs < 1)
		return SFN_ERROR_NUM_ARGS;

	sint32 argNum = 0;
	PLAYER_INDEX player;

	if(!args->GetPlayer(argNum++, player))
		return SFN_ERROR_TYPE_ARGS;
	Assert(player > 0);
	Assert(player < k_MAX_PLAYERS);
	m_result.m_int = Diplomat::GetDiplomat(player).AtWarCount();
	
	return SFN_ERROR_OK;
}


SFN_ERROR Slic_EffectiveAtWarCount::Call(SlicArgList *args)
{
	if(args->m_numArgs < 1)
		return SFN_ERROR_NUM_ARGS;

	sint32 argNum = 0;
	PLAYER_INDEX player;

	if(!args->GetPlayer(argNum++, player))
		return SFN_ERROR_TYPE_ARGS;
	Assert(player > 0);
	Assert(player < k_MAX_PLAYERS);
	m_result.m_int = Diplomat::GetDiplomat(player).EffectiveAtWarCount();
	
	return SFN_ERROR_OK;
}


SFN_ERROR Slic_AtWarWith::Call(SlicArgList *args)
{
	if(args->m_numArgs < 2)
		return SFN_ERROR_NUM_ARGS;

	sint32 argNum = 0;
	PLAYER_INDEX player;
	PLAYER_INDEX foreigner;

	if(!args->GetPlayer(argNum++, player))
		return SFN_ERROR_TYPE_ARGS;
	Assert(player > 0);
	Assert(player < k_MAX_PLAYERS);

	if(!args->GetPlayer(argNum++, foreigner))
		return SFN_ERROR_TYPE_ARGS;
	Assert(player > 0);
	Assert(player < k_MAX_PLAYERS);

	m_result.m_int = AgreementMatrix::s_agreements.HasAgreement(player,foreigner,PROPOSAL_TREATY_DECLARE_WAR);

	return SFN_ERROR_OK;
}


SFN_ERROR Slic_EffectiveWarWith::Call(SlicArgList *args)
{
	if(args->m_numArgs < 2)
		return SFN_ERROR_NUM_ARGS;

	sint32 argNum = 0;
	PLAYER_INDEX player;
	PLAYER_INDEX foreigner;

	if(!args->GetPlayer(argNum++, player))
		return SFN_ERROR_TYPE_ARGS;
	Assert(player > 0);
	Assert(player < k_MAX_PLAYERS);

	if(!args->GetPlayer(argNum++, foreigner))
		return SFN_ERROR_TYPE_ARGS;
	Assert(player > 0);
	Assert(player < k_MAX_PLAYERS);

	m_result.m_int = Diplomat::GetDiplomat(player).TestEffectiveRegard(foreigner, HOTWAR_REGARD);

	return SFN_ERROR_OK;
}







SFN_ERROR Slic_GetNuclearLaunchTarget::Call(SlicArgList *args)
{
	if(args->m_numArgs < 1)
		return SFN_ERROR_NUM_ARGS;

	sint32 argNum = 0;
	PLAYER_INDEX player;

	if(!args->GetPlayer(argNum++, player))
		return SFN_ERROR_TYPE_ARGS;
	Assert(player > 0);
	Assert(player < k_MAX_PLAYERS);

	m_result.m_int =
		Diplomat::GetDiplomat(player).GetNuclearLaunchTarget();

	return SFN_ERROR_OK;
}




SFN_ERROR Slic_TargetNuclearAttack::Call(SlicArgList *args)
{
	if(args->m_numArgs < 3)
		return SFN_ERROR_NUM_ARGS;

	sint32 argNum = 0;
	PLAYER_INDEX player;
	PLAYER_INDEX foreigner;
	sint32 launch_now;

	if(!args->GetPlayer(argNum++, player))
		return SFN_ERROR_TYPE_ARGS;
	Assert(player > 0);
	Assert(player < k_MAX_PLAYERS);

	if(!args->GetPlayer(argNum++, foreigner))
		return SFN_ERROR_TYPE_ARGS;
	Assert(player > 0);
	Assert(player < k_MAX_PLAYERS);

	if(!args->GetInt(argNum++, launch_now))
		return SFN_ERROR_TYPE_ARGS;
    
	Diplomat::GetDiplomat(player).TargetNuclearAttack(foreigner, (launch_now == 0 ? false : true));

	return SFN_ERROR_OK;
}


SFN_ERROR Slic_SetArmyDetachState::Call(SlicArgList *args)
{
	if(args->m_numArgs != 2)
		return SFN_ERROR_NUM_ARGS;

	Army a;
	if(!args->GetArmy(0, a))
		return SFN_ERROR_TYPE_ARGS;

	sint32 index;
	if(!args->GetInt(0, index))
		return SFN_ERROR_TYPE_ARGS;

	
	Scheduler::GetScheduler(a.GetOwner()).SetArmyDetachState(a,(index != 0));

	return SFN_ERROR_OK;
}
