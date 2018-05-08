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

#include <memory>
#include <vector>

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

#include "gfx/spritesys/SpriteState.h"
#include "gs/gameobj/GameOver.h"
#include "gs/world/MapPoint.h"
#include "gs/gameobj/message.h"
#include "gs/gameobj/Unit.h"

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
	DQActionMove() :
        DQAction        (),
        move_oldPos     (),
        move_newPos     (),
        move_soundID    (CTPRecord::INDEX_INVALID)
    {};

	std::weak_ptr<UnitActor> move_actor;
	MapPoint					move_oldPos;
	MapPoint					move_newPos;
  std::vector<std::weak_ptr<UnitActor> > moveActors;
  std::vector<std::weak_ptr<UnitActor> > revealedActors;
	sint32						move_soundID;
};

class DQActionMoveProjectile : public DQAction
{
public:
	DQActionMoveProjectile();
	~DQActionMoveProjectile();

	std::weak_ptr<UnitActor> pshooting_actor;
	std::weak_ptr<UnitActor> ptarget_actor;
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
        attacker_Pos    (),
        defender_Pos    (),
        attacker_ID     (0),
        defender_ID     (0),
        attacker_IsCity (false),
        defender_IsCity (false)
    {};

	std::weak_ptr<UnitActor> attacker;
	std::weak_ptr<UnitActor> defender;
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
        attackpos_attacker_pos  (),
        attackpos_target_pos    (),
        attackpos_soundID       (CTPRecord::INDEX_INVALID)
    {};

	std::weak_ptr<UnitActor> attackpos_attacker;
	MapPoint					attackpos_attacker_pos;
	MapPoint					attackpos_target_pos;
	sint32						attackpos_soundID;
};

class DQActionDeath : public DQAction
{
public:
	DQActionDeath()
    :
        DQAction            (),
        dead_id             (0),
        victor_id           (0),
        victor_Pos          (),
        dead_Pos            (),
        dead_soundID        (CTPRecord::INDEX_INVALID),
        victor_soundID      (CTPRecord::INDEX_INVALID)
    {};

	std::shared_ptr<UnitActor> death_dead;
	std::weak_ptr<UnitActor> death_victor;
	sint32						dead_id;
	sint32						victor_id;
	MapPoint					victor_Pos;
	MapPoint					dead_Pos;
	sint32						dead_soundID;
	sint32						victor_soundID;
};

class DQActionMorph : public DQAction
{
public:

  DQActionMorph()
    :
        DQAction        (),
        type            (0),
        id              ()
    {};

	std::weak_ptr<UnitActor> morphing_actor;
	SpriteStatePtr  ss;
	sint32          type;
	Unit            id;
};

class DQActionHideShow : public DQAction
{
public:
	DQActionHideShow()
    :
        DQAction        (),
        hiding_pos      ()
    {}

	std::weak_ptr<UnitActor> hiding_actor;
	MapPoint		hiding_pos;
};

class DQActionWork : public DQAction
{
public:
	DQActionWork()
    :
        DQAction        (),
        working_pos     (),
        working_soundID (CTPRecord::INDEX_INVALID)
    {};

	std::weak_ptr<UnitActor> working_actor;
	MapPoint		working_pos;
	sint32			working_soundID;

};

class DQActionFastKill : public DQAction
{
public:
	DQActionFastKill()
    :
        DQAction    ()
    {};

	std::shared_ptr<UnitActor> dead;
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
        owner           (PLAYER_UNASSIGNED)
    {};

	std::weak_ptr<UnitActor> setowner_actor;
	sint32			owner;
};

class DQActionSetVisibility : public DQAction
{
public:
	DQActionSetVisibility()
    :
        DQAction            (),
        visibilityFlag      (0)
    {};

	std::weak_ptr<UnitActor> setvisibility_actor;
	uint32			visibilityFlag;
};

class DQActionSetVisionRange : public DQAction
{
public:
	DQActionSetVisionRange()
    :
        DQAction                ()
    {}

	std::weak_ptr<UnitActor> setvisionrange_actor;
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
        faceoff_attacker_pos    (),
        faceoff_attacked_pos    ()
    {};
	~DQActionFaceoff() {}

	std::weak_ptr<UnitActor> faceoff_attacker;
	MapPoint	    faceoff_attacker_pos;
	std::weak_ptr<UnitActor> faceoff_attacked;
	MapPoint	    faceoff_attacked_pos;
};

class DQActionTerminateFaceOff : public DQAction
{
public:
	DQActionTerminateFaceOff()
    :
        DQAction    ()
    {};
	~DQActionTerminateFaceOff() {}

	std::weak_ptr<UnitActor> faceroffer;
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
