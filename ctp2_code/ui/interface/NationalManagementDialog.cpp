//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : Behaviour of the National Management dialog
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
// - Added extra checks on the rush buy code to fix the infinite rush buy bug,
//   and also altered the code that adds up the total rush buy cost so that
//   it displays the correct value.  John Bytheway, late 2003
// - Disabled rush buy button when it is not the players turn by John Bytheway.
// - No rush buy costs are displayed if all selected items are capitalization 
//   or infrastructure, by Martin Gühmann.
// - No turn numbers aren't shown anymore in the listbox if the build item is
//   infrastructure or capitalization, by Martin Gühmann.
// - Rush buy button is disabled when it is not the players turn, 
//   by Martin Gühmann.
// - #01 Added a third tab to the dialog that shows the nuber of experts and 
//   military units in each city.
//   (L. Hirth 6/2004)    
// - #02 // #02 Fixed sorting sequence for governor type in status tab
//   (L. Hirth 7/2004)     
//----------------------------------------------------------------------------

#include "c3.h"


#include "NationalManagementDialog.h"


#include "aui_Ldl.h"
#include "BuildingRecord.h"
#include "BuildListSequenceRecord.h"
#include "c3ui.h"
#include "CityControlPanel.h"
#include "CityData.h"
#include "ctp2_Button.h"
#include "ctp2_DropDown.h"
#include "ctp2_ListBox.h"
#include "ctp2_ListItem.h"
#include "ctp2_Static.h"
#include "ctp2_Window.h"
#include "ctp2_Tab.h"

#include "GameEventManager.h"
#include "MessageBoxDialog.h"
#include "Player.h"
#include "SelItem.h"
#include "StrDB.h"
#include "UnitRecord.h"
#include "WonderRecord.h"
#include "EditQueue.h"
#include "CityWindow.h"

#include "Director.h"
#include "ConstDB.h"
#include "ColorSet.h"

#include "network.h"

extern ConstDB *g_theConstDB ;
extern ColorSet	*g_colorSet;


class BuildQueueDropdownItem {
public:
	
	BuildQueueDropdownItem(uint32 category, sint32 type) {
		m_category = category;
		m_type = type;
	}

	
	uint32 m_category;

	
	sint32 m_type;
};


static const sint32 k_NMD_RES_CITY_NAME		= 0;
static const sint32 k_NMD_RES_POPULATION	= 1;
static const sint32 k_NMD_RES_HAPPINESS		= 2;
static const sint32 k_NMD_RES_FOOD			= 3;
static const sint32 k_NMD_RES_PRODUCTION	= 4;
static const sint32 k_NMD_RES_GOLD			= 5;
static const sint32 k_NMD_RES_SCIENCE		= 6;
static const sint32 k_NMD_RES_POLLUTION     = 7;
static const sint32 k_NMD_RES_CRIME         = 8;
static const sint32 k_NMD_STAT_CITY_NAME	= 0;
static const sint32 k_NMD_STAT_MAYOR		= 1;
static const sint32 k_NMD_STAT_PRIORITY		= 2;
static const sint32 k_NMD_STAT_BUILDING		= 3;
static const sint32 k_NMD_STAT_TIME			= 4;
#ifndef ACTIVISION_ORIGINAL
static const sint32 k_NMD_SPEC_CITY_NAME	= 0;
static const sint32 k_NMD_SPEC_POPULATION	= 1;
static const sint32 k_NMD_SPEC_WORKER		= 2;
static const sint32 k_NMD_SPEC_SLAVE		= 3;
static const sint32 k_NMD_SPEC_ENTERTAINER	= 4;
static const sint32 k_NMD_SPEC_FARMER		= 5;
static const sint32 k_NMD_SPEC_LABORER		= 6;
static const sint32 k_NMD_SPEC_MERCHANT		= 7;
static const sint32 k_NMD_SPEC_SCIENTIST	= 8;
static const sint32 k_NMD_SPEC_COMBAT_UNITS	= 9;
#endif

extern C3UI *g_c3ui;


NationalManagementDialog *g_nationalManagementDialog = NULL;


void NationalManagementDialog::Open()
{
	
	if(!g_nationalManagementDialog) {
		g_nationalManagementDialog = new NationalManagementDialog;
	}

	
	g_nationalManagementDialog->Show();
	
	g_nationalManagementDialog->UpdateResourceList();
	g_nationalManagementDialog->UpdateStatusList();
#ifndef ACTIVISION_ORIGINAL	// #01 Added a third tab to the dialog
	g_nationalManagementDialog->UpdateSpecialistList();
#endif
	g_nationalManagementDialog->UpdateBuildQueue();
	g_nationalManagementDialog->UpdateGovernor();
	g_nationalManagementDialog->UpdateRushBuy();

#ifdef ACTIVISION_ORIGINAL // #01 Added a third tab to the dialog
	if(g_nationalManagementDialog->m_resourceList->IsHidden()) {
		g_nationalManagementDialog->UpdateMainButtons(g_nationalManagementDialog->m_statusList);
	} else {
		g_nationalManagementDialog->UpdateMainButtons(g_nationalManagementDialog->m_resourceList);
	} 
#else
	if(!g_nationalManagementDialog->m_resourceList->IsHidden()) {
		g_nationalManagementDialog->UpdateMainButtons(g_nationalManagementDialog->m_resourceList);
	} else if (!g_nationalManagementDialog->m_statusList->IsHidden()) {
		g_nationalManagementDialog->UpdateMainButtons(g_nationalManagementDialog->m_statusList);
	} else if (!g_nationalManagementDialog->m_specialistList->IsHidden()) {
		g_nationalManagementDialog->UpdateMainButtons(g_nationalManagementDialog->m_specialistList);
	}
#endif
}

void NationalManagementDialog::Close()
{
	
	if(g_nationalManagementDialog)
		g_nationalManagementDialog->Hide();
}

void NationalManagementDialog::Cleanup()
{
	
	if(g_nationalManagementDialog) {
		if(!g_nationalManagementDialog->m_window->IsHidden())
			g_nationalManagementDialog->Hide();

		aui_Ldl::DeleteHierarchyFromRoot("CityStatusWin");
	}

	
	delete g_nationalManagementDialog;
	g_nationalManagementDialog = NULL;
}

#if !defined (ACTIVISION_ORIGINAL) // #01 Added a third tab to the dialog
bool NationalManagementDialog::IsShown()
{
    if (!g_nationalManagementDialog) {
		return false;
	} else {
		return  !g_nationalManagementDialog->m_window->IsHidden();
	}
}
#endif

//----------------------------------------------------------------------------
//
// Name       : NationalManagementDialog::NationalManagementDialog()
//
// Description: Creates the National Managment Window
//               
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
NationalManagementDialog::NationalManagementDialog() :
//
// Create the Elements of the dialog
//
m_window(static_cast<ctp2_Window*>(aui_Ldl::BuildHierarchyFromRoot(
	"CityStatusWin"))),
m_closeButton(static_cast<ctp2_Button*>(aui_Ldl::GetObject(
	"CityStatusWin.CloseButton"))),
m_resourceList(static_cast<ctp2_ListBox*>(aui_Ldl::GetObject(
	"CityStatusWin.TabGroup.Tab1.TabPanel.ResourceList"))),
m_statusList(static_cast<ctp2_ListBox*>(aui_Ldl::GetObject(
	"CityStatusWin.TabGroup.Tab2.TabPanel.StatusList"))),

#if !defined (ACTIVISION_ORIGINAL) // #01 Added a third tab to the dialog
m_specialistList(static_cast<ctp2_ListBox*>(aui_Ldl::GetObject(
	"CityStatusWin.TabGroup.Tab3.TabPanel.SpecialistList"))),
#endif

	
m_governorToggle(static_cast<ctp2_Button*>(aui_Ldl::GetObject(
	"CityStatusWin.TabGroup.Tab2.TabPanel.GovernorSelect.Toggle"))),
m_governorDropDown(static_cast<ctp2_DropDown*>(aui_Ldl::GetObject(
	"CityStatusWin.TabGroup.Tab2.TabPanel.GovernorSelect.Pulldown"))),


m_rushBuyButton(static_cast<ctp2_Button*>(aui_Ldl::GetObject(
	"CityStatusWin.TabGroup.Tab2.TabPanel.RushBuyButton"))),
m_rushBuyValue(static_cast<ctp2_Static*>(aui_Ldl::GetObject(
	"CityStatusWin.TabGroup.Tab2.TabPanel.RushBuyValue"))),
m_buildQueueButton(static_cast<ctp2_Button*>(aui_Ldl::GetObject(
	"CityStatusWin.BuildQueueButton"))),
m_disbandButton(static_cast<ctp2_Button*>(aui_Ldl::GetObject(
	"CityStatusWin.DisbandButton"))),
m_resourceTab(static_cast<ctp2_Tab*>(aui_Ldl::GetObject(
	"CityStatusWin.TabGroup.Tab1"))),

m_statusTab(static_cast<ctp2_Tab*>(aui_Ldl::GetObject(
	"CityStatusWin.TabGroup.Tab2")))
#if !defined (ACTIVISION_ORIGINAL) // #01 Added a third tab to the dialog
	,
m_specialistTab(static_cast<ctp2_Tab*>(aui_Ldl::GetObject(
	"CityStatusWin.TabGroup.Tab3")))
#endif
{
	g_c3ui->AddWindow(m_window);

	
	Assert(m_closeButton);
	Assert(m_resourceList);
	Assert(m_statusList);
#if !defined (ACTIVISION_ORIGINAL) // #01 Added a third tab to the dialog
	Assert(m_specialistList);
#endif
	Assert(m_governorToggle);
	Assert(m_governorDropDown);

	Assert(m_rushBuyButton);
	Assert(m_rushBuyValue);
	Assert(m_buildQueueButton);
	Assert(m_disbandButton);

	
	m_resourceList->SetMultiSelect(true);		// Allow the selction of multiple
	m_statusList->SetMultiSelect(true);         // items in the lists  
#if !defined (ACTIVISION_ORIGINAL) // #01 Added a third tab to the dialog
	m_specialistList->SetMultiSelect(true);         
#endif	


	
	m_closeButton->SetActionFuncAndCookie(CloseButtonActionCallback, this);
	m_statusList->SetActionFuncAndCookie(StatusListSelectActionCallback, this);
	m_governorToggle->SetActionFuncAndCookie(ToggleGovernorButtonActionCallback, this);
	m_governorDropDown->SetActionFuncAndCookie(SelectGovernorActionCallback, this);

	m_rushBuyButton->SetActionFuncAndCookie(RushBuyButtonActionCallback, this);
	m_buildQueueButton->SetActionFuncAndCookie(BuildQueueButtonActionCallback, this);
	m_disbandButton->SetActionFuncAndCookie(DisbandButtonActionCallback, this);
	m_resourceList->SetActionFuncAndCookie(ResourceListSelectActionCallback, this);

#if !defined (ACTIVISION_ORIGINAL) // #01 Added a third tab to the dialog
	m_specialistList->SetActionFuncAndCookie(SpecialistListSelectActionCallback, this);
#endif
	
	m_resourceTab->SetActionFuncAndCookie(TabActionCallback, this);
	m_statusTab->SetActionFuncAndCookie(TabActionCallback, this);
#if !defined (ACTIVISION_ORIGINAL) // #01 Added a third tab to the dialog
	m_specialistTab->SetActionFuncAndCookie(TabActionCallback, this);
#endif

	m_governorDropDown->Clear();

	
	for(sint32 governorIndex = 0; governorIndex <
		g_theBuildListSequenceDB->NumRecords(); governorIndex++) {
		
		ctp2_ListItem *listItem = static_cast<ctp2_ListItem*>(
			aui_Ldl::BuildHierarchyFromRoot("NationalGovernorListItem"));

		
		ctp2_Static *label = static_cast<ctp2_Static*>(
			listItem->GetChildByIndex(0));
		label->SetText(
			g_theBuildListSequenceDB->Get(
			governorIndex)->GetNameText());

		
		m_governorDropDown->AddItem(listItem);
	}
	m_mirroring = false;
}


void NationalManagementDialog::Show()
{
	g_c3ui->AddWindow(m_window);
	m_window->Show();
}


void NationalManagementDialog::Hide()
{
	m_window->Hide();
	g_c3ui->RemoveWindow(m_window->Id());
}

void NationalManagementDialog::Update()
{
	UpdateResourceList();
	UpdateStatusList();
#if !defined (ACTIVISION_ORIGINAL) // #01 Added a third tab to the dialog
	UpdateSpecialistList();
#endif

}


void NationalManagementDialog::UpdateResourceList()
{
	
	m_resourceList->BuildListStart();

	
	m_resourceList->Clear();

	
	UnitDynamicArray *cityList =
		g_player[g_selected_item->GetVisiblePlayer()]->GetAllCitiesList();

	
	for(sint32 cityIndex = 0; cityIndex < cityList->Num(); cityIndex++) {
		
		Unit city = cityList->Get(cityIndex);

		
		m_resourceList->AddItem(CreateResourceItem(city));
	}

	
	m_resourceList->BuildListEnd();
}


void NationalManagementDialog::UpdateStatusList()
{
	
	m_statusList->BuildListStart();

	
	m_statusList->Clear();

	
	UnitDynamicArray *cityList =
		g_player[g_selected_item->GetVisiblePlayer()]->GetAllCitiesList();

	
	for(sint32 cityIndex = 0; cityIndex < cityList->Num(); cityIndex++) {
		
		Unit city = cityList->Get(cityIndex);

		
		m_statusList->AddItem(CreateStatusItem(city));
	}

	
	m_statusList->BuildListEnd();
}

#if !defined (ACTIVISION_ORIGINAL) // #01 Added a third tab to the dialog
//----------------------------------------------------------------------------
//
// Name       : NationalManagementDialog::UpdateSpecialistList
//
// Description: Fills the list of the specialist tabs
//               
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
void NationalManagementDialog::UpdateSpecialistList()
{
	
	m_specialistList->BuildListStart();

	
	m_specialistList->Clear();

	
	UnitDynamicArray *cityList =
		g_player[g_selected_item->GetVisiblePlayer()]->GetAllCitiesList();

	
	for(sint32 cityIndex = 0; cityIndex < cityList->Num(); cityIndex++) {
		
		Unit city = cityList->Get(cityIndex);

		
		m_specialistList->AddItem(CreateSpecialistItem(city));
	}

	
	m_specialistList->BuildListEnd();
}
#endif

void NationalManagementDialog::UpdateGovernor()
{
	
	bool governorStatus = false;

	
	sint32 governorPriority = -1;

	
	tech_WLList<sint32> *selectedList = m_statusList->GetSelectedList();

	
	if( selectedList->L() < 1 )
	{
		
		m_governorToggle->SetText("");

		
		m_governorToggle->Enable( FALSE );

		
		m_governorDropDown->SetSelectedItem( governorPriority );

		
		m_governorDropDown->Enable( FALSE );

		
		return;
	}

	else
	
	{
		
		m_governorToggle->Enable( TRUE );
	}

	
	for( int selectIndex = 0; selectIndex < selectedList->L(); selectIndex++) 
	{
		
		Unit city;
		city.m_id = reinterpret_cast<uint32>(
			static_cast<ctp2_ListItem*>(m_statusList->GetItemByIndex(
			selectedList->GetAtIndex(selectIndex)
			))->GetUserData());

		
		
		

		
		if( selectIndex == 0 ) 
		{
			governorStatus = ( city.GetCityData()->GetUseGovernor() == TRUE );
			governorPriority = city.GetCityData()->GetBuildListSequenceIndex();
		}
		
		
		else
		{
			
			governorStatus = governorStatus && city.GetCityData()->GetUseGovernor();

			
			if( city.GetCityData()->GetBuildListSequenceIndex() != governorPriority )
			{
				
				governorPriority = -1;
			}
		} 
	}
		
	
	if( governorStatus )
	{
		
		m_governorDropDown->SetSelectedItem( governorPriority );

		
		m_governorDropDown->Enable(TRUE);

		
		m_governorToggle->SetText("X");
	}
	else
	
	{
		
		m_governorDropDown->SetSelectedItem( -1 );

		
		m_governorDropDown->Enable(FALSE);

		
		m_governorToggle->SetText("");
	}


}


void NationalManagementDialog::UpdateBuildQueue()
{
#if 0
	
	for(sint32 index = 0; index <
		m_buildDropDown->GetListBox()->NumItems(); index++) {
		
		ctp2_ListItem *item =
			static_cast<ctp2_ListItem*>(
			m_buildDropDown->GetListBox()->GetItemByIndex(index));

		
		if(item) {
			
			delete static_cast<BuildQueueDropdownItem*>(item->GetUserData());

			
			item->SetUserData(NULL);
		}
	}

	
	m_buildDropDown->BuildListStart();

	
	m_buildDropDown->Clear();

	
	for(index = 0; index < g_theUnitDB->NumRecords(); index++) {
		if(CanBuild(k_GAME_OBJ_TYPE_UNIT, index))
			m_buildDropDown->AddItem(
			CreateBuildQueueItem(k_GAME_OBJ_TYPE_UNIT, index));
	}

	
	for(index = 0; index < g_theBuildingDB->NumRecords(); index++) {
		if(CanBuild(k_GAME_OBJ_TYPE_IMPROVEMENT, index))
			m_buildDropDown->AddItem(
			CreateBuildQueueItem(k_GAME_OBJ_TYPE_IMPROVEMENT, index));
	}

	
	for(index = 0; index < g_theWonderDB->NumRecords(); index++) {
		if(CanBuild(k_GAME_OBJ_TYPE_WONDER, index))
			m_buildDropDown->AddItem(
			CreateBuildQueueItem(k_GAME_OBJ_TYPE_WONDER, index));
	}

	
	m_buildDropDown->BuildListEnd();

	
	tech_WLList<sint32> *selectedList = m_statusList->GetSelectedList();

	if(selectedList->L() < 1) {
		m_buildDropDown->Enable(FALSE);
	} else {
		m_buildDropDown->Enable(TRUE);
	}
#endif
}

//----------------------------------------------------------------------------
//
// Name       : NationalManagementDialog::UpdateRushBuy
//
// Description: Updates the status of the rush buy button, and calculates 
//              the costs of all selected items.
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
void NationalManagementDialog::UpdateRushBuy()
{
	sint32 rushBuyTotal = 0;
	
	tech_WLList<sint32> *selectedList = m_statusList->GetSelectedList();
	
	for(int selectIndex = 0; selectIndex < selectedList->L(); selectIndex++) {
		
		Unit city;
		city.m_id = reinterpret_cast<uint32>(
			static_cast<ctp2_ListItem*>(m_statusList->GetItemByIndex(
			selectedList->GetAtIndex(selectIndex)
			))->GetUserData());

#if defined(ACTIVISION_ORIGINAL)
		if(city.GetCityData()->GetBuildQueue()->GetLen())
				rushBuyTotal += city.GetCityData()->GetOvertimeCost();
#else
		CityData *		theCity	= city.GetCityData();
		sint32 const	cost	= theCity ? theCity->GetOvertimeCost() : 0;

		if ((cost <= 0)									||
			(theCity->GetBuildQueue()->GetLen() == 0)	||
			theCity->AlreadyBoughtFront()				||
			theCity->IsBuildingCapitalization()			||
			theCity->IsBuildingInfrastructure()
		   )
		{
			// No action: nothing to rush buy in this city
		}
		else
		{
			rushBuyTotal += cost;
		}
#endif
	}

	
#if defined(ACTIVISION_ORIGINAL)

	if((rushBuyTotal <= 0) ||
		(rushBuyTotal > g_player[g_selected_item->GetVisiblePlayer()]->GetGold()))
		m_rushBuyButton->Enable(false);
	else
		m_rushBuyButton->Enable(true);

	
	static MBCHAR stringBuffer[32];
	sprintf(stringBuffer, "%d", rushBuyTotal);
	m_rushBuyValue->SetText(stringBuffer);

#else
	// Extra conditions to prevent buying out of turn.
	sint32 const	player	= g_selected_item->GetVisiblePlayer();

	m_rushBuyButton->Enable((rushBuyTotal > 0)								&&
						    (rushBuyTotal <= g_player[player]->GetGold())	&&
							(player == g_selected_item->GetCurPlayer())
						   );

	if (rushBuyTotal <= 0)
	{
		m_rushBuyValue->SetText("---");
	}
	else
	{
		static MBCHAR stringBuffer[32];
		sprintf(stringBuffer, "%d", rushBuyTotal);
		m_rushBuyValue->SetText(stringBuffer);
	}
#endif
}


ctp2_Static *NationalManagementDialog::GetListItemColumn(ctp2_ListItem *item,
														 sint32 column)
{
	return(static_cast<ctp2_Static*>(item->GetChildByIndex(column)));
}


ctp2_ListItem *NationalManagementDialog::CreateResourceItem(const Unit &city)
{
	
	ctp2_ListItem *item = static_cast<ctp2_ListItem*>(
		aui_Ldl::BuildHierarchyFromRoot("NationalResourceListItem"));

	
	Assert(item);
	if(!item)
		return(NULL);

	
	item->SetUserData(reinterpret_cast<void*>(city.m_id));

	
	item->SetCompareCallback(CompareResources);

	
	UpdateResourceItem(item, city);

	
	return(item);
}


void NationalManagementDialog::UpdateResourceItem(ctp2_ListItem *item,
												  const Unit &city)
{
	const COLORREF colorNorm = g_colorSet->GetColorRef(COLOR_BLACK);
	const COLORREF colorCritical = g_colorSet->GetColorRef(COLOR_RED);

	
	static MBCHAR stringBuffer[32];

	
	CityData *cityData = new CityData(city.GetCityData());

	
	CityWindow::Project(cityData);

	BOOL cityCritical = false;
	
	if(ctp2_Static *column = GetListItemColumn(item, k_NMD_RES_POPULATION)) {
		sint32 population = 0;
		cityData->GetPop(population);
		sprintf(stringBuffer, "%d", population);
		column->SetText(stringBuffer);
		column->SetTextColor(colorNorm);
	}

	
	if(ctp2_Static *column = GetListItemColumn(item, k_NMD_RES_HAPPINESS)) {
		sint32 happiness = (sint32)(cityData->GetHappiness());
		sprintf(stringBuffer, "%d", happiness);
		column->SetText(stringBuffer);
		if (happiness < g_theConstDB->GetRiotLevel()) {
			column->SetTextColor(colorCritical);
			cityCritical = true;
		} else
			column->SetTextColor(colorNorm);
	}

	
	if(ctp2_Static *column = GetListItemColumn(item, k_NMD_RES_PRODUCTION)) {
		sprintf(stringBuffer, "%d", (sint32)(cityData->GetGrossCityProduction()));
		column->SetText(stringBuffer);
		column->SetTextColor(colorNorm);
	}

	
	if(ctp2_Static *column = GetListItemColumn(item, k_NMD_RES_FOOD)) {
		sint32 food = cityData->GetGrossCityFood();
		sprintf(stringBuffer, "%d", food);
		column->SetText(stringBuffer);
		if (food < cityData->GetFoodRequired()) {
			column->SetTextColor(colorCritical);
			cityCritical = true;
		} else
			column->SetTextColor(colorNorm);
	}

	
	if(ctp2_Static *column = GetListItemColumn(item, k_NMD_RES_GOLD)) {
		sprintf(stringBuffer, "%d", (sint32)(cityData->GetNetCityGold()));
		column->SetText(stringBuffer);
		column->SetTextColor(colorNorm);
	}

	
	if(ctp2_Static *column = GetListItemColumn(item, k_NMD_RES_SCIENCE)) {
		sprintf(stringBuffer, "%d", cityData->GetScience());
		column->SetText(stringBuffer);
		column->SetTextColor(colorNorm);
	}

	
	if(ctp2_Static *column = GetListItemColumn(item, k_NMD_RES_POLLUTION)) {
		sint32 pollution = cityData->GetPollution();
		sprintf(stringBuffer, "%d", pollution);
		column->SetText(stringBuffer);
		if (pollution > g_theConstDB->LocalPollutionLevel()) {
			column->SetTextColor(colorCritical);
			cityCritical = true;
		} else
			column->SetTextColor(colorNorm);
	}

	
	if(ctp2_Static *column = GetListItemColumn(item, k_NMD_RES_CRIME)) {
		sprintf(stringBuffer, "%d", (sint32)(cityData->GetHappyCrime() * 100.0));
		column->SetText(stringBuffer);
		column->SetTextColor(colorNorm);
	}

	
	if(ctp2_Static *column = GetListItemColumn(item, k_NMD_RES_CITY_NAME)) {
		column->SetText(cityData->GetName());
		if (cityCritical)
			column->SetTextColor(colorCritical);
		else
			column->SetTextColor(colorNorm);
	}

	delete cityData; 
}


ctp2_ListItem *NationalManagementDialog::CreateStatusItem(const Unit &city)
{
	
	ctp2_ListItem *item = static_cast<ctp2_ListItem*>(
		aui_Ldl::BuildHierarchyFromRoot("NationalStatusListItem"));

	
	Assert(item);
	if(!item)
		return(NULL);

	
	item->SetUserData(reinterpret_cast<void*>(city.m_id));

	
	item->SetCompareCallback(CompareStatus);

	
	UpdateStatusItem(item, city);

	
	return(item);
}


//----------------------------------------------------------------------------
//
// Name       : NationalManagementDialog::UpdateStatusItem
//
// Description: Updates the status of items including
//              the number of turns in the list box.
//
// Parameters : ctp2_ListItem *item
//              const Unit &city
//
// Globals    : -
//
// Returns    : -
//
// Remark(s)  : -
//
//----------------------------------------------------------------------------
void NationalManagementDialog::UpdateStatusItem(ctp2_ListItem *item,
												const Unit &city)
{
	
	
	CityData *cityData = new CityData(city.GetCityData());
	CityWindow::Project(cityData);

	
	if(ctp2_Static *column = GetListItemColumn(item, k_NMD_STAT_CITY_NAME)) {
		column->SetText(cityData->GetName());
	}

	
	if(ctp2_Static *column = GetListItemColumn(item, k_NMD_STAT_MAYOR)) {
		column->SetText(cityData->GetUseGovernor() ?
			g_theStringDB->GetNameStr("str_ldl_ND_ON") : g_theStringDB->GetNameStr("str_ldl_ND_OFF"));
	
	}

	
	if(ctp2_Static *column = GetListItemColumn(item, k_NMD_STAT_PRIORITY)) {

		
		if( cityData->GetUseGovernor() ) 
		{
			column->SetText(g_theBuildListSequenceDB->Get(
				cityData->GetBuildListSequenceIndex())->GetNameText());
		}
		else 
		{
			column->SetText("");
		}
	}

	
	BuildQueue *queue = cityData->GetBuildQueue();

	
	if(ctp2_Static *column = GetListItemColumn(item, k_NMD_STAT_BUILDING)) {
		column->SetText(queue->GetLen() ?
			CityControlPanel::GetBuildName(queue->GetHead()) : g_theStringDB->GetNameStr("str_ldl_ND_EMPTY"));
	}

	
	if(ctp2_Static *column = GetListItemColumn(item, k_NMD_STAT_TIME)) {
		
		if(queue->GetLen()) {
			static MBCHAR stringBuffer[32];
			sint32 turns = cityData->HowMuchLonger();
#if defined(ACTIVISION_ORIGINAL)
			//Removed by Martin Gühmann
			if (turns == 0x7fffffff)
#else
			if (turns == 0x7fffffff
			//Added by Martin Gühmann to disable the turn display in case of capitalization and infrastructure
			|| cityData->GetBuildQueue()->GetHead()->m_category == k_GAME_OBJ_TYPE_CAPITALIZATION
			|| cityData->GetBuildQueue()->GetHead()->m_category == k_GAME_OBJ_TYPE_INFRASTRUCTURE)
#endif
				sprintf(stringBuffer, "---");
			else
				sprintf(stringBuffer, "%d", turns);
			column->SetText(stringBuffer);
		} else
			column->SetText(g_theStringDB->GetNameStr("str_ldl_ND_NA"));
	}
	delete cityData;
}


#if !defined (ACTIVISION_ORIGINAL) // #01 Added a third tab to the dialog
//----------------------------------------------------------------------------
//
// Name       : NationalManagementDialog::CreateSpecialistItem
//
// Description: Creates an item of the specialist list
//               
//
// Parameters : Unit:	Cityunit from which the data is taken 
//
// Globals    : -
//
// Returns    : ctp2_ListItem: The item that was set up
//
// Remark(s)  : -
//
//----------------------------------------------------------------------------
ctp2_ListItem *NationalManagementDialog::CreateSpecialistItem(const Unit &city)
{
	
	ctp2_ListItem *item = static_cast<ctp2_ListItem*>(
		aui_Ldl::BuildHierarchyFromRoot("NationalSpecialistListItem"));

	
	Assert(item);
	if(!item)
		return(NULL);

	
	item->SetUserData(reinterpret_cast<void*>(city.m_id));

	
	item->SetCompareCallback(CompareSpecialists);

	
	UpdateSpecialistItem(item, city);

	
	return(item);
}

//----------------------------------------------------------------------------
//
// Name       : NationalManagementDialog::UpdateSpecialistItem
//
// Description: Updates the different columns of one item of the specialist list
//               
//
// Parameters : *item:	The Item to update
//				&city	The city from which the data will be taken
//
// Globals    : -
//
// Returns    : -
//
// Remark(s)  : -
//
//----------------------------------------------------------------------------
void NationalManagementDialog::UpdateSpecialistItem(ctp2_ListItem *item,
												  const Unit &city)
{
	const COLORREF colorNorm = g_colorSet->GetColorRef(COLOR_BLACK);
	const COLORREF colorCritical = g_colorSet->GetColorRef(COLOR_RED);

	
	static MBCHAR stringBuffer[32];

	
	CityData *cityData = new CityData(city.GetCityData());

	
	CityWindow::Project(cityData);

	BOOL cityCritical = false;
	
	if(ctp2_Static *column = GetListItemColumn(item, k_NMD_SPEC_POPULATION)) {
		sint32 population = 0;
		cityData->GetPop(population);
		sprintf(stringBuffer, "%d", population);
		column->SetText(stringBuffer);
		column->SetTextColor(colorNorm);
	}

	if(ctp2_Static *column = GetListItemColumn(item, k_NMD_SPEC_WORKER)) {
		sprintf(stringBuffer, "%d", (sint32)(cityData->WorkerCount()));
		column->SetText(stringBuffer);
		column->SetTextColor(colorNorm);
	}

	
	if(ctp2_Static *column = GetListItemColumn(item, k_NMD_SPEC_SLAVE)) {
		sprintf(stringBuffer, "%d", (sint32)(cityData->SlaveCount()));
		column->SetText(stringBuffer);
		column->SetTextColor(colorNorm);
	}

	
	if(ctp2_Static *column = GetListItemColumn(item, k_NMD_SPEC_ENTERTAINER)) {
		sprintf(stringBuffer, "%d", (sint32)(cityData->EntertainerCount()));
		column->SetText(stringBuffer);
		column->SetTextColor(colorNorm);
	}

	
	if(ctp2_Static *column = GetListItemColumn(item, k_NMD_SPEC_FARMER)) {
		sprintf(stringBuffer, "%d", cityData->FarmerCount());
		column->SetText(stringBuffer);
		column->SetTextColor(colorNorm);
	}

	
	if(ctp2_Static *column = GetListItemColumn(item, k_NMD_SPEC_LABORER)) {
		sprintf(stringBuffer, "%d", cityData->LaborerCount());
		column->SetText(stringBuffer);
		column->SetTextColor(colorNorm);
	}

	
	if(ctp2_Static *column = GetListItemColumn(item, k_NMD_SPEC_MERCHANT)) {
		sprintf(stringBuffer, "%d", (sint32)(cityData->MerchantCount()));
		column->SetText(stringBuffer);
		column->SetTextColor(colorNorm);
	}

	
	if(ctp2_Static *column = GetListItemColumn(item, k_NMD_SPEC_SCIENTIST)) {
		sprintf(stringBuffer, "%d", (sint32)(cityData->ScientistCount()));
		column->SetText(stringBuffer);
		column->SetTextColor(colorNorm);
	}

	if(ctp2_Static *column = GetListItemColumn(item, k_NMD_SPEC_COMBAT_UNITS)) {
		sprintf(stringBuffer, "%d", (sint32)(cityData->GetCombatUnits()));
		column->SetText(stringBuffer);
		column->SetTextColor(colorNorm);
	}

	if(ctp2_Static *column = GetListItemColumn(item, k_NMD_SPEC_CITY_NAME)) {
		column->SetText(cityData->GetName());
		if (cityCritical)
			column->SetTextColor(colorCritical);
		else
			column->SetTextColor(colorNorm);
	}
	delete cityData; 
}
#endif



bool NationalManagementDialog::CanBuild(uint32 category, sint32 type)
{
	
	tech_WLList<sint32> *selectedList = m_statusList->GetSelectedList();

	
	if(!selectedList->L())
		return(false);

	
	
	bool canBuild = true;

	
	
	for(int selectIndex = 0; selectIndex < selectedList->L(); selectIndex++) {
		
		Unit city;
		city.m_id = reinterpret_cast<uint32>(
			static_cast<ctp2_ListItem*>(m_statusList->GetItemByIndex(
			selectedList->GetAtIndex(selectIndex)
			))->GetUserData());

		
		switch(category) {
		case k_GAME_OBJ_TYPE_UNIT:
			canBuild = canBuild &&
				city.GetCityData()->CanBuildUnit(type);
			break;
		case k_GAME_OBJ_TYPE_IMPROVEMENT:
			canBuild = canBuild &&
				city.GetCityData()->CanBuildBuilding(type);
			break;
		case k_GAME_OBJ_TYPE_WONDER:
			canBuild = canBuild &&
				city.GetCityData()->CanBuildWonder(type);
			break;
		default:
			Assert(false);
			break;
		}
	}

	
	return(canBuild);
}


ctp2_ListItem *NationalManagementDialog::CreateBuildQueueItem(uint32 category,
															  sint32 type)
{
	
	ctp2_ListItem *listItem = static_cast<ctp2_ListItem*>(
		aui_Ldl::BuildHierarchyFromRoot("NationalBuildQueueListItem"));

	
	listItem->SetUserData(new BuildQueueDropdownItem(category, type));

	
	ctp2_Static *label = static_cast<ctp2_Static*>(
		listItem->GetChildByIndex(0));
	switch(category) {
	case k_GAME_OBJ_TYPE_UNIT:
		label->SetText(g_theUnitDB->Get(type)->GetNameText());
		break;
	case k_GAME_OBJ_TYPE_IMPROVEMENT:
		label->SetText(g_theBuildingDB->Get(type)->GetNameText());
		break;
	case k_GAME_OBJ_TYPE_WONDER:
		label->SetText(g_theWonderDB->Get(type)->GetNameText());
		break;
	default:
		Assert(false);
		break;
	}

	
	return(listItem);
}


sint32 NationalManagementDialog::CompareResources(ctp2_ListItem *item1,
												  ctp2_ListItem *item2,
												  sint32 column)
{
	
	Unit city1, city2;
	city1.m_id = reinterpret_cast<uint32>(item1->GetUserData());
	city2.m_id = reinterpret_cast<uint32>(item2->GetUserData());

	
	Assert(city1.IsValid());
	Assert(city2.IsValid());

	CityData *cityData1 = city1.GetCityData();
	CityData *cityData2 = city2.GetCityData();

	sint32 result = 0;

	ctp2_Static *column1 = GetListItemColumn(item1, column);
	ctp2_Static *column2 = GetListItemColumn(item2, column);

	
	switch(column) {
		case k_NMD_RES_CITY_NAME:
			result =(stricmp(column1->GetText(), column2->GetText()));
			break;
		case k_NMD_RES_POPULATION:
		case k_NMD_RES_HAPPINESS:
		case k_NMD_RES_PRODUCTION:
		case k_NMD_RES_FOOD:
		case k_NMD_RES_GOLD:
		case k_NMD_RES_SCIENCE:
		case k_NMD_RES_POLLUTION:
		case k_NMD_RES_CRIME:
			result = atoi(column1->GetText()) - atoi(column2->GetText());
			break;
		
        
#if 0
			{
				sint32 population1 = 0, population2 = 0;
				cityData1->GetPop(population1);
				cityData2->GetPop(population2);
				result =(population1 - population2);
			}
			break;
		case k_NMD_RES_HAPPINESS:
			result =(cityData1->GetHappiness() -
				cityData2->GetHappiness());
			break;
		case k_NMD_RES_PRODUCTION:
			result =(cityData1->GetGrossCityProduction() -
				cityData2->GetGrossCityProduction());
			break;
		case k_NMD_RES_FOOD:
			result =(cityData1->GetGrossCityFood() -
				cityData2->GetGrossCityFood());
			break;
		case k_NMD_RES_GOLD:
			result =(cityData1->GetGrossCityGold() -
				cityData2->GetGrossCityGold());
			break;
		case k_NMD_RES_SCIENCE:
			result =(cityData1->GetScience() -
				cityData2->GetScience());
			break;
		case k_NMD_RES_POLLUTION:
			result =(cityData1->GetPollution() -
				   cityData2->GetPollution());
			break;
		case k_NMD_RES_CRIME:
			result =(sint32(cityData1->GetHappyCrime() * 100.0) -
				   sint32(cityData2->GetHappyCrime() * 100.0));
			break;
#endif

		default:
			Assert(false);
			result =(0);
	}

	return result;
}


sint32 NationalManagementDialog::CompareStatus(ctp2_ListItem *item1,
											   ctp2_ListItem *item2,
											   sint32 column)
{
	
	Unit city1, city2;
	city1.m_id = reinterpret_cast<uint32>(item1->GetUserData());
	city2.m_id = reinterpret_cast<uint32>(item2->GetUserData());

	
	Assert(city1.IsValid());
	Assert(city2.IsValid());

	
	CityData *cityData1 = city1.GetCityData();
	CityData *cityData2 = city2.GetCityData();

	
	BuildQueue *queue1 = cityData1->GetBuildQueue();
	BuildQueue *queue2 = cityData2->GetBuildQueue();

	
	switch(column) {
		case k_NMD_STAT_CITY_NAME:
			return(stricmp(cityData1->GetName(), cityData2->GetName()));
		case k_NMD_STAT_MAYOR:
			return(cityData1->GetUseGovernor() ?
				(cityData2->GetUseGovernor() ? 0 : 1) :
				(cityData2->GetUseGovernor() ? -1 : 0));
		case k_NMD_STAT_PRIORITY:
#ifdef ACTIVISION_ORIGINAL // #02 Fixed sorting sequence for governor type in status tab
			return(stricmp(
				g_theBuildListSequenceDB->Get(
				cityData1->GetBuildListSequenceIndex())->GetNameText(),
				g_theBuildListSequenceDB->Get(
				cityData2->GetBuildListSequenceIndex())->GetNameText()));
#else
			return(stricmp(
				cityData1->GetUseGovernor() ?
				g_theBuildListSequenceDB->Get(cityData1->GetBuildListSequenceIndex())->GetNameText() : " ",
				cityData2->GetUseGovernor() ?
				g_theBuildListSequenceDB->Get(cityData2->GetBuildListSequenceIndex())->GetNameText() : " "));
#endif
		case k_NMD_STAT_BUILDING:
			return(stricmp(
				queue1->GetLen() ?
				CityControlPanel::GetBuildName(queue1->GetHead()) : g_theStringDB->GetNameStr("str_ldl_ND_EMPTY"),
				queue2->GetLen() ?
				CityControlPanel::GetBuildName(queue2->GetHead()) : g_theStringDB->GetNameStr("str_ldl_ND_EMPTY")));
		case k_NMD_STAT_TIME:
			return((queue1->GetLen() ? cityData1->HowMuchLonger() : -1) -
				(queue2->GetLen() ? cityData2->HowMuchLonger() : -1));
		default:
			Assert(false);
			return(0);
	}
}


#if !defined (ACTIVISION_ORIGINAL) // #01 Added a third tab to the dialog
//----------------------------------------------------------------------------
//
// Name       : NationalManagementDialog::CompareSpecialists
//
// Description: compares a specific column of two list items 
//               
//
// Parameters : *item1:	The first item for the compare
//				*item2:	The second item for the compare
//				column	The column that must be compared
//
// Globals    : -
//
// Returns    : -
//
// Remark(s)  : -
//
//----------------------------------------------------------------------------
sint32 NationalManagementDialog::CompareSpecialists(ctp2_ListItem *item1,
												  ctp2_ListItem *item2,
												  sint32 column)
{
	
	Unit city1, city2;
	city1.m_id = reinterpret_cast<uint32>(item1->GetUserData());
	city2.m_id = reinterpret_cast<uint32>(item2->GetUserData());

	
	Assert(city1.IsValid());
	Assert(city2.IsValid());

	CityData *cityData1 = city1.GetCityData();
	CityData *cityData2 = city2.GetCityData();

	sint32 result = 0;

	ctp2_Static *column1 = GetListItemColumn(item1, column);
	ctp2_Static *column2 = GetListItemColumn(item2, column);

	
	switch(column) {
		case k_NMD_RES_CITY_NAME:
			result =(stricmp(column1->GetText(), column2->GetText()));
			break;
		case k_NMD_SPEC_POPULATION:
		case k_NMD_SPEC_WORKER:
		case k_NMD_SPEC_SLAVE:
		case k_NMD_SPEC_ENTERTAINER:
		case k_NMD_SPEC_FARMER:
		case k_NMD_SPEC_LABORER:
		case k_NMD_SPEC_MERCHANT:
		case k_NMD_SPEC_SCIENTIST:
		case k_NMD_SPEC_COMBAT_UNITS:
			result = atoi(column1->GetText()) - atoi(column2->GetText());
			break;
        
		default:
			Assert(false);
			result =(0);
	}

	return result;
}
#endif

void NationalManagementDialog::StatusListSelectActionCallback(aui_Control *control,
	uint32 action, uint32 data, void *cookie)
{
	if(action == AUI_LISTBOX_ACTION_DOUBLECLICKSELECT) {
		GotoSelectedCity();
		return;
	}

	
	if(action != (uint32)(AUI_LISTBOX_ACTION_SELECT))
		return;

	
	NationalManagementDialog *dialog = static_cast<NationalManagementDialog*>(cookie);
	
	if(dialog->m_mirroring)
		return;

	
	dialog->UpdateGovernor();
	dialog->UpdateBuildQueue();
	dialog->UpdateRushBuy();
	dialog->UpdateMainButtons((ctp2_ListBox *)control);

	dialog->MirrorSelectedCities();
}


void NationalManagementDialog::CloseButtonActionCallback(aui_Control *control,
	uint32 action, uint32 data, void *cookie)
{
	
	if(action != static_cast<uint32>(AUI_BUTTON_ACTION_EXECUTE))
		return;

	
	static_cast<NationalManagementDialog*>(cookie)->Hide();
}


void NationalManagementDialog::BuildQueueButtonActionCallback(aui_Control *control,
	uint32 action, uint32 data, void *cookie)
{
	
	if(action != static_cast<uint32>(AUI_BUTTON_ACTION_EXECUTE))
		return;

	
	NationalManagementDialog *dialog = static_cast<NationalManagementDialog*>(cookie);

	
	bool governorStatus = false;

	ctp2_ListBox *visibleList = dialog->m_resourceList;
	if(visibleList->IsHidden())
		visibleList = dialog->m_statusList;

	
	tech_WLList<sint32> *selectedList = visibleList->GetSelectedList();

	static UnitDynamicArray cities;
	cities.Clear();

	
	for(int selectIndex = 0; selectIndex < selectedList->L(); selectIndex++) {
		
		Unit city;
		city.m_id = reinterpret_cast<uint32>(
			static_cast<ctp2_ListItem*>(visibleList->GetItemByIndex(
			selectedList->GetAtIndex(selectIndex)
			))->GetUserData());
		cities.Insert(city);
	}

	if(cities.Num() == 1) {
		
		if(g_network.IsClient() && g_network.GetSensitiveUIBlocked()) {
		} else {
			EditQueue::Display(CityWindow::GetCityData(cities.Access(0)));
		}
	} else {
		if(g_network.IsClient() && g_network.GetSensitiveUIBlocked()) {
		} else {
			EditQueue::Display(cities);
		}
	}
}


void NationalManagementDialog::DisbandButtonActionCallback(aui_Control *control,
	uint32 action, uint32 data, void *cookie)
{
	
	if(action != static_cast<uint32>(AUI_BUTTON_ACTION_EXECUTE))
		return;

	
	MessageBoxDialog::Query("str_ldl_ND_VERIFY_DISBAND", "QueryDisbandCity", DisbandCallback,
		cookie);
}


void NationalManagementDialog::DisbandCallback(bool response, void *userData)
{
	
	if(response) {
		
		NationalManagementDialog *dialog =
			static_cast<NationalManagementDialog*>(userData);

		
		ctp2_ListBox *visibleList = dialog->m_resourceList;
		if(visibleList->IsHidden())
			visibleList = dialog->m_statusList;

		
		tech_WLList<sint32> *selectedList = visibleList->GetSelectedList();

		
		for(int selectIndex = 0; selectIndex < selectedList->L(); selectIndex++) {
			
			ctp2_ListItem *item = static_cast<ctp2_ListItem*>(
				visibleList->GetItemByIndex(
				selectedList->GetAtIndex(selectIndex)));

			
			Unit city;
			city.m_id = reinterpret_cast<uint32>(item->GetUserData());

			
			g_gevManager->AddEvent(GEV_INSERT_Tail, GEV_DisbandCity, GEA_City, city, GEA_End);
		}

		
		dialog->UpdateResourceList();
		dialog->UpdateStatusList();
	}
}


void NationalManagementDialog::ToggleGovernorButtonActionCallback(aui_Control *control,
	uint32 action, uint32 data, void *cookie)
{
	
	if(action != static_cast<uint32>(AUI_BUTTON_ACTION_EXECUTE))
		return;

	
	NationalManagementDialog *dialog = static_cast<NationalManagementDialog*>(cookie);

	
	bool governorStatus = false;

	
	tech_WLList<sint32> *selectedList = dialog->m_statusList->GetSelectedList();

	
	for(int selectIndex = 0; selectIndex < selectedList->L(); selectIndex++) {
		
		Unit city;
		city.m_id = reinterpret_cast<uint32>(
			static_cast<ctp2_ListItem*>(dialog->m_statusList->GetItemByIndex(
			selectedList->GetAtIndex(selectIndex)
			))->GetUserData());

		
		
		if(selectIndex) {
			governorStatus = governorStatus && city.GetCityData()->GetUseGovernor();
		} else {
			governorStatus = (city.GetCityData()->GetUseGovernor() == TRUE);
		}
	}

	
	
	for(int selectIndex2 = 0; selectIndex2 < selectedList->L(); selectIndex2++) {
		
		ctp2_ListItem *item = static_cast<ctp2_ListItem*>(
			dialog->m_statusList->GetItemByIndex(
			selectedList->GetAtIndex(selectIndex2)));

		
		Unit city;
		city.m_id = reinterpret_cast<uint32>(item->GetUserData());

		
		city.GetCityData()->SetUseGovernor(!governorStatus);

		
		dialog->UpdateStatusItem(item, city);
	}

	
	dialog->m_statusList->ShouldDraw();

	
	dialog->UpdateGovernor();
}


void NationalManagementDialog::SelectGovernorActionCallback(aui_Control *control,
	uint32 action, uint32 data, void *cookie)
{
	
	if(action != static_cast<uint32>(AUI_DROPDOWN_ACTION_SELECT))
		return;

	
	NationalManagementDialog *dialog = static_cast<NationalManagementDialog*>(cookie);

	
	tech_WLList<sint32> *selectedList = dialog->m_statusList->GetSelectedList();

	
	for(int selectIndex = 0; selectIndex < selectedList->L(); selectIndex++) {
		
		ctp2_ListItem *item = static_cast<ctp2_ListItem*>(
			dialog->m_statusList->GetItemByIndex(
			selectedList->GetAtIndex(selectIndex)));

		
		Unit city;
		city.m_id = reinterpret_cast<uint32>(item->GetUserData());

		
		city.GetCityData()->SetBuildListSequenceIndex(
			static_cast<ctp2_DropDown*>(control)->GetSelectedItem());

		
		dialog->UpdateStatusItem(item, city);
	}

	
	dialog->m_statusList->ShouldDraw();

	
	dialog->UpdateGovernor();
}


void NationalManagementDialog::SelectBuildItemActionCallback(aui_Control *control,
	uint32 action, uint32 data, void *cookie)
{
	
	if(action != static_cast<uint32>(AUI_DROPDOWN_ACTION_SELECT))
		return;

	
	NationalManagementDialog *dialog = static_cast<NationalManagementDialog*>(cookie);

	
	sint32 selectedItem =
		static_cast<ctp2_DropDown*>(control)->GetSelectedItem();
	if(selectedItem < 0)
		return;

	
	ctp2_ListItem *buildItem = static_cast<ctp2_ListItem*>(
		static_cast<ctp2_DropDown*>(control)->GetListBox()->GetItemByIndex(
		selectedItem));

	
	BuildQueueDropdownItem *buildInformation =
		static_cast<BuildQueueDropdownItem*>(buildItem->GetUserData());

	
	tech_WLList<sint32> *selectedList = dialog->m_statusList->GetSelectedList();

	
	for(int selectIndex = 0; selectIndex < selectedList->L(); selectIndex++) {
		
		ctp2_ListItem *item = static_cast<ctp2_ListItem*>(
			dialog->m_statusList->GetItemByIndex(
			selectedList->GetAtIndex(selectIndex)));

		
		Unit city;
		city.m_id = reinterpret_cast<uint32>(item->GetUserData());

		
		city.GetCityData()->InsertBuildItem(
			city.GetCityData()->GetBuildQueue()->GetLen() ? 0 : -1,
			buildInformation->m_category, buildInformation->m_type);

		
		dialog->UpdateStatusItem(item, city);
	}

	
	dialog->m_statusList->ShouldDraw();
}


void NationalManagementDialog::RushBuyButtonActionCallback(aui_Control *control,
	uint32 action, uint32 data, void *cookie)
{
	
	if(action != static_cast<uint32>(AUI_BUTTON_ACTION_EXECUTE))
		return;

	
	NationalManagementDialog *dialog = static_cast<NationalManagementDialog*>(cookie);

	
	tech_WLList<sint32> *selectedList = dialog->m_statusList->GetSelectedList();

	
	for(int selectIndex = 0; selectIndex < selectedList->L(); selectIndex++) {
		
		ctp2_ListItem *item = static_cast<ctp2_ListItem*>(
			dialog->m_statusList->GetItemByIndex(
			selectedList->GetAtIndex(selectIndex)));

		
		Unit city;
		city.m_id = reinterpret_cast<uint32>(item->GetUserData());

		// JJB removed the following:
#if defined(ACTIVISION_ORIGINAL)
		g_gevManager->AddEvent(GEV_INSERT_Tail, GEV_BuyFront, GEA_City, city, GEA_End);
#else
		// and replaced it with the following:
		if (!city.GetCityData()->AlreadyBoughtFront()
		  && city.GetOwner() == g_selected_item->GetCurPlayer()) {
			city.GetCityData()->AddBuyFront();
		}
		// in the hope of fixing the rush buy bug.
		// We only allow rush buying when it is the player's turn, and only
		// when the given item has not yet been rush bought this turn.
#endif
		
		dialog->UpdateStatusItem(item, city);
	}

	
	dialog->m_statusList->ShouldDraw();

	
	dialog->UpdateRushBuy();
}

void NationalManagementDialog::ResourceListSelectActionCallback(aui_Control *control,
	uint32 action, uint32 data, void *cookie)
{
	if(action == AUI_LISTBOX_ACTION_DOUBLECLICKSELECT) {
		GotoSelectedCity();
		return;
	}

	if(action != AUI_LISTBOX_ACTION_SELECT) return;

	NationalManagementDialog *dialog = static_cast<NationalManagementDialog*>(cookie);

	if(dialog->m_mirroring)
		return;

	dialog->UpdateMainButtons((ctp2_ListBox *)control);

	dialog->MirrorSelectedCities();
}


#ifndef ACTIVISION_ORIGINAL // #01 Added a third tab to the dialog
//----------------------------------------------------------------------------
//
// Name       : NationalManagementDialog::SpecialistListSelectActionCallback
//
// Description: Callback function for the specialist listbox
//               
//
// Parameters : *control:	Which control was activated
//				action:		Which action has happend
//				data:	
//				*cookie:
//
// Globals    : -
//
// Returns    : -
//
// Remark(s)  : -
//
//----------------------------------------------------------------------------
void NationalManagementDialog::SpecialistListSelectActionCallback(aui_Control *control,
	uint32 action, uint32 data, void *cookie)
{
	if(action == AUI_LISTBOX_ACTION_DOUBLECLICKSELECT) {
		GotoSelectedCity();
		return;
	}

	if(action != AUI_LISTBOX_ACTION_SELECT) return;

	NationalManagementDialog *dialog = static_cast<NationalManagementDialog*>(cookie);

	if(dialog->m_mirroring)
		return;

	dialog->UpdateMainButtons((ctp2_ListBox *)control);

	dialog->MirrorSelectedCities();
}
#endif

void NationalManagementDialog::UpdateMainButtons(ctp2_ListBox *box)
{
	Assert(box);
	if(!box) return;

	if(box->GetSelectedList()->L() < 1) {
		m_disbandButton->Enable(FALSE);
		m_buildQueueButton->Enable(FALSE);
	} else {
		m_disbandButton->Enable(TRUE);
		m_buildQueueButton->Enable(TRUE);
	}
}

void NationalManagementDialog::TabActionCallback(aui_Control *control,
												 uint32 action,
												 uint32 data,
												 void *cookie)
{
	if(action != ctp2_Tab::ACTION_ACTIVATED)
		return;

	
	NationalManagementDialog *dialog = static_cast<NationalManagementDialog*>(cookie);

	if(dialog->m_resourceTab == (ctp2_Tab *)control) {
		dialog->UpdateMainButtons(dialog->m_resourceList);
	} else if(dialog->m_statusTab == (ctp2_Tab *)control) {
		dialog->UpdateMainButtons(dialog->m_statusList);
#ifndef ACTIVISION_ORIGINAL // #01 Added a third tab to the dialog
	} else if(dialog->m_specialistTab == (ctp2_Tab *)control) {
		dialog->UpdateMainButtons(dialog->m_specialistList);
#endif
	}
}

Unit NationalManagementDialog::GetSelectedCity()
{
	Unit city;
	if(!g_nationalManagementDialog) return city;
#ifdef ACTIVISION_ORIGINAL // #01 Added a third tab to the dialog
	ctp2_ListBox *visList = g_nationalManagementDialog->m_resourceList;
	if(!visList || visList->IsHidden()) {
		visList = g_nationalManagementDialog->m_statusList;
	}
#else
	ctp2_ListBox *visList;

	if(!g_nationalManagementDialog->m_resourceList->IsHidden()) {
		visList = g_nationalManagementDialog->m_resourceList;
	} else if (!g_nationalManagementDialog->m_statusList->IsHidden()) {
		visList = g_nationalManagementDialog->m_statusList;
	} else if (!g_nationalManagementDialog->m_specialistList->IsHidden()) {
		visList = g_nationalManagementDialog->m_specialistList;
	} else {
		Assert(false);
		return city;
	}
#endif
	Assert(visList);
	if(!visList) return city;

	ctp2_ListItem *item = (ctp2_ListItem *)visList->GetSelectedItem();

	if(!item) return city;

	city.m_id = (uint32)item->GetUserData();
	return city;
}

void NationalManagementDialog::MirrorSelectedCities()
{
	if(!g_nationalManagementDialog)
		return;

	Unit city = GetSelectedCity();
	if(!city.IsValid())
		return;

#ifdef ACTIVISION_ORIGINAL // #01 Added a third tab to the dialog
	ctp2_ListBox *visList = g_nationalManagementDialog->m_resourceList;
	ctp2_ListBox *invisList = g_nationalManagementDialog->m_statusList;

	if(!visList || visList->IsHidden()) {
		visList = g_nationalManagementDialog->m_statusList;
		invisList = g_nationalManagementDialog->m_resourceList;
	}
#else
	ctp2_ListBox *visList;
	ctp2_ListBox *invisList;
	ctp2_ListBox *invisList2;


	if(!g_nationalManagementDialog->m_resourceList->IsHidden()) {
		visList = g_nationalManagementDialog->m_resourceList;
		invisList = g_nationalManagementDialog->m_specialistList;
		invisList2 = g_nationalManagementDialog->m_statusList;

	} else if (!g_nationalManagementDialog->m_statusList->IsHidden()) {
		visList = g_nationalManagementDialog->m_statusList;
		invisList = g_nationalManagementDialog->m_specialistList;
		invisList2 = g_nationalManagementDialog->m_resourceList;
		
	} else if (!g_nationalManagementDialog->m_specialistList->IsHidden()) {
		visList = g_nationalManagementDialog->m_specialistList;
		invisList = g_nationalManagementDialog->m_statusList;
		invisList2 = g_nationalManagementDialog->m_resourceList;
	} else {
		Assert(false);
		return;
	}

#endif

	tech_WLList<sint32> *selectedList = visList->GetSelectedList();

	
	if( selectedList->L() < 1 )
	{
		return;
	}

	g_nationalManagementDialog->m_mirroring = true;

	sint32 i;
	for(i = 0; i < invisList->NumItems(); i++) {
		invisList->DeselectItem(i);
#ifndef ACTIVISION_ORIGINAL // #01 Added a third tab to the dialog
		invisList2->DeselectItem(i);
#endif
	}

	for(int selectIndex = 0; selectIndex < selectedList->L(); selectIndex++)
	{
		
		Unit city;
		city.m_id = reinterpret_cast<uint32>(
			static_cast<ctp2_ListItem*>(visList->GetItemByIndex(
				selectedList->GetAtIndex(selectIndex)
				))->GetUserData());

		for(i = 0; i < invisList->NumItems(); i++) {
			Unit invisCity;
			invisCity.m_id = reinterpret_cast<uint32>(
				static_cast<ctp2_ListItem*>(invisList->GetItemByIndex(i))->GetUserData());
			if(invisCity.m_id == city.m_id) {
				invisList->SelectItem(i);
				break;
			}
		}
#ifndef ACTIVISION_ORIGINAL // #01 Added a third tab to the dialog
		for(i = 0; i < invisList2->NumItems(); i++) {
			Unit invisCity;
			invisCity.m_id = reinterpret_cast<uint32>(
				static_cast<ctp2_ListItem*>(invisList2->GetItemByIndex(i))->GetUserData());
			if(invisCity.m_id == city.m_id) {
				invisList2->SelectItem(i);
				break;
			}
		}
#endif
	}
		
	g_nationalManagementDialog->m_mirroring = false;
}

void NationalManagementDialog::GotoSelectedCity()
{
	Unit city = GetSelectedCity();
	if(city.IsValid()) {
		g_selected_item->SetSelectCity(city);
		if(!g_director->TileWillBeCompletelyVisible(city.RetPos().x, city.RetPos().y)) {
			g_director->AddCenterMap(city.RetPos());
		}
	}
}
