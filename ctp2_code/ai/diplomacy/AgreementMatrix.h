











#ifdef HAVE_PRAGMA_ONCE
#pragma once
#endif
#ifndef __AGREEMENT_MATRIX_H__
#define __AGREEMENT_MATRIX_H__


#include <vector>
#include "c3debugstl.h"

#include "diplomattypes.h"

class CivArchive; 

class AgreementMatrix
{
public:

	
	
	

	
	static ai::Agreement s_badAgreement;
	static AgreementMatrix s_agreements;

	typedef std::vector<ai::Agreement> AgreementVector;

	AgreementMatrix();

	
	void Resize(const PLAYER_INDEX & newMaxPlayers);

	
	void Load(CivArchive & archive);

	
	void Save(CivArchive & archive) const;

	
	const ai::Agreement & GetAgreement( const PLAYER_INDEX sender_player,
										const PLAYER_INDEX receiver_player,
										const PROPOSAL_TYPE type ) const;

	
	sint16 GetAgreementDuration( const PLAYER_INDEX sender_player,
								 const PLAYER_INDEX receiver_player,
								 const PROPOSAL_TYPE type ) const;
	
	
	void SetAgreement( const ai::Agreement & agreement );

	
	
	
	bool HasAgreement(const PLAYER_INDEX & sender_player,
					  const PLAYER_INDEX & receiver_player,
					  const PROPOSAL_TYPE type) const;

	
	
	bool HasAgreement(const PLAYER_INDEX & sender_player,
					  const PROPOSAL_TYPE type) const;

	
	void CancelAgreement(const PLAYER_INDEX & sender_player,
						 const PLAYER_INDEX & receiver_player, 
						 const PROPOSAL_TYPE type );

	
	void BreakAgreements(const PLAYER_INDEX & sender_player,  const PLAYER_INDEX & foreign_player);

	
	sint32 TurnsSinceLastWar(const PLAYER_INDEX & player,
							 const PLAYER_INDEX & foreigner) const;

	
	sint32 TurnsAtWar(const PLAYER_INDEX & player,
					  const PLAYER_INDEX & foreigner) const;

	
	void SetAgreementFast(sint32 index, const ai::Agreement &agreement);
 
	
	sint16 GetMaxPlayers() { return m_maxPlayers; }


	
	
	sint32 AgreementIndex(const PLAYER_INDEX & sender_player,
							     const PLAYER_INDEX & receiver_player) const
	{
		
		Assert(sender_player<m_maxPlayers);
		Assert(receiver_player<m_maxPlayers);
		return PROPOSAL_MAX*(receiver_player * m_maxPlayers +  
			sender_player);						  
	}

	
	
	
	sint32 HasAgreementIndexed(const sint32 index,
							   const PROPOSAL_TYPE type,
							   const sint32 curRound) const
	{
		
		const ai::Agreement & agreement = m_agreements[ index+type ];

		
		
		return (agreement.start != -1) && ((agreement.end == -1) || (agreement.end > curRound));
	}

	
	void ClearAgreementsInvolving(const PLAYER_INDEX playerId);

private:
	AgreementVector m_agreements;
	sint16 m_maxPlayers;
};

#endif // __AGREEMENT_MATRIX_H__
