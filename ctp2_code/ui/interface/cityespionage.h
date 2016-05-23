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

class CityEspionage;

#include "auitypes.h"           // AUI_ERRCODE
#include "ctp2_inttypes.h"      // uint32
#include "Unit.h"               // Unit
class aui_Control;
class ctp2_Button;
class ctp2_ListBox;
class ctp2_Window;

/// City espionage window and data
/// \remarks    Implemented using a sort of singleton pattern.
///             Only 1 city espionage screen can be active at a given time.
///             A second call of Display will overwrite the data.
class CityEspionage
{
public:
    static void         Display(Unit a_City);

    // No Hide() needed: this is done through the CancelCallback when pressing
    // the Cancel button.

private:
    CityEspionage();
    ~CityEspionage();

    void                DisplayWindow(Unit a_City);
    void                HideWindow();

    static void         CancelCallback
    (
        aui_Control *   control,
        uint32          action,
        uint32          data,
        void *          cookie
    );

    static void         Cleanup();

    /// The actual window
    ctp2_Window *       m_window;
    /// Cancel button
    ctp2_Button *       m_cancelButton;
    /// Displayed intelligence from the espionage action
    ctp2_ListBox *      m_inventoryList;
};

#endif
