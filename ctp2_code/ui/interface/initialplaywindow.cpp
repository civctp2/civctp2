//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : Main menu screen
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
// _DEBUG
// - Generates debug information when set.
//
//----------------------------------------------------------------------------
//
// Modifications from the original Activision code:
//
// - Shifted buttons from the "Single Player" subscreen into this one to
//   simplify the interface.
//   (JJB)
// - Removed civilisation database references. (Aug 20th 2005 Martin Gühmann)
// - Repaired memory leaks.
//
//----------------------------------------------------------------------------

/// \file   InitialPlayWindow.cpp
/// \brief  The first window to display (defnitions).

#include "c3.h"
#include "initialplaywindow.h"

#include "aui.h"
#include "aui_imagebase.h"
#include "aui_ldl.h"
#include "aui_stringtable.h"
#include "aui_surface.h"
#include "aui_uniqueid.h"           // aui_UniqueId
#include "aui_textbase.h"
#include "aui_textfield.h"
#include "c3_button.h"
#include "c3_static.h"
#include "c3ui.h"
#include "c3window.h"
#include "ctp2_button.h"
#include "ctp2_Static.h"
#include "spnewgamewindow.h"        // spNew_ctp2_Button

InitPlayWindow::InitPlayWindow
(
    AUI_ERRCODE *       retval, 
    uint32              id,
    MBCHAR *			ldlBlock, 
    sint32              bpp, 
    AUI_WINDOW_TYPE     type, 
    bool                bevel
)
: 
    C3Window        (retval, id, ldlBlock, bpp, type, bevel),
    m_spriteTest    (spNew_ctp2_Button
                       (retval, ldlBlock, 
		   		        "SpriteTestButton", "Sprite Test", spritetest_spPress, 
                        "CTP2_BUTTON_TEXT_RIGHT_LARGE"
                       )
                    ),
    m_email         (spNew_ctp2_Button
                       (retval, ldlBlock, 
		   		        "EmailButton", "Email",	initialplayscreen_emailPress, 
                        "CTP2_BUTTON_TEXT_RIGHT_LARGE"
                       )
                    ),
    m_hotseat       (spNew_ctp2_Button
                       (retval, ldlBlock, 
		   		        "HotSeatButton", "Hotseat", initialplayscreen_hotseatPress, 
                        "CTP2_BUTTON_TEXT_RIGHT_LARGE"
                       )
                    ),
    m_mp            (spNew_ctp2_Button
                       (retval, ldlBlock, 
		   		        "MPButton", "Multiplayer", initialplayscreen_mpPress, 
                        "CTP2_BUTTON_TEXT_RIGHT_LARGE"
                       )
                    ),
    m_credits       (spNew_ctp2_Button
                       (retval, ldlBlock, 
		   		        "CreditsButton", "Credits", initialplayscreen_creditsPress, 
                        "CTP2_BUTTON_TEXT_RIGHT_LARGE"
                       )
                    ),
    m_quit          (spNew_ctp2_Button
                       (retval, ldlBlock, 
		   		        "QuitButton", "Quit Out", initialplayscreen_quitPress, 
                        "CTP2_BUTTON_TEXT_RIGHT_LARGE"
                       )
                    ),
    // New buttons moved from the SP screen (code taken from spwindow.cpp)
    m_tutorial      (spNew_ctp2_Button
                       (retval, ldlBlock, "TutorialButton", initialplayscreen_tutorialPress)
                    ),
    m_newgame       (spNew_ctp2_Button
                       (retval, ldlBlock, "NewGameButton", initialplayscreen_newgamePress)
                    ),
    m_loadgame      (spNew_ctp2_Button
                       (retval, ldlBlock, "LoadGameButton", initialplayscreen_loadgamePress)
                    ),
    m_options       (spNew_ctp2_Button
                       (retval, ldlBlock, "OptionsButton", initialplayscreen_optionsPress)
                    ),
    m_background    (spNew_c3_Static(retval, ldlBlock, "Background")),
    m_testBox       (new ctp2_Static
                        (retval, aui_UniqueId(), "InitPlayWindow.TestTextBox")
                    )
{
#ifndef _DEBUG
    m_spriteTest->Hide();
#endif
}

