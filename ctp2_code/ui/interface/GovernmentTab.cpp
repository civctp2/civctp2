






#include "c3.h"


#include "GovernmentTab.h"


#include "aui_ldl.h"
#include "ctp2_button.h"
#include "ctp2_dropdown.h"
#include "ctp2_listitem.h"
#include "ctp2_Static.h"
#include "GovernmentRecord.h"
#include "player.h"
#include "SelItem.h"
#include "StrDB.h"
#include "colorset.h"

#include "soundmanager.h"
#include "gamesounds.h"
#include "DomesticManagementDialog.h"
#include "UnitDynArr.h"
#include "UnitRecord.h"
#include "MessageBoxDialog.h"

extern ColorSet	*g_colorSet;


template <class type> const MBCHAR *ComparisonCharacter(type left, type right)
{
	
	static const sint32 LESS_THAN_CHARACTER = 0;
	static const sint32 EQUAL_CHARACTER = 1;
	static const sint32 GREATER_THAN_CHARACTER = 2;
	static const MBCHAR *compareCharacter[] = { " ", " ", " " };
	

	
	if(left < right)
		return(compareCharacter[LESS_THAN_CHARACTER]);
	if(right < left)
		return(compareCharacter[GREATER_THAN_CHARACTER]);
	return(compareCharacter[EQUAL_CHARACTER]);
}


template <class type> const COLORREF *ComparisonColor(type left, type right)
{
	
	static const sint32 LESS_THAN_CHARACTER = 0;
	static const sint32 EQUAL_CHARACTER = 1;
	static const sint32 GREATER_THAN_CHARACTER = 2;

	
	if(left < right)
		return((const COLORREF *)g_colorSet->GetColorRef(COLOR_RED));
	if(right < left)
		return((const COLORREF *)g_colorSet->GetColorRef(COLOR_DARK_GREEN));
	return((const COLORREF *)g_colorSet->GetColorRef(COLOR_BLACK));
}


GovernmentTab::GovernmentTab(MBCHAR *ldlBlock) :
m_tabPanel(static_cast<ctp2_Static*>(aui_Ldl::GetObject(ldlBlock))),
m_enactButton(static_cast<ctp2_Button*>(aui_Ldl::GetObject(ldlBlock,
	"EnactGovernment"))),
m_currentGovernment(static_cast<ctp2_Static*>(aui_Ldl::GetObject(ldlBlock,
	"CurrentGovernment"))),
m_compareGovernment(static_cast<ctp2_DropDown*>(aui_Ldl::GetObject(ldlBlock,
	"CompareGovernment")))
{
	
	m_currentInformation[GII_CITIES] = static_cast<ctp2_Static*>(
		aui_Ldl::GetObject(ldlBlock, "Cities.CurrentValue"));
	m_currentInformation[GII_GROWTH] = static_cast<ctp2_Static*>(
		aui_Ldl::GetObject(ldlBlock, "Growth.CurrentValue"));
	m_currentInformation[GII_PRODUCTION] = static_cast<ctp2_Static*>(
		aui_Ldl::GetObject(ldlBlock, "Production.CurrentValue"));
	m_currentInformation[GII_RESEARCH] = static_cast<ctp2_Static*>(
		aui_Ldl::GetObject(ldlBlock, "Research.CurrentValue"));
	m_currentInformation[GII_ECONOMIC] = static_cast<ctp2_Static*>(
		aui_Ldl::GetObject(ldlBlock, "Economic.CurrentValue"));
	m_currentInformation[GII_COMMERCE] = static_cast<ctp2_Static*>(
		aui_Ldl::GetObject(ldlBlock, "Commerce.CurrentValue"));
	m_currentInformation[GII_MILITARY] = static_cast<ctp2_Static*>(
		aui_Ldl::GetObject(ldlBlock, "Military.CurrentValue"));
	m_currentInformation[GII_LOYALTY] = static_cast<ctp2_Static*>(
		aui_Ldl::GetObject(ldlBlock, "Loyalty.CurrentValue"));
	m_currentInformation[GII_MARTIAL_LAW] = static_cast<ctp2_Static*>(
		aui_Ldl::GetObject(ldlBlock, "MartialLaw.CurrentValue"));
	m_currentInformation[GII_ANTI_POLLUTION] = static_cast<ctp2_Static*>(
		aui_Ldl::GetObject(ldlBlock, "AntiPollution.CurrentValue"));
	m_comparison[GII_CITIES] = static_cast<ctp2_Static*>(
		aui_Ldl::GetObject(ldlBlock, "Cities.Comparison"));
	m_comparison[GII_GROWTH] = static_cast<ctp2_Static*>(
		aui_Ldl::GetObject(ldlBlock, "Growth.Comparison"));
	m_comparison[GII_PRODUCTION] = static_cast<ctp2_Static*>(
		aui_Ldl::GetObject(ldlBlock, "Production.Comparison"));
	m_comparison[GII_RESEARCH] = static_cast<ctp2_Static*>(
		aui_Ldl::GetObject(ldlBlock, "Research.Comparison"));
	m_comparison[GII_ECONOMIC] = static_cast<ctp2_Static*>(
		aui_Ldl::GetObject(ldlBlock, "Economic.Comparison"));
	m_comparison[GII_COMMERCE] = static_cast<ctp2_Static*>(
		aui_Ldl::GetObject(ldlBlock, "Commerce.Comparison"));
	m_comparison[GII_MILITARY] = static_cast<ctp2_Static*>(
		aui_Ldl::GetObject(ldlBlock, "Military.Comparison"));
	m_comparison[GII_LOYALTY] = static_cast<ctp2_Static*>(
		aui_Ldl::GetObject(ldlBlock, "Loyalty.Comparison"));
	m_comparison[GII_MARTIAL_LAW] = static_cast<ctp2_Static*>(
		aui_Ldl::GetObject(ldlBlock, "MartialLaw.Comparison"));
	m_comparison[GII_ANTI_POLLUTION] = static_cast<ctp2_Static*>(
		aui_Ldl::GetObject(ldlBlock, "AntiPollution.Comparison"));
	m_compareInformation[GII_CITIES] = static_cast<ctp2_Static*>(
		aui_Ldl::GetObject(ldlBlock, "Cities.CompareValue"));
	m_compareInformation[GII_GROWTH] = static_cast<ctp2_Static*>(
		aui_Ldl::GetObject(ldlBlock, "Growth.CompareValue"));
	m_compareInformation[GII_PRODUCTION] = static_cast<ctp2_Static*>(
		aui_Ldl::GetObject(ldlBlock, "Production.CompareValue"));
	m_compareInformation[GII_RESEARCH] = static_cast<ctp2_Static*>(
		aui_Ldl::GetObject(ldlBlock, "Research.CompareValue"));
	m_compareInformation[GII_ECONOMIC] = static_cast<ctp2_Static*>(
		aui_Ldl::GetObject(ldlBlock, "Economic.CompareValue"));
	m_compareInformation[GII_COMMERCE] = static_cast<ctp2_Static*>(
		aui_Ldl::GetObject(ldlBlock, "Commerce.CompareValue"));
	m_compareInformation[GII_MILITARY] = static_cast<ctp2_Static*>(
		aui_Ldl::GetObject(ldlBlock, "Military.CompareValue"));
	m_compareInformation[GII_LOYALTY] = static_cast<ctp2_Static*>(
		aui_Ldl::GetObject(ldlBlock, "Loyalty.CompareValue"));
	m_compareInformation[GII_MARTIAL_LAW] = static_cast<ctp2_Static*>(
		aui_Ldl::GetObject(ldlBlock, "MartialLaw.CompareValue"));
	m_compareInformation[GII_ANTI_POLLUTION] = static_cast<ctp2_Static*>(
		aui_Ldl::GetObject(ldlBlock, "AntiPollution.CompareValue"));

	
	Assert(m_enactButton);
	Assert(m_currentGovernment);
	Assert(m_compareGovernment);
#ifdef _DEBUG
	for(int currentIndex = 0; currentIndex < GII_TOTAL; currentIndex++)
		Assert(m_currentInformation[currentIndex]);
	for(int comparisonIndex = 0; comparisonIndex < GII_TOTAL; comparisonIndex++)
		Assert(m_comparison[comparisonIndex]);
	for(int compareIndex = 0; compareIndex < GII_TOTAL; compareIndex++)
		Assert(m_compareInformation[compareIndex]);
#endif 

	
	m_compareGovernment->SetActionFuncAndCookie(
		CompareGovernmentActionCallback, this);
	m_enactButton->SetActionFuncAndCookie(
		EnactGovernmentActionCallback, this);
}


void GovernmentTab::Update()
{
	
	UpdateCurrentGovernment();
	UpdateCompareGovernmentDropdown();
}


void GovernmentTab::UpdateCurrentGovernment()
{
	
	Player *player = g_player[g_selected_item->GetVisiblePlayer()];

	
	const GovernmentRecord *government =
		g_theGovernmentDB->Get(player->GetGovernmentType());

	
	m_currentGovernment->SetText(government->GetNameText());

	
	UpdateGovernmentInformation(government, m_currentInformation);
}


void GovernmentTab::UpdateCompareGovernmentDropdown()
{
	
	Player *player = g_player[g_selected_item->GetVisiblePlayer()];

	
	std::pair<bool, sint32> currentSelection = GetGovernmentSelection();

	
	m_compareGovernment->Clear();

	
	ctp2_ListItem *reselect = NULL;

	
	for(sint32 governmentIndex = 1; governmentIndex <
		g_theGovernmentDB->NumRecords(); governmentIndex++ ) {
		
		if(governmentIndex == player->GetGovernmentType())
			continue;

		
		const GovernmentRecord *government =
			g_theGovernmentDB->Get(governmentIndex);

		
		sint32 enablingAdvance = government->GetEnableAdvanceIndex();

		
		
		bool obsoleteGovernment = false;
		for(sint32 advanceIndex = 0; advanceIndex <
			government->GetNumObsoleteAdvance(); advanceIndex++) {
			if(player->m_advances->HasAdvance(
				government->GetObsoleteAdvanceIndex(advanceIndex))) {
				obsoleteGovernment = true;
				break;
			}
		}

		
		
		if(!obsoleteGovernment &&
			((enablingAdvance < 0) ||
			player->m_advances->HasAdvance(enablingAdvance))) {
			
			ctp2_ListItem *listItem = static_cast<ctp2_ListItem*>(
				aui_Ldl::BuildHierarchyFromRoot("GovernmentListItem"));

			
			ctp2_Static *label = static_cast<ctp2_Static*>(
				listItem->GetChildByIndex(0));
			label->SetText(government->GetNameText());
			listItem->SetUserData(
				reinterpret_cast<void*>(governmentIndex));

			
			m_compareGovernment->AddItem(listItem);

			
			if(governmentIndex == currentSelection.second)
				reselect = listItem;
		}
	}

	
	if(reselect) {
		m_compareGovernment->GetListBox()->SelectItem(reselect);
		m_compareGovernment->ShouldDraw();	
	}

	
	
	if(m_compareGovernment->GetListBox()->NumItems())
		m_enactButton->Enable(true);
	else
		m_enactButton->Enable(false);

	
	UpdateCompareGovernment();
}


void GovernmentTab::UpdateCompareGovernment()
{
	
	std::pair<bool, sint32> currentSelection = GetGovernmentSelection();

	
	
	if(currentSelection.first) {
		
		Player *player = g_player[g_selected_item->GetVisiblePlayer()];

		
		const GovernmentRecord *currentGovernment =
			g_theGovernmentDB->Get(player->GetGovernmentType());

		
		const GovernmentRecord *compareGovernment =
			g_theGovernmentDB->Get(currentSelection.second);

		
		UpdateComparison(currentGovernment, compareGovernment);
		UpdateGovernmentInformation(compareGovernment, m_compareInformation);
	} else {
		
		ClearGovernmentInformation(m_comparison);
		ClearGovernmentInformation(m_compareInformation);
	}
}



void GovernmentTab::UpdateComparison(const GovernmentRecord *currentGovernment,
									 const GovernmentRecord *compareGovernment)
{
	
	m_comparison[GII_CITIES]->SetText(ComparisonCharacter(
		currentGovernment->GetTooManyCitiesThreshold(),
		compareGovernment->GetTooManyCitiesThreshold()));
	m_comparison[GII_GROWTH]->SetText(ComparisonCharacter(
		currentGovernment->GetGrowthRank(),
		compareGovernment->GetGrowthRank()));
	m_comparison[GII_PRODUCTION]->SetText(ComparisonCharacter(
		currentGovernment->GetProductionRank(),
		compareGovernment->GetProductionRank()));
	m_comparison[GII_RESEARCH]->SetText(ComparisonCharacter(
		currentGovernment->GetScienceRank(),
		compareGovernment->GetScienceRank()));
	m_comparison[GII_ECONOMIC]->SetText(ComparisonCharacter(
		currentGovernment->GetGoldRank(),
		compareGovernment->GetGoldRank()));
	m_comparison[GII_COMMERCE]->SetText(ComparisonCharacter(
		currentGovernment->GetCommerceRank(),
		compareGovernment->GetCommerceRank()));
	m_comparison[GII_MILITARY]->SetText(ComparisonCharacter(
		currentGovernment->GetMilitaryRank(),
		compareGovernment->GetMilitaryRank()));
	m_comparison[GII_LOYALTY]->SetText(ComparisonCharacter(
		currentGovernment->GetLoyaltyRank(),
		compareGovernment->GetLoyaltyRank()));
	m_comparison[GII_MARTIAL_LAW]->SetText(ComparisonCharacter(
		currentGovernment->GetMartialLawRank(),
		compareGovernment->GetMartialLawRank()));
	m_comparison[GII_ANTI_POLLUTION]->SetText(ComparisonCharacter(
		currentGovernment->GetPollutionRank(),
		compareGovernment->GetPollutionRank()));


	m_currentInformation[GII_CITIES]->SetTextColor((long)ComparisonColor(
		currentGovernment->GetTooManyCitiesThreshold(),
		compareGovernment->GetTooManyCitiesThreshold()));
	m_compareInformation[GII_CITIES]->SetTextColor((long)ComparisonColor(
		compareGovernment->GetTooManyCitiesThreshold(),
		currentGovernment->GetTooManyCitiesThreshold()));

	m_currentInformation[GII_GROWTH]->SetTextColor((long)ComparisonColor(
		currentGovernment->GetGrowthRank(),
		compareGovernment->GetGrowthRank()));
	m_compareInformation[GII_GROWTH]->SetTextColor((long)ComparisonColor(
		compareGovernment->GetGrowthRank(),
		currentGovernment->GetGrowthRank()));

	m_currentInformation[GII_PRODUCTION]->SetTextColor((long)ComparisonColor(
		currentGovernment->GetProductionRank(),
		compareGovernment->GetProductionRank()));
	m_compareInformation[GII_PRODUCTION]->SetTextColor((long)ComparisonColor(
		compareGovernment->GetProductionRank(),
		currentGovernment->GetProductionRank()));

	m_currentInformation[GII_RESEARCH]->SetTextColor((long)ComparisonColor(
		currentGovernment->GetScienceRank(),
		compareGovernment->GetScienceRank()));
	m_compareInformation[GII_RESEARCH]->SetTextColor((long)ComparisonColor(
		compareGovernment->GetScienceRank(),
		currentGovernment->GetScienceRank()));

	m_currentInformation[GII_ECONOMIC]->SetTextColor((long)ComparisonColor(
		currentGovernment->GetGoldRank(),
		compareGovernment->GetGoldRank()));
	m_compareInformation[GII_ECONOMIC]->SetTextColor((long)ComparisonColor(
		compareGovernment->GetGoldRank(),
		currentGovernment->GetGoldRank()));

	m_currentInformation[GII_COMMERCE]->SetTextColor((long)ComparisonColor(
		currentGovernment->GetCommerceRank(),
		compareGovernment->GetCommerceRank()));
	m_compareInformation[GII_COMMERCE]->SetTextColor((long)ComparisonColor(
		compareGovernment->GetCommerceRank(),
		currentGovernment->GetCommerceRank()));

	m_currentInformation[GII_MILITARY]->SetTextColor((long)ComparisonColor(
		currentGovernment->GetMilitaryRank(),
		compareGovernment->GetMilitaryRank()));
	m_compareInformation[GII_MILITARY]->SetTextColor((long)ComparisonColor(
		compareGovernment->GetMilitaryRank(),
		currentGovernment->GetMilitaryRank()));

	m_currentInformation[GII_LOYALTY]->SetTextColor((long)ComparisonColor(
		currentGovernment->GetLoyaltyRank(),
		compareGovernment->GetLoyaltyRank()));
	m_compareInformation[GII_LOYALTY]->SetTextColor((long)ComparisonColor(
		compareGovernment->GetLoyaltyRank(),
		currentGovernment->GetLoyaltyRank()));

	m_currentInformation[GII_MARTIAL_LAW]->SetTextColor((long)ComparisonColor(
		currentGovernment->GetMartialLawRank(),
		compareGovernment->GetMartialLawRank()));
	m_compareInformation[GII_MARTIAL_LAW]->SetTextColor((long)ComparisonColor(
		compareGovernment->GetMartialLawRank(),
		currentGovernment->GetMartialLawRank()));

	m_currentInformation[GII_ANTI_POLLUTION]->SetTextColor((long)ComparisonColor(
		currentGovernment->GetPollutionRank(),
		compareGovernment->GetPollutionRank()));
	m_compareInformation[GII_ANTI_POLLUTION]->SetTextColor((long)ComparisonColor(
		compareGovernment->GetPollutionRank(),
		currentGovernment->GetPollutionRank()));
}


void GovernmentTab::ClearGovernmentInformation(ctp2_Static **information)
{
	for(int currentIndex = 0; currentIndex < GII_TOTAL; currentIndex++)
		information[currentIndex]->SetText("");
}


void GovernmentTab::UpdateGovernmentInformation(
	const GovernmentRecord *government,
	ctp2_Static **information)
{
	
	static char stringBuffer[32];
	static char formatBuffer[64];

	
	sprintf(stringBuffer, "%d", government->GetTooManyCitiesThreshold());
	information[GII_CITIES]->SetText(stringBuffer);

	
	sprintf(formatBuffer,"EMPIRE_VALUE_DESC_GROWTH_%i",government->GetGrowthRank());
	sprintf(stringBuffer, "%s", g_theStringDB->GetNameStr(formatBuffer));
	
	information[GII_GROWTH]->SetText(stringBuffer);

	
	sprintf(formatBuffer,"EMPIRE_VALUE_DESC_PRODUCTION_%i",government->GetProductionRank());
	sprintf(stringBuffer, "%s", g_theStringDB->GetNameStr(formatBuffer));
	
	information[GII_PRODUCTION]->SetText(stringBuffer);

	
	sprintf(formatBuffer,"EMPIRE_VALUE_DESC_RESEARCH_%i",government->GetScienceRank());
	sprintf(stringBuffer, "%s", g_theStringDB->GetNameStr(formatBuffer));
	
	information[GII_RESEARCH]->SetText(stringBuffer);

	
	sprintf(formatBuffer,"EMPIRE_VALUE_DESC_ECONOMIC_%i",government->GetGoldRank());
	sprintf(stringBuffer, "%s", g_theStringDB->GetNameStr(formatBuffer));
	
	information[GII_ECONOMIC]->SetText(stringBuffer);

	
	sprintf(formatBuffer,"EMPIRE_VALUE_DESC_COMMERCE_%i",government->GetCommerceRank());
	sprintf(stringBuffer, "%s", g_theStringDB->GetNameStr(formatBuffer));
	
	information[GII_COMMERCE]->SetText(stringBuffer);

	
	sprintf(formatBuffer,"EMPIRE_VALUE_DESC_MILITARY_%i",government->GetMilitaryRank());
	sprintf(stringBuffer, "%s", g_theStringDB->GetNameStr(formatBuffer));
	
	information[GII_MILITARY]->SetText(stringBuffer);

	
	sprintf(formatBuffer,"EMPIRE_VALUE_DESC_LOYALTY_%i",government->GetLoyaltyRank());
	sprintf(stringBuffer, "%s", g_theStringDB->GetNameStr(formatBuffer));
	information[GII_LOYALTY]->SetText(stringBuffer);

	
	sprintf(formatBuffer,"EMPIRE_VALUE_DESC_MARTIALLAW_%i",government->GetMartialLawRank());
	sprintf(stringBuffer, "%s", g_theStringDB->GetNameStr(formatBuffer));
	information[GII_MARTIAL_LAW]->SetText(stringBuffer);

	
	sprintf(formatBuffer,"EMPIRE_VALUE_DESC_ANTI_POLLUTION_%i",government->GetPollutionRank());
	sprintf(stringBuffer, "%s", g_theStringDB->GetNameStr(formatBuffer));
	
	
	information[GII_ANTI_POLLUTION]->SetText(stringBuffer);
}




std::pair<bool, sint32> GovernmentTab::GetGovernmentSelection()
{
	
	
	std::pair<bool, sint32> currentSelection(false, 0);
	if(m_compareGovernment->GetListBox()->NumItems()) {
		
		
		
		ctp2_ListItem *currentItem = static_cast<ctp2_ListItem*>(
			m_compareGovernment->GetListBox()->GetItemByIndex(
			m_compareGovernment->GetSelectedItem()));

		if(currentItem) {
			
			currentSelection.first = true;

			
			currentSelection.second = *reinterpret_cast<sint32 *>
				(currentItem->GetUserData());
		}
	}

	
	return(currentSelection);
}


void GovernmentTab::CompareGovernmentActionCallback(aui_Control *control,
	uint32 action, uint32 data, void *cookie)
{
	
	if(action != static_cast<uint32>(AUI_DROPDOWN_ACTION_SELECT))
		return;

	
	static_cast<GovernmentTab*>(cookie)->UpdateCompareGovernment();
	static_cast<GovernmentTab*>(cookie)->m_tabPanel->ShouldDraw(TRUE);
}

void GovernmentTab::ConfirmGovernmentChange(bool result, void *data)
{
	
	GovernmentTab *tab = static_cast<GovernmentTab*>(data);

	if(result) {
		Player *player = g_player[g_selected_item->GetVisiblePlayer()];
		if(!player)
			return;

		player->SetGovernmentType(tab->GetGovernmentSelection().second);

		tab->Update();
		tab->m_tabPanel->ShouldDraw();
		g_soundManager->AddGameSound(GAMESOUNDS_ADVANCE); 

		g_domesticManagementDialog->Update();
	}		
}
	

void GovernmentTab::EnactGovernmentActionCallback(aui_Control *control,
	uint32 action, uint32 data, void *cookie)
{
	
	if(action != static_cast<uint32>(AUI_BUTTON_ACTION_EXECUTE))
		return;

	
	Player *player = g_player[g_selected_item->GetVisiblePlayer()];


	
	GovernmentTab *tab = static_cast<GovernmentTab*>(cookie);

	
	sint32 i;
	for(i = player->m_all_units->Num() - 1; i >= 0; i--) {

		const UnitRecord *rec = player->m_all_units->Access(i).GetDBRec();
		if(rec->GetNumGovernmentType() < 1)
			continue;

		sint32 j;
		bool found = false;
		for(j = 0; j < rec->GetNumGovernmentType(); j++) {
			if(tab->GetGovernmentSelection().second == rec->GetGovernmentTypeIndex(j)) {
				found = true;
				break;
			}
		}
		
		
		if(!found) {
			MessageBoxDialog::Query(g_theStringDB->GetNameStr("str_code_GovernmentChangeDisbandsUnits"),
									"GovernmentChangeDisbandsUnits",
									ConfirmGovernmentChange, (void *)tab);
			return;
		}
	}
								   
	
	Assert(tab->GetGovernmentSelection().first);

	
	player->SetGovernmentType(tab->GetGovernmentSelection().second);

	
	tab->Update();
	tab->m_tabPanel->ShouldDraw(TRUE);
	g_soundManager->AddGameSound(GAMESOUNDS_ADVANCE);

	g_domesticManagementDialog->Update();
}
