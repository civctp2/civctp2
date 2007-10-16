//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : Diplomacy response events
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
// - Added HotSeat and PBEM human-human diplomacy support. (17-Oct-2007 Martin Gühmann)
// - Seperated the NewProposal event from the Response event so that the 
//   NewProposal event can be called from slic witout any problems. (17-Oct-2007 Martin Gühmann) 
//
//----------------------------------------------------------------------------

#include "c3.h"

#include "ResponseEvent.h"

#include "Events.h"
#include "GameEventUser.h"
#include "Unit.h"

#include "StrDB.h"
#include "GameEventManager.h"

#include "aicause.h"
#include "player.h"

#include "Diplomat.h"
#include "mapanalysis.h" 

#include "SelItem.h"
extern SelectedItem *g_selected_item; 

#include "network.h"
#include "TurnCnt.h"

STDEHANDLER(NewProposalEvent)
{
	PLAYER_INDEX sender;
	PLAYER_INDEX receiver;

	
	if (!args->GetPlayer(0, sender))
		return GEV_HD_Continue;

	
	if (!args->GetPlayer(1, receiver))
		return GEV_HD_Continue;

	Diplomat::GetDiplomat(sender).ExecuteEventNewProposal(receiver);

	return GEV_HD_Continue;
}

STDEHANDLER(ResponseEvent)
{
	PLAYER_INDEX sender;
	PLAYER_INDEX receiver;

	if (!args->GetPlayer(0, sender))
		return GEV_HD_Continue;

	if (!args->GetPlayer(1, receiver))
		return GEV_HD_Continue;

	Diplomat & receiver_diplomat = Diplomat::GetDiplomat(receiver);
	Diplomat & sender_diplomat = Diplomat::GetDiplomat(sender);

	const Response & receiver_response_pending = receiver_diplomat.GetResponsePending(sender);
	const Response & sender_response_pending = sender_diplomat.GetResponsePending(receiver);

	bool show_response;
	if (sender_diplomat.GetReceiverHasInitiative(receiver))
	{
		show_response = 
		        receiver == g_selected_item->GetVisiblePlayer()
		    &&!(sender_response_pending == Diplomat::s_badResponse);
	}
	else
	{
		show_response = 
		        sender == g_selected_item->GetVisiblePlayer()
			&&!(receiver_response_pending == Diplomat::s_badResponse);
	}

	if (show_response)
	{
		g_gevManager->AddEvent(GEV_INSERT_Tail, GEV_ResponseReady, 
							   GEA_Player, sender,
							   GEA_Player, receiver,
							   GEA_End);
	}

	// If AI or network are involved
	if(
	   (
	    (           g_player[sender]->IsRobot()
	       && (    !g_network.IsActive()
	            ||  g_network.IsLocalPlayer(sender)
	          )
	    )
	||  (           g_network.IsActive()
	       &&       g_network.IsLocalPlayer(sender)
	       &&       sender != g_selected_item->GetVisiblePlayer()
	    )
	   )
	&&             !sender_diplomat.GetReceiverHasInitiative(receiver)
	  )
	{
		sender_diplomat.ExecuteResponse(sender, receiver);
		return GEV_HD_Continue;
	}

	// If AI is involved
	if(
	   (
	    (           g_player[receiver]->IsRobot()
	       && (    !g_network.IsActive()
	            ||  g_network.IsLocalPlayer(receiver)
	          )
	    )
	||  (           g_network.IsActive()
	       &&       g_network.IsLocalPlayer(receiver)
	       &&       receiver != g_selected_item->GetVisiblePlayer()
	    )
	   )
	&&              sender_diplomat.GetReceiverHasInitiative(receiver)
	  )
	{
		receiver_diplomat.ExecuteResponse(sender, receiver);
		return GEV_HD_Continue;
	}

	return GEV_HD_Continue;
}

void ResponseEventCallbacks::AddCallbacks()
{

	g_gevManager->AddCallback(GEV_NewProposal,
							  GEV_PRI_Primary,
							  &s_NewProposalEvent);


	g_gevManager->AddCallback(GEV_ProposalResponse,
							  GEV_PRI_Primary,
							  &s_ResponseEvent);


	g_gevManager->AddCallback(GEV_Reject,
							  GEV_PRI_Primary,
							  &s_ResponseEvent);


	g_gevManager->AddCallback(GEV_Counter,
							  GEV_PRI_Primary,
							  &s_ResponseEvent);


	g_gevManager->AddCallback(GEV_Threaten,
							  GEV_PRI_Primary,
							  &s_ResponseEvent);
}
