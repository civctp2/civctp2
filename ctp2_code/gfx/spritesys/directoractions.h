//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ header
// Description  : Director queue actions
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
// - Removed references to (undefined) ProjectileActor class.
// - Repaired memory leak (implemented DQActionMoveProjectile destructor)
//
//----------------------------------------------------------------------------
//
/// \file   DirectorActions.h
/// \brief  Action types for processing by the director

#if defined(HAVE_PRAGMA_ONCE)
#pragma once
#endif

#ifndef DIRECTORACTIONS_H__
#define DIRECTORACTIONS_H__

//----------------------------------------------------------------------------
//
// Library imports
//
//----------------------------------------------------------------------------

// none

//----------------------------------------------------------------------------
//
// Exported names
//
//----------------------------------------------------------------------------

class DQAction;
class DQActionMove;
class DQActionMoveProjectile;
class DQActionSpecialEffect;
class DQActionAttack;
class DQActionAttackPos;
class DQActionDeath;
class DQActionMorph;
class DQActionHideShow;
class DQActionWork;
class DQActionFastKill;
class DQActionVision;
class DQActionSetOwner;
class DQActionSetVisibility;
class DQActionSetVisionRange;
class DQActionCombatFlash;
class DQActionCopyVision;
class DQActionCenterMap;
class DQActionUnitSelection;
class DQActionEndTurn;
class DQActionBattle;
class DQActionPlaySound;
class DQActionPlayWonderMovie;
class DQActionPlayVictoryMovie;
class DQActionMessage;
class DQActionFaceoff;
class DQActionTerminateFaceOff;
class DQActionTerminateSound;
class DQActionInvokeThroneRoom;
class DQActionInvokeResearchAdvance;
class DQActionBeginScheduler;

//----------------------------------------------------------------------------
//
// Project imports
//
//----------------------------------------------------------------------------

#include "GameOver.h"
#include "MapPoint.h"
#include "message.h"
#include "Unit.h"

class UnitActor;
class EffectActor;
class TradeActor;
class aui_Surface;
class Battle;
class SpriteState;

//----------------------------------------------------------------------------
//
// Declarations
//
//----------------------------------------------------------------------------

class DQAction {
public:
	DQAction() {}
	virtual ~DQAction() {}
};

class DQActionMove : public DQAction
{
public:
	DQActionMove()
    :
        DQAction        (),
        move_actor      (NULL),
        move_oldPos     (),
        move_newPos     (),
        moveArraySize   (0),
        moveActors      (NULL),
        numRevealed     (0),
        revealedActors  (NULL),
        move_soundID    (CTPRecord::INDEX_INVALID)
    {};

	UnitActor					*move_actor;
	MapPoint					move_oldPos;
	MapPoint					move_newPos;
	sint32						moveArraySize;
	UnitActor					**moveActors;
	sint32						numRevealed;
	UnitActor					**revealedActors;
	sint32						move_soundID;
};

class DQActionMoveProjectile : public DQAction
{
public:
	DQActionMoveProjectile();
	~DQActionMoveProjectile();

	UnitActor					*pshooting_actor;
	UnitActor					*ptarget_actor;
	MapPoint					pmove_oldPos;
	MapPoint					pmove_newPos;
	EffectActor					*end_projectile;
	sint32						projectile_path;
};

class DQActionSpecialEffect : public DQAction
{
public:
	DQActionSpecialEffect() {}

	MapPoint					speceffect_pos;
	sint32						speceffect_spriteID;
	sint32						speceffect_soundID;
};

class DQActionAttack : public DQAction
{
public:
	DQActionAttack()
    :
        DQAction        (),
        attacker        (NULL),
        defender        (NULL),
        attacker_Pos    (),
        defender_Pos    (),
        attacker_ID     (0),
        defender_ID     (0),
        attacker_IsCity (false),
        defender_IsCity (false)
    {};

	UnitActor					*attacker;
	UnitActor					*defender;
	MapPoint					attacker_Pos;
	MapPoint					defender_Pos;
	sint32						attacker_ID;
	sint32						defender_ID;
	BOOL						attacker_IsCity;
	BOOL						defender_IsCity;
};

class DQActionAttackPos : public DQAction
{
public:
	DQActionAttackPos()
    :
        DQAction                (),
        attackpos_attacker      (NULL),
        attackpos_attacker_pos  (),
        attackpos_target_pos    (),
        attackpos_soundID       (CTPRecord::INDEX_INVALID)
    {};

	UnitActor					*attackpos_attacker;
	MapPoint					attackpos_attacker_pos;
	MapPoint					attackpos_target_pos;
	sint32						attackpos_soundID;
};

class DQActionDeath : public DQAction
{
public:
	DQActionDeath()
    :
        DQAction		(),
        dead			(NULL),
        dead_pos		(),
        dead_soundID	(CTPRecord::INDEX_INVALID)
    {};

	UnitActor	*dead;
	MapPoint	dead_pos;
	sint32		dead_soundID;
};

class DQActionMorph : public DQAction
{
public:
	DQActionMorph()
    :
        DQAction        (),
        morphing_actor  (NULL),
        ss              (NULL),
        type            (0),
        id              ()
    {};

	UnitActor					*morphing_actor;
	SpriteState					*ss;
	sint32				        type;
	Unit						id;
};

class DQActionHideShow : public DQAction
{
public:
	DQActionHideShow()
    :
        DQAction        (),
        hiding_actor    (NULL),
        hiding_pos      ()
    {}

	UnitActor *     hiding_actor;
	MapPoint		hiding_pos;
};

class DQActionWork : public DQAction
{
public:
	DQActionWork()
    :
        DQAction        (),
        working_actor   (NULL),
        working_pos     (),
        working_soundID (CTPRecord::INDEX_INVALID)
    {};

	UnitActor*      working_actor;
	MapPoint		working_pos;
	sint32			working_soundID;

};

class DQActionFastKill : public DQAction
{
public:
	DQActionFastKill()
    :
        DQAction    (),
        dead        (NULL)
    {};

	UnitActor *     dead;
};

class DQActionVision : public DQAction
{
public:
	DQActionVision() {}

	MapPoint		vision_pos;
	double			vision_range;
};

class DQActionSetOwner : public DQAction
{
public:
	DQActionSetOwner()
    :
        DQAction        (),
        setowner_actor  (NULL),
        owner           (PLAYER_UNASSIGNED)
    {};

	UnitActor *     setowner_actor;
	sint32			owner;
};

class DQActionSetVisibility : public DQAction
{
public:
	DQActionSetVisibility()
    :
        DQAction            (),
        setvisibility_actor (NULL),
        visibilityFlag      (0)
    {};

	UnitActor *     setvisibility_actor;
	uint32			visibilityFlag;
};

class DQActionSetVisionRange : public DQAction
{
public:
	DQActionSetVisionRange()
    :
        DQAction                (),
        setvisionrange_actor    (NULL)
    {}

	UnitActor *     setvisionrange_actor;
	double			range;
};

class DQActionCombatFlash : public DQAction
{
public:
	DQActionCombatFlash() {}

	MapPoint					flash_pos;
};
















class DQActionCopyVision : public DQAction
{
public:
	DQActionCopyVision() {}
	~DQActionCopyVision() {}

	BOOL						copyVision;
};

class DQActionCenterMap : public DQAction
{
public:
	DQActionCenterMap() {}
	~DQActionCenterMap() {}

	MapPoint					centerMap_pos;
};

class DQActionUnitSelection : public DQAction
{
public:
	DQActionUnitSelection() {}
	~DQActionUnitSelection() {}

	uint32 						flags;
};

class DQActionEndTurn : public DQAction
{
public:
	DQActionEndTurn() {}
	~DQActionEndTurn() {}

	BOOL						endTurn;
};

class DQActionBattle : public DQAction
{
public:
	DQActionBattle()
    :
        DQAction        (),
        battle          (NULL)
    {};
	~DQActionBattle() {}

	Battle						*battle;
};

class DQActionPlaySound : public DQAction
{
public:
	DQActionPlaySound()
    :
        DQAction            (),
        playsound_soundID   (CTPRecord::INDEX_INVALID),
        playsound_pos       ()
    {};
	~DQActionPlaySound() {};

	sint32						playsound_soundID;
	MapPoint					playsound_pos;
};

class DQActionPlayWonderMovie : public DQAction
{
public:
	DQActionPlayWonderMovie() {}
	~DQActionPlayWonderMovie() {}

	sint32						playwondermovie_which;
};

class DQActionPlayVictoryMovie : public DQAction
{
public:
	DQActionPlayVictoryMovie() {}
	~DQActionPlayVictoryMovie() {}

	GAME_OVER					playvictorymovie_reason;
};

class DQActionMessage : public DQAction
{
public:
	DQActionMessage() {}
	~DQActionMessage() {}

	Message						message;
};

class DQActionFaceoff : public DQAction
{
public:
	DQActionFaceoff()
    :
        DQAction                (),
        faceoff_attacker        (NULL),
        faceoff_attacker_pos    (),
        faceoff_attacked        (NULL),
        faceoff_attacked_pos    ()
    {};
	~DQActionFaceoff() {}

	UnitActor *     faceoff_attacker;
	MapPoint	    faceoff_attacker_pos;
	UnitActor *     faceoff_attacked;
	MapPoint	    faceoff_attacked_pos;
};

class DQActionTerminateFaceOff : public DQAction
{
public:
	DQActionTerminateFaceOff()
    :
        DQAction    (),
        faceroffer  (NULL)
    {};
	~DQActionTerminateFaceOff() {}

	UnitActor *     faceroffer;
};

class DQActionTerminateSound : public DQAction
{
public:
	DQActionTerminateSound() {}
	~DQActionTerminateSound() {}

	Unit            terminate_sound_unit;
};

class DQActionInvokeThroneRoom : public DQAction
{
public:
	DQActionInvokeThroneRoom() {}
	~DQActionInvokeThroneRoom() {}
};

class DQActionInvokeResearchAdvance : public DQAction
{
public:
	DQActionInvokeResearchAdvance()
    :
        DQAction    (),
        message     (NULL)
    {}
	~DQActionInvokeResearchAdvance();

	MBCHAR *        message;
};

class DQActionBeginScheduler : public DQAction
{
  public:
	DQActionBeginScheduler() {}
	~DQActionBeginScheduler() {}

	sint32 player;
};

#endif
