






#pragma once
#ifndef ___BMH_DOMESTIC_CONTROL_PANEL_HEADER
#define ___BMH_DOMESTIC_CONTROL_PANEL_HEADER


class aui_Control;
class ctp2_Button;
class ctp2_Static;


class DomesticControlPanel {
public:
	
	DomesticControlPanel(MBCHAR *ldlBlock);

	
	void Update();

private:
	
	static void EditResearchButtonActionCallback(aui_Control *control,
		uint32 action, uint32 data, void *cookie);

	
	void UpdateScience();

	
	void ClearScience();

	
	void UpdateStats();

	
	void UpdateGoldPW();

	
	ctp2_Static *m_scienceLabel;		
	ctp2_Button *m_scienceIconButton;	
	ctp2_Button *m_scienceTurnButton;	
	ctp2_Static *m_citiesValue;			
	ctp2_Static *m_populationValue;		
	ctp2_Static *m_publicWorksValue;	
	ctp2_Static *m_governmentValue;		
	ctp2_Static *m_scienceValue;		
	ctp2_Static *m_pollutionValue;		
	
	ctp2_Static *m_menuGoldValue;       
	ctp2_Static *m_menuPublicWorksValue; 
	ctp2_Static *m_menuHappinessValue;  
	ctp2_Static *m_menuPollutionValue;  

	sint32 m_currentResearch;            
	sint32 m_currentTurns;               
	sint32 m_currentGold;
	sint32 m_currentCities;              
	sint32 m_currentPopulation;
	sint32 m_currentPW;
	sint32 m_currentGovernment;
	sint32 m_currentScience;
	sint32 m_currentPollution;

	MBCHAR m_ldlBlock[100];
};

#endif 
