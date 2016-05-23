#ifdef HAVE_PRAGMA_ONCE
#pragma once
#endif

#ifndef __C3Population_H__
#define __C3Population_H__ 1

#include "ic3Population.h"

class Player ;
class CivArchive ;

class C3Population : public IC3Population
	{
	ULONG m_refCount ;

    Player *m_player ;
	sint32	m_owner ;

public:

	STDMETHODIMP QueryInterface(REFIID, void **obj) ;
	STDMETHODIMP_(ULONG) AddRef() ;
	STDMETHODIMP_(ULONG) Release() ;

    C3Population(sint32 idx) ;
    C3Population(CivArchive &archive) ;
    void Serialize(CivArchive &archive) ;

    STDMETHODIMP_ (BOOL) HasPopChanged(uint32 city_id, BOOL *is_unknown_id) ;
    STDMETHODIMP_ (BOOL) HasPopGrown(uint32 city_id, BOOL *is_unknown_id) ;
    STDMETHODIMP_ (BOOL) HasPopStarved(uint32 city_id, BOOL *is_unknown_id) ;
    STDMETHODIMP_ (BOOL) WasPopExpelled(uint32 city_id, BOOL *is_unknown_id) ;
    STDMETHODIMP_ (BOOL) WasImprovementBuilt(uint32 city_id, BOOL *is_unknown_id) ;
    STDMETHODIMP_ (BOOL) WasTerrainImprovementBuilt(uint32 city_id, BOOL *is_unknown_id) ;
    STDMETHODIMP_ (BOOL) WasHappinessAttacked(uint32 city_id, BOOL *is_unknown_id) ;
    STDMETHODIMP_ (BOOL) WasTerrainPolluted(void) ;
	STDMETHODIMP_ (sint32) GetCityPopCount(uint32 city_id, BOOL *is_unknowned_id) ;
	STDMETHODIMP_ (sint32) GetCitySlaveCount(uint32 city_id, BOOL *is_unknowned_id) ;
	STDMETHODIMP_ (void) SetCityPopInCity(uint32 city_id, uint32 pop_idx, BOOL inCity, BOOL *is_unknown_id) ;
	STDMETHODIMP_ (BOOL) GetCityPopIsSlave(uint32 city_id, uint32 pop_idx, BOOL *is_unknown_id) ;
	STDMETHODIMP_ (void) SetCityPopType(uint32 city_id, uint32 pop_idx, uint32 popType, BOOL *is_unknown_id) ;

    STDMETHODIMP_ (void) GetRawHappiness(uint32 city_id, BOOL *is_unknown_id, double *raw_happiness, double *happy_per_entertainer);

	STDMETHODIMP_ (sint32) GetTileFood(uint32 city_id, MapPointData *pos, BOOL *is_unknown_id) ;
	STDMETHODIMP_ (sint32) GetTileProduction(uint32 city_id, MapPointData *pos, BOOL *is_unknown_id) ;
	STDMETHODIMP_ (sint32) GetTileResource(uint32 city_id, MapPointData *pos, BOOL *is_unknown_id) ;
	STDMETHODIMP_ (void) GetCityProjectedFood(uint32 city_id, sint32 *food, BOOL *is_unknown_id) ;
	STDMETHODIMP_ (void) GetCityRequiredFood(uint32 city_id, sint32 *food, BOOL *is_unknown_id) ;
	STDMETHODIMP_ (void) GetCityProjectedProduction(uint32 city_id, sint32 *production, BOOL *is_unknown_id) ;
	STDMETHODIMP_ (void) GetCityProjectedTrade(uint32 city_id, BOOL *is_unknown_id,
        sint32 *projected_gross_gold, sint32 *projected_net_gold) ;
	STDMETHODIMP_ (void) GetCityProjectedTradeFromCell(uint32 city_id, MapPointData *p, sint32 *trade, BOOL *is_unknown_id) ;
	STDMETHODIMP_ (BOOL) TryPlacePop(uint32 city_id, uint32 pop_idx, sint32 player_id, MapPointData *p, BOOL *is_unknown_id) ;
	STDMETHODIMP_ (BOOL) IsPopAllowed(uint32 city_id, uint32 popType, BOOL *is_unknown_id) ;
	STDMETHODIMP_ (void) GetCityScience(uint32 city_id, sint32 *science, BOOL *is_unknown_id) ;
    STDMETHODIMP_ (double) GetCityScientistOutput(uint32 city_id, BOOL *is_unknown_id);
    STDMETHODIMP_ (double) GetCityGruntOutput(uint32 city_id, BOOL *is_unknown_id);
    STDMETHODIMP_ (double) GetCityMusicianOutput(uint32 city_id, BOOL *is_unknown_id);

    STDMETHODIMP_ (double) GetCitizensHunger ();
    STDMETHODIMP_ (double) GetSlaveHunger ();

    } ;

#endif __C3Population_H__
