










#pragma once

#ifndef __C3Wonder_H__
#define __C3Wonder_H__ 1

#include "IC3Wonder.h"

#define PLAYER_INDEX_INVALID	32

class Player; 
class CivArchive; 

class C3Wonder : public IC3Wonder
	{
	ULONG m_refCount;
	
	
    Player	*m_player ;
	sint32	m_owner ;

	public:
		
		STDMETHODIMP QueryInterface(REFIID, void **obj) ;
		STDMETHODIMP_(ULONG) AddRef() ;
		STDMETHODIMP_(ULONG) Release() ;

		C3Wonder(sint32 idx) ;
		C3Wonder(CivArchive &archive) ;
		void Serialize(CivArchive &archive) ;

		STDMETHODIMP_ (sint32) GetNumWonders(void) ;
		STDMETHODIMP_ (BOOL) HasBeenBuilt(sint32 wonder_type) ;
        STDMETHODIMP_ (double) MaxPecentageThisWonderDone(sint32 wonder_type); 
        STDMETHODIMP_ (BOOL) WondersAvailable(sint32 player_idx);

	   	STDMETHODIMP_ (sint32) EnablingDiscovery(sint32 wonder_type) ;
		STDMETHODIMP_ (sint32) GetOwner(sint32 wonderid) ;
	   	STDMETHODIMP_ (sint32) ObsoleteDiscovery(sint32 wonder_type) ;
        
        STDMETHODIMP_ (sint32) GetProductionCost(sint32 wonder_type); 
        STDMETHODIMP_ (void) GetAllEffects(sint32 idx_wonder, double *effects);
		STDMETHODIMP_ (char *) Wonder_Int_To_String(sint32 index); 
		STDMETHODIMP_ (sint32) Wonder_String_To_Int(char *wonder_name); 

        STDMETHODIMP_ (BOOL) CanBuild(sint32 wonder_type); 
        STDMETHODIMP_ (BOOL) IsObsolete(sint32 wonder_type); 

        STDMETHODIMP_ (BOOL) IsWormholeDetectorBuilt();

	} ;

#endif __C3Wonder_H__
