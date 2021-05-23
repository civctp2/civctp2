//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// File name    : \UI\Interface\CityControlPanel.cpp
// Description  : Handling for the city tab of the control panel
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
// - Made rush buy button behaviour consistent with other windows.
// - Disabled rushbuy button if infrastructure or captalization are
//   at the front of the build queue, by Martin Gühmann.
// - If infrastructure or capitalization are at the front of the
//   build queue no cost or turns are shown anymore, by Martin Gühmann.
// - Rush buy button should be disabled when it is not the player's turn
//   unfortunatly the button state is not updated on the end turn event.
// - Made update of rush buy button possible when (only) the gold of the
//   player has changed.
// - #01 Standardization of city selection and focus handling
//   (L. Hirth 6/2004)
// - Disabled ForceSelect while updating the city list. (Feb 4th 2007 Martin Gühmann)
// - Cleaned and made the build progress bar green. (Feb 4th 2007 Martin Gühmann)
// - The city tab is now updated when you modify the city in the city manager. (9-Feb-2008 Martin Gühmann)
//
//----------------------------------------------------------------------------

#include "c3.h"
#include "CityControlPanel.h"

#include <strstream>

#include "aui_ldl.h"
#include "BuildingRecord.h"
#include "BuildListSequenceRecord.h"
#include "citydata.h"
#include "citywindow.h"
#include "ctp2_button.h"
#include "ctp2_dropdown.h"
#include "ctp2_listitem.h"
#include "ctp2_Static.h"
#include "EditQueue.h"
#include "IconRecord.h"
#include "player.h"
#include "SelItem.h"
#include "StrDB.h"
#include "UnitRecord.h"
#include "WonderRecord.h"
#include "World.h"
#include "colorset.h"
#include "radarmap.h"
#include "UnitData.h"
#include "tiledmap.h"
#include "backgroundwin.h"
#include "director.h"
#include "network.h"
#include "c3ui.h"
#include "screenutils.h"
#include "NationalManagementDialog.h"
#include "Globals.h"                        // k_GAME_OBJ_TYPE_...

extern C3UI *g_c3ui;

namespace
{
	sint32 const CITY_PRODUCTION_HALTED	= 0x7fffffff;
}

CityControlPanel::CityControlPanel(MBCHAR *ldlBlock) :
m_buildItemLabel(static_cast<ctp2_Static*>(
				 aui_Ldl::GetObject(ldlBlock,
				 "CityTab.TabPanel.BuildProgress.Title"))),
m_buildItemIconButton(static_cast<ctp2_Button*>(
					  aui_Ldl::GetObject(ldlBlock,
					  "CityTab.TabPanel.BuildProgress."
					  "IconBorder.IconButton"))),
m_buildItemTurnButton(static_cast<ctp2_Button*>(
					  aui_Ldl::GetObject(ldlBlock,
					  "CityTab.TabPanel.BuildProgress."
					  "IconBorder.IconButton.RadialButton"))),
m_buildRushBuy(static_cast<ctp2_Button*>(
					 aui_Ldl::GetObject(ldlBlock,
					 "CityTab.TabPanel.RushBuyButton"))),
m_buildItemProgressBar(static_cast<ctp2_Static *>(
	aui_Ldl::GetObject(ldlBlock,
	"CityTab.TabPanel.BuildProgress.IconBorder.IconButton.ProgressBarParent.ProgressBar"))),
m_rushBuyCost(static_cast<ctp2_Static *>(
	aui_Ldl::GetObject(ldlBlock, "CityTab.TabPanel.RushBuyCost"))),
m_cityListPreviousButton(static_cast<ctp2_Button*>(
						 aui_Ldl::GetObject(ldlBlock,
						 "CityTab.TabPanel.CitySelect.Previous"))),
m_cityListDropDown(static_cast<ctp2_DropDown*>(
				   aui_Ldl::GetObject(ldlBlock,
				   "CityTab.TabPanel.CitySelect.Pulldown"))),
m_cityListNextButton(static_cast<ctp2_Button*>(
					 aui_Ldl::GetObject(ldlBlock,
					 "CityTab.TabPanel.CitySelect.Next"))),
m_governorToggleButton(static_cast<ctp2_Button*>(
						 aui_Ldl::GetObject(ldlBlock,
						 "CityTab.TabPanel.GovernorSelect.Toggle"))),
m_governorDropDown(static_cast<ctp2_DropDown*>(
				   aui_Ldl::GetObject(ldlBlock,
				   "CityTab.TabPanel.GovernorSelect.Pulldown")))
{

	Assert(m_buildItemLabel);
	Assert(m_buildItemIconButton);
	Assert(m_buildItemTurnButton);
	Assert(m_buildRushBuy);
	Assert(m_cityListPreviousButton);
	Assert(m_cityListDropDown);
	Assert(m_cityListNextButton);
	Assert(m_governorToggleButton);
	Assert(m_governorDropDown);
	Assert(m_buildItemProgressBar);

	m_buildItemIconButton->SetActionFuncAndCookie(
		EditBuildQueueButtonActionCallback, this);
	m_buildItemTurnButton->SetActionFuncAndCookie(
		EditBuildQueueButtonActionCallback, this);
	m_buildRushBuy->SetActionFuncAndCookie(
		RushBuyBuildButtonActionCallback, this);
	m_cityListPreviousButton->SetActionFuncAndCookie(
		PrevCityButtonActionCallback, this);
	m_cityListNextButton->SetActionFuncAndCookie(
		NextCityButtonActionCallback, this);
	m_governorToggleButton->SetActionFuncAndCookie(
		ToggleGovernorButtonActionCallback, this);
	m_governorDropDown->SetActionFuncAndCookie(
		SelectGovernorActionCallback, this);
	m_cityListDropDown->SetActionFuncAndCookie(
		CitySelectActionCallback, this);

	m_currentCity.m_id = 0;
	m_currentNumItems = -1;
	m_currentCategory = -1;
	m_currentItem = -1;
	m_currentTurns = -1;

	m_useGovernor = false;
	m_currentGovernor = -1;

	m_buildItemIconButton->Enable(false);
	m_buildItemTurnButton->Enable(false);
	m_cityListPreviousButton->Enable(false);
	m_cityListDropDown->Enable(false);
	m_cityListNextButton->Enable(false);
	m_governorToggleButton->Enable(false);
	m_governorDropDown->Enable(false);

	m_buildItemProgressBar->SetDrawCallbackAndCookie(ProgressDrawCallback, 0);

	m_governorDropDown->Clear();

	for(sint32 governorIndex = 0; governorIndex <
		g_theBuildListSequenceDB->NumRecords(); governorIndex++) {

		ctp2_ListItem *listItem = static_cast<ctp2_ListItem*>(
			aui_Ldl::BuildHierarchyFromRoot("GovernorListItem"));

		ctp2_Static *label = static_cast<ctp2_Static*>(
			listItem->GetChildByIndex(0));
		label->SetText(
			g_theBuildListSequenceDB->Get(
			governorIndex)->GetNameText());

		m_governorDropDown->AddItem(listItem);
	}
	if(m_governorDropDown->IsDisabled())
	{
		m_governorDropDown->SetSelectedItem(-1);
	}
}

void CityControlPanel::Update()
{
	UpdateBuildItem();
	UpdateGovernor();
}

void CityControlPanel::PrevCityButtonActionCallback(aui_Control * control, uint32 action, uint32 data, void * cookie)
{
	if (action != static_cast<uint32>(AUI_BUTTON_ACTION_EXECUTE)) {
		return;
	}

	CityControlPanel * cityControlPanel = static_cast<CityControlPanel*>(cookie);
	sint32 numberOfItems = cityControlPanel->m_cityListDropDown->GetListBox()->NumItems();
	if (numberOfItems < 2) {
		return;
	}

	cityControlPanel->m_cityListDropDown->SetSelectedItem(
			(cityControlPanel->m_cityListDropDown->GetSelectedItem() + numberOfItems - 1) % numberOfItems);
	cityControlPanel->Update();
}

void CityControlPanel::NextCityButtonActionCallback(aui_Control * control, uint32 action, uint32 data, void * cookie)
{
	if (action != static_cast<uint32>(AUI_BUTTON_ACTION_EXECUTE)) {
		return;
	}

	CityControlPanel * cityControlPanel = static_cast<CityControlPanel*>(cookie);
	sint32 numberOfItems = cityControlPanel->m_cityListDropDown->GetListBox()->NumItems();
	if(numberOfItems < 2) {
		return;
	}

	cityControlPanel->m_cityListDropDown->SetSelectedItem(
		(cityControlPanel->m_cityListDropDown->GetSelectedItem() + 1) % numberOfItems);
	cityControlPanel->Update();
}

void CityControlPanel::EditBuildQueueButtonActionCallback(aui_Control *control,
	uint32 action, uint32 data, void *cookie)
{

	if(action != static_cast<uint32>(AUI_BUTTON_ACTION_EXECUTE))
		return;

	Player *player = g_player[g_selected_item->GetVisiblePlayer()];
	if(!player)
		return;

	CityControlPanel *cityControlPanel =
		static_cast<CityControlPanel*>(cookie);

	sint32 numberOfItems =
		cityControlPanel->m_cityListDropDown->GetListBox()->NumItems();


	if(numberOfItems < 1)
		return;

	sint32 selectedItem = cityControlPanel->m_cityListDropDown->GetSelectedItem();
	if (selectedItem >= numberOfItems)
		selectedItem = numberOfItems - 1;

	if(g_network.IsClient() && g_network.GetSensitiveUIBlocked()) {
	} else {
		close_AllScreens();
		EditQueue::Display(player->GetCityFromIndex(selectedItem));
	}
}

void CityControlPanel::RushBuyBuildButtonActionCallback(aui_Control * control, uint32 action, uint32 data,
		void * cookie)
{
	if (action != static_cast<uint32>(AUI_BUTTON_ACTION_EXECUTE)) {
		return;
	}

	Unit selectedCity;
	if (!((CityControlPanel*)cookie)->GetSelectedCity(selectedCity)) {
		return;
	}

	if (!selectedCity.GetCityData()->AlreadyBoughtFront())
	{
		selectedCity.GetCityData()->AddBuyFront();
		((CityControlPanel *) cookie)->m_currentTurns = 0; // Force update of city control panel
		((CityControlPanel *) cookie)->Update();
		CityWindow::UpdateCity(selectedCity);
		EditQueue::UpdateCity(selectedCity);
		NationalManagementDialog::UpdateCity(selectedCity);
	}
}

void CityControlPanel::ToggleGovernorButtonActionCallback(aui_Control *control,
	uint32 action, uint32 data, void *cookie)
{

	if(action != static_cast<uint32>(AUI_BUTTON_ACTION_EXECUTE))
		return;

	Player *player = g_player[g_selected_item->GetVisiblePlayer()];
	if(!player)
		return;

	CityControlPanel *cityControlPanel =
		static_cast<CityControlPanel*>(cookie);

	sint32 numberOfItems =
		cityControlPanel->m_cityListDropDown->GetListBox()->NumItems();


	if(numberOfItems < 1)
		return;

	sint32 selIndex = cityControlPanel->m_cityListDropDown->GetSelectedItem();
	if(selIndex < 0 || selIndex >= player->m_all_cities->Num())
		return;

	CityData *cityData =
		player->GetCityFromIndex(
		cityControlPanel->m_cityListDropDown->GetSelectedItem()
		).GetCityData();
	cityData->SetUseGovernor(!cityData->GetUseGovernor());

	cityControlPanel->UpdateGovernor();
}

void CityControlPanel::SelectGovernorActionCallback(aui_Control *control,
	uint32 action, uint32 data, void *cookie)
{

	if(action != static_cast<uint32>(AUI_DROPDOWN_ACTION_SELECT))
		return;

	if(!g_selected_item)
		return;
	Player *player = g_player[g_selected_item->GetVisiblePlayer()];
	if(!player)
		return;

	CityControlPanel *cityControlPanel =
		static_cast<CityControlPanel*>(cookie);

	sint32 numberOfItems =
		cityControlPanel->m_cityListDropDown->GetListBox()->NumItems();


	if(numberOfItems < 1)
		return;

	Unit selectedCity;
	if(cityControlPanel->GetSelectedCity(selectedCity)) {
		selectedCity.GetCityData()->SetBuildListSequenceIndex(static_cast<ctp2_DropDown*>(control)->GetSelectedItem());
	}

	cityControlPanel->UpdateGovernor();
}

bool CityControlPanel::GetSelectedCity(Unit & selectedCity)
{
	ctp2_ListItem * selItem = (ctp2_ListItem *)m_cityListDropDown->GetListBox()->GetSelectedItem();
	if (!selItem) {
		return false;
	}

	selectedCity.m_id = (uint32)selItem->GetUserData();
	return selectedCity.IsValid();
}

//----------------------------------------------------------------------------
//
// Name       : CityControlPanel::CitySelectActionCallback
//
// Description: Will be called when a city is selected on the control panel
//              city list.
//
//----------------------------------------------------------------------------
void CityControlPanel::CitySelectActionCallback(aui_Control * control, uint32 action, uint32 data, void * cookie)
{
	if (action != static_cast<uint32>(AUI_DROPDOWN_ACTION_SELECT)) {
		return;
	}

	if (!g_selected_item) {
		return;
	}

	CityControlPanel * cityControlPanel = static_cast<CityControlPanel*>(cookie);
	Unit newSelectedCity;
	if (!cityControlPanel->GetSelectedCity(newSelectedCity)) {
		return;
	}

	Unit currentSelectedCity;
	if (!g_selected_item->GetSelectedCity(currentSelectedCity) || (currentSelectedCity != newSelectedCity)) {
		g_selected_item->SetSelectCity(newSelectedCity);
	}
	CityWindow::SelectCity(newSelectedCity);
	EditQueue::SelectCity(newSelectedCity);
}

//----------------------------------------------------------------------------
//
// Name       : CityControlPanel::UpdateBuildItem
//
// Description: Updates all the data concerning city build items of the
//              city tab including rush buy button, turn button rush buy
//              costs.
//
// Parameters : -
//
// Globals    : g_player        : list of players
//              g_selected_item : currently selected item
//
// Returns    : -
//
// Remark(s)  : -
//
//----------------------------------------------------------------------------
void CityControlPanel::UpdateBuildItem()
{
	sint32 const visiblePlayer = g_selected_item->GetVisiblePlayer();
	Player * player = g_player[visiblePlayer];
	if (!player || (player->GetNumCities() <= 0))
	{
		ClearBuildItem();
		return;
	}

	sint32 city_index = m_cityListDropDown->GetSelectedItem();
	if (city_index == -1) {
		return;
	}
	else if (city_index >= player->GetNumCities())
	{
		city_index	= 0;
	}

	Unit       city     = player->GetCityFromIndex(city_index);
	CityData * cityData = city.GetCityData();

	sint32       numberOfItems = m_cityListDropDown->GetListBox()->NumItems();
	BuildQueue * queue         = cityData ? cityData->GetBuildQueue() : NULL;
	BuildNode  * head          = queue ? queue->GetHead() : NULL;
	sint32 const cost          = cityData ? cityData->GetOvertimeCost() : 0;
	bool const   isMyTurn      = visiblePlayer == g_selected_item->GetCurPlayer();

	// Do update the rush buy button, even when the production has not changed.
	if ((cost <= 0) ||
	    cityData->AlreadyBoughtFront() ||
	    cityData->IsBuildingCapitalization() ||
	    cityData->IsBuildingInfrastructure()
	   )
	{
		m_rushBuyCost->SetText("---");
		m_buildRushBuy->Enable(false);
	}
	else
	{
		char buf[20];
		sprintf(buf, "%d", cost);
		m_rushBuyCost->SetText(buf);
		m_buildRushBuy->Enable(isMyTurn && (cost <= player->GetGold()));
	}

	sint32 turns = cityData ? cityData->HowMuchLonger() : CITY_PRODUCTION_HALTED;

	if ((m_currentCity.m_id == city.m_id) &&
	    (m_currentNumItems == numberOfItems) &&
		(m_currentCategory == (head ? head->m_category : -1)) &&
	    (m_currentItem == (head ? head->m_type : -1)) &&
	    (m_currentTurns == turns)
	   )
	{
		return;
	}

	m_currentCity.m_id	= city.m_id;
	m_currentNumItems	= numberOfItems;
	m_currentCategory	= head ? head->m_category : -1;
	m_currentItem		= head ? head->m_type : -1;
	m_currentTurns		= turns;
	m_buildItemProgressBar->SetDrawCallbackAndCookie
		(ProgressDrawCallback, (void *) m_currentCity.m_id);

	if(numberOfItems < 1) {
		ClearBuildItem();
		return;
	}

	m_buildItemIconButton->Enable(true);
	m_buildItemTurnButton->Enable(true);

	if(queue->GetLen() < 1) {
		NoBuildItem();
		return;
	}

	MBCHAR tempStr[100];
	strncpy(tempStr, GetBuildName(head), 99);
	tempStr[99] = 0;
	m_buildItemLabel->GetTextFont()->TruncateString(tempStr, m_buildItemLabel->Width());
	m_buildItemLabel->SetText(tempStr);

	const MBCHAR *buildIconName = GetBuildIcon(head);
	if (buildIconName && strcmp(buildIconName, "NULL"))
	{
		m_buildItemIconButton->SetText("");
		m_buildItemIconButton->ExchangeImage(4, 0, buildIconName);
	}
	else
	{
		m_buildItemIconButton->SetText("---");
		m_buildItemIconButton->ExchangeImage(4, 0, NULL);
	}

	MBCHAR numTurns[50];
	if ((turns == CITY_PRODUCTION_HALTED) ||
	    cityData->AlreadyBoughtFront() ||
	    cityData->IsBuildingCapitalization() ||
	    cityData->IsBuildingInfrastructure()
	   )
	{
		sprintf(numTurns, "---");
	}
	else
	{
		sprintf(numTurns, "%d", turns);
	}
	m_buildItemTurnButton->SetText(numTurns);
}

void CityControlPanel::NoBuildItem()
{
	m_buildItemLabel->SetText(
		g_theStringDB->GetNameStr("str_ldl_CP_BUILD_QUEUE_EMPTY"));
	m_buildItemIconButton->ExchangeImage(4, 0, NULL);
	m_buildItemIconButton->SetText(
		g_theStringDB->GetNameStr("str_ldl_CP_EDIT_BUILD_QUEUE"));
	m_buildItemTurnButton->SetText("-");
}

void CityControlPanel::ClearBuildItem()
{
	if(m_currentItem >= -1) {
		m_buildItemLabel->SetText("");
		m_buildItemIconButton->ExchangeImage(4, 0, NULL);
		m_buildItemIconButton->SetText("");
		m_buildItemTurnButton->SetText("");

		m_buildItemIconButton->Enable(false);
		m_buildItemTurnButton->Enable(false);
		m_currentItem = -2;
	}
}

void CityControlPanel::UpdateGovernor()
{

	Player *player = g_player[g_selected_item->GetVisiblePlayer()];

	sint32 numberOfItems =
		m_cityListDropDown->GetListBox()->NumItems();

	if (!player || numberOfItems < 1 || player->GetNumCities() <= 0) {
		if(!m_governorToggleButton->IsDisabled()) {

			m_governorToggleButton->Enable(false);
			m_governorToggleButton->SetText("");
		}

		if(!m_governorDropDown->IsDisabled()) {

			m_governorDropDown->Enable(false);
			m_governorDropDown->SetSelectedItem(-1);
		}
		return;
	}


	if(m_governorToggleButton->IsDisabled()) {

		m_governorToggleButton->Enable(true);
	}

	sint32 city_index = m_cityListDropDown->GetSelectedItem();
	if (city_index >= player->GetNumCities())
		city_index = 0;

	if(city_index < 0)
		return;

	Unit city = player->GetCityFromIndex(city_index);

	if(city.GetCityData()->GetUseGovernor()) {
		if(!m_useGovernor) {

			m_governorToggleButton->SetText("X");
			m_useGovernor = true;
		}

		if(m_governorDropDown->IsDisabled()) {

			m_governorDropDown->Enable(true);
		}

		if(m_currentGovernor != city.GetCityData()->GetBuildListSequenceIndex()) {
			m_governorDropDown->SetSelectedItem(
				city.GetCityData()->GetBuildListSequenceIndex());
			m_currentGovernor = city.GetCityData()->GetBuildListSequenceIndex();
		}
	} else {

		if(m_useGovernor) {
			m_governorToggleButton->SetText("");
			m_useGovernor = false;
		}

		if(!m_governorDropDown->IsDisabled())

			m_governorDropDown->Enable(false);

		if(m_currentGovernor >= 0) {
			m_governorDropDown->SetSelectedItem(-1);
			m_currentGovernor = -1;
		}
	}
}

//----------------------------------------------------------------------------
//
// Name       : CityControlPanel::UpdateCityList
//
// Description: Clears and refills the dropdown selection list of cities
//              located on the CityControlPanel
//
// Parameters : -
//
// Globals    : g_player		: list of players
//
// Returns    : -
//
// Remark(s)  : -
//
//----------------------------------------------------------------------------
void CityControlPanel::UpdateCityList()
{
	// Clear the list
	m_cityListDropDown->Clear();

	// Set the player
	Player *player = g_player[g_selected_item->GetVisiblePlayer()];
	if(!player)
		return;

	// How many cities has player
	sint32 numberOfCities = player->GetNumCities();

	// Deactivate prev and next button if less than 2 cities
	// activate if there are more than one city
	if(numberOfCities < 2)
	{
		m_cityListPreviousButton->Enable(false);
		m_cityListDropDown->Enable(false);
		m_cityListNextButton->Enable(false);
	}
	else
	{
		m_cityListPreviousButton->Enable(true);
		m_cityListDropDown->Enable(true);
		m_cityListNextButton->Enable(true);
	}

	// If no city exists, nothing more to do
	if(numberOfCities < 1)
		return;

	// Loop throgh the cities to fill the drop down list
	m_cityListDropDown->SetForceSelect(false);
	for(sint32 cityIndex = 0; cityIndex < numberOfCities; cityIndex++)
	{
		// Create the item
		ctp2_ListItem *listItem = static_cast<ctp2_ListItem*>(
			aui_Ldl::BuildHierarchyFromRoot("CityListItem"));

		// Set the cityname as text
		ctp2_Static *label = static_cast<ctp2_Static*>(
			listItem->GetChildByIndex(0));
		label->SetText(player->GetCityFromIndex(cityIndex).GetName());

		// Fill userdata of the dropdown list with the city ID
		listItem->SetUserData(reinterpret_cast<void*>(player->GetCityFromIndex(cityIndex).m_id));

		// Add the item to the list
		m_cityListDropDown->AddItem(listItem);
	}

	m_cityListDropDown->SetForceSelect(true);
}

const MBCHAR *CityControlPanel::GetBuildName(const BuildNode *buildNode)
{

	Assert(buildNode);

	return g_theStringDB->GetNameStr(GetBuildStringId(buildNode));
}

StringId CityControlPanel::GetBuildStringId(const BuildNode *buildNode)
{

	Assert(buildNode);

	sint32 id;

	switch(buildNode->m_category) {
		case k_GAME_OBJ_TYPE_UNIT:
			return(g_theUnitDB->Get(buildNode->m_type)->GetName());
			break;
		case k_GAME_OBJ_TYPE_IMPROVEMENT:
			return(g_theBuildingDB->Get(buildNode->m_type)->GetName());
			break;
		case k_GAME_OBJ_TYPE_WONDER:
			return(g_theWonderDB->Get(buildNode->m_type)->GetName());
			break;
		case k_GAME_OBJ_TYPE_CAPITALIZATION:
			g_theStringDB->GetStringID("CAPITALIZATION", id);
			return id;
		case k_GAME_OBJ_TYPE_INFRASTRUCTURE:
			g_theStringDB->GetStringID("INFRASTRUCTURE", id);
			return id;
		default:
			Assert(false);
			break;
	}

	return 0;
}

const MBCHAR *CityControlPanel::GetBuildIcon(const BuildNode *buildNode)
{

	Assert(buildNode);

	const MBCHAR *largeIcon = NULL;

	switch(buildNode->m_category) {
		case k_GAME_OBJ_TYPE_UNIT:
			largeIcon = g_theUnitDB->Get(
				buildNode->m_type)->GetDefaultIcon()->GetLargeIcon();
			break;
		case k_GAME_OBJ_TYPE_IMPROVEMENT:
			largeIcon = g_theBuildingDB->Get(
				buildNode->m_type)->GetDefaultIcon()->GetIcon();
			break;
		case k_GAME_OBJ_TYPE_WONDER:
			largeIcon = g_theWonderDB->Get(
				buildNode->m_type)->GetDefaultIcon()->GetIcon();
			break;
		case k_GAME_OBJ_TYPE_INFRASTRUCTURE:
		{
			sint32 index;
			if(g_theIconDB->GetNamedItem("ICON_IMPROVE_INFRASTRUCTURE", index)) {
				largeIcon = g_theIconDB->Get(index)->GetIcon();
			} else {
				largeIcon = NULL;
			}
			break;
		}
		case k_GAME_OBJ_TYPE_CAPITALIZATION:
		{
			sint32 index;
			if(g_theIconDB->GetNamedItem("ICON_IMPROVE_CAPITALIZATION", index)) {
				largeIcon = g_theIconDB->Get(index)->GetIcon();
			} else {
				largeIcon = NULL;
			}
			break;
		}

		default:
			Assert(false);
			break;
	}

	return(largeIcon);
}

void CityControlPanel::SelectedCity()
{
	Unit newCity;
	if (!g_selected_item->GetSelectedCity(newCity)) {
		return;
	}

	Unit currentCity;
	if (GetSelectedCity(currentCity) && currentCity == newCity)
	{
		if (!g_network.IsClient()) {
			CityWindow::Project(newCity);
		}
		return;
	}

	sint32 numberOfItems = m_cityListDropDown->GetListBox()->NumItems();
	for(sint32 i = 0; i < numberOfItems; i++)
	{
		ctp2_ListItem * item = static_cast<ctp2_ListItem*>(m_cityListDropDown->GetListBox()->GetItemByIndex(i));
		if (item && (uint32)item->GetUserData() == newCity.m_id)
		{
			m_cityListDropDown->SetSelectedItem(i);
			break;
		}
	}
}

void CityControlPanel::Activated()
{
	if (!g_selected_item) {
		return;
	}
	Unit city;
	if (g_selected_item->GetSelectedCity(city)) {
		return;
	}
	Player * player = g_player[g_selected_item->GetVisiblePlayer()];
	if (!player) {
		return;
	}

	ID id;
	PLAYER_INDEX playerIndex;
	SELECT_TYPE selectionType;
	g_selected_item->GetTopCurItem(playerIndex, id, selectionType);

	if (selectionType != SELECT_TYPE_NONE)
	{
		Unit city = g_theWorld->GetCity(g_selected_item->GetCurSelectPos());
		if (city.IsValid())
		{
			g_selected_item->SetSelectCity(city);
		}
	}
}

AUI_ERRCODE CityControlPanel::ProgressDrawCallback(ctp2_Static *control,
												   aui_Surface *surface,
												   RECT &rect,
												   void *cookie)
{
	g_c3ui->TheBlitter()->ColorBlt(surface, &rect, RGB(0,0,0), 0);

	Unit city;
	city.m_id = (uint32)cookie;
	if (!city.IsValid()) {
		return AUI_ERRCODE_OK;
	}
	CityData * cityData = city.GetCityData();
	if (!cityData || !cityData->GetBuildQueue()->GetHead()) {
		return AUI_ERRCODE_OK;
	}

	double storedProd = double(cityData->GetStoredCityProduction());
	double neededProd = 0.0;
	if (cityData->GetBuildQueue()->GetHead() && !cityData->AlreadyBoughtFront()) {
		neededProd = double(cityData->GetBuildQueue()->GetHead()->m_cost);
	}

	double percentComplete = (neededProd == 0) ? 1.0 : storedProd / neededProd;
	if(percentComplete > 1.0) {
		percentComplete = 1.0;
	}

	RECT destRect = rect;

	destRect.top += 1;
	destRect.bottom -= 1;
	destRect.left += 1;
	destRect.right -= 1;

	destRect.right = destRect.left + static_cast<LONG>(percentComplete * (destRect.right - destRect.left));
	g_c3ui->TheBlitter()->ColorBlt(surface, &destRect, RGB(0,255,0), 0);
	return AUI_ERRCODE_OK;
}
