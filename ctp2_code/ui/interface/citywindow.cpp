//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
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
// - Disband units as an army, to get the shields for the city.
// - Start the great library with the current research project of the player.
// - Repaired memory leaks.
// - Prevent buildings occurring twice in the pollution list.
// - Unloaded icons. These were causing an exit-popup in the debug version.
// - Turn display for capitalization and infrastructure does not show
//   anymore the number of turns to completion in the list box of the build
//   manager and city manager, by Martin Gühmann.
// - Turn display on the turn button under the image button is disabled for
//   capitalization and infrastructure as well, by Martin Gühmann.
// - Rush buy costs aren't shown anymore for capitalization and
//   infrastructure, by Martin Gühmann.
// - Rush buy costs aren't shown anymore for items that aren't at the front
//   of the build queue, by Martin Gühmann.
// - Turn count on the turn count button is now updated when another item is
//   selected than the first item of the build queue, by Martin Gühmann.
// - #01 Standardization of city selection and focus handling
//   (L. Hirth 6/2004)
// - Net food and net production are now displayed insted of gross food and
//   gross production. So it is done for science and gold. This helps the
//   player better to know how much food is needed, as a negative amount is
//   displayed if the city starves. - April 6th 2005 Martin Gühmann
// - Added OptimizeSpecialists function for specialists optimisation option.
//   - April 7th 2005 Martin Gühmann
// - The Project method now updates also the sprite of the city to support
//   new turns to next pop feature, when you change the specialist
//   distribution, unfortunatly it does work as exspected.
//   - April 23rd 2005 Martin Gühmann
// - Added National Manager button and functions callback. - July 24th 2005 Martin Gühmann
// - Added preparations for city resource calculation replacement. (Aug 12th 2005 Martin Gühmann)
// - Initialized local variables. (Sep 9th 2005 Martin Gühmann)
// - Standartized code (May 21st 2006 Martin Gühmann)
// - Made appear the progress bar of the build item icon button. (Feb 4th 2007 Martin Gühmann)
// - Pressing the build item icon button opens now the build manager. (Feb 4th 2007 Martin Gühmann)
// - Replaced old const database by new one. (5-Aug-2007 Martin Gühmann)
// - Completely redesigned the window. Removed all the tabs, and added them
//	 to sections of the window that are always visible. (28-Mar-2009 Maq)
//
//----------------------------------------------------------------------------

#include "c3.h"
#include "citywindow.h"

#include "ctp2_Window.h"
#include "citydata.h"
#include "aui_ldl.h"
#include "ctp2_button.h"
#include "c3ui.h"
#include "pointerlist.h"
#include "ctp2_Static.h"
#include "player.h"
#include "SelItem.h"
#include "ctp2_spinner.h"
#include "ctp2_listbox.h"
#include "ctp2_listitem.h"
#include "ctp2_dropdown.h"
#include "ctp2_hypertextbox.h"
#include "ctp2_TabGroup.h"
#include "ctp2_Tab.h"
#include "aui_image.h"

#include "aui_blitter.h"

#include "EditQueue.h"

#include "BuildingRecord.h"
#include "BuildListSequenceRecord.h"
#include "UnitRecord.h"
#include "WonderRecord.h"
#include "IconRecord.h"
#include "CitySizeRecord.h"

#include "StrDB.h"
#include "ConstRecord.h"
#include "colorset.h"               // g_colorSet

#include "BldQue.h"
#include "Gold.h"
#include "Happy.h"
#include "PopRecord.h"				// g_thePopDB
#include "UnitData.h"

#include "SlicContext.h"
#include "stringutils.h"

#include "screenutils.h"
#include "greatlibrary.h"

#include "resourcemap.h"
extern ResourceMap                  *g_resourceMap;

#include "workwin.h"

#include "HappyTracker.h"

#include "NationalManagementDialog.h"
#include "governor.h"

#include "World.h"
#include "Cell.h"

#include "GameEventManager.h"

#include "MessageBoxDialog.h"

#include "director.h"

#include "network.h"

#include "AICause.h"	// CAUSE_NEW_ARMY_GROUPING, CAUSE_REMOVE_ARMY_GROUPING
#include <algorithm>    // std::fill
#include "ArmyPool.h"	// g_armyPool
#include "buildingutil.h"

#include "Timer.h"

extern C3UI                         *g_c3ui;

static CityWindow                   *s_cityWindow = NULL;
static MBCHAR                       *s_cityWindowBlock = "CityWindow";
static MBCHAR                       *s_cityStatsBlock = "CityStatisticsWindow";//advisor window

static sint32 s_isBuilding = 1;

CityWindow::CityWindow(AUI_ERRCODE *err)
:
    m_window                (NULL),
    m_statsWindow           (NULL),
    m_updating              (false),
    m_queueList             (NULL),
    m_inventoryList         (NULL),
    m_happinessList         (NULL),
    m_pollutionList         (NULL),
    m_rushBuyButton         (NULL),
    m_sellButton            (NULL),
    m_happyIcon             (g_c3ui->LoadImage("upic10.tga")),
    m_unhappyIcon           (g_c3ui->LoadImage("updi43.tga")),
    m_growthBar             (NULL),
    m_happinessBar          (NULL),
    m_growthDelta           (NULL),
    m_happinessValue        (NULL),
    m_conversionLossValue   (NULL),
	m_franchiseLossValue    (NULL),
	m_growthTurns	        (NULL),
    m_buildProgressBar      (NULL),
    m_globalBox             (NULL),
    m_globalFood            (NULL),
    m_globalProduction      (NULL),
    m_globalTrade           (NULL),
    m_globalScience         (NULL),
    m_globalTradeFood       (NULL),
    m_globalTradeProduction (NULL),
    m_globalTradeGold       (NULL),
    m_globalPopulation      (NULL),
    m_activateButton        (NULL),
    m_disbandButton         (NULL)
{
	std::fill(m_popSpinners, m_popSpinners + POP_MAX, (ctp2_Spinner *) NULL);
	std::fill(m_resVal, m_resVal + CW_RES_MAX, (ctp2_Static *) NULL);
	std::fill(m_tabPanels, m_tabPanels + CW_PANEL_MAX, (ctp2_Static *) NULL);
	std::fill(m_unitId, m_unitId + k_MAX_ARMY_SIZE, 0);
	std::fill(m_unitButtons, m_unitButtons + k_MAX_ARMY_SIZE, (ctp2_Button *) NULL);

	m_window = (ctp2_Window *)aui_Ldl::BuildHierarchyFromRoot(s_cityWindowBlock);
	Assert(m_window);
	if(!m_window) {
		*err = AUI_ERRCODE_INVALIDPARAM;
		return;
	}

	m_statsWindow = (ctp2_Window *)aui_Ldl::BuildHierarchyFromRoot(s_cityStatsBlock);
	Assert(m_statsWindow);
	if(!m_statsWindow) {
		*err = AUI_ERRCODE_INVALIDPARAM;
		return;
	}

	m_statsWindow->Move(m_window->X() - m_statsWindow->Width(), m_window->Y());
	m_window->AddDockedWindow(m_statsWindow);
	m_statsWindow->SetDock(m_window);

	MBCHAR buttonBlock[k_AUI_LDL_MAXBLOCK + 1];

	sprintf(buttonBlock, "%s.%s", s_cityWindowBlock, "CloseButton");
	*err = aui_Ldl::SetActionFuncAndCookie(buttonBlock, CityWindow::Close, NULL);
	Assert(*err == AUI_ERRCODE_OK);

	sprintf(buttonBlock, "%s.%s", s_cityWindowBlock, "CityList.Next");
	*err = aui_Ldl::SetActionFuncAndCookie(buttonBlock, CityWindow::NextCity, NULL);
	Assert(*err == AUI_ERRCODE_OK);

	sprintf(buttonBlock, "%s.%s", s_cityWindowBlock, "CityList.Previous");
	*err = aui_Ldl::SetActionFuncAndCookie(buttonBlock, CityWindow::PreviousCity, NULL);
	Assert(*err == AUI_ERRCODE_OK);

	*err = aui_Ldl::SetActionFuncAndCookie(s_cityWindowBlock, "GovernorBox.Toggle", CityWindow::GovernorToggle, NULL);
	Assert(*err == AUI_ERRCODE_OK);

	*err = aui_Ldl::SetActionFuncAndCookie(s_cityWindowBlock, "GovernorBox.Pulldown", CityWindow::GovernorPriority, NULL);
	Assert(*err == AUI_ERRCODE_OK);

	*err = aui_Ldl::SetActionFuncAndCookie(s_cityWindowBlock, "CityList.Pulldown", CityWindow::SelectCity, NULL);
	Assert(*err == AUI_ERRCODE_OK);

	m_popSpinners[POP_ENTERTAINER] = (ctp2_Spinner *)aui_Ldl::GetObject(s_cityWindowBlock, "Globals.EntertainersSpinner");
	m_popSpinners[POP_ENTERTAINER]->SetSpinnerCallback(CityWindow::WorkerSpinnerCallback,
													   m_popSpinners[POP_ENTERTAINER]);

	m_popSpinners[POP_SCIENTIST] = (ctp2_Spinner *)aui_Ldl::GetObject(s_cityWindowBlock, "Globals.ScientistsSpinner");
	m_popSpinners[POP_SCIENTIST]->SetSpinnerCallback(CityWindow::WorkerSpinnerCallback,
													 m_popSpinners[POP_SCIENTIST]);

	m_popSpinners[POP_FARMER] = (ctp2_Spinner *)aui_Ldl::GetObject(s_cityWindowBlock, "Globals.FarmersSpinner");
	m_popSpinners[POP_FARMER]->SetSpinnerCallback(CityWindow::WorkerSpinnerCallback,
												  m_popSpinners[POP_FARMER]);

	m_popSpinners[POP_LABORER] = (ctp2_Spinner *)aui_Ldl::GetObject(s_cityWindowBlock, "Globals.LaborersSpinner");
	m_popSpinners[POP_LABORER]->SetSpinnerCallback(CityWindow::WorkerSpinnerCallback,
												   m_popSpinners[POP_LABORER]);

	m_popSpinners[POP_MERCHANT] = (ctp2_Spinner *)aui_Ldl::GetObject(s_cityWindowBlock, "Globals.MerchantsSpinner");
	m_popSpinners[POP_MERCHANT]->SetSpinnerCallback(CityWindow::WorkerSpinnerCallback,
													m_popSpinners[POP_MERCHANT]);

	*err = aui_Ldl::SetActionFuncAndCookie(s_cityWindowBlock, "EditQueueButton", CityWindow::EditQueue, NULL);
	Assert(*err == AUI_ERRCODE_OK);

	if (aui_Ldl::GetObject(s_cityWindowBlock, "NationalManagerButton"))
	{
		*err = aui_Ldl::SetActionFuncAndCookie(s_cityWindowBlock, "NationalManagerButton", CityWindow::OpenNationalManager, NULL);
		Assert(*err == AUI_ERRCODE_OK);
	}

	if (aui_Ldl::GetObject(s_cityWindowBlock, "OptimizeSpecialistButton"))
	{
		// Added by Martin Gühmann for specialist optimization option:
		*err = aui_Ldl::SetActionFuncAndCookie(s_cityWindowBlock, "OptimizeSpecialistButton", CityWindow::OptimizeSpecialists, NULL);
		Assert(*err == AUI_ERRCODE_OK);
	}
	// else No action: this button is not guaranteed to exist in mods.

	*err = aui_Ldl::SetActionFuncAndCookie(s_cityWindowBlock, "ProductionSection.List", CityWindow::BuildListSelect, NULL);
	Assert(*err == AUI_ERRCODE_OK);

	*err = aui_Ldl::SetActionFuncAndCookie(s_cityWindowBlock, "InventorySection.List", CityWindow::InventoryListSelect, NULL);

	*err = aui_Ldl::SetActionFuncAndCookie(s_cityWindowBlock, "ProductionSection.RushBuyButton", CityWindow::Buy, NULL);
	Assert(*err == AUI_ERRCODE_OK);

	*err = aui_Ldl::SetActionFuncAndCookie(s_cityWindowBlock, "InventorySection.SellButton", CityWindow::Sell, NULL);




	m_growthBar = (ctp2_Static *)aui_Ldl::GetObject(s_cityWindowBlock, "Globals.GrowthBar");
	if(m_growthBar) {
		m_growthBar->SetDrawCallbackAndCookie(CityWindow::DrawGrowthBar, NULL);
	}

	m_growthDelta = (ctp2_Static *)aui_Ldl::GetObject(s_cityWindowBlock, "Globals.GrowthDelta");

	m_happinessBar = (ctp2_Static *)aui_Ldl::GetObject(s_cityWindowBlock, "Globals.HappinessBar");
	if(m_happinessBar) {
		m_happinessBar->SetDrawCallbackAndCookie(CityWindow::DrawHappinessBar, NULL);
	}
	m_happinessValue = (ctp2_Static *)aui_Ldl::GetObject(s_cityWindowBlock, "Globals.HappinessValue");

	m_conversionLossValue = (ctp2_Static *)aui_Ldl::GetObject(s_cityWindowBlock, "StatisticsSection.ConversionLossValue");
	m_franchiseLossValue = (ctp2_Static *)aui_Ldl::GetObject(s_cityWindowBlock, "StatisticsSection.FranchiseLossValue");

	m_growthTurns = (ctp2_Static *)aui_Ldl::GetObject(s_cityWindowBlock, "StatisticsSection.GrowthTurnsValue");

	*err = aui_Ldl::SetActionFuncAndCookie(s_cityWindowBlock, "GarrisonSection.ItemProgress.IconBorder.IconButton", CityWindow::EditQueue, NULL);
	Assert(*err == AUI_ERRCODE_OK);

	*err = aui_Ldl::SetActionFuncAndCookie(s_cityWindowBlock, "GarrisonSection.ItemProgress.IconBorder.IconButton.RadialButton", CityWindow::EditQueue, NULL);
	Assert(*err == AUI_ERRCODE_OK);

	m_buildProgressBar = (ctp2_Static *)aui_Ldl::GetObject(s_cityWindowBlock, "GarrisonSection.ItemProgress.IconBorder.IconButton.ProgressBarParent.ProgressBar");
	if(m_buildProgressBar) {
		m_buildProgressBar->SetDrawCallbackAndCookie(CityWindow::DrawBuildBar, NULL);
	}

	m_globalBox = (ctp2_Static *)aui_Ldl::GetObject(s_cityWindowBlock, "Globals");
	m_globalFood = (ctp2_Static *)aui_Ldl::GetObject(s_cityWindowBlock, "Globals.FoodValue");
	m_globalProduction = (ctp2_Static *)aui_Ldl::GetObject(s_cityWindowBlock, "Globals.ProductionValue");
	m_globalScience = (ctp2_Static *)aui_Ldl::GetObject(s_cityWindowBlock, "Globals.ScienceValue");
	m_globalTrade = (ctp2_Static *)aui_Ldl::GetObject(s_cityWindowBlock, "Globals.TradeValue");
	m_globalTradeFood = (ctp2_Static *)aui_Ldl::GetObject(s_cityWindowBlock, "Globals.FoodTradeValue");
	m_globalTradeProduction = (ctp2_Static *)aui_Ldl::GetObject(s_cityWindowBlock, "Globals.ProductionTradeValue");
	m_globalTradeGold = (ctp2_Static *)aui_Ldl::GetObject(s_cityWindowBlock, "Globals.GoldTradeValue");
	m_globalPopulation = (ctp2_Static *)aui_Ldl::GetObject(s_cityWindowBlock, "Globals.PopulationValue");

	ctp2_Static *efficiencyBar = (ctp2_Static *)aui_Ldl::GetObject(s_cityWindowBlock, "Globals.EfficiencyBar");
	if(efficiencyBar) {
		efficiencyBar->SetDrawCallbackAndCookie(CityWindow::DrawEfficiencyBar, NULL);
	}

	ctp2_Static *mapStatic = (ctp2_Static *)aui_Ldl::GetObject(s_cityWindowBlock, "Globals.ResourceMap");
	if(mapStatic) {
		mapStatic->SetDrawCallbackAndCookie(CityWindow::DrawResourceMap, NULL);
	}

	ctp2_DropDown *dd = (ctp2_DropDown *)aui_Ldl::GetObject(s_cityWindowBlock, "GovernorBox.Pulldown");
	Assert(dd);
	if(dd) {
		dd->Clear();

		dd->SetWindowSize(g_theBuildListSequenceDB->NumRecords() * 22);

		sint32 i;
		for(i = 0; i < g_theBuildListSequenceDB->NumRecords(); i++) {
			ctp2_ListItem *item = (ctp2_ListItem *)aui_Ldl::BuildHierarchyFromRoot("cw_GovernorPriorityItem");
			Assert(item);
			if(item) {
				ctp2_Static *text = (ctp2_Static *)item->GetChildByIndex(0);
				text->SetText(g_theBuildListSequenceDB->Get(i)->GetNameText());
				dd->AddItem(item);
			}
		}
	}

	m_resVal[CW_RES_HAPPY] = (ctp2_Static *)aui_Ldl::GetObject(s_cityWindowBlock, "Globals.HappyWorkerValue");
	m_resVal[CW_RES_FOOD] = (ctp2_Static *)aui_Ldl::GetObject(s_cityWindowBlock, "Globals.FoodWorkerValue");
	m_resVal[CW_RES_PROD] = (ctp2_Static *)aui_Ldl::GetObject(s_cityWindowBlock, "Globals.ProductionWorkerValue");
	m_resVal[CW_RES_TRADE] = (ctp2_Static *)aui_Ldl::GetObject(s_cityWindowBlock, "Globals.TradeWorkerValue");
	m_resVal[CW_RES_SCI] = (ctp2_Static *)aui_Ldl::GetObject(s_cityWindowBlock, "Globals.ScienceWorkerValue");

	m_tabPanels[CW_PANEL_STATISTICS] = (ctp2_Static *)aui_Ldl::GetObject(s_cityWindowBlock, "StatisticsSection");
	m_tabPanels[CW_PANEL_GARRISON] = (ctp2_Static *)aui_Ldl::GetObject(s_cityWindowBlock, "GarrisonSection");

	m_queueList = (ctp2_ListBox *)aui_Ldl::GetObject(s_cityWindowBlock, "ProductionSection.List");
	m_inventoryList = (ctp2_ListBox *)aui_Ldl::GetObject(s_cityWindowBlock, "InventorySection.List");

	m_queueList->SetForceSelect(TRUE);
	m_inventoryList->SetForceSelect(TRUE);

	m_pollutionList = (ctp2_ListBox *)aui_Ldl::GetObject(s_cityWindowBlock, "StatisticsSection.PollutionList");
	Assert(m_pollutionList);
	m_happinessList = (ctp2_ListBox *)aui_Ldl::GetObject(s_cityWindowBlock, "StatisticsSection.HappinessList");
	Assert(m_happinessList);

	m_pollutionList->SetAbsorbancy(FALSE);
	m_happinessList->SetAbsorbancy(FALSE);

	m_rushBuyButton = (ctp2_Button *)aui_Ldl::GetObject(s_cityWindowBlock, "ProductionSection.RushBuyButton");
	m_sellButton = (ctp2_Button *)aui_Ldl::GetObject(s_cityWindowBlock, "InventorySection.SellButton");

	aui_Ldl::SetActionFuncAndCookie(s_cityWindowBlock, "GarrisonSection", BuildTabCallback, (void *)CW_PANEL_GARRISON);

	*err = aui_Ldl::SetActionFuncAndCookie(s_cityWindowBlock, "ShowStatisticsButton", ShowStatistics, NULL);//advisor window button
	Assert(*err == AUI_ERRCODE_OK);

	sint32 x,y;
	sint32 unitButton = 0;
	for(y = 0; y < 3; y++) {
		for(x = 0; x < 4; x++) {
			char buf[256];
			sprintf(buf, "GarrisonSection.UnitButtons.b%c%c.IconBorder.Button", char(x + '0'), char(y + '0'));
			m_unitButtons[unitButton] = (ctp2_Button *)aui_Ldl::GetObject(s_cityWindowBlock, buf);
			Assert(m_unitButtons[unitButton]);
			if(m_unitButtons[unitButton]) {
				m_unitButtons[unitButton]->Enable(FALSE);
				m_unitButtons[unitButton]->SetActionFuncAndCookie(UnitButtonCallback, (void *)unitButton);
			}

			unitButton++;
		}
	}

	m_activateButton = (ctp2_Button *)aui_Ldl::GetObject(s_cityWindowBlock, "GarrisonSection.ActivateButton");
	aui_Ldl::SetActionFuncAndCookie(s_cityWindowBlock, "GarrisonSection.ActivateButton", ActivateUnitCallback, NULL);

	m_disbandButton = (ctp2_Button *)aui_Ldl::GetObject(s_cityWindowBlock, "GarrisonSection.DisbandButton");
	m_disbandButton->SetActionFuncAndCookie(DisbandUnitCallback, NULL);
}

//----------------------------------------------------------------------------
//
// Name       : CityWindow::~CityWindow
//
// Description: Destructor
//
// Parameters : -
//
// Globals    : -
//
// Returns    : -
//
// Remark(s)  : - Member pointer variables that have been initialised with
//                GetObject are references, and do not have to be deleted.
//              - Member pointer variables that have been initialised with
//                BuildHierarchyFromRoot have to be deleted with
//                DeleteHierarchyFromRoot.
//              - Containers have to be cleared, but setting member variables
//                (to NULL) *in a destructor* is a waste of time.
//
//----------------------------------------------------------------------------
CityWindow::~CityWindow()
{
	g_c3ui->UnloadImage(m_happyIcon);
	g_c3ui->UnloadImage(m_unhappyIcon);

	if (m_inventoryList)	// container + reference
	{
		m_inventoryList->ClearUserData CALL_TEMPLATE_FUNCTION_WITHOUT_ARGUMENT(InventoryItemInfo);
		m_inventoryList->Clear();
	}

	if (m_queueList)		// container + reference
	{
		m_queueList->Clear();
	}

	ctp2_DropDown *	const	dropdown	=
		static_cast<ctp2_DropDown *>
			(aui_Ldl::GetObject(s_cityWindowBlock, "GovernorBox.Pulldown"));
	if (dropdown)			// container + reference
	{
		dropdown->Clear();
	}

	if (m_statsWindow)		// hierarchy
	{
		aui_Ldl::DeleteHierarchyFromRoot(s_cityStatsBlock);
	}

	if (m_window)			// hierarchy
	{
		aui_Ldl::DeleteHierarchyFromRoot(s_cityWindowBlock);
	}
}

AUI_ERRCODE CityWindow::Initialize()
{

	workwin_Initialize();

	if(s_cityWindow)
		return AUI_ERRCODE_OK;

	AUI_ERRCODE err = AUI_ERRCODE_OK;
	s_cityWindow = new CityWindow(&err);

	Assert(err == AUI_ERRCODE_OK);

	return err;
}

AUI_ERRCODE CityWindow::Cleanup()
{
	if(s_cityWindow)
	{
		s_cityWindow->Hide();
	}

	delete s_cityWindow;
	s_cityWindow = NULL;
	return AUI_ERRCODE_OK;
}

AUI_ERRCODE CityWindow::Display()
{
	if(g_player[g_selected_item->GetVisiblePlayer()]->m_all_cities->Num() < 1) {
		return AUI_ERRCODE_OK;
	}

	Display(g_player[g_selected_item->GetVisiblePlayer()]->m_all_cities->Access(0));
}

AUI_ERRCODE CityWindow::Display(const Unit & city)
{
	if(g_network.IsClient() && g_network.GetSensitiveUIBlocked()) {
		return AUI_ERRCODE_OK;
	}

	if(!s_cityWindow)
		Initialize();

	Assert(s_cityWindow);

	s_cityWindow->SetCity(city);

	AUI_ERRCODE err = g_c3ui->AddWindow(s_cityWindow->m_window);
	Assert(err == AUI_ERRCODE_OK);
	if(err != AUI_ERRCODE_OK)
		return err;

	err = s_cityWindow->m_window->Show();

	if(err == AUI_ERRCODE_OK) {
		g_c3ui->BringWindowToTop(s_cityWindow->m_window);
	}
#if 0
	ctp2_Static *resourceBox = (ctp2_Static *)aui_Ldl::GetObject("CityWindow.ResourceMapBox");
	Assert(resourceBox);
	resourceBox->Hide();
#endif

	s_cityWindow->Update();

	return err;
}

AUI_ERRCODE CityWindow::Hide()
{
	if(!s_cityWindow)
		return AUI_ERRCODE_OK;

	if(!s_cityWindow->m_window)
		return AUI_ERRCODE_OK;

	if(s_cityWindow->m_statsWindow) {
		g_c3ui->RemoveWindow(s_cityWindow->m_statsWindow->Id());
	}

	return g_c3ui->RemoveWindow(s_cityWindow->m_window->Id());
}

void CityWindow::SetCity(const Unit & city)
{
	m_city = city;

	Project();
	Update();

	if (s_cityWindow->m_city.IsValid()) {
		g_selected_item->SetSelectCity(s_cityWindow->m_city);
	}
}

void CityWindow::Project(const Unit & city)
{
	if (!city.IsValid()) {
		return;
	}

	CityData * cityData = city.GetCityData();
	cityData->ProcessAllResources();
	// To update turn count to next pop on the map.
	cityData->UpdateSprite();
}

void CityWindow::Project()
{
	Project(m_city);
}

void CityWindow::Update()
{
	if(m_updating)
		return;

	m_updating = true;

	ctp2_DropDown *cityDD = (ctp2_DropDown *)aui_Ldl::GetObject("CityWindow.CityList.Pulldown");
	if(cityDD) {
		UnitDynamicArray *cityList = g_player[g_selected_item->GetVisiblePlayer()]->m_all_cities;

		cityDD->Clear();

		sint32 height = cityList->Num() * 22;
		if(height > 330)
			height = 330;

		cityDD->SetWindowSize(height);

		sint32 viewingIndex = 0;
		for(sint32 i = 0; i < cityList->Num(); i++) {
			ctp2_ListItem *item = (ctp2_ListItem *)aui_Ldl::BuildHierarchyFromRoot("cw_CityItem");
			Assert(item);
			if(item) {
				ctp2_Static *label = (ctp2_Static *)item->GetChildByIndex(0);
				label->SetText(cityList->Access(i).GetName());
				cityDD->AddItem(item);
			}

			if(cityList->Access(i) == m_city)
				viewingIndex = i;

		}


		cityDD->SetSelectedItem(viewingIndex);
	}

	CityData * cityData = m_city.GetCityData();
	ctp2_Static *text = (ctp2_Static *)aui_Ldl::GetObject("CityWindow.Globals.WorkersCount");
	char value[1024];
	if(text) {
		sprintf(value, "%d", cityData->WorkerCount());
		text->SetText(value);
	}

	text = (ctp2_Static *)aui_Ldl::GetObject("CityWindow.Globals.SlaveCount");
	if(text) {
		sprintf(value, "%d", cityData->SlaveCount());
		text->SetText(value);
	}

	text = (ctp2_Static *)aui_Ldl::GetObject("CityWindow.Globals.TotalCount");
	if(text) {
		sprintf(value, "%d", cityData->PopCount());
		text->SetText(value);
	}

	for(sint32 popType = sint32(POP_WORKER) + 1; popType < sint32(POP_SLAVE); popType++) {
		if(m_popSpinners[popType]) {
			if(cityData->GetBestSpecialist((POP_TYPE)popType) < 0) {
				m_popSpinners[popType]->Enable(FALSE);
			} else {
				m_popSpinners[popType]->Enable(TRUE);
			}
			m_popSpinners[popType]->SetValue(cityData->SpecialistCount((POP_TYPE)popType), 0);
		}
	}

	for(sint32 i = 0; i < CW_RES_MAX; i++) {

		Player *player = g_player[g_selected_item->GetVisiblePlayer()];
		sint32 amt = 0;

		if(m_resVal[i]) {
			switch(i) {
				case CW_RES_FOOD:
					if (cityData->FarmerCount() > 0)
					{
						amt = cityData->FarmerCount() *
							(g_thePopDB->Get(cityData->GetBestSpecialist(POP_FARMER),
							player->GetGovernmentType())->GetFood());
					}
					sprintf(value, "+%d", amt);
					break;
				case CW_RES_HAPPY:
					sprintf(value, "+%d", static_cast<sint32>(cityData->GetHappyPopEntertainment()));
					break;
				case CW_RES_PROD:
					if (cityData->LaborerCount() > 0)
					{
						amt = cityData->LaborerCount() *
							(g_thePopDB->Get(cityData->GetBestSpecialist(POP_LABORER),
							player->GetGovernmentType())->GetProduction());
					}
					sprintf(value, "+%d", amt);
					break;
				case CW_RES_TRADE:
					if (cityData->MerchantCount() > 0)
					{
						amt = cityData->MerchantCount() *
							(g_thePopDB->Get(cityData->GetBestSpecialist(POP_MERCHANT),
							player->GetGovernmentType())->GetCommerce());
					}
					sprintf(value, "+%d", amt);
					break;
				case CW_RES_SCI:
					if (cityData->ScientistCount() > 0)
					{
						amt = cityData->ScientistCount() *
							(g_thePopDB->Get(cityData->GetBestSpecialist(POP_SCIENTIST),
							player->GetGovernmentType())->GetScience());
					}
					sprintf(value, "+%d", amt);
					break;
			}
			m_resVal[i]->SetText(value);
		}
	}
	UpdateBuildTabs();

	ctp2_Button *button = (ctp2_Button *)aui_Ldl::GetObject(s_cityWindowBlock, "GovernorBox.Toggle");
	Assert(button);

	ctp2_DropDown *dd = (ctp2_DropDown *)aui_Ldl::GetObject(s_cityWindowBlock, "GovernorBox.Pulldown");
	Assert(dd);

	if(button) {
		if(cityData->GetUseGovernor()) {
			button->SetText("X");

			if(dd) {
				dd->SetSelectedItem(cityData->GetBuildListSequenceIndex());
				dd->Enable(TRUE);
			}
		} else {
			button->SetText("");
			if(dd) {
				dd->SetSelectedItem(-1);
				dd->Enable(FALSE);
			}
		}
	}

	if(m_buildProgressBar) {
		m_buildProgressBar->ShouldDraw(TRUE);
	}

	if(m_growthBar) {
		m_growthBar->ShouldDraw(TRUE);
	}

	char buf[20];
	if(m_growthDelta) {
		if (cityData->GetGrowthRate() >= 0) {
			sprintf(buf, "+%d", cityData->GetGrowthRate());
			m_growthDelta->SetText(buf);
			m_growthDelta->SetTextColor(g_colorSet->GetColorRef(COLOR_BLACK));
		} else {
			m_growthDelta->SetText(g_theStringDB->GetNameStr("str_ldl_Starving"));
			m_growthDelta->SetTextColor(g_colorSet->GetColorRef(COLOR_RED));
		}
	}

	if(m_happinessBar) {
		m_happinessBar->ShouldDraw(TRUE);
	}

	if(m_happinessValue) {
		sprintf(buf, "%d", static_cast<sint32>(cityData->GetHappiness()));
		m_happinessValue->SetText(buf);
	}

	const COLORREF colorNorm = RGB(50,50,50);

	if(m_conversionLossValue) {
		sint32 convertedGold = cityData->GetConvertedGold();
		sprintf(buf, "%d", convertedGold);
		m_conversionLossValue->SetText(buf);
		if (convertedGold > 0 && cityData->GetConvertedTo() > -1) {
			COLOR color = g_colorSet->ComputePlayerColor(cityData->GetConvertedTo());
			m_conversionLossValue->SetTextColor(g_colorSet->GetColorRef(color));
		} else
			m_conversionLossValue->SetTextColor(colorNorm);
	}

	if(m_franchiseLossValue) {
		sint32 franchise = cityData->GetProductionLostToFranchise();
		sprintf(buf, "%d", franchise);
		m_franchiseLossValue->SetText(buf);
		if (franchise > 0 && cityData->GetFranchiseOwner() > -1) {
			COLOR color = g_colorSet->ComputePlayerColor(cityData->GetFranchiseOwner());
			m_franchiseLossValue->SetTextColor(g_colorSet->GetColorRef(color));
		} else
			m_franchiseLossValue->SetTextColor(colorNorm);
	}

	if(m_growthTurns) {
		sprintf(buf, "%d", static_cast<sint32>(cityData->TurnsToNextPop()));
		m_growthTurns->SetText(buf);
	}

	if(m_globalFood) {
		// Display net food instead of gross food. - Martin Gühmann
		sprintf(buf, "%d", cityData->GetNetCityFood());
		m_globalFood->SetText(buf);
	}

	if(m_globalTrade) {
		sprintf(buf, "%d", cityData->GetNetCityGold());
		m_globalTrade->SetText(buf);
	}

	if(m_globalScience) {
		sprintf(buf, "%d", cityData->GetScience());
		m_globalScience->SetText(buf);
	}

	if(m_globalProduction) {
		// Display net production instead of gross production - Martin Gühmann
		sprintf(buf, "%d", cityData->GetNetCityProduction());
		m_globalProduction->SetText(buf);
	}

	if(m_globalTradeFood) {
		sprintf(buf, "%d", cityData->FoodFromTrade());
		m_globalTradeFood->SetText(buf);
	}

	if(m_globalTradeProduction) {
		sprintf(buf, "%d", cityData->ProdFromTrade());
		m_globalTradeProduction->SetText(buf);
	}

	if(m_globalTradeGold) {
		sprintf(buf, "%d", cityData->GoldFromTrade());
		m_globalTradeGold->SetText(buf);
	}

	if(m_globalPopulation) {
		sprintf(buf, "%d", cityData->PopCount() * k_PEOPLE_PER_POPULATION + cityData->GetPartialPopulation());
		m_globalPopulation->SetText(buf);
	}

	if(m_globalBox) {
		m_globalBox->ShouldDraw(TRUE);
	}

	UpdateResourceMap();

	UpdateCostsGives();

	FillStatsLists();

	if(m_tabPanels[CW_PANEL_STATISTICS]) {
		m_tabPanels[CW_PANEL_STATISTICS]->ShouldDraw(TRUE);
	}

	m_updating = false;

	UpdateAdviceText();

	UpdateUnitButtons();
}

//----------------------------------------------------------------------------
//
// Name       : CityWindow::UpdateBuildTabs
//
// Description: Updates the build tab, one thing is the update of the
//              turn count button.
//
// Parameters : -
//
// Globals    : -
//
// Returns    : -
//
// Remark(s)  : -
//
//----------------------------------------------------------------------------
void CityWindow::UpdateBuildTabs()
{

	ctp2_ListBox *lb = m_queueList;// fill production tab queue list
	Assert(lb);
	if(lb) {
		PopulateQueueList(m_city, lb, "cw_InventoryListItem");
	}

	CityData * cityData = m_city.GetCityData();
	ctp2_Button *turnCountButton = (ctp2_Button *)aui_Ldl::GetObject(s_cityWindowBlock, "GarrisonSection.ItemProgress.IconBorder.IconButton.RadialButton");
	MBCHAR buf[20];
	if(turnCountButton) {
		sint32 turns = cityData->AlreadyBoughtFront() ? -1 : cityData->HowMuchLonger();
		//Added by Martin Gühmann to disable the turn count display for capitalization and infrastructure
		if(turns >= 0 && turns < 0x7fffffff
		&& cityData->GetBuildQueue()->GetHead()->m_category != k_GAME_OBJ_TYPE_CAPITALIZATION
		&& cityData->GetBuildQueue()->GetHead()->m_category != k_GAME_OBJ_TYPE_INFRASTRUCTURE)
			sprintf(buf, "%d", turns);
		else
			strcpy(buf, "---");
		turnCountButton->SetText(buf);
	}

	lb = m_inventoryList;// fill city inventory (buildings/wonders) list
	Assert(lb);
	if(lb) {
		lb->ClearUserData CALL_TEMPLATE_FUNCTION_WITHOUT_ARGUMENT(InventoryItemInfo);
		lb->Clear();

		sint32 i;
		for(i = 0; i < g_theBuildingDB->NumRecords(); i++) {
			if(cityData->GetImprovements() & ((uint64)1 << (uint64)i)) {
				ctp2_ListItem * item = (ctp2_ListItem *) aui_Ldl::BuildHierarchyFromRoot("inv_InventoryListItem");
				Assert(item);
				if(item) {
					ctp2_Static *box = (ctp2_Static *)item->GetChildByIndex(0);
					if(box) {
						ctp2_Static *label = (ctp2_Static *)box->GetChildByIndex(0);
						label->SetText(g_theBuildingDB->Get(i)->GetNameText());

						label = (ctp2_Static *)box->GetChildByIndex(1);
						sprintf(buf, "%d", buildingutil_Get(i, cityData->GetOwner())->GetUpkeep());
						label->SetText(buf);
					}
					InventoryItemInfo *info = new InventoryItemInfo(true, i);
					item->SetUserData(info);
					lb->AddItem(item);
				}
			}
		}

		for(i = 0; i < g_theWonderDB->NumRecords(); i++) {
			if(cityData->GetBuiltWonders() & ((uint64)1 << (uint64)i)) {
				ctp2_ListItem * item = (ctp2_ListItem *)aui_Ldl::BuildHierarchyFromRoot("inv_InventoryListItem");
				Assert(item);
				if(item) {
					ctp2_Static *box = (ctp2_Static *)item->GetChildByIndex(0);
					if(box) {
						ctp2_Static *label = (ctp2_Static *)box->GetChildByIndex(0);
						label->SetText(g_theWonderDB->Get(i)->GetNameText());

						label = (ctp2_Static *)box->GetChildByIndex(1);

						if(label) {
							label->SetText("");
						}
					}
					InventoryItemInfo *info = new InventoryItemInfo(false, i);
					item->SetUserData(info);
					lb->AddItem(item);
				}
			}
		}
	}

	m_tabPanels[CW_PANEL_GARRISON]->ShouldDraw(TRUE);

	UpdateBuildTabButtons();
}

//----------------------------------------------------------------------------
//
// Name       : CityWindow::UpdateBuildTabButtons
//
// Description: Enables disables the rush buy button and the sell
//              building button.
//
// Parameters : -
//
// Globals    : -
//
// Returns    : -
//
// Remark(s)  : -
//
//----------------------------------------------------------------------------
void CityWindow::UpdateBuildTabButtons()
{
	CityData * cityData = m_city.GetCityData();
	sint32 const	visiblePlayer	= g_selected_item->GetVisiblePlayer();
	sint32 const	cost			= cityData->GetOvertimeCost();

	if ((cost <= 0)									||
		(cost > g_player[visiblePlayer]->GetGold())	||
		cityData->AlreadyBoughtFront()			||
		cityData->IsBuildingCapitalization()		||
		cityData->IsBuildingInfrastructure()		||
		(m_queueList->GetSelectedItemIndex() > 0)
	   )
	{
		m_rushBuyButton->Enable(FALSE);
	}
	else
	{
		m_rushBuyButton->Enable(TRUE);
	}

	if(cityData->SellingBuilding() >= 0 ||
	   cityData->GetImprovements() == 0 ||
	   !m_inventoryList->GetSelectedItem()) {
		m_sellButton->Enable(FALSE);
	} else {
		m_sellButton->Enable(TRUE);
	}
}

//----------------------------------------------------------------------------
//
// Name       : CityWindow::UpdateCostsGives
//
// Description: Updates the rush buy costs of the current item.
//
// Parameters : -
//
// Globals    : -
//
// Returns    : -
//
// Remark(s)  : -
//
//----------------------------------------------------------------------------
void CityWindow::UpdateCostsGives()
{
	CityData * cityData = m_city->GetCityData();

	ctp2_Static *costsValue = (ctp2_Static *)aui_Ldl::GetObject(s_cityWindowBlock, "ProductionSection.CostsValue");
	ctp2_Static *givesValue = (ctp2_Static *)aui_Ldl::GetObject(s_cityWindowBlock, "InventorySection.GivesValue");
	Assert(costsValue);
	Assert(givesValue);

	if(cityData->GetBuildQueue()->GetLen() < 1
	|| cityData->AlreadyBoughtFront()
	//Added by Martin Gühmann to disable the rush buy cost in the case of infrastructure and capitalization
	|| cityData->GetBuildQueue()->GetHead()->m_category == k_GAME_OBJ_TYPE_CAPITALIZATION
	|| cityData->GetBuildQueue()->GetHead()->m_category == k_GAME_OBJ_TYPE_INFRASTRUCTURE
	// Make sure that costs aren't displayed if the first item is not selected
	|| s_cityWindow->m_queueList->GetSelectedItemIndex() != 0
	){
		costsValue->SetText("---");
	} else {
		MBCHAR buf[20];
		sprintf(buf, "%d", cityData->GetOvertimeCost());
		costsValue->SetText(buf);
	}

	ctp2_Static *tabpanel = (ctp2_Static *)aui_Ldl::GetObject(s_cityWindowBlock, "ProductionSection");
	ctp2_Static *tabpanel2 = (ctp2_Static *)aui_Ldl::GetObject(s_cityWindowBlock, "GarrisonSection");
	Assert(tabpanel);
	if(tabpanel) {
		tabpanel->ShouldDraw(TRUE);
	}
	Assert(tabpanel2);
	if(tabpanel2) {
		tabpanel2->ShouldDraw(TRUE);
	}

	ctp2_ListBox *lb = (ctp2_ListBox *)aui_Ldl::GetObject(s_cityWindowBlock, "InventorySection.List");

	Assert(lb);
	if(lb) {
		ctp2_ListItem *item = (ctp2_ListItem *)lb->GetSelectedItem();
		if(item) {
			InventoryItemInfo *info = (InventoryItemInfo *)item->GetUserData();
			Assert(info);
			if (info) {
				if(!info->m_isBuilding) {
					givesValue->SetText("---");
				} else {
					MBCHAR buf[20];
					sprintf(buf, "%d", sint32(double(buildingutil_Get(info->m_type, cityData->GetOwner())->GetProductionCost()) *
							g_theConstDB->Get(0)->GetBuildingProductionToValueModifier()));
					givesValue->SetText(buf);
				}
			} else {
				givesValue->SetText("---");
			}
		} else {
			givesValue->SetText("---");
		}

	}
}

void CityWindow::Close(aui_Control * control, uint32 action, uint32 data, void * cookie)
{
	if (action != AUI_BUTTON_ACTION_EXECUTE) {
		return;
	}

	Hide();
}

void CityWindow::NextCity(aui_Control *control, uint32 action, uint32 data, void *cookie)
{
	if(action != AUI_BUTTON_ACTION_EXECUTE)
		return;

	sint32 i;
	if(!s_cityWindow)
		return;

	UnitDynamicArray *cityList = g_player[g_selected_item->GetVisiblePlayer()]->m_all_cities;

	if(!s_cityWindow->m_city.IsValid()) {
		s_cityWindow->SetCity(cityList->Access(0));
		return;
	}

	if(cityList->Num() < 2)
		return;

	Unit curCity = s_cityWindow->m_city;
	for(i = 0; i < cityList->Num(); i++) {
		if(cityList->Access(i).m_id == curCity.m_id)
			break;
	}

	Assert(i < cityList->Num());
	if(i >= cityList->Num() - 1) {
		s_cityWindow->SetCity(cityList->Access(0));
	} else {
		s_cityWindow->SetCity(cityList->Access(i + 1));
	}

}

void CityWindow::PreviousCity(aui_Control *control, uint32 action, uint32 data, void *cookie)
{
	if(action != AUI_BUTTON_ACTION_EXECUTE)
		return;

	sint32 i;
	if(!s_cityWindow)
		return;

	UnitDynamicArray *cityList = g_player[g_selected_item->GetVisiblePlayer()]->m_all_cities;

	if(!s_cityWindow->m_city.IsValid()) {
		s_cityWindow->SetCity(cityList->Access(0));
		return;
	}

	if(cityList->Num() < 2)
		return;

	Unit curCity = s_cityWindow->m_city;
	for(i = 0; i < cityList->Num(); i++) {
		if(cityList->Access(i).m_id == curCity.m_id)
			break;
	}

	Assert(i < cityList->Num());
	if(i <= 0 || i >= cityList->Num()) {
		s_cityWindow->SetCity(cityList->Access(cityList->Num() - 1));
	} else {
		s_cityWindow->SetCity(cityList->Access(i - 1));
	}
}

void CityWindow::SelectCity(aui_Control *control, uint32 action, uint32 data, void *cookie)
{
	if(action != AUI_DROPDOWN_ACTION_SELECT)
		return;

	if(!s_cityWindow)
		return;

	if(s_cityWindow->m_updating)
		return;

	ctp2_DropDown *dd = (ctp2_DropDown *)control;
	if(dd->GetSelectedItem() >= 0) {
		Unit selectedCity = g_player[g_selected_item->GetVisiblePlayer()]->m_all_cities->Access(dd->GetSelectedItem());
		if (s_cityWindow->m_city != selectedCity) {
			s_cityWindow->SetCity(selectedCity);
		}
	}
}

void CityWindow::Resource(aui_Control *control, uint32 action, uint32 data, void *cookie)
{
	if(action != AUI_BUTTON_ACTION_EXECUTE)
		return;
#if 0
	ctp2_Static *workerBox = (ctp2_Static *)aui_Ldl::GetObject("CityWindow.Tabs.Specialists.TabPanel.WorkerBox");
	ctp2_Static *resourceBox = (ctp2_Static *)aui_Ldl::GetObject("CityWindow.ResourceMapBox");
	Assert(workerBox);
	Assert(resourceBox);

	if(workerBox->IsHidden()) {
		workerBox->Show();
		resourceBox->Hide();
	} else {
		workerBox->Hide();
		resourceBox->Show();
	}
#endif
}

void CityWindow::WorkerSpinnerCallback(aui_Control *control, uint32 action, uint32 data, void *cookie)
{
	static bool settingValueAutomatically = false;
	if(action != AUI_RANGER_ACTION_VALUECHANGE)
		return;

	if(settingValueAutomatically)
		return;

	ctp2_Spinner *spinner = (ctp2_Spinner *)cookie;

	Assert(s_cityWindow);
	if(!s_cityWindow)
		return;

	sint32 num = spinner->GetValueX();
	Assert(num >= 0);
	sint32 oldnum;
	POP_TYPE popType = POP_MAX;

	if(spinner == s_cityWindow->m_popSpinners[POP_ENTERTAINER]) {
		popType = POP_ENTERTAINER;
	} else if(spinner == s_cityWindow->m_popSpinners[POP_SCIENTIST]) {
		popType = POP_SCIENTIST;
	} else if(spinner == s_cityWindow->m_popSpinners[POP_LABORER]) {
		popType = POP_LABORER;
	} else if(spinner == s_cityWindow->m_popSpinners[POP_FARMER]) {
		popType = POP_FARMER;
	} else if(spinner == s_cityWindow->m_popSpinners[POP_MERCHANT]) {
		popType = POP_MERCHANT;
	} else {
		Assert(false);
	}

	if(popType != POP_MAX) {
		CityData * cityData = s_cityWindow->m_city.GetCityData();
		oldnum = cityData->SpecialistCount(popType);// Segfault location, probably m_cityData is invalid, after city disband
		sint32 delta = num - oldnum;
		if(delta > 0 && delta > cityData->WorkerCount()) {
			settingValueAutomatically = true;
			spinner->SetValue(oldnum, 0);
			settingValueAutomatically = false;
		} else if(delta != 0) {
			cityData->ChangeSpecialists(popType, delta);
			s_cityWindow->Project();
		}
	}

	s_cityWindow->Update();
}

void CityWindow::GovernorToggle(aui_Control *control, uint32 action, uint32 data, void *cookie)
{
	if(action != AUI_BUTTON_ACTION_EXECUTE)
		return;

	CityData * cityData = s_cityWindow->m_city.GetCityData();
	cityData->SetUseGovernor(!cityData->GetUseGovernor());

	s_cityWindow->Update();
}

void CityWindow::GovernorPriority(aui_Control *control, uint32 action, uint32 data, void *cookie)
{
	if(action != AUI_DROPDOWN_ACTION_SELECT)
		return;

	if(!s_cityWindow)
		return;

	ctp2_DropDown *dd = (ctp2_DropDown *)control;
	s_cityWindow->m_city.GetCityData()->SetBuildListSequenceIndex(dd->GetSelectedItem());

	s_cityWindow->Update();
}

//----------------------------------------------------------------------------
//
// Name       : CityWindow::EditQueue
//
// Description: Opens the Build Queue Manager. (Button callback function)
//
// Parameters : aui_Control *control
//              uint32 action
//              uint32 data
//              void *cookie
//
// Globals    : -
//
// Returns    : -
//
// Remark(s)  : -
//
//----------------------------------------------------------------------------
void CityWindow::EditQueue(aui_Control *control, uint32 action, uint32 data, void *cookie)
{
	if(action != AUI_BUTTON_ACTION_EXECUTE)
		return;

	if(!s_cityWindow)
		return;

	EditQueue::Display(s_cityWindow->m_city);

	s_cityWindow->Update();
}

//----------------------------------------------------------------------------
//
// Name       : CityWindow::OpenNationalManager
//
// Description: Opens the National Manager, when the National Manager button
//              is clicked. (Button callback function)
//
// Parameters : aui_Control *control
//              uint32 action
//              uint32 data
//              void *cookie
//
// Globals    : -
//
// Returns    : -
//
// Remark(s)  : -
//
//----------------------------------------------------------------------------
void CityWindow::OpenNationalManager(aui_Control *control, uint32 action, uint32 data, void *cookie)
{
	if(action != AUI_BUTTON_ACTION_EXECUTE)
		return;

	NationalManagementDialog::Open();
}

//----------------------------------------------------------------------------
//
// Name       : CityWindow::OptimizeSpecialists
//
// Description: Optimizes the specialists assignment. (Button callback function)
//
// Parameters : aui_Control *control
//              uint32 action
//              uint32 data
//              void *cookie
//
// Globals    : -
//
// Returns    : -
//
// Remark(s)  : -
//
//----------------------------------------------------------------------------
void CityWindow::OptimizeSpecialists(aui_Control *control, uint32 action, uint32 data, void *cookie)
{
	if(action != AUI_BUTTON_ACTION_EXECUTE)
		return;

	if(!s_cityWindow)
		return;

	PLAYER_INDEX playerId = g_selected_item->GetVisiblePlayer();
	Governor & governor = Governor::GetGovernor(playerId);

#if defined(_DEBUG) || defined(USE_LOGGING)
	Timer t;
	t.start();

	DPRINTF(k_DBG_AI, ("\n"));
	DPRINTF(k_DBG_AI, ("// ADJUST SPECIALIST SETTINGS -- Turn %d\n",   g_player[playerId]->GetCurRound()));
	DPRINTF(k_DBG_AI, ("//                               Player %d\n", playerId));
#endif

	CityData * cityData = s_cityWindow->m_city.GetCityData();
	g_player[playerId]->PreResourceCalculation(cityData);
	governor.AssignPopulation(cityData, g_player[playerId]->m_advances->HasAllAdvances());

#if defined(_DEBUG) || defined(USE_LOGGING)
	DPRINTF(k_DBG_AI, ("//  elapsed time = %f ms\n", t.getElapsedTimeInMilliSec()));
	DPRINTF(k_DBG_AI, ("\n"));

	t.stop();
#endif

	s_cityWindow->Project();

	s_cityWindow->Update();
}

//----------------------------------------------------------------------------
//
// Name       : CityWindow::BuildListSelect
//
// Description: Select the data for the image button and the hyper text box.
//              Now it also handles the turn count button below the image
//              button. (Button callback function)
//
// Parameters : aui_Control *control
//              uint32 action
//              uint32 data
//              void *cookie
//
// Globals    : -
//
// Returns    : -
//
// Remark(s)  : -
//
//----------------------------------------------------------------------------
void CityWindow::BuildListSelect(aui_Control *control, uint32 action, uint32 data, void *cookie)
{
	if((action != AUI_LISTBOX_ACTION_SELECT) &&
	   (action != AUI_LISTBOX_ACTION_DOUBLECLICKSELECT))
		return;

	s_cityWindow->UpdateCostsGives();

	ctp2_ListBox *lb = s_cityWindow->m_queueList;

	Assert(lb);

	CityData * cityData = s_cityWindow->m_city.GetCityData();
	if(lb) {
		ctp2_ListItem *item = (ctp2_ListItem *)lb->GetSelectedItem();
		SlicContext sc;

		ctp2_Button *imageButton = (ctp2_Button *)aui_Ldl::GetObject(s_cityWindowBlock, "GarrisonSection.ItemProgress.IconBorder.IconButton");

		if(!item) {
			SetItemIconOnly(NULL, sc, NULL, s_cityWindow->m_tabPanels[CW_PANEL_GARRISON],
							   imageButton);

			s_cityWindow->UpdateBuildTabButtons();
			return;
		}

		const IconRecord *icon = NULL;

		BuildNode *node = cityData->GetBuildQueue()->GetNodeByIndex(lb->GetSelectedItemIndex());
		Assert(node);
		if(node) {
			switch(node->m_category) {
				case k_GAME_OBJ_TYPE_UNIT:
					icon = g_theUnitDB->Get(node->m_type)->GetDefaultIcon();
					sc.AddUnitRecord(node->m_type);
					break;
				case k_GAME_OBJ_TYPE_IMPROVEMENT:
					icon = g_theBuildingDB->Get(node->m_type)->GetDefaultIcon();
					sc.AddBuilding(node->m_type);
					break;
				case k_GAME_OBJ_TYPE_WONDER:
					icon = g_theWonderDB->Get(node->m_type)->GetDefaultIcon();
					sc.AddWonder(node->m_type);
					break;
				case k_GAME_OBJ_TYPE_CAPITALIZATION:
				{
					sint32 index;
					if(g_theIconDB->GetNamedItem("ICON_IMPROVE_CAPITALIZATION", index)) {
						icon = g_theIconDB->Get(index);
					} else {
						icon = NULL;
					}
					break;
				}
				case k_GAME_OBJ_TYPE_INFRASTRUCTURE:
				{
					sint32 index;
					if(g_theIconDB->GetNamedItem("ICON_IMPROVE_INFRASTRUCTURE", index)) {
						icon = g_theIconDB->Get(index);
					} else {
						icon = NULL;
					}
					break;
				}
				default:
					Assert(FALSE);
					break;
			}
		}

		SetItemIconOnly(icon, sc, NULL, s_cityWindow->m_tabPanels[CW_PANEL_GARRISON],
						   imageButton);
	}
	s_cityWindow->UpdateBuildTabButtons();

	//Added by Martin Gühmann to update the turn count display of the image button
	ctp2_Button *	turnCountButton = (ctp2_Button *) aui_Ldl::GetObject
		(s_cityWindowBlock, "GarrisonSection.ItemProgress.IconBorder.IconButton.RadialButton");

	if (lb && turnCountButton)
	{
		BuildNode *		node	= cityData->GetBuildQueue()->GetNodeByIndex(lb->GetSelectedItemIndex());
		sint32 const	turns	= (lb->GetSelectedItemIndex() == 0)
				? (cityData->AlreadyBoughtFront() ? -1 : cityData->HowMuchLonger())
				: cityData->HowMuchLonger(node->m_cost);

		//Added by Martin Gühmann to disable the turn count display for capitalization and infrastructure
		MBCHAR buf[20];
		if ((turns >= 0)											&&
			(turns < 0x7fffffff)									&&
			(node->m_category != k_GAME_OBJ_TYPE_CAPITALIZATION)	&&
			(node->m_category != k_GAME_OBJ_TYPE_INFRASTRUCTURE)
		   )
		{
			sprintf(buf, "%d", turns);
		}
		else
		{
			strcpy(buf, "---");
		}
		turnCountButton->SetText(buf);
	}
}

void CityWindow::InventoryListSelect(aui_Control *control, uint32 action, uint32 data, void *cookie)
{
	if((action != AUI_LISTBOX_ACTION_SELECT) &&
	   (action != AUI_LISTBOX_ACTION_DOUBLECLICKSELECT))
		return;

	s_cityWindow->UpdateCostsGives();

	ctp2_ListBox *lb = s_cityWindow->m_inventoryList;

	Assert(lb);

	if(lb) {
		ctp2_ListItem *item = (ctp2_ListItem *)lb->GetSelectedItem();
		SlicContext sc;

		ctp2_Button *imageButton = (ctp2_Button *)aui_Ldl::GetObject(s_cityWindowBlock, "GarrisonSection.ItemImage.IconBorder.IconButton");

		if(!item) {
			SetItemIconOnly(NULL, sc, NULL, s_cityWindow->m_tabPanels[CW_PANEL_GARRISON], imageButton);

			s_cityWindow->UpdateBuildTabButtons();
			return;
		}

		const IconRecord *icon = NULL;
		InventoryItemInfo *info = (InventoryItemInfo *)item->GetUserData();
		Assert(info);
		if(info->m_isBuilding) {
			icon = g_theBuildingDB->Get(info->m_type)->GetDefaultIcon();
			sc.AddBuilding(info->m_type);
		} else {
			icon = g_theWonderDB->Get(info->m_type)->GetDefaultIcon();
			sc.AddWonder(info->m_type);
		}

		SetItemIconOnly(icon, sc, NULL, s_cityWindow->m_tabPanels[CW_PANEL_GARRISON], imageButton);
	}
	s_cityWindow->UpdateBuildTabButtons();
}

void CityWindow::Buy(aui_Control *control, uint32 action, uint32 data, void *cookie)
{
	if (action != AUI_BUTTON_ACTION_EXECUTE) {
		return;
	}
	if (!s_cityWindow || !s_cityWindow->m_city.IsValid()) {
		return;
	}

	const Unit & city = s_cityWindow->m_city;
	CityData * cityData = city.GetCityData();
	if (!cityData->AlreadyBoughtFront())
	{
		cityData->AddBuyFront();
		UpdateCity(city);
		EditQueue::UpdateCity(city);
		NationalManagementDialog::UpdateCity(city);
	}
}

void CityWindow::SelectCity(const Unit & city)
{
	if (!s_cityWindow) {
		return;
	}

	if (!s_cityWindow->m_city.IsValid() || s_cityWindow->m_city != city) {
		s_cityWindow->SetCity(city);
	}
}

void CityWindow::UpdateCity(const Unit & city)
{
	if (!s_cityWindow || !s_cityWindow->m_city.IsValid() || s_cityWindow->m_city != city) {
		return;
	}

	s_cityWindow->UpdateCostsGives();
	s_cityWindow->UpdateBuildTabs();
}

void CityWindow::Sell(aui_Control *control, uint32 action, uint32 data, void *cookie)
{

	if(action != AUI_BUTTON_ACTION_EXECUTE)
		return;

	ctp2_ListBox *lb = s_cityWindow->m_inventoryList;
	Assert(lb);
	if(lb) {
		ctp2_ListItem *item = (ctp2_ListItem *)lb->GetSelectedItem();
		if(item) {
			InventoryItemInfo *info = (InventoryItemInfo *)item->GetUserData();
			if(!info->m_isBuilding) {
				return;
			}

			s_cityWindow->m_city.GetCityData()->AddSellBuilding(info->m_type);
			s_cityWindow->UpdateBuildTabButtons();
		}
	}
}

void CityWindow::CityList( aui_Control *control, uint32 action, uint32 data, void *cookie )
{
	if ( action != (uint32)CTP2_HYPERLINK_ACTION_EXECUTE ) return;

	open_CityStatus();
}

//----------------------------------------------------------------------------
//
// Name       : CityWindow::PopulateQueueList
//
// Description: Looks like it initializes the queue list boxes and
//              updates them. The turn count calculatation is done here.
//
// Parameters : -
//
// Globals    : -
//
// Returns    : -
//
// Remark(s)  : -
//
//----------------------------------------------------------------------------
void CityWindow::PopulateQueueList(const Unit & city, ctp2_ListBox *listBox, char * itemBlock)
{
	if (!city.IsValid()) {
		return;
	}

	CityData * cityData = city.GetCityData();
	BuildQueue *bq = cityData->GetBuildQueue();
	PointerList<BuildNode> *blist = bq->GetList();
	listBox->Clear();
	PointerList<BuildNode>::Walker walk(blist);

	while(walk.IsValid()) {
		ctp2_ListItem *item;
		item = (ctp2_ListItem *)aui_Ldl::BuildHierarchyFromRoot(itemBlock);
		Assert(item);
		if(item) {
			ctp2_Static *box = (ctp2_Static *)item->GetChildByIndex(0);
			BuildNode *bn = walk.GetObj();
			if(box) {
				ctp2_Static *label = (ctp2_Static *)box->GetChildByIndex(0);
				ctp2_Static *turnstatic = (ctp2_Static *)box->GetChildByIndex(1);

				switch(bn->m_category) {
					case k_GAME_OBJ_TYPE_UNIT:
						label->SetText(g_theUnitDB->Get(bn->m_type)->GetNameText());
						break;
					case k_GAME_OBJ_TYPE_IMPROVEMENT:
						label->SetText(g_theBuildingDB->Get(bn->m_type)->GetNameText());
						break;
					case k_GAME_OBJ_TYPE_WONDER:
						label->SetText(g_theWonderDB->Get(bn->m_type)->GetNameText());
						break;
					case k_GAME_OBJ_TYPE_INFRASTRUCTURE:
						label->SetText(g_theStringDB->GetNameStr("INFRASTRUCTURE"));
						break;
					case k_GAME_OBJ_TYPE_CAPITALIZATION:
						label->SetText(g_theStringDB->GetNameStr("CAPITALIZATION"));
						break;
				}
				sint32 turns = -1;
				if(bn == blist->GetHead()) {
					turns = cityData->AlreadyBoughtFront() ? -1 : cityData->HowMuchLonger();
				} else {
					if(cityData->GetNetCityProduction() > 0) {
						turns = cityData->HowMuchLonger(bn->m_cost);
					} else {
						turns = -1;
					}
				}
				char buf[20];
				//Added by Martin Gühmann to remove number of turn
				//display in the Build Manager and City Manager
				//if infrastructure or capilization is displayed.
				if(turns < 0 || turns == 0x7fffffff
				|| bn->m_category == k_GAME_OBJ_TYPE_INFRASTRUCTURE
				|| bn->m_category == k_GAME_OBJ_TYPE_CAPITALIZATION)
				{
					strcpy(buf, "---");
				}
				else
				{
					sprintf(buf, "%d", turns);
				}
				turnstatic->SetText(buf);
			}

			listBox->AddItem(item);
		}
		walk.Next();
	}
}
AUI_ERRCODE CityWindow::DrawGrowthBar(ctp2_Static *control,
									  aui_Surface *surface,
									  RECT &rect,
									  void *cookie )
{
	if(!s_cityWindow)
		return AUI_ERRCODE_OK;

	if(!s_cityWindow->m_city.IsValid())
		return AUI_ERRCODE_OK;

	RECT destRect = rect;
	AUI_ERRCODE err;

	err = g_ui->TheBlitter()->ColorBlt(surface, &destRect, RGB(0,0,0), 0);

	destRect.top += 2;
	destRect.bottom -= 2;
	destRect.left += 2;
	destRect.right -= 2;

	sint32 width = destRect.right - destRect.left;
	CityData *cd = s_cityWindow->m_city.GetCityData();

	double overcrowding = cd->GetOvercrowdingCoefficient();
	if(overcrowding < 0)
		overcrowding = 0;

	if(overcrowding > 1)
		overcrowding = 1;

	destRect.right = destRect.left + sint32((1.0 - cd->GetOvercrowdingCoefficient()) * double(width));
	if(destRect.right > rect.right)
		destRect.right = rect.right;

	err = g_ui->TheBlitter()->ColorBlt(surface, &destRect, RGB(255,0,0), 0);

	if((cd->GetGrowthRate() > 0) && ((destRect.right - destRect.left) < width)) {
		destRect.left = destRect.right;


		sint32 maxAbsoluteGrowthRate = g_theConstDB->Get(0)->GetMaxAbsoluteGrowthRate();
		sint32 actualGrowthRate = cd->GetGrowthRate();
		double growthRatio = 0;
		if(actualGrowthRate >= maxAbsoluteGrowthRate) {
			growthRatio = 1;
		} else {
			growthRatio = double(actualGrowthRate) / double(maxAbsoluteGrowthRate);
		}
		width = rect.right - destRect.left;
		destRect.right = destRect.left + sint32(double(width) * growthRatio);
		if(destRect.right > rect.right)
			destRect.right = rect.right;
		err = g_ui->TheBlitter()->ColorBlt(surface, &destRect, RGB(0,255,0), 0);
	}
	return err;
}

AUI_ERRCODE CityWindow::DrawHappinessBar(ctp2_Static *control,
									  aui_Surface *surface,
									  RECT &rect,
									  void *cookie )
{
	if(!s_cityWindow)
		return AUI_ERRCODE_OK;

	if(!s_cityWindow->m_city.IsValid())
		return AUI_ERRCODE_OK;

	RECT destRect = rect;
	AUI_ERRCODE err;

	err = g_ui->TheBlitter()->ColorBlt(surface, &destRect, RGB(0,0,0), 0);

	destRect.top += 2;
	destRect.bottom -= 2;
	destRect.left += 2;
	destRect.right -= 2;

	sint32 width = destRect.right - destRect.left;
	CityData *cd = s_cityWindow->m_city.GetCityData();

	double happiness = cd->GetHappiness();
	if(happiness > 100) happiness = 100;

	destRect.right = destRect.left + static_cast<sint32>((happiness * width) / 100);

	uint32 color;
	if(happiness < g_theConstDB->Get(0)->GetRiotLevel()) {
		color = RGB(255,0,0);
	} else if(cd->IsCelebratingHappiness()) {
		color = RGB(0,255,0);
	} else {
		color = RGB(255,255,0);
	}
	err = g_ui->TheBlitter()->ColorBlt(surface, &destRect, color, 0);
	return err;
}

AUI_ERRCODE CityWindow::DrawEfficiencyBar(ctp2_Static *control,
									  aui_Surface *surface,
									  RECT &rect,
									  void *cookie )
{
	if(!s_cityWindow)
		return AUI_ERRCODE_OK;

	if(!s_cityWindow->m_city.IsValid())
		return AUI_ERRCODE_OK;

	RECT destRect = rect;
	AUI_ERRCODE err;

	err = g_ui->TheBlitter()->ColorBlt(surface, &destRect, RGB(0,0,0), 0);

	destRect.top += 2;
	destRect.bottom -= 2;
	destRect.left += 2;
	destRect.right -= 2;

	sint32 width = destRect.right - destRect.left;
	CityData *cd = s_cityWindow->m_city.GetCityData();

	double efficiency = (double)cd->WorkerCount() / cd->PopCount();
	sint32 efficientWidth = (sint32)(((double)width) * efficiency);

	destRect.right = destRect.left + efficientWidth;

	err = g_ui->TheBlitter()->ColorBlt(surface, &destRect, RGB(0,255,0), 0);
	return err;
}

AUI_ERRCODE CityWindow::DrawBuildBar(ctp2_Static *control,
									  aui_Surface *surface,
									  RECT &rect,
									  void *cookie )
{
	if(!s_cityWindow)
		return AUI_ERRCODE_OK;

	if(!s_cityWindow->m_city.IsValid())
		return AUI_ERRCODE_OK;

	RECT destRect = rect;
	AUI_ERRCODE err;

	err = g_ui->TheBlitter()->ColorBlt(surface, &destRect, RGB(0,0,0), 0);

	destRect.top += 1;
	destRect.bottom -= 1;
	destRect.left += 1;
	destRect.right -= 1;

	sint32 width = destRect.right - destRect.left;
	CityData *cd = s_cityWindow->m_city.GetCityData();

	double percentComplete = cd->GetBuildQueue()->GetPercentCompleted(cd->GetStoredCityProduction()) / 100.0;

	if(percentComplete > 1.0 || cd->AlreadyBoughtFront())
		percentComplete = 1.0;

	if(s_cityWindow->m_queueList->GetSelectedItemIndex() != 0
	|| percentComplete < 0) {

		destRect.right = destRect.left;
	} else {
		destRect.right = destRect.left + sint32(double(width) * percentComplete);
	}
	err = g_ui->TheBlitter()->ColorBlt(surface, &destRect, RGB(0,255,0), 0);

	return err;
}

void CityWindow::ShowStatistics(aui_Control *control, uint32 action, uint32 data, void *cookie)
{
	if(action != AUI_BUTTON_ACTION_EXECUTE)
		return;

	Assert(s_cityWindow);
	if(!s_cityWindow)
		return;

	Assert(s_cityWindow->m_statsWindow);
	if(s_cityWindow->m_statsWindow) {
		if(g_c3ui->GetChild(s_cityWindow->m_statsWindow->Id())) {
			g_c3ui->RemoveWindow(s_cityWindow->m_statsWindow->Id());
		} else {
			g_c3ui->AddWindow(s_cityWindow->m_statsWindow);
		}
	}
}

void CityWindow::UpdateResourceMap()
{
	if(g_resourceMap) {
		g_resourceMap->SetUnit(m_city);
		g_resourceMap->DrawSurface();
	}

	ctp2_Static *mapStatic = (ctp2_Static *)aui_Ldl::GetObject(s_cityStatsBlock, "Map");
	if(mapStatic) {
		mapStatic->ShouldDraw(TRUE);
	}
}

void CityWindow::UpdateAdviceText()
{
	ctp2_HyperTextBox *advice =
		(ctp2_HyperTextBox *)aui_Ldl::GetObject(s_cityStatsBlock, "Advice");

	Assert(advice);
	if(advice == NULL)
		return;

	if(!m_city.IsValid())
		{
			advice->SetHyperText("");
			return;
		}

	PLAYER_INDEX playerId = g_selected_item->GetVisiblePlayer();
	const Governor & governor = Governor::GetGovernor(playerId);

	StringId adviceId =
		governor.GetCityBuildQueueAdvice(m_city.GetCityData());

	if (adviceId < 0)
		{
			advice->SetHyperText("");
			return;
		}

	MBCHAR	strbuf[k_MAX_NAME_LEN];
	SlicContext sc;

	stringutils_Interpret(g_theStringDB->GetNameStr(adviceId),
						  sc, strbuf);
	advice->SetHyperText(strbuf);
}

void CityWindow::UpdateUnitButtons()
{
	Assert(m_city.IsValid());
	if(!m_city.IsValid())
		return;

	MapPoint pos = m_city.RetPos();
	Cell *cell = g_theWorld->GetCell(pos);
	sint32 i;
	sint32 button = 0;

	for(i = 0; i < cell->GetNumUnits(); i++) {
		Unit u = cell->AccessUnit(i);
		Assert(u.IsValid());
		if(u.IsValid()) {
			if(u.IsEntrenched() || u.IsEntrenching()) {
				if(u.m_id != m_unitId[button]) {
					m_unitId[button] = u.m_id;
					m_unitButtons[button]->ExchangeImage(5, 0, u.GetDBRec()->GetDefaultIcon()->GetLargeIcon());
					m_unitButtons[button]->Enable(TRUE);
					m_unitButtons[button]->ShouldDraw(TRUE);
					m_unitButtons[button]->SetToggleState(false);
				}
				button++;
			}
		}
	}
	for(; button < k_MAX_ARMY_SIZE; button++) {
		if(m_unitId[button] != 0) {
			m_unitId[button] = 0;
			m_unitButtons[button]->ExchangeImage(5, 0, NULL);
			m_unitButtons[button]->Enable(FALSE);
			m_unitButtons[button]->ShouldDraw(TRUE);
		}
	}

	UpdateActivateButtons();
}

void CityWindow::UpdateActivateButtons()
{

	sint32 i;
	for(i = 0; i < k_MAX_ARMY_SIZE; i++) {
		if(m_unitButtons[i]->GetToggleState()) {
			m_activateButton->Enable(TRUE);
			m_disbandButton->Enable(TRUE);
			return;
		}
	}

	m_activateButton->Enable(FALSE);
	m_disbandButton->Enable(FALSE);
}

AUI_ERRCODE CityWindow::DrawResourceMap(ctp2_Static *control,
										aui_Surface *surface,
										RECT &rect,
										void *cookie)
{
	if(!g_resourceMap) {
		workwin_Initialize();
	}

	Assert(g_resourceMap);
	if(g_resourceMap) {
		g_resourceMap->UpdateFromSurface(surface, &rect);
	}
	return AUI_ERRCODE_OK;
}

void CityWindow::SetItemIconOnly(const IconRecord *icon, SlicContext &sc, ctp2_Static *imageBox, aui_Region *parent, ctp2_Button *imageButton)
{
	Assert(imageBox || imageButton);
	if(!imageBox && !imageButton) return;

	if(icon) {
		MBCHAR dammit[1024];
		if(!icon->GetLargeIcon() || stricmp(icon->GetLargeIcon(), "null") == 0) {
			strcpy(dammit, icon->GetIcon());
		} else {
			strcpy(dammit, icon->GetLargeIcon());
		}
		if(imageBox) {
			imageBox->SetImage(dammit);
			imageBox->ShouldDraw(TRUE);
		} else if(imageButton) {
			imageButton->ExchangeImage(4, 0, dammit);
			imageButton->ShouldDraw(TRUE);
		}
	} else {
		if(imageBox) {
			imageBox->SetImage(NULL);
			imageBox->ShouldDraw(TRUE);
		} else if(imageButton) {
			imageButton->ExchangeImage(4, 0, NULL);
			imageButton->ShouldDraw(TRUE);
		}
	}
}

void CityWindow::BuildTabCallback(aui_Control *control, uint32 action, uint32 data, void *cookie)
{
	if(action != ctp2_Tab::ACTION_ACTIVATED) return;

	if(!s_cityWindow) return;

}

struct cw_HappyData {
	double amount;
	HAPPY_REASON reason;
	StringId name;
};

int cw_compareHappyValues(const void *item1, const void *item2)
{
	const cw_HappyData *h1 = (const cw_HappyData *)item1;
	const cw_HappyData *h2 = (const cw_HappyData *)item2;

	return static_cast<int>(h1->amount - h2->amount);
}

void CityWindow::FillHappinessList()
{
	Assert(m_happinessList);
	if(!m_happinessList) return;

	CityData * cityData = m_city.GetCityData();
	HappyTracker *ht = cityData->GetHappy()->GetHappyTracker();
	Assert(ht);
	if(!ht) return;

	cw_HappyData  happies[HAPPY_REASON_MAX];

	//note: this label was/is not used.
	ctp2_Static *happinessLabel = (ctp2_Static *)aui_Ldl::GetObject(s_cityWindowBlock, "StatisticsSection.HappinessTotalLabel");
	if (happinessLabel)
	{
		const char *    format = g_theStringDB->GetNameStr("str_code_CityWinTotalHappinessFormat");
		if (!format)
		{
			format  = "%d";
		}

		char buf[k_MAX_NAME_LEN];
		sprintf(buf, format, (sint32)cityData->GetHappiness());
	}

	sint32 i;
	sint32 numHappies = 0;
	for (i = 0; i < HAPPY_REASON_MAX; i++)
	{
		double      amount;
		StringId    name;
		ht->GetHappiness((HAPPY_REASON)i, amount, name);

		if (amount < 0.1 && amount > -0.1)
			continue;

		happies[numHappies].reason = (HAPPY_REASON)i;
		happies[numHappies].amount = amount;
		happies[numHappies].name = name;
		numHappies++;
	}

	qsort((void *)happies, numHappies, sizeof(cw_HappyData), cw_compareHappyValues);

	m_happinessList->Clear();
	for (i = 0; i < numHappies; i++)
	{
		ctp2_ListItem *item = (ctp2_ListItem *)aui_Ldl::BuildHierarchyFromRoot("cw_HappyListItem");
		Assert(item);
		if(!item) break;

		ctp2_Static *   box         = (ctp2_Static *)item->GetChildByIndex(0);
		ctp2_Static *   happyReason = (ctp2_Static *)box->GetChildByIndex(1);
		happyReason->SetText(g_theStringDB->GetNameStr(happies[i].name));

		if (happies[i].amount > 0)
		{
			ctp2_Static *   happyAmount = (ctp2_Static *)box->GetChildByIndex(2);
			happyAmount->SetDrawCallbackAndCookie
			    (DrawHappyIcons, (void *)(sint32)happies[i].amount, false);
		}
		else
		{
			ctp2_Static *   unhappyAmount = (ctp2_Static *)box->GetChildByIndex(0);
			unhappyAmount->SetDrawCallbackAndCookie
			    (DrawUnhappyIcons, (void *)(sint32)happies[i].amount, false);
		}

		char buf[20];
		sprintf(buf, "%c%d", char(happies[i].amount > 0 ? '+' : ' '), (sint32)happies[i].amount);
		ctp2_Static *numeric = (ctp2_Static *)box->GetChildByIndex(3);
		if (numeric)
		{
			numeric->SetText(buf);
		}

		m_happinessList->AddItem(item);
	}
}

#define k_NUM_POINTS_PER_ICON 3

AUI_ERRCODE CityWindow::DrawUnhappyIcons(ctp2_Static *control,
								  aui_Surface *surface,
								  RECT &rect,
								  void *cookie )
{
	sint32 amount = (sint32)cookie;
	aui_Image *im = s_cityWindow->m_unhappyIcon;
	Assert(im);
	if(!im) return AUI_ERRCODE_OK;

	sint32 numIcons = (abs(amount) + (k_NUM_POINTS_PER_ICON - 1)) / k_NUM_POINTS_PER_ICON;
	if(numIcons <= 0)
		numIcons = 1;

    sint32 w = (rect.right - im->TheSurface()->Width() - rect.left) / numIcons;
	if(w > im->TheSurface()->Width())
		w = im->TheSurface()->Width();

	RECT srcRect = {
		0, 0,
		im->TheSurface()->Width(),
		im->TheSurface()->Height()
	};
	im->SetChromakey(255,0,255);

	RECT destRect = rect;
	destRect.left = destRect.right - w;

	sint32 i;
	for(i = 0; i < numIcons; i++) {
		g_c3ui->TheBlitter()->Blt(surface,
								  destRect.left, destRect.top,
								  im->TheSurface(),
								  &srcRect,
								  k_AUI_BLITTER_FLAG_CHROMAKEY);
		destRect.left -= w;
	}
	return AUI_ERRCODE_OK;
}

AUI_ERRCODE CityWindow::DrawHappyIcons(ctp2_Static *control,
								  aui_Surface *surface,
								  RECT &rect,
								  void *cookie )
{

	sint32 amount = (sint32)cookie;
	aui_Image *im = s_cityWindow->m_happyIcon;
	Assert(im);
	if(!im) return AUI_ERRCODE_OK;

	sint32 numIcons = (abs(amount) + (k_NUM_POINTS_PER_ICON - 1)) / k_NUM_POINTS_PER_ICON;
	if(numIcons <= 0)
		numIcons = 1;

    sint32 w = (rect.right - im->TheSurface()->Width() - rect.left) / numIcons;
	if(w > im->TheSurface()->Width())
		w = im->TheSurface()->Width();

	RECT srcRect = {
		0, 0,
		im->TheSurface()->Width(),
		im->TheSurface()->Height()
	};
	im->SetChromakey(255,0,255);

	RECT destRect = rect;
	sint32 i;
	for(i = 0; i < numIcons; i++) {
		g_c3ui->TheBlitter()->Blt(surface,
								  destRect.left, destRect.top,
								  im->TheSurface(),
								  &srcRect,
								  k_AUI_BLITTER_FLAG_CHROMAKEY);
		destRect.left += w;
	}
	return AUI_ERRCODE_OK;
}

static int cw_comparePollutionItems(const void *item1, const void *item2)
{
	ctp2_ListItem *i1 = *(ctp2_ListItem **)item1;
	ctp2_ListItem *i2 = *(ctp2_ListItem **)item2;

	return (sint32)i1->GetUserData() - (sint32)i2->GetUserData();
}

void CityWindow::FillPollutionList()
{
	Assert(m_pollutionList);
	if(!m_pollutionList) return;

	CityData * cityData = m_city.GetCityData();
	const COLORREF colorNorm = RGB(50,50,50);
	const COLORREF colorCritical = g_colorSet->GetColorRef(COLOR_RED);

	ctp2_Static *pollutionLabel2 = (ctp2_Static *)aui_Ldl::GetObject(s_cityWindowBlock, "StatisticsSection.PollutionTotal");
	if(pollutionLabel2) {
		sint32 pollution = cityData->GetPollution();
		char buf[k_MAX_NAME_LEN];
		const char *format2 = g_theStringDB->GetNameStr("str_code_TotalPollutionFormat");
		Assert(format2);
		if(format2) {
			sprintf(buf, format2,  cityData->GetPollution());
			pollutionLabel2->SetText(buf);
		} else {
			pollutionLabel2->SetText("");
		}
		if (pollution > g_theConstDB->Get(0)->GetLocalPollutionLevel()) {
			pollutionLabel2->SetTextColor(colorCritical);
		} else
			pollutionLabel2->SetTextColor(colorNorm);
	}

	MBCHAR interp[k_MAX_NAME_LEN];
	ctp2_Static *label,*sublabel;

	m_pollutionList->Clear();

	ctp2_ListItem *allAbsItems[2 + 64];
	ctp2_ListItem *allPercentItems[64];
	sint32 numAbsItems = 0, numPercentItems = 0;

	if (cityData->GetPopulationPollution())
	{
		ctp2_ListItem *	item = (ctp2_ListItem *) aui_Ldl::BuildHierarchyFromRoot("cw_PollutionListItem");
		if (item)
		{
			label = (ctp2_Static *)item->GetChildByIndex(0);
			sublabel = (ctp2_Static *)label->GetChildByIndex(0);
			sublabel->SetText(g_theStringDB->GetNameStr("str_ldl_PollutionList_Population"));
			sublabel = (ctp2_Static *)label->GetChildByIndex(1);
			sprintf(interp,"%i", cityData->GetPopulationPollution());
			sublabel->SetText(interp);
			item->SetUserData((void *)cityData->GetPopulationPollution());
			allAbsItems[numAbsItems++] = item;
		}
	}

	if (cityData->GetProductionPollution())
	{
		ctp2_ListItem * item = (ctp2_ListItem *) aui_Ldl::BuildHierarchyFromRoot("cw_PollutionListItem");
		if (item)
		{
			label = (ctp2_Static *)item->GetChildByIndex(0);
			sublabel = (ctp2_Static *)label->GetChildByIndex(0);
			sublabel->SetText(g_theStringDB->GetNameStr("str_ldl_PollutionList_Production"));
			sublabel = (ctp2_Static *)label->GetChildByIndex(1);
			sprintf(interp,"%i",cityData->GetProductionPollution());
			sublabel->SetText(interp);
			item->SetUserData((void *)cityData->GetProductionPollution());
			allAbsItems[numAbsItems++] = item;
		}
	}

	sint32 i;
	for (i = 0; i < g_theBuildingDB->NumRecords(); i++)
	{
		if (cityData->HasBuilding(i))
		{
			const BuildingRecord* rec = buildingutil_Get(i, cityData->GetOwner());

			// For an improvement, there are 3 values that contribute to or
			// decrease the pollution:
			// - an intrinsic (fixed) building pollution value
			// - a modifier to the production pollution of the city
			// - a modifier to the population pollution of the city

			// intrinsic
			double	value							= 0;	// running total
			rec->GetPollutionAmount(value);

			// production pollution modifier
			double	production_pollution_percent	= 0;
			rec->GetProductionPollutionPercent(production_pollution_percent);
			value += cityData->GetProductionPollution() * production_pollution_percent;

			// population pollution modifier
			double	population_pollution_percent	= 0;	// population pollution modifier
			rec->GetPopulationPollutionPercent(population_pollution_percent);
			value += cityData->GetPopulationPollution() * population_pollution_percent;

			if (fabs(value) < 1)
			{
				// Do not report when impact is small.
			}
			else
			{
				ctp2_ListItem * item = (ctp2_ListItem *) aui_Ldl::BuildHierarchyFromRoot("cw_PollutionListItem");
				if (item)
				{
					label = (ctp2_Static *)item->GetChildByIndex(0);
					sublabel = (ctp2_Static *)label->GetChildByIndex(0);
					sublabel->SetText(rec->GetNameText());
					sublabel = (ctp2_Static *)label->GetChildByIndex(1);
					sprintf(interp,"%d",(sint32) value);
					sublabel->SetText(interp);
					item->SetUserData((void *)(sint32)value);
					allAbsItems[numAbsItems++] = item;
				}
			}
		}
	}

	qsort((void *)allAbsItems, numAbsItems, sizeof(ctp2_ListItem *), cw_comparePollutionItems);
	qsort((void *)allPercentItems, numPercentItems, sizeof(ctp2_ListItem *), cw_comparePollutionItems);

	//put most polluting items at top, so they can be seen when adjusting specialists.
	for(i = numAbsItems-1; i >= 0; i--) {
		m_pollutionList->AddItem(allAbsItems[i]);
	}
	for(i = numPercentItems-1; i >= 0; i--) {
		m_pollutionList->AddItem(allPercentItems[i]);
	}
}

void CityWindow::FillStatsLists()
{
	FillHappinessList();
	FillPollutionList();

	ctp2_Static *crimeBox2 = (ctp2_Static *)aui_Ldl::GetObject(s_cityWindowBlock, "StatisticsSection.CrimePercentage");
	if(crimeBox2) {
		MBCHAR buf[k_MAX_NAME_LEN];
		sprintf(buf, g_theStringDB->GetNameStr("str_ldl_CityWinSpecTabCrimeFormat"),
				sint32(100.0 * m_city.GetCityData()->GetHappyCrime()));
		crimeBox2->SetText(buf);
	}
}

// Not used?
void CityWindow::NotifyUnitChange()
{

	if(!s_cityWindow) return;
	if(!g_c3ui->GetWindow(s_cityWindow->m_window->Id())) return;

	s_cityWindow->UpdateUnitButtons();
}

void CityWindow::NotifyBuildChange(const Unit & city)
{
	if(!s_cityWindow) return;
	if(s_cityWindow->m_city != city) return;

	s_cityWindow->UpdateBuildTabs();
}

void CityWindow::UnitButtonCallback(aui_Control *control, uint32 action, uint32 data, void *cookie)
{
	if(action != AUI_BUTTON_ACTION_EXECUTE) return;

	Assert(s_cityWindow);
	if(!s_cityWindow) return;

	sint32 which = (sint32)cookie;
	ctp2_Button *button = s_cityWindow->m_unitButtons[which];
	Assert(button);
	if(button) {
		button->SetToggleState(!button->GetToggleState());
	}

	s_cityWindow->UpdateActivateButtons();
}

void CityWindow::ActivateUnitCallback(aui_Control *control, uint32 action, uint32 data, void *cookie)
{
	if(action != AUI_BUTTON_ACTION_EXECUTE) return;

	Assert(s_cityWindow);
	if(!s_cityWindow) return;

	sint32 b;
	for(b = 0; b < k_MAX_ARMY_SIZE; b++) {
		if(s_cityWindow->m_unitId[b] != 0) {
			if(s_cityWindow->m_unitButtons[b]->GetToggleState()) {
				Unit u(s_cityWindow->m_unitId[b]);
				Assert(u.IsValid());
				u->CreateOwnArmy();
				if(u.IsValid()) {
					g_gevManager->AddEvent(GEV_INSERT_Tail,
										   GEV_DetrenchOrder,
										   GEA_Army, u.GetArmy().m_id,
										   GEA_End);
				}
				s_cityWindow->m_unitButtons[b]->SetToggleState(false);
			}
		}
	}

	s_cityWindow->UpdateUnitButtons();
}

//----------------------------------------------------------------------------
//
// Name       : CityWindow::DisbandQuery
//
// Description: Disband the units in the selected boxes (when confirmed).
//
// Parameters : result      : the user has confirmed disbanding.
//              ud          : ? (not used)
//
// Returns    : -
//
// Remark(s)  : -
//
//----------------------------------------------------------------------------
void CityWindow::DisbandQuery(bool result, void *ud)
{
	if(result) {

		// Create a temporary army to collect the units from the selected boxes.
		Player *	owner	= g_player[g_selected_item->GetVisiblePlayer()];
		Army		temp(owner->GetNewArmy(CAUSE_NEW_ARMY_GROUPING));

		for (sint32 b = 0; b < k_MAX_ARMY_SIZE; b++)
		{
			if ((s_cityWindow->m_unitId[b] != 0) &&				 // box filled
				s_cityWindow->m_unitButtons[b]->GetToggleState() // box selected
			   )
			{
				Unit u(s_cityWindow->m_unitId[b]);
				Assert(u.IsValid());
				if (u.IsValid())
				{
					// Transfer the unit to the temporary army.
					u.ChangeArmy(temp, CAUSE_NEW_ARMY_GROUPING);

					s_cityWindow->m_unitButtons[b]->SetToggleState(false);
				}
			}
		} // for

		// Perform the actual disbanding.
		g_gevManager->AddEvent(GEV_INSERT_Tail,
							   GEV_DisbandArmyOrder,
							   GEA_Army,
							   temp.m_id,
							   GEA_End
							  );

		// Remove the disbanded units from the display.
		s_cityWindow->UpdateUnitButtons();
		// Update turn count button.
		s_cityWindow->UpdateBuildTabs();
	}
}

void CityWindow::DisbandUnitCallback(aui_Control *control, uint32 action, uint32 data, void *cookie)
{
	if(action != AUI_BUTTON_ACTION_EXECUTE) return;

	Assert(s_cityWindow);
	if(!s_cityWindow) return;

	sint32 b;
	for(b = 0; b < k_MAX_ARMY_SIZE; b++) {
		if(s_cityWindow->m_unitId[b] != 0) {
			if(s_cityWindow->m_unitButtons[b]->GetToggleState()) {
				Unit u(s_cityWindow->m_unitId[b]);
				Assert(u.IsValid());
				if(u.IsValid()) {
					MessageBoxDialog::Query(g_theStringDB->GetNameStr("str_ldl_DisbandSelectedUnits"),
											"QueryDisbandCity",
											DisbandQuery);
					return;
				}
			}
		}
	}

	Assert(FALSE);
}

void CityWindow::NotifyCityCaptured(const Unit &city)
{
	if(!s_cityWindow)
		return;

	EditQueue::NotifyCityCaptured(city);

	bool needToClose = false;
	if (s_cityWindow->m_city.IsValid() && s_cityWindow->m_city == city) {
		needToClose = true;
	}

	if(needToClose) {
		Hide();
	} else if(g_c3ui->GetWindow(s_cityWindow->m_window->Id())) {
		s_cityWindow->Update();
	}
}
