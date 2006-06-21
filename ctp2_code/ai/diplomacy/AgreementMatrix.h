//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ header
// Description  : Matrix of diplomatic agreements
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
//----------------------------------------------------------------------------
//
// Modifications from the original Activision code:
//
// - Decreased MSVC dependency.
// - Generalised index computation to include proposal type.
// - Removed unused HasAgreementIndexed function.
//
//----------------------------------------------------------------------------

#ifdef HAVE_PRAGMA_ONCE
#pragma once
#endif

#ifndef __AGREEMENT_MATRIX_H__
#define __AGREEMENT_MATRIX_H__

//----------------------------------------------------------------------------
// Library dependencies
//----------------------------------------------------------------------------

#include <vector>

//----------------------------------------------------------------------------
// Export overview
//----------------------------------------------------------------------------

class AgreementMatrix;

//----------------------------------------------------------------------------
// Project dependencies
//----------------------------------------------------------------------------

#include "CivArchive.h"     // CivArchive
#include "diplomattypes.h"  // ai::..., PROPOSAL_TYPE
#include "player.h"         // PLAYER_INDEX

//----------------------------------------------------------------------------
// Class declarations
//----------------------------------------------------------------------------

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

	
	void SetAgreementFast(size_t index, const ai::Agreement &agreement);
 
	
	sint16 GetMaxPlayers() const { return m_maxPlayers; }


	size_t AgreementIndex
    (
        PLAYER_INDEX const &    sender_player,
		PLAYER_INDEX const &    receiver_player,
        PROPOSAL_TYPE const &   proposalType    = PROPOSAL_NONE
    ) const
	{
		Assert((sender_player   < m_maxPlayers) &&
               (receiver_player < m_maxPlayers) &&
               (proposalType    < PROPOSAL_MAX)
              );

		return static_cast<size_t>
            ((PROPOSAL_MAX * m_maxPlayers * receiver_player) + 
             (PROPOSAL_MAX * sender_player) +
             proposalType
            );
	}

	void ClearAgreementsInvolving(const PLAYER_INDEX playerId);

private:
	AgreementVector m_agreements;
	sint16 m_maxPlayers;
};

#endif // __AGREEMENT_MATRIX_H__
