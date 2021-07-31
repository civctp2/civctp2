//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ header file
// Description  : Behaviour of the National Management dialog
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
// - #01 Added a third tab to the dialog that shows the nuber of experts and
//   military units in each city. (L. Hirth 6/2004)
// - Added City Manager button and functions callback. - July 24th 2005 Martin Gühmann
//
//----------------------------------------------------------------------------

#ifdef HAVE_PRAGMA_ONCE
#pragma once
#endif
#ifndef ___BMH_NATIONAL_MANAGEMENT_DIALOG_HEADER
#define ___BMH_NATIONAL_MANAGEMENT_DIALOG_HEADER

#include "Unit.h"

class aui_Control;
class ctp2_Button;
class ctp2_DropDown;
class ctp2_ListBox;
class ctp2_ListItem;
class ctp2_Static;
class ctp2_Window;
class ctp2_Tab;

class NationalManagementDialog {
public:
	static void Open();
	static void Close();
	static void Cleanup();
	static bool IsShown();
	static void UpdateCity(const Unit & city);

	NationalManagementDialog();

	void Update();

private:

	void Show();

	void Hide();

	void UpdateResourceList();

	void UpdateStatusList();

	void UpdateSpecialistList();

	void UpdateGovernor();

	void UpdateBuildQueue();

	void UpdateRushBuy();

	static ctp2_Static *GetListItemColumn(ctp2_ListItem *item, sint32 column);

	ctp2_ListItem *CreateResourceItem(const Unit & city);
	void UpdateResourceItem(ctp2_ListItem *item, const Unit & city);

	ctp2_ListItem *CreateStatusItem(const Unit &city);
	void UpdateStatusItem(ctp2_ListItem *item, const Unit & city);

	ctp2_ListItem *CreateSpecialistItem(const Unit &city);
	void UpdateSpecialistItem(ctp2_ListItem *item, const Unit & city);

	bool CanBuild(uint32 category, sint32 type);

	ctp2_ListItem *CreateBuildQueueItem(uint32 category, sint32 type);

	static sint32 CompareResources(ctp2_ListItem *item1, ctp2_ListItem *item2,
	                               sint32 column);

	static sint32 CompareStatus(ctp2_ListItem *item1, ctp2_ListItem *item2,
	                            sint32 column);

	static sint32 CompareSpecialists(ctp2_ListItem *item1, ctp2_ListItem *item2,
	                                 sint32 column);

	static void StatusListSelectActionCallback(aui_Control *control,
	                                           uint32 action, uint32 data, void *cookie);

	static void CloseButtonActionCallback(aui_Control *control,
	                                      uint32 action, uint32 data, void *cookie);

	static void BuildQueueButtonActionCallback(aui_Control *control,
	                                           uint32 action, uint32 data, void *cookie);

	static void CityManagerButtonActionCallback(aui_Control *control,
	                                            uint32 action, uint32 data, void *cookie);

	static void DisbandButtonActionCallback(aui_Control *control,
	                                        uint32 action, uint32 data, void *cookie);

	static void DisbandCallback(bool response, void *userData);

	static void ToggleGovernorButtonActionCallback(aui_Control *control,
	                                               uint32 action, uint32 data, void *cookie);

	static void SelectGovernorActionCallback(aui_Control *control,
	                                         uint32 action, uint32 data, void *cookie);

	static void RushBuyButtonActionCallback(aui_Control *control,
	                                        uint32 action, uint32 data, void *cookie);

	static void ResourceListSelectActionCallback(aui_Control *control,
	                                             uint32 action, uint32 data, void *cookie);

	static void SpecialistListSelectActionCallback(aui_Control *control,
	                                               uint32 action, uint32 data, void *cookie);

	void UpdateMainButtons(ctp2_ListBox *box);

	static void TabActionCallback(aui_Control *control,
	                              uint32 action, uint32 data, void *cookie);

	void MirrorSelectedCities();
	static Unit GetSelectedCity();
	static void GotoSelectedCity();
	static Unit GetCityAtIndex(ctp2_ListBox * listbox, uint32 index);

	ctp2_Window     *m_window;

	ctp2_Button     *m_closeButton;

	ctp2_ListBox    *m_resourceList;
	ctp2_ListBox    *m_statusList;
	ctp2_ListBox    *m_specialistList;

	ctp2_Button     *m_governorToggle;
	ctp2_DropDown   *m_governorDropDown;

	ctp2_Button     *m_rushBuyButton;
	ctp2_Static     *m_rushBuyValue;

	ctp2_Button     *m_buildQueueButton;
	ctp2_Button     *m_cityManagerButton;
	ctp2_Button     *m_disbandButton;

	ctp2_Tab        *m_statusTab;
	ctp2_Tab        *m_resourceTab;
	ctp2_Tab        *m_specialistTab;

	bool m_mirroring;
};

extern NationalManagementDialog *g_nationalManagementDialog;

#endif
