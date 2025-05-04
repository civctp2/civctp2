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
class ctp2_Button;
class ctp2_HyperTextBox;

/*sint32 musicscreen_displayMyWindow();
sint32 musicscreen_removeMyWindow(uint32 action);
AUI_ERRCODE musicscreen_Initialize( void );
void musicscreen_Cleanup();

void musicscreen_checkPress(aui_Control *control, uint32 action, uint32 data, void *cookie );
void musicscreen_selectTrackPress(aui_Control *control, uint32 action, uint32 data, void *cookie );
void musicscreen_acceptPress(aui_Control *control, uint32 action, uint32 data, void *cookie );*/

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
	static void GetLanguageFromOS(aui_Control *control, uint32 action, uint32 data, void *cookie);

private:
	static LanguageScreen* s_languageScreen;

	ctp2_ListBox*          m_LanguageListBox;
	ctp2_HyperTextBox*     m_languageDescription;
	ctp2_Button*           m_getLanguageFromOS;
};

#endif
