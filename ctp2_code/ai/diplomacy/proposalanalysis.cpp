









#include "c3.h"

#include "MapPoint.h"
#include "Player.h"
#include "Diplomat.h"
#include "ProposalAnalysis.h"
#include "AdvanceRecord.h"
#include "Unit.h"
#include "UnitData.h"
#include "CityData.h"
#include "AgreementMatrix.h"
#include "gslogs.h"
#include "MapAnalysis.h"
#include "c3math.h"
#include "Gold.h"
#include "DiplomacyProposalRecord.h"
#include "diplomacyutil.h"

#include "UnitPool.h"

using namespace ai;


void ProposalAnalysis::ComputeResult( const NewProposal & proposal,
									  DiplomacyResult & senderResult,
									  DiplomacyResult & receiverResult )
{
	ComputeResult( proposal.senderId,
				   proposal.receiverId,
				   proposal.detail.first_type,
				   proposal.detail.first_arg,
				   senderResult,
				   receiverResult );

	ComputeResult( proposal.senderId,
				   proposal.receiverId,
				   proposal.detail.second_type,
				   proposal.detail.second_arg,
				   senderResult,
				   receiverResult );


	
	
}


void ProposalAnalysis::ComputeResult( const Response & response,
									  DiplomacyResult & senderResult,
									  DiplomacyResult & receiverResult )
{
	if (response.type == RESPONSE_COUNTER)
		{
			ComputeResult( response.senderId,
						   response.receiverId,
						   response.counter.first_type,
						   response.counter.first_arg,
						   senderResult,
						   receiverResult );

			ComputeResult( response.senderId,
						   response.receiverId,
						   response.counter.second_type,
						   response.counter.second_arg,
						   senderResult,
						   receiverResult );
		}
	else
		{
			
			Assert(0);
		}
}

void ProposalAnalysis::DebugResult( const NewProposal & proposal)
{
	DPRINTF(k_DBG_DIPLOMACY, ("Evaluated NEW PROPOSAL [sender = %d, receiver = %d]\n", 
		proposal.senderId, proposal.receiverId));
	DPRINTF(k_DBG_DIPLOMACY, ("first = %s\n", s_proposalNames[proposal.detail.first_type].c_str()));
	DPRINTF(k_DBG_DIPLOMACY, ("second = %s\n", s_proposalNames[proposal.detail.second_type].c_str()));
	
	DiplomacyResult sender_result;
	DiplomacyResult receiver_result;
	ComputeResult(proposal,sender_result, receiver_result);
	DPRINTF(k_DBG_DIPLOMACY, ("\n"));
	DPRINTF(k_DBG_DIPLOMACY, ("Sender Result:\n"));
	DPRINTF(k_DBG_DIPLOMACY, ("  science = %d\n", sender_result.science));
	DPRINTF(k_DBG_DIPLOMACY, ("  gold = %d\n", sender_result.gold));
	DPRINTF(k_DBG_DIPLOMACY, ("  production = %d\n", sender_result.production));
	DPRINTF(k_DBG_DIPLOMACY, ("  regard = %d\n", sender_result.regard));
	DPRINTF(k_DBG_DIPLOMACY, ("\n"));
	DPRINTF(k_DBG_DIPLOMACY, ("Receiver Result:\n"));
	DPRINTF(k_DBG_DIPLOMACY, ("  science = %d\n", receiver_result.science));
	DPRINTF(k_DBG_DIPLOMACY, ("  gold = %d\n", receiver_result.gold));
	DPRINTF(k_DBG_DIPLOMACY, ("  production = %d\n", receiver_result.production));
	DPRINTF(k_DBG_DIPLOMACY, ("  regard = %d\n", receiver_result.regard));
}

void ProposalAnalysis::DebugResult( const Response & response )
{
	DPRINTF(k_DBG_DIPLOMACY, ("Evaluated RESPONSE type %s [sender = %d, receiver = %d]\n", 
		s_responseNames[response.type].c_str(),response.senderId, response.receiverId));

	if (response.type != RESPONSE_THREATEN)
	{
		DPRINTF(k_DBG_DIPLOMACY, ("first = %s\n", s_proposalNames[response.counter.first_type].c_str()));
		DPRINTF(k_DBG_DIPLOMACY, ("second = %s\n", s_proposalNames[response.counter.second_type].c_str()));
	}
	else
	{
		DPRINTF(k_DBG_DIPLOMACY, ("threat = %s\n", s_threatNames[response.threat.type].c_str()));
	}
	
	DiplomacyResult sender_result;
	DiplomacyResult receiver_result;
	ComputeResult(response,sender_result, receiver_result);
	DPRINTF(k_DBG_DIPLOMACY, ("\n"));
	DPRINTF(k_DBG_DIPLOMACY, ("Sender Result:\n"));
	DPRINTF(k_DBG_DIPLOMACY, ("  science = %d\n", sender_result.science));
	DPRINTF(k_DBG_DIPLOMACY, ("  gold = %d\n", sender_result.gold));
	DPRINTF(k_DBG_DIPLOMACY, ("  production = %d\n", sender_result.production));
	DPRINTF(k_DBG_DIPLOMACY, ("  regard = %d\n", sender_result.regard));
	DPRINTF(k_DBG_DIPLOMACY, ("\n"));
	DPRINTF(k_DBG_DIPLOMACY, ("Receiver Result:\n"));
	DPRINTF(k_DBG_DIPLOMACY, ("  science = %d\n", receiver_result.science));
	DPRINTF(k_DBG_DIPLOMACY, ("  gold = %d\n", receiver_result.gold));
	DPRINTF(k_DBG_DIPLOMACY, ("  production = %d\n", receiver_result.production));
	DPRINTF(k_DBG_DIPLOMACY, ("  regard = %d\n", receiver_result.regard));
}

void ProposalAnalysis::LogDebugResult( const NewProposal & proposal)
{
	gslog_dipprint("   NEW PROPOSAL [sender = %d, receiver = %d]\n", 
		proposal.senderId, proposal.receiverId);
	gslog_dipprint("   first = %s\n", s_proposalNames[proposal.detail.first_type].c_str());
	gslog_dipprint("   second = %s\n", s_proposalNames[proposal.detail.second_type].c_str());
	
	DiplomacyResult sender_result;
	DiplomacyResult receiver_result;
	ComputeResult(proposal,sender_result, receiver_result);
	gslog_dipprint("\n");
	gslog_dipprint("   Sender Result:\n");
	if (sender_result.science != 0)
		gslog_dipprint("     science = %d\n", sender_result.science);
	if (sender_result.gold != 0)
		gslog_dipprint("     gold = %d\n", sender_result.gold);
	if (sender_result.production != 0)
		gslog_dipprint("     production = %d\n", sender_result.production);
	if (sender_result.regard != 0)
		gslog_dipprint("     regard = %d\n", sender_result.regard);
	gslog_dipprint("\n");
	gslog_dipprint("   Receiver Result:\n");
	if (receiver_result.science != 0)
		gslog_dipprint("     science = %d\n", receiver_result.science);
	if (receiver_result.gold != 0)
		gslog_dipprint("     gold = %d\n", receiver_result.gold);
	if (receiver_result.production != 0)
		gslog_dipprint("     production = %d\n", receiver_result.production);
	if (receiver_result.regard != 0)
		gslog_dipprint("     regard = %d\n", receiver_result.regard);
	gslog_dipprint("\n");
}

void ProposalAnalysis::LogDebugResult( const Response & response )
{
	gslog_dipprint("   RESPONSE type %s [sender = %d, receiver = %d]\n", 
		s_responseNames[response.type].c_str(),response.senderId, response.receiverId);

	if (response.type == RESPONSE_THREATEN)
	{
		gslog_dipprint("      threat = %s\n", s_threatNames[response.threat.type].c_str());
	}
	else if (response.type == RESPONSE_COUNTER)
	{
		gslog_dipprint("      first = %s\n", s_proposalNames[response.counter.first_type].c_str());
		gslog_dipprint("      second = %s\n", s_proposalNames[response.counter.second_type].c_str());

		DiplomacyResult sender_result;
		DiplomacyResult receiver_result;
		ComputeResult(response,sender_result, receiver_result);
		gslog_dipprint("\n");
		gslog_dipprint("      Sender Result:\n");
		if (sender_result.science != 0)
			gslog_dipprint("        science = %d\n", sender_result.science);
		if (sender_result.gold != 0)
			gslog_dipprint("        gold = %d\n", sender_result.gold);
		if (sender_result.production != 0)
			gslog_dipprint("        production = %d\n", sender_result.production);
		if (sender_result.regard != 0)
			gslog_dipprint("        regard = %d\n", sender_result.regard);
		gslog_dipprint("\n");
		gslog_dipprint("      Receiver Result:\n");
		if (receiver_result.science != 0)
			gslog_dipprint("        science = %d\n", receiver_result.science);
		if (receiver_result.gold != 0)
			gslog_dipprint("        gold = %d\n", receiver_result.gold);
		if (receiver_result.production != 0)
			gslog_dipprint("        production = %d\n", receiver_result.production);
		if (receiver_result.regard != 0)
			gslog_dipprint("        regard = %d\n", receiver_result.regard);
	}
	gslog_dipprint("\n");
}


sint32 ProposalAnalysis::RoundGold( const sint32 gold )
{
	const sint32 unit = 100;
	sint32 tmp = gold - (gold % unit);
	if ((gold % unit) > (unit/2))
	{
		tmp += unit;
	}
	return tmp;
}


double ProposalAnalysis::RoundPercentReduction( const double percent)
{
	if (percent < 0.075)
		return 0.05;
	else if (percent < 0.15)
		return 0.10;
	else if (percent < 0.35)
		return 0.20;
	else if (percent < 0.625)
		return 0.50;
	else 
		return 0.75;
}






void ProposalAnalysis::ComputeResult( const PLAYER_INDEX &sender, 
									  const PLAYER_INDEX &receiver, 
									  const PROPOSAL_TYPE &proposal_type, 
									  const DiplomacyArg &proposal_arg, 
									  DiplomacyResult & senderResult,
									  DiplomacyResult & receiverResult )
{
	
	if (proposal_type == PROPOSAL_NONE)
		return;

	const MapAnalysis & map = MapAnalysis::GetMapAnalysis();
	const AgreementMatrix & agreements = AgreementMatrix::s_agreements;

	Player *receiver_ptr = g_player[receiver];
	Player *sender_ptr = g_player[sender];
	Assert(receiver_ptr);
	Assert(sender_ptr);
	
	
	if (receiver_ptr == NULL || sender_ptr == NULL)
		return;

	
	sint32 duration = 20; 

	Diplomat & sender_diplomat = Diplomat::GetDiplomat(sender);
	Diplomat & receiver_diplomat = Diplomat::GetDiplomat(receiver);
	Unit city;
	double scale_regard = 0.0;
	const AdvanceRecord * advance_record = NULL;

	
	
	sint32 turns_since_last_agreed
		= agreements.GetAgreementDuration(sender, receiver, proposal_type);
	if (turns_since_last_agreed < (duration) || turns_since_last_agreed == -1)
	{
		scale_regard = 1.0;
	}
	
	ai::Agreement agreement;
	switch (proposal_type) {
	case PROPOSAL_OFFER_GIVE_CITY:
		
		
		
		Assert(g_theUnitPool->IsValid(proposal_arg.cityId));
		if (!g_theUnitPool->IsValid(proposal_arg.cityId))
			break;

		city.m_id = proposal_arg.cityId;
		senderResult.gold -= city->GetCityData()->GetValue();
		senderResult.production -= city->GetNetCityProduction() * 10;
		senderResult.science -= city.CD()->GetScience() * 10;

		receiverResult.gold += city->GetCityData()->GetValue();
		receiverResult.production += city->GetNetCityProduction() * 10;
		receiverResult.science += city.CD()->GetScience() * 10;

		scale_regard = 
			(double) map.GetAlliedValue(sender, city.RetPos()) / 
					 map.GetMaxAlliedValue(sender);

		break;
	case PROPOSAL_REQUEST_GIVE_CITY:
		

		
		Assert(g_theUnitPool->IsValid(proposal_arg.cityId));
		if (!g_theUnitPool->IsValid(proposal_arg.cityId))
			break;

		city.m_id = proposal_arg.cityId;
		senderResult.gold += city->GetCityData()->GetValue();
		senderResult.production += city->GetNetCityProduction() * 10;
		senderResult.science += city.CD()->GetScience() * 10;

		receiverResult.gold -= city->GetCityData()->GetValue();
		receiverResult.production -= city->GetNetCityProduction() * 10;
		receiverResult.science -= city.CD()->GetScience() * 10;

		scale_regard = 
			(double) map.GetAlliedValue(receiver, city.RetPos()) / 
					 map.GetMaxAlliedValue(receiver);

		break;
	case PROPOSAL_OFFER_WITHDRAW_TROOPS:
		

		scale_regard = 0.0;
		if ((agreements.GetAgreementDuration(sender, receiver, proposal_type) > duration) &&
			receiver_diplomat.GetBorderIncursionBy(sender))
		{
			scale_regard = (double) map.AtRiskCitiesValue(receiver,sender) / 100.0;
		}
		break;
	case PROPOSAL_REQUEST_WITHDRAW_TROOPS:
		

		scale_regard = 0.0;
		if ((agreements.GetAgreementDuration(receiver, sender, proposal_type) > duration) &&
			sender_diplomat.GetBorderIncursionBy(receiver))
		{
			scale_regard = (double) map.AtRiskCitiesValue(sender,receiver) / 100.0;
		}

		break;
	case PROPOSAL_OFFER_STOP_PIRACY:
		
		
		scale_regard = 0.0;
		if (map.TotalPiracyLoss(receiver) > 0)
		{
			scale_regard = (double) map.GetPiracyIncomeByPlayer(sender, receiver) / 
				map.TotalPiracyLoss(receiver);
		}

		break;
	case PROPOSAL_REQUEST_STOP_PIRACY:
		

		scale_regard = 0.0;
		if (map.TotalPiracyLoss(sender) > 0)
		{
			scale_regard = (double) map.GetPiracyIncomeByPlayer(receiver, sender) / 
				map.TotalPiracyLoss(sender);
		}

		break;
	case PROPOSAL_OFFER_BREAK_AGREEMENT:
		
		if (turns_since_last_agreed >= 0 && turns_since_last_agreed < duration)
			scale_regard = ((double)turns_since_last_agreed / duration );

		break;
	case PROPOSAL_REQUEST_BREAK_AGREEMENT:
		
		if (turns_since_last_agreed >= 0 && turns_since_last_agreed < duration)
			scale_regard = ((double)turns_since_last_agreed / duration );

		break;
	case PROPOSAL_OFFER_STOP_RESEARCH:
		
		if (turns_since_last_agreed >= 0 && turns_since_last_agreed < duration)
			scale_regard = ((double)turns_since_last_agreed / duration );

		break;
	case PROPOSAL_REQUEST_STOP_RESEARCH:
		
		if (turns_since_last_agreed >= 0 && turns_since_last_agreed < duration)
			scale_regard = ((double)turns_since_last_agreed / duration );

		break;
	case PROPOSAL_OFFER_REDUCE_NUCLEAR_WEAPONS:
		
		if (turns_since_last_agreed >= 0 && turns_since_last_agreed < duration/2)
			scale_regard = MIN((proposal_arg.percent + 0.25), 1.0);
		break;
	case PROPOSAL_REQUEST_REDUCE_NUCLEAR_WEAPONS:
		
		if (turns_since_last_agreed >= 0 && turns_since_last_agreed < duration/2)
			scale_regard = MIN((proposal_arg.percent + 0.25), 1.0);
		break;
	case PROPOSAL_OFFER_REDUCE_BIO_WEAPONS:
		
		if (turns_since_last_agreed >= 0 && turns_since_last_agreed < duration/2)
			scale_regard = MIN((proposal_arg.percent + 0.25), 1.0);
		break;
	case PROPOSAL_REQUEST_REDUCE_BIO_WEAPONS:
		
		if (turns_since_last_agreed >= 0 && turns_since_last_agreed < duration/2)
			scale_regard = MIN((proposal_arg.percent + 0.25), 1.0);
		break;
	case PROPOSAL_OFFER_REDUCE_NANO_WEAPONS:
		
		if (turns_since_last_agreed >= 0 && turns_since_last_agreed < duration/2)
			scale_regard = MIN((proposal_arg.percent + 0.25), 1.0);
		break;
	case PROPOSAL_REQUEST_REDUCE_NANO_WEAPONS:
		
		if (turns_since_last_agreed >= 0 && turns_since_last_agreed < duration/2)
			scale_regard = MIN((proposal_arg.percent + 0.25), 1.0);
		break;
	case PROPOSAL_OFFER_GIVE_ADVANCE:
		
		
		advance_record = g_theAdvanceDB->Get(proposal_arg.advanceType);
		Assert(advance_record != NULL);
		if (advance_record != NULL)
		{
			senderResult.science -= 
				advance_record->GetCost();
			receiverResult.science += 
				advance_record->GetCost();

			if (receiver_ptr->m_advances)
			{
				scale_regard = 
					(double) advance_record->GetCost() /
					g_theAdvanceDB->Get(receiver_ptr->m_advances->GetResearching())->GetCost();
			}
		}
		break;
	case PROPOSAL_REQUEST_GIVE_ADVANCE:
		
		
		advance_record = g_theAdvanceDB->Get(proposal_arg.advanceType);
		Assert(advance_record != NULL);
		if (advance_record != NULL)
		{
			senderResult.science += 
				advance_record->GetCost();
			receiverResult.science -= 
				advance_record->GetCost();
			
			if (sender_ptr->m_advances)
			{
				scale_regard = 
					(double) advance_record->GetCost() /
					g_theAdvanceDB->Get(sender_ptr->m_advances->GetResearching())->GetCost();
			}
		}

		break;
	case PROPOSAL_OFFER_GIVE_GOLD:
		
		senderResult.gold -= proposal_arg.gold;
		receiverResult.gold += proposal_arg.gold;

		scale_regard = 
			(double) proposal_arg.gold / receiver_ptr->m_gold->GetIncome();

		break;
	case PROPOSAL_REQUEST_GIVE_GOLD:
		
		senderResult.gold += proposal_arg.gold;
		receiverResult.gold -= proposal_arg.gold;

		scale_regard = 
			(double) proposal_arg.gold / sender_ptr->m_gold->GetIncome();

		break;
	case PROPOSAL_OFFER_REDUCE_POLLUTION:
		
		if (turns_since_last_agreed >= 0 && turns_since_last_agreed < duration)
			scale_regard = MIN((proposal_arg.percent + 0.25), 1.0);
		break;
	case PROPOSAL_REQUEST_REDUCE_POLLUTION:
		
		if (turns_since_last_agreed >= 0 && turns_since_last_agreed < duration)
			scale_regard = MIN((proposal_arg.percent + 0.25), 1.0);
		break;
	case PROPOSAL_OFFER_MAP:
		
		if (turns_since_last_agreed >= 0 && turns_since_last_agreed < duration)
			scale_regard = ((double)turns_since_last_agreed / duration );
		break;
	case PROPOSAL_REQUEST_MAP:
		
		if (turns_since_last_agreed >= 0 && turns_since_last_agreed < duration)
			scale_regard = ((double)turns_since_last_agreed / duration );
		break;
	case PROPOSAL_OFFER_HONOR_MILITARY_AGREEMENT:
		
		if (turns_since_last_agreed >= 0 && turns_since_last_agreed < duration)
			scale_regard = ((double)turns_since_last_agreed / duration );
		break;
	case PROPOSAL_REQUEST_HONOR_MILITARY_AGREEMENT:
		
		if (turns_since_last_agreed >= 0 && turns_since_last_agreed < duration)
			scale_regard = ((double)turns_since_last_agreed / duration );
		break;
	case PROPOSAL_OFFER_HONOR_POLLUTION_AGREEMENT:
		
		if (turns_since_last_agreed >= 0 && turns_since_last_agreed < duration)
			scale_regard = ((double)turns_since_last_agreed / duration );
		break;
	case PROPOSAL_REQUEST_HONOR_POLLUTION_AGREEMENT:
		
		if (turns_since_last_agreed >= 0 && turns_since_last_agreed < duration)
			scale_regard = ((double)turns_since_last_agreed / duration );
		break;
	case PROPOSAL_OFFER_END_EMBARGO:
		
		if (turns_since_last_agreed >= 0 && turns_since_last_agreed < duration)
			scale_regard = ((double)turns_since_last_agreed / duration );
		break;
	case PROPOSAL_REQUEST_END_EMBARGO:
		
		if (turns_since_last_agreed >= 0 && turns_since_last_agreed < duration)
			scale_regard = ((double)turns_since_last_agreed / duration );
		break;
	
	case PROPOSAL_TREATY_DECLARE_WAR:
		scale_regard = 1.0;
		break;
	case PROPOSAL_TREATY_CEASEFIRE:
		scale_regard = 1.0;
		break;
	case PROPOSAL_TREATY_PEACE:
		scale_regard = 1.0;
		break;
	case PROPOSAL_TREATY_TRADE_PACT:
		scale_regard = 1.0;
		break;
	case PROPOSAL_TREATY_RESEARCH_PACT:
		scale_regard = 1.0;
		break;
	case PROPOSAL_TREATY_MILITARY_PACT:
		scale_regard = 1.0;
		break;
	case PROPOSAL_TREATY_POLLUTION_PACT:
		scale_regard = 1.0;
		break;
	case PROPOSAL_TREATY_ALLIANCE:
		scale_regard = 1.0;
		break;
	case PROPOSAL_MAX:
	default:
		
		Assert(0);
	}

	
	senderResult.regard += 
		(sint32) (scale_regard * receiver_diplomat.GetReceiverRegardResult(sender, proposal_type));

	
	receiverResult.regard += 
		(sint32) (scale_regard * sender_diplomat.GetSenderRegardResult(receiver, proposal_type));

}



void ProposalAnalysis::ComputeRegardResultFromProfit(DiplomacyResult & senderResult,
													 DiplomacyResult & receiverResult)
{
	
	
	sint32 total_receiver_result = receiverResult.gold + receiverResult.production + receiverResult.science;
	sint32 total_sender_result = senderResult.gold + senderResult.production + senderResult.science;

	
	
	double sender_profit_ratio = 0.0;
	
	if (total_sender_result > total_receiver_result)
	{
		if (total_receiver_result > 0)
		{
			sender_profit_ratio = 
				(((double)total_sender_result - total_receiver_result) / (total_sender_result + total_receiver_result) );
		}
		else
		{
			
			sender_profit_ratio = 1.0;
		}
	}
	if (total_receiver_result > total_sender_result)
	{
		if (total_sender_result > 0)
		{
			sender_profit_ratio = -1.0 *
				(((double)total_receiver_result - total_sender_result) / (total_sender_result + total_receiver_result));
		}
		else
		{
			
			sender_profit_ratio = -1.0;
		}
	}

	

	
	
	
	senderResult.regard -= sender_profit_ratio * ((double)MAX_REGARD / 5);
}


bool ProposalAnalysis::IsSimpleGift(const NewProposal & proposal)
{
	
	if (proposal.detail.second_type != PROPOSAL_NONE)
		return false;

	
	if (IsGift(proposal.detail))
		return true;

	
	return false;
}


bool ProposalAnalysis::IsGift(const ProposalData & proposal_data)
{
	const DiplomacyProposalRecord *first_rec = 
		g_theDiplomacyProposalDB->Get(diplomacyutil_GetDBIndex(proposal_data.first_type));
	
	const DiplomacyProposalRecord *second_rec = 
		g_theDiplomacyProposalDB->Get(diplomacyutil_GetDBIndex(proposal_data.second_type));
	
	if (second_rec->GetCategoryGift())
	{
		return true;
	}

	if (first_rec->GetCategoryGift())
	{
		return true;
	}
	return false;
}


bool ProposalAnalysis::GetTreatyFromProposal(const ProposalData & proposal_data, 
		PROPOSAL_TYPE & treaty_type, 
		DiplomacyArg & treaty_arg,
		bool & includes_gift,
		PROPOSAL_TYPE & other_type,
		DiplomacyArg & other_arg)
{
	bool treaty_found = false;
	other_type = PROPOSAL_NONE;
	includes_gift = false;

	const DiplomacyProposalRecord *first_rec = 
		g_theDiplomacyProposalDB->Get(diplomacyutil_GetDBIndex(proposal_data.first_type));

	const DiplomacyProposalRecord *second_rec = 
		g_theDiplomacyProposalDB->Get(diplomacyutil_GetDBIndex(proposal_data.second_type));

	if (first_rec->GetCategoryGift())
	{
		other_type = proposal_data.first_type;
		other_arg = proposal_data.first_arg;
		includes_gift = true;
	}
	else if (first_rec->GetCategoryRequest())
	{
		other_type = proposal_data.first_type;
		other_arg = proposal_data.first_arg;
		includes_gift = false;
	}
	else if (first_rec->GetCategoryTreaty())
	{
		treaty_type = proposal_data.first_type;
		treaty_arg = proposal_data.first_arg;
		treaty_found = true;
	}

	if (second_rec->GetCategoryGift())
	{
		other_type = proposal_data.second_type;
		other_arg = proposal_data.second_arg;
		includes_gift = true;
	}
	else if (second_rec->GetCategoryRequest())
	{
		other_type = proposal_data.second_type;
		other_arg = proposal_data.second_arg;
		includes_gift = false;
	}
	else if (second_rec->GetCategoryTreaty())
	{
		treaty_type = proposal_data.second_type;
		treaty_arg = proposal_data.second_arg;

		
		if (treaty_found)
			treaty_found = false;
		else
			treaty_found = true;
	}

	
	return (treaty_found);
}


bool ProposalAnalysis::IsAcceptResponse(const Response & response)
{
	
	if (response.type != RESPONSE_COUNTER && 
		response.type != RESPONSE_ACCEPT)
		return false;

	
	if (response.type == RESPONSE_COUNTER)
	{
		switch (response.counter.second_type)
		{
		case PROPOSAL_REQUEST_GIVE_CITY:
		case PROPOSAL_REQUEST_WITHDRAW_TROOPS:
		case PROPOSAL_REQUEST_STOP_PIRACY:
		case PROPOSAL_REQUEST_BREAK_AGREEMENT:
		case PROPOSAL_REQUEST_STOP_RESEARCH:
		case PROPOSAL_REQUEST_REDUCE_NUCLEAR_WEAPONS:
		case PROPOSAL_REQUEST_REDUCE_BIO_WEAPONS:
		case PROPOSAL_REQUEST_REDUCE_NANO_WEAPONS:
		case PROPOSAL_REQUEST_GIVE_ADVANCE:
		case PROPOSAL_REQUEST_GIVE_GOLD:
		case PROPOSAL_REQUEST_REDUCE_POLLUTION:
		case PROPOSAL_REQUEST_MAP:
		case PROPOSAL_REQUEST_HONOR_MILITARY_AGREEMENT:
		case PROPOSAL_REQUEST_HONOR_POLLUTION_AGREEMENT:
		case PROPOSAL_REQUEST_END_EMBARGO:
			return true;
		}
	}

	
	return false;
}




void ProposalAnalysis::AcceptTreatyCondition(const PLAYER_INDEX playerId,
											 const PLAYER_INDEX foreignerId,
											 const PROPOSAL_TYPE treaty_type, 
											 const DiplomacyArg treaty_arg, 
											 PROPOSAL_TYPE & desire_type, 
											 DiplomacyArg & desire_arg )
{

	
	desire_type = PROPOSAL_MAX;

	Diplomat & diplomat = Diplomat::GetDiplomat(playerId);

	
	bool not_friend = !diplomat.TestEffectiveRegard(foreignerId, FRIEND_REGARD);

	
	bool recent_attacks = (diplomat.GetLastHotwarAttack(foreignerId) < 10);

	
	bool piracy = (MapAnalysis::GetMapAnalysis().GetPiracyIncomeByPlayer(foreignerId, playerId) > 0);

	
	bool border_incursion = (diplomat.GetBorderIncursionBy(foreignerId));

	
	bool untrustworthy = (diplomat.GetTrust(foreignerId) < NEUTRAL_REGARD);

	
	bool has_embargo = (Diplomat::GetDiplomat(foreignerId).GetEmbargo(playerId));

	
	bool has_alliance = 
		AgreementMatrix::s_agreements.HasAgreement(playerId, foreignerId, PROPOSAL_TREATY_ALLIANCE);

	double regard_ratio = 1.0;
	if (not_friend)
	{
		sint32 regard_delta = FRIEND_REGARD - diplomat.GetPublicRegard(foreignerId);
		regard_ratio = regard_delta / diplomat.GetReceiverRegardResult(foreignerId, treaty_type);
	}
	
	Player *player_ptr = g_player[playerId];
	if (player_ptr == NULL)
	{
		Assert(0);
		return;
	}
	if (treaty_type == PROPOSAL_TREATY_TRADE_PACT)
	{
		sint32 trade_from = diplomat.GetTradeFrom(foreignerId);
		sint32 trade_total = MapAnalysis::GetMapAnalysis().GetTotalTrade(playerId);
		
		
		double trade_percent = (double) trade_from / trade_total;
		bool not_trading_partner = (trade_percent < 0.10);

		
		if (!not_trading_partner && !not_friend && !recent_attacks && !piracy && !untrustworthy && !has_embargo)
		{
			desire_type = PROPOSAL_NONE;
		}

		
		else if (not_friend && !not_trading_partner && !recent_attacks && !piracy && !untrustworthy && !has_embargo)
		{
			desire_type = PROPOSAL_OFFER_GIVE_GOLD;
			desire_arg.gold = player_ptr->m_gold->GetIncome() * regard_ratio;

			if (has_alliance)
			{
				desire_arg.gold = player_ptr->m_gold->GetIncome() * regard_ratio * 0.5;
			}

		}
		
		
		else if (has_embargo && !not_trading_partner && !recent_attacks && !piracy && !untrustworthy && !not_friend )
		{
			desire_type = PROPOSAL_OFFER_END_EMBARGO;
			desire_arg.gold = player_ptr->m_gold->GetIncome() * regard_ratio;
		}

		
		else if (not_trading_partner && !has_embargo && !recent_attacks && !piracy && !untrustworthy && !not_friend )
		{
			if (trade_total > 0)
			{
				desire_type = PROPOSAL_OFFER_GIVE_GOLD;
				desire_arg.gold =  5 * (0.10 - trade_percent) * MapAnalysis::GetMapAnalysis().GetTotalTrade(playerId);
			}
			if (has_alliance)
			{
				desire_type = PROPOSAL_NONE;
			}
		}
	}

	if (treaty_type == PROPOSAL_TREATY_RESEARCH_PACT)
	{
		sint32 player_science = MapAnalysis::GetMapAnalysis().GetProjectedScience(playerId);
		sint32 foreigner_science = MapAnalysis::GetMapAnalysis().GetProjectedScience(foreignerId);

		bool very_low_science = (player_science > 5 * foreigner_science);
		
		
		if (!very_low_science && !not_friend && !recent_attacks && !piracy && !untrustworthy && !has_embargo)
		{
			desire_type = PROPOSAL_NONE;
		}

		
		else if (not_friend && !very_low_science && !recent_attacks && !piracy && !untrustworthy && !has_embargo)
		{
			desire_type = PROPOSAL_OFFER_GIVE_GOLD;
			desire_arg.gold = player_ptr->m_gold->GetIncome() * regard_ratio;

			
			if (has_alliance)
			{
				desire_arg.gold = player_ptr->m_gold->GetIncome() * regard_ratio * 0.5;
			}
		}
		
		
		else if (has_embargo && !very_low_science && !recent_attacks && !piracy && !untrustworthy && !not_friend )
		{
			desire_type = PROPOSAL_OFFER_END_EMBARGO;
			desire_arg.gold = player_ptr->m_gold->GetIncome() * regard_ratio;
		}

		
		else if (very_low_science && !has_embargo && !recent_attacks && !piracy && !untrustworthy && !not_friend )
		{
			if (player_science - foreigner_science > 100)
			{
				desire_type = PROPOSAL_OFFER_GIVE_GOLD;
				desire_arg.gold =  (player_science - foreigner_science) * 5;
			}

			
			if (has_alliance)
			{
				desire_type = PROPOSAL_NONE;
			}

		}
	}
	
	if (treaty_type == PROPOSAL_TREATY_MILITARY_PACT)
	{
		
		bool cannot_form_alliance = !diplomat.CanFormAlliance(foreignerId);
		bool desire_war = (diplomat.DesireWarWith(foreignerId));

		
		if (has_alliance)
		{
			desire_type = PROPOSAL_NONE;
		}

		
		else if (!desire_war && !cannot_form_alliance && !not_friend && !recent_attacks && !piracy && !untrustworthy && !has_embargo)
		{
			desire_type = PROPOSAL_NONE;
		}

		
		else if (not_friend && !desire_war && !cannot_form_alliance && !recent_attacks && !piracy && !untrustworthy && !has_embargo)
		{
			desire_type = PROPOSAL_OFFER_GIVE_GOLD;
			desire_arg.gold = player_ptr->m_gold->GetIncome() * regard_ratio;
		}
		
		
		else if (has_embargo && !desire_war && !cannot_form_alliance && !recent_attacks && !piracy && !untrustworthy && !not_friend )
		{
			desire_type = PROPOSAL_OFFER_END_EMBARGO;
			desire_arg.gold = player_ptr->m_gold->GetIncome() * regard_ratio;
		}

	}
	if (treaty_type == PROPOSAL_TREATY_POLLUTION_PACT)
	{
		uint32 player_pollution = g_player[playerId]->GetPollutionLevel();
		uint32 foreigner_pollution = g_player[foreignerId]->GetPollutionLevel();
		double pollution_ratio = (double) player_pollution / foreigner_pollution;
		
		
		double requested_reduction = 1.0 - ((double)treaty_arg.pollution / player_pollution);

		
		if (diplomat.GetPersonality()->GetDiscoveryEcotopian() && pollution_ratio > 1.05)
		{
			desire_type = PROPOSAL_OFFER_REDUCE_POLLUTION;
			desire_arg.pollution = player_pollution * 1.05;
		}
		
		
		else if (diplomat.GetPersonality()->GetDiscoveryDiplomatic() &&
			(pollution_ratio < 0.8 || 
			 requested_reduction > 0.2))
		{
			desire_type = PROPOSAL_MAX;
		}
		
		else if (requested_reduction > 0.1)
		{
			desire_type = PROPOSAL_MAX;
		}
		
		
		desire_type = PROPOSAL_NONE;
	}
	if (treaty_type == PROPOSAL_TREATY_CEASEFIRE)
	{
		bool desire_war = (diplomat.DesireWarWith(foreignerId));

		
		if (!desire_war)
		{
			desire_type = PROPOSAL_NONE;
		}

		
		else 
		{
			desire_type = PROPOSAL_OFFER_GIVE_GOLD;
			desire_arg.gold = player_ptr->m_gold->GetIncome() * 33;
		}
	}
	if (treaty_type == PROPOSAL_TREATY_PEACE)
	{
		
		return;
	}
	if (treaty_type == PROPOSAL_TREATY_ALLIANCE)
	{
		
		return;
	}
}



bool ProposalAnalysis::PlayerGetsEnoughValue(const PLAYER_INDEX playerId,
											 const PLAYER_INDEX senderId,
											 const PLAYER_INDEX receiverId,
											 const ProposalData detail) 
{
	DiplomacyResult senderResult;
	DiplomacyResult receiverResult;

	
	ComputeResult( senderId,
				   receiverId,
				   detail.first_type,
				   detail.first_arg,
				   senderResult,
				   receiverResult );

	ComputeResult( senderId,
				   receiverId,
				   detail.second_type,
				   detail.second_arg,
				   senderResult,
				   receiverResult );
	

	const Diplomat & diplomat = Diplomat::GetDiplomat(playerId);

	
	double min_delta_gold_percent = -0.2;
	
	double min_delta_science_percent = -0.2; 

	
	if (diplomat.GetPersonality()->GetDiscoveryScientist())
	{
		
		min_delta_gold_percent = -0.1; 
		
		min_delta_science_percent = -0.3; 
	}

	
	if (diplomat.GetPersonality()->GetDiscoveryScientist())
	{
		
		min_delta_gold_percent = -0.3; 
		
		min_delta_science_percent = -0.1; 
	}

	
	ai::Regard regard = diplomat.GetEffectiveRegard(receiverId);
	if (regard > ALLIED_REGARD ||
		AgreementMatrix::s_agreements.HasAgreement(senderId, receiverId, PROPOSAL_TREATY_RESEARCH_PACT) ||
		AgreementMatrix::s_agreements.HasAgreement(senderId, receiverId, PROPOSAL_TREATY_ALLIANCE))
	{
		
		min_delta_gold_percent *= 2; 
		
		min_delta_science_percent *= 2; 
	}

	if (regard < COLDWAR_REGARD)
	{
		
		min_delta_gold_percent /= 2;
		
		min_delta_science_percent /= 2; 
	}
	
	DiplomacyResult & playerResult = senderResult;
	DiplomacyResult & foreignerResult = receiverResult;
	if (playerId == receiverId)
	{
		playerResult = receiverResult;
		foreignerResult = senderResult;
	}

	
	if (playerResult.science <= 0)
		return false;

	
	if (foreignerResult.science <= 0 &&
		foreignerResult.gold < (playerResult.science + (playerResult.science * min_delta_gold_percent)))
	{
		return true;
	}
	
	
	if (playerResult.science > (foreignerResult.science + (playerResult.science * min_delta_science_percent)) &&
		foreignerResult.gold <= 0)
	{
		return true;
	}

	return false;
}
