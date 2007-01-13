#ifndef __DIRECTORACTIONS_H__
#define __DIRECTORACTIONS_H__

class UnitActor;
class ProjectileActor;
class EffectActor;
class TradeActor;
class aui_Surface;
class Battle;
enum GAME_OVER;
class SpriteState;

#include "message.h"
#include "Unit.h"
#include "MapPoint.h"

class DQAction {
public:
	DQAction() {}
	~DQAction() {}
};

class DQActionMove : public DQAction
{
public:
	DQActionMove() {}
	~DQActionMove() {}

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
	DQActionMoveProjectile() {}
	~DQActionMoveProjectile() {}

	UnitActor					*pshooting_actor;
	UnitActor					*ptarget_actor;
	MapPoint					pmove_oldPos;
	MapPoint					pmove_newPos;
	ProjectileActor				*move_projectile;
	EffectActor					*end_projectile;
	sint32						projectile_path;
};

class DQActionSpecialEffect : public DQAction 
{
public:
	DQActionSpecialEffect() {}
	~DQActionSpecialEffect() {}

	MapPoint					speceffect_pos;
	sint32						speceffect_spriteID;
	sint32						speceffect_soundID;
};

class DQActionAttack : public DQAction
{
public:
	DQActionAttack() {}
	~DQActionAttack() {}

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
	DQActionAttackPos() {}
	~DQActionAttackPos() {}

	UnitActor					*attackpos_attacker;
	MapPoint					attackpos_attacker_pos;
	MapPoint					attackpos_target_pos;
	sint32						attackpos_soundID;
};


















class DQActionDeath : public DQAction
{
public:
	DQActionDeath() {}
	~DQActionDeath() {}

	UnitActor					*death_dead;
	UnitActor					*death_victor;
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
	DQActionMorph() {}
	~DQActionMorph() {}

	UnitActor					*morphing_actor;
	SpriteState					*ss;
	sint32				        type;
	Unit						id;
};

class DQActionHideShow : public DQAction
{
public:
	DQActionHideShow() {}
	~DQActionHideShow() {}

	UnitActor					*hiding_actor;
	MapPoint					hiding_pos;
};

class DQActionWork : public DQAction
{
public:
	DQActionWork() {}
	~DQActionWork() {}

	UnitActor					*working_actor;
	MapPoint					working_pos;
	sint32						working_soundID;

};

class DQActionFastKill : public DQAction
{
public:
	DQActionFastKill() {}
	~DQActionFastKill() {}

	UnitActor					*dead;
};

class DQActionVision : public DQAction
{
public:
	DQActionVision() {}
	~DQActionVision() {}

	MapPoint					vision_pos;
	double						vision_range;
};

class DQActionSetOwner : public DQAction
{
public:
	DQActionSetOwner() {}
	~DQActionSetOwner() {}

	UnitActor					*setowner_actor;
	sint32						owner;
};

class DQActionSetVisibility : public DQAction
{
public:
	DQActionSetVisibility() {}
	~DQActionSetVisibility() {}

	UnitActor					*setvisibility_actor;
	uint32						visibilityFlag;
};

class DQActionSetVisionRange : public DQAction
{
public:
	DQActionSetVisionRange() {}
	~DQActionSetVisionRange() {}

	UnitActor					*setvisionrange_actor;
	double						range;
};

class DQActionCombatFlash : public DQAction
{
public:
	DQActionCombatFlash() {}
	~DQActionCombatFlash() {}

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
	DQActionBattle() {}
	~DQActionBattle() {}

	Battle						*battle;
};

class DQActionPlaySound : public DQAction
{
public:
	DQActionPlaySound() {}
	~DQActionPlaySound() {}

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
	DQActionFaceoff() {}
	~DQActionFaceoff() {}

	UnitActor					*faceoff_attacker;
	MapPoint					faceoff_attacker_pos;
	UnitActor					*faceoff_attacked;
	MapPoint					faceoff_attacked_pos;
};

class DQActionTerminateFaceOff : public DQAction
{
public:
	DQActionTerminateFaceOff() {}
	~DQActionTerminateFaceOff() {}

	UnitActor					*faceroffer;
};

class DQActionTerminateSound : public DQAction
{
public:
	DQActionTerminateSound() {}
	~DQActionTerminateSound() {}

	Unit						terminate_sound_unit;
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
	DQActionInvokeResearchAdvance() {}
	~DQActionInvokeResearchAdvance() {}

	MBCHAR						*message;
};


class DQActionBeginScheduler : public DQAction
{
  public:
	DQActionBeginScheduler() {}
	~DQActionBeginScheduler() {}

	sint32 player;
};

#endif
