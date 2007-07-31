





















#pragma once
#ifndef __DIPLOMAT_H__
#define __DIPLOMAT_H__



#pragma warning(disable: 4786)


#include <vector>
#include <list-fixed>
#include "c3debugstl.h"

#include "Foreigner.h"
#include "StrategyRecord.h"
#include "PersonalityRecord.h"
#include "Unit.h"
#include "SlicContext.h"


const sint16 k_maxStategicState = 10;

class Diplomat 
{
public:

	
	
	

	
	
	

	struct PiracyHistory {
		PiracyHistory() 
			{
				m_sourceCity.m_id = 0;
				m_destinationCity.m_id = 0;
				m_piratingPlayer = -1;
				m_accumEvents = 0; 
				m_lastTurn = -1;
			}
		bool operator<(const PiracyHistory & rval) const
			{
				if (m_sourceCity.m_id < rval.m_sourceCity.m_id &&
					m_destinationCity.m_id < rval.m_destinationCity.m_id &&
					m_piratingPlayer < rval.m_piratingPlayer)
					return true;
				return false;
			}

		bool operator==(const PiracyHistory & rval) const
			{
				if (m_sourceCity.m_id == rval.m_sourceCity.m_id &&
					m_destinationCity.m_id == rval.m_destinationCity.m_id &&
					m_piratingPlayer == rval.m_piratingPlayer )
					return true;
				return false;
			}
		Unit m_sourceCity;
		Unit m_destinationCity;
		PLAYER_INDEX m_piratingPlayer;
		sint32 m_accumEvents;	
		sint32 m_lastTurn;		
	};


	
	typedef std::list<Motivation, dbgallocator<Motivation> > MotivationList;
	typedef std::vector<MotivationList::iterator, dbgallocator<MotivationList::iterator> > MotivationVector;
	typedef std::vector<AiState, dbgallocator<AiState> > AiStateVector;
	typedef std::vector<Foreigner, dbgallocator<Foreigner> > ForeignerVector;
	typedef std::list<Threat, dbgallocator<Threat> > ThreatList;
	typedef std::vector<DiplomacyRecord, dbgallocator<DiplomacyRecord> > DiplomacyRecordVector;
	typedef std::vector<Diplomat, dbgallocator<Diplomat> > DiplomatVector;
	typedef std::list<PiracyHistory, dbgallocator<PiracyHistory> > PiracyHistoryList;
	typedef std::list<AiState, dbgallocator<AiState> > AiStateList;
	typedef std::list<std::pair<sint32, Unit>, dbgallocator<std::pair<sint32, Unit> > > NukeTargetList;
	typedef std::vector<bool, dbgallocator<bool> > BoolVector;

	
	
	
	
	
	
	
	
	
	
	



	
	static AiState s_badAiState;
	static Threat s_badThreat;
	static Response s_badResponse;
	static NewProposal s_badNewProposal;
	static Motivation s_badMotivation;
	static ai::Agreement s_badAgreement;

	
	static sint32 s_proposalTypeToElemIndex[PROPOSAL_MAX];

	
	
	

	
	static Diplomat & GetDiplomat(const PLAYER_INDEX & playerId);

	
	static void ResizeAll(const PLAYER_INDEX & newMaxPlayers);

	
	
	
	static void CleanupAll();

	
	static void InitializeAll();

	
	static void LoadAll(CivArchive & archive);

	
	static void SaveAll(CivArchive & archive);

	static void DebugStatusAll();

	
	static void AddDiplomacyArgToSlicContext(SlicContext & sc, const DiplomacyArg & dip_arg);

	
	sint32 GetNextId();

	
	void SetNextId(const sint32 & id);

	
	
	

	
	Diplomat();

	
	void Resize(const PLAYER_INDEX & newMaxPlayers);

	
	void Load(CivArchive & archive);

	
	void Save(CivArchive & archive) const;

	
	void Cleanup();

	
	void Initialize();

	
	void InitForeigner(const PLAYER_INDEX & foreigner);

	
	void DebugStatus(const PLAYER_INDEX & foreignerId = -1) const;

	
	void LogDebugStatus(const PLAYER_INDEX & foreignerId = -1) const;

	
	void SetPlayerId(const PLAYER_INDEX &playerId);

	
	PLAYER_INDEX GetPlayerId() const;

	
	void SetPersonalityName(const char *personality_name);

	
	std::string GetPersonalityName() const;

	
	const PersonalityRecord *GetPersonality() const;

	
	bool GetReceiverHasInitiative(const PLAYER_INDEX & foreignerId) const;

	
	void SetReceiverHasInitiative(const PLAYER_INDEX & foreignerId, const bool & hasInitiative);

	
	void BeginTurn();

	
	
	

	
	
	void LogRegardEvent( const PLAYER_INDEX & foreignerId,
						 const sint32 & regardDelta,
						 const REGARD_EVENT_TYPE & type,
						 const StringId & explain,
						 const sint16 duration = -1);

	
	void LogViolationEvent(const PLAYER_INDEX foreignerId, const PROPOSAL_TYPE proposal_type);

	
	const ai::Regard GetPublicRegard( const PLAYER_INDEX & foreignerId,
									  const REGARD_EVENT_TYPE & type = REGARD_EVENT_ALL ) const;

	
	
	StringId ExplainRegard( const PLAYER_INDEX & foreignerId,
						  const REGARD_EVENT_TYPE & type = REGARD_EVENT_ALL ) const;

	
	const ai::Regard GetEffectiveRegard( const PLAYER_INDEX & foreignerId) const;

	
	static void ApplyGlobalTrustChange(const PLAYER_INDEX & foreignerId, const ai::Regard &trust_delta, const MBCHAR *reason = NULL);

	
	void ApplyTrustChange(const PLAYER_INDEX & foreignerId, const ai::Regard &trust_delta,  const MBCHAR *reason = NULL);

	
	const ai::Regard GetTrust(const PLAYER_INDEX & foreignerId) const;

	
	void SetTrust(const PLAYER_INDEX & foreignerId, const ai::Regard &trust);

	
	ai::Regard GetBaseRegard(const PLAYER_INDEX foreignerId) const;

	
	void RecomputeRegard();

	
	void UpdateRegard(const PLAYER_INDEX foreignerId);

	
	
	

	
	
	
	sint32 AddAgreement(const PLAYER_INDEX & foreignerId);

	
	void EnactStopPiracy(const PLAYER_INDEX victimId, const PLAYER_INDEX pirateId);

	
	
	bool CanAfford( const PLAYER_INDEX senderId,
					const PLAYER_INDEX receiverId,
					const ProposalData & proposal ) const;

	
	bool Execute_Agreement(const ai::Agreement & agreement);

	
	void DeclareWar(const PLAYER_INDEX foreginerId);

	
	void SetEmbargo(const PLAYER_INDEX foreignerId, const bool state);

	
	bool GetEmbargo(const PLAYER_INDEX foreignerId) const;


	
	
	

	
	void AddRejection(const PLAYER_INDEX & foreignerId);

	
	
	

	
	const Threat & GetThreatById(const sint32 & id) const;

	
	void RemoveThreatById(const sint32 & id);

	
	
	sint32 AddThreat(const PLAYER_INDEX & foreignerId);

	
	const ThreatList & GetThreatList() const;

	
	bool ExecuteThreat(const Threat & threat);

	
	
	bool HasThreat(const PLAYER_INDEX & foreignerId,
				   const THREAT_TYPE type,
				   Threat & threat) const;

	
	bool ComputeThreatResponse(const PLAYER_INDEX foreignerId, Response & threat_response) const;

	
	bool GetAgreementToBreak(const PLAYER_INDEX foreignerId, ai::Agreement & pact) const;

	
	
	
			
	
	void ConsiderResponse( const PLAYER_INDEX & senderId,
						   const Response & response );

	
	void ConsiderResponse( const PLAYER_INDEX senderId,
						   const RESPONSE_TYPE response_type,
						   const sint32 priority);

	
	void ConsiderCounterResponse( const PLAYER_INDEX senderId,
								  const PROPOSAL_TYPE counterProposalType,
								  const DiplomacyArg & argument,
								  const sint32 priority,
								  const DIPLOMATIC_TONE tone);

	
	const Response & GetMyLastResponse(const PLAYER_INDEX & foreignId) const;

	
	void SetMyLastResponse( const PLAYER_INDEX & foreignId, 
							const Response & response );

	
	void ExecuteResponse( const PLAYER_INDEX sender,
						  const PLAYER_INDEX receiver );	

	

	
	const Response & GetResponsePending(const PLAYER_INDEX foreignerId) const;

	
	const Response & GetResponse(const PLAYER_INDEX foreignerId) const;

	
	void ExecuteResponse(const Response & response);

	
	
	

	
	
	
	
	
	
	
	
	void ConsiderMotivation(const Motivation & motivation); 

	
	sint32 GetMotivationCount() const;

	
	void SortMotivations();

	
	const Motivation & GetCurrentMotivation() const;

	
	
	StringId GetDiplomacyAdvice(SlicContext & sc, const PLAYER_INDEX & foreignerId = -1) const;

	
	
	

	
	
	
	
	void ContinueDiplomacy( const PLAYER_INDEX & foreignerId );

	
	
	bool StartNegotiations(const PLAYER_INDEX hotseat_foreignerId = -1);

	
	bool InvalidNewProposal(const PLAYER_INDEX & foreignerId, const DiplomacyProposalRecord *rec) const;

	
	void ChooseNewProposal( const PLAYER_INDEX & foreignerId );

	
	void ConsiderNewProposal( const PLAYER_INDEX & foreignerId,
							  const NewProposal & newProposal );

	
	const NewProposal & GetMyLastNewProposal(const PLAYER_INDEX & foreignId) const;

	
	void SetMyLastNewProposal( const PLAYER_INDEX & foreignId, 
							   const NewProposal & newProposal );

	
	void ExecuteNewProposal(const PLAYER_INDEX & receiver );

	
	void SetMyLastNegotiatedProposal( const PLAYER_INDEX & foreignerId, const ProposalData & data, const RESPONSE_TYPE & response );
	
	
	RESPONSE_TYPE GetMyLastNegotiatedProposal( const PLAYER_INDEX & foreignerId, ProposalData & data ) const;

	
	bool GetNewProposalTimeout( const NewProposal & newProposal,
								const sint16 timeout_period = 20) const;

	
	const NegotiationEventList & GetNegotiationEvents( const PLAYER_INDEX & foreignerId ) const;

	
	sint32 GetNewProposalPriority(const PLAYER_INDEX foreignerId, 
								  const PROPOSAL_TYPE proposalType ) const;

	
	sint32 GetAcceptPriority(const PLAYER_INDEX foreignerId, 
							 const PROPOSAL_TYPE proposalType ) const;

	
	sint32 GetRejectPriority(const PLAYER_INDEX foreignerId, 
							 const PROPOSAL_TYPE proposalType ) const;

	
	sint32 GetSenderRegardResult(const PLAYER_INDEX foreignerId, 
								 const PROPOSAL_TYPE proposalType ) const;

	
	sint32 GetReceiverRegardResult(const PLAYER_INDEX foreignerId, 
					 			   const PROPOSAL_TYPE proposalType ) const;

	
	sint32 GetViolationRegardCost(const PLAYER_INDEX foreignerId, 
					 			  const PROPOSAL_TYPE proposalType ) const;

	
	sint32 GetViolationTrustCost(const PLAYER_INDEX foreignerId, 
					 			 const PROPOSAL_TYPE proposalType ) const;

	

	
	const NewProposal & GetNewProposalPending(const PLAYER_INDEX foreignerId) const;

	
	const NewProposal & GetNewProposalAdvice(const PLAYER_INDEX foreignerId) const;

	
	void ExecuteNewProposal(const NewProposal & proposal);


	
	
	

	
	void InitStrategicState();

	
	void NextStrategicState();

	
	void ConsiderStrategicState( const AiState & state );

	
	void SetStrategy(const sint32 index);

	
	void ComputeCurrentStrategy();

	
	const StrategyRecord & GetCurrentStrategy() const;

	
	
	

	
	void InitDiplomaticState( const PLAYER_INDEX & foreignerId );

	
	void NextDiplomaticState( const PLAYER_INDEX & foreignerId );

	
	void ConsiderDiplomaticState( const PLAYER_INDEX & foreignerId, const AiState & state );
	
	
	const AiState & GetBestDiplomaticState() const;

	
	const AiState & GetCurrentDiplomaticState( const PLAYER_INDEX & foreignerId ) const;

	
	void ChangeDiplomacy(const PLAYER_INDEX & foreignerId, const sint32 index);

	
	void SetDiplomaticState(const PLAYER_INDEX & foreignerId, const AiState & newState );

	
	const DiplomacyRecord & GetCurrentDiplomacy(const PLAYER_INDEX & foreignerId) const;

	
	
	

	
	sint32 GetNextAdvance() const;

	
	sint32 GetDesiredAdvanceFrom( const PLAYER_INDEX & foreignerId, const sint32 min_cost, const sint32 max_cost ) const;

	
	sint32 GetStopResearchingAdvance( const PLAYER_INDEX & foreignerId) const;

	
	sint32 GetNextKeyAdvance() const;

	
	StringId GetScienceAdvice(SlicContext & sc, StringId & advance_advice);

	
	
	

	
	sint32 GetTradeFrom(const PLAYER_INDEX &foreignId) const;

	
	sint32 GetTributeFrom(const PLAYER_INDEX &foreignId) const;

	
	sint32 GetGoldSurplusPercent() const;

	
	bool CanBuySurplus(const PLAYER_INDEX &foreignId) const;

	
	sint32 GetAdvanceLevelPercent(const PLAYER_INDEX &foreignId) const;

	
	sint16 AtWarCount() const;

	
	sint16 EffectiveAtWarCount();

	
	bool TestPublicRegard(const PLAYER_INDEX & foreignerId, const ai::Regard & test_regard) const;

	
	bool TestEffectiveRegard(const PLAYER_INDEX & foreignerId, const ai::Regard & test_regard);

	
	bool TestAlliedRegard(const PLAYER_INDEX & foreignerId);

	
	bool GetBorderIncursionBy(const PLAYER_INDEX & foreignerId) const;

	
	void SetBorderIncursionBy(const PLAYER_INDEX & foreignerId);

	
	void SetBorderPulloutBy(const PLAYER_INDEX & foreignerId);

	
	sint32 GetLastBorderIncursionBy(const PLAYER_INDEX & foreignerId) const;

	
	sint8 GetFriendCount() const;

	
	sint8 GetEnemyCount() const;

	
	sint32 GetFriendPower() const;

	
	sint32 GetEnemyThreat() const;

	
	bool GetTradeRoutePiracyRisk(const Unit & source_city, const Unit & dest_city) const;

	
	bool GetTradeRoutePiracyRisk(const PLAYER_INDEX foreignerId) const;

	
	void AddNewNegotiationEvent(const PLAYER_INDEX foreignerId, const NegotiationEvent &negotiation_event);

	
	uint32 GetIncursionPermission() const;

	
	bool IncursionPermission(const PLAYER_INDEX foreignerId) const;

	
	void SetHotwarAttack(const PLAYER_INDEX foreignerId, const sint16 last_hot_war_attack);

	
	sint16 GetLastHotwarAttack(const PLAYER_INDEX foreignerId) const;

	
	void SetColdwarAttack(const PLAYER_INDEX foreignerId, const sint16 last_cold_war_attack);

	
	sint16 GetLastColdwarAttack(const PLAYER_INDEX foreignerId) const;

	
	
	PLAYER_INDEX ComputeNuclearLaunchTarget();

	
	PLAYER_INDEX GetNuclearLaunchTarget() const;

	
	void TargetNuclearAttack(const PLAYER_INDEX foreignerId, const bool launch_now);

	
	bool DesireWarWith(const PLAYER_INDEX foreignerId) const;

	
	bool ComputeDesireWarWith(const PLAYER_INDEX foreignerId);

	
	void ComputeAllDesireWarWith();

	
	bool IsBestHotwarEnemy(const PLAYER_INDEX foreignerId) const;

	
	bool CanFormAlliance(const PLAYER_INDEX foreignerId);

	
	bool CanExtortCityFrom(const PLAYER_INDEX foreginerId, const sint32 max_value, Unit & city) const;

	
	void ThrowParty(const PLAYER_INDEX foreignerId);

	
	bool ReadyToParty() const;

	
	bool ShouldEscortSettlers() const;

	
	bool FearNukesFrom(const PLAYER_INDEX foreignerId) const;

	
	bool FearNanoAttackFrom(const PLAYER_INDEX foreignerId) const;

	
	bool HasLaunchedNukes() const;

	
	bool HasLaunchedNanoAttack() const;

	
	void SetHasLaunchedNukes(const bool val);

	
	void HasLaunchedNanoAttack(const bool val);

	
	void SendGreeting(const PLAYER_INDEX & foreignerId);
	
	
	void ClearInitiatives();

	
	bool FirstTurnOfWar() const;

private:
	
	static sint32 s_nextId;
	static Diplomat::DiplomatVector s_theDiplomats;

	
	void RemoveBestMotivation();

	
	void UpdateAttributes();

	
	void MergeStrategy(const sint32 index);

	
	void Execute_Proposal( const PLAYER_INDEX & sender,
						   const PLAYER_INDEX & receiver,
						   const PROPOSAL_TYPE & type,
						   const DiplomacyArg & argument );

	
	void ComputeTradeRoutePiracyRisk();

	
	void ComputeIncursionPermission();

	
	bool LaunchNuclearAttackOnCity(const Unit & city);

	
	void ComputeNukeTargets(NukeTargetList & city_list, const PLAYER_INDEX targetId) const;

	
	void DisbandNuclearWeapons(const double percent);

	
	void DisbandBioWeapons(const double percent);

	
	void DisbandNanoWeapons(const double percent);

	
	void ExecutePersistantAgreements();

	
	
	

	
	PLAYER_INDEX m_playerId;
	std::string m_personalityName;
	const PersonalityRecord *m_personality;

	
	
	

	
	MotivationList m_motivations;

	
	MotivationVector m_lastMotivation;

	
	AiStateVector m_diplomaticStates;

	
	AiStateList m_bestStrategicStates;

	
	AiState m_bestDiplomaticState;

	
	StrategyRecord m_strategy;
	
	
	DiplomacyRecordVector m_diplomacy;

	
	ForeignerVector m_foreigners;

	
	ThreatList m_threats;

	
	
	

	sint8 m_friendCount;
	sint8 m_enemyCount;
	sint32 m_friendPower;
	sint32 m_enemyThreat;

	
	PiracyHistoryList m_piracyHistory;

	
	uint32 m_incursionPermission;

	
	sint8 m_outstandingProposals;

	
	sint16 m_diplomcyVictoryCompleteTurn;

	
	PLAYER_INDEX m_nuclearAttackTarget;

	
	sint16 m_lastParty;

	bool m_launchedNukes;
	bool m_launchedNanoAttack;
	BoolVector m_desireWarWith;

	
	
	
	
	
	
	
	bool ComputeEffectiveRegard(const PLAYER_INDEX & foreignerId, const ai::Regard & test_regard) const;
public:
	
	void ClearEffectiveRegardCache();
private:

	struct cEffectiveRegardEntry
	{
	public:
		inline cEffectiveRegardEntry() {m_round=-666;}
		int m_round;	
		uint32 m_bits;	

		
		
		inline int RegardToIndex(int regard) {return (regard>>6);}
	};

	
	cEffectiveRegardEntry m_effectiveRegardCache[k_MAX_PLAYERS];
};

#endif __DIPLOMAT_H__
