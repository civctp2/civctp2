//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ header
// Description  : 
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
// _MSC_VER		
// - When defined, allows Microsoft C++ extensions.
// - When not defined, generates standard C++.
//
// Note: For the blocks with _MSC_VER preprocessor directives, the following
//       is implied: the (_MSC_VER) preprocessor directive lines and the blocks 
//       between #else and #endif are modified Apolyton code. The blocks 
//       between #if and #else are the original Activision code.
//
//----------------------------------------------------------------------------
//
// Modifications from the original Activision code:
//
// - Microsoft C++ extensions marked for future GCC compilation.
// - Functionality added to hide the data of the previous player for hotseat
//   games.  
//
//----------------------------------------------------------------------------

#if defined(_MSC_VER)
#pragma once
#else
#endif

#ifndef ___BMH_DOMESTIC_CONTROL_PANEL_HEADER
#define ___BMH_DOMESTIC_CONTROL_PANEL_HEADER


class aui_Control;
class ctp2_Button;
class ctp2_Static;


class DomesticControlPanel {
public:
	
	DomesticControlPanel(MBCHAR *ldlBlock);

#if !defined(ACTIVISION_ORIGINAL)
	void Blank();
#endif
	
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
