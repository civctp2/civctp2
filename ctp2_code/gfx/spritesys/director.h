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
//----------------------------------------------------------------------------

#ifdef HAVE_PRAGMA_ONCE
#pragma once
#endif

#ifndef __DIRECTOR_H__
#define __DIRECTOR_H__

#include "aui_mouse.h"
#include "gamesounds.h"
#include "Unit.h"               // Unit, SPECATTACK

class TradeRoute;
class UnitActor;
class Battle;

class Sequence;

class Director
{
public:
	static Director* CreateDirector();
	virtual ~Director() {}

	virtual void ReloadAllSprites() = 0;
	virtual void NotifyResync() = 0;

	virtual void Process() = 0;
	virtual void PauseDirector(BOOL pause) = 0;
	virtual void Draw(RECT *paintRect, sint32 layer) = 0;
	virtual void OffsetActors(sint32 deltaX, sint32 deltaY) = 0;

	virtual void NextPlayer() = 0;

	virtual void CatchUp() = 0;
	virtual bool CaughtUp() = 0;

#ifdef _DEBUG
	virtual void DumpInfo() = 0;
#endif

	virtual void AddMove(
        Unit                mover,
        MapPoint const &    oldPos,
        MapPoint const &    newPos,
        sint32              numRevealed,
        UnitActor **        revealedActors,
        sint32              numRest,
        UnitActor **        restOfStack,
        sint32              soundID
    ) = 0;

	virtual void AddTeleport(
        Unit                top,
        MapPoint const &    oldPos,
        MapPoint const &    newPos,
        sint32              numRevealed,
        UnitActor **        revealedActors,
		sint32              arraySize,
        UnitActor **        moveActors
    ) = 0;

	virtual void AddAttack(Unit attacker, Unit attacked) = 0;
	virtual void AddAttackPos(Unit attacker, MapPoint const & pos) = 0;
	virtual void AddSpecialAttack(Unit attacker, Unit attacked, SPECATTACK attack) = 0;
	virtual void AddDeath(UnitActor *dead, const MapPoint &deadPos, sint32 deadSoundID) = 0;
	virtual void AddProjectileAttack(
		Unit shooting,
		Unit target,
		SpriteState *projectile_state,
		SpriteState *projectileEnd_state,
		sint32 projectile_Path
	) = 0;
	virtual void AddSpecialEffect(MapPoint &pos, sint32 spriteID, sint32 soundID) = 0;
	virtual void AddMorphUnit(UnitActor *morphingActor, SpriteState *ss, sint32 type,  Unit id) = 0;
	virtual void AddHide(Unit hider) = 0;
	virtual void AddShow(Unit hider) = 0;
	virtual void AddWork(Unit worker) = 0;
	virtual void AddFastKill(UnitActor *dead) = 0;
	virtual void AddRemoveVision(const MapPoint &pos, double range) = 0;
	virtual void AddAddVision(const MapPoint &pos, double range) = 0;
	virtual void AddSetVisibility(UnitActor *actor, uint32 visibility) = 0;
	virtual void AddSetOwner(UnitActor *actor, sint32 owner) = 0;
	virtual void AddSetVisionRange(UnitActor *actor, double range) = 0;
	virtual void AddCombatFlash(MapPoint const & pos) = 0;
	virtual void AddCopyVision() = 0;
	virtual void AddCenterMap(const MapPoint &pos) = 0;
	virtual void AddSelectUnit(uint32 flags) = 0;
	virtual void AddEndTurn() = 0;
	virtual void AddBattle(Battle *battle) = 0;
	virtual void AddPlaySound(sint32 soundID, MapPoint const & pos) = 0;
	virtual void AddGameSound(GAMESOUNDS sound) = 0;
	virtual void AddPlayWonderMovie(sint32 which) = 0;
	virtual void AddPlayVictoryMovie(GAME_OVER reason, BOOL previouslyWon, BOOL previouslyLost) = 0;
	virtual void AddMessage(const Message &message) = 0;
	// Please check the implementation of 'terminate faceoff' (especially ActiveUnitRemove) when this is activated
	// virtual void AddFaceoff(Unit &attacker, Unit &defender) = 0;
	virtual void AddTerminateFaceoff(Unit &faceroffer) = 0;
	virtual void AddTerminateSound(Unit &unit) = 0;
	virtual void AddInvokeThroneRoom() = 0;
	virtual void AddInvokeResearchAdvance(MBCHAR *text) = 0;
	virtual void AddBeginScheduler(sint32 player) = 0;

	// Anim
	virtual uint32 GetMasterCurTime() = 0;

	// ArmyData
	virtual BOOL TileWillBeCompletelyVisible(sint32 x, sint32 y) = 0;

	// battleviewwindow
	virtual void UpdateTimingClock() = 0;

	// DirectorEvent
	virtual Sequence *GetHoldSchedulerSequence() = 0;
	virtual void SetHoldSchedulerSequence(Sequence *seq) = 0;

	// GameEventManager
	virtual void IncrementPendingGameActions() = 0;
	virtual void DecrementPendingGameActions() = 0;

	// TiledMap
	virtual UnitActor *GetClickedActiveUnit(aui_MouseEvent *mouse) = 0;

	// TradePool
	virtual void TradeActorCreate(TradeRoute newRoute) = 0;
	virtual void TradeActorDestroy(TradeRoute routeToDestroy) = 0;

	// Unit
	virtual void FastKill(UnitActor *actor) = 0;

	// UnitActor && DirectorEvent
	virtual void ActionFinished(Sequence *seq) = 0;

	// UnitActor
	#ifdef _DEBUG
		virtual void DumpSequence(Sequence *seq) = 0;
	#endif

	// UnseenCell
	virtual void ActiveUnitRemove(UnitActor *unitActor) = 0;
};

extern Director *g_director;

#endif
