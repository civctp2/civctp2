//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : Handling for the city tab of the control panel 
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
// ACTIVISION_ORIGINAL		
// - When defined, generates the original Activision code.
// - When not defined, generates the modified Apolyton code.
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
//
//----------------------------------------------------------------------------

#include "c3.h"


#include "CityControlPanel.h"


#include <strstream>


#include "aui_Ldl.h"
#include "BuildingRecord.h"
#include "BuildListSequenceRecord.h"
#include "CityData.h"
#include "CityWindow.h"
#include "ctp2_Button.h"
#include "ctp2_DropDown.h"
#include "ctp2_ListItem.h"
#include "ctp2_Static.h"
#include "EditQueue.h"
#include "IconRecord.h"
#include "Player.h"
#include "SelItem.h"
#include "StrDB.h"
#include "UnitRecord.h"
#include "WonderRecord.h"
#include "World.h"
#include "RadarMap.h"
#include "UnitData.h"
#include "TiledMap.h"
#include "BackgroundWin.h"
#include "Director.h"
#include "network.h"
#include "c3ui.h"
#include "aui_blitter.h"

extern C3UI *g_c3ui;

#if !defined(ACTIVISION_ORIGINAL)
namespace
{
	sint32 const CITY_PRODUCTION_HALTED	= 0x7fffffff;
}
#endif

CityControlPanel::CityControlPanel(MBCHAR *ldlBlock) :
m_buildItemLabel(static_cast<ctp2_Static*>(
				 aui_Ldl::GetObject(ldlBlock,
				 "CityTab.TabPanel.BuildProgress.Title"))),
m_buildItemIconButton(static_cast<ctp2_Button*>(
					  aui_Ldl::GetObject(ldlBlock,
					  "CityTab.TabPanel.BuildProgress."
					  "IconBorder.IconButton"))),
m_buildItemProgressBar(static_cast<ctp2_Static *>(
	aui_Ldl::GetObject(ldlBlock,
					   "CityTab.TabPanel.BuildProgress.IconBorder.IconButton.ProgressBarParent.ProgressBar"))),
m_buildItemTurnButton(static_cast<ctp2_Button*>(
					  aui_Ldl::GetObject(ldlBlock,
					  "CityTab.TabPanel.BuildProgress."
					  "IconBorder.IconButton.RadialButton"))),
m_buildRushBuy(static_cast<ctp2_Button*>(
					 aui_Ldl::GetObject(ldlBlock,
					 "CityTab.TabPanel.RushBuyButton"))),
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


void CityControlPanel::PrevCityButtonActionCallback(aui_Control *control,
	uint32 action, uint32 data, void *cookie)
{
	
	if(action != static_cast<uint32>(AUI_BUTTON_ACTION_EXECUTE))
		return;

	
	CityControlPanel *cityControlPanel =
		static_cast<CityControlPanel*>(cookie);

	
	sint32 numberOfItems =
		cityControlPanel->m_cityListDropDown->GetListBox()->NumItems();

	
	
	if(numberOfItems < 2)
		return;

	
	cityControlPanel->m_cityListDropDown->SetSelectedItem(
		(cityControlPanel->m_cityListDropDown->GetSelectedItem() +
		numberOfItems - 1) % numberOfItems);

	
	cityControlPanel->Update();
}


void CityControlPanel::NextCityButtonActionCallback(aui_Control *control,
	uint32 action, uint32 data, void *cookie)
{
	
	if(action != static_cast<uint32>(AUI_BUTTON_ACTION_EXECUTE))
		return;

	
	CityControlPanel *cityControlPanel =
		static_cast<CityControlPanel*>(cookie);

	
	sint32 numberOfItems =
		cityControlPanel->m_cityListDropDown->GetListBox()->NumItems();

	
	
	if(numberOfItems < 2)
		return;

	
	cityControlPanel->m_cityListDropDown->SetSelectedItem(
		(cityControlPanel->m_cityListDropDown->GetSelectedItem() + 1) %
		numberOfItems);

	
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
		EditQueue::Display(CityWindow::GetCityData(player->GetCityFromIndex(selectedItem)));
	}
}

void CityControlPanel::RushBuyBuildButtonActionCallback(aui_Control *control,
	uint32 action, uint32 data, void *cookie)
{
	
	if(action != static_cast<uint32>(AUI_BUTTON_ACTION_EXECUTE))
		return;

	CityData *theCity=((CityControlPanel*)cookie)->GetSelectedCity();
	Assert(theCity);
	if(!theCity)
	{
		return;
	}
	theCity->AddBuyFront();
#if !defined(ACTIVISION_ORIGINAL)
	((CityControlPanel*)cookie)->m_currentTurns = 0;	// Force update of city control panel
#endif
	((CityControlPanel*)cookie)->Update();
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

	
	CityData *cityData = cityControlPanel->GetSelectedCity();
	if(cityData) {
		cityData->SetBuildListSequenceIndex(
			static_cast<ctp2_DropDown*>(control)->GetSelectedItem());
	}

	
	cityControlPanel->UpdateGovernor();
}

CityData *CityControlPanel::GetSelectedCity()
{
	ctp2_ListItem *selItem = (ctp2_ListItem *)m_cityListDropDown->GetListBox()->GetSelectedItem();
	if(!selItem) return NULL;
	Unit u;
	u.m_id = (uint32)selItem->GetUserData();
	if(!u.IsValid()) return NULL;
	return u.CD();
}

void CityControlPanel::CitySelectActionCallback(aui_Control *control,
												uint32 action, uint32 data, void *cookie)
{
	if(action != static_cast<uint32>(AUI_DROPDOWN_ACTION_SELECT))
		return;

	
	CityControlPanel *cityControlPanel =
		static_cast<CityControlPanel*>(cookie);

	CityData *cd = cityControlPanel->GetSelectedCity();
	if(cd) {
		if(g_selected_item) {
			Unit oldCity;
			if(g_selected_item->GetSelectedCity(oldCity) && (oldCity.m_id == cd->GetHomeCity().m_id)) {
				
			} else {
				g_selected_item->SetSelectCity(cd->GetHomeCity());
				MapPoint pos = cd->GetHomeCity().RetPos();




			}
		}
		if(!g_network.IsClient()) {
			CityWindow::Project(cd);
		}
	}
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
// Globals    : g_player		: list of players
//				g_selected_item	: currently selected item
//
// Returns    : -
//
// Remark(s)  : -
//
//----------------------------------------------------------------------------

void CityControlPanel::UpdateBuildItem()
#if defined(ACTIVISION_ORIGINAL)
{
	
	Player *player = g_player[g_selected_item->GetVisiblePlayer()];
	if(!player) {
		ClearBuildItem();
		return;
	}

	
	sint32 numberOfItems =
		m_cityListDropDown->GetListBox()->NumItems();

	
	if(player->GetNumCities() <= 0) {
		ClearBuildItem();
		return;
	}

	
	sint32 city_index = m_cityListDropDown->GetSelectedItem();

	
	if (city_index == -1)
		return; 

	
	
	if(city_index >= player->GetNumCities())
		city_index = 0;
	Unit city = player->GetCityFromIndex(city_index);

	
	BuildQueue *queue = city.GetCityData()->GetBuildQueue();
	BuildNode *head = queue->GetHead();

	sint32 turns;
	if(g_network.IsActive()) {
		turns = city.CD()->HowMuchLonger();
	} else if(g_selected_item->GetCurPlayer() != g_selected_item->GetVisiblePlayer()) {
		turns = m_currentTurns;
	} else {
		turns = city.CD()->HowMuchLonger();
	}

	if((m_currentCity.m_id == city.m_id) &&
	   (m_currentNumItems == numberOfItems) &&
	   (m_currentCategory == (head ? head->m_category : -1)) &&
	   (m_currentItem == (head ? head->m_type : -1)) &&
	   (m_currentTurns == turns)) {
		
		return;
	}

	m_currentCity.m_id = city.m_id;
	m_currentNumItems = numberOfItems;
	m_currentCategory = head ? head->m_category : -1;
	m_currentItem = head ? head->m_type : -1;
	m_currentTurns = turns;

	m_buildItemProgressBar->SetDrawCallbackAndCookie(ProgressDrawCallback, (void *)m_currentCity.m_id);

	
	CityData *theCity=city.CD();
	if(theCity && theCity->GetBuildQueue()->GetHead())
	{
		m_buildRushBuy->Enable(theCity->GetOvertimeCost() <= g_player[g_selected_item->GetVisiblePlayer()]->GetGold() && theCity->HowMuchLonger()>1);
		if(!theCity->AlreadyBoughtFront()) {
			char buf[20];
			sprintf(buf, "%d", theCity->GetOvertimeCost());
			m_rushBuyCost->SetText(buf);
		} else {
			m_rushBuyCost->SetText("---");
		}
	}
	else
	{
		m_buildRushBuy->Enable(false);
		m_rushBuyCost->SetText("---");
	}

	
	
	if(numberOfItems < 1) {
		ClearBuildItem();
		return;
	}

	if(city_index < 0) {
		ClearBuildItem();
		return;
	}
	
	if(m_buildItemIconButton->IsDisabled())
		m_buildItemIconButton->Enable(true);
	if(m_buildItemTurnButton->IsDisabled())
		m_buildItemTurnButton->Enable(true);

	
	if(queue->GetLen() < 1) {
		NoBuildItem();
		return;
	}

	
	MBCHAR tempStr[100];

	strncpy(tempStr, GetBuildName(queue->GetHead()), 99);
	tempStr[99] = 0;
	m_buildItemLabel->GetTextFont()->TruncateString(tempStr, m_buildItemLabel->Width());
	m_buildItemLabel->SetText(tempStr);

	
	const MBCHAR *buildIconName = GetBuildIcon(queue->GetHead());

	
	
	if(buildIconName && strcmp(buildIconName, "NULL")) {
		
		m_buildItemIconButton->SetText("");
		m_buildItemIconButton->ExchangeImage(4, 0, buildIconName);
	} else {
		
		m_buildItemIconButton->SetText("---");
		m_buildItemIconButton->ExchangeImage(4, 0, NULL);
	}

	
	
	
	MBCHAR numTurns[50];
	if (turns == 0x7fffffff)
		sprintf(numTurns, "---");
	else
		sprintf(numTurns, "%d", turns);
	m_buildItemTurnButton->SetText(numTurns); 




}
#else	// ACTIVISION_ORIGINAL
{
	Player *	player	= g_player[g_selected_item->GetVisiblePlayer()];
	if(!player || (player->GetNumCities() <= 0)) 
	{
		ClearBuildItem();
		return;
	}

	sint32	city_index	= m_cityListDropDown->GetSelectedItem();
	if (city_index == -1)
	{
		return; 
	}
	else if (city_index >= player->GetNumCities())
	{
		city_index	= 0;
	}
	
	sint32			numberOfItems	= m_cityListDropDown->GetListBox()->NumItems();
	Unit			city			= player->GetCityFromIndex(city_index);
	CityData *		theCity			= city.CD();
	BuildQueue *	queue			= theCity ? theCity->GetBuildQueue() : NULL;
	BuildNode *		head			= queue ? queue->GetHead() : NULL;

	// Do update rush buy button even when production has not changed.
	if (head &&
	    !theCity->AlreadyBoughtFront() &&
	//Added by Martin Gühmann to disable the rush buy button when
	//capitalization or infrastructure are at the front of the city
	//build queue.
	    (head->m_category != k_GAME_OBJ_TYPE_CAPITALIZATION) &&
	    (head->m_category != k_GAME_OBJ_TYPE_INFRASTRUCTURE)
	)
	{
		// Allow rush buying when the player has enough gold, even when "turns to completion" is 1.
		m_buildRushBuy->Enable(
		        theCity->GetOvertimeCost() <= g_player[g_selected_item->GetVisiblePlayer()]->GetGold()
		        //Added by Martin Gühmann to make sure rush buying is only possible during the players turn.
		        && g_selected_item->GetCurPlayer() == g_selected_item->GetVisiblePlayer());
		char buf[20];
		sprintf(buf, "%d", theCity->GetOvertimeCost());
		m_rushBuyCost->SetText(buf);
	}
	else
	{
		m_buildRushBuy->Enable(false);
		m_rushBuyCost->SetText("---");
	}

	sint32			turns		= m_currentTurns;
	if (g_network.IsActive() ||
	    (g_selected_item->GetCurPlayer() == g_selected_item->GetVisiblePlayer()) 
	   )
	{
		turns = theCity ? theCity->HowMuchLonger() : CITY_PRODUCTION_HALTED;
	}

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

	if(m_buildItemIconButton->IsDisabled())
		m_buildItemIconButton->Enable(true);
	if(m_buildItemTurnButton->IsDisabled())
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
	if(buildIconName && strcmp(buildIconName, "NULL")) {
		
		m_buildItemIconButton->SetText("");
		m_buildItemIconButton->ExchangeImage(4, 0, buildIconName);
	} else {
		
		m_buildItemIconButton->SetText("---");
		m_buildItemIconButton->ExchangeImage(4, 0, NULL);
	}

	MBCHAR numTurns[50];
	//Added by Martin Gühmann to fix the turn count in the case of
	//captalization and infrastructure.
	if (turns == CITY_PRODUCTION_HALTED
	||  head->m_category == k_GAME_OBJ_TYPE_CAPITALIZATION
	||  head->m_category == k_GAME_OBJ_TYPE_INFRASTRUCTURE
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
#endif	// ACTIVISION_ORIGINAL

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


void CityControlPanel::UpdateCityList()
{
	
	m_cityListDropDown->Clear();

	
	Player *player = g_player[g_selected_item->GetVisiblePlayer()];
	if(!player)
		return;

	
	sint32 numberOfCities = player->GetNumCities();

	
	
	if(numberOfCities < 2) {
		m_cityListPreviousButton->Enable(false);
		m_cityListDropDown->Enable(false);
		m_cityListNextButton->Enable(false);
	} else {
		m_cityListPreviousButton->Enable(true);
		m_cityListDropDown->Enable(true);
		m_cityListNextButton->Enable(true);
	}

	
	if(numberOfCities < 1)
		return;

	
	for(sint32 cityIndex = 0; cityIndex < numberOfCities; cityIndex++) {
		
		ctp2_ListItem *listItem = static_cast<ctp2_ListItem*>(
			aui_Ldl::BuildHierarchyFromRoot("CityListItem"));

		
		ctp2_Static *label = static_cast<ctp2_Static*>(
			listItem->GetChildByIndex(0));
		label->SetText(player->GetCityFromIndex(cityIndex).GetName());

		
		listItem->SetUserData(reinterpret_cast<void*>(player->GetCityFromIndex(cityIndex).m_id));

		
		m_cityListDropDown->AddItem(listItem);
	}
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

	
	return(NULL);
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
	if(!g_selected_item->GetSelectedCity(newCity))
		return;

	CityData *oldCityData = GetSelectedCity();
	if(oldCityData && oldCityData->GetHomeCity().m_id == newCity.m_id) {
		
		if(!g_network.IsClient()) {
			CityWindow::Project(oldCityData);
		}
		return;
	}

	sint32 numberOfItems = m_cityListDropDown->GetListBox()->NumItems();

	sint32 i;
	for(i = 0; i < numberOfItems; i++) {
		ctp2_ListItem *item = static_cast<ctp2_ListItem*>(m_cityListDropDown->GetListBox()->GetItemByIndex(i));
		if(item) {
			Unit thisCity; thisCity.m_id = (uint32)item->GetUserData();
			if(thisCity.m_id == newCity.m_id) {
				m_cityListDropDown->SetSelectedItem(i);
				break;
			}
		}
	}
}

void CityControlPanel::Activated()
{
	Unit city;
	if(g_selected_item->GetSelectedCity(city))
		return;

	
	
	if(!g_selected_item)
		return;

	
	Player *player = g_player[g_selected_item->GetVisiblePlayer()];

	
	if(!player)
		return;

	
	ID id;
	PLAYER_INDEX playerIndex;
	SELECT_TYPE selectionType;
	g_selected_item->GetTopCurItem(playerIndex, id,
		selectionType);

	
  	if(selectionType != SELECT_TYPE_NONE) {
		
		
		Unit city = g_theWorld->GetCity(g_selected_item->GetCurSelectPos());
		if(city.IsValid()) {
			g_selected_item->SetSelectCity(city);
		}
	}
}

AUI_ERRCODE CityControlPanel::ProgressDrawCallback(ctp2_Static *control,
												   aui_Surface *surface, 
												   RECT &rect, 
												   void *cookie)
{
	Unit city; city.m_id = (uint32)cookie;

	g_c3ui->TheBlitter()->ColorBlt(surface, &rect, RGB(0,0,0), 0);

	if(!city.IsValid() || !city.CD()->GetBuildQueue()->GetHead()) {
		return AUI_ERRCODE_OK;
	}

	
	double storedProd = double(city.CD()->GetStoredCityProduction());
	double neededProd = double(city.CD()->GetBuildQueue()->GetHead()->m_cost);
	
	double percentComplete;
	if(neededProd == 0) {
		percentComplete = 1.0;
	} else {
		percentComplete = storedProd / neededProd;
	}

	if(percentComplete > 1.0)
		percentComplete = 1.0;

	RECT destRect = rect;
	destRect.right = destRect.left + percentComplete * (destRect.right - destRect.left);

	g_c3ui->TheBlitter()->ColorBlt(surface, &destRect, RGB(0,0,255), 0);
	return AUI_ERRCODE_OK;
}

