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

#ifdef HAVE_PRAGMA_ONCE
#pragma once
#endif

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

	void Blank();

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
