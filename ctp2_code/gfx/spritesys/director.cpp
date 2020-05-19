//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
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
// - Prevented game freeze when an item gets deleted while the program is
//   waiting for it to finish.
// - Prevented messages appearing out of turn in hoseat mode
// - PFT 29 mar 05, show # turns until city next grows a pop
// - Initialized local variables. (Sep 9th 2005 Martin Gühmann)
// - Outcommented some unreachable code. (Sep 9th 2005 Martin Gühmann)
// - Fixed memory leaks.
// - Removed some unused items
// - Changed occurances of UnitRecord::GetMaxHP to
//   UnitData::CalculateTotalHP. (Aug 3rd 2009 Maq)
//
//----------------------------------------------------------------------------

// TODO: Make moveActor protected
// TODO: verify that all EffectActors are deleted
// TODO: verify if Effect-actors need to be refactored
// TODO: check if stand-by units are not drawn outside position when center map has been called

#include "c3.h"
#include "director.h"

#include "Action.h"
#include "Actor.h"
#include "Anim.h"
#include "Army.h"
#include "background.h"
#include "backgroundwin.h"
#include "battleviewwindow.h"       // g_battleViewWindow
#include "c3errors.h"
#include "c3ui.h"
#include "Cell.h"
#include "cellunitlist.h"
#include "citydata.h"
#include "colorset.h"
#include "cursormanager.h"
#include "debugmemory.h"
#include "dynarr.h"
#include "EffectActor.h"
#include "GameEventManager.h"
#include "gamesounds.h"
#include "Globals.h"
#include "maputils.h"
#include "MessagePool.h"
#include "messagewin.h"
#include "net_info.h"
#include "network.h"
#include "pixelutils.h"
#include "player.h"                 // g_player
#include "primitives.h"
#include "profileDB.h"              // g_theProfileDB
#include "radarmap.h"               // g_radarMap
#include "sci_advancescreen.h"
#include "screenutils.h"
#include "SelItem.h"                // g_selected_item
#include "soundmanager.h"           // g_soundManager
#include "SpriteRecord.h"
#include "SpriteState.h"
#include "spriteutils.h"
#include "tech_wllist.h"
#include "tiledmap.h"               // g_tiledMap
#include "tileutils.h"
#include "TradeActor.h"
#include "TurnCnt.h"                // g_turn
#include "Unit.h"
#include "UnitActor.h"
#include "UnitData.h"
#include "UnitDynArr.h"
#include "UnitPool.h"
#include "UnitRecord.h"
#include "victorymoviewin.h"
#include "wondermoviewin.h"
#include "World.h"                  // g_theWorld

#include <set>
#include <unordered_set>

extern Background		*g_background;
extern C3UI				*g_c3ui;
extern MessagePool		*g_theMessagePool;

#ifdef _PLAYTEST
extern BOOL g_doingFastRounds;
#endif

#define k_MAXFRAMERATE	20
#define k_DEFAULT_FPS			10
#define k_ELAPSED_CEILING		100

enum DQACTION_TYPE {
	DQACTION_MOVE,
	DQACTION_MOVEPROJECTILE,
	DQACTION_SPECEFFECT,
	DQACTION_ATTACK,
	DQACTION_ATTACKPOS,
	DQACTION_SPECATTACK,
	DQACTION_DEATH,
	DQACTION_MORPH,
	DQACTION_HIDE,
	DQACTION_SHOW,
	DQACTION_WORK,
	DQACTION_FASTKILL,
	DQACTION_ADDVISION,
	DQACTION_REMOVEVISION,
	DQACTION_SETOWNER,
	DQACTION_SETVISIBILITY,
	DQACTION_SETVISIONRANGE,
	DQACTION_COMBATFLASH,
	DQACTION_TELEPORT,
	DQACTION_COPYVISION,
	DQACTION_CENTERMAP,
	DQACTION_SELECTUNIT,
	DQACTION_ENDTURN,
	DQACTION_BATTLE,
	DQACTION_PLAYSOUND,
	DQACTION_PLAYWONDERMOVIE,
	DQACTION_PLAYVICTORYMOVIE,
	DQACTION_MESSAGE,
	DQACTION_FACEOFF,
	DQACTION_TERMINATE_FACEOFF,
	DQACTION_TERMINATE_SOUND,
	DQACTION_INVOKE_THRONE_ROOM,
	DQACTION_INVOKE_RESEARCH_ADVANCE,
	DQACTION_BEGIN_SCHEDULER,
	DQACTION_MAX
};

class DQActiveLoopingSound {
public:
	DQActiveLoopingSound(uint32 unitID) :
	m_unitID	(unitID)
	{}
	~DQActiveLoopingSound()
	{
		if (g_soundManager) {
			g_soundManager->TerminateLoopingSound(SOUNDTYPE_SFX, m_unitID);
		}
	}

	uint32 GetUnitID() { return m_unitID; }
private:
	uint32	m_unitID;
};

class DQAction {
public:
	DQAction() {}
	virtual ~DQAction() {}
	virtual DQACTION_TYPE GetType() = 0;
	virtual bool IsTypeActive() { return false; }

	virtual void ProcessLoopingSound(DQActiveLoopingSound* &activeLoopingSound) = 0;
	virtual void Execute() = 0;
	virtual bool SkipProcess() = 0;
	virtual void PrepareProcess() = 0;
	virtual void Process() = 0;
	virtual bool IsFinished() = 0;
	virtual void Finalize() = 0;

	virtual void Dump() = 0;
protected:
	static bool TileIsVisibleToPlayer(const MapPoint &position)
	{
		#if defined(_PLAYTEST)
			if (g_doingFastRounds) return false;
		#endif

		return g_tiledMap && g_tiledMap->GetLocalVision()->IsVisible(position);
	}
};

class DQActionImmediate : public DQAction {
public:
	DQActionImmediate()
			: DQAction()
	{}
	virtual ~DQActionImmediate() {}
	virtual bool IsTypeImmediate() { return true; }

	virtual void ProcessLoopingSound(DQActiveLoopingSound* &activeLoopingSound) {}
	virtual void Execute() = 0;
	virtual bool SkipProcess() { return true; }
	virtual void PrepareProcess() {}
	virtual void Process() {}
	virtual bool IsFinished() { return true; }
	virtual void Finalize() {}
};

class DQActionActive : public DQAction
{
public:
	DQActionActive(sint8 owner)
			: DQAction(),
			  m_owner		(owner),
			  m_round		(0)
	{
		if (g_turn) {
			m_round = static_cast<uint16>(g_turn->GetRound());
		}
	}
	virtual ~DQActionActive() {}
	virtual bool IsTypeActive() { return true; }

	virtual void ProcessLoopingSound(DQActiveLoopingSound* &activeLoopingSound)
	{
		delete activeLoopingSound;
		activeLoopingSound = NULL;
	}

	virtual void Execute() {}

	virtual bool SkipProcess()
	{
		return (SkipPreviousRounds() || SkipEnemyMoves() || SkipRobotUnitAnimations() || DoSkipProcess());
	}

	virtual void PrepareProcess() = 0;

	virtual void Process()
	{
		for (auto it = m_activeActors.begin(); it != m_activeActors.end(); ) {
			UnitActor *actor = *it;
			actor->Process();
			if (actor->IsActionFinished()) {
				actor->SetActive(false);
				it = m_activeActors.erase(it);
			} else {
				++it;
			}
		}
	}

	virtual bool IsFinished()
	{
		return m_activeActors.empty();
	}

	virtual void Finalize() {}

	void Offset(sint32 deltaX, sint32 deltaY)
	{
		for (const auto &activeActor : m_activeActors) {
			activeActor->SetX(activeActor->GetX() + deltaX);
			activeActor->SetY(activeActor->GetY() + deltaY);
		}
	}

	void Draw(RECT *paintRect, sint32 layer)
	{
		for (const auto &activeActor : m_activeActors) {
			DrawUnitActor(activeActor, paintRect);
		}
	}

	void FastKill(UnitActor *actor)
	{
		actor->SetActive(false);
		m_activeActors.erase(actor);
	}

protected:
	virtual bool DoSkipProcess() { return false; }

	void AddActiveActor(UnitActor *actor) {
		m_activeActors.insert(actor);
		actor->SetActive(true);
	}
private:
	static void DrawUnitActor(UnitActor *actor, RECT *paintRect)
	{
		if (actor->GetUnitVisibility() & (1 << g_selected_item->GetVisiblePlayer())) {
			MapPoint pos = actor->GetPos();
			sint32	tileX;
			maputils_MapX2TileX(pos.x, pos.y, &tileX);

			if (maputils_TilePointInTileRect(tileX, pos.y, paintRect))
			{
				g_tiledMap->PaintUnitActor(actor);
			}
		}
	}

	bool SkipPreviousRounds()
	{
		return (m_round < g_turn->GetRound() - 1);
	}
	bool SkipEnemyMoves()
	{
		return (!g_theProfileDB->IsEnemyMoves()
				&&  m_owner != -1
				&&  m_owner != g_selected_item->GetVisiblePlayer());
	}
	bool SkipRobotUnitAnimations()
	{
		return (!g_theProfileDB->IsUnitAnim()
				&& m_owner != -1
				&&  g_player[m_owner] != NULL
				&& g_player[m_owner]->IsRobot());
	}

	sint8							m_owner;
	uint16							m_round;
	std::unordered_set<UnitActor*>	m_activeActors;
};

class DQActionExternal : public DQAction
{
public:
	DQActionExternal()
			: DQAction(),
			m_finished	(false)
	{}
	virtual ~DQActionExternal() {}

	virtual void ProcessLoopingSound(DQActiveLoopingSound* &activeLoopingSound)
	{
		delete activeLoopingSound;
		activeLoopingSound = NULL;
	}

	virtual bool SkipProcess() { return false; }
	virtual void PrepareProcess() {}
	virtual void Process() {}
	virtual bool IsFinished() { return m_finished; }
	virtual void Finalize() {}

	void SetFinished() { m_finished = true; }
protected:
	bool	m_finished;
};

class DirectorImpl : public Director {
public:
	DirectorImpl(void);
	virtual ~DirectorImpl(void);
	static DirectorImpl *Instance() { Assert(m_instance); return m_instance; }

	virtual void ReloadAllSprites();
	virtual void NotifyResync();

	virtual void Process();
	virtual void PauseDirector(BOOL pause);
	virtual void Draw(RECT *paintRect, sint32 layer);
	virtual void OffsetActors(sint32 deltaX, sint32 deltaY);

	virtual void ExternalActionFinished(DEACTION_TYPE externalActionType);

	virtual void NextPlayer();

	virtual void CatchUp();
	virtual bool CaughtUp();

#ifdef _DEBUG
	virtual void DumpInfo();
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
	);
	virtual void AddTeleport(
		Unit                top,
		MapPoint const &    oldPos,
		MapPoint const &    newPos,
		sint32              numRevealed,
		UnitActor **        revealedActors,
		sint32              arraySize,
		UnitActor **        moveActors
	);
	virtual void AddAttack(Unit attacker, Unit attacked);
	virtual void AddAttackPos(Unit attacker, MapPoint const & pos);
	virtual void AddSpecialAttack(Unit attacker, Unit attacked, SPECATTACK attack);
	virtual void AddDeath(UnitActor *dead, const MapPoint &deadPos, sint32 deadSoundID);
	virtual void AddProjectileAttack(
		Unit shooting,
		Unit target,
		SpriteState *projectile_state,
		SpriteState *projectileEnd_state,
		sint32 projectile_Path
	);
	virtual void AddSpecialEffect(MapPoint &pos, sint32 spriteID, sint32 soundID);
	virtual void AddMorphUnit(UnitActor *morphingActor, SpriteState *ss, sint32 type,  Unit id);
	virtual void AddHide(Unit hider);
	virtual void AddShow(Unit hider);
	virtual void AddWork(Unit worker);
	virtual void AddFastKill(UnitActor *dead);
	virtual void AddRemoveVision(const MapPoint &pos, double range);
	virtual void AddAddVision(const MapPoint &pos, double range);
	virtual void AddSetVisibility(UnitActor *actor, uint32 visibility);
	virtual void AddSetOwner(UnitActor *actor, sint32 owner);
	virtual void AddSetVisionRange(UnitActor *actor, double range);
	virtual void AddCombatFlash(MapPoint const & pos);
	virtual void AddCopyVision();
	virtual void AddCenterMap(const MapPoint &pos);
	virtual void AddSelectUnit(uint32 flags);
	virtual void AddEndTurn();
	virtual void AddBattle(Battle *battle);
	virtual void AddPlaySound(sint32 soundID, MapPoint const & pos);
	virtual void AddGameSound(GAMESOUNDS sound);
	virtual void AddPlayWonderMovie(sint32 which);
	virtual void AddPlayVictoryMovie(GAME_OVER reason, BOOL previouslyWon, BOOL previouslyLost);
	virtual void AddMessage(const Message &message);
	virtual void AddFaceoff(Unit &attacker, Unit &defender);
	virtual void AddTerminateFaceoff(Unit &faceroffer);
	virtual void AddTerminateSound(Unit &unit);
	virtual void AddInvokeThroneRoom();
	virtual void AddInvokeResearchAdvance(MBCHAR *text);
	virtual void AddBeginScheduler(sint32 player);

	// Anim
	virtual uint32 GetMasterCurTime() { return m_masterCurTime; }

	// ArmyData
	virtual BOOL TileWillBeCompletelyVisible(sint32 x, sint32 y);

	// battleviewwindow
	virtual void UpdateTimingClock();

	// GameEventManager
	virtual void IncrementPendingGameActions();
	virtual void DecrementPendingGameActions();

	// TradePool
	virtual void TradeActorCreate(TradeRoute newRoute);
	virtual void TradeActorDestroy(TradeRoute routeToDestroy);

	// Unit && UnseenCell
	virtual void FastKill(UnitActor *actor);

	// Used locally
	void		EndTurn();
	bool		HandleActor(bool immediate, UnitActor* actor, bool actionEnabled);
	void		ActiveEffectAdd(EffectActor *effectActor);
	void		CenterMap(const MapPoint &pos);

private:
	static void	FinalizeAction(DQAction* &action);

	bool    CanStartNextAction() { return m_actionQueue->GetCount() > 0 && !m_currentAction; }
	void	HandleNextAction();

	void	DrawStandbyUnits(RECT *paintRect, sint32 layer);
	void	DrawActiveUnits(RECT *paintRect, sint32 layer);
	void	DrawActiveEffects(RECT *paintRect, sint32 layer);
	void	DrawTradeRouteAnimations(RECT *paintRect, sint32 layer);

	void	DrawUnitActor(RECT *paintRect, UnitActor *actor, bool standby);

	void	OffsetStandbyUnits(sint32 deltaX, sint32 deltaY);
	void	OffsetActiveUnits(sint32 deltaX, sint32 deltaY);
	void	OffsetActiveEffects(sint32 deltaX, sint32 deltaY);
	void	OffsetTradeRouteAnimations(sint32 deltaX, sint32 deltaY);

	void	ProcessStandbyUnits();
	void	ProcessCurrentAction();
	uint32	ProcessActiveEffects();
	void	ProcessTradeRouteAnimations();

	void	FinalizeActor(UnitActor *unitActor);
	void	FastKill(EffectActor *actor);
	uint32	KillAllActiveEffects();

	void	UpdateStandbyUnits();

#ifdef _DEBUG
	void	DumpAction(DQAction *action);
#endif

	// Unused
	void	ActiveEffectRemove(EffectActor *effectActor);

	static DirectorImpl			*m_instance;
	DQAction					*m_currentAction;
	DQActiveLoopingSound		*m_activeLoopingSound;
	tech_WLList<EffectActor *>	*m_activeEffectList;
	tech_WLList<TradeActor *>	*m_tradeActorList;

	static const int            k_TIME_LOG_SIZE = 30;
	uint32						m_masterCurTime;
	sint32						m_lastTickCount;
	sint32						m_timeLog[k_TIME_LOG_SIZE];
	sint32						m_timeLogIndex;
	sint32						m_averageElapsed;
	sint32						m_averageFPS;

	bool						m_paused;
	bool						m_processingActiveEffects;

	PointerList<DQAction>		*m_actionQueue;

	std::set<UnitActor *>			m_standbyActors;
	std::unordered_set<UnitActor *>	m_dyingActors;
	PointerList<DQAction>::Walker	*m_actionWalker;

	sint32						m_pendingGameActions;
	bool						m_endTurnRequested;
};

Director* Director::CreateDirector() {
	return new DirectorImpl();
}

class DQActionTeleport : public DQActionImmediate
{
public:
	DQActionTeleport(
			UnitActor		*moveActor,
			const MapPoint	&startPosition,
			const MapPoint	&endPosition,
			sint32			numberOfMoveActors,
			UnitActor		**moveActors,
			sint32			numberOfRevealedActors,
			UnitActor		**revealedActors)
			: DQActionImmediate(),
			  moveActor					(moveActor),
			  startPosition				(startPosition),
			  endPosition				(endPosition),
			  numberOfMoveActors		(numberOfMoveActors),
			  moveActors				(moveActors),
			  numberOfRevealedActors	(numberOfRevealedActors),
			  revealedActors			(revealedActors)
	{}
	virtual ~DQActionTeleport() {}
	virtual DQACTION_TYPE GetType() { return DQACTION_TELEPORT; }

	virtual void Execute()
	{
		for (int i = 0; i < numberOfRevealedActors; i++)
		{
			UnitActor *tempActor = revealedActors[i];
			tempActor->SetVisSpecial(TRUE);
		}

		moveActor->PositionActor(endPosition);

		for (int i = 0; i < numberOfMoveActors; i++)
		{
			UnitActor *moveActor = moveActors[i];
			moveActor->PositionActor(endPosition);
		}
	}

	virtual void Dump()
	{
		DPRINTF(k_DBG_UI, ("Teleport\n"));
		DPRINTF(k_DBG_UI, ("  actor                  :%#x (%#.8lx)\n", moveActor->GetUnitID(), moveActor));
		DPRINTF(k_DBG_UI, ("  startPosition          :%d,%d\n", startPosition.x, startPosition.y));
		DPRINTF(k_DBG_UI, ("  endPosition            :%d,%d\n", endPosition.x, endPosition.y));
		DPRINTF(k_DBG_UI, ("  numberOfMoveActors          :%d\n", numberOfMoveActors));
		DPRINTF(k_DBG_UI, ("  moveActors                  :"));
		for (int i = 0; i < numberOfMoveActors; i++) {
			DPRINTF(k_DBG_UI, ("%#x (%#.8lx)  ", moveActors[i]->GetUnitID(), moveActors[i]));
		}
		DPRINTF(k_DBG_UI, ("\n"));
		DPRINTF(k_DBG_UI, ("  numberOfRevealedActors      :%d\n", numberOfRevealedActors));
		DPRINTF(k_DBG_UI, ("  revealedActors              :"));
		for (int i = 0; i < numberOfRevealedActors; i++) {
			DPRINTF(k_DBG_UI, ("%#x (%#.8lx)  ", revealedActors[i]->GetUnitID(), revealedActors[i]));
		}
		DPRINTF(k_DBG_UI, ("\n"));
	}

	// TODO: make protected
	UnitActor	*moveActor;

protected:
	MapPoint	startPosition;
	MapPoint	endPosition;
	sint32		numberOfMoveActors;
	UnitActor	**moveActors;
	sint32		numberOfRevealedActors;
	UnitActor	**revealedActors;
};

class DQActionMoveProjectile : public DQActionImmediate
{
public:
	DQActionMoveProjectile(SpriteState *projectileEndState, const MapPoint &startPosition, const MapPoint &endPosition)
			: DQActionImmediate(),
			  projectileEndState	(projectileEndState),
			  startPosition			(startPosition),
			  endPosition			(endPosition)
	{}
	virtual ~DQActionMoveProjectile() {}
	virtual DQACTION_TYPE GetType() { return DQACTION_MOVEPROJECTILE; }

	virtual void Execute()
	{
		if (TileIsVisibleToPlayer(startPosition))
		{
			EffectActor *projectileEnd = new EffectActor(projectileEndState, endPosition);
			sint32 actionType = EFFECTACTION_PLAY;
			Anim *anim = projectileEnd->CreateAnim(EFFECTACTION_PLAY);
			if (!anim) {
				anim = projectileEnd->CreateAnim(EFFECTACTION_FLASH);
				actionType = EFFECTACTION_FLASH;
				Assert(anim);
			}

			if (anim) {
				Action *action = new Action(actionType, ACTIONEND_PATHEND);
				action->SetAnim(anim);
				projectileEnd->AddAction(action);
				DirectorImpl::Instance()->ActiveEffectAdd(projectileEnd);
			} else {
				delete projectileEnd;
			}
		}
	}

	virtual void Dump()
	{
		DPRINTF(k_DBG_UI, ("Move Projectile\n"));
		DPRINTF(k_DBG_UI, ("  startPosition      :%d,%d\n", startPosition.x, startPosition.y));
		DPRINTF(k_DBG_UI, ("  endPosition        :%d,%d\n", endPosition.x, endPosition.y));
		DPRINTF(k_DBG_UI, ("  projectileEndState :%#.8lx\n", projectileEndState));
	}
protected:
	SpriteState	*projectileEndState;
	MapPoint	startPosition;
	MapPoint	endPosition;
};

class DQActionMorph : public DQActionImmediate
{
public:
	DQActionMorph(UnitActor *morphingActor, SpriteState *spriteState, sint32 type, Unit &id)
			: DQActionImmediate(),
			  morphingActor	(morphingActor),
			  spriteState		(spriteState),
			  type			(type),
			  id				(id)
	{}
	virtual ~DQActionMorph() {}
	virtual DQACTION_TYPE GetType() { return DQACTION_MORPH; }

	virtual void Execute()
	{
		morphingActor->ChangeType(spriteState, type, id, FALSE);
	}

	virtual void Dump()
	{
		DPRINTF(k_DBG_UI, ("Morph\n"));
		DPRINTF(k_DBG_UI, ("  morphingActor      :%#x (%#.8lx)\n", morphingActor->GetUnitID(), morphingActor));
		DPRINTF(k_DBG_UI, ("  spriteState        :%#.8lx (%d)\n", spriteState, spriteState->GetIndex()));
		DPRINTF(k_DBG_UI, ("  type               :%d\n", type));
		DPRINTF(k_DBG_UI, ("  id                 :%#.8lx\n", id.m_id));
	}
protected:
	UnitActor	*morphingActor;
	SpriteState	*spriteState;
	sint32		type;
	Unit		id;
};

class DQActionHideShow : public DQActionImmediate
{
public:
	DQActionHideShow(UnitActor *hidingActor)
			: DQActionImmediate(),
			  hidingActor	(hidingActor)
	{}
	virtual ~DQActionHideShow() {}

protected:
	UnitActor	*hidingActor;
};

class DQActionHide : public DQActionHideShow
{
public:
	DQActionHide(UnitActor *hidingActor)
			: DQActionHideShow(hidingActor)
	{}
	virtual ~DQActionHide() {}
	virtual DQACTION_TYPE GetType() { return DQACTION_HIDE; }

	virtual void Execute()
	{
		hidingActor->Hide();
	}

	virtual void Dump()
	{
		DPRINTF(k_DBG_UI, ("Hide\n"));
		DPRINTF(k_DBG_UI, ("  hidingActor        :%#x (%#.8lx)\n", hidingActor->GetUnitID(), hidingActor));
	}
};

class DQActionShow : public DQActionHideShow
{
public:
	DQActionShow(UnitActor *hidingActor, const MapPoint &hidingPosition)
			: DQActionHideShow(hidingActor),
			  hidingPosition	(hidingPosition)
	{}
	virtual ~DQActionShow() {}
	virtual DQACTION_TYPE GetType() { return DQACTION_SHOW; }

	virtual void Execute()
	{
		hidingActor->PositionActor(hidingPosition);
		hidingActor->Show();
	}

	virtual void Dump() {
		DPRINTF(k_DBG_UI, ("Show\n"));
		DPRINTF(k_DBG_UI, ("  hidingActor        :%#x (%#.8lx)\n", hidingActor->GetUnitID(), hidingActor));
		DPRINTF(k_DBG_UI, ("  hidingPosition     :%d,%d\n", hidingPosition.x, hidingPosition.y));
	}
protected:
	MapPoint hidingPosition;
};

class DQActionFastKill : public DQActionImmediate
{
public:
	DQActionFastKill(UnitActor *dead)
			: DQActionImmediate(),
			  dead	(dead)
	{}
	virtual ~DQActionFastKill() {}
	virtual DQACTION_TYPE GetType() { return DQACTION_FASTKILL; }

	virtual void Execute()
	{
		DirectorImpl::Instance()->FastKill(dead);
	}

	virtual void Dump()
	{
		DPRINTF(k_DBG_UI, ("Fast Kill\n"));
		DPRINTF(k_DBG_UI, ("  dead               :%#x (%#.8lx)\n", dead->GetUnitID(), dead));
	}
private:
	UnitActor	*dead;
};

class DQActionVision : public DQActionImmediate
{
public:
	DQActionVision(const MapPoint &visionPosition, double visionRange)
			: DQActionImmediate(),
			  visionPosition	(visionPosition),
			  visionRange		(visionRange)
	{}
	virtual ~DQActionVision() {}

protected:
	MapPoint	visionPosition;
	double		visionRange;
};

class DQActionRemoveVision : public DQActionVision
{
public:
	DQActionRemoveVision(const MapPoint &visionPosition, double visionRange)
			: DQActionVision(visionPosition, visionRange)
	{}
	virtual ~DQActionRemoveVision() {}
	virtual DQACTION_TYPE GetType() { return DQACTION_REMOVEVISION; }

	virtual void Execute()
	{
		if(g_tiledMap) {
			g_tiledMap->RemoveVisible(visionPosition, visionRange);
		}
	}

	virtual void Dump()
	{
		DPRINTF(k_DBG_UI, ("Remove Vision\n"));
		DPRINTF(k_DBG_UI, ("  visionPosition     :%d,%d\n", visionPosition.x, visionPosition.y));
		DPRINTF(k_DBG_UI, ("  visionRange        :%#.2f\n", visionRange));
	}
};

class DQActionAddVision : public DQActionVision {
public:
	DQActionAddVision(const MapPoint &visionPosition, double visionRange)
			: DQActionVision(visionPosition, visionRange)
	{}
	virtual ~DQActionAddVision() {}
	virtual DQACTION_TYPE GetType() { return DQACTION_ADDVISION; }

	virtual void Execute()
	{
		if(g_tiledMap) {
			g_tiledMap->AddVisible(visionPosition, visionRange);
		}
	}

	virtual void Dump()
	{
		DPRINTF(k_DBG_UI, ("Add Vision\n"));
		DPRINTF(k_DBG_UI, ("  visionPosition     :%d,%d\n", visionPosition.x, visionPosition.y));
		DPRINTF(k_DBG_UI, ("  visionRange        :%#.2f\n", visionRange));
	}
};

class DQActionSetVisibility : public DQActionImmediate
{
public:
	DQActionSetVisibility(UnitActor *actor, uint32 flag)
			: DQActionImmediate(),
			  actor	(actor),
			  flag	(flag)
	{}
	virtual ~DQActionSetVisibility() {}
	virtual DQACTION_TYPE GetType() { return DQACTION_SETVISIBILITY; }

	virtual void Execute()
	{
		actor->SetUnitVisibility(flag);
	}

	virtual void Dump()
	{
		DPRINTF(k_DBG_UI, ("Set Visibility\n"));
		DPRINTF(k_DBG_UI, ("  actor              :%#x (%#.8lx)\n", actor->GetUnitID(), actor));
		DPRINTF(k_DBG_UI, ("  flag               :%%#.8lx\n", flag));
	}
protected:
	UnitActor	*actor;
	uint32		flag;
};

class DQActionSetOwner : public DQActionImmediate
{
public:
	DQActionSetOwner(UnitActor *setOwnerActor, sint32 owner)
			: DQActionImmediate(),
			  setOwnerActor	(setOwnerActor),
			  owner			(owner)
	{}
	virtual ~DQActionSetOwner() {}
	virtual DQACTION_TYPE GetType() { return DQACTION_SETOWNER; }

	virtual void Execute()
	{
		setOwnerActor->SetPlayerNum(owner);
	}

	virtual void Dump()
	{
		DPRINTF(k_DBG_UI, ("Set Owner\n"));
		DPRINTF(k_DBG_UI, ("  setOwnerActor      :%#x (%#.8lx)\n", setOwnerActor->GetUnitID(), setOwnerActor));
		DPRINTF(k_DBG_UI, ("  owner              :%d\n", owner));
	}
protected:
	UnitActor	*setOwnerActor;
	sint32		owner;
};

class DQActionSetVisionRange : public DQActionImmediate
{
public:
	DQActionSetVisionRange(UnitActor *actor, double range)
			: DQActionImmediate(),
			  actor	(actor),
			  range	(range)
	{}
	virtual ~DQActionSetVisionRange() {}
	virtual DQACTION_TYPE GetType() { return DQACTION_SETVISIONRANGE; }

	virtual void Execute()
	{
		actor->SetUnitVisionRange(range);
	}

	virtual void Dump()
	{
		DPRINTF(k_DBG_UI, ("Set Vision Range\n"));
		DPRINTF(k_DBG_UI, ("  actor     :%#x (%#.8lx)\n", actor->GetUnitID(), actor));
		DPRINTF(k_DBG_UI, ("  range     :%#.2f\n", range));
	}
protected:
	UnitActor	*actor;
	double		range;
};

class DQActionCenterMap : public DQActionImmediate
{
public:
	DQActionCenterMap(const MapPoint &centerMapPosition)
			: DQActionImmediate(),
			  centerMapPosition	(centerMapPosition)
	{}
	virtual ~DQActionCenterMap() {}
	virtual DQACTION_TYPE GetType() { return DQACTION_CENTERMAP; }

	void CalculateCenteredMap(RECT *viewRect)
	{
		g_radarMap->ComputeCenteredMap(centerMapPosition, viewRect);
	}

	virtual void Execute()
	{
		if(!g_selected_item->GetIsPathing()) {
			DirectorImpl::Instance()->CenterMap(centerMapPosition);
		}
	}

	virtual void Dump()
	{
		DPRINTF(k_DBG_UI, ("Center Map\n"));
		DPRINTF(k_DBG_UI, ("  centerMapPosition:%d,%d\n", centerMapPosition.x, centerMapPosition.y));
	}
protected:
	MapPoint	centerMapPosition;
};

class DQActionCombatFlash : public DQActionImmediate
{
public:
	DQActionCombatFlash(const MapPoint &flashPosition)
			: DQActionImmediate(),
			  flashPosition	(flashPosition)
	{}
	virtual ~DQActionCombatFlash() {}
	virtual DQACTION_TYPE GetType() { return DQACTION_COMBATFLASH; }

	virtual void Execute()
	{
		SpriteState	*spriteState = new SpriteState(99);
		EffectActor	*flash = new EffectActor(spriteState, flashPosition);

		Anim *anim = flash->CreateAnim(EFFECTACTION_PLAY);
		if (!anim)
		{
			anim = flash->CreateAnim(EFFECTACTION_FLASH);
			Assert(anim);
		}

		if (anim)
		{
			Action * action = new Action(EFFECTACTION_FLASH, ACTIONEND_PATHEND);
			action->SetAnim(anim);
			flash->AddAction(action);
			DirectorImpl::Instance()->ActiveEffectAdd(flash);
		}
	}

	virtual void Dump()
	{
		DPRINTF(k_DBG_UI, ("Combat Flash\n"));
		DPRINTF(k_DBG_UI, ("  flashPosition     :%d,%d\n", flashPosition.x, flashPosition.y));
	}
protected:
	MapPoint	flashPosition;
};

class DQActionCopyVision : public DQActionImmediate
{
public:
	DQActionCopyVision()
			: DQActionImmediate()
	{}
	virtual ~DQActionCopyVision() {}
	virtual DQACTION_TYPE GetType() { return DQACTION_COPYVISION; }

	virtual void Execute()
	{
		g_tiledMap->CopyVision();
		g_radarMap->Update();
	}

	virtual void Dump()
	{
		DPRINTF(k_DBG_UI, ("Copy Vision\n"));
	}
};

class DQActionUnitSelection : public DQActionImmediate
{
public:
	DQActionUnitSelection(uint32 flags)
			: DQActionImmediate(),
			  flags	(flags)
	{}
	virtual ~DQActionUnitSelection() {}
	virtual DQACTION_TYPE GetType() { return DQACTION_SELECTUNIT; }

	virtual void Execute()
	{
		g_selected_item->DirectorUnitSelection(flags);
	}

	virtual void Dump()
	{
		DPRINTF(k_DBG_UI, ("Select Unit\n"));
		DPRINTF(k_DBG_UI, ("  flags     :%#.8lx\n", flags));
	}
protected:
	uint32	flags;
};

class DQActionEndTurn : public DQActionImmediate
{
public:
	DQActionEndTurn()
			: DQActionImmediate()
	{}
	virtual ~DQActionEndTurn() {}
	virtual DQACTION_TYPE GetType() { return DQACTION_ENDTURN; }

	virtual void Execute()
	{
		DirectorImpl::Instance()->EndTurn();
		g_gevManager->Pause();
		g_gevManager->AddEvent(GEV_INSERT_Tail, GEV_EndTurn,
							   GEA_Player, g_selected_item->GetCurPlayer(),
							   GEA_End);
		g_gevManager->Resume();
	}

	virtual void Dump()
	{
		DPRINTF(k_DBG_UI, ("End Turn\n"));
	}
};

class DQActionPlaySound : public DQActionImmediate
{
public:
	DQActionPlaySound(sint32 soundID, const MapPoint &position)
			: DQActionImmediate(),
			  soundID		(soundID),
			  position	(position)
	{}
	virtual ~DQActionPlaySound() {}
	virtual DQACTION_TYPE GetType() { return DQACTION_PLAYSOUND; }

	virtual void Execute()
	{
		if (g_soundManager) {
			g_soundManager->AddSound(SOUNDTYPE_SFX, 0, soundID, position.x, position.y);
		}
	}

	virtual void Dump()
	{
		DPRINTF(k_DBG_UI, ("Play Sound\n"));
		DPRINTF(k_DBG_UI, ("  soundID              :%d\n", soundID));
		DPRINTF(k_DBG_UI, ("  position             :%d,%d\n", position.x, position.y));
	}
protected:
	sint32		soundID;
	MapPoint	position;
};

class DQActionMessage : public DQActionImmediate
{
public:
	DQActionMessage(const Message &message)
			: DQActionImmediate(),
			  message	(message)
	{}
	virtual ~DQActionMessage() {}
	virtual DQACTION_TYPE GetType() { return DQACTION_MESSAGE; }

	virtual void Execute()
	{
		if (g_theMessagePool->IsValid(message)) {
			if (message.IsAlertBox()) {
				if (!messagewin_IsModalMessageDisplayed()) {
					messagewin_CreateModalMessage(message);
				}
			} else {
				if (!message.AccessData()->GetMessageWindow()) {
					messagewin_CreateMessage(message );
				}
				if (message.IsInstantMessage()
					// JJB added this to prevent instant messages showing
					// out of turn in hotseat games.
					// With the existing behaviour they would show immediately
					// which would often mean that they show on the wrong players
					// turn.
					&& g_selected_item->GetVisiblePlayer() == message.GetOwner())
				{
					message.Show();
				}
			}
		}
	}

	virtual void Dump()
	{
		DPRINTF(k_DBG_UI, ("Message\n"));
	}
protected:
	Message	message;
};

class DQActionTerminateFaceOff : public DQActionImmediate
{
public:
	DQActionTerminateFaceOff(UnitActor *faceOffer)
			: DQActionImmediate(),
			  faceOffer	(faceOffer)
	{}
	virtual ~DQActionTerminateFaceOff() {}
	virtual DQACTION_TYPE GetType() { return DQACTION_TERMINATE_FACEOFF; }

	virtual void Execute()
	{
		faceOffer->SetHealthPercent(-1.0);
		faceOffer->SetTempStackSize(0);

		// This is commented out as currently AddFaceoff is not called and thereby ActiveUnitAdd is not called.
		// DirectorImpl::Instance()->ActiveUnitRemove(faceOffer);
	}

	virtual void Dump()
	{
		DPRINTF(k_DBG_UI, ("Terminate Faceoff\n"));
		DPRINTF(k_DBG_UI, ("  faceroffer   :%#x (%#.8lx)\n", faceOffer->GetUnitID(), faceOffer));
	}
protected:
	UnitActor	*faceOffer;
};

class DQActionTerminateSound : public DQActionImmediate
{
public:
	DQActionTerminateSound(uint32 terminateSoundUnitID)
			: DQActionImmediate(),
			  terminateSoundUnitID	(terminateSoundUnitID)
	{}
	virtual ~DQActionTerminateSound() {}
	virtual DQACTION_TYPE GetType() { return DQACTION_TERMINATE_SOUND; }

	virtual void Execute()
	{
		if (g_soundManager) {
			g_soundManager->TerminateLoopingSound(SOUNDTYPE_SFX, terminateSoundUnitID);
		}
	}

	virtual void Dump()
	{
		DPRINTF(k_DBG_UI, ("Terminate Sound\n"));
		DPRINTF(k_DBG_UI, ("  terminate_sound_unit    :%#.8lx\n", terminateSoundUnitID));
	}
protected:
	uint32	terminateSoundUnitID;
};

class DQActionSpecialEffect : public DQActionImmediate
{
public:
	DQActionSpecialEffect(const MapPoint &position, sint32 spriteID, sint32 soundID)
			: DQActionImmediate(),
			  position	(position),
			  spriteID	(spriteID),
			  soundID		(soundID)
	{}
	virtual ~DQActionSpecialEffect() {}
	virtual DQACTION_TYPE GetType() { return DQACTION_SPECEFFECT; }

	virtual void Execute()
	{
		if (g_tiledMap->GetLocalVision()->IsVisible(position)) {
			SpriteState *spriteState = new SpriteState(spriteID);
			EffectActor *effectActor = new EffectActor(spriteState, position);

			Anim *anim = effectActor->CreateAnim(EFFECTACTION_PLAY);

			if (anim) {
				Action *action = new Action(EFFECTACTION_PLAY, ACTIONEND_PATHEND);
				action->SetAnim(anim);
				effectActor->AddAction(action);
				DirectorImpl::Instance()->ActiveEffectAdd(effectActor);

				if (g_soundManager) {
					g_soundManager->AddSound(SOUNDTYPE_SFX, 0, soundID, position.x, position.y);
				}
			}
		}
	}

	virtual void Dump()
	{
		DPRINTF(k_DBG_UI, ("Special Effect\n"));
		DPRINTF(k_DBG_UI, ("  position             :%d,%d\n", position.x, position.y));
		DPRINTF(k_DBG_UI, ("  spriteID             :%d\n", spriteID));
		DPRINTF(k_DBG_UI, ("  soundID              :%d\n", soundID));
	}
protected:
	MapPoint	position;
	sint32		spriteID;
	sint32		soundID;
};

class DQActionInvokeThroneRoom : public DQActionImmediate
{
public:
	DQActionInvokeThroneRoom()
			: DQActionImmediate()
	{}
	virtual ~DQActionInvokeThroneRoom() {}
	virtual DQACTION_TYPE GetType() { return DQACTION_INVOKE_THRONE_ROOM; }

	virtual void Execute() {}

	virtual void Dump()
	{
		DPRINTF(k_DBG_UI, ("Invoke Throne-room\n"));
	}
};

class DQActionMove : public DQActionActive
{
public:
	DQActionMove(
			sint8			owner,
			UnitActor		*moveActor,
			const MapPoint	&startPosition,
			const MapPoint	&endPosition,
			sint32			numberOfMoveActors,
			UnitActor		**moveActors,
			sint32			numberOfRevealedActors,
			UnitActor		**revealedActors,
			sint32			soundID)
			: DQActionActive(owner),
			  moveActor					(moveActor),
			  moveActorActive			(false),
			  startPosition				(startPosition),
			  endPosition				(endPosition),
			  numberOfMoveActors		(numberOfMoveActors),
			  moveActors				(moveActors),
			  numberOfRevealedActors	(numberOfRevealedActors),
			  revealedActors			(revealedActors),
			  soundID					(soundID)
	{}
	virtual ~DQActionMove()
	{
		delete [] moveActors;
	}
	virtual DQACTION_TYPE GetType() { return DQACTION_MOVE; }

	virtual void ProcessLoopingSound(DQActiveLoopingSound* &activeLoopingSound)
	{
		if (!activeLoopingSound || activeLoopingSound->GetUnitID() != moveActor->GetUnitID()) {
			delete activeLoopingSound;
			activeLoopingSound = new DQActiveLoopingSound(moveActor->GetUnitID());
		}
	}

	virtual void PrepareProcess()
	{
		Action *action = new Action();

		action->SetStartMapPoint(startPosition);
		action->SetEndMapPoint  (endPosition);
		action->CreatePath(startPosition.x, startPosition.y, endPosition.x, endPosition.y);

		moveActor->PositionActor(startPosition);

		uint32	maxActionCounter	= 1;
		sint32	speed				= g_theProfileDB->GetUnitSpeed();
		if (g_theProfileDB->IsUnitAnim())
			maxActionCounter = k_MAX_UNIT_MOVEMENT_ITERATIONS - speed;

		action->SetMaxActionCounter(maxActionCounter);
		action->SetCurActionCounter(0);

		action->SetSoundEffect(soundID);

		if(!moveActor->ActionMove(action))
		{
			delete action;
			return;
		}

		for (int i = 0; i < numberOfMoveActors; i++) {
			moveActors[i]->SetHiddenUnderStack(TRUE);
		}

		if (g_selected_item->GetVisiblePlayer()!= moveActor->GetPlayerNum()
			&& !g_tiledMap->TileIsVisible(moveActor->GetPos().x, moveActor->GetPos().y))
		{
			DirectorImpl::Instance()->CenterMap(moveActor->GetPos());
		}
		AddActiveActor(moveActor);
	}

	virtual void Finalize() {
		moveActor->PositionActor(endPosition);

		for (int i = 0; i < numberOfMoveActors; i++)
		{
			moveActors[i]->PositionActor(endPosition);
		}
	}

	virtual void Dump()
	{
		DPRINTF(k_DBG_UI, ("Move\n"));
		DPRINTF(k_DBG_UI, ("  actor                  :%#x (%#.8lx)\n", moveActor->GetUnitID(), moveActor));
		DPRINTF(k_DBG_UI, ("  startPosition          :%d,%d\n", startPosition.x, startPosition.y));
		DPRINTF(k_DBG_UI, ("  endPosition            :%d,%d\n", endPosition.x, endPosition.y));
		DPRINTF(k_DBG_UI, ("  numberOfMoveActors          :%d\n", numberOfMoveActors));
		DPRINTF(k_DBG_UI, ("  moveActors                  :"));
		for (int i = 0; i < numberOfMoveActors; i++) {
			DPRINTF(k_DBG_UI, ("%#x (%#.8lx)  ", moveActors[i]->GetUnitID(), moveActors[i]));
		}
		DPRINTF(k_DBG_UI, ("\n"));
		DPRINTF(k_DBG_UI, ("  numberOfRevealedActors      :%d\n", numberOfRevealedActors));
		DPRINTF(k_DBG_UI, ("  revealedActors              :"));
		for (int i = 0; i < numberOfRevealedActors; i++) {
			DPRINTF(k_DBG_UI, ("%#x (%#.8lx)  ", revealedActors[i]->GetUnitID(), revealedActors[i]));
		}
		DPRINTF(k_DBG_UI, ("\n"));
		DPRINTF(k_DBG_UI, ("  soundID                     :%d\n", soundID));
	}

	// TODO: make protected
	UnitActor	*moveActor;

protected:
	virtual bool DoSkipProcess()
	{
		// Unit invalid
		if (g_theUnitPool && !g_theUnitPool->IsValid(moveActor->GetUnitID())) {
			return true;
		}

		// Unit invisible
		return !TileIsVisibleToPlayer(startPosition) && !TileIsVisibleToPlayer(endPosition);
	}

	bool 		moveActorActive;
	MapPoint	startPosition;
	MapPoint	endPosition;
	sint32		numberOfMoveActors;
	UnitActor	**moveActors;
	sint32		numberOfRevealedActors;
	UnitActor	**revealedActors;
	sint32		soundID;
};

class DQActionAttack : public DQActionActive
{
public:
	DQActionAttack(
			sint8			owner,
			UnitActor		*attacker,
			const MapPoint	&attackerPosition,
			bool			attackerIsCity,
			UnitActor		*defender,
			const MapPoint	&defenderPosition,
			bool			defenderIsCity
	)
			: DQActionActive(owner),
			  attacker			(attacker),
			  attackerPosition	(attackerPosition),
			  attackerIsCity	(attackerIsCity),
			  defender			(defender),
			  defenderPosition	(defenderPosition),
			  defenderIsCity	(defenderIsCity)
	{}
	virtual ~DQActionAttack() {}
	virtual DQACTION_TYPE GetType() { return DQACTION_ATTACK; }

	virtual void PrepareProcess()
	{
		POINT  attackerPoints, defenderPoints;
		maputils_MapXY2PixelXY(attackerPosition.x, attackerPosition.y, attackerPoints);
		maputils_MapXY2PixelXY(defenderPosition.x, defenderPosition.y, defenderPoints);

		sint32  deltaX = defenderPoints.x - attackerPoints.x;
		sint32  deltaY = defenderPoints.y - attackerPoints.y;

		bool playerInvolved = (defender->GetPlayerNum() == g_selected_item->GetVisiblePlayer())
							  || (attacker->GetPlayerNum() == g_selected_item->GetVisiblePlayer());
		if (playerInvolved)
		{
			Action *action = new Action();
			action->SetStartMapPoint(attackerPosition);
			action->SetEndMapPoint  (attackerPosition);

			sint32 facingIndex = spriteutils_DeltaToFacing(deltaX, deltaY);
			attacker->ActionAttack(action, facingIndex);
			AddActiveActor(attacker);
		}

		bool defenderVisible = TileIsVisibleToPlayer(defenderPosition);
		if (!defenderIsCity && (playerInvolved || defenderVisible))
		{
			Action *action = new Action();
			action->SetStartMapPoint(defenderPosition);
			action->SetEndMapPoint  (defenderPosition);

			sint32 facingIndex = spriteutils_DeltaToFacing(-deltaX, -deltaY);
			defender->ActionAttack(action, facingIndex);

			AddActiveActor(defender);
		}
	}

	virtual void Dump()
	{
		DPRINTF(k_DBG_UI, ("%s\n", GetName()));
		DPRINTF(k_DBG_UI, ("  attacker			:%#x (%#.8lx)\n", attacker->GetUnitID(), attacker));
		DPRINTF(k_DBG_UI, ("  attackerPosition	:%d,%d\n", attackerPosition.x, attackerPosition.y));
		DPRINTF(k_DBG_UI, ("  attackerIsCity		:%s\n", attackerIsCity ? "true" : "false"));
		DPRINTF(k_DBG_UI, ("  defender			:%#x (%#.8lx)\n", defender->GetUnitID(), defender));
		DPRINTF(k_DBG_UI, ("  defenderPosition	:%d,%d\n", defenderPosition.x, defenderPosition.y));
		DPRINTF(k_DBG_UI, ("  defenderIsCity		:%s\n", defenderIsCity ? "true" : "false"));
	}
protected:
	virtual MBCHAR const * const GetName() { return "Attack"; }

	UnitActor	*attacker;
	MapPoint	attackerPosition;
	bool		attackerIsCity;
	UnitActor	*defender;
	MapPoint	defenderPosition;
	bool		defenderIsCity;
};

class DQActionSpecialAttack : public DQActionAttack
{
public:
	DQActionSpecialAttack(
			sint8			owner,
			UnitActor		*attacker,
			const MapPoint	&attackerPosition,
			bool			attackerIsCity,
			sint32			attackerSoundID,
			UnitActor		*defender,
			const MapPoint	&defenderPosition,
			bool			defenderIsCity)
			: DQActionAttack(
					owner, attacker, attackerPosition, attackerIsCity, defender, defenderPosition, defenderIsCity),
			  attackerSoundID	(attackerSoundID)
	{}
	virtual ~DQActionSpecialAttack() {}
	virtual DQACTION_TYPE GetType() { return DQACTION_SPECATTACK; }

	virtual void PrepareProcess()
	{
		if (attackerSoundID >= 0) {
			g_soundManager->AddSound(SOUNDTYPE_SFX, 0, attackerSoundID, 0, 0);
		}

		POINT attackerPoints, defenderPoints;
		maputils_MapXY2PixelXY(attackerPosition.x, attackerPosition.y, attackerPoints);
		maputils_MapXY2PixelXY(defenderPosition.x, defenderPosition.y, defenderPoints);

		sint32 deltaX = defenderPoints.x - attackerPoints.x;
		sint32 deltaY = defenderPoints.y - attackerPoints.y;

		bool attackerVisible = TileIsVisibleToPlayer(attackerPosition);
		if (!attackerIsCity && attackerVisible)
		{
			Action *attackerAction = new Action();
			attackerAction->SetStartMapPoint(attackerPosition);
			attackerAction->SetEndMapPoint  (attackerPosition);

			sint32 facingIndex = spriteutils_DeltaToFacing(deltaX, deltaY);
			if (attacker->ActionSpecialAttack(attackerAction, facingIndex)) {
				AddActiveActor(attacker);
			} else {
				delete attackerAction;
			}
		}

		bool defenderVisible = TileIsVisibleToPlayer(defenderPosition);
		if (!defenderIsCity && defenderVisible)
		{
			Action *defenderAction = new Action();
			defenderAction->SetStartMapPoint(defenderPosition);
			defenderAction->SetEndMapPoint  (defenderPosition);

			sint32 facingIndex = spriteutils_DeltaToFacing(-deltaX, -deltaY);
			if (defender->ActionSpecialAttack(defenderAction, facingIndex)) {
				AddActiveActor(defender);
			} else {
				delete defenderAction;
			}
		}
	}

	virtual void Dump()
	{
		DQActionAttack::Dump();
		DPRINTF(k_DBG_UI, ("  attackerSoundID    ::%d\n",       attackerSoundID));
	}

protected:
	virtual MBCHAR const * const GetName() { return "Special attack"; }

	virtual bool DoSkipProcess() {
		return attackerIsCity && defenderIsCity;
	}

	sint32	attackerSoundID;
};

class DQActionAttackPos : public DQActionActive
{
public:
	DQActionAttackPos(
			sint8			owner,
			UnitActor		*attacker,
			const MapPoint	&attackerPosition,
			const MapPoint	&targetPosition,
			sint32			soundID)
			: DQActionActive(owner),
			  attacker			(attacker),
			  attackerPosition	(attackerPosition),
			  targetPosition	(targetPosition),
			  soundID			(soundID)
	{}
	virtual ~DQActionAttackPos() {}
	virtual DQACTION_TYPE GetType() { return DQACTION_ATTACKPOS; }

	virtual void PrepareProcess()
	{
		Action *action = new Action(UNITACTION_ATTACK, ACTIONEND_ANIMEND,
									attacker->GetHoldingCurAnimPos(UNITACTION_ATTACK),
									attacker->GetHoldingCurAnimSpecialDelayProcess(UNITACTION_ATTACK));
		action->SetStartMapPoint(attackerPosition);
		action->SetEndMapPoint(attackerPosition);

		if (attacker->GetLoadType() != LOADTYPE_FULL) {
			attacker->FullLoad(UNITACTION_ATTACK);
		}
		Anim *attackerAnim = attacker->CreateAnim(UNITACTION_ATTACK);
		if (!attackerAnim) {
			attackerAnim = attacker->CreateAnim(UNITACTION_IDLE);
		}
		action->SetAnim(attackerAnim);

		POINT attackerPoints, attackedPoints;
		maputils_MapXY2PixelXY(attackerPosition.x, attackerPosition.y, attackerPoints);
		maputils_MapXY2PixelXY(targetPosition.x, targetPosition.y, attackedPoints);
		action->SetFacing(spriteutils_DeltaToFacing(attackedPoints.x - attackerPoints.x,
													attackedPoints.y - attackerPoints.y));

		action->SetUnitVisionRange(attacker->GetUnitVisionRange());
		action->SetUnitsVisibility(attacker->GetUnitVisibility());
		attacker->AddAction(action);
		AddActiveActor(attacker);

		if (g_soundManager) {
			sint32 visiblePlayer = g_selected_item->GetVisiblePlayer();
			if ((visiblePlayer == attacker->GetPlayerNum()) ||
				(attacker->GetUnitVisibility() & (1 << visiblePlayer)))
			{
				g_soundManager->AddSound(SOUNDTYPE_SFX, attacker->GetUnitID(), soundID,
										 attackerPosition.x, attackerPosition.y);
			}
		}
	}

	virtual void Dump()
	{
		DPRINTF(k_DBG_UI, ("Attack Pos\n"));
		DPRINTF(k_DBG_UI, ("  attacker               :%#x (%#.8lx)\n", attacker->GetUnitID(), attacker));
		DPRINTF(k_DBG_UI, ("  attackerPosition       :%d,%d\n", attackerPosition.x, attackerPosition.y));
		DPRINTF(k_DBG_UI, ("  targetPosition         :%d,%d\n", targetPosition.x, targetPosition.y));
		DPRINTF(k_DBG_UI, ("  soundID                :%d\n", soundID));
	}
protected:
	virtual bool DoSkipProcess()
	{
		// Unit invisible
		return !TileIsVisibleToPlayer(attackerPosition);
	}

	UnitActor	*attacker;
	MapPoint	attackerPosition;
	MapPoint	targetPosition;
	sint32		soundID;
};

class DQActionDeath : public DQActionActive
{
public:
	DQActionDeath(sint8 owner, UnitActor *dead, const MapPoint &deadPosition, sint32 deadSoundID)
			: DQActionActive(owner),
			  dead			(dead),
			  deadPosition	(deadPosition),
			  deadSoundID	(deadSoundID)
	{}
	virtual ~DQActionDeath() {}
	virtual DQACTION_TYPE GetType() { return DQACTION_DEATH; }

	virtual void PrepareProcess()
	{
		Anim	*deathAnim		= NULL;
		sint32	deathActionType	= UNITACTION_NONE;

		if(dead->HasDeath())
		{
			if (dead->GetLoadType() != LOADTYPE_FULL) {
				dead->FullLoad(UNITACTION_VICTORY);
			}

			deathActionType = UNITACTION_VICTORY;
			deathAnim = dead->CreateAnim((UNITACTION)deathActionType); // deathAnim must be deleted

			if(!deathAnim)
			{
				deathActionType = UNITACTION_FAKE_DEATH;
				deathAnim = dead->MakeFakeDeath();
			}
		}
		else
		{
			deathActionType = UNITACTION_FAKE_DEATH;
			deathAnim = dead->MakeFakeDeath();
		}

		dead->SetHealthPercent(-1.0);
		dead->SetTempStackSize(0);

		Action *action = new Action((UNITACTION)deathActionType, ACTIONEND_ANIMEND);
		action->SetStartMapPoint(deadPosition);
		action->SetEndMapPoint(deadPosition);

		action->SetAnim(deathAnim);

		action->SetUnitVisionRange(dead->GetUnitVisionRange());
		action->SetUnitsVisibility(dead->GetUnitVisibility());
		action->SetFacing(dead->GetFacing());
		dead->AddAction(action);
		AddActiveActor(dead);

		if (g_soundManager) {
			sint32 visiblePlayer = g_selected_item->GetVisiblePlayer();
			if ((visiblePlayer == dead->GetPlayerNum()) ||
				(dead->GetUnitVisibility() & (1 << visiblePlayer))) {
				g_soundManager->AddSound(SOUNDTYPE_SFX, (uint32)dead->GetUnitID(), deadSoundID,
										 deadPosition.x, deadPosition.y);
			}
		}
	}

	virtual void Dump()
	{
		DPRINTF(k_DBG_UI, ("Death\n"));
		DPRINTF(k_DBG_UI, ("  dead               :%#x (%#.8lx)\n", dead->GetUnitID(), dead));
		DPRINTF(k_DBG_UI, ("  deadPosition       :%d,%d\n", deadPosition.x, deadPosition.y));
		DPRINTF(k_DBG_UI, ("  deadSoundID        :%d\n", deadSoundID));
	}
protected:
	virtual bool DoSkipProcess()
	{
		// Unit invisible
		return !TileIsVisibleToPlayer(deadPosition);
	}

	UnitActor	*dead;
	MapPoint	deadPosition;
	sint32		deadSoundID;
};

class DQActionWork : public DQActionActive
{
public:
	DQActionWork(sint8 owner, UnitActor *workActor, const MapPoint &workPosition, sint32 workSoundID)
			: DQActionActive(owner),
			  workActor		(workActor),
			  workPosition	(workPosition),
			  workSoundID	(workSoundID)
	{}
	virtual ~DQActionWork() {}
	virtual DQACTION_TYPE GetType() { return DQACTION_WORK; }

	virtual void PrepareProcess()
	{
		Action *action = new Action(UNITACTION_WORK, ACTIONEND_ANIMEND);
		action->SetStartMapPoint(workPosition);
		action->SetEndMapPoint(workPosition);

		if (workActor->GetLoadType() != LOADTYPE_FULL) {
			workActor->FullLoad(UNITACTION_WORK);
		}

		Anim *anim = workActor->CreateAnim(UNITACTION_WORK);
		if (!anim)
		{
			anim = workActor->CreateAnim(UNITACTION_MOVE);
			if (!anim) {
				delete action;
				return;
			}
		}
		action->SetAnim(anim);
		workActor->AddAction(action);
		AddActiveActor(workActor);

		if (g_soundManager)
		{
			sint32 visiblePlayer = g_selected_item->GetVisiblePlayer();
			if ((visiblePlayer == workActor->GetPlayerNum()) ||
				(workActor->GetUnitVisibility() & (1 << visiblePlayer)))
			{
				g_soundManager->AddSound(SOUNDTYPE_SFX, workActor->GetUnitID(), workSoundID,
						workPosition.x, workPosition.y);
			}
		}
	}

	virtual void Dump()
	{
		DPRINTF(k_DBG_UI, ("Work\n"));
		DPRINTF(k_DBG_UI, ("  workActor          :%#x (%#.8lx)\n", workActor->GetUnitID(), workActor));
		DPRINTF(k_DBG_UI, ("  workPosition       :%d,%d\n", workPosition.x, workPosition.y));
		DPRINTF(k_DBG_UI, ("  workSoundID        :%d\n", workSoundID));
	}
protected:
	virtual bool DoSkipProcess()
	{
		// Unit invisible
		return !TileIsVisibleToPlayer(workPosition);
	}

	UnitActor	*workActor;
	MapPoint	workPosition;
	sint32		workSoundID;
};

class DQActionFaceoff : public DQActionActive
{
public:
	DQActionFaceoff(
			sint8			owner,
			UnitActor 		*attacker,
			const MapPoint	&attackerPosition,
			UnitActor		*attacked,
			const MapPoint	&attackedPosition)
			: DQActionActive(owner),
			  attacker			(attacker),
			  attackerPosition	(attackerPosition),
			  attacked			(attacked),
			  attackedPosition	(attackedPosition)
	{}
	virtual ~DQActionFaceoff() {}
	virtual DQACTION_TYPE GetType() { return DQACTION_FACEOFF; }

	virtual void PrepareProcess()
	{
		Action *attackerAction = new Action(UNITACTION_FACE_OFF, ACTIONEND_INTERRUPT);
		attackerAction->SetStartMapPoint(attackerPosition);
		attackerAction->SetEndMapPoint(attackerPosition);

		Action *attackedAction = new Action(UNITACTION_FACE_OFF, ACTIONEND_INTERRUPT);
		attackedAction->SetStartMapPoint(attackedPosition);
		attackedAction->SetEndMapPoint(attackedPosition);

		Anim * attackerAnim = attacker->MakeFaceoff();
		if (!attackerAnim)
		{
			attacker->AddIdle(TRUE);
			delete attackerAction;
			delete attackedAction;
			return;
		}
		attackerAction->SetAnim(attackerAnim);

		if (attacked->GetLoadType() != LOADTYPE_FULL) {
			attacked->FullLoad(UNITACTION_IDLE);
		}

		Anim *attackedAnim = attacked->MakeFaceoff();
		if (!attackedAnim)
		{
			attacked->AddIdle(TRUE);
			delete attackedAction;
			attackedAction = NULL;
		}

		POINT attackerPoints, attackedPoints;

		maputils_MapXY2PixelXY(attackerPosition.x, attackerPosition.y, attackerPoints);
		maputils_MapXY2PixelXY(attackedPosition.x, attackedPosition.y, attackedPoints);

		attackerAction->SetFacing(spriteutils_DeltaToFacing(
				attackedPoints.x - attackerPoints.x, attackedPoints.y - attackerPoints.y));

		if(attackedAnim)
		{
			attackedAction->SetAnim(attackedAnim);
			attackedAction->SetFacing(spriteutils_DeltaToFacing(
					attackerPoints.x - attackedPoints.x, attackerPoints.y - attackedPoints.y));
		}

		attackerAction->SetUnitVisionRange(attacker->GetUnitVisionRange());
		attackerAction->SetUnitsVisibility(attacker->GetUnitVisibility());
		attacker->AddAction(attackerAction);

		if(attackedAnim)
		{
			attackedAction->SetUnitVisionRange(attacked->GetUnitVisionRange());

			attackedAction->SetUnitsVisibility(attacker->GetUnitVisibility());
			attacked->AddAction(attackedAction);
		}

		bool attackedVisible = TileIsVisibleToPlayer(attackedPosition);
		if (attacker->GetPlayerNum() == g_selected_item->GetVisiblePlayer() ||
			attacked->GetPlayerNum() == g_selected_item->GetVisiblePlayer()) {
			attackedVisible = true;
		}
		if (attackedVisible) {
			AddActiveActor(attacked);
		}

		bool attackerVisible = TileIsVisibleToPlayer(attackerPosition);
		if (attacked->GetPlayerNum() == g_selected_item->GetVisiblePlayer() ||
			attacker->GetPlayerNum() == g_selected_item->GetVisiblePlayer()) {
			attackerVisible = true;
			attackedVisible = true;
		}
		if (attackerVisible && attackedVisible) {
			AddActiveActor(attacker);
		}
	}

	virtual void Dump()
	{
		DPRINTF(k_DBG_UI, ("Faceoff\n"));
		DPRINTF(k_DBG_UI, ("  attacker           :%#x (%#.8lx)\n", attacker->GetUnitID(), attacker));
		DPRINTF(k_DBG_UI, ("  attackerPosition   :%d,%d\n", attackerPosition.x, attackerPosition.y));
		DPRINTF(k_DBG_UI, ("  attacked           :%#x (%#.8lx)\n", attacked->GetUnitID(), attacked));
		DPRINTF(k_DBG_UI, ("  attackedPosition   :%d,%d\n", attackedPosition.x, attackedPosition.y));
	}
protected:
	UnitActor	*attacker;
	MapPoint	attackerPosition;
	UnitActor	*attacked;
	MapPoint	attackedPosition;
};

class DQActionInvokeResearchAdvance : public DQActionExternal
{
public:
	DQActionInvokeResearchAdvance(MBCHAR const * const message)
	: DQActionExternal(),
	message	(strdup(message))
	{}
	virtual ~DQActionInvokeResearchAdvance()
	{
		free(message);
	}
	virtual DQACTION_TYPE GetType() { return DQACTION_INVOKE_RESEARCH_ADVANCE; }

	virtual void Execute()
	{
		sci_advancescreen_displayMyWindow(message, 0);
	}

	virtual void Dump()
	{
		DPRINTF(k_DBG_UI, ("Invoke Research advance\n"));
		DPRINTF(k_DBG_UI, ("  message              :%s\n", message));
	}
protected:
	MBCHAR	*message;
};

class DQActionBattle : public DQActionExternal
{
public:
	DQActionBattle(Battle *battle)
			: DQActionExternal(),
			  battle	(battle)
	{}
	virtual ~DQActionBattle() {}
	virtual DQACTION_TYPE GetType() { return DQACTION_BATTLE; }

	virtual void Execute()
	{
		BattleViewWindow::Initialize();

		if (g_battleViewWindow)
		{
			g_battleViewWindow->SetupBattle(battle);
			g_c3ui->AddWindow(g_battleViewWindow);
			g_cursorManager->SetCursor(CURSORINDEX_DEFAULT);
		}
	}

	virtual void Dump()
	{
		DPRINTF(k_DBG_UI, ("Battle\n"));
	}
protected:
	Battle	*battle;
};

class DQActionPlayWonderMovie : public DQActionExternal
{
public:
	DQActionPlayWonderMovie(sint32 wonderMovieID)
			: DQActionExternal(),
			  wonderMovieID	(wonderMovieID)
	{}
	virtual ~DQActionPlayWonderMovie() {}
	virtual DQACTION_TYPE GetType() { return DQACTION_PLAYWONDERMOVIE; }

	virtual void Execute()
	{
		wondermoviewin_Initialize();
		wondermoviewin_DisplayWonderMovie(wonderMovieID);
	}

	virtual void Dump()
	{
		DPRINTF(k_DBG_UI, ("Play Wonder Movie\n"));
		DPRINTF(k_DBG_UI, ("  wonderMovieID        :%ld\n", wonderMovieID));
	}
protected:
	sint32	wonderMovieID;
};

class DQActionPlayVictoryMovie : public DQActionExternal
{
public:
	DQActionPlayVictoryMovie(GAME_OVER reason)
			: DQActionExternal(),
			  reason	(reason)
	{}
	virtual ~DQActionPlayVictoryMovie() {}
	virtual DQACTION_TYPE GetType() { return DQACTION_PLAYVICTORYMOVIE; }

	virtual void Execute()
	{
		victorymoviewin_Initialize();
		victorymoviewin_DisplayVictoryMovie(reason);
	}

	virtual void Dump()
	{
		DPRINTF(k_DBG_UI, ("Play Victory Movie\n"));
		DPRINTF(k_DBG_UI, ("  reason                :%ld\n", reason));
	}
protected:
	GAME_OVER reason;
};

class DQActionBeginScheduler : public DQActionExternal
{
public:
	DQActionBeginScheduler(sint32 player)
			: DQActionExternal(),
			  player	(player)
	{}
	virtual ~DQActionBeginScheduler() {}
	virtual DQACTION_TYPE GetType() { return DQACTION_BEGIN_SCHEDULER; }

	virtual void Execute()
	{
#ifdef _DEBUG
		static bool isCurrentPlayerOk = true; // static, to report the error only once
		if (isCurrentPlayerOk)
		{
			isCurrentPlayerOk = player == g_selected_item->GetCurPlayer();
			Assert(isCurrentPlayerOk);
		}
#endif

		if(g_network.IsHost())
		{
			g_network.Enqueue(new NetInfo(NET_INFO_CODE_BEGIN_SCHEDULER, player));
		}

		g_gevManager->Pause();
		g_gevManager->AddEvent(GEV_INSERT_Tail, GEV_BeginScheduler,
							   GEA_Player, player,
							   GEA_End);
		g_gevManager->Resume();

		bool canStartImmediate = g_network.IsActive() && !g_network.IsLocalPlayer(player);
		if (canStartImmediate) {
			DirectorImpl::Instance()->ExternalActionFinished(DEA_BEGIN_SCHEDULER);
		}
	}

	virtual void Dump()
	{
		DPRINTF(k_DBG_UI, ("Begin Scheduler\n"));
		DPRINTF(k_DBG_UI, ("  player: %d\n", player));
	}
protected:
	sint32 player;
};

DirectorImpl *DirectorImpl::m_instance = NULL;
DirectorImpl::DirectorImpl(void)
:
	m_currentAction				(NULL),
	m_activeLoopingSound		(NULL),
	m_activeEffectList			(new tech_WLList<EffectActor *>),
	m_tradeActorList			(new tech_WLList<TradeActor *>),
	m_masterCurTime				(0),
	m_lastTickCount				(0),
	m_timeLogIndex				(0),
	m_averageElapsed			(0),
	m_averageFPS				(k_DEFAULT_FPS),
	m_paused					(FALSE),
	m_processingActiveEffects	(FALSE),
	m_actionQueue				(new PointerList<DQAction>),
	m_actionWalker				(new PointerList<DQAction>::Walker),
	m_pendingGameActions		(0),
	m_endTurnRequested			(false)
{
	std::fill(m_timeLog, m_timeLog + k_TIME_LOG_SIZE, 0);

	delete m_instance;
	m_instance = this;
}

DirectorImpl::~DirectorImpl(void)
{
	delete m_actionQueue;
	delete m_actionWalker;
	delete m_activeEffectList;
	delete m_tradeActorList;
	delete m_activeLoopingSound;
	delete m_currentAction;
	m_instance = NULL;
}

void DirectorImpl::FastKill(UnitActor *actor)
{
	if (m_currentAction && m_currentAction->IsTypeActive()) {
		static_cast<DQActionActive*>(m_currentAction)->FastKill(actor);
	}
	m_dyingActors.erase(actor);
	m_standbyActors.erase(actor);
	delete actor;
}

void DirectorImpl::FastKill(EffectActor *actor)
{
	ListPos		pos =
		m_activeEffectList->Find(actor, m_activeEffectList->GetHeadPosition());
	if (pos)
	{
		if (m_processingActiveEffects)
		{
			actor->SetKillNow();
		}
		else
		{
			m_activeEffectList->DeleteAt(pos);
			delete actor;
			actor = NULL;
		}
	}
}

void DirectorImpl::UpdateTimingClock(void)
{
	sint32  elapsed;

	if (m_lastTickCount == 0) {
		elapsed = 1000/k_DEFAULT_FPS;
	} else {
		elapsed = GetTickCount() - m_lastTickCount;
	}

	if (elapsed > k_ELAPSED_CEILING)
		elapsed = k_ELAPSED_CEILING;

	m_lastTickCount = GetTickCount();

	m_timeLog[m_timeLogIndex++] = elapsed;

	if (m_timeLogIndex >= k_TIME_LOG_SIZE)
	{
		sint32		timeSum=0;
		for (sint32 i=0; i<k_TIME_LOG_SIZE; i++)
			timeSum += m_timeLog[i];

		m_averageElapsed = (timeSum / k_TIME_LOG_SIZE);

		if (m_averageElapsed > 0)
		{
			m_averageFPS = 1000 / m_averageElapsed;
		}
		else
		{
			m_averageFPS = k_DEFAULT_FPS;
		}

		m_timeLogIndex = 0;
	}

	m_masterCurTime += elapsed;
}

void DirectorImpl::UpdateStandbyUnits()
{
	if (!m_standbyActors.empty() || m_actionQueue->IsEmpty())
		return;

	PointerList<DQAction>::Walker walk(m_actionQueue);
	for (; walk.IsValid(); walk.Next()) {
		UnitActor *standbyActor = NULL;
		switch (walk.GetObj()->GetType()) {
			case DQACTION_MOVE:
				standbyActor = ((DQActionMove *) (walk.GetObj()))->moveActor;
				break;
			case DQACTION_TELEPORT:
				standbyActor = ((DQActionTeleport *) (walk.GetObj()))->moveActor;
				break;
			default:
				break;
		}
		if (standbyActor && !standbyActor->IsActive()) {
			m_standbyActors.insert(standbyActor);
		}
	}
}

void DirectorImpl::Process(void)
{
	UpdateTimingClock();

	static uint32 nextTime = 0;

	if (GetTickCount() > nextTime)
	{
		ProcessStandbyUnits();
		ProcessCurrentAction();
		ProcessActiveEffects();
		ProcessTradeRouteAnimations();

		HandleNextAction();

		if(g_tiledMap)
			g_tiledMap->ProcessLayerSprites(g_tiledMap->GetMapViewRect(), 0);

		nextTime = GetTickCount() + 5; // 75;
	}
}

void DirectorImpl::PauseDirector(BOOL pause)
{
	m_paused = pause;
}

#ifdef _DEBUG

void DirectorImpl::DumpAction(DQAction *action)
{
	if (action) {
		action->Dump();
	} else {
		DPRINTF(k_DBG_UI, ("Action is null\n"));
	}
}

#endif

#ifdef _DEBUG

void DirectorImpl::DumpInfo(void)
{
	DPRINTF(k_DBG_UI, (" ------------------\n"));
	DPRINTF(k_DBG_UI, ("Director Dump:\n"));
	DPRINTF(k_DBG_UI, (" ------------------\n"));
	DPRINTF(k_DBG_UI, (" Current Action:\n"));
	DumpAction(m_currentAction);
	DPRINTF(k_DBG_UI, (" ------------------\n"));
	DPRINTF(k_DBG_UI, (" Queued Actions:\n"));
	DPRINTF(k_DBG_UI, (" Count:%d\n", m_actionQueue->GetCount()));
	DPRINTF(k_DBG_UI, (" ------------------\n"));

	for (
		m_actionWalker->SetList(m_actionQueue);
		m_actionWalker->IsValid();
		m_actionWalker->Next()
		)
	{
		DumpAction(m_actionWalker->GetObj());
	}

	DPRINTF(k_DBG_UI, (" ------------------\n"));
}
#endif

void DirectorImpl::EndTurn() {

	for (auto it = m_dyingActors.begin(); it != m_dyingActors.end();) {
		UnitActor *actor = *it;
		// Move iterator forward before killing the actor as it will remove the actor from the set.
		it++;
		FastKill(actor);
	}
}

bool DirectorImpl::HandleActor(bool immediate, UnitActor *actor, bool enableAction)
{
	if (enableAction && !immediate) {
		return true;
	}

	FinalizeActor(actor);
	return false;
}

void DirectorImpl::FinalizeActor(UnitActor *actor)
{
	if (!actor)
		return;

	if (m_dyingActors.count(actor)) {
		FastKill(actor);
	} else {
		actor->EndTurnProcess();
	}
}

void DirectorImpl::HandleNextAction(void)
{
	if (m_paused) return;

	while (CanStartNextAction())
	{
	 	m_currentAction = m_actionQueue->RemoveHead();
	 	m_currentAction->ProcessLoopingSound(m_activeLoopingSound);
	 	m_currentAction->Execute();
		if (m_currentAction->SkipProcess()) {
			FinalizeAction(m_currentAction);
		} else {
			m_currentAction->PrepareProcess();
		}
	}

	if (!m_currentAction) {
		delete m_activeLoopingSound;
		m_activeLoopingSound = NULL;
	}
}

void DirectorImpl::ExternalActionFinished(DEACTION_TYPE externalActionType)
{
	DQACTION_TYPE actionType;
	switch(externalActionType) {
		case DEA_BEGIN_SCHEDULER:
			actionType = DQACTION_BEGIN_SCHEDULER;
			break;
		case DEA_BATTLE_WINDOW:
			actionType = DQACTION_BATTLE;
			break;
		case DEA_ADVANCE_WINDOW:
			actionType = DQACTION_INVOKE_RESEARCH_ADVANCE;
			break;
		case DEA_WONDER_MOVIE:
			actionType = DQACTION_PLAYWONDERMOVIE;
			break;
		case DEA_VICTORY_MOVIE:
			actionType = DQACTION_PLAYVICTORYMOVIE;
			break;
		default:
			Assert(false);
			break;
	}

	if (m_currentAction && m_currentAction->GetType() == actionType) {
		static_cast<DQActionExternal*>(m_currentAction)->SetFinished();
	}
}

void DirectorImpl::FinalizeAction(DQAction* &action)
{
	action->Finalize();
	delete action;
	action = NULL;
}

void DirectorImpl::CatchUp(void) {
	if (m_currentAction) {
		FinalizeAction(m_currentAction);
	}

	while (m_actionQueue->GetCount())
	{
		DQAction *action = m_actionQueue->RemoveHead();
		action->Execute();
		FinalizeAction(action);
	}

	if (!g_network.IsActive()) {

		KillAllActiveEffects();
		if (g_soundManager)
			g_soundManager->TerminateAllLoopingSounds(SOUNDTYPE_SFX);
	}
}

bool DirectorImpl::CaughtUp(void)
{
	return !m_actionQueue || (m_actionQueue->GetCount() == 0);
}

void DirectorImpl::CenterMap(const MapPoint &pos)
{
	g_radarMap->CenterMap(pos);
	g_tiledMap->Refresh();
	g_tiledMap->InvalidateMap();
	g_tiledMap->InvalidateMix();
	background_draw_handler(g_background);
}

void DirectorImpl::ActiveEffectAdd(EffectActor *effectActor)
{
	Assert(effectActor);
	ListPos		pos =
		m_activeEffectList->Find(effectActor, m_activeEffectList->GetHeadPosition());

	if (!pos)
	{
		pos = m_activeEffectList->AddHead(effectActor);
	}
}

void DirectorImpl::ActiveEffectRemove(EffectActor *effectActor)
{
	ListPos		pos =
		m_activeEffectList->Find(effectActor, m_activeEffectList->GetHeadPosition());

	if (pos)
	{
		m_activeEffectList->DeleteAt(pos);
	}
}

void DirectorImpl::TradeActorCreate(TradeRoute newRoute)
{
	ListPos         pos         = m_tradeActorList->GetHeadPosition();
	ListPos         foundPos    = pos;

	for (size_t i = 0; i < m_tradeActorList->L(); ++i)
	{
		TradeActor *    tActor  = m_tradeActorList->GetNext(pos);

		Assert(tActor);
		if (tActor->GetRouteID() == newRoute)
			break;
		else
			foundPos = pos;
	}

	if (!foundPos)
	{
		pos = m_tradeActorList->AddHead(new TradeActor(newRoute));
	}
}

void DirectorImpl::TradeActorDestroy(TradeRoute routeToDestroy)
{
	TradeActor *    tActor      = NULL;
	ListPos         pos         = m_tradeActorList->GetHeadPosition();
	ListPos         foundPos    = pos;

	for (size_t i = 0; i < m_tradeActorList->L(); i++)
	{
		tActor = m_tradeActorList->GetNext(pos);

		Assert(tActor);
		if(tActor->GetRouteID() == routeToDestroy)
			break;
		else
			foundPos = pos;
	}

	if (foundPos)
	{
		m_tradeActorList->DeleteAt(foundPos);
		delete tActor;
	}
}

void DirectorImpl::ProcessStandbyUnits()
{
	m_standbyActors.clear();
	UpdateStandbyUnits();

	for (std::set<UnitActor*>::iterator it=m_standbyActors.begin(); it != m_standbyActors.end(); ++it) {
		(*it)->Process();
	}
}

void DirectorImpl::ProcessCurrentAction(void)
{
	if (m_currentAction) {
		m_currentAction->Process();
		if (m_currentAction->IsFinished()) {
			FinalizeAction(m_currentAction);
		}
	}
}

uint32 DirectorImpl::ProcessActiveEffects(void)
{
	if (m_activeEffectList->IsEmpty()) return 0;

	m_processingActiveEffects = TRUE;

	ListPos pos = m_activeEffectList->GetHeadPosition();
	for (size_t i = m_activeEffectList->L(); i > 0; --i)
	{
		EffectActor * actor = m_activeEffectList->GetNext(pos);
		if (actor)
		{
			actor->Process();
		}
	}

	m_processingActiveEffects = FALSE;

	pos = m_activeEffectList->GetHeadPosition();
	for (size_t n = m_activeEffectList->L(); n > 0; --n)
	{
		ListPos	      oldPos    = pos;
		EffectActor * oldActor  = m_activeEffectList->GetNext(pos);
		if (oldActor && oldActor->GetKillNow())
		{
			m_activeEffectList->DeleteAt(oldPos);
			delete oldActor;
		}
	}

	return 0;
}

void DirectorImpl::ProcessTradeRouteAnimations(void)
{
	if (!g_theProfileDB->IsTradeAnim()) return;
	if (m_tradeActorList->IsEmpty()) return;

	ListPos             pos             = m_tradeActorList->GetHeadPosition();
	size_t const        numToProcess    = m_tradeActorList->L();
	for (size_t i = 0; i < numToProcess; ++i)
	{
		TradeActor *    actor           = m_tradeActorList->GetNext(pos);
		if (actor)
		{
			actor->Process();
		}
	}
}

void DirectorImpl::OffsetActors(sint32 deltaX, sint32 deltaY)
{
	OffsetStandbyUnits(-deltaX, -deltaY);
	OffsetActiveUnits(-deltaX, -deltaY);
	OffsetActiveEffects(-deltaX, -deltaY);
	OffsetTradeRouteAnimations(-deltaX, -deltaY);
}

void DirectorImpl::OffsetStandbyUnits(sint32 deltaX, sint32 deltaY)
{
	UpdateStandbyUnits();
	for (std::set<UnitActor*>::iterator it=m_standbyActors.begin(); it != m_standbyActors.end(); ++it) {
		UnitActor *actor = *it;
		actor->SetX(actor->GetX() + deltaX);
		actor->SetY(actor->GetY() + deltaY);
	}
}

void DirectorImpl::OffsetActiveUnits(sint32 deltaX, sint32 deltaY)
{
	if (m_currentAction && m_currentAction->IsTypeActive()) {
		static_cast<DQActionActive*>(m_currentAction)->Offset(deltaX, deltaY);
	}
}

void DirectorImpl::OffsetActiveEffects(sint32 deltaX, sint32 deltaY)
{
	ListPos pos             = m_activeEffectList->GetHeadPosition();
	size_t  numToProcess    = m_activeEffectList->L();

	for (size_t i = 0; i < numToProcess; i++)
	{
		EffectActor*    actor       = m_activeEffectList->GetNext(pos);

		if (actor)
		{
			actor->SetX(actor->GetX() + deltaX);
			actor->SetY(actor->GetY() + deltaY);
		}
	}
}

void DirectorImpl::OffsetTradeRouteAnimations(sint32 deltaX, sint32 deltaY)
{
	ListPos pos             = m_tradeActorList->GetHeadPosition();
	size_t  numToProcess    = m_tradeActorList->L();

	for (size_t i = 0; i < numToProcess; i++)
	{
		TradeActor *    tActor = m_tradeActorList->GetNext(pos);

		if (tActor)
		{
			tActor->SetX(tActor->GetX() + deltaX);
			tActor->SetY(tActor->GetY() + deltaY);
		}
	}
}

uint32 DirectorImpl::KillAllActiveEffects()
{
	if (m_activeEffectList->IsEmpty()) return 0;

	ListPos     pos = m_activeEffectList->GetHeadPosition();

	do
	{
		ListPos         actorPos    = pos;
		EffectActor *   actor       = m_activeEffectList->GetNext(pos);

		if (actor)
		{
			m_activeEffectList->DeleteAt(actorPos);
			delete actor;
		}
	} while (pos);

	return 0;
}

void DirectorImpl::NextPlayer() {
#ifdef _PLAYTEST
	if (!g_doingFastRounds &&
		(!g_network.IsActive() || g_player[g_selected_item->GetVisiblePlayer()]->IsRobot())) {
		return;
	}
#else
	return; // Next code isn't used, should it be used?
#endif
	if (!g_network.IsActive()) {
		if (m_currentAction) {
			FinalizeAction(m_currentAction);
		}
	}

	while(m_actionQueue->GetCount())
	{
		DQAction *action = m_actionQueue->RemoveHead();
		action->Execute();
		action->Finalize();
		delete action;
	}

	if (!g_network.IsActive())
	{
		if(g_tiledMap)
			g_tiledMap->NextPlayer();

		KillAllActiveEffects();

		if (g_soundManager)
			g_soundManager->TerminateAllLoopingSounds(SOUNDTYPE_SFX);
	}
}

void DirectorImpl::Draw(RECT *paintRect, sint32 layer)
{
	DrawTradeRouteAnimations(paintRect, layer);
	DrawStandbyUnits(paintRect, layer);
	DrawActiveUnits(paintRect, layer);
	DrawActiveEffects(paintRect, layer);
}

void DirectorImpl::DrawActiveUnits(RECT *paintRect, sint32 layer)
{
	if (m_currentAction && m_currentAction->IsTypeActive()) {
		static_cast<DQActionActive*>(m_currentAction)->Draw(paintRect, layer);
	}
}

void DirectorImpl::DrawStandbyUnits(RECT *paintRect, sint32 layer)
{
	UpdateStandbyUnits();
	for (std::set<UnitActor*>::iterator it=m_standbyActors.begin(); it != m_standbyActors.end(); ++it) {
		DrawUnitActor(paintRect, *it, true);
	}
}

void DirectorImpl::DrawUnitActor(RECT *paintRect, UnitActor *actor, bool standby)
{
	if (!actor)
		return;

	if (actor->GetUnitVisibility() & (1 << g_selected_item->GetVisiblePlayer())) {
		MapPoint pos = actor->GetPos();
		// Do not draw standby units over cities
		if (standby && g_theWorld->IsCity(pos)) {
			return;
		}

		sint32	tileX;
		maputils_MapX2TileX(pos.x, pos.y, &tileX);

		if (maputils_TilePointInTileRect(tileX, pos.y, paintRect))
		{
			g_tiledMap->PaintUnitActor(actor);
		}
	}
}

void DirectorImpl::DrawActiveEffects(RECT *paintRect, sint32 layer)
{
	ListPos				pos;
	EffectActor			*actor;
	uint32				numToProcess;

	pos = m_activeEffectList->GetHeadPosition();
	numToProcess = m_activeEffectList->L();
	for (uint32 i=0; i<numToProcess; i++)
	{
		actor = m_activeEffectList->GetNext(pos);

		if (actor )
		{
			MapPoint pos = actor->GetPos();

			sint32		tileX;

			maputils_MapX2TileX(pos.x, pos.y, &tileX);

			if (!maputils_TilePointInTileRect(tileX, pos.y, paintRect))
				continue;

			if (actor->GetActionQueueNumItems() <= 0 && actor->GetKillNow())
			{
				FastKill(actor);
			}
			else
			{
				if(actor->GetCurAction())
					g_tiledMap->PaintEffectActor(actor);
			}
		}
	}
}

void DirectorImpl::DrawTradeRouteAnimations(RECT *paintRect, sint32 layer)
{
	if (!g_theProfileDB->IsTradeAnim()) return;

	RECT			tempRect;
	ListPos pos             = m_tradeActorList->GetHeadPosition();
	size_t  numToProcess    = m_tradeActorList->L();
	for (size_t i = 0; i < numToProcess; i++)
	{
		TradeActor * tActor = m_tradeActorList->GetNext(pos);

		if (tActor)
		{
			MapPoint	pos = tActor->GetCurrentPos();

			sint32		tileX;

			maputils_MapX2TileX(pos.x, pos.y, &tileX);

			if (!maputils_TilePointInTileRect(tileX, pos.y, paintRect))
				continue;

			tActor->Draw(g_tiledMap->GetLocalVision());

			tActor->GetBoundingRect(&tempRect);


			g_tiledMap->AddDirtyRectToMix(tempRect);
		}
	}
}

void DirectorImpl::AddMove (
		Unit                mover,
		MapPoint const &    oldPos,
		MapPoint const &    newPos,
		sint32              numRevealed,
		UnitActor **        revealedActors,
		sint32              numRest,
		UnitActor **        restOfStack,
		sint32              soundID
	)
{
	Assert(!mover.IsCity());

	UnitActor *actor = mover.GetActor();
	if (!actor)
		return;

	Assert(actor->GetUnitID() == mover.m_id);

	if(g_theProfileDB->IsEnemyMoves() &&
	   mover.GetOwner() != g_selected_item->GetVisiblePlayer() &&
	   (mover.GetVisibility() & (1 << g_selected_item->GetVisiblePlayer())) &&
	   !TileWillBeCompletelyVisible(newPos.x, newPos.y)) {
		AddCenterMap(newPos);
	}

	actor->SetHiddenUnderStack(FALSE);

	DQActionMove *action = new DQActionMove(
			mover.GetOwner(),
			actor,
			oldPos,
			newPos,
			numRest,
			numRest > 0 ? restOfStack : NULL,
			numRevealed,
			numRevealed > 0 ? revealedActors : NULL,
			soundID);
	m_actionQueue->AddTail(action);
}

void DirectorImpl::AddTeleport (
		Unit                top,
		MapPoint const &    oldPos,
		MapPoint const &    newPos,
		sint32              numRevealed,
		UnitActor **        revealedActors,
		sint32              arraySize,
		UnitActor **        moveActors
	)
{
	if (!top->GetActor())
		return;

	DQActionTeleport *action = new DQActionTeleport(
			top->GetActor(),
			oldPos,
			newPos,
			arraySize,
			moveActors,
			numRevealed,
			revealedActors);
	m_actionQueue->AddTail(action);
}

void DirectorImpl::AddProjectileAttack(Unit shooting, Unit target, SpriteState *projectile_state,
									   SpriteState *projectileEnd_state, sint32 projectile_Path)
{
	DQActionMoveProjectile *action = new DQActionMoveProjectile(
			projectileEnd_state, shooting.RetPos(), target.RetPos());
	m_actionQueue->AddTail(action);
}

void DirectorImpl::AddSpecialEffect(MapPoint &pos, sint32 spriteID, sint32 soundID)
{
	DQActionSpecialEffect *action = new DQActionSpecialEffect(pos, spriteID, soundID);
	m_actionQueue->AddTail(action);
}

void DirectorImpl::AddCombatFlash(MapPoint const & pos)
{
	DQActionCombatFlash	*action = new DQActionCombatFlash(pos);
	m_actionQueue->AddTail(action);
}

void DirectorImpl::AddCopyVision(void)
{
	DQActionCopyVision *action = new DQActionCopyVision();
	m_actionQueue->AddTail(action);
}

void DirectorImpl::AddCenterMap(const MapPoint &pos)
{
	DQActionCenterMap *action = new DQActionCenterMap(pos);
	m_actionQueue->AddTail(action);
}

void DirectorImpl::AddSelectUnit(uint32 flags)
{
	DQActionUnitSelection *action = new DQActionUnitSelection(flags);
	m_actionQueue->AddTail(action);
}

void DirectorImpl::AddEndTurn(void)
{
	DPRINTF(k_DBG_GAMESTATE, ("Director::AddEndTurn, curPlayer = %d\n", g_selected_item->GetCurPlayer()));

	if(g_selected_item->GetCurPlayer() == g_selected_item->GetVisiblePlayer())
	{
		static sint32 last_turn_processed = -1;
		if(last_turn_processed != g_player[g_selected_item->GetCurPlayer()]->m_current_round) {
			last_turn_processed = g_player[g_selected_item->GetCurPlayer()]->m_current_round;

			g_gevManager->Pause();

			Player *p = g_player[g_selected_item->GetCurPlayer()];
			p->m_endingTurn = TRUE;

			for(sint32 i = 0; i < p->m_all_armies->Num(); i++)
			{
//				IncrementPendingGameActions();
				g_gevManager->AddEvent(GEV_INSERT_Tail, GEV_BeginTurnExecute,
									   GEA_Army, p->m_all_armies->Access(i).m_id,
									   GEA_End);
			}

			g_player[g_selected_item->GetCurPlayer()]->m_endingTurn = FALSE;
			g_gevManager->Resume();
		}
	}

	if(m_pendingGameActions > 0 || m_endTurnRequested)
	{
		DPRINTF(k_DBG_GAMESTATE, ("Director::AddEndTurn, but there are %d pending actions (or it was already requested), not doing it yet.\n", m_pendingGameActions));
		m_endTurnRequested = true;
		return;
	}

	static sint32 lastPlayer = -1;
	static sint32 lastRound = -1;

	if(g_selected_item->GetCurPlayer() == lastPlayer &&
	   g_player[lastPlayer] && g_player[lastPlayer]->m_current_round == lastRound) {
		PointerList<DQAction>::Walker walk(m_actionQueue);
		for(; walk.IsValid(); walk.Next()) {
			if(walk.GetObj()->GetType() == DQACTION_ENDTURN) {
				DPRINTF(k_DBG_GAMESTATE, ("Skipping duplicate end turn for %d,%d\n", lastPlayer, lastRound));
				return;
			}
		}
	}

	lastPlayer = g_selected_item->GetCurPlayer();
	if(g_player[lastPlayer])
	{
		lastRound = g_player[lastPlayer]->m_current_round;
	}
	else
	{
		lastRound = -1;
	}

	DQActionEndTurn *action = new DQActionEndTurn();
	m_actionQueue->AddTail(action);
}

void DirectorImpl::AddAttack(Unit attacker, Unit defender)
{
	if (!attacker.GetActor() || !defender.GetActor() || attacker == Unit() || defender == Unit())
		return;

	UnitActor *attackerActor = attacker.GetActor();
	attackerActor->SetHealthPercent(attacker.GetHP() / attacker->CalculateTotalHP());
	Army army = attacker.GetArmy();
	if (army.IsValid())
	{
		attackerActor->SetTempStackSize(army.Num());
	}

	UnitActor *defenderActor = defender.GetActor();
	defenderActor->SetHealthPercent(defender.GetHP() / defender->CalculateTotalHP());

	CellUnitList *unitList;
	unitList = g_theWorld->GetCell(defender.RetPos())->UnitArmy();
	sint32 num = unitList ? unitList->Num() : 1;
	defenderActor->SetTempStackSize(num);

	DQActionAttack *action = new DQActionAttack(
			attacker.GetOwner(),
			attackerActor,
			attacker.RetPos(),
			attacker.IsCity(),
			defender.GetActor(),
			defender.RetPos(),
			defender.IsCity());
	m_actionQueue->AddTail(action);

	Player *visiblePlayer = g_player[g_selected_item->GetVisiblePlayer()];
	if (visiblePlayer && visiblePlayer->IsVisible(attacker.RetPos()))
	{
		AddCombatFlash(attacker.RetPos());
	}

	if (visiblePlayer && visiblePlayer->IsVisible(defender.RetPos()))
	{
		AddCombatFlash(defender.RetPos());
	}
}

void DirectorImpl::AddAttackPos(Unit attacker, MapPoint const & pos)
{
	if (!attacker.GetActor())
		return;

	DQActionAttackPos *action = new DQActionAttackPos(
			attacker.GetOwner(), attacker.GetActor(), attacker.RetPos(), pos, attacker.GetAttackSoundID());
	m_actionQueue->AddTail(action);

	if (g_player[g_selected_item->GetVisiblePlayer()] &&
		g_player[g_selected_item->GetVisiblePlayer()]->IsVisible(pos))
	{
		AddCombatFlash(pos);
	}
}

void DirectorImpl::AddSpecialAttack(Unit attacker, Unit attacked, SPECATTACK attack)
{
	sint32	soundID;
	sint32	spriteID;
	if (!attacker.GetSpecialAttackInfo(attack, &soundID, &spriteID))
		return;
	if (!attacker.IsValid() || !attacker.GetActor() || !attacked.IsValid() || !attacked->GetActor())
		return;

	DQActionAttack *action = new DQActionSpecialAttack(
			attacker.GetOwner(),
			attacker.GetActor(),
			attacker.RetPos(),
			attacker.IsCity(),
			soundID,
			attacked.GetActor(),
			attacked.RetPos(),
			attacked.IsCity());
	m_actionQueue->AddTail(action);

	if (g_player[g_selected_item->GetVisiblePlayer()] &&
		g_player[g_selected_item->GetVisiblePlayer()]->IsVisible(attacked.RetPos()))
	{
		AddProjectileAttack(attacker, attacked, NULL, new SpriteState(spriteID), 0);
	}
}

void DirectorImpl::AddDeath(UnitActor *dead, const MapPoint &deadPos, sint32 deadSoundID)
{
	Assert(dead);

	DQActionDeath *action = new DQActionDeath(dead->GetPlayerNum(), dead, deadPos, deadSoundID);
	m_actionQueue->AddTail(action);

	m_dyingActors.insert(dead);
}

void DirectorImpl::AddMorphUnit(UnitActor *morphingActor, SpriteState *ss, sint32 type, Unit id)
{
	if (!morphingActor)
		return;

	DQActionMorph *action = new DQActionMorph(morphingActor, ss, type, id);
	m_actionQueue->AddTail(action);
}

void DirectorImpl::AddHide(Unit hider)
{
	UnitActor	*actor = hider.GetActor();
	Assert(actor);
	if (!actor) return;

	DQActionHideShow *action = new DQActionHide(actor);
	m_actionQueue->AddTail(action);
}

void DirectorImpl::AddShow(Unit hider)
{
	UnitActor	*actor = hider.GetActor();
	Assert(actor);
	if (!actor) return;

	DQActionHideShow *action = new DQActionShow(actor, hider.RetPos());
	m_actionQueue->AddTail(action);
}

void DirectorImpl::AddWork(Unit worker)
{
	if (!worker.GetActor())
		return;

	DQActionWork *action = new DQActionWork(
			worker.GetOwner(), worker.GetActor(), worker.RetPos(), worker.GetWorkSoundID());
	m_actionQueue->AddTail(action);
}

void DirectorImpl::AddFastKill(UnitActor *dead)
{
	Assert(dead);
	if (!dead) return;

	DQActionFastKill *action = new DQActionFastKill(dead);
	m_actionQueue->AddTail(action);
}

void DirectorImpl::AddRemoveVision(const MapPoint &pos, double range)
{
	DQActionVision *action = new DQActionRemoveVision(pos, range);
	m_actionQueue->AddTail(action);
}

void DirectorImpl::AddAddVision(const MapPoint &pos, double range)
{
	DQActionVision *action = new DQActionAddVision(pos, range);
	m_actionQueue->AddTail(action);
}

void DirectorImpl::AddSetVisibility(UnitActor *actor, uint32 visibility)
{
	Assert(actor);
	if (!actor) return;

	DQActionSetVisibility *action = new DQActionSetVisibility(actor, visibility);
	m_actionQueue->AddTail(action);
}

void DirectorImpl::AddSetOwner(UnitActor *actor, sint32 owner)
{
	Assert(actor);
	if (!actor) return;

	DQActionSetOwner *action = new DQActionSetOwner(actor, owner);
	m_actionQueue->AddTail(action);
}

void DirectorImpl::AddSetVisionRange(UnitActor *actor, double range)
{
	Assert(actor);
	if (!actor) return;

	DQActionSetVisionRange *action = new DQActionSetVisionRange(actor, range);
	m_actionQueue->AddTail(action);
}

void DirectorImpl::AddBattle(Battle *battle)
{
	if (!battle)
		return;

	DQActionBattle *action = new DQActionBattle(battle);
	m_actionQueue->AddTail(action);
}

void DirectorImpl::AddPlaySound(sint32 soundID, MapPoint const & pos)
{
	if (soundID <= 0) return;

	DQActionPlaySound *action = new DQActionPlaySound(soundID, pos);
	m_actionQueue->AddTail(action);
}

void DirectorImpl::AddGameSound(GAMESOUNDS sound)
{
	AddPlaySound(gamesounds_GetGameSoundID((sint32) sound), MapPoint());
}

void DirectorImpl::AddPlayWonderMovie(sint32 which)
{
	Assert(which >= 0);
	if (which < 0)
		return;

	DQActionPlayWonderMovie *action = new DQActionPlayWonderMovie(which);
	m_actionQueue->AddTail(action);
}

void DirectorImpl::AddPlayVictoryMovie(GAME_OVER reason, BOOL previouslyWon, BOOL previouslyLost)
{
	if (previouslyWon || previouslyLost)
	{
		PLAYER_INDEX	player = g_selected_item->GetVisiblePlayer();

		if (g_player[player] && !g_player[player]->m_isDead)
		{
			return;
		}
	}

	DQActionPlayVictoryMovie *action = new DQActionPlayVictoryMovie(reason);
	m_actionQueue->AddTail(action);
}

void DirectorImpl::AddMessage(const Message & message)
{
	DQActionMessage *action = new DQActionMessage(message);
	m_actionQueue->AddTail(action);
}

void DirectorImpl::AddFaceoff(Unit &attacker, Unit &defender)
{
	if (!attacker.IsValid() || !attacker.GetActor() || !defender.IsValid() || !defender.GetActor())
		return;

	DQActionFaceoff *action = new DQActionFaceoff(attacker.GetOwner(), attacker.GetActor(), attacker.RetPos(),
			defender.GetActor(), defender.RetPos());
	m_actionQueue->AddTail(action);
}

void DirectorImpl::AddTerminateFaceoff(Unit &faceroffer)
{
	if (!faceroffer->GetActor())
		return;

	DQActionTerminateFaceOff *action = new DQActionTerminateFaceOff(faceroffer->GetActor());
	m_actionQueue->AddTail(action);
}

void DirectorImpl::AddTerminateSound(Unit &unit)
{
	if (!unit.IsValid())
		return;

	DQActionTerminateSound *action = new DQActionTerminateSound(unit.m_id);
	m_actionQueue->AddTail(action);
}

void DirectorImpl::AddInvokeThroneRoom(void)
{
	DQActionInvokeThroneRoom *action = new DQActionInvokeThroneRoom;
	m_actionQueue->AddTail(action);
}

void DirectorImpl::AddInvokeResearchAdvance(MBCHAR * message)
{
	if (!message)
		return;

	DQActionInvokeResearchAdvance *action = new DQActionInvokeResearchAdvance(message);
	m_actionQueue->AddTail(action);
}

void DirectorImpl::AddBeginScheduler(sint32 player)
{
	Assert(player == g_selected_item->GetCurPlayer());
	if(player != g_selected_item->GetCurPlayer())
		return;

	DPRINTF(k_DBG_GAMESTATE, ("Director::AddBeginScheduler(%d)\n", player));

	DQActionBeginScheduler *action = new DQActionBeginScheduler(player);
	m_actionQueue->AddTail(action);
}


BOOL DirectorImpl::TileWillBeCompletelyVisible(sint32 x, sint32 y)
{
	RECT tempViewRect = *g_tiledMap->GetMapViewRect();

	m_actionWalker->SetList(m_actionQueue);
	while (m_actionWalker->IsValid()) {
		DQAction *action = m_actionWalker->GetObj();
		if (action->GetType() == DQACTION_CENTERMAP) {
			DQActionCenterMap *centerMap = static_cast<DQActionCenterMap*>(action);
			if (centerMap) {
				centerMap->CalculateCenteredMap(&tempViewRect);
			}
		}
		m_actionWalker->Next();
	}

	return g_tiledMap->TileIsCompletelyVisible(x, y, &tempViewRect);
}

void DirectorImpl::IncrementPendingGameActions()
{
	m_pendingGameActions++;
}

void DirectorImpl::DecrementPendingGameActions()
{
	m_pendingGameActions--;
	Assert(m_pendingGameActions >= 0);

	if(m_pendingGameActions <= 0) {
		m_pendingGameActions = 0;
		if(m_endTurnRequested) {
			Player *pl = g_player[g_selected_item->GetCurPlayer()];
			if(pl &&
			   (!g_network.IsActive() ||
				(g_network.IsLocalPlayer(g_selected_item->GetCurPlayer())))) {
				m_endTurnRequested = false;
				DPRINTF(k_DBG_GAMESTATE, ("Adding from DecrementPendingGameActions, %d\n", g_selected_item->GetCurPlayer()));
				AddEndTurn();
			}
		}
	}
}

void DirectorImpl::ReloadAllSprites()
{
	sint32 p, i;
	for(p = 0; p < k_MAX_PLAYERS; p++) {
		if(!g_player[p]) continue;
		//PFT  29 mar 05
		//cycle through human players' cities
		if(g_player[g_selected_item->GetVisiblePlayer()]->IsHuman()){
			for(i = 0; i < g_player[p]->m_all_cities->Num(); i++) {
				Unit u = g_player[p]->m_all_cities->Access(i);

				//recover the number of turns until the city next produces a pop from it's current state
				//CityData *cityData = u.GetData()->GetCityData();
				//cityData->TurnsToNextPop();

				UnitActor *actor = u.GetActor();
				u.GetSpriteState()->SetIndex(u.GetDBRec()->GetDefaultSprite()->GetValue());
				actor->ChangeImage(u.GetSpriteState(), u.GetType(), u);
			}
		}
		for(i = 0; i < g_player[p]->m_all_units->Num(); i++) {
			Unit u = g_player[p]->m_all_units->Access(i);
			UnitActor *actor = u.GetActor();
			u.GetSpriteState()->SetIndex(u.GetDBRec()->GetDefaultSprite()->GetValue());
			actor->ChangeImage(u.GetSpriteState(), u.GetType(), u);
		}
	}
}

void DirectorImpl::NotifyResync()
{
	m_endTurnRequested = false;
	m_pendingGameActions = 0;
}
