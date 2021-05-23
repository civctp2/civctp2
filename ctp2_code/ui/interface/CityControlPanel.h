//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ header
// File name    : \UI\Interface\CityControlPanel.h
// Description  : Handling for the city tab of the control panel
// Id           : $Id$
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
// - None
//
//----------------------------------------------------------------------------

#ifdef HAVE_PRAGMA_ONCE
#pragma once
#endif
#ifndef ___BMH_CITY_CONTROL_PANEL_HEADER
#define ___BMH_CITY_CONTROL_PANEL_HEADER

class aui_Control;
struct BuildNode;
class ctp2_Button;
class ctp2_Static;
class ctp2_DropDown;
class CityData;
class aui_Surface;

#include "Unit.h"

class CityControlPanel {
public:

	CityControlPanel(MBCHAR *ldlBlock);

	void Update();

	void UpdateCityList();

	void SelectedCity();

	static const MBCHAR *GetBuildName(const BuildNode *buildNode);

	static StringId GetBuildStringId(const BuildNode *buildNode);

	static const MBCHAR *GetBuildIcon(const BuildNode *buildNode);

	void Activated();

private:

	static void PrevCityButtonActionCallback(aui_Control *control,
		uint32 action, uint32 data, void *cookie);

	static void NextCityButtonActionCallback(aui_Control *control,
		uint32 action, uint32 data, void *cookie);

	static void EditBuildQueueButtonActionCallback(aui_Control *control,
		uint32 action, uint32 data, void *cookie);

	static void RushBuyBuildButtonActionCallback(aui_Control *control,
		uint32 action, uint32 data, void *cookie);

	static void ToggleGovernorButtonActionCallback(aui_Control *control,
		uint32 action, uint32 data, void *cookie);

	static void SelectGovernorActionCallback(aui_Control *control,
		uint32 action, uint32 data, void *cookie);

	static void CitySelectActionCallback(aui_Control *control,
		uint32 action, uint32 data, void *cookie);

	static AUI_ERRCODE ProgressDrawCallback(ctp2_Static *control,
											aui_Surface *surface,
											RECT &rect,
											void *cookie);

	bool GetSelectedCity(Unit & selectedCity);

	void UpdateBuildItem();

	void NoBuildItem();

	void ClearBuildItem();

	void UpdateGovernor();

	ctp2_Static *m_buildItemLabel;
	ctp2_Button *m_buildItemIconButton;
	ctp2_Button *m_buildItemTurnButton;
	ctp2_Button *m_buildRushBuy;
	ctp2_Static *m_buildItemProgressBar;
	ctp2_Static *m_rushBuyCost;

	ctp2_Button *m_cityListPreviousButton;
	ctp2_DropDown *m_cityListDropDown;
	ctp2_Button *m_cityListNextButton;

	ctp2_Button *m_governorToggleButton;
	ctp2_DropDown *m_governorDropDown;

	Unit m_currentCity;
	sint32 m_currentNumItems;
	sint32 m_currentCategory;
	sint32 m_currentItem;
	sint32 m_currentTurns;

	bool m_useGovernor;
	sint32 m_currentGovernor;

};

#endif
