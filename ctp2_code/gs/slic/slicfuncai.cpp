//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : SLIC AI functions
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
//
// Modifications from the original Activision code:
//
// - Many slic functions added. mea culpa, PFT
//
// Conventions (where used): <int|player> means this argument can be either an 
// int_t or a player builtin.
//
// Parameters surrounded by square brackets [ ] are optional.
//
// - Improved mod handling for Diplomod/WAW: reduces Asserts, restored 
//   compatibility.
//
//----------------------------------------------------------------------------

#include "c3.h"
#include "Unit.h"
#include "slicfuncai.h"
#include "AgreementMatrix.h"
#include "Army.h"
#include "Scheduler.h"
#include "diplomattypes.h"

#include "slicif.h"
#include "StrDB.h"
extern StringDB *g_theStringDB;

#include "Diplomat.h"
#include "mapanalysis.h"
#include "player.h"
#include "ProposalAnalysis.h"


//----------------------------------------------------------------------------
//
// Name       : ParseProposalDataBlock
//
// Description: Parse the proposal data of a single - optional - block.
//
// Parameters : args		: arguments of proposal data block
//				argNum		: (next) argument counter
//				blockType	: kind of proposal
//
// Globals    : -
//
// Returns    : bool		: data block has been parsed without errors
//				argNum		: updated
//              blockArgs	: filled with parsed data				
//
// Remark(s)  : - Added by Peter Triggs.
//              - Simple parameter (availability) checking only, no rollback
//				  when anything fails.
//				- A missing block (e.g. request without offer) will be 
//                indicated by a PROPOSAL_NONE marker, without parameters.
//
//----------------------------------------------------------------------------

bool ParseProposalDataBlock
(
	SlicArgList *		args, 
	sint32 &			argNum, 
	PROPOSAL_TYPE &		blockType,
	DiplomacyArg &		blockArgs
)
{
	sint32	type;
    Unit	city;
	// for clarity:
    sint32	adv;
    sint32	percent;
    sint32	gold;
    sint32	pollution;
    sint32	plyr;

	if(!args->GetInt(argNum++, type))
		return false;

	Assert(type >= PROPOSAL_NONE);
	Assert(type < PROPOSAL_MAX);

	blockType = static_cast<PROPOSAL_TYPE>(type);
	DPRINTF(k_DBG_SLIC, ("ParseProposalDataSlicArgs: type= %d\n", blockType));

	// get the proposal's arg1, if it exists
	switch (blockType) 
	{
	default:
		// No arguments expected.
		break;

    case PROPOSAL_OFFER_GIVE_CITY:  
	case PROPOSAL_REQUEST_GIVE_CITY:
	    if (!args->GetCity(argNum++, city)) 
		{
		    return false;
		}
		blockArgs.cityId = city.m_id;
		break;

	case PROPOSAL_OFFER_STOP_RESEARCH:		
	case PROPOSAL_REQUEST_STOP_RESEARCH:
	    if (!args->GetInt(argNum++, adv)) 
		{
		    return false;
		}
		blockArgs.advanceType = adv;
		break;

	case PROPOSAL_OFFER_REDUCE_NUCLEAR_WEAPONS:
	case PROPOSAL_REQUEST_REDUCE_NUCLEAR_WEAPONS:
	case PROPOSAL_OFFER_REDUCE_BIO_WEAPONS:
	case PROPOSAL_REQUEST_REDUCE_BIO_WEAPONS:
	case PROPOSAL_OFFER_REDUCE_NANO_WEAPONS:
	case PROPOSAL_REQUEST_REDUCE_NANO_WEAPONS:
        if (!args->GetInt(argNum++, percent)) 
		{
		    return false;
		}
		blockArgs.percent= ((double) percent)/100.0;
		break;

	case PROPOSAL_OFFER_GIVE_ADVANCE:
	case PROPOSAL_REQUEST_GIVE_ADVANCE:
		if (!args->GetInt(argNum++, adv)) 
		{
		    return false;
		}
		blockArgs.advanceType = adv;
		break;

	case PROPOSAL_OFFER_GIVE_GOLD:
	case PROPOSAL_REQUEST_GIVE_GOLD:
	    if (!args->GetInt(argNum++, gold)) 
		{
		    return false;
		}
		blockArgs.gold = gold;
		break;

	case PROPOSAL_OFFER_REDUCE_POLLUTION:
	case PROPOSAL_REQUEST_REDUCE_POLLUTION:
	case PROPOSAL_OFFER_HONOR_POLLUTION_AGREEMENT:
	case PROPOSAL_REQUEST_HONOR_POLLUTION_AGREEMENT:
        if (!args->GetInt(argNum++, pollution)) 
		{
		    return false;
		}
		blockArgs.pollution = pollution;
		break;

	case PROPOSAL_OFFER_HONOR_MILITARY_AGREEMENT:
	case PROPOSAL_REQUEST_HONOR_MILITARY_AGREEMENT:
        if (!args->GetInt(argNum++, plyr)) 
		{
		    return false;
		}
		blockArgs.playerId = plyr;
		break;
	}

	return true;
}

//----------------------------------------------------------------------------
//
// Name       : ParseProposalDataSlicArgs
//
// Description: Parse the request and offer data of a proposal.
//
// Parameters : args		: arguments of proposal data block
//				argNum		: (next) argument counter
//				blockType	: kind of proposal
//
// Globals    : -
//
// Returns    : bool		: data block has been parsed without errors
//				argNum		: updated
//              data		: filled with parsed data				
//
// Remark(s)  : - Simple parameter (availability) checking only, no rollback
//				  when anything fails.
//				- A missing block (e.g. request without offer) will be 
//                indicated by a PROPOSAL_NONE marker, without parameters.
//
//----------------------------------------------------------------------------

bool ParseProposalDataSlicArgs
(
	SlicArgList *	args, 
	sint32 &		argNum, 
	ProposalData &	data
)
{
	return ParseProposalDataBlock(args, argNum, data.first_type, data.first_arg) &&
		   ParseProposalDataBlock(args, argNum, data.second_type, data.second_arg);
}



//Added by Peter Triggs

// a somewhat complicated function

// it parses (sender,receiver,priority,type(=RESPONSE_TYPE)

// if type == RESPONSE_ACCEPT=1 or type == RESPONSE_REJECT=2, that's it: no more parameters

// if type == RESPONSE_COUNTER, it wants tone, num_types (1 or 2), 
// and then first_type[,first_arg][,second_type [,second_arg]] as in ConsiderNewProposal

// if type == RESPONSE_THREATEN, it wants an int for THREAT_TYPE, and then possibly a city_t 

bool ParseResponseSlicArgs(SlicArgList *args, sint32 &argNum, Response &response) {

	sint32 type;
	sint32 threat_type;
	sint32 priority;
	sint32 tone;
	Unit   city;
	sint32 num_types;

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
DPRINTF(k_DBG_SLIC, ("ParseResponseSlicArgs: type= %d\n", response.type));
    if ( type == RESPONSE_ACCEPT) {
        return true;
	}
	else if ( type == RESPONSE_REJECT) {
        return true;
	}
	else if ( type == RESPONSE_COUNTER ) {
        if(!args->GetInt(argNum++, tone))
		    return false;
	    Assert( tone >= 0);
	    response.counter.tone=static_cast<DIPLOMATIC_TONE>(tone);

        if(!args->GetInt(argNum++, num_types))// 1 or 2
		    return false;
	    Assert( num_types >0 && num_types <3);

	    if (!ParseProposalDataSlicArgs(args, argNum, response.counter))
		{
		    return false;
		}
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
            if(!args->GetCity(argNum++, city))
				return false;			
			response.threat.arg.cityId = city.m_id;
			break;
		}
	}
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

	// Diplomatic tone as optional argument
	sint32	tone;
	if ((argNum < args->m_numArgs) && args->GetInt(argNum++, tone))
	{
		Assert((tone >= DIPLOMATIC_TONE_NOT_CHOSEN) && (tone < DIPLOMATIC_TONE_MAX));
		new_proposal.detail.tone = static_cast<DIPLOMATIC_TONE>(tone);
	}

	return true;
}

//----------------------------------------------------------------------------
//
// Name       : Slic_<function>::Call
//
// Description: Handle a SLIC <function> call (e.g. from a .slc script).
//
// Parameters : args		: list of arguments
//
// Globals    : -
//
// Returns    : SFN_ERROR	: execution indicator
//
// Remark(s)  : - The function will return SFN_ERROR_OK on correct input.
//              - When applicable, data will be returned to the caller through
//                m_result.
//
//----------------------------------------------------------------------------

// VOID LogRegardEvent(<int|player>,         the current player
//                     <int|player>,         foreigner
//                      delta,               ammount the current player's regard to foreigner should change
//                      type                 REGARD_EVENT_TYPE 
//                      ID_string,           explanation string used in the Diplomacy intelligence screen
//                     [duration]            defaults to -1
//                     )

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
	Assert(player >= 0);
	Assert(player < k_MAX_PLAYERS);
	
	if(!args->GetPlayer(argNum++, foreigner))
		return SFN_ERROR_TYPE_ARGS;
	Assert(foreigner >= 0);
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

// INT GetPublicRegard(<int|player>, <int|player>, int)

// the last int is a REGARD_EVENT_TYPE, use REGARD_EVENT_ALL = 7 to get total regard
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

// INT GetEffectiveRegard(<int|player>, <int|player>)
// returns regard adjusted for being at war (how much does first player like second player)

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

// INT GetTrust(<int|player>, <int|player>)
// returns how much does first player trust second player
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

//VOID SetTrust(<int|player>, <int|player>, int)

// Sets first player's trust towards second player to int (which normally should be 0<= int <=1000).

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
	Assert(player >= 0);
	Assert(player < k_MAX_PLAYERS);
	
	if(!args->GetPlayer(argNum++, foreigner))
		return SFN_ERROR_TYPE_ARGS;
	Assert(foreigner >= 0);
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








// VOID  ConsiderResponse(sender,
//                        receiver, 
//                        priority,                      
//                        response_type,     RESPONSE_REJECT=1,RESPONSE_ACCEPT=2,RESPONSE_COUNTER=3,RESPONSE_THREATEN=4
//                        [counter_type|threat_type], 
//                             if counter:  tone,num_types,first_type [,first_arg][,second_type [,second_arg]]
//                             if threat:  [city]
//                        );
//

SFN_ERROR Slic_ConsiderResponse::Call(SlicArgList *args)
{
	if(args->m_numArgs < 4)
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

DPRINTF(k_DBG_SLIC, ("ConsiderResponse:sender %d, receiver %d, type %d\n",
		response.senderId,response.receiverId, response.type));

	return SFN_ERROR_OK;
}

// as Slic_ConsiderResponse (but priority should be set to 0 ?)
SFN_ERROR Slic_SetResponse::Call(SlicArgList *args)
{
	if(args->m_numArgs < 6)
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




// VOID ConsiderMotivation(player, priority, type[,<player|unit|advance>])
// here type is a MOTIVATION_TYPE
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

// ----------------------------------------------------------------------------------------
//
// VOID ConsiderNewProposal
// 1.	sender 
// 2.	receiver 
// 3.	priority			- int, the proposal's priority 
// 4.   first_type			- int, ProposalDB index of first proposal
//			[first_args]	- if proposal has 'arg1' in ProposalDB, this is it
// 5.   second_type 		- int, optional ProposalDB index of 'in addition' proposal 
//          [second_args]	- if proposal has 'arg1' in ProposalDB, this is it
// 6.   stringID1
// 7.   stringID2
// 8.	stringID3
//          [tone]			- int, 0 (Magnanimous/Kind) to 4 (Hostile/Angry)
//
// The type of first_arg and second_arg depends on first_type and second_type respectively.
// See ParseProposalDataSlicArgs
//
//-------------------------------------------------------------------------------------------
SFN_ERROR Slic_ConsiderNewProposal::Call(SlicArgList *args)
{
	if (args->m_numArgs < 8)
		return SFN_ERROR_NUM_ARGS;

	sint32 argNum = 0;
	NewProposal new_proposal;

	if (!ParseNewProposalSlicArgs(args, argNum, new_proposal))
		return SFN_ERROR_TYPE_ARGS;

	Diplomat::GetDiplomat(new_proposal.senderId).
		ConsiderNewProposal(new_proposal.receiverId, new_proposal);

DPRINTF(k_DBG_SLIC, ("ConsiderNewProposal:sender %d, receiver %d, prop %d\n",
		new_proposal.senderId,new_proposal.receiverId, new_proposal.detail.first_type));

	return SFN_ERROR_OK;
}


// ----------------------------------------------------------------------------------------
//
// VOID SetNewProposal 
// 1.	sender 
// 2.	receiver 
// 3.	priority			- int, the proposal's priority 
// 4.   first_type			- int, ProposalDB index of first proposal
//			[first_args]	- if proposal has 'arg1' in ProposalDB, this is it
// 5.   second_type 		- int, optional ProposalDB index of 'in addition' proposal 
//          [second_args]	- if proposal has 'arg1' in ProposalDB, this is it
// 6.   stringID1
// 7.   stringID2
// 8.	stringID3
//          [tone]			- int, 0 (Magnanimous/Kind) to 4 (Hostile/Angry)
//
// The type of first_arg and second_arg depends on first_type and second_type respectively.
// See ParseProposalDataSlicArgs
//
//------------------------------------------------------------------------------------------- 
SFN_ERROR Slic_SetNewProposal::Call(SlicArgList *args)
{
	if (args->m_numArgs < 8)
		return SFN_ERROR_NUM_ARGS;

	sint32 argNum = 0;
	NewProposal new_proposal;

	if (!ParseNewProposalSlicArgs(args, argNum, new_proposal))
		return SFN_ERROR_TYPE_ARGS;

	// Diplomatic tone as optional argument
	sint32	tone;
	if ((argNum < args->m_numArgs) && args->GetInt(argNum++, tone))
	{
		Assert((tone >= DIPLOMATIC_TONE_NOT_CHOSEN) && (tone < DIPLOMATIC_TONE_MAX));
		new_proposal.detail.tone = static_cast<DIPLOMATIC_TONE>(tone);
	}

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

    // optional string arguments:

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

// INT GetTradeFrom(player,foreigner)

// Returns how much trade (in gold) player got from foreigner

SFN_ERROR Slic_GetTradeFrom::Call(SlicArgList *args)
{
//Implemented by Peter Triggs
	if(args->m_numArgs < 2)
		return SFN_ERROR_NUM_ARGS;

	sint32 player;
	sint32 foreigner;
	sint32 argNum = 0;

	if(!args->GetPlayer(argNum++, player))
		return SFN_ERROR_TYPE_ARGS;

	if(!args->GetPlayer(argNum++, foreigner))
		return SFN_ERROR_TYPE_ARGS;

    m_result.m_int = Diplomat::GetDiplomat(player).GetTradeFrom(foreigner);
	return SFN_ERROR_OK;
}

// INT GetTributeFrom(player,foreigner)

// Returns how much tribute (in gold) player got from foreigner
 
SFN_ERROR Slic_GetTributeFrom::Call(SlicArgList *args)
{
//Implemented by Peter Triggs
	if(args->m_numArgs < 2)
		return SFN_ERROR_NUM_ARGS;

	sint32 player;
	sint32 foreigner;
	sint32 argNum = 0;

	if(!args->GetPlayer(argNum++, player))
		return SFN_ERROR_TYPE_ARGS;

	if(!args->GetPlayer(argNum++, foreigner))
		return SFN_ERROR_TYPE_ARGS;

    m_result.m_int = Diplomat::GetDiplomat(player).GetTributeFrom(foreigner);
	return SFN_ERROR_OK;
}

// INT GetGoldSurplusPercent(player);

// Returns player's current savings as a percentage of what he spends (so >100)
 
SFN_ERROR Slic_GetGoldSurplusPercent::Call(SlicArgList *args)
{
//Implemented by Peter Triggs
	if(args->m_numArgs < 1)
		return SFN_ERROR_NUM_ARGS;

	sint32 player;
	sint32 argNum = 0;

	if(!args->GetPlayer(argNum++, player))
		return SFN_ERROR_TYPE_ARGS;

    m_result.m_int = Diplomat::GetDiplomat(player).GetGoldSurplusPercent();
	return SFN_ERROR_OK;
}

// the function Diplomat::CanBuySurplus doesn't make any sense to me

SFN_ERROR Slic_CanBuySurplus::Call(SlicArgList *args)
{
	//Needs to be filled
	return SFN_ERROR_OK;
}
 
// INT GetAdvanceLevelPercent(player, foreigner);

// Returns player's advance level (number of advances he's researched) as a percentage of foreigner's
 
SFN_ERROR Slic_GetAdvanceLevelPercent::Call(SlicArgList *args)
{
//Implemented by Peter Triggs
    if(args->m_numArgs < 2)
		return SFN_ERROR_NUM_ARGS;

	sint32 player;
	sint32 foreigner;
	sint32 argNum = 0;

	if(!args->GetPlayer(argNum++, player))
		return SFN_ERROR_TYPE_ARGS;

	if(!args->GetPlayer(argNum++, foreigner))
		return SFN_ERROR_TYPE_ARGS;

	m_result.m_int = Diplomat::GetDiplomat(player).GetAdvanceLevelPercent(foreigner);
	return SFN_ERROR_OK;
}

//INT AtWarCount(<int|player>)

//Returns the number of civs that player is at war with.

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

//INT EffectiveAtWarCount(<int|player>)

//Returns the number of civs that player is effectively at war with (effective regard<100=HOTWAR_REGARD).

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

//INT AtWarWith(<int|player>, <int|player>) 

//Returns 1 or 0.

SFN_ERROR Slic_AtWarWith::Call(SlicArgList *args)
{
	if(args->m_numArgs < 2)
		return SFN_ERROR_NUM_ARGS;

	sint32 argNum = 0;
	PLAYER_INDEX player;
	PLAYER_INDEX foreigner;

	if(!args->GetPlayer(argNum++, player))
		return SFN_ERROR_TYPE_ARGS;

	Assert(player >= 0);

	Assert(player < k_MAX_PLAYERS);

	if(!args->GetPlayer(argNum++, foreigner))
		return SFN_ERROR_TYPE_ARGS;

	Assert(foreigner >= 0);

	Assert(foreigner < k_MAX_PLAYERS);

	// Everyone is always at war with the barbarians.
	m_result.m_int = (PLAYER_INDEX_VANDALS == player)		||
				     (PLAYER_INDEX_VANDALS == foreigner)	||
					 AgreementMatrix::s_agreements.HasAgreement
						(player, foreigner, PROPOSAL_TREATY_DECLARE_WAR);

	return SFN_ERROR_OK;
}

//INT EffectiveWarWith(<int|player>, <int|player>)  

//Returns 1 if first player is effectively at war with second player (effective regard<100=HOTWAR_REGARD).

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

	Assert(foreigner > 0);

	Assert(foreigner < k_MAX_PLAYERS);

	m_result.m_int = Diplomat::GetDiplomat(player).TestEffectiveRegard(foreigner, HOTWAR_REGARD);

	return SFN_ERROR_OK;
}

// INT GetNuclearLaunchTarget(<int|player>);

// Returns the player index of the civ we'd most like to nuke.
// See Diplomat::ComputeNuclearLaunchTarget() for how this is determined

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

// VOID TargetNuclearAttack(player,foreigner,launch_now);

// Target player's nukes onto foreigner's cities and launch if launch_now =1

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
	Assert(foreigner > 0);
	Assert(foreigner < k_MAX_PLAYERS);

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

// INT GetBorderIncursionBy(<int|player>,<int|player>); 
// returns 1 if second player is trespassing on first player

SFN_ERROR Slic_GetBorderIncursionBy::Call(SlicArgList *args)
{
    if(args->m_numArgs != 2)
		return SFN_ERROR_NUM_ARGS;

	sint32 argNum = 0;
	PLAYER_INDEX player;
	PLAYER_INDEX foreigner;

	if(!args->GetPlayer(argNum++, player))
		return SFN_ERROR_TYPE_ARGS;

    if(player < 0 || player >= k_MAX_PLAYERS)
		return SFN_ERROR_OUT_OF_RANGE;
	
	if(!args->GetPlayer(argNum++, foreigner))
		return SFN_ERROR_TYPE_ARGS;

    if(foreigner < 0 || foreigner >= k_MAX_PLAYERS)
		return SFN_ERROR_OUT_OF_RANGE;

	m_result.m_int = Diplomat::GetDiplomat(player).GetBorderIncursionBy(foreigner);


    DPRINTF(k_DBG_SLIC, ("Slic_GetBorderIncursionBy:player %d, foreigner %d, returns %d\n",player,foreigner, m_result.m_int));

	return SFN_ERROR_OK;
}

//  INT GetLastNewProposalType(player, foreigner, first/second_type); where player is the sender,
//  foreigner is the receiver, and first/second_type is 0 to get the type of the 
//  'initial' proposal or 1 to get the type of the 'in addition' proposal if it has one. 
//  The returned INT is the NewProposalDB index

SFN_ERROR Slic_GetLastNewProposalType::Call(SlicArgList *args)
{
    if(args->m_numArgs != 3)
		return SFN_ERROR_NUM_ARGS;

	sint32 argNum = 0;
	PLAYER_INDEX player;
	PLAYER_INDEX foreigner;
	sint32 type;

	if(!args->GetPlayer(argNum++, player))
		return SFN_ERROR_TYPE_ARGS;

    if(player < 0 || player >= k_MAX_PLAYERS)
		return SFN_ERROR_OUT_OF_RANGE;
	
	if(!args->GetPlayer(argNum++, foreigner))
		return SFN_ERROR_TYPE_ARGS;

    if(foreigner < 0 || foreigner >= k_MAX_PLAYERS)
		return SFN_ERROR_OUT_OF_RANGE;

    if(!args->GetInt(argNum++, type))
		return SFN_ERROR_TYPE_ARGS;

    if(type<0 || type >1)
        return SFN_ERROR_OUT_OF_RANGE;

	if(type==0){
        m_result.m_int =Diplomat::GetDiplomat(player).GetMyLastNewProposal(foreigner).detail.first_type;
    }
	else {
        m_result.m_int =Diplomat::GetDiplomat(player).GetMyLastNewProposal(foreigner).detail.second_type;
	}

    DPRINTF(k_DBG_SLIC, ("GetLastNewProposalType:sender %d, receiver %d, type %d returns %d\n",
		player,foreigner, type, m_result.m_int));

	return SFN_ERROR_OK;
}

// No wonder this was left undone. It looks very messy.

SFN_ERROR Slic_GetLastNewProposalArg::Call(SlicArgList *args)
{
	m_result.m_int = 0;
	return SFN_ERROR_OK;
}

//INT GetLastNewProposalTone(player, foreigner);
//Returns  0= Magnanimous/Kind, 1=Friendly/Equal,  2=Obsequious/Meek,  3=Haughty/Indignant,  4=Hostile/Angry. 

SFN_ERROR Slic_GetLastNewProposalTone::Call(SlicArgList *args)
{
    if(args->m_numArgs != 2)
		return SFN_ERROR_NUM_ARGS;

	sint32 argNum = 0;
	PLAYER_INDEX player;
	PLAYER_INDEX foreigner;

	if(!args->GetPlayer(argNum++, player))
		return SFN_ERROR_TYPE_ARGS;

    if(player < 0 || player >= k_MAX_PLAYERS)
		return SFN_ERROR_OUT_OF_RANGE;
	
	if(!args->GetPlayer(argNum++, foreigner))
		return SFN_ERROR_TYPE_ARGS;

    if(foreigner < 0 || foreigner >= k_MAX_PLAYERS)
		return SFN_ERROR_OUT_OF_RANGE;

	m_result.m_int = Diplomat::GetDiplomat(player).GetMyLastNewProposal(foreigner).detail.tone;

    DPRINTF(k_DBG_SLIC, ("Slic_GetLastNewProposalTone:player %d, foreigner %d, returns %d\n",player,foreigner, m_result.m_int));

	return SFN_ERROR_OK;
}

// INT GetLastResponseType(player, foreigner);
// player is the sender of the original proposal and foreigner is the responder.
// Returns 0=RESPONSE_INVALID, 1=RESPONSE_REJECT, 2=RESPONSE_ACCEPT, 3=RESPONSE_COUNTER, 4=RESPONSE_THREATEN
 
SFN_ERROR Slic_GetLastResponseType::Call(SlicArgList *args)
{
    if(args->m_numArgs != 2)
		return SFN_ERROR_NUM_ARGS;

	sint32 argNum = 0;
	PLAYER_INDEX player;
	PLAYER_INDEX foreigner;
	

	if(!args->GetPlayer(argNum++, player))
		return SFN_ERROR_TYPE_ARGS;

    if(player < 0 || player >= k_MAX_PLAYERS)
		return SFN_ERROR_OUT_OF_RANGE;
	
	if(!args->GetPlayer(argNum++, foreigner))
		return SFN_ERROR_TYPE_ARGS;

    if(foreigner < 0 || foreigner >= k_MAX_PLAYERS)
		return SFN_ERROR_OUT_OF_RANGE;

    m_result.m_int =Diplomat::GetDiplomat(player).GetMyLastResponse(foreigner).type;

    DPRINTF(k_DBG_SLIC, ("Slic_GetLastResponseType:player %d, foreigner %d, returns %d\n",player,foreigner, m_result.m_int));

	return SFN_ERROR_OK;
}

// INT GetLastCounterResponseType(player, foreigner, first/secondtype);
// player is the one who countered and foreigner is the sender of the original proposal.
// first/second_type is 0 to get the type of the 'initial' proposal or 1 to get the type of the
// 'in addition' proposal, if it has one. 
// The returned INT is the NewProposalDB index 

SFN_ERROR Slic_GetLastCounterResponseType::Call(SlicArgList *args)
{
	if(args->m_numArgs != 3)
		return SFN_ERROR_NUM_ARGS;

	sint32 argNum = 0;
	PLAYER_INDEX player;
	PLAYER_INDEX foreigner;
	sint32 dtype;

	if(!args->GetPlayer(argNum++, player))
		return SFN_ERROR_TYPE_ARGS;

    if(player < 0 || player >= k_MAX_PLAYERS)
		return SFN_ERROR_OUT_OF_RANGE;
	
	if(!args->GetPlayer(argNum++, foreigner))
		return SFN_ERROR_TYPE_ARGS;

    if(foreigner < 0 || foreigner >= k_MAX_PLAYERS)
		return SFN_ERROR_OUT_OF_RANGE;

    if(!args->GetInt(argNum++, dtype))
		return SFN_ERROR_TYPE_ARGS;

    if ( Diplomat::GetDiplomat(player).GetMyLastResponse(foreigner).type == RESPONSE_COUNTER ){
	    if(dtype==0){
            m_result.m_int =Diplomat::GetDiplomat(player).GetMyLastResponse(foreigner).counter.first_type;
		}
	    else {
		     if(dtype==1){
                 m_result.m_int =Diplomat::GetDiplomat(player).GetMyLastResponse(foreigner).counter.second_type;
			 }
		     else { 
			      if(dtype<0 || dtype >1){
                      return SFN_ERROR_OUT_OF_RANGE;
				  }
			 }
		}
	}
	else m_result.m_int =0;

    DPRINTF(k_DBG_SLIC, ("Slic_GetLastCounterResponseType:player %d, foreigner %d, dtype %d, returns %d\n",player,foreigner,dtype, m_result.m_int));

	return SFN_ERROR_OK;
}
// see Slic_GetLastNewProposalArg
SFN_ERROR Slic_GetLastCounterResponseArg::Call(SlicArgList *args)
{
	m_result.m_int = 0;
	return SFN_ERROR_OK;
}

// INT  GetLastThreatResponseType(player, foreigner);
// player is the sender of the original proposal and foreigner is the receiver.
//Returns: 1=THREAT_DESTROY_CITY, 2=THREAT_TRADE_EMBARGO, 3=THREAT_DECLARE_WAR

SFN_ERROR Slic_GetLastThreatResponseType::Call(SlicArgList *args)
{
    if(args->m_numArgs != 2)
		return SFN_ERROR_NUM_ARGS;

	sint32 argNum = 0;
	PLAYER_INDEX player;
	PLAYER_INDEX foreigner;
	

	if(!args->GetPlayer(argNum++, player))
		return SFN_ERROR_TYPE_ARGS;

    if(player < 0 || player >= k_MAX_PLAYERS)
		return SFN_ERROR_OUT_OF_RANGE;
	
	if(!args->GetPlayer(argNum++, foreigner))
		return SFN_ERROR_TYPE_ARGS;

    if(foreigner < 0 || foreigner >= k_MAX_PLAYERS)
		return SFN_ERROR_OUT_OF_RANGE;

    if ( Diplomat::GetDiplomat(player).GetMyLastResponse(foreigner).type == RESPONSE_THREATEN ){
        m_result.m_int =Diplomat::GetDiplomat(player).GetMyLastResponse(foreigner).threat.type;
    }
	else{
	    m_result.m_int = 0;
    }

    DPRINTF(k_DBG_SLIC, ("Slic_GetLastThreatResponseType:player %d, foreigner %d, returns %d\n",player,foreigner, m_result.m_int));

	return SFN_ERROR_OK;
}

// see Slic_GetLastNewProposalArg

SFN_ERROR Slic_GetLastThreatResponseArg::Call(SlicArgList *args)
{
	m_result.m_int = 0;
	return SFN_ERROR_OK;
}

//INT GetAgreementDuration(<int|player>, <int|player>, int) 

//The final int is a proposalDB index. Returns the number of turns the first player has 
//had this agreement with the second.

SFN_ERROR Slic_GetAgreementDuration::Call(SlicArgList *args)
{
    if(args->m_numArgs != 3)
		return SFN_ERROR_NUM_ARGS;

	sint32 argNum = 0;
	PLAYER_INDEX player;
	PLAYER_INDEX foreigner;
	sint32 type;

	if(!args->GetPlayer(argNum++, player))
		return SFN_ERROR_TYPE_ARGS;

    if(player < 0 || player >= k_MAX_PLAYERS)
		return SFN_ERROR_OUT_OF_RANGE;
	
	if(!args->GetPlayer(argNum++, foreigner))
		return SFN_ERROR_TYPE_ARGS;

    if(foreigner < 0 || foreigner >= k_MAX_PLAYERS)
		return SFN_ERROR_OUT_OF_RANGE;

    if(!args->GetInt(argNum++, type))
		return SFN_ERROR_TYPE_ARGS;

	m_result.m_int = AgreementMatrix::s_agreements.GetAgreementDuration(player, foreigner, (const enum PROPOSAL_TYPE)type);

	DPRINTF(k_DBG_SLIC, ("Slic_GetAgreementDuration:player %d, foreigner %d, type %d, returns %d\n",player,foreigner,type, m_result.m_int));
	
	return SFN_ERROR_OK;
}

//INT GetNewProposalPriority(player,foreigner,int)

// where the last int is a ProposalDB index. It returns the priority for this proposal as determined
// by the current diplomactic state that player takes toward foreigner. 

SFN_ERROR Slic_GetNewProposalPriority::Call(SlicArgList *args)
{
    if(args->m_numArgs != 3)
		return SFN_ERROR_NUM_ARGS;

	sint32 argNum = 0;
	PLAYER_INDEX player;
	PLAYER_INDEX foreigner;
	sint32 prop;

	if(!args->GetPlayer(argNum++, player))
		return SFN_ERROR_TYPE_ARGS;

    if(player < 0 || player >= k_MAX_PLAYERS)
		return SFN_ERROR_OUT_OF_RANGE;
	
	if(!args->GetPlayer(argNum++, foreigner))
		return SFN_ERROR_TYPE_ARGS;

    if(foreigner < 0 || foreigner >= k_MAX_PLAYERS)
		return SFN_ERROR_OUT_OF_RANGE;

    if(!args->GetInt(argNum++, prop))
		return SFN_ERROR_TYPE_ARGS;

	m_result.m_int = Diplomat::GetDiplomat(player).GetNewProposalPriority(foreigner, (const enum PROPOSAL_TYPE) prop) ;

	DPRINTF(k_DBG_SLIC, ("Slic_GetNewProposalPriority:player %d, foreigner %d, prop $d, returns %d\n",player,foreigner,prop, m_result.m_int));
		
	return SFN_ERROR_OK;
}

// INT GetNextAdvance(player);
// returns the advanceDB index of the first advance from the player's AdvanceList_XXX
// that the player has not yet researched

SFN_ERROR Slic_GetNextAdvance::Call(SlicArgList *args)
{
    if(args->m_numArgs != 1)
		return SFN_ERROR_NUM_ARGS;

	PLAYER_INDEX player;

	if(!args->GetPlayer(0, player))
		return SFN_ERROR_TYPE_ARGS;

    if(player < 0 || player >= k_MAX_PLAYERS)
		return SFN_ERROR_OUT_OF_RANGE;

	m_result.m_int = Diplomat::GetDiplomat(player).GetNextAdvance();
	return SFN_ERROR_OK;
}

// INT GetDesiredAdvanceFrom(player,foreigner,min_cost,max_cost)
// Returns the AdvanceDB index of 
SFN_ERROR Slic_GetDesiredAdvanceFrom::Call(SlicArgList *args)
{
if(args->m_numArgs != 4)
		return SFN_ERROR_NUM_ARGS;

	sint32 argNum = 0;
	PLAYER_INDEX player;
	PLAYER_INDEX foreigner;
	sint32 min_cost;
	sint32 max_cost;

	if(!args->GetPlayer(argNum++, player))
		return SFN_ERROR_TYPE_ARGS;

    if(player < 0 || player >= k_MAX_PLAYERS)
		return SFN_ERROR_OUT_OF_RANGE;
	
	if(!args->GetPlayer(argNum++, foreigner))
		return SFN_ERROR_TYPE_ARGS;

    if(foreigner < 0 || foreigner >= k_MAX_PLAYERS)
		return SFN_ERROR_OUT_OF_RANGE;

    if(!args->GetInt(argNum++, min_cost))
		return SFN_ERROR_TYPE_ARGS;

    if(!args->GetInt(argNum++, max_cost))
		return SFN_ERROR_TYPE_ARGS;

	m_result.m_int = Diplomat::GetDiplomat(player).GetDesiredAdvanceFrom(foreigner,min_cost,max_cost);
	return SFN_ERROR_OK;
}

// INT GetLastBorderIncursion(player, foreigner); 
// Returns the turn that foreigner last trespassed on player 

SFN_ERROR Slic_GetLastBorderIncursion::Call(SlicArgList *args)
{
    if(args->m_numArgs != 2)
		return SFN_ERROR_NUM_ARGS;

	sint32 argNum = 0;
	PLAYER_INDEX player;
	PLAYER_INDEX foreigner;
	

	if(!args->GetPlayer(argNum++, player))
		return SFN_ERROR_TYPE_ARGS;

    if(player < 0 || player >= k_MAX_PLAYERS)
		return SFN_ERROR_OUT_OF_RANGE;
	
	if(!args->GetPlayer(argNum++, foreigner))
		return SFN_ERROR_TYPE_ARGS;

    if(foreigner < 0 || foreigner >= k_MAX_PLAYERS)
		return SFN_ERROR_OUT_OF_RANGE;

	m_result.m_int = Diplomat::GetDiplomat(player).GetLastBorderIncursionBy(foreigner);

    DPRINTF(k_DBG_SLIC, ("Slic_GetLastBorderIncursion:player %d, foreigner %d, returns %d\n",player,foreigner, m_result.m_int));
	
	return SFN_ERROR_OK;
}
// INT GetPersonalityType(<int|player>);

// returns the PersonalityDB index of player's leader's personality

SFN_ERROR Slic_GetPersonalityType::Call(SlicArgList *args)
{
    if(args->m_numArgs != 1)
		return SFN_ERROR_NUM_ARGS;

	PLAYER_INDEX player;

	if(!args->GetPlayer(0, player))
		return SFN_ERROR_TYPE_ARGS;

    if(player < 0 || player >= k_MAX_PLAYERS)
		return SFN_ERROR_OUT_OF_RANGE;

	m_result.m_int =(sint32) Diplomat::GetDiplomat(player).GetPersonality();
	return SFN_ERROR_OK;
}

// INT GetAtRiskCitiesValue(player, foreigner);

// the 'value' of a city is determined by what buildings and wonders it contains.
// 'total value' is the sum of player's city values.

// returns a number that represents the percentage of player's total value that is threatened
// by foreigner. 

SFN_ERROR Slic_GetAtRiskCitiesValue::Call(SlicArgList *args)
{   
	if(args->m_numArgs != 2)
		return SFN_ERROR_NUM_ARGS;

	sint32 argNum = 0;
	PLAYER_INDEX player;
	PLAYER_INDEX foreigner;
	

	if(!args->GetPlayer(argNum++, player))
		return SFN_ERROR_TYPE_ARGS;

    if(player < 0 || player >= k_MAX_PLAYERS)
		return SFN_ERROR_OUT_OF_RANGE;
	
	if(!args->GetPlayer(argNum++, foreigner))
		return SFN_ERROR_TYPE_ARGS;

    if(foreigner < 0 || foreigner >= k_MAX_PLAYERS)
		return SFN_ERROR_OUT_OF_RANGE;

	m_result.m_int = MapAnalysis::GetMapAnalysis().AtRiskCitiesValue(player,foreigner);

    DPRINTF(k_DBG_SLIC, ("Slic_GetAtRiskCitiesValue:player %d, foreigner %d, returns %d\n",player,foreigner, m_result.m_int));

	return SFN_ERROR_OK;
}

// INT GetRelativeStrength(player, foreigner);
//
// Returns 0=DIPLOMATIC_STRENGTH_VERY_WEAK, 1=DIPLOMATIC_STRENGTH_WEAK, 2=DIPLOMATIC_STRENGTH_AVERAGE, 
// 3=DIPLOMATIC_STRENGTH_STRONG, 4=DIPLOMATIC_STRENGTH_VERY_STRONG

SFN_ERROR Slic_GetRelativeStrength::Call(SlicArgList *args)
{
    if(args->m_numArgs != 2)
		return SFN_ERROR_NUM_ARGS;

	sint32 argNum = 0;
	PLAYER_INDEX player;
	PLAYER_INDEX foreigner;
	

	if(!args->GetPlayer(argNum++, player))
		return SFN_ERROR_TYPE_ARGS;

    if(player < 0 || player >= k_MAX_PLAYERS)
		return SFN_ERROR_OUT_OF_RANGE;
	
	if(!args->GetPlayer(argNum++, foreigner))
		return SFN_ERROR_TYPE_ARGS;

    if(foreigner < 0 || foreigner >= k_MAX_PLAYERS)
		return SFN_ERROR_OUT_OF_RANGE;

	m_result.m_int = g_player[player]->GetRelativeStrength(foreigner);

    DPRINTF(k_DBG_SLIC, ("Slic_GetRelativeStrength:player %d, foreigner %d, returns %d\n",player,foreigner, m_result.m_int));

	return SFN_ERROR_OK;
}

// INT GetDesireWarWith(player, foreigner);
// Returns 1 if player desires war with foreigner, else 0.

SFN_ERROR Slic_GetDesireWarWith::Call(SlicArgList *args)
{   
    if(args->m_numArgs != 2)
		return SFN_ERROR_NUM_ARGS;

	sint32 argNum = 0;
	PLAYER_INDEX player;
	PLAYER_INDEX foreigner;
	
	if(!args->GetPlayer(argNum++, player))
		return SFN_ERROR_TYPE_ARGS;

    if(player < 0 || player >= k_MAX_PLAYERS)
		return SFN_ERROR_OUT_OF_RANGE;
	
	if(!args->GetPlayer(argNum++, foreigner))
		return SFN_ERROR_TYPE_ARGS;

    if(foreigner < 0 || foreigner >= k_MAX_PLAYERS)
		return SFN_ERROR_OUT_OF_RANGE;

	m_result.m_int = Diplomat::GetDiplomat(player).DesireWarWith(foreigner);

    DPRINTF(k_DBG_SLIC, ("Slic_GetDesireWarWith:player %d, foreigner %d, returns %d\n",player,foreigner, m_result.m_int));

	return SFN_ERROR_OK;
}
// INT RoundPercentReduction(int); 0<=int<=100

// rounds off to a nice percentage
SFN_ERROR Slic_RoundPercentReduction::Call(SlicArgList *args)
{
    sint32 pc_int;
    if(!args->GetInt(1, pc_int))
		return SFN_ERROR_TYPE_ARGS;
	Assert( pc_int >= 0 && pc_int <= 100);
    double percent=pc_int/100;
	m_result.m_int = (sint32)(ProposalAnalysis::RoundPercentReduction(percent)*100);
	return SFN_ERROR_OK;
}

// INT RoundGold(int);

// rounds off int to the nearest 100 

SFN_ERROR Slic_RoundGold::Call(SlicArgList *args)
{
	sint32 gold;
    if(!args->GetInt(1, gold))
		return SFN_ERROR_TYPE_ARGS;
	Assert( gold >= 0);

	m_result.m_int = ProposalAnalysis::RoundGold( gold );
	return SFN_ERROR_OK;
}

// can't find this
SFN_ERROR Slic_GetPollutionLevelPromisedTo::Call(SlicArgList *args)
{
	m_result.m_int = 0;
	return SFN_ERROR_OK;
}

// INT GetPiracyIncomeFrom(player,foreigner)
// Returns the amount of piracy income that player gets from foreigner
SFN_ERROR Slic_GetPiracyIncomeFrom::Call(SlicArgList *args)
{   
    if(args->m_numArgs != 2)
		return SFN_ERROR_NUM_ARGS;

	sint32 argNum = 0;
	PLAYER_INDEX player;
	PLAYER_INDEX foreigner;
	
	if(!args->GetPlayer(argNum++, player))
		return SFN_ERROR_TYPE_ARGS;

    if(player < 0 || player >= k_MAX_PLAYERS)
		return SFN_ERROR_OUT_OF_RANGE;
	
	if(!args->GetPlayer(argNum++, foreigner))
		return SFN_ERROR_TYPE_ARGS;

    if(foreigner < 0 || foreigner >= k_MAX_PLAYERS)
		return SFN_ERROR_OUT_OF_RANGE;

	m_result.m_int = MapAnalysis::GetMapAnalysis().GetPiracyIncomeByPlayer(player,foreigner);

    DPRINTF(k_DBG_SLIC, ("Slic_GetPiracyIncome:player %d, foreigner %d, returns %d\n",player,foreigner, m_result.m_int));

	return SFN_ERROR_OK;
}

// INT GetProjectedScience(player) or city? Why not both?

SFN_ERROR Slic_GetProjectedScience::Call(SlicArgList *args)
{
	m_result.m_int = 0;
	return SFN_ERROR_OK;
}

// INT CanFormAlliance(player,foreigner)
// Returns 1 if player and foreigner can form an alliance, else 0
SFN_ERROR Slic_CanFormAlliance::Call(SlicArgList *args)
{
    if(args->m_numArgs != 2)
		return SFN_ERROR_NUM_ARGS;

	sint32 argNum = 0;
	PLAYER_INDEX player;
	PLAYER_INDEX foreigner;
	
	if(!args->GetPlayer(argNum++, player))
		return SFN_ERROR_TYPE_ARGS;

    if(player < 0 || player >= k_MAX_PLAYERS)
		return SFN_ERROR_OUT_OF_RANGE;
	
	if(!args->GetPlayer(argNum++, foreigner))
		return SFN_ERROR_TYPE_ARGS;

    if(foreigner < 0 || foreigner >= k_MAX_PLAYERS)
		return SFN_ERROR_OUT_OF_RANGE;

	m_result.m_int = Diplomat::GetDiplomat(player).CanFormAlliance(foreigner);

    DPRINTF(k_DBG_SLIC, ("Slic_CanFormAlliance:player %d, foreigner %d, returns %d\n",player,foreigner, m_result.m_int));

	return SFN_ERROR_OK;
}

SFN_ERROR Slic_GetStopResearchingAdvance::Call(SlicArgList *args)
{
	m_result.m_int = 0;
	return SFN_ERROR_OK;
}

// INT GetNanoWeaponsCount(player);

// returns the number of NanoWeapons that player owns

SFN_ERROR Slic_GetNanoWeaponsCount::Call(SlicArgList *args)
{
    if(args->m_numArgs != 1)
		return SFN_ERROR_NUM_ARGS;

	PLAYER_INDEX player;

	if(!args->GetPlayer(0, player))
		return SFN_ERROR_TYPE_ARGS;

    if(player < 0 || player >= k_MAX_PLAYERS)
		return SFN_ERROR_OUT_OF_RANGE;

	m_result.m_int = MapAnalysis::GetMapAnalysis().GetNanoWeaponsCount(player);;
	return SFN_ERROR_OK;
}

// INT GetBioWeaponsCount(player);

// returns the number of BioWeapons that player owns

SFN_ERROR Slic_GetBioWeaponsCount::Call(SlicArgList *args)
{
    if(args->m_numArgs != 1)
		return SFN_ERROR_NUM_ARGS;

	PLAYER_INDEX player;

	if(!args->GetPlayer(0, player))
		return SFN_ERROR_TYPE_ARGS;

    if(player < 0 || player >= k_MAX_PLAYERS)
		return SFN_ERROR_OUT_OF_RANGE;

	m_result.m_int = MapAnalysis::GetMapAnalysis().GetBioWeaponsCount(player);
	return SFN_ERROR_OK;
}

// INT GetNuclearWeaponsCount(player);

// returns the number of NuclearWeapons that player owns

SFN_ERROR Slic_GetNuclearWeaponsCount::Call(SlicArgList *args)
{
    if(args->m_numArgs != 1)
		return SFN_ERROR_NUM_ARGS;

	PLAYER_INDEX player;

	if(!args->GetPlayer(0, player))
		return SFN_ERROR_TYPE_ARGS;

    if(player < 0 || player >= k_MAX_PLAYERS)
		return SFN_ERROR_OUT_OF_RANGE;

	m_result.m_int = MapAnalysis::GetMapAnalysis().GetNuclearWeaponsCount(player);
	return SFN_ERROR_OK;
}


//INT FindCityToExtortFrom(player, foreigner, max_value, &cityvar)
SFN_ERROR Slic_FindCityToExtortFrom::Call(SlicArgList *args)
{
	m_result.m_int = 0;
	return SFN_ERROR_OK;
}

// INT GetEmbargo(player, foreigner)
// Returns 1 if foreigner has an embargo on player, else 0 (or -1 if no contact ?)
SFN_ERROR Slic_GetEmbargo::Call(SlicArgList *args)
{
    if(args->m_numArgs != 2)
		return SFN_ERROR_NUM_ARGS;

	sint32 argNum = 0;
	PLAYER_INDEX player;
	PLAYER_INDEX foreigner;
	
	if(!args->GetPlayer(argNum++, player))
		return SFN_ERROR_TYPE_ARGS;

    if(player < 0 || player >= k_MAX_PLAYERS)
		return SFN_ERROR_OUT_OF_RANGE;
	
	if(!args->GetPlayer(argNum++, foreigner))
		return SFN_ERROR_TYPE_ARGS;

    if(foreigner < 0 || foreigner >= k_MAX_PLAYERS)
		return SFN_ERROR_OUT_OF_RANGE;

	m_result.m_int = Diplomat::GetDiplomat(player).GetEmbargo(foreigner);

    DPRINTF(k_DBG_SLIC, ("Slic_GetEmbargo:player %d, foreigner %d, returns %d\n",player,foreigner, m_result.m_int));

	return SFN_ERROR_OK;
}

// actually this will threaten to set an embargo

// VOID SetEmbargo(player, foreigner, state)
// Toggle player to have an embargo against foreigner using 1=on, 0=off
SFN_ERROR Slic_SetEmbargo::Call(SlicArgList *args)
{

    if(args->m_numArgs != 3)
		return SFN_ERROR_NUM_ARGS;

	sint32 argNum = 0;
	PLAYER_INDEX player;
	PLAYER_INDEX foreigner;
	sint32 state;

	if(!args->GetPlayer(argNum++, player))
		return SFN_ERROR_TYPE_ARGS;

    if(player < 0 || player >= k_MAX_PLAYERS)
		return SFN_ERROR_OUT_OF_RANGE;
	
	if(!args->GetPlayer(argNum++, foreigner))
		return SFN_ERROR_TYPE_ARGS;

    if(foreigner < 0 || foreigner >= k_MAX_PLAYERS)
		return SFN_ERROR_OUT_OF_RANGE;

    if(!args->GetInt(argNum++, state))
		return SFN_ERROR_TYPE_ARGS;

    Diplomat::GetDiplomat(player).SetEmbargo(foreigner, (state==1 ? true:false));//convert state to bool

    DPRINTF(k_DBG_SLIC, ("Slic_SetEmbargo:player %d, foreigner %d, state $d, returns %d\n",player,foreigner,state, m_result.m_int));

	return SFN_ERROR_OK;
}


// see mapgrid.h and mapanalysis
SFN_ERROR Slic_GetTotalValue::Call(SlicArgList *args)
{
    if(args->m_numArgs != 1)
		return SFN_ERROR_NUM_ARGS;

	PLAYER_INDEX player;

	if(!args->GetPlayer(0, player))
		return SFN_ERROR_TYPE_ARGS;

    if(player < 0 || player >= k_MAX_PLAYERS)
		return SFN_ERROR_OUT_OF_RANGE;

	m_result.m_int = MapAnalysis::GetMapAnalysis().TotalValue(player);
	return SFN_ERROR_OK;
}

// INT GetNewProposalResult(sender,receiver,player,type)
// type: 0 = science, 1 = gold, 2 = production, 3 = regard

SFN_ERROR Slic_GetNewProposalResult::Call(SlicArgList *args)
{
    if(args->m_numArgs != 4)
		return SFN_ERROR_NUM_ARGS;

	sint32 argNum = 0;
    PLAYER_INDEX sender;
    PLAYER_INDEX receiver;
	PLAYER_INDEX player;	
	sint32 type;

	if(!args->GetPlayer(argNum++, sender))
		return SFN_ERROR_TYPE_ARGS;

    if(sender < 0 || sender >= k_MAX_PLAYERS)
		return SFN_ERROR_OUT_OF_RANGE;

    if(!args->GetPlayer(argNum++, receiver))
		return SFN_ERROR_TYPE_ARGS;

    if(receiver < 0 || receiver >= k_MAX_PLAYERS)
		return SFN_ERROR_OUT_OF_RANGE;

    if(!args->GetPlayer(argNum++, player))
		return SFN_ERROR_TYPE_ARGS;

    if(player < 0 || player >= k_MAX_PLAYERS)
		return SFN_ERROR_OUT_OF_RANGE;

    if(!args->GetInt(argNum++, type))
		return SFN_ERROR_TYPE_ARGS;
	
    NewProposal proposal=Diplomat::GetDiplomat(sender).GetMyLastNewProposal(receiver);

    DiplomacyResult sender_result;
	DiplomacyResult receiver_result;
	ProposalAnalysis::ComputeResult(proposal,sender_result, receiver_result);

    if (player==sender){
	    switch(type){
	        case 0:m_result.m_int = sender_result.science;
               return SFN_ERROR_OK;
	        case 1:m_result.m_int = sender_result.gold;
               return SFN_ERROR_OK;
            case 2:m_result.m_int = sender_result.production;
               return SFN_ERROR_OK;
            case 3:m_result.m_int = sender_result.regard;
               return SFN_ERROR_OK;
		    default:m_result.m_int = 0;
		}
	}

    if (player==receiver){
        switch(type){
	        case 0:m_result.m_int = receiver_result.science;
                return SFN_ERROR_OK;
	        case 1:m_result.m_int = receiver_result.gold;
                return SFN_ERROR_OK;
            case 2:m_result.m_int = receiver_result.production;
                return SFN_ERROR_OK;
            case 3:m_result.m_int = receiver_result.regard;
                return SFN_ERROR_OK;
		    default:m_result.m_int = 0;
		}
    }

	return SFN_ERROR_OK;
}

// INT GetCounterProposalResult(sender,receiver,player,type);  // like GetNewProposalResult	 
// type: 0 = science, 1 = gold, 2 = production, 3 = regard

SFN_ERROR Slic_GetCounterProposalResult::Call(SlicArgList *args)
{
    if(args->m_numArgs != 4)
		return SFN_ERROR_NUM_ARGS;

	sint32 argNum = 0;
    PLAYER_INDEX sender;
    PLAYER_INDEX receiver;
	PLAYER_INDEX player;	
	sint32 type;

	if(!args->GetPlayer(argNum++, sender))
		return SFN_ERROR_TYPE_ARGS;

    if(sender < 0 || sender >= k_MAX_PLAYERS)
		return SFN_ERROR_OUT_OF_RANGE;

    if(!args->GetPlayer(argNum++, receiver))
		return SFN_ERROR_TYPE_ARGS;

    if(receiver < 0 || receiver >= k_MAX_PLAYERS)
		return SFN_ERROR_OUT_OF_RANGE;

    if(!args->GetPlayer(argNum++, player))
		return SFN_ERROR_TYPE_ARGS;

    if(player < 0 || player >= k_MAX_PLAYERS)
		return SFN_ERROR_OUT_OF_RANGE;

    if(!args->GetInt(argNum++, type))
		return SFN_ERROR_TYPE_ARGS;

    Response response=Diplomat::GetDiplomat(sender).GetMyLastResponse(receiver);

    DiplomacyResult sender_result;
	DiplomacyResult receiver_result;
	ProposalAnalysis::ComputeResult(response,sender_result, receiver_result);

    if (player==sender){
	    switch(type){
	        case 0:m_result.m_int = sender_result.science;
               return SFN_ERROR_OK;
	        case 1:m_result.m_int = sender_result.gold;
               return SFN_ERROR_OK;
            case 2:m_result.m_int = sender_result.production;
               return SFN_ERROR_OK;
            case 3:m_result.m_int = sender_result.regard;
               return SFN_ERROR_OK;
		    default:m_result.m_int = 0;
		}
	}

    if (player==receiver){
        switch(type){
	        case 0:m_result.m_int = receiver_result.science;
                return SFN_ERROR_OK;
	        case 1:m_result.m_int = receiver_result.gold;
                return SFN_ERROR_OK;
            case 2:m_result.m_int = receiver_result.production;
                return SFN_ERROR_OK;
            case 3:m_result.m_int = receiver_result.regard;
                return SFN_ERROR_OK;
		    default:m_result.m_int = 0;
		}
    }
	
	return SFN_ERROR_OK;
}

//INT GetMostAtRiskCity(player, foreigner, &cityvar) ? TBD

SFN_ERROR Slic_GetMostAtRiskCity::Call(SlicArgList *args)
{
    
    m_result.m_int = 0;
	return SFN_ERROR_OK;
}

// VOID DeclareWar(player, foreigner); 

SFN_ERROR Slic_DeclareWar::Call(SlicArgList *args)
{
    if(args->m_numArgs != 2)
		return SFN_ERROR_NUM_ARGS;

	sint32 argNum = 0;
	PLAYER_INDEX player;
	PLAYER_INDEX foreigner;

	if(!args->GetPlayer(argNum++, player))
		return SFN_ERROR_TYPE_ARGS;

    if(player < 0 || player >= k_MAX_PLAYERS)
		return SFN_ERROR_OUT_OF_RANGE;
	
	if(!args->GetPlayer(argNum++, foreigner))
		return SFN_ERROR_TYPE_ARGS;

    if(foreigner < 0 || foreigner >= k_MAX_PLAYERS)
		return SFN_ERROR_OUT_OF_RANGE;

    Diplomat::GetDiplomat(player).DeclareWar(foreigner);

    DPRINTF(k_DBG_SLIC, ("Slic_DeclareWar:player %d, foreigner %d\n",player,foreigner));

	return SFN_ERROR_OK;
}
