//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : Selection item click handling
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
// - Bug fix: prevent easy invisible unit detection, by reporting the basic
//   terrain info when (right-)clicking on an enemy object.
// - Added selection of goods. (Oct 8th 2005 Martin Gühmann)
// - Added option to open cities or the army manager if there are units
//   on top of a city. (Oct 8th 2005 Martin Gühmann)
// - Treat entrenching units like entrenched units. (Oct 16th 2005 Martin Gühmann)
// - Standardized code (May 21st 2006 Martin Gühmann)
// - Added debug pathing for the city astar. (17-Jan-2008 Martin Gühmann)
//
//----------------------------------------------------------------------------

#include "c3.h"
#include "SelItem.h"    // SelItemClick.h does not exist

#include "Army.h"
#include "armymanagerwindow.h"
#include "aui.h"
#include "aui_mouse.h"
#include "aui_uniqueid.h"
#include "Cell.h"
#include "citywindow.h"
#include "controlpanelwindow.h"
#include "GameEventManager.h"
#include "gfx_options.h"
#include "helptile.h"
#include "Path.h"
#include "player.h"
#include "profileDB.h"
#include "QuickSlic.h"
#include "SlicObject.h"
#include "SlicEngine.h"
#include "Unit.h"
#include "UnitData.h"
#include "UnitRecord.h"
#include "World.h"

void SelectedItem::SetupClickFunctions()
{
	SELECT_MODE mode;
	SELECT_BUTTON button;
	SELECT_TYPE selected;
	SELECT_TYPE clicked;

	m_justNowSelectedArmy = false;

	m_justGotDoubleClick = false;

	for(selected = SELECT_TYPE_NONE; selected < SELECT_TYPE_MAX; selected = SELECT_TYPE(sint32(selected) + 1)) {
		for(clicked = SELECT_TYPE_NONE; clicked < SELECT_TYPE_MAX; clicked = SELECT_TYPE(sint32(clicked) + 1)) {
			for(button = SELECT_BUTTON_LEFT; button < SELECT_BUTTON_MAX; button = SELECT_BUTTON(sint32(button) + 1)) {
				for(mode = SELECT_MODE_RIGHT; mode < SELECT_MODE_MAX; mode = SELECT_MODE(sint32(mode) + 1)) {
					m_clickFunc[selected][clicked][button][mode] = &SelectedItem::NullClick;
				}
			}
		}
	}

	mode	= SELECT_MODE_RIGHT;
	button	= SELECT_BUTTON_LEFT;

	selected = SELECT_TYPE_NONE;

	m_clickFunc[selected][SELECT_TYPE_NONE]                [button][mode] = &SelectedItem::NullClick;
	m_clickFunc[selected][SELECT_TYPE_LOCAL_ARMY]          [button][mode] = &SelectedItem::SelectArmyClick;
	m_clickFunc[selected][SELECT_TYPE_LOCAL_ARMY_UNLOADING][button][mode] = &SelectedItem::ErrorClick;
	m_clickFunc[selected][SELECT_TYPE_LOCAL_CITY]          [button][mode] = &SelectedItem::SelectCityClick;
	m_clickFunc[selected][SELECT_TYPE_REMOTE_CITY]         [button][mode] = &SelectedItem::SelectEnemyCityClick;
	m_clickFunc[selected][SELECT_TYPE_REMOTE_ARMY]         [button][mode] = &SelectedItem::SelectEnemyArmyClick;
	m_clickFunc[selected][SELECT_TYPE_TRADE_ROUTE]         [button][mode] = &SelectedItem::SelectTradeRouteClick;
	m_clickFunc[selected][SELECT_TYPE_GOOD]                [button][mode] = &SelectedItem::SelectGoodClick;

	mode = SELECT_MODE_RIGHT;
	button = SELECT_BUTTON_LEFT;
	selected = SELECT_TYPE_LOCAL_ARMY;

	m_clickFunc[selected][SELECT_TYPE_NONE]                [button][mode] = &SelectedItem::DeselectClick;
	m_clickFunc[selected][SELECT_TYPE_LOCAL_ARMY]          [button][mode] = &SelectedItem::SelectArmyClick;
	m_clickFunc[selected][SELECT_TYPE_LOCAL_ARMY_UNLOADING][button][mode] = &SelectedItem::ErrorClick;
	m_clickFunc[selected][SELECT_TYPE_LOCAL_CITY]          [button][mode] = &SelectedItem::SelectCityClick;
	m_clickFunc[selected][SELECT_TYPE_REMOTE_CITY]         [button][mode] = &SelectedItem::SelectEnemyCityClick;
	m_clickFunc[selected][SELECT_TYPE_REMOTE_ARMY]         [button][mode] = &SelectedItem::SelectEnemyArmyClick;
	m_clickFunc[selected][SELECT_TYPE_TRADE_ROUTE]         [button][mode] = &SelectedItem::SelectTradeRouteClick;
	m_clickFunc[selected][SELECT_TYPE_GOOD]                [button][mode] = &SelectedItem::SelectGoodClick;

	mode = SELECT_MODE_RIGHT;
	button = SELECT_BUTTON_LEFT;
	selected = SELECT_TYPE_LOCAL_CITY;

	m_clickFunc[selected][SELECT_TYPE_NONE]                [button][mode] = &SelectedItem::DeselectClick;
	m_clickFunc[selected][SELECT_TYPE_LOCAL_ARMY]          [button][mode] = &SelectedItem::SelectArmyClick;
	m_clickFunc[selected][SELECT_TYPE_LOCAL_ARMY_UNLOADING][button][mode] = &SelectedItem::ErrorClick;
	m_clickFunc[selected][SELECT_TYPE_LOCAL_CITY]          [button][mode] = &SelectedItem::SelectCityClick;
	m_clickFunc[selected][SELECT_TYPE_REMOTE_CITY]         [button][mode] = &SelectedItem::SelectEnemyCityClick;
	m_clickFunc[selected][SELECT_TYPE_REMOTE_ARMY]         [button][mode] = &SelectedItem::SelectEnemyArmyClick;
	m_clickFunc[selected][SELECT_TYPE_TRADE_ROUTE]         [button][mode] = &SelectedItem::SelectTradeRouteClick;
	m_clickFunc[selected][SELECT_TYPE_GOOD]                [button][mode] = &SelectedItem::SelectGoodClick;

	mode = SELECT_MODE_RIGHT;
	button = SELECT_BUTTON_LEFT;
	selected = SELECT_TYPE_LOCAL_ARMY_UNLOADING;

	m_clickFunc[selected][SELECT_TYPE_NONE]                [button][mode] = &SelectedItem::UnloadClick;
	m_clickFunc[selected][SELECT_TYPE_LOCAL_ARMY]          [button][mode] = &SelectedItem::UnloadClick;
	m_clickFunc[selected][SELECT_TYPE_LOCAL_ARMY_UNLOADING][button][mode] = &SelectedItem::ErrorClick;
	m_clickFunc[selected][SELECT_TYPE_LOCAL_CITY]          [button][mode] = &SelectedItem::UnloadClick;
	m_clickFunc[selected][SELECT_TYPE_REMOTE_CITY]         [button][mode] = &SelectedItem::UnloadClick;
	m_clickFunc[selected][SELECT_TYPE_REMOTE_ARMY]         [button][mode] = &SelectedItem::UnloadClick;
	m_clickFunc[selected][SELECT_TYPE_TRADE_ROUTE]         [button][mode] = &SelectedItem::UnloadClick;
	m_clickFunc[selected][SELECT_TYPE_GOOD]                [button][mode] = &SelectedItem::UnloadClick;

	mode = SELECT_MODE_RIGHT;
	button = SELECT_BUTTON_LEFT;
	selected = SELECT_TYPE_REMOTE_CITY;

	m_clickFunc[selected][SELECT_TYPE_NONE]                [button][mode] = &SelectedItem::DeselectClick;
	m_clickFunc[selected][SELECT_TYPE_LOCAL_ARMY]          [button][mode] = &SelectedItem::SelectArmyClick;
	m_clickFunc[selected][SELECT_TYPE_LOCAL_ARMY_UNLOADING][button][mode] = &SelectedItem::ErrorClick;
	m_clickFunc[selected][SELECT_TYPE_LOCAL_CITY]          [button][mode] = &SelectedItem::SelectCityClick;
	m_clickFunc[selected][SELECT_TYPE_REMOTE_CITY]         [button][mode] = &SelectedItem::SelectEnemyCityClick;
	m_clickFunc[selected][SELECT_TYPE_REMOTE_ARMY]         [button][mode] = &SelectedItem::SelectEnemyArmyClick;
	m_clickFunc[selected][SELECT_TYPE_TRADE_ROUTE]         [button][mode] = &SelectedItem::SelectTradeRouteClick;
	m_clickFunc[selected][SELECT_TYPE_GOOD]                [button][mode] = &SelectedItem::SelectGoodClick;

	mode = SELECT_MODE_RIGHT;
	button = SELECT_BUTTON_LEFT;
	selected = SELECT_TYPE_REMOTE_ARMY;

	m_clickFunc[selected][SELECT_TYPE_NONE]                [button][mode] = &SelectedItem::DeselectClick;
	m_clickFunc[selected][SELECT_TYPE_LOCAL_ARMY]          [button][mode] = &SelectedItem::SelectArmyClick;
	m_clickFunc[selected][SELECT_TYPE_LOCAL_ARMY_UNLOADING][button][mode] = &SelectedItem::ErrorClick;
	m_clickFunc[selected][SELECT_TYPE_LOCAL_CITY]          [button][mode] = &SelectedItem::SelectCityClick;
	m_clickFunc[selected][SELECT_TYPE_REMOTE_CITY]         [button][mode] = &SelectedItem::SelectEnemyCityClick;
	m_clickFunc[selected][SELECT_TYPE_REMOTE_ARMY]         [button][mode] = &SelectedItem::SelectEnemyArmyClick;
	m_clickFunc[selected][SELECT_TYPE_TRADE_ROUTE]         [button][mode] = &SelectedItem::SelectTradeRouteClick;
	m_clickFunc[selected][SELECT_TYPE_GOOD]                [button][mode] = &SelectedItem::SelectGoodClick;

	mode = SELECT_MODE_RIGHT;
	button = SELECT_BUTTON_LEFT;
	selected = SELECT_TYPE_GOOD;

	m_clickFunc[selected][SELECT_TYPE_NONE]                [button][mode] = &SelectedItem::DeselectClick;
	m_clickFunc[selected][SELECT_TYPE_LOCAL_ARMY]          [button][mode] = &SelectedItem::SelectArmyClick;
	m_clickFunc[selected][SELECT_TYPE_LOCAL_ARMY_UNLOADING][button][mode] = &SelectedItem::ErrorClick;
	m_clickFunc[selected][SELECT_TYPE_LOCAL_CITY]          [button][mode] = &SelectedItem::SelectCityClick;
	m_clickFunc[selected][SELECT_TYPE_REMOTE_CITY]         [button][mode] = &SelectedItem::SelectEnemyCityClick;
	m_clickFunc[selected][SELECT_TYPE_REMOTE_ARMY]         [button][mode] = &SelectedItem::SelectEnemyArmyClick;
	m_clickFunc[selected][SELECT_TYPE_TRADE_ROUTE]         [button][mode] = &SelectedItem::SelectTradeRouteClick;
	m_clickFunc[selected][SELECT_TYPE_GOOD]                [button][mode] = &SelectedItem::SelectGoodClick;

	mode = SELECT_MODE_RIGHT;
	button = SELECT_BUTTON_LEFT;
	selected = SELECT_TYPE_TRADE_ROUTE;

	m_clickFunc[selected][SELECT_TYPE_NONE]                [button][mode] = &SelectedItem::DeselectClick;
	m_clickFunc[selected][SELECT_TYPE_LOCAL_ARMY]          [button][mode] = &SelectedItem::SelectArmyClick;
	m_clickFunc[selected][SELECT_TYPE_LOCAL_ARMY_UNLOADING][button][mode] = &SelectedItem::ErrorClick;
	m_clickFunc[selected][SELECT_TYPE_LOCAL_CITY]          [button][mode] = &SelectedItem::SelectCityClick;
	m_clickFunc[selected][SELECT_TYPE_REMOTE_CITY]         [button][mode] = &SelectedItem::SelectEnemyCityClick;
	m_clickFunc[selected][SELECT_TYPE_REMOTE_ARMY]         [button][mode] = &SelectedItem::SelectEnemyArmyClick;
	m_clickFunc[selected][SELECT_TYPE_TRADE_ROUTE]         [button][mode] = &SelectedItem::SelectTradeRouteClick;
	m_clickFunc[selected][SELECT_TYPE_GOOD]                [button][mode] = &SelectedItem::SelectGoodClick;

	mode = SELECT_MODE_RIGHT;
	button = SELECT_BUTTON_RIGHT;
	selected = SELECT_TYPE_NONE;

	m_clickFunc[selected][SELECT_TYPE_NONE]                [button][mode] = &SelectedItem::TerrainContextClick;
	m_clickFunc[selected][SELECT_TYPE_LOCAL_ARMY]          [button][mode] = &SelectedItem::ArmyContextClick;
	m_clickFunc[selected][SELECT_TYPE_LOCAL_ARMY_UNLOADING][button][mode] = &SelectedItem::ErrorClick;
	m_clickFunc[selected][SELECT_TYPE_LOCAL_CITY]          [button][mode] = &SelectedItem::CityContextClick;
	m_clickFunc[selected][SELECT_TYPE_REMOTE_CITY]         [button][mode] = &SelectedItem::EnemyCityContextClick;
	m_clickFunc[selected][SELECT_TYPE_REMOTE_ARMY]         [button][mode] = &SelectedItem::EnemyArmyContextClick;
	m_clickFunc[selected][SELECT_TYPE_TRADE_ROUTE]         [button][mode] = &SelectedItem::TradeRouteContextClick;
	m_clickFunc[selected][SELECT_TYPE_GOOD]                [button][mode] = &SelectedItem::GoodContextClick;

	mode = SELECT_MODE_RIGHT;
	button = SELECT_BUTTON_RIGHT;
	selected = SELECT_TYPE_LOCAL_ARMY;

	m_clickFunc[selected][SELECT_TYPE_NONE]                [button][mode] = &SelectedItem::MoveArmyClick;
	m_clickFunc[selected][SELECT_TYPE_LOCAL_ARMY]          [button][mode] = &SelectedItem::MoveArmyClick;
	m_clickFunc[selected][SELECT_TYPE_LOCAL_ARMY_UNLOADING][button][mode] = &SelectedItem::ErrorClick;
	m_clickFunc[selected][SELECT_TYPE_LOCAL_CITY]          [button][mode] = &SelectedItem::MoveArmyClick;
	m_clickFunc[selected][SELECT_TYPE_REMOTE_CITY]         [button][mode] = &SelectedItem::MoveArmyClick;
	m_clickFunc[selected][SELECT_TYPE_REMOTE_ARMY]         [button][mode] = &SelectedItem::MoveArmyClick;
	m_clickFunc[selected][SELECT_TYPE_TRADE_ROUTE]         [button][mode] = &SelectedItem::MoveArmyClick;
	m_clickFunc[selected][SELECT_TYPE_GOOD]                [button][mode] = &SelectedItem::MoveArmyClick;

	mode = SELECT_MODE_RIGHT;
	button = SELECT_BUTTON_RIGHT;
	selected = SELECT_TYPE_LOCAL_CITY;

	m_clickFunc[selected][SELECT_TYPE_NONE]                [button][mode] = &SelectedItem::TerrainContextClick;
	m_clickFunc[selected][SELECT_TYPE_LOCAL_ARMY]          [button][mode] = &SelectedItem::ArmyContextClick;
	m_clickFunc[selected][SELECT_TYPE_LOCAL_ARMY_UNLOADING][button][mode] = &SelectedItem::ErrorClick;
	m_clickFunc[selected][SELECT_TYPE_LOCAL_CITY]          [button][mode] = &SelectedItem::CityContextClick;
	m_clickFunc[selected][SELECT_TYPE_REMOTE_CITY]         [button][mode] = &SelectedItem::EnemyCityContextClick;
	m_clickFunc[selected][SELECT_TYPE_REMOTE_ARMY]         [button][mode] = &SelectedItem::EnemyArmyContextClick;
	m_clickFunc[selected][SELECT_TYPE_TRADE_ROUTE]         [button][mode] = &SelectedItem::TradeRouteContextClick;
	m_clickFunc[selected][SELECT_TYPE_GOOD]                [button][mode] = &SelectedItem::GoodContextClick;

	mode = SELECT_MODE_RIGHT;
	button = SELECT_BUTTON_RIGHT;
	selected = SELECT_TYPE_LOCAL_ARMY_UNLOADING;

	m_clickFunc[selected][SELECT_TYPE_NONE]                [button][mode] = &SelectedItem::UnloadClick;
	m_clickFunc[selected][SELECT_TYPE_LOCAL_ARMY]          [button][mode] = &SelectedItem::UnloadClick;
	m_clickFunc[selected][SELECT_TYPE_LOCAL_ARMY_UNLOADING][button][mode] = &SelectedItem::ErrorClick;
	m_clickFunc[selected][SELECT_TYPE_LOCAL_CITY]          [button][mode] = &SelectedItem::UnloadClick;
	m_clickFunc[selected][SELECT_TYPE_REMOTE_CITY]         [button][mode] = &SelectedItem::UnloadClick;
	m_clickFunc[selected][SELECT_TYPE_REMOTE_ARMY]         [button][mode] = &SelectedItem::UnloadClick;
	m_clickFunc[selected][SELECT_TYPE_TRADE_ROUTE]         [button][mode] = &SelectedItem::UnloadClick;
	m_clickFunc[selected][SELECT_TYPE_GOOD]                [button][mode] = &SelectedItem::UnloadClick;

	mode = SELECT_MODE_RIGHT;
	button = SELECT_BUTTON_RIGHT;
	selected = SELECT_TYPE_REMOTE_CITY;

	m_clickFunc[selected][SELECT_TYPE_NONE]                [button][mode] = &SelectedItem::TerrainContextClick;
	m_clickFunc[selected][SELECT_TYPE_LOCAL_ARMY]          [button][mode] = &SelectedItem::ArmyContextClick;
	m_clickFunc[selected][SELECT_TYPE_LOCAL_ARMY_UNLOADING][button][mode] = &SelectedItem::ErrorClick;
	m_clickFunc[selected][SELECT_TYPE_LOCAL_CITY]          [button][mode] = &SelectedItem::CityContextClick;
	m_clickFunc[selected][SELECT_TYPE_REMOTE_CITY]         [button][mode] = &SelectedItem::EnemyCityContextClick;
	m_clickFunc[selected][SELECT_TYPE_REMOTE_ARMY]         [button][mode] = &SelectedItem::EnemyArmyContextClick;
	m_clickFunc[selected][SELECT_TYPE_TRADE_ROUTE]         [button][mode] = &SelectedItem::TradeRouteContextClick;
	m_clickFunc[selected][SELECT_TYPE_GOOD]                [button][mode] = &SelectedItem::GoodContextClick;

	mode = SELECT_MODE_RIGHT;
	button = SELECT_BUTTON_RIGHT;
	selected = SELECT_TYPE_REMOTE_ARMY;

	m_clickFunc[selected][SELECT_TYPE_NONE]                [button][mode] = &SelectedItem::TerrainContextClick;
	m_clickFunc[selected][SELECT_TYPE_LOCAL_ARMY]          [button][mode] = &SelectedItem::ArmyContextClick;
	m_clickFunc[selected][SELECT_TYPE_LOCAL_ARMY_UNLOADING][button][mode] = &SelectedItem::ErrorClick;
	m_clickFunc[selected][SELECT_TYPE_LOCAL_CITY]          [button][mode] = &SelectedItem::CityContextClick;
	m_clickFunc[selected][SELECT_TYPE_REMOTE_CITY]         [button][mode] = &SelectedItem::EnemyCityContextClick;
	m_clickFunc[selected][SELECT_TYPE_REMOTE_ARMY]         [button][mode] = &SelectedItem::EnemyArmyContextClick;
	m_clickFunc[selected][SELECT_TYPE_TRADE_ROUTE]         [button][mode] = &SelectedItem::TradeRouteContextClick;
	m_clickFunc[selected][SELECT_TYPE_GOOD]                [button][mode] = &SelectedItem::GoodContextClick;

	mode = SELECT_MODE_RIGHT;
	button = SELECT_BUTTON_RIGHT;
	selected = SELECT_TYPE_GOOD;

	m_clickFunc[selected][SELECT_TYPE_NONE]                [button][mode] = &SelectedItem::TerrainContextClick;
	m_clickFunc[selected][SELECT_TYPE_LOCAL_ARMY]          [button][mode] = &SelectedItem::ArmyContextClick;
	m_clickFunc[selected][SELECT_TYPE_LOCAL_ARMY_UNLOADING][button][mode] = &SelectedItem::ErrorClick;
	m_clickFunc[selected][SELECT_TYPE_LOCAL_CITY]          [button][mode] = &SelectedItem::SendGoodClick;
	m_clickFunc[selected][SELECT_TYPE_REMOTE_CITY]         [button][mode] = &SelectedItem::EnemyCityContextClick;
	m_clickFunc[selected][SELECT_TYPE_REMOTE_ARMY]         [button][mode] = &SelectedItem::EnemyArmyContextClick;
	m_clickFunc[selected][SELECT_TYPE_TRADE_ROUTE]         [button][mode] = &SelectedItem::TradeRouteContextClick;
	m_clickFunc[selected][SELECT_TYPE_GOOD]                [button][mode] = &SelectedItem::GoodContextClick;

	mode = SELECT_MODE_RIGHT;
	button = SELECT_BUTTON_RIGHT;
	selected = SELECT_TYPE_TRADE_ROUTE;

	m_clickFunc[selected][SELECT_TYPE_NONE]                [button][mode] = &SelectedItem::TerrainContextClick;
	m_clickFunc[selected][SELECT_TYPE_LOCAL_ARMY]          [button][mode] = &SelectedItem::ArmyContextClick;
	m_clickFunc[selected][SELECT_TYPE_LOCAL_ARMY_UNLOADING][button][mode] = &SelectedItem::ErrorClick;
	m_clickFunc[selected][SELECT_TYPE_LOCAL_CITY]          [button][mode] = &SelectedItem::CityContextClick;
	m_clickFunc[selected][SELECT_TYPE_REMOTE_CITY]         [button][mode] = &SelectedItem::EnemyCityContextClick;
	m_clickFunc[selected][SELECT_TYPE_REMOTE_ARMY]         [button][mode] = &SelectedItem::EnemyArmyContextClick;
	m_clickFunc[selected][SELECT_TYPE_TRADE_ROUTE]         [button][mode] = &SelectedItem::TradeRouteContextClick;
	m_clickFunc[selected][SELECT_TYPE_GOOD]                [button][mode] = &SelectedItem::GoodContextClick;

	mode = SELECT_MODE_LEFT;
	button = SELECT_BUTTON_LEFT;
	selected = SELECT_TYPE_NONE;

	m_clickFunc[selected][SELECT_TYPE_NONE]                [button][mode] = &SelectedItem::NullClick;
	m_clickFunc[selected][SELECT_TYPE_LOCAL_ARMY]          [button][mode] = &SelectedItem::SelectArmyClick;
	m_clickFunc[selected][SELECT_TYPE_LOCAL_ARMY_UNLOADING][button][mode] = &SelectedItem::ErrorClick;
	m_clickFunc[selected][SELECT_TYPE_LOCAL_CITY]          [button][mode] = &SelectedItem::SelectCityClick;
	m_clickFunc[selected][SELECT_TYPE_REMOTE_CITY]         [button][mode] = &SelectedItem::SelectEnemyCityClick;
	m_clickFunc[selected][SELECT_TYPE_REMOTE_ARMY]         [button][mode] = &SelectedItem::SelectEnemyArmyClick;
	m_clickFunc[selected][SELECT_TYPE_TRADE_ROUTE]         [button][mode] = &SelectedItem::SelectTradeRouteClick;
	m_clickFunc[selected][SELECT_TYPE_GOOD]                [button][mode] = &SelectedItem::SelectGoodClick;

	mode = SELECT_MODE_LEFT;
	button = SELECT_BUTTON_LEFT;
	selected = SELECT_TYPE_LOCAL_ARMY;

	m_clickFunc[selected][SELECT_TYPE_NONE]                [button][mode] = &SelectedItem::MoveArmyClick;
	m_clickFunc[selected][SELECT_TYPE_LOCAL_ARMY]          [button][mode] = &SelectedItem::MoveArmyClick;
	m_clickFunc[selected][SELECT_TYPE_LOCAL_ARMY_UNLOADING][button][mode] = &SelectedItem::ErrorClick;
	m_clickFunc[selected][SELECT_TYPE_LOCAL_CITY]          [button][mode] = &SelectedItem::MoveArmyClick;
	m_clickFunc[selected][SELECT_TYPE_REMOTE_CITY]         [button][mode] = &SelectedItem::MoveArmyClick;
	m_clickFunc[selected][SELECT_TYPE_REMOTE_ARMY]         [button][mode] = &SelectedItem::MoveArmyClick;
	m_clickFunc[selected][SELECT_TYPE_TRADE_ROUTE]         [button][mode] = &SelectedItem::MoveArmyClick;
	m_clickFunc[selected][SELECT_TYPE_GOOD]                [button][mode] = &SelectedItem::MoveArmyClick;

	mode = SELECT_MODE_LEFT;
	button = SELECT_BUTTON_LEFT;
	selected = SELECT_TYPE_LOCAL_CITY;

	m_clickFunc[selected][SELECT_TYPE_NONE]                [button][mode] = &SelectedItem::DeselectClick;
	m_clickFunc[selected][SELECT_TYPE_LOCAL_ARMY]          [button][mode] = &SelectedItem::SelectArmyClick;
	m_clickFunc[selected][SELECT_TYPE_LOCAL_ARMY_UNLOADING][button][mode] = &SelectedItem::ErrorClick;
	m_clickFunc[selected][SELECT_TYPE_LOCAL_CITY]          [button][mode] = &SelectedItem::SelectCityClick;
	m_clickFunc[selected][SELECT_TYPE_REMOTE_CITY]         [button][mode] = &SelectedItem::SelectEnemyCityClick;
	m_clickFunc[selected][SELECT_TYPE_REMOTE_ARMY]         [button][mode] = &SelectedItem::SelectEnemyArmyClick;
	m_clickFunc[selected][SELECT_TYPE_TRADE_ROUTE]         [button][mode] = &SelectedItem::SelectTradeRouteClick;
	m_clickFunc[selected][SELECT_TYPE_GOOD]                [button][mode] = &SelectedItem::SelectGoodClick;

	mode = SELECT_MODE_LEFT;
	button = SELECT_BUTTON_LEFT;
	selected = SELECT_TYPE_LOCAL_ARMY_UNLOADING;

	m_clickFunc[selected][SELECT_TYPE_NONE]                [button][mode] = &SelectedItem::UnloadClick;
	m_clickFunc[selected][SELECT_TYPE_LOCAL_ARMY]          [button][mode] = &SelectedItem::UnloadClick;
	m_clickFunc[selected][SELECT_TYPE_LOCAL_ARMY_UNLOADING][button][mode] = &SelectedItem::ErrorClick;
	m_clickFunc[selected][SELECT_TYPE_LOCAL_CITY]          [button][mode] = &SelectedItem::UnloadClick;
	m_clickFunc[selected][SELECT_TYPE_REMOTE_CITY]         [button][mode] = &SelectedItem::UnloadClick;
	m_clickFunc[selected][SELECT_TYPE_REMOTE_ARMY]         [button][mode] = &SelectedItem::UnloadClick;
	m_clickFunc[selected][SELECT_TYPE_TRADE_ROUTE]         [button][mode] = &SelectedItem::UnloadClick;
	m_clickFunc[selected][SELECT_TYPE_GOOD]                [button][mode] = &SelectedItem::UnloadClick;

	mode = SELECT_MODE_LEFT;
	button = SELECT_BUTTON_LEFT;
	selected = SELECT_TYPE_REMOTE_CITY;

	m_clickFunc[selected][SELECT_TYPE_NONE]                [button][mode] = &SelectedItem::DeselectClick;
	m_clickFunc[selected][SELECT_TYPE_LOCAL_ARMY]          [button][mode] = &SelectedItem::SelectArmyClick;
	m_clickFunc[selected][SELECT_TYPE_LOCAL_ARMY_UNLOADING][button][mode] = &SelectedItem::ErrorClick;
	m_clickFunc[selected][SELECT_TYPE_LOCAL_CITY]          [button][mode] = &SelectedItem::SelectCityClick;
	m_clickFunc[selected][SELECT_TYPE_REMOTE_CITY]         [button][mode] = &SelectedItem::SelectEnemyCityClick;
	m_clickFunc[selected][SELECT_TYPE_REMOTE_ARMY]         [button][mode] = &SelectedItem::SelectEnemyArmyClick;
	m_clickFunc[selected][SELECT_TYPE_TRADE_ROUTE]         [button][mode] = &SelectedItem::SelectTradeRouteClick;
	m_clickFunc[selected][SELECT_TYPE_GOOD]                [button][mode] = &SelectedItem::SelectGoodClick;

	mode = SELECT_MODE_LEFT;
	button = SELECT_BUTTON_LEFT;
	selected = SELECT_TYPE_REMOTE_ARMY;

	m_clickFunc[selected][SELECT_TYPE_NONE]                [button][mode] = &SelectedItem::DeselectClick;
	m_clickFunc[selected][SELECT_TYPE_LOCAL_ARMY]          [button][mode] = &SelectedItem::SelectArmyClick;
	m_clickFunc[selected][SELECT_TYPE_LOCAL_ARMY_UNLOADING][button][mode] = &SelectedItem::ErrorClick;
	m_clickFunc[selected][SELECT_TYPE_LOCAL_CITY]          [button][mode] = &SelectedItem::SelectCityClick;
	m_clickFunc[selected][SELECT_TYPE_REMOTE_CITY]         [button][mode] = &SelectedItem::SelectEnemyCityClick;
	m_clickFunc[selected][SELECT_TYPE_REMOTE_ARMY]         [button][mode] = &SelectedItem::SelectEnemyArmyClick;
	m_clickFunc[selected][SELECT_TYPE_TRADE_ROUTE]         [button][mode] = &SelectedItem::SelectTradeRouteClick;
	m_clickFunc[selected][SELECT_TYPE_GOOD]                [button][mode] = &SelectedItem::SelectGoodClick;

	mode = SELECT_MODE_LEFT;
	button = SELECT_BUTTON_LEFT;
	selected = SELECT_TYPE_GOOD;

	m_clickFunc[selected][SELECT_TYPE_NONE]                [button][mode] = &SelectedItem::DeselectClick;
	m_clickFunc[selected][SELECT_TYPE_LOCAL_ARMY]          [button][mode] = &SelectedItem::SelectArmyClick;
	m_clickFunc[selected][SELECT_TYPE_LOCAL_ARMY_UNLOADING][button][mode] = &SelectedItem::ErrorClick;
	m_clickFunc[selected][SELECT_TYPE_LOCAL_CITY]          [button][mode] = &SelectedItem::SelectCityClick;
	m_clickFunc[selected][SELECT_TYPE_REMOTE_CITY]         [button][mode] = &SelectedItem::SelectEnemyCityClick;
	m_clickFunc[selected][SELECT_TYPE_REMOTE_ARMY]         [button][mode] = &SelectedItem::SelectEnemyArmyClick;
	m_clickFunc[selected][SELECT_TYPE_TRADE_ROUTE]         [button][mode] = &SelectedItem::SelectTradeRouteClick;
	m_clickFunc[selected][SELECT_TYPE_GOOD]                [button][mode] = &SelectedItem::SelectGoodClick;

	mode = SELECT_MODE_LEFT;
	button = SELECT_BUTTON_LEFT;
	selected = SELECT_TYPE_TRADE_ROUTE;

	m_clickFunc[selected][SELECT_TYPE_NONE]                [button][mode] = &SelectedItem::DeselectClick;
	m_clickFunc[selected][SELECT_TYPE_LOCAL_ARMY]          [button][mode] = &SelectedItem::SelectArmyClick;
	m_clickFunc[selected][SELECT_TYPE_LOCAL_ARMY_UNLOADING][button][mode] = &SelectedItem::ErrorClick;
	m_clickFunc[selected][SELECT_TYPE_LOCAL_CITY]          [button][mode] = &SelectedItem::SelectCityClick;
	m_clickFunc[selected][SELECT_TYPE_REMOTE_CITY]         [button][mode] = &SelectedItem::SelectEnemyCityClick;
	m_clickFunc[selected][SELECT_TYPE_REMOTE_ARMY]         [button][mode] = &SelectedItem::SelectEnemyArmyClick;
	m_clickFunc[selected][SELECT_TYPE_TRADE_ROUTE]         [button][mode] = &SelectedItem::SelectTradeRouteClick;
	m_clickFunc[selected][SELECT_TYPE_GOOD]                [button][mode] = &SelectedItem::SelectGoodClick;

	mode = SELECT_MODE_LEFT;
	button = SELECT_BUTTON_RIGHT;
	selected = SELECT_TYPE_NONE;

	m_clickFunc[selected][SELECT_TYPE_NONE]                [button][mode] = &SelectedItem::TerrainContextClick;
	m_clickFunc[selected][SELECT_TYPE_LOCAL_ARMY]          [button][mode] = &SelectedItem::ArmyContextClick;
	m_clickFunc[selected][SELECT_TYPE_LOCAL_ARMY_UNLOADING][button][mode] = &SelectedItem::ErrorClick;
	m_clickFunc[selected][SELECT_TYPE_LOCAL_CITY]          [button][mode] = &SelectedItem::CityContextClick;
	m_clickFunc[selected][SELECT_TYPE_REMOTE_CITY]         [button][mode] = &SelectedItem::EnemyCityContextClick;
	m_clickFunc[selected][SELECT_TYPE_REMOTE_ARMY]         [button][mode] = &SelectedItem::EnemyArmyContextClick;
	m_clickFunc[selected][SELECT_TYPE_TRADE_ROUTE]         [button][mode] = &SelectedItem::TradeRouteContextClick;
	m_clickFunc[selected][SELECT_TYPE_GOOD]                [button][mode] = &SelectedItem::GoodContextClick;

	mode = SELECT_MODE_LEFT;
	button = SELECT_BUTTON_RIGHT;
	selected = SELECT_TYPE_LOCAL_ARMY;

	m_clickFunc[selected][SELECT_TYPE_NONE]                [button][mode] = &SelectedItem::DeselectClick;
	m_clickFunc[selected][SELECT_TYPE_LOCAL_ARMY]          [button][mode] = &SelectedItem::DeselectClick;
	m_clickFunc[selected][SELECT_TYPE_LOCAL_ARMY_UNLOADING][button][mode] = &SelectedItem::ErrorClick;
	m_clickFunc[selected][SELECT_TYPE_LOCAL_CITY]          [button][mode] = &SelectedItem::DeselectClick;
	m_clickFunc[selected][SELECT_TYPE_REMOTE_CITY]         [button][mode] = &SelectedItem::DeselectClick;
	m_clickFunc[selected][SELECT_TYPE_REMOTE_ARMY]         [button][mode] = &SelectedItem::DeselectClick;
	m_clickFunc[selected][SELECT_TYPE_TRADE_ROUTE]         [button][mode] = &SelectedItem::DeselectClick;
	m_clickFunc[selected][SELECT_TYPE_GOOD]                [button][mode] = &SelectedItem::DeselectClick;

	mode = SELECT_MODE_LEFT;
	button = SELECT_BUTTON_RIGHT;
	selected = SELECT_TYPE_LOCAL_CITY;

	m_clickFunc[selected][SELECT_TYPE_NONE]                [button][mode] = &SelectedItem::TerrainContextClick;
	m_clickFunc[selected][SELECT_TYPE_LOCAL_ARMY]          [button][mode] = &SelectedItem::ArmyContextClick;
	m_clickFunc[selected][SELECT_TYPE_LOCAL_ARMY_UNLOADING][button][mode] = &SelectedItem::ErrorClick;
	m_clickFunc[selected][SELECT_TYPE_LOCAL_CITY]          [button][mode] = &SelectedItem::CityContextClick;
	m_clickFunc[selected][SELECT_TYPE_REMOTE_CITY]         [button][mode] = &SelectedItem::EnemyCityContextClick;
	m_clickFunc[selected][SELECT_TYPE_REMOTE_ARMY]         [button][mode] = &SelectedItem::EnemyArmyContextClick;
	m_clickFunc[selected][SELECT_TYPE_TRADE_ROUTE]         [button][mode] = &SelectedItem::TradeRouteContextClick;
	m_clickFunc[selected][SELECT_TYPE_GOOD]                [button][mode] = &SelectedItem::GoodContextClick;

	mode = SELECT_MODE_LEFT;
	button = SELECT_BUTTON_RIGHT;
	selected = SELECT_TYPE_LOCAL_ARMY_UNLOADING;

	m_clickFunc[selected][SELECT_TYPE_NONE]                [button][mode] = &SelectedItem::UnloadClick;
	m_clickFunc[selected][SELECT_TYPE_LOCAL_ARMY]          [button][mode] = &SelectedItem::UnloadClick;
	m_clickFunc[selected][SELECT_TYPE_LOCAL_ARMY_UNLOADING][button][mode] = &SelectedItem::ErrorClick;
	m_clickFunc[selected][SELECT_TYPE_LOCAL_CITY]          [button][mode] = &SelectedItem::UnloadClick;
	m_clickFunc[selected][SELECT_TYPE_REMOTE_CITY]         [button][mode] = &SelectedItem::UnloadClick;
	m_clickFunc[selected][SELECT_TYPE_REMOTE_ARMY]         [button][mode] = &SelectedItem::UnloadClick;
	m_clickFunc[selected][SELECT_TYPE_TRADE_ROUTE]         [button][mode] = &SelectedItem::UnloadClick;
	m_clickFunc[selected][SELECT_TYPE_GOOD]                [button][mode] = &SelectedItem::UnloadClick;

	mode = SELECT_MODE_LEFT;
	button = SELECT_BUTTON_RIGHT;
	selected = SELECT_TYPE_REMOTE_CITY;

	m_clickFunc[selected][SELECT_TYPE_NONE]                [button][mode] = &SelectedItem::TerrainContextClick;
	m_clickFunc[selected][SELECT_TYPE_LOCAL_ARMY]          [button][mode] = &SelectedItem::ArmyContextClick;
	m_clickFunc[selected][SELECT_TYPE_LOCAL_ARMY_UNLOADING][button][mode] = &SelectedItem::ErrorClick;
	m_clickFunc[selected][SELECT_TYPE_LOCAL_CITY]          [button][mode] = &SelectedItem::CityContextClick;
	m_clickFunc[selected][SELECT_TYPE_REMOTE_CITY]         [button][mode] = &SelectedItem::EnemyCityContextClick;
	m_clickFunc[selected][SELECT_TYPE_REMOTE_ARMY]         [button][mode] = &SelectedItem::EnemyArmyContextClick;
	m_clickFunc[selected][SELECT_TYPE_TRADE_ROUTE]         [button][mode] = &SelectedItem::TradeRouteContextClick;
	m_clickFunc[selected][SELECT_TYPE_GOOD]                [button][mode] = &SelectedItem::GoodContextClick;

	mode = SELECT_MODE_LEFT;
	button = SELECT_BUTTON_RIGHT;
	selected = SELECT_TYPE_REMOTE_ARMY;

	m_clickFunc[selected][SELECT_TYPE_NONE]                [button][mode] = &SelectedItem::TerrainContextClick;
	m_clickFunc[selected][SELECT_TYPE_LOCAL_ARMY]          [button][mode] = &SelectedItem::ArmyContextClick;
	m_clickFunc[selected][SELECT_TYPE_LOCAL_ARMY_UNLOADING][button][mode] = &SelectedItem::ErrorClick;
	m_clickFunc[selected][SELECT_TYPE_LOCAL_CITY]          [button][mode] = &SelectedItem::CityContextClick;
	m_clickFunc[selected][SELECT_TYPE_REMOTE_CITY]         [button][mode] = &SelectedItem::EnemyCityContextClick;
	m_clickFunc[selected][SELECT_TYPE_REMOTE_ARMY]         [button][mode] = &SelectedItem::EnemyArmyContextClick;
	m_clickFunc[selected][SELECT_TYPE_TRADE_ROUTE]         [button][mode] = &SelectedItem::TradeRouteContextClick;
	m_clickFunc[selected][SELECT_TYPE_GOOD]                [button][mode] = &SelectedItem::GoodContextClick;

	mode = SELECT_MODE_LEFT;
	button = SELECT_BUTTON_RIGHT;
	selected = SELECT_TYPE_GOOD;

	m_clickFunc[selected][SELECT_TYPE_NONE]                [button][mode] = &SelectedItem::TerrainContextClick;
	m_clickFunc[selected][SELECT_TYPE_LOCAL_ARMY]          [button][mode] = &SelectedItem::ArmyContextClick;
	m_clickFunc[selected][SELECT_TYPE_LOCAL_ARMY_UNLOADING][button][mode] = &SelectedItem::ErrorClick;
	m_clickFunc[selected][SELECT_TYPE_LOCAL_CITY]          [button][mode] = &SelectedItem::SendGoodClick;
	m_clickFunc[selected][SELECT_TYPE_REMOTE_CITY]         [button][mode] = &SelectedItem::EnemyCityContextClick;
	m_clickFunc[selected][SELECT_TYPE_REMOTE_ARMY]         [button][mode] = &SelectedItem::EnemyArmyContextClick;
	m_clickFunc[selected][SELECT_TYPE_TRADE_ROUTE]         [button][mode] = &SelectedItem::TradeRouteContextClick;
	m_clickFunc[selected][SELECT_TYPE_GOOD]                [button][mode] = &SelectedItem::GoodContextClick;

	mode = SELECT_MODE_LEFT;
	button = SELECT_BUTTON_RIGHT;
	selected = SELECT_TYPE_TRADE_ROUTE;

	m_clickFunc[selected][SELECT_TYPE_NONE]                [button][mode] = &SelectedItem::TerrainContextClick;
	m_clickFunc[selected][SELECT_TYPE_LOCAL_ARMY]          [button][mode] = &SelectedItem::ArmyContextClick;
	m_clickFunc[selected][SELECT_TYPE_LOCAL_ARMY_UNLOADING][button][mode] = &SelectedItem::ErrorClick;
	m_clickFunc[selected][SELECT_TYPE_LOCAL_CITY]          [button][mode] = &SelectedItem::CityContextClick;
	m_clickFunc[selected][SELECT_TYPE_REMOTE_CITY]         [button][mode] = &SelectedItem::EnemyCityContextClick;
	m_clickFunc[selected][SELECT_TYPE_REMOTE_ARMY]         [button][mode] = &SelectedItem::EnemyArmyContextClick;
	m_clickFunc[selected][SELECT_TYPE_TRADE_ROUTE]         [button][mode] = &SelectedItem::TradeRouteContextClick;
	m_clickFunc[selected][SELECT_TYPE_GOOD]                [button][mode] = &SelectedItem::GoodContextClick;

	mode = SELECT_MODE_CTP2;
	button = SELECT_BUTTON_LEFT;
	selected = SELECT_TYPE_NONE;

	m_clickFunc[selected][SELECT_TYPE_NONE]                [button][mode] = &SelectedItem::NullClick;
	m_clickFunc[selected][SELECT_TYPE_LOCAL_ARMY]          [button][mode] = &SelectedItem::SelectArmyStartMoveClick;
	m_clickFunc[selected][SELECT_TYPE_LOCAL_ARMY_UNLOADING][button][mode] = &SelectedItem::ErrorClick;
	m_clickFunc[selected][SELECT_TYPE_LOCAL_CITY]          [button][mode] = &SelectedItem::SelectCityClick;
	m_clickFunc[selected][SELECT_TYPE_REMOTE_CITY]         [button][mode] = &SelectedItem::SelectEnemyCityClick;
	m_clickFunc[selected][SELECT_TYPE_REMOTE_ARMY]         [button][mode] = &SelectedItem::SelectEnemyArmyClick;
	m_clickFunc[selected][SELECT_TYPE_TRADE_ROUTE]         [button][mode] = &SelectedItem::SelectTradeRouteClick;
	m_clickFunc[selected][SELECT_TYPE_GOOD]                [button][mode] = &SelectedItem::SelectGoodClick;

	mode = SELECT_MODE_CTP2;
	button = SELECT_BUTTON_LEFT_PATHING;
	selected = SELECT_TYPE_NONE;

	m_clickFunc[selected][SELECT_TYPE_NONE]                [button][mode] = &SelectedItem::ActionClick;
	m_clickFunc[selected][SELECT_TYPE_LOCAL_ARMY]          [button][mode] = &SelectedItem::ActionClick;
	m_clickFunc[selected][SELECT_TYPE_LOCAL_ARMY_UNLOADING][button][mode] = &SelectedItem::ErrorClick;
	m_clickFunc[selected][SELECT_TYPE_LOCAL_CITY]          [button][mode] = &SelectedItem::ActionClick;
	m_clickFunc[selected][SELECT_TYPE_REMOTE_CITY]         [button][mode] = &SelectedItem::ActionClick;
	m_clickFunc[selected][SELECT_TYPE_REMOTE_ARMY]         [button][mode] = &SelectedItem::ActionClick;
	m_clickFunc[selected][SELECT_TYPE_TRADE_ROUTE]         [button][mode] = &SelectedItem::ActionClick;
	m_clickFunc[selected][SELECT_TYPE_GOOD]                [button][mode] = &SelectedItem::ActionClick;

	mode = SELECT_MODE_CTP2;
	button = SELECT_BUTTON_LEFT;
	selected = SELECT_TYPE_LOCAL_ARMY;

	m_clickFunc[selected][SELECT_TYPE_NONE]                [button][mode] = &SelectedItem::DeselectClick;
	m_clickFunc[selected][SELECT_TYPE_LOCAL_ARMY]          [button][mode] = &SelectedItem::SelectArmyStartMoveClick;
	m_clickFunc[selected][SELECT_TYPE_LOCAL_ARMY_UNLOADING][button][mode] = &SelectedItem::ErrorClick;
	m_clickFunc[selected][SELECT_TYPE_LOCAL_CITY]          [button][mode] = &SelectedItem::SelectCityClick;
	m_clickFunc[selected][SELECT_TYPE_REMOTE_CITY]         [button][mode] = &SelectedItem::SelectEnemyCityClick;
	m_clickFunc[selected][SELECT_TYPE_REMOTE_ARMY]         [button][mode] = &SelectedItem::SelectEnemyArmyClick;
	m_clickFunc[selected][SELECT_TYPE_TRADE_ROUTE]         [button][mode] = &SelectedItem::SelectTradeRouteClick;
	m_clickFunc[selected][SELECT_TYPE_GOOD]                [button][mode] = &SelectedItem::SelectGoodClick;

	mode = SELECT_MODE_CTP2;
	button = SELECT_BUTTON_LEFT_PATHING;
	selected = SELECT_TYPE_LOCAL_ARMY;

	m_clickFunc[selected][SELECT_TYPE_NONE]                [button][mode] = &SelectedItem::ActionClick;
	m_clickFunc[selected][SELECT_TYPE_LOCAL_ARMY]          [button][mode] = &SelectedItem::ActionClick;
	m_clickFunc[selected][SELECT_TYPE_LOCAL_ARMY_UNLOADING][button][mode] = &SelectedItem::ErrorClick;
	m_clickFunc[selected][SELECT_TYPE_LOCAL_CITY]          [button][mode] = &SelectedItem::ActionClick;
	m_clickFunc[selected][SELECT_TYPE_REMOTE_CITY]         [button][mode] = &SelectedItem::ActionClick;
	m_clickFunc[selected][SELECT_TYPE_REMOTE_ARMY]         [button][mode] = &SelectedItem::ActionClick;
	m_clickFunc[selected][SELECT_TYPE_TRADE_ROUTE]         [button][mode] = &SelectedItem::ActionClick;
	m_clickFunc[selected][SELECT_TYPE_GOOD]                [button][mode] = &SelectedItem::ActionClick;

	mode = SELECT_MODE_CTP2;
	button = SELECT_BUTTON_LEFT_DRAG;
	selected = SELECT_TYPE_LOCAL_ARMY;

	m_clickFunc[selected][SELECT_TYPE_NONE]                [button][mode] = &SelectedItem::MoveDrag;
	m_clickFunc[selected][SELECT_TYPE_LOCAL_ARMY]          [button][mode] = &SelectedItem::MoveDrag;
	m_clickFunc[selected][SELECT_TYPE_LOCAL_ARMY_UNLOADING][button][mode] = &SelectedItem::ErrorClick;
	m_clickFunc[selected][SELECT_TYPE_LOCAL_CITY]          [button][mode] = &SelectedItem::MoveDrag;
	m_clickFunc[selected][SELECT_TYPE_REMOTE_CITY]         [button][mode] = &SelectedItem::MoveDrag;
	m_clickFunc[selected][SELECT_TYPE_REMOTE_ARMY]         [button][mode] = &SelectedItem::MoveDrag;
	m_clickFunc[selected][SELECT_TYPE_TRADE_ROUTE]         [button][mode] = &SelectedItem::MoveDrag;
	m_clickFunc[selected][SELECT_TYPE_GOOD]                [button][mode] = &SelectedItem::MoveDrag;

	mode = SELECT_MODE_CTP2;
	button = SELECT_BUTTON_LEFT_DROP;
	selected = SELECT_TYPE_LOCAL_ARMY;

	m_clickFunc[selected][SELECT_TYPE_NONE]                [button][mode] = &SelectedItem::MoveDrop;
	m_clickFunc[selected][SELECT_TYPE_LOCAL_ARMY]          [button][mode] = &SelectedItem::MoveDrop;
	m_clickFunc[selected][SELECT_TYPE_LOCAL_ARMY_UNLOADING][button][mode] = &SelectedItem::ErrorClick;
	m_clickFunc[selected][SELECT_TYPE_LOCAL_CITY]          [button][mode] = &SelectedItem::MoveDrop;
	m_clickFunc[selected][SELECT_TYPE_REMOTE_CITY]         [button][mode] = &SelectedItem::MoveDrop;
	m_clickFunc[selected][SELECT_TYPE_REMOTE_ARMY]         [button][mode] = &SelectedItem::MoveDrop;
	m_clickFunc[selected][SELECT_TYPE_TRADE_ROUTE]         [button][mode] = &SelectedItem::MoveDrop;
	m_clickFunc[selected][SELECT_TYPE_GOOD]                [button][mode] = &SelectedItem::MoveDrop;

	mode = SELECT_MODE_CTP2;
	button = SELECT_BUTTON_LEFT;
	selected = SELECT_TYPE_LOCAL_CITY;

	m_clickFunc[selected][SELECT_TYPE_NONE]                [button][mode] = &SelectedItem::DeselectClick;
	m_clickFunc[selected][SELECT_TYPE_LOCAL_ARMY]          [button][mode] = &SelectedItem::SelectArmyStartMoveClick;
	m_clickFunc[selected][SELECT_TYPE_LOCAL_ARMY_UNLOADING][button][mode] = &SelectedItem::ErrorClick;
	m_clickFunc[selected][SELECT_TYPE_LOCAL_CITY]          [button][mode] = &SelectedItem::SelectCityClick;
	m_clickFunc[selected][SELECT_TYPE_REMOTE_CITY]         [button][mode] = &SelectedItem::SelectEnemyCityClick;
	m_clickFunc[selected][SELECT_TYPE_REMOTE_ARMY]         [button][mode] = &SelectedItem::SelectEnemyArmyClick;
	m_clickFunc[selected][SELECT_TYPE_TRADE_ROUTE]         [button][mode] = &SelectedItem::SelectTradeRouteClick;
	m_clickFunc[selected][SELECT_TYPE_GOOD]                [button][mode] = &SelectedItem::SelectGoodClick;

	mode = SELECT_MODE_CTP2;
	button = SELECT_BUTTON_LEFT_DROP;
	selected = SELECT_TYPE_LOCAL_CITY;

	m_clickFunc[selected][SELECT_TYPE_NONE]                [button][mode] = &SelectedItem::DeselectClick;
	m_clickFunc[selected][SELECT_TYPE_LOCAL_ARMY]          [button][mode] = &SelectedItem::SelectArmyClick;
	m_clickFunc[selected][SELECT_TYPE_LOCAL_ARMY_UNLOADING][button][mode] = &SelectedItem::ErrorClick;
	m_clickFunc[selected][SELECT_TYPE_LOCAL_CITY]          [button][mode] = &SelectedItem::SelectCityClick;
	m_clickFunc[selected][SELECT_TYPE_REMOTE_CITY]         [button][mode] = &SelectedItem::SelectEnemyCityClick;
	m_clickFunc[selected][SELECT_TYPE_REMOTE_ARMY]         [button][mode] = &SelectedItem::SelectEnemyArmyClick;
	m_clickFunc[selected][SELECT_TYPE_TRADE_ROUTE]         [button][mode] = &SelectedItem::SelectTradeRouteClick;
	m_clickFunc[selected][SELECT_TYPE_GOOD]                [button][mode] = &SelectedItem::SelectGoodClick;

	mode = SELECT_MODE_CTP2;
	button = SELECT_BUTTON_LEFT_PATHING;
	selected = SELECT_TYPE_LOCAL_CITY;

	m_clickFunc[selected][SELECT_TYPE_NONE]                [button][mode] = &SelectedItem::DeselectClick;
	m_clickFunc[selected][SELECT_TYPE_LOCAL_ARMY]          [button][mode] = &SelectedItem::SelectArmyClick;
	m_clickFunc[selected][SELECT_TYPE_LOCAL_ARMY_UNLOADING][button][mode] = &SelectedItem::ErrorClick;
	m_clickFunc[selected][SELECT_TYPE_LOCAL_CITY]          [button][mode] = &SelectedItem::SelectCityClick;
	m_clickFunc[selected][SELECT_TYPE_REMOTE_CITY]         [button][mode] = &SelectedItem::SelectEnemyCityClick;
	m_clickFunc[selected][SELECT_TYPE_REMOTE_ARMY]         [button][mode] = &SelectedItem::SelectEnemyArmyClick;
	m_clickFunc[selected][SELECT_TYPE_TRADE_ROUTE]         [button][mode] = &SelectedItem::SelectTradeRouteClick;
	m_clickFunc[selected][SELECT_TYPE_GOOD]                [button][mode] = &SelectedItem::SelectGoodClick;

	mode = SELECT_MODE_CTP2;
	button = SELECT_BUTTON_LEFT_PATHING;
	selected = SELECT_TYPE_GOOD;

	m_clickFunc[selected][SELECT_TYPE_NONE]                [button][mode] = &SelectedItem::ActionClick;
	m_clickFunc[selected][SELECT_TYPE_LOCAL_ARMY]          [button][mode] = &SelectedItem::ActionClick;
	m_clickFunc[selected][SELECT_TYPE_LOCAL_ARMY_UNLOADING][button][mode] = &SelectedItem::ErrorClick;
	m_clickFunc[selected][SELECT_TYPE_LOCAL_CITY]          [button][mode] = &SelectedItem::ActionClick;
	m_clickFunc[selected][SELECT_TYPE_REMOTE_CITY]         [button][mode] = &SelectedItem::ActionClick;
	m_clickFunc[selected][SELECT_TYPE_REMOTE_ARMY]         [button][mode] = &SelectedItem::ActionClick;
	m_clickFunc[selected][SELECT_TYPE_TRADE_ROUTE]         [button][mode] = &SelectedItem::ActionClick;
	m_clickFunc[selected][SELECT_TYPE_GOOD]                [button][mode] = &SelectedItem::ActionClick;

	mode = SELECT_MODE_CTP2;
	button = SELECT_BUTTON_LEFT;
	selected = SELECT_TYPE_LOCAL_ARMY_UNLOADING;

	m_clickFunc[selected][SELECT_TYPE_NONE]                [button][mode] = &SelectedItem::UnloadClick;
	m_clickFunc[selected][SELECT_TYPE_LOCAL_ARMY]          [button][mode] = &SelectedItem::UnloadClick;
	m_clickFunc[selected][SELECT_TYPE_LOCAL_ARMY_UNLOADING][button][mode] = &SelectedItem::ErrorClick;
	m_clickFunc[selected][SELECT_TYPE_LOCAL_CITY]          [button][mode] = &SelectedItem::UnloadClick;
	m_clickFunc[selected][SELECT_TYPE_REMOTE_CITY]         [button][mode] = &SelectedItem::UnloadClick;
	m_clickFunc[selected][SELECT_TYPE_REMOTE_ARMY]         [button][mode] = &SelectedItem::UnloadClick;
	m_clickFunc[selected][SELECT_TYPE_TRADE_ROUTE]         [button][mode] = &SelectedItem::UnloadClick;
	m_clickFunc[selected][SELECT_TYPE_GOOD]                [button][mode] = &SelectedItem::UnloadClick;

	mode = SELECT_MODE_CTP2;
	button = SELECT_BUTTON_LEFT;
	selected = SELECT_TYPE_REMOTE_CITY;

	m_clickFunc[selected][SELECT_TYPE_NONE]                [button][mode] = &SelectedItem::DeselectClick;
	m_clickFunc[selected][SELECT_TYPE_LOCAL_ARMY]          [button][mode] = &SelectedItem::SelectArmyClick;
	m_clickFunc[selected][SELECT_TYPE_LOCAL_ARMY_UNLOADING][button][mode] = &SelectedItem::ErrorClick;
	m_clickFunc[selected][SELECT_TYPE_LOCAL_CITY]          [button][mode] = &SelectedItem::SelectCityClick;
	m_clickFunc[selected][SELECT_TYPE_REMOTE_CITY]         [button][mode] = &SelectedItem::SelectEnemyCityClick;
	m_clickFunc[selected][SELECT_TYPE_REMOTE_ARMY]         [button][mode] = &SelectedItem::SelectEnemyArmyClick;
	m_clickFunc[selected][SELECT_TYPE_TRADE_ROUTE]         [button][mode] = &SelectedItem::SelectTradeRouteClick;
	m_clickFunc[selected][SELECT_TYPE_GOOD]                [button][mode] = &SelectedItem::SelectGoodClick;

	mode = SELECT_MODE_CTP2;
	button = SELECT_BUTTON_LEFT;
	selected = SELECT_TYPE_REMOTE_ARMY;

	m_clickFunc[selected][SELECT_TYPE_NONE]                [button][mode] = &SelectedItem::DeselectClick;
	m_clickFunc[selected][SELECT_TYPE_LOCAL_ARMY]          [button][mode] = &SelectedItem::SelectArmyClick;
	m_clickFunc[selected][SELECT_TYPE_LOCAL_ARMY_UNLOADING][button][mode] = &SelectedItem::ErrorClick;
	m_clickFunc[selected][SELECT_TYPE_LOCAL_CITY]          [button][mode] = &SelectedItem::SelectCityClick;
	m_clickFunc[selected][SELECT_TYPE_REMOTE_CITY]         [button][mode] = &SelectedItem::SelectEnemyCityClick;
	m_clickFunc[selected][SELECT_TYPE_REMOTE_ARMY]         [button][mode] = &SelectedItem::SelectEnemyArmyClick;
	m_clickFunc[selected][SELECT_TYPE_TRADE_ROUTE]         [button][mode] = &SelectedItem::SelectTradeRouteClick;
	m_clickFunc[selected][SELECT_TYPE_GOOD]                [button][mode] = &SelectedItem::SelectGoodClick;

	mode = SELECT_MODE_CTP2;
	button = SELECT_BUTTON_LEFT;
	selected = SELECT_TYPE_GOOD;

	m_clickFunc[selected][SELECT_TYPE_NONE]                [button][mode] = &SelectedItem::DeselectClick;
	m_clickFunc[selected][SELECT_TYPE_LOCAL_ARMY]          [button][mode] = &SelectedItem::SelectArmyStartMoveClick;
	m_clickFunc[selected][SELECT_TYPE_LOCAL_ARMY_UNLOADING][button][mode] = &SelectedItem::ErrorClick;
	m_clickFunc[selected][SELECT_TYPE_LOCAL_CITY]          [button][mode] = &SelectedItem::SelectCityClick;
	m_clickFunc[selected][SELECT_TYPE_REMOTE_CITY]         [button][mode] = &SelectedItem::SelectEnemyCityClick;
	m_clickFunc[selected][SELECT_TYPE_REMOTE_ARMY]         [button][mode] = &SelectedItem::SelectEnemyArmyClick;
	m_clickFunc[selected][SELECT_TYPE_TRADE_ROUTE]         [button][mode] = &SelectedItem::SelectTradeRouteClick;
	m_clickFunc[selected][SELECT_TYPE_GOOD]                [button][mode] = &SelectedItem::SelectGoodClick;

	mode = SELECT_MODE_CTP2;
	button = SELECT_BUTTON_LEFT_DROP;
	selected = SELECT_TYPE_GOOD;

	m_clickFunc[selected][SELECT_TYPE_NONE]                [button][mode] = &SelectedItem::DeselectClick;
	m_clickFunc[selected][SELECT_TYPE_LOCAL_ARMY]          [button][mode] = &SelectedItem::SelectArmyClick;
	m_clickFunc[selected][SELECT_TYPE_LOCAL_ARMY_UNLOADING][button][mode] = &SelectedItem::ErrorClick;
	m_clickFunc[selected][SELECT_TYPE_LOCAL_CITY]          [button][mode] = &SelectedItem::SelectCityClick;
	m_clickFunc[selected][SELECT_TYPE_REMOTE_CITY]         [button][mode] = &SelectedItem::SelectEnemyCityClick;
	m_clickFunc[selected][SELECT_TYPE_REMOTE_ARMY]         [button][mode] = &SelectedItem::SelectEnemyArmyClick;
	m_clickFunc[selected][SELECT_TYPE_TRADE_ROUTE]         [button][mode] = &SelectedItem::SelectTradeRouteClick;
	m_clickFunc[selected][SELECT_TYPE_GOOD]                [button][mode] = &SelectedItem::SelectGoodClick;

	mode = SELECT_MODE_CTP2;
	button = SELECT_BUTTON_LEFT;
	selected = SELECT_TYPE_TRADE_ROUTE;

	m_clickFunc[selected][SELECT_TYPE_NONE]                [button][mode] = &SelectedItem::DeselectClick;
	m_clickFunc[selected][SELECT_TYPE_LOCAL_ARMY]          [button][mode] = &SelectedItem::SelectArmyClick;
	m_clickFunc[selected][SELECT_TYPE_LOCAL_ARMY_UNLOADING][button][mode] = &SelectedItem::ErrorClick;
	m_clickFunc[selected][SELECT_TYPE_LOCAL_CITY]          [button][mode] = &SelectedItem::SelectCityClick;
	m_clickFunc[selected][SELECT_TYPE_REMOTE_CITY]         [button][mode] = &SelectedItem::SelectEnemyCityClick;
	m_clickFunc[selected][SELECT_TYPE_REMOTE_ARMY]         [button][mode] = &SelectedItem::SelectEnemyArmyClick;
	m_clickFunc[selected][SELECT_TYPE_TRADE_ROUTE]         [button][mode] = &SelectedItem::SelectTradeRouteClick;
	m_clickFunc[selected][SELECT_TYPE_GOOD]                [button][mode] = &SelectedItem::SelectGoodClick;

	mode = SELECT_MODE_CTP2;
	button = SELECT_BUTTON_RIGHT;
	selected = SELECT_TYPE_NONE;

	m_clickFunc[selected][SELECT_TYPE_NONE]                [button][mode] = &SelectedItem::TerrainContextClick;
	m_clickFunc[selected][SELECT_TYPE_LOCAL_ARMY]          [button][mode] = &SelectedItem::ArmyContextClick;
	m_clickFunc[selected][SELECT_TYPE_LOCAL_ARMY_UNLOADING][button][mode] = &SelectedItem::ErrorClick;
	m_clickFunc[selected][SELECT_TYPE_LOCAL_CITY]          [button][mode] = &SelectedItem::CityContextClick;
	m_clickFunc[selected][SELECT_TYPE_REMOTE_CITY]         [button][mode] = &SelectedItem::EnemyCityContextClick;
	m_clickFunc[selected][SELECT_TYPE_REMOTE_ARMY]         [button][mode] = &SelectedItem::EnemyArmyContextClick;
	m_clickFunc[selected][SELECT_TYPE_TRADE_ROUTE]         [button][mode] = &SelectedItem::TradeRouteContextClick;
	m_clickFunc[selected][SELECT_TYPE_GOOD]                [button][mode] = &SelectedItem::GoodContextClick;

	mode = SELECT_MODE_CTP2;
	button = SELECT_BUTTON_RIGHT;
	selected = SELECT_TYPE_LOCAL_ARMY;

	m_clickFunc[selected][SELECT_TYPE_NONE]                [button][mode] = &SelectedItem::TerrainContextClick;
	m_clickFunc[selected][SELECT_TYPE_LOCAL_ARMY]          [button][mode] = &SelectedItem::ArmyContextClick;
	m_clickFunc[selected][SELECT_TYPE_LOCAL_ARMY_UNLOADING][button][mode] = &SelectedItem::ErrorClick;
	m_clickFunc[selected][SELECT_TYPE_LOCAL_CITY]          [button][mode] = &SelectedItem::CityContextClick;
	m_clickFunc[selected][SELECT_TYPE_REMOTE_CITY]         [button][mode] = &SelectedItem::EnemyCityContextClick;
	m_clickFunc[selected][SELECT_TYPE_REMOTE_ARMY]         [button][mode] = &SelectedItem::EnemyArmyContextClick;
	m_clickFunc[selected][SELECT_TYPE_TRADE_ROUTE]         [button][mode] = &SelectedItem::TradeRouteContextClick;
	m_clickFunc[selected][SELECT_TYPE_GOOD]                [button][mode] = &SelectedItem::GoodContextClick;

	mode = SELECT_MODE_CTP2;
	button = SELECT_BUTTON_RIGHT_PATHING;
	selected = SELECT_TYPE_LOCAL_ARMY;

	m_clickFunc[selected][SELECT_TYPE_NONE]                [button][mode] = &SelectedItem::CancelPathingClick;
	m_clickFunc[selected][SELECT_TYPE_LOCAL_ARMY]          [button][mode] = &SelectedItem::CancelPathingClick;
	m_clickFunc[selected][SELECT_TYPE_LOCAL_ARMY_UNLOADING][button][mode] = &SelectedItem::ErrorClick;
	m_clickFunc[selected][SELECT_TYPE_LOCAL_CITY]          [button][mode] = &SelectedItem::CancelPathingClick;
	m_clickFunc[selected][SELECT_TYPE_REMOTE_CITY]         [button][mode] = &SelectedItem::CancelPathingClick;
	m_clickFunc[selected][SELECT_TYPE_REMOTE_ARMY]         [button][mode] = &SelectedItem::CancelPathingClick;
	m_clickFunc[selected][SELECT_TYPE_TRADE_ROUTE]         [button][mode] = &SelectedItem::CancelPathingClick;
	m_clickFunc[selected][SELECT_TYPE_GOOD]                [button][mode] = &SelectedItem::CancelPathingClick;

	mode = SELECT_MODE_CTP2;
	button = SELECT_BUTTON_RIGHT_PATHING;
	selected = SELECT_TYPE_LOCAL_CITY;

	m_clickFunc[selected][SELECT_TYPE_NONE]                [button][mode] = &SelectedItem::CancelPathingClick;
	m_clickFunc[selected][SELECT_TYPE_LOCAL_ARMY]          [button][mode] = &SelectedItem::CancelPathingClick;
	m_clickFunc[selected][SELECT_TYPE_LOCAL_ARMY_UNLOADING][button][mode] = &SelectedItem::ErrorClick;
	m_clickFunc[selected][SELECT_TYPE_LOCAL_CITY]          [button][mode] = &SelectedItem::CancelPathingClick;
	m_clickFunc[selected][SELECT_TYPE_REMOTE_CITY]         [button][mode] = &SelectedItem::CancelPathingClick;
	m_clickFunc[selected][SELECT_TYPE_REMOTE_ARMY]         [button][mode] = &SelectedItem::CancelPathingClick;
	m_clickFunc[selected][SELECT_TYPE_TRADE_ROUTE]         [button][mode] = &SelectedItem::CancelPathingClick;
	m_clickFunc[selected][SELECT_TYPE_GOOD]                [button][mode] = &SelectedItem::CancelPathingClick;

	mode = SELECT_MODE_CTP2;
	button = SELECT_BUTTON_RIGHT_PATHING;
	selected = SELECT_TYPE_GOOD;

	m_clickFunc[selected][SELECT_TYPE_NONE]                [button][mode] = &SelectedItem::CancelPathingClick;
	m_clickFunc[selected][SELECT_TYPE_LOCAL_ARMY]          [button][mode] = &SelectedItem::CancelPathingClick;
	m_clickFunc[selected][SELECT_TYPE_LOCAL_ARMY_UNLOADING][button][mode] = &SelectedItem::ErrorClick;
	m_clickFunc[selected][SELECT_TYPE_LOCAL_CITY]          [button][mode] = &SelectedItem::CancelPathingClick;
	m_clickFunc[selected][SELECT_TYPE_REMOTE_CITY]         [button][mode] = &SelectedItem::CancelPathingClick;
	m_clickFunc[selected][SELECT_TYPE_REMOTE_ARMY]         [button][mode] = &SelectedItem::CancelPathingClick;
	m_clickFunc[selected][SELECT_TYPE_TRADE_ROUTE]         [button][mode] = &SelectedItem::CancelPathingClick;
	m_clickFunc[selected][SELECT_TYPE_GOOD]                [button][mode] = &SelectedItem::CancelPathingClick;

	mode = SELECT_MODE_CTP2;
	button = SELECT_BUTTON_RIGHT_PATHING;
	selected = SELECT_TYPE_NONE;

	m_clickFunc[selected][SELECT_TYPE_NONE]                [button][mode] = &SelectedItem::CancelPathingClick;
	m_clickFunc[selected][SELECT_TYPE_LOCAL_ARMY]          [button][mode] = &SelectedItem::CancelPathingClick;
	m_clickFunc[selected][SELECT_TYPE_LOCAL_ARMY_UNLOADING][button][mode] = &SelectedItem::ErrorClick;
	m_clickFunc[selected][SELECT_TYPE_LOCAL_CITY]          [button][mode] = &SelectedItem::CancelPathingClick;
	m_clickFunc[selected][SELECT_TYPE_REMOTE_CITY]         [button][mode] = &SelectedItem::CancelPathingClick;
	m_clickFunc[selected][SELECT_TYPE_REMOTE_ARMY]         [button][mode] = &SelectedItem::CancelPathingClick;
	m_clickFunc[selected][SELECT_TYPE_TRADE_ROUTE]         [button][mode] = &SelectedItem::CancelPathingClick;
	m_clickFunc[selected][SELECT_TYPE_GOOD]                [button][mode] = &SelectedItem::CancelPathingClick;

	mode = SELECT_MODE_CTP2;
	button = SELECT_BUTTON_RIGHT;
	selected = SELECT_TYPE_LOCAL_CITY;

	m_clickFunc[selected][SELECT_TYPE_NONE]                [button][mode] = &SelectedItem::TerrainContextClick;
	m_clickFunc[selected][SELECT_TYPE_LOCAL_ARMY]          [button][mode] = &SelectedItem::ArmyContextClick;
	m_clickFunc[selected][SELECT_TYPE_LOCAL_ARMY_UNLOADING][button][mode] = &SelectedItem::ErrorClick;
	m_clickFunc[selected][SELECT_TYPE_LOCAL_CITY]          [button][mode] = &SelectedItem::CityContextClick;
	m_clickFunc[selected][SELECT_TYPE_REMOTE_CITY]         [button][mode] = &SelectedItem::EnemyCityContextClick;
	m_clickFunc[selected][SELECT_TYPE_REMOTE_ARMY]         [button][mode] = &SelectedItem::EnemyArmyContextClick;
	m_clickFunc[selected][SELECT_TYPE_TRADE_ROUTE]         [button][mode] = &SelectedItem::TradeRouteContextClick;
	m_clickFunc[selected][SELECT_TYPE_GOOD]                [button][mode] = &SelectedItem::GoodContextClick;

	mode = SELECT_MODE_CTP2;
	button = SELECT_BUTTON_RIGHT;
	selected = SELECT_TYPE_LOCAL_ARMY_UNLOADING;

	m_clickFunc[selected][SELECT_TYPE_NONE]                [button][mode] = &SelectedItem::UnloadClick;
	m_clickFunc[selected][SELECT_TYPE_LOCAL_ARMY]          [button][mode] = &SelectedItem::UnloadClick;
	m_clickFunc[selected][SELECT_TYPE_LOCAL_ARMY_UNLOADING][button][mode] = &SelectedItem::ErrorClick;
	m_clickFunc[selected][SELECT_TYPE_LOCAL_CITY]          [button][mode] = &SelectedItem::UnloadClick;
	m_clickFunc[selected][SELECT_TYPE_REMOTE_CITY]         [button][mode] = &SelectedItem::UnloadClick;
	m_clickFunc[selected][SELECT_TYPE_REMOTE_ARMY]         [button][mode] = &SelectedItem::UnloadClick;
	m_clickFunc[selected][SELECT_TYPE_TRADE_ROUTE]         [button][mode] = &SelectedItem::UnloadClick;
	m_clickFunc[selected][SELECT_TYPE_GOOD]                [button][mode] = &SelectedItem::UnloadClick;

	mode = SELECT_MODE_CTP2;
	button = SELECT_BUTTON_RIGHT;
	selected = SELECT_TYPE_REMOTE_CITY;

	m_clickFunc[selected][SELECT_TYPE_NONE]                [button][mode] = &SelectedItem::TerrainContextClick;
	m_clickFunc[selected][SELECT_TYPE_LOCAL_ARMY]          [button][mode] = &SelectedItem::ArmyContextClick;
	m_clickFunc[selected][SELECT_TYPE_LOCAL_ARMY_UNLOADING][button][mode] = &SelectedItem::ErrorClick;
	m_clickFunc[selected][SELECT_TYPE_LOCAL_CITY]          [button][mode] = &SelectedItem::CityContextClick;
	m_clickFunc[selected][SELECT_TYPE_REMOTE_CITY]         [button][mode] = &SelectedItem::EnemyCityContextClick;
	m_clickFunc[selected][SELECT_TYPE_REMOTE_ARMY]         [button][mode] = &SelectedItem::EnemyArmyContextClick;
	m_clickFunc[selected][SELECT_TYPE_TRADE_ROUTE]         [button][mode] = &SelectedItem::TradeRouteContextClick;
	m_clickFunc[selected][SELECT_TYPE_GOOD]                [button][mode] = &SelectedItem::GoodContextClick;

	mode = SELECT_MODE_CTP2;
	button = SELECT_BUTTON_RIGHT;
	selected = SELECT_TYPE_REMOTE_ARMY;

	m_clickFunc[selected][SELECT_TYPE_NONE]                [button][mode] = &SelectedItem::TerrainContextClick;
	m_clickFunc[selected][SELECT_TYPE_LOCAL_ARMY]          [button][mode] = &SelectedItem::ArmyContextClick;
	m_clickFunc[selected][SELECT_TYPE_LOCAL_ARMY_UNLOADING][button][mode] = &SelectedItem::ErrorClick;
	m_clickFunc[selected][SELECT_TYPE_LOCAL_CITY]          [button][mode] = &SelectedItem::CityContextClick;
	m_clickFunc[selected][SELECT_TYPE_REMOTE_CITY]         [button][mode] = &SelectedItem::EnemyCityContextClick;
	m_clickFunc[selected][SELECT_TYPE_REMOTE_ARMY]         [button][mode] = &SelectedItem::EnemyArmyContextClick;
	m_clickFunc[selected][SELECT_TYPE_TRADE_ROUTE]         [button][mode] = &SelectedItem::TradeRouteContextClick;
	m_clickFunc[selected][SELECT_TYPE_GOOD]                [button][mode] = &SelectedItem::GoodContextClick;

	mode = SELECT_MODE_CTP2;
	button = SELECT_BUTTON_RIGHT;
	selected = SELECT_TYPE_GOOD;

	m_clickFunc[selected][SELECT_TYPE_NONE]                [button][mode] = &SelectedItem::TerrainContextClick;
	m_clickFunc[selected][SELECT_TYPE_LOCAL_ARMY]          [button][mode] = &SelectedItem::ArmyContextClick;
	m_clickFunc[selected][SELECT_TYPE_LOCAL_ARMY_UNLOADING][button][mode] = &SelectedItem::ErrorClick;
	m_clickFunc[selected][SELECT_TYPE_LOCAL_CITY]          [button][mode] = &SelectedItem::SendGoodClick;
	m_clickFunc[selected][SELECT_TYPE_REMOTE_CITY]         [button][mode] = &SelectedItem::SendGoodClick;
	m_clickFunc[selected][SELECT_TYPE_REMOTE_ARMY]         [button][mode] = &SelectedItem::EnemyArmyContextClick;
	m_clickFunc[selected][SELECT_TYPE_TRADE_ROUTE]         [button][mode] = &SelectedItem::TradeRouteContextClick;
	m_clickFunc[selected][SELECT_TYPE_GOOD]                [button][mode] = &SelectedItem::GoodContextClick;

	mode = SELECT_MODE_CTP2;
	button = SELECT_BUTTON_RIGHT;
	selected = SELECT_TYPE_TRADE_ROUTE;

	m_clickFunc[selected][SELECT_TYPE_NONE]                [button][mode] = &SelectedItem::TerrainContextClick;
	m_clickFunc[selected][SELECT_TYPE_LOCAL_ARMY]          [button][mode] = &SelectedItem::ArmyContextClick;
	m_clickFunc[selected][SELECT_TYPE_LOCAL_ARMY_UNLOADING][button][mode] = &SelectedItem::ErrorClick;
	m_clickFunc[selected][SELECT_TYPE_LOCAL_CITY]          [button][mode] = &SelectedItem::CityContextClick;
	m_clickFunc[selected][SELECT_TYPE_REMOTE_CITY]         [button][mode] = &SelectedItem::EnemyCityContextClick;
	m_clickFunc[selected][SELECT_TYPE_REMOTE_ARMY]         [button][mode] = &SelectedItem::EnemyArmyContextClick;
	m_clickFunc[selected][SELECT_TYPE_TRADE_ROUTE]         [button][mode] = &SelectedItem::TradeRouteContextClick;
	m_clickFunc[selected][SELECT_TYPE_GOOD]                [button][mode] = &SelectedItem::GoodContextClick;
}

void SelectedItem::NewRegisterClick(const MapPoint &pos, const aui_MouseEvent *data, bool doubleClick,
									bool leftDrag, bool leftDrop)
{
	PLAYER_INDEX      vPlayer = GetVisiblePlayer();
	bool			     left = data->lbutton != 0;
	bool		        right = data->rbutton != 0;
	SELECT_BUTTON button = SELECT_BUTTON_MAX;
	SELECT_MODE mode = g_theProfileDB->IsUseLeftClick() ? SELECT_MODE_LEFT : SELECT_MODE_RIGHT;

	bool do_targeting_mode = false;

	if (g_controlPanel!=NULL)
		do_targeting_mode = (g_controlPanel->GetTargetingMode()!=CP_TARGETING_MODE_OFF);

	if(g_theProfileDB->IsUseCTP2Mode())
		mode = SELECT_MODE_CTP2;

	if(left)
		button = SELECT_BUTTON_LEFT;
	else if(right)
		button = SELECT_BUTTON_RIGHT;

	if(leftDrag)
		button = SELECT_BUTTON_LEFT_DRAG;
	else if(leftDrop)
		button = SELECT_BUTTON_LEFT_DROP;

	SELECT_TYPE clickedThing = GetClickedThing(pos, button != SELECT_BUTTON_LEFT_DROP);

	if(mode == SELECT_MODE_CTP2)
	{
		if( (m_is_pathing
		||   do_targeting_mode)
		){
			if(button == SELECT_BUTTON_LEFT)
			{
				button = SELECT_BUTTON_LEFT_PATHING;
			}
			else if(button == SELECT_BUTTON_RIGHT)
			{
				button = SELECT_BUTTON_RIGHT_PATHING;
			}
		}
	}

	if(m_select_state[vPlayer] == SELECT_TYPE_LOCAL_ARMY &&
	   pos == m_select_pos[vPlayer] &&
	   !leftDrop &&
	   clickedThing != SELECT_TYPE_LOCAL_ARMY)
	{
		clickedThing = SELECT_TYPE_LOCAL_ARMY;
	}

	if(doubleClick && button == SELECT_BUTTON_LEFT && pos == m_select_pos[vPlayer])
	{
		m_justGotDoubleClick = true;
		clickedThing = m_select_state[vPlayer];
	}

	if(button == SELECT_BUTTON_LEFT_DROP && m_justGotDoubleClick)
	{
		m_justGotDoubleClick = false;
		return;
	}

	if(button == SELECT_BUTTON_LEFT &&
	   (m_select_state[vPlayer] == SELECT_TYPE_NONE || m_select_pos[vPlayer] != pos) &&
	   clickedThing == SELECT_TYPE_LOCAL_ARMY)
	{
		m_justNowSelectedArmy = true;
	}

	if(button == SELECT_BUTTON_MAX)
		return;

	ClickFunctionPtr func = m_clickFunc
		[m_select_state[vPlayer]]
		[clickedThing]
		[button]
		[mode];

	(this->*func)(pos, data, doubleClick);
}

void SelectedItem::ErrorClick(const MapPoint &pos, const aui_MouseEvent *data, bool doubleClick)
{
	Assert(false);
}

void SelectedItem::NullClick(const MapPoint &pos, const aui_MouseEvent *data, bool doubleClick)
{
	// Does nothing
}

void SelectedItem::SelectArmyClick(const MapPoint &pos, const aui_MouseEvent *data, bool doubleClick)
{
	Unit top;
	GetTopUnit(pos, top);
	SetSelectUnit(top);

	if(doubleClick)
	{
		ArmyManagerWindow::Display();
	}

	g_gevManager->AddEvent(GEV_INSERT_Tail, GEV_ArmyClicked, GEA_Army, top.GetArmy(), GEA_End);
}

void SelectedItem::SelectArmyStartMoveClick(const MapPoint &pos, const aui_MouseEvent *data, bool doubleClick)
{
	if(!GetIsPathing())
	{
		SelectArmyClick(pos, data, doubleClick);
		if(!doubleClick)
			StartMoveClick(pos, data, doubleClick);
	}
}

void SelectedItem::SelectCityClick(const MapPoint &pos, const aui_MouseEvent *data,bool doubleClick)
{
	if( !GetIsPathing()
	|| ( IsLocalCity()
	&&   g_theProfileDB->IsDebugCityAstar()
	   )
	  )
	{
		Unit top;
		GetTopUnitOrCity(pos, top);
		SetSelectCity(top);
		if(doubleClick)
		{
			if(top.IsCity())
				CityWindow::Display(top);
			else
				ArmyManagerWindow::Display();
		}
	}

	Unit top;
	GetTopUnitOrCity(pos, top);
	g_gevManager->AddEvent(GEV_INSERT_Tail, GEV_CityClicked, GEA_City, top, GEA_End);
}

void SelectedItem::SelectGoodClick(const MapPoint &pos, const aui_MouseEvent *data, bool doubleClick)
{
	if(!GetIsPathing())
	{
		SetSelectGood(pos);
	}
}

void SelectedItem::SelectEnemyArmyClick(const MapPoint &pos, const aui_MouseEvent *data, bool doubleClick)
{
	Unit top;
	GetTopUnit(pos, top);
	g_gevManager->AddEvent(GEV_INSERT_Tail, GEV_ArmyClicked, GEA_Army, top->GetArmy(), GEA_End);
	Deselect(GetVisiblePlayer());
}

void SelectedItem::SelectEnemyCityClick(const MapPoint &pos, const aui_MouseEvent *data, bool doubleClick)
{
	Unit top;
	GetTopUnitOrCity(pos, top);
	g_gevManager->AddEvent(GEV_INSERT_Tail, GEV_CityClicked, GEA_City, top, GEA_End);
	Deselect(GetVisiblePlayer());
}

void SelectedItem::SelectTradeRouteClick(const MapPoint &pos, const aui_MouseEvent *data, bool doubleClick)
{

}

void SelectedItem::DeselectClick(const MapPoint &pos, const aui_MouseEvent *data, bool doubleClick)
{
	m_select_pos[GetVisiblePlayer()] = pos;

	Deselect(GetVisiblePlayer());
}

void SelectedItem::UnloadClick(const MapPoint &pos, const aui_MouseEvent *data, bool doubleClick)
{
	sint32 player = GetVisiblePlayer();

	g_gevManager->AddEvent(GEV_INSERT_Tail, GEV_UnloadOrder,
						   GEA_Army, m_selected_army[player],
						   GEA_MapPoint, pos,
						   GEA_End);
}

//----------------------------------------------------------------------------
//
// Name       : SelectedItem::TerrainContextClick
//
// Description: Handle mouse click on an unoccupied tile.
//
// Parameters : pos         : position on the map
//              data        : click properties (right, left, etc.)
//              doubleClick : is it a double click?
//
// Returns    : -
//
// Remark(s)  : When the tile is unexplored, nothing will be shown.
//
//----------------------------------------------------------------------------
void SelectedItem::TerrainContextClick(const MapPoint &pos, const aui_MouseEvent *data, bool doubleClick)
{
	if(g_player[GetVisiblePlayer()] && g_player[GetVisiblePlayer()]->IsExplored(pos))
	{
		helptile_displayData(pos);
	}
}

//----------------------------------------------------------------------------
//
// Name       : SelectedItem::ArmyContextClick
//
// Description: Handle mouse click on an army of our own.
//
// Parameters : pos         : position on the map
//              data        : click properties (right, left, etc.)
//              doubleClick : is it a double click?
//
// Returns    : -
//
// Remark(s)  : -
//
//----------------------------------------------------------------------------
void SelectedItem::ArmyContextClick(const MapPoint &pos, const aui_MouseEvent *data, bool doubleClick)
{
	SelectArmyClick(pos, data, false);

	g_controlPanel->ActivateSelectedInfo(SELECT_TYPE_LOCAL_ARMY);
}

//----------------------------------------------------------------------------
//
// Name       : SelectedItem::CityContextClick
//
// Description: Handle mouse click on a city of our own.
//
// Parameters : pos         : position on the map
//              data        : click properties (right, left, etc.)
//              doubleClick : is it a double click?
//
// Returns    : -
//
// Remark(s)  : -
//
//----------------------------------------------------------------------------
void SelectedItem::CityContextClick(const MapPoint &pos, const aui_MouseEvent *data, bool doubleClick)
{
	SelectCityClick(pos, data, false);

	g_controlPanel->ActivateSelectedInfo(SELECT_TYPE_LOCAL_CITY);
}

//----------------------------------------------------------------------------
//
// Name       : SelectedItem::EnemyCityContextClick
//
// Description: Handle mouse click on an enemy city.
//
// Parameters : pos         : position on the map
//              data        : click properties (right, left, etc.)
//              doubleClick : is it a double click?
//
// Returns    : -
//
// Remark(s)  : -
//
//----------------------------------------------------------------------------
void SelectedItem::EnemyCityContextClick(const MapPoint &pos, const aui_MouseEvent *data, bool doubleClick)
{
	// Report the underlying terrain information. This is just being useful.
	TerrainContextClick(pos, data, doubleClick);
}

//----------------------------------------------------------------------------
//
// Name       : SelectedItem::EnemyArmyContextClick
//
// Description: Handle mouse click on an enemy army.
//
// Parameters : pos         : position on the map
//              data        : click properties (right, left, etc.)
//              doubleClick : is it a double click?
//
// Returns    : -
//
// Remark(s)  : -
//
//----------------------------------------------------------------------------
void SelectedItem::EnemyArmyContextClick(const MapPoint &pos, const aui_MouseEvent *data, bool doubleClick)
{
	// Report the underlying terrain information. For visible units, this is
	// just being useful. For invisible units, it will fix the bug that
	// detection is too easy.
	TerrainContextClick(pos, data, doubleClick);
}

void SelectedItem::TradeRouteContextClick(const MapPoint &pos, const aui_MouseEvent *data, bool doubleClick)
{
}

void SelectedItem::GoodContextClick(const MapPoint &pos, const aui_MouseEvent *data, bool doubleClick)
{
	if(g_player[GetVisiblePlayer()] && g_player[GetVisiblePlayer()]->IsExplored(pos))
	{
		helptile_displayData(pos);
	}
}

void SelectedItem::SendGoodClick(const MapPoint &pos, const aui_MouseEvent *data, bool doubleClick)
{
	sint32 player = GetVisiblePlayer();

	bool isForeignGood = false;

	Unit homeCity = g_theWorld->GetCell(m_select_pos[player])->GetCityOwner();
	if(homeCity.m_id != 0)
	{
		if(homeCity.GetOwner() != player)
		{
			isForeignGood = true;
		}
	}
	else
	{
		return;
	}

	if (!homeCity.IsValid()) return;

	Unit destCity;
	if (!GetTopUnitOrCity(pos, destCity)) return;
	if (!destCity.IsCity()) return;

	sint32		resIndex;
	if (!g_theWorld->GetGood(m_select_pos[player], resIndex)) return;

	if(!isForeignGood)
	{
		g_gevManager->AddEvent(GEV_INSERT_Tail, GEV_SendGood,
							   GEA_Int, resIndex,
							   GEA_City, homeCity,
							   GEA_City, destCity,
							   GEA_End);
	}
#if 0
	else
	{
		g_gevManager->AddEvent(GEV_INSERT_Tail, GEV_TradeBid,
							   GEA_Player, player,
							   GEA_Int, resIndex,
							   GEA_City, homeCity,
							   GEA_City, destCity,
							   GEA_End);
	}
#endif
}

void SelectedItem::MoveArmyClick(const MapPoint &pos, const aui_MouseEvent *data, bool doubleClick)
{
	sint32 player = GetVisiblePlayer();

	MapPoint army_pos;
	m_selected_army[player].GetPos(army_pos);

	if (army_pos == pos &&
	    GetClickedThing(pos, true) == SELECT_TYPE_LOCAL_ARMY
	   )
	{
		// Does nothing
	}
	else
	{
		Unit unit = m_selected_army[player][0];

		if (m_good_path &&
			unit.GetDBRec()->GetCanCarry() &&
			(unit.GetNumCarried() > 0)
		   )
		{
			// Ask user confirmations about special situations when moving a
			// non-empty cargo unit

			if (unit.GetDBRec()->GetCargoPod() &&
				unit.GetData()->CargoHasLandUnits() &&
				g_theWorld->IsWater(pos)
			   )
			{
				// Dropping land units in a sea square could be lethal
				QuickSlic("15IALandInOcean", player);
				return;
			}

			if ((unit.GetMovementTypeSea() || unit.GetMovementTypeShallowWater()) &&
				g_theWorld->IsLand(pos)
			   )
			{
				if(g_player[player]->IsHuman())
				{
					// Reaching a shore: unload units?
					SlicObject * so = new SlicObject("14IAAutoUnload");
					so->AddLocation(pos);
					so->AddCivilisation(player);
					so->AddRecipient(player);
					g_slicEngine->Execute(so);
					return;
				}
				else
				{
					SetAutoUnload(true);
				}
			}
		}

		EnterArmyMove(player, pos);
	}
}

void SelectedItem::StartMoveClick(const MapPoint &pos, const aui_MouseEvent *data, bool doubleClick)
{
	DPRINTF(k_DBG_INFO, ("Start Move: %d,%d\n", pos.x, pos.y));
	m_startDragPos = pos;
	m_gotClick = true;
	m_clickStartTime = GetTickCount();
}

void SelectedItem::MoveDrag(const MapPoint &pos, const aui_MouseEvent *data, bool doubleClick)
{
	DPRINTF(k_DBG_INFO, ("Move Drag: %d,%d\n", pos.x, pos.y));
	if(!m_gotClick) {

		return;
	}

	if(pos != m_startDragPos)
	{
		m_isDragging = true;
		g_controlPanel->BeginOrderDelivery(m_moveOrder);
	}
}

void SelectedItem::MoveDrop(const MapPoint &pos, const aui_MouseEvent *data, bool doubleClick)
{
	DPRINTF(k_DBG_INFO, ("Move Drop: %d,%d\n", pos.x, pos.y));
	if(!m_gotClick)
	{
		return;
	}
	m_gotClick = false;

	if(pos != m_startDragPos)
	{
		m_isDragging = true;
		g_controlPanel->BeginOrderDelivery(m_moveOrder);
	}

	if(m_isDragging)
	{
		m_justNowSelectedArmy = false;
		if(GetIsPathing())
		{
			m_isDragging = false;
			ActionClick(pos, data, doubleClick);
			return;
		}

		if(pos == m_select_pos[GetVisiblePlayer()])
		{
			CancelPathingClick(pos, data, doubleClick);
			return;
		}
	} else {
		if(!m_justNowSelectedArmy) {
			switch(GetClickedThing(pos, false))
			{
				case SELECT_TYPE_LOCAL_ARMY:
					break;
				case SELECT_TYPE_GOOD:
					SelectGoodClick(pos, data, doubleClick);
					break;
				case SELECT_TYPE_LOCAL_CITY:
					SelectCityClick(pos, data, doubleClick);
					break;
			}
		}
		else
		{
			m_justNowSelectedArmy = false;
		}
	}
}

void SelectedItem::ActionClick(const MapPoint &pos, const aui_MouseEvent *data, bool doubleClick)
{
	m_startDragPos = pos;

	if(g_controlPanel->ExecuteTargetingModeClick(pos))
	{
		m_is_pathing = false;
		return;
	}

	if(!m_isDragging
	&& !GetIsPathing()
	){
		if(m_select_pos[GetVisiblePlayer()] != pos)
		{
			DeselectClick(pos, data, doubleClick);
		}
		return;
	}

	if(GetIsPathing())
	{
		MapPoint army_pos;
		sint32 player = GetVisiblePlayer();

		g_controlPanel->ClearTargetingMode();

		m_selected_army[player].GetPos(army_pos);

		m_is_pathing = false;

		if (army_pos != pos)
		{
			Unit unit = m_selected_army[player][0];
			if (m_good_path &&
				(unit.GetDBRec()->GetCanCarry()) &&
				(unit.GetNumCarried() > 0)
			   )
			{
				if((unit.GetMovementTypeSea() || unit.GetMovementTypeShallowWater()) &&
					(g_theWorld->IsLand(pos))
				   )
				{
					if(g_player[player]->IsHuman())
					{
						SlicObject *so = new SlicObject("14IAAutoUnload");
						so->AddLocation(pos);
						so->AddCivilisation(player);
						so->AddRecipient(player);
						g_slicEngine->Execute(so);
						return;
					}
					else
					{
						SetAutoUnload(true);
					}
				}
			}

			EnterArmyMove(player, pos);
		}
	}
}

void SelectedItem::CancelPathingClick(const MapPoint &pos, const aui_MouseEvent *data, bool doubleClick)
{
	m_is_pathing = false;
	m_isDragging = false;
	g_controlPanel->ClearTargetingMode();
}

SELECT_TYPE SelectedItem::GetClickedThing(const MapPoint &pos, bool click)
{
	Unit		top;
	sint32		visiblePlayer = GetVisiblePlayer();
	bool hasUnitOrCity = GetTopUnitOrCity(pos, top);
	Cell *cell = g_theWorld->GetCell(pos);

	if(m_select_state[visiblePlayer] != SELECT_TYPE_NONE &&
	   m_select_pos[visiblePlayer] == pos)
	{
		if(click)
			return m_select_state[visiblePlayer];

		switch(m_select_state[visiblePlayer])
		{
			case SELECT_TYPE_LOCAL_CITY:

				if(cell->GetNumUnits() > 0)
				{
					if(GetTopUnit(pos, top))
					{
						if(top.GetOwner() == visiblePlayer)
						{
							return SELECT_TYPE_LOCAL_ARMY;
						}
						else
						{
							return SELECT_TYPE_REMOTE_ARMY;
						}
					}
				}
				else if(hasUnitOrCity && top.IsCity())
				{

					if(top.GetOwner() == visiblePlayer)
					{
						return SELECT_TYPE_LOCAL_CITY;
					}
					else
					{
						return SELECT_TYPE_REMOTE_CITY;
					}
				}
				break;
			case SELECT_TYPE_LOCAL_ARMY:
				if(hasUnitOrCity && top.IsCity())
				{
					if(top.GetOwner() == visiblePlayer)
					{
						return SELECT_TYPE_LOCAL_CITY;
					}
					else
					{
						return SELECT_TYPE_REMOTE_CITY;
					}
				}
				else if(hasUnitOrCity)
				{
					if(top.GetOwner() == visiblePlayer)
						return SELECT_TYPE_LOCAL_ARMY;
					else
						return SELECT_TYPE_REMOTE_ARMY;
				}
				return SELECT_TYPE_NONE;
				break;
			case SELECT_TYPE_GOOD:
				if(hasUnitOrCity)
				{
					if(top.IsCity())
					{
						if(top.GetOwner() == visiblePlayer)
						{
							return SELECT_TYPE_LOCAL_CITY;
						}
						else
						{
							return SELECT_TYPE_REMOTE_CITY;
						}
					}
					else
					{
						if(top.GetOwner() == visiblePlayer)
						{
							return SELECT_TYPE_LOCAL_ARMY;
						}
						else
						{
							return SELECT_TYPE_REMOTE_ARMY;
						}
					}
				}
				return SELECT_TYPE_NONE;
				break;
			default:
				break;
		}
	}

	if(GetTopUnitOrCity(pos, top))
	{
		if(top.IsCity())
		{
			if(top.GetOwner() == visiblePlayer)
			{
				if(!g_theProfileDB->GetValueByName("CityClick")
				&& g_theWorld->GetCell(pos)->GetNumUnits() > 0
				){
					for(sint32 i = 0; i < g_theWorld->GetCell(pos)->GetNumUnits(); i++)
					{
						if(!g_theWorld->GetCell(pos)->AccessUnit(i).IsEntrenched()
						&& !g_theWorld->GetCell(pos)->AccessUnit(i).IsEntrenching()
						){
							return SELECT_TYPE_LOCAL_ARMY;
						}
					}
				}
				return SELECT_TYPE_LOCAL_CITY;
			}
			else
			{
				return SELECT_TYPE_REMOTE_CITY;
			}
		}
		else
		{
			if(top.GetOwner() == visiblePlayer)
			{
				return SELECT_TYPE_LOCAL_ARMY;
			}
			else
			{
				return SELECT_TYPE_REMOTE_ARMY;
			}
		}
	}
	else if(g_theWorld->IsGood(pos))
	{
		return SELECT_TYPE_GOOD;
	}

	return SELECT_TYPE_NONE;
}
