












#include "c3.h"

#include <vector>
using namespace std;

#include "CivArchive.h"
#include "AgreementMatrix.h"
#include "Player.h"

#include "diplomacyutil.h"
#include "DiplomacyProposalRecord.h"
#include "NewTurnCount.h"
#include "CtpAi.h"
#include "MoveFlags.h"
#include "Diplomat.h"

ai::Agreement AgreementMatrix::s_badAgreement;
AgreementMatrix AgreementMatrix::s_agreements;

AgreementMatrix::AgreementMatrix()
{
	
	m_maxPlayers = 0;
}


void AgreementMatrix::Resize(const PLAYER_INDEX & newMaxPlayers)
{
	
	AgreementVector old_agreements(m_agreements);
	m_maxPlayers = newMaxPlayers;

	
	m_agreements.clear();
	
	
	m_agreements.resize( m_maxPlayers * m_maxPlayers * PROPOSAL_MAX, s_badAgreement );

#ifdef _DEBUG
	
	if (m_agreements.size() == 0)
	{
		m_agreements.~vector();
		return;
	}
#endif _DEBUG

	
	
	
	

	
	sint16 index;
	PLAYER_INDEX senderId;
	PLAYER_INDEX receiverId;
	for ( index = 0; index < old_agreements.size(); index++ )
		{
			senderId = old_agreements[index].senderId;
			receiverId = old_agreements[index].receiverId;
			if (senderId > -1 && senderId < m_maxPlayers &&
				receiverId > -1 && receiverId < m_maxPlayers &&
				g_player[senderId] && !g_player[senderId]->IsDead() &&
				g_player[receiverId] && !g_player[receiverId]->IsDead())
			{
				
				SetAgreement( old_agreements[index] );
			}
		}
}


void AgreementMatrix::Load(CivArchive & archive)
{
	sint16 i,count;
	ai::Agreement agreement;

	
	archive >> count;
	m_maxPlayers = count;

	
	archive >> count;
	m_agreements.resize(count);
	for (i = 0; i < count; i++)
	{
		archive.Load((uint8 *)&agreement, sizeof(ai::Agreement));
		m_agreements[i] = agreement;
	}
}


void AgreementMatrix::Save(CivArchive & archive) const
{
	
	archive << (sint16) m_maxPlayers;

	
	archive << (sint16) m_agreements.size();

	for (sint16 i = 0; i < m_agreements.size(); i++)
	{
		archive.Store((uint8 *) &(m_agreements[i]), sizeof(ai::Agreement));
	}
}


const ai::Agreement & AgreementMatrix::GetAgreement( const PLAYER_INDEX sender_player,
													 const PLAYER_INDEX receiver_player,
													 const PROPOSAL_TYPE type ) const
{
	
	
	Assert(sender_player<m_maxPlayers);
	Assert(receiver_player<m_maxPlayers);

	sint32 index = (receiver_player * m_maxPlayers * PROPOSAL_MAX) +  
		(sender_player * PROPOSAL_MAX) +							  
		type;														  

#ifdef _DEBUG
	Assert(index < m_agreements.size());
	ai::Agreement agreement = m_agreements[ index ];
	if (agreement != AgreementMatrix::s_badAgreement)
	{
		if ((agreement.receiverId != receiver_player && agreement.receiverId != sender_player) ||
		    (agreement.senderId != sender_player && agreement.senderId != receiver_player))
		{
			
			return AgreementMatrix::s_badAgreement;
		}
	}
#endif _DEBUG

	return m_agreements[ index ];
}


sint16 AgreementMatrix::GetAgreementDuration( const PLAYER_INDEX sender_player,
											  const PLAYER_INDEX receiver_player,
											  const PROPOSAL_TYPE type ) const
{
	const ai::Agreement & agreement = GetAgreement(sender_player, receiver_player, type);

	
	if (agreement.start == -1 || agreement.end != -1)
		return -1;

	
	return (NewTurnCount::GetCurrentRound() - agreement.start);
}


void AgreementMatrix::SetAgreement( const ai::Agreement & agreement )
{
	
	
	
	
	
	

	
	if (agreement == AgreementMatrix::s_badAgreement)
		return;

	PROPOSAL_TYPE reciprocalType;
	sint32 reciprocalDBIndex;
	sint32 agreementIndex;

	
	
	

	
	agreementIndex = ( agreement.receiverId * m_maxPlayers * PROPOSAL_MAX) + 
		(agreement.senderId * PROPOSAL_MAX) +							   
		agreement.proposal.first_type;									   

	
	Assert(agreementIndex < m_agreements.size());
	m_agreements[agreementIndex] = agreement;

#ifdef _DEBUG
	if (agreement != GetAgreement(agreement.senderId, agreement.receiverId, agreement.proposal.first_type))
	{
		Assert(0);
	}
#endif

	
	if(diplomacyutil_GetRecord(agreement.proposal.first_type)->GetReciprocalIndex(reciprocalDBIndex)) {
		reciprocalType = diplomacyutil_GetProposalType(reciprocalDBIndex);
	}	
	else
	{
		
		reciprocalType = agreement.proposal.first_type;
	}

	
	agreementIndex = ( agreement.senderId * m_maxPlayers * PROPOSAL_MAX) +   
		(agreement.receiverId * PROPOSAL_MAX) +							   
		reciprocalType ;												   
	
	
	Assert(agreementIndex < m_agreements.size());
	m_agreements[agreementIndex] = agreement;

#ifdef _DEBUG
	if (agreement != GetAgreement(agreement.receiverId, agreement.senderId, reciprocalType))
	{
		Assert(0);
	}
#endif

	
	
	
	if (agreement.proposal.second_type != PROPOSAL_NONE)
	{
		
		
		agreementIndex = ( agreement.receiverId * m_maxPlayers * PROPOSAL_MAX) + 
			(agreement.senderId * PROPOSAL_MAX) +							   
			agreement.proposal.second_type;									   
		
		
		Assert(agreementIndex < m_agreements.size());
		m_agreements[agreementIndex] = agreement;

#ifdef _DEBUG
		if (agreement != GetAgreement(agreement.senderId, agreement.receiverId, agreement.proposal.second_type))
		{
			Assert(0);
		}
#endif
		
		
		if(diplomacyutil_GetRecord(agreement.proposal.second_type)->GetReciprocalIndex(reciprocalDBIndex)) {
			reciprocalType = diplomacyutil_GetProposalType(reciprocalDBIndex);
		}	
		else
		{
			
			reciprocalType = agreement.proposal.second_type;
		}
		
		
		agreementIndex = ( agreement.senderId * m_maxPlayers * PROPOSAL_MAX) +   
			(agreement.receiverId * PROPOSAL_MAX) +							   
			reciprocalType ;												   
		
		
		Assert(agreementIndex < m_agreements.size());
		m_agreements[agreementIndex] = agreement;
		
#ifdef _DEBUG
		if (agreement != GetAgreement(agreement.receiverId, agreement.senderId, reciprocalType))
		{
			Assert(0);
		}
#endif

	} 
}




bool AgreementMatrix::HasAgreement(const PLAYER_INDEX & sender_player,
								   const PLAYER_INDEX & receiver_player,
								   const PROPOSAL_TYPE type) const
{
	Assert(sender_player < k_MAX_PLAYERS);
	Assert(sender_player >= 0);

	if (sender_player > k_MAX_PLAYERS || sender_player <= 0)
		return false;

	Assert(receiver_player < k_MAX_PLAYERS);
	Assert(receiver_player >= 0);
	
	if (receiver_player > k_MAX_PLAYERS || receiver_player <= 0)
		return false;

	Player *player_ptr = g_player[sender_player];
	Assert(player_ptr != NULL);

	if (player_ptr == NULL)
		return false;

	sint32 round = NewTurnCount::GetCurrentRound();
	
	
	const ai::Agreement & agreement = GetAgreement(sender_player, receiver_player, type);

	
	if (agreement.start == -1)
		return false;

	
	if (agreement.end > round || agreement.end == -1)
		return true;
	
	return false;
}



bool AgreementMatrix::HasAgreement(const PLAYER_INDEX & sender_player,
								   const PROPOSAL_TYPE type) const
{
	for (PLAYER_INDEX foreignerId = 0; foreignerId < CtpAi::s_maxPlayers; foreignerId++)
	{
		if (HasAgreement(sender_player, foreignerId, type))
			return true;
	}
	return false;
}


void AgreementMatrix::CancelAgreement(const PLAYER_INDEX & sender_player,
									  const PLAYER_INDEX & receiver_player, 
									  const PROPOSAL_TYPE type )
{
	ai::Agreement agreement = 
		GetAgreement(sender_player, receiver_player, type);
	
	
	agreement.end = NewTurnCount::GetCurrentRound();

	
	SetAgreement(agreement);

	agreement = 
		GetAgreement(receiver_player, sender_player, type);

	
	agreement.end = NewTurnCount::GetCurrentRound();

	
	SetAgreement(agreement);
}


void AgreementMatrix::BreakAgreements(const PLAYER_INDEX & sender_player, const PLAYER_INDEX & foreign_player)
{
	Diplomat & foreign_diplomat = Diplomat::GetDiplomat(foreign_player);

	if (HasAgreement(sender_player, foreign_player, PROPOSAL_TREATY_PEACE))
	{
		CancelAgreement(sender_player, foreign_player, PROPOSAL_TREATY_PEACE);
		foreign_diplomat.LogViolationEvent(sender_player, PROPOSAL_TREATY_PEACE);
	}

	if (HasAgreement(sender_player, foreign_player, PROPOSAL_TREATY_TRADE_PACT))
	{
		CancelAgreement(sender_player, foreign_player, PROPOSAL_TREATY_TRADE_PACT);
		foreign_diplomat.LogViolationEvent(sender_player, PROPOSAL_TREATY_TRADE_PACT);
	}

	if (HasAgreement(sender_player, foreign_player, PROPOSAL_TREATY_RESEARCH_PACT))
	{
		CancelAgreement(sender_player, foreign_player, PROPOSAL_TREATY_RESEARCH_PACT);
		foreign_diplomat.LogViolationEvent(sender_player, PROPOSAL_TREATY_RESEARCH_PACT);
	}

	if (HasAgreement(sender_player, foreign_player, PROPOSAL_TREATY_MILITARY_PACT))
	{
		CancelAgreement(sender_player, foreign_player, PROPOSAL_TREATY_MILITARY_PACT);
		foreign_diplomat.LogViolationEvent(sender_player, PROPOSAL_TREATY_MILITARY_PACT);
	}

	if (HasAgreement(sender_player, foreign_player, PROPOSAL_TREATY_ALLIANCE))
	{
		CancelAgreement(sender_player, foreign_player, PROPOSAL_TREATY_ALLIANCE);
		foreign_diplomat.LogViolationEvent(sender_player, PROPOSAL_TREATY_ALLIANCE);
	}
}


sint32 AgreementMatrix::TurnsSinceLastWar(const PLAYER_INDEX & player,
										  const PLAYER_INDEX & foreigner) const
{

	
	if (HasAgreement(player, foreigner, PROPOSAL_TREATY_DECLARE_WAR))
		return 0;

	const ai::Agreement & last_war =
		GetAgreement(player, foreigner, PROPOSAL_TREATY_DECLARE_WAR);

	
	if (last_war.start == -1)
		return -1;

	return (NewTurnCount::GetCurrentRound() - last_war.end);
}


sint32 AgreementMatrix::TurnsAtWar(const PLAYER_INDEX & player,
											const PLAYER_INDEX & foreigner) const
{
	
	if (HasAgreement(player, foreigner, PROPOSAL_TREATY_PEACE))
		return -1;

	
	if (HasAgreement(player, foreigner, PROPOSAL_TREATY_CEASEFIRE))
		return -1;

	const ai::Agreement & last_war =
		GetAgreement(player, foreigner, PROPOSAL_TREATY_DECLARE_WAR);

	
	if (!HasAgreement(player, foreigner, PROPOSAL_TREATY_DECLARE_WAR))
		return -1;

	
	if (last_war.start == -1) 
		return -1;

	
	
	Assert(last_war.end == -1);
	
	
	return (NewTurnCount::GetCurrentRound() - last_war.start);
}

void AgreementMatrix::SetAgreementFast(sint32 index, const ai::Agreement &agreement)
{
	Assert(index < m_agreements.size());
	if(index < m_agreements.size()) {
		m_agreements[index] = agreement;
	}
}


void AgreementMatrix::ClearAgreementsInvolving(const PLAYER_INDEX playerId)
{
	for (sint16 i = 0; i < m_agreements.size(); i++)
	{
		if ((m_agreements[i].senderId == playerId) ||
			(m_agreements[i].receiverId == playerId))
			m_agreements[i] = s_badAgreement;
	}
}
