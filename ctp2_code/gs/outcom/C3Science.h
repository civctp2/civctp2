










#pragma once

#ifndef __C3Science_H__
#define __C3Science_H__ 1

#include "ic3Science.h"

class Player; 
class CivArchive; 

class C3Science : public IC3Science
{
	ULONG m_refCount;
	
	
    Player	*m_player ;
	sint32	m_owner ;

public:
	
	STDMETHODIMP QueryInterface(REFIID, void **obj);
	STDMETHODIMP_(ULONG) AddRef();
	STDMETHODIMP_(ULONG) Release();

    C3Science(sint32 idx); 
    C3Science(CivArchive &archive);
    void Serialize(CivArchive &archive);
	
	STDMETHODIMP_ (sint32) GetNumAdvances(void) ;
	STDMETHODIMP_ (sint32) GetNumDependencies(sint32 adv) ;
	STDMETHODIMP_ (sint32) GetNumPrerequisites(sint32 adv) ;
	STDMETHODIMP_ (sint32) GetLeadsTo(sint32 adv, sint32 lead) ;
	STDMETHODIMP_ (sint32) GetPrereq(sint32 adv, sint32 req) ;

    
    STDMETHODIMP_ (BOOL) HasAdvance (sint32 adv);
    STDMETHODIMP_ (void) SetResearching (sint32 adv);
    
    STDMETHODIMP_ (BOOL) IsCurrentlyReserching();
    STDMETHODIMP_ (sint32) GetCurrentScienceCost ();
    STDMETHODIMP_ (sint32) GetCurrentScienceLevel();
	STDMETHODIMP_ (sint32) GetCanAskFor(PLAYER_INDEX other_player, uint8 *askable);
	STDMETHODIMP_ (sint32) GetCanOffer(PLAYER_INDEX other_player, uint8 *offerable, sint32 *advancesSkipped);

	STDMETHODIMP_ (sint32) GetMinPrerequisites(sint32 adv);
	STDMETHODIMP_(sint32) GetCost(sint32 adv);
	STDMETHODIMP_(sint32) GetHisCost(PLAYER_INDEX other_player, sint32 adv);
	STDMETHODIMP_(sint32) GetHisMinPrerequisites(PLAYER_INDEX other_player, sint32 adv);
	STDMETHODIMP_(sint32) HowManyDoesHeKnow(PLAYER_INDEX other_player);
	STDMETHODIMP_(sint32) GetCostOfWhatHeKnows(PLAYER_INDEX other_player);

	STDMETHODIMP_ (char *) Science_Int_To_String(sint32 index);
	STDMETHODIMP_ (sint32) Science_String_To_Int(char *Science_name);

};

#endif __C3Science_H__
