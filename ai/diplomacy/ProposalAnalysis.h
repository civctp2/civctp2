











#pragma once
#ifndef __PROPOSAL_ANALYSIS_H__
#define __PROPOSAL_ANALYSIS_H__



#pragma warning(disable: 4786)

#include "diplomattypes.h"

class ProposalAnalysis {
public:

	
	static void ComputeResult( const NewProposal & proposal,
							   DiplomacyResult & senderResult,
							   DiplomacyResult & receiverResult );

	
	static void ComputeResult( const Response & response,
							   DiplomacyResult & senderResult,
							   DiplomacyResult & receiverResult );

	static void DebugResult( const NewProposal & proposal);

	static void DebugResult( const Response & response );

	static void LogDebugResult( const NewProposal & proposal);

	static void LogDebugResult( const Response & response );

	
	static sint32 RoundGold( const sint32 gold );

	
	static double RoundPercentReduction( const double percent);

	
	static bool IsSimpleGift(const NewProposal & proposal);

	
	static bool IsGift(const ProposalData & proposal_data);

	
	static bool GetTreatyFromProposal(const ProposalData & proposal_data, 
		PROPOSAL_TYPE & treaty_type, 
		DiplomacyArg & treaty_arg,
		bool & includes_gift,
		PROPOSAL_TYPE & other_type,
		DiplomacyArg & other_arg);

	
	static bool IsAcceptResponse(const Response & response);

	
	
	
	static void AcceptTreatyCondition(
		const PLAYER_INDEX playerId,
		const PLAYER_INDEX foreignerId,
		const PROPOSAL_TYPE treaty_type, 
		const DiplomacyArg treaty_arg, 
		PROPOSAL_TYPE & desire_type, 
		DiplomacyArg & desire_arg );

	
	
	static bool PlayerGetsEnoughValue(const PLAYER_INDEX playerId,
									  const PLAYER_INDEX senderId,
									  const PLAYER_INDEX receiverId,
									  const ProposalData detail);
private:

	
	static void ComputeResult( const PLAYER_INDEX &sender, 
							   const PLAYER_INDEX &receiver, 
							   const PROPOSAL_TYPE &proposal_type, 
							   const DiplomacyArg &proposal_arg,
							   DiplomacyResult & senderResult,
							   DiplomacyResult & receiverResult );

	
	static void ComputeRegardResultFromProfit(DiplomacyResult & senderResult,
											  DiplomacyResult & receiverResult);


};

#endif __PROPOSAL_ANALYSIS_H__
