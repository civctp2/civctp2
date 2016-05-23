#pragma once

#ifndef __C3Government_H__
#define __C3Government_H__ 1

#include "ic3Government.h"

class Player;
class CivArchive;

class C3Government : public IC3Government
	{
	ULONG m_refCount;

    Player	*m_player ;
	sint32	m_owner ;

	public:

		STDMETHODIMP QueryInterface(REFIID, void **obj) ;
		STDMETHODIMP_(ULONG) AddRef() ;
		STDMETHODIMP_(ULONG) Release() ;

		C3Government(sint32 idx) ;
		C3Government(CivArchive &archive) ;
		void Serialize(CivArchive &archive) ;

		STDMETHODIMP_ (double) GetWorkdayCoef(sint32 gov_id) ;
        STDMETHODIMP_ (double) GetWorkdayBase(sint32 gov_id) ;
		STDMETHODIMP_ (double) GetWorkdayExpectation(sint32 gov_id) ;
		STDMETHODIMP_ (double) GetWorkdayUnit(sint32 gov_id) ;

		STDMETHODIMP_ (double) GetWagesCoef(sint32 gov_id) ;
        STDMETHODIMP_ (double) GetWagesBase(sint32 gov_id) ;
		STDMETHODIMP_ (double) GetWagesExpectation(THIS_ sint32 gov_id) ;
		STDMETHODIMP_ (double) GetWagesUnit(THIS_ sint32 gov_id) ;

		STDMETHODIMP_ (double) GetRationsCoef(sint32 gov_id) ;
        STDMETHODIMP_ (double) GetRationsBase(sint32 gov_id) ;
		STDMETHODIMP_ (double) GetRationsExpectation(sint32 gov_id) ;
	    STDMETHODIMP_ (double) GetRationsUnit(sint32 gov_id) ;

        STDMETHODIMP_ (double) GetMaxScienceRate(sint32 gov_id);
		STDMETHODIMP_ (double) GetKnowledgeCoef(sint32 gov_id) ;
		STDMETHODIMP_ (double) GetPollutionCoef(sint32 gov_id) ;
		STDMETHODIMP_ (double) GetPollutionUnhappyCoef(sint32 gov_id) ;

		STDMETHODIMP_ (double) GetEmpireDistanceScale(sint32 gov_id) ;
		STDMETHODIMP_ (double) GetMinEmpireDistance(sint32 gov_id) ;
		STDMETHODIMP_ (double) GetMaxEmpireDistance(sint32 gov_id) ;
        STDMETHODIMP_ (double) GetBigCityCoef();
        STDMETHODIMP_ (double) GetBigCityOffset();

		STDMETHODIMP_ (double) GetCrimeCoef(sint32 gov_id) ;
		STDMETHODIMP_ (double) GetCrimeOffset(sint32 gov_id) ;
   		STDMETHODIMP_ (sint32) EnablingDiscovery(sint32 gov_id) ;
		STDMETHODIMP_ (sint32) GetNumGovs(void) ;

		STDMETHODIMP_ (sint32) GetMaxMartialUnits(sint32 gov_id) ;
		STDMETHODIMP_ (double) GetMartialLawEffect(sint32 gov_id) ;
        STDMETHODIMP_ (sint32) GetMartialLawThreshold(sint32 gov_id);

		STDMETHODIMP_ (double) GetConqeustDistress(sint32 gov_id) ;
		STDMETHODIMP_ (double) GetAtHomeRadius(sint32 gov_id) ;
		STDMETHODIMP_ (double) GetOverseasCoef(sint32 gov_id) ;
		STDMETHODIMP_ (double) GetOverseasDefeatDecay(sint32 gov_id) ;
		STDMETHODIMP_ (double) GetOverseasDefeatCoef(sint32 gov_id) ;
		STDMETHODIMP_ (double) GetHomeDefeatDecay(sint32 gov_id) ;
		STDMETHODIMP_ (double) GetHomeDefeatCoef(sint32 gov_id) ;

		STDMETHODIMP_ (sint32) GetProfessionalUnits(sint32 gov_id) ;
		STDMETHODIMP_ (sint32) GetTurnsToNewReadiness(sint32 gov_id) ;
		STDMETHODIMP_ (double) GetReadyPeaceCoef(sint32 gov_id) ;
		STDMETHODIMP_ (double) GetReadyPeaceHP(sint32 gov_id) ;
		STDMETHODIMP_ (double) GetReadyAlertCoef(sint32 gov_id) ;
		STDMETHODIMP_ (double) GetReadyAlertHP(sint32 gov_id) ;
		STDMETHODIMP_ (double) GetReadyWarCoef(sint32 gov_id) ;
		STDMETHODIMP_ (double) GetReadyWarHP(sint32 gov_id) ;

        STDMETHODIMP_ (sint32) GetTooManyCitiesThreshold(sint32 gov_id);
        STDMETHODIMP_ (double) GetTooManyCitiesCoefficient(sint32 gov_id);

        STDMETHODIMP_ (double) GetGoldCoefficient(sint32 gov_id);
        STDMETHODIMP_ (double) GetProductionCoefficient(sint32 gov_id);
	} ;

#endif __C3Government_H__
