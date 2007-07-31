






#include "c3.h"


#include "ControlTabPanel.h"


#include "CityControlPanel.h"
#include "DomesticControlPanel.h"
#include "MessageControlPanel.h"
#include "TilesControlPanel.h"
#include "UnitControlPanel.h"


static MBCHAR *AppendBlockName(MBCHAR *ldlBlock, MBCHAR *subBlock)
{
	
	MBCHAR *newBlock = new char[strlen(ldlBlock) + strlen(subBlock) + 2];

	
	sprintf(newBlock, "%s.%s", ldlBlock, subBlock);

	
	return(newBlock);
}


ControlTabPanel::ControlTabPanel(MBCHAR *ldlBlock) :
m_ldlBlock(AppendBlockName(ldlBlock, "ControlTabPanel")),
m_domesticControlPanel(new DomesticControlPanel(m_ldlBlock.get())),
m_cityControlPanel(new CityControlPanel(m_ldlBlock.get())),
m_unitControlPanel(new UnitControlPanel(m_ldlBlock.get())),
m_messageControlPanel(new MessageControlPanel(m_ldlBlock.get())),
m_tilesControlPanel(new TilesControlPanel(m_ldlBlock.get()))
{
}


void ControlTabPanel::Update()
{
	m_domesticControlPanel->Update();
	m_cityControlPanel->Update();
	m_unitControlPanel->Update();
}


void ControlTabPanel::UpdateCityList()
{
	m_cityControlPanel->UpdateCityList();
}


void ControlTabPanel::SelectedCity()
{
	m_cityControlPanel->SelectedCity();
}


void ControlTabPanel::SelectedUnit()
{
	m_unitControlPanel->SelectedUnit();
}

void ControlTabPanel::UnitPanelActivated()
{
	m_unitControlPanel->Activated();
}

void ControlTabPanel::CityPanelActivated()
{
	m_cityControlPanel->Activated();
}

bool ControlTabPanel::GetSelectedCargo(CellUnitList &cargo)
{
	return m_unitControlPanel->GetSelectedCargo(cargo);
}

void ControlTabPanel::SwitchToTransportView()
{
	m_unitControlPanel->SetSelectionMode(UnitControlPanel::TRANSPORT_SELECTION);
}
