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

enum DEACTION_TYPE {
	DEA_BEGIN_SCHEDULER,
	DEA_BATTLE_WINDOW,
	DEA_ADVANCE_WINDOW,
	DEA_WONDER_MOVIE,
	DEA_VICTORY_MOVIE
};

class Director
{
public:
	static Director* CreateDirector();
	static void ReloadAllSprites();

	virtual ~Director() {}

	virtual void Clear() = 0;

	virtual void Process() = 0;
	virtual void PauseDirector(bool pause) = 0;
	virtual void Draw(const RECT & paintRect, sint32 layer) = 0;
	virtual void OffsetActors(sint32 deltaX, sint32 deltaY) = 0;

	virtual void ExternalActionFinished(DEACTION_TYPE externalActionType) = 0;

	virtual void NextPlayer() = 0;

	virtual void CatchUp() = 0;
	virtual bool CaughtUp() = 0;

#ifdef _DEBUG
	virtual void DumpInfo() = 0;
#endif

	virtual void AddMove(
		const Unit     &mover,
		MapPoint const &startPos,
		MapPoint const &endPos,
		sint32         numberOfMoveActors,
		UnitActor      **moveActors,
		sint32         soundID
    ) = 0;

	virtual void AddTeleport(
		const Unit     &mover,
		MapPoint const &startPos,
		MapPoint const &endPos,
		sint32         numberOfMoveActors,
		UnitActor      **moveActors
    ) = 0;

	virtual void AddSpaceLaunch(const Unit & launcher, const MapPoint & destinationPos) = 0;
	virtual void AddAttack(const Unit &attacker, const Unit &attacked) = 0;
	virtual void AddAttackPos(const Unit &attacker, const MapPoint &pos) = 0;
	virtual void AddSpecialAttack(const Unit &attacker, const Unit &attacked, SPECATTACK attack) = 0;
	virtual void AddDeath(UnitActor *dead, const MapPoint &deadPos, sint32 deadSoundID, PLAYER_INDEX killedBy) = 0;
	virtual void AddProjectileAttack(
		const Unit  &shooting,
		const Unit  &target,
		SpriteState *projectileState,
		SpriteState *projectileEndState,
		sint32      projectilePath
	) = 0;
	virtual void AddSpecialEffect(const MapPoint &pos, sint32 spriteID, sint32 soundID) = 0;
	virtual void AddMorphUnit(UnitActor *morphingActor, SpriteState *spriteState, sint32 type,  const Unit &id) = 0;
	virtual void AddHide(const Unit &hider) = 0;
	virtual void AddShow(const Unit &hider) = 0;
	virtual void AddWork(const Unit &worker) = 0;
	virtual void AddFastKill(UnitActor *dead) = 0;
	virtual void AddRemoveVision(const MapPoint &pos, double range) = 0;
	virtual void AddAddVision(const MapPoint &pos, double range) = 0;
	virtual void AddSetVisibility(UnitActor *actor, uint32 visibility) = 0;
	virtual void AddSetOwner(UnitActor *actor, sint32 owner) = 0;
	virtual void AddCombatFlash(const MapPoint &pos) = 0;
	virtual void AddCopyVision() = 0;
	virtual void AddCenterMap(const MapPoint &pos) = 0;
	virtual void AddSelectUnit(uint32 flags) = 0;
	virtual void AddEndTurn() = 0;
	virtual void AddBattle(Battle *battle) = 0;
	virtual void AddPlaySound(sint32 soundID, const MapPoint &pos) = 0;
	virtual void AddGameSound(GAMESOUNDS sound) = 0;
	virtual void AddPlayWonderMovie(sint32 which) = 0;
	virtual void AddPlayVictoryMovie(GAME_OVER reason, bool previouslyWon, bool previouslyLost) = 0;
	virtual void AddMessage(const Message &message) = 0;
	virtual void AddFaceoff(const Unit &attacker, const Unit &defender) = 0;
	virtual void AddTerminateFaceoff(const Unit &faceroffer) = 0;
	virtual void AddTerminateSound(const Unit &unit) = 0;
	virtual void AddInvokeThroneRoom() = 0;
	virtual void AddInvokeResearchAdvance(MBCHAR *text) = 0;
	virtual void AddBeginScheduler(sint32 player) = 0;

	// Animations
	virtual void AddTradeRouteAnimation(const TradeRoute &tradeRoute) = 0;
	virtual void RemoveTradeRouteAnimation(const TradeRoute &tradeRoute) = 0;

	// Anim
	virtual uint32 GetMasterCurTime() = 0;
	// battleviewwindow
	virtual void UpdateTimingClock() = 0;

	// ArmyData
	virtual bool TileWillBeCompletelyVisible(sint32 x, sint32 y) = 0;
};

extern Director *g_director;

#endif
