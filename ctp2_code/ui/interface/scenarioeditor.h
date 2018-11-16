//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ header
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
// -Added a private static variable and the accoriding access function
//  to allow newly created cities to have the size displayed in the
//  city pop spinner, by Martin Gühmann.
// -Added GetLastPlayer() to get the last player in the game, by Martin Gühmann.
//
//----------------------------------------------------------------------------

#ifndef SCENARIO_EDITOR_H__
#define SCENARIO_EDITOR_H__

class ctp2_Window;
class ctp2_Switch;
class ctp2_ListBox;
class MapCopyBuffer;
class FileDialog;
class ctp2_Button;

class ctp2_DropDown;

#include "civscenarios.h"
#include "MapPoint.h"
#include "gstypes.h"

enum SCEN_START_LOC_MODE {
	SCEN_START_LOC_MODE_NONE,
	SCEN_START_LOC_MODE_PLAYER,
	SCEN_START_LOC_MODE_PLAYER_WITH_CIV,
	SCEN_START_LOC_MODE_CIV,

	SCEN_START_LOC_MODE_MAX
};

enum SCEN_TAB {
	SCEN_TAB_WORLD,
	SCEN_TAB_UNIT,
	SCEN_TAB_CITY,
	SCEN_TAB_CIV,
	SCEN_TAB_SCRIPT,
	SCEN_TAB_MAX
};

enum SCEN_UNIT_CAT {
	SCEN_UNIT_CAT_LAND,
	SCEN_UNIT_CAT_SEA,
	SCEN_UNIT_CAT_AIR,
	SCEN_UNIT_CAT_SPECIAL
};

enum SCEN_ADD {
	SCEN_ADD_BUILDINGS,
	SCEN_ADD_WONDERS,
	SCEN_ADD_ADVANCES
};

enum SCEN_MAP_MODE {
	SCEN_MAP_NONE,
	SCEN_MAP_STARTFLAGS,
	SCEN_MAP_SELECT,
	SCEN_MAP_PASTE,
	SCEN_MAP_UNIT,
	SCEN_MAP_CITY,
	SCEN_MAP_TERRAIN,
	SCEN_MAP_TERRAINIMP,
	SCEN_MAP_RIVER,
	SCEN_MAP_HUT,
	SCEN_MAP_GOOD1,
	SCEN_MAP_GOOD2,
	SCEN_MAP_GOOD3,
	SCEN_MAP_GOOD4,
	SCEN_MAP_MAX
};

enum TILEPAD_TYPE {
	TILEPAD_TYPE_GOODY = TERRAIN_MAX,
	TILEPAD_TYPE_RIVER,
	TILEPAD_TYPE_GOODS1,
	TILEPAD_TYPE_GOODS2,

	TILEPAD_TYPE_MAX
};

#define k_NUM_OTHER_MAP_SWITCHES 6
#define k_NUM_TAB_BUTTONS 4

class ScenarioEditor {
  private:
	ctp2_Window *m_window;
	ctp2_Switch **m_terrainSwitches;
	ctp2_Switch **m_terrainImpSwitches;
	ctp2_Window *m_addStuffWindow;
	ctp2_Switch *m_otherMapSwitch[k_NUM_OTHER_MAP_SWITCHES];

	ctp2_Switch *m_eraseButton;

	ctp2_Button *m_tabButton[k_NUM_TAB_BUTTONS];

	ctp2_Switch *m_xWrapButton;
	bool m_xWrap;
	bool m_yWrap;
	ctp2_Switch *m_yWrapButton;

	SCEN_ADD m_addMode;
	sint32 m_paintTerrain;
	sint32 m_paintTerrainImprovement;
	sint32 m_brushSize;
	sint32 m_unitIndex;
	sint32 m_cityStyle;
	//Added by Martin Gühmann to add the pop number
	//displayed in the CityPopSpinner to new created cities.
	sint32 m_newPopSize;
	SCEN_START_LOC_MODE m_startLocMode;
	bool m_haveRegion;
	SCEN_MAP_MODE m_mapMode;
	MapPoint m_regionStart;
	sint32 m_regionWidth, m_regionHeight;
	MapCopyBuffer *m_copyBuffer;
	FileDialog *m_fileDialog;
	bool m_initializing;
	sint32 m_placeNationFlag;
	bool m_isGivingAdvances;

	MBCHAR m_scenarioName[k_SCENARIO_NAME_MAX];

  public:
	ScenarioEditor(AUI_ERRCODE *err);
	~ScenarioEditor();

	static AUI_ERRCODE Initialize();
	static AUI_ERRCODE Cleanup();
	static AUI_ERRCODE Display();
	static AUI_ERRCODE Hide();
	static bool IsShown();
	static bool IsGivingAdvances();

	static void NotifySelection();
	static void NotifyPlayerChange();

	void Update();
	static void Reupdate();
	void PopulateTerrainList();
	void PopulateUnitList(SCEN_UNIT_CAT cat);
	void PopulateCityList();

	void PopulateTerrainImprovementList();

	void RehideUnitSwitches();

	static bool HandleClicks();
	static bool PaintTerrainMode();
	static bool PaintTerrainImprovementMode();
	static sint32 PaintTerrain();
	static sint32 PaintTerrainImprovement();
	static sint32 BrushSize();
	static bool PlaceUnitsMode();
	static sint32 UnitIndex();
	static bool PlaceCityMode();
	static sint32 CityStyle();
	//Function added by Martin Gühmann so that the
	//pop size displayed in the CityPopSpinner can
	//be added to newly created cities.
	static sint32 CitySize();
	static bool PlaceStartFlags();
	static bool ShowStartFlags();
	static SCEN_START_LOC_MODE GetStartLocMode();
	static bool PasteMode();
	static bool PaintHutMode();
	static bool PaintRiverMode();
	static bool PaintGoodsMode();
	static sint32 PaintGood();

	static void TerrainSwitch(aui_Control *control, uint32 action, uint32 data, void *cookie);
	static void TerrainImprovementSwitch(aui_Control *control, uint32 action, uint32 data, void *cookie);
	static void UnitSwitch(aui_Control *control, uint32 action, uint32 data, void *cookie);
	static void CityStyleSwitch(aui_Control *control, uint32 action, uint32 data, void *cookie);

	void SetTab(SCEN_TAB tab);
	static void TabCallback(aui_Control *control, uint32 action, uint32 data, void *cookie);

	static void UnitTabButton(aui_Control *control, uint32 action, uint32 data, void *cookie);

	static void ToggleLabels(aui_Control *control, uint32 action, uint32 data, void *cookie);

	static void CityPopSpinner(aui_Control *control, uint32 action, uint32 data, void *cookie);
	static void CityAddBuildings(aui_Control *control, uint32 action, uint32 data, void *cookie);
	static void CityAddWonders(aui_Control *control, uint32 action, uint32 data, void *cookie);
	static void Exit(aui_Control *control, uint32 action, uint32 data, void *cookie);

	static void CivAddAdvances(aui_Control *control, uint32 action, uint32 data, void *cookie);
	static void CivAddRemovePlayer(aui_Control *control, uint32 action, uint32 data, void *cookie);
	static void CivModeSwitch(aui_Control *control, uint32 action, uint32 data, void *cookie);
	void SetCivSwitches();
	void UpdateCivMode();
	void SetupNations();
	void UpdatePlayerSelect();

	static void PlayerSpinner(aui_Control *control, uint32 action, uint32 data, void *cookie);


	void ShowAddList(SCEN_ADD addtype);
	bool UpdateAddList(SCEN_ADD addtype);

	static void CloseAddStuff(aui_Control *control, uint32 action, uint32 data, void *cookie);
	static void AddLeftList(aui_Control *control, uint32 action, uint32 data, void *cookie);
	static void AddRightList(aui_Control *control, uint32 action, uint32 data, void *cookie);
	static void AddAddItem(ctp2_ListBox *list, const MBCHAR *text, sint32 userData);
	static void AddAddButton(aui_Control *control, uint32 action, uint32 data, void *cookie);
	static void AddRemoveButton(aui_Control *control, uint32 action, uint32 data, void *cookie);

	static void BrushSize(aui_Control *control, uint32 action, uint32 data, void *cookie);

	static void LoadScenario(aui_Control *control, uint32 action, uint32 data, void *cookie);
	static void SaveScenario(aui_Control *control, uint32 action, uint32 data, void *cookie);
	static void SaveScenarioAs(aui_Control *control, uint32 action, uint32 data, void *cookie);
	static void NameTheScenarioCallback(MBCHAR *text, sint32 accepted, void *data);
	static void LoadMap(aui_Control *control, uint32 action, uint32 data, void *cookie);
	static void SaveMap(aui_Control *control, uint32 action, uint32 data, void *cookie);

	static bool WorldHasPlayerOrCiv(sint32 playerOrCiv, sint32 &index);
	static void PlaceFlag(MapPoint &pos);
	static void GetLabel(MBCHAR *labelString, sint32 playerOrCiv);

	static void RegionButton(aui_Control *control, uint32 action, uint32 data, void *cookie);
	static bool DrawRegion();
	static bool SelectRegion();
	static void StartRegion(MapPoint &pos);
	static void EndRegion(MapPoint &pos);
	static void ExpandRegion(MapPoint &pos);
	static MapPoint GetRegionUpperLeft();
	static sint32 GetRegionWidth();
	static sint32 GetRegionHeight();
	void Copy();
	void Cut();
	static void CutRegion(aui_Control *control, uint32 action, uint32 data, void *cookie);
	static void CopyRegion(aui_Control *control, uint32 action, uint32 data, void *cookie);
	static void Paste(MapPoint &pos);
	static void PasteRegion(aui_Control *control, uint32 action, uint32 data, void *cookie);

	static void SaveClip(aui_Control *control, uint32 action, uint32 data, void *cookie);
	static void LoadClip(aui_Control *control, uint32 action, uint32 data, void *cookie);

	static void FileAction(FileDialog *dialog, uint32 action, const MBCHAR *filePath, void *cookie);

	static void Pollution(aui_Control *control, uint32 action, uint32 data, void *cookie);
	static void MapSize(aui_Control *control, uint32 action, uint32 data, void *cookie);
	static void ChangeMapSizeCallback(bool response, void *userData);
	static void Year(aui_Control *control, uint32 action, uint32 data, void *cookie);
	static void Barbarians(aui_Control *control, uint32 action, uint32 data, void *cookie);
	static void Difficulty(aui_Control *control, uint32 action, uint32 data, void *cookie);

	static void SetXWrap(aui_Control *control, uint32 action, uint32 data, void *cookie);
	static void SetYWrap(aui_Control *control, uint32 action, uint32 data, void *cookie);

	static void SetGovernment(aui_Control *control, uint32 action, uint32 data, void *cookie);
	static void SetPlayerNation(aui_Control *control, uint32 action, uint32 data, void *cookie);

	static void CityName(aui_Control *control, uint32 action, uint32 data, void *cookie);
	static void LeaderName(aui_Control *control, uint32 action, uint32 data, void *cookie);

	static void EraseMode(aui_Control *control, uint32 action, uint32 data, void *cookie);

	static void WorldTabSwitch(aui_Control *control, uint32 action, uint32 data, void *cookie);

	static void ClearWorld(aui_Control *control, uint32 action, uint32 data, void *cookie);
	static void ExploreButton(aui_Control *control, uint32 action, uint32 data, void *cookie);
	static void UnexploreButton(aui_Control *control, uint32 action, uint32 data, void *cookie);
	static void FogButton(aui_Control *control, uint32 action, uint32 data, void *cookie);

	void SetupGlobalControls();
	void UpdatePlayerCount();

	void AddDropDownItem(ctp2_DropDown *dd, MBCHAR *ldlblock, char * item);
	sint32 GetNumPlayers();
	sint32 GetLastPlayer();
	static void LimitPlayerChoice(aui_Control *control, uint32 action, uint32 data, void *cookie);
	static void ExcludeSwitch(aui_Control *control, uint32 action, uint32 data, void *cookie);

	static void DisableErase(void);
	static void ReloadSlic(aui_Control *control, uint32 action, uint32 data, void *cookie);
	static void RemoveGoods(aui_Control *control, uint32 action, uint32 data, void *cookie);
	static void GenerateGoods(aui_Control *control, uint32 action, uint32 data, void *cookie);
	static void AddPW(aui_Control *control, uint32 action, uint32 data, void *cookie);
	static void AddGold(aui_Control *control, uint32 action, uint32 data, void *cookie);
	static void ResetButts();
	static void FindPosNow(aui_Control *control, uint32 action, uint32 data, void *cookie);
};

#endif
