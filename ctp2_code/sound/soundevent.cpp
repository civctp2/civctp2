//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ header
// Description  : General declarations
//
//----------------------------------------------------------------------------
//
// Disclaimer
//
// THIS FILE IS NOT GENERATED OR SUPPORTED BY ACTIVISION.
//
// This material has been developed at apolyton.net by the Apolyton CtP2
// Source Code Project. Contact the authors at ctp2source@apolyton.net.
//
//----------------------------------------------------------------------------
//
// Compiler flags
//
// _DEBUG
// - Generate debug version
//
// _MSC_VER
// - Use Microsoft C++ extensions when set.
//
//----------------------------------------------------------------------------
//
// Modifications from the original Activision code:
//
// - includes fixed for case sensitive filesystems
//
//----------------------------------------------------------------------------

#include "ctp/c3.h"
#include "gs/gameobj/Events.h"
#include "sound/soundmanager.h"
#include "sound/gamesounds.h"
#include "gs/events/GameEventUser.h"
#include "gs/gameobj/Unit.h"
#include "ui/aui_ctp2/SelItem.h"

STDEHANDLER(SoundCreateCityEvent)
{
	Unit city;
	if(!args->GetCity(0, city)) return GEV_HD_Continue;

	if(g_soundManager) {
		if(g_selected_item->GetVisiblePlayer() == city.GetOwner()) {
			g_soundManager->AddSound(SOUNDTYPE_SFX, 0,
									 gamesounds_GetGameSoundID(GAMESOUNDS_SETTLE_CITY),
									 city.RetPos().x,
									 city.RetPos().y);
		}
	}
	return GEV_HD_Continue;
}

void soundevent_Initialize()
{
	g_gevManager->AddCallback(GEV_CreateCity, GEV_PRI_Post, &s_SoundCreateCityEvent);
}

void soundevent_Cleanup()
{
}
