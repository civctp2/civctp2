//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ header
// Description  : Tribe relations handling
// Id           : $Id:$
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
// - Added HotSeat and PBEM human-human diplomacy support. (17-Oct-2007 Martin Gühmann)
//
//----------------------------------------------------------------------------

#ifdef HAVE_PRAGMA_ONCE
#pragma once
#endif
#ifndef __FOREIGNER_H__
#define __FOREIGNER_H__


#include <list>
#include <deque>

struct  RegardEvent;
class   Foreigner;

#include "CivArchive.h"
#include "ctp2_inttypes.h"
#include "dbtypes.h"
#include "diplomattypes.h"
#include "DiplomacyRecord.h"
#include "MapPoint.h"





struct RegardEvent {
	RegardEvent() : turn(-1), explainStrId(-1), duration(-1) {}
	RegardEvent(const ai::Regard r, 
		const sint16 t,
		const StringId e,
		const sint16 d) : regard(r), turn(t), explainStrId(e), duration(d) {};
	bool operator>(const RegardEvent & a) const {
		return ((regard > a.regard) && (turn > a.turn));
	}
	ai::Regard regard;		
	sint16 turn;		
	StringId explainStrId;
	sint16 duration;    
};

typedef std::list<RegardEvent> RegardEventList;
typedef std::deque<NegotiationEvent> NegotiationEventList;
typedef std::list<ai::Agreement> AgreementList;
typedef AgreementList::iterator AgreementListIter;

class Foreigner 
{
public:
	Foreigner();

	void Initialize();

	
	void Load(CivArchive & archive);

	
	void Save(CivArchive & archive) const;

	
	void BeginTurn();

	
	
	

	
	ai::Regard GetEffectiveRegard() const;

	
	const ai::Regard & GetPublicRegard(const REGARD_EVENT_TYPE &type = REGARD_EVENT_ALL ) const;

	
	const ai::Regard & GetTrust() const;

	
	void SetTrust(const ai::Regard &trust);

	
	
	

	
	void LogRegardEvent( const REGARD_EVENT_TYPE & type, 
		const RegardEvent & regardEvent );
		
	
	
	void RecomputeRegard(const DiplomacyRecord & diplomacy,
						 const sint32 & decayRound,
						 const ai::Regard baseRegard);

	
	const StringId & GetBestRegardExplain(const REGARD_EVENT_TYPE &type = REGARD_EVENT_ALL) const;

	
	
	

	
	void ConsiderResponse( const Response & response );

	
	const Response & GetMyLastResponse() const;

	
	void SetMyLastResponse(const Response & response);

	
	
	

	
	void AddTradeValue(const sint32 value);

	
	sint32 GetTradeFrom() const;

	
	sint32 GetTributeFrom() const;

	
	
	

	
	void ConsiderNewProposal(const NewProposal & newProposal );

	
	const NewProposal & GetMyLastNewProposal() const;

	
	void SetMyLastNewProposal(const NewProposal & newProposal);

	

	
	
	

	
	void SetMyLastNegotiatedProposal( const ProposalData & data, const RESPONSE_TYPE & response )
		{ 
			m_lastNegotiatedProposal = data;
			m_lastNegotiatedProposalResult = response;
		}
	
	
	RESPONSE_TYPE GetMyLastNegotiatedProposal( ProposalData & data ) const 
		{ 
			data = m_lastNegotiatedProposal;
			return m_lastNegotiatedProposalResult;
		}

	
	NegotiationEventList::const_iterator GetNegotiationEventIndex( const NewProposal & newProposal ) const;

	
	bool GetNewProposalTimeout(const NewProposal & newProposal, 
							   const sint16 timeout_period) const;

	
	void AddNewNegotiationEvent(const NegotiationEvent & dip);

	
	const NegotiationEventList & GetNegotiationEvents() const;

	
	
	

	
	void SetLastIncursion(const sint32 last_incursion);

	
	sint32 GetLastIncursion() const;

	
	void SetHotwarAttack(const sint16 last_hot_war_attack);

	
	sint32 GetLastHotwarAttack() const;

	
	void SetColdwarAttack(const sint16 last_cold_war_attack);

	
	sint32 GetLastColdwarAttack() const;

	
	void SetGreetingTurn();

	
	sint32 GetTurnsSinceGreeting() const;

	
	bool GetEmbargo() const;

	
	void SetEmbargo(const bool state);

	
	void DebugStatus() const;

	
	void LogDebugStatus(const DiplomacyRecord & diplomacy) const;

	void ExecuteDelayedNegotiations();

public:    
    bool m_hasInitiative;

private:
	ai::Regard m_regard[REGARD_EVENT_ALL];  
	ai::Regard m_regardTotal;			  
	StringId m_bestRegardExplain;	  
	ai::Regard m_effectiveRegardModifier; 
	ai::Regard m_trustworthiness;		  

	
	RegardEventList m_regardEventList[REGARD_EVENT_ALL];

	
	NewProposal m_myLastNewProposal;

	
	Response m_myLastResponse;

	
	sint32 m_goldFromTrade;
	sint32 m_goldFromTribute;

	
	sint32 m_lastIncursion;

	
	RESPONSE_TYPE m_lastNegotiatedProposalResult;
	ProposalData m_lastNegotiatedProposal;

	NegotiationEventList m_negotiationEvents;

	sint16 m_hotwarAttackedMe;  
	sint16 m_coldwarAttackedMe; 

	sint16 m_greetingTurn;		
	bool m_embargo;				

};

#endif // __FOREIGNER_H__
