#include "c3.h"

#include "ResponseEvent.h"

#include "Events.h"
#include "GameEventUser.h"
#include "Unit.h"

#include "StrDB.h"
#include "GameEventManager.h"

#include "AICause.h"
#include "player.h"

#include "Diplomat.h"
#include "mapanalysis.h"

#include "SelItem.h"
extern SelectedItem *g_selected_item;

#include "network.h"







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
		 ((receiver == g_selected_item->GetVisiblePlayer()) &&
		 !(sender_response_pending == Diplomat::s_badResponse));
	}
	else
	{
		show_response =
		 ((sender == g_selected_item->GetVisiblePlayer()) &&
		 !(receiver_response_pending == Diplomat::s_badResponse));
	}

	if (show_response)
	{

		g_gevManager->AddEvent(GEV_INSERT_Tail, GEV_ResponseReady,
							   GEA_Player, sender,
							   GEA_Player, receiver,
							   GEA_End);
	}




	if(((g_player[sender]->m_playerType == PLAYER_TYPE_ROBOT && (!g_network.IsActive() || g_network.IsLocalPlayer(sender))) ||
		(g_network.IsActive() && g_network.IsLocalPlayer(sender) && sender != g_selected_item->GetVisiblePlayer())) &&
	   !sender_diplomat.GetReceiverHasInitiative(receiver))
	{

		sender_diplomat.ExecuteResponse(sender, receiver);

		return GEV_HD_Continue;
	}




	if(((g_player[receiver]->m_playerType == PLAYER_TYPE_ROBOT && (!g_network.IsActive() || g_network.IsLocalPlayer(receiver))) ||
		(g_network.IsActive() && g_network.IsLocalPlayer(receiver) && receiver != g_selected_item->GetVisiblePlayer())) &&
	   sender_diplomat.GetReceiverHasInitiative(receiver)) {

		receiver_diplomat.ExecuteResponse(sender, receiver);

		return GEV_HD_Continue;
	}

	return GEV_HD_Continue;
}

void ResponseEventCallbacks::AddCallbacks()
{

	g_gevManager->AddCallback(GEV_NewProposal,
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
