//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : Language settings screen
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
// - Created file. (04-May-2025 Martin Gühmann)
//
//----------------------------------------------------------------------------

#if defined(HAVE_PRAGMA_ONCE)
#pragma once
#endif

#ifndef LANGUAGESCREEN_H_
#define LANGUAGESCREEN_H_

#include "auitypes.h"       // AUI_ERRCODE
#include "ctp2_inttypes.h"  // sint32, uint32

#include "c3_popupwindow.h"

class aui_Control;
class ctp2_ListBox;
class ctp2_ListItem;
class ctp2_Button;
class ctp2_Static;

class LanguageScreen : c3_PopupWindow
{
public:
	LanguageScreen(AUI_ERRCODE *errcode, sint32 bpp);
	virtual ~LanguageScreen();

	static void Init();
	static void Cleanup();
	static void DisplayWindow();
	static void RemoveWindow(uint32 action);

	static void AcceptPress(aui_Control *control, uint32 action, uint32 data, void *cookie);
	static void CancelPress(aui_Control *control, uint32 action, uint32 data, void *cookie);
	static void GetLanguageFromOS(aui_Control *control, uint32 action, uint32 data, void *cookie);
	static void ItemSelected(aui_Control *control, uint32 action, uint32 data, void *cookie);
	static sint32 CompareItems(ctp2_ListItem *item1, ctp2_ListItem *item2, sint32 column);

	void SetLanguageDescription();
	void ApplyLanguage();
	void SelectLocLanguage();

private:

	static LanguageScreen* s_languageScreen;

	ctp2_ListBox*          m_LanguageListBox;
	ctp2_Static*           m_languageDescription;
	ctp2_Button*           m_getLanguageFromOS;
	ctp2_Static*           m_warning;

	sint32                 m_startLanguage;
};

#endif
