





#include "c3.h"  
#include "WonderTbl.h"
#include "FlatPtr.h"
#include "CivArchive.h"
#include "IC3GameState.h"
#include "IC3Wonder.h"
#include "IC3Science.h"



#include "AiMain.h"
#include "WonderBlg.h"
#include "CityAgent.h"


#include "GoalWonder.h"

ZEROMEM(WonderTableFlat); 
ZEROMEM(WonderTablePtr); 
ZERODEL(WonderTablePtr); 
FLATSERIALIZE(WonderTableFlat); 
PTRSERIALIZE(WonderTablePtr); 

WonderTable::WonderTable(AiMain *ai)
{ 
    m_num_wonders = ai->m_wonderDB->GetNumWonders(); 
    m_array = new WonderBlg[m_num_wonders];
    
    sint32 idx_wonder; 

	m_emancipation = FALSE;
	
	
	

    for (idx_wonder=0; idx_wonder<m_num_wonders; idx_wonder++) { 
        m_array[idx_wonder].Init(ai, idx_wonder); 

		
		if (!m_emancipation &&
			m_array[idx_wonder].GetGlobalState() == WONDER_STATE_DONE &&
			GetEffects(idx_wonder)[WONDER_EFFECT_FREE_SLAVES])
			m_emancipation = TRUE;

		
		









    }

    sint32 num_sci = ai->m_science->GetNumAdvances();
    sint32 idx_sci;
    for (idx_sci=0; idx_sci<num_sci; idx_sci++) {     
        if (ai->m_science->HasAdvance(idx_sci)) { 
           for (idx_wonder=0; idx_wonder<m_num_wonders; idx_wonder++) { 
               m_array[idx_wonder].RegisterNewScience(idx_sci); 
           }
        } 
    } 

}


WonderTable::WonderTable(CivArchive &archive)
{ 
    Serialize(archive); 

	
	m_emancipation = FALSE;
	
	

















} 

WonderTable::~WonderTable()
{ 
    Assert(m_array); 
    delete[] m_array;
    m_array = NULL; 
} 

void WonderTablePtr::Store (CivArchive &archive)
{

    archive << m_num_wonders; 

    sint32 idx_wonder;
    for (idx_wonder=0; idx_wonder<m_num_wonders; idx_wonder++) { 
        m_array[idx_wonder].Serialize(archive); 
    } 
    archive << (sint32) m_emancipation;
}

void WonderTablePtr::Load(CivArchive &archive, sint32 pcount, uint8 nmask[])
{
	sint32 a_bool;
    archive >> m_num_wonders; 

	sint32 idx_wonder;
    m_array = new WonderBlg[m_num_wonders]; 
    for (idx_wonder=0; idx_wonder<m_num_wonders; idx_wonder++) { 
        m_array[idx_wonder].Serialize(archive); 
    } 
    archive >> a_bool;
	m_emancipation = (BOOL) a_bool;
}

void WonderTable::Serialize(CivArchive &archive)
{
    CHECKSERIALIZE 

    WonderTableFlat::Serialize(archive); 
    WonderTablePtr::Serialize(archive); 
}


void WonderTable::RegisterNewScience(const sint32  idx_sci)
{ 
    sint32 idx_wonder; 

    for (idx_wonder=0; idx_wonder<m_num_wonders; idx_wonder++) { 
        m_array[idx_wonder].RegisterNewScience(idx_sci); 
    } 
} 

double * WonderTable::GetEffects(const sint32 idx_wonder)
{
    Assert(0 <= idx_wonder); 
    Assert(idx_wonder < m_num_wonders); 

    return m_array[idx_wonder].GetEffects(); 
}





















bool WonderTable::CanInsertWonderGoalForCity
(
	AiMain *ai,
	CityAgent *the_city,
	sint32 which_wonder
)
{
    GoalWonder *goal_obj=NULL; 

    if (ai->m_wonderDB->HasBeenBuilt(which_wonder)) { 
        return FALSE; 
    } 

    if (0.3 < ai->m_wonderDB->MaxPecentageThisWonderDone(which_wonder)) { 
        if (!the_city->HasLotOfStoredProduction()) { 
            return FALSE; 
        }
    } 

    return (m_array[which_wonder].CanBuild(ai) && 
        (m_array[which_wonder].GetMyState() == WONDER_STATE_UNBUILT));
}

void WonderTable::SetWonderState(sint32 which_wonder, WONDER_STATE state)
{
	m_array[which_wonder].SetMyState(state); 
}































double WonderTable::GetProductionCost(const sint32 idx_wonder) const
{
    
    Assert(0 <= idx_wonder); 
    Assert(idx_wonder < m_num_wonders); 
    
    return m_array[idx_wonder].GetProductionCost(); 
}

BOOL WonderTable::CanBuild(AiMain *ai, const sint32 idx_wonder) const
{
    Assert(0 <= idx_wonder); 
    Assert(idx_wonder < m_num_wonders); 
    
    return m_array[idx_wonder].CanBuild(ai); 

}

BOOL WonderTable::RegisterCreateWonder(uint32 city_id, const sint32 idx_wonder, 
    const PLAYER_INDEX  my_idx, const PLAYER_INDEX owner)
{
    Assert(0 <= idx_wonder); 
    Assert(idx_wonder < m_num_wonders); 

	
	if (GetEffects(idx_wonder)[WONDER_EFFECT_FREE_SLAVES])
		m_emancipation = TRUE;

    return m_array[idx_wonder].RegisterCreateWonder(city_id, my_idx, owner); 
}

void WonderTable::SetIamBuilding(const sint32 idx_wonder)
{
    Assert(0 <= idx_wonder); 
    Assert(idx_wonder < m_num_wonders); 
    
    m_array[idx_wonder].SetIamBuilding(); 
}


sint32 WonderTable::GetNumWonders() const
{
    return m_num_wonders;
}

void WonderTable::RegisterCapturedWonder(const sint32 idx_wonder)
{
    Assert(0 <= idx_wonder); 
    Assert(idx_wonder < m_num_wonders); 
    
    m_array[idx_wonder].RegisterCapturedWonder(); 
}

WONDER_STATE WonderTable::GetMyState(const sint32 idx_wonder) const
{
    Assert(0 <= idx_wonder); 
    Assert(idx_wonder < m_num_wonders); 
    
    return m_array[idx_wonder].GetMyState(); 
}


WONDER_STATE WonderTable::GetGlobalState(const sint32 idx_wonder) const
{
    Assert(0 <= idx_wonder); 
    Assert(idx_wonder < m_num_wonders); 
    
    return m_array[idx_wonder].GetGlobalState(); 
}

BOOL WonderTable::IOwnIt(const sint32 idx_wonder) const  
{
    Assert(0 <= idx_wonder); 
    Assert(idx_wonder < m_num_wonders); 
    
    return m_array[idx_wonder].IOwnIt(); 
}

BSetID *WonderTable::GetHomeCity(const sint32 idx_wonder) 
{ 
	return m_array[idx_wonder].GetHomeCity(); 
}









