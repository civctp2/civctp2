#pragma once

#ifndef __IC3Population_H__
#define __IC3Population_H__ 1

#include <OBJBASE.H>

DEFINE_GUID(CLSID_IC3Population, 0x89ae7a1, 0xff93, 0x11d1, 0x83, 0x55, 0x0, 0xc0, 0x4f, 0xba, 0x43, 0xc0) ;


struct MapPointData;

#undef INTERFACE
#define INTERFACE IC3Population

DECLARE_INTERFACE_(IC3Population, IUnknown)
	{

	STDMETHOD_ (BOOL, HasPopChanged)(THIS_ uint32 city_id, BOOL *is_unknown_id) PURE;
	STDMETHOD_ (BOOL, HasPopGrown)(THIS_ uint32 city_id, BOOL *is_unknown_id) PURE;
	STDMETHOD_ (BOOL, HasPopStarved)(THIS_ uint32 city_id, BOOL *is_unknown_id) PURE;
	STDMETHOD_ (BOOL, WasPopExpelled)(THIS_ uint32 city_id, BOOL *is_unknown_id) PURE;
	STDMETHOD_ (BOOL, WasImprovementBuilt)(THIS_ uint32 city_id, BOOL *is_unknown_id) PURE;
	STDMETHOD_ (BOOL, WasTerrainImprovementBuilt)(THIS_ uint32 city_id, BOOL *is_unknown_id) PURE;
	STDMETHOD_ (BOOL, WasHappinessAttacked)(THIS_ uint32 city_id, BOOL *is_unknown_id) PURE;
	STDMETHOD_ (BOOL, WasTerrainPolluted)(THIS_ void) PURE;
	STDMETHOD_ (sint32, GetCityPopCount)(THIS_ uint32 city_id, BOOL *is_unknowned_id) PURE;
	STDMETHOD_ (sint32, GetCitySlaveCount)(THIS_ uint32 city_id, BOOL *is_unknowned_id) PURE;
	STDMETHOD_ (void, SetCityPopInCity)(THIS_ uint32 city_id, uint32 pop_idx, BOOL inCity, BOOL *is_unknown_id) PURE ;
	STDMETHOD_ (BOOL, GetCityPopIsSlave)(THIS_ uint32 city_id, uint32 pop_idx, BOOL *is_unknown_id) PURE ;
	STDMETHOD_ (void, SetCityPopType)(THIS_ uint32 city_id, uint32 pop_idx, uint32 popType, BOOL *is_unknown_id) PURE ;

    STDMETHOD_ (void, GetRawHappiness) (THIS_ uint32 city_id, BOOL *is_unknown_id, double *raw_happiness, double *happy_per_entertainer) PURE;

	STDMETHOD_ (sint32, GetTileFood)(THIS_ uint32 city_id, MapPointData *pos, BOOL *is_unknown_id) PURE ;
	STDMETHOD_ (sint32, GetTileProduction)(THIS_ uint32 city_id, MapPointData *pos, BOOL *is_unknown_id) PURE ;
	STDMETHOD_ (sint32, GetTileResource)(THIS_ uint32 city_id, MapPointData *pos, BOOL *is_unknown_id) PURE ;
	STDMETHOD_ (void, GetCityProjectedFood)(THIS_ uint32 city_id, sint32 *food, BOOL *is_unknown_id) PURE ;
	STDMETHOD_ (void, GetCityProjectedTrade)(THIS_ uint32 city_id, BOOL *is_unknown_id,
         sint32 *projected_gross_gold, sint32 *projected_net_gold) PURE ;
	STDMETHOD_ (void, GetCityRequiredFood)(THIS_ uint32 city_id, sint32 *food, BOOL *is_unknown_id) PURE ;
	STDMETHOD_ (void, GetCityProjectedProduction)(THIS_ uint32 city_id, sint32 *production, BOOL *is_unknown_id) PURE ;
	STDMETHOD_ (BOOL, TryPlacePop)(THIS_ uint32 city_id, uint32 pop_idx, sint32 player_id, MapPointData *p, BOOL *is_unknown_id) PURE ;
	STDMETHOD_ (void, GetCityProjectedTradeFromCell)(THIS_ uint32 city_id, MapPointData *p, sint32 *trade, BOOL *is_unknown_id) PURE ;
	STDMETHOD_ (BOOL, IsPopAllowed)(THIS_ uint32 city_id, uint32 popType, BOOL *is_unknown_id) PURE ;
	STDMETHOD_ (void, GetCityScience)(THIS_ uint32 city_id, sint32 *science, BOOL *is_unknown_id) PURE ;

    STDMETHOD_ (double, GetCityScientistOutput)(THIS_ uint32 city_id, BOOL *is_unknown_id) PURE;
    STDMETHOD_ (double, GetCityGruntOutput)(THIS_ uint32 city_id, BOOL *is_unknown_id) PURE;
    STDMETHOD_ (double, GetCityMusicianOutput)(THIS_ uint32 city_id, BOOL *is_unknown_id) PURE;

    STDMETHOD_ (double, GetCitizensHunger) (THIS_) PURE;
    STDMETHOD_ (double, GetSlaveHunger) (THIS_ ) PURE;
	} ;

#endif __IC3Population_H__
