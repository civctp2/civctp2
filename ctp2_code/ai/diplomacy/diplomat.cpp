//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : Diplomacy
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
// _DEBUG
// - Generates debug information when set.
//
// _BFR_
// - Force CD checking when set (build final release).
// - If not defined it anabled some more loogs.
//
//----------------------------------------------------------------------------
//
// Modifications from the original Activision code:
//
// - Minor modification of the Diplomat::StartNegotiations function 
//   by Peter Triggs to enable some minor AI-AI-Diplomacy.
// - Corrected bug in list insertion.
// - Corrected non-standard syntax and some compiler warnings.
// - Prevented invalid strategies to be merged in. 
// - Prevented crash on number of strategies wrap-around to negative. 
// - Add an isStealth parameter in CharacterizeArmy method - Calvitix
// - Made Cleanup really clean up.
// - Prevented crash on incorrect input (0 foreigners).
// - Prevented crash on incorrect input (personality typo).
// - Improved CleanupAll.
// - Some agreements have limited duration, PFT 05 MAR 05
// - Replaced old civilisation database by new civilisation database. (Aug 20th 2005 Martin Gühmann)
// - Initialized local variables. (Sep 9th 2005 Martin Gühmann)
//
//----------------------------------------------------------------------------

#include "c3.h"


#include "c3errors.h"
#include "c3math.h"

#include "MoveFlags.h"

#include "MapPoint.h"
#include "player.h"
#include "Events.h"
#include "GameEventUser.h"
#include "Unit.h"
#include "StrDB.h"
#include "Agreement.h"
#include "Army.h"
#include "ArmyData.h"
#include "dynarr.h"
#include "ArmyPool.h"
#include "World.h"
#include "Cell.h"
#include "Gold.h"
#include "AdvanceListRecord.h"
#include "AdvanceRecord.h"
#include "UnitData.h"
#include "UnitPool.h"
#include "newturncount.h"
#include "ProposalAnalysis.h"
#include "PersonalityRecord.h"
#include "DiplomacyRecord.h"
#include "mapanalysis.h"
#include "stringutils.h"
#include "EndGameObjectRecord.h"
#include "GameOver.h"
#include "UnitRecord.h"
#include "Strengths.h"
#include "DiplomacyProposalRecord.h"
#include "pollution.h"
#include "TradePool.h"
#include "TradeRouteData.h"
#include "wonderutil.h"

#include "unitutil.h"
#include "director.h"
#include "ctpai.h"
#include "GSLogs.h"
#include "profileDB.h"
#include "AICause.h"


#include "CivilisationRecord.h"
#include "Civilisation.h"

extern sint32 g_saveFileVersion;


#include "TurnCnt.h"
extern TurnCount *g_turn;


#include <algorithm>
using namespace std;

#include "Diplomat.h"
#include "AgreementMatrix.h"

#include "SlicObject.h"
#include "SlicEngine.h"

#include "dipwizard.h"
#include "network.h"
#include "net_action.h"
#include "SelItem.h"
#include "net_action.h"

#include "Path.h"
#include "ctpagent.h"
#include "OrderRecord.h"
#include "gfx_options.h"

#include "net_diplomacy.h"

#include "RandGen.h"
extern	RandomGenerator *g_rand;

using namespace ai;





AiState Diplomat::s_badAiState;
Threat Diplomat::s_badThreat;
Response Diplomat::s_badResponse;
NewProposal Diplomat::s_badNewProposal;
Motivation Diplomat::s_badMotivation;
ai::Agreement Diplomat::s_badAgreement;

sint32 Diplomat::s_nextId = 0;
Diplomat::DiplomatVector Diplomat::s_theDiplomats;
sint32 Diplomat::s_proposalTypeToElemIndex[PROPOSAL_MAX];





#define RELDBG(x) { FILE *f = fopen("reldbg.txt", "a"); fprintf x; fclose(f); }
Diplomat & Diplomat::GetDiplomat(const PLAYER_INDEX & playerId) { 
	Assert(playerId >= 0);
	Assert((unsigned) playerId < s_theDiplomats.size());
	Assert(playerId == s_theDiplomats[playerId].GetPlayerId());

	return s_theDiplomats[playerId]; 
}


void Diplomat::ResizeAll(const PLAYER_INDEX & newMaxPlayers) 
{
	sint32 old_size = s_theDiplomats.size();
	s_theDiplomats.resize(newMaxPlayers);

	
	sint32 foreignerId;
	for (sint32 playerId = 0; playerId < newMaxPlayers; playerId++)
		{
			s_theDiplomats[playerId].SetPlayerId(playerId);
			s_theDiplomats[playerId].Resize(newMaxPlayers);

			
			if (playerId < old_size)
			{
				
				for (foreignerId=0; foreignerId < newMaxPlayers; foreignerId++)
				{
					if (foreignerId >= old_size || g_player[foreignerId] == NULL)
					{
						s_theDiplomats[playerId].InitForeigner(foreignerId);
					}
				}
			}
			else
			{
				
				s_theDiplomats[playerId].Initialize();
			}
		}
}




void Diplomat::CleanupAll()
{
	for (uint32 i = 0; i < s_theDiplomats.size(); i++)
	{
		
		s_theDiplomats[i].Cleanup();
	}

	s_theDiplomats.clear();
}


void Diplomat::InitializeAll() 
{
	
	for (uint32 i = 0; i < s_theDiplomats.size(); i++)
	{
		s_theDiplomats[i].Initialize();
	}
}


void Diplomat::LoadAll(CivArchive & archive)
{
	archive >> s_nextId;

	
	AgreementMatrix::s_agreements.Load(archive);

	for (uint32 i = 0; i < s_theDiplomats.size(); i++)
	{
		
		s_theDiplomats[i].Load(archive);
	}
}


void Diplomat::SaveAll(CivArchive & archive)
{
	archive << s_nextId;

	
	AgreementMatrix::s_agreements.Save(archive);

	for (uint32 i = 0; i < s_theDiplomats.size(); i++)
		{
			
			s_theDiplomats[i].Save(archive);
		}
}

void Diplomat::DebugStatusAll() {
	
	for (uint32 playerId = 1; playerId < s_theDiplomats.size(); playerId++) {

		
		if (!g_player[playerId] || g_player[playerId]->IsDead())
			continue;

		Diplomat & diplomat = GetDiplomat(playerId);

		printf("Player %d: ",diplomat.GetPlayerId());

		if (g_player[diplomat.GetPlayerId()]->m_playerType == PLAYER_TYPE_ROBOT) 
			printf("(AI)");
		printf("\n");

		diplomat.DebugStatus();
	}
}


void Diplomat::AddDiplomacyArgToSlicContext(SlicContext & sc, const DiplomacyArg & dip_arg)
{
	if (dip_arg.playerId != -1)
		sc.AddPlayer(dip_arg.playerId);
	else if (dip_arg.cityId != -1)
		sc.AddCity(dip_arg.cityId);
	else if (dip_arg.armyId != -1)
		sc.AddArmy(dip_arg.armyId);
	else if (dip_arg.agreementId != -1)
		sc.AddArmy(dip_arg.armyId);
	else if (dip_arg.advanceType != -1)
		sc.AddAdvance(dip_arg.advanceType);
	else if (dip_arg.unitType != -1)
		sc.AddUnitRecord(dip_arg.unitType);
	
	
	else if (dip_arg.gold != -1)
		sc.AddGold(dip_arg.gold);
	else if (dip_arg.pollution != -1)
		sc.AddInt(dip_arg.pollution);
	else if (dip_arg.percent != -1.0)
		sc.AddInt(static_cast<sint32>(100.0 * dip_arg.percent));
	
}






sint32 Diplomat::GetNextId() {
	return s_nextId++;
}


void Diplomat::SetNextId(const sint32 & id) {
	s_nextId = id;
}


Diplomat::Diplomat() 
{
	
	m_playerId = -1;

	
	
}


void Diplomat::Resize(const PLAYER_INDEX & newMaxPlayerId)
{
	sint32 old_size = m_lastMotivation.size();

	m_lastMotivation.resize(newMaxPlayerId);
	m_diplomaticStates.resize(newMaxPlayerId);
	m_diplomacy.resize(newMaxPlayerId);
	m_foreigners.resize(newMaxPlayerId);
	m_desireWarWith.resize(newMaxPlayerId);
}


void Diplomat::Load(CivArchive & archive)
{
	uint16	count;
	size_t	i;
	char *str;
	ai::Agreement agreement;
	Threat threat;
	AiState ai_state;

	
	Initialize();

	
	archive >> count;
	str = new char [count+1];
	archive.Load((uint8 *)str, count);
	str[count] = '\0';
	SetPersonalityName(str);
	delete [] str;
	 
	archive >> count;
	for (i = 0; i < count; i++)
	{
		archive.Load((uint8 *)&ai_state, sizeof(AiState));
		m_bestStrategicStates.push_back(ai_state);
	}
	
	
	ComputeCurrentStrategy();
	
	
	archive >> count;
	for (i = 0; i < count; i++)
	{
		archive.Load((uint8 *)&threat, sizeof(Threat));
		m_threats.push_back(threat);
	}

	
	archive >> count;

	Assert(count == CtpAi::s_maxPlayers);

	if (count > CtpAi::s_maxPlayers)
	{
		Resize(count);
	}

	for (uint32 foreigner=0; foreigner < m_foreigners.size(); foreigner++)
		{
			m_lastMotivation[foreigner] = m_motivations.end();
			if (foreigner < count)
			{
				m_foreigners[foreigner].Load(archive);
				archive.Load((uint8 *)&ai_state, sizeof(AiState));
			}
			else
			{
				m_foreigners[foreigner].Initialize();
				ai_state.dbIndex = -1;
			}
			
			
			if (g_player[m_playerId] && g_player[foreigner])
			{
				ChangeDiplomacy(foreigner, ai_state.dbIndex);

				
				UpdateRegard(foreigner);
			}
		}
	archive >> m_diplomcyVictoryCompleteTurn;
	archive >> m_nuclearAttackTarget;

	if (g_saveFileVersion >= 59)
	{
		archive >> m_lastParty;
	}

	if (g_saveFileVersion >= 61)
	{
		uint8 val;
		archive >> val;
		m_launchedNukes = (val?true:false) ;
		archive >> val;
		m_launchedNanoAttack = (val?true:false) ;
	}

	
	m_desireWarWith.resize(m_foreigners.size());
	ComputeAllDesireWarWith();
}


void Diplomat::Save(CivArchive & archive) const
{
	archive << static_cast<uint16>(m_personalityName.size());
	archive.Store((uint8 *) m_personalityName.c_str(), m_personalityName.size());

	archive << static_cast<uint16>(m_bestStrategicStates.size());
	AiStateList::const_iterator ai_state_iter = m_bestStrategicStates.begin();
	while (ai_state_iter != m_bestStrategicStates.end())
	{
		archive.Store((uint8 *) &(*ai_state_iter), sizeof(AiState));
		ai_state_iter++;
	}
	
	
	archive << static_cast<uint16>(m_threats.size());
	ThreatList::const_iterator threat_iter = m_threats.begin();
	while (threat_iter != m_threats.end())
	{
		archive.Store((uint8 *) &(*threat_iter), sizeof(Threat));
		threat_iter++;
	}

	
	archive << static_cast<uint16>(m_foreigners.size());
	for (uint32 foreigner=0; foreigner < m_foreigners.size(); foreigner++)
	{
		m_foreigners[foreigner].Save(archive);

		
		archive.Store((uint8 *) &(m_diplomaticStates[foreigner]), sizeof(AiState));
	}
	archive << m_diplomcyVictoryCompleteTurn;
	archive << m_nuclearAttackTarget;
	archive << m_lastParty;

	uint8 val = (m_launchedNukes?1:0);
	archive << val;

	val = (m_launchedNanoAttack?1:0);
	archive << val;
}


void Diplomat::Cleanup()
{
	
	m_motivations.clear();

	
	for (uint32 foreigner=0; foreigner < m_lastMotivation.size(); foreigner++)
	{
		m_foreigners[foreigner].Initialize();
	}

	m_lastMotivation.resize(0);
	m_foreigners.resize(0);
	m_diplomaticStates.resize(0);
	m_diplomacy.resize(0);
	m_desireWarWith.resize(0);

	
	m_threats.clear();
	
	
	m_bestDiplomaticState = s_badAiState;
	m_personality = 0x0;
	m_nuclearAttackTarget = -1;
	m_lastParty = -1;
	m_launchedNukes = false;
	m_launchedNanoAttack = false;

	m_bestStrategicStates.clear();
	m_diplomcyVictoryCompleteTurn	= -1;
	m_enemyCount					= 0;
	m_enemyThreat					= 0;
	m_friendCount					= 0;
	m_friendPower					= 0;
	m_incursionPermission			= 1 << PLAYER_INDEX_VANDALS;
	m_outstandingProposals			= 0;
	m_personalityName.resize(0);
	m_piracyHistory.clear();
	
	ClearEffectiveRegardCache();
}


void Diplomat::Initialize()
{
	if ((m_playerId >= 0) && (m_playerId < k_MAX_PLAYERS))
	{
		Player *        player  = g_player[m_playerId];
		Civilisation *  civ     = player ? player->GetCivilisation() : NULL;
		CivilisationRecord const *
		                civRec  = civ ? civ->GetDBRec() : NULL;

		if (civRec)
		{
			SetPersonalityName(GENDER_MALE == civ->GetGender()
			                   ? civRec->GetPersonalityMale()->GetIDText()
			                   : civRec->GetPersonalityFemale()->GetIDText()
			                   );
		}
	}

	if (!m_personality)
	{
		m_personality = g_thePersonalityDB->Get(0);
	}
	
	m_motivations.clear();

	
	for (uint32 foreigner=0; foreigner < m_lastMotivation.size(); foreigner++)
	{
		InitForeigner(foreigner);
	}

	
	m_threats.clear();
	
	
	Assert(g_thePersonalityDB);
	m_nuclearAttackTarget = -1;
	m_lastParty = -1;
	m_launchedNukes = false;
	m_launchedNanoAttack = false;
	m_diplomcyVictoryCompleteTurn = -1;

	ClearEffectiveRegardCache();
}


void Diplomat::InitForeigner(const PLAYER_INDEX & foreigner)
{
	m_lastMotivation[foreigner] = m_motivations.end();

	
	
	
	m_foreigners[foreigner].Initialize();
	InitDiplomaticState(foreigner);
}


void Diplomat::DebugStatus(const PLAYER_INDEX & foreignerId) const
{
	
	for (uint32 tmp_foreignerId = 1; tmp_foreignerId< m_foreigners.size(); tmp_foreignerId++) {
		
		if (tmp_foreignerId == m_playerId)
			continue;

		
		if (foreignerId != -1 && tmp_foreignerId != foreignerId)
			continue;
		
		const Foreigner &foreigner = m_foreigners[tmp_foreignerId];
		
		DPRINTF(k_DBG_DIPLOMACY, ("Player %d -> Foreigner %d\n", 
			m_playerId, tmp_foreignerId));
		
		
		if (!g_player[m_playerId]->HasContactWith(tmp_foreignerId))
		{
			DPRINTF(k_DBG_DIPLOMACY, ("  No Contact\n" ));
			continue;
		}
		
		DPRINTF(k_DBG_DIPLOMACY, ("  Public Regard = %d\n",
			foreigner.GetPublicRegard()));
		DPRINTF(k_DBG_DIPLOMACY, ("  Effective Regard = %d\n",
			foreigner.GetEffectiveRegard()));
		DPRINTF(k_DBG_DIPLOMACY, ("  Trust = %d\n",
			foreigner.GetTrust()));
		
		if (foreigner.GetBestRegardExplain() >= 0)
		{
			DPRINTF(k_DBG_DIPLOMACY, ("  Explaination = %s\n",
				g_theStringDB->GetNameStr(foreigner.GetBestRegardExplain()) ));
		}
		else
			DPRINTF(k_DBG_DIPLOMACY, ("  Explaination = <NO EXPLAINATION>\n"));
		
		if (GetReceiverHasInitiative(tmp_foreignerId))
		{
			DPRINTF(k_DBG_DIPLOMACY, ("  (has initiative)\n"));
		}

		
		if (foreignerId != -1)
		{
			foreigner.DebugStatus();
		}
	}
	
	DPRINTF(k_DBG_DIPLOMACY, ("\n"));
}


void Diplomat::LogDebugStatus(const PLAYER_INDEX & foreignerId) const
{
	
	Player *player_ptr = g_player[m_playerId];

	if(!player_ptr) 
		return;

	gslog_dipprint("[Player %d] [Turn %d]\n", m_playerId, player_ptr->m_current_round);

	
	StringId explainStrId;

	
	for (int tmp_foreignerId = 1; tmp_foreignerId < k_MAX_PLAYERS; tmp_foreignerId++)
	{
		
		if (tmp_foreignerId == m_playerId)
			continue;

		if (foreignerId != -1 && tmp_foreignerId != foreignerId)
			continue;

		
		if (g_player[tmp_foreignerId] == NULL)
			continue;

		const Foreigner &foreigner = m_foreigners[tmp_foreignerId];

		gslog_dipprint("Foreigner %d:\n", tmp_foreignerId);

		gslog_dipprint("    Base Regard = %d\n", GetBaseRegard(tmp_foreignerId));
		gslog_dipprint("    Public Regard = %d\n", foreigner.GetPublicRegard());
		gslog_dipprint("    Effective Regard = %d\n", foreigner.GetEffectiveRegard());
		gslog_dipprint("    Trust = %d\n", foreigner.GetTrust());

		
		if (!g_player[m_playerId]->HasContactWith(tmp_foreignerId))
		{
			gslog_dipprint("    No Contact.\n" );
		}
		else
		{
			explainStrId = foreigner.GetBestRegardExplain();
			if (explainStrId >= 0)
			{
				gslog_dipprint("    Best Regard Explaination = [%s]\n",
					g_theStringDB->GetNameStr(explainStrId));
			}
			else
			{
				gslog_dipprint("    Best Regard Explaination = []\n");
			}
		}

		gslog_dipprint("    Regard Events:\n");

		const DiplomacyRecord & diplomacy = GetCurrentDiplomacy(tmp_foreignerId);
		
		foreigner.LogDebugStatus(diplomacy);
	}
	
	gslog_dipprint("\n");
}


void Diplomat::SetPlayerId(const PLAYER_INDEX &playerId) {
	m_playerId = playerId;
}


PLAYER_INDEX Diplomat::GetPlayerId() const {
	return m_playerId;
}


void Diplomat::SetPersonalityName(const char *personality_name) {
	m_personalityName = string(personality_name);

	
	sint32 index;
	bool found = g_thePersonalityDB->GetNamedItem(personality_name, index);
	Assert(found);
	if (found)
		m_personality = g_thePersonalityDB->Get(index);
	
}


string Diplomat::GetPersonalityName() const {
	return m_personalityName;
}


const PersonalityRecord *Diplomat::GetPersonality() const
{
	return m_personality;
}


bool Diplomat::GetReceiverHasInitiative(const PLAYER_INDEX & foreignerId) const {
	return m_foreigners[foreignerId].m_hasInitiative;
}


void Diplomat::SetReceiverHasInitiative(const PLAYER_INDEX & foreignerId, 
										const bool & hasInitiative) {
	m_foreigners[foreignerId].m_hasInitiative = hasInitiative;

	if (hasInitiative) {
		DPRINTF(k_DBG_DIPLOMACY, ("  Player %d has initiative.\n", foreignerId));
	}
	else {
		DPRINTF(k_DBG_DIPLOMACY, ("  Player %d has initiative.\n", m_playerId));
	}
}


void Diplomat::BeginTurn() 
{
	
	ClearEffectiveRegardCache();

	
	Assert(m_playerId != -1);

	
	if (m_playerId < 0 || (g_player[m_playerId] == NULL) || g_player[m_playerId]->IsDead())
	{
		return;
	}

	
	UpdateAttributes();

	
	ComputeTradeRoutePiracyRisk();

	
	RecomputeRegard();

	
	m_motivations.clear();

	
	for (uint32 foreigner=0; foreigner < m_lastMotivation.size(); foreigner++)
	{
			m_lastMotivation[foreigner] = m_motivations.end();
	}

	
	for ( ForeignerVector::iterator iter = m_foreigners.begin(); 
		  iter != m_foreigners.end(); 
		  iter++ ) {
		
		iter->SetMyLastResponse( Diplomat::s_badResponse );
		iter->SetMyLastNewProposal( Diplomat::s_badNewProposal );
	}

	
	g_gevManager->AddEvent(GEV_INSERT_Tail, GEV_ComputeMotivations,
			  GEA_Player, m_playerId,
			  GEA_End);

	
	NextStrategicState();

	
	for(uint32 foreignerId=1; foreignerId < m_foreigners.size(); foreignerId++) {
		
		if (foreignerId == m_playerId)
			continue;

		
		if (!g_player[m_playerId]->HasContactWith(foreignerId) ||
			!g_player[foreignerId] ||
			!g_player[foreignerId]->HasContactWith(m_playerId))
			continue;

		NextDiplomaticState(foreignerId);
	}

	
	ComputeIncursionPermission();

	
	ExecutePersistantAgreements();

	
	sint32 diplomacyEndgameIndex = CtpAi::GetEndgameWorldUnionIndex();
		
	Assert(diplomacyEndgameIndex);
	if (diplomacyEndgameIndex >= 0)
	{
		double population_percent;
		double land_percent;
		PLAYER_INDEX leaderId;
		double population_needed;
		const EndGameObjectRecord *rec = 
			g_theEndGameObjectDB->Get(diplomacyEndgameIndex);
		MapAnalysis::GetMapAnalysis().ComputeAllianceSize(m_playerId, leaderId, population_percent, land_percent);

		
		if (leaderId > 0 && rec->GetMinCoverage())
		{
			rec->GetMinCoverage(population_needed);
			if (leaderId == m_playerId && population_percent >= population_needed)
			{
				if (m_diplomcyVictoryCompleteTurn > 0)
				{
					if (m_diplomcyVictoryCompleteTurn <= NewTurnCount::GetCurrentRound())
						g_player[m_playerId]->GameOver(GAME_OVER_WON_DIPLOMACY, -1);
				}
				else if (rec->GetTurnsToActivate())
				{
					sint32 turns_to_activate;
					rec->GetTurnsToActivate(turns_to_activate);
					m_diplomcyVictoryCompleteTurn = static_cast<sint16>
					    (NewTurnCount::GetCurrentRound() + turns_to_activate);
				}
				else
				{
					
					m_diplomcyVictoryCompleteTurn = static_cast<sint16>(NewTurnCount::GetCurrentRound() + 1);
				}
			}
			else
			{
				m_diplomcyVictoryCompleteTurn = -1;
			}
		}

	}
}







void Diplomat::LogRegardEvent( const PLAYER_INDEX & foreignerId,
					 const sint32 & regardDelta,
					 const REGARD_EVENT_TYPE & type,
					 const StringId & explain,
					 const sint16 duration) {
	
	
	RegardEvent regardEvent(regardDelta, 
							static_cast<sint16>(NewTurnCount::GetCurrentRound()+1), explain, duration);
	m_foreigners[foreignerId].LogRegardEvent(type, regardEvent);
}


void Diplomat::LogViolationEvent(const PLAYER_INDEX foreignerId, const PROPOSAL_TYPE proposal_type)
{
	StringId strId;
	REGARD_EVENT_TYPE regard_event_type = REGARD_EVENT_DIPLOMACY;
	bool act_of_war = false;
	char * trust_message = NULL;

	
	if (foreignerId == 0)
		return;

	
	if (m_playerId == 0)
		return;

	switch (proposal_type)
	{
	
	case PROPOSAL_REQUEST_WITHDRAW_TROOPS:
		g_theStringDB->GetStringID("REGARD_EVENT_WITHDRAW_TROOPS_VIOLATION", strId);
		regard_event_type = REGARD_EVENT_DIPLOMACY;
		trust_message = "TrustLossFailToWithdraw";
		break;
	
	
	case PROPOSAL_REQUEST_STOP_PIRACY:
		SetColdwarAttack(foreignerId, (sint16) NewTurnCount::GetCurrentRound());
		
		g_theStringDB->GetStringID("REGARD_EVENT_VIOLATED_STOP_PIRACY", strId);
		regard_event_type = REGARD_EVENT_DIPLOMACY;
		trust_message = "TrustLossFailToStopPiracy";

		
		LogViolationEvent(foreignerId, PROPOSAL_TREATY_MILITARY_PACT);
		LogViolationEvent(foreignerId, PROPOSAL_TREATY_RESEARCH_PACT);
		LogViolationEvent(foreignerId, PROPOSAL_TREATY_TRADE_PACT);
		break;

	
	case PROPOSAL_REQUEST_STOP_RESEARCH:
		g_theStringDB->GetStringID("REGARD_EVENT_STOP_RESEARCH_VIOLATION", strId);
		regard_event_type = REGARD_EVENT_DIPLOMACY;
		trust_message = "TrustLossViolatedResearchPact";

		break;

	
	case PROPOSAL_REQUEST_REDUCE_POLLUTION:
		g_theStringDB->GetStringID("REGARD_EVENT_PROPOSAL_REDUCE_POLLUTION", strId);
		regard_event_type = REGARD_EVENT_DIPLOMACY;

		trust_message = "TrustLossViolatedPollutionPact";
		break;

	case PROPOSAL_TREATY_PEACE:
		g_theStringDB->GetStringID("REGARD_EVENT_UNPROVOKED_ATTACK",strId);
		regard_event_type = REGARD_EVENT_MILITARY_SAFETY;

		trust_message = "TrustLossViolatedPeaceTreaty";
		break;
		
	
	case PROPOSAL_TREATY_CEASEFIRE:
		g_theStringDB->GetStringID("REGARD_EVENT_UNPROVOKED_ATTACK",strId);
		regard_event_type = REGARD_EVENT_MILITARY_SAFETY;

		
		LogViolationEvent(foreignerId, PROPOSAL_REQUEST_WITHDRAW_TROOPS);

		
		LogViolationEvent(foreignerId, PROPOSAL_TREATY_PEACE);

		
		LogViolationEvent(foreignerId, PROPOSAL_TREATY_ALLIANCE);

		
		LogViolationEvent(foreignerId, PROPOSAL_TREATY_MILITARY_PACT);
		LogViolationEvent(foreignerId, PROPOSAL_TREATY_TRADE_PACT);
		LogViolationEvent(foreignerId, PROPOSAL_TREATY_RESEARCH_PACT);
		LogViolationEvent(foreignerId, PROPOSAL_TREATY_POLLUTION_PACT);

		SetHotwarAttack(foreignerId, (sint16) NewTurnCount::GetCurrentRound());

		trust_message = "TrustLossViolatedCeaseFire";
		act_of_war = true;
		break;

	
	case PROPOSAL_TREATY_TRADE_PACT:
		g_theStringDB->GetStringID("REGARD_EVENT_VIOLATE_TRADE_PACT", strId);
		regard_event_type = REGARD_EVENT_DIPLOMACY;

		
		LogViolationEvent(foreignerId, PROPOSAL_TREATY_ALLIANCE);

		trust_message = "TrustLossViolatedTradePact";
		break;

	
	case PROPOSAL_TREATY_MILITARY_PACT:
		g_theStringDB->GetStringID("REGARD_EVENT_VIOLATED_MILITARY_PACT", strId);
		regard_event_type = REGARD_EVENT_DIPLOMACY;

		trust_message = "TrustLossViolatedMilitaryPact";
		break;

	
	case PROPOSAL_TREATY_POLLUTION_PACT:
		g_theStringDB->GetStringID("REGARD_EVENT_VIOLATED_POLLUTION_PACT", strId);
		regard_event_type = REGARD_EVENT_DIPLOMACY;

		trust_message = "TrustLossViolatedPollutionPact";
		break;

	
	case PROPOSAL_TREATY_ALLIANCE:
		g_theStringDB->GetStringID("REGARD_EVENT_VIOLATED_ALLIANCE", strId);
		regard_event_type = REGARD_EVENT_DIPLOMACY;

		trust_message = "TrustLossViolatedAlliance";
		break;

	
	case PROPOSAL_TREATY_RESEARCH_PACT:
		g_theStringDB->GetStringID("REGARD_EVENT_VIOLATED_RESEACH_PACT", strId);
		regard_event_type = REGARD_EVENT_DIPLOMACY;

		trust_message = "TrustLossViolatedResearchPact";
		break;

	default:
		
		Assert(0);
		return;
	}

	
	if (AgreementMatrix::s_agreements.HasAgreement(m_playerId, foreignerId, proposal_type))
	{
		
		sint32 regard_cost = GetViolationRegardCost(foreignerId, proposal_type);
		
		
		sint32 trust_cost = GetViolationTrustCost(foreignerId, proposal_type);

		if (trust_message != NULL && trust_cost != 0)
		{
			SlicObject *so = new SlicObject(trust_message) ;
			so->AddCivilisation(foreignerId) ;		
			so->AddCivilisation(m_playerId) ;
			so->AddRecipient(foreignerId);
			g_slicEngine->Execute(so) ;
		}

		
		const DiplomacyProposalRecord * rec = g_theDiplomacyProposalDB->Get(s_proposalTypeToElemIndex[proposal_type]);
		const MBCHAR * reason = g_theStringDB->GetNameStr(strId);
		if (rec->GetTrustViolationGlobal())
		{
			Diplomat::ApplyGlobalTrustChange(foreignerId, trust_cost, reason);
		}
		else
		{
			ApplyTrustChange(foreignerId, trust_cost, reason);
		}
		
		LogRegardEvent( foreignerId, regard_cost, regard_event_type, strId);

		
		UpdateRegard( foreignerId );
		
		
		AgreementMatrix::s_agreements.CancelAgreement(m_playerId, foreignerId, proposal_type);
		AgreementMatrix::s_agreements.CancelAgreement(foreignerId, m_playerId, proposal_type);
	}

	
	bool war_declared = 
		AgreementMatrix::s_agreements.HasAgreement(m_playerId, foreignerId, PROPOSAL_TREATY_DECLARE_WAR);

	sint32 turnsatwar = 
		AgreementMatrix::s_agreements.TurnsAtWar(m_playerId, foreignerId);

	
	if (act_of_war)
	{
		
		ai::Agreement war_agreement = AgreementMatrix::s_agreements.GetAgreement(m_playerId, foreignerId, PROPOSAL_TREATY_DECLARE_WAR);
		if (!war_declared || (turnsatwar == 0 && war_agreement.senderId != m_playerId))
		{
			if (GetCurrentDiplomacy(foreignerId).GetPreemptiveAttackRegardCost())
			{
				sint32 regard_cost;
				GetCurrentDiplomacy(foreignerId).GetPreemptiveAttackRegardCost(regard_cost);
				
				g_theStringDB->GetStringID("REGARD_EVENT_UNPROVOKED_ATTACK",strId);
				LogRegardEvent( foreignerId,
					regard_cost,
					REGARD_EVENT_MILITARY_SAFETY,
					strId);
			}
		
			if (GetCurrentDiplomacy(foreignerId).GetPreemptiveAttackTrustCost())
			{
				sint32 trust_cost;
				GetCurrentDiplomacy(foreignerId).GetPreemptiveAttackTrustCost(trust_cost);
				Diplomat::ApplyGlobalTrustChange(foreignerId, trust_cost, "Made preemptive attack without declaring war first.");
			}
		}
	}

	
	if (act_of_war && !war_declared)
	{
		
		DeclareWar(foreignerId);
	}
}


const ai::Regard Diplomat::GetPublicRegard( const PLAYER_INDEX & foreignerId,
								  const REGARD_EVENT_TYPE &type) const {
	ai::Regard regard = m_foreigners[foreignerId].GetPublicRegard(type);

	
	if (foreignerId == 0)
		return MIN_REGARD;

	
	if (m_playerId == 0)
		return MIN_REGARD;
	
	
	if ((regard > HOTWAR_REGARD) && 
		AgreementMatrix::s_agreements.HasAgreement(m_playerId, foreignerId, PROPOSAL_TREATY_DECLARE_WAR))
	{
		regard = HOTWAR_REGARD;
	}
	
	else if ((regard <= HOTWAR_REGARD) &&
		AgreementMatrix::s_agreements.HasAgreement(m_playerId, foreignerId, PROPOSAL_TREATY_PEACE))
	{
		
		regard = HOTWAR_REGARD + 1;
	}
	
	else if ((regard <= COLDWAR_REGARD) &&
		(AgreementMatrix::s_agreements.HasAgreement(m_playerId, foreignerId, PROPOSAL_TREATY_ALLIANCE) ||
		 AgreementMatrix::s_agreements.HasAgreement(m_playerId, foreignerId, PROPOSAL_TREATY_RESEARCH_PACT) ||
		 AgreementMatrix::s_agreements.HasAgreement(m_playerId, foreignerId, PROPOSAL_TREATY_MILITARY_PACT)))
	{
		
		regard = COLDWAR_REGARD + 1;
	}
	return regard;
}



StringId Diplomat::ExplainRegard( const PLAYER_INDEX &foreignerId,
								  const REGARD_EVENT_TYPE &type) const {
	return m_foreigners[foreignerId].GetBestRegardExplain(type);
}


const ai::Regard Diplomat::GetEffectiveRegard( const PLAYER_INDEX & foreignerId) const {

	
	if (foreignerId == 0)
		return MIN_REGARD;

	
	if (m_playerId == 0)
		return MIN_REGARD;

	return m_foreigners[foreignerId].GetEffectiveRegard();
}


void Diplomat::ApplyGlobalTrustChange(const PLAYER_INDEX & foreignerId, const ai::Regard &trust_delta, const MBCHAR *reason)
{
#ifndef _BFR_
	gslog_dipprint("\nApplyGlobalTrustChange: all players receive %d trust for player %d.\n",trust_delta,foreignerId);
	if (reason != NULL)
	{
		gslog_dipprint("                        [%s].\n\n",reason);
	}
#endif

	for (uint32 i = 1; i < s_theDiplomats.size(); i++)
		{
			if (i != foreignerId)
				s_theDiplomats[i].ApplyTrustChange(foreignerId, trust_delta, NULL);
		}
}


void Diplomat::ApplyTrustChange(const PLAYER_INDEX & foreignerId, const ai::Regard &trust_delta, const MBCHAR *reason)
{
#ifndef _BFR_
	gslog_dipprint("\nApplyTrustChange: player %d receives %d trust for player %d.\n",m_playerId, trust_delta, foreignerId);
	if (reason != NULL)
	{
		gslog_dipprint("                        [%s].\n\n",reason);
	}
#endif

	ai::Regard new_trust = m_foreigners[foreignerId].GetTrust();

	
	if (new_trust + trust_delta < MAX_REGARD)
		new_trust += trust_delta;
	else
		new_trust = MAX_REGARD;

	m_foreigners[foreignerId].SetTrust(new_trust);
}


const ai::Regard Diplomat::GetTrust(const PLAYER_INDEX & foreignerId) const {

	
	if (foreignerId == 0)
		return MIN_REGARD;

	return m_foreigners[foreignerId].GetTrust();
}


void Diplomat::SetTrust(const PLAYER_INDEX & foreignerId, const ai::Regard &trust) {
	m_foreigners[foreignerId].SetTrust(trust);
}


ai::Regard Diplomat::GetBaseRegard(const PLAYER_INDEX foreignerId) const
{
	if (AgreementMatrix::s_agreements.HasAgreement(m_playerId, foreignerId, PROPOSAL_TREATY_ALLIANCE))
	{
		return ALLIED_REGARD;
	}
	else if (AgreementMatrix::s_agreements.HasAgreement(m_playerId, foreignerId, PROPOSAL_TREATY_MILITARY_PACT) ||
		AgreementMatrix::s_agreements.HasAgreement(m_playerId, foreignerId, PROPOSAL_TREATY_RESEARCH_PACT) ||
		AgreementMatrix::s_agreements.HasAgreement(m_playerId, foreignerId, PROPOSAL_TREATY_TRADE_PACT))
	{
		return FRIEND_REGARD;
	}
	else if (!AgreementMatrix::s_agreements.HasAgreement(m_playerId, foreignerId, PROPOSAL_TREATY_PEACE))
	{
		return COLDWAR_REGARD;
	}

	
    
	
	
	
	
	

	return NEUTRAL_REGARD;
}


void Diplomat::RecomputeRegard() 
{
	
	sint32 wonder_regard = wonderutil_GetIncreaseRegard(g_player[m_playerId]->GetBuiltWonders());

	
 	for (uint32 foreignerId = 1; foreignerId < m_foreigners.size(); foreignerId++) {

		
		if (g_player[m_playerId]->HasContactWith(foreignerId))
		{

			
			if (wonder_regard != 0)
			{
				StringId strId;
				g_theStringDB->GetStringID("REGARD_EVENT_WONDER",strId);
				Diplomat::GetDiplomat(foreignerId).
					LogRegardEvent(m_playerId, 
					wonder_regard, 
					REGARD_EVENT_DIPLOMACY,
					strId,
					1);
			}

			ai::Regard baseRegard = GetBaseRegard(foreignerId);
			
			m_foreigners[foreignerId].
				RecomputeRegard(m_diplomacy[foreignerId], 
				NewTurnCount::GetCurrentRound(),
				baseRegard);
		}

	} 

	
	ClearEffectiveRegardCache();
}


void Diplomat::UpdateRegard(const PLAYER_INDEX foreignerId)
{
	Assert(foreignerId >= 0);
	Assert((unsigned) foreignerId < m_foreigners.size());

	
	if (foreignerId < 0 || static_cast<uint32>(foreignerId) > m_foreigners.size())
		return;

	ai::Regard baseRegard = GetBaseRegard(foreignerId);

	
	m_foreigners[foreignerId].
		RecomputeRegard(m_diplomacy[foreignerId], -1, baseRegard);
}






sint32 Diplomat::AddAgreement(const PLAYER_INDEX & foreignerId) 
{
	
	ClearEffectiveRegardCache();

	ai::Agreement agreement;

	const Response & receiver_response = 
		GetDiplomat(foreignerId).GetMyLastResponse(m_playerId);
	const NewProposal & sender_proposal = 
		GetMyLastNewProposal(foreignerId);
	const Response & sender_response = 
		GetMyLastResponse(foreignerId);

	
	if ( receiver_response.type == RESPONSE_ACCEPT ) 
	{
		agreement.proposal = sender_proposal.detail;
		agreement.explainStrId = sender_proposal.explainStrId;

		
		if (sender_response.type == RESPONSE_THREATEN)
		{
			agreement.newsStrId = sender_response.newsStrId;
		}
		else
		{
			agreement.newsStrId = sender_proposal.newsStrId;
		}
	}
	
	else if ( receiver_response.type == RESPONSE_COUNTER &&
			  sender_response.type == RESPONSE_ACCEPT )
	{
		agreement.proposal = receiver_response.counter;
		agreement.explainStrId = receiver_response.explainStrId;
		agreement.newsStrId = receiver_response.newsStrId;
	}
	
	else
	{
		
		Assert(false);
		return -1;
	}

	Player * player_ptr = g_player[m_playerId];
	Assert(player_ptr != NULL);

	
	agreement.id = GetNextId();
	agreement.senderId = m_playerId;
	agreement.receiverId = foreignerId;
	agreement.start = static_cast<sint16>(player_ptr->GetCurRound());
	
	agreement.end = -1; 

	
	NegotiationEvent negotiation_event;
	negotiation_event.proposal = sender_proposal;
	negotiation_event.response = receiver_response;
	negotiation_event.agreement = agreement;
	negotiation_event.round = static_cast<sint16>(NewTurnCount::GetCurrentRound());
	AddNewNegotiationEvent(foreignerId, negotiation_event);

	
	if (Execute_Agreement( agreement ))
	{
		
		AgreementMatrix::s_agreements.SetAgreement( agreement );
	}

	return agreement.id;
}


void Diplomat::EnactStopPiracy(const PLAYER_INDEX victimId, const PLAYER_INDEX pirateId)
{
	Player *player_ptr = g_player[victimId];
	Assert(player_ptr != NULL);

	sint32 num_cities = player_ptr->m_all_cities->Num();
	sint32 i,r;
	TradeRoute route;
	Unit city;
	Army army;

	
	for (i = 0; i < num_cities; i++)
	{
		city = player_ptr->m_all_cities->Access(i);
		Assert( g_theUnitPool->IsValid(city) );
		
		
		for(r = 0; r < city.CD()->GetTradeSourceList()->Num(); r++) 
		{
			route = city.CD()->GetTradeSourceList()->Access(r);

			
			army = route->GetPiratingArmy();
			if (!g_theArmyPool->IsValid(army))
				continue;
			
			
			if (army.GetOwner() == pirateId)
			{
				
				g_gevManager->AddEvent(GEV_INSERT_Tail, GEV_SetPiratingArmy,
					   GEA_TradeRoute, route,
					   GEA_Army, Army(0),
					   GEA_End);
			}
		} 
	} 
}



bool Diplomat::CanAfford( const PLAYER_INDEX senderId,
						  const PLAYER_INDEX receiverId,
						  const ProposalData & proposal ) const
{
	
	if (senderId != -1 &&
		proposal.first_type == PROPOSAL_OFFER_GIVE_GOLD &&
		proposal.first_arg.gold > g_player[senderId]->m_gold->GetLevel())
		return false;
	
	if (senderId != -1 &&
		proposal.second_type == PROPOSAL_OFFER_GIVE_GOLD &&
		proposal.second_arg.gold > g_player[senderId]->m_gold->GetLevel())
		return false;

	if (receiverId != -1 &&
		proposal.second_type == PROPOSAL_REQUEST_GIVE_GOLD &&
		proposal.first_arg.gold > g_player[receiverId]->m_gold->GetLevel())
		return false;

	if (receiverId != -1 &&
		proposal.second_type == PROPOSAL_REQUEST_GIVE_GOLD &&
		proposal.second_arg.gold > g_player[receiverId]->m_gold->GetLevel())
		return false;

	return true;
}


bool Diplomat::Execute_Agreement( const ai::Agreement & agreement )
{
	
	if (!CanAfford(agreement.senderId, agreement.receiverId, agreement.proposal))
		return false;

	Execute_Proposal( agreement.senderId, 
					  agreement.receiverId,
					  agreement.proposal.first_type,
					  agreement.proposal.first_arg );

	Execute_Proposal( agreement.senderId, 
					  agreement.receiverId,
					  agreement.proposal.second_type,
					  agreement.proposal.second_arg );

	
	DiplomacyResult senderResult;
	DiplomacyResult receiverResult;
	StringId strId;
	NewProposal proposal;
	proposal.senderId = agreement.senderId;
	proposal.receiverId = agreement.receiverId;
	proposal.detail = agreement.proposal;
	ProposalAnalysis::ComputeResult( proposal, senderResult, receiverResult);

	
	if (receiverResult.regard < 0)
	{
		g_theStringDB->GetStringID("REGARD_EVENT_NEGATIVE_RECEIVER_DIPLOMACY_RESULT",strId);
		Diplomat::GetDiplomat(agreement.receiverId).LogRegardEvent( agreement.senderId,	
			receiverResult.regard, REGARD_EVENT_DIPLOMACY, strId);
	}
	else if (receiverResult.regard > 0)
	{
		g_theStringDB->GetStringID("REGARD_EVENT_POSITIVE_RECEIVER_DIPLOMACY_RESULT",strId);
		Diplomat::GetDiplomat(agreement.receiverId).LogRegardEvent( agreement.senderId,
			receiverResult.regard, REGARD_EVENT_DIPLOMACY, strId);
	}

	if (senderResult.regard < 0)
	{
		g_theStringDB->GetStringID("REGARD_EVENT_NEGATIVE_SENDER_DIPLOMACY_RESULT",strId);
		Diplomat::GetDiplomat(agreement.senderId).LogRegardEvent( agreement.receiverId, 
			senderResult.regard, REGARD_EVENT_DIPLOMACY, strId);
	}
	else if (senderResult.regard > 0)
	{
		g_theStringDB->GetStringID("REGARD_EVENT_POSITIVE_SENDER_DIPLOMACY_RESULT",strId);
		Diplomat::GetDiplomat(agreement.senderId).LogRegardEvent( agreement.receiverId, 
			senderResult.regard, REGARD_EVENT_DIPLOMACY, strId);
	}
	return true;
}


void Diplomat::Execute_Proposal( const PLAYER_INDEX & sender,
								 const PLAYER_INDEX & receiver,
								 const PROPOSAL_TYPE & proposal_type,
								 const DiplomacyArg & proposal_arg )
{
	
	Assert(g_player[receiver]);
	Assert(g_player[sender]);
	if (g_player[sender] == NULL || g_player[receiver] == NULL)
		return;

	sint32 gold = 0;
	switch (proposal_type) {
	case PROPOSAL_NONE:
		break;
	case PROPOSAL_OFFER_GIVE_CITY:
		
		g_gevManager->AddEvent(GEV_INSERT_Tail, GEV_GiveCity,
			GEA_City, proposal_arg.cityId,
			GEA_Player, receiver,
			GEA_End);
		break;
	case PROPOSAL_REQUEST_GIVE_CITY:
		
		g_gevManager->AddEvent(GEV_INSERT_Tail, GEV_GiveCity,
			GEA_City, proposal_arg.cityId,
			GEA_Player, sender,
			GEA_End);
		break;
	case PROPOSAL_OFFER_WITHDRAW_TROOPS:
		
		break;
	case PROPOSAL_REQUEST_WITHDRAW_TROOPS:
		
		break;
	case PROPOSAL_OFFER_STOP_PIRACY:
		
		EnactStopPiracy(receiver, sender);
		break;
	case PROPOSAL_REQUEST_STOP_PIRACY:
		
		EnactStopPiracy(sender, receiver);
		break;
	case PROPOSAL_OFFER_BREAK_AGREEMENT:
		
		AgreementMatrix::s_agreements.BreakAgreements(sender, proposal_arg.playerId);
		break;
	case PROPOSAL_REQUEST_BREAK_AGREEMENT:
		
		AgreementMatrix::s_agreements.BreakAgreements(receiver, proposal_arg.playerId);
		break;
	case PROPOSAL_OFFER_STOP_RESEARCH:
		
		break;
	case PROPOSAL_REQUEST_STOP_RESEARCH:
		
		break;
	case PROPOSAL_OFFER_REDUCE_NUCLEAR_WEAPONS:
		
		Diplomat::GetDiplomat(sender).DisbandNuclearWeapons(proposal_arg.percent);
		break;
	case PROPOSAL_REQUEST_REDUCE_NUCLEAR_WEAPONS:
		
		Diplomat::GetDiplomat(receiver).DisbandNuclearWeapons(proposal_arg.percent);
		break;
	case PROPOSAL_OFFER_REDUCE_BIO_WEAPONS:
		
		Diplomat::GetDiplomat(sender).DisbandBioWeapons(proposal_arg.percent);
		break;
	case PROPOSAL_REQUEST_REDUCE_BIO_WEAPONS:
		
		Diplomat::GetDiplomat(receiver).DisbandBioWeapons(proposal_arg.percent);
		break;
	case PROPOSAL_OFFER_REDUCE_NANO_WEAPONS:
		
		Diplomat::GetDiplomat(sender).DisbandNanoWeapons(proposal_arg.percent);
		break;
	case PROPOSAL_REQUEST_REDUCE_NANO_WEAPONS:
		
		Diplomat::GetDiplomat(receiver).DisbandNanoWeapons(proposal_arg.percent);
		break;
	case PROPOSAL_OFFER_GIVE_ADVANCE:
		
		g_gevManager->AddEvent(GEV_INSERT_Tail, GEV_GrantAdvance,
			GEA_Player, receiver,
			GEA_Int, proposal_arg.advanceType,
			GEA_Int, 0,
			GEA_End);
		break;
	case PROPOSAL_REQUEST_GIVE_ADVANCE:
		
		g_gevManager->AddEvent(GEV_INSERT_Tail, GEV_GrantAdvance,
			GEA_Player, sender,
			GEA_Int, proposal_arg.advanceType,
			GEA_Int, 0,
			GEA_End);
		break;
	case PROPOSAL_OFFER_GIVE_GOLD:
		
		gold = MIN(proposal_arg.gold, g_player[sender]->m_gold->GetLevel());

		
		g_gevManager->AddEvent(GEV_INSERT_Tail, GEV_SubGold,
			GEA_Player, sender,
			GEA_Int, gold,
			GEA_End);

		g_gevManager->AddEvent(GEV_INSERT_Tail, GEV_AddGold,
			GEA_Player, receiver,
			GEA_Int, gold,
			GEA_End);
		break;
	case PROPOSAL_REQUEST_GIVE_GOLD:
		
		gold = MIN(proposal_arg.gold, g_player[receiver]->m_gold->GetLevel());

		
		g_gevManager->AddEvent(GEV_INSERT_Tail, GEV_SubGold,
			GEA_Player, receiver,
			GEA_Int, gold,
			GEA_End);

		g_gevManager->AddEvent(GEV_INSERT_Tail, GEV_AddGold,
			GEA_Player, sender,
			GEA_Int, gold,
			GEA_End);
		break;
	case PROPOSAL_OFFER_REDUCE_POLLUTION:
		
		break;
	case PROPOSAL_REQUEST_REDUCE_POLLUTION:
		
		break;
	case PROPOSAL_OFFER_MAP:
		
		g_gevManager->AddEvent(GEV_INSERT_Tail, GEV_GiveMap,
			GEA_Player, sender,
			GEA_Player, receiver,
			GEA_End);
		break;
	case PROPOSAL_REQUEST_MAP:
		
		g_gevManager->AddEvent(GEV_INSERT_Tail, GEV_GiveMap,
			GEA_Player, receiver,
			GEA_Player, sender,
			GEA_End);
		break;
		
	case PROPOSAL_OFFER_HONOR_MILITARY_AGREEMENT:
		Diplomat::GetDiplomat(sender).DeclareWar(proposal_arg.playerId);
		break;
		
	case PROPOSAL_REQUEST_HONOR_MILITARY_AGREEMENT:
		Diplomat::GetDiplomat(receiver).DeclareWar(proposal_arg.playerId);
		break;
		
	case PROPOSAL_OFFER_HONOR_POLLUTION_AGREEMENT:
		break;
		
	case PROPOSAL_REQUEST_HONOR_POLLUTION_AGREEMENT:
		
	case PROPOSAL_OFFER_END_EMBARGO:
		Diplomat::GetDiplomat(sender).SetEmbargo(receiver, false);
		break;
		
	case PROPOSAL_REQUEST_END_EMBARGO:
		Diplomat::GetDiplomat(receiver).SetEmbargo(sender, false);
		break;
	
	case PROPOSAL_TREATY_DECLARE_WAR:
		DeclareWar(proposal_arg.playerId);
		break;
	case PROPOSAL_TREATY_CEASEFIRE:
	case PROPOSAL_TREATY_PEACE:
		
		AgreementMatrix::s_agreements.
			CancelAgreement(sender, receiver, PROPOSAL_TREATY_DECLARE_WAR);
		break;
	case PROPOSAL_TREATY_TRADE_PACT:
		break;
	case PROPOSAL_TREATY_RESEARCH_PACT:
		break;
	case PROPOSAL_TREATY_MILITARY_PACT:
		break;
	case PROPOSAL_TREATY_POLLUTION_PACT:
		break;
	case PROPOSAL_TREATY_ALLIANCE:
		
		
		break;
	case PROPOSAL_MAX:
	default:
		
		Assert(0);
	}
}


void Diplomat::DeclareWar(const PLAYER_INDEX foreignerId)
{
	Player * player_ptr = g_player[m_playerId];
	Player * foreigner_ptr = g_player[foreignerId];
	bool NO_CONTACT_DECLARE_WAR = false;

	
	if (foreignerId != 0)
	{
		if (!player_ptr || !player_ptr->HasContactWith(foreignerId))
		{
			Assert(NO_CONTACT_DECLARE_WAR)
			return;
		}
		
		if (!foreigner_ptr || !foreigner_ptr->HasContactWith(m_playerId))
		{
			Assert(NO_CONTACT_DECLARE_WAR)
			return;
		}
	}

	
	if (AgreementMatrix::s_agreements.HasAgreement(m_playerId, foreignerId, PROPOSAL_TREATY_DECLARE_WAR))
		return;

	
	if (m_playerId != 0 && foreignerId != 0)
	{
		SlicObject *so;
		
		if (foreignerId == g_selected_item->GetCurPlayer())
		{
			
			so = new SlicObject((MBCHAR *)"DIPLOMACY_POPUP_DECLARE_WAR");
			so->AddRecipient(foreignerId);
			so->AddCivilisation(foreignerId) ;		
			so->AddCivilisation(m_playerId) ;
			g_slicEngine->Execute(so);

			
			so = new SlicObject((MBCHAR *)"128CivStartedWar");
			so->AddAllRecipientsBut(m_playerId, foreignerId);
			so->AddCivilisation(m_playerId) ;
			so->AddCivilisation(foreignerId) ;		
			g_slicEngine->Execute(so);
		}
		
		else
		{
			
			so = new SlicObject((MBCHAR *)"128CivStartedWar");
			so->AddAllRecipientsBut(m_playerId);
			so->AddCivilisation(m_playerId) ;
			so->AddCivilisation(foreignerId) ;		
			g_slicEngine->Execute(so);
		}
	}

	ai::Agreement agreement;
	agreement.senderId = m_playerId;
	agreement.receiverId = foreignerId;
	agreement.start = static_cast<sint16>(NewTurnCount::GetCurrentRound());
	agreement.end = -1;
	agreement.proposal.first_type = PROPOSAL_TREATY_DECLARE_WAR;

	AgreementMatrix::s_agreements.SetAgreement(agreement);

	
	AgreementMatrix::s_agreements.
		CancelAgreement(m_playerId, foreignerId, PROPOSAL_OFFER_WITHDRAW_TROOPS);

	
	AgreementMatrix::s_agreements.
		CancelAgreement(m_playerId, foreignerId, PROPOSAL_TREATY_PEACE);

	
	AgreementMatrix::s_agreements.
		CancelAgreement(m_playerId, foreignerId, PROPOSAL_TREATY_CEASEFIRE);

		
	AgreementMatrix::s_agreements.
		CancelAgreement(m_playerId, foreignerId, PROPOSAL_TREATY_TRADE_PACT);

	AgreementMatrix::s_agreements.
		CancelAgreement(m_playerId, foreignerId, PROPOSAL_TREATY_RESEARCH_PACT);

	AgreementMatrix::s_agreements.
		CancelAgreement(m_playerId, foreignerId, PROPOSAL_TREATY_MILITARY_PACT);

	AgreementMatrix::s_agreements.
		CancelAgreement(m_playerId, foreignerId, PROPOSAL_TREATY_ALLIANCE);

	if(g_network.IsHost()) {
		g_network.Block(m_playerId);
		g_network.QueuePacketToAll(new NetAgreementMatrix());
		g_network.Unblock(m_playerId);
	}

    
    g_theTradePool->BreakOffTrade(m_playerId, foreignerId);

	
	player_ptr->CloseEmbassy(foreignerId);
	foreigner_ptr->CloseEmbassy(m_playerId);
}


void Diplomat::SetEmbargo(const PLAYER_INDEX foreignerId, const bool state)
{
	Diplomat & foreigner = Diplomat::GetDiplomat(foreignerId);

	if (state == true && !m_foreigners[foreignerId].GetEmbargo())
	{
		
		Threat threat;
		threat.id = GetNextId();
		threat.senderId = m_playerId;
		threat.receiverId = foreignerId;
		threat.start = static_cast<sint16>(NewTurnCount::GetCurrentRound());
		threat.end = -1; 
		
		
		m_threats.push_back(threat);

		
		LogViolationEvent(foreignerId, PROPOSAL_TREATY_TRADE_PACT);

		sint32 regard_cost = 0;
		if (foreigner.GetCurrentDiplomacy(m_playerId).GetEmbargoTradeRegardCost())
		{
			foreigner.GetCurrentDiplomacy(m_playerId).GetEmbargoTradeRegardCost(regard_cost);
				
			StringId strId;
			g_theStringDB->GetStringID("REGARD_EVENT_EMBARGO_TRADE", strId);
			foreigner.LogRegardEvent(m_playerId, 
				regard_cost, 
				REGARD_EVENT_GOLD, 
				strId,
				1); 
		}
		
		g_theTradePool->BreakOffTrade(m_playerId, foreignerId);
	}

	m_foreigners[foreignerId].SetEmbargo(state);
}


bool Diplomat::GetEmbargo(const PLAYER_INDEX foreignerId) const
{
	
	if (AgreementMatrix::s_agreements.HasAgreement(m_playerId, foreignerId, PROPOSAL_TREATY_DECLARE_WAR))
		return true;
	return m_foreigners[foreignerId].GetEmbargo();
}






void Diplomat::AddRejection(const PLAYER_INDEX & foreignerId)
{
	const Response & receiver_response = 
		GetDiplomat(foreignerId).GetMyLastResponse(m_playerId);

	const NewProposal & sender_proposal = 
		GetMyLastNewProposal(foreignerId);

	
	

	
	
	if (sender_proposal.detail.first_type == PROPOSAL_REQUEST_HONOR_MILITARY_AGREEMENT ||
		sender_proposal.detail.second_type == PROPOSAL_REQUEST_HONOR_MILITARY_AGREEMENT)
	{
		if (AgreementMatrix::s_agreements.HasAgreement(m_playerId, foreignerId, PROPOSAL_TREATY_ALLIANCE))
		{
			LogViolationEvent(foreignerId, PROPOSAL_TREATY_ALLIANCE);
		}
		else if (AgreementMatrix::s_agreements.HasAgreement(m_playerId, foreignerId, PROPOSAL_TREATY_MILITARY_PACT))
		{
			LogViolationEvent(foreignerId, PROPOSAL_TREATY_MILITARY_PACT);
		}
	}
	else if (sender_proposal.detail.first_type == PROPOSAL_REQUEST_HONOR_POLLUTION_AGREEMENT ||
		sender_proposal.detail.second_type == PROPOSAL_REQUEST_HONOR_POLLUTION_AGREEMENT)
	{
		LogViolationEvent(foreignerId, PROPOSAL_TREATY_POLLUTION_PACT);
	}

	
	sint32 regard_cost = GetViolationRegardCost(foreignerId, sender_proposal.detail.first_type) / 3;
	if (regard_cost <= 0)
	{
		StringId strId;
		g_theStringDB->GetStringID("REGARD_EVENT_NEGATIVE_SENDER_DIPLOMACY_RESULT",strId);
		LogRegardEvent( foreignerId, regard_cost, REGARD_EVENT_DIPLOMACY, strId);
	}

	
	NegotiationEvent negotiation_event;
	negotiation_event.proposal = sender_proposal;
	negotiation_event.response = receiver_response;
	negotiation_event.round = static_cast<sint16>(NewTurnCount::GetCurrentRound());
	AddNewNegotiationEvent(foreignerId, negotiation_event);
}






const Threat & Diplomat::GetThreatById(const sint32 & id) const {
	static Threat bad_threat;
	
	ThreatList::const_iterator found = find(m_threats.begin(), m_threats.end(), id);
	if (found != m_threats.end())
		return *found;
	else
		return bad_threat;
}


void Diplomat::RemoveThreatById(const sint32 & id) {
	
	ThreatList::iterator found = find(m_threats.begin(), m_threats.end(), id);
	if (found != m_threats.end())
		m_threats.erase(found);
	else
	{
		
		Assert(0);
	}
}


sint32 Diplomat::AddThreat(const PLAYER_INDEX & foreignerId) {

	Threat threat;

	const Response & receiver_response = 
		GetDiplomat(foreignerId).GetMyLastResponse(m_playerId);

	const NewProposal & sender_proposal = 
		GetMyLastNewProposal(foreignerId);

	const Response & sender_response = 
		GetMyLastResponse(foreignerId);

	
	if ( sender_response.type == RESPONSE_THREATEN &&
		 receiver_response.type != RESPONSE_ACCEPT )
	{
		threat.detail = sender_response.threat;
		threat.newsStrId = sender_response.newsStrId;
		threat.explainStrId = sender_response.explainStrId;
	}
	else
	{
		
		Assert(false);
		return -1;
	}

	
	threat.id = GetNextId();
	threat.senderId = m_playerId;
	threat.receiverId = foreignerId;
	threat.start = static_cast<sint16>(NewTurnCount::GetCurrentRound());
	threat.end = -1; 

	
	NegotiationEvent negotiation_event;
	negotiation_event.proposal = sender_proposal;
	negotiation_event.response = receiver_response;
	negotiation_event.threat = threat;
	negotiation_event.round = static_cast<sint16>(NewTurnCount::GetCurrentRound());
	AddNewNegotiationEvent(foreignerId, negotiation_event);

	
	if (!ExecuteThreat(threat))
		
		m_threats.push_back(threat);

	return threat.id;
}


const Diplomat::ThreatList & Diplomat::GetThreatList() const {
	return m_threats;
}


bool Diplomat::LaunchNuclearAttackOnCity(const Unit & city)
{
	Assert(g_player[m_playerId]);
	Player *player_ptr = g_player[m_playerId];
	
	Assert(city.m_id != 0x0);
	if (city.m_id == 0x0)
		return false;

	
	TargetNuclearAttack(city.GetOwner(), false);
	
	
	sint32 num_units = player_ptr->m_all_units->Num();
	Unit tmp_unit;
	bool found_targeted_nuke = false;
	
	
	for (sint16 i = 0; i < num_units; i++)
	{
		tmp_unit = player_ptr->m_all_units->Access(i);
		Assert( g_theArmyPool->IsValid(tmp_unit) );
		if ( !g_theArmyPool->IsValid(tmp_unit) )
			continue;
		
		
		if(!tmp_unit.GetDBRec()->GetNuclearAttack())
			continue;
		
		
		found_targeted_nuke = tmp_unit->GetTargetCity().m_id != 0x0;
		if (tmp_unit->GetTargetCity().m_id == city.m_id)
		{
			break;
		}
	}

	if (found_targeted_nuke)
	{
		unitutil_ExecuteMadLaunch(tmp_unit);

		return true;
	}
	return false;
}


bool Diplomat::ExecuteThreat(const Threat & threat)
{
	const NewProposal & sender_proposal = 
		GetMyLastNewProposal(threat.receiverId);
	const Diplomat & receiver_diplomat = Diplomat::GetDiplomat(threat.receiverId);
	StringId strId;
	sint32 regard_cost=0;
	sint32 trust_cost=0;
	char *str_buf = NULL;

	switch (threat.detail.type)
	{
	case THREAT_DESTROY_CITY:
		receiver_diplomat.GetCurrentDiplomacy(threat.senderId).GetNukeCityRegardCost(regard_cost);
		receiver_diplomat.GetCurrentDiplomacy(threat.senderId).GetUsedNukesTrustCost(trust_cost);
		trust_cost /= 2;
		str_buf = "Threatened to destroy our city.";
		break;
	
		
		
	
	
	
	
		
		
	
	
	
	case THREAT_TRADE_EMBARGO:
		
		receiver_diplomat.GetCurrentDiplomacy(threat.senderId).GetEmbargoTradeRegardCost(regard_cost);
		receiver_diplomat.GetCurrentDiplomacy(threat.senderId).GetFollowThroughTrustBonus(trust_cost);
		trust_cost /= -2;
		str_buf = "Threatened to embargo trade.";
		break;
	case THREAT_DECLARE_WAR:
		
		
		receiver_diplomat.GetCurrentDiplomacy(threat.senderId).GetPreemptiveAttackRegardCost(regard_cost);
		receiver_diplomat.GetCurrentDiplomacy(threat.senderId).GetPreemptiveAttackTrustCost(trust_cost);
		trust_cost /= 2;
		str_buf = "Threatened to declare war.";
		break;
	}

	
	g_theStringDB->GetStringID("REGARD_EVENT_NEGATIVE_RECEIVER_DIPLOMACY_RESULT", strId);
	
	Diplomat::GetDiplomat(threat.receiverId).
		LogRegardEvent(threat.senderId, 
		regard_cost, 
		REGARD_EVENT_DIPLOMACY,
		strId);

	Diplomat::GetDiplomat(threat.receiverId).
		ApplyTrustChange(threat.senderId, trust_cost, str_buf);

	return false;
}



bool Diplomat::HasThreat(const PLAYER_INDEX & foreignerId,
						 const THREAT_TYPE type,
						 Threat & threat) const
{
	ThreatList::const_iterator threat_iter;
	for (threat_iter = m_threats.begin(); threat_iter != m_threats.end(); threat_iter++)
	{
		if ((threat_iter->detail.type == type || type == THREAT_NONE) &&
			threat_iter->receiverId == foreignerId)
		{
			threat = *threat_iter;
			if (threat.end >= NewTurnCount::GetCurrentRound())

			return true;
		}
	}

	return false;
}


bool Diplomat::ComputeThreatResponse(const PLAYER_INDEX foreignerId, Response & threat_response) const
{
	
	if (GetPersonality()->GetConquestPassive())
		return false;

	
	ai::Regard regard = GetEffectiveRegard(foreignerId);
	ai::Regard trust = GetTrust(foreignerId);

	DIPLOMATIC_STRENGTH sender_strength = 
		g_player[m_playerId]->GetRelativeStrength(foreignerId);
	
	if (regard > FRIEND_REGARD)
		{
			
			if (!GetPersonality()->GetAlignmentEvil() &&
				!GetPersonality()->GetTrustworthinessChaotic())
				 return false;
		}
	else if (regard > NEUTRAL_REGARD)
		{
			
			if (!GetPersonality()->GetConquestAgressive() &&
				!GetPersonality()->GetDiscoveryMilitary())
				return false;

			
			if (sender_strength < DIPLOMATIC_STRENGTH_STRONG)
				return false;
		}
	else if (regard > COLDWAR_REGARD)
		{
			
			if (GetPersonality()->GetAlignmentGood() ||
				GetPersonality()->GetConquestNeutral() ||
				GetPersonality()->GetDiscoveryDiplomatic())
				return false;
		}

	const Diplomat & foreign_diplomat = Diplomat::GetDiplomat(foreignerId);
	const MapAnalysis & map_analysis = MapAnalysis::GetMapAnalysis();
	const AgreementMatrix & agreements = AgreementMatrix::s_agreements;
	const Response & receiver_response = GetDiplomat(foreignerId).GetMyLastResponse(m_playerId);

	
	ai::Agreement pact;

	
	bool has_pacts = GetAgreementToBreak(foreignerId, pact);

	Unit atrisk_city;
	bool threaten_destroy_city = false;
	bool threaten_attack_city = (map_analysis.MostAtRiskCity(foreignerId, atrisk_city, m_playerId) > 50);
	bool threaten_pirate = foreign_diplomat.GetTradeRoutePiracyRisk(m_playerId);
	bool threaten_special_attack = (map_analysis.GetSpecialAttackers(m_playerId) > 0);
	bool threaten_end_agreement = has_pacts;
	bool threaten_embargo = !GetEmbargo(foreignerId);
	sint32 turns_since_last_war = agreements.TurnsSinceLastWar(m_playerId, foreignerId);
	bool threaten_declare_war = (turns_since_last_war > 5 || turns_since_last_war < 0);

	bool has_more_nukes = 
		map_analysis.GetNuclearWeaponsCount(m_playerId) > map_analysis.GetNuclearWeaponsCount(foreignerId);
	bool has_only_nano =
		map_analysis.GetNanoWeaponsCount(m_playerId) && !map_analysis.GetNanoWeaponsCount(foreignerId);
	bool has_only_bio =
		map_analysis.GetBioWeaponsCount(m_playerId) && !map_analysis.GetBioWeaponsCount(foreignerId);

	
	if (GetPersonality()->GetTrustworthinessChaotic())
	{
		threaten_destroy_city = true;
		threaten_attack_city = true;
		threaten_pirate = true;
		threaten_special_attack = true;
	}
	
	else if (GetPersonality()->GetAlignmentEvil() ||
			 GetPersonality()->GetConquestAgressive() ||
			 GetPersonality()->GetDiscoveryMilitary())
	{
		
		if (agreements.TurnsAtWar(m_playerId, foreignerId) >= 0)
		{
			
			threaten_destroy_city &= (has_more_nukes || has_only_nano || has_only_bio);
		}
		else
		{
			threaten_destroy_city = false;
			threaten_attack_city = false;
			
			
			if (has_pacts || (regard > COLDWAR_REGARD))
			{
				threaten_pirate = false;
				threaten_special_attack = false;
				threaten_end_agreement  = false;
				threaten_embargo = false;
				threaten_declare_war = false;
			}
		}
	}
	
	else if (GetPersonality()->GetDiscoveryScientist()||
		GetPersonality()->GetDiscoveryEconomic())
	{
		
		if (agreements.TurnsAtWar(m_playerId, foreignerId) > 50)
		{
			
			threaten_destroy_city = (regard < HOTWAR_REGARD && trust < HOTWAR_REGARD);
			threaten_destroy_city &= (has_more_nukes || has_only_nano || has_only_bio);
		}
		else
		{
			threaten_destroy_city = false;
			threaten_attack_city = false;
			if (has_pacts || (regard > HOTWAR_REGARD))
			{
				threaten_special_attack = false;
				threaten_declare_war = false;
				threaten_end_agreement  = false;
			}
		}

		
		threaten_embargo = false;
		threaten_pirate = false;
	}

	
	else if (GetPersonality()->GetDiscoveryDiplomatic() &&
		GetPersonality()->GetAlignmentNeutral())
	{
		
		if (agreements.TurnsAtWar(m_playerId, foreignerId) > 75)
		{
			
			
			threaten_destroy_city = (trust < MIN_REGARD) && (regard < HOTWAR_REGARD);
			threaten_destroy_city &= (has_more_nukes || has_only_nano || has_only_bio);
			
			
			threaten_attack_city &= (trust <= HOTWAR_REGARD) || (regard < HOTWAR_REGARD);
		}
		else
		{
			threaten_destroy_city = false;
			threaten_attack_city = false;
			if (has_pacts || (regard > HOTWAR_REGARD))
			{
				threaten_special_attack = false;
				threaten_declare_war = false;
				threaten_pirate = false;
				
				threaten_embargo = false;
			}
		}
	}

	
	if (GetPersonality()->GetAlignmentGood())
	{
		
		if (agreements.TurnsAtWar(m_playerId, foreignerId) > 150)
		{
			if ((trust > MIN_REGARD) && (regard >= MIN_REGARD))
			{
				
				threaten_destroy_city = false;
				threaten_attack_city = false;
				threaten_special_attack = false;
				threaten_declare_war = false;
				threaten_pirate = false;
				threaten_end_agreement  = false;
				threaten_embargo = false;
			}
		}
		else
		{
				threaten_destroy_city = false;
				threaten_attack_city = false;
				threaten_special_attack = false;
				threaten_declare_war = false;
				threaten_pirate = false;
				threaten_end_agreement  = false;
				threaten_embargo = false;
		}
	}

	
	threat_response.senderId = m_playerId;
	threat_response.senderId = foreignerId;
	threat_response.priority = 1;

	
	if (threaten_destroy_city)
	{
		NukeTargetList nuke_city_list;
		ComputeNukeTargets(nuke_city_list, foreignerId);

		if (nuke_city_list.size() > 0)
		{
			threat_response.threat.type = THREAT_DESTROY_CITY;
			Unit nuke_city = nuke_city_list.front().second;
			threat_response.threat.arg.cityId = nuke_city.m_id;
			return true;
		}
	}
	else if (threaten_attack_city)
	{
		Assert(atrisk_city.m_id != 0);
		if (atrisk_city.m_id != 0)
		{
			threat_response.threat.type = THREAT_DESTROY_CITY;
			threat_response.threat.arg.cityId = atrisk_city.m_id;
			return true;
		}
	}
	
	
	
	
	
	else if (threaten_declare_war)
	{
		threat_response.threat.type = THREAT_DECLARE_WAR;
		return true;
	}
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	else if (threaten_embargo)
	{
		threat_response.threat.type = THREAT_TRADE_EMBARGO;
		return true;
	}

	return false;
}


bool Diplomat::GetAgreementToBreak(const PLAYER_INDEX foreignerId, ai::Agreement & pact) const
{
	const AgreementMatrix & agreements = AgreementMatrix::s_agreements;

	if (agreements.HasAgreement(m_playerId, foreignerId, PROPOSAL_TREATY_ALLIANCE))
	{
		pact = agreements.GetAgreement(m_playerId, foreignerId, PROPOSAL_TREATY_ALLIANCE);
		return true;
	}
	else if (agreements.HasAgreement(m_playerId, foreignerId, PROPOSAL_TREATY_MILITARY_PACT))
	{
		pact = agreements.GetAgreement(m_playerId, foreignerId, PROPOSAL_TREATY_MILITARY_PACT);
		return true;
	}
	else if (agreements.HasAgreement(m_playerId, foreignerId, PROPOSAL_TREATY_RESEARCH_PACT))
	{
		pact = agreements.GetAgreement(m_playerId, foreignerId, PROPOSAL_TREATY_RESEARCH_PACT);
		return true;
	}
	else if (agreements.HasAgreement(m_playerId, foreignerId, PROPOSAL_TREATY_TRADE_PACT))
	{
		pact = agreements.GetAgreement(m_playerId, foreignerId, PROPOSAL_TREATY_TRADE_PACT);
		return true;
	}
	else if (agreements.HasAgreement(m_playerId, foreignerId, PROPOSAL_TREATY_POLLUTION_PACT))
	{
		pact = agreements.GetAgreement(m_playerId, foreignerId, PROPOSAL_TREATY_POLLUTION_PACT);
		return true;
	}
	else if (agreements.HasAgreement(m_playerId, foreignerId, PROPOSAL_TREATY_PEACE))
	{
		pact = agreements.GetAgreement(m_playerId, foreignerId, PROPOSAL_TREATY_PEACE);
		return true;
	}


	return false;
}
	



			

void Diplomat::ConsiderResponse( const PLAYER_INDEX & foreignId,
								 const Response & response ) 
{
	
	if (response.priority <= 0)
		return;

	Diplomat & sender_diplomat = 
		Diplomat::GetDiplomat(response.senderId);

	const NewProposal & sender_proposal = 
		sender_diplomat.GetMyLastNewProposal(m_playerId);

	
	if (response.type == RESPONSE_ACCEPT &&
		!CanAfford(-1, sender_proposal.receiverId, sender_proposal.detail))
		return;
	else if (response.type == RESPONSE_COUNTER &&
		!CanAfford(-1, response.receiverId, response.counter))
		return;

	Assert(response.senderId != -1);
	Assert(response.receiverId != -1);

	
	
	m_foreigners[foreignId].ConsiderResponse(response);

	DPRINTF(k_DBG_DIPLOMACY,("  Player %d considers Response: %s for player %d.\n",
			m_playerId, s_responseNames[response.type].c_str(), foreignId));

}


void Diplomat::ConsiderResponse( const PLAYER_INDEX senderId, 
								 const RESPONSE_TYPE response_type,
								 const sint32 priority)
{
	Assert(response_type == RESPONSE_REJECT || response_type == RESPONSE_ACCEPT);
	
	
	if (priority <= 0)
		return;

	Response response;
	
	response.priority = static_cast<sint16>(priority);
	response.receiverId = m_playerId;
	response.senderId = senderId;
	response.type = response_type;
	
	
	ConsiderResponse( senderId, response);
}


void Diplomat::ConsiderCounterResponse( const PLAYER_INDEX senderId,
									    const PROPOSAL_TYPE counterProposalType,
									    const DiplomacyArg & argument,
									    const sint32 priority,
										const DIPLOMATIC_TONE tone)
{

	
	if (priority <= 0)
		return;

	Response response;

	response.priority = static_cast<sint16>(priority);
	response.receiverId = m_playerId;
	response.senderId = senderId;

	Diplomat & sender_diplomat = Diplomat::GetDiplomat(senderId);

	const NewProposal & sender_proposal = 
		sender_diplomat.GetMyLastNewProposal(m_playerId);

	if (sender_proposal.detail.second_type == counterProposalType &&
		sender_proposal.detail.second_arg == argument)
	{
		response.type = RESPONSE_ACCEPT;
	}
	else
	{
		response.type = RESPONSE_COUNTER;

		
		response.counter = sender_proposal.detail;

		
		response.counter.tone = tone;
		
		
		response.counter.second_type = counterProposalType;
		response.counter.second_arg = argument;
	}
	
	
	ConsiderResponse(senderId, response);
}


const Response & Diplomat::GetMyLastResponse(const PLAYER_INDEX & foreignId) const {
	return m_foreigners[foreignId].GetMyLastResponse();
}


void Diplomat::SetMyLastResponse( const PLAYER_INDEX & foreignId, 
								  const Response & response ) {
	m_foreigners[foreignId].SetMyLastResponse(response);	

	DPRINTF(k_DBG_DIPLOMACY,("  Player %d set Response: %s for player %d.\n",
			m_playerId, s_responseNames[response.type].c_str(), foreignId));

	if (response.type == RESPONSE_COUNTER)
	{
		DPRINTF(k_DBG_DIPLOMACY,("  Counter Proposal first = %s.\n", s_proposalNames[response.counter.first_type].c_str() ));
		DPRINTF(k_DBG_DIPLOMACY,("                   second = %s.\n", s_proposalNames[response.counter.second_type].c_str() ));
	}
}


void Diplomat::ExecuteResponse( const PLAYER_INDEX sender,
								const PLAYER_INDEX receiver ) {
	
	

	Response response;
	PROPOSAL_TYPE proposal_type;
	RESPONSE_TYPE other_response;
	THREAT_TYPE threat_type;
	PLAYER_INDEX other_player;

	
	if (m_playerId == sender) {
		
		response = GetMyLastResponse(receiver);

		
		proposal_type = Diplomat::GetDiplomat(receiver).GetMyLastResponse(sender).counter.second_type;
		other_player = receiver;
		other_response = Diplomat::GetDiplomat(receiver).GetMyLastResponse(sender).type;
	}
	else {
		
		response = GetMyLastResponse(sender);

		
		proposal_type = Diplomat::GetDiplomat(sender).GetMyLastNewProposal(receiver).detail.first_type;
		threat_type = Diplomat::GetDiplomat(sender).GetMyLastResponse(receiver).threat.type;
		other_player = sender;
		other_response = Diplomat::GetDiplomat(sender).GetMyLastResponse(receiver).type;
	}

	
	response.id = GetNextId();

#ifndef _BFR_
	gslog_dipprint("Player %d responds:\n", m_playerId);

	if(g_theProfileDB->GetEnableLogs())
	  {
	    ProposalAnalysis::LogDebugResult(response);
	  }
#endif // _BFR_

	Assert(response.type != RESPONSE_INVALID);
	if (response.type == RESPONSE_INVALID)
	{
		DPRINTF(k_DBG_DIPLOMACY, ("\n"));
		DPRINTF(k_DBG_DIPLOMACY, (">>> No response found by player %d.\n",
			static_cast<sint32>(m_playerId)));
		DPRINTF(k_DBG_DIPLOMACY, ("    (Why didn't the default response fire?)\n\n"));
		bool NO_RESPONSE_FOUND = false;
		Assert(NO_RESPONSE_FOUND);

		
		response.type = RESPONSE_REJECT;
	}

	
	
	if (response.type == RESPONSE_ACCEPT) {

		DPRINTF(k_DBG_DIPLOMACY, ("\n"));
		DPRINTF(k_DBG_DIPLOMACY, (">>> Agreement ACCEPTED by player %d.\n",
			static_cast<sint32>(m_playerId)));
		DPRINTF(k_DBG_DIPLOMACY, ("    (new agreement created)\n\n"));

		
		
		
		
		DipWizard::NotifyResponse(response, m_playerId, other_player);
		g_network.NotifyDiplomacyResponse(response, m_playerId, other_player);

		
		Diplomat::GetDiplomat(sender).AddAgreement(receiver);

		
		g_gevManager->AddEvent(GEV_INSERT_Tail, GEV_ContinueDiplomacy,
			GEA_Player, sender,
			GEA_Player, receiver,
			GEA_End);

		return;
	}

	
	else if ( (response.type == RESPONSE_REJECT) &&
			  (m_playerId == sender) ) {
		
		DPRINTF(k_DBG_DIPLOMACY, ("\n"));
		DPRINTF(k_DBG_DIPLOMACY, (">>> Agreement REJECTED by player %d.\n\n",
			static_cast<sint32>(m_playerId)));

		
		
		
		
		if (other_response == RESPONSE_COUNTER)
		{
			DipWizard::NotifyResponse(response, m_playerId, other_player);
			g_network.NotifyDiplomacyResponse(response, m_playerId, other_player);
		}

		
		Diplomat::GetDiplomat(sender).AddRejection(receiver);

		
		g_gevManager->AddEvent(GEV_INSERT_Tail, GEV_ContinueDiplomacy,
			GEA_Player, sender,
			GEA_Player, receiver,
			GEA_End);
		
		return;
	} else {
		g_network.NotifyDiplomacyResponse(response, m_playerId, other_player);
	}

	
	const Response & sender_response =
		Diplomat::GetDiplomat(sender).GetMyLastResponse(receiver);
	if ( (response.type != RESPONSE_ACCEPT) &&
		 (m_playerId == receiver) &&
		 (sender_response.type == RESPONSE_THREATEN)) {
		
		DPRINTF(k_DBG_DIPLOMACY, ("\n"));
		DPRINTF(k_DBG_DIPLOMACY, (">>> Threat REJECTED by player %d.\n",
			static_cast<sint32>(m_playerId)));
		DPRINTF(k_DBG_DIPLOMACY, ("    (new threat created)\n\n"));

		
		DipWizard::NotifyThreatRejected(response, sender_response, m_playerId, other_player);
		g_network.NotifyDiplomacyThreatRejected(response, sender_response, m_playerId, other_player);

		
		Diplomat::GetDiplomat(sender).AddThreat(receiver);

		
		g_gevManager->AddEvent(GEV_INSERT_Tail, GEV_ContinueDiplomacy,
			GEA_Player, sender,
			GEA_Player, receiver,
			GEA_End);

		return;
	}
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	

	
	
	

	
	g_gevManager->AddEvent(GEV_INSERT_Tail, GEV_ToggleInitiative, 
						   GEA_Player, sender,
						   GEA_Player, receiver,
						   GEA_End);

	switch (response.type) {
	case RESPONSE_REJECT:
		
		g_gevManager->AddEvent(GEV_INSERT_Tail, GEV_Reject,
			GEA_Player, sender,
			GEA_Player, receiver,
			GEA_End);
		break;

	case RESPONSE_COUNTER:
		
		
		g_gevManager->AddEvent(GEV_INSERT_Tail, GEV_Counter,
			GEA_Player, sender,
			GEA_Player, receiver,
			GEA_End);

		break;

	case RESPONSE_THREATEN:
		
		g_gevManager->AddEvent(GEV_INSERT_Tail, GEV_Threaten,
			GEA_Player, sender,
			GEA_Player, receiver,
			GEA_End);
		break;
	}
}

	


const Response & Diplomat::GetResponsePending(const PLAYER_INDEX foreignerId) const
{
	const Response & response = GetMyLastResponse(foreignerId);

	if (response != s_badResponse)
	{
		
		if (response.senderId == m_playerId &&
			GetReceiverHasInitiative(foreignerId) == false)
			return s_badResponse;

		
		
		if (response.receiverId == m_playerId &&
			GetDiplomat(response.senderId).GetReceiverHasInitiative(m_playerId) == true)
			return s_badResponse;
	}

	return response;
}
	

const Response & Diplomat::GetResponse(const PLAYER_INDEX foreignerId) const
{
	const Response & response = GetMyLastResponse(foreignerId);

	if (response != s_badResponse)
	{
		
		if (response.senderId == m_playerId &&
			GetReceiverHasInitiative(foreignerId) == true)
			return s_badResponse;

		
		
		if (response.receiverId == m_playerId &&
			GetDiplomat(response.senderId).GetReceiverHasInitiative(m_playerId) == false)
			return s_badResponse;
	}

	return response;
}
	

void Diplomat::ExecuteResponse(const Response & response)
{
	if (response.senderId == m_playerId)
	{
		SetMyLastResponse(response.receiverId, response);

		
		Assert(GetReceiverHasInitiative(response.receiverId) == false);

		
		Assert(GetMyLastNewProposal(response.receiverId) != s_badNewProposal);

		
		Assert(Diplomat::GetDiplomat(response.receiverId).GetMyLastResponse(response.senderId) != s_badResponse);
	}
	else if (response.receiverId == m_playerId)
	{
		SetMyLastResponse(response.senderId, response);

		
		Assert(Diplomat::GetDiplomat(response.senderId).GetReceiverHasInitiative(response.receiverId) == true);

		
		Assert(Diplomat::GetDiplomat(response.senderId).GetMyLastNewProposal(response.receiverId) != s_badNewProposal);
	}
	else
	{
		
		Assert(0);
		return;
	}

	
	ExecuteResponse(response.senderId, response.receiverId);
}














void Diplomat::ConsiderMotivation(const Motivation & motivation) {

	
	m_motivations.push_back(motivation);
	DPRINTF(k_DBG_DIPLOMACY,(" Player %d consider motivation: %s.\n",
			m_playerId, s_motivationNames[motivation.type].c_str()));

} 


sint32 Diplomat::GetMotivationCount() const 
{
	return (m_motivations.size());
}


void Diplomat::SortMotivations() 
{
	
	m_motivations.sort();
}


const Motivation & Diplomat::GetCurrentMotivation() const
{
	if (m_motivations.size() > 0)
		return m_motivations.front();

	return s_badMotivation;
}



StringId Diplomat::GetDiplomacyAdvice(SlicContext & sc, const PLAYER_INDEX & foreignerId) const
{
	
	sc.AddPlayer(m_playerId);

	
	if (foreignerId == m_playerId || foreignerId <= 0)
	{
		if (m_motivations.size() > 0)
		{
			Motivation motivation = m_motivations.front();
			return motivation.adviceStrId;
		}
	}
	
	else
	{
		
		sc.AddPlayer(foreignerId);

		MotivationList::const_iterator motivation_iter = m_lastMotivation[foreignerId];

		
		const NewProposal & new_proposal = GetMyLastNewProposal(foreignerId);
		if (new_proposal.adviceStrId != -1)
		{
			Diplomat::AddDiplomacyArgToSlicContext(sc, new_proposal.detail.first_arg);
			Diplomat::AddDiplomacyArgToSlicContext(sc, new_proposal.detail.second_arg);
			return new_proposal.adviceStrId;
		}
		
		
		else if (motivation_iter != m_motivations.end())
		{
			return m_lastMotivation[foreignerId]->adviceStrId;
		}
		
		
		
		else
		{
			return m_diplomaticStates[foreignerId].adviceStrId;
		}
			
	}
	
	return -1;
}









void Diplomat::ContinueDiplomacy(const PLAYER_INDEX & foreignerId) {

	MotivationList::iterator motivation_iter = m_lastMotivation[foreignerId];

	RESPONSE_TYPE response;

	Motivation topMotivation = s_badMotivation;
	if (motivation_iter != m_motivations.end())
		topMotivation = *motivation_iter;

	
	

	
	bool end_diplomacy = false;
	if ( Diplomat::GetDiplomat(foreignerId).GetMyLastResponse(m_playerId).type 
		 == RESPONSE_REJECT )
		{
			end_diplomacy = true;
			response = RESPONSE_REJECT; 
		}
	
	else if ( ( Diplomat::GetDiplomat(foreignerId).GetMyLastResponse(m_playerId).type 
				== RESPONSE_COUNTER ) &&
			  ( Diplomat::GetDiplomat(m_playerId).GetMyLastResponse(foreignerId).type
				== RESPONSE_REJECT ) )
		{
			end_diplomacy = true;
			response = RESPONSE_REJECT; 
		}

	
	else if ( ( Diplomat::GetDiplomat(m_playerId).GetMyLastResponse(foreignerId).type
				== RESPONSE_THREATEN ) &&
			  ( Diplomat::GetDiplomat(m_playerId).GetMyLastResponse(foreignerId).type
				!= RESPONSE_ACCEPT ) )
		{
			end_diplomacy = true;
			response = RESPONSE_REJECT; 
		}
	else
		{
			end_diplomacy = false;
			response = RESPONSE_ACCEPT; 
		}

	
	SetMyLastNegotiatedProposal( foreignerId, 
								 GetMyLastNewProposal(foreignerId).detail, 
								 response );

	
	SetMyLastResponse( foreignerId, Diplomat::s_badResponse);
	SetMyLastNewProposal( foreignerId, Diplomat::s_badNewProposal);

	
	if ( motivation_iter != m_motivations.end() ) 
	{
		motivation_iter++;
		m_lastMotivation[foreignerId] = motivation_iter;
	}
	else
	{
		
		end_diplomacy = true;
	}

	
	m_outstandingProposals--;

	
	if (end_diplomacy)
	{
		
		
		
		if((g_turn->IsHotSeat() || g_turn->IsEmail()) &&
			g_player[foreignerId] &&
			g_player[foreignerId]->GetPlayerType() != PLAYER_TYPE_ROBOT &&
			g_player[m_playerId] &&
			g_player[m_playerId]->GetPlayerType() == PLAYER_TYPE_ROBOT)
		{
			return;
		}

		
		if (m_outstandingProposals == 0)
		{
			
			
			
			if(!g_network.IsActive() ||
			   (g_network.IsHost() && g_network.IsLocalPlayer(m_playerId))) {
				g_director->AddBeginScheduler(m_playerId);
			}
		}
	}
	else
	{
	
		
		
		

		ChooseNewProposal(foreignerId);
	}
}


bool Diplomat::StartNegotiations(const PLAYER_INDEX hotseat_foreignerId)
{
	Player *player_ptr;
	player_ptr = g_player[m_playerId];
	Assert(player_ptr);

	Player *foreigner_ptr;

	uint32 foreignerId;
	bool found = false;

	
	m_outstandingProposals = 0;

	
	if(g_network.IsHost() && !g_network.IsLocalPlayer(m_playerId)) {
		
		return false;
	}

	
	for (foreignerId = 1; (unsigned) foreignerId < m_foreigners.size(); foreignerId++) 
	{
		
		if (foreignerId == m_playerId)
			continue;

		
		foreigner_ptr = g_player[foreignerId];
		if (foreigner_ptr == NULL)
			continue;

		
		if (g_turn->IsHotSeat() || g_turn->IsEmail())
		{
			
			if (hotseat_foreignerId != -1)
			{
				if (hotseat_foreignerId != foreignerId)
					continue;
			}
			
			else if (g_player[foreignerId]->m_playerType != PLAYER_TYPE_ROBOT)
				continue;
		}

		
	//	Modification by Peter Triggs
	//	from >=0 pt: 
	//	if player has contact with foreignerId, 
    //	set m_lastMotivation[foreignerId] = the player's top motivation 
	//	and choose a new proposal for foreignerId
		if (m_foreigners[foreignerId].GetTurnsSinceGreeting() >= -1 &&
			g_player[m_playerId] && g_player[m_playerId]->HasContactWith(foreignerId))
		{
			m_lastMotivation[foreignerId] = m_motivations.begin();

			DPRINTF(k_DBG_AI, ("(Start Negotiations)%d choosing proposal for foreigner %d\n",m_playerId,foreignerId));
			
			ChooseNewProposal(foreignerId);
			found = true;
		}
		else
		{
			m_lastMotivation[foreignerId] = m_motivations.end();
		}
	}

	return found;
}


bool Diplomat::InvalidNewProposal(const PLAYER_INDEX & foreignerId, const DiplomacyProposalRecord *rec) const
{
	if (foreignerId == -1)
		return true;

	
	if (rec->GetDisabled())
		return true;

	Player * player_ptr = g_player[m_playerId];
	Assert(player_ptr);
	if (player_ptr == NULL)
		return true;

	Player * foreigner_ptr = g_player[foreignerId];
	Assert(foreigner_ptr);
	if (foreigner_ptr == NULL)
		return true;

	
	if (rec->GetHasPollution() &&
		player_ptr->GetPollutionLevel() <= 0 &&
		foreigner_ptr->GetPollutionLevel() <= 0)
		return true;

	
	if (rec->GetHasEmbassy() &&
		!player_ptr->HasEmbassyWith(foreignerId))
		return true;

	
	if (rec->GetHasHotwar() &&
		!AgreementMatrix::s_agreements.HasAgreement(m_playerId, foreignerId, PROPOSAL_TREATY_DECLARE_WAR))
		return true;

	if (rec->GetNoHotwar() &&
		AgreementMatrix::s_agreements.HasAgreement(m_playerId, foreignerId, PROPOSAL_TREATY_DECLARE_WAR))
		return true;

	if (rec->GetHasPeaceTreaty() &&
		!AgreementMatrix::s_agreements.HasAgreement(m_playerId, foreignerId, PROPOSAL_TREATY_PEACE))
		return true;

	if (rec->GetHasAlliance() &&
		!AgreementMatrix::s_agreements.HasAgreement(m_playerId, foreignerId, PROPOSAL_TREATY_ALLIANCE))
		return true;

	if (rec->GetHasMilitaryAgreement() &&
		!AgreementMatrix::s_agreements.HasAgreement(m_playerId, foreignerId, PROPOSAL_TREATY_MILITARY_PACT) &&
		!AgreementMatrix::s_agreements.HasAgreement(m_playerId, foreignerId, PROPOSAL_TREATY_ALLIANCE))
		return true;

	if (rec->GetHasPollutionAgreement() &&
		!AgreementMatrix::s_agreements.HasAgreement(m_playerId, foreignerId, PROPOSAL_TREATY_POLLUTION_PACT))
		return true;

	
	if (rec->GetClassTreaty())
	{
		if ((rec == m_diplomacy[foreignerId].GetProposalElement(s_proposalTypeToElemIndex[PROPOSAL_TREATY_CEASEFIRE])->GetProposal()) &&
			AgreementMatrix::s_agreements.HasAgreement(m_playerId, foreignerId, PROPOSAL_TREATY_CEASEFIRE))
			return true;
		else if ((rec == m_diplomacy[foreignerId].GetProposalElement(s_proposalTypeToElemIndex[PROPOSAL_TREATY_PEACE])->GetProposal()) &&
			AgreementMatrix::s_agreements.HasAgreement(m_playerId, foreignerId, PROPOSAL_TREATY_PEACE))
			return true;
		else if ((rec == m_diplomacy[foreignerId].GetProposalElement(s_proposalTypeToElemIndex[PROPOSAL_TREATY_TRADE_PACT])->GetProposal()) &&
			AgreementMatrix::s_agreements.HasAgreement(m_playerId, foreignerId, PROPOSAL_TREATY_TRADE_PACT))
			return true;
		else if ((rec == m_diplomacy[foreignerId].GetProposalElement(s_proposalTypeToElemIndex[PROPOSAL_TREATY_MILITARY_PACT])->GetProposal()) &&
			AgreementMatrix::s_agreements.HasAgreement(m_playerId, foreignerId, PROPOSAL_TREATY_MILITARY_PACT))
			return true;
		else if ((rec == m_diplomacy[foreignerId].GetProposalElement(s_proposalTypeToElemIndex[PROPOSAL_TREATY_RESEARCH_PACT])->GetProposal()) &&
			AgreementMatrix::s_agreements.HasAgreement(m_playerId, foreignerId, PROPOSAL_TREATY_RESEARCH_PACT))
			return true;
		else if ((rec == m_diplomacy[foreignerId].GetProposalElement(s_proposalTypeToElemIndex[PROPOSAL_TREATY_POLLUTION_PACT])->GetProposal()) &&
			AgreementMatrix::s_agreements.HasAgreement(m_playerId, foreignerId, PROPOSAL_TREATY_POLLUTION_PACT))
			return true;
		else if ((rec == m_diplomacy[foreignerId].GetProposalElement(s_proposalTypeToElemIndex[PROPOSAL_TREATY_ALLIANCE])->GetProposal()) &&
			AgreementMatrix::s_agreements.HasAgreement(m_playerId, foreignerId, PROPOSAL_TREATY_ALLIANCE))
			return true;
	}

	const Diplomat & foreign_diplomat =	Diplomat::GetDiplomat(foreignerId);

	if (rec->GetCategoryGift())
	{
		
		if (rec->GetHasAlly() &&
			!AgreementMatrix::s_agreements.HasAgreement(foreignerId, PROPOSAL_TREATY_ALLIANCE))
			return true;
		
		if (rec->GetHasBorderIncursion() &&
			!foreign_diplomat.GetBorderIncursionBy(m_playerId))
			return true;

		if (rec->GetIsPirating() &&
			!MapAnalysis::GetMapAnalysis().GetMaxPiracyLoss(foreignerId))
			return true;

		if (rec->GetHasNuclearWeapons() &&
			MapAnalysis::GetMapAnalysis().GetNuclearWeaponsCount(m_playerId) <= 0)
			return true;

		if (rec->GetHasBioWeapons() &&
			MapAnalysis::GetMapAnalysis().GetBioWeaponsCount(m_playerId) <= 0)
			return true;

		if (rec->GetHasNanoWeapons() &&
			MapAnalysis::GetMapAnalysis().GetNanoWeaponsCount(m_playerId) <= 0)
			return true;

		if (rec->GetHasEmbargo() &&
			GetEmbargo(foreignerId) == false)
			return true;
	}
	else if (rec->GetCategoryRequest())
	{
		
		if (rec->GetHasAlly() &&
			!AgreementMatrix::s_agreements.HasAgreement(m_playerId, PROPOSAL_TREATY_ALLIANCE))
			return true;

		
		
		
		

		if (rec->GetIsPirating() &&
			!MapAnalysis::GetMapAnalysis().GetMaxPiracyLoss(m_playerId))
			return true;

		if (rec->GetHasNuclearWeapons() &&
			MapAnalysis::GetMapAnalysis().GetNuclearWeaponsCount(foreignerId) <= 0)
			return true;

		if (rec->GetHasBioWeapons() &&
			MapAnalysis::GetMapAnalysis().GetBioWeaponsCount(foreignerId) <= 0)
			return true;

		if (rec->GetHasNanoWeapons() &&
			MapAnalysis::GetMapAnalysis().GetNanoWeaponsCount(foreignerId) <= 0)
			return true;

		if (rec->GetHasEmbargo() &&
			foreign_diplomat.GetEmbargo(m_playerId) == false)
			return true;
	}
	else
	{
		
	}
	return false;
}


void Diplomat::ChooseNewProposal(const PLAYER_INDEX & foreignerId) {

	
	

	DPRINTF(k_DBG_DIPLOMACY,("Player %d choosing new proposal for player %d\n",m_playerId, foreignerId));

	
	SetMyLastNewProposal( foreignerId, Diplomat::s_badNewProposal );

	
	SetReceiverHasInitiative(foreignerId, false);

	MotivationList::iterator motivation_iter = m_lastMotivation[foreignerId];

	
	if (motivation_iter == m_motivations.end())
		{
			DPRINTF(k_DBG_DIPLOMACY,("  No Motivations.\n"));
		}
	else
		{
			Motivation topMotivation = *motivation_iter;
			GAME_EVENT motivation_event = static_cast<GAME_EVENT>(0);
	
			switch (topMotivation.type)
			{
			case MOTIVATION_NONE:
				
				Assert(0);
				break;
			
			case MOTIVATION_FEAR_SCENARIO:
			case MOTIVATION_FEAR_INVASION:
			case MOTIVATION_FEAR_CITY_DEFENSE:
			case MOTIVATION_FEAR_PIRACY:
			case MOTIVATION_FEAR_SPECIAL_ATTACKS:
			case MOTIVATION_FEAR_ADVANCE:
			case MOTIVATION_FEAR_SPYING:
			case MOTIVATION_FEAR_SCIENCE_RANK:
			case MOTIVATION_FEAR_MILITARY_RANK:
			case MOTIVATION_FEAR_TRADE_RANK:
			case MOTIVATION_FEAR_POLLUTION:
				motivation_event = GEV_FearMotivation;
				break;
			
			case MOTIVATION_DESIRE_SCENARIO:
			case MOTIVATION_DESIRE_ATTACK:
			case MOTIVATION_DESIRE_ATTACK_CITY:
			case MOTIVATION_DESIRE_ADVANCE:
			case MOTIVATION_DESIRE_TRADE:
			case MOTIVATION_DESIRE_GROWTH:
			case MOTIVATION_DESIRE_GOLD:
			case MOTIVATION_DESIRE_INTIMIDATE:
			case MOTIVATION_DESIRE_MAKE_FRIEND:
			case MOTIVATION_DESIRE_ENLIST_FRIEND:
				motivation_event = GEV_DesireMotivation;
				break;
			case MOTIVATION_MAX:
				
				Assert(0);
			}

			DPRINTF(k_DBG_DIPLOMACY,("  Player %d has top motivation: %s.\n",
									 m_playerId, s_motivationNames[topMotivation.type].c_str()));

			
			g_gevManager->AddEvent(GEV_INSERT_Tail, motivation_event,
								   GEA_Player, m_playerId,
								   GEA_Player, foreignerId,
								   GEA_Int, static_cast<sint32>(topMotivation.type),
								   GEA_End);

		}

	
	
	g_gevManager->AddEvent(GEV_INSERT_Tail, GEV_ReactionMotivation,
		GEA_Player, m_playerId,
		GEA_Player, foreignerId,
		GEA_End);

	
	m_outstandingProposals++;
}



void Diplomat::ConsiderNewProposal( const PLAYER_INDEX & foreignerId,
									const NewProposal & newProposal ) {

	Assert(newProposal.senderId != -1);
	Assert(newProposal.receiverId != -1);

	
	if (!CanAfford(newProposal.senderId, -1, newProposal.detail))
		return;

	
	
	m_foreigners[foreignerId].ConsiderNewProposal(newProposal);

	DPRINTF(k_DBG_DIPLOMACY,("  Player %d considers New Proposal for player %d: first = %s .\n",
			m_playerId, foreignerId, s_proposalNames[newProposal.detail.first_type].c_str()));
	DPRINTF(k_DBG_DIPLOMACY,("                                                  second = %s.\n",
			s_proposalNames[newProposal.detail.second_type].c_str()));

}


const NewProposal & Diplomat::GetMyLastNewProposal(const PLAYER_INDEX & foreignerId) const {
	return (m_foreigners[foreignerId].GetMyLastNewProposal());
}


void Diplomat::SetMyLastNewProposal( const PLAYER_INDEX & foreignerId, 
									 const NewProposal & newProposal ) {
	m_foreigners[foreignerId].SetMyLastNewProposal(newProposal);

	DPRINTF(k_DBG_DIPLOMACY,("  Player %d sets New Proposal for player %d: first = %s .\n",
			m_playerId, foreignerId, s_proposalNames[newProposal.detail.first_type].c_str()));
	DPRINTF(k_DBG_DIPLOMACY,("                                                  second = %s.\n",
			s_proposalNames[newProposal.detail.second_type].c_str()));

}


void Diplomat::ExecuteNewProposal( const PLAYER_INDEX & receiver ) {

	
	

	
	SetMyLastResponse( receiver, Diplomat::s_badResponse );
	GetDiplomat(receiver).SetMyLastResponse(m_playerId, Diplomat::s_badResponse );
	
	
	SetReceiverHasInitiative(receiver, true);

	
	NewProposal proposal = m_foreigners[receiver].GetMyLastNewProposal();
	proposal.id = GetNextId();
	m_foreigners[receiver].SetMyLastNewProposal(proposal);

	DPRINTF(k_DBG_DIPLOMACY,("  Player %d executes New Proposal for player %d: first = %s .\n",
			m_playerId, receiver, s_proposalNames[GetMyLastNewProposal(receiver).detail.first_type].c_str()));
	DPRINTF(k_DBG_DIPLOMACY,("                                                  second = %s.\n",
			s_proposalNames[GetMyLastNewProposal(receiver).detail.second_type].c_str()));

	
	g_gevManager->AddEvent(GEV_INSERT_Tail, GEV_NewProposal,
			GEA_Player, m_playerId,
			GEA_Player, receiver,
			GEA_End);

#ifndef _BFR_
	ProposalAnalysis::LogDebugResult(proposal);
#endif // _BFR_
}


void Diplomat::SetMyLastNegotiatedProposal( const PLAYER_INDEX & foreignerId, 
										    const ProposalData & data, 
										    const RESPONSE_TYPE & response )
{
	m_foreigners[foreignerId].SetMyLastNegotiatedProposal(data,response);
}
	

RESPONSE_TYPE Diplomat::GetMyLastNegotiatedProposal( const PLAYER_INDEX & foreignerId, 
													 ProposalData & data ) const
{
	return m_foreigners[foreignerId].GetMyLastNegotiatedProposal(data);
}



bool Diplomat::GetNewProposalTimeout( const NewProposal & newProposal, const sint16 timeout_period ) const
{
	if (m_foreigners[newProposal.receiverId].GetNewProposalTimeout( newProposal, timeout_period))
		return true;
	return false;
}


const NegotiationEventList & Diplomat::GetNegotiationEvents( const PLAYER_INDEX & foreignerId ) const
{
	return m_foreigners[foreignerId].GetNegotiationEvents();
}


sint32 Diplomat::GetNewProposalPriority(const PLAYER_INDEX foreignerId, 
							  const PROPOSAL_TYPE proposalType ) const
{
	Assert(s_proposalTypeToElemIndex[proposalType] < m_diplomacy[foreignerId].GetNumProposalElement());
	Assert((unsigned) foreignerId < m_diplomacy.size() );
	
	const DiplomacyRecord::ProposalElement * elem =
		m_diplomacy[foreignerId].GetProposalElement(s_proposalTypeToElemIndex[proposalType]);

	
	const DiplomacyProposalRecord * rec = elem->GetProposal();
	if (InvalidNewProposal(foreignerId, rec))
		return -1;

	sint32 priority;
	if (elem->GetSendPriority())
	{
		elem->GetSendPriority(priority);
		return priority;
	}

	return -1;
}


sint32 Diplomat::GetAcceptPriority(const PLAYER_INDEX foreignerId, 
						 const PROPOSAL_TYPE proposalType ) const
{
	Assert(s_proposalTypeToElemIndex[proposalType] < m_diplomacy[foreignerId].GetNumProposalElement());
	Assert((unsigned) foreignerId < m_diplomacy.size() );
	
	const DiplomacyRecord::ProposalElement * elem =
		m_diplomacy[foreignerId].GetProposalElement(s_proposalTypeToElemIndex[proposalType]);
	sint32 value;
	elem->GetAcceptPriority(value);
	return value;
}


sint32 Diplomat::GetRejectPriority(const PLAYER_INDEX foreignerId, 
						 const PROPOSAL_TYPE proposalType ) const
{
	Assert(s_proposalTypeToElemIndex[proposalType] < m_diplomacy[foreignerId].GetNumProposalElement());
	Assert((unsigned) foreignerId < m_diplomacy.size() );
	
	const DiplomacyRecord::ProposalElement * elem =
		m_diplomacy[foreignerId].GetProposalElement(s_proposalTypeToElemIndex[proposalType]);
	
	sint32 value;
	elem->GetRejectPriority(value);
	return value;
}


sint32 Diplomat::GetSenderRegardResult(const PLAYER_INDEX foreignerId, 
							 const PROPOSAL_TYPE proposalType ) const
{
	Assert(s_proposalTypeToElemIndex[proposalType] < m_diplomacy[foreignerId].GetNumProposalElement());
	Assert((unsigned) foreignerId < m_diplomacy.size() );
	
	const DiplomacyRecord::ProposalElement * elem =
		m_diplomacy[foreignerId].GetProposalElement(s_proposalTypeToElemIndex[proposalType]);
	
	sint32 value;
	elem->GetSenderRegardResult(value);
	return value;
}


sint32 Diplomat::GetReceiverRegardResult(const PLAYER_INDEX foreignerId, 
							   const PROPOSAL_TYPE proposalType ) const
{
	Assert(s_proposalTypeToElemIndex[proposalType] < m_diplomacy[foreignerId].GetNumProposalElement());
	Assert((unsigned) foreignerId < m_diplomacy.size() );
	
	const DiplomacyRecord::ProposalElement * elem =
		m_diplomacy[foreignerId].GetProposalElement(s_proposalTypeToElemIndex[proposalType]);

	Assert(elem);
	if(!elem) return 0;

	sint32 value;
	elem->GetReceiverRegardResult(value);
	return value;
}


sint32 Diplomat::GetViolationRegardCost(const PLAYER_INDEX foreignerId, 
							  const PROPOSAL_TYPE proposalType ) const
{
	Assert(s_proposalTypeToElemIndex[proposalType] < m_diplomacy[foreignerId].GetNumProposalElement());
	Assert((unsigned) foreignerId < m_diplomacy.size() );
	
	const DiplomacyRecord::ProposalElement * elem =
		m_diplomacy[foreignerId].GetProposalElement(s_proposalTypeToElemIndex[proposalType]);

	sint32 value;
	elem->GetViolationRegardCost(value);
	return value;
}


sint32 Diplomat::GetViolationTrustCost(const PLAYER_INDEX foreignerId, 
							 const PROPOSAL_TYPE proposalType ) const
{
	Assert(s_proposalTypeToElemIndex[proposalType] < m_diplomacy[foreignerId].GetNumProposalElement());
	Assert((unsigned) foreignerId < m_diplomacy.size() );
	
	const DiplomacyRecord::ProposalElement * elem =
		m_diplomacy[foreignerId].GetProposalElement(s_proposalTypeToElemIndex[proposalType]);

	sint32 value;
	elem->GetViolationTrustCost(value);
	return value;
}




const NewProposal & Diplomat::GetNewProposalPending(const PLAYER_INDEX foreignerId) const
{
	const NewProposal & new_proposal = GetMyLastNewProposal(foreignerId);

	if (new_proposal != s_badNewProposal)
	{
		
		if (new_proposal.senderId == m_playerId &&
			GetReceiverHasInitiative(foreignerId) == false)
			return s_badNewProposal;

		
		
		if (new_proposal.receiverId == m_playerId &&
			GetDiplomat(new_proposal.senderId).GetReceiverHasInitiative(m_playerId) == true)
			return s_badNewProposal;
	}

	return new_proposal;
}


const NewProposal & Diplomat::GetNewProposalAdvice(const PLAYER_INDEX foreignerId) const
{
	const NewProposal & new_proposal = GetMyLastNewProposal(foreignerId);

	if (new_proposal != s_badNewProposal)
	{
		
		if (new_proposal.senderId == m_playerId &&
			GetReceiverHasInitiative(foreignerId) == true)
			return s_badNewProposal;

		
		
		if (new_proposal.receiverId == m_playerId &&
			GetDiplomat(new_proposal.senderId).GetReceiverHasInitiative(m_playerId) == false)
			return s_badNewProposal;
	}

	return new_proposal;
}


void Diplomat::ExecuteNewProposal(const NewProposal & proposal)
{
	if (proposal.senderId == m_playerId)
	{
		SetMyLastNewProposal(proposal.receiverId, proposal);

		
		
	}
	else if (proposal.receiverId == m_playerId)
	{
		
		Assert(0);
	}
	else
	{
		
		Assert(0);
		return;
	}

	
	ExecuteNewProposal(proposal.receiverId);
}







void Diplomat::InitStrategicState() {

	
	

	
	Assert(m_playerId >=0);

	
	m_bestStrategicStates.clear();

	
	if (GetPersonality()->GetIndex() == 0)
	{
		sint32 index;
		g_theStrategyDB->GetNamedItem("STRATEGY_BARBARIAN", index);
		SetStrategy(index);
	}
	else
	{

		
		g_gevManager->AddEvent(GEV_INSERT_Tail, GEV_InitStrategicState,
							   GEA_Player, m_playerId,
							   GEA_End);
	}

}


void Diplomat::NextStrategicState() {

	
	

	
	m_bestStrategicStates.clear();
		

	
	if (GetPersonality()->GetIndex() == 0)
	{
		sint32 index;
		g_theStrategyDB->GetNamedItem("STRATEGY_BARBARIAN", index);
		SetStrategy(index);
	}
	else
	{
		
		g_gevManager->AddEvent(GEV_INSERT_Tail, GEV_NextStrategicState,
							   GEA_Player, m_playerId,
							   GEA_End);
	}
}


void Diplomat::ConsiderStrategicState( const AiState & state ) {
	if (state.dbIndex < 0) 
	{
		// Nothing to merge. This may happen with the new strategy definition
		// when there are no nukes yet.
		return;
	}

	AiStateList::iterator ai_state_iter = 
		m_bestStrategicStates.begin();

	
	while (ai_state_iter != m_bestStrategicStates.end())
	{
		if (ai_state_iter->priority > state.priority)
			break;
		ai_state_iter++;
	}

	
	if (ai_state_iter == m_bestStrategicStates.end())
	{
		m_bestStrategicStates.push_back(state);
	}
	
	else if (ai_state_iter == m_bestStrategicStates.begin())
	{
		m_bestStrategicStates.push_front(state);
	}
	
	else
	{
		ai_state_iter--;
		m_bestStrategicStates.insert(ai_state_iter, state);
	}

	
	if (m_bestStrategicStates.size() > k_maxStategicState)
		m_bestStrategicStates.pop_front();
}


void Diplomat::ComputeCurrentStrategy()
{
	
	AiStateList::const_iterator ai_state_iter = m_bestStrategicStates.begin();

	while (ai_state_iter != m_bestStrategicStates.end())
	{
		
		Assert(ai_state_iter->dbIndex >= 0);
		Assert(ai_state_iter->dbIndex < g_theStrategyDB->NumRecords());
		if ((ai_state_iter->dbIndex < 0) ||
			(ai_state_iter->dbIndex >= g_theStrategyDB->NumRecords()))
			break;

		DPRINTF(k_DBG_AI, ("    merging in strategy %s, priority = %d.\n",
			g_theStrategyDB->Get(ai_state_iter->dbIndex)->GetNameText(),
			ai_state_iter->priority));

		MergeStrategy(ai_state_iter->dbIndex);

		
		ai_state_iter++;
	}
}


const StrategyRecord & Diplomat::GetCurrentStrategy() const {
      // Relaxed assert (when loading a file)
	Assert(m_strategy.GetIndex() >= -1);
	return m_strategy;
}


void Diplomat::SetStrategy(const sint32 index) {
	const StrategyRecord *strategy = g_theStrategyDB->Get(index);
	
	if (strategy && strategy->GetNumInherit() > 0) {
		const StrategyRecord *inherit_strategy = strategy->GetInherit(0);
		
		SetStrategy(inherit_strategy->GetIndex());
		m_strategy.Merge(*strategy);
	}
	else if(strategy) {
		
		m_strategy = *strategy;
	}
}


void Diplomat::MergeStrategy(const sint32 index) {
	const StrategyRecord *strategy = g_theStrategyDB->Get(index);
	
	if (strategy && strategy->GetNumInherit() > 0) {
		const StrategyRecord *inherit_strategy = strategy->GetInherit(0);
		
		MergeStrategy(inherit_strategy->GetIndex());
		m_strategy.Merge(*strategy);
	}
	else if(strategy) {
		
		m_strategy.Merge(*strategy);
	}
}







void Diplomat::InitDiplomaticState( const PLAYER_INDEX & foreignerId ) {

	
	m_bestDiplomaticState = s_badAiState;
	m_diplomaticStates[foreignerId] = s_badAiState;

	
	if (g_player[foreignerId] == NULL || g_player[foreignerId]->IsDead())
		return;

	
	g_gevManager->AddEvent(GEV_INSERT_Tail, GEV_InitDiplomaticState,
			GEA_Player, m_playerId,
			GEA_Player, foreignerId,
			GEA_End);
}


void Diplomat::NextDiplomaticState( const PLAYER_INDEX & foreignerId ) {

	
	
	
	m_bestDiplomaticState = s_badAiState;

	
	g_gevManager->AddEvent(GEV_INSERT_Tail, GEV_NextDiplomaticState,
			GEA_Player, m_playerId,
			GEA_Player, foreignerId,
			GEA_End);

    // added by PFT 05 MAR 05: some agreements have limited duration
    sint32 duration;
    sint32 send_warning;
    sint32 cancel_agt;

	for (sint32 prop_type=1; prop_type<PROPOSAL_MAX; prop_type++){

	    if (AgreementMatrix::s_agreements.HasAgreement(m_playerId, foreignerId, (const enum PROPOSAL_TYPE)prop_type)){

			duration=AgreementMatrix::s_agreements.GetAgreementDuration(m_playerId, foreignerId, (const enum PROPOSAL_TYPE)prop_type);
				
			send_warning=0;
            cancel_agt=0;
			switch (prop_type) {
	            case PROPOSAL_NONE:
	            case PROPOSAL_OFFER_GIVE_CITY:
	            case PROPOSAL_REQUEST_GIVE_CITY:
		            break;

	            case PROPOSAL_OFFER_WITHDRAW_TROOPS:
                case PROPOSAL_REQUEST_WITHDRAW_TROOPS:
                case PROPOSAL_OFFER_STOP_PIRACY:
	            case PROPOSAL_REQUEST_STOP_PIRACY:
					if(duration==17){send_warning=1;}
                    else if (duration==20){cancel_agt=1;}
		            break;

	            case PROPOSAL_OFFER_BREAK_AGREEMENT:
		        case PROPOSAL_REQUEST_BREAK_AGREEMENT:
		            break;
	
	            case PROPOSAL_OFFER_STOP_RESEARCH:
	            case PROPOSAL_REQUEST_STOP_RESEARCH:
		        case PROPOSAL_OFFER_REDUCE_NUCLEAR_WEAPONS:
                case PROPOSAL_REQUEST_REDUCE_NUCLEAR_WEAPONS:
                case PROPOSAL_OFFER_REDUCE_BIO_WEAPONS:
                case PROPOSAL_REQUEST_REDUCE_BIO_WEAPONS:
                case PROPOSAL_OFFER_REDUCE_NANO_WEAPONS:
                case PROPOSAL_REQUEST_REDUCE_NANO_WEAPONS:
                    if(duration==27){send_warning=1;}
                    else if (duration==30){cancel_agt=1;}
		            break;

	            case PROPOSAL_OFFER_GIVE_ADVANCE:
	            case PROPOSAL_REQUEST_GIVE_ADVANCE:
	            case PROPOSAL_OFFER_GIVE_GOLD:
	            case PROPOSAL_REQUEST_GIVE_GOLD:
	            case PROPOSAL_OFFER_REDUCE_POLLUTION:
	            case PROPOSAL_REQUEST_REDUCE_POLLUTION:		
	            case PROPOSAL_OFFER_MAP:
	            case PROPOSAL_REQUEST_MAP:
	            case PROPOSAL_OFFER_HONOR_MILITARY_AGREEMENT:
	            case PROPOSAL_REQUEST_HONOR_MILITARY_AGREEMENT:
	            case PROPOSAL_OFFER_HONOR_POLLUTION_AGREEMENT:
	            case PROPOSAL_REQUEST_HONOR_POLLUTION_AGREEMENT:   		
	            case PROPOSAL_OFFER_END_EMBARGO:// possible term ?
	            case PROPOSAL_REQUEST_END_EMBARGO:
		            break;
	
	            case PROPOSAL_TREATY_DECLARE_WAR:
		            break;
	            case PROPOSAL_TREATY_CEASEFIRE:
                    if(duration==17){send_warning=1;}
                    else if (duration==20){cancel_agt=1;}
		            break;
	            case PROPOSAL_TREATY_PEACE:

	            case PROPOSAL_TREATY_TRADE_PACT:
	            case PROPOSAL_TREATY_RESEARCH_PACT:
	            case PROPOSAL_TREATY_MILITARY_PACT:
	            case PROPOSAL_TREATY_POLLUTION_PACT:
                    if(duration==37){send_warning=1;}
                    else if (duration==40){cancel_agt=1;}
		            break;

	            case PROPOSAL_TREATY_ALLIANCE:// possible term ?
		            break;
	            case PROPOSAL_MAX:
	            default:
		            Assert(0);
			}
			if(send_warning){
                SlicObject *so = new SlicObject("001TreatyToExpire");
				so->AddRecipient(m_playerId);
				so->AddCivilisation(foreignerId);
			    g_slicEngine->Execute(so);
			}
			else if (cancel_agt){

                AgreementMatrix::s_agreements.CancelAgreement(m_playerId, foreignerId, (const enum PROPOSAL_TYPE)prop_type);
			}
        }
	}// end: some aggreements have limited duration
}


void Diplomat::ConsiderDiplomaticState( const PLAYER_INDEX & foreignerId, const AiState & state ) {
	
	
	if (m_bestDiplomaticState.priority < state.priority) 
		m_bestDiplomaticState = state;
}
	

const AiState & Diplomat::GetBestDiplomaticState() const {
	return m_bestDiplomaticState;
}


const AiState & Diplomat::GetCurrentDiplomaticState( const PLAYER_INDEX & foreignerId ) const {
	return m_diplomaticStates[foreignerId];
}


void Diplomat::SetDiplomaticState(const PLAYER_INDEX & foreignerId, const AiState & newState ) {
	if (newState != s_badAiState) {
		m_diplomaticStates[foreignerId] = newState;

		
		Assert(newState.dbIndex >= 0);
		if (newState.dbIndex >= 0)
			ChangeDiplomacy(foreignerId, newState.dbIndex);
	}

	
	if (g_player[m_playerId] && 
		(g_player[m_playerId]->GetPlayerType() == PLAYER_TYPE_ROBOT &&
		(!g_network.IsActive() || g_network.IsLocalPlayer(m_playerId))))
	{
		bool declare_war = true;
		if (!AgreementMatrix::s_agreements.HasAgreement(m_playerId, foreignerId, PROPOSAL_TREATY_DECLARE_WAR))
		{
			declare_war = false;

			
			
			sint32 turns_since_last_war = 
				AgreementMatrix::s_agreements.TurnsSinceLastWar(m_playerId, foreignerId);

			
			declare_war = TestPublicRegard(foreignerId, HOTWAR_REGARD);
			
			declare_war = declare_war && (turns_since_last_war > 5 || turns_since_last_war < 0);
			
			declare_war = declare_war && (AtWarCount() == 0);
			
			declare_war = declare_war && DesireWarWith(foreignerId);
			
			declare_war = declare_war && (m_personality->GetTrustworthinessChaotic() == false);
			
			
			Threat war_threat;
			if (HasThreat(foreignerId, THREAT_DECLARE_WAR, war_threat))
			{
				if (m_personality->GetTrustworthinessLawful() ||
					g_rand->Next(100) < m_personality->GetThreatFollowThrough() * 100)
				{
					declare_war = true;
					RemoveThreatById(war_threat.id);
				}
			}

			
			if (declare_war && 			
				wonderutil_GetCloseEmbassies(g_player[foreignerId]->GetBuiltWonders()))
			{
				declare_war = false;
			}

			if (declare_war) 
			{
				if(g_network.IsClient() && g_network.IsLocalPlayer(m_playerId)) {
					g_network.SendAction(new NetAction(NET_ACTION_DECLARE_WAR, foreignerId));
				}
				
				DeclareWar(foreignerId);
			}
		}
		
		
		Threat embargo_threat;
		if (HasThreat(foreignerId, THREAT_TRADE_EMBARGO, embargo_threat))
		{
			if (!GetEmbargo(foreignerId))
			{
				if (m_personality->GetTrustworthinessLawful() ||
					g_rand->Next(100) < m_personality->GetThreatFollowThrough() * 100)
				{
					SetEmbargo(foreignerId, true);
				}
			}
			
			RemoveThreatById(embargo_threat.id);
		}

		Threat nuke_threat;
		if (HasThreat(foreignerId, THREAT_DESTROY_CITY, nuke_threat))
		{
			if (declare_war && 
				MapAnalysis::GetMapAnalysis().GetNuclearWeaponsCount(m_playerId) &&
				g_theUnitPool->IsValid(nuke_threat.detail.arg.cityId))
			{
				
				if (GetDiplomat(nuke_threat.receiverId).FearNukesFrom(m_playerId) ||
					g_rand->Next(100) < m_personality->GetThreatFollowThrough() * 100)
				{
					
					LaunchNuclearAttackOnCity(nuke_threat.detail.arg.cityId);
				}
			}

			
			RemoveThreatById(nuke_threat.id);
		}
	} 
}


const DiplomacyRecord & Diplomat::GetCurrentDiplomacy(const PLAYER_INDEX & foreignerId) const {
	return m_diplomacy[foreignerId];
}


void Diplomat::ChangeDiplomacy(const PLAYER_INDEX & foreignerId, const sint32 index) {

	
	if (index < 0)
		return;

	const DiplomacyRecord *diplomacy = g_theDiplomacyDB->Get(index);
	if (diplomacy->GetNumInherit() > 0) {
		const DiplomacyRecord *inherit_diplomacy = diplomacy->GetInherit(0);
		
		ChangeDiplomacy(foreignerId, inherit_diplomacy->GetIndex());
		m_diplomacy[foreignerId].Merge(*diplomacy);
	}
	else {
		
		m_diplomacy[foreignerId] = *diplomacy;
	}
}






sint32 Diplomat::GetNextAdvance() const
{
	if (g_player[m_playerId] == NULL)
		return -1;

	
	const AdvanceListRecord *rec = m_strategy.GetResearchPtr();
	if (rec == NULL)
		return -1;

	
	sint32 advance_index = -1;
	sint32 wanted_advance;
	for (wanted_advance = 0; wanted_advance < rec->GetNumAdvance(); wanted_advance++)
	{
		advance_index = rec->GetAdvance(wanted_advance)->GetIndex();

		
		if (g_player[m_playerId]->HasAdvance(advance_index) == TRUE)
			continue;

		
		break;
	}

	if (wanted_advance == rec->GetNumAdvance())
	{
		
		return -1;
	}

	
	return advance_index;
}


sint32 Diplomat::GetDesiredAdvanceFrom( const PLAYER_INDEX & foreignerId, const sint32 min_cost, const sint32 max_cost ) const
{

	if (g_player[m_playerId] == NULL)
		return -1;

	if (g_player[foreignerId] == NULL)
		return -1;

	
	const AdvanceListRecord *rec = m_strategy.GetResearchPtr();
	if (rec == NULL)
		return -1;

	
	sint32 advance_index = -1;
	sint32 wanted_advance;
	for (wanted_advance = 0; wanted_advance < rec->GetNumAdvance(); wanted_advance++)
	{

		advance_index = rec->GetAdvance(wanted_advance)->GetIndex();

		
		if (rec->GetAdvance(wanted_advance)->GetCost() > max_cost)
			continue;

		
		if (g_player[m_playerId]->HasAdvance(advance_index) == TRUE)
			continue;

		
		if (g_player[foreignerId]->HasAdvance(advance_index) == FALSE)
			continue;

		
		break;
	}

	if (wanted_advance == rec->GetNumAdvance())
	{
		
		return -1;
	}

	
	return advance_index;
}


sint32 Diplomat::GetStopResearchingAdvance( const PLAYER_INDEX & foreignerId) const
{
	
	Assert(g_player[m_playerId]);
	Assert(g_player[foreignerId]);

	if (g_player[m_playerId] == NULL)
		return -1;

	if (g_player[foreignerId] == NULL)
		return -1;

	
	AdvanceType foreigner_research = g_player[foreignerId]->m_advances->GetResearching();
	AdvanceType my_research = g_player[m_playerId]->m_advances->GetResearching();

	
	const AdvanceListRecord *rec = m_strategy.GetStopResearchPtr();
	if (rec == NULL)
		return -1;

	
	sint32 max_cost = g_theAdvanceDB->Get(my_research)->GetCost();

	
	sint32 advance_index = -1;
	for (sint32 stop_advance = 0; stop_advance < rec->GetNumAdvance(); stop_advance++)
	{
		
		if (rec->GetAdvance(stop_advance)->GetIndex() != foreigner_research)
			continue;

		
		if (rec->GetAdvance(stop_advance)->GetCost() > max_cost)
			continue;

		advance_index = rec->GetAdvance(stop_advance)->GetIndex();
		break;
	}

	return advance_index;
}


sint32 Diplomat::GetNextKeyAdvance() const
{
	
	Assert(g_player[m_playerId]);

	if (g_player[m_playerId] == NULL)
		return -1;

	
	AdvanceType my_research = g_player[m_playerId]->m_advances->GetResearching();

	
	
	const AdvanceRecord *rec;
	sint32 advance_index;
	for (advance_index = 0; advance_index < g_theAdvanceDB->NumRecords(); advance_index++)
	{
		rec = g_theAdvanceDB->Get(advance_index);

		
		if (!rec->GetKeyAdvanceAdvice())
			continue;
		
		
		if (g_player[m_playerId]->HasAdvance(advance_index) == TRUE)
			continue;

		break;
	}

	
	if (advance_index < g_theAdvanceDB->NumRecords())
		return -1;

	return advance_index;
}


StringId Diplomat::GetScienceAdvice(SlicContext & sc, StringId & advance_advice)
{
	static StringId enemyHasAdvanceAdvice = -1;
	static StringId allyHasAdvanceAdvice = -1;
	static StringId keyAdvanceAdvice = -1;
	static StringId	nonProliferationAdvice = -1;
	static StringId	manhattanProjectAdvice = -1;

	stringutils_SetStaticStringId(enemyHasAdvanceAdvice, "ENEMY_HAS_ADVANCE_ADVICE");	
	stringutils_SetStaticStringId(allyHasAdvanceAdvice, "ALLY_HAS_ADVANCE_ADVICE");	
	stringutils_SetStaticStringId(keyAdvanceAdvice, "KEY_ADVANCE_ADVICE");
	stringutils_SetStaticStringId(nonProliferationAdvice, "NON_PROLIFERATION_ADVICE");
	stringutils_SetStaticStringId(manhattanProjectAdvice, "MANHATTAN_PROJECT_ADVICE");

	
	advance_advice = -1;

	
	sint32 stop_researching_adv;
	uint32 foreignerId;
	for (foreignerId = 1; foreignerId < s_theDiplomats.size(); foreignerId)
		{
			
			if (TestEffectiveRegard(foreignerId, ALLIED_REGARD ))
				continue;

			stop_researching_adv = GetStopResearchingAdvance(foreignerId);
			
			if (stop_researching_adv >= 0)
				{
					sc.AddPlayer(foreignerId);
					sc.AddAdvance(stop_researching_adv);

					
					if (g_theAdvanceDB->Get(stop_researching_adv)->GetKeyAdvanceAdvice())
						g_theAdvanceDB->Get(stop_researching_adv)->GetKeyAdvanceAdvice(advance_advice);

					
					if (g_player[m_playerId]->HasAdvance(stop_researching_adv) == TRUE)
						{
							return nonProliferationAdvice;
						}
					else
						{
							return manhattanProjectAdvice;
						}
				}
		}

	
	sint32 next_key_adv = GetNextKeyAdvance();

	
	if (next_key_adv != -1)
	{
		
		sc.AddAdvance(next_key_adv);

		
		if (g_theAdvanceDB->Get(next_key_adv)->GetKeyAdvanceAdvice())
			g_theAdvanceDB->Get(next_key_adv)->GetKeyAdvanceAdvice(advance_advice);
		
		
		ai::Regard best_enemy_regard = MAX_REGARD;
		PLAYER_INDEX best_enemy = -1;
		
		
		ai::Regard best_friend_regard = MIN_REGARD;
		PLAYER_INDEX best_friend = -1;

		sint32 unit_type;
		for (unit_type = 0; unit_type < g_theUnitDB->NumRecords(); unit_type++)
		{
			const UnitRecord *unit_rec = g_theUnitDB->Get(unit_type);
			if (unit_rec->GetEnableAdvance() &&
				unit_rec->GetEnableAdvance()->GetIndex() == next_key_adv)
				break;
		}

		
		if (unit_type < g_theUnitDB->NumRecords())
		{
			sc.AddUnitRecord(unit_type);
		}
		else
		{
			
			sc.AddUnitRecord(0);
		}
		
		
		ai::Regard regard;
		for (foreignerId = 1; (unsigned) foreignerId < s_theDiplomats.size(); foreignerId++)
		{
			regard = GetEffectiveRegard(foreignerId);
			
			
			if ((g_player[foreignerId]->HasAdvance(next_key_adv) == TRUE) &&
				(regard < best_enemy_regard) &&
				TestEffectiveRegard(foreignerId, COLDWAR_REGARD))
			{
				best_enemy_regard = regard;
				best_enemy = foreignerId;
			}
			
			
			if ((g_player[foreignerId]->HasAdvance(next_key_adv) == TRUE) &&
				(regard > best_friend_regard) &&
				TestEffectiveRegard(foreignerId, NEUTRAL_REGARD))
			{
				best_friend_regard = regard;
				best_friend = foreignerId;
			}
		}
		
		
		if (best_friend != -1)
		{
			sc.AddPlayer(best_friend);
			
			return allyHasAdvanceAdvice;
		}
		
		else if (best_enemy != -1)
		{
			sc.AddPlayer(best_enemy);
			
			return enemyHasAdvanceAdvice;
		}
		else
		{
			return keyAdvanceAdvice;
		}
	}

	

	

	return -1;
}




	

sint32 Diplomat::GetTradeFrom(const PLAYER_INDEX & foreignId) const 
{
	return m_foreigners[foreignId].GetTradeFrom();
}


sint32 Diplomat::GetTributeFrom(const PLAYER_INDEX & foreignId) const 
{
	return m_foreigners[foreignId].GetTributeFrom();
}


sint32 Diplomat::GetGoldSurplusPercent() const 
{
	Assert(g_player[m_playerId] != NULL);
	Assert(g_player[m_playerId]->m_gold != NULL);
    sint32 lost_to_cleric;
	sint32 lost_to_crime;
	sint32 maintenance;
	sint32 wages; 
    sint32 science; 
	sint32 old_savings; 
	sint32 current_savings;
	sint32 income;
    g_player[m_playerId]->m_gold->
		GetGoldLevels(&income, &lost_to_cleric, &lost_to_crime, &maintenance, 
					  &wages, &science, &old_savings, &current_savings); 

	sint32 goldSpent = lost_to_cleric + lost_to_crime + maintenance + wages +
		science;
	return static_cast<sint32>(floor( (current_savings / goldSpent) * 100.0 ));
}


bool Diplomat::CanBuySurplus(const PLAYER_INDEX &foreignId) const {
	sint32 goldReserve = 100; 
	sint32 minSurplusCost = 50; 
	return ((minSurplusCost > 0) && (minSurplusCost > goldReserve));
}


sint32 Diplomat::GetAdvanceLevelPercent(const PLAYER_INDEX &foreignId) const {
	sint32 foreign_advances;
	if (g_player[foreignId] != NULL && (g_player[foreignId]->IsDead() == FALSE) )
		
		foreign_advances = g_player[foreignId]->NumAdvances();
	else 
		return 0;
	Assert(g_player[m_playerId] != NULL);
	sint32 my_advances = g_player[m_playerId]->NumAdvances();
	return static_cast<sint32>(floor((foreign_advances/ my_advances) * 100.0));
}


sint16 Diplomat::AtWarCount() const {
	sint16 atWarCount = 0;
	for (uint32 foreigner = 1; foreigner < m_foreigners.size(); foreigner++)
		atWarCount += 
			AgreementMatrix::s_agreements.HasAgreement(m_playerId, foreigner,PROPOSAL_TREATY_DECLARE_WAR);
	return atWarCount;
}


sint16 Diplomat::EffectiveAtWarCount() {
	int atWarCount = 0;
	for (uint32 foreigner = 0; foreigner < m_foreigners.size(); foreigner++)
		atWarCount += 
			( TestEffectiveRegard(foreigner, HOTWAR_REGARD ) );
	return atWarCount;
}


bool Diplomat::TestPublicRegard(const PLAYER_INDEX & foreignerId, const ai::Regard & test_regard) const
{
	ai::Regard regard = GetPublicRegard(foreignerId);

	if ( test_regard <= HOTWAR_REGARD )
		return ( regard <= HOTWAR_REGARD );
	else if ( test_regard <= COLDWAR_REGARD )
		return ( regard <= COLDWAR_REGARD );
	else if ( test_regard < FRIEND_REGARD )
		return ( regard > COLDWAR_REGARD );
	else if ( test_regard >= ALLIED_REGARD )
		return ( regard >= ALLIED_REGARD );
	else if ( test_regard >= FRIEND_REGARD )
		return ( regard >= FRIEND_REGARD );
	return true;
}







bool Diplomat::TestEffectiveRegard(const PLAYER_INDEX & foreignerId, const ai::Regard & test_regard)
{
	const sint32 curRound = NewTurnCount::GetCurrentRound();
	static int s_regardList[7] = 
	{MIN_REGARD,HOTWAR_REGARD,COLDWAR_REGARD,NEUTRAL_REGARD,FRIEND_REGARD,ALLIED_REGARD,MAX_REGARD};

	cEffectiveRegardEntry *pEntry;
	pEntry = &(m_effectiveRegardCache[foreignerId]);

	if (pEntry->m_round != curRound) {
		
		pEntry->m_round = curRound;
		pEntry->m_bits = 0;
		
		for (int i=0;i<7;++i) {
			int regard = s_regardList[i];
			if (ComputeEffectiveRegard(foreignerId,regard)) {
				pEntry->m_bits |= (1<<pEntry->RegardToIndex(regard));
			}
		}
	}
	const bool retVal = (pEntry->m_bits>>pEntry->RegardToIndex(test_regard))&1;

#ifdef _DEBUG
	
	
	
	
	
	
	
	
	
	const bool test = ComputeEffectiveRegard(foreignerId,test_regard);
	
#endif

	return retVal;
}

void Diplomat::ClearEffectiveRegardCache()
{
	int i;
	for (i=0;i<k_MAX_PLAYERS;++i) {
		m_effectiveRegardCache[i].m_round=-666;
	}
}


bool Diplomat::ComputeEffectiveRegard(const PLAYER_INDEX & foreignerId, const ai::Regard & test_regard) const
{
	ai::Regard regard = m_foreigners[foreignerId].GetEffectiveRegard();

	if (AgreementMatrix::s_agreements.HasAgreement(m_playerId, 
		foreignerId, 
		PROPOSAL_TREATY_DECLARE_WAR))
	{
		
		if (test_regard < NEUTRAL_REGARD)
			return true;
		
		else
			return false;
	}

	if ( test_regard <= HOTWAR_REGARD )
	{
		if ( regard <= HOTWAR_REGARD )
		{
			if (!DesireWarWith(foreignerId))
			{
				
				if (AgreementMatrix::s_agreements.HasAgreement(m_playerId, 
					foreignerId, 
					PROPOSAL_TREATY_CEASEFIRE))
					return false;
				
				
				if (AgreementMatrix::s_agreements.HasAgreement(m_playerId, 
					foreignerId, 
					PROPOSAL_TREATY_PEACE))
					return false;
			}

			return true;
		}
		return false;
	}
	else if ( test_regard <= COLDWAR_REGARD )
	{
		if ( regard <= COLDWAR_REGARD )
		{
			if (!DesireWarWith(foreignerId))
			{
				
				if (AgreementMatrix::s_agreements.HasAgreement(m_playerId, 
					foreignerId, 
					PROPOSAL_TREATY_PEACE))
					return false;
			}

			return true;
		}
		return false;
	}
	else if ( test_regard < FRIEND_REGARD )
		return ( regard > COLDWAR_REGARD );
	else if ( test_regard >= ALLIED_REGARD )
		return ( regard >= ALLIED_REGARD );
	else if ( test_regard >= FRIEND_REGARD )
		return ( regard >= FRIEND_REGARD );
	else if ( test_regard > COLDWAR_REGARD )
		return ( regard > COLDWAR_REGARD );
	return true;
}

bool Diplomat::TestAlliedRegard(const PLAYER_INDEX & foreignerId)
{
	
	
	Diplomat & diplomat = Diplomat::GetDiplomat(m_playerId);
	return  foreignerId == m_playerId ||
		 diplomat.TestEffectiveRegard(foreignerId, ALLIED_REGARD) ||
		 AgreementMatrix::s_agreements.HasAgreement(m_playerId, foreignerId, PROPOSAL_TREATY_ALLIANCE) ||
		 AgreementMatrix::s_agreements.HasAgreement(m_playerId, foreignerId, PROPOSAL_TREATY_MILITARY_PACT);
}




bool Diplomat::GetBorderIncursionBy(const PLAYER_INDEX & foreignerId) const
{
	return (m_foreigners[foreignerId].GetLastIncursion() == NewTurnCount::GetCurrentRound());
}


void Diplomat::SetBorderIncursionBy(const PLAYER_INDEX & foreignerId)
{
	m_foreigners[foreignerId].
		SetLastIncursion(NewTurnCount::GetCurrentRound());
}


void Diplomat::SetBorderPulloutBy(const PLAYER_INDEX & foreignerId)
{
	m_foreigners[foreignerId].
		SetLastIncursion(NewTurnCount::GetCurrentRound()-1);
}


sint32 Diplomat::GetLastBorderIncursionBy(const PLAYER_INDEX & foreignerId) const
{
	return m_foreigners[foreignerId].GetLastIncursion();
}


sint8 Diplomat::GetFriendCount() const
{
	return m_friendCount;
}


sint8 Diplomat::GetEnemyCount() const
{
	return m_enemyCount;
}


sint32 Diplomat::GetFriendPower() const
{
	return m_friendPower;
}


sint32 Diplomat::GetEnemyThreat() const
{
	return m_enemyThreat;
}


void Diplomat::UpdateAttributes() 
{
	
	
	Army army;
	sint32 cell_owner;
	sint32 num_armies;
	MapPoint pos;
	bool isspecial,cancapture,haszoc,canbombard;
	bool isstealth;
	sint32 maxattack,maxdefense;
	
	Player *player_ptr = g_player[m_playerId];
	Assert(player_ptr);
	Assert(g_theArmyPool);
	
	
	m_friendCount = 0;
	m_enemyCount = 0;
	
	m_friendPower = 0;
	m_enemyThreat = 0;

	
	uint32 foreigner;
	sint32 i;
	for ( foreigner = 1;  (unsigned) foreigner< m_foreigners.size(); foreigner++ ) 
	{
		
		m_foreigners[foreigner].BeginTurn();
		
		
		if (foreigner == m_playerId)
			continue;
		
		
		Player *foreign_player_ptr = g_player[foreigner];
		
		
		if (foreign_player_ptr == NULL || foreign_player_ptr->IsDead())
			continue;
		
		
		if ( !player_ptr->HasContactWith(foreigner) == TRUE )
			continue;
		
		
		if (m_foreigners[foreigner].GetEffectiveRegard() <= COLDWAR_REGARD ||
			AgreementMatrix::s_agreements.HasAgreement(m_playerId, foreigner, PROPOSAL_TREATY_DECLARE_WAR))
		{
			m_enemyCount++;
			m_enemyThreat += MapAnalysis::GetMapAnalysis().TotalThreat(foreigner);
		}
		else if (m_foreigners[foreigner].GetEffectiveRegard() >= FRIEND_REGARD ||
			AgreementMatrix::s_agreements.HasAgreement(m_playerId, foreigner, PROPOSAL_TREATY_ALLIANCE) ||
			AgreementMatrix::s_agreements.HasAgreement(m_playerId, foreigner, PROPOSAL_TREATY_MILITARY_PACT))
		{
			m_friendCount++;
			m_friendPower += MapAnalysis::GetMapAnalysis().TotalThreat(foreigner);
		}
		
		
		sint32 add_trust;
		if (AgreementMatrix::s_agreements.
			HasAgreement(m_playerId, foreigner, PROPOSAL_TREATY_MILITARY_PACT) ||
			AgreementMatrix::s_agreements.
			HasAgreement(m_playerId, foreigner, PROPOSAL_TREATY_POLLUTION_PACT) ||
			AgreementMatrix::s_agreements.
			HasAgreement(m_playerId, foreigner, PROPOSAL_TREATY_RESEARCH_PACT) ||
			AgreementMatrix::s_agreements.
			HasAgreement(m_playerId, foreigner, PROPOSAL_TREATY_ALLIANCE))
		{
			if (GetCurrentDiplomacy(foreigner).GetHasPactTrustBonus())
			{
				GetCurrentDiplomacy(foreigner).GetHasPactTrustBonus(add_trust);
				
				
				ApplyTrustChange(foreigner, add_trust, "Increase trust because they kept our pact.");
			}
		}

		if (AgreementMatrix::s_agreements.
			HasAgreement(m_playerId, foreigner, PROPOSAL_TREATY_CEASEFIRE) ||
			AgreementMatrix::s_agreements.
			HasAgreement(m_playerId, foreigner, PROPOSAL_TREATY_PEACE))
		{
			if (GetCurrentDiplomacy(foreigner).GetNoWarTrustBonus())
			{
				GetCurrentDiplomacy(foreigner).GetNoWarTrustBonus(add_trust);

				
				if (GetTrust(foreigner) < (NEUTRAL_REGARD + 20))
				{
					
					ApplyTrustChange(foreigner, add_trust, "Increase trust because we're not at war.");
				}
			}
		}

		
		if (AgreementMatrix::s_agreements.
			HasAgreement(m_playerId, foreigner, PROPOSAL_REQUEST_WITHDRAW_TROOPS))
		{
			sint16 duration = 
				AgreementMatrix::s_agreements.GetAgreementDuration(m_playerId, foreigner, PROPOSAL_REQUEST_WITHDRAW_TROOPS);

			if ((duration > 20) &&
				GetCurrentDiplomacy(foreigner).GetFollowThroughTrustBonus())
			{
				GetCurrentDiplomacy(foreigner).GetFollowThroughTrustBonus(add_trust);
				
				
				ApplyTrustChange(foreigner, add_trust, "Increase trust because we followed through with Withdraw Troops agreement.");
			}
		}
			
		if (AgreementMatrix::s_agreements.
			HasAgreement(m_playerId, foreigner, PROPOSAL_REQUEST_STOP_PIRACY))
		{
			const ai::Agreement & agreement = AgreementMatrix::s_agreements.
				GetAgreement(m_playerId, foreigner, PROPOSAL_REQUEST_STOP_PIRACY);
			
			if ((NewTurnCount::GetCurrentRound() - agreement.start == 20) &&
				GetCurrentDiplomacy(foreigner).GetFollowThroughTrustBonus())
			{
				GetCurrentDiplomacy(foreigner).GetFollowThroughTrustBonus(add_trust);
				
				
				ApplyTrustChange(foreigner, add_trust, "Increase trust because we followed through with Stop Piracy agreement.");
			}
		}

		if (AgreementMatrix::s_agreements.
			HasAgreement(m_playerId, foreigner, PROPOSAL_REQUEST_REDUCE_POLLUTION))
		{
			const ai::Agreement & agreement = AgreementMatrix::s_agreements.
				GetAgreement(m_playerId, foreigner, PROPOSAL_REQUEST_REDUCE_POLLUTION);
			
			if ((NewTurnCount::GetCurrentRound() - agreement.start == 20) &&
				GetCurrentDiplomacy(foreigner).GetFollowThroughTrustBonus())
			{
				GetCurrentDiplomacy(foreigner).GetFollowThroughTrustBonus(add_trust);
				
				
				ApplyTrustChange(foreigner, add_trust, "Increase trust because we followed through with Reduce Pollution agreement.");
			}
		}
		
		if (AgreementMatrix::s_agreements.
			HasAgreement(m_playerId, foreigner, PROPOSAL_REQUEST_STOP_RESEARCH))
		{
			const ai::Agreement & agreement = AgreementMatrix::s_agreements.
				GetAgreement(m_playerId, foreigner, PROPOSAL_REQUEST_STOP_RESEARCH);
			
			if ((NewTurnCount::GetCurrentRound() - agreement.start == 20) &&
				GetCurrentDiplomacy(foreigner).GetFollowThroughTrustBonus())
			{
				GetCurrentDiplomacy(foreigner).GetFollowThroughTrustBonus(add_trust);
				
				
				ApplyTrustChange(foreigner, add_trust, "Increase trust because we followed through with Stop Research agreement.");
			}
		}
			
		
		
		

		
		sint32 last_incursion = GetLastBorderIncursionBy(foreigner);
		if ( last_incursion >= 0 && last_incursion + 5 < NewTurnCount::GetCurrentRound())
		{
			
			continue;
		}
		
		
		if ( AgreementMatrix::s_agreements.
			HasAgreement(m_playerId, foreigner, PROPOSAL_TREATY_DECLARE_WAR) )
			continue;
		
		
		if ( AgreementMatrix::s_agreements.
			HasAgreement(m_playerId, foreigner, PROPOSAL_TREATY_MILITARY_PACT) )
			continue;
		
		
		if ( AgreementMatrix::s_agreements.
			HasAgreement(m_playerId, foreigner, PROPOSAL_TREATY_ALLIANCE) )
			continue;
		
		
		num_armies = foreign_player_ptr->m_all_armies->Num();
		for (i = 0; i < num_armies; i++)
		{
			army = foreign_player_ptr->m_all_armies->Access(i);
			if ( g_theArmyPool->IsValid(army) )
			{
				army->GetPos(pos);
				cell_owner = g_theWorld->GetCell(pos)->GetOwner();
				bool is_threat = (army->HasCargo() == TRUE);
				if (!is_threat)
				{
					army->CharacterizeArmy(isspecial,
						isstealth,
						maxattack,
						maxdefense,
						cancapture,
						haszoc,
						canbombard);
					
					
					is_threat = (maxattack || cancapture || haszoc || canbombard);
				}
				if (is_threat  && 
					player_ptr->IsVisible(pos) &&
					(cell_owner == m_playerId) )
				{
					SetBorderIncursionBy(foreigner);
					break;
				}
			}
		} 
		
	} 
	
	sint32 num_cities = player_ptr->m_all_cities->Num();
	sint32 r;
	TradeRoute route;
	Unit city;
	sint32 value;
	
	for (i = 0; i < num_cities; i++)
	{
		city = player_ptr->m_all_cities->Access(i);
		Assert( g_theUnitPool->IsValid(city) );
		
		
		for(r = 0; r < city.CD()->GetTradeSourceList()->Num(); r++) 
		{
			route = city.CD()->GetTradeSourceList()->Access(r);
			
			
			value = route->GetValue();
			foreigner = route->GetDestination().GetOwner();
			m_foreigners[foreigner].AddTradeValue(value);
		}
	}

	
	ComputeAllDesireWarWith();
}


bool Diplomat::GetTradeRoutePiracyRisk(const Unit & source_city, const Unit & dest_city) const
{
	
	
	sint32 max_piracy_events;
	GetCurrentStrategy().GetMaxPiracyEvents(max_piracy_events);

	PiracyHistoryList::const_iterator piracy_iter = m_piracyHistory.begin(); 
	while( piracy_iter != m_piracyHistory.end() )
		{ 
			
			if (piracy_iter->m_sourceCity == source_city &&
				piracy_iter->m_destinationCity == dest_city &&
				!AgreementMatrix::s_agreements.HasAgreement(piracy_iter->m_piratingPlayer,
														   m_playerId, PROPOSAL_OFFER_STOP_PIRACY))
				{
					return (piracy_iter->m_accumEvents >= max_piracy_events);
				}

			piracy_iter++;
		}
	return false;
}


void Diplomat::ComputeTradeRoutePiracyRisk()
{
	Player * player_ptr = g_player[m_playerId];
	Assert(player_ptr != NULL);

	sint32 cur_round = player_ptr->GetCurRound();

	
	PiracyHistoryList::iterator piracy_iter = m_piracyHistory.begin(); 
	while( piracy_iter != m_piracyHistory.end() )
		{ 
			
			sint32 piracy_memory_turns;
			GetCurrentStrategy().GetPiracyMemoryTurns(piracy_memory_turns);
			
			
			if ( (cur_round - piracy_iter->m_lastTurn) > piracy_memory_turns)
				{
					
					piracy_iter = m_piracyHistory.erase( piracy_iter );
				}
			else
				{
					
					piracy_iter++;
				}
		}

	sint32 num_cities = player_ptr->m_all_cities->Num();
	sint32 i,r;
	TradeRoute route;
	Unit city;
	PiracyHistory piracy;
	StringId piracy_str_id;
	g_theStringDB->GetStringID("REGARD_EVENT_PER_ROUTE_PIRACY", piracy_str_id);
	sint32 piracy_regard_cost;

	
	for (i = 0; i < num_cities; i++)
		{
			city = player_ptr->m_all_cities->Access(i);
			Assert( g_theUnitPool->IsValid(city) );

			
			for(r = 0; r < city.CD()->GetTradeSourceList()->Num(); r++) {
				route = city.CD()->GetTradeSourceList()->Access(r);

				
				if (route->IsBeingPirated())
					{
						
						piracy.m_sourceCity = route->GetSource();
						piracy.m_destinationCity = route->GetDestination();
						piracy.m_piratingPlayer = 
							route->GetPiratingArmy().GetOwner();

						
						GetCurrentDiplomacy(piracy.m_piratingPlayer).
							GetPerRoutePiracyRegardCost(piracy_regard_cost);

						LogRegardEvent( piracy.m_piratingPlayer,
										piracy_regard_cost,
										REGARD_EVENT_GOLD,
										piracy_str_id );

						
						piracy_iter = find(m_piracyHistory.begin(), 
										   m_piracyHistory.end(), 
										   piracy);

						if (piracy_iter != m_piracyHistory.end())
							{
								
								piracy_iter->m_accumEvents++;
								piracy_iter->m_lastTurn = cur_round;
							}
						else
							{
								
								piracy.m_accumEvents = 1;
								piracy.m_lastTurn = cur_round;
								m_piracyHistory.push_front(piracy);
							}
					} 
			} 
		} 
}


bool Diplomat::GetTradeRoutePiracyRisk(const PLAYER_INDEX foreignerId) const
{
	sint32 max_piracy_events;
	GetCurrentStrategy().GetMaxPiracyEvents(max_piracy_events);

	PiracyHistoryList::const_iterator piracy_iter = m_piracyHistory.begin(); 
	while( piracy_iter != m_piracyHistory.end() )
	{ 
		
		
		if (piracy_iter->m_piratingPlayer == foreignerId &&
			!AgreementMatrix::s_agreements.HasAgreement(piracy_iter->m_piratingPlayer,
			m_playerId, PROPOSAL_OFFER_STOP_PIRACY))
		{
			return true;
		}
		piracy_iter++;
	}
	return false;
}


void Diplomat::AddNewNegotiationEvent(const PLAYER_INDEX foreignerId, const NegotiationEvent &negotiation_event)
{
	m_foreigners[foreignerId].AddNewNegotiationEvent(negotiation_event);

	g_gevManager->AddEvent(GEV_INSERT_Tail, GEV_NewNegotiationEvent,
						   GEA_Player, m_playerId,
						   GEA_Player, foreignerId,
						   GEA_End);

}


void Diplomat::ComputeIncursionPermission()
{
	
	m_incursionPermission = 0x1; 

	Player *player_ptr = g_player[m_playerId];
	Assert(player_ptr);

	const AgreementMatrix & agreements = AgreementMatrix::s_agreements;

	
	Assert(s_theDiplomats.size() <= sizeof(uint32) * 8);

	
	for (uint32 foreignerId = 1; foreignerId < s_theDiplomats.size() ; foreignerId++)
		{
			
		if (foreignerId == m_playerId)
		{
			m_incursionPermission |= (0x1 << foreignerId);
			continue;
		}
		
		
		if (player_ptr->HasContactWith(foreignerId) == FALSE)
		{
			m_incursionPermission |= (0x1 << foreignerId);
			continue;				
		}
		
		
		if (agreements.HasAgreement(m_playerId, 
			foreignerId, 
			PROPOSAL_TREATY_MILITARY_PACT) ||
			agreements.HasAgreement(m_playerId, 
			foreignerId, 
			PROPOSAL_TREATY_ALLIANCE) )
		{
			m_incursionPermission |= (0x1 << foreignerId);
			continue;
		}
		
		
		if ( !agreements.HasAgreement(m_playerId, 
			foreignerId, 
			PROPOSAL_OFFER_WITHDRAW_TROOPS) &&
			
			
			TestEffectiveRegard(foreignerId, COLDWAR_REGARD) ) 
		{
			
			m_incursionPermission |= (0x1 << foreignerId);					
		}
	}

	
	






























}


uint32 Diplomat::GetIncursionPermission() const
{
	return m_incursionPermission;
}


bool Diplomat::IncursionPermission(const PLAYER_INDEX foreignerId) const
{
	return ((m_incursionPermission & (0x1 << foreignerId)) != 0x0);
}


void Diplomat::SetHotwarAttack(const PLAYER_INDEX foreignerId, const sint16 last_hot_war_attack)
{
	m_foreigners[foreignerId].SetHotwarAttack(last_hot_war_attack);
}


sint16 Diplomat::GetLastHotwarAttack(const PLAYER_INDEX foreignerId) const
{
	return m_foreigners[foreignerId].GetLastHotwarAttack();
}


void Diplomat::SetColdwarAttack(const PLAYER_INDEX foreignerId, const sint16 last_cold_war_attack)
{
	m_foreigners[foreignerId].SetColdwarAttack(last_cold_war_attack);
}


sint16 Diplomat::GetLastColdwarAttack(const PLAYER_INDEX foreignerId) const
{
	return m_foreigners[foreignerId].GetLastColdwarAttack();
}


PLAYER_INDEX Diplomat::ComputeNuclearLaunchTarget()
{
	
	m_nuclearAttackTarget = -1;

	if ((m_strategy.GetNuclearFirstStrikeDisabled() &&
		m_strategy.GetNuclearTargetingDisabled()) ||
		(!m_strategy.GetNuclearFirstStrikeEnabled() &&
		 !m_strategy.GetNuclearTargetingEnabled()))
		return -1;

	
	
	
	
		
		
	
	

	sint32 our_nuke_count = 
		MapAnalysis::GetMapAnalysis().GetNuclearWeaponsCount(m_playerId);

	Player *player_ptr = g_player[m_playerId];
	Assert(player_ptr);

	
	PLAYER_INDEX best_foreigner = -1;

	
	bool has_nuke_enemy = false;

	
	sint32 our_vulnerable_city_count = 0;
	for (sint32 i = 0; i < player_ptr->GetNumCities(); i++)
	{
		if (!player_ptr->GetCityFromIndex(i)->SafeFromNukes())
			our_vulnerable_city_count++;
	}

	
	
	sint32 tmp_nuke_count;
	for (uint32 foreignerId = 1; foreignerId < s_theDiplomats.size(); foreignerId++)
	{
		
		player_ptr = g_player[foreignerId];
		if (player_ptr == NULL)
			continue;
			
		
		if (foreignerId == m_playerId)
			continue;

		
		if (m_strategy.GetPreemptiveStrikeRegard())
		{
			sint32 regard;
			m_strategy.GetPreemptiveStrikeRegard(regard);
			if (GetPublicRegard(foreignerId) > regard)
				continue;
		}

		
		if (!TestEffectiveRegard(foreignerId, HOTWAR_REGARD))
			continue;

		tmp_nuke_count = 
			MapAnalysis::GetMapAnalysis().GetNuclearWeaponsCount(foreignerId);

		
		if (tmp_nuke_count > 0)
		{
			if (has_nuke_enemy)
				return -1;
			has_nuke_enemy = true;
		}
		else
			
			continue;

		
		
		
		if (m_strategy.GetPreemptiveStrikeRiskRatio())
		{
			double risk;
			m_strategy.GetPreemptiveStrikeRiskRatio(risk);
			
			if (((double) tmp_nuke_count / our_vulnerable_city_count) > risk)
				return -1;
		}

		sint32 foreign_vulnerable_city_count = 0;
		for (sint32 i = 0; i < player_ptr->GetNumCities(); i++)
		{
			if (!player_ptr->GetCityFromIndex(i)->SafeFromNukes())
				foreign_vulnerable_city_count++;
		}

		
		
		
		if (m_strategy.GetPreemptiveStrikeSuperiorityRatio())
		{
			double superiority;
			m_strategy.GetPreemptiveStrikeSuperiorityRatio(superiority);
			if (((double) our_nuke_count / foreign_vulnerable_city_count) < superiority)
				return -1;
		}

		
		best_foreigner = foreignerId;
	}

	m_nuclearAttackTarget = best_foreigner;

	return best_foreigner;
}


PLAYER_INDEX Diplomat::GetNuclearLaunchTarget() const
{
	return m_nuclearAttackTarget;
}


void Diplomat::TargetNuclearAttack(const PLAYER_INDEX foreignerId, const bool launch_now)
{
	sint32 our_nuke_count = 
		MapAnalysis::GetMapAnalysis().GetNuclearWeaponsCount(m_playerId);
	
	if (our_nuke_count <= 0)
		return;
	
	Player * player_ptr = g_player[m_playerId];
	Assert(player_ptr != NULL);
	Assert(player_ptr->m_all_armies != NULL);
	
	sint32 num_armies = player_ptr->m_all_armies->Num();
	Army army;
	
	NukeTargetList nuke_city_list;
	
	
	ComputeNukeTargets(nuke_city_list, foreignerId);

	
	Unit unit;
	list<Unit> weapon_list;
	Assert(player_ptr->m_all_units);
	for(sint32 i = 0; i < player_ptr->m_all_units->Num(); i++) {
		
		unit = player_ptr->m_all_units->Access(i);
		
		if(!unit.GetDBRec()->GetNuclearAttack())
			continue;

		
		if (unit->GetArmy().m_id != 0)
			unit->GetArmy()->ClearOrders();

		weapon_list.push_back(unit);
	}

	
	bool no_first_strike = false;
	if (m_strategy.GetNuclearFirstStrikeDisabled() ||
		!m_strategy.GetNuclearFirstStrikeEnabled())
	{
		if (m_strategy.GetNuclearTargetingDisabled())
			
			return;
		else
			
			no_first_strike = true;
	}

	
	NukeTargetList::iterator city_iter;
	MapPoint target_pos;
	list<Unit>::iterator nuke_iter;
	list<Unit>::iterator closest_nuke_iter;
	MapPoint weapon_pos;
	sint32 closest_nuke_dist;
	sint32 tmp_nuke_dist;
	Path found_path;
	bool continue_targeting = true;
	
	while (continue_targeting)
	{
		
		continue_targeting = false;
		
		
		city_iter = nuke_city_list.begin(); 
		while(city_iter != nuke_city_list.end())
		{
			if (!g_theUnitPool->IsValid(city_iter->second.m_id))
			{
				city_iter = nuke_city_list.erase(city_iter);
				continue;
			}

			
			closest_nuke_iter = weapon_list.end();
			closest_nuke_dist = 99999;
			target_pos = city_iter->second->GetPos();
			
			for (nuke_iter = weapon_list.begin();
			nuke_iter != weapon_list.end();
			nuke_iter++)
			{
				if (!g_theUnitPool->IsValid((*nuke_iter).m_id))
					continue;
				weapon_pos = (*nuke_iter).RetPos();
				tmp_nuke_dist = MapPoint::GetSquaredDistance(target_pos, weapon_pos);
				if (tmp_nuke_dist < closest_nuke_dist)
				{
					closest_nuke_dist = tmp_nuke_dist;
					closest_nuke_iter = nuke_iter;
				}
			}
			
			
			bool close_enough = false;
			if (closest_nuke_iter != weapon_list.end())
			{
				sint32 nuke_range = static_cast<sint32>
					(((*closest_nuke_iter).GetDBRec()->GetMaxMovePoints() / k_MOVE_AIR_COST) - 5);
				close_enough = ((nuke_range * nuke_range) > closest_nuke_dist);
			}

			if (close_enough)
			{
				
				(*closest_nuke_iter)->SetTargetCity(city_iter->second);

				if (launch_now && !no_first_strike)
				{
					
					
					unitutil_ExecuteMadLaunch(*closest_nuke_iter);
				}
				
				
				weapon_list.erase(closest_nuke_iter);

				
				city_iter++;
			}
			else
			{
				
				city_iter = nuke_city_list.erase(city_iter);

				
			}
		} 
		continue_targeting = (weapon_list.size() > 0) && (nuke_city_list.size() > 0);
	} 
}


void Diplomat::ComputeNukeTargets(NukeTargetList & city_list, const PLAYER_INDEX targetId) const
{
	Player * player_ptr;
	sint32 nuke_count;
	pair<sint32, Unit> nuke_target;
	Unit city;

	city_list.clear();

	
	for (uint32 foreignerId = 0; foreignerId < s_theDiplomats.size(); foreignerId++)
	{
		
		player_ptr = g_player[foreignerId];
		if (player_ptr == NULL)
			continue;

		
		if (foreignerId == m_playerId)
			continue;

		sint32 regard = GetPublicRegard(foreignerId);

		
		if (targetId != -1)
		{
			
			if (foreignerId != targetId)
				continue;
		}
		else
		{
			
			if (regard >= COLDWAR_REGARD)
				continue;
			
			nuke_count = 
				MapAnalysis::GetMapAnalysis().GetNuclearWeaponsCount(foreignerId);
			
			
			if (nuke_count <= 0)
				continue;
		}

		
		for (sint32 i = 0; i < player_ptr->GetNumCities(); i++)
		{
			city = player_ptr->GetCityFromIndex(i);
			Assert(city.m_id != 0);
			if (city.m_id && city.CD() && !city->SafeFromNukes())
			{
				
				nuke_target.first = city.CD()->GetValue();
				
				
				nuke_target.first += (city->IsCapitol() ? static_cast<sint32>(nuke_target.first * 0.25) : 0);

				
				nuke_target.first += (regard <= HOTWAR_REGARD ? static_cast<sint32>(nuke_target.first * 0.75) : 0); 

				nuke_target.second = city.m_id;

				city_list.push_back(nuke_target);
			}
		}
	}
	
	city_list.sort(greater<pair<sint32, Unit> >());
}


void Diplomat::DisbandNuclearWeapons(const double percent)
{
	Assert(g_player[m_playerId]);
	Player *player_ptr = g_player[m_playerId];

	
	if (player_ptr == NULL)
		return;
	
	sint32 total_weapons = 0;
	Unit unit;
	sint32 i;
	for(i = player_ptr->m_all_units->Num() - 1; i >= 0; i--) {
		
		unit = player_ptr->m_all_units->Access(i);
		if (!g_theUnitPool->IsValid(unit.m_id))
			continue;

		if(unit.GetDBRec()->GetNuclearAttack())
			total_weapons++;
	}

	sint32 goal_weapons = static_cast<sint32>(total_weapons * (1.0 - percent));

	
	Assert(player_ptr->m_all_units);
	for(i = player_ptr->m_all_units->Num() - 1; i >= 0 && (total_weapons > goal_weapons); i--) {
		
		unit = player_ptr->m_all_units->Access(i);
		if (!g_theUnitPool->IsValid(unit.m_id))
			continue;

		if(unit.GetDBRec()->GetNuclearAttack())
		{
			unit.Kill(CAUSE_REMOVE_ARMY_DISBANDED, -1);
			total_weapons--;
		}
	}
	MapAnalysis::GetMapAnalysis().SetNuclearWeaponsCount(m_playerId, total_weapons);

}


void Diplomat::DisbandBioWeapons(const double percent)
{
	Assert(g_player[m_playerId]);
	Player *player_ptr = g_player[m_playerId];
	
	
	if (player_ptr == NULL)
		return;
	
	sint32 total_weapons = 0;
	Unit unit;
	sint32 i;
	for(i = player_ptr->m_all_units->Num() - 1; i >= 0; i--) {
		
		unit = player_ptr->m_all_units->Access(i);
		if (!g_theUnitPool->IsValid(unit.m_id))
			continue;

		if(unit.GetDBRec()->GetBioTerror())
			total_weapons++;
	}

	sint32 goal_weapons = static_cast<sint32>(total_weapons * (1.0 - percent));

	
	Assert(player_ptr->m_all_units);
	for(i = player_ptr->m_all_units->Num() - 1; i >= 0 && (total_weapons > goal_weapons); i--) {
		
		unit = player_ptr->m_all_units->Access(i);
		if (!g_theUnitPool->IsValid(unit.m_id))
			continue;

		if(unit.GetDBRec()->GetBioTerror())
		{
			unit.Kill(CAUSE_REMOVE_ARMY_DISBANDED, -1);
			total_weapons--;
		}
	}
	MapAnalysis::GetMapAnalysis().SetBioWeaponsCount(m_playerId, total_weapons);
}


void Diplomat::DisbandNanoWeapons(const double percent)
{
	Assert(g_player[m_playerId]);
	Player *player_ptr = g_player[m_playerId];
	
	
	if (player_ptr == NULL)
		return;
	
	sint32 total_weapons = 0;
	Unit unit;
	sint32 i;
	for(i = player_ptr->m_all_units->Num() - 1; i >= 0; i--) {
		
		unit = player_ptr->m_all_units->Access(i);
		if (!g_theUnitPool->IsValid(unit.m_id))
			continue;

		if(unit.GetDBRec()->GetCreateParks())
			total_weapons++;
	}

	sint32 goal_weapons = static_cast<sint32>(total_weapons * (1.0 - percent));

	
	Assert(player_ptr->m_all_units);
	for(i = player_ptr->m_all_units->Num() - 1; i >= 0 && (total_weapons > goal_weapons); i--) {
		
		unit = player_ptr->m_all_units->Access(i);
		if (!g_theUnitPool->IsValid(unit.m_id))
			continue;

		if(unit.GetDBRec()->GetCreateParks())
		{
			unit.Kill(CAUSE_REMOVE_ARMY_DISBANDED, -1);
			total_weapons--;
		}
	}
	MapAnalysis::GetMapAnalysis().SetNanoWeaponsCount(m_playerId, total_weapons);
}


void Diplomat::ExecutePersistantAgreements()
{
	AgreementMatrix & agreements = AgreementMatrix::s_agreements;
	
	for (uint32 foreignerId = 1; foreignerId < m_foreigners.size(); foreignerId++)
	{
		
		if (foreignerId == m_playerId)
			continue;

		
		if (g_player[foreignerId] == NULL)
			continue;
	
			
		
		
		if (GetBorderIncursionBy(foreignerId) == true &&
			AgreementMatrix::s_agreements.HasAgreement(m_playerId, foreignerId, 
				PROPOSAL_REQUEST_WITHDRAW_TROOPS))
		{
			
			
			sint16 duration = 
				AgreementMatrix::s_agreements.GetAgreementDuration(m_playerId, foreignerId, PROPOSAL_REQUEST_WITHDRAW_TROOPS);
			
			if (duration > 20)
			{
				LogViolationEvent(foreignerId, PROPOSAL_REQUEST_WITHDRAW_TROOPS);
			}
			
		} 

		
		const ai::Agreement	& stop_research = 
			AgreementMatrix::s_agreements.GetAgreement(m_playerId, foreignerId, PROPOSAL_REQUEST_STOP_RESEARCH);
			
		
		if (stop_research.start != -1 && stop_research.end == -1)
		{
			double science_tax;
			g_player[foreignerId]->GetScienceTaxRate(science_tax);
			if ( (g_player[foreignerId]->m_advances->GetResearching() == 
				  stop_research.proposal.first_arg.advanceType ) &&
				 (science_tax > 0.0) )
			{
				LogViolationEvent(foreignerId, PROPOSAL_REQUEST_STOP_RESEARCH);
			}
		}

		
		const ai::Agreement	& reduce_pollution = 
			AgreementMatrix::s_agreements.GetAgreement(m_playerId, foreignerId, PROPOSAL_REQUEST_REDUCE_POLLUTION);
			
		sint32 agreement_duration = 
			NewTurnCount::GetCurrentRound() - reduce_pollution.start;

		
		if (reduce_pollution.start != -1 && reduce_pollution.end == -1 &&
			agreement_duration > 25)
		{
			uint32 target_pollution = reduce_pollution.proposal.first_arg.pollution;
			
			if (g_player[foreignerId]->GetPollutionLevel() > (unsigned) target_pollution)
			{
				LogViolationEvent(foreignerId, PROPOSAL_REQUEST_REDUCE_POLLUTION);
			}
		}

		
		const ai::Agreement	& pollution_pact = 
			AgreementMatrix::s_agreements.GetAgreement(m_playerId, foreignerId, PROPOSAL_TREATY_POLLUTION_PACT);
		
		const ai::Agreement	& honor_pollution_pact = 
			AgreementMatrix::s_agreements.GetAgreement(m_playerId, foreignerId, PROPOSAL_REQUEST_HONOR_POLLUTION_AGREEMENT);

		agreement_duration = 
			NewTurnCount::GetCurrentRound() - honor_pollution_pact.start;
		
		
		if (honor_pollution_pact.start != -1 && honor_pollution_pact.end == -1 &&
			agreement_duration > 10)
		{
			
			Assert(pollution_pact.start != -1 && pollution_pact.end == -1);

			uint32 target_pollution = pollution_pact.proposal.first_arg.pollution;
			
			if (g_player[foreignerId]->GetPollutionLevel() > (unsigned) target_pollution)
			{
				LogViolationEvent(foreignerId, PROPOSAL_TREATY_POLLUTION_PACT);
			}
		}

		
	} 
}


void Diplomat::SendGreeting(const PLAYER_INDEX & foreignerId)
{
	m_foreigners[foreignerId].SetGreetingTurn();

	
	if (AgreementMatrix::s_agreements.HasAgreement(m_playerId, foreignerId, PROPOSAL_TREATY_DECLARE_WAR))
		return;

	DIPLOMATIC_STRENGTH sender_strength = 
		g_player[m_playerId]->GetRelativeStrength(foreignerId);

	StringId greeting;
	if ( sender_strength > DIPLOMATIC_STRENGTH_STRONG )
	{
		greeting = m_personality->GetStrongGreeting();
	}
	else
	{
		greeting = m_personality->GetWeakGreeting();
	}

	SlicObject *so = new SlicObject((MBCHAR *)"DIPLOMACY_POPUP_GREETING");
	so->AddRecipient(foreignerId);
	so->AddCivilisation(m_playerId) ;
	MBCHAR buf[k_MAX_NAME_LEN];
	stringutils_Interpret(g_theStringDB->GetNameStr(greeting), *so, buf);
	so->AddAction(buf);
	g_slicEngine->Execute(so);
}


bool Diplomat::DesireWarWith(const PLAYER_INDEX foreignerId) const
{
	Assert((unsigned) foreignerId < m_desireWarWith.size());
	Assert(foreignerId >= 0);
	if (foreignerId >= 0 && static_cast<uint32>(foreignerId) < m_desireWarWith.size())
		return m_desireWarWith[foreignerId];
	return false;
}


bool Diplomat::ComputeDesireWarWith(const PLAYER_INDEX foreignerId)
{
	
	if (!g_player[m_playerId]) 
		return false;
	sint32 const		turns_at_peace		= 
		AgreementMatrix::s_agreements.TurnsSinceLastWar(m_playerId, foreignerId);
	sint32 const		turns_at_war		=
		AgreementMatrix::s_agreements.TurnsAtWar(m_playerId, foreignerId);
	DIPLOMATIC_STRENGTH relative_strength = 
		g_player[m_playerId]->GetRelativeStrength(foreignerId);

	sint32 last_hotwar_attack = GetLastHotwarAttack(foreignerId);

	bool best_hotwar_enemy = IsBestHotwarEnemy(foreignerId);

	if (!best_hotwar_enemy)
		return false;

	
	sint32 my_cities_at_risk =
		MapAnalysis::GetMapAnalysis().AtRiskCitiesValue(m_playerId, foreignerId);

	bool want_friend = false;
	if (m_personality->GetConquestPassive())
	{
		if (relative_strength < DIPLOMATIC_STRENGTH_VERY_STRONG &&
			(turns_at_peace == -1 ||
			turns_at_peace > 14 ))
			want_friend = true;

		
		if (my_cities_at_risk >= 20)
			want_friend = true;

	}
	
	
	else if (GetPersonality()->GetConquestNeutral())
	{
		if (relative_strength < DIPLOMATIC_STRENGTH_STRONG &&
			(turns_at_peace == -1 ||
			turns_at_peace > 29))
			want_friend = true;

		
		if (my_cities_at_risk >= 25)
			want_friend = true;
	}
	
	else {
		if (relative_strength < DIPLOMATIC_STRENGTH_AVERAGE)
			want_friend = true;

		
		if (my_cities_at_risk >= 30)
			want_friend = true;
	}

	if (want_friend)
		return false;

	
	
	if (GetPersonality()->GetDiscoveryDiplomatic() ||
		GetPersonality()->GetDiscoveryEconomic())
	{
		if ((last_hotwar_attack > 10) &&
			(GetEffectiveRegard(foreignerId) > HOTWAR_REGARD) &&
			(GetTrust(foreignerId) > COLDWAR_REGARD))
			return false;
	}

	sint32 ideal_war_length;
	if (GetPersonality()->GetConquestAgressive())
		ideal_war_length = 90;
	else if (GetPersonality()->GetConquestNeutral())
		ideal_war_length = 45;
	else
	{
		ideal_war_length = 30;
	}

	
	if (turns_at_war < ideal_war_length)
		return true;
	
	return false;
}


void Diplomat::ComputeAllDesireWarWith()
{
	size_t const	foreignerCount	= m_desireWarWith.size();
	for (size_t foreignerId = 0; foreignerId < foreignerCount; ++foreignerId)
	{
		m_desireWarWith[foreignerId] =
			(foreignerId != (unsigned) m_playerId) && ComputeDesireWarWith(foreignerId);
	}
}


bool Diplomat::IsBestHotwarEnemy(const PLAYER_INDEX foreignerId) const
{
	
	Player *foreigner_ptr = g_player[foreignerId];
	if ( foreigner_ptr == NULL)
		return false;

	
	bool look_for_human = false;
	if (foreigner_ptr->GetPlayerType() != PLAYER_TYPE_ROBOT)
	{
		look_for_human = true;
	}

	DIPLOMATIC_STRENGTH relative_strength;
	
	DIPLOMATIC_STRENGTH lowest_relative_strength = DIPLOMATIC_STRENGTH_VERY_STRONG;
	PLAYER_INDEX weakest_enemy = -1; 

	for (uint32 otherId = 1; otherId < m_foreigners.size(); otherId++) {

		foreigner_ptr = g_player[otherId];

		
		if (foreigner_ptr == NULL)
			continue;

		
		if (foreignerId == m_playerId)
			continue;

		
		if (!AgreementMatrix::s_agreements.
			 HasAgreement(m_playerId, otherId, PROPOSAL_TREATY_DECLARE_WAR))
			continue;

		if (look_for_human)
		{
			
			if (foreigner_ptr->GetPlayerType() == PLAYER_TYPE_ROBOT) 
				continue;
		}
		
		else
		{
			if (foreigner_ptr->GetPlayerType() != PLAYER_TYPE_ROBOT) 
			{
				
				return false;
			}
		}
			
		relative_strength = foreigner_ptr->GetRelativeStrength(m_playerId);
		if (relative_strength < lowest_relative_strength)
		{
			lowest_relative_strength = relative_strength;
			weakest_enemy = otherId;
		}
	}

	
	if (weakest_enemy == -1)
		return true;

	
	return (weakest_enemy == foreignerId);
}


bool Diplomat::CanFormAlliance(const PLAYER_INDEX foreignerId)
{
	Diplomat & foreign_diplomat =
		Diplomat::GetDiplomat(foreignerId);

	Player *player_ptr = g_player[m_playerId];
	Player *foreigner_ptr = g_player[foreignerId];
	Assert(player_ptr);
	Assert(foreigner_ptr);

	
	for (uint32 thirdpartyId = 1; thirdpartyId < m_foreigners.size(); thirdpartyId++) {
		
		if (thirdpartyId == m_playerId ||
			thirdpartyId == foreignerId)
			continue;

		
		if (!player_ptr->HasContactWith(thirdpartyId) ||
			!foreigner_ptr->HasContactWith(thirdpartyId))
			continue;

		
		if ((AgreementMatrix::s_agreements.HasAgreement(m_playerId, thirdpartyId, PROPOSAL_TREATY_PEACE) ||
			 AgreementMatrix::s_agreements.HasAgreement(m_playerId, thirdpartyId, PROPOSAL_TREATY_ALLIANCE) || 
			 AgreementMatrix::s_agreements.HasAgreement(m_playerId, thirdpartyId, PROPOSAL_TREATY_MILITARY_PACT)) &&
			AgreementMatrix::s_agreements.HasAgreement(foreignerId, thirdpartyId, PROPOSAL_TREATY_DECLARE_WAR))
			return false;

		
		if ((AgreementMatrix::s_agreements.HasAgreement(foreignerId, thirdpartyId, PROPOSAL_TREATY_ALLIANCE) || 
			 AgreementMatrix::s_agreements.HasAgreement(foreignerId, thirdpartyId, PROPOSAL_TREATY_MILITARY_PACT)) &&
			AgreementMatrix::s_agreements.HasAgreement(m_playerId, thirdpartyId, PROPOSAL_TREATY_DECLARE_WAR))
			return false;

	} 

	return true;
}


bool Diplomat::CanExtortCityFrom(const PLAYER_INDEX foreginerId, const sint32 max_value, Unit & city) const
{
	city.m_id = 0;
	Player *sender_ptr = g_player[m_playerId];
	Assert(sender_ptr);
	if (sender_ptr == NULL)
		return GEV_HD_Continue;

	Player *receiver_ptr = g_player[foreginerId];
	Assert(receiver_ptr);
	if (receiver_ptr == NULL)
		return GEV_HD_Continue;

	
	sint32 num_cities = receiver_ptr->m_all_cities->Num();
	double risk_ratio;
	for (sint32 i = num_cities-1; i >= 0; i--)
		{
			city = receiver_ptr->m_all_cities->Access(i);
			Assert( g_theUnitPool->IsValid(city) );
			Assert( city->GetCityData() != NULL );
			
			
			if (!sender_ptr->IsExplored(city->GetPos()))
				continue;

			
			if ( city->GetCityData()->GetValue() > max_value)
				continue;

			
			if (city->GetCityData()->GetFounder() == m_playerId)
				risk_ratio = 0.3;
			else
				risk_ratio = 0.75;

			
			if (MapAnalysis::GetMapAnalysis().CityAtRiskRatio(city, m_playerId) < risk_ratio)
				continue;

			
			break;
			
			return true;
		}				

	return false;
}


void Diplomat::ThrowParty(const PLAYER_INDEX foreignerId)
{
	
	
	if (ReadyToParty())
	{
		sint32 regard_bonus;
		StringId strId;
		GetCurrentDiplomacy(foreignerId).GetHoldReceptionRegardBonus(regard_bonus);
		
		g_theStringDB->GetStringID("REGARD_EVENT_HOLD_RECEPTION",strId);
		LogRegardEvent(foreignerId,
			regard_bonus,
			REGARD_EVENT_DIPLOMACY,
			strId,
			10);
		m_lastParty = static_cast<sint16>(NewTurnCount::GetCurrentRound());
	}
}


bool Diplomat::ReadyToParty() const
{
	if (m_lastParty <0)
		return true;
	if (m_lastParty + 10 < NewTurnCount::GetCurrentRound())
		return true;
	return false;
}


bool Diplomat::ShouldEscortSettlers() const
{
	if (g_player[m_playerId] && 
		g_player[m_playerId]->m_playerType == PLAYER_TYPE_HUMAN)
		return false;

	
	if (AtWarCount() <= 0)
		return false;

	
	if (g_player[m_playerId] && 
		g_player[m_playerId]->GetCargoCapacity() < 2)
		return false;

	return true;
}


bool Diplomat::FearNukesFrom(const PLAYER_INDEX foreignerId) const
{
	bool used_nukes = (Diplomat::GetDiplomat(foreignerId).HasLaunchedNukes());

	bool low_trust = (GetTrust(foreignerId) <= COLDWAR_REGARD);

	bool hate_us = (Diplomat::GetDiplomat(foreignerId).GetPublicRegard(foreignerId) <= COLDWAR_REGARD);

	bool at_war = AgreementMatrix::s_agreements.HasAgreement(m_playerId, foreignerId, PROPOSAL_TREATY_DECLARE_WAR);

	DIPLOMATIC_STRENGTH our_strength = 
		g_player[m_playerId]->GetRelativeStrength(foreignerId);

	sint32 their_nukes = MapAnalysis::GetMapAnalysis().GetNuclearWeaponsCount(foreignerId);
	sint32 our_nukes = MapAnalysis::GetMapAnalysis().GetNuclearWeaponsCount(m_playerId);

	bool has_more_nukes = (their_nukes > 3 * our_nukes);

	return ((low_trust && hate_us) || at_war) && 
		used_nukes &&
		(our_strength < DIPLOMATIC_STRENGTH_AVERAGE || has_more_nukes);
}


bool Diplomat::FearNanoAttackFrom(const PLAYER_INDEX foreignerId) const
{
	bool used_nano = (Diplomat::GetDiplomat(foreignerId).HasLaunchedNanoAttack());

	bool low_trust = (GetTrust(foreignerId) <= COLDWAR_REGARD);

	bool hate_us = (Diplomat::GetDiplomat(foreignerId).GetPublicRegard(foreignerId) <= COLDWAR_REGARD);

	bool at_war = AgreementMatrix::s_agreements.HasAgreement(m_playerId, foreignerId, PROPOSAL_TREATY_DECLARE_WAR);

	DIPLOMATIC_STRENGTH our_strength = 
		g_player[m_playerId]->GetRelativeStrength(foreignerId);

	sint32 their_nano = MapAnalysis::GetMapAnalysis().GetNanoWeaponsCount(foreignerId);
	sint32 our_nano = MapAnalysis::GetMapAnalysis().GetNanoWeaponsCount(m_playerId);

	bool has_more_nano = (their_nano > our_nano);

	return ((low_trust && hate_us) || at_war) && 
		used_nano &&
		(our_strength < DIPLOMATIC_STRENGTH_AVERAGE || has_more_nano);
}


bool Diplomat::HasLaunchedNukes() const
{
	return m_launchedNukes;
}


bool Diplomat::HasLaunchedNanoAttack() const
{
	return m_launchedNanoAttack;
}


void Diplomat::SetHasLaunchedNukes(const bool val)
{
	m_launchedNukes = val;
}


void Diplomat::HasLaunchedNanoAttack(const bool val)
{
	m_launchedNanoAttack = val;
}


void Diplomat::ClearInitiatives()
{
	sint32 p;
	for(p = 0; p < k_MAX_PLAYERS; p++) {
		if(g_player[p]) {
			SetReceiverHasInitiative(p, false);
			Diplomat::GetDiplomat(p).SetReceiverHasInitiative(m_playerId, false);
			SetMyLastNewProposal(p, Diplomat::s_badNewProposal);
			SetMyLastResponse(p, Diplomat::s_badResponse);
		}
	}
}


bool Diplomat::FirstTurnOfWar() const
{
	uint32 foreignerId;
	sint32 duration;

	
	if (g_player[m_playerId] == NULL)
		return false;

	bool at_war = false;
	for (foreignerId = 1; foreignerId < m_foreigners.size(); foreignerId++)
	{
		
		if (foreignerId == m_playerId)
			continue;
		
		
		if (g_player[foreignerId] == NULL)
			continue;

		duration = AgreementMatrix::s_agreements.GetAgreementDuration(m_playerId, foreignerId, PROPOSAL_TREATY_DECLARE_WAR);
		
		if (duration > 1)
			return false;

		
		if (duration == 1)
			at_war = true;
	}
	
	return at_war;
}

