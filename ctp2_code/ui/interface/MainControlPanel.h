//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ header
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
// - Blank function added to hide the data of the previous player for hotseat
//   games.  
//
//----------------------------------------------------------------------------

#if defined(_MSC_VER)
#pragma once
#else
#endif

#ifndef ___BMH_MAIN_CONTROL_PANEL_HEADER
#define ___BMH_MAIN_CONTROL_PANEL_HEADER



#if defined(ACTIVISION_ORIGINAL)
#include <memory>

#include "globals.h"


class ControlTabPanel;
class EndTurnButton;
class ShortcutPad;
class StatusBar;
class TurnYearStatus;

class aui_ProgressBar;
class CellUnitList;
#else
//----------------------------------------------------------------------------
// Library imports
//----------------------------------------------------------------------------

#include <memory>				// std::auto_ptr

//----------------------------------------------------------------------------
// Exported names
//----------------------------------------------------------------------------

class MainControlPanel;

//----------------------------------------------------------------------------
// Project imports
//----------------------------------------------------------------------------

#include "aui_ProgressBar.h"	// aui_ProgressBar
#include "c3types.h"			// MBCHAR
#include "CellUnitList.h"		// CellUnitList
#include "ControlTabPanel.h"	// ControlTabPanel
#include "EndTurnButton.h"		// EndTurnButton
#include "Player.h"				// PLAYER_INDEX
#include "ShortCutPad.h"		// ShortCutPad
#include "StatusBar.h"			// StatusBar
#include "TurnYearStatus.h"		// TurnYearStatus

//----------------------------------------------------------------------------
// Class declarations
//----------------------------------------------------------------------------
#endif

class MainControlPanel {
public:
	
	static void Initialize(MBCHAR *ldlBlock);

	
	static void InitializeEvents();

#if !defined(ACTIVISION_ORIGINAL)
	static void Blank();
#endif

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

	
#if defined(_MSC_VER)
	MainControlPanel::~MainControlPanel();
#else
	virtual ~MainControlPanel();
#endif

private:
	
	std::auto_ptr<ControlTabPanel>	m_controlTabPanel;	
	std::auto_ptr<EndTurnButton>	m_endTurnButton;	
	std::auto_ptr<ShortcutPad>		m_shortcutPad;		
	std::auto_ptr<StatusBar>		m_statusBar;		
	std::auto_ptr<TurnYearStatus>	m_turnYearStatus;	

};


extern MainControlPanel *g_mainControlPanel;

#endif 
