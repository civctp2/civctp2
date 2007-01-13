








#ifdef HAVE_PRAGMA_ONCE
#pragma once
#endif
#ifndef __AGREEMENTPOOL_H__
#define __AGREEMENTPOOL_H__

#include "ObjPool.h"

class AgreementData;
class Agreement;

#include "AgreementTypes.h"

class DiplomaticRequest ;


class AgreementPool : public ObjPool
{
public:
	AgreementData* AccessAgreement(const Agreement id);
	AgreementData* GetAgreement(const Agreement id) const;
	
	AgreementPool() ;
	AgreementPool(CivArchive &archive) ;

	Agreement Create(PLAYER_INDEX owner, PLAYER_INDEX recipient, AGREEMENT_TYPE request) ;
	void EndRound(void) ;

	void Serialize(CivArchive &archive) ;

} ;

extern AgreementPool* g_theAgreementPool ;
#else

class AgreementPool ;

#endif
