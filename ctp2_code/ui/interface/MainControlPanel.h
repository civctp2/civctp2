






#pragma once
#ifndef ___BMH_MAIN_CONTROL_PANEL_HEADER
#define ___BMH_MAIN_CONTROL_PANEL_HEADER


#include <memory>


#include "globals.h"


class ControlTabPanel;
class EndTurnButton;
class ShortcutPad;
class StatusBar;
class TurnYearStatus;

class aui_ProgressBar;
class CellUnitList;


class MainControlPanel {
public:
	
	static void Initialize(MBCHAR *ldlBlock);

	
	static void InitializeEvents();

	
	static void CleanUp();

	
	static void Update();

	
	static void UpdateCityList();

	
	static void UpdatePlayer(PLAYER_INDEX player);

	
	static void UpdateZoom();

	
	static void SelectedCity();

	
	static void SelectedUnit();

	static void UnitPanelActivated();
	static void CityPanelActivated();

	static bool GetSelectedCargo(CellUnitList &cargo);

	static void SwitchToTransportView();

	static aui_ProgressBar* GetProgressBar();
	
	
	MainControlPanel(MBCHAR *ldlBlock);

	
	MainControlPanel::~MainControlPanel();

private:
	
	std::auto_ptr<ControlTabPanel>	m_controlTabPanel;	
	std::auto_ptr<EndTurnButton>	m_endTurnButton;	
	std::auto_ptr<ShortcutPad>		m_shortcutPad;		
	std::auto_ptr<StatusBar>		m_statusBar;		
	std::auto_ptr<TurnYearStatus>	m_turnYearStatus;	

};


extern MainControlPanel *g_mainControlPanel;

#endif 
