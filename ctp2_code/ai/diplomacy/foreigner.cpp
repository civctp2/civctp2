//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source file
// Description  : Tribe relations handling
// Id           : $Id$
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
// - Marked MS version specific code.
// - Standardised <list> import.
// - Initialized local variables. (Sep 9th 2005 Martin Gühmann)
//
//----------------------------------------------------------------------------

#include "c3.h"

#include <functional>
#include <list>

#include "Foreigner.h"

#include "player.h"
#include "Army.h"
#include "ArmyData.h"
#include "World.h"
#include "newturncount.h"

#include "SlicObject.h"
#include "SlicEngine.h"
#include "stringutils.h"
#include "StrDB.h"

#include "civarchive.h"
#include "GSLogs.h"

extern sint32 g_saveFileVersion;

Foreigner::Foreigner() {

	Initialize();
}

void Foreigner::Initialize()
{
	for (sint32 type = 0; type < (sint32) REGARD_EVENT_ALL; type++)
		m_regard[type] = NEUTRAL_REGARD;

	m_regardTotal = NEUTRAL_REGARD;
	m_bestRegardExplain = -1;
	m_effectiveRegardModifier = 0;
	m_trustworthiness = NEUTRAL_REGARD;
	m_hasInitiative = false;
	m_lastIncursion = -1;
	m_negotiationEvents.clear();
	m_hotwarAttackedMe = -1;
	m_coldwarAttackedMe = -1;
	m_greetingTurn = -1;
	m_embargo = false;
}

void Foreigner::Load(CivArchive & archive)
{
	RegardEventList::iterator event_iter;
	sint16 i;
	sint16 size;
	sint16 buf_size;
	sint8 val;
	RegardEvent event;
	uint8 name_str [1024];

	archive >> m_trustworthiness;
	archive >> val;
	m_hasInitiative = (val?true:false);
	archive >> m_lastIncursion;

	for (sint32 type = 0; type < (sint32) REGARD_EVENT_ALL; type++)
	{
		m_regardEventList[type].clear();

		archive >> size;

		for (i = 0; i < size; i++)
		{

			if (g_saveFileVersion >= 57)
			{
				archive >> event.regard;
				archive >> event.turn;
				archive >> event.duration;

				archive >> buf_size;
				if (buf_size > 0)
				{
					Assert(buf_size < 256);
					archive.Load((uint8 *) &name_str[0], buf_size);
					g_theStringDB->GetStringID((char*)name_str, event.explainStrId);
				}
				else
				{
					event.explainStrId = -1;
				}
			}
			else
			{

				archive.Load((uint8 *) &event, sizeof(RegardEvent));
				event.explainStrId = -1;
			}
			m_regardEventList[type].push_back(event);
		}

	}

	archive >> size;
	NegotiationEvent negotiation;
	m_negotiationEvents.clear();
	for (i = 0; i < size; i++)
	{

		if (g_saveFileVersion >= 60)
		{
			archive.Load((uint8 *) &negotiation, sizeof(NegotiationEvent));
		}
		else
		{

			OldNegotiationEvent oldnegotiation;
			archive.Load((uint8 *) &oldnegotiation, sizeof(OldNegotiationEvent));
			negotiation.proposal = oldnegotiation.proposal;
			negotiation.response = oldnegotiation.response;
			negotiation.agreement = oldnegotiation.agreement;
			negotiation.threat = oldnegotiation.threat;
			negotiation.round = 0;
		}
		m_negotiationEvents.push_back(negotiation);
	}

	archive >> m_hotwarAttackedMe;
	archive >> m_coldwarAttackedMe;
	archive >> m_greetingTurn;

	if (g_saveFileVersion >= 51)
	{
		archive >> val;
		m_embargo = (val?true:false);
	}
	else
		m_embargo = false;
}

void Foreigner::Save(CivArchive & archive) const
{
	RegardEventList::const_iterator event_iter;
	const MBCHAR *name_str;

	archive << m_trustworthiness;
	archive << (sint8)(m_hasInitiative?1:0);
	archive << m_lastIncursion;

	for (sint32 type = 0; type < (sint32) REGARD_EVENT_ALL; type++) {

		archive << (sint16) m_regardEventList[type].size();

		for (event_iter = m_regardEventList[type].begin();
			 event_iter != m_regardEventList[type].end();
			 event_iter++)
		{
			archive << (ai::Regard) event_iter->regard;
			archive << (sint16) event_iter->turn;
			archive << (sint16) event_iter->duration;
			if (event_iter->explainStrId != -1)
			{
				name_str = g_theStringDB->GetIdStr(event_iter->explainStrId);
				archive << (sint16) (strlen(name_str)+1);
				archive.Store((uint8*)name_str, strlen(name_str)+1);
			}
			else
			{

				archive << (sint16) 0;
			}


		}

	}

	archive << (sint16) m_negotiationEvents.size();
	NegotiationEventList::const_iterator negotiation_iter;
	for (negotiation_iter = m_negotiationEvents.begin();
		 negotiation_iter != m_negotiationEvents.end();
		 negotiation_iter++)
	{
		archive.Store((uint8 *) &(*negotiation_iter), sizeof(NegotiationEvent));
	}

	archive << m_hotwarAttackedMe;
	archive << m_coldwarAttackedMe;
	archive << m_greetingTurn;
	archive << (sint8)(m_embargo?1:0);
}

void Foreigner::BeginTurn()
{

    m_goldFromTrade = 0;
	m_goldFromTribute = 0;
}






const ai::Regard & Foreigner::GetEffectiveRegard() const {
	static ai::Regard effectiveRegard;
	if ((m_regardTotal + m_effectiveRegardModifier) > MAX_REGARD)
		effectiveRegard = MAX_REGARD;
	else if ((m_regardTotal + m_effectiveRegardModifier) < MIN_REGARD)
		effectiveRegard = MIN_REGARD;
	else
		effectiveRegard = (m_regardTotal + m_effectiveRegardModifier);
	return effectiveRegard;
}

const ai::Regard & Foreigner::GetPublicRegard(const REGARD_EVENT_TYPE &type) const {
	if (type == REGARD_EVENT_ALL)
		return m_regardTotal;
	else
		return m_regard[type];
}

const ai::Regard & Foreigner::GetTrust() const {
	return m_trustworthiness;
}

void Foreigner::SetTrust(const ai::Regard &trust) {
	m_trustworthiness = trust;
}






void Foreigner::LogRegardEvent(const REGARD_EVENT_TYPE & type,
							   const RegardEvent & regardEvent ) {
	m_regardEventList[type].push_front(regardEvent);
}


void Foreigner::RecomputeRegard(const DiplomacyRecord & diplomacy,
								const sint32 & decayRound,
								const ai::Regard baseRegard) {

	ai::Regard max_regard_delta(0);
	RegardEventList::iterator event_iter;

	m_regardTotal = baseRegard;
	const DiplomacyRecord::RegardDecay *regard_decay = NULL;

	for (sint32 type = 0; type < (sint32) REGARD_EVENT_ALL; type++) {

		m_regard[type] = NEUTRAL_REGARD;

		switch (type)
		{
		case REGARD_EVENT_SCENARIO:
			diplomacy.GetScenarioEvent(regard_decay);
			break;
		case REGARD_EVENT_MILITARY_POWER:
			diplomacy.GetMilitaryPowerEvent(regard_decay);
			break;
		case REGARD_EVENT_MILITARY_SAFETY:
			diplomacy.GetMilitarySafetyEvent(regard_decay);
			break;
		case REGARD_EVENT_DIPLOMACY:
			diplomacy.GetDiplomacyEvent(regard_decay);
			break;
		case REGARD_EVENT_GOLD:
			diplomacy.GetGoldEvent(regard_decay);
			break;
		case REGARD_EVENT_KNOWLEDGE:
			diplomacy.GetKnowledgeEvent(regard_decay);
			break;
		case REGARD_EVENT_PRODUCTION:
			diplomacy.GetProductionEvent(regard_decay);
			break;
		default:

			Assert(0);
		}

		event_iter = m_regardEventList[type].begin();
		while (event_iter != m_regardEventList[type].end())
		{





			if (event_iter->turn < decayRound)
			{

				if (event_iter->regard == 0 ) {
					event_iter = m_regardEventList[type].erase(event_iter);
					continue;
				}

				if (event_iter->duration <= 0)
				{
					if (event_iter->regard > 0)
						event_iter->regard =
							sint16(event_iter->regard*regard_decay->GetPositiveDecay());

					else
						event_iter->regard =
							sint16(event_iter->regard*regard_decay->GetNegativeDecay());
				}

				else if ((sint32) event_iter->turn + (sint32) event_iter->duration < decayRound) {
					event_iter = m_regardEventList[type].erase(event_iter);
					continue;
				}

			}







			if (m_regard[type] + event_iter->regard > (NEUTRAL_REGARD - MAX_REGARD) &&
				m_regard[type] + event_iter->regard < (NEUTRAL_REGARD + MAX_REGARD))
			{
				m_regardTotal += event_iter->regard;
				m_regard[type] += event_iter->regard;
			}
			else
			{
				DPRINTF(k_DBG_DIPLOMACY, ("Event type [%s] saturated at %d regard.\n",
					s_regardEventNames[type].c_str(), m_regard[type]));

			}

			if (abs(event_iter->regard) > max_regard_delta) {

				m_bestRegardExplain = event_iter->explainStrId;
				max_regard_delta = abs(event_iter->regard);
			}
			event_iter++;
		}




        m_regardEventList[type].sort(std::greater<RegardEvent>());

	}

}

const StringId & Foreigner::GetBestRegardExplain(const REGARD_EVENT_TYPE &type) const {
	if (type == REGARD_EVENT_ALL)
		return m_bestRegardExplain;
	else
		return m_regardEventList[type].begin()->explainStrId;
}






void Foreigner::ConsiderResponse( const Response & response ) {


	if (m_myLastResponse.priority < response.priority) {
		m_myLastResponse = response;
	}
}

const Response & Foreigner::GetMyLastResponse() const {
	return m_myLastResponse;
}

void Foreigner::SetMyLastResponse(const Response & response) {
	m_myLastResponse = response;
}







void Foreigner::AddTradeValue(const sint32 value)
{

	m_goldFromTrade += value;
}

sint32 Foreigner::GetTradeFrom() const {
	return m_goldFromTrade;
}

sint32 Foreigner::GetTributeFrom() const {
	return m_goldFromTribute;
}






void Foreigner::ConsiderNewProposal(const NewProposal & newProposal ) {

	if (m_myLastNewProposal.priority < newProposal.priority) {
		m_myLastNewProposal = newProposal;
	}
}

const NewProposal & Foreigner::GetMyLastNewProposal() const {
	return m_myLastNewProposal;
}






void Foreigner::SetMyLastNewProposal(const NewProposal & newProposal) {
	m_myLastNewProposal = newProposal;
}

NegotiationEventList::const_iterator Foreigner::GetNegotiationEventIndex( const NewProposal & newProposal ) const
{
	NegotiationEventList::const_iterator iter = m_negotiationEvents.begin();
	sint32 count = 0;
	while ( iter != m_negotiationEvents.end() )
	{
		if (( iter->proposal.senderId ==
			   newProposal.senderId) &&
			( iter->proposal.detail.first_type ==
			newProposal.detail.first_type ) &&

			( iter->proposal.detail.second_type ==
			newProposal.detail.second_type ) )

		{
			return iter;
		}
		iter++;
		count++;
	}

	return iter;
}

bool Foreigner::GetNewProposalTimeout(const NewProposal & newProposal,
						   const sint16 timeout_period) const
{
	NegotiationEventList::const_iterator iter =
		GetNegotiationEventIndex(newProposal);

	if (iter == m_negotiationEvents.end())
		return false;

	if (iter->round + timeout_period < NewTurnCount::GetCurrentRound())
		return false;

	return true;
}


void Foreigner::AddNewNegotiationEvent(const NegotiationEvent & event)
{

	m_negotiationEvents.push_front(event);

	if (m_negotiationEvents.size() > 3)
		m_negotiationEvents.pop_back();
}

const NegotiationEventList & Foreigner::GetNegotiationEvents() const
{
	return m_negotiationEvents;
}






void Foreigner::SetLastIncursion(const sint32 last_incursion)
{
	m_lastIncursion = last_incursion;
}

sint32 Foreigner::GetLastIncursion() const
{
	return m_lastIncursion;
}

void Foreigner::SetHotwarAttack(const sint16 last_hot_war_attack)
{
	m_hotwarAttackedMe = last_hot_war_attack;
}

sint16 Foreigner::GetLastHotwarAttack() const
{
	if (m_hotwarAttackedMe == -1)
		return 9999;

	return NewTurnCount::GetCurrentRound() - m_hotwarAttackedMe;
}

void Foreigner::SetColdwarAttack(const sint16 last_cold_war_attack)
{
	m_coldwarAttackedMe = last_cold_war_attack;
}

sint16 Foreigner::GetLastColdwarAttack() const
{
	if (m_coldwarAttackedMe == -1)
		return 9999;

	return NewTurnCount::GetCurrentRound() - m_coldwarAttackedMe;
}

void Foreigner::SetGreetingTurn()
{
	m_greetingTurn = (sint16) NewTurnCount::GetCurrentRound();
}

sint16 Foreigner::GetTurnsSinceGreeting() const
{
	if (m_greetingTurn == -1)
		return -1;

	return NewTurnCount::GetCurrentRound() - m_greetingTurn;
}

void Foreigner::DebugStatus() const
{
	RegardEventList::const_iterator event_iter;

	for (sint32 type = 0; type < (sint32) REGARD_EVENT_ALL; type++)
	{
		DPRINTF(k_DBG_DIPLOMACY, ("Type = %s:\n",
			s_regardEventNames[type].c_str()));

		event_iter = m_regardEventList[type].begin();
		while (event_iter != m_regardEventList[type].end())
		{
			DPRINTF(k_DBG_DIPLOMACY, (" delta = %d [%s]\n",
				event_iter->regard,
				g_theStringDB->GetNameStr(event_iter->explainStrId) ));

			event_iter++;
		}
	}
}

void Foreigner::LogDebugStatus(const DiplomacyRecord & diplomacy) const
{
	RegardEventList::const_iterator event_iter;
	double decay;
	sint32 round;
	const DiplomacyRecord::RegardDecay *regard_decay = NULL;

	gslog_dipprint("     delta  : rnds/decay : regard change reason (type) \n");
	for (sint32 type = 0; type < (sint32) REGARD_EVENT_ALL; type++)
	{
		switch (type)
		{
		case REGARD_EVENT_SCENARIO:
			diplomacy.GetScenarioEvent(regard_decay);
			break;
		case REGARD_EVENT_MILITARY_POWER:
			diplomacy.GetMilitaryPowerEvent(regard_decay);
			break;
		case REGARD_EVENT_MILITARY_SAFETY:
			diplomacy.GetMilitarySafetyEvent(regard_decay);
			break;
		case REGARD_EVENT_DIPLOMACY:
			diplomacy.GetDiplomacyEvent(regard_decay);
			break;
		case REGARD_EVENT_GOLD:
			diplomacy.GetGoldEvent(regard_decay);
			break;
		case REGARD_EVENT_KNOWLEDGE:
			diplomacy.GetKnowledgeEvent(regard_decay);
			break;
		case REGARD_EVENT_PRODUCTION:
			diplomacy.GetProductionEvent(regard_decay);
			break;
		default:

			Assert(0);
		}

		event_iter = m_regardEventList[type].begin();
		while (event_iter != m_regardEventList[type].end())
		{

			if (event_iter->duration < 0)
			{
				if (event_iter->regard < 0)
					decay = regard_decay->GetNegativeDecay();
				else
					decay= regard_decay->GetPositiveDecay();

				gslog_dipprint("    %5d   :    %2.2f    : [%s] (%s)\n",
					event_iter->regard,
					decay,
					event_iter->explainStrId<0?"":g_theStringDB->GetNameStr(event_iter->explainStrId),
					s_regardEventNames[type].c_str());
			}
			else
			{
				round = (event_iter->turn - NewTurnCount::GetCurrentRound())
					+ (sint32) event_iter->duration;

				gslog_dipprint("    %5d   :   %5d    : [%s] (%s)\n",
					event_iter->regard,
					round,
					event_iter->explainStrId<0?"":g_theStringDB->GetNameStr(event_iter->explainStrId),
					s_regardEventNames[type].c_str());
			}
			event_iter++;
		}
	}
}

bool Foreigner::GetEmbargo() const
{
	return m_embargo;
}

void Foreigner::SetEmbargo(const bool embargo)
{
	m_embargo = embargo;
}
