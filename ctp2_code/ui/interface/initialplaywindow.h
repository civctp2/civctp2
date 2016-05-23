//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : Main menu screen
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
//----------------------------------------------------------------------------
//
// Modifications from the original Activision code:
//
// - Shifted buttons from the "Single Player" subscreen into this one to
//   simplify the interface.
//   (JJB)
//
//----------------------------------------------------------------------------

/// \file   InitialPlayWindow.h
/// \brief  The first window to display (declarations).

#ifdef HAVE_PRAGMA_ONCE
#pragma once
#endif

#ifndef INITIALPLAYWINDOW_H
#define INITIALPLAYWINDOW_H

//----------------------------------------------------------------------------
// Library dependencies
//----------------------------------------------------------------------------

#include <memory>          // std::auto_ptr

//----------------------------------------------------------------------------
// Export overview
//----------------------------------------------------------------------------

class InitPlayWindow;

//----------------------------------------------------------------------------
// Project dependencies
//----------------------------------------------------------------------------

#include "aui_window.h"     // AUI_WINDOW_TYPE
#include "auitypes.h"       // AUI_ERRCODE
#include "ctp2_inttypes.h"  // sint32, uint32
#include "c3window.h"       // C3Window

class aui_Control;
class c3_Button;
class c3_Static;
class ctp2_Button;
class ctp2_Static;

//----------------------------------------------------------------------------
// Class declarations
//----------------------------------------------------------------------------

class InitPlayWindow : public C3Window
{
public:
	InitPlayWindow
    (
		AUI_ERRCODE *   retval,
		uint32          id,
		MBCHAR *        ldlBlock,
		sint32          bpp,
		AUI_WINDOW_TYPE type    = AUI_WINDOW_TYPE_STANDARD,
		bool            bevel   = true
    );

private:
    std::auto_ptr<ctp2_Button>  m_spriteTest;
    std::auto_ptr<ctp2_Button>  m_email;
    std::auto_ptr<ctp2_Button>  m_hotseat;
    std::auto_ptr<ctp2_Button>  m_mp;
    std::auto_ptr<ctp2_Button>  m_credits;
    std::auto_ptr<ctp2_Button>  m_quit;
    std::auto_ptr<ctp2_Button>  m_tutorial;
    std::auto_ptr<ctp2_Button>  m_newgame;
    std::auto_ptr<ctp2_Button>  m_loadgame;
    std::auto_ptr<ctp2_Button>  m_options;
    std::auto_ptr<c3_Static>    m_background;
    std::auto_ptr<ctp2_Static>  m_testBox;
};

//----------------------------------------------------------------------------
// Function declarations
// The definitions are in InitialPlayScreen.cpp
//----------------------------------------------------------------------------

sint32 initialplayscreen_displayMyWindow();
sint32 initialplayscreen_removeMyWindow(uint32 action);
AUI_ERRCODE initialplayscreen_Initialize( void );
void initialplayscreen_Cleanup(void);

void
spritetest_spPress(aui_Control *control, uint32 action, uint32 data, void *cookie );

void
initialplayscreen_spPress(aui_Control *control, uint32 action, uint32 data, void *cookie );
void
initialplayscreen_mpPress(aui_Control *control, uint32 action, uint32 data, void *cookie );
void
initialplayscreen_loadPress(aui_Control *control, uint32 action, uint32 data, void *cookie );
void
initialplayscreen_continuePress(aui_Control *control, uint32 action, uint32 data, void *cookie );
void
initialplayscreen_instantPress(aui_Control *control, uint32 action, uint32 data, void *cookie );
void
initialplayscreen_mapeditorPress(aui_Control *control, uint32 action, uint32 data, void *cookie );
void
initialplayscreen_quitPress(aui_Control *control, uint32 action, uint32 data, void *cookie );
void
initialplayscreen_creditsPress(aui_Control *control, uint32 action, uint32 data, void *cookie );
void initialplayscreen_hotseatPress(aui_Control *control, uint32 action, uint32 data, void *cookie);
void initialplayscreen_emailPress(aui_Control *control, uint32 action, uint32 data, void *cookie);
void initialplayscreen_newgamePress(aui_Control *control, uint32 action, uint32 data, void *cookie );
void initialplayscreen_loadgamePress(aui_Control *control, uint32 action, uint32 data, void *cookie );
void initialplayscreen_tutorialPress(aui_Control *control, uint32 action, uint32 data, void *cookie );
void initialplayscreen_optionsPress(aui_Control *control, uint32 action, uint32 data, void *cookie );

#endif
