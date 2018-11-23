#include "c3.h"

#include "CityGrowth.h"
#include "FlatPtr.h"
#include "civarchive.h"

#include "dynarr.h"
#include "bset.h"

#include "ic3GameState.h"
#include "aimain.h"

#include "CityAgent.h"
#include "FzOut.h"

GrowTime::GrowTime()
{
    m_id = NULL;
    m_turns_total = 0.0;
    m_turns_remaining = 0.0;
}

GrowTime::GrowTime(CityAgent *ca, double tt, double tr)
{
    m_city_agent = ca;
    m_id = ca->GetID().GetVal();
    m_turns_total = tt;
    m_turns_remaining = tr;
}

GrowTime::GrowTime(BSet<CityAgent> *city_set, CivArchive &archive)
{
    Serialize(city_set, archive);
}

void GrowTime::Serialize(BSet<CityAgent> *city_set, CivArchive &archive)
{
    CHECKSERIALIZE

    if (archive.IsStoring()) {
        archive << m_id;
        archive << m_turns_total;
        archive << m_turns_remaining;
    } else {
        archive >> m_id;
        BSetID tmp = m_id;
        m_city_agent = city_set->Find(tmp);
        archive >> m_turns_total;
        archive >> m_turns_remaining;
    }
}

double GrowTime::GetVal()
{
    return m_turns_total;
}

CityGrowth::CityGrowth()
{
}

CityGrowth::CityGrowth(BSet<CityAgent> *city_set, CivArchive &archive)
{
    Serialize(city_set, archive);
}

CityGrowth::~CityGrowth()
{

	if(m_list) {
		const sint32 n = m_list->Num();
		for (sint32 i=0; i<n; i++) {
			delete m_list->Access(i);
		}
	}

}

void CityGrowth::Serialize(BSet<CityAgent> *city_set, CivArchive &archive)
{
    CHECKSERIALIZE

    CityList::Serialize(city_set, archive);

    sint32 i, n;
    GrowTime *gt=NULL;
    if (archive.IsStoring()) {

    	archive << m_list->ArraySize();
        n = m_list->Num();
        archive << n;
        for (i=0; i<n; i++) {
            gt = (GrowTime*)m_list->Access(i);
            gt->Serialize(city_set, archive);
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
            gt  = new GrowTime(city_set, archive);
            m_list->Access(i) = gt;
        }
    }

}

void CityGrowth::InsertTime(CityAgent *ca, double turns_total,
    double turns_remaining)

{
    GrowTime *tmp = new GrowTime(ca, turns_total, turns_remaining);
    m_list->InsertFlat(tmp);
}

void CityGrowth::EstimateGrowthTime(AiMain *ai)
{
    double turns_total;
    double turns_remaining;
    CityAgent *agent;
    BSetID id;

    Clear();
    double nf, at, ag;
    for (agent = ai->m_city_set->First(id); ai->m_city_set->Last(); agent = ai->m_city_set->Next(id)) {
        agent->GetFoodGrowth(nf, ag, at);
        if (0 < nf) {
            turns_total =  at ;
            turns_remaining = (at - ag);

            InsertTime(agent, turns_total, turns_remaining);
        } else {

            InsertTime(agent, k_BIG_TIME, k_BIG_TIME);
        }
    }
}

void CityGrowth::StopUnhappyGrowth(AiMain *ai)
{

    sint32 idx_city;
    sint32 m;
    GrowTime *gt=NULL;
    sint32 n = m_list->Num();

    for (idx_city=0; idx_city < n; idx_city++)  {
        m = m_list->Access(idx_city)->GetAgent()->GetNumMusicians();
        if (0 < m) {
            gt = (GrowTime *)(m_list->Access(idx_city));
            gt->m_turns_total *= m * 10;
            gt->m_turns_remaining *= m * 10;
        }
    }
}
