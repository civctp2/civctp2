//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : 
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
//----------------------------------------------------------------------------
//
// Modifications from the original Activision code:
//
// - Bug fix: prevent easy invisible unit detection, by reporting the basic 
//   terrain info when (right-)clicking on an enemy object.
//
//----------------------------------------------------------------------------

#include "c3.h"
#include "SelItem.h"
#include "Unit.h"
#include "Army.h"
#include "ProfileDB.h"
#include "QuickSlic.h"
#include "SlicObject.h"
#include "SlicEngine.h"
#include "GameEventManager.h"
#include "Path.h"
#include "World.h"
#include "UnitData.h"

#include "UnitRecord.h"

#include "aui.h"
#include "aui_uniqueid.h"
#include "aui_mouse.h"
#include "Cell.h"
#include "ControlPanelWindow.h"
#include "ArmyManagerWindow.h"
#include "CityWindow.h"

#include "helptile.h"
#include "Player.h"



#include "gfx_options.h"








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
					m_clickFunc[selected][clicked][button][mode] = NullClick;
				}
			}
		}
	}

	



	
	
	
	mode	= SELECT_MODE_RIGHT;
	button	= SELECT_BUTTON_LEFT;
	
	
	selected = SELECT_TYPE_NONE;

	m_clickFunc[selected][SELECT_TYPE_NONE]                [button][mode] = NullClick;	
	m_clickFunc[selected][SELECT_TYPE_LOCAL_ARMY]          [button][mode] = SelectArmyClick;
	m_clickFunc[selected][SELECT_TYPE_LOCAL_ARMY_UNLOADING][button][mode] = ErrorClick;
	m_clickFunc[selected][SELECT_TYPE_LOCAL_CITY]          [button][mode] = SelectCityClick;
	m_clickFunc[selected][SELECT_TYPE_REMOTE_CITY]         [button][mode] = SelectEnemyCityClick;
	m_clickFunc[selected][SELECT_TYPE_REMOTE_ARMY]         [button][mode] = SelectEnemyArmyClick;
	m_clickFunc[selected][SELECT_TYPE_TRADE_ROUTE]         [button][mode] = SelectTradeRouteClick;
	m_clickFunc[selected][SELECT_TYPE_GOOD]                [button][mode] = SelectGoodClick;

	
	mode = SELECT_MODE_RIGHT;
	button = SELECT_BUTTON_LEFT;
	selected = SELECT_TYPE_LOCAL_ARMY;

	m_clickFunc[selected][SELECT_TYPE_NONE]                [button][mode] = DeselectClick;
	m_clickFunc[selected][SELECT_TYPE_LOCAL_ARMY]          [button][mode] = SelectArmyClick;
	m_clickFunc[selected][SELECT_TYPE_LOCAL_ARMY_UNLOADING][button][mode] = ErrorClick;
	m_clickFunc[selected][SELECT_TYPE_LOCAL_CITY]          [button][mode] = SelectCityClick;
	m_clickFunc[selected][SELECT_TYPE_REMOTE_CITY]         [button][mode] = SelectEnemyCityClick;
	m_clickFunc[selected][SELECT_TYPE_REMOTE_ARMY]         [button][mode] = SelectEnemyArmyClick;
	m_clickFunc[selected][SELECT_TYPE_TRADE_ROUTE]         [button][mode] = SelectTradeRouteClick;
	m_clickFunc[selected][SELECT_TYPE_GOOD]                [button][mode] = SelectGoodClick;

	
	mode = SELECT_MODE_RIGHT;
	button = SELECT_BUTTON_LEFT;
	selected = SELECT_TYPE_LOCAL_CITY;

	m_clickFunc[selected][SELECT_TYPE_NONE]                [button][mode] = DeselectClick;
	m_clickFunc[selected][SELECT_TYPE_LOCAL_ARMY]          [button][mode] = SelectArmyClick;
	m_clickFunc[selected][SELECT_TYPE_LOCAL_ARMY_UNLOADING][button][mode] = ErrorClick;
	m_clickFunc[selected][SELECT_TYPE_LOCAL_CITY]          [button][mode] = SelectCityClick;
	m_clickFunc[selected][SELECT_TYPE_REMOTE_CITY]         [button][mode] = SelectEnemyCityClick;
	m_clickFunc[selected][SELECT_TYPE_REMOTE_ARMY]         [button][mode] = SelectEnemyArmyClick;
	m_clickFunc[selected][SELECT_TYPE_TRADE_ROUTE]         [button][mode] = SelectTradeRouteClick;
	m_clickFunc[selected][SELECT_TYPE_GOOD]                [button][mode] = SelectGoodClick;

	
	mode = SELECT_MODE_RIGHT;
	button = SELECT_BUTTON_LEFT;
	selected = SELECT_TYPE_LOCAL_ARMY_UNLOADING;

	m_clickFunc[selected][SELECT_TYPE_NONE]                [button][mode] = UnloadClick;
	m_clickFunc[selected][SELECT_TYPE_LOCAL_ARMY]          [button][mode] = UnloadClick;
	m_clickFunc[selected][SELECT_TYPE_LOCAL_ARMY_UNLOADING][button][mode] = ErrorClick;
	m_clickFunc[selected][SELECT_TYPE_LOCAL_CITY]          [button][mode] = UnloadClick;
	m_clickFunc[selected][SELECT_TYPE_REMOTE_CITY]         [button][mode] = UnloadClick;
	m_clickFunc[selected][SELECT_TYPE_REMOTE_ARMY]         [button][mode] = UnloadClick;
	m_clickFunc[selected][SELECT_TYPE_TRADE_ROUTE]         [button][mode] = UnloadClick;
	m_clickFunc[selected][SELECT_TYPE_GOOD]                [button][mode] = UnloadClick;

	
	mode = SELECT_MODE_RIGHT;
	button = SELECT_BUTTON_LEFT;
	selected = SELECT_TYPE_REMOTE_CITY;

	m_clickFunc[selected][SELECT_TYPE_NONE]                [button][mode] = DeselectClick;
	m_clickFunc[selected][SELECT_TYPE_LOCAL_ARMY]          [button][mode] = SelectArmyClick;
	m_clickFunc[selected][SELECT_TYPE_LOCAL_ARMY_UNLOADING][button][mode] = ErrorClick;
	m_clickFunc[selected][SELECT_TYPE_LOCAL_CITY]          [button][mode] = SelectCityClick;
	m_clickFunc[selected][SELECT_TYPE_REMOTE_CITY]         [button][mode] = SelectEnemyCityClick;
	m_clickFunc[selected][SELECT_TYPE_REMOTE_ARMY]         [button][mode] = SelectEnemyArmyClick;
	m_clickFunc[selected][SELECT_TYPE_TRADE_ROUTE]         [button][mode] = SelectTradeRouteClick;
	m_clickFunc[selected][SELECT_TYPE_GOOD]                [button][mode] = SelectGoodClick;

	
	mode = SELECT_MODE_RIGHT;
	button = SELECT_BUTTON_LEFT;
	selected = SELECT_TYPE_REMOTE_ARMY;

	m_clickFunc[selected][SELECT_TYPE_NONE]                [button][mode] = DeselectClick;
	m_clickFunc[selected][SELECT_TYPE_LOCAL_ARMY]          [button][mode] = SelectArmyClick;
	m_clickFunc[selected][SELECT_TYPE_LOCAL_ARMY_UNLOADING][button][mode] = ErrorClick;
	m_clickFunc[selected][SELECT_TYPE_LOCAL_CITY]          [button][mode] = SelectCityClick;
	m_clickFunc[selected][SELECT_TYPE_REMOTE_CITY]         [button][mode] = SelectEnemyCityClick;
	m_clickFunc[selected][SELECT_TYPE_REMOTE_ARMY]         [button][mode] = SelectEnemyArmyClick;
	m_clickFunc[selected][SELECT_TYPE_TRADE_ROUTE]         [button][mode] = SelectTradeRouteClick;
	m_clickFunc[selected][SELECT_TYPE_GOOD]                [button][mode] = SelectGoodClick;

	
	mode = SELECT_MODE_RIGHT;
	button = SELECT_BUTTON_LEFT;
	selected = SELECT_TYPE_GOOD;

	m_clickFunc[selected][SELECT_TYPE_NONE]                [button][mode] = DeselectClick;
	m_clickFunc[selected][SELECT_TYPE_LOCAL_ARMY]          [button][mode] = SelectArmyClick;
	m_clickFunc[selected][SELECT_TYPE_LOCAL_ARMY_UNLOADING][button][mode] = ErrorClick;
	m_clickFunc[selected][SELECT_TYPE_LOCAL_CITY]          [button][mode] = SelectCityClick;
	m_clickFunc[selected][SELECT_TYPE_REMOTE_CITY]         [button][mode] = SelectEnemyCityClick;
	m_clickFunc[selected][SELECT_TYPE_REMOTE_ARMY]         [button][mode] = SelectEnemyArmyClick;
	m_clickFunc[selected][SELECT_TYPE_TRADE_ROUTE]         [button][mode] = SelectTradeRouteClick;
	m_clickFunc[selected][SELECT_TYPE_GOOD]                [button][mode] = SelectGoodClick;

	
	mode = SELECT_MODE_RIGHT;
	button = SELECT_BUTTON_LEFT;
	selected = SELECT_TYPE_TRADE_ROUTE;

	m_clickFunc[selected][SELECT_TYPE_NONE]                [button][mode] = DeselectClick;
	m_clickFunc[selected][SELECT_TYPE_LOCAL_ARMY]          [button][mode] = SelectArmyClick;
	m_clickFunc[selected][SELECT_TYPE_LOCAL_ARMY_UNLOADING][button][mode] = ErrorClick;
	m_clickFunc[selected][SELECT_TYPE_LOCAL_CITY]          [button][mode] = SelectCityClick;
	m_clickFunc[selected][SELECT_TYPE_REMOTE_CITY]         [button][mode] = SelectEnemyCityClick;
	m_clickFunc[selected][SELECT_TYPE_REMOTE_ARMY]         [button][mode] = SelectEnemyArmyClick;
	m_clickFunc[selected][SELECT_TYPE_TRADE_ROUTE]         [button][mode] = SelectTradeRouteClick;
	m_clickFunc[selected][SELECT_TYPE_GOOD]                [button][mode] = SelectGoodClick;
	



	


	
	mode = SELECT_MODE_RIGHT;
	button = SELECT_BUTTON_RIGHT;
	selected = SELECT_TYPE_NONE;

	m_clickFunc[selected][SELECT_TYPE_NONE]                [button][mode] = TerrainContextClick;
	m_clickFunc[selected][SELECT_TYPE_LOCAL_ARMY]          [button][mode] = ArmyContextClick;
	m_clickFunc[selected][SELECT_TYPE_LOCAL_ARMY_UNLOADING][button][mode] = ErrorClick;
	m_clickFunc[selected][SELECT_TYPE_LOCAL_CITY]          [button][mode] = CityContextClick;
	m_clickFunc[selected][SELECT_TYPE_REMOTE_CITY]         [button][mode] = EnemyCityContextClick;
	m_clickFunc[selected][SELECT_TYPE_REMOTE_ARMY]         [button][mode] = EnemyArmyContextClick;
	m_clickFunc[selected][SELECT_TYPE_TRADE_ROUTE]         [button][mode] = TradeRouteContextClick;
	m_clickFunc[selected][SELECT_TYPE_GOOD]                [button][mode] = GoodContextClick;

	
	mode = SELECT_MODE_RIGHT;
	button = SELECT_BUTTON_RIGHT;
	selected = SELECT_TYPE_LOCAL_ARMY;

	m_clickFunc[selected][SELECT_TYPE_NONE]                [button][mode] = MoveArmyClick;
	m_clickFunc[selected][SELECT_TYPE_LOCAL_ARMY]          [button][mode] = MoveArmyClick;
	m_clickFunc[selected][SELECT_TYPE_LOCAL_ARMY_UNLOADING][button][mode] = ErrorClick;
	m_clickFunc[selected][SELECT_TYPE_LOCAL_CITY]          [button][mode] = MoveArmyClick;
	m_clickFunc[selected][SELECT_TYPE_REMOTE_CITY]         [button][mode] = MoveArmyClick;
	m_clickFunc[selected][SELECT_TYPE_REMOTE_ARMY]         [button][mode] = MoveArmyClick;
	m_clickFunc[selected][SELECT_TYPE_TRADE_ROUTE]         [button][mode] = MoveArmyClick;
	m_clickFunc[selected][SELECT_TYPE_GOOD]                [button][mode] = MoveArmyClick;

	
	mode = SELECT_MODE_RIGHT;
	button = SELECT_BUTTON_RIGHT;
	selected = SELECT_TYPE_LOCAL_CITY;

	m_clickFunc[selected][SELECT_TYPE_NONE]                [button][mode] = TerrainContextClick;
	m_clickFunc[selected][SELECT_TYPE_LOCAL_ARMY]          [button][mode] = ArmyContextClick;
	m_clickFunc[selected][SELECT_TYPE_LOCAL_ARMY_UNLOADING][button][mode] = ErrorClick;
	m_clickFunc[selected][SELECT_TYPE_LOCAL_CITY]          [button][mode] = CityContextClick;
	m_clickFunc[selected][SELECT_TYPE_REMOTE_CITY]         [button][mode] = EnemyCityContextClick;
	m_clickFunc[selected][SELECT_TYPE_REMOTE_ARMY]         [button][mode] = EnemyArmyContextClick;
	m_clickFunc[selected][SELECT_TYPE_TRADE_ROUTE]         [button][mode] = TradeRouteContextClick;
	m_clickFunc[selected][SELECT_TYPE_GOOD]                [button][mode] = GoodContextClick;

	
	mode = SELECT_MODE_RIGHT;
	button = SELECT_BUTTON_RIGHT;
	selected = SELECT_TYPE_LOCAL_ARMY_UNLOADING;

	m_clickFunc[selected][SELECT_TYPE_NONE]                [button][mode] = UnloadClick;
	m_clickFunc[selected][SELECT_TYPE_LOCAL_ARMY]          [button][mode] = UnloadClick;
	m_clickFunc[selected][SELECT_TYPE_LOCAL_ARMY_UNLOADING][button][mode] = ErrorClick;
	m_clickFunc[selected][SELECT_TYPE_LOCAL_CITY]          [button][mode] = UnloadClick;
	m_clickFunc[selected][SELECT_TYPE_REMOTE_CITY]         [button][mode] = UnloadClick;
	m_clickFunc[selected][SELECT_TYPE_REMOTE_ARMY]         [button][mode] = UnloadClick;
	m_clickFunc[selected][SELECT_TYPE_TRADE_ROUTE]         [button][mode] = UnloadClick;
	m_clickFunc[selected][SELECT_TYPE_GOOD]                [button][mode] = UnloadClick;

	
	mode = SELECT_MODE_RIGHT;
	button = SELECT_BUTTON_RIGHT;
	selected = SELECT_TYPE_REMOTE_CITY;

	m_clickFunc[selected][SELECT_TYPE_NONE]                [button][mode] = TerrainContextClick;
	m_clickFunc[selected][SELECT_TYPE_LOCAL_ARMY]          [button][mode] = ArmyContextClick;
	m_clickFunc[selected][SELECT_TYPE_LOCAL_ARMY_UNLOADING][button][mode] = ErrorClick;
	m_clickFunc[selected][SELECT_TYPE_LOCAL_CITY]          [button][mode] = CityContextClick;
	m_clickFunc[selected][SELECT_TYPE_REMOTE_CITY]         [button][mode] = EnemyCityContextClick;
	m_clickFunc[selected][SELECT_TYPE_REMOTE_ARMY]         [button][mode] = EnemyArmyContextClick;
	m_clickFunc[selected][SELECT_TYPE_TRADE_ROUTE]         [button][mode] = TradeRouteContextClick;
	m_clickFunc[selected][SELECT_TYPE_GOOD]                [button][mode] = GoodContextClick;

	
	mode = SELECT_MODE_RIGHT;
	button = SELECT_BUTTON_RIGHT;
	selected = SELECT_TYPE_REMOTE_ARMY;

	m_clickFunc[selected][SELECT_TYPE_NONE]                [button][mode] = TerrainContextClick;
	m_clickFunc[selected][SELECT_TYPE_LOCAL_ARMY]          [button][mode] = ArmyContextClick;
	m_clickFunc[selected][SELECT_TYPE_LOCAL_ARMY_UNLOADING][button][mode] = ErrorClick;
	m_clickFunc[selected][SELECT_TYPE_LOCAL_CITY]          [button][mode] = CityContextClick;
	m_clickFunc[selected][SELECT_TYPE_REMOTE_CITY]         [button][mode] = EnemyCityContextClick;
	m_clickFunc[selected][SELECT_TYPE_REMOTE_ARMY]         [button][mode] = EnemyArmyContextClick;
	m_clickFunc[selected][SELECT_TYPE_TRADE_ROUTE]         [button][mode] = TradeRouteContextClick;
	m_clickFunc[selected][SELECT_TYPE_GOOD]                [button][mode] = GoodContextClick;

	
	mode = SELECT_MODE_RIGHT;
	button = SELECT_BUTTON_RIGHT;
	selected = SELECT_TYPE_GOOD;

	m_clickFunc[selected][SELECT_TYPE_NONE]                [button][mode] = TerrainContextClick;
	m_clickFunc[selected][SELECT_TYPE_LOCAL_ARMY]          [button][mode] = ArmyContextClick;
	m_clickFunc[selected][SELECT_TYPE_LOCAL_ARMY_UNLOADING][button][mode] = ErrorClick;
	m_clickFunc[selected][SELECT_TYPE_LOCAL_CITY]          [button][mode] = SendGoodClick;
	m_clickFunc[selected][SELECT_TYPE_REMOTE_CITY]         [button][mode] = EnemyCityContextClick;
	m_clickFunc[selected][SELECT_TYPE_REMOTE_ARMY]         [button][mode] = EnemyArmyContextClick;
	m_clickFunc[selected][SELECT_TYPE_TRADE_ROUTE]         [button][mode] = TradeRouteContextClick;
	m_clickFunc[selected][SELECT_TYPE_GOOD]                [button][mode] = GoodContextClick;

	
	mode = SELECT_MODE_RIGHT;
	button = SELECT_BUTTON_RIGHT;
	selected = SELECT_TYPE_TRADE_ROUTE;

	m_clickFunc[selected][SELECT_TYPE_NONE]                [button][mode] = TerrainContextClick;
	m_clickFunc[selected][SELECT_TYPE_LOCAL_ARMY]          [button][mode] = ArmyContextClick;
	m_clickFunc[selected][SELECT_TYPE_LOCAL_ARMY_UNLOADING][button][mode] = ErrorClick;
	m_clickFunc[selected][SELECT_TYPE_LOCAL_CITY]          [button][mode] = CityContextClick;
	m_clickFunc[selected][SELECT_TYPE_REMOTE_CITY]         [button][mode] = EnemyCityContextClick;
	m_clickFunc[selected][SELECT_TYPE_REMOTE_ARMY]         [button][mode] = EnemyArmyContextClick;
	m_clickFunc[selected][SELECT_TYPE_TRADE_ROUTE]         [button][mode] = TradeRouteContextClick;
	m_clickFunc[selected][SELECT_TYPE_GOOD]                [button][mode] = GoodContextClick;
	


	


	


	
	mode = SELECT_MODE_LEFT;
	button = SELECT_BUTTON_LEFT;
	selected = SELECT_TYPE_NONE;

	m_clickFunc[selected][SELECT_TYPE_NONE]                [button][mode] = NullClick;	
	m_clickFunc[selected][SELECT_TYPE_LOCAL_ARMY]          [button][mode] = SelectArmyClick;
	m_clickFunc[selected][SELECT_TYPE_LOCAL_ARMY_UNLOADING][button][mode] = ErrorClick;
	m_clickFunc[selected][SELECT_TYPE_LOCAL_CITY]          [button][mode] = SelectCityClick;
	m_clickFunc[selected][SELECT_TYPE_REMOTE_CITY]         [button][mode] = SelectEnemyCityClick;
	m_clickFunc[selected][SELECT_TYPE_REMOTE_ARMY]         [button][mode] = SelectEnemyArmyClick;
	m_clickFunc[selected][SELECT_TYPE_TRADE_ROUTE]         [button][mode] = SelectTradeRouteClick;
	m_clickFunc[selected][SELECT_TYPE_GOOD]                [button][mode] = SelectGoodClick;

	
	mode = SELECT_MODE_LEFT;
	button = SELECT_BUTTON_LEFT;
	selected = SELECT_TYPE_LOCAL_ARMY;

	m_clickFunc[selected][SELECT_TYPE_NONE]                [button][mode] = MoveArmyClick;
	m_clickFunc[selected][SELECT_TYPE_LOCAL_ARMY]          [button][mode] = MoveArmyClick;
	m_clickFunc[selected][SELECT_TYPE_LOCAL_ARMY_UNLOADING][button][mode] = ErrorClick;
	m_clickFunc[selected][SELECT_TYPE_LOCAL_CITY]          [button][mode] = MoveArmyClick;
	m_clickFunc[selected][SELECT_TYPE_REMOTE_CITY]         [button][mode] = MoveArmyClick;
	m_clickFunc[selected][SELECT_TYPE_REMOTE_ARMY]         [button][mode] = MoveArmyClick;
	m_clickFunc[selected][SELECT_TYPE_TRADE_ROUTE]         [button][mode] = MoveArmyClick;
	m_clickFunc[selected][SELECT_TYPE_GOOD]                [button][mode] = MoveArmyClick;

	
	mode = SELECT_MODE_LEFT;
	button = SELECT_BUTTON_LEFT;
	selected = SELECT_TYPE_LOCAL_CITY;

	m_clickFunc[selected][SELECT_TYPE_NONE]                [button][mode] = DeselectClick;
	m_clickFunc[selected][SELECT_TYPE_LOCAL_ARMY]          [button][mode] = SelectArmyClick;
	m_clickFunc[selected][SELECT_TYPE_LOCAL_ARMY_UNLOADING][button][mode] = ErrorClick;
	m_clickFunc[selected][SELECT_TYPE_LOCAL_CITY]          [button][mode] = SelectCityClick;
	m_clickFunc[selected][SELECT_TYPE_REMOTE_CITY]         [button][mode] = SelectEnemyCityClick;
	m_clickFunc[selected][SELECT_TYPE_REMOTE_ARMY]         [button][mode] = SelectEnemyArmyClick;
	m_clickFunc[selected][SELECT_TYPE_TRADE_ROUTE]         [button][mode] = SelectTradeRouteClick;
	m_clickFunc[selected][SELECT_TYPE_GOOD]                [button][mode] = SelectGoodClick;

	
	mode = SELECT_MODE_LEFT;
	button = SELECT_BUTTON_LEFT;
	selected = SELECT_TYPE_LOCAL_ARMY_UNLOADING;

	m_clickFunc[selected][SELECT_TYPE_NONE]                [button][mode] = UnloadClick;
	m_clickFunc[selected][SELECT_TYPE_LOCAL_ARMY]          [button][mode] = UnloadClick;
	m_clickFunc[selected][SELECT_TYPE_LOCAL_ARMY_UNLOADING][button][mode] = ErrorClick;
	m_clickFunc[selected][SELECT_TYPE_LOCAL_CITY]          [button][mode] = UnloadClick;
	m_clickFunc[selected][SELECT_TYPE_REMOTE_CITY]         [button][mode] = UnloadClick;
	m_clickFunc[selected][SELECT_TYPE_REMOTE_ARMY]         [button][mode] = UnloadClick;
	m_clickFunc[selected][SELECT_TYPE_TRADE_ROUTE]         [button][mode] = UnloadClick;
	m_clickFunc[selected][SELECT_TYPE_GOOD]                [button][mode] = UnloadClick;

	
	mode = SELECT_MODE_LEFT;
	button = SELECT_BUTTON_LEFT;
	selected = SELECT_TYPE_REMOTE_CITY;

	m_clickFunc[selected][SELECT_TYPE_NONE]                [button][mode] = DeselectClick;
	m_clickFunc[selected][SELECT_TYPE_LOCAL_ARMY]          [button][mode] = SelectArmyClick;
	m_clickFunc[selected][SELECT_TYPE_LOCAL_ARMY_UNLOADING][button][mode] = ErrorClick;
	m_clickFunc[selected][SELECT_TYPE_LOCAL_CITY]          [button][mode] = SelectCityClick;
	m_clickFunc[selected][SELECT_TYPE_REMOTE_CITY]         [button][mode] = SelectEnemyCityClick;
	m_clickFunc[selected][SELECT_TYPE_REMOTE_ARMY]         [button][mode] = SelectEnemyArmyClick;
	m_clickFunc[selected][SELECT_TYPE_TRADE_ROUTE]         [button][mode] = SelectTradeRouteClick;
	m_clickFunc[selected][SELECT_TYPE_GOOD]                [button][mode] = SelectGoodClick;

	
	mode = SELECT_MODE_LEFT;
	button = SELECT_BUTTON_LEFT;
	selected = SELECT_TYPE_REMOTE_ARMY;

	m_clickFunc[selected][SELECT_TYPE_NONE]                [button][mode] = DeselectClick;
	m_clickFunc[selected][SELECT_TYPE_LOCAL_ARMY]          [button][mode] = SelectArmyClick;
	m_clickFunc[selected][SELECT_TYPE_LOCAL_ARMY_UNLOADING][button][mode] = ErrorClick;
	m_clickFunc[selected][SELECT_TYPE_LOCAL_CITY]          [button][mode] = SelectCityClick;
	m_clickFunc[selected][SELECT_TYPE_REMOTE_CITY]         [button][mode] = SelectEnemyCityClick;
	m_clickFunc[selected][SELECT_TYPE_REMOTE_ARMY]         [button][mode] = SelectEnemyArmyClick;
	m_clickFunc[selected][SELECT_TYPE_TRADE_ROUTE]         [button][mode] = SelectTradeRouteClick;
	m_clickFunc[selected][SELECT_TYPE_GOOD]                [button][mode] = SelectGoodClick;

	
	mode = SELECT_MODE_LEFT;
	button = SELECT_BUTTON_LEFT;
	selected = SELECT_TYPE_GOOD;

	m_clickFunc[selected][SELECT_TYPE_NONE]                [button][mode] = DeselectClick;
	m_clickFunc[selected][SELECT_TYPE_LOCAL_ARMY]          [button][mode] = SelectArmyClick;
	m_clickFunc[selected][SELECT_TYPE_LOCAL_ARMY_UNLOADING][button][mode] = ErrorClick;
	m_clickFunc[selected][SELECT_TYPE_LOCAL_CITY]          [button][mode] = SelectCityClick;
	m_clickFunc[selected][SELECT_TYPE_REMOTE_CITY]         [button][mode] = SelectEnemyCityClick;
	m_clickFunc[selected][SELECT_TYPE_REMOTE_ARMY]         [button][mode] = SelectEnemyArmyClick;
	m_clickFunc[selected][SELECT_TYPE_TRADE_ROUTE]         [button][mode] = SelectTradeRouteClick;
	m_clickFunc[selected][SELECT_TYPE_GOOD]                [button][mode] = SelectGoodClick;

	
	mode = SELECT_MODE_LEFT;
	button = SELECT_BUTTON_LEFT;
	selected = SELECT_TYPE_TRADE_ROUTE;

	m_clickFunc[selected][SELECT_TYPE_NONE]                [button][mode] = DeselectClick;
	m_clickFunc[selected][SELECT_TYPE_LOCAL_ARMY]          [button][mode] = SelectArmyClick;
	m_clickFunc[selected][SELECT_TYPE_LOCAL_ARMY_UNLOADING][button][mode] = ErrorClick;
	m_clickFunc[selected][SELECT_TYPE_LOCAL_CITY]          [button][mode] = SelectCityClick;
	m_clickFunc[selected][SELECT_TYPE_REMOTE_CITY]         [button][mode] = SelectEnemyCityClick;
	m_clickFunc[selected][SELECT_TYPE_REMOTE_ARMY]         [button][mode] = SelectEnemyArmyClick;
	m_clickFunc[selected][SELECT_TYPE_TRADE_ROUTE]         [button][mode] = SelectTradeRouteClick;
	m_clickFunc[selected][SELECT_TYPE_GOOD]                [button][mode] = SelectGoodClick;
	


	


	
	mode = SELECT_MODE_LEFT;
	button = SELECT_BUTTON_RIGHT;
	selected = SELECT_TYPE_NONE;

	m_clickFunc[selected][SELECT_TYPE_NONE]                [button][mode] = TerrainContextClick;
	m_clickFunc[selected][SELECT_TYPE_LOCAL_ARMY]          [button][mode] = ArmyContextClick;
	m_clickFunc[selected][SELECT_TYPE_LOCAL_ARMY_UNLOADING][button][mode] = ErrorClick;
	m_clickFunc[selected][SELECT_TYPE_LOCAL_CITY]          [button][mode] = CityContextClick;
	m_clickFunc[selected][SELECT_TYPE_REMOTE_CITY]         [button][mode] = EnemyCityContextClick;
	m_clickFunc[selected][SELECT_TYPE_REMOTE_ARMY]         [button][mode] = EnemyArmyContextClick;
	m_clickFunc[selected][SELECT_TYPE_TRADE_ROUTE]         [button][mode] = TradeRouteContextClick;
	m_clickFunc[selected][SELECT_TYPE_GOOD]                [button][mode] = GoodContextClick;

	
	mode = SELECT_MODE_LEFT;
	button = SELECT_BUTTON_RIGHT;
	selected = SELECT_TYPE_LOCAL_ARMY;

	m_clickFunc[selected][SELECT_TYPE_NONE]                [button][mode] = DeselectClick;
	m_clickFunc[selected][SELECT_TYPE_LOCAL_ARMY]          [button][mode] = DeselectClick;
	m_clickFunc[selected][SELECT_TYPE_LOCAL_ARMY_UNLOADING][button][mode] = ErrorClick;
	m_clickFunc[selected][SELECT_TYPE_LOCAL_CITY]          [button][mode] = DeselectClick;
	m_clickFunc[selected][SELECT_TYPE_REMOTE_CITY]         [button][mode] = DeselectClick;
	m_clickFunc[selected][SELECT_TYPE_REMOTE_ARMY]         [button][mode] = DeselectClick;
	m_clickFunc[selected][SELECT_TYPE_TRADE_ROUTE]         [button][mode] = DeselectClick;
	m_clickFunc[selected][SELECT_TYPE_GOOD]                [button][mode] = DeselectClick;

	
	mode = SELECT_MODE_LEFT;
	button = SELECT_BUTTON_RIGHT;
	selected = SELECT_TYPE_LOCAL_CITY;

	m_clickFunc[selected][SELECT_TYPE_NONE]                [button][mode] = TerrainContextClick;
	m_clickFunc[selected][SELECT_TYPE_LOCAL_ARMY]          [button][mode] = ArmyContextClick;
	m_clickFunc[selected][SELECT_TYPE_LOCAL_ARMY_UNLOADING][button][mode] = ErrorClick;
	m_clickFunc[selected][SELECT_TYPE_LOCAL_CITY]          [button][mode] = CityContextClick;
	m_clickFunc[selected][SELECT_TYPE_REMOTE_CITY]         [button][mode] = EnemyCityContextClick;
	m_clickFunc[selected][SELECT_TYPE_REMOTE_ARMY]         [button][mode] = EnemyArmyContextClick;
	m_clickFunc[selected][SELECT_TYPE_TRADE_ROUTE]         [button][mode] = TradeRouteContextClick;
	m_clickFunc[selected][SELECT_TYPE_GOOD]                [button][mode] = GoodContextClick;

	
	mode = SELECT_MODE_LEFT;
	button = SELECT_BUTTON_RIGHT;
	selected = SELECT_TYPE_LOCAL_ARMY_UNLOADING;

	m_clickFunc[selected][SELECT_TYPE_NONE]                [button][mode] = UnloadClick;
	m_clickFunc[selected][SELECT_TYPE_LOCAL_ARMY]          [button][mode] = UnloadClick;
	m_clickFunc[selected][SELECT_TYPE_LOCAL_ARMY_UNLOADING][button][mode] = ErrorClick;
	m_clickFunc[selected][SELECT_TYPE_LOCAL_CITY]          [button][mode] = UnloadClick;
	m_clickFunc[selected][SELECT_TYPE_REMOTE_CITY]         [button][mode] = UnloadClick;
	m_clickFunc[selected][SELECT_TYPE_REMOTE_ARMY]         [button][mode] = UnloadClick;
	m_clickFunc[selected][SELECT_TYPE_TRADE_ROUTE]         [button][mode] = UnloadClick;
	m_clickFunc[selected][SELECT_TYPE_GOOD]                [button][mode] = UnloadClick;

	
	mode = SELECT_MODE_LEFT;
	button = SELECT_BUTTON_RIGHT;
	selected = SELECT_TYPE_REMOTE_CITY;

	m_clickFunc[selected][SELECT_TYPE_NONE]                [button][mode] = TerrainContextClick;
	m_clickFunc[selected][SELECT_TYPE_LOCAL_ARMY]          [button][mode] = ArmyContextClick;
	m_clickFunc[selected][SELECT_TYPE_LOCAL_ARMY_UNLOADING][button][mode] = ErrorClick;
	m_clickFunc[selected][SELECT_TYPE_LOCAL_CITY]          [button][mode] = CityContextClick;
	m_clickFunc[selected][SELECT_TYPE_REMOTE_CITY]         [button][mode] = EnemyCityContextClick;
	m_clickFunc[selected][SELECT_TYPE_REMOTE_ARMY]         [button][mode] = EnemyArmyContextClick;
	m_clickFunc[selected][SELECT_TYPE_TRADE_ROUTE]         [button][mode] = TradeRouteContextClick;
	m_clickFunc[selected][SELECT_TYPE_GOOD]                [button][mode] = GoodContextClick;

	
	mode = SELECT_MODE_LEFT;
	button = SELECT_BUTTON_RIGHT;
	selected = SELECT_TYPE_REMOTE_ARMY;

	m_clickFunc[selected][SELECT_TYPE_NONE]                [button][mode] = TerrainContextClick;
	m_clickFunc[selected][SELECT_TYPE_LOCAL_ARMY]          [button][mode] = ArmyContextClick;
	m_clickFunc[selected][SELECT_TYPE_LOCAL_ARMY_UNLOADING][button][mode] = ErrorClick;
	m_clickFunc[selected][SELECT_TYPE_LOCAL_CITY]          [button][mode] = CityContextClick;
	m_clickFunc[selected][SELECT_TYPE_REMOTE_CITY]         [button][mode] = EnemyCityContextClick;
	m_clickFunc[selected][SELECT_TYPE_REMOTE_ARMY]         [button][mode] = EnemyArmyContextClick;
	m_clickFunc[selected][SELECT_TYPE_TRADE_ROUTE]         [button][mode] = TradeRouteContextClick;
	m_clickFunc[selected][SELECT_TYPE_GOOD]                [button][mode] = GoodContextClick;

	
	mode = SELECT_MODE_LEFT;
	button = SELECT_BUTTON_RIGHT;
	selected = SELECT_TYPE_GOOD;

	m_clickFunc[selected][SELECT_TYPE_NONE]                [button][mode] = TerrainContextClick;
	m_clickFunc[selected][SELECT_TYPE_LOCAL_ARMY]          [button][mode] = ArmyContextClick;
	m_clickFunc[selected][SELECT_TYPE_LOCAL_ARMY_UNLOADING][button][mode] = ErrorClick;
	m_clickFunc[selected][SELECT_TYPE_LOCAL_CITY]          [button][mode] = SendGoodClick;
	m_clickFunc[selected][SELECT_TYPE_REMOTE_CITY]         [button][mode] = EnemyCityContextClick;
	m_clickFunc[selected][SELECT_TYPE_REMOTE_ARMY]         [button][mode] = EnemyArmyContextClick;
	m_clickFunc[selected][SELECT_TYPE_TRADE_ROUTE]         [button][mode] = TradeRouteContextClick;
	m_clickFunc[selected][SELECT_TYPE_GOOD]                [button][mode] = GoodContextClick;

	
	mode = SELECT_MODE_LEFT;
	button = SELECT_BUTTON_RIGHT;
	selected = SELECT_TYPE_TRADE_ROUTE;

	m_clickFunc[selected][SELECT_TYPE_NONE]                [button][mode] = TerrainContextClick;
	m_clickFunc[selected][SELECT_TYPE_LOCAL_ARMY]          [button][mode] = ArmyContextClick;
	m_clickFunc[selected][SELECT_TYPE_LOCAL_ARMY_UNLOADING][button][mode] = ErrorClick;
	m_clickFunc[selected][SELECT_TYPE_LOCAL_CITY]          [button][mode] = CityContextClick;
	m_clickFunc[selected][SELECT_TYPE_REMOTE_CITY]         [button][mode] = EnemyCityContextClick;
	m_clickFunc[selected][SELECT_TYPE_REMOTE_ARMY]         [button][mode] = EnemyArmyContextClick;
	m_clickFunc[selected][SELECT_TYPE_TRADE_ROUTE]         [button][mode] = TradeRouteContextClick;
	m_clickFunc[selected][SELECT_TYPE_GOOD]                [button][mode] = GoodContextClick;
	


	
	

	


	


	
	mode = SELECT_MODE_CTP2;
	button = SELECT_BUTTON_LEFT;
	selected = SELECT_TYPE_NONE;

	m_clickFunc[selected][SELECT_TYPE_NONE]                [button][mode] = NullClick;	
	m_clickFunc[selected][SELECT_TYPE_LOCAL_ARMY]          [button][mode] = SelectArmyStartMoveClick;
	m_clickFunc[selected][SELECT_TYPE_LOCAL_ARMY_UNLOADING][button][mode] = ErrorClick;
	m_clickFunc[selected][SELECT_TYPE_LOCAL_CITY]          [button][mode] = SelectCityClick;
	m_clickFunc[selected][SELECT_TYPE_REMOTE_CITY]         [button][mode] = SelectEnemyCityClick;
	m_clickFunc[selected][SELECT_TYPE_REMOTE_ARMY]         [button][mode] = SelectEnemyArmyClick;
	m_clickFunc[selected][SELECT_TYPE_TRADE_ROUTE]         [button][mode] = SelectTradeRouteClick;
	m_clickFunc[selected][SELECT_TYPE_GOOD]                [button][mode] = SelectGoodClick;

	
	mode = SELECT_MODE_CTP2;
	button = SELECT_BUTTON_LEFT_PATHING;
	selected = SELECT_TYPE_NONE;

	m_clickFunc[selected][SELECT_TYPE_NONE]                [button][mode] = ActionClick;
	m_clickFunc[selected][SELECT_TYPE_LOCAL_ARMY]          [button][mode] = ActionClick;
	m_clickFunc[selected][SELECT_TYPE_LOCAL_ARMY_UNLOADING][button][mode] = ErrorClick;
	m_clickFunc[selected][SELECT_TYPE_LOCAL_CITY]          [button][mode] = ActionClick;
	m_clickFunc[selected][SELECT_TYPE_REMOTE_CITY]         [button][mode] = ActionClick;
	m_clickFunc[selected][SELECT_TYPE_REMOTE_ARMY]         [button][mode] = ActionClick;
	m_clickFunc[selected][SELECT_TYPE_TRADE_ROUTE]         [button][mode] = ActionClick;
	m_clickFunc[selected][SELECT_TYPE_GOOD]                [button][mode] = ActionClick;

	
	mode = SELECT_MODE_CTP2;
	button = SELECT_BUTTON_LEFT;
	selected = SELECT_TYPE_LOCAL_ARMY;

	m_clickFunc[selected][SELECT_TYPE_NONE]                [button][mode] = DeselectClick; 
	m_clickFunc[selected][SELECT_TYPE_LOCAL_ARMY]          [button][mode] = SelectArmyStartMoveClick;
	m_clickFunc[selected][SELECT_TYPE_LOCAL_ARMY_UNLOADING][button][mode] = ErrorClick;
	m_clickFunc[selected][SELECT_TYPE_LOCAL_CITY]          [button][mode] = SelectCityClick;
	m_clickFunc[selected][SELECT_TYPE_REMOTE_CITY]         [button][mode] = SelectEnemyCityClick;
	m_clickFunc[selected][SELECT_TYPE_REMOTE_ARMY]         [button][mode] = SelectEnemyArmyClick;
	m_clickFunc[selected][SELECT_TYPE_TRADE_ROUTE]         [button][mode] = SelectTradeRouteClick;
	m_clickFunc[selected][SELECT_TYPE_GOOD]                [button][mode] = SelectGoodClick;

	
	mode = SELECT_MODE_CTP2;
	button = SELECT_BUTTON_LEFT_PATHING;
	selected = SELECT_TYPE_LOCAL_ARMY;

	m_clickFunc[selected][SELECT_TYPE_NONE]                [button][mode] = ActionClick;
	m_clickFunc[selected][SELECT_TYPE_LOCAL_ARMY]          [button][mode] = ActionClick;
	m_clickFunc[selected][SELECT_TYPE_LOCAL_ARMY_UNLOADING][button][mode] = ErrorClick;
	m_clickFunc[selected][SELECT_TYPE_LOCAL_CITY]          [button][mode] = ActionClick;
	m_clickFunc[selected][SELECT_TYPE_REMOTE_CITY]         [button][mode] = ActionClick;
	m_clickFunc[selected][SELECT_TYPE_REMOTE_ARMY]         [button][mode] = ActionClick;
	m_clickFunc[selected][SELECT_TYPE_TRADE_ROUTE]         [button][mode] = ActionClick;
	m_clickFunc[selected][SELECT_TYPE_GOOD]                [button][mode] = ActionClick;

	
	mode = SELECT_MODE_CTP2;
	button = SELECT_BUTTON_LEFT_DRAG;
	selected = SELECT_TYPE_LOCAL_ARMY;

	m_clickFunc[selected][SELECT_TYPE_NONE]                [button][mode] = MoveDrag;
	m_clickFunc[selected][SELECT_TYPE_LOCAL_ARMY]          [button][mode] = MoveDrag;
	m_clickFunc[selected][SELECT_TYPE_LOCAL_ARMY_UNLOADING][button][mode] = ErrorClick;
	m_clickFunc[selected][SELECT_TYPE_LOCAL_CITY]          [button][mode] = MoveDrag;
	m_clickFunc[selected][SELECT_TYPE_REMOTE_CITY]         [button][mode] = MoveDrag;
	m_clickFunc[selected][SELECT_TYPE_REMOTE_ARMY]         [button][mode] = MoveDrag;
	m_clickFunc[selected][SELECT_TYPE_TRADE_ROUTE]         [button][mode] = MoveDrag;
	m_clickFunc[selected][SELECT_TYPE_GOOD]                [button][mode] = MoveDrag;
	
	
	mode = SELECT_MODE_CTP2;
	button = SELECT_BUTTON_LEFT_DROP;
	selected = SELECT_TYPE_LOCAL_ARMY;

	m_clickFunc[selected][SELECT_TYPE_NONE]                [button][mode] = MoveDrop;
	m_clickFunc[selected][SELECT_TYPE_LOCAL_ARMY]          [button][mode] = MoveDrop;
	m_clickFunc[selected][SELECT_TYPE_LOCAL_ARMY_UNLOADING][button][mode] = ErrorClick;
	m_clickFunc[selected][SELECT_TYPE_LOCAL_CITY]          [button][mode] = MoveDrop;
	m_clickFunc[selected][SELECT_TYPE_REMOTE_CITY]         [button][mode] = MoveDrop;
	m_clickFunc[selected][SELECT_TYPE_REMOTE_ARMY]         [button][mode] = MoveDrop;
	m_clickFunc[selected][SELECT_TYPE_TRADE_ROUTE]         [button][mode] = MoveDrop;
	m_clickFunc[selected][SELECT_TYPE_GOOD]                [button][mode] = MoveDrop;
	

	
	mode = SELECT_MODE_CTP2;
	button = SELECT_BUTTON_LEFT;
	selected = SELECT_TYPE_LOCAL_CITY;

	m_clickFunc[selected][SELECT_TYPE_NONE]                [button][mode] = DeselectClick;
	m_clickFunc[selected][SELECT_TYPE_LOCAL_ARMY]          [button][mode] = SelectArmyStartMoveClick;
	m_clickFunc[selected][SELECT_TYPE_LOCAL_ARMY_UNLOADING][button][mode] = ErrorClick;
	m_clickFunc[selected][SELECT_TYPE_LOCAL_CITY]          [button][mode] = SelectCityClick;
	m_clickFunc[selected][SELECT_TYPE_REMOTE_CITY]         [button][mode] = SelectEnemyCityClick;
	m_clickFunc[selected][SELECT_TYPE_REMOTE_ARMY]         [button][mode] = SelectEnemyArmyClick;
	m_clickFunc[selected][SELECT_TYPE_TRADE_ROUTE]         [button][mode] = SelectTradeRouteClick;
	m_clickFunc[selected][SELECT_TYPE_GOOD]                [button][mode] = SelectGoodClick;

	
	mode = SELECT_MODE_CTP2;
	button = SELECT_BUTTON_LEFT_DROP;
	selected = SELECT_TYPE_LOCAL_CITY;

	m_clickFunc[selected][SELECT_TYPE_NONE]                [button][mode] = DeselectClick;
	m_clickFunc[selected][SELECT_TYPE_LOCAL_ARMY]          [button][mode] = SelectArmyClick;
	m_clickFunc[selected][SELECT_TYPE_LOCAL_ARMY_UNLOADING][button][mode] = ErrorClick;
	m_clickFunc[selected][SELECT_TYPE_LOCAL_CITY]          [button][mode] = SelectCityClick;
	m_clickFunc[selected][SELECT_TYPE_REMOTE_CITY]         [button][mode] = SelectEnemyCityClick;
	m_clickFunc[selected][SELECT_TYPE_REMOTE_ARMY]         [button][mode] = SelectEnemyArmyClick;
	m_clickFunc[selected][SELECT_TYPE_TRADE_ROUTE]         [button][mode] = SelectTradeRouteClick;
	m_clickFunc[selected][SELECT_TYPE_GOOD]                [button][mode] = SelectGoodClick;

	
	mode = SELECT_MODE_CTP2;
	button = SELECT_BUTTON_LEFT_PATHING;
	selected = SELECT_TYPE_LOCAL_CITY;

	m_clickFunc[selected][SELECT_TYPE_NONE]                [button][mode] = ActionClick;
	m_clickFunc[selected][SELECT_TYPE_LOCAL_ARMY]          [button][mode] = ActionClick;
	m_clickFunc[selected][SELECT_TYPE_LOCAL_ARMY_UNLOADING][button][mode] = ErrorClick;
	m_clickFunc[selected][SELECT_TYPE_LOCAL_CITY]          [button][mode] = ActionClick;
	m_clickFunc[selected][SELECT_TYPE_REMOTE_CITY]         [button][mode] = ActionClick;
	m_clickFunc[selected][SELECT_TYPE_REMOTE_ARMY]         [button][mode] = ActionClick;
	m_clickFunc[selected][SELECT_TYPE_TRADE_ROUTE]         [button][mode] = ActionClick;
	m_clickFunc[selected][SELECT_TYPE_GOOD]                [button][mode] = ActionClick;

	
	mode = SELECT_MODE_CTP2;
	button = SELECT_BUTTON_LEFT_PATHING;
	selected = SELECT_TYPE_GOOD;

	m_clickFunc[selected][SELECT_TYPE_NONE]                [button][mode] = ActionClick;
	m_clickFunc[selected][SELECT_TYPE_LOCAL_ARMY]          [button][mode] = ActionClick;
	m_clickFunc[selected][SELECT_TYPE_LOCAL_ARMY_UNLOADING][button][mode] = ErrorClick;
	m_clickFunc[selected][SELECT_TYPE_LOCAL_CITY]          [button][mode] = ActionClick;
	m_clickFunc[selected][SELECT_TYPE_REMOTE_CITY]         [button][mode] = ActionClick;
	m_clickFunc[selected][SELECT_TYPE_REMOTE_ARMY]         [button][mode] = ActionClick;
	m_clickFunc[selected][SELECT_TYPE_TRADE_ROUTE]         [button][mode] = ActionClick;
	m_clickFunc[selected][SELECT_TYPE_GOOD]                [button][mode] = ActionClick;

	
	mode = SELECT_MODE_CTP2;
	button = SELECT_BUTTON_LEFT;
	selected = SELECT_TYPE_LOCAL_ARMY_UNLOADING;

	m_clickFunc[selected][SELECT_TYPE_NONE]                [button][mode] = UnloadClick;
	m_clickFunc[selected][SELECT_TYPE_LOCAL_ARMY]          [button][mode] = UnloadClick;
	m_clickFunc[selected][SELECT_TYPE_LOCAL_ARMY_UNLOADING][button][mode] = ErrorClick;
	m_clickFunc[selected][SELECT_TYPE_LOCAL_CITY]          [button][mode] = UnloadClick;
	m_clickFunc[selected][SELECT_TYPE_REMOTE_CITY]         [button][mode] = UnloadClick;
	m_clickFunc[selected][SELECT_TYPE_REMOTE_ARMY]         [button][mode] = UnloadClick;
	m_clickFunc[selected][SELECT_TYPE_TRADE_ROUTE]         [button][mode] = UnloadClick;
	m_clickFunc[selected][SELECT_TYPE_GOOD]                [button][mode] = UnloadClick;

	
	mode = SELECT_MODE_CTP2;
	button = SELECT_BUTTON_LEFT;
	selected = SELECT_TYPE_REMOTE_CITY;

	m_clickFunc[selected][SELECT_TYPE_NONE]                [button][mode] = DeselectClick;
	m_clickFunc[selected][SELECT_TYPE_LOCAL_ARMY]          [button][mode] = SelectArmyClick;
	m_clickFunc[selected][SELECT_TYPE_LOCAL_ARMY_UNLOADING][button][mode] = ErrorClick;
	m_clickFunc[selected][SELECT_TYPE_LOCAL_CITY]          [button][mode] = SelectCityClick;
	m_clickFunc[selected][SELECT_TYPE_REMOTE_CITY]         [button][mode] = SelectEnemyCityClick;
	m_clickFunc[selected][SELECT_TYPE_REMOTE_ARMY]         [button][mode] = SelectEnemyArmyClick;
	m_clickFunc[selected][SELECT_TYPE_TRADE_ROUTE]         [button][mode] = SelectTradeRouteClick;
	m_clickFunc[selected][SELECT_TYPE_GOOD]                [button][mode] = SelectGoodClick;

	
	mode = SELECT_MODE_CTP2;
	button = SELECT_BUTTON_LEFT;
	selected = SELECT_TYPE_REMOTE_ARMY;

	m_clickFunc[selected][SELECT_TYPE_NONE]                [button][mode] = DeselectClick;
	m_clickFunc[selected][SELECT_TYPE_LOCAL_ARMY]          [button][mode] = SelectArmyClick;
	m_clickFunc[selected][SELECT_TYPE_LOCAL_ARMY_UNLOADING][button][mode] = ErrorClick;
	m_clickFunc[selected][SELECT_TYPE_LOCAL_CITY]          [button][mode] = SelectCityClick;
	m_clickFunc[selected][SELECT_TYPE_REMOTE_CITY]         [button][mode] = SelectEnemyCityClick;
	m_clickFunc[selected][SELECT_TYPE_REMOTE_ARMY]         [button][mode] = SelectEnemyArmyClick;
	m_clickFunc[selected][SELECT_TYPE_TRADE_ROUTE]         [button][mode] = SelectTradeRouteClick;
	m_clickFunc[selected][SELECT_TYPE_GOOD]                [button][mode] = SelectGoodClick;

	
	
	mode = SELECT_MODE_CTP2;
	button = SELECT_BUTTON_LEFT;
	selected = SELECT_TYPE_GOOD;

	m_clickFunc[selected][SELECT_TYPE_NONE]                [button][mode] = DeselectClick;
	m_clickFunc[selected][SELECT_TYPE_LOCAL_ARMY]          [button][mode] = SelectArmyStartMoveClick;
	m_clickFunc[selected][SELECT_TYPE_LOCAL_ARMY_UNLOADING][button][mode] = ErrorClick;
	m_clickFunc[selected][SELECT_TYPE_LOCAL_CITY]          [button][mode] = SelectCityClick;
	m_clickFunc[selected][SELECT_TYPE_REMOTE_CITY]         [button][mode] = SelectEnemyCityClick;
	m_clickFunc[selected][SELECT_TYPE_REMOTE_ARMY]         [button][mode] = SelectEnemyArmyClick;
	m_clickFunc[selected][SELECT_TYPE_TRADE_ROUTE]         [button][mode] = SelectTradeRouteClick;
	m_clickFunc[selected][SELECT_TYPE_GOOD]                [button][mode] = SelectGoodClick;

	
	mode = SELECT_MODE_CTP2;
	button = SELECT_BUTTON_LEFT_DROP;
	selected = SELECT_TYPE_GOOD;

	m_clickFunc[selected][SELECT_TYPE_NONE]                [button][mode] = DeselectClick;
	m_clickFunc[selected][SELECT_TYPE_LOCAL_ARMY]          [button][mode] = SelectArmyClick;
	m_clickFunc[selected][SELECT_TYPE_LOCAL_ARMY_UNLOADING][button][mode] = ErrorClick;
	m_clickFunc[selected][SELECT_TYPE_LOCAL_CITY]          [button][mode] = SelectCityClick;
	m_clickFunc[selected][SELECT_TYPE_REMOTE_CITY]         [button][mode] = SelectEnemyCityClick;
	m_clickFunc[selected][SELECT_TYPE_REMOTE_ARMY]         [button][mode] = SelectEnemyArmyClick;
	m_clickFunc[selected][SELECT_TYPE_TRADE_ROUTE]         [button][mode] = SelectTradeRouteClick;
	m_clickFunc[selected][SELECT_TYPE_GOOD]                [button][mode] = SelectGoodClick;

	
	mode = SELECT_MODE_CTP2;
	button = SELECT_BUTTON_LEFT;
	selected = SELECT_TYPE_TRADE_ROUTE;

	m_clickFunc[selected][SELECT_TYPE_NONE]                [button][mode] = DeselectClick;
	m_clickFunc[selected][SELECT_TYPE_LOCAL_ARMY]          [button][mode] = SelectArmyClick;
	m_clickFunc[selected][SELECT_TYPE_LOCAL_ARMY_UNLOADING][button][mode] = ErrorClick;
	m_clickFunc[selected][SELECT_TYPE_LOCAL_CITY]          [button][mode] = SelectCityClick;
	m_clickFunc[selected][SELECT_TYPE_REMOTE_CITY]         [button][mode] = SelectEnemyCityClick;
	m_clickFunc[selected][SELECT_TYPE_REMOTE_ARMY]         [button][mode] = SelectEnemyArmyClick;
	m_clickFunc[selected][SELECT_TYPE_TRADE_ROUTE]         [button][mode] = SelectTradeRouteClick;
	m_clickFunc[selected][SELECT_TYPE_GOOD]                [button][mode] = SelectGoodClick;
	



	


	
	mode = SELECT_MODE_CTP2;
	button = SELECT_BUTTON_RIGHT;
	selected = SELECT_TYPE_NONE;

	m_clickFunc[selected][SELECT_TYPE_NONE]                [button][mode] = TerrainContextClick;
	m_clickFunc[selected][SELECT_TYPE_LOCAL_ARMY]          [button][mode] = ArmyContextClick;
	m_clickFunc[selected][SELECT_TYPE_LOCAL_ARMY_UNLOADING][button][mode] = ErrorClick;
	m_clickFunc[selected][SELECT_TYPE_LOCAL_CITY]          [button][mode] = CityContextClick;
	m_clickFunc[selected][SELECT_TYPE_REMOTE_CITY]         [button][mode] = EnemyCityContextClick;
	m_clickFunc[selected][SELECT_TYPE_REMOTE_ARMY]         [button][mode] = EnemyArmyContextClick;
	m_clickFunc[selected][SELECT_TYPE_TRADE_ROUTE]         [button][mode] = TradeRouteContextClick;
	m_clickFunc[selected][SELECT_TYPE_GOOD]                [button][mode] = GoodContextClick;

	
	mode = SELECT_MODE_CTP2;
	button = SELECT_BUTTON_RIGHT;
	selected = SELECT_TYPE_LOCAL_ARMY;

	m_clickFunc[selected][SELECT_TYPE_NONE]                [button][mode] = TerrainContextClick;
	m_clickFunc[selected][SELECT_TYPE_LOCAL_ARMY]          [button][mode] = ArmyContextClick;
	m_clickFunc[selected][SELECT_TYPE_LOCAL_ARMY_UNLOADING][button][mode] = ErrorClick;
	m_clickFunc[selected][SELECT_TYPE_LOCAL_CITY]          [button][mode] = CityContextClick;
	m_clickFunc[selected][SELECT_TYPE_REMOTE_CITY]         [button][mode] = EnemyCityContextClick;
	m_clickFunc[selected][SELECT_TYPE_REMOTE_ARMY]         [button][mode] = EnemyArmyContextClick;
	m_clickFunc[selected][SELECT_TYPE_TRADE_ROUTE]         [button][mode] = TradeRouteContextClick;
	m_clickFunc[selected][SELECT_TYPE_GOOD]                [button][mode] = GoodContextClick;

	
	mode = SELECT_MODE_CTP2;
	button = SELECT_BUTTON_RIGHT_PATHING;
	selected = SELECT_TYPE_LOCAL_ARMY;

	m_clickFunc[selected][SELECT_TYPE_NONE]                [button][mode] = CancelPathingClick;
	m_clickFunc[selected][SELECT_TYPE_LOCAL_ARMY]          [button][mode] = CancelPathingClick;
	m_clickFunc[selected][SELECT_TYPE_LOCAL_ARMY_UNLOADING][button][mode] = ErrorClick;
	m_clickFunc[selected][SELECT_TYPE_LOCAL_CITY]          [button][mode] = CancelPathingClick;
	m_clickFunc[selected][SELECT_TYPE_REMOTE_CITY]         [button][mode] = CancelPathingClick;
	m_clickFunc[selected][SELECT_TYPE_REMOTE_ARMY]         [button][mode] = CancelPathingClick;
	m_clickFunc[selected][SELECT_TYPE_TRADE_ROUTE]         [button][mode] = CancelPathingClick;
	m_clickFunc[selected][SELECT_TYPE_GOOD]                [button][mode] = CancelPathingClick;

	
	mode = SELECT_MODE_CTP2;
	button = SELECT_BUTTON_RIGHT_PATHING;
	selected = SELECT_TYPE_LOCAL_CITY;

	m_clickFunc[selected][SELECT_TYPE_NONE]                [button][mode] = CancelPathingClick;
	m_clickFunc[selected][SELECT_TYPE_LOCAL_ARMY]          [button][mode] = CancelPathingClick;
	m_clickFunc[selected][SELECT_TYPE_LOCAL_ARMY_UNLOADING][button][mode] = ErrorClick;
	m_clickFunc[selected][SELECT_TYPE_LOCAL_CITY]          [button][mode] = CancelPathingClick;
	m_clickFunc[selected][SELECT_TYPE_REMOTE_CITY]         [button][mode] = CancelPathingClick;
	m_clickFunc[selected][SELECT_TYPE_REMOTE_ARMY]         [button][mode] = CancelPathingClick;
	m_clickFunc[selected][SELECT_TYPE_TRADE_ROUTE]         [button][mode] = CancelPathingClick;
	m_clickFunc[selected][SELECT_TYPE_GOOD]                [button][mode] = CancelPathingClick;

	
	mode = SELECT_MODE_CTP2;
	button = SELECT_BUTTON_RIGHT_PATHING;
	selected = SELECT_TYPE_GOOD;

	m_clickFunc[selected][SELECT_TYPE_NONE]                [button][mode] = CancelPathingClick;
	m_clickFunc[selected][SELECT_TYPE_LOCAL_ARMY]          [button][mode] = CancelPathingClick;
	m_clickFunc[selected][SELECT_TYPE_LOCAL_ARMY_UNLOADING][button][mode] = ErrorClick;
	m_clickFunc[selected][SELECT_TYPE_LOCAL_CITY]          [button][mode] = CancelPathingClick;
	m_clickFunc[selected][SELECT_TYPE_REMOTE_CITY]         [button][mode] = CancelPathingClick;
	m_clickFunc[selected][SELECT_TYPE_REMOTE_ARMY]         [button][mode] = CancelPathingClick;
	m_clickFunc[selected][SELECT_TYPE_TRADE_ROUTE]         [button][mode] = CancelPathingClick;
	m_clickFunc[selected][SELECT_TYPE_GOOD]                [button][mode] = CancelPathingClick;

	
	mode = SELECT_MODE_CTP2;
	button = SELECT_BUTTON_RIGHT_PATHING;
	selected = SELECT_TYPE_NONE;

	m_clickFunc[selected][SELECT_TYPE_NONE]                [button][mode] = CancelPathingClick;
	m_clickFunc[selected][SELECT_TYPE_LOCAL_ARMY]          [button][mode] = CancelPathingClick;
	m_clickFunc[selected][SELECT_TYPE_LOCAL_ARMY_UNLOADING][button][mode] = ErrorClick;
	m_clickFunc[selected][SELECT_TYPE_LOCAL_CITY]          [button][mode] = CancelPathingClick;
	m_clickFunc[selected][SELECT_TYPE_REMOTE_CITY]         [button][mode] = CancelPathingClick;
	m_clickFunc[selected][SELECT_TYPE_REMOTE_ARMY]         [button][mode] = CancelPathingClick;
	m_clickFunc[selected][SELECT_TYPE_TRADE_ROUTE]         [button][mode] = CancelPathingClick;
	m_clickFunc[selected][SELECT_TYPE_GOOD]                [button][mode] = CancelPathingClick;

	
	mode = SELECT_MODE_CTP2;
	button = SELECT_BUTTON_RIGHT;
	selected = SELECT_TYPE_LOCAL_CITY;

	m_clickFunc[selected][SELECT_TYPE_NONE]                [button][mode] = TerrainContextClick;
	m_clickFunc[selected][SELECT_TYPE_LOCAL_ARMY]          [button][mode] = ArmyContextClick;
	m_clickFunc[selected][SELECT_TYPE_LOCAL_ARMY_UNLOADING][button][mode] = ErrorClick;
	m_clickFunc[selected][SELECT_TYPE_LOCAL_CITY]          [button][mode] = CityContextClick;
	m_clickFunc[selected][SELECT_TYPE_REMOTE_CITY]         [button][mode] = EnemyCityContextClick;
	m_clickFunc[selected][SELECT_TYPE_REMOTE_ARMY]         [button][mode] = EnemyArmyContextClick;
	m_clickFunc[selected][SELECT_TYPE_TRADE_ROUTE]         [button][mode] = TradeRouteContextClick;
	m_clickFunc[selected][SELECT_TYPE_GOOD]                [button][mode] = GoodContextClick;

	
	mode = SELECT_MODE_CTP2;
	button = SELECT_BUTTON_RIGHT;
	selected = SELECT_TYPE_LOCAL_ARMY_UNLOADING;

	m_clickFunc[selected][SELECT_TYPE_NONE]                [button][mode] = UnloadClick;
	m_clickFunc[selected][SELECT_TYPE_LOCAL_ARMY]          [button][mode] = UnloadClick;
	m_clickFunc[selected][SELECT_TYPE_LOCAL_ARMY_UNLOADING][button][mode] = ErrorClick;
	m_clickFunc[selected][SELECT_TYPE_LOCAL_CITY]          [button][mode] = UnloadClick;
	m_clickFunc[selected][SELECT_TYPE_REMOTE_CITY]         [button][mode] = UnloadClick;
	m_clickFunc[selected][SELECT_TYPE_REMOTE_ARMY]         [button][mode] = UnloadClick;
	m_clickFunc[selected][SELECT_TYPE_TRADE_ROUTE]         [button][mode] = UnloadClick;
	m_clickFunc[selected][SELECT_TYPE_GOOD]                [button][mode] = UnloadClick;

	
	mode = SELECT_MODE_CTP2;
	button = SELECT_BUTTON_RIGHT;
	selected = SELECT_TYPE_REMOTE_CITY;

	m_clickFunc[selected][SELECT_TYPE_NONE]                [button][mode] = TerrainContextClick;
	m_clickFunc[selected][SELECT_TYPE_LOCAL_ARMY]          [button][mode] = ArmyContextClick;
	m_clickFunc[selected][SELECT_TYPE_LOCAL_ARMY_UNLOADING][button][mode] = ErrorClick;
	m_clickFunc[selected][SELECT_TYPE_LOCAL_CITY]          [button][mode] = CityContextClick;
	m_clickFunc[selected][SELECT_TYPE_REMOTE_CITY]         [button][mode] = EnemyCityContextClick;
	m_clickFunc[selected][SELECT_TYPE_REMOTE_ARMY]         [button][mode] = EnemyArmyContextClick;
	m_clickFunc[selected][SELECT_TYPE_TRADE_ROUTE]         [button][mode] = TradeRouteContextClick;
	m_clickFunc[selected][SELECT_TYPE_GOOD]                [button][mode] = GoodContextClick;

	
	mode = SELECT_MODE_CTP2;
	button = SELECT_BUTTON_RIGHT;
	selected = SELECT_TYPE_REMOTE_ARMY;

	m_clickFunc[selected][SELECT_TYPE_NONE]                [button][mode] = TerrainContextClick;
	m_clickFunc[selected][SELECT_TYPE_LOCAL_ARMY]          [button][mode] = ArmyContextClick;
	m_clickFunc[selected][SELECT_TYPE_LOCAL_ARMY_UNLOADING][button][mode] = ErrorClick;
	m_clickFunc[selected][SELECT_TYPE_LOCAL_CITY]          [button][mode] = CityContextClick;
	m_clickFunc[selected][SELECT_TYPE_REMOTE_CITY]         [button][mode] = EnemyCityContextClick;
	m_clickFunc[selected][SELECT_TYPE_REMOTE_ARMY]         [button][mode] = EnemyArmyContextClick;
	m_clickFunc[selected][SELECT_TYPE_TRADE_ROUTE]         [button][mode] = TradeRouteContextClick;
	m_clickFunc[selected][SELECT_TYPE_GOOD]                [button][mode] = GoodContextClick;

	
	mode = SELECT_MODE_CTP2;
	button = SELECT_BUTTON_RIGHT;
	selected = SELECT_TYPE_GOOD;

	m_clickFunc[selected][SELECT_TYPE_NONE]                [button][mode] = TerrainContextClick;
	m_clickFunc[selected][SELECT_TYPE_LOCAL_ARMY]          [button][mode] = ArmyContextClick;
	m_clickFunc[selected][SELECT_TYPE_LOCAL_ARMY_UNLOADING][button][mode] = ErrorClick;
	m_clickFunc[selected][SELECT_TYPE_LOCAL_CITY]          [button][mode] = SendGoodClick;
	m_clickFunc[selected][SELECT_TYPE_REMOTE_CITY]         [button][mode] = SendGoodClick;
	m_clickFunc[selected][SELECT_TYPE_REMOTE_ARMY]         [button][mode] = EnemyArmyContextClick;
	m_clickFunc[selected][SELECT_TYPE_TRADE_ROUTE]         [button][mode] = TradeRouteContextClick;
	m_clickFunc[selected][SELECT_TYPE_GOOD]                [button][mode] = GoodContextClick;

	
	mode = SELECT_MODE_CTP2;
	button = SELECT_BUTTON_RIGHT;
	selected = SELECT_TYPE_TRADE_ROUTE;

	m_clickFunc[selected][SELECT_TYPE_NONE]                [button][mode] = TerrainContextClick;
	m_clickFunc[selected][SELECT_TYPE_LOCAL_ARMY]          [button][mode] = ArmyContextClick;
	m_clickFunc[selected][SELECT_TYPE_LOCAL_ARMY_UNLOADING][button][mode] = ErrorClick;
	m_clickFunc[selected][SELECT_TYPE_LOCAL_CITY]          [button][mode] = CityContextClick;
	m_clickFunc[selected][SELECT_TYPE_REMOTE_CITY]         [button][mode] = EnemyCityContextClick;
	m_clickFunc[selected][SELECT_TYPE_REMOTE_ARMY]         [button][mode] = EnemyArmyContextClick;
	m_clickFunc[selected][SELECT_TYPE_TRADE_ROUTE]         [button][mode] = TradeRouteContextClick;
	m_clickFunc[selected][SELECT_TYPE_GOOD]                [button][mode] = GoodContextClick;
	


	
}

void SelectedItem::NewRegisterClick(const MapPoint &pos, const aui_MouseEvent *data, BOOL doubleClick,
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

	
	if(mode == SELECT_MODE_CTP2) {
		if(m_is_pathing || do_targeting_mode) {
			if(button == SELECT_BUTTON_LEFT) {
				button = SELECT_BUTTON_LEFT_PATHING;
			} else if(button == SELECT_BUTTON_RIGHT) {
				button = SELECT_BUTTON_RIGHT_PATHING;
			}
		}
	}

	
	
	if(m_select_state[vPlayer] == SELECT_TYPE_LOCAL_ARMY &&
	   pos == m_select_pos[vPlayer] &&
	   !leftDrop &&
	   clickedThing != SELECT_TYPE_LOCAL_ARMY) {
		clickedThing = SELECT_TYPE_LOCAL_ARMY;
	}

	
	if(doubleClick && button == SELECT_BUTTON_LEFT && pos == m_select_pos[vPlayer]) {
		m_justGotDoubleClick = true;
		clickedThing = m_select_state[vPlayer];
	}

	if(button == SELECT_BUTTON_LEFT_DROP && m_justGotDoubleClick) {
		m_justGotDoubleClick = false;
		return;
	}

	
	
	if(button == SELECT_BUTTON_LEFT && 
	   (m_select_state[vPlayer] == SELECT_TYPE_NONE || m_select_pos[vPlayer] != pos) &&
	   clickedThing == SELECT_TYPE_LOCAL_ARMY) {
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

void SelectedItem::ErrorClick(const MapPoint &pos, const aui_MouseEvent *data, BOOL doubleClick)
{
	
	
	
	Assert(FALSE);
}

void SelectedItem::NullClick(const MapPoint &pos, const aui_MouseEvent *data, BOOL doubleClick)
{
	
}

void SelectedItem::SelectArmyClick(const MapPoint &pos, const aui_MouseEvent *data, BOOL doubleClick)
{
	
	Unit top;
	GetTopUnit(pos, top);
	SetSelectUnit(top);

	if(doubleClick) {
		ArmyManagerWindow::Display();
	}
	
	
	g_gevManager->AddEvent(GEV_INSERT_Tail, GEV_ArmyClicked, GEA_Army, top.GetArmy(), GEA_End);



	




}



void SelectedItem::SelectArmyStartMoveClick(const MapPoint &pos, const aui_MouseEvent *data, BOOL doubleClick)
{
	if(!GetIsPathing()) {
		SelectArmyClick(pos, data, doubleClick);
		if(!doubleClick)
			StartMoveClick(pos, data, doubleClick);
	}
}

void SelectedItem::SelectCityClick(const MapPoint &pos, const aui_MouseEvent *data, BOOL doubleClick)
{
	
	if(!GetIsPathing()) {
		Unit top;
		GetTopUnitOrCity(pos, top);
		SetSelectCity(top);
		if(doubleClick) {
			if(top.IsCity())
				CityWindow::Display(top.CD());
			else
				ArmyManagerWindow::Display();
		}
	}

	
	Unit top;
	GetTopUnitOrCity(pos, top);
	g_gevManager->AddEvent(GEV_INSERT_Tail, GEV_CityClicked, GEA_City, top, GEA_End);

}

void SelectedItem::SelectGoodClick(const MapPoint &pos, const aui_MouseEvent *data, BOOL doubleClick)
{
	if(!GetIsPathing()) {
		SetSelectGood(pos);
	}
}

void SelectedItem::SelectEnemyArmyClick(const MapPoint &pos, const aui_MouseEvent *data, BOOL doubleClick)
{
	
	Army a;
	GetSelectedArmy(a);
	g_gevManager->AddEvent(GEV_INSERT_Tail, GEV_ArmyClicked, GEA_Army, a, GEA_End);
}

void SelectedItem::SelectEnemyCityClick(const MapPoint &pos, const aui_MouseEvent *data, BOOL doubleClick)
{
	
	Unit top;
	GetTopUnitOrCity(pos, top);
	g_gevManager->AddEvent(GEV_INSERT_Tail, GEV_CityClicked, GEA_City, top, GEA_End);
}

void SelectedItem::SelectTradeRouteClick(const MapPoint &pos, const aui_MouseEvent *data, BOOL doubleClick)
{
	
}

void SelectedItem::DeselectClick(const MapPoint &pos, const aui_MouseEvent *data, BOOL doubleClick)
{
	m_select_pos[GetVisiblePlayer()] = pos;

	Deselect(GetVisiblePlayer());
}

void SelectedItem::UnloadClick(const MapPoint &pos, const aui_MouseEvent *data, BOOL doubleClick)
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
// Parameters : pos			: position on the map
//				data		: click properties (right, left, etc.)
//              doubleClick	: is it a double click?
//
// Returns    : -
//
// Remark(s)  : When the tile is unexplored, nothing will be shown.
//
//----------------------------------------------------------------------------

void SelectedItem::TerrainContextClick(const MapPoint &pos, const aui_MouseEvent *data, BOOL doubleClick)
{
	if(g_player[GetVisiblePlayer()] && g_player[GetVisiblePlayer()]->IsExplored(pos)) {
		helptile_displayData(pos);
	}
}

//----------------------------------------------------------------------------
//
// Name       : SelectedItem::ArmyContextClick
//
// Description: Handle mouse click on an army of our own.
//
// Parameters : pos			: position on the map
//				data		: click properties (right, left, etc.)
//              doubleClick	: is it a double click?
//
// Returns    : -
//
// Remark(s)  : -
//
//----------------------------------------------------------------------------

void SelectedItem::ArmyContextClick(const MapPoint &pos, const aui_MouseEvent *data, BOOL doubleClick)
{
	SelectArmyClick(pos, data, FALSE);

	g_controlPanel->ActivateSelectedInfo(SELECT_TYPE_LOCAL_ARMY);
}

//----------------------------------------------------------------------------
//
// Name       : SelectedItem::CityContextClick
//
// Description: Handle mouse click on a city of our own.
//
// Parameters : pos			: position on the map
//				data		: click properties (right, left, etc.)
//              doubleClick	: is it a double click?
//
// Returns    : -
//
// Remark(s)  : -
//
//----------------------------------------------------------------------------

void SelectedItem::CityContextClick(const MapPoint &pos, const aui_MouseEvent *data, BOOL doubleClick)
{
	SelectCityClick(pos, data, FALSE);

	g_controlPanel->ActivateSelectedInfo(SELECT_TYPE_LOCAL_CITY);
}

//----------------------------------------------------------------------------
//
// Name       : SelectedItem::EnemyCityContextClick
//
// Description: Handle mouse click on an enemy city.
//
// Parameters : pos			: position on the map
//				data		: click properties (right, left, etc.)
//              doubleClick	: is it a double click?
//
// Returns    : -
//
// Remark(s)  : -
//
//----------------------------------------------------------------------------

void SelectedItem::EnemyCityContextClick(const MapPoint &pos, const aui_MouseEvent *data, BOOL doubleClick)
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
// Parameters : pos			: position on the map
//				data		: click properties (right, left, etc.)
//              doubleClick	: is it a double click?
//
// Returns    : -
//
// Remark(s)  : -
//
//----------------------------------------------------------------------------

void SelectedItem::EnemyArmyContextClick(const MapPoint &pos, const aui_MouseEvent *data, BOOL doubleClick)
{
	// Report the underlying terrain information. For visible units, this is 
	// just being useful. For invisible units, it will fix the bug that 
	// detection is too easy.
	TerrainContextClick(pos, data, doubleClick);
}

void SelectedItem::TradeRouteContextClick(const MapPoint &pos, const aui_MouseEvent *data, BOOL doubleClick)
{
}

void SelectedItem::GoodContextClick(const MapPoint &pos, const aui_MouseEvent *data, BOOL doubleClick)
{
}


void SelectedItem::SendGoodClick(const MapPoint &pos, const aui_MouseEvent *data, BOOL doubleClick)
{
	sint32 player = GetVisiblePlayer();

	

	BOOL isForeignGood = FALSE;
	
	
	Unit homeCity = g_theWorld->GetCell(m_select_pos[player])->GetCityOwner();
	if(homeCity.m_id != 0) {
		if (homeCity.GetOwner() != player) {
			isForeignGood = TRUE;
		}
	} else {
		
		return;
	}
	
	
	if (!homeCity.IsValid()) return;
	
	
	Unit destCity;
	if (!GetTopUnitOrCity(pos, destCity)) return;
	if (!destCity.IsCity()) return;
	
	
	sint32		resIndex;
	if (!g_theWorld->GetGood(m_select_pos[player], resIndex)) return;
	
	if(!isForeignGood) {
		g_gevManager->AddEvent(GEV_INSERT_Tail, GEV_SendGood,
							   GEA_Int, resIndex,
							   GEA_City, homeCity,
							   GEA_City, destCity,
							   GEA_End);
		
		
		
		
	}
#if 0
	else {
		g_gevManager->AddEvent(GEV_INSERT_Tail, GEV_TradeBid,
							   GEA_Player, player,
							   GEA_Int, resIndex,
							   GEA_City, homeCity,
							   GEA_City, destCity,
							   GEA_End);
	}
#endif
}

void SelectedItem::MoveArmyClick(const MapPoint &pos, const aui_MouseEvent *data, BOOL doubleClick)
{
	MapPoint army_pos;
	sint32 player = GetVisiblePlayer();

	
	m_selected_army[player].GetPos(army_pos);
	
	if(army_pos == pos && 
	   GetClickedThing(pos, true) == SELECT_TYPE_LOCAL_ARMY) {

	} else {
		Unit *unit = &((m_selected_army[player])[0]);
		if (m_good_path &&
			(unit->GetDBRec()->GetCanCarry()) &&
			(unit->GetNumCarried() > 0)) {
			
								
								
			if ((unit->GetDBRec()->GetCargoPod()) &&
				(g_theWorld->IsWater(pos)) &&
				(unit->GetData()->CargoHasLandUnits())) {
				QuickSlic("15IALandInOcean", player, TRUE);
				return;
			}
			
								
								
			if ((unit->GetMovementTypeSea() || unit->GetMovementTypeShallowWater()) &&
				(g_theWorld->IsLand(pos))) {
				SlicObject *so = new SlicObject("14IAAutoUnload");
				so->AddLocation(pos);
				so->AddCivilisation(player);
				so->AddRecipient(player);
				g_slicEngine->Execute(so);
				return;
			}
		}
		EnterArmyMove(player, pos);
	}
}

void SelectedItem::StartMoveClick(const MapPoint &pos, const aui_MouseEvent *data, BOOL doubleClick)
{
	DPRINTF(k_DBG_INFO, ("Start Move: %d,%d\n", pos.x, pos.y));
	m_startDragPos = pos;
	m_gotClick = true;
	m_clickStartTime = GetTickCount();
	
	
	
}

void SelectedItem::MoveDrag(const MapPoint &pos, const aui_MouseEvent *data, BOOL doubleClick)
{
	DPRINTF(k_DBG_INFO, ("Move Drag: %d,%d\n", pos.x, pos.y));
	if(!m_gotClick) {
		
		return;
	}

	if(pos != m_startDragPos) {
		m_isDragging = true;
		g_controlPanel->BeginOrderDelivery(m_moveOrder);
		
		
	}
}

void SelectedItem::MoveDrop(const MapPoint &pos, const aui_MouseEvent *data, BOOL doubleClick)
{
	DPRINTF(k_DBG_INFO, ("Move Drop: %d,%d\n", pos.x, pos.y));
	if(!m_gotClick) {
		
		
		return;
	}
	m_gotClick = false;

	if(pos != m_startDragPos) {
		m_isDragging = true;
		g_controlPanel->BeginOrderDelivery(m_moveOrder);
	}

	if(m_isDragging) {
		m_justNowSelectedArmy = false;
		if(GetIsPathing()) {
			m_isDragging = false;
			ActionClick(pos, data, doubleClick);
			return;
		}

		if(pos == m_select_pos[GetVisiblePlayer()]) {
			CancelPathingClick(pos, data, doubleClick);
			return;
		}
	} else {
		if(!m_justNowSelectedArmy) {
			switch(GetClickedThing(pos, false)) {
				case SELECT_TYPE_LOCAL_ARMY:
					break;
				case SELECT_TYPE_GOOD:
					SelectGoodClick(pos, data, doubleClick);
					break;
				case SELECT_TYPE_LOCAL_CITY:
					SelectCityClick(pos, data, doubleClick);
					break;
			}
		} else {
			m_justNowSelectedArmy = false;
		}
	}




		







}

void SelectedItem::ActionClick(const MapPoint &pos, const aui_MouseEvent *data, BOOL doubleClick)
{	
	m_startDragPos = pos;

	if(g_controlPanel->ExecuteTargetingModeClick(pos)) {
		m_is_pathing = FALSE;
		return;
	}

	if(!m_isDragging && !GetIsPathing()) {
		
		
		if(m_select_pos[GetVisiblePlayer()] != pos) {
			
			DeselectClick(pos, data, doubleClick);
		}
		return;
	}

	if(GetIsPathing()) {
		MapPoint army_pos;
		sint32 player = GetVisiblePlayer();
		
		g_controlPanel->ClearTargetingMode();

		
		m_selected_army[player].GetPos(army_pos);
		
		m_is_pathing = FALSE;

		if(army_pos != pos) {
			Unit *unit = &((m_selected_army[player])[0]);
			if (m_good_path &&
				(unit->GetDBRec()->GetCanCarry()) &&
				(unit->GetNumCarried() > 0)) {

				
				
				if ((unit->GetMovementTypeSea() || unit->GetMovementTypeShallowWater()) &&
					(g_theWorld->IsLand(pos))) {
					SlicObject *so = new SlicObject("14IAAutoUnload");
					so->AddLocation(pos);
					so->AddCivilisation(player);
					so->AddRecipient(player);
					g_slicEngine->Execute(so);
					return;
				}
			}

			EnterArmyMove(player, pos);
		}
	}
}

void SelectedItem::CancelPathingClick(const MapPoint &pos, const aui_MouseEvent *data, BOOL doubleClick)
{
	m_is_pathing = FALSE;
	m_isDragging = false;
	g_controlPanel->ClearTargetingMode();
}




SELECT_TYPE SelectedItem::GetClickedThing(const MapPoint &pos, bool click)
{
	Unit		top;
	sint32		visiblePlayer = GetVisiblePlayer();
	BOOL hasUnitOrCity = GetTopUnitOrCity(pos, top);
	Cell *cell = g_theWorld->GetCell(pos);

	if(m_select_state[visiblePlayer] != SELECT_TYPE_NONE &&
	   m_select_pos[visiblePlayer] == pos) {
		if(click)
			return m_select_state[visiblePlayer];

		
		
		switch(m_select_state[visiblePlayer]) {
			case SELECT_TYPE_LOCAL_CITY:
				
				if(cell->GetNumUnits() > 0) {
					if(GetTopUnit(pos, top)) {
						if(top.GetOwner() == visiblePlayer) {
							return SELECT_TYPE_LOCAL_ARMY;
						} else {
							return SELECT_TYPE_REMOTE_ARMY;
						}
					}
				}
				
				
				break;
			case SELECT_TYPE_LOCAL_ARMY:
				if(hasUnitOrCity && top.IsCity()) {
					if(top.GetOwner() == visiblePlayer) {						
						return SELECT_TYPE_LOCAL_CITY;
					} else {
						return SELECT_TYPE_REMOTE_CITY;
					}
				
				
				} else if(hasUnitOrCity) {
					if(top.GetOwner() == visiblePlayer)
						return SELECT_TYPE_LOCAL_ARMY;
					else
						return SELECT_TYPE_REMOTE_ARMY;
				}
				return SELECT_TYPE_NONE;
				break;
			case SELECT_TYPE_GOOD:
				if(hasUnitOrCity) {
					if(top.IsCity()) {
						if(top.GetOwner() == visiblePlayer) {
							return SELECT_TYPE_LOCAL_CITY;
						} else {
							return SELECT_TYPE_REMOTE_CITY;
						}
					} else {
						if(top.GetOwner() == visiblePlayer) {
							return SELECT_TYPE_LOCAL_ARMY;
						} else {
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

	
	if (GetTopUnitOrCity(pos, top)) {
		
		if (top.IsCity()) {
			if (top.GetOwner() == visiblePlayer) {
				if(g_theWorld->GetCell(pos)->GetNumUnits() > 0) {
					sint32 i;
					for(i = 0; i < g_theWorld->GetCell(pos)->GetNumUnits(); i++) {
						if(!g_theWorld->GetCell(pos)->AccessUnit(i).IsEntrenched()) {
							return SELECT_TYPE_LOCAL_ARMY;
						}
					}
				}
				return SELECT_TYPE_LOCAL_CITY;
			} else {
				return SELECT_TYPE_REMOTE_CITY;
			}
		} else {
			if (top.GetOwner() == visiblePlayer) {
				
				return SELECT_TYPE_LOCAL_ARMY;
			} else {
				return SELECT_TYPE_REMOTE_ARMY;
			}
		}
	} else {
		
		
		
	}

	
	return SELECT_TYPE_NONE;
}

