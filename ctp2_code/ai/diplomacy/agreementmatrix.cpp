//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : Matrix of all diplomatic agreements between players
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
// _DEBUG
// - Generate debug version when set.
//
//----------------------------------------------------------------------------
//
// Modifications from the original Activision code:
//
// - Input checks corrected, preventing a crash to desktop.
// - Made some sizes unsigned, to allow more than 28 players.
// - Standardised min/max usage.
//
//----------------------------------------------------------------------------

#include "c3.h"

#include <vector>
using namespace std;

#include "civarchive.h"
#include "AgreementMatrix.h"
#include "player.h"

#include "diplomacyutil.h"
#include "DiplomacyProposalRecord.h"
#include "newturncount.h"
#include "ctpai.h"
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
	// Just make sure that we can rely on testing against m_maxPlayers to have
	// a valid index in g_player.
	Assert(newMaxPlayers <= k_MAX_PLAYERS);
	m_maxPlayers = std::min<sint16>(newMaxPlayers, k_MAX_PLAYERS);
	
	m_agreements.clear();
	
	
	m_agreements.resize( m_maxPlayers * m_maxPlayers * PROPOSAL_MAX, s_badAgreement );

#ifdef _DEBUG
	
	if (m_agreements.size() == 0)
	{
		m_agreements.~vector();
		return;
	}
#endif // _DEBUG

	
	
	
	

	for (size_t index = 0; index < old_agreements.size(); index++ )
		{
			PLAYER_INDEX const	senderId	= old_agreements[index].senderId;
			PLAYER_INDEX const	receiverId	= old_agreements[index].receiverId;
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
	archive >> m_maxPlayers;

	uint16	count;
	archive >> count;
	m_agreements.resize(count);

	for (size_t i = 0; i < count; ++i)
	{
		archive.Load(reinterpret_cast<uint8 *>(&(m_agreements[i])),
					 sizeof(ai::Agreement)
					);
	}
}


void AgreementMatrix::Save(CivArchive & archive) const
{
	archive << m_maxPlayers;

	Assert(m_agreements.size() < 0x10000);	// Report when it is too large.
	archive << static_cast<uint16>(m_agreements.size());

	for (size_t i = 0; i < m_agreements.size(); ++i)
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
	Assert((unsigned) index < m_agreements.size());
	ai::Agreement agreement = m_agreements[ index ];
	if (agreement != AgreementMatrix::s_badAgreement)
	{
		if ((agreement.receiverId != receiver_player && agreement.receiverId != sender_player) ||
		    (agreement.senderId != sender_player && agreement.senderId != receiver_player))
		{
			
			return AgreementMatrix::s_badAgreement;
		}
	}
#endif // _DEBUG

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

	
	Assert((unsigned) agreementIndex < m_agreements.size());
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
	
	
	Assert((unsigned) agreementIndex < m_agreements.size());
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
		
		
		Assert((unsigned) agreementIndex < m_agreements.size());
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
		
		
		Assert((unsigned) agreementIndex < m_agreements.size());
		m_agreements[agreementIndex] = agreement;
		
#ifdef _DEBUG
		if (agreement != GetAgreement(agreement.receiverId, agreement.senderId, reciprocalType))
		{
			Assert(0);
		}
#endif

	} 
}




//----------------------------------------------------------------------------
//
// Name       : AgreementMatrix::HasAgreement
//
// Description: Test whether 2 players have the indicated agreement.
//
// Parameters : sender_player	: initiator of the agreement
//				receiver_player	: responder to the agreement
//				type			: type of the agreement
//
// Globals    : g_player		: all players in the game
//
// Returns    : -
//
// Remark(s)  : -
//
//----------------------------------------------------------------------------
bool AgreementMatrix::HasAgreement(const PLAYER_INDEX & sender_player,
								   const PLAYER_INDEX & receiver_player,
								   const PROPOSAL_TYPE type) const
{
	Assert(sender_player < m_maxPlayers);
	Assert(sender_player >= 0);

	if ((sender_player >= m_maxPlayers)  || (sender_player < 0))
	{
		return false;
	}

	Assert(receiver_player < m_maxPlayers);
	Assert(receiver_player >= 0);

	if ((receiver_player >= m_maxPlayers) || (receiver_player < 0))
	{
		return false;
	}

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
	Assert((unsigned) index < m_agreements.size());
	if((unsigned) index < m_agreements.size()) {
		m_agreements[index] = agreement;
	}
}


void AgreementMatrix::ClearAgreementsInvolving(const PLAYER_INDEX playerId)
{
	for (size_t i = 0; i < m_agreements.size(); i++)
	{
		if ((m_agreements[i].senderId == playerId) ||
			(m_agreements[i].receiverId == playerId))
			m_agreements[i] = s_badAgreement;
	}
}
