//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : Scenario editor
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
// - Make the number of city styles you can place with the scenario editor mod
//   dependent, by Martin Gühmann.
// - Make sure that newly created cities have the size as displayed in the 
//   CityPopSpinner, by Martin Gühmann.
// - Corrected wrap handling, by Fromafar.
// - Fixed Auto-Turn-Off-Pollution-Bug, by Martin Gühmann.
// - Memory leaks fixed, by Martin Gühmann and Fromafar.
// - Fixed switch to player 1 bug when the scenario editor is loaded for the
//   first time in a game session, by Martin Gühmann.
// - Added GetLastPlayer() to get the last player in the game, by Martin Gühmann.
// - Fixed player spinners in the scenario editor so that the last player
//   is still accessable even if players before in the row were killed,
//   by Martin Gühmann. 
//   Unfortunatly it looks like here are more problems. Soon after some turns
//   with the dead player I got Asserts when I try to access the dead player.
// - Fix of a crash by Martin Gühmann. If you selected a city changed the 
//   player, the city was destroyed by in game events, conquest, starvation
//   slic and you switch back via the Scenario Editor to that player the game
//   crashed, the problem is solved by deselecting everything before player
//   changing.
// - Added icons and tooltips to city style buttons, by Martin Gühmann.
// - Repaired backwards compatibility and possible crashes.
// - Replaced old civilisation database by new one. (Aug 21st 2005 Martin Gühmann)
// - Replaced old risk database by new one. (Aug 29th 2005 Martin Gühmann)
// - Initialized local variables. (Sep 9th 2005 Martin Gühmann)
// - Added a civ city style choser on the civ tab. (Jan 4th 2005 Martin Gühmann)
// - Spinner callbacks are added at the end so that they aren't called due to
//   min or max modifications. This accelerates Scenario Editor initalisation. (Feb 4th 2007 Martin Gühmann)
// - Switching between players now updates the city list of the main control 
//   panel city tab. (Feb 4th 2007 Martin Gühmann)
// - TODO add show Enemy Health and Debug AI buttons to Unit Tab
//
//----------------------------------------------------------------------------

#include "c3.h"
#include "c3ui.h"
#include "scenarioeditor.h"
#include "aui_ldl.h"
#include "ctp2_Window.h"
#include "controlpanelwindow.h"

#include "TerrainRecord.h"
#include "IconRecord.h"
#include "UnitRecord.h"

#include "ctp2_listbox.h"
#include "ctp2_listitem.h"
#include "aui_imagelist.h"
#include "ctp2_button.h"
#include "ctp2_Static.h"
#include "ctp2_Switch.h"
#include "ctp2_spinner.h"
#include "aui_switch.h" //emod

#include "StrDB.h"

#include "Unit.h"
#include "citydata.h"
#include "UnitData.h"

//Added by Martin Gühmann to have the appropiate number 
//on the city style tab 
#include "CityStyleRecord.h"
#include "DifficultyRecord.h"   // g_theDifficultyDB

#include "BuildingRecord.h"
#include "WonderRecord.h"
#include "AdvanceRecord.h"
#include "GovernmentRecord.h"

#include "SelItem.h"

#include "WonderTracker.h"
#include "wonderutil.h"
#include "player.h"
#include "Advances.h"

#include "director.h"
#include "tiledmap.h"
#include "radarmap.h"
#include "TurnCnt.h"

#include "Civilisation.h"
#include "aui_tipwindow.h"

#include "loadsavewindow.h"
#include "GameSettings.h"

#include "World.h"

#include "c3_utilitydialogbox.h"

#include "scenariowindow.h"

#include "profileDB.h"
#include "CivilisationRecord.h"

#include "MapCopyBuffer.h"

#include "FileDialog.h"
#include "MapFile.h"
#include "CivPaths.h"
#include "ctp2_dropdown.h"

#include "RiskRecord.h"

#include "Cell.h"
#include "aicause.h"
#include "TerrImprove.h"
#include "ConstDB.h"
#include "civapp.h"

#include "ctp2_textfield.h"

#include "terrainutil.h"
#include "cellunitlist.h"

#include "GameOver.h"

#include "Exclusions.h"
#include "SlicEngine.h"
#include "MessageBoxDialog.h"

#include "MainControlPanel.h"
#include "newturncount.h"
#include "TurnYearStatus.h"
#include "MaterialPool.h"
#include "gold.h"
#include "cursormanager.h"
#include "network.h"
#include "AttractWindow.h"
#include "gfx_options.h"

extern C3UI *               g_c3ui;
extern sint32               g_fog_toggle;
extern RadarMap *           g_radarMap;
extern ControlPanelWindow * g_controlPanel;
extern sint32               g_startInfoType;
extern CivApp *             g_civApp;
extern MBCHAR               g_slic_filename[_MAX_PATH];

extern void WhackScreen(); 

static MBCHAR *s_scenarioEditorBlock = "ScenarioEditor";
static ScenarioEditor *s_scenarioEditor = NULL;
static MBCHAR *s_scenarioAddStuffBlock = "ScenAddStuffWindow";

#define k_TERRAIN_COLS_PER_ROW 6
#define k_CITY_COLS_PER_ROW 6
#define k_UNIT_COLS_PER_ROW 5
#define k_TERRAINIMP_COLS_PER_ROW 6


#define k_SCEN_FILE_LOAD_MAP 1
#define k_SCEN_FILE_SAVE_MAP 2
#define k_SCEN_FILE_LOAD_CLIP 3
#define k_SCEN_FILE_SAVE_CLIP 4

#define k_MAX_ADD_GOLD_OR_PW 1000000
#define MAX_CHARS 7

static MBCHAR *k_WORLD_TAB_BUTTON = "ScenarioEditor.TabGroup.WorldButton";
static MBCHAR *k_UNIT_TAB_BUTTON = "ScenarioEditor.TabGroup.UnitButton";
static MBCHAR *k_CITY_TAB_BUTTON = "ScenarioEditor.TabGroup.CityButton";
static MBCHAR *k_CIV_TAB_BUTTON = "ScenarioEditor.TabGroup.CivButton";


char *s_scenTabNames[SCEN_TAB_MAX] = {
	"World",
	"Unit",
	"City",
	"Civ",
};

static char *s_playerSpinners[] = {
	"UnitControls.Player",
	"CityControls.Player",
	"CivControls.Player"
};
#define k_NUM_PLAYER_SPINNERS (sizeof(s_playerSpinners) / sizeof(char *))

BOOL s_wasKeepingScore = FALSE;


BOOL g_toeMode = FALSE;


static char *s_modeSwitchNames[SCEN_START_LOC_MODE_MAX] = {
	"TabGroup.Civ.FullModeSwitch",
	"TabGroup.Civ.PlayerNoCivSwitch",
	"TabGroup.Civ.PlayerWithCivSwitch",
	"TabGroup.Civ.JustCivSwitch"
};

//emod
//static aui_Switch		*s_ShowEnemyHealth			= NULL,
//						*s_NULL				= NULL;


sint32 g_isCheatModeOn = FALSE;

void scenarioeditor_SetSaveOptionsFromMode(void)
{
	switch (s_scenarioEditor->GetStartLocMode()) {
	case SCEN_START_LOC_MODE_NONE:
		g_startInfoType = STARTINFOTYPE_NOLOCS;
		
		break;
	case SCEN_START_LOC_MODE_PLAYER_WITH_CIV:
		g_startInfoType = STARTINFOTYPE_CIVSFIXED;
		break;
	case SCEN_START_LOC_MODE_PLAYER:
		g_startInfoType = STARTINFOTYPE_POSITIONSFIXED;
		break;
	case SCEN_START_LOC_MODE_CIV:
		g_startInfoType = STARTINFOTYPE_CIVS;

		break;
	}
}

ScenarioEditor::ScenarioEditor(AUI_ERRCODE *err)  //called by intialize does same as initialize in other files
:



	m_window                    (NULL),
	m_terrainSwitches           (NULL),
	m_terrainImpSwitches        (NULL),
	m_addStuffWindow            (NULL),
//	ctp2_Switch *m_otherMapSwitch[k_NUM_OTHER_MAP_SWITCHES];
	m_eraseButton               (NULL),
//	ctp2_Button *m_tabButton[k_NUM_TAB_BUTTONS];
	m_xWrapButton               (NULL),     // unused
	m_xWrap                     (true),
	m_yWrap                     (false),
	m_yWrapButton               (NULL),     // unused
	m_addMode                   (SCEN_ADD_BUILDINGS),
    m_paintTerrain              (-1),
    m_paintTerrainImprovement   (-1),
    m_brushSize                 (1),
    m_unitIndex                 (-1),
    m_cityStyle                 (CITY_STYLE_EDITOR), 	//displayed in the CityPopSpinner to new created cities.
 	m_newPopSize                (1), 	//Added by Martin Gühmann to add the pop number
    m_startLocMode              (SCEN_START_LOC_MODE_NONE),
    m_haveRegion                (false),
    m_mapMode                   (SCEN_MAP_NONE),
    m_regionStart               (),
    m_regionWidth               (0),
    m_regionHeight              (0),
	m_copyBuffer                (NULL),
	m_fileDialog                (NULL),
	m_initializing              (true),
	m_placeNationFlag           (0),
	m_isGivingAdvances          (false)
	// MBCHAR m_scenarioName[k_SCENARIO_NAME_MAX];
{
	m_scenarioName[0] = 0;
    if (g_theWorld)
    {
	    m_xWrap = g_theWorld->IsXwrap();
	    m_yWrap = g_theWorld->IsYwrap();
    }

	m_window = (ctp2_Window *)aui_Ldl::BuildHierarchyFromRoot(s_scenarioEditorBlock);
	Assert(m_window);
	if(!m_window) {
		*err = AUI_ERRCODE_INVALIDPARAM;
		return;
	}

	m_addStuffWindow = (ctp2_Window *)aui_Ldl::BuildHierarchyFromRoot(s_scenarioAddStuffBlock);
	Assert(m_addStuffWindow);

	m_tabButton[0] = (ctp2_Button *)aui_Ldl::GetObject(s_scenarioEditorBlock, "TabGroup.WorldButton");
	m_tabButton[1] = (ctp2_Button *)aui_Ldl::GetObject(s_scenarioEditorBlock, "TabGroup.UnitButton");
	m_tabButton[2] = (ctp2_Button *)aui_Ldl::GetObject(s_scenarioEditorBlock, "TabGroup.CityButton");
	m_tabButton[3] = (ctp2_Button *)aui_Ldl::GetObject(s_scenarioEditorBlock, "TabGroup.CivButton");

	m_tabButton[0]->SetActionFuncAndCookie(TabCallback, (void *)SCEN_TAB_WORLD);
	m_tabButton[1]->SetActionFuncAndCookie(TabCallback, (void *)SCEN_TAB_UNIT);
	m_tabButton[2]->SetActionFuncAndCookie(TabCallback, (void *)SCEN_TAB_CITY);
	m_tabButton[3]->SetActionFuncAndCookie(TabCallback, (void *)SCEN_TAB_CIV);

	

	aui_Ldl::SetActionFuncAndCookie(s_scenarioEditorBlock, "Globals.SaveScenarioButton", SaveScenario, NULL);
	aui_Ldl::SetActionFuncAndCookie(s_scenarioEditorBlock, "Globals.LoadMapButton", LoadMap, NULL);
	aui_Ldl::SetActionFuncAndCookie(s_scenarioEditorBlock, "Globals.SaveMapButton", SaveMap, NULL);

	aui_Ldl::SetActionFuncAndCookie(s_scenarioEditorBlock, "Globals.MapSize", MapSize, NULL);
	aui_Ldl::SetActionFuncAndCookie(s_scenarioEditorBlock, "Globals.Barbarians", Barbarians, NULL);
	aui_Ldl::SetActionFuncAndCookie(s_scenarioEditorBlock, "Globals.Difficulty", Difficulty, NULL);

	
	aui_Ldl::SetActionFuncAndCookie(s_scenarioEditorBlock, "UniversalControls.ReloadSlicButton", ReloadSlic, NULL);
		
	ctp2_Spinner *spin;
	sint32 i;
	for(i = 0; i < k_NUM_PLAYER_SPINNERS; i++)
	{
		spin = (ctp2_Spinner *)aui_Ldl::GetObject(s_scenarioEditorBlock, s_playerSpinners[i]);
		if(spin)
		{
			//Added by Martin Gühmann to make sure that the Scenario Editor 
			//does not set the player to player 1 when the scenario editor
			//is loaded for the first time in a session.
			spin->SetValue((sint32)g_selected_item->GetPlayerOnScreen(), 0);
			spin->SetMinimum(0, 0);
			spin->SetMaximum(GetLastPlayer(), 0);
			// Set the callback at the end, otherwise it is always executed when you set min or max
			spin->SetSpinnerCallback(PlayerSpinner, NULL);
		}
	}

	spin = (ctp2_Spinner *)aui_Ldl::GetObject(s_scenarioEditorBlock, "Globals.YearSpinner");
	spin->SetSpinnerCallback(Year, NULL);
	
	aui_Ldl::SetActionFuncAndCookie(s_scenarioEditorBlock, "TabGroup.Unit.LandButton", 
									UnitTabButton, (void *)SCEN_UNIT_CAT_LAND);
	aui_Ldl::SetActionFuncAndCookie(s_scenarioEditorBlock, "TabGroup.Unit.SeaButton", 
									UnitTabButton, (void *)SCEN_UNIT_CAT_SEA);
	aui_Ldl::SetActionFuncAndCookie(s_scenarioEditorBlock, "TabGroup.Unit.AirButton", 
									UnitTabButton, (void *)SCEN_UNIT_CAT_AIR);
	aui_Ldl::SetActionFuncAndCookie(s_scenarioEditorBlock, "TabGroup.Unit.SpecialButton", 
									UnitTabButton, (void *)SCEN_UNIT_CAT_SPECIAL);
	//aui_Ldl::SetActionFuncAndCookie(s_scenarioEditorBlock, "TabGroup.Unit.ShowEnemyHealth", ShowEnemyHealth, NULL); //emod
	aui_Ldl::SetActionFuncAndCookie(s_scenarioEditorBlock, "TabGroup.Unit.DebugAI", DebugAI, NULL); //emod
	//s_ShowEnemyHealth		= spNew_aui_Switch(err, s_scenarioEditorBlock, "TabGroup.Unit.ShowEnemyHealth", ShowEnemyHealth, NULL); //emod5
	//ctp2_Switch *s_ShowEnemyHealth = (ctp2_Switch *)aui_Ldl::GetObject(s_scenarioEditorBlock, "TabGroup.Unit.ShowEnemyHealth");
    //s_ShowEnemyHealth->SetState(g_theProfileDB->GetShowEnemyHealth());

	aui_Ldl::SetActionFuncAndCookie(s_scenarioEditorBlock, "UnitControls.LabelToggle", ToggleLabels, NULL);

	spin = (ctp2_Spinner *)aui_Ldl::GetObject(s_scenarioEditorBlock, "TabGroup.City.Pop");
	if(spin) spin->SetSpinnerCallback(CityPopSpinner, NULL);

	aui_Ldl::SetActionFuncAndCookie(s_scenarioEditorBlock, "TabGroup.City.AddBuildings", CityAddBuildings, NULL);
	aui_Ldl::SetActionFuncAndCookie(s_scenarioEditorBlock, "TabGroup.City.AddWonders", CityAddWonders, NULL);

	aui_Ldl::SetActionFuncAndCookie(s_scenarioEditorBlock, "CivControls.AddAdvances", CivAddAdvances, NULL);
	
	spin = (ctp2_Spinner *)aui_Ldl::GetObject(s_scenarioEditorBlock, "CivControls.CityStyleSpinner");
	if(spin)
	{
		spin->SetMinimum(0, 0);
		spin->SetMaximum(g_theCityStyleDB->NumRecords()-1, 0);
		spin->SetDispalyValue(false);

		sint32 style = g_player[g_selected_item->GetPlayerOnScreen()]->GetCivilisation()->GetCityStyle();
		style = (style >= 0 && style < g_theCityStyleDB->NumRecords()) ? style : 0;

		spin->SetValue(style, 0);
		
		const CityStyleRecord* rec = g_theCityStyleDB->Get(style);

		spin->SetText(rec->GetNameText());
		spin->SetDisplay();

		aui_TipWindow* tipWindow = (aui_TipWindow *)spin->GetTipWindow();
		if(tipWindow)
		{
			tipWindow->SetTipText(const_cast<char*>(rec->GetNameText()));
		}
		// Set callback at least so that nothing else calls it, before everything is finished
		spin->SetSpinnerCallback(CivCityStyleSpinner, NULL);
	}

	aui_Ldl::SetActionFuncAndCookie(s_scenarioEditorBlock, "TabGroup.Civ.SetGovernment", SetGovernment, NULL);
	aui_Ldl::SetActionFuncAndCookie(s_scenarioEditorBlock, "TabGroup.Civ.PlayerSelect", LimitPlayerChoice, NULL);

	
	
	

	aui_Ldl::SetActionFuncAndCookie(s_scenarioEditorBlock, "TabGroup.Civ.FullModeSwitch", 
									CivModeSwitch, (void *)SCEN_START_LOC_MODE_NONE);
	aui_Ldl::SetActionFuncAndCookie(s_scenarioEditorBlock, "TabGroup.Civ.PlayerNoCivSwitch", 
									CivModeSwitch, (void *)SCEN_START_LOC_MODE_PLAYER);
	aui_Ldl::SetActionFuncAndCookie(s_scenarioEditorBlock, "TabGroup.Civ.PlayerWithCivSwitch", 
									CivModeSwitch, (void *)SCEN_START_LOC_MODE_PLAYER_WITH_CIV);
	aui_Ldl::SetActionFuncAndCookie(s_scenarioEditorBlock, "TabGroup.Civ.JustCivSwitch", 
									CivModeSwitch, (void *)SCEN_START_LOC_MODE_CIV);

	aui_Ldl::SetActionFuncAndCookie(s_scenarioEditorBlock, "TabGroup.Civ.AddPW", AddPW, NULL);
	aui_Ldl::SetActionFuncAndCookie(s_scenarioEditorBlock, "TabGroup.Civ.AddGold", AddGold, NULL);

	aui_Ldl::SetActionFuncAndCookie(s_scenarioEditorBlock, "Globals.ExitButton", Exit, NULL);

	aui_Ldl::SetActionFuncAndCookie(s_scenarioEditorBlock, "WorldControls.CutButton", CutRegion, NULL);
	aui_Ldl::SetActionFuncAndCookie(s_scenarioEditorBlock, "WorldControls.CopyButton", CopyRegion, NULL);
	aui_Ldl::SetActionFuncAndCookie(s_scenarioEditorBlock, "WorldControls.PasteButton", PasteRegion, NULL);

	aui_Ldl::SetActionFuncAndCookie(s_scenarioEditorBlock, "WorldControls.SmallBrushButton", BrushSize, (void *)1);
	aui_Ldl::SetActionFuncAndCookie(s_scenarioEditorBlock, "WorldControls.MedBrushButton", BrushSize, (void *)2);
	aui_Ldl::SetActionFuncAndCookie(s_scenarioEditorBlock, "WorldControls.LargeBrushButton", BrushSize, (void *)4);

	aui_Ldl::SetActionFuncAndCookie(s_scenarioEditorBlock, "WorldControls.SaveClipButton", SaveClip, NULL);
	aui_Ldl::SetActionFuncAndCookie(s_scenarioEditorBlock, "WorldControls.LoadClipButton", LoadClip, NULL);


	aui_Ldl::SetActionFuncAndCookie(s_scenarioEditorBlock, "WorldControls.ExploreButton", ExploreButton, NULL);
	aui_Ldl::SetActionFuncAndCookie(s_scenarioEditorBlock, "WorldControls.UnexploreButton", UnexploreButton, NULL);

	aui_Ldl::SetActionFuncAndCookie(s_scenarioEditorBlock, "UniversalControls.FogButton", FogButton, NULL);
	
	aui_Ldl::SetActionFuncAndCookie(s_scenarioEditorBlock, "CityExtraControls.CityField", CityName, NULL);
	aui_Ldl::SetActionFuncAndCookie(s_scenarioEditorBlock, "CivExtraControls.LeaderField", LeaderName, NULL);


	
	aui_Ldl::SetActionFuncAndCookie(s_scenarioEditorBlock, "UniversalControls.SelectButton", RegionButton, NULL);

	
	aui_Ldl::SetActionFuncAndCookie(s_scenarioAddStuffBlock, "CloseButton", CloseAddStuff, NULL);
	aui_Ldl::SetActionFuncAndCookie(s_scenarioAddStuffBlock, "Left", AddLeftList, NULL);
	aui_Ldl::SetActionFuncAndCookie(s_scenarioAddStuffBlock, "Right", AddRightList, NULL);
	aui_Ldl::SetActionFuncAndCookie(s_scenarioAddStuffBlock, "AddButton", AddAddButton, NULL);
	aui_Ldl::SetActionFuncAndCookie(s_scenarioAddStuffBlock, "RemoveButton", AddRemoveButton, NULL);
	aui_Ldl::SetActionFuncAndCookie(s_scenarioAddStuffBlock, "AddStuffTitle", AddLeftList, NULL);

	
	
	aui_Ldl::SetActionFuncAndCookie(s_scenarioEditorBlock, "UniversalControls.EraseButton", EraseMode, NULL);

	m_otherMapSwitch[0] = (ctp2_Switch *)aui_Ldl::GetObject(s_scenarioEditorBlock, "TabGroup.World.HutSwitch");
	m_otherMapSwitch[1] = (ctp2_Switch *)aui_Ldl::GetObject(s_scenarioEditorBlock, "TabGroup.World.RiverSwitch");
	m_otherMapSwitch[2] = (ctp2_Switch *)aui_Ldl::GetObject(s_scenarioEditorBlock, "TabGroup.World.Good1Switch");
	m_otherMapSwitch[3] = (ctp2_Switch *)aui_Ldl::GetObject(s_scenarioEditorBlock, "TabGroup.World.Good2Switch");
	m_otherMapSwitch[4] = (ctp2_Switch *)aui_Ldl::GetObject(s_scenarioEditorBlock, "TabGroup.World.Good3Switch");
	m_otherMapSwitch[5] = (ctp2_Switch *)aui_Ldl::GetObject(s_scenarioEditorBlock, "TabGroup.World.Good4Switch");

	m_otherMapSwitch[0]->SetActionFuncAndCookie(WorldTabSwitch, (void *)SCEN_MAP_HUT);
	m_otherMapSwitch[1]->SetActionFuncAndCookie(WorldTabSwitch, (void *)SCEN_MAP_RIVER);
	m_otherMapSwitch[2]->SetActionFuncAndCookie(WorldTabSwitch, (void *)SCEN_MAP_GOOD1);
	m_otherMapSwitch[3]->SetActionFuncAndCookie(WorldTabSwitch, (void *)SCEN_MAP_GOOD2);
	m_otherMapSwitch[4]->SetActionFuncAndCookie(WorldTabSwitch, (void *)SCEN_MAP_GOOD3);
	m_otherMapSwitch[5]->SetActionFuncAndCookie(WorldTabSwitch, (void *)SCEN_MAP_GOOD4);

	aui_Ldl::SetActionFuncAndCookie(s_scenarioEditorBlock, "UniversalControls.ClearWorldButton", ClearWorld, NULL);

	
	aui_Ldl::SetActionFuncAndCookie(s_scenarioEditorBlock, "WorldExtraControls.FindMeButton", FindPosNow, NULL);

	aui_Ldl::SetActionFuncAndCookie(s_scenarioEditorBlock, "WorldExtraControls.RemoveGoods", RemoveGoods, NULL);
	aui_Ldl::SetActionFuncAndCookie(s_scenarioEditorBlock, "WorldExtraControls.GenerateGoods", GenerateGoods, NULL);
	aui_Ldl::SetActionFuncAndCookie(s_scenarioEditorBlock, "TabGroup.World.Pollution", Pollution, NULL);

	
	m_eraseButton = (ctp2_Switch *)aui_Ldl::GetObject(s_scenarioEditorBlock, "UniversalControls.EraseButton");

	m_window->SetDraggable( TRUE );
	m_addStuffWindow->SetDraggable( TRUE);
	
	
	ctp2_Switch *pButton = (ctp2_Switch *)aui_Ldl::GetObject(s_scenarioEditorBlock, "WorldControls.XWrapButton");
    pButton->SetState((m_xWrap) ? 1 : 0);
	
	aui_Ldl::SetActionFuncAndCookie(s_scenarioEditorBlock, "WorldControls.XWrapButton", SetXWrap, NULL);

	pButton = (ctp2_Switch *)aui_Ldl::GetObject(s_scenarioEditorBlock, "WorldControls.YWrapButton");
    pButton->SetState((m_yWrap) ? 1 : 0);
	aui_Ldl::SetActionFuncAndCookie(s_scenarioEditorBlock, "WorldControls.YWrapButton", SetYWrap, NULL);

	*err = AUI_ERRCODE_OK;
}

//----------------------------------------------------------------------------
//
// Name       : ~ScenarioEditor
//
// Description: Destructor
//
// Parameters : -
//
// Globals    : -
//
// Returns    : -
//
// Remark(s)  : Also m_terrainImpSwitches needs to be deleted.
//              Nulling m_terrainSwitches seems to be contraproductive.
//              Unfortunatly both things don't have any effect
//              PopulateTerrainImprovementList and PopulateTerrainList
//              still show up in the CTP_LEAKS_99999.TXT, but 
//              m_terrainImpSwitches are created with new[] and therefore
//              should be deleted with delete[].
//
//----------------------------------------------------------------------------
ScenarioEditor::~ScenarioEditor()
{
	if (m_window) 
    {
		aui_Ldl::DeleteHierarchyFromRoot(s_scenarioEditorBlock);
	}

	if (m_addStuffWindow) 
    {
		aui_Ldl::DeleteHierarchyFromRoot(s_scenarioAddStuffBlock);
	}

	//Added by Martin Gühmann
	delete [] m_terrainSwitches;
	delete [] m_terrainImpSwitches;

	delete m_copyBuffer;
	delete m_fileDialog;
}

AUI_ERRCODE ScenarioEditor::Initialize()
{
	if(s_scenarioEditor)
		return AUI_ERRCODE_OK;

	AUI_ERRCODE err = AUI_ERRCODE_OK;
	s_scenarioEditor = new ScenarioEditor(&err);
	Assert(err == AUI_ERRCODE_OK);

	return err;
}

AUI_ERRCODE ScenarioEditor::Cleanup()
{
	if(s_scenarioEditor) {

		// Only execute the necessary stuff from ScenarioEditor::Hide
		if(s_scenarioEditor->m_addStuffWindow) {
			g_c3ui->RemoveWindow(s_scenarioEditor->m_addStuffWindow->Id());
		}

		if(s_scenarioEditor->m_window){
			g_c3ui->RemoveWindow(s_scenarioEditor->m_window->Id());
		}

        allocated::clear(s_scenarioEditor);
	}

	return AUI_ERRCODE_OK;
}

AUI_ERRCODE ScenarioEditor::Display()
{
	if(g_network.IsActive())
		
		return AUI_ERRCODE_OK;

	AUI_ERRCODE err = AUI_ERRCODE_OK;
	if (!s_scenarioEditor) 
    {
		err = Initialize();
	    Assert(err == AUI_ERRCODE_OK);
	    if (err != AUI_ERRCODE_OK) 
        {
		    return err;
	    }
	}

	if(!g_attractWindow) {
		g_attractWindow->Initialize();
	}

	if(s_scenarioEditor->m_window) {
		err = g_c3ui->AddWindow(s_scenarioEditor->m_window);
		Assert(err == AUI_ERRCODE_OK);
		if(err == AUI_ERRCODE_OK) {
			err = s_scenarioEditor->m_window->Show();
		}

		s_scenarioEditor->SetCivSwitches();
		
		
		if (s_scenarioEditor->m_initializing == true)
			s_scenarioEditor->SetupNations();
		s_scenarioEditor->SetTab(SCEN_TAB_WORLD);
		s_scenarioEditor->PopulateTerrainList();
		s_scenarioEditor->PopulateUnitList(SCEN_UNIT_CAT_LAND);
		s_scenarioEditor->PopulateCityList();

		
		s_scenarioEditor->PopulateTerrainImprovementList();  //emod5 defientely need something to populate it

		s_scenarioEditor->SetupGlobalControls();
		s_scenarioEditor->Update();
	}

	s_scenarioEditor->m_initializing = false;

	return err;
}

AUI_ERRCODE ScenarioEditor::Hide()
{
	if(!s_scenarioEditor)
		return AUI_ERRCODE_OK;

	if(!s_scenarioEditor->m_window)
		return AUI_ERRCODE_OK;

	if(s_scenarioEditor->m_addStuffWindow) {
		g_c3ui->RemoveWindow(s_scenarioEditor->m_addStuffWindow->Id());
	}

	if (g_controlPanel!=NULL)
		g_controlPanel->TileImpPanelRedisplay();

	s_scenarioEditor->m_paintTerrain            = -1;
	s_scenarioEditor->m_paintTerrainImprovement = -1;	
	s_scenarioEditor->m_unitIndex               = -1;
	s_scenarioEditor->m_cityStyle               = CITY_STYLE_EDITOR;
	s_scenarioEditor->m_mapMode                 = SCEN_MAP_NONE;

	if(g_theWorld)
		g_theWorld->NumberContinents();
	
	ScenarioEditor::Reupdate();

	return g_c3ui->RemoveWindow(s_scenarioEditor->m_window->Id());
}

bool ScenarioEditor::IsShown()
{
	if(!s_scenarioEditor) return false;
	return g_c3ui->GetWindow(s_scenarioEditor->m_window->Id()) != NULL;
}

void ScenarioEditor::NotifySelection()
{
	if(!s_scenarioEditor) return;

	if(s_scenarioEditor->m_addStuffWindow) {
		if(g_c3ui->GetChild(s_scenarioEditor->m_addStuffWindow->Id()) != NULL) {
			s_scenarioEditor->UpdateAddList(s_scenarioEditor->m_addMode);
		}
	}

	Unit city;
	if(g_selected_item->GetSelectedCity(city)) {
		ctp2_TextField *tf = (ctp2_TextField *)aui_Ldl::GetObject(s_scenarioEditorBlock, "CityExtraControls.CityField");
		Assert(tf);
		if(tf) {
			tf->SetFieldText(city.GetName());
		}
	}
}

void ScenarioEditor::Update()
{
	if(!s_scenarioEditor) return;

	for(sint32 i = 0; i < k_MAX_PLAYERS; i++)
	{
		if(g_player[i])
		{
			g_player[i]->m_playerType = PLAYER_TYPE_HUMAN;
		}
	}
}

void ScenarioEditor::Reupdate()
{
	if(!s_scenarioEditor) return;

	for(sint32 i = 0; i < k_MAX_PLAYERS; i++)
	{
		if(g_player[i])
		{
			g_player[i]->m_playerType = PLAYER_TYPE_ROBOT;
		}
	}

	if(g_player[g_selected_item->GetVisiblePlayer()]) {
		g_player[g_selected_item->GetVisiblePlayer()]->m_playerType = PLAYER_TYPE_HUMAN;
		NewTurnCount::SetStopPlayer(g_selected_item->GetVisiblePlayer());
	}

	if (g_toeMode)
	{
		g_toeMode = 0;
		s_scenarioEditor->m_eraseButton->SetState(0);
	}
}

void ScenarioEditor::PopulateTerrainList()
{
	ctp2_ListBox *lb = (ctp2_ListBox *)aui_Ldl::GetObject(s_scenarioEditorBlock, "TabGroup.World.List");
	Assert(lb);
	if(!lb)	return;

	lb->SetAbsorbancy(FALSE);
	lb->Clear();

	sint32 col = 0;
	ctp2_ListItem *curItem = NULL;
	ctp2_Static *curItemBox = NULL;

	delete [] m_terrainSwitches;
	m_terrainSwitches = new ctp2_Switch *[g_theTerrainDB->NumRecords()];

	for (sint32 t = 0; t < g_theTerrainDB->NumRecords(); t++) {
		if(col == 0) {
			curItem = (ctp2_ListItem *)aui_Ldl::BuildHierarchyFromRoot("ScenTerrainItem");
			Assert(curItem);
			if(!curItem)
				break;

			curItemBox = (ctp2_Static *)curItem->GetChildByIndex(0);
			Assert(curItemBox);
			if(!curItemBox)
				break;

			lb->AddItem(curItem);
		}
		
		Assert(curItem && curItemBox);
		if(!curItem || !curItemBox)
			break;

		ctp2_Switch *sw = (ctp2_Switch *)curItemBox->GetChildByIndex(col);
		Assert(sw);
		if(!sw)
			break;

		const TerrainRecord *trec = g_theTerrainDB->Get(t);
		Assert(trec);
		if(!trec)
			break;

		const MBCHAR *iconname = trec->GetIcon()->GetIcon();
		Assert(iconname);
		if(iconname) {
			sw->SetImage(iconname, 0);
			sw->SetImage(iconname, 1);
		}
		
		m_terrainSwitches[t] = sw;

		sw->SetActionFuncAndCookie(ScenarioEditor::TerrainSwitch, (void *)t);
		((aui_TipWindow *)sw->GetTipWindow())->SetTipText((MBCHAR *)trec->GetNameText());

		col++;
		if(col >= k_TERRAIN_COLS_PER_ROW) {
			col = 0;
			curItem = NULL;
			curItemBox = NULL;
		}
	}

	
	if(col > 0) {
		Assert(curItem && curItemBox);
		sint32 dis;
		for(dis = col; dis < k_TERRAIN_COLS_PER_ROW; dis++) {
			ctp2_Switch *sw = (ctp2_Switch *)curItemBox->GetChildByIndex(dis);
			Assert(sw);
			if(sw)
				sw->Enable(FALSE);
		}
	}
}

void ScenarioEditor::PopulateUnitList(SCEN_UNIT_CAT cat)
{
	ctp2_ListBox *lb = (ctp2_ListBox *)aui_Ldl::GetObject(s_scenarioEditorBlock, "TabGroup.Unit.List");
	Assert(lb);
	if(!lb) return;

	lb->SetAbsorbancy(FALSE);
	lb->Clear();

	ctp2_ListItem *curItem = NULL;
	ctp2_Static *curItemBox = NULL;
	sint32 col = 0;

	for (sint32 ui = 0; ui < g_theUnitDB->NumRecords(); ui++) {
		bool addme = false;
		const UnitRecord *rec = g_theUnitDB->Get(ui);

		
		if(rec->GetHasPopAndCanBuild())
			continue;

		
		switch(cat) {
			case SCEN_UNIT_CAT_LAND:
				if((rec->GetMovementTypeLand() ||
					rec->GetMovementTypeMountain()) &&
				   (rec->GetAttack() > 0)) {
					addme = true;
				}
				break;
			case SCEN_UNIT_CAT_SEA:
				if((rec->GetMovementTypeSea() ||
					rec->GetMovementTypeShallowWater()) &&
				   (rec->GetAttack() > 0)) {
					addme = true;
				}
				break;
			case SCEN_UNIT_CAT_AIR:
				if(rec->GetMovementTypeAir() &&
				   (rec->GetAttack() > 0)) {
					addme = true;
				}
				break;
			case SCEN_UNIT_CAT_SPECIAL:
				if((rec->GetAttack() <= 0) ||
				   (rec->GetMovementTypeTrade())) {
					addme = true;
				}
				break;
		}
		if(!addme)
			continue;

		if(col == 0) {
			curItem = (ctp2_ListItem *)aui_Ldl::BuildHierarchyFromRoot("ScenUnitItem");
			Assert(curItem);
			if(!curItem)
				break;

			curItemBox = (ctp2_Static *)curItem->GetChildByIndex(0);
			Assert(curItemBox);
			if(!curItemBox)
				break;

			lb->AddItem(curItem);
		}

		Assert(curItem && curItemBox);
		if(!curItem || !curItemBox) break;

		
		ctp2_Switch *sw = (ctp2_Switch *)curItemBox->GetChildByIndex(col * 2);
		Assert(sw);
		if(!sw) break;

		const MBCHAR *iconname = rec->GetDefaultIcon()->GetIcon();
		Assert(iconname);
		if(iconname) {
			sw->SetImage((char *)iconname, 0);
			sw->SetImage((char *)iconname, 1);
		}

		sw->SetActionFuncAndCookie(ScenarioEditor::UnitSwitch, (void *)ui);

		((aui_TipWindow *)sw->GetTipWindow())->SetTipText((MBCHAR *)rec->GetNameText());

		
		
		
		sw = (ctp2_Switch *)curItemBox->GetChildByIndex((col * 2) + 1);
		if( g_exclusions->IsUnitExcluded(ui) )
		{
			sw->SetState(1);
		}
		sw->SetActionFuncAndCookie(ScenarioEditor::ExcludeSwitch, (void *)ui);
		col++;
		if(col >= k_UNIT_COLS_PER_ROW) {
			col = 0;
			curItem = NULL;
			curItemBox = NULL;
		}
	}

	
	
	if(col > 0) {
		Assert(curItem && curItemBox);
		for (sint32 dis = col; dis < k_UNIT_COLS_PER_ROW; dis++) {
			ctp2_Switch *sw = (ctp2_Switch *)curItemBox->GetChildByIndex(dis * 2);
			sw->SetActionFuncAndCookie(ScenarioEditor::UnitSwitch, (void *)-1);
			Assert(sw);
			if(sw) sw->Enable(FALSE);
		}
	}
}

void ScenarioEditor::PopulateCityList()
{
	
	ctp2_ListBox *lb = (ctp2_ListBox *)aui_Ldl::GetObject(s_scenarioEditorBlock, "TabGroup.City.List");
	Assert(lb);
	if(!lb) return;

	lb->SetAbsorbancy(FALSE);
	lb->Clear();

	ctp2_ListItem *curItem = NULL;
	ctp2_Static *curItemBox = NULL;
	sint32 col = 0;
	//Added by Martin Gühmann so that there are now as much buttons
	//as city styles.
	for (sint32 cs = 0; cs < g_theCityStyleDB->NumRecords(); cs++) {
		if(col == 0) {
			curItem = (ctp2_ListItem *)aui_Ldl::BuildHierarchyFromRoot("ScenCityItem");
			Assert(curItem);
			if(!curItem)
				break;

			curItemBox = (ctp2_Static *)curItem->GetChildByIndex(0);
			Assert(curItemBox);
			if(!curItemBox)
				break;

			lb->AddItem(curItem);
		}

		Assert(curItem && curItemBox);
		if(!curItem || !curItemBox) break;

		ctp2_Switch *sw = (ctp2_Switch *)curItemBox->GetChildByIndex(col);
		Assert(sw);
		if(!sw) break;

		
		sw->SetActionFuncAndCookie(ScenarioEditor::CityStyleSwitch, (void *)cs);
		col++;
		if(col >= k_CITY_COLS_PER_ROW) {
			col = 0;
			curItem = NULL;
			curItemBox = NULL;
		}

//Added by Martin Gühmann to give the city buttons an icon.
//Added by Martin Gühmann to show the according city style name in the tooltip.

// Modified to allow buttons not to have an icon:
// - moved down check to prevent a premature break
// - removed annoying popups 
// - prevented crashes when TipWindow would be NULL.
		CityStyleRecord const *	rec		= g_theCityStyleDB->Get(cs);

		if (rec)
		{
			MBCHAR const *	iconName;
			if (rec->GetCPIcon(iconName))
			{
				sw->SetImage(const_cast<char *>(iconName), 0);
				sw->SetImage(const_cast<char *>(iconName), 1);
			}
		
			aui_TipWindow *	tipWindow	= 
				static_cast<aui_TipWindow *>(sw->GetTipWindow());
				// should be dynamic_cast, but RTTI has been disabled

			if (tipWindow)
			{
				tipWindow->SetTipText(const_cast<char *>(rec->GetNameText()));
			}
		}
	}

	if(col > 0) {
		Assert(curItem && curItemBox);
		for (sint32 dis = col; dis < k_CITY_COLS_PER_ROW; dis++) {
			ctp2_Switch *sw = (ctp2_Switch *)curItemBox->GetChildByIndex(dis);
			sw->SetActionFuncAndCookie(ScenarioEditor::UnitSwitch, (void *)-1);
			Assert(sw);
			if(sw) sw->Enable(FALSE);
		}
	}
}


void ScenarioEditor::PopulateTerrainImprovementList()  //emod1 note  use this format instead of current switches but add item populating checks?
{

	ctp2_ListBox *lb = (ctp2_ListBox *)aui_Ldl::GetObject(s_scenarioEditorBlock, "TabGroup.City.TerrainImprovementList");
	Assert(lb);
	if(!lb)	return;

	lb->SetAbsorbancy(FALSE);
	lb->Clear();

	sint32 col = 0;
	ctp2_ListItem *curItem = NULL;
	ctp2_Static *curItemBox = NULL;

	delete [] m_terrainImpSwitches;
	m_terrainImpSwitches = new ctp2_Switch *[g_theTerrainImprovementDB->NumRecords()];

	for (sint32 t = 0; t < g_theTerrainImprovementDB->NumRecords(); t++) {
		if(col == 0) {
			curItem = (ctp2_ListItem *)aui_Ldl::BuildHierarchyFromRoot("ScenTerrainImprovementItem");
			Assert(curItem);
			if(!curItem)
				break;

			curItemBox = (ctp2_Static *)curItem->GetChildByIndex(0);
			Assert(curItemBox);
			if(!curItemBox)
				break;

			lb->AddItem(curItem);
		}
		
		Assert(curItem && curItemBox);
		if(!curItem || !curItemBox)
			break;

		ctp2_Switch *sw = (ctp2_Switch *)curItemBox->GetChildByIndex(col);
		Assert(sw);
		if(!sw)
			break;

		const TerrainImprovementRecord *rec = g_theTerrainImprovementDB->Get(t);
		Assert(rec);
		if(!rec)
			break;

		const MBCHAR *iconname = rec->GetIcon()->GetIcon();
		Assert(iconname);
		if(iconname) {
			sw->SetImage((char *)iconname, 0);
			sw->SetImage((char *)iconname, 1);
		}
		
		m_terrainImpSwitches[t] = sw;

		sw->SetActionFuncAndCookie(ScenarioEditor::TerrainImprovementSwitch, (void *)t);
		((aui_TipWindow *)sw->GetTipWindow())->SetTipText((MBCHAR *)rec->GetNameText());

		col++;
		if(col >= k_TERRAINIMP_COLS_PER_ROW) {
			col = 0;
			curItem = NULL;
			curItemBox = NULL;
		}
	}

	
	if(col > 0) {
		Assert(curItem && curItemBox);
		for (sint32 dis = col; dis < k_TERRAINIMP_COLS_PER_ROW; dis++) {
			ctp2_Switch *sw = (ctp2_Switch *)curItemBox->GetChildByIndex(dis);
			Assert(sw);
			if(sw)
				sw->Enable(FALSE);
		}
	}
}

void ScenarioEditor::RehideUnitSwitches()
{
	ctp2_ListBox *lb = (ctp2_ListBox *)aui_Ldl::GetObject(s_scenarioEditorBlock, "TabGroup.Unit.List");
	Assert(lb);
	if(!lb) return;

	ctp2_ListItem *item = (ctp2_ListItem *)lb->GetItemByIndex(lb->NumItems() - 1);
	Assert(item);
	if(!item) return;

	ctp2_Static *box = (ctp2_Static *)item->GetChildByIndex(0);
	Assert(box);
	if(!box) return;

	for (sint32 i = 0; i < k_UNIT_COLS_PER_ROW; i++) {
		ctp2_Switch *sw = (ctp2_Switch *)box->GetChildByIndex(i * 2);
		Assert(sw);
		if(!sw) break;

		if((sint32)sw->GetCookie() < 0) {
			
			sw->Enable(FALSE);
		}
	}
}

bool ScenarioEditor::HandleClicks() //emod2 need these?
{
	
	return PaintTerrainMode() || PaintTerrainImprovementMode() || PlaceUnitsMode() || PlaceCityMode() || 
		PlaceStartFlags() || SelectRegion() || PasteMode() || PaintHutMode() || PaintRiverMode() || PaintGoodsMode();
}

bool ScenarioEditor::PaintTerrainMode()
{
	if(!s_scenarioEditor) return false;
	return s_scenarioEditor->m_mapMode == SCEN_MAP_TERRAIN;
}
sint32 ScenarioEditor::PaintTerrain()
{
	if(!s_scenarioEditor) return -1;
	return s_scenarioEditor->m_paintTerrain;
}
bool ScenarioEditor::PaintTerrainImprovementMode() //emod3 need this?
{
	if(!s_scenarioEditor) return false;
	return s_scenarioEditor->m_mapMode == SCEN_MAP_TERRAINIMP;
}
sint32 ScenarioEditor::PaintTerrainImprovement()
{
	if(!s_scenarioEditor) return -1;
	return s_scenarioEditor->m_paintTerrainImprovement;
}

sint32 ScenarioEditor::BrushSize()
{
	if(!s_scenarioEditor) return 1;
	return s_scenarioEditor->m_brushSize;
}

bool ScenarioEditor::PlaceUnitsMode()
{
	if(!s_scenarioEditor) return false;
	return s_scenarioEditor->m_mapMode == SCEN_MAP_UNIT; 
}

sint32 ScenarioEditor::UnitIndex()
{
	if(!s_scenarioEditor) return -1;
	return s_scenarioEditor->m_unitIndex;
}

bool ScenarioEditor::PlaceCityMode()
{
	if(!s_scenarioEditor) return false;
	return s_scenarioEditor->m_mapMode == SCEN_MAP_CITY; 
}

sint32 ScenarioEditor::CityStyle()
{
    return (s_scenarioEditor) ? s_scenarioEditor->m_cityStyle : CITY_STYLE_EDITOR;
}

//Added by Martin Gühmann to make 
//shure that newly created cities 
//have the same pop size as displayed in
//the CityPopSpinner
sint32 ScenarioEditor::CitySize()
{
	if(!s_scenarioEditor) return 1;
	return s_scenarioEditor->m_newPopSize;
}

bool ScenarioEditor::PlaceStartFlags()
{
	if(!s_scenarioEditor) return false;
	return s_scenarioEditor->m_mapMode == SCEN_MAP_STARTFLAGS;
}

bool ScenarioEditor::ShowStartFlags()
{
	if(!s_scenarioEditor) return false;
	return s_scenarioEditor->m_startLocMode != SCEN_START_LOC_MODE_NONE;
}

SCEN_START_LOC_MODE ScenarioEditor::GetStartLocMode()
{
	if(!s_scenarioEditor) return SCEN_START_LOC_MODE_NONE;
	return s_scenarioEditor->m_startLocMode;
}

bool ScenarioEditor::PasteMode()
{
	if(!s_scenarioEditor) return false;
	return s_scenarioEditor->m_mapMode == SCEN_MAP_PASTE;
}

bool ScenarioEditor::PaintHutMode()
{
	if(!s_scenarioEditor) return false;
	return s_scenarioEditor->m_mapMode == SCEN_MAP_HUT;
}

bool ScenarioEditor::PaintRiverMode()
{
	if(!s_scenarioEditor) return false;
	return s_scenarioEditor->m_mapMode == SCEN_MAP_RIVER;
}

bool ScenarioEditor::PaintGoodsMode()
{
	if(!s_scenarioEditor) return false;
	return s_scenarioEditor->m_mapMode >= SCEN_MAP_GOOD1 && s_scenarioEditor->m_mapMode <= SCEN_MAP_GOOD4;
}

sint32 ScenarioEditor::PaintGood()
{
	if(!s_scenarioEditor) return false;
	if(s_scenarioEditor->m_mapMode >= SCEN_MAP_GOOD1) {
		return s_scenarioEditor->m_mapMode - SCEN_MAP_GOOD1;
	}
	return 0;
}

void ScenarioEditor::TerrainImprovementSwitch(aui_Control *control, uint32 action, uint32 data, void *cookie)
{//emod4 need this functionality should be there
	Assert(s_scenarioEditor);
	if(!s_scenarioEditor)
		return;

	if(action == 0) {
		
		return;
	}

	if (action == AUI_SWITCH_ACTION_PRESS)
		DisableErase();

	sint32 ter = (sint32)cookie;

	if(s_scenarioEditor->m_terrainImpSwitches[ter]->GetState() == 0) {
		s_scenarioEditor->m_paintTerrainImprovement = -1;
		if(s_scenarioEditor->m_mapMode == SCEN_MAP_TERRAINIMP) {
			s_scenarioEditor->m_mapMode = SCEN_MAP_NONE;
		}
		
		return;
	}

	for (sint32 i = 0; i < g_theTerrainImprovementDB->NumRecords(); i++) {
		if(i == ter)
			continue;
		if(s_scenarioEditor->m_terrainImpSwitches[i]) {
			s_scenarioEditor->m_terrainImpSwitches[i]->SetState(0);
		}
	}

	s_scenarioEditor->m_paintTerrainImprovement = ter;
	s_scenarioEditor->m_mapMode = SCEN_MAP_TERRAINIMP;
}
void ScenarioEditor::TerrainSwitch(aui_Control *control, uint32 action, uint32 data, void *cookie)
{
	Assert(s_scenarioEditor);
	if(!s_scenarioEditor)
		return;

	if(action == 0) {
		
		return;
	}

	if (action == AUI_SWITCH_ACTION_PRESS)
		DisableErase();

	sint32 ter = (sint32)cookie;
	if(s_scenarioEditor->m_terrainSwitches[ter]->GetState() == 0) {
		s_scenarioEditor->m_paintTerrain = -1;
		if(s_scenarioEditor->m_mapMode == SCEN_MAP_TERRAIN) {
			s_scenarioEditor->m_mapMode = SCEN_MAP_NONE;
		}
		
		return;
	}

	for(sint32 i = 0; i < g_theTerrainDB->NumRecords(); i++) {
		if(i == ter)
			continue;
		if(s_scenarioEditor->m_terrainSwitches[i]) {
			s_scenarioEditor->m_terrainSwitches[i]->SetState(0);
		}
	}

	s_scenarioEditor->m_paintTerrain = ter;
	s_scenarioEditor->m_mapMode = SCEN_MAP_TERRAIN;
}

void ScenarioEditor::ExcludeSwitch(aui_Control *control, uint32 action, uint32 data, void *cookie)
{
	if(( action != AUI_SWITCH_ACTION_ON) && (action != AUI_SWITCH_ACTION_OFF) )
		return;
	BOOL isExcluded = g_exclusions->IsUnitExcluded((sint32)cookie);
	g_exclusions->ExcludeUnit((sint32)cookie, !isExcluded);
}
void ScenarioEditor::UnitSwitch(aui_Control *control, uint32 action, uint32 data, void *cookie)
{
	if(action == AUI_SWITCH_ACTION_OFF) {
		if(s_scenarioEditor && s_scenarioEditor->m_unitIndex == (sint32)cookie) {
			s_scenarioEditor->m_unitIndex = -1;
			if(s_scenarioEditor->m_mapMode == SCEN_MAP_UNIT) {
				s_scenarioEditor->m_mapMode = SCEN_MAP_NONE;
			}
		}
		return;
	}

	
	if (action == AUI_SWITCH_ACTION_PRESS)
		DisableErase();


	if(action != AUI_SWITCH_ACTION_ON)
		return;

	
	ctp2_Switch *me = (ctp2_Switch *)control;
	
	if(me->GetState() == 0) {
		
		return;
	}

	ctp2_ListBox *lb = (ctp2_ListBox *)aui_Ldl::GetObject(s_scenarioEditorBlock, "TabGroup.Unit.List");
	Assert(lb);
	if(!lb) return;

	
	for (sint32 itemIndex = 0; itemIndex < lb->NumItems(); itemIndex++) {
		ctp2_ListItem *item = (ctp2_ListItem *)lb->GetItemByIndex(itemIndex);
		Assert(item);
		if(!item)
			continue;
		
		ctp2_Static *box = (ctp2_Static *)item->GetChildByIndex(0);
		Assert(box);
		if(!box)
			continue;

		for (sint32 i = 0; i < k_UNIT_COLS_PER_ROW; i++) {
			ctp2_Switch *sw = (ctp2_Switch *)box->GetChildByIndex(i * 2);
			if(!sw || sw->IsHidden())
				break;

			if(sw == me)
				continue;

			sw->SetState(0);
		}
	}
	Assert(s_scenarioEditor);
	if (s_scenarioEditor) 
    {
		s_scenarioEditor->m_unitIndex   = (sint32) cookie;
		s_scenarioEditor->m_mapMode     = SCEN_MAP_UNIT;
	}
}

void ScenarioEditor::CityStyleSwitch(aui_Control *control, uint32 action, uint32 data, void *cookie)
{
	if(action == AUI_SWITCH_ACTION_OFF) {
		if (s_scenarioEditor && s_scenarioEditor->m_cityStyle == (sint32)cookie) 
        {
			s_scenarioEditor->m_cityStyle = CITY_STYLE_EDITOR;
			if(s_scenarioEditor->m_mapMode == SCEN_MAP_CITY) {
				s_scenarioEditor->m_mapMode = SCEN_MAP_NONE;
			}
		}
		return;
	}

	
	if (action == AUI_SWITCH_ACTION_PRESS)
		DisableErase();


	if(action != AUI_SWITCH_ACTION_ON)
		return;



	ctp2_Switch *me = (ctp2_Switch *)control;
	if(me->GetState() == 0) {
		
		return;
	}

	ctp2_ListBox *lb = (ctp2_ListBox *)aui_Ldl::GetObject(s_scenarioEditorBlock, "TabGroup.City.List");
	Assert(lb);
	if(!lb) return;

	for(sint32 itemIndex = 0; itemIndex < lb->NumItems(); itemIndex++) {
		ctp2_ListItem *item = (ctp2_ListItem *)lb->GetItemByIndex(itemIndex);
		Assert(item);
		if(!item)
			continue;
		
		ctp2_Static *box = (ctp2_Static *)item->GetChildByIndex(0);
		Assert(box);
		if(!box)
			continue;

		for (sint32 i = 0; i < k_CITY_COLS_PER_ROW; i++) {
			ctp2_Switch *sw = (ctp2_Switch *)box->GetChildByIndex(i);
			if(!sw || sw->IsHidden())
				break;

			if(sw == me)
				continue;

			sw->SetState(0);
		}
	}
	Assert(s_scenarioEditor);
	if (s_scenarioEditor) 
    {
		s_scenarioEditor->m_cityStyle   = (sint32) cookie;
		s_scenarioEditor->m_mapMode     = SCEN_MAP_CITY;
	}
}


void ScenarioEditor::SetTab(SCEN_TAB tab)
{
	for (sint32 t = 0; t < SCEN_TAB_MAX; t++) {
		MBCHAR panelName[k_MAX_NAME_LEN];
		sprintf(panelName, "%s.TabGroup.%s", s_scenarioEditorBlock, s_scenTabNames[t]);

		MBCHAR ctrlsName[k_MAX_NAME_LEN];
		sprintf(ctrlsName, "%s.%sControls", s_scenarioEditorBlock, s_scenTabNames[t]);

		MBCHAR extraName[k_MAX_NAME_LEN];
		sprintf(extraName, "%s.%sExtraControls", s_scenarioEditorBlock, s_scenTabNames[t]);

		ctp2_Static *panel = (ctp2_Static *)aui_Ldl::GetObject(panelName);
		ctp2_Static *ctrls = (ctp2_Static *)aui_Ldl::GetObject(ctrlsName);
		ctp2_Static *extra = (ctp2_Static *)aui_Ldl::GetObject(extraName);

		
		
		if(t == tab) {
			if(panel) panel->Show();
			if(ctrls) ctrls->Show();
			if(extra) extra->Show();
		} else {
			if(panel) panel->Hide();
			if(ctrls) ctrls->Hide();
			if(extra) extra->Hide();
		}
	}

	switch(tab) {
		case SCEN_TAB_UNIT:
			if(m_unitIndex >= 0) {
				m_mapMode = SCEN_MAP_UNIT;
			} else {
				m_mapMode = SCEN_MAP_NONE;
			}
			break;
		case SCEN_TAB_CIV:
			m_mapMode = SCEN_MAP_STARTFLAGS;
			UpdateCivMode();
			break;
		case SCEN_TAB_CITY:
			if(m_cityStyle >= 0) {
				m_mapMode = SCEN_MAP_CITY;
			} else {
				m_mapMode = SCEN_MAP_NONE;
			}
			break;
		case SCEN_TAB_WORLD:
			if(m_paintTerrain >= 0) {
				m_mapMode = SCEN_MAP_TERRAIN;
			} else if (m_paintTerrainImprovement >= 0) {
				m_mapMode = SCEN_MAP_TERRAINIMP;
			} else {
				m_mapMode = SCEN_MAP_NONE;
			}
			break;
		default:
			break;
	}
	for(int i=0; i<4; i++)
	{
		s_scenarioEditor->m_tabButton[i]->SetToggleState(tab == i);
	}
}

void ScenarioEditor::ResetButts()
{
}

void ScenarioEditor::TabCallback(aui_Control *control, uint32 action, uint32 data, void *cookie)
{
	if(action != AUI_SWITCH_ACTION_ON) {
		return;
	}

	Assert(s_scenarioEditor);
	if (s_scenarioEditor)
    {
	    s_scenarioEditor->SetTab((SCEN_TAB)(sint32)cookie);
    }
}

void ScenarioEditor::UnitTabButton(aui_Control *control, uint32 action, uint32 data, void *cookie)
{
	if(action != AUI_BUTTON_ACTION_EXECUTE) return;

	Assert(s_scenarioEditor);
	if(s_scenarioEditor) {
		s_scenarioEditor->PopulateUnitList((SCEN_UNIT_CAT)(sint32)cookie);
	}
}

void ScenarioEditor::CityPopSpinner(aui_Control *control, uint32 action, uint32 data, void *cookie)
{
	if(action != AUI_RANGER_ACTION_VALUECHANGE) return;

	ctp2_Spinner *spinner = (ctp2_Spinner *)control;

	if(!spinner) return;
	sint32 newPop = spinner->GetValueX();
	//Added by Martin Gühmann to make shure 
	//newly created cities have the same pop 
	//size as displayed in the CityPopSpinner
	s_scenarioEditor->m_newPopSize = newPop;
	Unit city;
	if(!g_selected_item->GetSelectedCity(city))
		return;

	if(city.PopCount() == newPop)
		return;

	city.CD()->ChangePopulation(newPop - city.PopCount());
}


void ScenarioEditor::ShowAddList(SCEN_ADD addtype)
{
	
	Assert(m_addStuffWindow);
	if(!m_addStuffWindow)
		return;

	if(UpdateAddList(addtype)) {
		g_c3ui->AddWindow(m_addStuffWindow);
		m_addStuffWindow->Show();
	}
}

bool ScenarioEditor::UpdateAddList(SCEN_ADD addtype)
{
	ctp2_ListBox *leftList = (ctp2_ListBox *)aui_Ldl::GetObject(s_scenarioAddStuffBlock, "Left");
	ctp2_ListBox *rightList = (ctp2_ListBox *)aui_Ldl::GetObject(s_scenarioAddStuffBlock, "Right");

	Assert(leftList);
	Assert(rightList);

	if(!leftList || !rightList)
		return false;

	leftList->Clear();
	rightList->Clear();
	sint32 player = g_selected_item->GetVisiblePlayer();

	Unit city;
	BOOL haveCity = g_selected_item->GetSelectedCity(city);
	sint32 i;

	switch(addtype) {
		case SCEN_ADD_BUILDINGS:
			
			if(!haveCity) {
				ctp2_Static *tb = (ctp2_Static *)aui_Ldl::GetObject(s_scenarioAddStuffBlock, "AddStuffTitle");
				tb->SetText(g_theStringDB->GetNameStr("str_ldl_SelectCity"));
				return false;
			}
			Assert(city.IsValid());
			if(!city.IsValid())
				return false;

			
			for(i = 0; i < g_theBuildingDB->NumRecords(); i++) {
				if(city.CD()->HaveImprovement(i)) {
					AddAddItem(rightList, g_theBuildingDB->Get(i)->GetNameText(), i);
				} else {
					AddAddItem(leftList, g_theBuildingDB->Get(i)->GetNameText(), i);
				}
			}
			if(haveCity) {
				ctp2_Static *tb = (ctp2_Static *)aui_Ldl::GetObject(s_scenarioAddStuffBlock, "AddStuffTitle");
				tb->SetText(city.CD()->GetName());
			}
			break;
		case SCEN_ADD_WONDERS:
			if(!haveCity){
				ctp2_Static *tb = (ctp2_Static *)aui_Ldl::GetObject(s_scenarioAddStuffBlock, "AddStuffTitle");
				tb->SetText(g_theStringDB->GetNameStr("str_ldl_SelectCity"));
				return false;
			}
			Assert(city.IsValid());
			if(!city.IsValid())
				return false;

			for(i = 0; i < g_theWonderDB->NumRecords(); i++) {
				if(city.CD()->GetBuiltWonders() & ((uint64)1 << (uint64)i)) {
					AddAddItem(rightList, g_theWonderDB->Get(i)->GetNameText(), i);
				} else {
					AddAddItem(leftList, g_theWonderDB->Get(i)->GetNameText(), i);
					
				}
			}
			if(haveCity) {
				ctp2_Static *tb = (ctp2_Static *)aui_Ldl::GetObject(s_scenarioAddStuffBlock, "AddStuffTitle");
				tb->SetText(city.CD()->GetName());
			}
			break;
		case SCEN_ADD_ADVANCES:
			Assert(player >= 0);
			Assert(player < k_MAX_PLAYERS);

			if(player < 0 || player >= k_MAX_PLAYERS)
				return false;

			if(!g_player[player])
				return false;

			for(i = 0; i < g_theAdvanceDB->NumRecords(); i++) {
				if(g_player[player]->HasAdvance(i)) {
					AddAddItem(rightList, g_theAdvanceDB->Get(i)->GetNameText(), i);
				} else {
					AddAddItem(leftList, g_theAdvanceDB->Get(i)->GetNameText(), i);
				}
			}
			Player *p = g_player[g_selected_item->GetVisiblePlayer()];
			if(p) {
				ctp2_Static *tb = (ctp2_Static *)aui_Ldl::GetObject(s_scenarioAddStuffBlock, "AddStuffTitle");			
				tb->SetText(p->m_civilisation->GetLeaderName());
			}
			break;
	}

	m_addMode = addtype;
	return true;
}

void ScenarioEditor::CloseAddStuff(aui_Control *control, uint32 action, uint32 data, void *cookie)
{
	if(action != AUI_BUTTON_ACTION_EXECUTE) return;

	Assert(s_scenarioEditor);
	if(!s_scenarioEditor)
		return;

	Assert(s_scenarioEditor->m_addStuffWindow);
	if(!s_scenarioEditor->m_addStuffWindow)
		return;

	g_c3ui->RemoveWindow(s_scenarioEditor->m_addStuffWindow->Id());
}




void ScenarioEditor::ToggleLabels(aui_Control *control, uint32 action, uint32 data, void *cookie)
{
	switch ( action ) {
		case AUI_SWITCH_ACTION_ON:
			g_showUnitLabels = TRUE;
			break;
		case AUI_SWITCH_ACTION_OFF:
			g_showUnitLabels = FALSE;
			break;
	}
}

void ScenarioEditor::CityAddBuildings(aui_Control *control, uint32 action, uint32 data, void *cookie)
{
	if(action != AUI_BUTTON_ACTION_EXECUTE) return;

	Assert(s_scenarioEditor);
	if(s_scenarioEditor) {
		s_scenarioEditor->ShowAddList(SCEN_ADD_BUILDINGS);
	}
}

void ScenarioEditor::CityAddWonders(aui_Control *control, uint32 action, uint32 data, void *cookie)
{
	if(action != AUI_BUTTON_ACTION_EXECUTE) return;

	Assert(s_scenarioEditor);
	if(s_scenarioEditor) {
		s_scenarioEditor->ShowAddList(SCEN_ADD_WONDERS);
	}
		
}

void ScenarioEditor::Exit(aui_Control *control, uint32 action, uint32 data, void *cookie)
{
	if(action != AUI_BUTTON_ACTION_EXECUTE) return;

	ScenarioEditor::Hide();
}

void ScenarioEditor::CivAddAdvances(aui_Control *control, uint32 action, uint32 data, void *cookie)
{
	if(action != AUI_BUTTON_ACTION_EXECUTE) return;
	Assert(s_scenarioEditor);
	if(s_scenarioEditor) {
		s_scenarioEditor->ShowAddList(SCEN_ADD_ADVANCES);
	}
}


void ScenarioEditor::CivCityStyleSpinner(aui_Control *control, uint32 action, uint32 data, void *cookie)
{
	if(action != AUI_RANGER_ACTION_VALUECHANGE) return;

	ctp2_Spinner *spinner = (ctp2_Spinner *)control;

	const CityStyleRecord* rec = g_theCityStyleDB->Get(spinner->GetValueX());

	if(rec){
		g_player[g_selected_item->GetVisiblePlayer()]->GetCivilisation()->AccessData()->SetCityStyle(spinner->GetValueX());
		spinner->SetText(rec->GetNameText());

		aui_TipWindow* tipWindow = (aui_TipWindow *)spinner->GetTipWindow();
		if(tipWindow)
		{
			tipWindow->SetTipText(const_cast<char*>(rec->GetNameText()));
		}
	}
}

void ScenarioEditor::CivAddRemovePlayer(aui_Control *control, uint32 action, uint32 data, void *cookie)
{
	if(action != AUI_BUTTON_ACTION_EXECUTE) return;
	Assert(s_scenarioEditor);
	if(s_scenarioEditor) {
		ctp2_Spinner *spin = (ctp2_Spinner *)aui_Ldl::GetObject(s_scenarioEditorBlock, s_playerSpinners[0]);
		sint32 pl = spin->GetValueX();
		if(pl < 1) {


			
			return;
		}
		Assert(pl < k_MAX_PLAYERS);
		if(pl >= k_MAX_PLAYERS)
			return;

		if(g_player[pl]) {
			g_player[pl]->GameOver(GAME_OVER_LOST_INEPT, -1);
			Player::RemoveDeadPlayers();

		} else {
			civilisation_CreateNewPlayer(pl, -1);
			g_selected_item->SetPlayerOnScreen(pl);
			g_selected_item->SetCurPlayer(pl);
			g_director->AddCopyVision();

			g_tiledMap->InvalidateMix();
			g_tiledMap->InvalidateMap();
			g_tiledMap->Refresh();
			g_radarMap->Update();
			g_turn->InformMessages();

		}
		s_scenarioEditor->UpdatePlayerCount();
	}
}

void ScenarioEditor::CivModeSwitch(aui_Control *control, uint32 action, uint32 data, void *cookie)
{
	Assert(s_scenarioEditor);
	if(!s_scenarioEditor)
		return;

	sint32 i;
	SCEN_START_LOC_MODE newMode = (SCEN_START_LOC_MODE)(sint32)cookie;

	if(action == AUI_SWITCH_ACTION_OFF) {
		if(s_scenarioEditor->m_mapMode == SCEN_MAP_STARTFLAGS) {
			s_scenarioEditor->m_mapMode = SCEN_MAP_NONE;
		}
		for(i = 0; i < SCEN_START_LOC_MODE_MAX; i++) {
			ctp2_Switch *sw = (ctp2_Switch *)aui_Ldl::GetObject(s_scenarioEditorBlock, s_modeSwitchNames[i]);
			Assert(sw);
			if(sw->GetState() != 0)
				return;
		}

		
		DisableErase();

		
		ctp2_Switch *sw = (ctp2_Switch *)control;
		sw->SetState(1);

		return;
	} else if(action != AUI_SWITCH_ACTION_ON) {
		return;
	}

	for(i = 0; i < SCEN_START_LOC_MODE_MAX; i++) {
		if(i == (sint32)newMode)
			continue;
		ctp2_Switch *sw = (ctp2_Switch *)aui_Ldl::GetObject(s_scenarioEditorBlock, s_modeSwitchNames[i]);
		Assert(sw);
		if(sw) {
			sw->SetState(0);
		}
	}
	s_scenarioEditor->m_mapMode = SCEN_MAP_STARTFLAGS;

	
	
	

	s_scenarioEditor->m_startLocMode = newMode;	

	s_scenarioEditor->UpdateCivMode();
}

void ScenarioEditor::UpdateCivMode()
{
	
	ctp2_Static *showSubgroup = NULL;
	ctp2_Static *hideSubgroup = NULL;
	switch(s_scenarioEditor->m_startLocMode) {
		case SCEN_START_LOC_MODE_NONE:
		case SCEN_START_LOC_MODE_PLAYER:
		case SCEN_START_LOC_MODE_PLAYER_WITH_CIV:




		case SCEN_START_LOC_MODE_CIV:

			showSubgroup = (ctp2_Static *)aui_Ldl::GetObject(s_scenarioEditorBlock, "CivControls.Nation");
			break;
		default:
			Assert(FALSE);
			break;
	}

	if(showSubgroup) showSubgroup->Show();
	if(hideSubgroup) hideSubgroup->Hide();
}

void ScenarioEditor::SetCivSwitches()
{
	for (sint32 i = 0; i < SCEN_START_LOC_MODE_MAX; i++) {
		ctp2_Switch *sw = (ctp2_Switch *)aui_Ldl::GetObject(s_scenarioEditorBlock, s_modeSwitchNames[i]);
		Assert(sw);
		if(sw) {
			if(i == m_startLocMode) {
				sw->SetState(1);
			} else {
				sw->SetState(0);
			}
		}
	}
}

void ScenarioEditor::UpdatePlayerSelect()
{
	ctp2_DropDown *players = (ctp2_DropDown *)aui_Ldl::GetObject(s_scenarioEditorBlock, "TabGroup.Civ.PlayerSelect");
	players->Clear();
	const MBCHAR *plr_choice = g_theStringDB->GetNameStr("str_player_choice");
	AddDropDownItem(players, "ScenNationItem", (MBCHAR *)plr_choice);

    char str[k_MAX_NAME_LEN];
	for (sint32 i = 1; i < k_MAX_PLAYERS; i++)
	{
		if(g_player[i])
		{
			g_player[i]->GetCivilisation()->GetCountryName(str);
			AddDropDownItem(players, "ScenNationItem", str);
		}
	}

	players->SetSelectedItem(0);
}

void ScenarioEditor::LimitPlayerChoice(aui_Control *control, uint32 action, uint32 data, void *cookie)
{
	if (action != AUI_DROPDOWN_ACTION_SELECT)
		return;

	sint32 index = ((ctp2_DropDown *)control)->GetSelectedItem();

	if (index == 0)
	{
		
		g_startingPlayer = -1;
	} else {
		
		g_startingPlayer = index;
	}


}
void ScenarioEditor::SetupNations()
{	
	ctp2_DropDown *plgroup = (ctp2_DropDown *)aui_Ldl::GetObject(s_scenarioEditorBlock, "CivControls.Nation");

	Assert(plgroup);
	if(!plgroup ) return;

	plgroup->Clear();


	plgroup->SetActionFuncAndCookie(SetPlayerNation, (void *)0);


	
	sint32 i;
	for(i = 0; i < g_theCivilisationDB->NumRecords(); i++) {
		const MBCHAR *name = g_theStringDB->GetNameStr(g_theCivilisationDB->Get(i)->GetCountryName());

		
		AddDropDownItem(plgroup, "ScenNationItem", (MBCHAR *)name);



	}

	Player *p = g_player[g_selected_item->GetVisiblePlayer()];
	if(p) {
		 
		plgroup->SetSelectedItem(p->m_civilisation->GetCivilisation()); 

		m_placeNationFlag = p->m_civilisation->GetCivilisation();

		ctp2_TextField *tf = (ctp2_TextField *)aui_Ldl::GetObject(s_scenarioEditorBlock, "CivExtraControls.LeaderField");
		Assert(tf);
		if(tf) {
			tf->SetFieldText(p->m_civilisation->GetLeaderName());
		}

	}

	
	
	ctp2_DropDown *govs = (ctp2_DropDown *)aui_Ldl::GetObject(s_scenarioEditorBlock, "TabGroup.Civ.SetGovernment");
	for (i = 0; i < g_theGovernmentDB->NumRecords(); i++)
	{
		AddDropDownItem(govs, "ScenNationItem", (MBCHAR *)g_theStringDB->GetNameStr(g_theGovernmentDB->GetName(i)) );
	}

	if (p)
	{
		
		ctp2_DropDown *govs = (ctp2_DropDown *)aui_Ldl::GetObject(s_scenarioEditorBlock, "TabGroup.Civ.SetGovernment");
		govs->SetSelectedItem(p->GetGovernmentType());
	}

	
	
	
	
	
	




	UpdatePlayerSelect();

}

void ScenarioEditor::AddLeftList(aui_Control *control, uint32 action, uint32 data, void *cookie)
{
	ScenarioEditor::AddAddButton(NULL, AUI_LISTBOX_ACTION_DOUBLECLICKSELECT, 0, NULL);	
}

void ScenarioEditor::AddRightList(aui_Control *control, uint32 action, uint32 data, void *cookie)
{
	ScenarioEditor::AddRemoveButton(NULL, AUI_LISTBOX_ACTION_DOUBLECLICKSELECT, 0, NULL);
}

void ScenarioEditor::AddAddItem(ctp2_ListBox * a_List, const MBCHAR *text, sint32 userData)
{
	ctp2_ListItem *item = (ctp2_ListItem*)aui_Ldl::BuildHierarchyFromRoot("ScenAddStuffItem");
	Assert(item);
	if(!item)
		return;
	
	ctp2_Static *textBox = (ctp2_Static *)item->GetChildByIndex(0);
	Assert(textBox);
	if(!textBox)
		return;
	
	textBox->SetText(text);
	item->SetUserData((void *)userData);
	a_List->AddItem(item);
}

			
void ScenarioEditor::AddAddButton(aui_Control *control, uint32 action, uint32 data, void *cookie)
{
	if(action != AUI_BUTTON_ACTION_EXECUTE && action != AUI_LISTBOX_ACTION_DOUBLECLICKSELECT) return;

	ctp2_ListBox *leftList = (ctp2_ListBox *)aui_Ldl::GetObject(s_scenarioAddStuffBlock, "Left");
	Assert(leftList);
	if(!leftList)
		return;

	ctp2_ListBox *rightList = (ctp2_ListBox *)aui_Ldl::GetObject(s_scenarioAddStuffBlock, "Right");
	Assert(rightList);
	if(!rightList)
		return;

	ctp2_ListItem *selItem = (ctp2_ListItem *)leftList->GetSelectedItem();
	if(!selItem)
		return;

	Assert(s_scenarioEditor);
	if(!s_scenarioEditor)
		return;

	sint32 dbindex, i;

	Unit city;
	BOOL haveCity = g_selected_item->GetSelectedCity(city);
	sint32 player = g_selected_item->GetVisiblePlayer();

	switch(s_scenarioEditor->m_addMode) {
		case SCEN_ADD_BUILDINGS:
		{
			if(!haveCity)
				return;

			dbindex = (sint32)selItem->GetUserData();
			Assert(dbindex >= 0);
			Assert(dbindex < g_theBuildingDB->NumRecords());
			if(dbindex < 0 || dbindex >= g_theBuildingDB->NumRecords())
				return;

			if(city.CD()->HaveImprovement(dbindex))
				
				return;

			city.CD()->AddImprovement(dbindex);
			leftList->RemoveItem(selItem->Id());

			AddAddItem(rightList, g_theBuildingDB->Get(dbindex)->GetNameText(), dbindex);
			break;
		}
		case SCEN_ADD_WONDERS:
		{
			if(!haveCity)
				return;

			dbindex = (sint32)selItem->GetUserData();
			Assert(dbindex >= 0);
			Assert(dbindex < g_theWonderDB->NumRecords());
			if(dbindex < 0 || dbindex >= g_theWonderDB->NumRecords())
				return;

			if(city.CD()->GetBuiltWonders() & ((uint64)1 << (uint64)dbindex))
				
				return;

			Unit oldCity;
			if(g_theWonderTracker->GetCityWithWonder(dbindex, oldCity)) {
				oldCity.CD()->DestroyWonder(dbindex);
			}
			city.CD()->SetWonders(city.CD()->GetBuiltWonders() | ((uint64)1 << (uint64)dbindex));
			wonderutil_AddBuilt(dbindex);
			g_player[city->GetOwner()]->AddWonder(dbindex, city);

			leftList->RemoveItem(selItem->Id());
			AddAddItem(rightList, g_theWonderDB->Get(dbindex)->GetNameText(), dbindex);
			break;
		}
		case SCEN_ADD_ADVANCES:
			dbindex = (sint32) selItem->GetUserData();
			Assert(dbindex >= 0);
			Assert(dbindex < g_theAdvanceDB->NumRecords());
			if(dbindex < 0 || dbindex >= g_theAdvanceDB->NumRecords())
				return;

			Assert(player >= 0);
			Assert(player < k_MAX_PLAYERS);
			if(player < 0 || player >= k_MAX_PLAYERS)
				return;

			if(g_player[player]->HasAdvance(dbindex))
				return;
			s_scenarioEditor->m_isGivingAdvances = true;
			g_player[player]->m_advances->GiveAdvancePlusPrerequisites(dbindex);
			s_scenarioEditor->m_isGivingAdvances = false;
	
			
			
			g_player[player]->m_advances->ResetCanResearch(dbindex);
			
			leftList->Clear();
			rightList->Clear();

			for(i = 0; i < g_theAdvanceDB->NumRecords(); i++) {
				if(g_player[player]->HasAdvance(i)) {
					AddAddItem(rightList, g_theAdvanceDB->Get(i)->GetNameText(), i);
				} else {
					AddAddItem(leftList, g_theAdvanceDB->Get(i)->GetNameText(), i);
				}
			}
	
	 
	
			break;
		default:
			Assert(FALSE);
			break;
	}
}

void ScenarioEditor::AddRemoveButton(aui_Control *control, uint32 action, uint32 data, void *cookie)
{
	if(action != AUI_BUTTON_ACTION_EXECUTE && action != AUI_LISTBOX_ACTION_DOUBLECLICKSELECT) return;

	ctp2_ListBox *leftList = (ctp2_ListBox *)aui_Ldl::GetObject(s_scenarioAddStuffBlock, "Left");
	Assert(leftList);
	if(!leftList)
		return;

	ctp2_ListBox *rightList = (ctp2_ListBox *)aui_Ldl::GetObject(s_scenarioAddStuffBlock, "Right");
	Assert(rightList);
	if(!rightList)
		return;

	ctp2_ListItem *selItem = (ctp2_ListItem *)rightList->GetSelectedItem();
	if(!selItem)
		return;

	Assert(s_scenarioEditor);
	if(!s_scenarioEditor)
		return;

	sint32 dbindex;

	Unit city;
	BOOL haveCity = g_selected_item->GetSelectedCity(city);

	dbindex = (sint32)selItem->GetUserData();

	sint32 player = g_selected_item->GetVisiblePlayer();

	switch(s_scenarioEditor->m_addMode) {
		case SCEN_ADD_BUILDINGS:
		{
			if(!haveCity)
				return;

			Assert(dbindex >= 0);
			Assert(dbindex < g_theBuildingDB->NumRecords());
			if(dbindex < 0 || dbindex >= g_theBuildingDB->NumRecords())
				return;

			if(!city.CD()->HaveImprovement(dbindex))
				return;
			city.CD()->DestroyImprovement(dbindex);
			rightList->RemoveItem(selItem->Id());
			AddAddItem(leftList, g_theBuildingDB->Get(dbindex)->GetNameText(), dbindex);
			break;
		}
		case SCEN_ADD_WONDERS:
			if(!haveCity)
				return;

			Assert(dbindex >= 0);
			Assert(dbindex < g_theWonderDB->NumRecords());
			if(dbindex < 0 || dbindex >= g_theWonderDB->NumRecords())
				return;

			if(!(city.CD()->GetBuiltWonders() & ((uint64)1 << (uint64)dbindex)))
				return;

			city.CD()->DestroyWonder(dbindex);
			rightList->RemoveItem(selItem->Id());
			AddAddItem(leftList, g_theWonderDB->Get(dbindex)->GetNameText(), dbindex);
			break;

		case SCEN_ADD_ADVANCES:
			Assert(dbindex >= 0);
			Assert(dbindex < g_theAdvanceDB->NumRecords());
			if(dbindex < 0 || dbindex >= g_theAdvanceDB->NumRecords())
				return;

			Assert(player >= 0);
			Assert(player < k_MAX_PLAYERS);
			if(player < 0 || player >= k_MAX_PLAYERS)
				return;

			if(!g_player[player]->HasAdvance(dbindex))
				return;
				
			g_player[player]->m_advances->TakeAdvance(dbindex);
			rightList->RemoveItem(selItem->Id());
			AddAddItem(leftList, g_theAdvanceDB->Get(dbindex)->GetNameText(), dbindex);
			break;
		default:
			Assert(FALSE);
			break;
	}			
}

void ScenarioEditor::NotifyPlayerChange()
{
	if(!s_scenarioEditor)
		return;

	sint32 player = g_selected_item->GetVisiblePlayer();
	
	sint32 i;
	for(i = 0; i < k_NUM_PLAYER_SPINNERS; i++) {
		ctp2_Spinner *spin = (ctp2_Spinner *)aui_Ldl::GetObject(s_scenarioEditorBlock, s_playerSpinners[i]);
		Assert(spin);
		if(spin) {
			spin->SetValue(player, 0);
		}
	}

	if(g_player[player]) {
		if(s_scenarioEditor->m_startLocMode != SCEN_START_LOC_MODE_CIV) {
			ctp2_DropDown *plgroup = (ctp2_DropDown *)aui_Ldl::GetObject(s_scenarioEditorBlock, "CivControls.Nation");

			
			sint32 nation = g_player[player]->m_civilisation->GetCivilisation();
			
			
			bool wasInitializing = s_scenarioEditor->m_initializing;
			s_scenarioEditor->m_initializing = true;
			
			 
			if(plgroup) plgroup->SetSelectedItem(nation);

			plgroup->SetActionFuncAndCookie(SetPlayerNation, (void *)0);
			
			s_scenarioEditor->m_initializing = wasInitializing;
		}

		ctp2_TextField *tf = (ctp2_TextField *)aui_Ldl::GetObject(s_scenarioEditorBlock, "CivExtraControls.LeaderField");
		Assert(tf);
		if(tf) {
			tf->SetFieldText(g_player[player]->m_civilisation->GetLeaderName());
		}

		ctp2_Spinner* spin = (ctp2_Spinner *)aui_Ldl::GetObject(s_scenarioEditorBlock, "CivControls.CityStyleSpinner");
		if(spin)
		{
			spin->SetValue(g_player[player]->GetCivilisation()->GetCityStyle(), 0);
		}

		
		ctp2_DropDown *govs = (ctp2_DropDown *)aui_Ldl::GetObject(s_scenarioEditorBlock, "TabGroup.Civ.SetGovernment");
		govs->SetSelectedItem(g_player[g_selected_item->GetVisiblePlayer()]->GetGovernmentType());
	}
}

void ScenarioEditor::PlayerSpinner(aui_Control *control, uint32 action, uint32 data, void *cookie)
{
	if(action != AUI_RANGER_ACTION_VALUECHANGE)
		return;

	if(!g_tiledMap)
		return;

	static bool inCallback = false;
	if(inCallback)
		return;
	inCallback = true;

	ctp2_Spinner *spin = (ctp2_Spinner *)control;
	sint32 newPlayer = spin->GetValueX();
	ctp2_TextField *tf = (ctp2_TextField *)aui_Ldl::GetObject(s_scenarioEditorBlock, "CivExtraControls.LeaderField");
		tf->SetFieldText("");

	if(g_player[newPlayer]) {

		// Added by Martin Gühmann to prevent a crash if you use the
		// Scenario Editor to select a city, change the player without deselecting
		// it, destroy this city by conquest or slic or starvation and switching back
		// to that player.
		g_selected_item->Deselect(g_selected_item->GetVisiblePlayer());

		tf->SetFieldText(g_player[newPlayer]->m_civilisation->GetLeaderName());
		g_selected_item->SetPlayerOnScreen(newPlayer);
		g_selected_item->SetCurPlayer(newPlayer);
		g_director->AddCopyVision();

		g_tiledMap->InvalidateMix();
		g_tiledMap->InvalidateMap();
		g_tiledMap->Refresh();
		g_radarMap->Update();
		g_turn->InformMessages();
		MainControlPanel::UpdateCityList();
	}
	inCallback = false;
}

void ScenarioEditor::BrushSize(aui_Control *control, uint32 action, uint32 data, void *cookie)
{
	if(action != AUI_BUTTON_ACTION_EXECUTE) return;

	Assert(s_scenarioEditor);
	if (s_scenarioEditor)
    {
    	s_scenarioEditor->m_brushSize = (sint32)cookie;
    }
}


void ScenarioEditor::LoadScenario(aui_Control *control, uint32 action, uint32 data, void *cookie)
{
	if(action != AUI_BUTTON_ACTION_EXECUTE) return;

	Assert(s_scenarioEditor);
	if(!s_scenarioEditor) return;


	
	ScenarioWindow::Display(true);
	
	
	scenarioscreen_SetExitCallback(spnewgamescreen_scenarioExitCallback);
}

void ScenarioEditor::SaveScenario(aui_Control *control, uint32 action, uint32 data, void *cookie)
{
	if(action != AUI_BUTTON_ACTION_EXECUTE) return;

	Assert(s_scenarioEditor);
	if(!s_scenarioEditor) return;

	

	

	
	if (strlen(s_scenarioEditor->m_scenarioName) <= 0) {
		SaveScenarioAs(control, action, data, cookie);
		return;
	}

	ScenarioWindow::Display(false);
}

void ScenarioEditor::LoadMap(aui_Control *control, uint32 action, uint32 data, void *cookie)
{
	if(action != AUI_BUTTON_ACTION_EXECUTE) return;

	Assert(s_scenarioEditor);
	if(!s_scenarioEditor) return;

	if(!s_scenarioEditor->m_fileDialog) {
		s_scenarioEditor->m_fileDialog = new FileDialog();
	}

	MBCHAR path[_MAX_PATH];
	g_civPaths->GetSavePath(C3SAVEDIR_MAP, path);
	s_scenarioEditor->m_fileDialog->Open(FILE_DIALOG_LOAD, FileAction, (void *)k_SCEN_FILE_LOAD_MAP, path);

	WhackScreen();
}

void ScenarioEditor::SaveMap(aui_Control *control, uint32 action, uint32 data, void *cookie)
{
	if(action != AUI_BUTTON_ACTION_EXECUTE) return;

	Assert(s_scenarioEditor);
	if(!s_scenarioEditor) return;

	if(!s_scenarioEditor->m_fileDialog) {
		s_scenarioEditor->m_fileDialog = new FileDialog();
	}
	MBCHAR path[_MAX_PATH];
	g_civPaths->GetSavePath(C3SAVEDIR_MAP, path);
	s_scenarioEditor->m_fileDialog->Open(FILE_DIALOG_SAVE, FileAction, (void *)k_SCEN_FILE_SAVE_MAP, path);
}

void ScenarioEditor::SaveScenarioAs(aui_Control *control, uint32 action, uint32 data, void *cookie)
{
	if ( action != (uint32)AUI_BUTTON_ACTION_EXECUTE ) return;

	Assert(s_scenarioEditor);
	if(!s_scenarioEditor) return;

	ScenarioWindow::Display(false);
}

void ScenarioEditor::NameTheScenarioCallback(MBCHAR *text, sint32 accepted, void *data)
{
	Assert(s_scenarioEditor);
	if(!s_scenarioEditor)
		return;

	if (accepted) {
		
		strcpy(s_scenarioEditor->m_scenarioName, text);

		
		s_wasKeepingScore = g_theGameSettings->GetKeeppScore();
		g_theGameSettings->SetKeepScore( TRUE );

		
		scenarioeditor_SetSaveOptionsFromMode();
		g_isScenario = TRUE;

		
		loadsavescreen_displayMyWindow( LSS_SAVE_SCEN );
	}


}







bool ScenarioEditor::WorldHasPlayerOrCiv(sint32 playerOrCiv, sint32 &index)
{
	if (!s_scenarioEditor)
		return false;

	if (s_scenarioEditor->m_startLocMode == SCEN_START_LOC_MODE_CIV) 
    {
		for (sint32 i = 0; i < g_theWorld->GetNumStartingPositions(); i++) 
        {
			if (playerOrCiv == g_theWorld->GetStartingPointCiv(i)) 
            {
				index = i;
				return true;
			}
		}
	} 
    else 
    {
		index = playerOrCiv - 1;
		return g_theWorld->GetNumStartingPositions() >= playerOrCiv;
	}

    return false;
}




void ScenarioEditor::PlaceFlag(MapPoint &pos)
{
	if(!s_scenarioEditor)
		return;

	sint32 playerOrCiv = g_selected_item->GetVisiblePlayer();
	if(s_scenarioEditor->m_startLocMode == SCEN_START_LOC_MODE_CIV) {
		playerOrCiv = s_scenarioEditor->m_placeNationFlag;
	}
	
	sint32 index;
	if (WorldHasPlayerOrCiv(playerOrCiv, index)) {
		
		

		
		
		
		if (s_scenarioEditor->m_startLocMode == SCEN_START_LOC_MODE_PLAYER) {
			Player *p = NULL;
			p = g_player[g_selected_item->GetVisiblePlayer()];
			if (p) {
				playerOrCiv = p->GetCivilisation()->GetCivilisation();
			}
		}
		
		g_theWorld->SetStartingPoint(index, pos, playerOrCiv);
	} else {
		
		

		if (s_scenarioEditor->m_startLocMode == SCEN_START_LOC_MODE_PLAYER) {
			
			if (playerOrCiv > g_theWorld->GetNumStartingPositions()) {
				playerOrCiv = g_theWorld->GetNumStartingPositions()+1;

				
			}

			Player * p = g_player[playerOrCiv];
			if (p) {
				playerOrCiv = p->GetCivilisation()->GetCivilisation();
			}
		}

		
		g_theWorld->AddStartingPoint(pos, playerOrCiv);
	}
}



void ScenarioEditor::GetLabel(MBCHAR *labelString, sint32 playerOrCiv)
{
	sint32 index;

	if (playerOrCiv != -1) {
		index = playerOrCiv;
	} else {
		index = g_selected_item->GetVisiblePlayer();
	}

	SCEN_START_LOC_MODE mode = s_scenarioEditor->m_startLocMode;

	if (mode == SCEN_START_LOC_MODE_PLAYER ||
		mode == SCEN_START_LOC_MODE_PLAYER_WITH_CIV) 
    {
		sprintf(labelString, "%s (%d/%d)", 
					g_theStringDB->GetNameStr("str_ldl_Player_Text"),
					index,
					g_theProfileDB->GetNPlayers()-1);
	} 
    else if (mode == SCEN_START_LOC_MODE_CIV) 
    {
		if(g_theCivilisationDB->Get(index)) {
			sprintf(labelString, "%s (%d/%d)",
					g_theStringDB->GetNameStr(g_theCivilisationDB->Get(index)->GetPluralCivName()),
					index,
					g_theCivilisationDB->NumRecords()-1);
		} else {
			sprintf(labelString, "???");
		}
	}
}

void ScenarioEditor::RegionButton(aui_Control *control, uint32 action, uint32 data, void *cookie)
{
	if(action != AUI_BUTTON_ACTION_EXECUTE) return;

	Assert(s_scenarioEditor);
	if(!s_scenarioEditor) return;

	
	DisableErase();

	s_scenarioEditor->m_mapMode = SCEN_MAP_SELECT;
	s_scenarioEditor->m_haveRegion = false;
}

bool ScenarioEditor::DrawRegion()
{
	return s_scenarioEditor && s_scenarioEditor->m_haveRegion;
}

bool ScenarioEditor::SelectRegion()
{
	return s_scenarioEditor && (s_scenarioEditor->m_mapMode == SCEN_MAP_SELECT);
}

void ScenarioEditor::StartRegion(MapPoint &pos)
{
	if(!s_scenarioEditor) return;
	s_scenarioEditor->m_regionStart = pos;
	s_scenarioEditor->m_regionWidth = 1;
	s_scenarioEditor->m_regionHeight = 1;
	s_scenarioEditor->m_haveRegion = true;
}

void ScenarioEditor::EndRegion(MapPoint &pos)
{
	if (s_scenarioEditor)
    {
	    ExpandRegion(pos);
	    s_scenarioEditor->m_haveRegion = true;
    }
}

void ScenarioEditor::ExpandRegion(MapPoint &pos)
{
	if (s_scenarioEditor)
    {
	    s_scenarioEditor->m_regionHeight    = 
            pos.y - s_scenarioEditor->m_regionStart.y + 1;
	    s_scenarioEditor->m_regionWidth     = 
            pos.x - s_scenarioEditor->m_regionStart.x + 
		        (s_scenarioEditor->m_regionHeight / 2) + 1;
    }
}

MapPoint ScenarioEditor::GetRegionUpperLeft()
{
	if(!s_scenarioEditor) return MapPoint(0,0);
	return s_scenarioEditor->m_regionStart;
}

sint32 ScenarioEditor::GetRegionWidth()
{
	if(!s_scenarioEditor) return 0;
	return s_scenarioEditor->m_regionWidth;
}

sint32 ScenarioEditor::GetRegionHeight()
{
	if(!s_scenarioEditor) return 0;
	return s_scenarioEditor->m_regionHeight;
}

void ScenarioEditor::Copy()
{
	if(m_regionWidth <= 0 || m_regionHeight <= 0)
		return;

	if(!m_copyBuffer)
		m_copyBuffer = new MapCopyBuffer;

	m_copyBuffer->Copy(m_regionStart, m_regionWidth, m_regionHeight);
}

void ScenarioEditor::Cut()
{
	if(m_regionWidth <= 0 || m_regionHeight <= 0)
		return;

	Copy();

	// x and y are orthogonal coordinates now
	for (sint32 y = 0; y < m_regionHeight; ++y)
	{
		for (sint32 x = (y & 1); x < (2 * m_regionWidth); x += 2)
		{
			OrthogonalPoint	cur(m_regionStart);
			cur.Move(MapPointData((sint16) x, (sint16) y));
			if (cur.IsValid())
			{
				g_theWorld->SmartSetTerrain(cur.GetRC(), TERRAIN_WATER_SHALLOW, 0);
			}
		}
	}
}

void ScenarioEditor::CutRegion(aui_Control *control, uint32 action, uint32 data, void *cookie)
{
	if(action != AUI_BUTTON_ACTION_EXECUTE) return;
	Assert(s_scenarioEditor);
	if(!s_scenarioEditor) return;

	s_scenarioEditor->Cut();
	s_scenarioEditor->m_haveRegion = false;
	if(s_scenarioEditor->m_mapMode == SCEN_MAP_SELECT) {
		s_scenarioEditor->m_mapMode = SCEN_MAP_NONE;
	}
}

void ScenarioEditor::CopyRegion(aui_Control *control, uint32 action, uint32 data, void *cookie)
{
	if(action != AUI_BUTTON_ACTION_EXECUTE) return;
	Assert(s_scenarioEditor);
	if(!s_scenarioEditor) return;

	s_scenarioEditor->Copy();
	s_scenarioEditor->m_haveRegion = false;	
	if(s_scenarioEditor->m_mapMode == SCEN_MAP_SELECT) {
		s_scenarioEditor->m_mapMode = SCEN_MAP_NONE;
	}
}

void ScenarioEditor::Paste(MapPoint &pos)
{
	if(!s_scenarioEditor) return;
	if(!s_scenarioEditor->m_copyBuffer) return;

	
	DisableErase();

	s_scenarioEditor->m_copyBuffer->Paste(pos);
	if(s_scenarioEditor->m_mapMode == SCEN_MAP_PASTE) {
		s_scenarioEditor->m_mapMode = SCEN_MAP_NONE;
	}
}

void ScenarioEditor::PasteRegion(aui_Control *control, uint32 action, uint32 data, void *cookie)
{
	if(action != AUI_BUTTON_ACTION_EXECUTE) return;
	Assert(s_scenarioEditor);
	if(!s_scenarioEditor) return;


	
	DisableErase();

	s_scenarioEditor->m_mapMode = SCEN_MAP_PASTE;
	if(s_scenarioEditor->m_mapMode == SCEN_MAP_SELECT) {
		s_scenarioEditor->m_mapMode = SCEN_MAP_NONE;
	}
}

void ScenarioEditor::SaveClip(aui_Control *control, uint32 action, uint32 data, void *cookie)
{
	if(action != AUI_BUTTON_ACTION_EXECUTE) return;
	Assert(s_scenarioEditor);
	if(!s_scenarioEditor) return;

	if(!s_scenarioEditor->m_copyBuffer) return;

	if(!s_scenarioEditor->m_fileDialog) {
		s_scenarioEditor->m_fileDialog = new FileDialog();
	}

	MBCHAR path[_MAX_PATH];
	g_civPaths->GetSavePath(C3SAVEDIR_CLIPS, path);
	s_scenarioEditor->m_fileDialog->Open(FILE_DIALOG_SAVE, FileAction, (void *)k_SCEN_FILE_SAVE_CLIP, path);
}

void ScenarioEditor::LoadClip(aui_Control *control, uint32 action, uint32 data, void *cookie)
{
	if(action != AUI_BUTTON_ACTION_EXECUTE) return;
	Assert(s_scenarioEditor);
	if(!s_scenarioEditor) return;

	if(!s_scenarioEditor->m_fileDialog) {
		s_scenarioEditor->m_fileDialog = new FileDialog();
	}

	MBCHAR path[_MAX_PATH];
	g_civPaths->GetSavePath(C3SAVEDIR_CLIPS, path);
	s_scenarioEditor->m_fileDialog->Open(FILE_DIALOG_LOAD, FileAction, (void *)k_SCEN_FILE_LOAD_CLIP, path);
}

void ScenarioEditor::FileAction(FileDialog *dialog, uint32 action, const MBCHAR *filePath, void *cookie)
{
	Assert(s_scenarioEditor);
	if(!s_scenarioEditor) return;

	Assert(dialog == s_scenarioEditor->m_fileDialog);
	uint32 mode = (uint32)cookie;

	if(action == k_FILE_DIALOG_CANCEL) return;

	switch(mode) {
		case k_SCEN_FILE_LOAD_MAP:
		{
			MapFile mf;
			if(mf.Load(filePath)) {
				g_tiledMap->PostProcessMap(TRUE);
				g_tiledMap->Refresh();

				
				
				ctp2_DropDown *plgroup = (ctp2_DropDown *)aui_Ldl::GetObject(s_scenarioEditorBlock, "CivControls.Nation");


				Player *p = g_player[g_selected_item->GetVisiblePlayer()];
				
				
				MBCHAR leaderName[k_MAX_NAME_LEN];
				strcpy(leaderName, p->GetLeaderName());
				 
				plgroup->SetSelectedItem(p->m_civilisation->GetCivilisation());		


				
				p->m_civilisation->AccessData()->SetLeaderName(leaderName);

				if(g_selected_item->GetVisiblePlayer() == g_theProfileDB->GetPlayerIndex()) {
					g_theProfileDB->SetLeaderName(leaderName);
				}

				ctp2_TextField *tf = (ctp2_TextField *)aui_Ldl::GetObject(s_scenarioEditorBlock, "CivExtraControls.LeaderField");
				Assert(tf);
				if(tf) {
					tf->SetFieldText(p->m_civilisation->GetLeaderName());
				}
			}
			break;
		}
		case k_SCEN_FILE_SAVE_MAP:
		{
			MapFile mf;
			mf.Save(filePath);
			break;
		}
		case k_SCEN_FILE_LOAD_CLIP:
		{
			if(!s_scenarioEditor->m_copyBuffer) {
				s_scenarioEditor->m_copyBuffer = new MapCopyBuffer();
			}

			s_scenarioEditor->m_copyBuffer->Load(filePath);
			break;
		}
		case k_SCEN_FILE_SAVE_CLIP:
		{
			Assert(s_scenarioEditor->m_copyBuffer);
			if(!s_scenarioEditor->m_copyBuffer) {
				return;
			}
			s_scenarioEditor->m_copyBuffer->Save(filePath);
			break;
		}
		default:
			Assert(FALSE);
			break;
	}
}

void ScenarioEditor::SetupGlobalControls()
{
	bool initstate=m_initializing;
	m_initializing=true;

	ctp2_DropDown * dd = (ctp2_DropDown *)
        aui_Ldl::GetObject(s_scenarioEditorBlock, "Globals.MapSize");

	AUI_ERRCODE err = AUI_ERRCODE_OK;
	aui_StringTable *table = new aui_StringTable(&err, "SPMapSizeStringTable");

	Assert(err == AUI_ERRCODE_OK);
	Assert(dd);

    if(dd && (err == AUI_ERRCODE_OK)) {
	    dd->Clear();

		for (sint32 i = MAPSIZE_SMALL; i <= MAPSIZE_GIGANTIC; i++) 
        {
			AddDropDownItem(dd, "ScenMapSizeItem", table->GetString(i));
			dd->SetSelectedItem(g_theProfileDB->GetMapSize());
		}
	}

	delete table;

	dd = (ctp2_DropDown *)aui_Ldl::GetObject(s_scenarioEditorBlock, "Globals.Barbarians");
	Assert(dd);
	table = new aui_StringTable(&err, "SPRiskLevelStringTable");
	if (dd) 
    {
	    dd->Clear();
		for (sint32 i = 0; i < g_theRiskDB->NumRecords(); i++) {
			// Should be taken from the string database directly
			AddDropDownItem(dd, "ScenBarbarianItem", table->GetString(i));	
		}
		dd->SetSelectedItem(g_theProfileDB->GetRiskLevel());
	}

	delete table;

	dd = (ctp2_DropDown *)aui_Ldl::GetObject(s_scenarioEditorBlock, "Globals.Difficulty");
	Assert(dd);
	table = new aui_StringTable(&err, "SPNewGameStrings");
	Assert(err == AUI_ERRCODE_OK);
	if (dd && (err == AUI_ERRCODE_OK)) 
    {
	    dd->Clear();
		for (sint32 i = 0; i < g_theDifficultyDB->NumRecords(); i++) {
			AddDropDownItem(dd, "ScenDifficultyItem", g_theDifficultyDB->Get(i)->GetNameText());
		}
		dd->SetSelectedItem(g_theProfileDB->GetDifficulty());
	}

	delete table;

	ctp2_Switch *pollSwitch = (ctp2_Switch *)aui_Ldl::GetObject(s_scenarioEditorBlock, "TabGroup.World.Pollution");
	Assert(pollSwitch);
	if(pollSwitch) {
		pollSwitch->SetState(g_theProfileDB->IsPollutionRule() ? 1 : 0);
	}

	
	
	table = new aui_StringTable(&err, "WorldControlsStringTable");

	ctp2_Button * button;
	button = (ctp2_Button *)aui_Ldl::GetObject(s_scenarioEditorBlock, "WorldControls.XWrapButton");
	((aui_TipWindow *)button->GetTipWindow())->SetTipText( table->GetString(0) );
	button = (ctp2_Button *)aui_Ldl::GetObject(s_scenarioEditorBlock, "WorldControls.YWrapButton");
	((aui_TipWindow *)button->GetTipWindow())->SetTipText( table->GetString(1) );

	button = (ctp2_Button *)aui_Ldl::GetObject(s_scenarioEditorBlock, "WorldControls.CutButton");
	((aui_TipWindow *)button->GetTipWindow())->SetTipText( table->GetString(2) );
	button = (ctp2_Button *)aui_Ldl::GetObject(s_scenarioEditorBlock, "WorldControls.CopyButton");
	((aui_TipWindow *)button->GetTipWindow())->SetTipText( table->GetString(3) );
	button = (ctp2_Button *)aui_Ldl::GetObject(s_scenarioEditorBlock, "WorldControls.PasteButton");
	((aui_TipWindow *)button->GetTipWindow())->SetTipText( table->GetString(4) );

	button = (ctp2_Button *)aui_Ldl::GetObject(s_scenarioEditorBlock, "WorldControls.SmallBrushButton");
	((aui_TipWindow *)button->GetTipWindow())->SetTipText( table->GetString(5) );
	button = (ctp2_Button *)aui_Ldl::GetObject(s_scenarioEditorBlock, "WorldControls.MedBrushButton");
	((aui_TipWindow *)button->GetTipWindow())->SetTipText( table->GetString(6) );
	button = (ctp2_Button *)aui_Ldl::GetObject(s_scenarioEditorBlock, "WorldControls.LargeBrushButton");
	((aui_TipWindow *)button->GetTipWindow())->SetTipText( table->GetString(7) );

	button = (ctp2_Button *)aui_Ldl::GetObject(s_scenarioEditorBlock, "WorldControls.SaveClipButton");
	((aui_TipWindow *)button->GetTipWindow())->SetTipText( table->GetString(8) );
	button = (ctp2_Button *)aui_Ldl::GetObject(s_scenarioEditorBlock, "WorldControls.LoadClipButton");
	((aui_TipWindow *)button->GetTipWindow())->SetTipText( table->GetString(9) );


	button = (ctp2_Button *)aui_Ldl::GetObject(s_scenarioEditorBlock, "WorldControls.ExploreButton");
	((aui_TipWindow *)button->GetTipWindow())->SetTipText( table->GetString(10) );
	button = (ctp2_Button *)aui_Ldl::GetObject(s_scenarioEditorBlock, "WorldControls.UnexploreButton");
	((aui_TipWindow *)button->GetTipWindow())->SetTipText( table->GetString(11) );
	button = (ctp2_Button *)aui_Ldl::GetObject(s_scenarioEditorBlock, "UniversalControls.FogButton");
	((aui_TipWindow *)button->GetTipWindow())->SetTipText( table->GetString(12) );

	delete table;

	ctp2_Static *st = (ctp2_Static *)aui_Ldl::GetObject(s_scenarioEditorBlock, "Globals.YearDisplay");
	st->SetText(TurnYearStatus::GetCurrentYear());

	UpdatePlayerCount();

	m_initializing=initstate;
}

void ScenarioEditor::UpdatePlayerCount()
{

	ctp2_Static *st = (ctp2_Static *)aui_Ldl::GetObject(s_scenarioEditorBlock, "Globals.Players");
	MBCHAR tempstr[_MAX_PATH];
	
	sprintf(tempstr, "%d", GetNumPlayers()); 
	st->SetText(tempstr);
}

//Added by Martin Gühmann

//----------------------------------------------------------------------------
//
// Name       : ScenarioEditor::Pollution
//
// Description: Handles calls when the pollution on/off button is pressed.
//
// Parameters : control - The controll element on that an action was
//                        performed.
//              action  - The action type that was used.
//              data    - Unused
//              cookie  - Unused
//
// Globals    : -
//
// Returns    : -
//
// Remark(s)  : Only enables disables pulltion if  
//              action == AUI_BUTTON_ACTION_PRESS.
//
//----------------------------------------------------------------------------
void ScenarioEditor::Pollution(aui_Control *control, uint32 action, uint32 data, void *cookie)
{
	//No idea why the above action is called more than once
	//but this action is only called when the button is actually
	//pressed. The result now is that pollution is not turned off
	//automaticly when the Scenario Editor is used.
	if(action != AUI_BUTTON_ACTION_PRESS)
		return;

	ctp2_Button *sw = (ctp2_Button *)control;

	sw->SetToggleState(!sw->GetToggleState());
	//This makes sure that also on the first press on that button the
	//whole think work.
	g_theProfileDB->SetPollutionRule(!g_theProfileDB->IsPollutionRule());
}

AUI_ACTION_BASIC(ReopenEditorAction);

void ReopenEditorAction::Execute(aui_Control *control, uint32 action, uint32 data )
{
	ScenarioEditor::Display();
}

AUI_ACTION_BASIC(PostReopenEditorActionAction);

void PostReopenEditorActionAction::Execute(aui_Control *control, uint32 action, uint32 data )
{
	g_c3ui->AddAction(new ReopenEditorAction);
}

void ScenarioEditor::MapSize(aui_Control *control, uint32 action, uint32 data, void *cookie)
{
	if(action != AUI_DROPDOWN_ACTION_SELECT) return;

	if(!s_scenarioEditor || s_scenarioEditor->m_initializing)
		return;

	ctp2_DropDown *dd = (ctp2_DropDown *)control;
	sint32 mapSize = dd->GetSelectedItem();

	if(mapSize == g_theProfileDB->GetMapSize())
		return;

	MessageBoxDialog::Query( "str_ldl_Confirm_Restart", "ConfirmMapSizeRestart", 
		ScenarioEditor::ChangeMapSizeCallback, (void*)mapSize );








}

void ScenarioEditor::ChangeMapSizeCallback(bool response, void *userData)
{
	if(!response)
		return;

	g_theProfileDB->SetMapSize((MAPSIZE)(sint32)userData);

	g_civApp->PostRestartGameAction();

	
	g_c3ui->AddAction(new PostReopenEditorActionAction);
}

void ScenarioEditor::Year(aui_Control *control, uint32 action, uint32 data, void *cookie)
{
	if(action != AUI_RANGER_ACTION_VALUECHANGE) return;

	ctp2_Spinner *spinner = (ctp2_Spinner *)control;

	if(!spinner) return;

	PLAYER_INDEX current_player = g_selected_item->GetCurPlayer();
	Assert(g_player && g_player[current_player]);
	if(!g_player || !g_player[current_player]) return;

	sint32 newRound = spinner->GetValueX();

	for(int i=0; i<k_MAX_PLAYERS; i++)
	{
		if(g_player[i])
		{
			g_player[i]->m_current_round = newRound;
		}
	}

	ctp2_Static *st = (ctp2_Static *)aui_Ldl::GetObject(s_scenarioEditorBlock, "Globals.YearDisplay");
	st->SetText(TurnYearStatus::GetCurrentYear());

	MainControlPanel::UpdatePlayer(g_selected_item->GetVisiblePlayer());
}

void ScenarioEditor::Barbarians(aui_Control *control, uint32 action, uint32 data, void *cookie)
{
	if(action != AUI_DROPDOWN_ACTION_SELECT) return;
	if(!s_scenarioEditor || s_scenarioEditor->m_initializing)
		return;

	ctp2_DropDown *dd = (ctp2_DropDown *)control;
	g_theProfileDB->SetRiskLevel(dd->GetSelectedItem());
	
}
void ScenarioEditor::SetGovernment(aui_Control *control, uint32 action, uint32 data, void *cookie)
{
	if(action != AUI_DROPDOWN_ACTION_SELECT) return;
	if(!s_scenarioEditor || s_scenarioEditor->m_initializing)
		return;

	ctp2_DropDown *dd = (ctp2_DropDown *)control;
	g_player[g_selected_item->GetVisiblePlayer()]->ActuallySetGovernment
        (dd->GetSelectedItem());
}

void ScenarioEditor::Difficulty(aui_Control *control, uint32 action, uint32 data, void *cookie)
{
	if(action != AUI_DROPDOWN_ACTION_SELECT) return;

	if(!s_scenarioEditor || s_scenarioEditor->m_initializing)
		return;

	ctp2_DropDown *dd = (ctp2_DropDown *)control;
	g_theProfileDB->SetDifficulty(dd->GetSelectedItem());
}


void ScenarioEditor::SetXWrap(aui_Control *control, uint32 action, uint32 data, void *cookie)
{
	if(( action != AUI_SWITCH_ACTION_ON) && (action != AUI_SWITCH_ACTION_OFF) )
		return;

	s_scenarioEditor->m_xWrap = !s_scenarioEditor->m_xWrap;
	g_theWorld->SetXWrap(s_scenarioEditor->m_xWrap);
}


void ScenarioEditor::SetYWrap(aui_Control *control, uint32 action, uint32 data, void *cookie)
{
	if(( action != AUI_SWITCH_ACTION_ON) && (action != AUI_SWITCH_ACTION_OFF) )
		return;

	s_scenarioEditor->m_yWrap = !s_scenarioEditor->m_yWrap;
	g_theWorld->SetYWrap(s_scenarioEditor->m_yWrap);
}


void ScenarioEditor::SetPlayerNation(aui_Control *control, uint32 action, uint32 data, void *cookie)
{
	if(action != AUI_DROPDOWN_ACTION_SELECT) return;
	
	if(!s_scenarioEditor || s_scenarioEditor->m_initializing)
		return;

	ctp2_DropDown *dd = (ctp2_DropDown *)control;

	sint32 nation = dd->GetSelectedItem(); 

	Player *p = g_player[g_selected_item->GetVisiblePlayer()];

	switch(s_scenarioEditor->m_startLocMode) {
		case SCEN_START_LOC_MODE_NONE:
		case SCEN_START_LOC_MODE_PLAYER:
		case SCEN_START_LOC_MODE_PLAYER_WITH_CIV:
		{
			if(!p) return;
			p->m_civilisation->ResetCiv((CIV_INDEX)nation, p->m_civilisation->GetGender());
			
			ctp2_TextField *tf = (ctp2_TextField *)aui_Ldl::GetObject(s_scenarioEditorBlock, "CivExtraControls.LeaderField");
			Assert(tf);
			if(tf) {
				tf->SetFieldText(p->m_civilisation->GetLeaderName());
		}
			break;
		}
		case SCEN_START_LOC_MODE_CIV:
		{
			s_scenarioEditor->m_placeNationFlag = nation;
			break;
		}
	}
	

	
	
	s_scenarioEditor->UpdatePlayerSelect();
}

void ScenarioEditor::CityName(aui_Control *control, uint32 action, uint32 data, void *cookie)
{
	if(action != AUI_TEXTFIELD_ACTION_EXECUTE) return;

	ctp2_TextField *tf = (ctp2_TextField *)control;

	MBCHAR newName[_MAX_PATH];
	tf->GetFieldText(newName, _MAX_PATH);
	if(newName[0] != 0) {
		Unit city;
		if(g_selected_item->GetSelectedCity(city)) {
			city.CD()->SetName(newName);
		}
	}
}

void ScenarioEditor::LeaderName(aui_Control *control, uint32 action, uint32 data, void *cookie)
{
	if( (action != AUI_TEXTFIELD_ACTION_EXECUTE) 
		&& (action != 	AUI_TEXTFIELD_ACTION_DISMISS) )
		return;

	ctp2_TextField *tf = (ctp2_TextField *)control;

	MBCHAR newName[_MAX_PATH];
	tf->GetFieldText(newName, _MAX_PATH);

	if(g_player[g_selected_item->GetVisiblePlayer()]) {
		g_player[g_selected_item->GetVisiblePlayer()]->m_civilisation->AccessData()->SetLeaderName(newName);
		if(g_selected_item->GetVisiblePlayer() == g_theProfileDB->GetPlayerIndex()) {
			g_theProfileDB->SetLeaderName(newName);
		}
	}
}




void ScenarioEditor::EraseMode(aui_Control *control, uint32 action, uint32 data, void *cookie)
{
	if(action != AUI_BUTTON_ACTION_EXECUTE) return;

	
	
	

	if (PlaceUnitsMode() || PlaceCityMode() || PlaceStartFlags() || PaintHutMode() || PaintRiverMode() || PaintGoodsMode())
	{
		if (s_scenarioEditor)
		{
			s_scenarioEditor->m_mapMode = SCEN_MAP_NONE; 
		}
	}
	
	g_toeMode = !g_toeMode;
}

void ScenarioEditor::WorldTabSwitch(aui_Control *control, uint32 action, uint32 data, void *cookie)
{
	Assert(s_scenarioEditor);
	if(!s_scenarioEditor)
		return;

	if(action == 0) {
		
		return;
	}

	
	DisableErase();


	ctp2_Switch *sw = (ctp2_Switch *)control;

	if(sw->GetState() != 0) {
		if(s_scenarioEditor->m_mapMode == SCEN_MAP_TERRAIN) 
        {
			for(sint32 i = 0; i < g_theTerrainDB->NumRecords(); i++) {
				if(s_scenarioEditor->m_terrainSwitches[i]) {
					s_scenarioEditor->m_terrainSwitches[i]->SetState(0);
				}
			}
		}
		
		for (sint32 i = 0; i < k_NUM_OTHER_MAP_SWITCHES; i++) {
			if(s_scenarioEditor->m_otherMapSwitch[i] == control)
				continue;
			s_scenarioEditor->m_otherMapSwitch[i]->SetState(0);
		}
		s_scenarioEditor->m_mapMode =  (SCEN_MAP_MODE)(sint32)cookie;
	}
}	

void ScenarioEditor::ClearWorld(aui_Control *control, uint32 action, uint32 data, void *cookie)
{
	if(action != AUI_BUTTON_ACTION_EXECUTE) return;

	sint32 x, y;

	
	for(x = 0; x < g_theWorld->GetXWidth(); x++) {
		for(y = 0; y < g_theWorld->GetYHeight(); y++) {
			Cell *cell = g_theWorld->GetCell(x, y);
			if(cell->UnitArmy())
				cell->UnitArmy()->KillList(CAUSE_REMOVE_ARMY_CHEAT, -1);
			if(cell->HasCity())
				cell->GetCity().Kill(CAUSE_REMOVE_ARMY_CHEAT, -1);
			if(cell->GetGoodyHut())
				cell->DeleteGoodyHut();
		}
	}

	Unit c;

	for(x = 0; x < g_theWorld->GetXWidth(); x++) {
		for(y = 0; y < g_theWorld->GetYHeight(); y++) {
			Cell *cell = g_theWorld->GetCell(x, y);
			cell->SetEnv(0);
			cell->SetTerrain(TERRAIN_WATER_SHALLOW);
			cell->SetOwner(-1);
			cell->SetCityOwner(c);
		}
	}

	g_theWorld->NumberContinents();

	g_tiledMap->PostProcessMap();
	
	WhackScreen(); 
}

void ScenarioEditor::ExploreButton(aui_Control *control, uint32 action, uint32 data, void *cookie)
{
	if(action != AUI_BUTTON_ACTION_EXECUTE) return;

	sint32 player = g_selected_item->GetVisiblePlayer();

	if ((player > 0) && g_player[player])
    {
		g_player[player]->m_vision->SetTheWholeWorldExplored();
	}

	g_director->AddCopyVision();
	WhackScreen();

}
void ScenarioEditor::UnexploreButton(aui_Control *control, uint32 action, uint32 data, void *cookie)
{
	if(action != AUI_BUTTON_ACTION_EXECUTE) return;

	sint32 player = g_selected_item->GetVisiblePlayer();

	if(player > 0) {
		g_player[player]->m_vision->SetTheWholeWorldUnexplored();
	}

	g_director->AddCopyVision();
	WhackScreen();

}

void ScenarioEditor::FogButton(aui_Control *control, uint32 action, uint32 data, void *cookie)
{
	if(action != AUI_BUTTON_ACTION_EXECUTE) return;

	g_fog_toggle = !g_fog_toggle;
	WhackScreen(); 
}


void ScenarioEditor::AddDropDownItem(ctp2_DropDown *dd, MBCHAR *ldlblock, const char * item)
{
	ctp2_ListItem * listItem    = static_cast<ctp2_ListItem*>
        (aui_Ldl::BuildHierarchyFromRoot((MBCHAR *)ldlblock));
	ctp2_Static *   label       = static_cast<ctp2_Static*>
        (listItem->GetChildByIndex(0));

	label->SetText(item);
	dd->AddItem(listItem);
}

sint32 ScenarioEditor::GetNumPlayers() 
{
	sint32 players = 0;
	for (sint32 i = 1; i < k_MAX_PLAYERS; i++) {
		if(g_player[i])
			players++;
	}
	return players;
}

//Added by Martin Gühmann to get the last player in the game.
sint32 ScenarioEditor::GetLastPlayer() 
{
	sint32 players = 0;
	for (sint32 i = 1; i < k_MAX_PLAYERS; i++) {
		if(g_player[i])
			players = i;
	}
	return players;
}

void ScenarioEditor::DisableErase(void)
{
	g_toeMode = 0;
	s_scenarioEditor->m_eraseButton->SetState(0);
}

void ScenarioEditor::ReloadSlic(aui_Control *control, uint32 action, uint32 data, void *cookie)
{
	if(action != AUI_BUTTON_ACTION_EXECUTE) 
		return;

	for (sint32 p = 0; p < k_MAX_PLAYERS; p++) {
		if(g_player[p]) {
			g_player[p]->m_messages->KillList();
		}
	}

    if (SlicEngine::Reload(g_slic_filename))
    {
		MessageBoxDialog::Information("str_ReloadSlic_OK", "ReloadSlicOK"); 
	} 
    else 
    {
		MessageBoxDialog::Information("str_ReloadSlic_Fail", "ReloadSlicFail");
	}
}

void ScenarioEditor::RemoveGoods(aui_Control *control, uint32 action, uint32 data, void *cookie)
{
	if(action != AUI_BUTTON_ACTION_EXECUTE) return;

	for(sint32 x = 0; x < g_theWorld->GetXWidth(); x++) {
		for(sint32 y = 0; y < g_theWorld->GetYHeight(); y++) {
			g_theWorld->ClearGoods(x,y);		
		}
	}
	MessageBoxDialog::Information("str_Goods_All_Gone", "GoodsAllGone"); 
}

void ScenarioEditor::GenerateGoods(aui_Control *control, uint32 action, uint32 data, void *cookie)
{
	if(action != AUI_BUTTON_ACTION_EXECUTE) return;

	g_theWorld->GenerateGoods();

	sint32 good;
	for (sint32 x = 0; x < g_theWorld->GetXWidth(); x++) {
		for (sint32 y = 0; y < g_theWorld->GetYHeight(); y++) {
			MapPoint pos(x,y);
			if(g_theWorld->GetGood(pos, good)) { 
				g_tiledMap->PostProcessTile(pos, g_theWorld->GetTileInfo(pos));
				g_tiledMap->TileChanged(pos);
				g_tiledMap->RedrawTile(&pos);
			}
		}
	}	
	MessageBoxDialog::Information("str_Goods_Generated", "GoodsGenerated"); 
}

void ScenarioEditor::AddPW(aui_Control *control, uint32 action, uint32 data, void *cookie)
{
	if(action != AUI_TEXTFIELD_ACTION_EXECUTE) return;
	ctp2_TextField *tf = (ctp2_TextField *)control;

	MBCHAR text[MAX_CHARS];
	tf->GetFieldText(text, MAX_CHARS);

	sint32 lemurPoo = atoi(text);
	sint32 player = g_selected_item->GetVisiblePlayer();

	if(abs(lemurPoo) <= k_MAX_ADD_GOLD_OR_PW) {
		g_player[player]->m_materialPool->AddMaterials(lemurPoo);
	} else {
		tf->SetFieldText("");
		MainControlPanel::UpdatePlayer(g_selected_item->GetVisiblePlayer());
	}
	tf->SetFieldText("");
}

void ScenarioEditor::AddGold(aui_Control *control, uint32 action, uint32 data, void *cookie)
{
	if(action != AUI_TEXTFIELD_ACTION_EXECUTE) return;
	ctp2_TextField *tf = (ctp2_TextField *)control;

	sint32 player = g_selected_item->GetVisiblePlayer();

	MBCHAR text[MAX_CHARS];
	tf->GetFieldText(text, MAX_CHARS);
	sint32 furd = atoi(text);

	if(abs(furd) <= k_MAX_ADD_GOLD_OR_PW) {
		g_player[player]->m_gold->AddGold(furd);
	} else {
		tf->SetFieldText("");
		MainControlPanel::UpdatePlayer(g_selected_item->GetVisiblePlayer());
	}
	tf->SetFieldText("");
}

void ScenarioEditor::FindPosNow(aui_Control *control, uint32 action, uint32 data, void *cookie) 
{
	if(action != static_cast<uint32>(AUI_BUTTON_ACTION_EXECUTE))
		return;

	MBCHAR Xtext[MAX_CHARS];
	MBCHAR Ytext[MAX_CHARS];
	uint32 posX = 0;
	uint32 posY = 0;

	ctp2_TextField *tf = (ctp2_TextField *)aui_Ldl::GetObject(s_scenarioEditorBlock, "WorldExtraControls.FindPosXField");
	if(!tf->GetFieldText(Xtext, MAX_CHARS)) {
		return;
	} else if (atoi(Xtext) < g_theWorld->GetXWidth() && atoi(Xtext) >= 0) { 
		posX = atoi(Xtext);
	}

	tf = (ctp2_TextField *)aui_Ldl::GetObject(s_scenarioEditorBlock, "WorldExtraControls.FindPosYField");
	if(!tf->GetFieldText(Ytext, MAX_CHARS)) {
		return;
	} else if (atoi(Ytext) < g_theWorld->GetYHeight() && atoi(Ytext) >= 0) { 
		posY = atoi(Ytext);
	} 

	if(posX >= 0 && posY >= 0) {
		MapPoint pos(posX, posY);
		if(!g_fog_toggle) {
			g_fog_toggle = !g_fog_toggle;
			WhackScreen();
		}
		g_tiledMap->SetHiliteMouseTile(pos);
		g_tiledMap->DrawHilite(TRUE);
		g_director->AddCenterMap(pos);
	} else {
		tf = (ctp2_TextField *)aui_Ldl::GetObject(s_scenarioEditorBlock, "WorldExtraControls.FindPosXField");
			tf->SetFieldText("");
		tf = (ctp2_TextField *)aui_Ldl::GetObject(s_scenarioEditorBlock, "WorldExtraControls.FindPosYField");
			tf->SetFieldText("");
		return;
	}
		tf = (ctp2_TextField *)aui_Ldl::GetObject(s_scenarioEditorBlock, "WorldExtraControls.FindPosXField");
			tf->SetFieldText("");
		tf = (ctp2_TextField *)aui_Ldl::GetObject(s_scenarioEditorBlock, "WorldExtraControls.FindPosYField");
			tf->SetFieldText("");
}

bool ScenarioEditor::IsGivingAdvances()
{
	return s_scenarioEditor && s_scenarioEditor->m_isGivingAdvances;
}


// EMOD adding function for new cheat options
/*
void ScenarioEditor::ShowEnemyHealth(aui_Control *control, uint32 action, uint32 data, void *cookie)
{
	if(action != AUI_SWITCH_ACTION_PRESS) return;


	void (ProfileDB::*func)(BOOL) = 0;
	uint32 state = data;

	func = &ProfileDB::SetEnemyHealth;  //emod

	if(func)
		(g_theProfileDB->*func)(state ? FALSE : TRUE); 
}
*/

void ScenarioEditor::DebugAI(aui_Control *control, uint32 action, uint32 data, void *cookie)
    {
		
        if(g_graphicsOptions->IsArmyTextOn()){
			g_graphicsOptions->ArmyTextOff();
        }
		else
            g_graphicsOptions->ArmyTextOn();
	}

