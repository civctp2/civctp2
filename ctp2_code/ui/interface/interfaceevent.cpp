//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : Event handlers for the user interface.
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
// - Prevent production errors when pressing F3 after end of turn.
// - The information window is no more closed on the begin of a new turn.
//   (Aug 7th 2005 Martin Gühmann)
// - The initial city interface is no more displayed if the visible player
//   is a robot. (26-Jan-2008 Martin Gühmann)
//
//----------------------------------------------------------------------------

#include "c3.h"
#include "interfaceevent.h"
#include "Events.h"
#include "GameEventUser.h"
#include "Unit.h"
#include "SelItem.h"
#include "c3_utilitydialogbox.h"
#include "controlpanelwindow.h"
#include "MainControlPanel.h"
#include "profileDB.h"
#include "citywindow.h"
#include "ArmyData.h"
#include "director.h"
#include "EditQueue.h"
#include "screenutils.h"
#include "soundmanager.h"
#include "network.h"
#include "gamesounds.h"

extern sint32               g_modalWindow;

STDEHANDLER(InterfaceCreateCityEvent)
{
	Unit city;
	if(!args->GetCity(0, city)) return GEV_HD_Continue;

	if(city.GetOwner() == g_selected_item->GetVisiblePlayer()) {
		if(g_theProfileDB->GetAutoRenameCities()) {
			c3_utilitydialogbox_NameCity(city);
		}
	}

	return GEV_HD_Continue;
}

STDEHANDLER(InterfaceMakePopEvent)
{
#if 0
	if(g_theProfileDB->GetAutoOpenCityWindow()) {
		static Unit city;
		if(!args->GetCity(0, city)) return GEV_HD_Continue;
		if(city.GetOwner() == g_selected_item->GetVisiblePlayer() &&
		   city.CD()->PopCount() == 1) {

			EditQueue::Display(CityWindow::GetCityData(city));
		}
	}
#endif
	return GEV_HD_Continue;
}

STDEHANDLER(InterfaceOpenInitialCityInterfaceEvent)
{
	if(g_theProfileDB->GetAutoOpenCityWindow()) {
		static Unit city;
		if(!args->GetCity(0, city)) return GEV_HD_Continue;
		if( city.GetOwner() == g_selected_item->GetVisiblePlayer()
		&& !g_player[city.GetOwner()]->IsRobot()
		){
			EditQueue::Display(city);
		}
	}
	return GEV_HD_Continue;
}

STDEHANDLER(InterfaceUpdateCityEvent)
{
	static Unit city, selCity;
	if(!args->GetCity(0, city)) return GEV_HD_Continue;

	if(g_selected_item->GetSelectedCity(selCity) && city.m_id == selCity.m_id) {
		controlpanelwindow_Update(&city);
	}
	return GEV_HD_Continue;
}

STDEHANDLER(InterfaceStartMovePhaseEvent)
{
	sint32 pl;
	if(!args->GetPlayer(0, pl)) return GEV_HD_Continue;
	if(pl == g_selected_item->GetVisiblePlayer()) {
		static Unit selCity;
		if(g_selected_item->GetSelectedCity(selCity)) {
			controlpanelwindow_Update(&selCity);
		}
	}
	return GEV_HD_Continue;
}

STDEHANDLER(InterfaceCityWindowUnitNotification)
{
	CityWindow::NotifyUnitChange();
	return GEV_HD_Continue;
}

STDEHANDLER(InterfaceUpdateCityProjection)
{
	sint32 pl;
	if(!args->GetPlayer(0, pl)) return GEV_HD_Continue;

	if(pl == g_selected_item->GetVisiblePlayer()) {
		MainControlPanel::SelectedCity();
		// Reenable opening the city window - see InterfacePreBeginTurnEvent.
		if (g_modalWindow > 0)
		{
			--g_modalWindow;
		}
	}

	return GEV_HD_Continue;
}

STDEHANDLER(InterfaceBeginTurnRecenter)
{
	sint32 pl;
	if(!args->GetPlayer(0, pl)) return GEV_HD_Continue;

	if(pl == g_selected_item->GetVisiblePlayer())
	{
		Army a;
		Unit c;
		MapPoint pos(-1,-1);
		if(g_selected_item->GetSelectedArmy(a))
		{
			pos = a->RetPos();
		}
		else if(g_selected_item->GetSelectedCity(c))
		{
			pos = c.RetPos();
		}

		if(g_selected_item->IsAutoCenterOn()
		&& pos.x >= 0
		&& !g_director->TileWillBeCompletelyVisible(pos.x, pos.y)
		){
			g_director->AddCenterMap(pos);
		}
	}
	return GEV_HD_Continue;
}

STDEHANDLER(InterfacePreBeginTurn)
{
	sint32 pl;
	if(!args->GetPlayer(0, pl)) return GEV_HD_Continue;

	if(pl == g_selected_item->GetVisiblePlayer()) {
		close_AllScreensAndUpdateInfoScreen();
		// Prevent opening the city window during the production computations.
		// It will be reenabled in InterfaceUpdateCityProjection.
		++g_modalWindow;
		if(g_controlPanel)
			g_controlPanel->ClearTargetingMode();

		if(g_soundManager) {
			g_soundManager->TerminateAllLoopingSounds(SOUNDTYPE_SFX);
			g_soundManager->TerminateAllLoopingSounds(SOUNDTYPE_VOICE);
		}

		if(g_network.IsHost() && g_network.GetPlayerIndex() == pl) {

			if (g_soundManager) {
				g_soundManager->AddSound(SOUNDTYPE_SFX, (uint32)0,
				                         gamesounds_GetGameSoundID(GAMESOUNDS_NET_YOUR_TURN),
				                         0,
				                         0);
			}
		}
	}
	return GEV_HD_Continue;
}

void interfaceevent_Initialize()
{

	MainControlPanel::InitializeEvents();

	g_gevManager->AddCallback(GEV_CreateCity, GEV_PRI_Post, &s_InterfaceCreateCityEvent);
	g_gevManager->AddCallback(GEV_MakePop, GEV_PRI_Post, &s_InterfaceMakePopEvent);
	g_gevManager->AddCallback(GEV_OpenInitialCityInterface, GEV_PRI_Post, &s_InterfaceOpenInitialCityInterfaceEvent);


	g_gevManager->AddCallback(GEV_BuildUnit, GEV_PRI_Post, &s_InterfaceUpdateCityEvent);
	g_gevManager->AddCallback(GEV_BuildBuilding, GEV_PRI_Post, &s_InterfaceUpdateCityEvent);
	g_gevManager->AddCallback(GEV_BuildWonder, GEV_PRI_Post, &s_InterfaceUpdateCityEvent);
	g_gevManager->AddCallback(GEV_BuildFront, GEV_PRI_Post, &s_InterfaceUpdateCityEvent);
	g_gevManager->AddCallback(GEV_CityBeginTurn, GEV_PRI_Post, &s_InterfaceUpdateCityEvent);
	g_gevManager->AddCallback(GEV_ZeroProduction, GEV_PRI_Post, &s_InterfaceUpdateCityEvent);
	g_gevManager->AddCallback(GEV_RollOverProduction, GEV_PRI_Post, &s_InterfaceUpdateCityEvent);

	g_gevManager->AddCallback(GEV_StartMovePhase, GEV_PRI_Post, &s_InterfaceStartMovePhaseEvent);

	g_gevManager->AddCallback(GEV_DisbandUnit, GEV_PRI_Post, &s_InterfaceCityWindowUnitNotification);

	g_gevManager->AddCallback(GEV_StartMovePhase, GEV_PRI_Post, &s_InterfaceUpdateCityProjection);
	g_gevManager->AddCallback(GEV_StartMovePhase, GEV_PRI_Post, &s_InterfaceBeginTurnRecenter);

	g_gevManager->AddCallback(GEV_BeginTurn, GEV_PRI_Pre, &s_InterfacePreBeginTurn);
}

void interfaceevent_Cleanup()
{
}
