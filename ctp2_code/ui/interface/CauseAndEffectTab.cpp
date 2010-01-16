//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : Empire manager main tab
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
// - Use the same science percentage everywhere.
// - Added optimize sliders button and according callback function to allow
//   the player to optimize sliders, automaticly. - April 8th 2005 Martin Gühmann
// - Backwards compatibility crash prevention
// - All food, production and gold values are now updated, when a single 
//   slider is moved, because happiness modifies crime, crime modifies
//   losses and production modifies pollution and pollution modifies crime.
//   This means all the values are modified even if only a single slider
//   is moved. Jul 7th 2005 Martin Gühmann
// - Added preparations for city resource calculation replacement. (Aug 12th 2005 Martin Gühmann)
// - Added SupportGold and CommodityGold to totalsaving calculation
// - TODO: need to expand this window and break down income (add franchises, advertsing etc)
// - Added more information to empire manager domestic tab. (22-Jul-2009 Maq)
//
//----------------------------------------------------------------------------

#include "c3.h"


#include "CauseAndEffectTab.h"


#include "aui_ldl.h"
#include "aui_region.h"
#include "buildingutil.h"
#include "c3math.h"                     // AsPercentage
#include "c3slider.h"
#include "colorset.h"                   // g_colorSet
#include "ctp2_button.h"
#include "ctp2_spinner.h"
#include "ctp2_Static.h"
#include "ctp2_Tab.h"
#include "DomesticManagementDialog.h"
#include "FeatTracker.h"				// g_featTracker
#include "governor.h"                   // To allow automatic slider optimization
#include "GovernmentRecord.h"
#include "Happy.h"	
#include "network.h"
#include "pixelutils.h"
#include "player.h"
#include "PopRecord.h"					// g_thePopDB
#include "primitives.h"
#include "profileDB.h"					// g_theProfileDB
#include "SelItem.h"
#include "StrDB.h"
#include "UnitData.h"
#include "UnitDynArr.h"
#include "wonderutil.h"


static sint32 k_ONE_FIVE__NEG_TWO_TWO_CONVERSION = 3;
static sint32 k_ZERO_FOUR__NEG_TWO_TWO_CONVERSION = 2;


CauseAndEffectTab::CauseAndEffectTab(MBCHAR *ldlBlock) :
m_tabPanel(static_cast<ctp2_Static*>(aui_Ldl::GetObject(ldlBlock))),
m_detailsButton(static_cast<ctp2_Button*>(aui_Ldl::GetObject(
	"DomesticDialog.DetailsButton"))),
m_optimizeSliderButton(static_cast<ctp2_Button*>(aui_Ldl::GetObject(
	"DomesticDialog.OptimizeSlidersButton"))), // Added by Martin Gühmann
m_numberOfCities(static_cast<ctp2_Static*>(aui_Ldl::GetObject(ldlBlock,
	"CitiesValue"))),
m_population(static_cast<ctp2_Static*>(aui_Ldl::GetObject(ldlBlock,
	"PopulationValue"))),

m_productionPublicWorksSpinner(static_cast<ctp2_Spinner*>(aui_Ldl::GetObject(ldlBlock,
	"PWSpinner"))),

m_happinessValue(static_cast<ctp2_Static*>(aui_Ldl::GetObject(ldlBlock,
	"HappinessValue"))),
m_happinessBar(static_cast<ctp2_Static*>(aui_Ldl::GetObject(ldlBlock,
	"HappinessBar"))),

m_commerceScienceTaxSpinner(static_cast<ctp2_Spinner*>(aui_Ldl::GetObject(ldlBlock,
	"ScieTaxSpinner"))),

m_government(static_cast<ctp2_Static*>(aui_Ldl::GetObject(ldlBlock,
	"CurrentGovernment"))),
m_foodRationsSpinner(static_cast<C3Slider*>(aui_Ldl::GetObject(ldlBlock,
	"Food.Modify.Primary.Slider"))),
//m_foodRequired(static_cast<ctp2_Static*>(aui_Ldl::GetObject(ldlBlock,
//	"Food.Modify.Secondary.Value"))),
m_foodHappinessValue(static_cast<ctp2_Static*>(aui_Ldl::GetObject(ldlBlock,
	"Food.Modify.Primary.Value"))),
m_foodCurValue(static_cast<ctp2_Static*>(aui_Ldl::GetObject(ldlBlock,
	"Food.Modify.Primary.CurValue"))),
m_foodHappinessIcon(static_cast<ctp2_Static*>(aui_Ldl::GetObject(ldlBlock,
	"Food.Modify.Primary.Icon"))),

m_detailsFoodTotal(static_cast<ctp2_Static*>(aui_Ldl::GetObject(ldlBlock,
	"Food.InformationDetails.Collected.Value"))),
m_detailsFoodTerrainTotal(static_cast<ctp2_Static*>(aui_Ldl::GetObject(ldlBlock,
	"Food.InformationDetails.Terrain.Value"))),
m_detailsFoodFromBuildings(static_cast<ctp2_Static*>(aui_Ldl::GetObject(ldlBlock,
	"Food.InformationDetails.Buildings.Value"))),
m_detailsFoodFarmers(static_cast<ctp2_Static*>(aui_Ldl::GetObject(ldlBlock,
	"Food.InformationDetails.Farmers.Value"))),
m_detailsFoodFeatWonder(static_cast<ctp2_Static*>(aui_Ldl::GetObject(ldlBlock,
	"Food.InformationDetails.FeatsWonders.Value"))),
m_detailsFoodGovBonus(static_cast<ctp2_Static*>(aui_Ldl::GetObject(ldlBlock,
	"Food.InformationDetails.FoodGovBonus.Value"))),
m_detailsFoodCrime(static_cast<ctp2_Static*>(aui_Ldl::GetObject(ldlBlock,
	"Food.InformationDetails.Crime.Value"))),
m_detailsFoodConsumed(static_cast<ctp2_Static*>(aui_Ldl::GetObject(ldlBlock,
	"Food.InformationDetails.Consumed.Value"))),
m_detailsFoodStored(static_cast<ctp2_Static*>(aui_Ldl::GetObject(ldlBlock,
	"Food.InformationDetails.Remainder.Value"))),
m_summaryFoodTotal(static_cast<ctp2_Static*>(aui_Ldl::GetObject(ldlBlock,
	"Food.InformationSummary.Collected.Value"))),
m_summaryFoodConsumed(static_cast<ctp2_Static*>(aui_Ldl::GetObject(ldlBlock,
	"Food.InformationSummary.Consumed.Value"))),
m_summaryFoodCrime(static_cast<ctp2_Static*>(aui_Ldl::GetObject(ldlBlock,
	"Food.InformationSummary.Crime.Value"))),
m_summaryFoodStored(static_cast<ctp2_Static*>(aui_Ldl::GetObject(ldlBlock,
	"Food.InformationSummary.Remainder.Value"))),

m_productionWorkdaySpinner(static_cast<C3Slider*>(aui_Ldl::GetObject(ldlBlock,
	"Production.Modify.Primary.Slider"))),
m_productionHappinessValue(static_cast<ctp2_Static*>(aui_Ldl::GetObject(ldlBlock,
	"Production.Modify.Primary.Value"))),
m_productionCurValue(static_cast<ctp2_Static*>(aui_Ldl::GetObject(ldlBlock,
	"Production.Modify.Primary.CurValue"))),
m_productionHappinessIcon(static_cast<ctp2_Static*>(aui_Ldl::GetObject(ldlBlock,
	"Production.Modify.Primary.Icon"))),

m_detailsProductionTotal(static_cast<ctp2_Static*>(aui_Ldl::GetObject(ldlBlock,
	"Production.InformationDetails.Collected.Value"))),
m_detailsProdTerrainTotal(static_cast<ctp2_Static*>(aui_Ldl::GetObject(ldlBlock,
	"Production.InformationDetails.Terrain.Value"))),
m_detailsProdWorkdayTotal(static_cast<ctp2_Static*>(aui_Ldl::GetObject(ldlBlock,
	"Production.InformationDetails.Workday.Value"))),
m_detailsProdFromBuildingsTotal(static_cast<ctp2_Static*>(aui_Ldl::GetObject(ldlBlock,
	"Production.InformationDetails.ProdBuildings.Value"))),
m_detailsProductionLaborersTotal(static_cast<ctp2_Static*>(aui_Ldl::GetObject(ldlBlock,
	"Production.InformationDetails.Laborers.Value"))),
m_detailsProductionFeatWonderTotal(static_cast<ctp2_Static*>(aui_Ldl::GetObject(ldlBlock,
	"Production.InformationDetails.FeatsWonders.Value"))),
m_detailsProductionGovBonusTotal(static_cast<ctp2_Static*>(aui_Ldl::GetObject(ldlBlock,
	"Production.InformationDetails.ProdGovBonus.Value"))),
m_detailsProductionFranchiseBioLoss(static_cast<ctp2_Static*>(aui_Ldl::GetObject(ldlBlock,
	"Production.InformationDetails.Enemy.Value"))),
m_detailsProductionCrime(static_cast<ctp2_Static*>(aui_Ldl::GetObject(ldlBlock,
	"Production.InformationDetails.Crime.Value"))),
m_detailsProductionUnitUpkeep(static_cast<ctp2_Static*>(aui_Ldl::GetObject(ldlBlock,
	"Production.InformationDetails.Upkeep.Value"))),
m_detailsProductionPublicWorks(static_cast<ctp2_Static*>(aui_Ldl::GetObject(ldlBlock,
	"Production.InformationDetails.PublicWorks.Value"))),
m_detailsProductionCityUse(static_cast<ctp2_Static*>(aui_Ldl::GetObject(ldlBlock,
	"Production.InformationDetails.CityUse.Value"))),
m_summaryProductionTotal(static_cast<ctp2_Static*>(aui_Ldl::GetObject(ldlBlock,
	"Production.InformationSummary.Collected.Value"))),
m_summaryProductionFranchiseBioLoss(static_cast<ctp2_Static*>(aui_Ldl::GetObject(ldlBlock,
	"Production.InformationSummary.Enemy.Value"))),
m_summaryProductionUnitUpkeep(static_cast<ctp2_Static*>(aui_Ldl::GetObject(ldlBlock,
	"Production.InformationSummary.Upkeep.Value"))),
m_summaryProductionCrime(static_cast<ctp2_Static*>(aui_Ldl::GetObject(ldlBlock,
	"Production.InformationSummary.Crime.Value"))),
m_summaryProductionPublicWorks(static_cast<ctp2_Static*>(aui_Ldl::GetObject(ldlBlock,
	"Production.InformationSummary.PublicWorks.Value"))),
m_summaryProductionCityUse(static_cast<ctp2_Static*>(aui_Ldl::GetObject(ldlBlock,
	"Production.InformationSummary.CityUse.Value"))),

m_commerceWagesSpinner(static_cast<C3Slider*>(aui_Ldl::GetObject(ldlBlock,
	"Commerce.Modify.Primary.Slider"))),
m_commerceHappinessValue(static_cast<ctp2_Static*>(aui_Ldl::GetObject(ldlBlock,
	"Commerce.Modify.Primary.Value"))),
m_commerceCurValue(static_cast<ctp2_Static*>(aui_Ldl::GetObject(ldlBlock,
	"Commerce.Modify.Primary.CurValue"))),
m_commerceHappinessIcon(static_cast<ctp2_Static*>(aui_Ldl::GetObject(ldlBlock,
	"Commerce.Modify.Primary.Icon"))),

m_detailsCommerceTerrain(static_cast<ctp2_Static*>(aui_Ldl::GetObject(ldlBlock,
	"Commerce.InformationDetails.Terrain.Value"))),
m_detailsCommerceWages(static_cast<ctp2_Static*>(aui_Ldl::GetObject(ldlBlock,
	"Commerce.InformationDetails.Wages.Value"))),
m_detailsCommerceBuildingUpkeep(static_cast<ctp2_Static*>(aui_Ldl::GetObject(ldlBlock,
	"Commerce.InformationDetails.Upkeep.Value"))),
m_detailsCommerceTotal(static_cast<ctp2_Static*>(aui_Ldl::GetObject(ldlBlock,
	"Commerce.InformationDetails.Collected.Value"))),
m_detailsCommerceScienceBasic(static_cast<ctp2_Static*>(aui_Ldl::GetObject(ldlBlock,
	"Commerce.InformationDetails.BasicScience.Value"))),
m_detailsGoldFromCommerce(static_cast<ctp2_Static*>(aui_Ldl::GetObject(ldlBlock,
	"Commerce.InformationDetails.BasicGold.Value"))),
m_detailsCommerceScienceBuildings(static_cast<ctp2_Static*>(aui_Ldl::GetObject(ldlBlock,
	"Commerce.InformationDetails.ScienceBuildings.Value"))),
m_detailsCommerceGoldBuildings(static_cast<ctp2_Static*>(aui_Ldl::GetObject(ldlBlock,
	"Commerce.InformationDetails.GoldBuildings.Value"))),
m_detailsCommerceScientistsScie(static_cast<ctp2_Static*>(aui_Ldl::GetObject(ldlBlock,
	"Commerce.InformationDetails.Scientists.Value"))),
m_detailsCommerceFeatWonderScie(static_cast<ctp2_Static*>(aui_Ldl::GetObject(ldlBlock,
	"Commerce.InformationDetails.FeatsWondersScie.Value"))),
m_detailsCommerceMerchantsGold(static_cast<ctp2_Static*>(aui_Ldl::GetObject(ldlBlock,
	"Commerce.InformationDetails.Merchants.Value"))),
m_detailsCommerceFeatWonderGold(static_cast<ctp2_Static*>(aui_Ldl::GetObject(ldlBlock,
	"Commerce.InformationDetails.FeatsWondersGold.Value"))),
m_detailsCommerceTradeGold(static_cast<ctp2_Static*>(aui_Ldl::GetObject(ldlBlock,
	"Commerce.InformationDetails.TradeGold.Value"))),
m_detailsCommerceScienceGov(static_cast<ctp2_Static*>(aui_Ldl::GetObject(ldlBlock,
	"Commerce.InformationDetails.ScieGovBonus.Value"))),
m_detailsCommerceGoldGov(static_cast<ctp2_Static*>(aui_Ldl::GetObject(ldlBlock,
	"Commerce.InformationDetails.GoldGovBonus.Value"))),
m_detailsCommerceScieSubTotal(static_cast<ctp2_Static*>(aui_Ldl::GetObject(ldlBlock,
	"Commerce.InformationDetails.ScieSubTotal.Value"))),
m_detailsCommerceGoldSubTotal(static_cast<ctp2_Static*>(aui_Ldl::GetObject(ldlBlock,
	"Commerce.InformationDetails.GoldSubTotal.Value"))),
m_detailsCommerceConvGoldLoss(static_cast<ctp2_Static*>(aui_Ldl::GetObject(ldlBlock,
	"Commerce.InformationDetails.Enemy.Value"))),
m_detailsCommerceUnitWages(static_cast<ctp2_Static*>(aui_Ldl::GetObject(ldlBlock,
	"Commerce.InformationDetails.UnitWages.Value"))),
m_detailsCommerceScience(static_cast<ctp2_Static*>(aui_Ldl::GetObject(ldlBlock,
	"Commerce.InformationDetails.Science.Value"))),
m_detailsCommerceSavings(static_cast<ctp2_Static*>(aui_Ldl::GetObject(ldlBlock,
	"Commerce.InformationDetails.Savings.Value"))),
m_detailsCommerceConversionsGold(static_cast<ctp2_Static*>(aui_Ldl::GetObject(ldlBlock,
	"Commerce.InformationDetails.ConversionsGold.Value"))),
m_detailsCommerceScieCrime(static_cast<ctp2_Static*>(aui_Ldl::GetObject(ldlBlock,
	"Commerce.InformationDetails.ScieCrime.Value"))),
m_detailsCommerceGoldCrime(static_cast<ctp2_Static*>(aui_Ldl::GetObject(ldlBlock,
	"Commerce.InformationDetails.GoldCrime.Value"))),
m_summaryCommerceTotal(static_cast<ctp2_Static*>(aui_Ldl::GetObject(ldlBlock,
	"Commerce.InformationSummary.Collected.Value"))),
m_summaryCommerceScieSubTotal(static_cast<ctp2_Static*>(aui_Ldl::GetObject(ldlBlock,
	"Commerce.InformationSummary.ScieSubTotal.Value"))),
m_summaryCommerceGoldSubTotal(static_cast<ctp2_Static*>(aui_Ldl::GetObject(ldlBlock,
	"Commerce.InformationSummary.GoldSubTotal.Value"))),
m_summaryCommerceConvGoldLoss(static_cast<ctp2_Static*>(aui_Ldl::GetObject(ldlBlock,
	"Commerce.InformationSummary.Enemy.Value"))),
m_summaryCommerceUnitWages(static_cast<ctp2_Static*>(aui_Ldl::GetObject(ldlBlock,
	"Commerce.InformationSummary.UnitWages.Value"))),
m_summaryCommerceScieCrime(static_cast<ctp2_Static*>(aui_Ldl::GetObject(ldlBlock,
	"Commerce.InformationSummary.ScieCrime.Value"))),
m_summaryCommerceGoldCrime(static_cast<ctp2_Static*>(aui_Ldl::GetObject(ldlBlock,
	"Commerce.InformationSummary.GoldCrime.Value"))),
m_summaryCommerceScience(static_cast<ctp2_Static*>(aui_Ldl::GetObject(ldlBlock,
	"Commerce.InformationSummary.Science.Value"))),
m_summaryCommerceSavings(static_cast<ctp2_Static*>(aui_Ldl::GetObject(ldlBlock,
	"Commerce.InformationSummary.Savings.Value"))),
m_summaryCommerceConversionsGold(static_cast<ctp2_Static*>(aui_Ldl::GetObject(ldlBlock,
	"Commerce.InformationSummary.ConversionsGold.Value")))
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
	Assert(m_detailsFoodTerrainTotal);
	Assert(m_detailsFoodFromBuildings);
	Assert(m_detailsFoodFarmers);
	Assert(m_detailsFoodFeatWonder);
	Assert(m_detailsFoodGovBonus);
	Assert(m_detailsFoodCrime);
	Assert(m_detailsFoodConsumed);
	Assert(m_detailsFoodStored);
	Assert(m_summaryFoodTotal);
	Assert(m_summaryFoodConsumed);
	Assert(m_summaryFoodCrime);
	Assert(m_summaryFoodStored);
	Assert(m_productionWorkdaySpinner);
	Assert(m_productionPublicWorksSpinner);
	Assert(m_productionHappinessValue);
	Assert(m_productionHappinessIcon);
	Assert(m_detailsProductionTotal);
	Assert(m_detailsProdTerrainTotal);
	Assert(m_detailsProdWorkdayTotal);
	Assert(m_detailsProdFromBuildingsTotal);
	Assert(m_detailsProductionLaborersTotal);
	Assert(m_detailsProductionFeatWonderTotal);
	Assert(m_detailsProductionGovBonusTotal);
	Assert(m_detailsProductionFranchiseBioLoss);
	Assert(m_detailsProductionCrime);
	Assert(m_detailsProductionUnitUpkeep);
	Assert(m_detailsProductionPublicWorks);
	Assert(m_detailsProductionCityUse);
	Assert(m_summaryProductionTotal);
	Assert(m_summaryProductionFranchiseBioLoss);
	Assert(m_summaryProductionUnitUpkeep);
	Assert(m_summaryProductionCrime);
	Assert(m_summaryProductionPublicWorks);
	Assert(m_summaryProductionCityUse);
	Assert(m_commerceWagesSpinner);
	Assert(m_commerceScienceTaxSpinner);
	Assert(m_commerceHappinessValue);
	Assert(m_commerceHappinessIcon);
	Assert(m_detailsCommerceTerrain);
	Assert(m_detailsCommerceWages);
	Assert(m_detailsCommerceBuildingUpkeep);
	Assert(m_detailsCommerceTotal);
	Assert(m_detailsCommerceScieCrime);
	Assert(m_detailsCommerceGoldCrime);
	Assert(m_detailsCommerceScienceBasic);
	Assert(m_detailsGoldFromCommerce);
	Assert(m_detailsCommerceScienceBuildings);
	Assert(m_detailsCommerceGoldBuildings);
	Assert(m_detailsCommerceScientistsScie);
	Assert(m_detailsCommerceFeatWonderScie);
	Assert(m_detailsCommerceMerchantsGold);
	Assert(m_detailsCommerceFeatWonderGold);
	Assert(m_detailsCommerceTradeGold);
	Assert(m_detailsCommerceConversionsGold);
	Assert(m_detailsCommerceScienceGov);
	Assert(m_detailsCommerceGoldGov);
	Assert(m_detailsCommerceScieSubTotal);
	Assert(m_detailsCommerceGoldSubTotal);
	Assert(m_detailsCommerceConvGoldLoss);
	Assert(m_detailsCommerceUnitWages);
	Assert(m_detailsCommerceScience);
	Assert(m_detailsCommerceSavings);
	Assert(m_summaryCommerceTotal);
	Assert(m_summaryCommerceScieSubTotal);
	Assert(m_summaryCommerceGoldSubTotal);
	Assert(m_summaryCommerceConvGoldLoss);
	Assert(m_summaryCommerceUnitWages);
	Assert(m_summaryCommerceScieCrime);
	Assert(m_summaryCommerceGoldCrime);
	Assert(m_summaryCommerceScience);
	Assert(m_summaryCommerceSavings);
	Assert(m_summaryCommerceConversionsGold);

	m_tab = (static_cast<ctp2_Tab*>(aui_Ldl::GetObject("DomesticDialog.TabGroup.Tab1")));

	strcpy(m_ldlBlock,ldlBlock);

	
	DisplayDetails(false);

	
	m_tabPanel->SetShowCallback(DetailsShowCallback, this);
	m_detailsButton->SetActionFuncAndCookie(
		DetailsButtonActionCallback, this);

	if (m_optimizeSliderButton)
	{
		m_optimizeSliderButton->SetActionFuncAndCookie(OptimizeSlidersButtonActionCallback, this);
	}
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

	
	m_foodRationsSpinner->SetValue(static_cast<sint32>(player->GetUnitlessRations()) +
		k_ZERO_FOUR__NEG_TWO_TWO_CONVERSION, 0);
}

void CauseAndEffectTab::UpdateProductionSpinners()
{
	
	Player *player = g_player[g_selected_item->GetVisiblePlayer()];

	
	m_productionWorkdaySpinner->SetValue(static_cast<sint32>(-player->GetUnitlessWorkday()) +
		k_ZERO_FOUR__NEG_TWO_TWO_CONVERSION, 0);

	
	m_productionPublicWorksSpinner->SetValue(
		static_cast<sint32>((player->m_materialsTax * 100.0) + 0.5), 0);
}

void CauseAndEffectTab::UpdateCommerceSpinners()
{
	
	Player *player = g_player[g_selected_item->GetVisiblePlayer()];

	
	m_commerceWagesSpinner->SetValue(static_cast<sint32>(player->GetUnitlessWages()) +
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
	
	double totalFoodRequired = 0.0, totalFood = 0.0,
		totalFoodCrime = 0.0, totalFoodConsumed = 0.0,
		totalTerrainFood = 0.0, totalFoodFromBuildings = 0.0,
		totalFoodGovBonus = 0.0, totalFoodFarmers = 0.0,
		totalFeatWonderFood = 0.0;

	
	Player *player = g_player[g_selected_item->GetVisiblePlayer()];

	
	
	
	UnitDynamicArray *cityList = player->GetAllCitiesList();
	for(int cityIndex = 0; cityIndex < cityList->Num(); cityIndex++) {
		
		CityData *cityData = (*cityList)[cityIndex].GetData()->GetCityData();
		sint32 bestSpecialist = cityData->GetBestSpecialist(POP_FARMER);
		
		// Required.
		double foodRequired = cityData->GetFoodRequired();

		// Gross food.
		double food = static_cast<double>(cityData->GetGrossCityFood());
		double noBonusesGrossFood = static_cast<double>(cityData->GetGrossCityFoodBeforeBonuses());

		// Buildings.
		double foodBonus;
		buildingutil_GetFoodPercent(cityData->GetEffectiveBuildings(), foodBonus, cityData->GetOwner());
		double foodBuildingBonus = noBonusesGrossFood * foodBonus;

		// Farmer specialists.
        double farmersFood = 0.0;
		if (cityData->FarmerCount() > 0)
		{
			farmersFood = static_cast<double>(cityData->FarmerCount() *
				g_thePopDB->Get(bestSpecialist, player->GetGovernmentType())->GetFood());
		}

		// Feats & Wonder bonuses (no food feat ability yet - 13-Jul-2009-Maq)
		double featWonderFood = static_cast<double>(wonderutil_GetIncreaseFoodAllCities(
			player->m_builtWonders));

		// Gov bonus.
        double totalFoodWithoutGov = noBonusesGrossFood + foodBuildingBonus
			+ farmersFood + featWonderFood;
		double foodGovBonus = ceil(totalFoodWithoutGov 
			* g_theGovernmentDB->Get(player->m_government_type)->GetFoodCoef());
		foodGovBonus -= totalFoodWithoutGov;

		// Crime.
		sint32 ifoodCrime = 0;
		cityData->GetFoodCrime(ifoodCrime);
		double foodCrime = static_cast<double>(ifoodCrime);

		// Consumed.
		double foodConsumed = cityData->GetConsumedFood();

		// TOTALS
		totalTerrainFood += noBonusesGrossFood;
        totalFoodFromBuildings += foodBuildingBonus;
		totalFoodFarmers += farmersFood;
        totalFoodGovBonus += foodGovBonus;
		totalFoodRequired += foodRequired;
		totalFood += food;
		totalFeatWonderFood += featWonderFood;
		totalFoodCrime += foodCrime;
		totalFoodConsumed += foodConsumed;
	}

	
	double subTotal = totalFoodCrime + totalFoodConsumed;
	double totalFoodStored = totalFood - subTotal;

	
	static char stringBuffer[16];
	static char formatBuffer[64];

	
	//sprintf(stringBuffer, "%d", totalFoodRequired);
	//m_foodRequired->SetText(stringBuffer);

	sprintf(stringBuffer, "%.0f", totalTerrainFood);
	m_detailsFoodTerrainTotal->SetText(stringBuffer);

	sprintf(stringBuffer, "%.0f", totalFoodFromBuildings);
	m_detailsFoodFromBuildings->SetText(stringBuffer);

	sprintf(stringBuffer, "%.0f", totalFoodFarmers);
	m_detailsFoodFarmers->SetText(stringBuffer);

	sprintf(stringBuffer, "%.0f", totalFeatWonderFood);
	m_detailsFoodFeatWonder->SetText(stringBuffer);

	sprintf(stringBuffer, "%.0f", totalFoodGovBonus);
	m_detailsFoodGovBonus->SetText(stringBuffer);

	sprintf(stringBuffer, "%.0f", totalFood);
	m_detailsFoodTotal->SetText(stringBuffer);
	m_summaryFoodTotal->SetText(stringBuffer);

	sprintf(stringBuffer, "%.0f", totalFoodCrime);
	m_detailsFoodCrime->SetText(stringBuffer);
	m_summaryFoodCrime->SetText(stringBuffer);

	sprintf(stringBuffer, "%.0f", totalFoodConsumed);
	m_detailsFoodConsumed->SetText(stringBuffer);
	m_summaryFoodConsumed->SetText(stringBuffer);

	sprintf(stringBuffer, "%.0f", totalFoodStored);
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
	
	double totalProduction = 0.0, totalProductionCrime = 0.0,
		totalTerrainProd = 0.0, totalProdFromBuildings = 0.0,
		totalLaborersProd = 0.0, totalProdGovBonus = 0.0,
		totalFranchiseBioLoss = 0.0, totalFeatWonderProd = 0.0,
		totalWorkdayBonus = 0.0;
	sint32 totalProductionUnitUpkeep = 0, totalProductionPublicWorks = 0;

	
	Player *player = g_player[g_selected_item->GetVisiblePlayer()];

	
	
	
	UnitDynamicArray *cityList = player->GetAllCitiesList();
	for(int cityIndex = 0; cityIndex < cityList->Num(); cityIndex++) {
		
		CityData *cityData = (*cityList)[cityIndex].GetData()->GetCityData();
		sint32 bestSpecialist = cityData->GetBestSpecialist(POP_LABORER);

		// Gross production.
		double production = static_cast<double>(cityData->GetGrossCityProduction());

		// Terrain (raw terrain values).
		double prodNoBonuses = static_cast<double>(cityData->GetGrossCityProdBeforeBonuses());

		// Workday bonus (terrain * workday - terrain).
		double workdayBonus = ceil(prodNoBonuses * player->GetWorkdayPerPerson());
		workdayBonus -= prodNoBonuses;

		// Buildings bonus (terrain * building).
		double prodBuildingBonus;
		double prodBonus;
		buildingutil_GetProductionPercent(cityData->GetEffectiveBuildings(), prodBonus, cityData->GetOwner());
		prodBuildingBonus = ceil(prodBonus * (prodNoBonuses + workdayBonus));

		double buildingsWorkdayAndBaseProd = prodNoBonuses + workdayBonus + prodBuildingBonus;

		// Feats and wonders.
		double featWonderProd = 0.0;
		double featProd = 0.0;
		double wonderProd = 0.0;
		sint32 featPercent = g_featTracker->GetAdditiveEffect(FEAT_EFFECT_INCREASE_PRODUCTION, cityData->GetOwner());
		featProd = ceil(buildingsWorkdayAndBaseProd * (static_cast<double>(featPercent) / 100.0));
		wonderProd = ceil((buildingsWorkdayAndBaseProd + featProd) *
	                        (wonderutil_GetIncreaseProduction(player->m_builtWonders) * 0.01));

		featWonderProd = (featProd + wonderProd);
		
		// Labourers bonus.
        double laborersProd = 0.0;
		if (cityData->LaborerCount() > 0)
		{
			laborersProd = static_cast<double>(cityData->LaborerCount() *
				g_thePopDB->Get(bestSpecialist, player->GetGovernmentType())->GetProduction());
		}

		// Gov bonus.
        double totalProdWithoutGov = prodNoBonuses + workdayBonus
			+ prodBuildingBonus + laborersProd + featWonderProd;
		double prodGovBonus = ceil(totalProdWithoutGov 
			* g_theGovernmentDB->Get(player->m_government_type)->GetProductionCoef());
		prodGovBonus -= totalProdWithoutGov;

		// Franchise and bio infection losses.
		sint32 tmpProduction = 0, crimeLoss = 0, franchiseLoss = 0;
		tmpProduction = static_cast<sint32>(production);

#if defined(NEW_RESOURCE_PROCESS)
		double franchiseBioLoss = 0.0;
#else
		//  Production is returned with only bio infection removed, crimeLoss and franchiseLoss are filled:
		sint32 franchiseBioLoss = cityData->ComputeProductionLosses(tmpProduction, crimeLoss, franchiseLoss);
		// This now leaves bio infection loss:
		franchiseBioLoss = tmpProduction - franchiseBioLoss;
		// Now add franchise loss:
		franchiseBioLoss += franchiseLoss;
		if(franchiseBioLoss < 0)
		{
			franchiseBioLoss = 0;
		}
#endif

		// Crime.
		sint32 productionCrime = cityData->GetProdCrime();

		
		sint32 productionPublicWorks =
			cityData->ComputeMaterialsPaid(player->m_materialsTax);

		totalTerrainProd += prodNoBonuses;
		totalWorkdayBonus += workdayBonus;
        totalProdFromBuildings += prodBuildingBonus;
		totalLaborersProd += laborersProd;
		totalFeatWonderProd += featWonderProd;
		totalProdGovBonus += prodGovBonus;
		totalProduction += production;// "sub total"
		totalFranchiseBioLoss += franchiseBioLoss;
		totalProductionCrime += productionCrime;
		totalProductionPublicWorks += productionPublicWorks;
	}
	
	totalProductionUnitUpkeep = player->GetReadinessCost();

	// This always returns 0, seems it is reset once the franchise
	// amount is given.
	//sint32 franchiseGain = player->GetProductionFromFranchises();

	//if(franchiseGain >= totalProductionUnitUpkeep) {
	//	franchiseGain -= totalProductionUnitUpkeep;
	//	totalProductionUnitUpkeep = 0;
	//	totalProductionPublicWorks += franchiseGain;
	//}
	//else
	//{
	//	totalProductionUnitUpkeep -= franchiseGain;
	//}

	
	double deductions = totalProductionCrime + totalProductionUnitUpkeep + totalFranchiseBioLoss;
	double totalProductionCityUse = totalProduction -
		(deductions + totalProductionPublicWorks);

	
	static char stringBuffer[16];
	static char formatBuffer[64];

	// sub total
	sprintf(stringBuffer, "%.0f", totalProduction);
	m_detailsProductionTotal->SetText(stringBuffer);
	m_summaryProductionTotal->SetText(stringBuffer);

	sprintf(stringBuffer, "%.0f", totalTerrainProd);
	m_detailsProdTerrainTotal->SetText(stringBuffer);

	sprintf(stringBuffer, "%.0f", totalWorkdayBonus);
	m_detailsProdWorkdayTotal->SetText(stringBuffer);

	sprintf(stringBuffer, "%.0f", totalProdFromBuildings);
	m_detailsProdFromBuildingsTotal->SetText(stringBuffer);

	sprintf(stringBuffer, "%.0f", totalLaborersProd);
	m_detailsProductionLaborersTotal->SetText(stringBuffer);

	sprintf(stringBuffer, "%.0f", totalFeatWonderProd);
	m_detailsProductionFeatWonderTotal->SetText(stringBuffer);

	sprintf(stringBuffer, "%.0f", totalProdGovBonus);
	m_detailsProductionGovBonusTotal->SetText(stringBuffer);

	sprintf(stringBuffer, "%.0f", totalFranchiseBioLoss);
	m_detailsProductionFranchiseBioLoss->SetText(stringBuffer);
	m_summaryProductionFranchiseBioLoss->SetText(stringBuffer);

	sprintf(stringBuffer, "%.0f", totalProductionCrime);
	m_detailsProductionCrime->SetText(stringBuffer);
	m_summaryProductionCrime->SetText(stringBuffer);

	sprintf(stringBuffer, "%d", totalProductionUnitUpkeep);
	m_detailsProductionUnitUpkeep->SetText(stringBuffer);
	m_summaryProductionUnitUpkeep->SetText(stringBuffer);

	sprintf(stringBuffer, "%d", totalProductionPublicWorks);
	m_detailsProductionPublicWorks->SetText(stringBuffer);
	m_summaryProductionPublicWorks->SetText(stringBuffer);

	sprintf(stringBuffer, "%.0f", totalProductionCityUse);
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
	
	sint32 totalCommerce = 0, totalCommerceGoldCrime = 0,
		totalCommerceWages = 0, totalCommerceCityBuildingUpkeep = 0,
		totalCommerceScience = 0, totalCommerceSavings = 0,
		totalCommerceTerrain = 0, totalCommerceScieCrime = 0,
		// Science and Savings stuff.
		totalScienceFromBuildings = 0, totalGoldFromBuildings = 0,
		totalScienceFromCommerce = 0, totalGoldFromCommerce = 0,
		totalScientistsScie = 0, totalMerchantsGold = 0,
		totalGoldConversionLoss = 0, totalScienceGov = 0,
		totalGoldGov = 0, totalFeatWonderScie = 0,
		totalFeatWonderGold = 0, totalScieSubTotal = 0,
		totalGoldSubTotal = 0, totalUnitWages = 0,
		totalGoldTradeRoutes = 0, totalGoldConversions = 0;

	
	Player *player = g_player[g_selected_item->GetVisiblePlayer()];
	
	UnitDynamicArray *cityList = player->GetAllCitiesList();
	for(int cityIndex = 0; cityIndex < cityList->Num(); cityIndex++) {
		
		CityData *cityData = (*cityList)[cityIndex].GetData()->GetCityData();
		sint32 bestScientist = cityData->GetBestSpecialist(POP_SCIENTIST);
		sint32 bestMerchant = cityData->GetBestSpecialist(POP_MERCHANT);

		sint32 commerceTerrain = static_cast<sint32>(cityData->GetGrossCityGold());
		

		// Worker wages - deducted from commerce.
		sint32 commerceWages = cityData->CalcWages(
			static_cast<sint32>(player->GetWagesPerPerson()));


		// Building and city maintenance - deducted from commerce.
		sint32 commerceCityBuildingUpkeep = cityData->GetSupportBuildingsCost();
		commerceCityBuildingUpkeep += cityData->GetSupportCityCost();


		// Sub Total commerce, just before splitting to science and savings.
		sint32 commerce = commerceTerrain - (commerceWages + commerceCityBuildingUpkeep);
		
		// These are the terrain values for science and gold.
		double s;
		player->GetScienceTaxRate(s);
		sint32 scienceFromCommerce = static_cast<sint32>(ceil(commerce * s));
		sint32 goldFromCommerce = commerce - scienceFromCommerce;


		// Buildings (building bonus * terrain science or savings).
		sint32 commerceScience = scienceFromCommerce;// Terrain city science.
		double ss;
		buildingutil_GetSciencePercent(cityData->GetEffectiveBuildings(), ss, cityData->GetOwner());
		sint32 scienceBuildingsBonus = static_cast<sint32>(ceil(commerceScience * ss));

		scienceBuildingsBonus += static_cast<sint32>(buildingutil_GetIncreaseSciencePerPop(cityData->GetEffectiveBuildings(), cityData->GetOwner())
			* static_cast<double>(cityData->PopCount() - cityData->SlaveCount()));

		sint32 commerceSavings = cityData->GetNetCityGold();// final gold savings figure

		sint32 terrainSavings = goldFromCommerce;
		sint32 goldBuildingsBonus = 0;
		double goldBonus;
		buildingutil_GetCommercePercent(cityData->GetEffectiveBuildings(), goldBonus, player->GetOwner());
		goldBuildingsBonus += static_cast<sint32>(terrainSavings * goldBonus);


		// FEATS AND WONDERS.
		// - science
		sint32 buildingAndTaxScience = commerceScience + scienceBuildingsBonus;

		// science feats multiply terrain scie + buildings scie - see citydata::splitscience.
		sint32 featWonderScie = 0;
		sint32 featPercent = g_featTracker->GetAdditiveEffect(FEAT_EFFECT_INCREASE_SCIENCE, player->GetOwner());
		sint32 featScie = static_cast<sint32>(ceil(buildingAndTaxScience * (static_cast<double>(featPercent) / 100.0)));

		// science wonders multiply terrain scie + buildings scie + feats scie - see citydata::splitscience.
		buildingAndTaxScience += featScie;
		double ws = 0.01 * wonderutil_GetIncreaseKnowledgePercentage(player->GetBuiltWonders());
		sint32 wonderScie = static_cast<sint32>(ceil(buildingAndTaxScience * ws));

		featWonderScie = (featScie + wonderScie);


		// - gold savings
		sint32 featGold = 0;
		sint32 buildingAndTaxGold = goldFromCommerce + goldBuildingsBonus;

		// Gold savings feat multiplies terrain gold + buildings gold.
		featPercent = g_featTracker->GetAdditiveEffect(FEAT_EFFECT_INCREASE_COMMERCE, player->GetOwner());
		featGold = static_cast<sint32>(buildingAndTaxGold * featPercent / 100.0);

		// Gold wonders are done on an empire-wide basis.


		// NOTE: goldpercitizen buildings are not modified by feat bonus so have to be added after.
		sint32 goldPerCitizen = 
			buildingutil_GetGoldPerCitizen(cityData->GetEffectiveBuildings(), cityData->GetOwner());
		goldBuildingsBonus += goldPerCitizen * cityData->PopCount();


		// Specialists.
        sint32 scientistsScie = 0;
		if (cityData->ScientistCount() > 0)
		{
			double popWonderModifier = 
				static_cast<double>(wonderutil_GetIncreaseScientists(cityData->GetBuiltWonders()));
			popWonderModifier += 
				static_cast<double>(wonderutil_GetIncreaseSpecialists(player->GetBuiltWonders()));
			
			if(popWonderModifier && bestScientist >= 0 
				&& bestScientist < g_thePopDB->NumRecords())
			{
				scientistsScie += static_cast<sint32>(popWonderModifier * 
							 g_thePopDB->Get(bestScientist, player->GetGovernmentType())->GetScience());
			}

			scientistsScie += cityData->ScientistCount() *
				g_thePopDB->Get(bestScientist, player->GetGovernmentType())->GetScience();
		}

        sint32 merchantsGold = 0;
		if (cityData->MerchantCount() > 0)
		{
			merchantsGold = cityData->MerchantCount() *
				g_thePopDB->Get(bestMerchant, player->GetGovernmentType())->GetCommerce();
		}


		// Savings from trade routes.
		sint32 goldTradeRoutes = cityData->CalculateTradeRoutes(true);


		// Government modifiers to science and savings.
        sint32 totalScieWithoutGov = scienceFromCommerce + scienceBuildingsBonus +
			featWonderScie + scientistsScie;
        sint32 totalGoldWithoutGov = goldFromCommerce + goldBuildingsBonus +
			featGold + merchantsGold + goldTradeRoutes;

		sint32 scienceGov = static_cast<sint32>(ceil(totalScieWithoutGov 
			* g_theGovernmentDB->Get(player->m_government_type)->GetKnowledgeCoef()));
		scienceGov -= totalScieWithoutGov;

		sint32 goldGov = static_cast<sint32>(totalGoldWithoutGov 
			* g_theGovernmentDB->Get(player->m_government_type)->GetGoldCoef());
		goldGov -= totalGoldWithoutGov;


		// Gold lost to conversion.
		sint32 GoldConversionLoss = cityData->GetConvertedGold();


		// Science and savings lost to crime
		sint32 commerceScieCrime = cityData->GetScienceCrime();
		sint32 commerceGoldCrime = cityData->GetTradeCrime();


		////////////////////////////////////
		// TOTALS:
		////////////////////////////////////

		// Total commerce from terrain only.
		totalCommerceTerrain += commerceTerrain;

		// Total upkeep (buildings and cities).
		totalCommerceCityBuildingUpkeep += commerceCityBuildingUpkeep;
		// Total wages (citizens and units).
		totalCommerceWages += commerceWages;

		// Total terrain commerce minus wages and upkeep ("sub total").
		totalCommerce += commerce;

		// Totals for specialists.
		totalScientistsScie += scientistsScie;
		totalMerchantsGold += merchantsGold;

		// Totals for feats and wonder bonuses.
		totalFeatWonderScie += featWonderScie;
		totalFeatWonderGold += featGold;

		// Total gold from trade routes.
		totalGoldTradeRoutes += goldTradeRoutes;

		// Totals for government bonuses.
		totalScienceGov += scienceGov;
		totalGoldGov += goldGov;

		// Total gold lost to conversion.
		totalGoldConversionLoss += GoldConversionLoss;

		// Total crime deducted from science and savings.
		totalCommerceScieCrime += commerceScieCrime;
		totalCommerceGoldCrime += commerceGoldCrime;

        totalScienceFromCommerce += scienceFromCommerce;
		totalGoldFromCommerce += goldFromCommerce;
        totalScienceFromBuildings += scienceBuildingsBonus;
        totalGoldFromBuildings += goldBuildingsBonus;
		totalCommerceScience += cityData->GetScience(); //commerceScience;
		totalCommerceSavings += commerceSavings;
	}

	// Wonders - empire-wide only.
	sint32 totalWonderGold = player->CalcWonderGold();
	totalFeatWonderGold += totalWonderGold;

	// This is "commodity colonies" E added,
	// just adding it straight to savings for now
	// since there is no room in the empire manager,
	// and it's rarely used.-Maq
	totalCommerceSavings += player->CommodityMarket();

	// "Sub totals" for science and savings.
	totalScieSubTotal = totalCommerceScience + totalCommerceScieCrime;
	totalGoldSubTotal = totalCommerceSavings + totalCommerceGoldCrime + totalGoldConversionLoss
		+ totalGoldConversions;

	// Gold wonders ("bonusgold") are empire wide only,
	// and not subject to losses.
	totalGoldSubTotal += totalWonderGold;
	totalCommerceSavings += totalWonderGold;

	// Total unit gold support - deducted from savings.
	if (g_theProfileDB->IsGoldPerUnitSupport()) {
		totalUnitWages += player->CalcUnitSupportGold();
		// Recalculate savings after unit gold support.
		totalCommerceSavings -= totalUnitWages;
	}

	// Gold profit from conversions.
	sint32 j, k;
	for(j = 0; j < k_MAX_PLAYERS; j++) {
		if(!g_player[j] || g_player[j] == player)
			continue;
		for(k = g_player[j]->m_all_cities->Num() - 1; k >= 0; k--) {
			if (g_player[j]->m_all_cities->Access(k)->GetCityData()->IsConvertedTo() == 
				player->GetOwner())
				totalGoldConversions += 
					g_player[j]->m_all_cities->Access(k)->GetCityData()->GetConvertedGold();
		}
	}




	
	static char stringBuffer[16];
	static char formatBuffer[64];

	sprintf(stringBuffer, "%d", totalCommerceTerrain);
	m_detailsCommerceTerrain->SetText(stringBuffer);

	sprintf(stringBuffer, "%d", totalCommerceWages);
	m_detailsCommerceWages->SetText(stringBuffer);

	sprintf(stringBuffer, "%d", totalCommerceCityBuildingUpkeep);
	m_detailsCommerceBuildingUpkeep->SetText(stringBuffer);

	sprintf(stringBuffer, "%d", totalCommerce);
	m_detailsCommerceTotal->SetText(stringBuffer);
	m_summaryCommerceTotal->SetText(stringBuffer);

	sprintf(stringBuffer, "%d", totalScienceFromCommerce);
	m_detailsCommerceScienceBasic->SetText(stringBuffer);

	sprintf(stringBuffer, "%d", totalGoldFromCommerce);
	m_detailsGoldFromCommerce->SetText(stringBuffer);

	sprintf(stringBuffer, "%d", totalCommerceScience);
	m_detailsCommerceScience->SetText(stringBuffer);
	m_summaryCommerceScience->SetText(stringBuffer);

	sprintf(stringBuffer, "%d", totalScienceFromBuildings);
	m_detailsCommerceScienceBuildings->SetText(stringBuffer);

	sprintf(stringBuffer, "%d", totalGoldFromBuildings);
	m_detailsCommerceGoldBuildings->SetText(stringBuffer);

	sprintf(stringBuffer, "%d", totalScientistsScie);
	m_detailsCommerceScientistsScie->SetText(stringBuffer);

	sprintf(stringBuffer, "%d", totalFeatWonderScie);
	m_detailsCommerceFeatWonderScie->SetText(stringBuffer);

	sprintf(stringBuffer, "%d", totalMerchantsGold);
	m_detailsCommerceMerchantsGold->SetText(stringBuffer);

	sprintf(stringBuffer, "%d", totalFeatWonderGold);
	m_detailsCommerceFeatWonderGold->SetText(stringBuffer);

	sprintf(stringBuffer, "%d", totalGoldTradeRoutes);
	m_detailsCommerceTradeGold->SetText(stringBuffer);

	sprintf(stringBuffer, "%d", totalScienceGov);
	m_detailsCommerceScienceGov->SetText(stringBuffer);

	sprintf(stringBuffer, "%d", totalGoldGov);
	m_detailsCommerceGoldGov->SetText(stringBuffer);

	sprintf(stringBuffer, "%d", totalScieSubTotal);
	m_detailsCommerceScieSubTotal->SetText(stringBuffer);
	m_summaryCommerceScieSubTotal->SetText(stringBuffer);

	sprintf(stringBuffer, "%d", totalGoldSubTotal);
	m_detailsCommerceGoldSubTotal->SetText(stringBuffer);
	m_summaryCommerceGoldSubTotal->SetText(stringBuffer);

	sprintf(stringBuffer, "%d", totalGoldConversionLoss);
	m_detailsCommerceConvGoldLoss->SetText(stringBuffer);
	m_summaryCommerceConvGoldLoss->SetText(stringBuffer);

	sprintf(stringBuffer, "%d", totalUnitWages);
	m_detailsCommerceUnitWages->SetText(stringBuffer);
	m_summaryCommerceUnitWages->SetText(stringBuffer);

	sprintf(stringBuffer, "%d", totalCommerceGoldCrime);
	m_detailsCommerceGoldCrime->SetText(stringBuffer);
	m_summaryCommerceGoldCrime->SetText(stringBuffer);

	sprintf(stringBuffer, "%d", totalCommerceScieCrime);
	m_detailsCommerceScieCrime->SetText(stringBuffer);
	m_summaryCommerceScieCrime->SetText(stringBuffer);

	sprintf(stringBuffer, "%d", totalCommerceSavings);
	m_detailsCommerceSavings->SetText(stringBuffer);
	m_summaryCommerceSavings->SetText(stringBuffer);

	sprintf(stringBuffer, "%d", totalGoldConversions);
	m_detailsCommerceConversionsGold->SetText(stringBuffer);
	m_summaryCommerceConversionsGold->SetText(stringBuffer);

	
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
	for(sint32 i = 0; i < cityList->Num(); i++)
	{
		CityData *cityData = (*cityList)[i].GetData()->GetCityData();

		cityData->ProcessAllResources();
		cityData->UpdateSprite();
	}
}


AUI_ERRCODE CauseAndEffectTab::HappinessBarActionCallback(ctp2_Static *control,
                                                          aui_Surface *surface, 
                                                          RECT &rect, 
                                                          void *cookie)
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
                                                     uint32 action, 
                                                     uint32 data,
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

	// Happiness is modified by this spinner and happiness modfies crime
	// and crime modifies food, production and commerce, therefore all three
	// need to be updated.
	tab->UpdateFoodValues();
	tab->UpdateProductionValues();
	tab->UpdateCommerceValues();
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

	
	// Happiness is modified by this spinner and happiness modfies crime
	// and crime modifies food, production and commerce, therefore all three
	// need to be updated.
	tab->UpdateFoodValues();
	tab->UpdateProductionValues();
	tab->UpdateCommerceValues();
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

	
	// Happiness is modified by this spinner and happiness modfies crime
	// and crime modifies food, production and commerce, therefore all three
	// need to be updated.
	tab->UpdateFoodValues();
	tab->UpdateProductionValues();
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

//----------------------------------------------------------------------------
//
// Name       : CauseAndEffectTab::OptimizeSlidersButtonActionCallback
//
// Description: Executes action when the optimize sliders button is pressed.
//
// Parameters : control:         -
//              action:          -
//              data:            -
//              cookie:          -
//
// Globals    : g_selected_item: The currently selected item.
//
// Returns    : -
//
// Remark(s)  : Info about parameters has to be added.
//
//----------------------------------------------------------------------------
void CauseAndEffectTab::OptimizeSlidersButtonActionCallback(aui_Control *control,
	uint32 action, uint32 data, void *cookie)
{
	
	if(action != static_cast<uint32>(AUI_BUTTON_ACTION_EXECUTE))
		return;

	Governor::SlidersSetting sliders_setting;
	PLAYER_INDEX playerId = g_selected_item->GetVisiblePlayer();
	Governor & governor = Governor::GetGovernor(playerId);

	time_t t1 = GetTickCount();

	DPRINTF(k_DBG_AI, ("\n"));
	DPRINTF(k_DBG_AI, ("// ADJUST SLIDER SETTINGS -- Turn %d\n", g_player[playerId]->GetCurRound()));
	DPRINTF(k_DBG_AI, ("//						     Player %d\n", playerId));

	governor.OptimizeSliders(sliders_setting);
	governor.SetSliders(sliders_setting, true);

	DPRINTF(k_DBG_AI, ("//  elapsed time = %d ms\n", (GetTickCount() - t1)));
	DPRINTF(k_DBG_AI, ("\n"));

	UpdateCities();

	// Update tab
	CauseAndEffectTab *tab = static_cast<CauseAndEffectTab*>(cookie);
	tab->UpdateFoodValues();
	tab->UpdateFoodSpinners();
	tab->UpdateProductionValues();
	tab->UpdateProductionSpinners();
	tab->UpdateCommerceValues();
	tab->UpdateCommerceSpinners();
	tab->UpdateGeneral();
	tab->m_tabPanel->ShouldDraw(TRUE);

}


void CauseAndEffectTab::DetailsShowCallback(aui_Region *region,
                                            void *userData)
{
	
	CauseAndEffectTab *dialog = static_cast<CauseAndEffectTab*>(userData);

	
	dialog->DisplayDetails(dialog->m_displayDetails);
}

void CauseAndEffectTab::CauseAndEffectTabActionCallback(aui_Control *control,
                                                        uint32 action, 
                                                        uint32 data, 
                                                        void *cookie)
{
	CauseAndEffectTab * tab = reinterpret_cast<CauseAndEffectTab *>(cookie);

	if (action == ctp2_Tab::ACTION_ACTIVATED)
	{
		tab->m_detailsButton->Show();
		if (tab->m_optimizeSliderButton)
		{
			tab->m_optimizeSliderButton->Show();
		}
	}
	else if (action == ctp2_Tab::ACTION_DEACTIVATED)
	{
		tab->m_detailsButton->Hide();
		if (tab->m_optimizeSliderButton)
		{
			tab->m_optimizeSliderButton->Hide();
		}
	}
}
