//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ header
// Description  : City window
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
// - Added OptimizeSpecialists function for specialists optimisation option.
//   - April 7th 2005 Martin G�hmann
// - Added National Manager button and functions callback. - July 24th 2005 Martin G�hmann
//
//----------------------------------------------------------------------------

#if defined(HAVE_PRAGMA_ONCE)
#pragma once
#endif

#ifndef CITY_WINDOW_H__
#define CITY_WINDOW_H__

class ctp2_Window;
class CityData;
class aui_Control;
class ctp2_Spinner;
class ctp2_ListBox;
class ctp2_Static;
class aui_Surface;
class IconRecord;
class SlicContext;
class ctp2_HyperTextBox;
class ctp2_Button;
class aui_Image;
class aui_Region;

template <class T> class PointerList;

#include "citydata.h"

class InventoryItemInfo {
  public:
	InventoryItemInfo(bool isBuilding, sint32 type) {
		m_isBuilding = isBuilding;
		m_type = type;
	}

	bool m_isBuilding;
	sint32 m_type;
};

enum {
	CW_RES_HAPPY,
	CW_RES_FOOD,
	CW_RES_PROD,
	CW_RES_TRADE,
	CW_RES_SCI,
	CW_RES_MAX
};

enum {
	CW_PANEL_RESOURCES,
	CW_PANEL_STATISTICS,
	CW_PANEL_BUILD,
	CW_PANEL_INVENTORY,
	CW_PANEL_MAX
};

class CityWindow {
  private:
	ctp2_Window *m_window;
	ctp2_Window *m_statsWindow;
	CityData *m_cityData;
	PointerList<CityData> *m_cities;
	bool m_updating;

	ctp2_Spinner *m_popSpinners[POP_MAX];
	ctp2_Static *m_resVal[CW_RES_MAX];

	ctp2_Static *m_tabPanels[CW_PANEL_MAX];

	ctp2_ListBox *m_queueList, *m_inventoryList;
	ctp2_ListBox *m_happinessList, *m_pollutionList;

	ctp2_Button *m_rushBuyButton;
	ctp2_Button *m_sellButton;

	aui_Image *m_happyIcon, *m_unhappyIcon;

	ctp2_Static *m_growthBar;
	ctp2_Static *m_happinessBar;
	ctp2_Static *m_growthDelta;
	ctp2_Static *m_happinessValue;

	ctp2_Static *m_buildProgressBar;

	ctp2_Static *m_globalBox;
	ctp2_Static *m_globalFood, *m_globalProduction, *m_globalTrade, *m_globalScience, *m_globalPopulation;

	uint32 m_unitId[k_MAX_ARMY_SIZE];

	ctp2_Button *m_unitButtons[k_MAX_ARMY_SIZE];
	ctp2_Button *m_activateButton;
	ctp2_Button *m_disbandButton;

  public:
	CityWindow(AUI_ERRCODE *err);
	~CityWindow();

	static AUI_ERRCODE Initialize();
	static AUI_ERRCODE Cleanup();
	static AUI_ERRCODE Display(CityData *city);
	static AUI_ERRCODE Hide();

	void ClearInventoryUserData();

	static void CopyCitiesBack();

	static void Close(aui_Control *control, uint32 action, uint32 data, void *cookie);
	static void Cancel(aui_Control *control, uint32 action, uint32 data, void *cookie);
	static void NextCity(aui_Control *control, uint32 action, uint32 data, void *cookie);
	static void PreviousCity(aui_Control *control, uint32 action, uint32 data, void *cookie);
	static void Resource(aui_Control *control, uint32 action, uint32 data, void *cookie);
	static void WorkerSpinnerCallback(aui_Control *control, uint32 action, uint32 data, void *cookie);
	static void GovernorToggle(aui_Control *control, uint32 action, uint32 data, void *cookie);
	static void GovernorPriority(aui_Control *control, uint32 action, uint32 data, void *cookie);
	static void ToggleQueueInventory(aui_Control *control, uint32 action, uint32 data, void *cookie);
	static void EditQueue(aui_Control *control, uint32 action, uint32 data, void *cookie);
	static void OpenNationalManager(aui_Control *control, uint32 action, uint32 data, void *cookie);
	static void BuildListSelect(aui_Control *control, uint32 action, uint32 data, void *cookie);
	static void InventoryListSelect(aui_Control *control, uint32 action, uint32 data, void *cookie);
	static void SelectCity(aui_Control *control, uint32 action, uint32 data, void *cookie);
	static void Buy(aui_Control *control, uint32 action, uint32 data, void *cookie);
	static void Sell(aui_Control *control, uint32 action, uint32 data, void *cookie);
	static void HyperLink(aui_Control *control, uint32 action, uint32 data, void *cookie);
	static void CityList(aui_Control *control, uint32 action, uint32 data, void *cookie);
	static void ShowStatistics(aui_Control *control, uint32 action, uint32 data, void *cookie);

	static void BuildTabCallback(aui_Control *control, uint32 action, uint32 data, void *cookie);

	static void OptimizeSpecialists(aui_Control *control, uint32 action, uint32 data, void *cookie);

	static AUI_ERRCODE DrawGrowthBar(ctp2_Static *control,
									 aui_Surface *surface,
									 RECT &rect,
									 void *cookie );
	static AUI_ERRCODE DrawHappinessBar(ctp2_Static *control,
										aui_Surface *surface,
										RECT &rect,
										void *cookie );
	static AUI_ERRCODE DrawEfficiencyBar(ctp2_Static *control,
									 aui_Surface *surface,
									 RECT &rect,
									 void *cookie );
	static AUI_ERRCODE DrawResourceMap(ctp2_Static *control,
									   aui_Surface *surface,
									   RECT &rect,
									   void *cookie );
	static AUI_ERRCODE DrawBuildBar(ctp2_Static *control,
									 aui_Surface *surface,
									 RECT &rect,
									 void *cookie );

	static CityData *GetCityData(const Unit &city);
	void SetCity(CityData *city);
	void Update();
	void UpdateBuildTabs();
	void UpdateBuildTabButtons();
	void UpdateCostsGives();
	void UpdateInfoBoxes();
	void UpdateResourceMap();
	void UpdateAdviceText();
	void UpdateUnitButtons();
	void UpdateActivateButtons();

	static void NotifyUnitChange();
	static void NotifyBuildChange(CityData *city);
	static void NotifyCityCaptured(const Unit &c);

	static void Project(CityData *cityData);

	void Project();
	void SetValueBox(MBCHAR *component, sint32 val);

	static void PopulateQueueList(CityData *cd, ctp2_ListBox *lb, const char *itemBlock);

	static void SetItemDescription(const IconRecord *icon, SlicContext &sc, ctp2_Static *imageBox, ctp2_HyperTextBox *hyperTextBox, aui_Region *parent,
								   ctp2_Button *itemButton = NULL);

	void FillHappinessList();
	void FillPollutionList();
	void FillStatsLists();

	static AUI_ERRCODE DrawHappyIcons(ctp2_Static *control,
									aui_Surface *surface,
									RECT &rect,
									void *cookie );
	static AUI_ERRCODE DrawUnhappyIcons(ctp2_Static *control,
									  aui_Surface *surface,
									  RECT &rect,
									  void *cookie );

	static void DoneEditingQueue(CityData *cd);

	static void UnitButtonCallback(aui_Control *control, uint32 action, uint32 data, void *cookie);
	static void ActivateUnitCallback(aui_Control *control, uint32 action, uint32 data, void *cookie);
	static void DisbandQuery(bool result, void *ud);
	static void DisbandUnitCallback(aui_Control *control, uint32 action, uint32 data, void *cookie);
};

#endif
