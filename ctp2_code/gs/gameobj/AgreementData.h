









#pragma once

#ifndef __AGREEMENTDATA_H__
#define __AGREEMENTDATA_H__

#include "Gameobj.h"
#include "Unit.h"
#include "gstypes.h"


class CivArchive;

typedef sint32 PLAYER_INDEX ;
typedef sint32 AdvanceType;

#define k_EXPIRATION_NEVER		(sint32)(-1)

	
	
#include "AgreementTypes.h"

class AgreementData : public GAMEOBJ
{
private:
	
	
	PLAYER_INDEX	m_owner,			
		            m_recipient,		
		            m_thirdParty ;		
	
	AGREEMENT_TYPE	m_agreement ;		
		
	sint32	m_round,					
		    m_expires ;					
		
	uint32  m_ownerPollution,           
		    m_recipientPollution;       

	BOOL    m_isBroken;                 
	
	

	
	
	Unit	m_targetCity ;				
	
	
	
	
	void ExtractPlayer(sint32 indexId, sint32 memberId, MBCHAR *sExpanded) ;
	void Interpret(MBCHAR *msg, MBCHAR *sInterpreted) ;
	
	
	friend class NetAgreement ;
	friend class NetClientAgreement ;
	
public:
	AgreementData(const ID id) ;
	AgreementData(const ID id, 
				  PLAYER_INDEX sender, 
				  PLAYER_INDEX recipient, 
				  AGREEMENT_TYPE agreement) ;
	AgreementData(CivArchive &archive);
	
	void Init(); 
	
	AGREEMENT_TYPE GetAgreement(void) const { return (m_agreement) ; }
	
	
	PLAYER_INDEX GetOwner(void) const { return (m_owner) ; }
	
	
	PLAYER_INDEX GetRecipient(void) const { return (m_recipient) ; }
	
	
	PLAYER_INDEX GetThirdParty(void) const { return (m_thirdParty) ; }
	
	
	Unit GetTarget(void) const { return (m_targetCity) ; }
	
	
	sint32 GetTurns(void) const { return (m_expires) ; }
	
	
	sint32 GetStartTurn(void) const { return (m_round) ; }
	
	
	BOOL IsExpired(void) const { return (m_expires == 0) ; }
	
	
	BOOL DoesExpire(void) const { return (m_expires != k_EXPIRATION_NEVER) ; }
	
	void SetTarget(const Unit &city) ;
	void SetExpires(const sint32 turns);
	void SetThirdParty(const PLAYER_INDEX player) { 
		Assert((player>=0) && (player<=k_MAX_PLAYERS)) ; 
		m_thirdParty = player ; 
	}
	
	
	void Expire(void) { m_expires = 0 ; }
	
	void MakeAgreement(const PLAYER_INDEX owner, 
					   const PLAYER_INDEX recipient, 
					   const AGREEMENT_TYPE agreement) ;
	void FulfillAgreement(void) ;
	sint32 DecrementTurns(void) ;
	
	
	void EndTurn(void) ;							
	
	void ToString(MBCHAR *s) ;
	
	
	void Dump(const sint32 i) ;
	void Serialize(CivArchive &archive) ;
	
	BOOL IsBroken() const { return m_isBroken; }
	void Break();
	
	void RecipientIsViolating(PLAYER_INDEX curPlayer, BOOL forceBreak = FALSE);
	void OwnerIsViolating(PLAYER_INDEX curPlayer);
	void BeginTurnOwner();
	void BeginTurnRecipient();
};

#endif 
