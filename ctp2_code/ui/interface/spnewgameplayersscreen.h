//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : Handling single player game start options. 
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
// - Redisign of the single player new game players screen by Martin Gühmann.
//   Instead of buttons to determine the number of players in a new game 
//   this screen now allows the player to specify the player index he wants
//   to use determines the player color, he can also specify how many civs
//   should be in the game at the start and how many civs in the game should 
//   be maximal in the game. The maximum number of players in one game is
//   currently 32 and is hard encoded somewhere else. 
//
//----------------------------------------------------------------------------


#pragma once

#ifndef __SPNEWGAMEPLAYERSSCREEN_H__
#define __SPNEWGAMEPLAYERSSCREEN_H__

#include "aui_control.h"

sint32 spnewgameplayersscreen_displayMyWindow();
sint32 spnewgameplayersscreen_removeMyWindow(uint32 action);
AUI_ERRCODE spnewgameplayersscreen_Initialize( aui_Control::ControlActionCallback *callback = NULL );
AUI_ERRCODE spnewgameplayersscreen_Cleanup();

void spnewgameplayersscreen_backPress(aui_Control *control, uint32 action, uint32 data, void *cookie );
void spnewgameplayersscreen_SetMaxPlayers(sint32 maxPlayers);

void spnewgameplayersscreen_NumPlayerSpinner(aui_Control *control, uint32 action, uint32 data, void *cookie);
void spnewgameplayersscreen_MaxPlayerSpinner(aui_Control *control, uint32 action, uint32 data, void *cookie);
void spnewgameplayersscreen_PlayerSpinner(aui_Control *control, uint32 action, uint32 data, void *cookie);


#endif
