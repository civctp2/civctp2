//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : Empire manager main tab
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
// - Use the same science percentage everywhere.
//
//----------------------------------------------------------------------------

#include "c3.h"


#include "CauseAndEffectTab.h"


#include "aui_Ldl.h"
#include "aui_Region.h"
#include "BuildingUtil.h"
#include "C3Slider.h"
#include "ColorSet.h"
#include "ctp2_Button.h"
#include "ctp2_Spinner.h"
#include "ctp2_Static.h"
#include "ctp2_Tab.h"
#include "GovernmentRecord.h"
#include "Happy.h"	
#include "PixelUtils.h"
#include "Player.h"
#include "Primitives.h"
#include "SelItem.h"
#include "StrDB.h"
#include "UnitData.h"
#include "UnitDynArr.h"
#include "WonderUtil.h"
#include "DomesticManagementDialog.h"
#include "network.h"
#include "c3math.h"		// AsPercentage


extern ColorSet *g_colorSet;


static sint32 k_ONE_FIVE__NEG_TWO_TWO_CONVERSION = 3;
static sint32 k_ZERO_FOUR__NEG_TWO_TWO_CONVERSION = 2;


CauseAndEffectTab::CauseAndEffectTab(MBCHAR *ldlBlock) :
m_tabPanel(static_cast<ctp2_Static*>(aui_Ldl::GetObject(ldlBlock))),
m_detailsButton(static_cast<ctp2_Button*>(aui_Ldl::GetObject(
	"DomesticDialog.DetailsButton"))),
m_numberOfCities(static_cast<ctp2_Static*>(aui_Ldl::GetObject(ldlBlock,
	"CitiesValue"))),
m_population(static_cast<ctp2_Static*>(aui_Ldl::GetObject(ldlBlock,
	"PopulationValue"))),
m_happinessValue(static_cast<ctp2_Static*>(aui_Ldl::GetObject(ldlBlock,
	"HappinessValue"))),
m_happinessBar(static_cast<ctp2_Static*>(aui_Ldl::GetObject(ldlBlock,
	"HappinessBar"))),
m_government(static_cast<ctp2_Static*>(aui_Ldl::GetObject(ldlBlock,
	"CurrentGovernment"))),
m_foodRationsSpinner(static_cast<C3Slider*>(aui_Ldl::GetObject(ldlBlock,
	"Food.Modify.Primary.Slider"))),
m_foodRequired(static_cast<ctp2_Static*>(aui_Ldl::GetObject(ldlBlock,
	"Food.Modify.Secondary.Value"))),
m_foodHappinessValue(static_cast<ctp2_Static*>(aui_Ldl::GetObject(ldlBlock,
	"Food.Modify.Primary.Value"))),
m_foodCurValue(static_cast<ctp2_Static*>(aui_Ldl::GetObject(ldlBlock,
	"Food.Modify.Primary.CurValue"))),
m_foodHappinessIcon(static_cast<ctp2_Static*>(aui_Ldl::GetObject(ldlBlock,
	"Food.Modify.Primary.Icon"))),

m_detailsFoodTotal(static_cast<ctp2_Static*>(aui_Ldl::GetObject(ldlBlock,
	"Food.InformationDetails.Collected.Value"))),
m_summaryFoodTotal(static_cast<ctp2_Static*>(aui_Ldl::GetObject(ldlBlock,
	"Food.InformationSummary.Collected.Value"))),
m_detailsFoodCrime(static_cast<ctp2_Static*>(aui_Ldl::GetObject(ldlBlock,
	"Food.InformationDetails.Crime.Value"))),
m_detailsFoodConsumed(static_cast<ctp2_Static*>(aui_Ldl::GetObject(ldlBlock,
	"Food.InformationDetails.Consumed.Value"))),
m_summaryFoodConsumed(static_cast<ctp2_Static*>(aui_Ldl::GetObject(ldlBlock,
	"Food.InformationSummary.Consumed.Value"))),
m_detailsFoodStored(static_cast<ctp2_Static*>(aui_Ldl::GetObject(ldlBlock,
	"Food.InformationDetails.Remainder.Value"))),
m_summaryFoodStored(static_cast<ctp2_Static*>(aui_Ldl::GetObject(ldlBlock,
	"Food.InformationSummary.Remainder.Value"))),

m_productionWorkdaySpinner(static_cast<C3Slider*>(aui_Ldl::GetObject(ldlBlock,
	"Production.Modify.Primary.Slider"))),
m_productionPublicWorksSpinner(static_cast<ctp2_Spinner*>(aui_Ldl::GetObject(ldlBlock,
	"Production.Modify.Secondary.Spinner"))),
m_productionHappinessValue(static_cast<ctp2_Static*>(aui_Ldl::GetObject(ldlBlock,
	"Production.Modify.Primary.Value"))),
m_productionCurValue(static_cast<ctp2_Static*>(aui_Ldl::GetObject(ldlBlock,
	"Production.Modify.Primary.CurValue"))),
m_productionHappinessIcon(static_cast<ctp2_Static*>(aui_Ldl::GetObject(ldlBlock,
	"Production.Modify.Primary.Icon"))),

m_detailsProductionTotal(static_cast<ctp2_Static*>(aui_Ldl::GetObject(ldlBlock,
	"Production.InformationDetails.Collected.Value"))),
m_summaryProductionTotal(static_cast<ctp2_Static*>(aui_Ldl::GetObject(ldlBlock,
	"Production.InformationSummary.Collected.Value"))),
m_detailsProductionCrime(static_cast<ctp2_Static*>(aui_Ldl::GetObject(ldlBlock,
	"Production.InformationDetails.Crime.Value"))),
m_detailsProductionUnitUpkeep(static_cast<ctp2_Static*>(aui_Ldl::GetObject(ldlBlock,
	"Production.InformationDetails.Upkeep.Value"))),
m_detailsProductionPublicWorks(static_cast<ctp2_Static*>(aui_Ldl::GetObject(ldlBlock,
	"Production.InformationDetails.PublicWorks.Value"))),
m_summaryProductionPublicWorks(static_cast<ctp2_Static*>(aui_Ldl::GetObject(ldlBlock,
	"Production.InformationSummary.PublicWorks.Value"))),
m_detailsProductionCityUse(static_cast<ctp2_Static*>(aui_Ldl::GetObject(ldlBlock,
	"Production.InformationDetails.CityUse.Value"))),
m_summaryProductionCityUse(static_cast<ctp2_Static*>(aui_Ldl::GetObject(ldlBlock,
	"Production.InformationSummary.CityUse.Value"))),

m_commerceWagesSpinner(static_cast<C3Slider*>(aui_Ldl::GetObject(ldlBlock,
	"Commerce.Modify.Primary.Slider"))),
m_commerceScienceTaxSpinner(static_cast<ctp2_Spinner*>(aui_Ldl::GetObject(ldlBlock,
	"Commerce.Modify.Secondary.Spinner"))),
m_commerceHappinessValue(static_cast<ctp2_Static*>(aui_Ldl::GetObject(ldlBlock,
	"Commerce.Modify.Primary.Value"))),
m_commerceCurValue(static_cast<ctp2_Static*>(aui_Ldl::GetObject(ldlBlock,
	"Commerce.Modify.Primary.CurValue"))),
m_commerceHappinessIcon(static_cast<ctp2_Static*>(aui_Ldl::GetObject(ldlBlock,
	"Commerce.Modify.Primary.Icon"))),

m_detailsCommerceTotal(static_cast<ctp2_Static*>(aui_Ldl::GetObject(ldlBlock,
	"Commerce.InformationDetails.Collected.Value"))),
m_summaryCommerceTotal(static_cast<ctp2_Static*>(aui_Ldl::GetObject(ldlBlock,
	"Commerce.InformationSummary.Collected.Value"))),
m_detailsCommerceCrime(static_cast<ctp2_Static*>(aui_Ldl::GetObject(ldlBlock,
	"Commerce.InformationDetails.Crime.Value"))),
m_detailsCommerceWages(static_cast<ctp2_Static*>(aui_Ldl::GetObject(ldlBlock,
	"Commerce.InformationDetails.Wages.Value"))),
m_detailsCommerceBuildingUpkeep(static_cast<ctp2_Static*>(aui_Ldl::GetObject(ldlBlock,
	"Commerce.InformationDetails.Upkeep.Value"))),
m_detailsCommerceScience(static_cast<ctp2_Static*>(aui_Ldl::GetObject(ldlBlock,
	"Commerce.InformationDetails.Science.Value"))),
m_summaryCommerceScience(static_cast<ctp2_Static*>(aui_Ldl::GetObject(ldlBlock,
	"Commerce.InformationSummary.Science.Value"))),
m_detailsCommerceSavings(static_cast<ctp2_Static*>(aui_Ldl::GetObject(ldlBlock,
	"Commerce.InformationDetails.Savings.Value"))),
m_summaryCommerceSavings(static_cast<ctp2_Static*>(aui_Ldl::GetObject(ldlBlock,
	"Commerce.InformationSummary.Savings.Value")))
{
	
	Assert(m_numberOfCities);
	Assert(m_population);
	Assert(m_happinessValue);
	Assert(m_happinessBar);
	Assert(m_government);
	Assert(m_foodRationsSpinner);
	Assert(m_foodHappinessValue);
	Assert(m_foodCurValue);
	Assert(m_foodHappinessIcon);
	Assert(m_detailsFoodTotal);
	Assert(m_detailsFoodCrime);
	Assert(m_detailsFoodConsumed);
	Assert(m_detailsFoodStored);
	Assert(m_summaryFoodTotal);
	Assert(m_summaryFoodConsumed);
	Assert(m_summaryFoodStored);
	Assert(m_productionWorkdaySpinner);
	Assert(m_productionPublicWorksSpinner);
	Assert(m_productionHappinessValue);
	Assert(m_productionHappinessIcon);
	Assert(m_detailsProductionTotal);
	Assert(m_detailsProductionCrime);
	Assert(m_detailsProductionUnitUpkeep);
	Assert(m_detailsProductionPublicWorks);
	Assert(m_detailsProductionCityUse);
	Assert(m_summaryProductionTotal);
	Assert(m_summaryProductionPublicWorks);
	Assert(m_summaryProductionCityUse);
	Assert(m_commerceWagesSpinner);
	Assert(m_commerceScienceTaxSpinner);
	Assert(m_commerceHappinessValue);
	Assert(m_commerceHappinessIcon);
	Assert(m_detailsCommerceTotal);
	Assert(m_detailsCommerceCrime);
	Assert(m_detailsCommerceWages);
	Assert(m_detailsCommerceBuildingUpkeep);
	Assert(m_detailsCommerceScience);
	Assert(m_detailsCommerceSavings);
	Assert(m_summaryCommerceTotal);
	Assert(m_summaryCommerceScience);
	Assert(m_summaryCommerceSavings);

	m_tab = (static_cast<ctp2_Tab*>(aui_Ldl::GetObject("DomesticDialog.TabGroup.Tab1")));

	strcpy(m_ldlBlock,ldlBlock);

	
	DisplayDetails(false);

	
	m_tabPanel->SetShowCallback(DetailsShowCallback, this);
	m_detailsButton->SetActionFuncAndCookie(
		DetailsButtonActionCallback, this);
	m_happinessBar->SetDrawCallbackAndCookie(
		HappinessBarActionCallback, this, false);
	m_foodRationsSpinner->SetActionFuncAndCookie(
		RationsSpinnerActionCallback, this);
	m_productionWorkdaySpinner->SetActionFuncAndCookie(
		WorkdaySpinnerActionCallback, this);
	m_productionPublicWorksSpinner->SetSpinnerCallback(
		PublicWorksSpinnerActionCallback, this);
	m_commerceWagesSpinner->SetActionFuncAndCookie(
		WagesSpinnerActionCallback, this);
	m_commerceScienceTaxSpinner->SetSpinnerCallback(
		ScienceTaxSpinnerActionCallback, this);

	m_tab->SetActionFuncAndCookie(CauseAndEffectTabActionCallback, this);
}


void CauseAndEffectTab::Update()
{
	
	if(g_network.GetSensitiveUIBlocked())
		return;

	UpdateCities();

	
	UpdateGeneral();

	
	UpdateFoodSpinners();
	UpdateProductionSpinners();
	UpdateCommerceSpinners();

	
	UpdateFoodValues();
	UpdateProductionValues();
	UpdateCommerceValues();
}


void CauseAndEffectTab::DisplayDetails(bool flag)
{
	
	m_displayDetails = flag;

	
	if(m_tabPanel->IsHidden())
		return;

	
	if(m_displayDetails) {
		static_cast<ctp2_Static*>(aui_Ldl::GetObject(m_ldlBlock,
			"Food.InformationDetails"))->Show();
		static_cast<ctp2_Static*>(aui_Ldl::GetObject(m_ldlBlock,
			"Production.InformationDetails"))->Show();
		static_cast<ctp2_Static*>(aui_Ldl::GetObject(m_ldlBlock,
			"Commerce.InformationDetails"))->Show();
		static_cast<ctp2_Static*>(aui_Ldl::GetObject(m_ldlBlock,
			"Food.InformationSummary"))->Hide();
		static_cast<ctp2_Static*>(aui_Ldl::GetObject(m_ldlBlock,
			"Production.InformationSummary"))->Hide();
		static_cast<ctp2_Static*>(aui_Ldl::GetObject(m_ldlBlock,
			"Commerce.InformationSummary"))->Hide();
	} else {
		static_cast<ctp2_Static*>(aui_Ldl::GetObject(m_ldlBlock,
			"Food.InformationDetails"))->Hide();
		static_cast<ctp2_Static*>(aui_Ldl::GetObject(m_ldlBlock,
			"Production.InformationDetails"))->Hide();
		static_cast<ctp2_Static*>(aui_Ldl::GetObject(m_ldlBlock,
			"Commerce.InformationDetails"))->Hide();
		static_cast<ctp2_Static*>(aui_Ldl::GetObject(m_ldlBlock,
			"Food.InformationSummary"))->Show();
		static_cast<ctp2_Static*>(aui_Ldl::GetObject(m_ldlBlock,
			"Production.InformationSummary"))->Show();
		static_cast<ctp2_Static*>(aui_Ldl::GetObject(m_ldlBlock,
			"Commerce.InformationSummary"))->Show();
	}
}


void CauseAndEffectTab::UpdateGeneral()
{
	
	Player *player = g_player[g_selected_item->GetVisiblePlayer()];

	
	static char stringBuffer[16];

	
	sprintf(stringBuffer, "%d", player->GetNumCities());
	m_numberOfCities->SetText(stringBuffer);
	sprintf(stringBuffer, "%d", player->GetTotalPopulation()*k_PEOPLE_PER_POPULATION+player->GetPartialPopulation());
	m_population->SetText(stringBuffer);

	
	sint32 happiness;
	happiness=player->GetAverageHappiness();
	sprintf(stringBuffer, "%d", happiness);
	m_happinessValue->SetText(stringBuffer);

	
	const GovernmentRecord *government =
		g_theGovernmentDB->Get(player->GetGovernmentType());

	
	m_government->SetText(government->GetNameText());
}


void CauseAndEffectTab::UpdateFoodSpinners()
{
	
	Player *player = g_player[g_selected_item->GetVisiblePlayer()];

	
	m_foodRationsSpinner->SetValue(player->GetUnitlessRations() +
		k_ZERO_FOUR__NEG_TWO_TWO_CONVERSION, 0);
}

void CauseAndEffectTab::UpdateProductionSpinners()
{
	
	Player *player = g_player[g_selected_item->GetVisiblePlayer()];

	
	m_productionWorkdaySpinner->SetValue(-player->GetUnitlessWorkday() +
		k_ZERO_FOUR__NEG_TWO_TWO_CONVERSION, 0);

	
	m_productionPublicWorksSpinner->SetValue(
		static_cast<sint32>((player->m_materialsTax * 100.0) + 0.5), 0);
}

void CauseAndEffectTab::UpdateCommerceSpinners()
{
	
	Player *player = g_player[g_selected_item->GetVisiblePlayer()];

	
	m_commerceWagesSpinner->SetValue(player->GetUnitlessWages() +
		k_ZERO_FOUR__NEG_TWO_TWO_CONVERSION, 0);

	
	double currentScienceTax = 0.0;
	player->GetScienceTaxRate(currentScienceTax);
	double const	maxScienceTax	= 
		g_theGovernmentDB->Get(player->GetGovernmentType())->GetMaxScienceRate();
	m_commerceScienceTaxSpinner->SetMaximum(AsPercentage(maxScienceTax), 0);
	m_commerceScienceTaxSpinner->SetValue(AsPercentage(currentScienceTax), 0);
}


void CauseAndEffectTab::UpdateFoodValues()
{
	
	sint32 totalFoodRequired = 0, totalFood = 0,
		totalFoodCrime = 0, totalFoodConsumed = 0;

	
	Player *player = g_player[g_selected_item->GetVisiblePlayer()];

	
	
	
	UnitDynamicArray *cityList = player->GetAllCitiesList();
	for(int cityIndex = 0; cityIndex < cityList->Num(); cityIndex++) {
		
		CityData *cityData = (*cityList)[cityIndex].GetData()->GetCityData();

		
		sint32 foodRequired =
			static_cast<sint32>(cityData->GetFoodRequired());

		
		sint32 food = static_cast<sint32>(cityData->GetGrossCityFood());

		
		static sint32 foodCrime = 0;
		cityData->GetFoodCrime(foodCrime);

		
		sint32 foodConsumed =
			static_cast<sint32>(cityData->GetConsumedFood());

		
		totalFoodRequired += foodRequired;
		totalFood += food;
		totalFoodCrime += foodCrime;
		totalFoodConsumed += foodConsumed;
	}

	
	sint32 subTotal = totalFoodCrime + totalFoodConsumed;
	sint32 totalFoodStored = totalFood - subTotal;

	
	static char stringBuffer[16];
	static char formatBuffer[64];

	
	sprintf(stringBuffer, "%d", totalFoodRequired);
	m_foodRequired->SetText(stringBuffer);
	sprintf(stringBuffer, "%d", totalFood);
	m_detailsFoodTotal->SetText(stringBuffer);
	m_summaryFoodTotal->SetText(stringBuffer);
	sprintf(stringBuffer, "%d", totalFoodCrime);
	m_detailsFoodCrime->SetText(stringBuffer);
	sprintf(stringBuffer, "%d", totalFoodConsumed);
	m_detailsFoodConsumed->SetText(stringBuffer);
	m_summaryFoodConsumed->SetText(stringBuffer);
	sprintf(stringBuffer, "%d", totalFoodStored);
	m_detailsFoodStored->SetText(stringBuffer);
	m_summaryFoodStored->SetText(stringBuffer);

	
	sint32 happiness = static_cast<sint32>(
		Happy::CalcCityIndependentRations(player));
	sprintf(stringBuffer, "%d", happiness);
	m_foodHappinessValue->SetText(stringBuffer);
	SetHappinessIcon(m_foodHappinessIcon, happiness);

	sprintf(formatBuffer, "EMPIRE_RATIONS_AMOUNT_%i",
		(int) player->GetUnitlessRations()+k_ZERO_FOUR__NEG_TWO_TWO_CONVERSION);
	sprintf(stringBuffer, "%s%s", g_theStringDB->GetNameStr(formatBuffer),
		g_theStringDB->GetNameStr("EMPIRE_RATIONS_AMOUNT_UNIT"));
	m_foodCurValue->SetText(stringBuffer);
}

void CauseAndEffectTab::UpdateProductionValues()
{
	
	sint32 totalProduction = 0, totalProductionCrime = 0,
		totalProductionUnitUpkeep = 0, totalProductionPublicWorks = 0;

	
	Player *player = g_player[g_selected_item->GetVisiblePlayer()];

	
	
	
	UnitDynamicArray *cityList = player->GetAllCitiesList();
	for(int cityIndex = 0; cityIndex < cityList->Num(); cityIndex++) {
		
		CityData *cityData = (*cityList)[cityIndex].GetData()->GetCityData();

		
		sint32 production =
			static_cast<sint32>(cityData->GetGrossCityProduction());

		
		sint32 productionCrime = cityData->GetProdCrime();

		
		sint32 productionPublicWorks =
			cityData->ComputeMaterialsPaid(player->m_materialsTax);

		
		totalProduction += production;
		totalProductionCrime += productionCrime;
		totalProductionPublicWorks += productionPublicWorks;
	}

	
	totalProductionUnitUpkeep = player->GetReadinessCost();

	
	sint32 subTotal = totalProductionCrime + totalProductionUnitUpkeep;
	sint32 totalProductionCityUse = totalProduction -
		(subTotal + totalProductionPublicWorks);

	
	static char stringBuffer[16];
	static char formatBuffer[64];

	
	sprintf(stringBuffer, "%d", totalProduction);
	m_detailsProductionTotal->SetText(stringBuffer);
	m_summaryProductionTotal->SetText(stringBuffer);
	sprintf(stringBuffer, "%d", totalProductionCrime);
	m_detailsProductionCrime->SetText(stringBuffer);
	sprintf(stringBuffer, "%d", totalProductionUnitUpkeep);
	m_detailsProductionUnitUpkeep->SetText(stringBuffer);
	sprintf(stringBuffer, "%d", totalProductionPublicWorks);
	m_detailsProductionPublicWorks->SetText(stringBuffer);
	m_summaryProductionPublicWorks->SetText(stringBuffer);
	sprintf(stringBuffer, "%d", totalProductionCityUse);
	m_detailsProductionCityUse->SetText(stringBuffer);
	m_summaryProductionCityUse->SetText(stringBuffer);

	
	sint32 happiness = static_cast<sint32>(
		Happy::CalcCityIndependentWorkday(player));
	sprintf(stringBuffer, "%d", happiness);
	m_productionHappinessValue->SetText(stringBuffer);
	SetHappinessIcon(m_productionHappinessIcon, happiness);
	
	sprintf(formatBuffer, "EMPIRE_WORKDAY_AMOUNT_%i",
		(int) (4-(player->GetUnitlessWorkday()+k_ZERO_FOUR__NEG_TWO_TWO_CONVERSION)));
	sprintf(stringBuffer, "%s%s", g_theStringDB->GetNameStr(formatBuffer),
		g_theStringDB->GetNameStr("EMPIRE_WORKDAY_AMOUNT_UNIT"));
	m_productionCurValue->SetText(stringBuffer);
}

void CauseAndEffectTab::UpdateCommerceValues()
{
	
	sint32 totalCommerce = 0, totalCommerceCrime = 0,
		totalCommerceWages = 0, totalCommerceBuildingUpkeep = 0,
		totalCommerceScience = 0, totalCommerceSavings = 0;

	
	Player *player = g_player[g_selected_item->GetVisiblePlayer()];

	
	
	
	UnitDynamicArray *cityList = player->GetAllCitiesList();
	for(int cityIndex = 0; cityIndex < cityList->Num(); cityIndex++) {
		
		CityData *cityData = (*cityList)[cityIndex].GetData()->GetCityData();

		
		sint32 commerce =
			static_cast<sint32>(cityData->GetGrossCityGold());

		
		sint32 commerceCrime = cityData->GetTradeCrime();

		
		sint32 commerceWages = cityData->CalcWages(
			static_cast<sint32>(player->GetWagesPerPerson()));

		
		sint32 commerceBuildingUpkeep = buildingutil_GetTotalUpkeep(
			cityData->GetImprovements(),
			wonderutil_GetDecreaseMaintenance(player->m_builtWonders));

		
		
		
		
		sint32 commerceScience = cityData->GetScience();

		
		sint32 commerceSavings = cityData->GetNetCityGold();

		
		totalCommerce += commerce;
		totalCommerceCrime += commerceCrime;
		totalCommerceWages += commerceWages;
		totalCommerceBuildingUpkeep += commerceBuildingUpkeep;
		totalCommerceScience += commerceScience;
		totalCommerceSavings += commerceSavings;
	}

	
	sint32 subTotal = totalCommerceCrime + totalCommerceWages +
		totalCommerceBuildingUpkeep;

	
	totalCommerceSavings += player->CalcWonderGold();

	
	static char stringBuffer[16];
	static char formatBuffer[64];

	
	sprintf(stringBuffer, "%d", totalCommerce);
	m_detailsCommerceTotal->SetText(stringBuffer);
	m_summaryCommerceTotal->SetText(stringBuffer);
	sprintf(stringBuffer, "%d", totalCommerceCrime);
	m_detailsCommerceCrime->SetText(stringBuffer);
	sprintf(stringBuffer, "%d", totalCommerceWages);
	m_detailsCommerceWages->SetText(stringBuffer);
	sprintf(stringBuffer, "%d", totalCommerceBuildingUpkeep);
	m_detailsCommerceBuildingUpkeep->SetText(stringBuffer);
	sprintf(stringBuffer, "%d", totalCommerceScience);
	m_detailsCommerceScience->SetText(stringBuffer);
	m_summaryCommerceScience->SetText(stringBuffer);
	sprintf(stringBuffer, "%d", totalCommerceSavings);
	m_detailsCommerceSavings->SetText(stringBuffer);
	m_summaryCommerceSavings->SetText(stringBuffer);

	
	sint32 happiness = static_cast<sint32>(
		Happy::CalcCityIndependentWages(player));
	sprintf(stringBuffer, "%d", happiness);
	m_commerceHappinessValue->SetText(stringBuffer);
	SetHappinessIcon(m_commerceHappinessIcon, happiness);

	sprintf(formatBuffer, "EMPIRE_COMMERCE_AMOUNT_%i",
		(int) player->GetUnitlessWages()+k_ZERO_FOUR__NEG_TWO_TWO_CONVERSION);
	sprintf(stringBuffer, "%s%s", g_theStringDB->GetNameStr(formatBuffer),
		g_theStringDB->GetNameStr("EMPIRE_COMMERCE_AMOUNT_UNIT"));
	m_commerceCurValue->SetText(stringBuffer);
}



void CauseAndEffectTab::SetHappinessIcon(ctp2_Static *control,
										 sint32 happiness)
{
	
	if(happiness < -1)
		control->ExchangeImage(0, 0, "updi43.tga");
	else if(happiness == -1)
		control->ExchangeImage(0, 0, "updi42.tga");
	else if(happiness == 0)
		control->ExchangeImage(0, 0, "updi41.tga");
	else if(happiness == 1)
		control->ExchangeImage(0, 0, "updi40.tga");
	else if(happiness > 1)
		control->ExchangeImage(0, 0, "updi39.tga");
	else {
		Assert(false);	
	}

	
	control->ShouldDraw();
}



void CauseAndEffectTab::UpdateCities()
{
	
	UnitDynamicArray *cityList =
		g_player[g_selected_item->GetVisiblePlayer()]->GetAllCitiesList();
	for(int cityIndex = 0; cityIndex < cityList->Num(); cityIndex++) {
		
		CityData *cityData = (*cityList)[cityIndex].GetData()->GetCityData();

		
		static sint32 gold = 0;
		
		cityData->CalcHappiness(gold, false); 
		cityData->CollectResources();
		cityData->DoSupport(true);
		cityData->SplitScience(true);
		cityData->ProcessFood();
		cityData->CollectOtherTrade(true, false);
		cityData->ProcessProduction(true);
		
		cityData->EatFood();
		cityData->CalculateGrowthRate();
	}
}


AUI_ERRCODE CauseAndEffectTab::HappinessBarActionCallback(ctp2_Static *control,
	aui_Surface *surface, RECT &rect, void *cookie)
{
	
	if(g_selected_item == NULL)
		return(AUI_ERRCODE_OK);

	
	sint32 hapvals[3];
	g_player[g_selected_item->GetVisiblePlayer()]->CountCityHappiness(
		hapvals[0],hapvals[1],hapvals[2]);

	
	float total = static_cast<float>(hapvals[0]+hapvals[1]+hapvals[2]);

	
	RECT barRect=rect;
	barRect.top += 2;
	barRect.left += 2;
	barRect.right -= 2;
	barRect.bottom -= 2;

	
	if(total <= 0.0f)
		primitives_PaintRect16(surface, &barRect, g_colorSet->GetColor(COLOR_YELLOW));
	else {
		
		float width = static_cast<float>(barRect.right - barRect.left);

		
		Pixel16 colors[3] = {
			g_colorSet->GetColor(COLOR_RED),
			g_colorSet->GetColor(COLOR_YELLOW),
			g_colorSet->GetColor(COLOR_GREEN)
		};

		
		
		for (uint32 index = 0; index < 3; index++) {
			
			barRect.right = barRect.left +
				static_cast<sint32>(
				(static_cast<float>(hapvals[index]) /
				total) * width);
			
			primitives_PaintRect16(surface, &barRect, colors[index]);
			
			barRect.left = barRect.right;
		}
	}

	
	return(AUI_ERRCODE_OK);
}


void CauseAndEffectTab::RationsSpinnerActionCallback(aui_Control *control,
													 uint32 action, uint32 data,
													 void *cookie)
{
	
	if(action != static_cast<uint32>(AUI_RANGER_ACTION_VALUECHANGE))
		return;

	
	CauseAndEffectTab *tab = static_cast<CauseAndEffectTab*>(cookie);
	ctp2_Spinner *spinner = static_cast<ctp2_Spinner*>(control);

	
	Player *player = g_player[g_selected_item->GetVisiblePlayer()];

	
	sint32 rationLevelSet = spinner->GetValueX() -
		k_ZERO_FOUR__NEG_TWO_TWO_CONVERSION;
	if(rationLevelSet != player->GetUnitlessRations())	{ 
		player->SetRationsLevel(rationLevelSet);
		UpdateCities();
	}

	
	tab->UpdateFoodValues();
	tab->UpdateGeneral();
	tab->m_tabPanel->ShouldDraw(TRUE);
}

void CauseAndEffectTab::WorkdaySpinnerActionCallback(aui_Control *control,
	uint32 action, uint32 data, void *cookie)
{
	
	if(action != static_cast<uint32>(AUI_RANGER_ACTION_VALUECHANGE))
		return;

	
	CauseAndEffectTab *tab = static_cast<CauseAndEffectTab*>(cookie);
	ctp2_Spinner *spinner = static_cast<ctp2_Spinner*>(control);

	
	Player *player = g_player[g_selected_item->GetVisiblePlayer()];

	
	sint32 workdayLevelSet = spinner->GetValueX() -
		k_ZERO_FOUR__NEG_TWO_TWO_CONVERSION;
	if(workdayLevelSet != player->GetUnitlessWorkday())	{ 
		player->SetWorkdayLevel(-workdayLevelSet);
		UpdateCities();
	}

	
	tab->UpdateProductionValues();
	tab->UpdateGeneral();
	tab->m_tabPanel->ShouldDraw(TRUE);
}

void CauseAndEffectTab::PublicWorksSpinnerActionCallback(aui_Control *control,
	uint32 action, uint32 data, void *cookie)
{
	
	if(action != static_cast<uint32>(AUI_RANGER_ACTION_VALUECHANGE))
		return;

	
	CauseAndEffectTab *tab = static_cast<CauseAndEffectTab*>(cookie);
	ctp2_Spinner *spinner = static_cast<ctp2_Spinner*>(control);

	
	Player *player = g_player[g_selected_item->GetVisiblePlayer()];

	
	sint32 publicWorksLevelSet = spinner->GetValueX();
	if(publicWorksLevelSet !=
		static_cast<sint32>((player->m_materialsTax * 100.0) + 0.5))	{ 
		player->SetMaterialsTax(
			static_cast<double>(publicWorksLevelSet) / 100.0);
		UpdateCities();
	}

	
	tab->UpdateProductionValues();
	tab->UpdateGeneral();
	tab->m_tabPanel->ShouldDraw(TRUE);
}

void CauseAndEffectTab::WagesSpinnerActionCallback(aui_Control *control,
	uint32 action, uint32 data, void *cookie)
{
	
	if(action != static_cast<uint32>(AUI_RANGER_ACTION_VALUECHANGE))
		return;

	
	CauseAndEffectTab *tab = static_cast<CauseAndEffectTab*>(cookie);
	ctp2_Spinner *spinner = static_cast<ctp2_Spinner*>(control);

	
	Player *player = g_player[g_selected_item->GetVisiblePlayer()];

	
	sint32 wagesLevelSet = spinner->GetValueX() -
		k_ZERO_FOUR__NEG_TWO_TWO_CONVERSION;
	if(wagesLevelSet != player->GetUnitlessWages())	{ 
		player->SetWagesLevel(wagesLevelSet);
		UpdateCities();
	}

	
	tab->UpdateCommerceValues();
	tab->UpdateGeneral();
	tab->m_tabPanel->ShouldDraw(TRUE);
}

void CauseAndEffectTab::ScienceTaxSpinnerActionCallback(aui_Control *control,
	uint32 action, uint32 data, void *cookie)
{
	
	if(action != static_cast<uint32>(AUI_RANGER_ACTION_VALUECHANGE))
		return;

	
	CauseAndEffectTab *tab = static_cast<CauseAndEffectTab*>(cookie);
	ctp2_Spinner *spinner = static_cast<ctp2_Spinner*>(control);

	
	Player *player = g_player[g_selected_item->GetVisiblePlayer()];

	
	sint32 scienceTaxLevelSet = spinner->GetValueX();
	double currentScienceTax = 0.0;
	player->GetScienceTaxRate(currentScienceTax);
	if (scienceTaxLevelSet != AsPercentage(currentScienceTax))
	{
		player->SetTaxes(
			static_cast<double>(scienceTaxLevelSet) / 100.0);
		UpdateCities();
	}

	
	tab->UpdateCommerceValues();
	tab->UpdateGeneral();
	tab->m_tabPanel->ShouldDraw(TRUE);
}


void CauseAndEffectTab::DetailsButtonActionCallback(aui_Control *control,
	uint32 action, uint32 data, void *cookie)
{
	
	if(action != static_cast<uint32>(AUI_BUTTON_ACTION_EXECUTE))
		return;

	
	static_cast<CauseAndEffectTab*>(cookie)->DisplayDetails(
		!(static_cast<CauseAndEffectTab*>(cookie)->m_displayDetails));

	static_cast<CauseAndEffectTab*>(cookie)->m_tabPanel->ShouldDraw(TRUE);
}



void CauseAndEffectTab::DetailsShowCallback(aui_Region *region,
											void *userData)
{
	
	CauseAndEffectTab *dialog = static_cast<CauseAndEffectTab*>(userData);

	
	dialog->DisplayDetails(dialog->m_displayDetails);
}

void CauseAndEffectTab::CauseAndEffectTabActionCallback(aui_Control *control,
	uint32 action, uint32 data, void *cookie)
{
	if(action == ctp2_Tab::ACTION_ACTIVATED)
	{
		((CauseAndEffectTab*)cookie)->m_detailsButton->Show();
	}
	else if(action == ctp2_Tab::ACTION_DEACTIVATED)
	{
		((CauseAndEffectTab*)cookie)->m_detailsButton->Hide();
	}

	



	

}
