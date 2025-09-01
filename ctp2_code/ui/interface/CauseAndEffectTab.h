//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ header
// Description  : Cause and effect tab of the domestic manager
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
// - Added m_optimizeSliderButton button for automatic slider optimization
//   option. Including a callback function. - April 7th 2005 Martin G�hmann
// - Added more information to empire manager domestic tab. (22-Jul-2009 Maq)
//
//----------------------------------------------------------------------------
#ifdef HAVE_PRAGMA_ONCE
#pragma once
#endif
#ifndef ___BMH_CAUSE_AND_EFFECT_TAB_HEADER
#define ___BMH_CAUSE_AND_EFFECT_TAB_HEADER

class aui_Control;
class aui_Region;
class aui_Surface;
class C3Slider;
class ctp2_Button;
class ctp2_Spinner;
class ctp2_Static;
class ctp2_Tab;
union Cookie;

class CauseAndEffectTab {
public:
	CauseAndEffectTab(const MBCHAR *ldlBlock);

	void Update();

	void DisplayDetails(bool flag = true);

private:
	void UpdateGeneral();

	void UpdateFoodSpinners();
	void UpdateProductionSpinners();
	void UpdateCommerceSpinners();

	void UpdateFoodValues();
	void UpdateProductionValues();
	void UpdateCommerceValues();


	void SetHappinessIcon(ctp2_Static *control, sint32 happiness);


	static void UpdateCities();

	static AUI_ERRCODE HappinessBarActionCallback(ctp2_Static *control,
		aui_Surface *surface, RECT &rect, Cookie cookie);

	static void RationsSpinnerActionCallback(aui_Control *control,
		uint32 action, uint32 data, Cookie cookie);
	static void WorkdaySpinnerActionCallback(aui_Control *control,
		uint32 action, uint32 data, Cookie cookie);
	static void PublicWorksSpinnerActionCallback(aui_Control *control,
		uint32 action, uint32 data, Cookie cookie);
	static void WagesSpinnerActionCallback(aui_Control *control,
		uint32 action, uint32 data, Cookie cookie);
	static void ScienceTaxSpinnerActionCallback(aui_Control *control,
		uint32 action, uint32 data, Cookie cookie);

	static void DetailsButtonActionCallback(aui_Control *control,
		uint32 action, uint32 data, Cookie cookie);

	static void OptimizeSlidersButtonActionCallback(aui_Control *control,
		uint32 action, uint32 data, Cookie cookie);// Added by Martin G�hmann

	static void CauseAndEffectTabActionCallback(aui_Control *control,
		uint32 action, uint32 data, Cookie cookie);


	static void DetailsShowCallback(aui_Region *region,
		void *userData);

	ctp2_Static		*m_tabPanel;
	ctp2_Tab		*m_tab;

	bool			m_displayDetails;

	ctp2_Button		*m_detailsButton;
	ctp2_Button		*m_optimizeSliderButton; // Added by Martin G�hmann

	ctp2_Static		*m_numberOfCities;
	ctp2_Static		*m_population;
	ctp2_Static		*m_happinessValue;
	ctp2_Static		*m_happinessBar;
	ctp2_Static		*m_government;

	C3Slider		*m_foodRationsSpinner;
	//ctp2_Static		*m_foodRequired;
	ctp2_Static		*m_foodHappinessValue;
	ctp2_Static		*m_foodCurValue;
	ctp2_Static		*m_foodHappinessIcon;

	ctp2_Static		*m_detailsFoodTotal;
	ctp2_Static		*m_detailsFoodTerrainTotal;
	ctp2_Static		*m_detailsFoodFromBuildings;
	ctp2_Static		*m_detailsFoodFarmers;
	ctp2_Static		*m_detailsFoodFeatWonder;
	ctp2_Static		*m_detailsFoodGovBonus;
	ctp2_Static		*m_detailsFoodCrime;
	ctp2_Static		*m_detailsFoodConsumed;
	ctp2_Static		*m_detailsFoodSubTotal;
	ctp2_Static		*m_detailsFoodStored;

	ctp2_Static		*m_summaryFoodTotal;
	ctp2_Static		*m_summaryFoodConsumed;
	ctp2_Static		*m_summaryFoodCrime;
	ctp2_Static		*m_summaryFoodStored;


	C3Slider		*m_productionWorkdaySpinner;
	ctp2_Spinner	*m_productionPublicWorksSpinner;
	ctp2_Static		*m_productionHappinessValue;
	ctp2_Static		*m_productionCurValue;
	ctp2_Static		*m_productionHappinessIcon;

	ctp2_Static		*m_detailsProductionTotal;
	ctp2_Static		*m_detailsProdTerrainTotal;
	ctp2_Static		*m_detailsProdWorkdayTotal;
	ctp2_Static		*m_detailsProdFromBuildingsTotal;
	ctp2_Static		*m_detailsProductionLaborersTotal;
	ctp2_Static		*m_detailsProductionFeatWonderTotal;
	ctp2_Static		*m_detailsProductionGovBonusTotal;
	ctp2_Static		*m_detailsProductionFranchiseBioLoss;
	ctp2_Static		*m_detailsProductionCrime;
	ctp2_Static		*m_detailsProductionUnitUpkeep;
	ctp2_Static		*m_detailsProductionSubTotal;
	ctp2_Static		*m_detailsProductionPublicWorks;
	ctp2_Static		*m_detailsProductionCityUse;

	ctp2_Static		*m_summaryProductionTotal;
	ctp2_Static		*m_summaryProductionFranchiseBioLoss;
	ctp2_Static		*m_summaryProductionUnitUpkeep;
	ctp2_Static		*m_summaryProductionCrime;
	ctp2_Static		*m_summaryProductionPublicWorks;
	ctp2_Static		*m_summaryProductionCityUse;

	C3Slider		*m_commerceWagesSpinner;
	ctp2_Spinner	*m_commerceScienceTaxSpinner;
	ctp2_Static		*m_commerceHappinessValue;
	ctp2_Static		*m_commerceCurValue;
	ctp2_Static		*m_commerceHappinessIcon;

	ctp2_Static		*m_detailsCommerceTerrain;
	ctp2_Static		*m_detailsCommerceWages;
	ctp2_Static		*m_detailsCommerceBuildingUpkeep;
	ctp2_Static		*m_detailsCommerceTotal;
	ctp2_Static		*m_detailsCommerceScieCrime;
	ctp2_Static		*m_detailsCommerceGoldCrime;
	ctp2_Static		*m_detailsCommerceScienceBasic;
	ctp2_Static		*m_detailsGoldFromCommerce;
	ctp2_Static		*m_detailsCommerceScienceBuildings;
	ctp2_Static		*m_detailsCommerceGoldBuildings;
	ctp2_Static		*m_detailsCommerceScientistsScie;
	ctp2_Static		*m_detailsCommerceFeatWonderScie;
	ctp2_Static		*m_detailsCommerceMerchantsGold;
	ctp2_Static		*m_detailsCommerceFeatWonderGold;
	ctp2_Static		*m_detailsCommerceTradeGold;
	ctp2_Static		*m_detailsCommerceConversionsGold;
	ctp2_Static		*m_detailsCommerceScienceGov;
	ctp2_Static		*m_detailsCommerceGoldGov;
	ctp2_Static		*m_detailsCommerceScieSubTotal;
	ctp2_Static		*m_detailsCommerceGoldSubTotal;
	ctp2_Static		*m_detailsCommerceConvGoldLoss;
	ctp2_Static		*m_detailsCommerceUnitWages;
	ctp2_Static		*m_detailsCommerceScience;
	ctp2_Static		*m_detailsCommerceSavings;

	ctp2_Static		*m_summaryCommerceTotal;
	ctp2_Static		*m_summaryCommerceScieSubTotal;
	ctp2_Static		*m_summaryCommerceGoldSubTotal;
	ctp2_Static		*m_summaryCommerceConvGoldLoss;
	ctp2_Static		*m_summaryCommerceUnitWages;
	ctp2_Static		*m_summaryCommerceScieCrime;
	ctp2_Static		*m_summaryCommerceGoldCrime;
	ctp2_Static		*m_summaryCommerceScience;
	ctp2_Static		*m_summaryCommerceSavings;
	ctp2_Static		*m_summaryCommerceConversionsGold;

	MBCHAR			m_ldlBlock[100];
};

#endif
