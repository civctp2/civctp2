









#pragma once
#ifndef __DIPLOMATICREQUESTDATA_H__
#define __DIPLOMATICREQUESTDATA_H__

#include "GameObj.h"
#include "ID.h"
#include "gold.h"
#include "Unit.h"


enum ATTITUDE_TYPE ;
typedef sint32 AdvanceType;

class CivArchive;

#include "diplomatictypes.h"

class DiplomaticRequestData : public GAMEOBJ
	{
	private:
		
		sint32	m_round ;											
		
		PLAYER_INDEX	m_owner,									
						m_recipient,								
						m_thirdParty ;								

		REQUEST_TYPE	m_request ;									
		REQUEST_RESPONSE_TYPE	m_response ;						



		sint32 m_tone;                                       

		AdvanceType	m_advance ;										
		AdvanceType	m_reciprocalAdvance ;							
		

		
		Unit	m_targetCity ;										
		Unit	m_reciprocalCity ;									
		Gold	m_amount ;											
		

		friend class NetDiplomaticRequest;
		friend class NetAction;

	public:
		DiplomaticRequestData(const ID id) ;
		DiplomaticRequestData(CivArchive &archive) : GAMEOBJ(0) { Serialize(archive) ; }
		DiplomaticRequestData(const ID id, PLAYER_INDEX sender, PLAYER_INDEX recipient, REQUEST_TYPE request) ;

		void MakeRequest(const PLAYER_INDEX owner, const PLAYER_INDEX recipient, const REQUEST_TYPE request) ;
		REQUEST_TYPE GetRequest(void) const { return (m_request) ; }

		PLAYER_INDEX GetOwner(void) const { return (m_owner) ; }	
		PLAYER_INDEX GetRecipient(void) const { return (m_recipient) ; }	
		void SetAdvance(const AdvanceType &advance);
		void SetWanted(const AdvanceType &advance);
		void SetWanted(const Unit &city);
		void SetThirdParty(const PLAYER_INDEX thirdParty) ;
		void SetResponse(const REQUEST_RESPONSE_TYPE response) ;



		void SetTarget(const Unit &city) ;
		void SetGold(const Gold &amount) ;

		AdvanceType GetAdvance()  { return m_advance; }
		AdvanceType GetWantedAdvance() { return m_reciprocalAdvance; }
		Unit &GetWantedCity()  { return m_reciprocalCity; }
		PLAYER_INDEX GetThirdParty() { return m_thirdParty; }
		REQUEST_RESPONSE_TYPE GetResponse() { return m_response; }



		Unit &GetTarget() { return m_targetCity; }
		Gold &GetGold() { return m_amount; }

		void Dump(const sint32 i) ;
		void Enact(BOOL fromServer = FALSE) ;
		void InterpretString(MBCHAR *ip, MBCHAR *op) ;
		void GetRequestString(MBCHAR *s) ;
		void Reject(BOOL fromServer = FALSE) ;
		void Serialize(CivArchive &archive) ;
		ATTITUDE_TYPE GetAttitude(PLAYER_INDEX p1, PLAYER_INDEX p2) ;
		MBCHAR *GetRequestString(void) ;

		sint32 GetTone() const;

		sint32 GetRound() const { return m_round; }

		void Complete();

		DiplomaticRequestData *GetLesser() { return (DiplomaticRequestData*)m_lesser; }
		DiplomaticRequestData *GetGreater() { return (DiplomaticRequestData*)m_greater; }

	} ;

#endif
