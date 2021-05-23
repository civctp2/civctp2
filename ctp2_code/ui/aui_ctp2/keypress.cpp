//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : Key press handling
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
// _PLAYTEST
// - Generates version with utilities for playtesting
//
// _DEBUG
// - Generate debug version when set.
//
//----------------------------------------------------------------------------
//
// Modifications from the original Activision code:
//
// - Added music screen key by ahenobarb.
// - Start the great library with the current research project of the player.
// - Disabled restart key in network, hot seat and email gmase, by
//   Martin Gühmann.
// - Opening the score tab of the info window does not close other windows
//   anymore like the other tabs. - Aug 7th 2005 Martin Gühmann
// - Strongly modal windows like the DipWizzard cannot closed anymore by
//   by keypresses that open other windows. (20-10-2007 Martin Gühmann)
//
//----------------------------------------------------------------------------

#include "c3.h"

#include "pixelutils.h"
#include "UnitSpriteGroup.h"

#include "aui.h"
#include "c3ui.h"
#include "aui_window.h"
#include "radarwindow.h"
#include "statuswindow.h"
#include "primitives.h"
#include "pattern.h"

#include "c3debug.h"
#include "c3cmdline.h"

#include "keymap.h"

#include "player.h"
#include "gamefile.h"

#include "network.h"
#include "net_action.h"
#include "net_info.h"
#include "net_rand.h"

#include "tiledmap.h"
#include "background.h"
#include "World.h"
#include "civ3_main.h"
#include "pollution.h"
#include "SelItem.h"
#include "newturncount.h"
#include "TurnCnt.h"
#include "AICause.h"
#include "radarmap.h"
#include "DataCheck.h"

#include "km_screen.h"
#include "screenutils.h"
#include "optionswindow.h"
#include "resourcemap.h"

#include "civapp.h"
#include "Order.h"

#include "controlpanelwindow.h"
#include "c3_listbox.h"
#include "aui_ranger.h"

#include "loadsavewindow.h"
#include "ArmyPool.h"
#include "GameSettings.h"
#include "messagemodal.h"
#include "MessagePool.h"
#include "SlicButton.h"
#include "messagewindow.h"
#include "messageactions.h"
#include "workwindow.h"
#include "workwin.h"

#include "SlicEngine.h"

#include "segmentlist.h"

#include "UnitData.h"
#include "citydata.h"

#include "GameEventManager.h"
#include "CTP2Combat.h"

#include "armymanagerwindow.h"
#include "ctp2_Window.h"
#include "optionwarningscreen.h"

#include "dipwizard.h"
#include "keypress.h"
#include "sciencevictorydialog.h"
#include "citywindow.h"
#include "infowindow.h"
#include "cityespionage.h"
#include "trademanager.h"
#include "EditQueue.h"
#include "soundscreen.h"
// music added by ahenobarb
#include "musicscreen.h"
#include "graphicsscreen.h"
#include "gameplayoptions.h"
#include "OrderRecord.h"
#include "ProfileEdit.h"
#include "cellunitlist.h"
#include "ArmyData.h"
#include "MainControlPanel.h"
#include "UnitControlPanel.h"
#include "segmentlist.h"
#include "helptile.h"

extern C3UI			*g_c3ui;
extern BOOL			gSuspended;
extern TiledMap 	*g_tiledMap;
extern RadarMap     *g_radarMap;
extern ResourceMap		*g_resourceMap;
extern Background	*g_background;
extern WorkWindow	*g_workWindow;

extern StatusWindow	*g_statusWindow;
extern Player		**g_player;
extern World        *g_theWorld;
extern SelectedItem *g_selected_item;
extern Pollution	*g_thePollution ;

extern BOOL			g_helpMode;

extern Network g_network;

extern HWND gHwnd;

extern sint32		g_is_rand_test;
extern SelectedItem *g_selected_item;
extern TurnCount	*g_turn;

#ifdef _PLAYTEST
extern CommandLine	g_commandLine;
#endif

KEYMAP *theKeyMap = NULL;

extern DataCheck	*g_DataCheck;
extern sint32		g_debugOwner;

extern CivApp		*g_civApp;

extern sint32		g_isKMScreen;
extern ControlPanelWindow *g_controlPanel;




#include "profileDB.h"
extern ProfileDB *g_theProfileDB;

#include "c3_utilitydialogbox.h"
c3_UtilityPlayerListPopup *g_networkPlayersScreen = NULL;
extern c3_UtilityTextMessagePopup		*g_utilityTextMessage;

#include "chatbox.h"
extern ChatBox		*g_chatBox;

#include "StrDB.h"
extern StringDB		*g_theStringDB;

#include "keyboardhandler.h"

extern MessageModal *g_modalMessage;
extern MessagePool *g_theMessagePool;
extern MessageWindow	*g_currentMessageWindow;

extern sint32 g_modalWindow;

#include "debugwindow.h"
extern DebugWindow		*g_debugWindow;

#include "battleviewwindow.h"
extern BattleViewWindow *g_battleViewWindow;

#include "spriteeditor.h"  // g_spriteEditWindow
extern SpriteEditWindow     *g_spriteEditWindow;

PointerList<KeyboardHandler> g_keyboardHandlers;

void keypress_QuitCallback( sint32 val )
{
	if ( val ) {
#ifdef __AUI_USE_DIRECTX__
        PostMessage(gHwnd, WM_CLOSE, 0, 0);
#elif defined(__AUI_USE_SDL__)
	static SDL_Event quit = { 0 };
        quit.type = SDL_QUIT;
        quit.quit.type = SDL_QUIT;
        int e = SDL_PushEvent(&quit);
#endif
	}
}

void keypress_RegisterHandler(KeyboardHandler *handler)
{
	g_keyboardHandlers.AddTail(handler);
}

void keypress_RemoveHandler(KeyboardHandler *handler)
{
	PointerList<KeyboardHandler>::Walker walk(&g_keyboardHandlers);
	while(walk.IsValid()) {
		if(walk.GetObj() == handler) {
			walk.Remove();
		} else {
			walk.Next();
		}
	}
}

void init_keymap() {
	if ( !theKeyMap )
		theKeyMap = new KEYMAP(FALSE);

}

void init_defaultKeymap() {
	if(!theKeyMap)
		theKeyMap = new KEYMAP(TRUE);
}

void cleanup_keymap()
{
	delete theKeyMap;
	theKeyMap = NULL;
}
































sint32 g_keypress_stop_player;

BOOL	commandMode = FALSE;

sint32 ui_HandleKeypress(WPARAM wParam, LPARAM lParam)
{
    WORLD_DIRECTION	d;
    BOOL			move = FALSE;
#ifdef _PLAYTEST
    int i;
#endif
	BOOL isMyTurn = !g_network.IsActive() || g_network.IsMyTurn();
	Unit city;

	if ( g_isKMScreen) {
		switch(wParam) {
			case VK_ESCAPE:
			case VK_LEFT + 256:
			case VK_RIGHT + 256:
			case VK_UP + 256:
			case VK_DOWN + 256:
			case 29:
			case 28:

				return TRUE;
			default:

				km_screen_remapKey( wParam, lParam );
				return TRUE;
		}
	}

#ifdef _PLAYTEST
	if(commandMode) {
		switch(wParam) {
			case '\r' + 128: wParam = '\r'; break;
			case '\t' + 128: wParam = '\t'; break;
			case 8 + 128: wParam = 8; break;
		}
		commandMode = g_commandLine.AddKey(static_cast<char>(wParam));
		return TRUE;
	}
#endif

	// If a tooltip is shown, close it to prevent keyboard events being sent to it.
	if (g_c3ui->TopWindow() && g_c3ui->TopWindow()->Type() == AUI_WINDOW_TYPE_TIP) {
		g_ui->RemoveWindow(g_c3ui->TopWindow()->Id());
	}

	if (wParam == VK_ESCAPE) {

		extern OptionsWindow *g_optionsWindow;

		if(g_c3ui->TopWindow() && g_c3ui->TopWindow() == DipWizard::GetWindow()) {
			DipWizard::Hide();
		} else if(g_keyboardHandlers.GetTail()) {
			g_keyboardHandlers.GetTail()->kh_Close();
		} else if (g_civApp->IsGameLoaded()) {
			if(g_currentMessageWindow &&
			   g_currentMessageWindow->GetMessage() &&
			   (g_theMessagePool->IsValid(*g_currentMessageWindow->GetMessage()))) {
				g_currentMessageWindow->GetMessage()->Minimize();
			} else if(g_modalMessage) {
				Message *msg = g_modalMessage->GetMessage();
				if(msg && g_theMessagePool->IsValid(*msg)) {
					Assert(msg->IsAlertBox());
					MessageData *data = msg->AccessData();
					if(data->GetNumButtons() <= 2 && data->GetNumButtons() > 0) {
						data->GetButton(0)->Callback();
					}
				}
			} else if (g_chatBox->IsActive()) {
				g_chatBox->SetActive(false);
			} else if (helptile_isShown()) {
				helptile_closeWindow();
			} else if (g_optionsWindow && g_c3ui->GetWindow(g_optionsWindow->Id())) {
				optionsscreen_removeMyWindow(AUI_BUTTON_ACTION_EXECUTE);
			} else if (g_c3ui->TopWindow() && g_c3ui->TopWindow()->HandleKey(wParam)) {
				// Nothing
			} else if (g_battleViewWindow) {
				battleview_ExitButtonActionCallback(NULL, AUI_BUTTON_ACTION_EXECUTE, 0, NULL);
			} else {
				optionsscreen_Initialize();
				optionsscreen_displayMyWindow(1);
			}
		}
		return TRUE;
	}

	aui_Window *topWindow = g_c3ui->TopWindow();
	if(topWindow && (!g_controlPanel || topWindow != g_controlPanel->GetWindow()) && topWindow != g_statusWindow) {
		if(topWindow->HandleKey(wParam))
			return 0;
	}

	if(g_spriteEditWindow && g_spriteEditWindow->HandleKey(wParam)){ // pass keys to text field in sprite edit bar (open/save sprites)
			return 0;
		}

	if (!theKeyMap) return 0;

	if (g_slicEngine && g_slicEngine->RunKeyboardTrigger(static_cast<char>(wParam)))
	{
		return 0;
	}

	if (!g_civApp->IsGameLoaded()) {
		return TRUE;
	}

	KEY_FUNCTION	kf = theKeyMap->get_function(wParam);
	if (kf != KEY_FUNCTION_NOOP)
	{
		g_selected_item->RegisterUIClick();
	}

	if (topWindow->IsStronglyModal() /*&& keypress_IsGameFunction(kf)*/) // I should not be able to open other windows if I have have open strongly modal windows like the DipWizzard
	{
		return 0;
	}

#if 0
	MapPoint point;
	g_tiledMap->GetMouseTilePos(point);
#endif

	switch (kf) {
#ifdef _PLAYTEST
#ifdef CTP2_ENABLE_SLICDEBUG // Should be enabled in the debug version
	case KEY_FUNCTION_ENTER_COMMAND:
		segmentlist_Display();
		break;
#endif // CTP2_ENABLE_SLICDEBUG
	case KEY_FUNCTION_ENTER_COMMAND_ALTERNATE:
		commandMode = TRUE;
		move = FALSE;
		break;
#endif
    case KEY_FUNCTION_NOOP: return FALSE;
    case KEY_FUNCTION_MOVE_NORTHWEST: d = NORTHWEST; move = TRUE; break;
    case KEY_FUNCTION_MOVE_NORTH: d = NORTH; move = TRUE; break;
    case KEY_FUNCTION_MOVE_NORTHEAST: d = NORTHEAST; move = TRUE; break;
    case KEY_FUNCTION_MOVE_WEST: d = WEST; move = TRUE; break;
    case KEY_FUNCTION_MOVE_EAST: d = EAST; move = TRUE; break;
    case KEY_FUNCTION_MOVE_SOUTHWEST: d = SOUTHWEST; move = TRUE; break;
    case KEY_FUNCTION_MOVE_SOUTH: d = SOUTH; move = TRUE; break;
    case KEY_FUNCTION_MOVE_SOUTHEAST: d = SOUTHEAST; move = TRUE; break;

	case KEY_FUNCTION_ENTRENCH:
		if(isMyTurn) g_selected_item->Entrench();
		break;









	case KEY_FUNCTION_UNLOAD_TRANS:
	{
		sint32 order;
		if(!g_theOrderDB->GetNamedItem("ORDER_UNLOAD", order))
			return 0;

		Army a;
		if(g_selected_item->GetSelectedArmy(a))
			g_controlPanel->BeginOrderDelivery(g_theOrderDB->Access(order));
	}
	break;
	case KEY_FUNCTION_SETTLE:
	{
		g_selected_item->Settle();
		move = FALSE;
		break;
	}
	case KEY_FUNCTION_SPACE_LAUNCH:
		if(isMyTurn) {
			sint32 index;
			if(g_theOrderDB->GetNamedItem("ORDER_SPACE_LAUNCH", index)) {
				g_controlPanel->BeginOrderDelivery(g_theOrderDB->Access(index));
			}

			return TRUE;
		}
		break;
	case KEY_FUNCTION_DESCEND:
		g_selected_item->Descend();
		break;
	case KEY_FUNCTION_PILLAGE:
	{
		sint32 order;
		if(!g_theOrderDB->GetNamedItem("ORDER_PILLAGE", order))
			return 0;

		Army a;
		if(g_selected_item->GetSelectedArmy(a) && a.CanPillage()) {
			g_controlPanel->BeginOrderDelivery(g_theOrderDB->Access(order));
		} else {
			g_selected_item->Pillage();
			return 0;
		}
	}
		break;
	case KEY_FUNCTION_BOMBARD:
	{
		sint32 order;
		if(!g_theOrderDB->GetNamedItem("ORDER_BOMBARD", order))
			return 0;

		Army a;
		if(g_selected_item->GetSelectedArmy(a) && a.CanBombard()) {
			g_controlPanel->BeginOrderDelivery(g_theOrderDB->Access(order));
		} else {
			return 0;
		}
	}





		break;
	case KEY_FUNCTION_EXPEL:
	{
		sint32 order;
		if(!g_theOrderDB->GetNamedItem("ORDER_EXPEL", order))
			return 0;

		Army a;
		if(g_selected_item->GetSelectedArmy(a) && a.CanExpel()) {
			g_controlPanel->BeginOrderDelivery(g_theOrderDB->Access(order));
		} else {
			return 0;
		}
	}




		break;
	case KEY_FUNCTION_SLEEP:
		if(isMyTurn) {
			move = FALSE;

			g_selected_item->Sleep();
		}
		break;
    case KEY_FUNCTION_NEXT_ITEM:
        g_selected_item->NextItem();
		move = FALSE;
        break;

	case KEY_FUNCTION_PARADROP:
		if(isMyTurn) {
			MapPoint point;
			g_tiledMap->GetMouseTilePos(point);
			g_selected_item->Paradrop(point);
			return TRUE;
		}
		break;

	case KEY_FUNCTION_INVESTIGATE_CITY:
        {
            MapPoint    point;
            g_tiledMap->GetMouseTilePos(point);
		    g_selected_item->InvestigateCity(point);
        }
		break;

	case KEY_FUNCTION_PLANT_NUKE:
	{
		sint32 order;
		if(!g_theOrderDB->GetNamedItem("ORDER_PLANT_NUKE", order))
			return 0;

		Army a;
		double chance, escape_chance;
		if(g_selected_item->GetSelectedArmy(a) && a.CanPlantNuke(chance, escape_chance)) {
			g_controlPanel->BeginOrderDelivery(g_theOrderDB->Access(order));
		} else {
			return 0;
		}
	}

		break;
	case KEY_FUNCTION_BIOINFECT:
	{
		sint32 order;
		if(!g_theOrderDB->GetNamedItem("ORDER_BIO_INFECT", order))
			return 0;

		Army a;
		double chance;
		if(g_selected_item->GetSelectedArmy(a) && a.CanBioInfect(chance)) {
			g_controlPanel->BeginOrderDelivery(g_theOrderDB->Access(order));
		} else {
			return 0;
		}
	}

		break;
	case KEY_FUNCTION_NANOTERROR:
	{
		sint32 order;
		if(!g_theOrderDB->GetNamedItem("ORDER_NANO_INFECT", order))
			return 0;

		Army a;
		double chance;
		if(g_selected_item->GetSelectedArmy(a) && a.CanNanoInfect(chance)) {
			g_controlPanel->BeginOrderDelivery(g_theOrderDB->Access(order));
		} else {
			return 0;
		}
	}

		break;
	case KEY_FUNCTION_CREATE_PARK:
	{
		sint32 order;
		if(!g_theOrderDB->GetNamedItem("ORDER_CREATE_PARK", order))
			return 0;

		Army a;
		if(g_selected_item->GetSelectedArmy(a) && a.AccessData()->CanCreatePark()) {
			g_controlPanel->BeginOrderDelivery(g_theOrderDB->Access(order));
		} else {
			return 0;
		}
	}

		break;
	case KEY_FUNCTION_REFORM:
	{
		sint32 order;
		if(!g_theOrderDB->GetNamedItem("ORDER_REFORM", order))
			return 0;

		Army a;
		if(g_selected_item->GetSelectedArmy(a) && a.AccessData()->CanReformCity()) {
			g_controlPanel->BeginOrderDelivery(g_theOrderDB->Access(order));
		} else {
			return 0;
		}
	}

		break;

	case KEY_FUNCTION_OPEN_WORK_VIEW:
		ArmyManagerWindow::Toggle();
		break;
	case KEY_FUNCTION_OPEN_CITY_VIEW:
		if(!g_modalWindow) {
			close_AllScreens();
			open_CityView();
		}
		break;
	case KEY_FUNCTION_OPEN_CITY_STATUS:
		if ( !g_modalWindow ) {
			close_AllScreens();
			open_CityStatus();
		}
		break;
	case KEY_FUNCTION_OPEN_CIV_STATUS:
		if ( !g_modalWindow ) {
			close_AllScreens();
			open_CivStatus();
		}
		break;
	case KEY_FUNCTION_OPEN_SCIENCE_STATUS:
		if ( !g_modalWindow ) {
			close_AllScreens();
			open_ScienceStatus();
		}
		break;
	case KEY_FUNCTION_OPEN_UNIT_STATUS:
		if ( !g_modalWindow ) {
			close_AllScreens();
			open_UnitStatus();
		}
		break;
	case KEY_FUNCTION_OPEN_TRADE_STATUS:
		if ( !g_modalWindow ) {
			close_AllScreens();
			TradeManager::Display();
			TradeManager::SetMode(TRADE_MANAGER_MARKET);

		}
		break;
	case KEY_FUNCTION_OPEN_DIPLOMACY:
		if ( !g_modalWindow ) {
			close_AllScreens();
			open_Diplomacy();
		}
		break;
	case KEY_FUNCTION_OPEN_INFO_SCREEN:
		if ( !g_modalWindow ) {
			InfoWindow::SelectScoreTab();
			InfoWindow::Open();
		}
		break;
	case KEY_FUNCTION_OPEN_GREAT_LIBRARY:
		if ( !g_modalWindow ) {
			close_AllScreens();
			open_GreatLibrary();
		}
		break;
	case KEY_FUNCTION_OPEN_OPTIONS_SCREEN:
		if ( !g_modalWindow ) {
			close_AllScreens();
			open_OptionsScreen(1);
		}
		break;

	case KEY_FUNCTION_OPEN_SCENARIO_EDITOR:
		if(!g_modalWindow
		&& !g_turn->IsEmail()
		){
			close_AllScreens();
			optionsscreen_mapeditorPress(NULL, AUI_BUTTON_ACTION_EXECUTE, 0, NULL);
		}
		break;


    case KEY_FUNCTION_ZOOM_IN1:
        if (g_tiledMap) {
			g_tiledMap->ZoomIn();
		}
		break;

    case KEY_FUNCTION_ZOOM_OUT1:
        if (g_tiledMap) {
			g_tiledMap->ZoomOut();
        }

		break;

	case KEY_FUNCTION_CENTER_MAP:
	{
		PLAYER_INDEX s_player;
		ID s_item;
		SELECT_TYPE s_state;
		g_selected_item->GetTopCurItem(s_player, s_item, s_state);

        MapPoint pos;
		switch(s_state) {
			case SELECT_TYPE_LOCAL_ARMY:
			{
				Army army(s_item);
				army.GetPos(pos);
				g_radarMap->CenterMap(pos);
				g_tiledMap->Refresh();
				g_tiledMap->InvalidateMap();
				break;
			}
			case SELECT_TYPE_LOCAL_CITY:
			{
				Unit unit(s_item);
				unit.GetPos(pos);
				g_radarMap->CenterMap(pos);
				g_tiledMap->Refresh();
				g_tiledMap->InvalidateMap();

				break;
			}
		}
	}
	break;

	case KEY_FUNCTION_SAVE_GAME:
		if(g_network.IsActive()) {
			if(g_network.IsHost()) {

				g_isScenario = FALSE;
				loadsavescreen_displayMyWindow(LSS_SAVE_MP);
			}
		} else {

			g_isScenario = FALSE;

			loadsavescreen_displayMyWindow(LSS_SAVE_GAME);
		}
		break;
	case KEY_FUNCTION_LOAD_GAME:
		if(!g_network.IsActive()) {
			loadsavescreen_displayMyWindow(LSS_LOAD_GAME);
		}
		break;
	case KEY_FUNCTION_REMAP_KEYBOARD:
		open_KeyMappingScreen();
		break;

    case KEY_FUNCTION_KEYBOARD_SELECT_UNIT:
         g_selected_item->KeyboardSelectFirstUnit();
		 move = FALSE;
		 break;

#ifdef _PLAYTEST
    case KEY_FUNCTION_RAND_TEST:
       g_is_rand_test=TRUE;
         return TRUE;
		 break;
#endif

#if 0
	case KEY_FUNCTION_ENDSLICE:
		if(g_network.IsActive()) {
			if(g_network.IsMyTurn() ||
			   (g_network.IsHost() &&
				g_player[g_selected_item->GetCurPlayer()]->IsRobot())) {
				g_turn->EndThisSliceBeginNewSlice();
			}
		} else {
			g_turn->EndThisSliceBeginNewSlice();
		}
		move = FALSE;
		break;
#endif
    case KEY_FUNCTION_ENDTURN:
#ifdef _PLAYTEST
        if (g_selected_item->GetCurPlayer() != g_selected_item->GetVisiblePlayer())
           break;

		if(g_network.IsActive()) {
			g_turn->NetworkEndTurn();
		} else {
            g_selected_item->Deselect(g_selected_item->GetCurPlayer());




			NewTurnCount::StartNextPlayer(true);












			g_tiledMap->InvalidateMix();
			g_tiledMap->InvalidateMap();
			g_tiledMap->Refresh();
			g_radarMap->Update();
			g_turn->InformMessages();
		}
		move = FALSE;
        break;
#endif

    case KEY_FUNCTION_NEXT_ROUND:


		if(g_modalMessage) {
			Message *msg = g_modalMessage->GetMessage();
			if(g_theMessagePool->IsValid(*msg)) {
				Assert(msg->IsAlertBox());
				MessageData *data = msg->AccessData();
				if(data->GetNumButtons() <= 2 && data->GetNumButtons() > 0) {
					data->GetButton(0)->Callback();
				}
			}
		} else if(g_utilityTextMessage) {
			g_utilityTextMessage->m_callback(FALSE);
			g_utilityTextMessage->RemoveWindow();
		} else {

			if(g_selected_item->GetVisiblePlayer() == g_selected_item->GetCurPlayer()) {
				DPRINTF(k_DBG_GAMESTATE, ("Keypress end turn, %d\n", g_selected_item->GetCurPlayer()));
				g_selected_item->RegisterManualEndTurn();
				g_gevManager->EndTurnRequest();
			}
			else
			{


			}
		}
        break;

    case KEY_FUNCTION_SAVE_WORLD :
		if (g_civApp->IsGameLoaded() && !g_network.IsClient()) {
			g_civApp->AutoSave(g_selected_item->GetVisiblePlayer(), true);






		}
		move = FALSE ;
		break ;

    case KEY_FUNCTION_LOAD_WORLD :
		if (g_civApp->IsGameLoaded() && !g_network.IsActive()) {
			{
				g_civApp->PostLoadQuickSaveAction(g_selected_item->GetVisiblePlayer());





			}
			move = FALSE ;
		}
		break ;

    case KEY_FUNCTION_QUIT:
		optionwarningscreen_displayMyWindow(OWS_QUIT);

        return 1;
        break;

#ifdef _PLAYTEST
    case KEY_FUNCTION_GAMESTATE_DEBUG:
        g_theWorld->GamestateDebug();
        for (i=0; i<2; i++) {
            g_player[i]->GamestateDebug();
        }
        return TRUE;
		break;
#endif

    case KEY_FUNCTION_GROUP_ARMY:
	{
		Army a;
		if(g_selected_item->GetSelectedArmy(a)) {
			if(g_network.IsClient())
            {
				CellUnitList units;
				Cell *cell = g_theWorld->GetCell(a->RetPos());
				for (sint32 i = 0; i < cell->GetNumUnits(); i++) {
					if(cell->AccessUnit(i).GetArmy().m_id != a.m_id) {
						units.Insert(cell->AccessUnit(i));
					}
				}
				if(units.Num() > 0) {
					g_network.SendGroupRequest(units, a);
				}
			} else {
				g_gevManager->AddEvent(GEV_INSERT_Tail, GEV_GroupOrder,
									   GEA_Army, a.m_id,
									   GEA_End);
			}
		}

		return TRUE;
	}
	break;
	case KEY_FUNCTION_UNGROUP_ARMY:
	{
		Army a;
		if(g_selected_item->GetSelectedArmy(a)) {
			if(g_network.IsClient()) {
				g_network.SendUngroupRequest(a, *a.AccessData());
			} else {
				g_gevManager->AddEvent(GEV_INSERT_Tail, GEV_UngroupOrder,
									   GEA_Army, a.m_id,
									   GEA_End);
			}
		}

		return TRUE;
	}
	break;
	case KEY_FUNCTION_PROCESS_UNIT_ORDERS:
		if(isMyTurn) {
			g_gevManager->Pause();
			for
            (
                sint32 i = 0;
                i < g_player[g_selected_item->GetVisiblePlayer()]->m_all_armies->Num();
                i++
            )
            {
				g_gevManager->AddEvent(GEV_INSERT_Tail, GEV_BeginTurnExecute,
									   GEA_Army, g_player[g_selected_item->GetVisiblePlayer()]->m_all_armies->Access(i),
									   GEA_End);
			}
			g_gevManager->Resume();

		}
		break;
	case KEY_FUNCTION_EXECUTE_ORDERS:
		if(isMyTurn)
		{
			PLAYER_INDEX s_player;
			ID s_item;
			SELECT_TYPE s_state;
			g_selected_item->GetTopCurItem(s_player, s_item, s_state);
			Army army(s_item);
			if (s_state == SELECT_TYPE_LOCAL_ARMY && army.IsValid())
            {
				army.ExecuteOrders();
			}
		}
		break;

#if defined(CTP1_HAS_RISEN_FROM_THE_GRAVE)
	case KEY_FUNCTION_TOGGLE_SPACE:
	{
		if(!g_network.IsActive()) {
			extern sint32 g_fog_toggle;
			g_fog_toggle = !g_fog_toggle;
			void WhackScreen();
			WhackScreen();
		}
		break;
	}
#endif

	case KEY_FUNCTION_TOGGLE_CITY_NAMES:
		{
			uint32 alpha = g_theProfileDB->GetShowCityNameAlpha();
			if (alpha == pixelutils_TRANSPARENT) {
				alpha = pixelutils_OPAQUE;
			} else if (alpha == pixelutils_OPAQUE) {
				alpha -= 64;
			} else if (alpha < 128) {
				alpha = pixelutils_TRANSPARENT;
			} else {
				alpha -= 32;
			}
			g_theProfileDB->SetShowCityNameAlpha(alpha);
		}
		break;

	case KEY_FUNCTION_TOGGLE_TRADE_ROUTES:
		g_theProfileDB->SetShowTradeRoutes(!g_theProfileDB->GetShowTradeRoutes());
		break;
//#if 0
//	case KEY_FUNCTION_TOGGLE_ARMY_NAMES: //emod
//		g_theProfileDB->SetShowArmyNames(!g_theProfileDB->GetShowArmyNames());
//		break;
//#endif

#ifdef _DEBUG
#if 0
	case KEY_FUNCTION_CRC:
		if(g_debugOwner != k_DEBUG_OWNER_CRC) {
            if (!g_dataCheck) {
                g_dataCheck = new DataCheck();
            }
			g_debugOwner = k_DEBUG_OWNER_CRC;
			g_dataCheck->BeginTurn();
			g_dataCheck->SetDisplay(1);
		} else {
			g_debugOwner = k_DEBUG_OWNER_NONE;
		}
		break;
#endif
#endif
	case KEY_FUNCTION_HELP_MODE_TOGGLE:
		break;

	case KEY_FUNCTION_CHAT_KEY:
		if (g_chatBox)
        {
			g_chatBox->SetActive(!g_chatBox->IsActive());
		}
		break;

	case KEY_FUNCTION_UNIT_CITY_TOGGLE:
		g_selected_item->UnitCityToggle();
		break;
	case KEY_FUNCTION_END_UNIT_TURN:
		g_selected_item->EndUnitTurn();
		break;

	case KEY_FUNCTION_NETWORK_PLAYERS_SCREEN:
		if ( !g_networkPlayersScreen ) {
			g_networkPlayersScreen = new c3_UtilityPlayerListPopup((c3_UtilityPlayerListCallback *)network_PlayerListCallback);
		}
		g_networkPlayersScreen->DisplayWindow();
		if ( g_network.IsHost() ) {
			g_networkPlayersScreen->EnableButtons();
		}
		else if ( g_network.IsClient() ) {
			g_networkPlayersScreen->DisableButtons();
		}

		break;
	case KEY_FUNCTION_CIV_TAB:
		if(g_controlPanel) {
			g_controlPanel->SetTab(CP_TAB_CIV);
		}
		break;
	case KEY_FUNCTION_MSG_TAB:
		if(g_controlPanel) {
			g_controlPanel->SetTab(CP_TAB_MSGLOG);
		}
		break;
	case KEY_FUNCTION_CITY_TAB:
		if(g_controlPanel) {
			g_controlPanel->SetTab(CP_TAB_CITY);
		}
		break;
	case KEY_FUNCTION_UNIT_TAB:
		if(g_controlPanel) {
			g_controlPanel->SetTab(CP_TAB_UNIT);
		}
		break;
	case KEY_FUNCTION_TILE_TAB:
		if(g_controlPanel) {
			g_controlPanel->SetTab(CP_TAB_TILEIMP);
		}
		break;

	case KEY_FUNCTION_CONTROL_NEXT:
#if 0
		if(g_controlPanel) {

			if(g_cp_productionTab &&
			   (g_cp_productionTab->GetMode() == PRODTAB_MODE_CHANGE ||
				g_cp_productionTab->GetMode() == PRODTAB_MODE_QUEUE)) {
				c3_ListBox *plist = g_cp_productionTab->GetPurchaseList();
				if(plist && plist->NumItems() > 0) {
					sint32 curIndex = plist->GetSelectedItemIndex();
					if(curIndex >= plist->NumItems() - 1) {
						curIndex = -1;
					}
					plist->SelectItem(curIndex + 1);
					plist->GetVerticalRanger()->SetValue(0, curIndex + 1 - 2);
				}
			}
		}
#endif
		break;
	case KEY_FUNCTION_CONTROL_PREV:
#if 0
		if(g_controlPanel) {

			if(g_cp_productionTab &&
			   (g_cp_productionTab->GetMode() == PRODTAB_MODE_CHANGE ||
				g_cp_productionTab->GetMode() == PRODTAB_MODE_QUEUE)) {
				c3_ListBox *plist = g_cp_productionTab->GetPurchaseList();
				if(plist && plist->NumItems() > 0) {
					sint32 curIndex = plist->GetSelectedItemIndex();
					if(curIndex <= 0) {
						curIndex = plist->NumItems();
					}
					plist->SelectItem(curIndex - 1);
					plist->GetVerticalRanger()->SetValue(0, curIndex - 1 - 2);
				}
			}
		}
#endif
		break;
	case KEY_FUNCTION_CLOSE:
		if(g_currentMessageWindow) {
			if(g_theMessagePool->IsValid(*g_currentMessageWindow->GetMessage())) {
				g_currentMessageWindow->GetMessage()->Kill();
			}
		} else if(g_modalMessage) {
			Message *msg = g_modalMessage->GetMessage();
			if(g_theMessagePool->IsValid(*msg)) {
				Assert(msg->IsAlertBox());
				MessageData *data = msg->AccessData();
				if(data->GetNumButtons() <= 2 && data->GetNumButtons() > 0) {
					data->GetButton(0)->Callback();
				}
			}
		}

		break;
	case KEY_FUNCTION_YES:
	{
		if(g_modalMessage) {
			Message *msg = g_modalMessage->GetMessage();
			if(g_theMessagePool->IsValid(*msg)) {
				Assert(msg->IsAlertBox());
				MessageData *data = msg->AccessData();
				if(data->GetNumButtons() == 2) {
					data->GetButton(1)->Callback();
				}
			}
		} else if(g_utilityTextMessage) {
			g_utilityTextMessage->m_callback(TRUE);
			g_utilityTextMessage->RemoveWindow();
		}
		break;
	}
	case KEY_FUNCTION_NO:
	{
		if(g_modalMessage) {
			Message *msg = g_modalMessage->GetMessage();
			if(g_theMessagePool->IsValid(*msg)) {
				Assert(msg->IsAlertBox());
				MessageData *data = msg->AccessData();
				if(data->GetNumButtons() == 2) {
					data->GetButton(0)->Callback();
				}
			}
		} else if(g_utilityTextMessage) {
			g_utilityTextMessage->m_callback(FALSE);
			g_utilityTextMessage->RemoveWindow();
		}
		break;
	}
	case KEY_FUNCTION_CLEAR_QUEUE:
		if(g_controlPanel

			) {
			Unit city;
			if(g_selected_item->GetSelectedCity(city)) {
				city.GetData()->GetCityData()->GetBuildQueue()->Clear();
#if 0
				if(g_cp_productionTab) {
					g_cp_productionTab->LoadCityData(city);
					g_cp_productionTab->FillQueueList(city);
				}
#endif
			}
		}
		break;
	case KEY_FUNCTION_OPEN_MESSAGE:
		{

			extern sint32 g_modalWindow;
			if (g_modalWindow == 0) {
				if(g_currentMessageWindow) {
					if(g_currentMessageWindow->GetMinimizeAction()) {
						g_currentMessageWindow->GetMinimizeAction()->Execute(NULL,
																		   AUI_BUTTON_ACTION_EXECUTE,
																		   0);
					}
				} else if(g_selected_item) {
					sint32 visPlayer = g_selected_item->GetVisiblePlayer();

					if(g_player[visPlayer] && g_player[visPlayer]->m_messages->Num() > 0) {
						sint32 m;
						for(m = 0; m < g_player[visPlayer]->m_messages->Num(); m++) {
							if(!g_player[visPlayer]->m_messages->Access(m).IsRead()) {
								g_player[visPlayer]->m_messages->Access(m).Show();
								break;
							}
						}
					}
				}
			}
		}
		break;
	case KEY_FUNCTION_EXECUTE_EYEPOINT:
		{

			if(g_currentMessageWindow) {
				if(g_theMessagePool->IsValid(*g_currentMessageWindow->GetMessage())) {
					g_currentMessageWindow->GetMessage()->AccessData()->EyePointCallback(0);
				}
			}
			break;
		}
	case KEY_FUNCTION_MOVE_ORDER:
		g_controlPanel->BeginOrderDelivery(g_selected_item->GetMoveOrder());
		break;

	case KEY_FUNCTION_TOGGLE_RADAR:
		radarwindow_Toggle();
		break;

	case KEY_FUNCTION_TOGGLE_CONTROL_PANEL:
		g_controlPanel->Toggle();
		break;

	case KEY_FUNCTION_TOGGLE_ALL:
		if(!g_c3ui->GetWindow(g_controlPanel->GetWindow()->Id())) {
			g_c3ui->AddWindow(g_controlPanel->GetWindow());
			radarwindow_Show();
		} else {
			g_c3ui->RemoveWindow(g_controlPanel->GetWindow()->Id());
			radarwindow_Hide();
		}
		break;

	case KEY_FUNCTION_TRADE_SUMMARY:
		if (!g_modalWindow) {
			close_AllScreens();
			TradeManager::Display();
			TradeManager::SetMode(TRADE_MANAGER_SUMMARY);
		}
		break;

	case KEY_FUNCTION_GAIA:
		if(!g_modalWindow) {
			close_AllScreens();
			ScienceVictoryDialog::Open();
		}
		break;

	case KEY_FUNCTION_BUILD_QUEUE:
	{
		if(!g_modalWindow && g_player[g_selected_item->GetVisiblePlayer()]) {
			Unit city;
			if(g_selected_item->GetSelectedCity(city)) {
				city = g_theWorld->GetCity(g_selected_item->GetCurSelectPos());
			} else if(g_player[g_selected_item->GetVisiblePlayer()]->GetNumCities()) {
				city = g_player[g_selected_item->GetVisiblePlayer()]->m_all_cities->Access(0);
			}
				if(city.IsValid()) {
					close_AllScreens();
					EditQueue::Display(city);
				}
		}
		break;
	}
	case KEY_FUNCTION_CITY_MANAGEMENT:
		if(!g_modalWindow && g_player[g_selected_item->GetVisiblePlayer()]) {
			close_AllScreens();
			Unit city;
			if(g_selected_item->GetSelectedCity(city)) {
				CityWindow::Display(city);
			} else if(g_player[g_selected_item->GetVisiblePlayer()]->GetNumCities()) {
				city = g_player[g_selected_item->GetVisiblePlayer()]->m_all_cities->Access(0);
				g_selected_item->SetSelectCity(city);
				CityWindow::Display(city);
			}
		}
		break;

	case KEY_FUNCTION_NEW_PROPOSAL:
		if(!g_modalWindow) {
			if(DipWizard::CanInitiateRightNow()) {
				DipWizard::Display();
			}
		}
		break;

	case KEY_FUNCTION_TIMELINE:
		if(!g_modalWindow) {
			InfoWindow::SelectWonderTab();
			InfoWindow::Open();
		}
		break;

	case KEY_FUNCTION_RANK:
		if(!g_modalWindow) {
			InfoWindow::SelectRankingTab();
			InfoWindow::Open();
		}
		break;

	case KEY_FUNCTION_RESTART:
		//Added by Martin Gühmann to disable also the restart key in network
		//games, hot seat games and email games.
		if(!g_modalWindow
		&& !g_theProfileDB->IsScenario()
		&& !g_isScenario
		&& !g_network.IsActive()
		&& !g_turn->IsHotSeat()
		&& !g_turn->IsEmail()
		) {
			optionwarningscreen_displayMyWindow(OWS_RESTART) ;
		}
		break;

	case KEY_FUNCTION_NEW_GAME:
		if(!g_modalWindow) {

			optionwarningscreen_displayMyWindow(OWS_QUITTOSHELL);
		}
		break;

	case KEY_FUNCTION_SOUND_OPTIONS:
		if(!g_modalWindow) {
			soundscreen_displayMyWindow();
		}
		break;

	// MUSIC added by ahenobarb
	case KEY_FUNCTION_MUSIC_OPTIONS:
		if(!g_modalWindow) {
			musicscreen_displayMyWindow();
		}
		break;

	case KEY_FUNCTION_GRAPHICS_OPTIONS:
		if(!g_modalWindow) {
			graphicsscreen_displayMyWindow();
		}
		break;

	case KEY_FUNCTION_GAMEPLAY_OPTIONS:
		if(!g_modalWindow) {
			gameplayoptions_displayMyWindow();
		}
		break;
	case KEY_FUNCTION_ADVANCED_OPTIONS:
		if(!g_modalWindow) {
			ProfileEdit::Display();
		}
		break;
	default :
        Assert(FALSE);
    }

    if (move && isMyTurn) {

		PLAYER_INDEX s_player;
		ID s_item;
		SELECT_TYPE s_state;

		g_selected_item->GetTopCurItem(s_player, s_item, s_state);

		switch(s_state) {
			case SELECT_TYPE_LOCAL_ARMY:
			{
				Army army(s_item);
				MapPoint pos;
				army.GetPos(pos);
				MapPoint newpos;

				if(pos.GetNeighborPosition(d, newpos)) {
					g_gevManager->AddEvent(GEV_INSERT_Tail, GEV_MoveToOrder,
										   GEA_Army, army.m_id,
										   GEA_Direction, d,
										   GEA_End);

					g_selected_item->DidKeyboardMove();
				}
				break;
			}

			default:
				break;
		}
	}

	return TRUE;
}

void WhackScreen()
{
	g_tiledMap->Refresh();
	g_tiledMap->InvalidateMap();
	g_radarMap->Update();
}

bool keypress_IsGameFunction(KEY_FUNCTION kf)
{
	switch(kf) {
		case KEY_FUNCTION_MOVE_NORTH:
		case KEY_FUNCTION_MOVE_NORTHWEST:
		case KEY_FUNCTION_MOVE_NORTHEAST:
		case KEY_FUNCTION_MOVE_WEST:
		case KEY_FUNCTION_MOVE_EAST:
		case KEY_FUNCTION_MOVE_SOUTHWEST:
		case KEY_FUNCTION_MOVE_SOUTH:
		case KEY_FUNCTION_MOVE_SOUTHEAST:
		case KEY_FUNCTION_MOVE_UP_OR_DOWN:
		case KEY_FUNCTION_EXECUTE_ORDERS:
		case KEY_FUNCTION_GROUP_ARMY:
		case KEY_FUNCTION_UNGROUP_ARMY:
		case KEY_FUNCTION_ENTRENCH:
		case KEY_FUNCTION_UNLOAD_TRANS:
		case KEY_FUNCTION_SETTLE:
		case KEY_FUNCTION_SPACE_LAUNCH:
		case KEY_FUNCTION_DESCEND:
		case KEY_FUNCTION_PILLAGE:
		case KEY_FUNCTION_BOMBARD:
		case KEY_FUNCTION_EXPEL:
		case KEY_FUNCTION_SLEEP:
		case KEY_FUNCTION_END_UNIT_TURN:
		case KEY_FUNCTION_NEXT_ROUND:
		case KEY_FUNCTION_ENDTURN:
		case KEY_FUNCTION_PARADROP:
		case KEY_FUNCTION_INVESTIGATE_CITY:
		case KEY_FUNCTION_PLANT_NUKE:
		case KEY_FUNCTION_BIOINFECT:
		case KEY_FUNCTION_NANOTERROR:
		case KEY_FUNCTION_CREATE_PARK:
		case KEY_FUNCTION_REFORM:
		case KEY_FUNCTION_CENTER_MAP:
		case KEY_FUNCTION_PROCESS_UNIT_ORDERS:
		case KEY_FUNCTION_MOVE_ORDER:
		case KEY_FUNCTION_TOGGLE_RADAR:
		case KEY_FUNCTION_TOGGLE_CONTROL_PANEL:
		case KEY_FUNCTION_TOGGLE_ALL:
		case KEY_FUNCTION_LOAD_WORLD:
		case KEY_FUNCTION_SAVE_WORLD:
		case KEY_FUNCTION_ZOOM_IN1:
		case KEY_FUNCTION_ZOOM_OUT1:
			return true;
		default:
			return false;
	}
}
