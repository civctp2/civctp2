//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ header
// Description  : Handling of the action on the screen
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
// Extracted from Director.h
//
//----------------------------------------------------------------------------
#pragma once

//----------------------------------------------------------------------------
//
// Project imports
//
//----------------------------------------------------------------------------

#include "gfx/spritesys/Sequence.h"

class DQAction;

//----------------------------------------------------------------------------
//
// Exported names
//
//----------------------------------------------------------------------------

enum DHEXECUTE {
  DHEXECUTE_NONE = -1,

  DHEXECUTE_NORMAL,
  DHEXECUTE_IMMEDIATE,

  DHEXECUTE_MAX
};

typedef void(DQHandler)(DQAction* action,
                        SequenceWeakPtr seq,
                        DHEXECUTE executeType);

DQHandler dh_move;
DQHandler dh_teleport;
DQHandler dh_projectileMove;
DQHandler dh_speceffect;
DQHandler dh_attack;
DQHandler dh_attackpos;
DQHandler dh_specialAttack;
DQHandler dh_death;
DQHandler dh_morphUnit;
DQHandler dh_hide;
DQHandler dh_show;
DQHandler dh_work;
DQHandler dh_fastkill;
DQHandler dh_removeVision;
DQHandler dh_addVision;
DQHandler dh_setVisibility;
DQHandler dh_setOwner;
DQHandler dh_setVisionRange;
DQHandler dh_combatflash;
DQHandler dh_copyVision;
DQHandler dh_centerMap;
DQHandler dh_selectUnit;
DQHandler dh_endTurn;
DQHandler dh_battle;
DQHandler dh_playSound;
DQHandler dh_playWonderMovie;
DQHandler dh_playVictoryMovie;
DQHandler dh_message;
DQHandler dh_faceoff;
DQHandler dh_terminateFaceoff;
DQHandler dh_terminateSound;
DQHandler dh_invokeThroneRoom;
DQHandler dh_invokeResearchAdvance;
DQHandler dh_beginScheduler;
