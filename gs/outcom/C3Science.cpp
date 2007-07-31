
#include "c3.h"
#include "Player.h"


#include "C3Science.h"


#include "AdvanceRecord.h"
#include "CivArchive.h"
#include "Advances.h"

#include "StrDB.h"

extern StringDB *g_theStringDB;


	extern	Player	**g_player ;


STDMETHODIMP C3Science::QueryInterface(REFIID riid, void **obj)
	{
	*obj = NULL ;

	if(IsEqualIID(riid, IID_IUnknown))
		{
		*obj = (IUnknown *)this ;
		AddRef() ;

		return (S_OK) ;
		}
	else if(IsEqualIID(riid, CLSID_IC3Science))
		{
		*obj = (IC3Science*)this ;
		AddRef() ;
		return (S_OK) ;
		}

	return (E_NOINTERFACE) ;
	}


STDMETHODIMP_(ULONG) C3Science::AddRef()
	{
	return (++m_refCount) ;
	}

STDMETHODIMP_(ULONG) C3Science::Release()
	{
	if(--m_refCount)
		return m_refCount ;

	delete this ;

	return (0) ;
	}


C3Science::C3Science(sint32 idx)
	{
    m_refCount = 0 ;
    Assert(idx >= 0) ;
    Assert(idx < k_MAX_PLAYERS) ;

    m_owner = idx ;
    Assert(g_player[idx]) ;
    m_player = g_player[idx] ;
    Assert(m_player) ;
	}

C3Science::C3Science(CivArchive &archive)
	{
    Serialize(archive) ;
	}

void C3Science::Serialize(CivArchive &archive) 
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


sint32 C3Science::GetNumAdvances(void)
	{
	return (g_theAdvanceDB->NumRecords()) ;
	}


sint32 C3Science::GetNumDependencies(sint32 adv)
	{
	Assert(adv >=0 && adv <= g_theAdvanceDB->NumRecords()) ;
	
	return (g_theAdvanceDB->Get(adv)->GetNumDependencies()) ;
	}

sint32 C3Science::GetNumPrerequisites(sint32 adv)
	{
	Assert(adv >=0 && adv <= g_theAdvanceDB->NumRecords()) ;
	
	return (g_theAdvanceDB->Get(adv)->GetNumPrerequisites()) ;
	}

sint32 C3Science::GetLeadsTo(sint32 adv, sint32 lead)
	{
	Assert(adv >=0 && adv <= g_theAdvanceDB->NumRecords()) ;
	Assert(lead >=0 && lead <= g_theAdvanceDB->Get(adv)->GetNumDependencies()) ;
	
	return 0; 
	}

sint32 C3Science::GetPrereq(sint32 adv, sint32 req)
	{
	Assert(adv >=0 && adv <= g_theAdvanceDB->NumRecords()) ;
	Assert(req >=0 && req <= g_theAdvanceDB->Get(adv)->GetNumPrerequisites()) ;

	return (g_theAdvanceDB->Get(adv)->GetPrerequisitesIndex(req)) ;
	}


void C3Science::SetResearching(sint32 adv)
{
	m_player->SetResearching(adv) ;
}









BOOL C3Science::HasAdvance(sint32 adv)
{
	return (m_player->HasAdvance(adv)) ;
}


BOOL C3Science::IsCurrentlyReserching()

{ 
    return FALSE; 
} 

sint32 C3Science::GetCurrentScienceCost ()
{
    return m_player->GetCurrentScienceCost();    
}

sint32 C3Science::GetCurrentScienceLevel()
{
    return GetCurrentScienceLevel();
}

sint32 C3Science::GetCanAskFor(PLAYER_INDEX other_player, uint8 *askable)
{
	uint8 *temp;
	sint32 num;

	temp = m_player->m_advances->CanAskFor(g_player[other_player]->m_advances, num);
	memcpy(askable, temp, g_theAdvanceDB->NumRecords() * sizeof(uint8));
	delete [] temp;
	return num;
}

sint32 C3Science::GetCanOffer(PLAYER_INDEX other_player, uint8 *offerable, sint32 *advancesSkipped)
{
	uint8 *temp;
	sint32 num;
	temp = m_player->m_advances->CanOffer(g_player[other_player]->m_advances, num);
	memcpy(offerable, temp, g_theAdvanceDB->NumRecords() * sizeof(uint8));
	delete [] temp;
	for(sint32 i = 0; i < g_theAdvanceDB->NumRecords(); i++) {
		if(offerable[i]) {
			advancesSkipped[i] = g_player[other_player]->m_advances->GetMinPrerequisites(i);
		}
	}
	return num;
}

STDMETHODIMP_(sint32) C3Science::GetMinPrerequisites(sint32 adv)
{
	return m_player->m_advances->GetMinPrerequisites(adv);
}

STDMETHODIMP_(sint32) C3Science::GetCost(sint32 adv)
{
	return m_player->m_advances->GetCost(adv);
}

STDMETHODIMP_(sint32) C3Science::GetHisCost(PLAYER_INDEX other_player, sint32 adv)
{
	Assert(g_player[other_player]);
	if(g_player[other_player]) {
		return g_player[other_player]->m_advances->GetCost(adv);
	}
	return 0;
}

STDMETHODIMP_(sint32) C3Science::HowManyDoesHeKnow(PLAYER_INDEX other_player)
{
	Assert(g_player[other_player]);
	if(g_player[other_player]) {
		return g_player[other_player]->m_advances->GetDiscovered();
	}
	return 0;
}

STDMETHODIMP_(sint32) C3Science::GetCostOfWhatHeKnows(PLAYER_INDEX other_player)
{
	Assert(g_player[other_player]);
	if(g_player[other_player]) {
		return g_player[other_player]->m_advances->GetCostOfWhatHeKnows();
	}
	return 0;
}

STDMETHODIMP_(sint32) C3Science::GetHisMinPrerequisites(PLAYER_INDEX other_player, sint32 adv)
{
	Assert(g_player[other_player]);
	if(g_player[other_player]) {
		return g_player[other_player]->m_advances->GetMinPrerequisites(adv);
	}
	return 0;
}


char * C3Science::Science_Int_To_String(sint32 index)
{
	char * Science_string;

	
	Science_string = (char *) g_theAdvanceDB->GetNameStr(index);

	return Science_string;
}

sint32 C3Science::Science_String_To_Int(char *Science_name)
{
	StringId str_id;
	sint32 index;

	
	if (g_theStringDB->GetStringID(Science_name, str_id))
	{
		
		if (g_theAdvanceDB->GetNamedItem(str_id, index))
		{
			
			return index;

		} 

	} 

	Assert(false);

	return -1;

}
