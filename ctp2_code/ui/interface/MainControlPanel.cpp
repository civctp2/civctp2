//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : Collection of control panels during actual play.
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
// - Blank function added to hide the data of the previous player for hotseat
//   games.  
//
//----------------------------------------------------------------------------

#include "c3.h"


#include "MainControlPanel.h"


#include "CityControlPanel.h"
#include "ControlTabPanel.h"
#include "DomesticControlPanel.h"
#include "EndTurnButton.h"
#include "Events.h"
#include "GameEventUser.h"
#include "MessageControlPanel.h"
#include "ShortcutPad.h"
#include "StatusBar.h"
#include "TilesControlPanel.h"
#include "TurnYearStatus.h"
#include "UnitControlPanel.h"
#include "ZoomPad.h"

#include "aui_uniqueid.h"
#include "aui_progressbar.h"


MainControlPanel *g_mainControlPanel = NULL;



static aui_ProgressBar	*s_progressBar;




































STDEHANDLER(MainControlPanel_BeginTurn)
{
	
	PLAYER_INDEX player = 0;
	if(!args->GetPlayer(0, player))
		return(GEV_HD_Continue);

	
	MainControlPanel::UpdatePlayer(player);

	
	return(GEV_HD_Continue);
}


void MainControlPanel::InitializeEvents()
{
	
	g_gevManager->AddCallback(GEV_BeginTurn, GEV_PRI_Post, &s_MainControlPanel_BeginTurn);
}


void MainControlPanel::Initialize(MBCHAR *ldlBlock)
{
	
	if(g_mainControlPanel)
		return;

	
	g_mainControlPanel = new MainControlPanel(ldlBlock);
}

#if !defined(ACTIVISION_ORIGINAL)
//----------------------------------------------------------------------------
//
// Name       : MainControlPanel::Blank
//
// Description: Blank out the data of the previous player in between turns for
//              hotseat play.
//
// Parameters : -
//
// Globals    : g_mainControlPanel	: control panel to blank
//
// Returns    : -
//
// Remark(s)  : -
//
//----------------------------------------------------------------------------
void MainControlPanel::Blank()
{
	if (g_mainControlPanel)
	{
		g_mainControlPanel->m_controlTabPanel->Blank();
	}
}
#endif

void MainControlPanel::CleanUp()
{
	
	delete g_mainControlPanel;
	g_mainControlPanel = NULL;
}


void MainControlPanel::Update()
{
	
	if(g_mainControlPanel) {
		g_mainControlPanel->m_controlTabPanel->Update();
	}
}


void MainControlPanel::UpdateCityList()
{
	
	if(g_mainControlPanel) {
		g_mainControlPanel->m_controlTabPanel->UpdateCityList();
	}
}


void MainControlPanel::UpdatePlayer(PLAYER_INDEX player)
{
	
	if(g_mainControlPanel) {
		
		g_mainControlPanel->m_endTurnButton->UpdatePlayer(player);
		g_mainControlPanel->m_turnYearStatus->UpdatePlayer(player);
	}
}
	

void MainControlPanel::UpdateZoom()
{






}


void MainControlPanel::SelectedCity()
{
	if(g_mainControlPanel) {
		g_mainControlPanel->m_controlTabPanel->SelectedCity();
	}
}


void MainControlPanel::SelectedUnit()
{
	if(g_mainControlPanel) {
		g_mainControlPanel->m_controlTabPanel->SelectedUnit();
	}
}

void MainControlPanel::UnitPanelActivated()
{
	if(g_mainControlPanel) {
		g_mainControlPanel->m_controlTabPanel->UnitPanelActivated();
	}
}

void MainControlPanel::CityPanelActivated()
{
	if(g_mainControlPanel) {
		g_mainControlPanel->m_controlTabPanel->CityPanelActivated();
	}
}

aui_ProgressBar* MainControlPanel::GetProgressBar()
{
	return s_progressBar;
}


MainControlPanel::MainControlPanel(MBCHAR *ldlBlock) :
m_controlTabPanel(new ControlTabPanel(ldlBlock)),
m_endTurnButton(new EndTurnButton(ldlBlock)),
m_shortcutPad(new ShortcutPad(ldlBlock)),
m_statusBar(new StatusBar(ldlBlock)),
m_turnYearStatus(new TurnYearStatus(ldlBlock))

{	







}

MainControlPanel::~MainControlPanel()
{








}

bool MainControlPanel::GetSelectedCargo(CellUnitList &cargo)
{
	return g_mainControlPanel->m_controlTabPanel->GetSelectedCargo(cargo);
}

void MainControlPanel::SwitchToTransportView()
{
	if(g_mainControlPanel) {
		g_mainControlPanel->m_controlTabPanel->SwitchToTransportView();
	}
}
