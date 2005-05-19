

#include "c3.h"  
#include "globals.h"
#include "ic3Wonder.h"
#include "WonderBlg.h"
#include "FlatPtr.h"
#include "civarchive.h"

#include "ic3GameState.h"
#include "aimain.h"
#include "FzOut.h"
#include "bset.h"

ZEROMEM(WonderBlgFlat); 
ZEROMEM(WonderBlgPtr); 
ZERODEL(WonderBlgPtr); 
FLATSERIALIZE(WonderBlgFlat); 

WonderBlg::WonderBlg()
{ 
    
    m_global_state = WONDER_STATE_CANT_BUILD; 
    m_global_percent_done = 0.0; 
    m_my_state = WONDER_STATE_CANT_BUILD; 
    m_my_pecent_done = 0.0;
    
    m_effect = new double [WONDER_EFFECT_MAX]; 

    m_i_own_it = FALSE; 
    m_home_city = NULL; 
}

void WonderBlg::Init(AiMain *ai, const sint32 iw)
{
    m_idx_wonder = iw; 

    m_production_cost = ai->m_wonderDB->GetProductionCost(m_idx_wonder); 

    m_enabling = ai->m_wonderDB->EnablingDiscovery(m_idx_wonder);
    m_obsolete = ai->m_wonderDB->ObsoleteDiscovery(m_idx_wonder);

	if (ai->m_wonderDB->HasBeenBuilt(m_idx_wonder))
		{
			PLAYER_INDEX owner = 
				ai->m_wonderDB->GetOwner(m_idx_wonder);

			
			RegisterCreateWonder(0x0, m_idx_wonder, owner);
		}

    ai->m_wonderDB->GetAllEffects(m_idx_wonder, m_effect); 
}

WonderBlg::WonderBlg(CivArchive &archive)
{ 
	m_effect = NULL;

    Serialize(archive); 
} 

WonderBlg::~WonderBlg()
{ 
    delete [] m_effect; 
    m_effect = NULL; 
} 

void WonderBlgPtr::Store (CivArchive &archive)
{
    Assert(m_effect); 

    archive.Store((uint8*) m_effect, sizeof(double) * WONDER_EFFECT_MAX); 
}

void WonderBlgPtr::Load(CivArchive &archive, sint32 pcount, uint8 nmask[])
{
	if (!m_effect)
	    m_effect = new double[WONDER_EFFECT_MAX]; 
    archive.Load((uint8*) m_effect, sizeof(double) * WONDER_EFFECT_MAX); 
}

void WonderBlgPtr::Serialize(CivArchive &archive)
{ 
    CHECKSERIALIZE

    if (archive.IsStoring()) { 
        Store(archive);
    } else { 
        sint32 pcount=1; 
        uint8 nmask[1]; 
        Load(archive, pcount, nmask);
    }
}

void WonderBlg::Serialize(CivArchive &archive)
{
    CHECKSERIALIZE 

    WonderBlgFlat::Serialize(archive); 
    WonderBlgPtr::Serialize(archive); 
}

void WonderBlg::SetMyState(const WONDER_STATE s)
{
    m_my_state = s; 
} 




void WonderBlg::RegisterNewScience(const sint32  idx_sci)
{
    if (idx_sci == m_obsolete) { 
        m_global_state = WONDER_STATE_OBSOLETE; 
        m_my_state = WONDER_STATE_OBSOLETE; 
    } else if ((m_global_state != WONDER_STATE_OBSOLETE) &&
        (idx_sci == m_enabling)) { 
        
       if (m_my_state == WONDER_STATE_CANT_BUILD) 
             m_my_state = WONDER_STATE_UNBUILT; 

    } 
}


BOOL WonderBlg::CanBuild(AiMain *ai) 
{ 

    BOOL ai_state = TRUE; 

    switch (m_global_state) { 
    case WONDER_STATE_DONE:
    case WONDER_STATE_OBSOLETE:
        ai_state = FALSE; 
    } 

    switch (m_my_state) { 
    case WONDER_STATE_CANT_BUILD:
    case WONDER_STATE_BUILDING: 
    case WONDER_STATE_DONE:
    case WONDER_STATE_OBSOLETE:
        ai_state = FALSE; 
    } 

	
	if (ai_state == TRUE && 
		ai->m_wonderDB->IsObsolete(m_idx_wonder))
	{
		m_global_state = WONDER_STATE_OBSOLETE;
		m_my_state = WONDER_STATE_OBSOLETE;
		ai_state = FALSE;
	}

	#ifdef _DEBUG
    BOOL game_state = ai->m_wonderDB->CanBuild(m_idx_wonder); 
    if (m_my_state != WONDER_STATE_BUILDING) { 
        Assert(ai_state == game_state);
		
		
		
		if (ai_state == game_state)
	    game_state = ai->m_wonderDB->CanBuild(m_idx_wonder); 
    }
	#endif

    return ai_state; 
}

double* WonderBlg::GetEffects()
{ 
    return m_effect; 
}

double WonderBlg::GetProductionCost()
{
    return m_production_cost; 
}

BOOL WonderBlg::RegisterCreateWonder(const uint32 city_id,                                 
    const PLAYER_INDEX  my_idx, const PLAYER_INDEX owner)
{

    BOOL is_building = (m_my_state == WONDER_STATE_BUILDING); 

    
    m_global_state = WONDER_STATE_DONE; 
    if (my_idx == owner) { 
        m_my_state = WONDER_STATE_DONE; 

        m_i_own_it = TRUE; 
    } else { 
        m_my_state = WONDER_STATE_CANT_BUILD; 
        m_i_own_it = FALSE; 
    } 
    
    m_wonder_owner = owner;
    
    m_home_city = new BSetID; 
    *m_home_city = city_id;
 
    return is_building;
}


void WonderBlg::SetIamBuilding()
{
    m_my_state = WONDER_STATE_BUILDING;
}


void WonderBlg::RegisterCapturedWonder()
{

    if ((m_global_state != WONDER_STATE_DONE) &&
        (m_global_state != WONDER_STATE_OBSOLETE)) { 
        m_global_state = WONDER_STATE_DONE; 
    }

    if ((m_my_state != WONDER_STATE_DONE) &&
        (m_my_state != WONDER_STATE_OBSOLETE)) { 
        m_my_state = WONDER_STATE_DONE; 
    }

    m_i_own_it = TRUE; 
}

PLAYER_INDEX WonderBlg::GetOwner()
{
	return m_wonder_owner;
}
