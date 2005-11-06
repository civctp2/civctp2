//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ header
// Description  : Screen utilities
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
// - Generate debug version when set.
//
//----------------------------------------------------------------------------
//
// Modifications from the original Activision code:
//
// - Added close_AllScreensAndUpdateInfoScreen so that on a new turn the 
//   information window can stay open. (Aug. 7th 2005 Martin Gühmann)
//
//----------------------------------------------------------------------------


#ifdef HAVE_PRAGMA_ONCE
#pragma once
#endif
#ifndef __SCREENUTILS_H__
#define __SCREENUTILS_H__

#ifdef _DEBUG

#define SET_TIME				g_screenTime = GetTickCount();

#define GET_ELAPSED_TIME(x)		g_screenTime = (GetTickCount() - g_screenTime) * 0.001;	\
								MBCHAR str[50];	\
								sprintf( str, "%4.2f secs - %s", g_screenTime, x );	\
								if ( g_debugWindow) g_debugWindow->AddText( str );
#endif

class Sequence;

sint32 open_CreditsScreen( void );
sint32 close_CreditsScreen( void );

sint32 open_WorkView( void );
sint32 close_WorkView( void );

sint32 open_CityView( void );
sint32 close_CityView( void );

sint32 open_CityStatus( void );
sint32 close_CityStatus( void );

sint32 open_CivStatus( void );
sint32 close_CivStatus( void );

sint32 open_ScienceStatus( void );
sint32 close_ScienceStatus( void );

sint32 open_ScienceVictory( void );
sint32 close_ScienceVictory( void );

sint32 open_UnitStatus( void );
sint32 close_UnitStatus( void );

sint32 open_TradeStatus( void );
sint32 close_TradeStatus( void );

sint32 open_VictoryWindow( void );
sint32 close_VictoryWindow( void );

sint32 open_Diplomacy( void );
sint32 close_Diplomacy( void );

sint32 open_InfoScreen( void );
sint32 close_InfoScreen( void );

sint32 open_GreatLibrary( sint32 index, BOOL sci = FALSE );
sint32 open_GreatLibrary( void );
sint32 close_GreatLibrary( void );

sint32 open_OptionsScreen( sint32 fromWhichScreen );
sint32 close_OptionsScreen( void );

sint32 open_KeyMappingScreen( void );
sint32 close_KeyMappingScreen( void );

sint32 open_EndGame( void );
sint32 close_EndGame( void );

sint32 open_TutorialWin( void );
sint32 close_TutorialWin( void );

sint32 close_AllScreens( void );
sint32 close_AllScreensAndUpdateInfoScreen( void );

sint32 open_ScenarioEditor(void);
sint32 close_ScenarioEditor(void);

#endif
