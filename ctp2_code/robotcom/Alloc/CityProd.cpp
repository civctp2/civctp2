

#include "c3.h"
  
#include "CityProd.h"
#include "FlatPtr.h"
#include "civarchive.h"

#include "dynarr.h"
#include "bset.h"

#include "ic3GameState.h"
#include "aimain.h"

#include "CityAgent.h"
#include "FzOut.h"

ProdVal::ProdVal()
{
    m_id = NULL; 
}

ProdVal::ProdVal(CityAgent *ca, const double p)
{ 
    m_city_agent = ca; 
    m_id = ca->GetID().GetVal(); 
    m_net_production = p ; 
} 

ProdVal::ProdVal(BSet<CityAgent> *city_set, CivArchive &archive)
{
    Serialize(city_set, archive);
}
   
void ProdVal::Serialize(BSet<CityAgent> *city_set, CivArchive &archive)
{
    CHECKSERIALIZE

    if (archive.IsStoring()) { 
        archive << m_id; 
        archive << m_net_production;
    } else { 
        archive >> m_id; 
        BSetID tmp = m_id; 
        m_city_agent = city_set->Find(tmp); 
        archive >> m_net_production;
    } 
}

double ProdVal::GetVal()
{
    return m_net_production;
}


CityProduction::CityProduction()
{
}

CityProduction::CityProduction(BSet<CityAgent> *city_set, CivArchive &archive)
{
    Serialize(city_set, archive); 
}

CityProduction::~CityProduction()
{
	
	if (m_list) {
		const sint32 n = m_list->Num();
		for (sint32 i=0; i<n; i++) {
			delete m_list->Access(i);
		}
	}
}

void CityProduction::Serialize(BSet<CityAgent> *city_set, CivArchive &archive)
{
    CHECKSERIALIZE

    CityList::Serialize(city_set, archive); 

    sint32 i, n; 
    ProdVal *pv=NULL; 
    if (archive.IsStoring()) { 

    	archive << m_list->ArraySize(); 
        n = m_list->Num(); 
        archive << n; 
        for (i=0; i<n; i++) { 
            pv = (ProdVal*)m_list->Access(i); 
            pv->Serialize(city_set, archive); 
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
            pv  = new ProdVal(city_set, archive); 
            m_list->Access(i) = pv; 
        } 
    }
}

void CityProduction::InsertProduction(CityAgent *ca, const double p)
{ 
    ProdVal *tmp = new ProdVal(ca, p); 
    m_list->InsertFlat(tmp); 
} 

void CityProduction::EstimateProduction(AiMain *ai)
{
    CityAgent *agent; 
    BSetID id; 
    
    Clear(); 
    double p;
    for (agent = ai->m_city_set->First(id); ai->m_city_set->Last(); agent = ai->m_city_set->Next(id)) {         
        p = agent->GetNetProduction(); 
        InsertProduction(agent, p); 
    }
}

