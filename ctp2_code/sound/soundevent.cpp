
#include "c3.h"
#include "Events.h"
#include "SoundManager.h"
#include "gamesounds.h"
#include "GameEventUser.h"
#include "Unit.h"
#include "SelItem.h"

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
	
