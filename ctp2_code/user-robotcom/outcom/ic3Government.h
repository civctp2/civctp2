













#pragma once

#ifndef __IC3Government_H__
#define __IC3Government_H__ 1

#include <OBJBASE.H>  

DEFINE_GUID(CLSID_IC3Government, 0x20335c21, 0x5fc, 0x11d2, 0x83, 0x55, 0x0, 0xc0, 0x4f, 0xba, 0x43, 0xc0) ;

#undef INTERFACE
#define INTERFACE IC3Government

DECLARE_INTERFACE_(IC3Government, IUnknown)
	{
 	

	STDMETHOD_ (double, GetWorkdayCoef)(THIS_ sint32 ) PURE ;
	STDMETHOD_ (double, GetWorkdayBase)(THIS_ sint32 gov_id) PURE ;
	STDMETHOD_ (double, GetWorkdayExpectation)(THIS_ sint32 gov_id) PURE ;
	STDMETHOD_ (double, GetWorkdayUnit)(THIS_ sint32 gov_id) PURE ;
 
	STDMETHOD_ (double, GetWagesCoef)(THIS_ sint32 gov_id) PURE ;
	STDMETHOD_ (double, GetWagesBase)(THIS_ sint32 gov_id) PURE ;
	STDMETHOD_ (double, GetWagesExpectation)(THIS_ sint32 gov_id) PURE ;
	STDMETHOD_ (double, GetWagesUnit)(THIS_ sint32 gov_id) PURE ;

	STDMETHOD_ (double, GetRationsCoef)(THIS_ sint32 gov_id) PURE ;
    STDMETHOD_ (double, GetRationsBase)(THIS_ sint32 gov_id) PURE ;
	STDMETHOD_ (double, GetRationsExpectation)(THIS_ sint32 gov_id) PURE ;
	STDMETHOD_ (double, GetRationsUnit)(THIS_ sint32 gov_id) PURE ;

    STDMETHOD_ (double, GetMaxScienceRate)(THIS_ sint32 gov_id) PURE;
	STDMETHOD_ (double, GetKnowledgeCoef)(THIS_ sint32 gov_id) PURE ;
	STDMETHOD_ (double, GetPollutionCoef)(THIS_ sint32 gov_id) PURE ;
	STDMETHOD_ (double, GetPollutionUnhappyCoef)(THIS_ sint32 gov_id) PURE ;
	STDMETHOD_ (double, GetEmpireDistanceScale)(THIS_ sint32 gov_id) PURE ;

    STDMETHOD_ (double, GetMinEmpireDistance)(THIS_ sint32 gov_id) PURE ;
	STDMETHOD_ (double, GetMaxEmpireDistance)(THIS_ sint32 gov_id) PURE ;
    STDMETHOD_ (double, GetBigCityCoef) (THIS_) PURE;
    STDMETHOD_ (double, GetBigCityOffset)(THIS_) PURE; 

	STDMETHOD_ (double, GetCrimeCoef)(THIS_ sint32 gov_id) PURE ;
	STDMETHOD_ (double, GetCrimeOffset)(THIS_ sint32 gov_id) PURE ;
   	STDMETHOD_ (sint32, EnablingDiscovery)(THIS_ sint32 gov_id) PURE ;
	STDMETHOD_ (sint32, GetNumGovs)(THIS_) PURE ;

	STDMETHOD_ (sint32, GetMaxMartialUnits)(THIS_ sint32 gov_id) PURE ;
	STDMETHOD_ (double, GetMartialLawEffect)(THIS_ sint32 gov_id) PURE ;
    STDMETHOD_ (sint32, GetMartialLawThreshold) (THIS_ sint32 gov_id) PURE;

	STDMETHOD_ (double, GetConqeustDistress)(THIS_ sint32 gov_id) PURE ;
	STDMETHOD_ (double, GetAtHomeRadius)(THIS_ sint32 gov_id) PURE ;
	STDMETHOD_ (double, GetOverseasCoef)(THIS_ sint32 gov_id) PURE ;
	STDMETHOD_ (double, GetOverseasDefeatDecay)(THIS_ sint32 gov_id) PURE ;
	STDMETHOD_ (double, GetOverseasDefeatCoef)(THIS_ sint32 gov_id) PURE ;
	STDMETHOD_ (double, GetHomeDefeatDecay)(THIS_ sint32 gov_id) PURE ;
	STDMETHOD_ (double, GetHomeDefeatCoef)(THIS_ sint32 gov_id) PURE ;
	STDMETHOD_ (sint32, GetProfessionalUnits)(THIS_ sint32 gov_id) PURE ;
	STDMETHOD_ (sint32, GetTurnsToNewReadiness)(THIS_ sint32 gov_id) PURE ;
	STDMETHOD_ (double, GetReadyPeaceCoef)(THIS_ sint32 gov_id) PURE ;
	STDMETHOD_ (double, GetReadyPeaceHP)(THIS_ sint32 gov_id) PURE ;
	STDMETHOD_ (double, GetReadyAlertCoef)(THIS_ sint32 gov_id) PURE ;
	STDMETHOD_ (double, GetReadyAlertHP)(THIS_ sint32 gov_id) PURE ;
	STDMETHOD_ (double, GetReadyWarCoef)(THIS_ sint32 gov_id) PURE ;
	STDMETHOD_ (double, GetReadyWarHP)(THIS_ sint32 gov_id) PURE ;

    STDMETHOD_ (sint32, GetTooManyCitiesThreshold) (THIS_ sint32 gov_id) PURE;
    STDMETHOD_ (double, GetTooManyCitiesCoefficient) (THIS_ sint32 gov_id) PURE;

    STDMETHOD_ (double, GetGoldCoefficient)(THIS_ sint32 gov_id) PURE;
    STDMETHOD_ (double, GetProductionCoefficient)(THIS_ sint32 gov_id) PURE;  

} ;

#endif __IC3Government_H__
