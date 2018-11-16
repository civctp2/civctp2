//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ header
// Description  : Control panel window
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
// HAVE_PRAGMA_ONCE
// - Compiles with #pragma once
//
//----------------------------------------------------------------------------
//
// Modifications from the original Activision code:
//
// - Cleaned up structure
// - Removed unused methods: FillBank, ClearButtons and AddButton.
//   (Aug 16th 2005 Martin Gühmann)
//
//----------------------------------------------------------------------------

#ifdef HAVE_PRAGMA_ONCE
#pragma once
#endif

#ifndef __CONTROLPANELWINDOW_H__
#define __CONTROLPANELWINDOW_H__

//----------------------------------------------------------------------------
// Library dependencies
//----------------------------------------------------------------------------

// #include <>

//----------------------------------------------------------------------------
// Export overview
//----------------------------------------------------------------------------

#define k_CONTROLPANEL_CITYUNIT_MODE        1
#define k_CONTROLPANEL_UNSELECT_MODE        2
#define k_CONTROLPANEL_LANDTILE_MODE        3
#define k_CONTROLPANEL_SEATILE_MODE         4
#define k_CONTROLPANEL_SPACETILE_MODE       5
#define k_CONTROLPANEL_TERRATILE_MODE       6
#define k_CONTROLPANEL_TILEIMP_MODE         7
#define k_CONTROLPANEL_CIVTAB_MODE          8

class ControlPanelWindow;

#include "SelItem.h"

class aui_TabGroup;
class BattleOrderBox;
class CellUnitList;
class Army;
class CityInventoryListBox;
class ButtonBank;
class TextTab;
class c3_Static;
class ctp2_Static;
class c3_ColoredSwitch;
class Thermometer;
class StaticTextBox;
class aui_Tab;
class MapPoint;
struct aui_MouseData;

class ControlSheet;
class aui_ProgressBar;
class aui_Static;
class ctp2_Window;
class ctp2_ListBox;
class ctp2_ListItem;
class ctp2_DropDown;
class ctp2_Button;
class ctp2_Menu;
class ctp2_MenuBar;
class aui_Control;
class OrderRecord;
class ArmyData;
class TerrainImprovementRecord;
class TerrainRecord;
class Message;
class ctp2_TabGroup;
class ctp2_Tab;
class ctp2_Switch;

enum CP_SELECT
{
	CP_SELECT_GOOD,
	CP_SELECT_UNIT,
	CP_SELECT_CITY,

	CP_SELECT_MAX
};

enum CP_TAB
{
	CP_TAB_INVALID = -1,
	CP_TAB_CIV,
	CP_TAB_MSGLOG,
	CP_TAB_CITY,
	CP_TAB_UNIT,
	CP_TAB_TILEIMP,
	CP_TAB_LAST
};

enum CP_TARGETING_MODE
{
	CP_TARGETING_MODE_OFF=0,
	CP_TARGETING_MODE_ORDER_PENDING,
	CP_TARGETING_MODE_TILEIMP_PENDING,
	CP_TARGETING_MODE_TERRAFORM_PENDING,
	CP_TARGETING_MODE_MAX,
};

enum CP_OFFSETS
{
	CP_NORTH,
	CP_SOUTH,
	CP_EAST,
	CP_WEST,
	CP_MAX
};

enum CP_MENU_ITEM
{
	CP_MENU_ITEM_0,
	CP_MENU_ITEM_1,
	CP_MENU_ITEM_2,
	CP_MENU_ITEM_3,
	CP_MENU_ITEM_4,
	CP_MENU_ITEM_5,
	CP_MENU_ITEM_6,
	CP_MENU_ITEM_7,
	CP_MENU_ITEM_8,
	CP_MENU_ITEM_9,
	CP_MENU_ITEM_10,
	CP_MENU_ITEM_11,
	CP_MENU_ITEM_12,
	CP_MENU_ITEM_13,
	CP_MENU_ITEM_14,
	CP_MENU_ITEM_15
};




#define CP_TILEIMP_ROWS		3
#define CP_TILEIMP_COLS		4
#define CP_MAX_BUTTONS_PER_BANK	(CP_TILEIMP_ROWS*CP_TILEIMP_COLS)

enum
{
	CP_TILEIMP_LAND=0,
	CP_TILEIMP_OCEAN,
	CP_TILEIMP_SPECIAL,
	CP_TERRAFORM_LAND,

	CP_TILEIMP_MAX,
};

#define CP_MAX_TILEIMPBUTTONS	(CP_TILEIMP_MAX*CP_MAX_BUTTONS_PER_BANK)

//----------------------------------------------------------------------------
// Project dependencies
//----------------------------------------------------------------------------

#include "player.h"             // PLAYER_INDEX
#include "ctp2_inttypes.h"      // sint32
#include "c3window.h"
#include "c3_button.h"
#include "aui_stringtable.h"
#include "Unit.h"

#include "SelItem.h"

class Unit;
class aui_TabGroup;
class BattleOrderBox;
class CellUnitList;
class Army;
class CityInventoryListBox;
class ButtonBank;
class TextTab;
class c3_Static;
class ctp2_Static;
class c3_ColoredSwitch;
class Thermometer;
class StaticTextBox;
class aui_Tab;
class MapPoint;
struct aui_MouseData;

class ControlSheet;
class aui_ProgressBar;
class aui_Static;
class ctp2_Window;
class ctp2_ListBox;
class ctp2_ListItem;
class ctp2_DropDown;
class ctp2_Button;
class ctp2_Menu;
class ctp2_MenuBar;
class aui_Control;
class OrderRecord;
class ArmyData;
class TerrainImprovementRecord;
class TerrainRecord;
class Message;
class ctp2_TabGroup;
class ctp2_Tab;
class ctp2_Switch;

//----------------------------------------------------------------------------
// Class declarations
//----------------------------------------------------------------------------

class ControlPanelWindow
{
public:
	ControlPanelWindow(
		AUI_ERRCODE *retval,
		uint32 id,
		MBCHAR *ldlBlock,
		sint32 bpp,
		AUI_WINDOW_TYPE type = AUI_WINDOW_TYPE_BACKGROUND );

	virtual ~ControlPanelWindow();

	ctp2_Window     *GetWindow()  { return m_mainWindow;  }
	ctp2_MenuBar    *GetMenuBar() { return m_mainMenuBar; }




	sint32  X       ();
	sint32  Y       ();
	sint32  Width   ();
	sint32  Height  ();

	void    Idle();
	void    Move(sint32 x, sint32 y);
	void    Offset(sint32 dx, sint32 dy);
	void    Resize(sint32 width, sint32 height);

	void    AdjustToScreen();

	void    ActivateSelectedInfo(sint32 type);
	void    DeActivateSelectedInfo();

	void    BuildList(sint32 index);

	void	SetStack(const Army &selectedArmy, CellUnitList *fullArmy, Unit singleUnit = Unit(0));
	void    BuildUnitList();
	void    BuildUnitListBox();
	void    BuildCityList(const MapPoint &pos);
	void    AppendItem(ctp2_ListBox *,MBCHAR *string);

	void    Select  ();
	void    Deselect();

	void    HappinessRedisplay(aui_Surface *surface,RECT &rect,void *cookie);

	void    BuildMainMenu     ();
	void    BuildCivMenu      ();
	void    BuildCityMenu     ();
	void    BuildUnitMenu     ();
	void    BuildDipMenu      ();
	void    BuildSciMenu      ();
	void    BuildTradeMenu    ();
	void    BuildGLMenu	      ();
	void    BuildStatsMenu    ();
	void    BuildOptionsMenu  ();
	void    BuildEspionageMenu();

	static void RebuildMenus  ();

	void    ResetTabGroup();
	void    CreateTabGroup(MBCHAR *ldlBlock);
	void    CreateTab(sint32 which,MBCHAR *name);

	void    CreateTileImpBanks ();
	void    ActivateTileImpBank(unsigned int group_id);
	void    ToggleTerraforming();






	void    InitCivTab();

	static void TabCallback(aui_Control *control, uint32 action,
	                        uint32 data, void *cookie);

	void    InitMessageTab();
	void    AddMessage(Message &message,bool initializing=false);
	void    SetMessageRead(const Message &message);
	void    RemoveMessage(Message &message);
	void    PopulateMessageList(PLAYER_INDEX player);

	void    InitCityTab();

	void    InitUnitTab();

	void    InitTileImpTab();

	void    BeginImprovementCycle(void *rawrecord);
	void    BeginImprovementCycle(TerrainImprovementRecord *rec);
	void    BeginImprovementCycle(TerrainRecord *rec);


	Unit    CityPanelGetCurrent();
	void    CityPanelRebuild();
	void    CityPanelRedisplay();
	void    CityPanelNextCity();

	Army    UnitPanelGetCurrent();
	void    UnitPanelNextUnit();
	void    UnitPanelRedisplay();
	void    UnitPanelNextCity();

	void    TileImpPanelRedisplay();
	void    TileImpButtonRedisplay      (uint32 player_id,uint32 index);
	void    TerraformButtonRedisplay    (uint32 player_id,uint32 index);

	void    BeginOrderDelivery();
	static void PerformOrderAfterConfirmation(bool response, void *userData);

	void    BeginOrderDelivery(OrderRecord *rec);

	void    TargetingMode();

	void    OrderDeliveryUpdate();
	void    TileImpUpdate();
	void    TerraFormUpdate();
	void    Update();

	bool    IsLand(const TerrainRecord *rec);
	bool    IsOcean(const TerrainRecord *rec);

	bool    ExecuteTargetingModeClick(const MapPoint &pos);

	bool    OrderDeliveryClick(const MapPoint &pos);
	bool    TileImpClick(const MapPoint &pos);
	bool    TerraFormClick(const MapPoint &pos);

	uint32  GetTargetingMode() const {return m_targetingMode;}

	const OrderRecord *GetCurrentOrder() const { return m_currentOrder; }

	void    ClearTargetingMode();

	void    SetTab(CP_TAB tab);

	void    SetProgressText(aui_Static *progressText) { m_progressText = progressText; }
	void    SetProgressBar(aui_ProgressBar *progressBar) { m_progressBar = progressBar; }

	AUI_ERRCODE UpdatePlayerBeginProgress(sint32 currentPlayer);

	AUI_ERRCODE UpdatePlayerEndProgress(sint32 currentPlayer);

	void    Hide();
	void    Show();
	void    Toggle();

private:

	void    PollCIVStatus();
	void    PollMSGLOGStatus();
	void    PollCITYStatus();
	void    PollUNITStatus();
	void    PollTILEIMPStatus();

	BOOL    CursorOverControlPanel();

	void    SetControlText(aui_Control *control,MBCHAR *fmt,...);

	ctp2_MenuBar    *m_mainMenuBar;

	ctp2_Window     *m_mainWindow;

	ctp2_Menu       *m_contextMenu;

	float   m_widthRatio;

	bool    m_turnToggle;

	uint32  m_targetingMode;

	OrderRecord     *m_currentOrder;






	ctp2_ListBox    *m_messageList;


	ctp2_Static     *m_numCities;
	ctp2_Static     *m_totalPop;
	ctp2_Static     *m_currentHappiness;
	ctp2_Static     *m_currentAdvance;
	ctp2_Static     *m_turnsToAdvance;

	ctp2_DropDown   *m_mainDropDown;

	ctp2_Static     *m_cityHappiness;
	ctp2_Static     *m_cityPopulation;
	ctp2_Static     *m_cityGrowth;
	ctp2_Static     *m_buildingItem;
	ctp2_Static     *m_turnsRemaining;

	ctp2_Static     *m_unitImage;

	ctp2_Static     *m_unitType;
	ctp2_Static     *m_unitMove;
	ctp2_Static     *m_unitAttack;
	ctp2_Static     *m_unitDefense;
	ctp2_Static     *m_unitStatus;

	ctp2_ListBox    *m_unitOrders;

	ctp2_Static     *m_publicWorks;
	ctp2_Button     *m_activatorButtons [CP_TILEIMP_MAX];
	ctp2_Static     *m_tileImpPanes     [CP_TILEIMP_MAX];
	ctp2_Button     *m_tileImpButtons   [CP_MAX_TILEIMPBUTTONS];
	ctp2_Button     *m_terraFormButtons [CP_MAX_TILEIMPBUTTONS];

	ctp2_TabGroup   *m_tabGroup;
	ctp2_Tab        *m_tabs[CP_TAB_LAST];

	unsigned        m_currentTerrainSelection;
	TerrainImprovementRecord    *m_currentTerrainImpRec;
	TerrainRecord               *m_currentTerrainRec;

	bool            m_terraFormMode;


	sint32              m_currentProgress;
	aui_Static          *m_progressText;
	aui_ProgressBar     *m_progressBar;
};

void ThrowPartyUtilityDialogBoxCallback(MBCHAR *text, sint32 val2, void *data);
void GotoCityUtilityDialogBoxCallback  (Unit city, sint32 val2);

void controlpanelwindow_Update(Unit *city = NULL);

sint32  controlpanelwindow_Initialize();
sint32  controlpanelwindow_InitializeHats(void);
void    controlpanelwindow_Cleanup(void);

extern ControlPanelWindow *g_controlPanel;

#endif
