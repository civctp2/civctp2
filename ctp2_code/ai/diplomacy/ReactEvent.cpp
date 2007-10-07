//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : Diplomacy reaction events
// Id           : $Id:$
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
// - None
//
//----------------------------------------------------------------------------
//
// Modifications from the original Activision code:
//
// - None
//
//----------------------------------------------------------------------------

#include "c3.h"

#include "reactevent.h"

#include "Events.h"
#include "GameEventUser.h"
#include "Unit.h"
#include "StrDB.h"
#include "GameEventManager.h"

#include "aicause.h"
#include "player.h"
#include "c3_utilitydialogbox.h"


#include "SlicObject.h"
#include "SlicEngine.h"

#include "Diplomat.h"

STDEHANDLER(ReactEvent)
{
	PLAYER_INDEX sender;
	PLAYER_INDEX receiver;

	
	if (!args->GetPlayer(0, sender))
		return GEV_HD_Continue;

	
	if (!args->GetPlayer(1, receiver))
		return GEV_HD_Continue;

	return GEV_HD_Continue;
}

STDEHANDLER(ContinueDiplomacy)
{
	PLAYER_INDEX sender;
	PLAYER_INDEX receiver;

	
	if (!args->GetPlayer(0, sender))
		return GEV_HD_Continue;

	
	if (!args->GetPlayer(1, receiver))
		return GEV_HD_Continue;

	Diplomat & sender_diplomat = Diplomat::GetDiplomat(sender);

	sender_diplomat.ContinueDiplomacy(receiver);

	return GEV_HD_Continue;	
}

STDEHANDLER(ToggleInitiative)
{
	PLAYER_INDEX sender;
	PLAYER_INDEX receiver;

	
	if (!args->GetPlayer(0, sender))
		return GEV_HD_Continue;

	
	if (!args->GetPlayer(1, receiver))
		return GEV_HD_Continue;

	Diplomat & sender_diplomat = Diplomat::GetDiplomat(sender);
	bool receiver_has_initiative = 
		sender_diplomat.GetReceiverHasInitiative(receiver);
	
	if (receiver_has_initiative) {
		
		sender_diplomat.SetReceiverHasInitiative(receiver, false);
	}
	else {
		

		if (sender_diplomat.GetMyLastResponse(receiver).type == RESPONSE_THREATEN) 
		{
			
			Diplomat::GetDiplomat(receiver).SetMyLastResponse(sender, Diplomat::s_badResponse);
		}

		sender_diplomat.SetReceiverHasInitiative(receiver, true);
	}

	return GEV_HD_Continue;	
}

void ReactEventCallbacks::AddCallbacks() 
{
	
	g_gevManager->AddCallback(GEV_ResponseReady, 
							  GEV_PRI_Primary, 
							  &s_ReactEvent);

	g_gevManager->AddCallback(GEV_NewProposalReady, 
							  GEV_PRI_Primary, 
							  &s_ReactEvent);

	g_gevManager->AddCallback(GEV_ContinueDiplomacy, 
							  GEV_PRI_Primary, 
							  &s_ContinueDiplomacy);

	g_gevManager->AddCallback(GEV_ToggleInitiative, 
							  GEV_PRI_Primary, 
							  &s_ToggleInitiative);
}
