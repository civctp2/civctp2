







#pragma once
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


class CauseAndEffectTab {
public:
	
	CauseAndEffectTab(MBCHAR *ldlBlock);

	
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
		aui_Surface *surface, RECT &rect, void *cookie);

	
	static void RationsSpinnerActionCallback(aui_Control *control,
		uint32 action, uint32 data, void *cookie);
	static void WorkdaySpinnerActionCallback(aui_Control *control,
		uint32 action, uint32 data, void *cookie);
	static void PublicWorksSpinnerActionCallback(aui_Control *control,
		uint32 action, uint32 data, void *cookie);
	static void WagesSpinnerActionCallback(aui_Control *control,
		uint32 action, uint32 data, void *cookie);
	static void ScienceTaxSpinnerActionCallback(aui_Control *control,
		uint32 action, uint32 data, void *cookie);

	
	static void DetailsButtonActionCallback(aui_Control *control,
		uint32 action, uint32 data, void *cookie);

	static void CauseAndEffectTabActionCallback(aui_Control *control,
		uint32 action, uint32 data, void *cookie);

	
	
	static void DetailsShowCallback(aui_Region *region,
		void *userData);

	
	ctp2_Static		*m_tabPanel;
	ctp2_Tab		*m_tab;

	
	bool			m_displayDetails;

	
	ctp2_Button		*m_detailsButton;

	
	ctp2_Static		*m_numberOfCities;
	ctp2_Static		*m_population;
	ctp2_Static		*m_happinessValue;
	ctp2_Static		*m_happinessBar;
	ctp2_Static		*m_government;

	
	C3Slider		*m_foodRationsSpinner;
	ctp2_Static		*m_foodRequired;
	ctp2_Static		*m_foodHappinessValue;
	ctp2_Static		*m_foodCurValue;
	ctp2_Static		*m_foodHappinessIcon;
	
	ctp2_Static		*m_detailsFoodTotal;
	ctp2_Static		*m_detailsFoodCrime;
	ctp2_Static		*m_detailsFoodConsumed;
	ctp2_Static		*m_detailsFoodSubTotal;
	ctp2_Static		*m_detailsFoodStored;
	
	ctp2_Static		*m_summaryFoodTotal;
	ctp2_Static		*m_summaryFoodConsumed;
	ctp2_Static		*m_summaryFoodStored;


	
	C3Slider		*m_productionWorkdaySpinner;
	ctp2_Spinner	*m_productionPublicWorksSpinner;
	ctp2_Static		*m_productionHappinessValue;
	ctp2_Static		*m_productionCurValue;
	ctp2_Static		*m_productionHappinessIcon;
	
	ctp2_Static		*m_detailsProductionTotal;
	ctp2_Static		*m_detailsProductionCrime;
	ctp2_Static		*m_detailsProductionUnitUpkeep;
	ctp2_Static		*m_detailsProductionSubTotal;
	ctp2_Static		*m_detailsProductionPublicWorks;
	ctp2_Static		*m_detailsProductionCityUse;
	
	ctp2_Static		*m_summaryProductionTotal;
	ctp2_Static		*m_summaryProductionPublicWorks;
	ctp2_Static		*m_summaryProductionCityUse;

	
	C3Slider		*m_commerceWagesSpinner;
	ctp2_Spinner	*m_commerceScienceTaxSpinner;
	ctp2_Static		*m_commerceHappinessValue;
	ctp2_Static		*m_commerceCurValue;
	ctp2_Static		*m_commerceHappinessIcon;
	
	ctp2_Static		*m_detailsCommerceTotal;
	ctp2_Static		*m_detailsCommerceCrime;
	ctp2_Static		*m_detailsCommerceWages;
	ctp2_Static		*m_detailsCommerceBuildingUpkeep;
	ctp2_Static		*m_detailsCommerceSubTotal;
	ctp2_Static		*m_detailsCommerceScience;
	ctp2_Static		*m_detailsCommerceSavings;
	
	ctp2_Static		*m_summaryCommerceTotal;
	ctp2_Static		*m_summaryCommerceScience;
	ctp2_Static		*m_summaryCommerceSavings;

	MBCHAR			m_ldlBlock[100];
};

#endif 
