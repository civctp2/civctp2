//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : Control panel handling
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
// _DEBUG
// - Set when generating the debug version.
//
//----------------------------------------------------------------------------
//
// Modifications from the original Activision code:
//
// - Music added by ahenobarb.
// - Added option to show info of tile improvements that are too expensive
//   and made it modifiable in-game.
// - Do not display the world map of the first player at start-up for hotseat
//   play.
// - Start the great library with the current research project of the player.
// - Added option to add new messages at the top.
// - Fixed crossed Sword bug. - Oct. 14th 2004 Martin Gühmann
// - Fixed the crossed sword bug that was caused by the previous bug fix,
//   cossed swords even if the city is hidden. - Oct. 15th 2004 Martin Gühmann
// - Added unit display name.
// - Relaxed Assert for invisible buttons with mods.
// - Prevented crashes with mods.
// - Added special attack window. (Aug 15th 2005 Martin Gühmann)
// - Removed unused methods: FillBank, ClearButtons and AddButton.
//   (Aug 16th 2005 Martin Gühmann)
// - Restored Fifth Tileimp Button Bank by E 2-27-2007
// - TODO fix obsolete tileimp defect
// - TODO add buttons for orders button bank
// - TODO create scroll bar like CityStyles in Scenario Editor for
//   Orders and tileimprovements so they are not limited
// - Replaced old const database by new one. (5-Aug-2007 Martin Gühmann)
// - Replaced old const database by new one. (5-Aug-2007 Martin Gühmann)
//
//----------------------------------------------------------------------------
//
// Code remarks:
//
// - Some of the functionality in this file seems to be duplicated in or from
//   MainControlPanel. Actually, some functions in this file (e.g. InitCityTab,
//   CityPanelRebuild - notice the strange immediate return) are never called,
//   and some variables (e.g. m_mainDropDown) are not NULL-initialised in the
//   constructor. Maybe this is some leftover of the CTP1 code?
// - Initialized local variables. (Sep 9th 2005 Martin Gühmann)
// - Standatized code (May 21st 2006 Martin Gühmann)
// - None of the unit or order stuff is used here? see UnitControlPanel.cpp
//
//----------------------------------------------------------------------------

#include "c3.h"
#include "controlpanelwindow.h"

#include <inttypes.h>

#include <algorithm>                    // std::fill
#include "aui.h"
#include "aui_uniqueid.h"
#include "aui_stringtable.h"
#include "aui_tab.h"
#include "aui_ldl.h"
#include "aui_button.h"
#include "aui_ranger.h"

#include "pattern.h"
#include "pointerlist.h"
#include "primitives.h"

#include "pixelutils.h"
#include "maputils.h"
#include "terrainutil.h"

#include "c3ui.h"

#include "citywindow.h"
#include "background.h"
#include "tiledmap.h"
#include "radarwindow.h"
#include "InfoBar.h"
#include "network.h"
#include "OrderRecord.h"

#include "newturncount.h"

#include "gameplayoptions.h"
#include "graphicsscreen.h"
#include "soundscreen.h"
// music added by ahenobarb
#include "musicscreen.h"
#include "optionwarningscreen.h"
#include "loadsavewindow.h"
#include "km_screen.h"
#include "optionswindow.h"
#include "infowindow.h"
#include "MessagePool.h"
#include "message.h"

#include "cursormanager.h"
#include "debugwindow.h"

#include "c3cmdline.h"
#include "SelItem.h"                    // g_selected_item
#include "player.h"                     // g_player
#include "c3window.h"
#include "ctp2_Window.h"
#include "ctp2_Menu.h"
#include "ctp2_Static.h"
#include "ctp2_Switch.h"

#include "ctp2_menubar.h"
#include "ctp2_Window.h"
#include "ctp2_button.h"
#include "ctp2_listitem.h"
#include "ctp2_listbox.h"
#include "ctp2_dropdown.h"

#include "c3_static.h"

#include "IconRecord.h"
#include "SelItem.h"
#include "TurnCnt.h"
#include "UnitPool.h"                   // g_theUnitPool
#include "Unit.h"
#include "UnitData.h"
#include "UnitRecord.h"
#include "StrDB.h"                      // g_theStringDB
#include "screenutils.h"
#include "Advances.h"
#include "AdvanceRecord.h"
#include "advanceutil.h"
#include "ArmyData.h"
#include "cellunitlist.h"
#include "Cell.h"
#include "World.h"                      // g_theWorld
#include "director.h"                   // g_director
#include "Events.h"
#include "GameEventUser.h"
#include "TerrainRecord.h"

#include "pixelutils.h"
#include "colorset.h"                   // g_colorSet
#include "primitives.h"

#include "trademanager.h"

#include "MainControlPanel.h"
#include "EditQueue.h"
#include "diplomacywindow.h"
#include "dipwizard.h"
#include "cityespionage.h"

#include "civ3_main.h"

#include "ctp2_Tab.h"
#include "ctp2_TabButton.h"
#include "ctp2_TabGroup.h"

#include "armymanagerwindow.h"

#include "ProfileEdit.h"
#include "c3_utilitydialogbox.h"
#include "NationalManagementDialog.h"
#include "MessageBoxDialog.h"

#include "tileimptracker.h"

#include "filenamedb.h"
#include "messageiconwindow.h"
#include "scenarioeditor.h"

#include "progresswindow.h"
#include "aui_progressbar.h"
#include "aui_tipwindow.h"

#include "Order.h"

#include "soundmanager.h"
#include "gamesounds.h"

#include "km_screen.h"
#include "keymap.h"

#include "ConstRecord.h"

#include "profileDB.h"
#include "helptile.h"

#include "SpecialAttackWindow.h"
#include "gameinit.h"                   // g_startHotseatGame
#include <string>                       // std::string

#include "chatbox.h"
extern ChatBox        *g_chatBox;

extern ProgressWindow *g_theProgressWindow;

#define k_UNIT_CONTEXT_ARMY_MANAGER 10000
#define k_UNIT_CONTEXT_CLEAR_ORDERS 10001
#define k_UNIT_CONTEXT_GROUP_ALL 10002
#define k_UNIT_CONTEXT_UNGROUP_ALL 10003
#define k_UNIT_CONTEXT_TRANSPORT_VIEW 10004

#define k_CONTEXT_CITY_VIEW 20000
#define k_CONTEXT_CITY_BUILD 20001
#define k_CONTEXT_CITY_RENAME 20002
#define k_CONTEXT_CITY_DISBAND 20004
#define k_CONTEXT_CITY_ACTIVATE_UNITS 20005
#define k_CONTEXT_CITY_TOGGLE_GOVERNOR 20006

#define k_CONTEXT_MISC_TILE_INFO 30000

extern sint32               g_ScreenWidth;
extern sint32               g_ScreenHeight;
extern C3UI                 *g_c3ui;
extern InfoBar              *g_infoBar;
extern CursorManager        *g_cursorManager;
extern KEYMAP               *theKeyMap;

ctp2_MenuBar                *s_menubar=NULL;
ControlPanelWindow          *g_controlPanel;

extern Network              g_network;

extern FilenameDB           *g_theMessageIconFileDB;

void CityManagerButtonCallback(aui_Control *control, uint32 action, uint32 data, void *cookie);

void TabGroupButtonCallback   (aui_Control *control, uint32 action, uint32 data, void *cookie);
void TileImpSelectionCallback (aui_Control *control, uint32 action, uint32 data, void *cookie);

void CityPanelDropDownCallback(aui_Control *control, uint32 action, uint32 data, void *cookie);
void CityPanelNextCityCallback(aui_Control *control, uint32 action, uint32 data, void *cookie);

void UnitPanelNextUnitCallback(aui_Control *control, uint32 action, uint32 data, void *cookie);
void UnitPanelListBoxCallback (aui_Control *control, uint32 action, uint32 data, void *cookie);

#define k_CPW_THOUSAND  1000
#define k_CPW_MILLION   1000000
#define k_CPW_BILLION   1000000000
#ifdef _MSC_VER
#define k_CPW_TRILLION  1000000000000
#elif defined(__GNUC__)
#define k_CPW_TRILLION  1000000000000ULL
#endif

void TurnNextUnitButtonActionCallback( aui_Control *control, uint32 action, uint32 data, void *cookie )
{
	if ( action != (uint32)AUI_BUTTON_ACTION_EXECUTE ) return;

	g_selected_item->NextUnmovedUnit();
}

void TurnNextCityButtonActionCallback( aui_Control *control, uint32 action, uint32 data, void *cookie )
{
	if ( action != (uint32)AUI_BUTTON_ACTION_EXECUTE ) return;
	if ( g_selected_item->GetState() == SELECT_TYPE_LOCAL_CITY )
	{
		g_selected_item->NextItem();
	}
	else
	{
		if (!g_player[g_selected_item->GetVisiblePlayer()] ||
			!g_player[g_selected_item->GetVisiblePlayer()]->GetNumCities())
			return;

		Unit city = g_player[g_selected_item->GetVisiblePlayer()]->GetCityFromIndex(0);
		g_selected_item->SetSelectUnit(city);
		if(g_selected_item->IsAutoCenterOn())
		{
			MapPoint pos;
			city.GetPos(pos);
			if(!g_tiledMap->TileIsVisible(pos.x, pos.y))
				g_director->AddCenterMap(pos);
		}
	}

	controlpanelwindow_Update();
}

void CityManagerButtonCallback(aui_Control *control, uint32 action, uint32 data, void *cookie)
{
	if(action != (uint32)AUI_BUTTON_ACTION_EXECUTE) return;

	CityWindow::Initialize();

	CityData *	selection	= NULL;
	Unit 		city;
	Army		a;

	if (g_selected_item->GetSelectedCity(city))
	{
		selection = city.CD();
	}
	else if (g_selected_item->GetSelectedArmy(a) &&
	         g_theWorld->HasCity(a->RetPos())
	        )
	{
		selection = g_theWorld->GetCity(a->RetPos()).CD();
	}

	CityWindow::Display(selection);
}

void TabGroupButtonCallback(aui_Control *control, uint32 action, uint32 data, void *cookie)
{
}

void CityPanelDropDownCallback( aui_Control *control, uint32 action, uint32 data, void *cookie )
{
	if ( action != (uint32)AUI_DROPDOWN_ACTION_SELECT )
		return;

	g_controlPanel->CityPanelRedisplay();
}

void CityPanelNextCityCallback( aui_Control *control, uint32 action, uint32 data, void *cookie )
{
	if ( action != (uint32)AUI_BUTTON_ACTION_EXECUTE )
		return;

	g_controlPanel->CityPanelNextCity();
}

void UnitPanelNextUnitCallback( aui_Control *control, uint32 action, uint32 data, void *cookie )
{
	if ( action != (uint32)AUI_BUTTON_ACTION_EXECUTE )
		return;

	TurnNextUnitButtonActionCallback(control,action,data,cookie);

	g_controlPanel->UnitPanelRedisplay();
}

void UnitPanelListBoxCallback ( aui_Control *control, uint32 action, uint32 data, void *cookie )
{
	if ( action != (uint32)AUI_LISTBOX_ACTION_SELECT)
		return;

	g_controlPanel->BeginOrderDelivery();
	g_controlPanel->UnitPanelRedisplay();
}

void TileImpSelectionCallback(aui_Control *control, uint32 action, uint32 data, void *cookie)
{
	if (action != (uint32)AUI_SWITCH_ACTION_ON)
		return;

	uint32 index=(uint32)cookie;

	if (index>=CP_TILEIMP_MAX)
		return;

	g_controlPanel->ActivateTileImpBank(index);
}

void TerraformToggleCallback(aui_Control *control, uint32 action, uint32 data, void *cookie)
{
	if ( action != (uint32)AUI_BUTTON_ACTION_EXECUTE )
		return;
}

void TileImpButtonCallback2(aui_Control *control, uint32 action, uint32 data, void *cookie)  //emod1 tileimps
{
	if ( action != (uint32)AUI_BUTTON_ACTION_EXECUTE ) return;

	const TerrainImprovementRecord *rec =g_theTerrainImprovementDB->Get(sint32(cookie));

	if (rec==NULL)
		return;

	g_controlPanel->BeginImprovementCycle((void *)rec);
}

void GotoCityUtilityDialogBoxCallback(Unit city, sint32 val2)
{
	if (!val2) return;

	if (city.IsValid())
	{
	    MapPoint        destPos = city.RetPos();
	    g_selected_item->Goto(destPos);
	}
}

void controlpanelwindow_MessageListCallback(aui_Control *control, uint32 action, uint32 data, void *cookie)
{
	if ((action != AUI_LISTBOX_ACTION_SELECT) &&
	    (action != AUI_LISTBOX_ACTION_RMOUSESELECT)
	     )
	{
		return;
	}

	ctp2_ListBox *	l_List  = reinterpret_cast<ctp2_ListBox *>(control);
	ctp2_ListItem *	item    = static_cast<ctp2_ListItem *>
	                            (l_List ? l_List->GetSelectedItem() : NULL);

	if (item)
	{
		Message theMessage((uint32)item->GetCookie());

		if (g_theMessagePool->IsValid(theMessage))
		{
			if (action == AUI_LISTBOX_ACTION_SELECT)
			{
				theMessage.Show();

				ctp2_Static *staticContainer = (ctp2_Static *)item->GetChildByIndex(0);
				ctp2_Static *staticThing = (ctp2_Static *)staticContainer->GetChildByIndex(1);
				staticThing->SetTextColor(g_colorSet->GetColorRef(COLOR_GRAY));
			}
			else // action == AUI_LISTBOX_ACTION_RMOUSESELECT
			{
				theMessage.Kill();
			}

			ctp2_Static *   tabLabel = reinterpret_cast<ctp2_Static *>
				(aui_Ldl::GetObject("ControlPanelWindow.ControlPanel.ControlTabPanel.MessageTab.TabButton.Label"));

			tabLabel->SetTextColor(g_colorSet->GetColorRef(COLOR_BUTTON_TEXT_PLAIN));
			tabLabel->ShouldDraw(TRUE);
		}

		l_List->DeselectItem((aui_Item *)item);
	}
}

AUI_ERRCODE controlpanelwindow_HappinessDrawCallback(ctp2_Static *control,
                                                     aui_Surface *surface,
                                                     RECT &rect,
                                                     void *cookie)
{
	if (g_controlPanel)
		g_controlPanel->HappinessRedisplay(surface,rect,cookie);

	return AUI_ERRCODE_OK;
}

sint32 controlpanelwindow_Initialize()
{
	if (!g_controlPanel)
	{
		MBCHAR			windowBlock[ k_AUI_LDL_MAXBLOCK + 1 ];
		strcpy(windowBlock, "ControlPanelWindow");

		AUI_ERRCODE		errcode = AUI_ERRCODE_OK;
		g_controlPanel = new ControlPanelWindow(&errcode, aui_UniqueId(), windowBlock, 16 );
		Assert( AUI_NEWOK(g_controlPanel, errcode) );
		if ( !AUI_NEWOK(g_controlPanel, errcode) ) return -1;

		g_controlPanel->ActivateTileImpBank(CP_TILEIMP_LAND);
	}

	return 0;
}

sint32 controlpanelwindow_InitializeHats()
{
	return 0;
}

void controlpanelwindow_Cleanup(void)
{
	CityWindow::Cleanup();
	tileimptracker_Cleanup();
	specialAttackWindow_Cleanup();

	delete g_controlPanel;
	g_controlPanel = NULL;
}

void HideElement(aui_Region *element)
{
	if (element!=NULL)
		element->Hide();
}

void HideControlPanel()
{
	HideElement((aui_Window*)g_controlPanel->GetWindow());
}

void controlpanelwindow_DisbandCity(bool response, void *userData)
{
	if(response)
	{
		Unit city(reinterpret_cast<uint32>(userData));
		if(city.IsValid())
		{
			g_gevManager->AddEvent(GEV_INSERT_Tail, GEV_DisbandCity, GEA_City, city, GEA_End);
		}
	}
}

void ContextMenuCallback(ctp2_Menu *menu, CTP2_MENU_ACTION action, sint32 itemIndex, void *cookie)
{
	if (action!= (uint32)CTP2_MENU_ACTION_SELECT)
		return;

	Unit city;
	bool haveCity = g_selected_item->GetSelectedCity(city) != FALSE;

	if(!haveCity)
	{
		city = g_theWorld->GetCity(g_selected_item->GetCurSelectPos());
		if(city.IsValid())
		{
			haveCity = true;
		}
		else if(g_player[g_selected_item->GetVisiblePlayer()]->GetNumCities())
		{
			city = g_player[g_selected_item->GetVisiblePlayer()]->m_all_cities->Access(0);

			if(city.IsValid())
				haveCity = true;
		}
	}

	switch((sint32)cookie)
	{
		case k_CONTEXT_CITY_VIEW:
			if(haveCity) CityWindow::Display(city.CD());
			break;
		case k_CONTEXT_CITY_BUILD:
			if(g_network.IsClient() && g_network.GetSensitiveUIBlocked())
			{
			}
			else
			{
				EditQueue::Display(CityWindow::GetCityData(city));
			}
			break;
		case k_CONTEXT_CITY_RENAME:
			c3_utilitydialogbox_NameCity(city);
			break;
		case k_CONTEXT_CITY_DISBAND:
			{
				if(city->PopCount() > g_theConstDB->Get(0)->GetMaxDisbandSize())
				{
					MessageBoxDialog::Information("str_ldl_CantDisbandCitySize", "InfoCantDisCitySz");
				}
				else if(g_player[city->GetOwner()]->m_all_cities->Num() < 2)
				{
					MessageBoxDialog::Information("str_ldl_CantDisbandLastCity", "InfoCantDisLastCity");
				}
				else
				{
					MessageBoxDialog::Query("str_ldl_ND_VERIFY_DISBAND", "VerifyDisbandCity", controlpanelwindow_DisbandCity, (void *)city.m_id);
				}
				break;
			}
		case k_CONTEXT_CITY_ACTIVATE_UNITS:
			g_gevManager->AddEvent(GEV_INSERT_Tail, GEV_ActivateAllUnits,
			                       GEA_MapPoint, city.RetPos(),
			                       GEA_End);
			break;
		case k_CONTEXT_CITY_TOGGLE_GOVERNOR:
			if(haveCity) city.CD()->SetUseGovernor(!city.CD()->GetUseGovernor());
			break;
		case k_CONTEXT_MISC_TILE_INFO:
			helptile_displayData(g_selected_item->GetCurSelectPos());
			break;
		default:
		{
			Army a;
			if(!g_selected_item->GetSelectedArmy(a))
			{
				MapPoint pos = g_selected_item->GetCurSelectPos();
				Cell *cell = g_theWorld->GetCell(pos);
				if(cell->UnitArmy())
				{
					g_selected_item->SetSelectUnit(cell->UnitArmy()->GetTopVisibleUnit(g_selected_item->GetVisiblePlayer()));
					g_selected_item->GetSelectedArmy(a);
				}
			}

			sint32 orderIndex = (sint32)cookie;
			if(orderIndex >= 0 && orderIndex < g_theOrderDB->NumRecords())
			{
				const OrderRecord *order=g_theOrderDB->Get(orderIndex);
				g_controlPanel->BeginOrderDelivery((OrderRecord *)order);
			}
			else
			{
				switch(orderIndex)
				{
					case k_UNIT_CONTEXT_ARMY_MANAGER:
						ArmyManagerWindow::Display();
						break;
					case k_UNIT_CONTEXT_CLEAR_ORDERS:
					{
						if(a.IsValid())
						{
							g_gevManager->AddEvent(GEV_INSERT_Tail, GEV_ClearOrders,
												   GEA_Army, a.m_id,
												   GEA_End);
						}
					}
					break;
					case k_UNIT_CONTEXT_GROUP_ALL:
					{
						if(a.IsValid()) {
							if(g_network.IsClient())
							{
								CellUnitList units;
								Cell *cell = g_theWorld->GetCell(a->RetPos());
								for(sint32 i = 0; i < cell->GetNumUnits(); i++)
								{
									if(cell->AccessUnit(i).GetArmy().m_id != a.m_id)
									{
										units.Insert(cell->AccessUnit(i));
									}
								}
								if(units.Num() > 0)
								{
									g_network.SendGroupRequest(units, a);
								}
							} else
							{
								g_gevManager->AddEvent(GEV_INSERT_Tail, GEV_GroupOrder,
								                       GEA_Army, a.m_id,
								                       GEA_End);
							}
						}
					}
					break;
					case k_UNIT_CONTEXT_UNGROUP_ALL:
					{
						if(a.IsValid())
						{
							if(g_network.IsClient())
							{
								g_network.SendUngroupRequest(a, *a.AccessData());
							}
							else
							{
								g_gevManager->AddEvent(GEV_INSERT_Tail, GEV_UngroupOrder,
								                       GEA_Army, a.m_id,
								                       GEA_End);
							}
						}
					}
					break;
					case k_UNIT_CONTEXT_TRANSPORT_VIEW:
					{
						if(a.IsValid())
						{
							g_controlPanel->SetTab(CP_TAB_UNIT);
							MainControlPanel::SwitchToTransportView();
						}
					}
					break;
				}
			}
		}
	}
}

void CivMenuCallback(ctp2_Menu *menu, CTP2_MENU_ACTION action, sint32 itemIndex, void *cookie)
{

	if (action!= (uint32)CTP2_MENU_ACTION_SELECT)
		return;

	close_AllScreens();

	switch(itemIndex) {
		case CP_MENU_ITEM_0:
			open_CivStatus();
			break;
		case CP_MENU_ITEM_1:
			TradeManager::Display();
			TradeManager::SetMode(TRADE_MANAGER_MARKET);
			break;
		case CP_MENU_ITEM_2:
			TradeManager::Display();
			TradeManager::SetMode(TRADE_MANAGER_SUMMARY);
			break;
		case CP_MENU_ITEM_3:
			open_ScienceStatus();
			break;
		case CP_MENU_ITEM_4:

			open_ScienceVictory();
			break;
	}
}

void CityMenuCallback(ctp2_Menu *menu, CTP2_MENU_ACTION action, sint32 itemIndex, void *cookie)
{
	if (action!= (uint32)CTP2_MENU_ACTION_SELECT)
		return;

	close_AllScreens();

	Unit city;
	bool haveCity = g_selected_item->GetSelectedCity(city) != FALSE;

	if(!haveCity)
	{
		city = g_theWorld->GetCity(g_selected_item->GetCurSelectPos());
		if(city.IsValid())
			haveCity = true;

	}
	else if(g_player[g_selected_item->GetVisiblePlayer()]->GetNumCities())
	{
		city = g_player[g_selected_item->GetVisiblePlayer()]->m_all_cities->Access(0);
		g_selected_item->SetSelectCity(city);

		if(city.IsValid())
			haveCity = true;
	}

	switch (itemIndex)
	{
		case CP_MENU_ITEM_0:
			if(g_selected_item->GetSelectedCity(city))
			{
				if(g_network.IsClient() && g_network.GetSensitiveUIBlocked())
				{
				}
				else
				{
					EditQueue::Display(CityWindow::GetCityData(city));
				}
			}
			else if(g_player[g_selected_item->GetVisiblePlayer()]->m_all_cities->Num() > 0)
			{
				if(g_network.IsClient() && g_network.GetSensitiveUIBlocked())
				{
				}
				else
				{
					EditQueue::Display(CityWindow::GetCityData(g_player[g_selected_item->GetVisiblePlayer()]->
					                                           m_all_cities->Access(0)));
				}
			}
			break;
		case CP_MENU_ITEM_1:
			CityWindow::Initialize();

			if(g_selected_item->GetSelectedCity(city))
				CityWindow::Display(city.CD());
			else
				CityWindow::Display(NULL);
			break;
		case CP_MENU_ITEM_2:
			open_CityStatus();

			break;
	}
}

void UnitMenuCallback(ctp2_Menu *menu, CTP2_MENU_ACTION action, sint32 itemIndex, void *cookie)
{
	if (action!= (uint32)CTP2_MENU_ACTION_SELECT)
		return;

	close_AllScreens();

	switch (itemIndex)
	{
	case	CP_MENU_ITEM_0:
			open_UnitStatus();
			break;
	case	CP_MENU_ITEM_1:
			ArmyManagerWindow::Display();
			break;
	case	CP_MENU_ITEM_2:
	case	CP_MENU_ITEM_3:
	case	CP_MENU_ITEM_4:
	case	CP_MENU_ITEM_5:
	case	CP_MENU_ITEM_6:
	case	CP_MENU_ITEM_7:
			break;
	}
}

void DiplomacyMenuCallback(ctp2_Menu *menu, CTP2_MENU_ACTION action, sint32 itemIndex, void *cookie)
{
	if (action!= (uint32)CTP2_MENU_ACTION_SELECT)
		return;

	close_AllScreens();

	switch (itemIndex)
	{
		case CP_MENU_ITEM_0:

			DiplomacyWindow::Display();

			break;
		case CP_MENU_ITEM_1:

			if(DipWizard::CanInitiateRightNow())
			{
				DipWizard::Display();
			}
			else
			{
				Assert(g_network.IsActive());
				MessageBoxDialog::Information("str_code_CantInitiateDiplomacyNow", "WhyNoDiplomacy", NULL, NULL, "str_ldl_MB_OK", false);
			}

			break;
		case CP_MENU_ITEM_2:
		case CP_MENU_ITEM_3:
		case CP_MENU_ITEM_4:
		case CP_MENU_ITEM_5:
		case CP_MENU_ITEM_6:
		case CP_MENU_ITEM_7:
			break;
	}
}

void SciMenuCallback(ctp2_Menu *menu, CTP2_MENU_ACTION action, sint32 itemIndex, void *cookie)
{
	if (action!= (uint32)CTP2_MENU_ACTION_SELECT)
		return;

	close_AllScreens();

	switch (itemIndex)
	{
	case	CP_MENU_ITEM_0:
			open_ScienceStatus();
			break;
	case	CP_MENU_ITEM_1:
			open_GreatLibrary();
			break;
	default:
			break;
	}
}

void TradeMenuCallback(ctp2_Menu *menu, CTP2_MENU_ACTION action, sint32 itemIndex, void *cookie)
{
	if (action!= (uint32)CTP2_MENU_ACTION_SELECT)
		return;

	close_AllScreens();

	switch (itemIndex)
	{
	case	CP_MENU_ITEM_0:
	case	CP_MENU_ITEM_1:
			open_TradeStatus();
			break;
	}
}

void GLMenuCallback(ctp2_Menu *menu, CTP2_MENU_ACTION action, sint32 itemIndex, void *cookie)
{
	if (action!= (uint32)CTP2_MENU_ACTION_SELECT)
		return;

	close_AllScreens();

	open_GreatLibrary();
}

void StatsMenuCallback(ctp2_Menu *menu, CTP2_MENU_ACTION action, sint32 itemIndex, void *cookie)
{
	if (action!= (uint32)CTP2_MENU_ACTION_SELECT)
		return;

	switch (itemIndex)
	{
	case	CP_MENU_ITEM_0:
			open_GreatLibrary();
			break;
	case	CP_MENU_ITEM_1:
			InfoWindow::SelectRankingTab();
			break;
	case	CP_MENU_ITEM_2:
			InfoWindow::SelectScoreTab();
			break;
	case	CP_MENU_ITEM_3:
			InfoWindow::SelectWonderTab();
			break;
	default:
			break;
	}
}

void OptionsMenuCallback(ctp2_Menu *menu, CTP2_MENU_ACTION action, sint32 itemIndex, void *cookie)
{
	if (action!= (uint32)CTP2_MENU_ACTION_SELECT)
		return;

	switch ((sint32)cookie)
	{
	case	CP_MENU_ITEM_0:
			gameplayoptions_displayMyWindow();
			break;
	case	CP_MENU_ITEM_1:
			graphicsscreen_displayMyWindow();
			break;
	case	CP_MENU_ITEM_2:
			soundscreen_displayMyWindow();
			break;
	case	CP_MENU_ITEM_3:
			ProfileEdit::Display();
			break;
	case	CP_MENU_ITEM_4:
			optionsscreen_mapeditorPress(NULL, AUI_BUTTON_ACTION_EXECUTE, 0, NULL);

			break;
	case	CP_MENU_ITEM_5:
			if (!g_network.IsClient())
			{
				uint32 type = (g_network.IsActive()) ? LSS_SAVE_MP : LSS_SAVE_GAME;
				g_isScenario = FALSE;
				loadsavescreen_displayMyWindow( type );
			}
			break;
	case	CP_MENU_ITEM_6:
			if(!g_network.IsClient())
			{
				optionwarningscreen_displayMyWindow(OWS_LOAD);
			}
			break;
	case	CP_MENU_ITEM_7:
			if(!g_network.IsActive())
			{
				optionwarningscreen_displayMyWindow(OWS_RESTART);
			}
			break;
	case	CP_MENU_ITEM_8:
			optionwarningscreen_displayMyWindow(OWS_QUITTOSHELL);
			break;
	case	CP_MENU_ITEM_9:
			optionwarningscreen_displayMyWindow(OWS_QUIT);
			break;

	case CP_MENU_ITEM_10:
		if(g_tiledMap)
		{
			g_tiledMap->ZoomOut();
		}
		break;
	case CP_MENU_ITEM_11:
		if(g_tiledMap)
		{
			g_tiledMap->ZoomIn();
		}
		break;
	case	CP_MENU_ITEM_12:
		musicscreen_displayMyWindow();
		break;
	case	CP_MENU_ITEM_13:
		if(g_chatBox) g_chatBox->SetActive(!g_chatBox->IsActive());;
		break;
	}
}

void EspionageMenuCallback(ctp2_Menu *menu, CTP2_MENU_ACTION action, sint32 itemIndex, void *cookie)
{
	if (action!= (uint32)CTP2_MENU_ACTION_SELECT)
		return;

	close_AllScreens();

	switch (itemIndex)
	{
	case CP_MENU_ITEM_0:
		{
			Unit    city;
			if (g_selected_item->GetSelectedCity(city))
			{
				CityEspionage::Display(city);
			}
		}
		break;

	default:
		break;
	}
}

void cpw_DisbandCallback( sint32 val )
{
	if ( val ) {
		g_selected_item->Disband();
	}
}

void controlPanel_popupPress(aui_Control *control, uint32 action, uint32 data, void *cookie )
{
	if ( action != (uint32)AUI_LISTBOX_ACTION_SELECT )
		return;

	if (g_controlPanel!=NULL)
		g_controlPanel->DeActivateSelectedInfo();
}

ControlPanelWindow::ControlPanelWindow(
	AUI_ERRCODE *retval,
	uint32 id,
	MBCHAR *ldlBlock,
	sint32 bpp,
	AUI_WINDOW_TYPE type ) :
m_mainWindow(NULL),
m_contextMenu(NULL),
m_turnToggle(true),
m_targetingMode(CP_TARGETING_MODE_OFF),
m_currentOrder(NULL),
m_terraFormMode(false)
{
	*retval = AUI_ERRCODE_OK;

	BuildMainMenu();

	m_contextMenu = new ctp2_Menu(true, ContextMenuCallback);
	m_mainWindow = static_cast<ctp2_Window*>(
		aui_Ldl::BuildHierarchyFromRoot(ldlBlock));

	Assert(m_mainWindow);
	if(!m_mainWindow)
	{
		*retval = AUI_ERRCODE_INVALIDPARAM;
		return;
	}

	m_mainWindow->SetBevelMode(false);
	m_mainWindow->SetType(AUI_WINDOW_TYPE_SINKING);
	m_mainWindow->SetDynamic(FALSE);

	m_currentProgress = 0;

	static MBCHAR *ldlControlBlock = "ControlPanelWindow.ControlPanel";
	MainControlPanel::Initialize(ldlControlBlock);

	AdjustToScreen();

	CreateTabGroup(ldlControlBlock);

	m_mainWindow->SetTransparent(TRUE);
	m_mainWindow->SetStencilFromImage("control_panel_stencil.tga");

	g_c3ui->AddWindow(m_mainWindow);
	if (g_startHotseatGame)
	{
		// Do not display control panel yet.
		Hide();
	}
	else
	{
		m_mainWindow->Show();
		m_mainWindow->ShouldDraw(TRUE);
	}

}

ControlPanelWindow::~ControlPanelWindow()
{
	AUI_ERRCODE errcode;

	MainControlPanel::CleanUp();

	if (m_mainWindow)
	{
		errcode = g_c3ui->RemoveWindow(m_mainWindow->Id());
		Assert(errcode == AUI_ERRCODE_OK);

		errcode = aui_Ldl::DeleteHierarchyFromRoot("ControlPanelWindow");
		Assert(errcode == AUI_ERRCODE_OK);

		m_mainWindow = NULL;
	}

	if (m_mainMenuBar)
	{
		errcode = g_c3ui->RemoveWindow(m_mainMenuBar->Id());
		Assert(errcode == AUI_ERRCODE_OK);

		errcode = aui_Ldl::DeleteHierarchyFromRoot("MainMenu");
		Assert(errcode == AUI_ERRCODE_OK);

		m_mainMenuBar = NULL;
	}

	delete m_contextMenu;
}

void ControlPanelWindow::BuildMainMenu()
{
	m_mainMenuBar=(ctp2_MenuBar *)aui_Ldl::BuildHierarchyFromRoot("MainMenu");

	m_mainMenuBar->SetType(AUI_WINDOW_TYPE_SINKING);

	Assert(m_mainMenuBar!=NULL);

	if (m_mainMenuBar==NULL)
		return;

	BuildCivMenu	();
	BuildCityMenu	();
	BuildUnitMenu	();
	BuildDipMenu	();


	BuildStatsMenu 	();
	BuildOptionsMenu();

	m_mainMenuBar->BuildNeighbors();

	g_c3ui->AddWindow(m_mainMenuBar);
}

void ControlPanelWindow::RebuildMenus()
{
	ctp2_MenuBar *mb = (ctp2_MenuBar*)aui_Ldl::GetObject("MainMenu");

	if(mb==NULL) return;

	ctp2_Menu *menu = mb->GetMenu("MainMenu","CivActivator");
	Assert(menu);
	if(menu==NULL) return;

	menu->Clear();

	mb->SetMenuCallback	(menu,CivMenuCallback);

	mb->AddMenuItem(menu, g_theStringDB->GetNameStr("str_ldl_EmpireManager"),
		KeyListItem::GetKeyFromKMScreen(theKeyMap->get_keycode(KEY_FUNCTION_OPEN_CIV_STATUS)),(void *)CP_MENU_ITEM_0);
	mb->AddMenuItem(menu, g_theStringDB->GetNameStr("str_ldl_TradeOpenMarket"),
		KeyListItem::GetKeyFromKMScreen(theKeyMap->get_keycode(KEY_FUNCTION_OPEN_TRADE_STATUS)), (void *)CP_MENU_ITEM_1);
	mb->AddMenuItem(menu, g_theStringDB->GetNameStr("str_ldl_TradeSummary"),
		KeyListItem::GetKeyFromKMScreen(theKeyMap->get_keycode(KEY_FUNCTION_TRADE_SUMMARY)), (void *)CP_MENU_ITEM_2);
	mb->AddMenuItem(menu, g_theStringDB->GetNameStr("str_ldl_ScienceManager"),
		KeyListItem::GetKeyFromKMScreen(theKeyMap->get_keycode(KEY_FUNCTION_OPEN_SCIENCE_STATUS)), (void *)CP_MENU_ITEM_3);
	mb->AddMenuItem(menu, g_theStringDB->GetNameStr("str_ldl_Gaia"),
		KeyListItem::GetKeyFromKMScreen(theKeyMap->get_keycode(KEY_FUNCTION_GAIA)), (void *)CP_MENU_ITEM_4);

	menu = mb->GetMenu("MainMenu", "CityActivator");
	menu->Clear();
	mb->SetMenuCallback	(menu,CityMenuCallback);

	mb->AddMenuItem(menu, g_theStringDB->GetNameStr("str_ldl_BuildQueue"),
		KeyListItem::GetKeyFromKMScreen(theKeyMap->get_keycode(KEY_FUNCTION_BUILD_QUEUE)),(void *)CP_MENU_ITEM_0);
	mb->AddMenuItem(menu, g_theStringDB->GetNameStr("str_ldl_CityManager"),
		KeyListItem::GetKeyFromKMScreen(theKeyMap->get_keycode(KEY_FUNCTION_CITY_MANAGEMENT)),(void *)CP_MENU_ITEM_1);
	mb->AddMenuItem(menu, g_theStringDB->GetNameStr("str_ldl_NatlManager"),
		KeyListItem::GetKeyFromKMScreen(theKeyMap->get_keycode(KEY_FUNCTION_OPEN_CITY_STATUS)),(void *)CP_MENU_ITEM_2);

	menu = mb->GetMenu("MainMenu", "UnitsActivator");
	menu->Clear();
	mb->SetMenuCallback	(menu,UnitMenuCallback);

	mb->AddMenuItem(menu, g_theStringDB->GetNameStr("str_ldl_UnitManager"),
		KeyListItem::GetKeyFromKMScreen(theKeyMap->get_keycode(KEY_FUNCTION_OPEN_UNIT_STATUS)),(void *)CP_MENU_ITEM_0);
	mb->AddMenuItem(menu, g_theStringDB->GetNameStr("str_ldl_ArmyManagement"),
		KeyListItem::GetKeyFromKMScreen(theKeyMap->get_keycode(KEY_FUNCTION_OPEN_WORK_VIEW)),(void *)CP_MENU_ITEM_1);

	menu = mb->GetMenu("MainMenu", "DiplomacyActivator");
	menu->Clear();
	mb->SetMenuCallback	(menu,DiplomacyMenuCallback);

	mb->AddMenuItem(menu, g_theStringDB->GetNameStr("str_ldl_DiplomacyManager"),
		KeyListItem::GetKeyFromKMScreen(theKeyMap->get_keycode(KEY_FUNCTION_OPEN_DIPLOMACY)),(void *)CP_MENU_ITEM_0);
	mb->AddMenuItem(menu, g_theStringDB->GetNameStr("str_ldl_NewProposal"),
		KeyListItem::GetKeyFromKMScreen(theKeyMap->get_keycode(KEY_FUNCTION_NEW_PROPOSAL)),(void *)CP_MENU_ITEM_1);

	menu = mb->GetMenu("MainMenu", "StatsActivator");
	menu->Clear();
	mb->SetMenuCallback	(menu,StatsMenuCallback);

	mb->AddMenuItem(menu, g_theStringDB->GetNameStr("str_ldl_GreatLibrary"),
		KeyListItem::GetKeyFromKMScreen(theKeyMap->get_keycode(KEY_FUNCTION_OPEN_GREAT_LIBRARY)),(void *)CP_MENU_ITEM_0);
	mb->AddMenuItem(menu, g_theStringDB->GetNameStr("str_ldl_Ranking"),
		KeyListItem::GetKeyFromKMScreen(theKeyMap->get_keycode(KEY_FUNCTION_RANK)),(void *)CP_MENU_ITEM_1);
	mb->AddMenuItem(menu, g_theStringDB->GetNameStr("str_ldl_Score"),
		KeyListItem::GetKeyFromKMScreen(theKeyMap->get_keycode(KEY_FUNCTION_OPEN_INFO_SCREEN)),(void *)CP_MENU_ITEM_2);
	mb->AddMenuItem(menu, g_theStringDB->GetNameStr("str_tbl_ldl_Open_Wonders"),
		KeyListItem::GetKeyFromKMScreen(theKeyMap->get_keycode(KEY_FUNCTION_TIMELINE)),(void *)CP_MENU_ITEM_3);

	menu = mb->GetMenu("MainMenu", "OptionsActivator");
	menu->Clear();
	mb->SetMenuCallback	(menu,OptionsMenuCallback);

	mb->AddMenuItem(menu, g_theStringDB->GetNameStr("str_ldl_ZoomOut"),
					KeyListItem::GetKeyFromKMScreen(theKeyMap->get_keycode(KEY_FUNCTION_ZOOM_OUT1)),
					(void *)CP_MENU_ITEM_10);
	mb->AddMenuItem(menu, g_theStringDB->GetNameStr("str_ldl_ZoomIn"),
					KeyListItem::GetKeyFromKMScreen(theKeyMap->get_keycode(KEY_FUNCTION_ZOOM_IN1)),
					(void *)CP_MENU_ITEM_11);
	mb->AddMenuItem(menu, g_theStringDB->GetNameStr("str_ldl_Gameplay"),
		KeyListItem::GetKeyFromKMScreen(theKeyMap->get_keycode(KEY_FUNCTION_GAMEPLAY_OPTIONS)),(void *)CP_MENU_ITEM_0);
	mb->AddMenuItem(menu, g_theStringDB->GetNameStr("str_ldl_Graphics"),
		KeyListItem::GetKeyFromKMScreen(theKeyMap->get_keycode(KEY_FUNCTION_GRAPHICS_OPTIONS)),(void *)CP_MENU_ITEM_1);
	mb->AddMenuItem(menu, g_theStringDB->GetNameStr("str_ldl_Sound"),
		KeyListItem::GetKeyFromKMScreen(theKeyMap->get_keycode(KEY_FUNCTION_SOUND_OPTIONS)),(void *)CP_MENU_ITEM_2);

	MBCHAR const *  musicItemText   = g_theStringDB->GetNameStr("str_ldl_Music");
	if (musicItemText)
	{
		mb->AddMenuItem(menu,
		                musicItemText,
		                KeyListItem::GetKeyFromKMScreen
		                    (theKeyMap->get_keycode(KEY_FUNCTION_MUSIC_OPTIONS)),
		                (void *) CP_MENU_ITEM_12
		               );
	}
	// else: No action: backwards compatibility for Mods.

	MBCHAR const *  chatBoxText   = g_theStringDB->GetNameStr("str_tbl_ldl_Open_Chat");
	if (chatBoxText)
	{
		mb->AddMenuItem(menu,
		                chatBoxText,
		                KeyListItem::GetKeyFromKMScreen
		                    (theKeyMap->get_keycode(KEY_FUNCTION_CHAT_KEY)),
		                (void *) CP_MENU_ITEM_13
		               );
	}
	// else: No action: backwards compatibility for Mods.

	mb->AddMenuItem(menu, g_theStringDB->GetNameStr("str_ldl_Advanced"),
		KeyListItem::GetKeyFromKMScreen(theKeyMap->get_keycode(KEY_FUNCTION_ADVANCED_OPTIONS)),(void *)CP_MENU_ITEM_3);
	mb->AddMenuItem(menu, g_theStringDB->GetNameStr("str_ldl_Cheat_Mode_Case"),
		KeyListItem::GetKeyFromKMScreen(theKeyMap->get_keycode(KEY_FUNCTION_OPEN_SCENARIO_EDITOR)),(void *)CP_MENU_ITEM_4);
	mb->AddMenuItem(menu, g_theStringDB->GetNameStr("str_ldl_SaveGame"),
		KeyListItem::GetKeyFromKMScreen(theKeyMap->get_keycode(KEY_FUNCTION_SAVE_GAME)),(void *)CP_MENU_ITEM_5);
	mb->AddMenuItem(menu, g_theStringDB->GetNameStr("str_ldl_LoadGame"),
		KeyListItem::GetKeyFromKMScreen(theKeyMap->get_keycode(KEY_FUNCTION_LOAD_GAME)),(void *)CP_MENU_ITEM_6);

	if (!g_theProfileDB->IsScenario() && !g_isScenario)
	{
		mb->AddMenuItem(menu, g_theStringDB->GetNameStr("str_ldl_Restart"),
			KeyListItem::GetKeyFromKMScreen(theKeyMap->get_keycode(KEY_FUNCTION_RESTART)),(void *)CP_MENU_ITEM_7);
	}
	mb->AddMenuItem(menu, g_theStringDB->GetNameStr("str_ldl_NewGame"),
		KeyListItem::GetKeyFromKMScreen(theKeyMap->get_keycode(KEY_FUNCTION_NEW_GAME)),(void *)CP_MENU_ITEM_8);
	mb->AddMenuItem(menu, g_theStringDB->GetNameStr("str_ldl_Quit"),
		KeyListItem::GetKeyFromKMScreen(theKeyMap->get_keycode(KEY_FUNCTION_QUIT)),(void *)CP_MENU_ITEM_9);
}

void ControlPanelWindow::BuildCivMenu()
{
	Assert(m_mainMenuBar!=NULL);

	if (m_mainMenuBar==NULL)
		return;

	ctp2_Menu  *menu=m_mainMenuBar->GetMenu("MainMenu","CivActivator");

	Assert(menu!=NULL);

	if (menu==NULL)
		return;

	m_mainMenuBar->SetMenuCallback	(menu,CivMenuCallback);

	m_mainMenuBar->AddMenuItem(menu, g_theStringDB->GetNameStr("str_ldl_EmpireManager"),
		KeyListItem::GetKeyFromKMScreen(theKeyMap->get_keycode(KEY_FUNCTION_OPEN_CIV_STATUS)),(void *)CP_MENU_ITEM_0);
	m_mainMenuBar->AddMenuItem(menu, g_theStringDB->GetNameStr("str_ldl_TradeOpenMarket"),
		KeyListItem::GetKeyFromKMScreen(theKeyMap->get_keycode(KEY_FUNCTION_OPEN_TRADE_STATUS)), (void *)CP_MENU_ITEM_1);
	m_mainMenuBar->AddMenuItem(menu, g_theStringDB->GetNameStr("str_ldl_TradeSummary"),
		KeyListItem::GetKeyFromKMScreen(theKeyMap->get_keycode(KEY_FUNCTION_TRADE_SUMMARY)), (void *)CP_MENU_ITEM_2);
	m_mainMenuBar->AddMenuItem(menu, g_theStringDB->GetNameStr("str_ldl_ScienceManager"),
		KeyListItem::GetKeyFromKMScreen(theKeyMap->get_keycode(KEY_FUNCTION_OPEN_SCIENCE_STATUS)), (void *)CP_MENU_ITEM_3);
	m_mainMenuBar->AddMenuItem(menu, g_theStringDB->GetNameStr("str_ldl_Gaia"),
		KeyListItem::GetKeyFromKMScreen(theKeyMap->get_keycode(KEY_FUNCTION_GAIA)), (void *)CP_MENU_ITEM_4);
}

void ControlPanelWindow::BuildCityMenu()
{
	Assert(m_mainMenuBar!=NULL);

	if (m_mainMenuBar==NULL)
		return;

	ctp2_Menu  *menu=m_mainMenuBar->GetMenu("MainMenu","CityActivator");

	Assert(menu!=NULL);

	if (menu==NULL)
		return;

	m_mainMenuBar->SetMenuCallback	(menu,CityMenuCallback);

	m_mainMenuBar->AddMenuItem(menu, g_theStringDB->GetNameStr("str_ldl_BuildQueue"),
		KeyListItem::GetKeyFromKMScreen(theKeyMap->get_keycode(KEY_FUNCTION_BUILD_QUEUE)),(void *)CP_MENU_ITEM_0);
	m_mainMenuBar->AddMenuItem(menu, g_theStringDB->GetNameStr("str_ldl_CityManager"),
		KeyListItem::GetKeyFromKMScreen(theKeyMap->get_keycode(KEY_FUNCTION_CITY_MANAGEMENT)),(void *)CP_MENU_ITEM_1);
	m_mainMenuBar->AddMenuItem(menu, g_theStringDB->GetNameStr("str_ldl_NatlManager"),
		KeyListItem::GetKeyFromKMScreen(theKeyMap->get_keycode(KEY_FUNCTION_OPEN_CITY_STATUS)),(void *)CP_MENU_ITEM_2);
}

void ControlPanelWindow::BuildUnitMenu()
{
	Assert(m_mainMenuBar!=NULL);

	if (m_mainMenuBar==NULL)
		return;

	ctp2_Menu  *menu=m_mainMenuBar->GetMenu("MainMenu","UnitsActivator");

	Assert(menu!=NULL);

	if (menu==NULL)
		return;

	m_mainMenuBar->SetMenuCallback	(menu,UnitMenuCallback);

	m_mainMenuBar->AddMenuItem(menu, g_theStringDB->GetNameStr("str_ldl_UnitManager"),
		KeyListItem::GetKeyFromKMScreen(theKeyMap->get_keycode(KEY_FUNCTION_OPEN_UNIT_STATUS)),(void *)CP_MENU_ITEM_0);
	m_mainMenuBar->AddMenuItem(menu, g_theStringDB->GetNameStr("str_ldl_ArmyManagement"),
		KeyListItem::GetKeyFromKMScreen(theKeyMap->get_keycode(KEY_FUNCTION_OPEN_WORK_VIEW)),(void *)CP_MENU_ITEM_1);
}

void ControlPanelWindow::BuildDipMenu()
{
	Assert(m_mainMenuBar!=NULL);

	if (m_mainMenuBar==NULL)
		return;

	ctp2_Menu  *menu=m_mainMenuBar->GetMenu("MainMenu","DiplomacyActivator");

	Assert(menu!=NULL);

	if (menu==NULL)
		return;

	m_mainMenuBar->SetMenuCallback	(menu,DiplomacyMenuCallback);

	m_mainMenuBar->AddMenuItem(menu, g_theStringDB->GetNameStr("str_ldl_DiplomacyManager"),
		KeyListItem::GetKeyFromKMScreen(theKeyMap->get_keycode(KEY_FUNCTION_OPEN_DIPLOMACY)),(void *)CP_MENU_ITEM_0);
	m_mainMenuBar->AddMenuItem(menu, g_theStringDB->GetNameStr("str_ldl_NewProposal"),
		KeyListItem::GetKeyFromKMScreen(theKeyMap->get_keycode(KEY_FUNCTION_NEW_PROPOSAL)),(void *)CP_MENU_ITEM_1);
}

void ControlPanelWindow::BuildSciMenu()
{
	Assert(m_mainMenuBar!=NULL);

	if (m_mainMenuBar==NULL)
		return;

	ctp2_Menu  *menu=m_mainMenuBar->GetMenu("MainMenu","ScienceActivator");

	Assert(menu!=NULL);

	if (menu==NULL)
		return;

	m_mainMenuBar->SetMenuCallback	(menu,SciMenuCallback);

	m_mainMenuBar->AddMenuItem(menu, g_theStringDB->GetNameStr("str_ldl_ScienceScreen"),
		KeyListItem::GetKeyFromKMScreen(theKeyMap->get_keycode(KEY_FUNCTION_OPEN_SCIENCE_STATUS)),(void *)CP_MENU_ITEM_0);
	m_mainMenuBar->AddMenuItem(menu, g_theStringDB->GetNameStr("str_ldl_GreatLibrary"),
		KeyListItem::GetKeyFromKMScreen(theKeyMap->get_keycode(KEY_FUNCTION_OPEN_GREAT_LIBRARY)),(void *)CP_MENU_ITEM_1);
}

void ControlPanelWindow::BuildTradeMenu()
{
	Assert(m_mainMenuBar!=NULL);

	if (m_mainMenuBar==NULL)
		return;

	ctp2_Menu  *menu=m_mainMenuBar->GetMenu("MainMenu","TradeActivator");

	Assert(menu!=NULL);

	if (menu==NULL)
		return;

	m_mainMenuBar->SetMenuCallback	(menu,TradeMenuCallback);

	m_mainMenuBar->AddMenuItem(menu, g_theStringDB->GetNameStr("str_ldl_TradeScreen"),
		KeyListItem::GetKeyFromKMScreen(theKeyMap->get_keycode(KEY_FUNCTION_OPEN_TRADE_STATUS)),(void *)CP_MENU_ITEM_0);
}

void ControlPanelWindow::BuildGLMenu()
{
	Assert(m_mainMenuBar!=NULL);

	if (m_mainMenuBar==NULL)
		return;

	ctp2_Menu  *menu=m_mainMenuBar->GetMenu("MainMenu","GLActivator");

	Assert(menu!=NULL);

	if (menu==NULL)
		return;

	m_mainMenuBar->SetMenuCallback	(menu,GLMenuCallback);

	m_mainMenuBar->AddMenuItem(menu, g_theStringDB->GetNameStr("str_ldl_GreatLibrary"),
		KeyListItem::GetKeyFromKMScreen(theKeyMap->get_keycode(KEY_FUNCTION_OPEN_GREAT_LIBRARY)),(void *)CP_MENU_ITEM_0);
}

void ControlPanelWindow::BuildStatsMenu()
{
	Assert(m_mainMenuBar!=NULL);

	if (m_mainMenuBar==NULL)
		return;

	ctp2_Menu  *menu=m_mainMenuBar->GetMenu("MainMenu","StatsActivator");

	Assert(menu!=NULL);

	if (menu==NULL)
		return;

	m_mainMenuBar->SetMenuCallback	(menu,StatsMenuCallback);

	m_mainMenuBar->AddMenuItem(menu, g_theStringDB->GetNameStr("str_ldl_GreatLibrary"),
		KeyListItem::GetKeyFromKMScreen(theKeyMap->get_keycode(KEY_FUNCTION_OPEN_GREAT_LIBRARY)),(void *)CP_MENU_ITEM_0);
	m_mainMenuBar->AddMenuItem(menu, g_theStringDB->GetNameStr("str_ldl_Ranking"),
		KeyListItem::GetKeyFromKMScreen(theKeyMap->get_keycode(KEY_FUNCTION_RANK)),(void *)CP_MENU_ITEM_1);
	m_mainMenuBar->AddMenuItem(menu, g_theStringDB->GetNameStr("str_ldl_Score"),
		KeyListItem::GetKeyFromKMScreen(theKeyMap->get_keycode(KEY_FUNCTION_OPEN_INFO_SCREEN)),(void *)CP_MENU_ITEM_2);
	m_mainMenuBar->AddMenuItem(menu, g_theStringDB->GetNameStr("str_tbl_ldl_Open_Wonders"),
		KeyListItem::GetKeyFromKMScreen(theKeyMap->get_keycode(KEY_FUNCTION_TIMELINE)),(void *)CP_MENU_ITEM_3);
}

void ControlPanelWindow::BuildOptionsMenu()
{
	Assert(m_mainMenuBar!=NULL);

	if (m_mainMenuBar==NULL)
		return;

	ctp2_Menu  *menu=m_mainMenuBar->GetMenu("MainMenu","OptionsActivator");

	Assert(menu!=NULL);

	if (menu==NULL)
		return;

	m_mainMenuBar->SetMenuCallback	(menu,OptionsMenuCallback);

	m_mainMenuBar->AddMenuItem(menu, g_theStringDB->GetNameStr("str_ldl_ZoomOut"),
							   KeyListItem::GetKeyFromKMScreen(theKeyMap->get_keycode(KEY_FUNCTION_ZOOM_OUT1)),
							   (void *)CP_MENU_ITEM_10);
	m_mainMenuBar->AddMenuItem(menu, g_theStringDB->GetNameStr("str_ldl_ZoomIn"),
							   KeyListItem::GetKeyFromKMScreen(theKeyMap->get_keycode(KEY_FUNCTION_ZOOM_IN1)),
							   (void *)CP_MENU_ITEM_11);

	m_mainMenuBar->AddMenuItem(menu, g_theStringDB->GetNameStr("str_ldl_Gameplay"),
		KeyListItem::GetKeyFromKMScreen(theKeyMap->get_keycode(KEY_FUNCTION_GAMEPLAY_OPTIONS)),(void *)CP_MENU_ITEM_0);
	m_mainMenuBar->AddMenuItem(menu, g_theStringDB->GetNameStr("str_ldl_Graphics"),
		KeyListItem::GetKeyFromKMScreen(theKeyMap->get_keycode(KEY_FUNCTION_GRAPHICS_OPTIONS)),(void *)CP_MENU_ITEM_1);
	m_mainMenuBar->AddMenuItem(menu, g_theStringDB->GetNameStr("str_ldl_Sound"),
		KeyListItem::GetKeyFromKMScreen(theKeyMap->get_keycode(KEY_FUNCTION_SOUND_OPTIONS)),(void *)CP_MENU_ITEM_2);

	{
		MBCHAR const *  musicItemText   = g_theStringDB->GetNameStr("str_ldl_Music");

		if (musicItemText)
		{
			// MUSIC added by ahenobarb
			m_mainMenuBar->AddMenuItem
			    (menu,
			     musicItemText,
			     KeyListItem::GetKeyFromKMScreen(theKeyMap->get_keycode(KEY_FUNCTION_MUSIC_OPTIONS)),
			     (void *) CP_MENU_ITEM_12
			    );
		}
		// else: No action: backwards compatibility for Mods.
	}
	{
		MBCHAR const *  chatBoxText = g_theStringDB->GetNameStr("str_tbl_ldl_Open_Chat");

		if(chatBoxText)
		{
			m_mainMenuBar->AddMenuItem
			(menu,
			 chatBoxText,
			 KeyListItem::GetKeyFromKMScreen(theKeyMap->get_keycode(KEY_FUNCTION_CHAT_KEY)),
			 (void *)CP_MENU_ITEM_13
			);
		}
		// else: No action: backwards compatibility for Mods.
	}

	m_mainMenuBar->AddMenuItem(menu, g_theStringDB->GetNameStr("str_ldl_Advanced"),
		KeyListItem::GetKeyFromKMScreen(theKeyMap->get_keycode(KEY_FUNCTION_ADVANCED_OPTIONS)),(void *)CP_MENU_ITEM_3);
	m_mainMenuBar->AddMenuItem(menu, g_theStringDB->GetNameStr("str_ldl_Cheat_Mode_Case"),
		KeyListItem::GetKeyFromKMScreen(theKeyMap->get_keycode(KEY_FUNCTION_OPEN_SCENARIO_EDITOR)),(void *)CP_MENU_ITEM_4);
	m_mainMenuBar->AddMenuItem(menu, g_theStringDB->GetNameStr("str_ldl_SaveGame"),
		KeyListItem::GetKeyFromKMScreen(theKeyMap->get_keycode(KEY_FUNCTION_SAVE_GAME)),(void *)CP_MENU_ITEM_5);
	m_mainMenuBar->AddMenuItem(menu, g_theStringDB->GetNameStr("str_ldl_LoadGame"),
		KeyListItem::GetKeyFromKMScreen(theKeyMap->get_keycode(KEY_FUNCTION_LOAD_GAME)),(void *)CP_MENU_ITEM_6);

	if (!g_theProfileDB->IsScenario() && !g_isScenario)
	{
		m_mainMenuBar->AddMenuItem(menu, g_theStringDB->GetNameStr("str_ldl_Restart"),
			KeyListItem::GetKeyFromKMScreen(theKeyMap->get_keycode(KEY_FUNCTION_RESTART)),(void *)CP_MENU_ITEM_7);
	}
	m_mainMenuBar->AddMenuItem(menu, g_theStringDB->GetNameStr("str_ldl_NewGame"),
		KeyListItem::GetKeyFromKMScreen(theKeyMap->get_keycode(KEY_FUNCTION_NEW_GAME)),(void *)CP_MENU_ITEM_8);
	m_mainMenuBar->AddMenuItem(menu, g_theStringDB->GetNameStr("str_ldl_Quit"),
		KeyListItem::GetKeyFromKMScreen(theKeyMap->get_keycode(KEY_FUNCTION_QUIT)),(void *)CP_MENU_ITEM_9);
}

void ControlPanelWindow::BuildEspionageMenu()
{
	Assert(m_mainMenuBar!=NULL);

	if (m_mainMenuBar==NULL)
		return;

	ctp2_Menu  *menu=m_mainMenuBar->GetMenu("MainMenu","EspionageActivator");

	Assert(menu!=NULL);

	if (menu==NULL)
		return;

	m_mainMenuBar->SetMenuCallback	(menu,EspionageMenuCallback);

	m_mainMenuBar->AddMenuItem(menu,(char*)g_theStringDB->GetNameStr("str_ldl_CityEspionage"),
									(char*)g_theStringDB->GetNameStr("str_ldl_CityEspionageHotKey"),(void *)CP_MENU_ITEM_0);
}

void ControlPanelWindow::BeginOrderDelivery()
{
	if (m_unitOrders==NULL)
		return;

	OrderRecord *order=NULL;

	ctp2_ListItem *item=(ctp2_ListItem *)m_unitOrders->GetSelectedItem();

	if (item!=NULL)
		order=(OrderRecord *)item->GetUserData();

	BeginOrderDelivery(order);
}

struct OrderDataRec
{
	ArmyData *data;
	OrderRecord *rec;
};

void ControlPanelWindow::BeginOrderDelivery(OrderRecord *rec)
{
	m_currentOrder = NULL;
	m_targetingMode= CP_TARGETING_MODE_OFF;

	if (rec==NULL)
		return;

	MapPoint pos;

	g_tiledMap->GetMouseTilePos(pos);

	Army army = UnitPanelGetCurrent();

	if (!army.IsValid())
		return;

	ArmyData *data=army.AccessData();

	if (data==NULL)
		return;

	ORDER_TEST test=data->TestOrderHere(rec,pos);

	if ((test==ORDER_TEST_OK || test == ORDER_TEST_NO_MOVEMENT) && (!rec->GetTargetPretestMovePosition() && !rec->GetTargetPretestAttackPosition() && !rec->GetUnitPretest_CanTransport()))
	{
		if(strcmp(rec->GetEventName(),"DisbandArmyOrder")==0)
		{
			OrderDataRec *callbackorder=new OrderDataRec;
			callbackorder->data=data;
			callbackorder->rec=rec;
			MessageBoxDialog::Query("str_ldl_Disband_selected_units", "VerifyDisbandUnit", ControlPanelWindow::PerformOrderAfterConfirmation, (void *)callbackorder);
		}
		else
		{
			data->PerformOrder(rec);
		}
	}
	else
	{
		m_currentOrder = rec;
		m_targetingMode= CP_TARGETING_MODE_ORDER_PENDING;
		g_selected_item->SetDrawablePathDest(pos);
	}
}

void ControlPanelWindow::PerformOrderAfterConfirmation(bool response, void *userData)
{
	if(response)
	{
		((OrderDataRec*)userData)->data->PerformOrder(((OrderDataRec*)userData)->rec);
	}
	delete (OrderDataRec*) userData;
}

void ControlPanelWindow::BeginImprovementCycle(void *rawrec)  //emod2 what does this do?
{
	BeginImprovementCycle((TerrainImprovementRecord *)rawrec);
}

void ControlPanelWindow::BeginImprovementCycle(TerrainImprovementRecord *rec) //emod3 is this the check? i think it misses obsoletes here
{
	if ((rec==NULL)||(g_selected_item==NULL))
		return;

	if (!terrainutil_CanPlayerBuild(rec,g_selected_item->GetVisiblePlayer(),false))
	{
		ClearTargetingMode();
		return;
	}

	m_targetingMode		= CP_TARGETING_MODE_TILEIMP_PENDING;
	m_currentTerrainImpRec	= rec;
}

void ControlPanelWindow::BeginImprovementCycle(TerrainRecord *rec)
{
	m_targetingMode		= CP_TARGETING_MODE_TERRAFORM_PENDING;
	m_currentTerrainRec	= rec;
}

void ControlPanelWindow::TargetingMode()
{
	switch (m_targetingMode)
	{
	case	CP_TARGETING_MODE_ORDER_PENDING:
			OrderDeliveryUpdate();
			break;
	case	CP_TARGETING_MODE_TILEIMP_PENDING:
			TileImpUpdate();
			break;
	case	CP_TARGETING_MODE_TERRAFORM_PENDING:
			TerraFormUpdate();
			break;
	default:
			ClearTargetingMode();
	}
}

void ControlPanelWindow::OrderDeliveryUpdate()
{
	MapPoint pos;

	g_tiledMap->GetMouseTilePos(pos);

	if(m_currentOrder==NULL)
		ClearTargetingMode();

	Army army = UnitPanelGetCurrent();

	if (!army.IsValid())
		ClearTargetingMode();

	if(m_targetingMode==CP_TARGETING_MODE_OFF)
		return;

	ArmyData *data=army.AccessData();

	if(data==NULL)
	{
		specialAttackWindow_DisplayData(pos, -1);
		return;
	}

	ORDER_TEST test=army->TestOrderHere(m_currentOrder,pos);

	if (test != ORDER_TEST_INVALID_TARGET &&
	    test != ORDER_TEST_NEEDS_TARGET )
	{
		if(m_currentOrder->GetTargetPretestMovePosition())
		{
			if(army->IsOccupiedByForeigner(pos) && army->CheckWasEnemyVisible(pos, true))
			{
				static CellUnitList defender;
				defender.Clear();
				g_theWorld->GetArmy(pos, defender);
				if(army->CanAtLeastOneCaptureCity() || army->CanFight(defender))
				{
					g_cursorManager->SetCursor(CURSORINDEX_ASSAULT);
				}
				else
				{
					g_cursorManager->SetCursor(CURSORINDEX_NOMOVE);
				}
			}
			else if(g_theWorld->HasCity(pos)
			&&      g_theWorld->GetCity(pos)->GetOwner() != army->GetOwner()
			&&      army->CanAtLeastOneCaptureCity()
			&&      army->CheckWasEnemyVisible(pos, true)
			){
				g_cursorManager->SetCursor(CURSORINDEX_ASSAULT);
			}
			else
			{
				g_cursorManager->SetCursor(CURSORINDEX_MOVE);
			}
			specialAttackWindow_DisplayData(pos, -1);
		}
		else
		{
			g_cursorManager->SetCursor(Order::GetCursor(m_currentOrder));
			if(army->CheckWasEnemyVisible(pos, true))
			{
				specialAttackWindow_DisplayData(pos, m_currentOrder->GetIndex());
			}
			else
			{
				specialAttackWindow_DisplayData(pos, -1);
			}
		}
	}
	else
	{
		if(m_currentOrder->GetTargetPretestMovePosition())
		{
			g_cursorManager->SetCursor(CURSORINDEX_NOMOVE);
		}
		else
		{
			g_cursorManager->SetCursor(Order::GetCursor(m_currentOrder));
		}
		specialAttackWindow_DisplayData(pos, -1);
	}
}

void ControlPanelWindow::TileImpUpdate()  //emod4 definetely needs this but schould it look like sce
{
	if ((m_currentTerrainImpRec==NULL)||(g_selected_item==NULL))
		return;

	sint32 player_id =g_selected_item->GetVisiblePlayer();

	MapPoint pos;

	g_tiledMap->GetMouseTilePos(pos);

	bool const 	hideExpensive	= !g_theProfileDB->GetValueByName("ShowExpensive");

	if (!terrainutil_CanPlayerBuild(m_currentTerrainImpRec,player_id, hideExpensive))
	{
		ClearTargetingMode();
		return;
	}

	Assert(g_player[player_id]);
	Player *player = g_player[player_id];

	if (player==NULL)
	{
		ClearTargetingMode();
		return;
	}

	if (CursorOverControlPanel())
	{
		g_cursorManager->SetCursor(CURSORINDEX_DEFAULT);
		tileimptracker_DisplayData(pos, -1);
	}
	else
	{
		Pixel16 color;
		if (player->CanCreateImprovement(m_currentTerrainImpRec->GetIndex(), pos, true))
		{
			g_cursorManager->SetCursor(CURSORINDEX_MOVE);
			color = g_colorSet->GetColor(COLOR_GREEN);
		}
		else
		{
			g_cursorManager->SetCursor(CURSORINDEX_NOMOVE);
			color = g_colorSet->GetColor(COLOR_RED);
		}

		if(g_player[player_id]->IsExplored(pos))
		{
			g_tiledMap->SetTerrainOverlay(m_currentTerrainImpRec, pos, color);
			tileimptracker_DisplayData(pos, m_currentTerrainImpRec->GetIndex());
		}
		else
		{
			tileimptracker_DisplayData(pos, -1);
		}
	}
}

void ControlPanelWindow::TerraFormUpdate() //emod5
{
	if ((m_currentTerrainRec==NULL)||(g_selected_item==NULL))
		return;

	ClearTargetingMode();
}

bool ControlPanelWindow::ExecuteTargetingModeClick(const MapPoint &pos)
{
	switch (m_targetingMode)
	{
	case	CP_TARGETING_MODE_ORDER_PENDING:
			return OrderDeliveryClick(pos);
	case	CP_TARGETING_MODE_TILEIMP_PENDING:
			return TileImpClick(pos);
	case	CP_TARGETING_MODE_TERRAFORM_PENDING:
			return TerraFormClick(pos);
	default:
			ClearTargetingMode();
	}
	return false;
}

bool ControlPanelWindow::OrderDeliveryClick(const MapPoint &pos)
{
	if(!m_currentOrder)
	{
		ClearTargetingMode();
		return false;
	}

	Army army;

	if(!g_selected_item->GetSelectedArmy(army))
	{
		ClearTargetingMode();
		return false;
	}

	ORDER_TEST test=army->TestOrderHere(m_currentOrder,pos);

	if (test!=ORDER_TEST_INVALID_TARGET)
		g_cursorManager->SetCursor(CURSORINDEX_MOVE);
	else
		g_cursorManager->SetCursor(CURSORINDEX_NOMOVE);

	bool handled = false;

	if (!m_currentOrder->GetTargetPretestMovePosition() )
	{
		if (army->RetPos() == pos &&
			( m_currentOrder->GetTargetPretestTerrainImprovement() ||  //emod6
			  m_currentOrder->GetTargetPretestTradeRoute() ||
			  m_currentOrder->GetTargetPretestAdjacentPosition() ) )
		{
			army->PerformOrder(m_currentOrder);
			handled = true;
		}
		else if(test == ORDER_TEST_INVALID_TARGET)
		{
			g_soundManager->AddGameSound(GAMESOUNDS_ILLEGAL_SPECIAL);
			handled = true;
		}
		else if (g_selected_item->GetGoodPath()->GetMovesRemaining() <= 0 &&
				 g_selected_item->GetBadPath().GetMovesRemaining() > 0)
		{
			if(m_currentOrder->GetUnitPretest_CanTransport())
			{
				static CellUnitList cargoToUnload;
				if(MainControlPanel::GetSelectedCargo(cargoToUnload))
				{
					for(sint32 i = 0; i < cargoToUnload.Num(); i++)
					{
						if(cargoToUnload[i].IsValid())
						{
							cargoToUnload[i].SetFlag(k_UDF_TEMP_TRANSPORT_SELECT);
						}
					}
				}
			}

			Path    badPath = g_selected_item->GetBadPath();
			army->PerformOrderHere(m_currentOrder, &badPath);
			handled = true;
		}
		else if (g_selected_item->GetGoodPath()->GetMovesRemaining() > 0 )
		{
			Path path(g_selected_item->GetGoodPath());

			path.Concat(g_selected_item->GetBadPath());
			if(m_currentOrder->GetUnitPretest_CanTransport())
			{
				static CellUnitList cargoToUnload;
				if(MainControlPanel::GetSelectedCargo(cargoToUnload))
				{
					for(sint32 i = 0; i < cargoToUnload.Num(); i++)
					{
						if(cargoToUnload[i].IsValid()) {
							cargoToUnload[i].SetFlag(k_UDF_TEMP_TRANSPORT_SELECT);
						}
					}
				}
			}

			if(pos == army->RetPos())
			{
				army->PerformOrder(m_currentOrder);
			}
			else
			{
				Assert(path.GetEnd() == pos);

				g_gevManager->Pause();
				army->PerformOrderHere(m_currentOrder, &path);
				g_gevManager->Resume();
			}
			handled = true;
		}
		else
		{
			Assert(0);
		}

		ClearTargetingMode();
	}

	return handled;
}

bool ControlPanelWindow::TileImpClick(const MapPoint &pos) //emod7
{
	if ((m_currentTerrainImpRec==NULL)||(g_selected_item==NULL))
	{
		ClearTargetingMode();
		return true;
	}

	sint32 player=g_selected_item->GetVisiblePlayer();

	if (!terrainutil_CanPlayerBuild(m_currentTerrainImpRec,player,true))
		return true;

	if (!terrainutil_CanPlayerBuildAt(m_currentTerrainImpRec,player,pos))
		return true;

	g_gevManager->AddEvent(GEV_INSERT_Tail, GEV_CreateImprovement,
	                       GEA_Player,player,
	                       GEA_MapPoint, pos,
	                       GEA_Int,m_currentTerrainImpRec->GetIndex(),
	                       GEA_Int,0,
	                       GEA_End);

	return true;
}

bool ControlPanelWindow::TerraFormClick(const MapPoint &pos)
{
	if ((m_currentTerrainRec==NULL)||(g_selected_item==NULL))
	{
		ClearTargetingMode();
	}

	return true;
}

void ControlPanelWindow::ClearTargetingMode()
{
	m_targetingMode= CP_TARGETING_MODE_OFF;

	m_currentOrder		   = NULL;
	m_currentTerrainRec    = NULL;
	m_currentTerrainImpRec = NULL;

	if(g_cursorManager)
		g_cursorManager->SetCursor(CURSORINDEX_DEFAULT);

	if(!g_tiledMap) return;

	MapPoint pos;
	g_tiledMap->GetMouseTilePos(pos);
	tileimptracker_DisplayData(pos, -1);
	specialAttackWindow_DisplayData(pos, -1);
}

void ControlPanelWindow::CreateTabGroup(MBCHAR *ldlBlock)
{
	aui_Ldl::SetActionFuncAndCookie("ControlPanelWindow.ControlPanel.ControlTabPanel.DomesticTab", TabCallback, (void *)CP_TAB_CIV);
	aui_Ldl::SetActionFuncAndCookie("ControlPanelWindow.ControlPanel.ControlTabPanel.CityTab",     TabCallback, (void *)CP_TAB_CITY);
	aui_Ldl::SetActionFuncAndCookie("ControlPanelWindow.ControlPanel.ControlTabPanel.UnitTab",     TabCallback, (void *)CP_TAB_UNIT);
	aui_Ldl::SetActionFuncAndCookie("ControlPanelWindow.ControlPanel.ControlTabPanel.MessageTab",  TabCallback, (void *)CP_TAB_MSGLOG);
	aui_Ldl::SetActionFuncAndCookie("ControlPanelWindow.ControlPanel.ControlTabPanel.TilesTab",    TabCallback, (void *)CP_TAB_TILEIMP);

	CreateTab(CP_TAB_CIV,    "CivTabWindow"     );
	CreateTab(CP_TAB_MSGLOG, "MsgLogTabWindow"  );
	CreateTab(CP_TAB_CITY,   "CityInfoTabWindow");
	CreateTab(CP_TAB_UNIT,   "UnitInfoTabWindow");
	CreateTab(CP_TAB_TILEIMP,"TileImpTabWindow" );

	m_tabGroup = (ctp2_TabGroup *)aui_Ldl::GetObject("ControlPanelWindow.ControlPanel.ControlTabPanel");
	m_tabs[CP_TAB_CIV] = (ctp2_Tab *)aui_Ldl::GetObject("ControlPanelWindow.ControlPanel.ControlTabPanel.DomesticTab");
	m_tabs[CP_TAB_MSGLOG] = (ctp2_Tab *)aui_Ldl::GetObject("ControlPanelWindow.ControlPanel.ControlTabPanel.MessageTab");
	m_tabs[CP_TAB_CITY] = (ctp2_Tab *)aui_Ldl::GetObject("ControlPanelWindow.ControlPanel.ControlTabPanel.CityTab");
	m_tabs[CP_TAB_UNIT] = (ctp2_Tab *)aui_Ldl::GetObject("ControlPanelWindow.ControlPanel.ControlTabPanel.UnitTab");
	m_tabs[CP_TAB_TILEIMP] = (ctp2_Tab *)aui_Ldl::GetObject("ControlPanelWindow.ControlPanel.ControlTabPanel.TilesTab");
}

void ControlPanelWindow::CreateTab(sint32 which,MBCHAR *name)
{
	if ((which<0)||(which>=CP_TAB_LAST)||(name==NULL))
		return;

	switch (which)
	{
	case	CP_TAB_CIV:

			break;
	case CP_TAB_MSGLOG:
			InitMessageTab();
			break;
	case	CP_TAB_CITY:

			break;
	case	CP_TAB_UNIT:

			break;
	case	CP_TAB_TILEIMP:
			InitTileImpTab();
			break;
	}
}

void ControlPanelWindow::InitCivTab(void)
{
	m_numCities			= (ctp2_Static *)aui_Ldl::GetObject("ControlPanelWindow.ControlPanel.ControlTabPanel.DomesticTab.TabPanel.CityIconText"		);
	m_totalPop			= (ctp2_Static *)aui_Ldl::GetObject("ControlPanelWindow.ControlPanel.ControlTabPanel.DomesticTab.TabPanel.LockIconText"		);
	m_currentAdvance	= (ctp2_Static *)aui_Ldl::GetObject("ControlPanelWindow.ControlPanel.ControlTabPanel.DomesticTab.TabPanel.AdvanceText"		);
	m_turnsToAdvance	= (ctp2_Static *)aui_Ldl::GetObject("ControlPanelWindow.ControlPanel.ControlTabPanel.DomesticTab.TabPanel.TurnsRemainingText");
	m_currentHappiness	= (ctp2_Static *)aui_Ldl::GetObject("ControlPanelWindow.ControlPanel.ControlTabPanel.DomesticTab.TabPanel.HappinessIconBar");

	ctp2_Button *target;

	target = (ctp2_Button *)aui_Ldl::GetObject("ControlPanelWindow.ControlPanel.ControlTabPanel.DomesticTab.TabPanel.UnitStatusButton");

	if (target!=NULL)
		target->SetActionFuncAndCookie(TurnNextUnitButtonActionCallback,NULL);

	target = (ctp2_Button *)aui_Ldl::GetObject("ControlPanelWindow.ControlPanel.ControlTabPanel.DomesticTab.TabPanel.CityStatusButton");

	if (target!=NULL)
		target->SetActionFuncAndCookie(TurnNextCityButtonActionCallback,NULL);

	target = (ctp2_Button *)aui_Ldl::GetObject("ControlPanelWindow.ControlPanel.ControlTabPanel.DomesticTab.TabPanel.AdvanceButton");

	if (m_currentHappiness!=NULL)
		m_currentHappiness->SetDrawCallbackAndCookie(controlpanelwindow_HappinessDrawCallback, NULL);
}

void ControlPanelWindow::InitMessageTab(void)
{
	m_messageList = (ctp2_ListBox *)aui_Ldl::GetObject("ControlPanelWindow.ControlPanel.ControlTabPanel.MessageTab.TabPanel.MessageListBox");
	Assert(m_messageList != NULL);
	if (m_messageList == NULL)
		return;

	m_messageList->SetActionFuncAndCookie(controlpanelwindow_MessageListCallback, NULL);
}

void ControlPanelWindow::AddMessage(Message &message,bool initializing)
{
	if (!m_messageList)
		return;

	if (!g_theMessagePool->IsValid(message))
		return;

	if (message.GetOwner() != g_selected_item->GetVisiblePlayer())
		return;

	ctp2_ListItem *item;
	item = (ctp2_ListItem *) aui_Ldl::BuildHierarchyFromRoot("MessagePanelListItem");

	ctp2_Static *staticContainer = (ctp2_Static *)item->GetChildByIndex(0);

	ctp2_Static *staticThing = (ctp2_Static *)staticContainer->GetChildByIndex(0);

	MBCHAR *iconName = g_theMessageIconFileDB->GetFilename(message.GetMsgType());
	if(!iconName)
		iconName = k_MESSAGE_ICON_PICTURE_WARNING;
	staticThing->SetImage(iconName);

	staticThing = (ctp2_Static *)staticContainer->GetChildByIndex(1);

	MBCHAR tempStr[k_MAX_MSG_LEN];
	memset((char *)tempStr, 0, k_MAX_MSG_LEN);
	strncpy(tempStr, message.GetText(), k_MAX_MSG_LEN-1);

	MBCHAR *begin,*end;
	sint32 length;
	while(strchr(tempStr,'<'))
	{
		begin=strchr(tempStr,'<');
		end=strchr(begin,'>');
		if(begin && end)
		{
			length=strlen(end);
			memmove(begin,end+1,length);
		}
		else
		{
			break;
		}
	}

	begin = tempStr;

	while(*begin)
	{
		if(*begin < ' ' && *begin>0)
		{
			*begin=' ';
		}
		begin++;
	}
	staticThing->TextReloadFont();

	staticThing->GetTextFont()->TruncateString(tempStr, staticThing->Width());
	staticThing->SetText(tempStr);
	staticThing->SetTextColor(message.IsRead()?g_colorSet->GetColorRef(COLOR_GRAY):g_colorSet->GetColorRef(COLOR_BLACK));

	ctp2_Static *tabLabel;
	tabLabel = (static_cast<ctp2_Static*>(aui_Ldl::GetObject("ControlPanelWindow.ControlPanel.ControlTabPanel.MessageTab.TabButton.Label")));

	if(!initializing && !message.IsRead())
	{
		tabLabel->SetTextColor(g_colorSet->GetColorRef(COLOR_RED));
	}

	tabLabel->ShouldDraw(TRUE);

	item->SetActionFuncAndCookie(controlpanelwindow_MessageListCallback, (void *)(uint32)message);

	bool const 	isAddBottom	= !g_theProfileDB->GetValueByName("RecentAtTop");
	m_messageList->InsertItem(item, isAddBottom ? m_messageList->NumItems() : 0);

	if (initializing)
	{
		// Postpone ranger updates until all messages have been added.
		return;
	}

	aui_Ranger *ranger = m_messageList->GetVerticalRanger();
	Assert(ranger);
	if (!ranger)
		return;

	ranger->SetValue
		(ranger->GetValueX(),
		 isAddBottom ? ranger->GetMaximumY() : ranger->GetMinimumY()
		);

	m_messageList->RangerMoved();
}

void ControlPanelWindow::SetMessageRead(Message const & msg)
{
	for(sint32 n = 0; n < m_messageList->NumItems(); n++)
	{
		ctp2_ListItem *item = (ctp2_ListItem *)m_messageList->GetItemByIndex(n);

		if(!item)
			break;

		Message imsg; imsg.m_id = (uint32)item->GetCookie();
		if(imsg.m_id == msg.m_id)
		{
			ctp2_Static *staticContainer = (ctp2_Static *)item->GetChildByIndex(0);
			if(!staticContainer)
				break;

			ctp2_Static *staticThing = (ctp2_Static *)staticContainer->GetChildByIndex(1);
			if(!staticThing)
				break;

			staticThing->SetTextColor(g_colorSet->GetColorRef(COLOR_GRAY));
			staticThing->ShouldDraw(TRUE);
		}
	}
}

void ControlPanelWindow::RemoveMessage(Message &message)
{
	if (!m_messageList)
		return;

	sint32			numItems = m_messageList->NumItems();
	ctp2_ListItem	*listItem = NULL;

	for (sint32 i = 0; i < numItems; i++)
	{
		listItem = (ctp2_ListItem *)m_messageList->GetItemByIndex(i);
		if (listItem->GetCookie() == (void *)(uint32)message)
		{
			m_messageList->RemoveItemByIndex(i);
			delete listItem;
			break;
		}
	}
}

void ControlPanelWindow::PopulateMessageList(PLAYER_INDEX player)
{
	static PLAYER_INDEX		oldIndex = 0;

	Assert(m_messageList);
	if (!m_messageList)
		return;

	Assert(g_player[player]);
	if (!g_player[player])
		return;

	if (player == oldIndex) return;

	DynamicArray<Message>	*playerMessages = g_player[player]->GetMessages();
	Assert(playerMessages);
	if (!playerMessages)
		return;

	m_messageList->Clear();

	m_messageList->BuildListStart();

	bool const 		isAddBottom	= !g_theProfileDB->GetValueByName("RecentAtTop");
	sint32 const	copyCount	= playerMessages->Num();

	if (isAddBottom)
	{
		for (sint32 i = copyCount - 1; i >= 0; --i)
		{
			AddMessage(playerMessages->Access(i), true);
		}
	}
	else
	{
		// Reordered to have the most recent message at the top.
		for (sint32 i = 0; i < copyCount; ++i)
		{
			AddMessage(playerMessages->Access(i), true);
		}
	}

	m_messageList->BuildListEnd(isAddBottom);
}

void ControlPanelWindow::InitCityTab(void)
{
	aui_Ldl::SetActionFuncAndCookie("ControlPanelWindow.ControlPanel.ControlTabPanel.CityTab.TabPanel.MainDropdown"  ,CityPanelDropDownCallback,NULL);
	aui_Ldl::SetActionFuncAndCookie("ControlPanelWindow.ControlPanel.ControlTabPanel.CityTab.TabPanel.NextCityButton",CityPanelNextCityCallback,NULL);

	m_mainDropDown = (ctp2_DropDown *)aui_Ldl::GetObject("ControlPanelWindow.ControlPanel.ControlTabPanel.CityTab.TabPanel", "MainDropdown");

	m_cityHappiness   =(ctp2_Static *)aui_Ldl::GetObject("ControlPanelWindow.ControlPanel.ControlTabPanel.CityTab.TabPanel", "CityHappiness");
	m_cityPopulation  =(ctp2_Static *)aui_Ldl::GetObject("ControlPanelWindow.ControlPanel.ControlTabPanel.CityTab.TabPanel", "CityPopulation");
	m_cityGrowth	  =(ctp2_Static *)aui_Ldl::GetObject("ControlPanelWindow.ControlPanel.ControlTabPanel.CityTab.TabPanel", "CityGrowth");
	m_buildingItem	  =(ctp2_Static *)aui_Ldl::GetObject("ControlPanelWindow.ControlPanel.ControlTabPanel.CityTab.TabPanel", "BuildingItem");
	m_turnsRemaining  =(ctp2_Static *)aui_Ldl::GetObject("ControlPanelWindow.ControlPanel.ControlTabPanel.CityTab.TabPanel", "TurnsRemaining");
}

void ControlPanelWindow::InitUnitTab(void)
{
	aui_Ldl::SetActionFuncAndCookie("ControlPanelWindow.ControlPanel.ControlTabPanel.UnitTab.TabPanel.NextUnit",UnitPanelNextUnitCallback,NULL);

	m_unitImage   = (ctp2_Static*)aui_Ldl::GetObject("ControlPanelWindow.ControlPanel.ControlTabPanel.UnitTab.TabPanel","UnitIcon");

	m_unitType    = (ctp2_Static*)aui_Ldl::GetObject("ControlPanelWindow.ControlPanel.ControlTabPanel.UnitTab.TabPanel","UnitData");
	m_unitMove    = (ctp2_Static*)aui_Ldl::GetObject("ControlPanelWindow.ControlPanel.ControlTabPanel.UnitTab.TabPanel","MoveData");
	m_unitAttack  = (ctp2_Static*)aui_Ldl::GetObject("ControlPanelWindow.ControlPanel.ControlTabPanel.UnitTab.TabPanel","AttackData");
	m_unitDefense = (ctp2_Static*)aui_Ldl::GetObject("ControlPanelWindow.ControlPanel.ControlTabPanel.UnitTab.TabPanel","DefenseData");
	m_unitStatus  = (ctp2_Static*)aui_Ldl::GetObject("ControlPanelWindow.ControlPanel.ControlTabPanel.UnitTab.TabPanel","StatusData");
	m_unitOrders  = (ctp2_ListBox*)aui_Ldl::GetObject("ControlPanelWindow.ControlPanel.ControlTabPanel.UnitTab.TabPanel","OrderListBox");
	// Added by E: unitaction here?
	if (m_unitOrders!=NULL)
		m_unitOrders->SetActionFuncAndCookie(UnitPanelListBoxCallback,NULL);
}

void ControlPanelWindow::InitTileImpTab(void)
{
	CreateTileImpBanks   ();

	m_currentTerrainSelection = CP_TILEIMP_LAND;
	m_activatorButtons[m_currentTerrainSelection]->SetToggleState(TRUE);

	TileImpPanelRedisplay();
}

bool ControlPanelWindow::IsLand(const TerrainRecord *rec)
{
	if (rec==NULL)
		return false;

	if (rec->GetMovementType() & (k_Terrain_MovementType_Land_Bit    |
								  k_Terrain_MovementType_Mountain_Bit))
			return true;

	return false;
}

bool ControlPanelWindow::IsOcean(const TerrainRecord *rec)
{
	if (rec==NULL)
		return false;

	if (rec->GetMovementType()&(k_Terrain_MovementType_Sea_Bit           |
								k_Terrain_MovementType_ShallowWater_Bit))
			return true;

	return false;
}

void ControlPanelWindow::CreateTileImpBanks()
{
	m_activatorButtons[CP_TILEIMP_LAND   ]=(ctp2_Button*)aui_Ldl::GetObject("ControlPanelWindow.ControlPanel.ControlTabPanel.TilesTab.TabPanel.LandImpButton"	);
	m_activatorButtons[CP_TILEIMP_OCEAN  ]=(ctp2_Button*)aui_Ldl::GetObject("ControlPanelWindow.ControlPanel.ControlTabPanel.TilesTab.TabPanel.OceanImpButton"	);
	m_activatorButtons[CP_TILEIMP_SPECIAL]=(ctp2_Button*)aui_Ldl::GetObject("ControlPanelWindow.ControlPanel.ControlTabPanel.TilesTab.TabPanel.SpecialImpButton");
	m_activatorButtons[CP_TERRAFORM_LAND] = (ctp2_Button *)aui_Ldl::GetObject("ControlPanelWindow.ControlPanel.ControlTabPanel.TilesTab.TabPanel.TerraformLandButton");
	//emod to utilize all 64 slots for tileimps
	m_activatorButtons[CP_TERRAFORM_OCEAN] = (ctp2_Button *)aui_Ldl::GetObject("ControlPanelWindow.ControlPanel.ControlTabPanel.TilesTab.TabPanel.TerraformOceanButton");
//	m_activatorButtons[CP_EXTRA] = (ctp2_Button *)aui_Ldl::GetObject("ControlPanelWindow.ControlPanel.ControlTabPanel.TilesTab.TabPanel.ExtraButton");


	m_tileImpPanes[CP_TILEIMP_LAND   ]=(ctp2_Static*)aui_Ldl::GetObject("ControlPanelWindow.ControlPanel.ControlTabPanel.TilesTab.TabPanel.tiLandButtonBank"   );
	m_tileImpPanes[CP_TILEIMP_OCEAN  ]=(ctp2_Static*)aui_Ldl::GetObject("ControlPanelWindow.ControlPanel.ControlTabPanel.TilesTab.TabPanel.tiOceanButtonBank"  );
	m_tileImpPanes[CP_TILEIMP_SPECIAL]=(ctp2_Static*)aui_Ldl::GetObject("ControlPanelWindow.ControlPanel.ControlTabPanel.TilesTab.TabPanel.tiSpecialButtonBank");
	m_tileImpPanes[CP_TERRAFORM_LAND ]=(ctp2_Static*)aui_Ldl::GetObject("ControlPanelWindow.ControlPanel.ControlTabPanel.TilesTab.TabPanel.tfLandButtonBank"   );
	//emod to utilize all 64 spaces
	m_tileImpPanes[CP_TERRAFORM_OCEAN ]=(ctp2_Static*)aui_Ldl::GetObject("ControlPanelWindow.ControlPanel.ControlTabPanel.TilesTab.TabPanel.tfOceanButtonBank"   );
//	m_tileImpPanes[CP_EXTRA ]=(ctp2_Static*)aui_Ldl::GetObject("ControlPanelWindow.ControlPanel.ControlTabPanel.TilesTab.TabPanel.tfExtraButtonBank"   );

	for(sint32 i = 0; i < CP_TILEIMP_MAX; i++)
	{
		if ((m_activatorButtons[i]!=NULL)&&(i<CP_TILEIMP_MAX))
			m_activatorButtons[i]->SetActionFuncAndCookie(TileImpSelectionCallback,(void *)i);

		if (m_tileImpPanes[i]!=NULL)
			m_tileImpPanes[i]->Hide();
	}

	std::fill(m_tileImpButtons, m_tileImpButtons + CP_MAX_TILEIMPBUTTONS, (ctp2_Button *) NULL);
	std::fill(m_terraFormButtons, m_terraFormButtons + CP_MAX_TILEIMPBUTTONS, (ctp2_Button *) NULL);

	sint32		index;
	MBCHAR		button_id[256];
	MBCHAR		*thePaneLDL;
	ctp2_Button	*a_button;
	const	TerrainImprovementRecord *timpRec;

	const	IconRecord *irec;
	bool	found;
	uint32	row = 0, column = 0, theClass = 0, group_id = 0;

	char *panels[CP_TILEIMP_MAX] = {
			"tiLandButtonBank",
			"tiSpecialButtonBank",
			"tiOceanButtonBank",
			"tfLandButtonBank",
			"tfOceanButtonBank", //emod
			//"tfExtraButtonBank", //emod

	};
	sint32 panel;
	for(panel = 0; panel < CP_TILEIMP_MAX; panel++)
	{
		for(column = 0; column < CP_TILEIMP_COLS; column++)
		{
			for(row = 0; row < CP_TILEIMP_ROWS; row++)
			{
				sprintf(button_id,"ControlPanelWindow.ControlPanel.ControlTabPanel.TilesTab.TabPanel.%s.b%d%d",panels[panel],column+1,row+1);

				a_button=(ctp2_Button*)aui_Ldl::GetObject(button_id);
				Assert(a_button);
				if(a_button)
				{
					a_button->ExchangeImage(4,0,NULL);
					a_button->Enable(FALSE);
				}
			}
		}
	}

	for(sint32 current = 0; current < g_theTerrainImprovementDB->NumRecords(); current++)
	{
		found		= false;
		group_id	= 0;

		timpRec=g_theTerrainImprovementDB->Get(current);

		if (timpRec!=NULL)
		{
			found = true;
			theClass=timpRec->GetClass();

			switch(theClass)
			{
			case	k_TerrainImprovement_Class_Terraform_Bit    :
			case	k_TerrainImprovement_Class_Oceanform_Bit    :
					timpRec->GetColumn((sint32 &)column);
					break;
			case	k_TerrainImprovement_Class_Farm_Bit			:
			case	k_TerrainImprovement_Class_OceanFarm_Bit	:
			case	k_TerrainImprovement_Class_Structure1_Bit	:
					column = 0;
					break;
			case	k_TerrainImprovement_Class_Road_Bit			:
			case	k_TerrainImprovement_Class_OceanMine_Bit	:
			case	k_TerrainImprovement_Class_Structure2_Bit	:
					column = 1;
					break;
			case	k_TerrainImprovement_Class_Mine_Bit			:
			case	k_TerrainImprovement_Class_OceanATM_Bit	:
			case	k_TerrainImprovement_Class_LandDetector_Bit :
					column = 2;
					break;
			case	k_TerrainImprovement_Class_ATM_Bit			:
			case	k_TerrainImprovement_Class_OceanRoad_Bit		:
			case	k_TerrainImprovement_Class_OceanDetector_Bit:
					column = 3;
					break;
			default:
					found = false;
			}
		}

		thePaneLDL="tiLandButtonBank";
		group_id  = CP_TILEIMP_LAND;

		if (theClass>k_TerrainImprovement_Class_ATM_Bit)
		{
			if(
			       theClass >  k_TerrainImprovement_Class_OceanRoad_Bit
			    && theClass != k_TerrainImprovement_Class_Terraform_Bit
			    && theClass != k_TerrainImprovement_Class_Oceanform_Bit
			  )
			{
				thePaneLDL="tiSpecialButtonBank";
				group_id  = CP_TILEIMP_SPECIAL;
			}
			else if(theClass == k_TerrainImprovement_Class_Terraform_Bit)
			{
				sint32 terrain;
				timpRec->GetTerraformTerrainIndex(terrain);

				thePaneLDL = "tfLandButtonBank";
				group_id = CP_TERRAFORM_LAND;

				sint32 col;
				timpRec->GetColumn(col);
				column = (uint32)col;
			}
			else if(theClass == k_TerrainImprovement_Class_Oceanform_Bit)
			{
				sint32 terrain;
				timpRec->GetTerraformTerrainIndex(terrain);

				thePaneLDL = "tfOceanButtonBank";
				group_id = CP_TERRAFORM_OCEAN;

				sint32 col;
				timpRec->GetColumn(col);
				column = (uint32)col;
			}
			else
			{
				thePaneLDL="tiOceanButtonBank";
				group_id  = CP_TILEIMP_OCEAN;
			}
		}

		if (found)
		{
			row		= timpRec->GetLevel();

			sprintf(button_id,"ControlPanelWindow.ControlPanel.ControlTabPanel.TilesTab.TabPanel.%s.b%d%d",thePaneLDL,column+1,row);

			a_button=(ctp2_Button*)aui_Ldl::GetObject(button_id);

			if (a_button==NULL)
			{
#if defined(_DEBUG)
				if (row < CP_TILEIMP_MAX)
				{
					Assert(button_id=="Could not find button");
				}
				// else: No report. Used by a lot of mods for inaccessible tile improvements.
#endif // _DEBUG
				continue;
			}
			else
			{
				a_button->SetActionFuncAndCookie(TileImpButtonCallback2,(void *)current);

				irec = timpRec->GetIcon();

				a_button->SetStatusText(g_theStringDB->GetNameStr( timpRec->GetStatusbar() ));
				aui_TipWindow *tipwin = (aui_TipWindow *)a_button->GetTipWindow();
				if(!tipwin)
				{
					AUI_ERRCODE errcode = AUI_ERRCODE_OK;
					tipwin = new aui_TipWindow(&errcode, aui_UniqueId(), "DefaultTipWindow");
					Assert( AUI_NEWOK(tipwin,errcode) );
					if ( AUI_NEWOK(tipwin,errcode) )
					{
						aui_Ldl::Remove( tipwin );

						a_button->SetTipWindow( tipwin);
					}
				}

				if(tipwin)
					tipwin->SetTipText((char *)g_theStringDB->GetNameStr( timpRec->GetTooltip() ));
			}

			if (row>0)
				row--;

			index=CP_MAX_BUTTONS_PER_BANK*group_id+row*CP_TILEIMP_COLS+column;

			if (index<CP_MAX_TILEIMPBUTTONS)
				m_tileImpButtons[index] = a_button;
		}
	}

	row = column = 0;

#if 0

	for(current=0;current<g_theTerrainDB->NumRecords();current++)
	{
		found	 = false;
		terraRec =g_theTerrainDB->Get(current);

		if (terraRec==NULL)
			continue;

		if (column>=CP_TILEIMP_COLS)
			continue;

		thePaneLDL="tfLandButtonBank";
		group_id  = CP_TILEIMP_LAND;

		if ((!IsLand(terraRec))&&(!IsOcean(terraRec)))
			continue;

		sprintf(button_id,"ControlPanelWindow.ControlPanel.ControlTabPanel.TilesTab.TabPanel.%s.b%d%d",thePaneLDL,column+1,row+1);

		a_button=(ctp2_Button*)aui_Ldl::GetObject(button_id);

		index=CP_MAX_BUTTONS_PER_BANK*group_id+row*CP_TILEIMP_COLS+column;

		if (index<CP_MAX_TILEIMPBUTTONS)
		{
			if (a_button==NULL)
			{
				Assert(button_id=="Could not find button");
				continue;
			}
			else
			{
				a_button->SetActionFuncAndCookie(TileImpButtonCallback2,(void *)terraRec->GetIndex());

				irec = timpRec->GetIcon();

				if (irec!=NULL)
					a_button->ExchangeImage(4, 0, irec->GetIcon());

				m_terraFormButtons[index] = a_button;

				row ++;

				if (row>=CP_TILEIMP_ROWS)
				{
					row = 0;
					column ++;
				}
			}
		}
	}
#endif
}

void ControlPanelWindow::ActivateTileImpBank(unsigned int group_id)
{
	if (m_terraFormMode)
	   group_id += CP_TERRAFORM_LAND;

	if ((g_selected_item==NULL)||(group_id>=CP_TILEIMP_MAX))
		return;

	if ( (m_activatorButtons[group_id]->GetToggleState() == 0) &&
	      m_tileImpPanes[group_id] )
	{
		m_tileImpPanes[group_id]->Hide();
	}
	else
	{
		for(uint32 index=0;index<CP_TILEIMP_MAX;index++)
		{
			if (m_tileImpPanes[index]==NULL)
				continue;

			if (index!=group_id)
			{
				m_activatorButtons[index]->SetToggleState(0);
				m_tileImpPanes[index]->Hide();
			}
			else
			{
				if (!m_activatorButtons[index]->GetToggleState())
					m_activatorButtons[index]->SetToggleState(1);
				m_tileImpPanes[index]->Show();
			}
		}
	}

	m_currentTerrainSelection = group_id;

	TileImpPanelRedisplay();
}

void ControlPanelWindow::AppendItem(ctp2_ListBox * a_List, MBCHAR *string)
//void ScenarioEditor::AddAddItem(ctp2_ListBox * a_List, const MBCHAR *text, sint32 userData)
//void ScenarioEditor::AddAddItem(ctp2_ListBox *list, const MBCHAR *text, sint32 userData)
{
	if ((a_List==NULL)||(string==NULL))
		return;

	ctp2_ListItem *item = (ctp2_ListItem *) aui_Ldl::BuildHierarchyFromRoot("ControlPopupItem");

	if (item==NULL)
		return;
	c3_Static *staticThing = (c3_Static *)item->GetChildByIndex(0);

	if (staticThing==NULL)
		return;

	staticThing->SetText(string);

	a_List->AddItem(item);
}

void ControlPanelWindow::BuildUnitList()
{
	if (m_contextMenu==NULL)
		return;

	Army army=UnitPanelGetCurrent();

	if (!army.IsValid())
	{
		MapPoint pos = g_selected_item->GetCurSelectPos();
		Cell *cell = g_theWorld->GetCell(pos);
		if(cell->AccessUnit(0).GetOwner() == g_selected_item->GetVisiblePlayer())
		{
			Unit top = cell->UnitArmy()->GetTopVisibleUnit(g_selected_item->GetVisiblePlayer());

			if(!top.IsValid())
				return;

			if(top.IsEntrenched())
			{
				return;
			}

			army = top.GetArmy();
		}

		if (!army.IsValid())
		{
			return;
		}
	}

	if (army.Num())
	{
		m_contextMenu->AddItem(g_theStringDB->GetNameStr("CONTEXT_ARMY_MANAGEMENT"), NULL, (void *)k_UNIT_CONTEXT_ARMY_MANAGER);
		if(army->NumOrders() > 0) {
			m_contextMenu->AddItem(g_theStringDB->GetNameStr("CONTEXT_CLEAR_ORDERS"), NULL, (void *)k_UNIT_CONTEXT_CLEAR_ORDERS);
		}

		ArmyData *  data = army.AccessData();
		Assert(data);
		Cell *      cell = g_theWorld->GetCell(data->RetPos());
		if(cell->GetNumUnits() != data->Num())
		{
			m_contextMenu->AddItem(g_theStringDB->GetNameStr("CONTEXT_GROUP_ALL"), NULL, (void *)k_UNIT_CONTEXT_GROUP_ALL);
		}

		for(sint32 i = 0; i < cell->GetNumUnits(); i++)
		{
			if(cell->AccessUnit(i).GetArmy().Num() > 1)
			{
				m_contextMenu->AddItem(g_theStringDB->GetNameStr("CONTEXT_UNGROUP_ALL"), NULL, (void *)k_UNIT_CONTEXT_UNGROUP_ALL);
				break;
			}
		}

		if(data->HasCargo())
		{
			m_contextMenu->AddItem(g_theStringDB->GetNameStr("CONTEXT_VIEW_CARGO"), NULL, (void *)k_UNIT_CONTEXT_TRANSPORT_VIEW);
		}

		for (sint32 i = 0; i < g_theOrderDB->NumRecords(); i++)
		{
			OrderRecord const * rec = g_theOrderDB->Get(i);

			if (rec==NULL)
				continue;

			if(stricmp(rec->GetIDText(), "ORDER_ENSLAVE_SETTLER") == 0)
				continue;

			ORDER_TEST	test=data->TestOrder(rec);

			if (test!=ORDER_TEST_ILLEGAL)
			{
				sint32 string_index=rec->GetLocalizedName();

				MBCHAR order[k_MAX_NAME_LEN];
				strcpy(order, "  ");
				strcat(order, g_theStringDB->GetNameStr(string_index));

				m_contextMenu->AddItem(order, NULL,(void *)i);
			}
		}
	}
}

void ControlPanelWindow::BuildUnitListBox()
{
	if (m_unitOrders==NULL)
		return;

	m_unitOrders->Clear();

	Army army=UnitPanelGetCurrent();

	if (!army.IsValid())
		return;

	if (army.Num())
	{
		const OrderRecord	*rec;
		ArmyData			*data;
		ctp2_ListItem		*item;
		c3_Static			*staticThing;

		m_unitOrders->BuildListStart();

		for (sint32 i=0;i<g_theOrderDB->NumRecords();i++)
		{
			rec=g_theOrderDB->Get(i);

			data = army.AccessData();

			if ((rec==NULL)||(data==NULL))
				continue;

			ORDER_TEST	test=data->TestOrder(rec);

			if (test != ORDER_TEST_ILLEGAL)
			{
				sint32 string_index=rec->GetLocalizedName();

				const MBCHAR *string=g_theStringDB->GetNameStr(string_index);

				item = (ctp2_ListItem *) aui_Ldl::BuildHierarchyFromRoot("UnitInfoListItem");

				staticThing = (c3_Static *)item->GetChildByIndex(0);

				staticThing->SetText(string);

				item->SetUserData((void *)rec);

				m_unitOrders->AddItem(item);
			}
		}
	}
	m_unitOrders->BuildListEnd();
}

void ControlPanelWindow::BuildCityList (const MapPoint &pos)
{
	if (m_contextMenu==NULL)
		return;

	m_contextMenu->AddItem(g_theStringDB->GetNameStr("CONTEXT_CITY_VIEW"), NULL, (void *)k_CONTEXT_CITY_VIEW);
	m_contextMenu->AddItem(g_theStringDB->GetNameStr("CONTEXT_CITY_BUILD"), NULL, (void *)k_CONTEXT_CITY_BUILD);
	m_contextMenu->AddItem(g_theStringDB->GetNameStr("CONTEXT_CITY_RENAME"), NULL, (void *)k_CONTEXT_CITY_RENAME);
	m_contextMenu->AddItem(g_theStringDB->GetNameStr("CONTEXT_CITY_DISBAND"), NULL, (void *)k_CONTEXT_CITY_DISBAND);

	Cell *cell = g_theWorld->GetCell(pos);
	sint32 i;
	for(i = 0; i < cell->GetNumUnits(); i++)
	{
		if(cell->AccessUnit(i).IsEntrenched() ||
		   cell->AccessUnit(i).IsAsleep())
		{
			m_contextMenu->AddItem(g_theStringDB->GetNameStr("CONTEXT_CITY_ACTIVATE"), NULL, (void *)k_CONTEXT_CITY_ACTIVATE_UNITS);
			break;
		}
	}

	Unit city = cell->GetCity();
	Assert(city.IsValid());
	if(city.IsValid())
	{
		if(city.CD()->GetUseGovernor())
		{
			m_contextMenu->AddItem(g_theStringDB->GetNameStr("CONTEXT_CITY_MAYOR_OFF"), NULL, (void *)k_CONTEXT_CITY_TOGGLE_GOVERNOR);
		}
		else
		{
			m_contextMenu->AddItem(g_theStringDB->GetNameStr("CONTEXT_CITY_MAYOR_ON"), NULL, (void *)k_CONTEXT_CITY_TOGGLE_GOVERNOR);
		}
	}
}

void ControlPanelWindow::BuildList (sint32 index)
{
	m_targetingMode=CP_TARGETING_MODE_OFF;
	m_currentOrder =NULL;

	if (index>=SELECT_TYPE_MAX)
		return;

	if (m_contextMenu==NULL)
		return;

	m_contextMenu->Clear();

	if(index == SELECT_TYPE_NONE)
		return;

	MapPoint pos = g_selected_item->GetCurSelectPos();
	Cell *cell = g_theWorld->GetCell(pos);
	if(cell->GetNumUnits() > 0)
	{
		if(cell->AccessUnit(0).GetOwner() == g_selected_item->GetVisiblePlayer())
		{
			BuildUnitList();
		}
	}

	if(cell->HasCity())
	{
		if(cell->GetCity().GetOwner() == g_selected_item->GetVisiblePlayer())
		{
			BuildCityList(pos);
		}
	}

	if(g_theStringDB->GetNameStr("CONTEXT_TILE_INFO"))
	{
		m_contextMenu->AddItem(g_theStringDB->GetNameStr("CONTEXT_TILE_INFO"), NULL, (void *)k_CONTEXT_MISC_TILE_INFO);
	}
}

void ControlPanelWindow::DeActivateSelectedInfo()
{
	if (m_contextMenu!=NULL)
		m_contextMenu->Close();
}

void ControlPanelWindow::ActivateSelectedInfo(sint32 type)
{
	if (m_contextMenu!=NULL)
	{
		BuildList(type);

		m_contextMenu->Open();
	}
}

void ControlPanelWindow::Select	()
{
	UnitPanelRedisplay();
}

void ControlPanelWindow::Deselect()
{
	UnitPanelRedisplay();
}

sint32 ControlPanelWindow::X()
{
	if (m_mainWindow!=NULL)
		return m_mainWindow->X();

	return 0;
}

sint32 ControlPanelWindow::Y()
{
	if (m_mainWindow!=NULL)
		return m_mainWindow->Y();

	return 0;
}

sint32 ControlPanelWindow::Width()
{
	if (m_mainWindow!=NULL)
		return m_mainWindow->Width();

	return 0;
}

sint32 ControlPanelWindow::Height()
{
	if (m_mainWindow!=NULL)
		return m_mainWindow->Height();

	return 0;
}

void ControlPanelWindow::Idle()
{
	if(g_tiledMap && !g_tiledMap->ReadyToDraw())
		return;

	MainControlPanel::Update();

	ctp2_Static *tileImpPanel = static_cast<ctp2_Static*>(
		aui_Ldl::GetObject(
		"ControlPanelWindow.ControlPanel.ControlTabPanel.TilesTab.TabPanel"));
	if(!tileImpPanel->IsHidden())
		TileImpPanelRedisplay();

	if (m_mainWindow!=NULL)
		m_mainWindow-> Idle();

	if ((g_tiledMap!=NULL)&&(m_contextMenu!=NULL))
	{
		if (g_tiledMap->IsScrolling())
			m_contextMenu->Close();
	}

	if (m_targetingMode)
		TargetingMode();
	else
		// prevent resetting of scroll-cursor
		if (!g_tiledMap->IsScrolling())
			g_cursorManager->SetCursor(CURSORINDEX_DEFAULT);
}

void ControlPanelWindow::Move( sint32 x, sint32 y )
{
	if (m_mainWindow!=NULL)
		m_mainWindow->Move(x,y);
}

void ControlPanelWindow::Offset( sint32 dx, sint32 dy )
{
	if (m_mainWindow!=NULL)
		m_mainWindow->Offset(dx,dy);
}

void ControlPanelWindow::Resize( sint32 width, sint32 height )
{
	if (m_mainWindow!=NULL)
		m_mainWindow->Resize(width,height);
}

void ControlPanelWindow::AdjustToScreen()
{
	sint32 basex=g_ScreenWidth - Width();
	sint32 basey=g_ScreenHeight - Height();
	Move(basex,basey);
}

void ControlPanelWindow::SetControlText(aui_Control *control,MBCHAR *fmt,...)
{
	if ((control==NULL)||(fmt==NULL))
		return;

	va_list          v_args;
	char			 text[512];

	va_start(v_args, fmt);
	vsprintf(text,fmt,v_args);
	va_end( v_args );

	control->SetText(text);
	control->ShouldDraw(TRUE);
}

void ControlPanelWindow::PollCIVStatus()
{
	if ((g_selected_item==NULL) || (g_player==NULL))
		return;

	sint32 p_index = g_selected_item->GetVisiblePlayer();

	Player *current=g_player[p_index];

	if (current==NULL)
		return;

	sint32		numCities=current->GetNumCities();
	sint32		totalPop =current->GetGold();
	sint32		turnLeft =current->m_advances->TurnsToNextAdvance();
	AdvanceType	advance	 =current->m_advances->GetResearching();

	const AdvanceRecord *rec=g_theAdvanceDB->Get(advance);

	SetControlText((aui_Control *)m_numCities	  ,"%d",numCities);
	SetControlText((aui_Control *)m_totalPop	  ,"%d",totalPop );
	SetControlText((aui_Control *)m_turnsToAdvance,"%d",turnLeft );

	if (rec!=NULL)
		SetControlText((aui_Control *)m_currentAdvance,"%s",rec->GetNameText());
}

void ControlPanelWindow::PollUNITStatus()
{
}

void ControlPanelWindow::PollTILEIMPStatus()
{
#if 0   /// @todo Find out what this code was supposed to do
	if ((g_selected_item==NULL)||(g_player==NULL))
		return;

	sint32 p_index = g_selected_item->GetVisiblePlayer();

	Player *current=g_player[p_index];

	if (current==NULL)
		return;

	sint32		pw=current->GetMaterialsStored();
#endif
}

void ControlPanelWindow::HappinessRedisplay(aui_Surface *surface,RECT &rect,void *cookie)
{
	if (g_selected_item==NULL)
		return;

	sint32 hapvals[3];
	float total;

	g_player[g_selected_item->GetVisiblePlayer()]->CountCityHappiness(hapvals[0],hapvals[1],hapvals[2]);

	total = (float)(hapvals[0]+hapvals[1]+hapvals[2]);

	if(total<=0.0f)
		primitives_PaintRect16(surface, &rect, g_colorSet->GetPlayerColor(g_selected_item->GetCurPlayer()));
	else
	{
		RECT tmp=rect;

		float width = (float)(rect.right-rect.left);

		Pixel16 colors[3]={
							g_colorSet->GetColor(COLOR_RED),
							g_colorSet->GetColor(COLOR_YELLOW),
							g_colorSet->GetColor(COLOR_GREEN)
						  };

		for (uint32 i=0;i<3;i++)
		{
			tmp.right = tmp.left + (sint32)(((float)hapvals[i]/total)*width);
			primitives_PaintRect16(surface, &tmp, colors[i]);

			tmp.left = tmp.right;
		}
	}
}

Unit ControlPanelWindow::CityPanelGetCurrent()
{
	Unit    city;

	if (m_mainDropDown)
	{
		Player * current = g_player[g_selected_item->GetVisiblePlayer()];

		if (current && current->GetNumCities())
		{
			city = current->GetCityFromIndex(m_mainDropDown->GetSelectedItem());
		}
	}

	return city;
}

void ControlPanelWindow::CityPanelRebuild()
{
	return;
#if 0   // Unreachable
	if (m_mainDropDown==NULL)
		return;

	m_mainDropDown->Clear();

	sint32 p_index = g_selected_item->GetVisiblePlayer();

	Player *current=g_player[p_index];

	if (current==NULL)
		return;

	sint32 num=current->GetNumCities();

	if (num<1)
		return;

	Unit city;
	const MBCHAR *name;
	ctp2_ListItem *item;
	c3_Static *staticThing;

	for(sint32 i=0;i<num;i++)
	{
		city = g_player[g_selected_item->GetVisiblePlayer()]->GetCityFromIndex(i);

		name=city.GetName();

		item = (ctp2_ListItem *) aui_Ldl::BuildHierarchyFromRoot("InitPlayListItem");

		staticThing = (c3_Static *)item->GetChildByIndex(0);

		staticThing->SetText(name);

		m_mainDropDown->AddItem(item);
	}
#endif
}

void
ControlPanelWindow::CityPanelRedisplay()
{
	Unit city=CityPanelGetCurrent();

	if (!city.IsValid())
		return;

	CityData *c=city.GetCityData();

	sint32 pop;
	double happiness	=c->GetHappiness();
	sint32 growth		=c->GetGrowthRate();
	BuildQueue		*q	=c->GetBuildQueue();

	MBCHAR  *buildName		=q->GetName();
	sint32  buildRemaining	=c->HowMuchLonger();

	c->GetPop(pop);;

	SetControlText((aui_Control *)m_cityHappiness  ,"%3.2f",happiness);
	SetControlText((aui_Control *)m_cityPopulation ,"%d",pop);
	SetControlText((aui_Control *)m_cityGrowth	   ,"%d",growth);
	SetControlText((aui_Control *)m_buildingItem   ,buildName);
	SetControlText((aui_Control *)m_turnsRemaining ,"%d",buildRemaining);
}

void ControlPanelWindow::CityPanelNextCity()
{
	if (m_mainDropDown==NULL)
		return;

	sint32 p_index = g_selected_item->GetVisiblePlayer();

	Player *current=g_player[p_index];

	if (current==NULL)
		return;

	sint32 num=current->GetNumCities();

	if (num<1)
		return;

	sint32 index=m_mainDropDown->GetSelectedItem();

	index ++;

	if (index>=num)
		index = 0;

	m_mainDropDown->SetSelectedItem(index);

	CityPanelRedisplay();
}

Army ControlPanelWindow::UnitPanelGetCurrent()
{
	Army army;

	sint32 p_index = g_selected_item->GetVisiblePlayer();

	Player *current=g_player[p_index];

	if (current==NULL)
		return army;

	PLAYER_INDEX	pIndex;
	ID				id;
	SELECT_TYPE		sType;

	g_selected_item->GetTopCurItem(pIndex,id,sType);

	if(sType==SELECT_TYPE_LOCAL_ARMY )
	{
		return Army(id);
	}

	return army;
}

void ControlPanelWindow::UnitPanelRedisplay()
{
	return;
#if 0   // Unreachable
	Army army=UnitPanelGetCurrent();

	Unit unit;

	if (army.IsValid())
		unit=army[0];

	std::string   name("None");
	const MBCHAR *order="";

	double attack =0.0;
	double defense=0.0;
	double movement=0.0;

	if (unit.IsValid())
	{
		name.assign(unit.GetDisplayName());
		attack  =unit.GetAttack();
		defense =unit.GetDefense();
		movement=unit.GetMovementPoints();

		StringId str_id;

		if (unit.GetCurrentOrderString(str_id))
			order =g_theStringDB->GetNameStr(str_id);
	}

	BuildUnitListBox();

	SetControlText((aui_Control *)m_unitType	,"%s"	,name.c_str());
	SetControlText((aui_Control *)m_unitAttack	,"%3.2f",attack  );
	SetControlText((aui_Control *)m_unitDefense	,"%3.2f",defense );
	SetControlText((aui_Control *)m_unitMove	,"%3.2f",movement);
	SetControlText((aui_Control *)m_unitStatus	,"%s"	,order);

	if (unit.IsValid())
	{
		const IconRecord *irec = unit.GetDBRec()->GetDefaultIcon();

		const MBCHAR *unitIconName = irec->GetLargeIcon();

		if(unitIconName && strcmp(unitIconName, "NULL"))
		{
			m_unitImage->SetImage((char *)unitIconName,
				0, AUI_IMAGEBASE_SUBSTATE_STATE);
			m_unitImage->SetImage((char *)unitIconName,
				0, AUI_IMAGEBASE_SUBSTATE_STATEACTIVE);
			m_unitImage->Enable(TRUE);
			m_unitImage->ShouldDraw(TRUE);
		}
		else
		{
			m_unitImage->SetImage((char *)irec->GetIcon(),
				0, AUI_IMAGEBASE_SUBSTATE_STATE);
			m_unitImage->SetImage((char *)irec->GetIcon(),
				0, AUI_IMAGEBASE_SUBSTATE_STATEACTIVE);
			m_unitImage->Enable(TRUE);
			m_unitImage->ShouldDraw(TRUE);
		}
	}

	static_cast<aui_Control*>(
		aui_Ldl::GetObject(
		"ControlPanelWindow.ControlPanel.ControlTabPanel.UnitTab.TabPanel"
		))->ShouldDraw();
#endif
}

void ControlPanelWindow::TileImpButtonRedisplay(uint32 player_id,uint32 button)
{
	//emod TODO  removing obsolete tileimps

	if (button>=CP_MAX_TILEIMPBUTTONS)
		return;

	if (m_tileImpButtons[button]==NULL)
		return;

	const	TerrainImprovementRecord *rec;
	bool	show_button,grey_button;

	rec = g_theTerrainImprovementDB->Get((sint32)m_tileImpButtons[button]->GetCookie());

	Assert(rec != NULL);

	if (rec==NULL)
	{
		m_tileImpButtons[button]->Enable(false);
		return;
	}

	bool const	hideExpensive	= !g_theProfileDB->GetValueByName("ShowExpensive");

	grey_button = !terrainutil_CanPlayerBuild(rec,player_id, hideExpensive);
	show_button = terrainutil_PlayerHasAdvancesFor(rec, player_id);  //emod this needs to see obsolete tileimps

	aui_TipWindow *tipwin = (aui_TipWindow *)m_tileImpButtons[button]->GetTipWindow();

	if(!show_button)
	{
		m_tileImpButtons[button]->ExchangeImage(4, 0, NULL);
		if(tipwin)
			tipwin->SetTipText("");
	}
	else if(!rec->GetClassTerraform() || !rec->GetClassOceanform())
	{
		const IconRecord *irec = rec->GetIcon();
		m_tileImpButtons[button]->ExchangeImage(4, 0, irec->GetIcon());
		if(tipwin)
			tipwin->SetTipText((char *)g_theStringDB->GetNameStr(rec->GetTooltip()));
	}
	else
	{
		MBCHAR iconName[30];
		sint32 terrain;
		rec->GetTerraformTerrainIndex(terrain);
		sprintf(iconName, "UPNI%02d.tga", g_theTerrainDB->Get(terrain)->GetTilesetIndex());
		m_tileImpButtons[button]->ExchangeImage(4, 0, iconName);
		if(tipwin)
			tipwin->SetTipText((char *)g_theStringDB->GetNameStr(rec->GetTooltip()));
	}

	m_tileImpButtons[button]->Enable(!grey_button);
}

void ControlPanelWindow::TerraformButtonRedisplay(uint32 player_id,uint32 button)
{
	if (button>=CP_MAX_TILEIMPBUTTONS)
		return;

	if (m_terraFormButtons[button]==NULL)
		return;

	const	TerrainRecord *rec = g_theTerrainDB->Get(sint32(m_terraFormButtons[button]->GetCookie()));

	Assert(rec != NULL);

	if (rec==NULL)
	{
		m_tileImpButtons[button]->Enable(false);
		return;
	}

	const	IconRecord *irec = rec->GetIcon();

	if (irec!=NULL)
	{
		m_terraFormButtons[button]->ExchangeImage(4, 0, irec->GetIcon());
#if 0
		aui_TipWindow *tipwin = (aui_TipWindow *)m_terraFormButtons[button]->GetTipWindow();
		if(tipwin) {
			tipwin->SetTipText((char *)g_theStringDB->GetNameStr(irec->GetTooltip()));
		}
#endif
	}

	m_tileImpButtons[button]->Enable(true);
}

void ControlPanelWindow::TileImpPanelRedisplay()
{
	ctp2_Static *tileImpPanel = static_cast<ctp2_Static*>(
		aui_Ldl::GetObject(
		"ControlPanelWindow.ControlPanel.ControlTabPanel.TilesTab.TabPanel"));

	if(tileImpPanel->IsHidden())
		return;

	if ((g_selected_item==NULL)||(m_currentTerrainSelection>=CP_TILEIMP_MAX))
		return;

	for(uint32 index=0;index<CP_TILEIMP_MAX;index++)
	{
		if (m_tileImpPanes[index]==NULL)
			continue;

		if (index!=m_currentTerrainSelection)
		{
			if(!m_tileImpPanes[index]->IsHidden())
			{
				m_tileImpPanes[index]->Hide();
				m_activatorButtons[index]->SetToggleState(FALSE);
			}
		}
		else
		{
			if(m_tileImpPanes[index]->IsHidden())
			{
				m_tileImpPanes[index]->Show();
				m_activatorButtons[index]->SetToggleState(TRUE);
			}
		}
	}

	uint32	start=(m_currentTerrainSelection*CP_MAX_BUTTONS_PER_BANK);
	uint32	end	=(start+CP_MAX_BUTTONS_PER_BANK);
	sint32	p1	= g_selected_item->GetVisiblePlayer();
	uint32	button;

	for (button=start;button<end;button++)
	{
		TileImpButtonRedisplay(p1,button);
	}

	m_tileImpPanes[m_currentTerrainSelection]->ShouldDraw();
}

void ControlPanelWindow::ToggleTerraforming()
{
	m_terraFormMode	= !m_terraFormMode;

	uint32 newindex=(m_terraFormMode?CP_TERRAFORM_LAND:CP_TILEIMP_LAND);

	ActivateTileImpBank(newindex);

	TileImpPanelRedisplay();
}

void
ControlPanelWindow::SetStack(Army const &selectedArmy, CellUnitList *fullArmy, Unit singleUnit)
{
}

void ThrowPartyUtilityDialogBoxCallback(MBCHAR *text, sint32 val2, void *data)
{
}

void controlpanelwindow_Update(Unit *city)
{
}

void cpw_NumberToCommas( uint64 number, MBCHAR *s )
{
	uint64 temp = number;
	sint32 trillion = (sint32)(temp / k_CPW_TRILLION);
	temp -= (uint64)trillion * k_CPW_TRILLION;
	sint32 billion = (sint32)(temp / k_CPW_BILLION);
	temp -= (uint64)billion * k_CPW_BILLION;
	sint32 million = (sint32)(temp / k_CPW_MILLION);
	temp -= (uint64)million * k_CPW_MILLION;
	sint32 thousand = (sint32)(temp / k_CPW_THOUSAND);
	temp -= (uint64)thousand * k_CPW_THOUSAND;

	MBCHAR c[_MAX_PATH];
	sprintf( c, g_theStringDB->GetNameStr("str_ldl_comma") );

	if ( trillion )
	{
		sprintf( s, "%ld%s%.3ld%s%.3ld%s%.3ld%s%.3" PRIu64, trillion, c, billion, c, million, c, thousand, c, temp );
	}
	else if ( billion )
	{
		sprintf( s, "%ld%s%.3ld%s%.3ld%s%.3" PRIu64, billion, c, million, c, thousand, c, temp );
	}
	else if ( million )
	{
		sprintf( s, "%ld%s%.3ld%s%.3" PRIu64, million, c, thousand, c, temp );
	}
	else if ( thousand )
	{
		sprintf( s, "%ld%s%.3" PRIu64, thousand, c, temp );
	}
	else
	{
		sprintf( s, "%" PRIu64, temp );
	}
}

void ControlPanelWindow::TabCallback(aui_Control *control, uint32 action,
									 uint32 data, void *cookie)
{
	CP_TAB tab = (CP_TAB)(sint32)cookie;

	if(action == ctp2_Tab::ACTION_ACTIVATED)
	{
		g_controlPanel->ClearTargetingMode();

		switch(tab)
		{
			case CP_TAB_UNIT:
				g_mainControlPanel->UnitPanelActivated();
				break;
			case CP_TAB_CITY:
				g_mainControlPanel->CityPanelActivated();
				break;
			case CP_TAB_MSGLOG:
				ctp2_Static *tabLabel;
				tabLabel = (static_cast<ctp2_Static*>(aui_Ldl::GetObject("ControlPanelWindow.ControlPanel.ControlTabPanel.MessageTab.TabButton.Label")));
				tabLabel->SetTextColor(g_colorSet->GetColorRef(COLOR_BUTTON_TEXT_PLAIN));
				tabLabel->ShouldDraw(TRUE);
				break;
		}
	}
}

void ControlPanelWindow::SetTab(CP_TAB tab)
{
	Assert(m_tabGroup);
	if(!m_tabGroup) return;

	Assert(tab > CP_TAB_INVALID && tab < CP_TAB_LAST);
	if(tab <= CP_TAB_INVALID || tab >= CP_TAB_LAST) return;

	Assert(m_tabs[tab]);
	if(!m_tabs[tab]) return;

	m_tabGroup->SelectTab(m_tabs[tab]);
}

AUI_ERRCODE ControlPanelWindow::UpdatePlayerBeginProgress(sint32 currentPlayer)
{
	sint32 visiblePlayer = g_selected_item->GetVisiblePlayer();

	if(currentPlayer == visiblePlayer || g_network.IsActive())
	{
	}
	else
	{
		Assert(1 || g_theProgressWindow);
		if (g_theProgressWindow)
			g_theProgressWindow->SetBarColor(g_colorSet->GetColorRef(g_colorSet->ComputePlayerColor(currentPlayer)));
	}

	return AUI_ERRCODE_OK;
}

static sint32 s_totalPlayers = 0;

AUI_ERRCODE ControlPanelWindow::UpdatePlayerEndProgress(sint32 currentPlayer)
{
	sint32 visiblePlayer = g_selected_item->GetVisiblePlayer();

	if(currentPlayer == visiblePlayer)
	{
		s_totalPlayers = 0;
		for(int i = 0; i < k_MAX_PLAYERS; i++)
			if(g_player[i]) s_totalPlayers++;

		if(0 && !g_network.IsActive()) // Never true
		{
			ProgressWindow::BeginProgress(
				g_theProgressWindow,
				"TurnProgressWindow",
				s_totalPlayers);

			g_theProgressWindow->SetType(AUI_WINDOW_TYPE_SINKING);

			g_theProgressWindow->Move(0, g_radarWindow->Y() - g_theProgressWindow->Height());
		}

		m_currentProgress = 1;

		if(g_theProgressWindow)
		{
			g_theProgressWindow->SetStronglyModal(false);
			g_theProgressWindow->StartCountingTo(m_currentProgress, (char*)g_theStringDB->GetNameStr("str_COMPUTER_TURNS"));
		}
	}
	else
	{
		m_currentProgress++;

		if(g_theProgressWindow && 0 && !g_network.IsActive())
		{
			g_theProgressWindow->StartCountingTo(m_currentProgress);
		}
	}

	if(0 && !g_network.IsActive()) // Never true
	{
		if (g_selected_item->GetCurPlayer() == NewTurnCount::GetStopPlayer())
		{
			ProgressWindow::EndProgress( g_theProgressWindow );

			if(g_theProgressWindow)
			{
				g_theProgressWindow->SetType(AUI_WINDOW_TYPE_FLOATING);
			}
		}
	}

	return AUI_ERRCODE_OK;
}

BOOL ControlPanelWindow::CursorOverControlPanel()
{
	sint32 x = g_c3ui->TheMouse()->X();
	sint32 y = g_c3ui->TheMouse()->Y();

	sint32 basex=g_ScreenWidth - Width();
	sint32 basey=g_ScreenHeight - Height();

	return ( (x > basex) && (x < g_ScreenWidth) && (y > basey) && (y < g_ScreenHeight) );
}

void ControlPanelWindow::Toggle()
{
	if(!m_mainWindow)
		return;

	if(g_c3ui->GetWindow(m_mainWindow->Id()))
	{
		g_c3ui->RemoveWindow(m_mainWindow->Id());
	}
	else
	{
		g_c3ui->AddWindow(m_mainWindow);
		m_mainWindow->ShouldDraw(TRUE);
	}
}

void ControlPanelWindow::Hide()
{
	if(!m_mainWindow)
		return;

	g_c3ui->RemoveWindow(m_mainWindow->Id());
}

void ControlPanelWindow::Show()
{
	if(!m_mainWindow) return;

	if(!g_c3ui->GetWindow(m_mainWindow->Id()))
	{
		g_c3ui->AddWindow(m_mainWindow);
	}
}
