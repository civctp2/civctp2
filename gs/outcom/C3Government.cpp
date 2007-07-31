
#include "c3.h"


#include "C3Government.h"


#include "Player.h"
#include "XY_Coordinates.h"
#include "World.h"
#include "CivArchive.h"
#include "ConstDB.h"
#include "DiffDB.h"
#include "GovernmentRecord.h"

extern Player	**g_player ;
extern ConstDB *g_theConstDB; 
extern DifficultyDB *g_theDifficultyDB; 








STDMETHODIMP C3Government::QueryInterface(REFIID riid, void **obj)
	{
	*obj = NULL ;

	if(IsEqualIID(riid, IID_IUnknown))
		{
		*obj = (IUnknown *)this ;
		AddRef() ;

		return (S_OK) ;
		}
	else if(IsEqualIID(riid, CLSID_IC3Government))
		{
		*obj = (IC3Government*)this ;
		AddRef() ;
		return (S_OK) ;
		}

	return (E_NOINTERFACE) ;
	}









STDMETHODIMP_(ULONG) C3Government::AddRef()
	{
	return (++m_refCount) ;
	}








STDMETHODIMP_(ULONG) C3Government::Release()
	{
	if(--m_refCount)
		return m_refCount ;

	delete this ;

	return (0) ;
	}









C3Government::C3Government(sint32 idx)
	{
    m_refCount = 0 ;
    Assert(idx >= 0) ;
    Assert(idx < k_MAX_PLAYERS) ;

    m_owner = idx ;
    Assert(g_player[idx]) ;
    m_player = g_player[idx] ;
    Assert(m_player) ;
	}








C3Government::C3Government(CivArchive &archive)
	{
    Serialize(archive) ;
	}









void C3Government::Serialize(CivArchive &archive) 
	{

    CHECKSERIALIZE

    if (archive.IsStoring())
		{
        archive << m_refCount ;
		archive << m_owner ;
		}
	else
		{
        archive >> m_refCount ;
		archive >> m_owner ;
		m_player = g_player[m_owner] ;
		}

	}









double C3Government::GetWorkdayCoef(sint32 gov_id)
	{
    return (g_theGovernmentDB->Get(gov_id)->GetWorkdayCoef()) ; 
	}









double C3Government::GetWorkdayBase(sint32 gov_id)
{ 
    return g_theConstDB->GetBaseWorkday();
}

double C3Government::GetWorkdayUnit(sint32 gov_id)
{ 
    return g_theConstDB->GetUnitWorkday();
}

double C3Government::GetWorkdayExpectation(sint32 gov_id)
{
    return (g_theGovernmentDB->Get(gov_id)->GetWorkdayExpectation()) ; 
}









double C3Government::GetWagesCoef(sint32 gov_id)
	{
    return (g_theGovernmentDB->Get(gov_id)->GetWagesCoef()) ; 
	}


double C3Government::GetWagesBase(sint32 gov_id)
{
    return g_theConstDB->GetBaseWages();
}

double C3Government::GetWagesUnit(sint32 gov_id)
{
    return g_theConstDB->GetUnitWages();
}








double C3Government::GetWagesExpectation(sint32 gov_id)
	{
    return (g_theGovernmentDB->Get(gov_id)->GetWagesExpectation()) ; 
	}









double C3Government::GetRationsCoef(sint32 gov_id)
	{
    return (g_theGovernmentDB->Get(gov_id)->GetRationsCoef()) ; 
	}









double C3Government::GetRationsExpectation(sint32 gov_id)
	{
    return (g_theGovernmentDB->Get(gov_id)->GetRationsExpectation()) ; 
	}


double C3Government::GetRationsBase(sint32 gov_id)
{ 
    return g_theConstDB->GetBaseRations();
}


double C3Government::GetRationsUnit(sint32 gov_id)
{ 
    return g_theConstDB->GetUnitRations();
}








double C3Government::GetMaxScienceRate(sint32 gov_id)
{ 
    return g_theGovernmentDB->Get(gov_id)->GetMaxScienceRate();
}

double C3Government::GetKnowledgeCoef(sint32 gov_id)
{
return (g_theGovernmentDB->Get(gov_id)->GetKnowledgeCoef()) ; 
}









double C3Government::GetPollutionCoef(sint32 gov_id)
	{
    return (g_theGovernmentDB->Get(gov_id)->GetPollutionCoef()) ; 
	}









double C3Government::GetPollutionUnhappyCoef(sint32 gov_id)
	{
    return (g_theGovernmentDB->Get(gov_id)->GetPollutionUnhappyCoef()) ; 
	}









double C3Government::GetEmpireDistanceScale(sint32 gov_id)
	{
    return (g_theGovernmentDB->Get(gov_id)->GetEmpireDistanceScale()) ; 
	}









double C3Government::GetMinEmpireDistance(sint32 gov_id)
{
    return (g_theGovernmentDB->Get(gov_id)->GetMinEmpireDistance()) ; 
}

double C3Government::GetMaxEmpireDistance(sint32 gov_id)
	{
    return (g_theGovernmentDB->Get(gov_id)->GetMaxEmpireDistance()) ; 
	}









double C3Government::GetCrimeCoef(sint32 gov_id)
	{
    return (g_theGovernmentDB->Get(gov_id)->GetCrimeCoef()) ; 
	}









double C3Government::GetCrimeOffset(sint32 gov_id)
	{
    return (g_theGovernmentDB->Get(gov_id)->GetCrimeOffset()) ; 
	}









sint32 C3Government::EnablingDiscovery(sint32 gov_id)
	{
	return (g_theGovernmentDB->Get(gov_id)->GetEnableAdvanceIndex()) ;
	}









sint32 C3Government::GetNumGovs(void)
	{
	return (g_theGovernmentDB->NumRecords()) ;
	}









sint32 C3Government::GetMaxMartialUnits(sint32 gov_id)
	{
	return (g_theGovernmentDB->Get(gov_id)->GetMaxMartialLawUnits()) ;
	}


sint32 C3Government::GetMartialLawThreshold(sint32 gov_id)
{
    return g_theGovernmentDB->Get(gov_id)->GetMartialLawThreshold(); 
}










double C3Government::GetMartialLawEffect(sint32 gov_id)
	{
    return (g_theGovernmentDB->Get(gov_id)->GetMartialLawEffect()) ;
	}









double C3Government::GetConqeustDistress(sint32 gov_id)
    {
	return (g_theGovernmentDB->Get(gov_id)->GetConquestDistress()) ;
	}









double C3Government::GetAtHomeRadius(sint32 gov_id)
	{
	return (g_theGovernmentDB->Get(gov_id)->GetAtHomeRadius()) ;
	}









double C3Government::GetOverseasCoef(sint32 gov_id)
	{
	return (g_theGovernmentDB->Get(gov_id)->GetOverseasCoef()) ;
	}









double C3Government::GetOverseasDefeatDecay(sint32 gov_id)
	{
	return (g_theGovernmentDB->Get(gov_id)->GetOverseasDefeatDecay()) ;
	}









double C3Government::GetOverseasDefeatCoef(sint32 gov_id)
	{
	return (g_theGovernmentDB->Get(gov_id)->GetOverseasDefeatCoef()) ;
	}









double C3Government::GetHomeDefeatDecay(sint32 gov_id)
	{
	return (g_theGovernmentDB->Get(gov_id)->GetHomeDefeatDecay()) ;
	}









double C3Government::GetHomeDefeatCoef(sint32 gov_id)
	{
	return (g_theGovernmentDB->Get(gov_id)->GetHomeDefeatCoef()) ;
	}

	







sint32 C3Government::GetProfessionalUnits(sint32 gov_id)
	{
	return (g_theGovernmentDB->Get(gov_id)->GetProfessionalUnits()) ;
	}









sint32 C3Government::GetTurnsToNewReadiness(sint32 gov_id)
	{
	return (g_theGovernmentDB->Get(gov_id)->GetTurnsToNewReadiness()) ;
	}









double C3Government::GetReadyPeaceCoef(sint32 gov_id)
	{
	return (g_theGovernmentDB->Get(gov_id)->GetReadyPeaceCoef()) ;
	}









double C3Government::GetReadyPeaceHP(sint32 gov_id)
	{
	return (g_theGovernmentDB->Get(gov_id)->GetReadyPeaceHP()) ;
	}









double C3Government::GetReadyAlertCoef(sint32 gov_id)
	{
	return (g_theGovernmentDB->Get(gov_id)->GetReadyAlertCoef()) ;
	}









double C3Government::GetReadyAlertHP(sint32 gov_id)
	{
	return (g_theGovernmentDB->Get(gov_id)->GetReadyAlertHP()) ;
	}









double C3Government::GetReadyWarCoef(sint32 gov_id)
	{
	return (g_theGovernmentDB->Get(gov_id)->GetReadyWarCoef()) ;
	}









double C3Government::GetReadyWarHP(sint32 gov_id)
	{
	return (g_theGovernmentDB->Get(gov_id)->GetReadyWarHP()) ;
	}



double C3Government::GetBigCityCoef() 
{ 
    return g_theDifficultyDB->GetBigCityScale(0); 
}

double C3Government::GetBigCityOffset() 
{
    return g_theDifficultyDB->GetBigCityOffset(0); 
}

sint32 C3Government::GetTooManyCitiesThreshold(sint32 gov_id)
{ 
    return (g_theGovernmentDB->Get(gov_id)->GetTooManyCitiesThreshold()) ; 
}

double C3Government::GetTooManyCitiesCoefficient(sint32 gov_id) 
{ 
    return (g_theGovernmentDB->Get(gov_id)->GetTooManyCitiesCoefficient()) ; 
} 

double C3Government::GetGoldCoefficient(sint32 gov_id) 
{ 
   return (g_theGovernmentDB->Get(gov_id)->GetGoldCoef()) ; 
}

double C3Government::GetProductionCoefficient(sint32 gov_id) 
{
    return (g_theGovernmentDB->Get(gov_id)->GetProductionCoef()) ; 
}

