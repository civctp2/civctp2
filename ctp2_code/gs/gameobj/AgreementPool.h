









#pragma once
#ifndef __AGREEMENTPOOL_H__
#define __AGREEMENTPOOL_H__

#include "ObjPool.h"

class AgreementData;
class Agreement;

enum DIPLOMACY_TYPE ;
enum DIPLOMATIC_REQUEST_TYPE ;
enum DIPLOMATIC_ITEM_TYPE ;

enum AGREEMENT_TYPE;

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
