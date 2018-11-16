#pragma once

#ifndef __IC3Science_H__
#define __IC3Science_H__ 1

#include <OBJBASE.H>

DEFINE_GUID(CLSID_IC3Science,
0xbea2ff81, 0x50e, 0x11d2, 0x83, 0x55, 0x0, 0xc0, 0x4f, 0xba, 0x43, 0xc0);

#undef INTERFACE
#define INTERFACE IC3Science

DECLARE_INTERFACE_(IC3Science, IUnknown)
{


	STDMETHOD_ (sint32, GetNumAdvances)(THIS) PURE ;
	STDMETHOD_ (sint32, GetNumDependencies)(THIS_ sint32 adv) PURE ;
	STDMETHOD_ (sint32, GetNumPrerequisites)(THIS_ sint32 adv) PURE ;
	STDMETHOD_ (sint32, GetLeadsTo)(THIS_ sint32 adv, sint32 lead) PURE ;
	STDMETHOD_ (sint32, GetPrereq)(THIS_ sint32 adv, sint32 req) PURE ;

    STDMETHOD_ (BOOL, HasAdvance)(THIS_ sint32 adv) PURE ;
    STDMETHOD_ (void, SetResearching)(THIS_ sint32 adv) PURE;

    STDMETHOD_ (BOOL, IsCurrentlyReserching) (THIS) PURE;

    STDMETHOD_ (sint32, GetCurrentScienceCost)(THIS)PURE;
    STDMETHOD_ (sint32, GetCurrentScienceLevel)(THIS)PURE;
	STDMETHOD_ (sint32, GetCanAskFor) (THIS_ PLAYER_INDEX other_player, uint8 *askable) PURE;
	STDMETHOD_ (sint32, GetCanOffer) (THIS_ PLAYER_INDEX other_player, uint8 *offerable, sint32 *advancesSkipped) PURE;
	STDMETHOD_ (sint32, GetMinPrerequisites) (THIS_ sint32 adv) PURE;
	STDMETHOD_(sint32, GetCost) (THIS_ sint32 adv) PURE;
	STDMETHOD_(sint32, GetHisCost) (THIS_ PLAYER_INDEX other_player, sint32 adv) PURE;
	STDMETHOD_(sint32, GetHisMinPrerequisites) (THIS_ PLAYER_INDEX other_player, sint32 adv) PURE;

	STDMETHOD_ (char *, Science_Int_To_String) (sint32 index) PURE;
	STDMETHOD_ (sint32, Science_String_To_Int) (char *Science_name) PURE;

	STDMETHOD_(sint32, HowManyDoesHeKnow) (THIS_ PLAYER_INDEX other_player) PURE;
	STDMETHOD_(sint32, GetCostOfWhatHeKnows) (THIS_ PLAYER_INDEX other_player) PURE;

};

#endif __IC3Science_H__
