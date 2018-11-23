//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ header
// Description  : The city espionage screen
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
// - Improved cleanup and structure.
//
//----------------------------------------------------------------------------
//
/// \file   CityEspionage.h
/// \brief  City espionage screen display (declarations)

#if defined(HAVE_PRAGMA_ONCE)
#pragma once
#endif

#ifndef CITYESPIONAGE_H__
#define CITYESPIONAGE_H__

class ctp2_Window;
class ctp2_ListBox;
class ctp2_Static;
class ctp2_DropDown;
class ctp2_Button;
class aui_Control;
class DiplomacyProposalRecord;
class SlicObject;
class Unit;

#include "ctp2_Menu.h"

#include "diplomattypes.h"

class CityEspionage
{
  private:

	ctp2_Window *m_window;

	static ctp2_Button *m_cancelButton;

  public:
	CityEspionage(AUI_ERRCODE *err);
	~CityEspionage();

	static AUI_ERRCODE Initialize();
	static AUI_ERRCODE Cleanup();
	static AUI_ERRCODE Display(Unit *c=NULL);
	static AUI_ERRCODE Hide();

	static void CancelCallback(aui_Control *control, uint32 action, uint32 data, void *cookie);
};

#endif
