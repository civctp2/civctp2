






#pragma once
#ifndef ___BMH_CONTROL_PANEL_TAB_HEADER
#define ___BMH_CONTROL_PANEL_TAB_HEADER


#include <memory>


class CityControlPanel;
class DomesticControlPanel;
class MessageControlPanel;
class TilesControlPanel;
class UnitControlPanel;
class CellUnitList;


class ControlTabPanel {
public:
	
	ControlTabPanel(MBCHAR *ldlBlock);

	
	void Update();

	
	void UpdateCityList();

	
	void SelectedCity();

	
	void SelectedUnit();

	void UnitPanelActivated();
	void CityPanelActivated();

	bool GetSelectedCargo(CellUnitList &cargo);
	void SwitchToTransportView();

private:
	
	std::auto_ptr<MBCHAR> m_ldlBlock;

	
	std::auto_ptr<DomesticControlPanel>	m_domesticControlPanel;	
	std::auto_ptr<CityControlPanel>		m_cityControlPanel;		
	std::auto_ptr<UnitControlPanel>		m_unitControlPanel;		
	std::auto_ptr<MessageControlPanel>	m_messageControlPanel;	
	std::auto_ptr<TilesControlPanel>	m_tilesControlPanel;	
};

#endif 
