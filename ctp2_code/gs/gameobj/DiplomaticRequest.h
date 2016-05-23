#ifdef HAVE_PRAGMA_ONCE
#pragma once
#endif
#ifndef __DIPLOMATICREQUEST_H__
#define __DIPLOMATICREQUEST_H__

class DiplomaticRequest;

#include "ID.h"
#include "DiplomaticRequestData.h"

class DiplomaticRequest : public ID
{
public:
	DiplomaticRequest() : ID() { return ; }
	DiplomaticRequest(sint32 val) : ID(val) { return ; }
	DiplomaticRequest(uint32 val) : ID(val) { return ; }

	void KillRequest() ;
	void Kill() { KillRequest() ; }
	void RemoveAllReferences() ;

	const DiplomaticRequestData* GetData() const ;
	DiplomaticRequestData* AccessData() ;

	void Castrate() {} ;
	PLAYER_INDEX GetOwner(void) const { return (GetData()->GetOwner()) ; }
	PLAYER_INDEX GetRecipient(void) const { return (GetData()->GetRecipient()) ; }

	REQUEST_TYPE GetRequest(void) const { return (GetData()->GetRequest()) ; }


	void SetAdvance(const AdvanceType &advance) { AccessData()->SetAdvance(advance) ; }
	void SetWanted(const AdvanceType &advance) { AccessData()->SetWanted(advance) ; }
	void SetWanted(const Unit &city) { AccessData()->SetWanted(city) ; }
	void SetTarget(const Unit &city) { AccessData()->SetTarget(city) ; }
	void SetGold(const Gold &amount) { AccessData()->SetGold(amount) ; }
	void SetThirdParty(const PLAYER_INDEX &third_party) { AccessData()->SetThirdParty(third_party) ; }
	AdvanceType GetAdvance()  { return AccessData()->GetAdvance(); }
	AdvanceType GetWantedAdvance() { return AccessData()->GetWantedAdvance(); }
	Unit &GetWantedCity()  { return AccessData()->GetWantedCity(); }
	PLAYER_INDEX GetThirdParty() { return AccessData()->GetThirdParty(); }
	REQUEST_RESPONSE_TYPE GetResponse() { return AccessData()->GetResponse(); }


	Unit &GetTarget() { return AccessData()->GetTarget(); }
	Gold &GetGold() { return AccessData()->GetGold(); }

	void Reject(BOOL fromServer = FALSE) { AccessData()->Reject(fromServer) ; }
	void Enact(BOOL fromServer = FALSE) { AccessData()->Enact(fromServer) ; }
	void Dump(const sint32 i) { AccessData()->Dump(i) ; }
	MBCHAR *GetRequestString(void) { return (AccessData()->GetRequestString()) ; }

	sint32 GetTone() const;
	sint32 GetRound() const;

	void Complete();
};

#endif
