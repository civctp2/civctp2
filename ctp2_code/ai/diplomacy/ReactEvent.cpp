//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : Diplomacy reaction events
// Id           : $Id$
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

#include "ctp/c3.h"

#include "ai/diplomacy/reactevent.h"

#include "gs/gameobj/Events.h"
#include "gs/events/GameEventUser.h"
#include "gs/gameobj/Unit.h"
#include "gs/database/StrDB.h"
#include "gs/events/GameEventManager.h"

#include "gs/outcom/AICause.h"
#include "gs/gameobj/Player.h"
#include "ui/aui_ctp2/c3_utilitydialogbox.h"

#include "gs/slic/SlicObject.h"
#include "gs/slic/SlicEngine.h"

#include "ai/diplomacy/Diplomat.h"

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
