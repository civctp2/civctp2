

#include "c3.h"
  
#include "CityFP.h"
#include "FlatPtr.h"
#include "CivArchive.h"

#include "DynArr.h"
#include "BSet.h"

#include "IC3GameState.h"
#include "AiMain.h"

#include "CityAgent.h"
#include "FzOut.h"

FPVal::FPVal()
{ 
    m_id = NULL; 
}

FPVal::FPVal(CityAgent *ca, const double p)
{ 
    m_city_agent = ca; 
    m_id = ca->GetID().GetVal(); 
    m_fp_ratio = p ; 
} 
   
FPVal::FPVal(BSet<CityAgent> *city_set, CivArchive &archive)
{
    Serialize(city_set, archive); 
}

void FPVal::Serialize(BSet<CityAgent> *city_set, CivArchive &archive)
{
    CHECKSERIALIZE

    if (archive.IsStoring()) { 
        archive << m_id; 
        archive << m_fp_ratio;
    } else { 
        archive >> m_id; 
        BSetID tmp = m_id; 
        m_city_agent = city_set->Find(tmp); 
        archive >> m_fp_ratio;
    } 
}

double FPVal::GetVal()
{
    return m_fp_ratio;
}


CityFP::CityFP()
{
}

CityFP::CityFP(BSet<CityAgent> *city_set, CivArchive &archive)
{
    Serialize(city_set, archive); 
}

CityFP::~CityFP()
{
	
	if (m_list) {
		const sint32 n = m_list->Num();
		for (sint32 i=0; i<n; i++) {
			delete m_list->Access(i);
		}
	}
}

void CityFP::Serialize(BSet<CityAgent> *city_set, CivArchive &archive)
{
    CHECKSERIALIZE

    CityList::Serialize(city_set, archive); 

    
    sint32 i, n; 
    FPVal *fp=NULL; 
    if (archive.IsStoring()) { 

    	archive << m_list->ArraySize(); 
        n = m_list->Num(); 
        archive << n; 
        for (i=0; i<n; i++) { 
            fp = (FPVal*)m_list->Access(i); 
            fp->Serialize(city_set, archive); 
        } 
    } else { 
	    sint32 arr_size; 

        archive >> arr_size; 
	    Assert(0 < arr_size); 

        if (m_list) { 
            delete m_list;
            m_list = NULL; 
        } 

        m_list = new DynamicArray<CityVal *>(arr_size); 

	    archive >> n; 
        for (i=0; i<n; i++) { 
            m_list->ExtendByOneFlat(); 
            fp  = new FPVal(city_set, archive); 
            m_list->Access(i) = fp; 
        } 
    }
}

void CityFP::InsertFP(CityAgent *ca, const double p)
{ 
    FPVal *tmp = new FPVal(ca, p); 
    m_list->InsertFlat(tmp); 
} 

void CityFP::EstimateFP(AiMain *ai)
{
    CityAgent *agent; 
    BSetID id; 
    
    Clear(); 
    double p;
    double f;
    for (agent = ai->m_city_set->First(id); ai->m_city_set->Last(); agent = ai->m_city_set->Next(id)) {         
        p = agent->GetNetProduction(); 
        f = agent->GetNetFood(); 
        if (p < 1.0) { 
            InsertFP(agent, f/p);
        } else { 
            InsertFP(agent, 1000.0);
        }
    }
}

