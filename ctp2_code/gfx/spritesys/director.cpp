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

// TODO: Deprecate DQItem
// TODO: combine Execute and ExecuteImmediate in a single method
// TODO: verify that all EffectActors are deleted
// TODO: verify m_currentItem finish and deletion

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

#define k_MAX_DIRECTOR_QUEUE_ITEMS	2000
#define k_MAXFRAMERATE	20

#define k_FIRSTACTOR	0
#define k_NOPROJECTILE	-1

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

class DQAction {
public:
	DQAction() {}
	virtual ~DQAction() {}
	virtual DQACTION_TYPE GetType() = 0;
	virtual bool IsTypeImmediate() { return false; }
	virtual bool IsTypeActor() { return false; }

	virtual void Execute() = 0;
	virtual void ExecuteImmediate() = 0;

	virtual bool IsActionFinished() = 0;

	virtual void Dump() = 0;
};

class DQActionActor : public DQAction
{
public:
	DQActionActor()
			: DQAction()
	{}
	virtual ~DQActionActor() {}
	virtual bool IsTypeActor() { return true; }

	virtual void Execute()
	{
		DoExecute(false);
	}

	virtual void ExecuteImmediate()
	{
		DoExecute(true);
	}

	virtual bool IsActionFinished()
	{
		return m_activeActors.empty();
	}

	void Process()
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

	void Finish()
	{
		m_activeActors.clear();
		DoFinish();
	}
protected:
	virtual void DoExecute(bool immediate) = 0;
	virtual void DoFinish() {}

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

	std::unordered_set<UnitActor*>	m_activeActors;
};

class DQItem {
public:
	DQItem(DQAction *action);
	~DQItem();

	void Process()
	{
		if (m_action->IsTypeActor()) {
			static_cast<DQActionActor *>(m_action)->Process();
		}
	}

	void Offset(sint32 deltaX, sint32 deltaY)
	{
		if (m_action->IsTypeActor()) {
			static_cast<DQActionActor *>(m_action)->Offset(deltaX, deltaY);
		}
	}

	void Draw(RECT *paintRect, sint32 layer)
	{
		if (m_action->IsTypeActor()) {
			static_cast<DQActionActor *>(m_action)->Draw(paintRect, layer);
		}
	}

	void FastKill(UnitActor *actor)
	{
		if (m_action->IsTypeActor()) {
			static_cast<DQActionActor *>(m_action)->FastKill(actor);
		}
	}

	bool IsActionFinished()
	{
		return m_action->IsActionFinished();
	}

	void Finish()
	{
		if (m_action->IsTypeActor()) {
			static_cast<DQActionActor *>(m_action)->Finish();
		}
	}

	void			SetOwner(sint32 owner) { m_owner = (sint8)owner; }
	sint32			GetOwner(void) { return (sint32) m_owner; }

	sint8			m_owner;
	uint16			m_round;
	DQAction		*m_action;
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
	bool 		TileIsVisibleToPlayer(MapPoint &pos);
	void		ActiveEffectAdd(EffectActor *effectActor);
	void		CenterMap(const MapPoint &pos);

private:
	bool    CanStartNextAction() { return m_itemQueue->GetCount() > 0 && !m_currentItem; }
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
	void	ProcessCurrentItem();
	uint32	ProcessActiveEffects();
	void	ProcessTradeRouteAnimations();

	void	FinalizeActor(UnitActor *unitActor);
	void	FastKill(EffectActor *actor);
	uint32	KillAllActiveEffects();

	void	UpdateStandbyUnits();

#ifdef _DEBUG
	void	DumpItem(DQItem *item);
#endif

	// Unused
	void	ActiveEffectRemove(EffectActor *effectActor);

	static DirectorImpl			*m_instance;
	DQItem						*m_currentItem;
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

	PointerList<DQItem>			*m_itemQueue;

	std::set<UnitActor *>		m_standbyActors;
	std::unordered_set<UnitActor *>	m_dyingActors;
	PointerList<DQItem>::Walker	*m_itemWalker;

	sint32						m_pendingGameActions;
	bool						m_endTurnRequested;
};

Director* Director::CreateDirector() {
	return new DirectorImpl();
}

class DQActionImmediate : public DQAction {
public:
	DQActionImmediate()
			: DQAction()
	{}
	virtual ~DQActionImmediate() {}
	virtual bool IsTypeImmediate() { return true; }

	virtual void Execute() { ExecuteImmediate(); }
	virtual void ExecuteImmediate()
	{
		DoExecuteImmediate();
	}
	virtual bool IsActionFinished() { return true; }
protected:
	virtual void DoExecuteImmediate() = 0;
};

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
	virtual void DoExecuteImmediate()
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

protected:
	virtual void DoExecuteImmediate()
	{
		if (DirectorImpl::Instance()->TileIsVisibleToPlayer(startPosition))
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

	virtual void Dump()
	{
		DPRINTF(k_DBG_UI, ("Morph\n"));
		DPRINTF(k_DBG_UI, ("  morphingActor      :%#x (%#.8lx)\n", morphingActor->GetUnitID(), morphingActor));
		DPRINTF(k_DBG_UI, ("  spriteState        :%#.8lx (%d)\n", spriteState, spriteState->GetIndex()));
		DPRINTF(k_DBG_UI, ("  type               :%d\n", type));
		DPRINTF(k_DBG_UI, ("  id                 :%#.8lx\n", id.m_id));
	}
protected:
	virtual void DoExecuteImmediate()
	{
		morphingActor->ChangeType(spriteState, type, id, FALSE);
	}

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

	virtual void Dump()
	{
		DPRINTF(k_DBG_UI, ("Hide\n"));
		DPRINTF(k_DBG_UI, ("  hidingActor        :%#x (%#.8lx)\n", hidingActor->GetUnitID(), hidingActor));
	}
protected:
	virtual void DoExecuteImmediate()
	{
		hidingActor->Hide();
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

	virtual void Dump() {
		DPRINTF(k_DBG_UI, ("Show\n"));
		DPRINTF(k_DBG_UI, ("  hidingActor        :%#x (%#.8lx)\n", hidingActor->GetUnitID(), hidingActor));
		DPRINTF(k_DBG_UI, ("  hidingPosition     :%d,%d\n", hidingPosition.x, hidingPosition.y));
	}
protected:
	virtual void DoExecuteImmediate()
	{
		hidingActor->PositionActor(hidingPosition);
		hidingActor->Show();
	}
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

	virtual void Dump()
	{
		DPRINTF(k_DBG_UI, ("Fast Kill\n"));
		DPRINTF(k_DBG_UI, ("  dead               :%#x (%#.8lx)\n", dead->GetUnitID(), dead));
	}
private:
	virtual void DoExecuteImmediate()
	{
		DirectorImpl::Instance()->FastKill(dead);
	}

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

	virtual void Dump()
	{
		DPRINTF(k_DBG_UI, ("Remove Vision\n"));
		DPRINTF(k_DBG_UI, ("  visionPosition     :%d,%d\n", visionPosition.x, visionPosition.y));
		DPRINTF(k_DBG_UI, ("  visionRange        :%#.2f\n", visionRange));
	}
protected:
	virtual void DoExecuteImmediate()
	{
		if(g_tiledMap) {
			g_tiledMap->RemoveVisible(visionPosition, visionRange);
		}
	}
};

class DQActionAddVision : public DQActionVision {
public:
	DQActionAddVision(const MapPoint &visionPosition, double visionRange)
			: DQActionVision(visionPosition, visionRange)
	{}
	virtual ~DQActionAddVision() {}
	virtual DQACTION_TYPE GetType() { return DQACTION_ADDVISION; }

	virtual void Dump()
	{
		DPRINTF(k_DBG_UI, ("Add Vision\n"));
		DPRINTF(k_DBG_UI, ("  visionPosition     :%d,%d\n", visionPosition.x, visionPosition.y));
		DPRINTF(k_DBG_UI, ("  visionRange        :%#.2f\n", visionRange));
	}
protected:
	virtual void DoExecuteImmediate()
	{
		if(g_tiledMap) {
			g_tiledMap->AddVisible(visionPosition, visionRange);
		}
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

	virtual void Dump()
	{
		DPRINTF(k_DBG_UI, ("Set Visibility\n"));
		DPRINTF(k_DBG_UI, ("  actor              :%#x (%#.8lx)\n", actor->GetUnitID(), actor));
		DPRINTF(k_DBG_UI, ("  flag               :%%#.8lx\n", flag));
	}
protected:
	virtual void DoExecuteImmediate()
	{
		actor->SetUnitVisibility(flag);
	}

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

	virtual void Dump()
	{
		DPRINTF(k_DBG_UI, ("Set Owner\n"));
		DPRINTF(k_DBG_UI, ("  setOwnerActor      :%#x (%#.8lx)\n", setOwnerActor->GetUnitID(), setOwnerActor));
		DPRINTF(k_DBG_UI, ("  owner              :%d\n", owner));
	}
protected:
	virtual void DoExecuteImmediate()
	{
		setOwnerActor->SetPlayerNum(owner);
	}

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

	virtual void Dump()
	{
		DPRINTF(k_DBG_UI, ("Set Vision Range\n"));
		DPRINTF(k_DBG_UI, ("  actor     :%#x (%#.8lx)\n", actor->GetUnitID(), actor));
		DPRINTF(k_DBG_UI, ("  range     :%#.2f\n", range));
	}
protected:
	virtual void DoExecuteImmediate()
	{
		actor->SetUnitVisionRange(range);
	}

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

	virtual void Dump()
	{
		DPRINTF(k_DBG_UI, ("Center Map\n"));
		DPRINTF(k_DBG_UI, ("  centerMapPosition:%d,%d\n", centerMapPosition.x, centerMapPosition.y));
	}
protected:
	virtual void DoExecuteImmediate()
	{
		if(!g_selected_item->GetIsPathing()) {
			DirectorImpl::Instance()->CenterMap(centerMapPosition);
		}
	}

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

	virtual void Dump()
	{
		DPRINTF(k_DBG_UI, ("Combat Flash\n"));
		DPRINTF(k_DBG_UI, ("  flashPosition     :%d,%d\n", flashPosition.x, flashPosition.y));
	}
protected:
	virtual void DoExecuteImmediate()
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

	virtual void Dump()
	{
		DPRINTF(k_DBG_UI, ("Copy Vision\n"));
	}
protected:
	virtual void DoExecuteImmediate()
	{
		g_tiledMap->CopyVision();
		g_radarMap->Update();
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

	virtual void Dump()
	{
		DPRINTF(k_DBG_UI, ("Select Unit\n"));
		DPRINTF(k_DBG_UI, ("  flags     :%#.8lx\n", flags));
	}
protected:
	virtual void DoExecuteImmediate()
	{
		g_selected_item->DirectorUnitSelection(flags);
	}

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

	virtual void Dump()
	{
		DPRINTF(k_DBG_UI, ("End Turn\n"));
	}
protected:
	virtual void DoExecuteImmediate()
	{
		DirectorImpl::Instance()->EndTurn();

		g_gevManager->AddEvent(GEV_INSERT_Tail, GEV_EndTurn,
							   GEA_Player, g_selected_item->GetCurPlayer(),
							   GEA_End);
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

	virtual void Dump()
	{
		DPRINTF(k_DBG_UI, ("Play Sound\n"));
		DPRINTF(k_DBG_UI, ("  soundID              :%d\n", soundID));
		DPRINTF(k_DBG_UI, ("  position             :%d,%d\n", position.x, position.y));
	}
protected:
	virtual void DoExecuteImmediate()
	{
		if (g_soundManager) {
			g_soundManager->AddSound(SOUNDTYPE_SFX, 0, soundID, position.x, position.y);
		}
	}

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

	virtual void Dump()
	{
		DPRINTF(k_DBG_UI, ("Message\n"));
	}
protected:
	virtual void DoExecuteImmediate()
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

	virtual void Dump()
	{
		DPRINTF(k_DBG_UI, ("Terminate Faceoff\n"));
		DPRINTF(k_DBG_UI, ("  faceroffer   :%#x (%#.8lx)\n", faceOffer->GetUnitID(), faceOffer));
	}
protected:
	virtual void DoExecuteImmediate()
	{
		faceOffer->SetHealthPercent(-1.0);
		faceOffer->SetTempStackSize(0);

		// This is commented out as currently AddFaceoff is not called and thereby ActiveUnitAdd is not called.
		// DirectorImpl::Instance()->ActiveUnitRemove(faceOffer);
	}

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

	virtual void Dump()
	{
		DPRINTF(k_DBG_UI, ("Terminate Sound\n"));
		DPRINTF(k_DBG_UI, ("  terminate_sound_unit    :%#.8lx\n", terminateSoundUnitID));
	}
protected:
	virtual void DoExecuteImmediate()
	{
		if (g_soundManager) {
			g_soundManager->TerminateLoopingSound(SOUNDTYPE_SFX, terminateSoundUnitID);
		}
	}

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

	virtual void Dump()
	{
		DPRINTF(k_DBG_UI, ("Special Effect\n"));
		DPRINTF(k_DBG_UI, ("  position             :%d,%d\n", position.x, position.y));
		DPRINTF(k_DBG_UI, ("  spriteID             :%d\n", spriteID));
		DPRINTF(k_DBG_UI, ("  soundID              :%d\n", soundID));
	}
protected:
	virtual void DoExecuteImmediate()
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

	virtual void Dump()
	{
		DPRINTF(k_DBG_UI, ("Invoke Throne-room\n"));
	}
protected:
	virtual void DoExecuteImmediate() {}
};

class DQActionMove : public DQActionActor
{
public:
	DQActionMove(
			UnitActor		*moveActor,
			const MapPoint	&startPosition,
			const MapPoint	&endPosition,
			sint32			numberOfMoveActors,
			UnitActor		**moveActors,
			sint32			numberOfRevealedActors,
			UnitActor		**revealedActors,
			sint32			soundID)
			: DQActionActor(),
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
	virtual ~DQActionMove() {
		if (!moveActor->IsActive()) {
			g_soundManager->TerminateLoopingSound(SOUNDTYPE_SFX, moveActor->GetUnitID());
		}
		delete [] moveActors;
	}
	virtual DQACTION_TYPE GetType() { return DQACTION_MOVE; }

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
	virtual void DoExecute(bool immediate)
	{
		if (g_theUnitPool && !g_theUnitPool->IsValid(moveActor->GetUnitID())) {
			return;
		}

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

		bool visible = DirectorImpl::Instance()->TileIsVisibleToPlayer(startPosition)
					   || DirectorImpl::Instance()->TileIsVisibleToPlayer(endPosition);

		if (visible && !immediate)
		{
			if (g_selected_item->GetVisiblePlayer()!= moveActor->GetPlayerNum()
				&& !g_tiledMap->TileIsVisible(moveActor->GetPos().x, moveActor->GetPos().y))
			{
				DirectorImpl::Instance()->CenterMap(moveActor->GetPos());
			}
		}
		if (DirectorImpl::Instance()->HandleActor(immediate, moveActor, visible)) {
			AddActiveActor(moveActor);
		}
	}

	virtual void DoFinish() {
		moveActor->PositionActor(endPosition);

		for (int i = 0; i < numberOfMoveActors; i++)
		{
			moveActors[i]->PositionActor(endPosition);
		}
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

class DQActionAttack : public DQActionActor
{
public:
	DQActionAttack(
			UnitActor		*attacker,
			const MapPoint	&attackerPosition,
			bool			attackerIsCity,
			UnitActor		*defender,
			const MapPoint	&defenderPosition,
			bool			defenderIsCity
	)
			: DQActionActor(),
			  attacker			(attacker),
			  attackerPosition	(attackerPosition),
			  attackerIsCity	(attackerIsCity),
			  defender			(defender),
			  defenderPosition	(defenderPosition),
			  defenderIsCity	(defenderIsCity)
	{}
	virtual ~DQActionAttack() {}
	virtual DQACTION_TYPE GetType() { return DQACTION_ATTACK; }

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
	virtual void DoExecute(bool immediate)
	{
		//	bool attackerVisible = DirectorImpl::Instance()->TileIsVisibleToPlayer(attackerPos);
		bool defenderVisible = DirectorImpl::Instance()->TileIsVisibleToPlayer(defenderPosition);

		bool playerInvolved  = (defender->GetPlayerNum() == g_selected_item->GetVisiblePlayer()) ||
							   (attacker->GetPlayerNum() == g_selected_item->GetVisiblePlayer());

		POINT  AttackerPoints, DefenderPoints;

		maputils_MapXY2PixelXY(attackerPosition.x, attackerPosition.y, AttackerPoints);
		maputils_MapXY2PixelXY(defenderPosition.x, defenderPosition.y, DefenderPoints);

		sint32  deltax = DefenderPoints.x - AttackerPoints.x;
		sint32  deltay = DefenderPoints.y - AttackerPoints.y;

		sint32 facingIndex = spriteutils_DeltaToFacing(deltax, deltay);

		Action *action = new Action();
		action->SetStartMapPoint(attackerPosition);
		action->SetEndMapPoint  (attackerPosition);

		attacker->ActionAttack(action, facingIndex);

		if (DirectorImpl::Instance()->HandleActor(immediate, attacker, playerInvolved)) {
			AddActiveActor(attacker);
		}

		if (!defenderIsCity)
		{
			facingIndex = spriteutils_DeltaToFacing(-deltax, -deltay);

			action = new Action();
			action->SetStartMapPoint(defenderPosition);
			action->SetEndMapPoint  (defenderPosition);

			defender->ActionAttack(action, facingIndex);

			if (playerInvolved) {
				defenderVisible = true;
			}
			if (DirectorImpl::Instance()->HandleActor(immediate, defender, defenderVisible)) {
				AddActiveActor(defender);
			}
		}
	}

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
			UnitActor		*attacker,
			const MapPoint	&attackerPosition,
			bool			attackerIsCity,
			sint32			attackerSoundID,
			UnitActor		*defender,
			const MapPoint	&defenderPosition,
			bool			defenderIsCity)
			: DQActionAttack(attacker, attackerPosition, attackerIsCity, defender, defenderPosition, defenderIsCity),
			  attackerSoundID	(attackerSoundID)
	{}
	virtual ~DQActionSpecialAttack() {}
	virtual DQACTION_TYPE GetType() { return DQACTION_SPECATTACK; }

	virtual void Dump()
	{
		DQActionAttack::Dump();
		DPRINTF(k_DBG_UI, ("  attackerSoundID    ::%d\n",       attackerSoundID));
	}

protected:
	virtual MBCHAR const * const GetName() { return "Special attack"; }

	virtual void DoExecute(bool immediate)
	{
		BOOL attackerCanAttack = !attackerIsCity;
		BOOL defenderIsAttackable = !defenderIsCity;

		if (!attackerCanAttack && !defenderIsAttackable)
		{
			return;
		}

		sint32 facingIndex;
		POINT  AttackerPoints, DefenderPoints;
		maputils_MapXY2PixelXY(attackerPosition.x, attackerPosition.y, AttackerPoints);
		maputils_MapXY2PixelXY(defenderPosition.x, defenderPosition.y, DefenderPoints);

		sint32  deltax = DefenderPoints.x - AttackerPoints.x;
		sint32  deltay = DefenderPoints.y - AttackerPoints.y;

		if (attackerSoundID >= 0) {
			g_soundManager->AddSound(SOUNDTYPE_SFX, 0, attackerSoundID, 0, 0);
		}

		if (attackerCanAttack)
		{
			Action *attackerAction = new Action();
			attackerAction->SetStartMapPoint(attackerPosition);
			attackerAction->SetEndMapPoint  (attackerPosition);

			facingIndex = spriteutils_DeltaToFacing(deltax, deltay);

			if(!attacker->ActionSpecialAttack(attackerAction, facingIndex))
			{
				delete attackerAction;
				return;
			}

			bool visible = DirectorImpl::Instance()->TileIsVisibleToPlayer(attackerPosition);
			if (DirectorImpl::Instance()->HandleActor(immediate, attacker, visible)) {
				AddActiveActor(attacker);
			}
		}

		if (defenderIsAttackable)
		{
			Action *defenderAction = new Action();

			defenderAction->SetStartMapPoint(defenderPosition);
			defenderAction->SetEndMapPoint  (defenderPosition);

			facingIndex=spriteutils_DeltaToFacing(-deltax, -deltay);

			if(!defender->ActionSpecialAttack(defenderAction, facingIndex))
			{
				delete defenderAction;
				return;
			}

			bool visible = DirectorImpl::Instance()->TileIsVisibleToPlayer(defenderPosition);
			if (DirectorImpl::Instance()->HandleActor(immediate, defender, visible)) {
				AddActiveActor(defender);
			}
		}
	}

	sint32	attackerSoundID;
};

class DQActionAttackPos : public DQActionActor
{
public:
	DQActionAttackPos(
			UnitActor *attacker,
			const MapPoint &attackerPosition,
			const MapPoint &targetPosition,
			sint32 soundID)
			: DQActionActor(),
			  attacker			(attacker),
			  attackerPosition	(attackerPosition),
			  targetPosition	(targetPosition),
			  soundID			(soundID)
	{}
	virtual ~DQActionAttackPos() {}
	virtual DQACTION_TYPE GetType() { return DQACTION_ATTACKPOS; }

	virtual void Dump()
	{
		DPRINTF(k_DBG_UI, ("Attack Pos\n"));
		DPRINTF(k_DBG_UI, ("  attacker               :%#x (%#.8lx)\n", attacker->GetUnitID(), attacker));
		DPRINTF(k_DBG_UI, ("  attackerPosition       :%d,%d\n", attackerPosition.x, attackerPosition.y));
		DPRINTF(k_DBG_UI, ("  targetPosition         :%d,%d\n", targetPosition.x, targetPosition.y));
		DPRINTF(k_DBG_UI, ("  soundID                :%d\n", soundID));
	}
protected:
	virtual void DoExecute(bool immediate)
	{
		Action *action = new Action(UNITACTION_ATTACK, ACTIONEND_ANIMEND,
									attacker->GetHoldingCurAnimPos(UNITACTION_ATTACK),
									attacker->GetHoldingCurAnimSpecialDelayProcess(UNITACTION_ATTACK));
		action->SetStartMapPoint(attackerPosition);
		action->SetEndMapPoint(attackerPosition);

		if (attacker->GetLoadType() != LOADTYPE_FULL) {
			attacker->FullLoad(UNITACTION_ATTACK);
		}

		Anim	*attackerAnim = attacker->CreateAnim(UNITACTION_ATTACK);
		if (!attackerAnim) {
			attackerAnim = attacker->CreateAnim(UNITACTION_IDLE);
		}
		action->SetAnim(attackerAnim);

		POINT AttackerPoints, AttackedPoints;
		maputils_MapXY2PixelXY(attackerPosition.x, attackerPosition.y, AttackerPoints);
		maputils_MapXY2PixelXY(targetPosition.x, targetPosition.y, AttackedPoints);

		action->SetFacing(spriteutils_DeltaToFacing(AttackedPoints.x - AttackerPoints.x,
													AttackedPoints.y - AttackerPoints.y));

		action->SetUnitVisionRange(attacker->GetUnitVisionRange());
		action->SetUnitsVisibility(attacker->GetUnitVisibility());

		if (g_soundManager) {
			sint32 visiblePlayer = g_selected_item->GetVisiblePlayer();
			if ((visiblePlayer == attacker->GetPlayerNum()) ||
				(attacker->GetUnitVisibility() & (1 << visiblePlayer)))
			{
				g_soundManager->AddSound(SOUNDTYPE_SFX, attacker->GetUnitID(), soundID,
										 attackerPosition.x, attackerPosition.y);
			}
		}

		attacker->AddAction(action);

		bool attackerVisible = DirectorImpl::Instance()->TileIsVisibleToPlayer(attackerPosition);
		if (DirectorImpl::Instance()->HandleActor(immediate, attacker, attackerVisible)) {
			AddActiveActor(attacker);
		}
	}

	UnitActor	*attacker;
	MapPoint	attackerPosition;
	MapPoint	targetPosition;
	sint32		soundID;
};

class DQActionDeath : public DQActionActor
{
public:
	DQActionDeath(UnitActor *dead, const MapPoint &deadPosition, sint32 deadSoundID)
			: DQActionActor(),
			  dead			(dead),
			  deadPosition	(deadPosition),
			  deadSoundID	(deadSoundID)
	{}
	virtual ~DQActionDeath() {}
	virtual DQACTION_TYPE GetType() { return DQACTION_DEATH; }

	virtual void Dump()
	{
		DPRINTF(k_DBG_UI, ("Death\n"));
		DPRINTF(k_DBG_UI, ("  dead               :%#x (%#.8lx)\n", dead->GetUnitID(), dead));
		DPRINTF(k_DBG_UI, ("  deadPosition       :%d,%d\n", deadPosition.x, deadPosition.y));
		DPRINTF(k_DBG_UI, ("  deadSoundID        :%d\n", deadSoundID));
	}
protected:
	virtual void DoExecute(bool immediate)
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

		if (g_soundManager) {
			sint32 visiblePlayer = g_selected_item->GetVisiblePlayer();
			if ((visiblePlayer == dead->GetPlayerNum()) ||
				(dead->GetUnitVisibility() & (1 << visiblePlayer))) {
				g_soundManager->AddSound(SOUNDTYPE_SFX, (uint32)dead->GetUnitID(), deadSoundID,
										 deadPosition.x, deadPosition.y);
			}
		}

		dead->AddAction(action);

		bool visible = DirectorImpl::Instance()->TileIsVisibleToPlayer(deadPosition);
		if (DirectorImpl::Instance()->HandleActor(immediate, dead, visible)) {
			AddActiveActor(dead);
		}
	}

	UnitActor	*dead;
	MapPoint	deadPosition;
	sint32		deadSoundID;
};

class DQActionWork : public DQActionActor
{
public:
	DQActionWork(UnitActor *workActor, const MapPoint &workPosition, sint32 workSoundID)
			: DQActionActor(),
			  workActor		(workActor),
			  workPosition	(workPosition),
			  workSoundID	(workSoundID)
	{}
	virtual ~DQActionWork() {}
	virtual DQACTION_TYPE GetType() { return DQACTION_WORK; }

	virtual void Dump()
	{
		DPRINTF(k_DBG_UI, ("Work\n"));
		DPRINTF(k_DBG_UI, ("  workActor          :%#x (%#.8lx)\n", workActor->GetUnitID(), workActor));
		DPRINTF(k_DBG_UI, ("  workPosition       :%d,%d\n", workPosition.x, workPosition.y));
		DPRINTF(k_DBG_UI, ("  workSoundID        :%d\n", workSoundID));
	}
protected:
	virtual void DoExecute(bool immediate)
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

		if (g_soundManager)
		{
			sint32 visiblePlayer = g_selected_item->GetVisiblePlayer();
			if ((visiblePlayer == workActor->GetPlayerNum()) ||
				(workActor->GetUnitVisibility() & (1 << visiblePlayer))) {
				g_soundManager->AddSound(SOUNDTYPE_SFX, workActor->GetUnitID(), workSoundID, workPosition.x, workPosition.y);
			}
		}

		bool visible = DirectorImpl::Instance()->TileIsVisibleToPlayer(workPosition);
		if (DirectorImpl::Instance()->HandleActor(immediate, workActor, visible)) {
			AddActiveActor(workActor);
		}
	}

	UnitActor	*workActor;
	MapPoint	workPosition;
	sint32		workSoundID;
};

class DQActionFaceoff : public DQActionActor
{
public:
	DQActionFaceoff(
			UnitActor 		*attacker,
			const MapPoint	&attackerPosition,
			UnitActor		*attacked,
			const MapPoint	&attackedPosition)
			: DQActionActor(),
			  attacker			(attacker),
			  attackerPosition	(attackerPosition),
			  attacked			(attacked),
			  attackedPosition	(attackedPosition)
	{}
	virtual ~DQActionFaceoff() {}
	virtual DQACTION_TYPE GetType() { return DQACTION_FACEOFF; }

	virtual void Dump()
	{
		DPRINTF(k_DBG_UI, ("Faceoff\n"));
		DPRINTF(k_DBG_UI, ("  attacker           :%#x (%#.8lx)\n", attacker->GetUnitID(), attacker));
		DPRINTF(k_DBG_UI, ("  attackerPosition   :%d,%d\n", attackerPosition.x, attackerPosition.y));
		DPRINTF(k_DBG_UI, ("  attacked           :%#x (%#.8lx)\n", attacked->GetUnitID(), attacked));
		DPRINTF(k_DBG_UI, ("  attackedPosition   :%d,%d\n", attackedPosition.x, attackedPosition.y));
	}
protected:
	virtual void DoExecute(bool immediate)
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

		POINT AttackerPoints, AttackedPoints;

		maputils_MapXY2PixelXY(attackerPosition.x, attackerPosition.y, AttackerPoints);
		maputils_MapXY2PixelXY(attackedPosition.x, attackedPosition.y, AttackedPoints);

		attackerAction->SetFacing(spriteutils_DeltaToFacing(
				AttackedPoints.x - AttackerPoints.x,AttackedPoints.y - AttackerPoints.y));

		if(attackedAnim)
		{
			attackedAction->SetAnim(attackedAnim);
			attackedAction->SetFacing(spriteutils_DeltaToFacing(
					AttackerPoints.x - AttackedPoints.x, AttackerPoints.y - AttackedPoints.y));
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

		bool attackedVisible = DirectorImpl::Instance()->TileIsVisibleToPlayer(attackedPosition);
		if (attacker->GetPlayerNum() == g_selected_item->GetVisiblePlayer() ||
			attacked->GetPlayerNum() == g_selected_item->GetVisiblePlayer()) {
			attackedVisible = true;
		}

		if (DirectorImpl::Instance()->HandleActor(immediate, attacked, attackedVisible)) {
			AddActiveActor(attacked);
		}

		bool attackerVisible = DirectorImpl::Instance()->TileIsVisibleToPlayer(attackerPosition);
		if (attacked->GetPlayerNum() == g_selected_item->GetVisiblePlayer() ||
			attacker->GetPlayerNum() == g_selected_item->GetVisiblePlayer()) {
			attackerVisible = true;
			attackedVisible = true;
		}
		if (DirectorImpl::Instance()->HandleActor(
				immediate, attacker, attackerVisible && attackedVisible)) {
			AddActiveActor(attacker);
		}
	}

	UnitActor	*attacker;
	MapPoint	attackerPosition;
	UnitActor	*attacked;
	MapPoint	attackedPosition;
};

class DQActionExternal : public DQAction
{
public:
	DQActionExternal()
	: DQAction()
	{}
	virtual ~DQActionExternal() {}

	virtual void ExecuteImmediate()
	{
		// External dependency so no immediate execution available
		Execute();
	}
	virtual bool IsActionFinished() { return false; }
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
		if(g_battleViewWindow)
		{
			BattleViewWindow::Cleanup();
		}

		BattleViewWindow::Initialize();

		if(g_battleViewWindow)
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

DQItem::DQItem(DQAction *action)
:
	m_owner             (PLAYER_UNASSIGNED),
	m_round             (0),
	m_action            (action)
{
	if (g_turn)
	{
		m_round = static_cast<uint16>(g_turn->GetRound());
	}
}

DQItem::~DQItem()
{
	//DPRINTF(k_DBG_GAMESTATE, ("Deleting item @ %lx, type=%d\n", this, m_type));
	delete m_action;
}

#define k_MAX_DISPATCHED_QUEUE_ITEMS		1000

DirectorImpl *DirectorImpl::m_instance = NULL;
DirectorImpl::DirectorImpl(void)
:
	m_currentItem               (NULL),
	m_activeEffectList          (new tech_WLList<EffectActor *>),
	m_tradeActorList            (new tech_WLList<TradeActor *>),
	m_masterCurTime             (0),
	m_lastTickCount             (0),
	m_timeLogIndex              (0),
	m_averageElapsed            (0),
	m_averageFPS                (k_DEFAULT_FPS),
	m_paused                    (FALSE),
	m_processingActiveEffects   (FALSE),
	m_itemQueue                 (new PointerList<DQItem>),
	m_itemWalker                (new PointerList<DQItem>::Walker),
	m_pendingGameActions        (0),
	m_endTurnRequested          (false)
{
	std::fill(m_timeLog, m_timeLog + k_TIME_LOG_SIZE, 0);

	delete m_instance;
	m_instance = this;
}

DirectorImpl::~DirectorImpl(void)
{
	delete m_itemQueue;
	delete m_itemWalker;
	delete m_activeEffectList;
	delete m_tradeActorList;
	delete m_currentItem;
	m_instance = NULL;
}

void DirectorImpl::FastKill(UnitActor *actor)
{
	if (m_currentItem) {
		m_currentItem->FastKill(actor);
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
	if (!m_standbyActors.empty() || m_itemQueue->IsEmpty())
		return;

	PointerList<DQItem>::Walker walk(m_itemQueue);
	for (; walk.IsValid(); walk.Next()) {
		UnitActor *standbyActor = NULL;
		switch (walk.GetObj()->m_action->GetType()) {
			case DQACTION_MOVE:
				standbyActor = ((DQActionMove *) (walk.GetObj()->m_action))->moveActor;
				break;
			case DQACTION_TELEPORT:
				standbyActor = ((DQActionTeleport *) (walk.GetObj()->m_action))->moveActor;
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
		DQItem *currentItem = m_currentItem;
		ProcessStandbyUnits();
		ProcessCurrentItem();
		ProcessActiveEffects();
		ProcessTradeRouteAnimations();

		HandleNextAction();
		if (m_currentItem != currentItem) {
			delete currentItem;
		}

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

void DirectorImpl::DumpItem(DQItem *item)
{
	if (item) {
		item->m_action->Dump();
	} else {
		DPRINTF(k_DBG_UI, ("Item is null\n"));
	}
}

#endif

#ifdef _DEBUG

void DirectorImpl::DumpInfo(void)
{
	DPRINTF(k_DBG_UI, (" ------------------\n"));
	DPRINTF(k_DBG_UI, ("Director Dump:\n"));
	DPRINTF(k_DBG_UI, (" ------------------\n"));
	DPRINTF(k_DBG_UI, (" Current Item:\n"));
	DumpItem(m_currentItem);
	DPRINTF(k_DBG_UI, (" ------------------\n"));
	DPRINTF(k_DBG_UI, (" Queued Items:\n"));
	DPRINTF(k_DBG_UI, (" Count:%d\n", m_itemQueue->GetCount()));
	DPRINTF(k_DBG_UI, (" ------------------\n"));

	for (
		m_itemWalker->SetList(m_itemQueue);
		m_itemWalker->IsValid();
		m_itemWalker->Next()
		)
	{
		DumpItem(m_itemWalker->GetObj());
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
		// ActiveUnitAdd(actor);
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
	 	m_currentItem = m_itemQueue->RemoveHead();

		if (
			m_currentItem->m_action->IsTypeImmediate() ||
			m_currentItem->m_round < g_turn->GetRound() - 1
			||
			(
				!g_theProfileDB->IsEnemyMoves()
				&&  m_currentItem->GetOwner() != -1
				&&  m_currentItem->GetOwner() != g_selected_item->GetVisiblePlayer()
			)
			||
			(
				!g_theProfileDB->IsUnitAnim()
				&&  m_currentItem->GetOwner() != -1
				&&  g_player[m_currentItem->GetOwner()] != NULL
				&&  g_player[m_currentItem->GetOwner()]->IsRobot()
			)
			)
		{
			m_currentItem->m_action->ExecuteImmediate();
			m_currentItem->Finish();
			delete m_currentItem;
			m_currentItem = NULL;
		} else {
			m_currentItem->m_action->Execute();
			if (m_currentItem->IsActionFinished()) {
				m_currentItem->Finish();
				delete m_currentItem;
				m_currentItem = NULL;
			}
		}
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

	if (m_currentItem && m_currentItem->m_action->GetType() == actionType) {
		delete m_currentItem;
		m_currentItem = NULL;
	}
}

void DirectorImpl::CatchUp(void)
{
	if (m_currentItem) {
		m_currentItem->Finish();
		delete m_currentItem;
		m_currentItem = NULL;
	}

	while(m_itemQueue->GetCount())
	{
		DQItem *item = m_itemQueue->RemoveHead();
		item->m_action->ExecuteImmediate();
		item->Finish();
		delete item;
	}

	if (!g_network.IsActive()) {

		KillAllActiveEffects();
		if (g_soundManager)
			g_soundManager->TerminateAllLoopingSounds(SOUNDTYPE_SFX);
	}
}

bool DirectorImpl::CaughtUp(void)
{
	return !m_itemQueue || (m_itemQueue->GetCount() == 0);
}

void DirectorImpl::CenterMap(const MapPoint &pos)
{
	g_radarMap->CenterMap(pos);
	g_tiledMap->Refresh();
	g_tiledMap->InvalidateMap();
	g_tiledMap->InvalidateMix();
	background_draw_handler(g_background);
}

bool DirectorImpl::TileIsVisibleToPlayer(MapPoint &pos)
{
#if defined(_PLAYTEST)
	if (g_doingFastRounds) return false;
#endif

	return g_tiledMap && g_tiledMap->GetLocalVision()->IsVisible(pos);
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

void DirectorImpl::ProcessCurrentItem(void)
{
	if (m_currentItem) {
		m_currentItem->Process();
		if (m_currentItem->IsActionFinished()) {
			m_currentItem->Finish();
			m_currentItem = NULL;
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
	if (m_currentItem) {
		m_currentItem->Offset(deltaX, deltaY);
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
		if (m_currentItem) {
			m_currentItem->Finish();
			delete m_currentItem;
			m_currentItem = NULL;
		}
	}

	while(m_itemQueue->GetCount())
	{
		DQItem *item = m_itemQueue->RemoveHead();
		item->m_action->ExecuteImmediate();
		item->Finish();
		delete item;
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
	if (m_currentItem) {
		m_currentItem->Draw(paintRect, layer);
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

	DQActionMove		*action = new DQActionMove(
			actor,
			oldPos,
			newPos,
			numRest,
			numRest > 0 ? restOfStack : NULL,
			numRevealed,
			numRevealed > 0 ? revealedActors : NULL,
			soundID);
	DQItem				*item = new DQItem(action);
	item->SetOwner(mover.GetOwner());

	m_itemQueue->AddTail(item);
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

	DQActionTeleport	*action = new DQActionTeleport(
			top->GetActor(),
			oldPos,
			newPos,
			arraySize,
			moveActors,
			numRevealed,
			revealedActors);
	DQItem				*item	= new DQItem(action);

	m_itemQueue->AddTail(item);
}

void DirectorImpl::AddProjectileAttack(Unit shooting, Unit target, SpriteState *projectile_state,
									   SpriteState *projectileEnd_state, sint32 projectile_Path)
{
	DQActionMoveProjectile	*action = new DQActionMoveProjectile(
			projectileEnd_state, shooting.RetPos(), target.RetPos());
	DQItem					*item = new DQItem(action);

	m_itemQueue->AddTail(item);
}

void DirectorImpl::AddSpecialEffect(MapPoint &pos, sint32 spriteID, sint32 soundID)
{
	DQActionSpecialEffect	*action = new DQActionSpecialEffect(pos, spriteID, soundID);
	DQItem					*item = new DQItem(action);

	m_itemQueue->AddTail(item);
}

void DirectorImpl::AddCombatFlash(MapPoint const & pos)
{
	DQActionCombatFlash	*   action  = new DQActionCombatFlash(pos);
	DQItem *                item    = new DQItem(action);

	m_itemQueue->AddTail(item);
}

void DirectorImpl::AddCopyVision(void)
{
	DQActionCopyVision	*action = new DQActionCopyVision();
	DQItem				*item = new DQItem(action);

	m_itemQueue->AddTail(item);
}

void DirectorImpl::AddCenterMap(const MapPoint &pos)
{
	DQActionCenterMap	*action = new DQActionCenterMap(pos);
	DQItem				*item = new DQItem(action);

	m_itemQueue->AddTail(item);
}

void DirectorImpl::AddSelectUnit(uint32 flags)
{
	DQActionUnitSelection	*action = new DQActionUnitSelection(flags);
	DQItem				*item = new DQItem(action);

	m_itemQueue->AddTail(item);
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
		PointerList<DQItem>::Walker walk(m_itemQueue);
		for(; walk.IsValid(); walk.Next()) {
			if(walk.GetObj()->m_action->GetType() == DQACTION_ENDTURN) {
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

	DQActionEndTurn		*action = new DQActionEndTurn();
	DQItem				*item = new DQItem(action);

	m_itemQueue->AddTail(item);
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

	DQActionAttack		*action = new DQActionAttack(
			attackerActor,
			attacker.RetPos(),
			attacker.IsCity(),
			defender.GetActor(),
			defender.RetPos(),
			defender.IsCity());
	DQItem				*item = new DQItem(action);
	item->SetOwner(attacker.GetOwner());

	m_itemQueue->AddTail(item);

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

	DQActionAttackPos	*action = new DQActionAttackPos(
			attacker.GetActor(), attacker.RetPos(), pos, attacker.GetAttackSoundID());
	DQItem *item = new DQItem(action);
	item->SetOwner(attacker.GetOwner());
	m_itemQueue->AddTail(item);

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

	DQActionAttack	*action = new DQActionSpecialAttack(
			attacker.GetActor(),
			attacker.RetPos(),
			attacker.IsCity(),
			soundID,
			attacked.GetActor(),
			attacked.RetPos(),
			attacked.IsCity());

	DQItem *            item            = new DQItem(action);
	item->SetOwner(attacker.GetOwner());

	m_itemQueue->AddTail(item);

	if (g_player[g_selected_item->GetVisiblePlayer()] &&
		g_player[g_selected_item->GetVisiblePlayer()]->IsVisible(attacked.RetPos()))
	{
		AddProjectileAttack(attacker, attacked, NULL, new SpriteState(spriteID), 0);
	}
}

void DirectorImpl::AddDeath(UnitActor *dead, const MapPoint &deadPos, sint32 deadSoundID)
{
	Assert(dead);

	DQActionDeath	*action = new DQActionDeath(dead, deadPos, deadSoundID);
	DQItem			*item = new DQItem(action);
	item->SetOwner(dead->GetPlayerNum());

	m_dyingActors.insert(dead);
	m_itemQueue->AddTail(item);
}

void DirectorImpl::AddMorphUnit(UnitActor *morphingActor, SpriteState *ss, sint32 type, Unit id)
{
	if (!morphingActor)
		return;

	DQActionMorph	*action = new DQActionMorph(morphingActor, ss, type, id);
	DQItem			*item = new DQItem(action);
	m_itemQueue->AddTail(item);
}

void DirectorImpl::AddHide(Unit hider)
{
	UnitActor	*actor = hider.GetActor();
	Assert(actor);
	if (!actor) return;

	DQActionHideShow	*action = new DQActionHide(actor);
	DQItem				*item = new DQItem(action);

	m_itemQueue->AddTail(item);
}

void DirectorImpl::AddShow(Unit hider)
{
	UnitActor	*actor = hider.GetActor();
	Assert(actor);
	if (!actor) return;

	DQActionHideShow	*action = new DQActionShow(actor, hider.RetPos());
	DQItem				*item = new DQItem(action);

	m_itemQueue->AddTail(item);
}

void DirectorImpl::AddWork(Unit worker)
{
	if (!worker.GetActor())
		return;

	DQActionWork	*action = new DQActionWork(worker.GetActor(), worker.RetPos(), worker.GetWorkSoundID());
	DQItem			*item   = new DQItem(action);
	item->SetOwner(worker.GetOwner());

	m_itemQueue->AddTail(item);
}

void DirectorImpl::AddFastKill(UnitActor *dead)
{
	Assert(dead);
	if (!dead) return;

	DQActionFastKill	*action = new DQActionFastKill(dead);
	DQItem				*item = new DQItem(action);

	m_itemQueue->AddTail(item);
}

void DirectorImpl::AddRemoveVision(const MapPoint &pos, double range)
{
	DQActionVision		*action = new DQActionRemoveVision(pos, range);
	DQItem				*item = new DQItem(action);

	m_itemQueue->AddTail(item);
}

void DirectorImpl::AddAddVision(const MapPoint &pos, double range)
{
	DQActionVision *    action  = new DQActionAddVision(pos, range);
	DQItem				*item = new DQItem(action);

	m_itemQueue->AddTail(item);
}

void DirectorImpl::AddSetVisibility(UnitActor *actor, uint32 visibility)
{
	Assert(actor);
	if (!actor) return;

	DQActionSetVisibility * action  = new DQActionSetVisibility(actor, visibility);
	DQItem				*item = new DQItem(action);

	m_itemQueue->AddTail(item);
}

void DirectorImpl::AddSetOwner(UnitActor *actor, sint32 owner)
{
	Assert(actor);
	if (!actor) return;

	DQActionSetOwner *  action  = new DQActionSetOwner(actor, owner);
	DQItem				*item = new DQItem(action);

	m_itemQueue->AddTail(item);
}

void DirectorImpl::AddSetVisionRange(UnitActor *actor, double range)
{
	Assert(actor);
	if (!actor) return;

	DQActionSetVisionRange *    action  = new DQActionSetVisionRange(actor, range);
	DQItem				*item = new DQItem(action);

	m_itemQueue->AddTail(item);
}

void DirectorImpl::AddBattle(Battle *battle)
{
	if (!battle)
		return;

	DQActionBattle *    action  = new DQActionBattle(battle);
	DQItem				*item = new DQItem(action);

	m_itemQueue->AddTail(item);
}

void DirectorImpl::AddPlaySound(sint32 soundID, MapPoint const & pos)
{
	if (soundID <= 0) return;

	DQActionPlaySound * action  = new DQActionPlaySound(soundID, pos);
	DQItem				*item = new DQItem(action);

	m_itemQueue->AddTail(item);
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

	DQActionPlayWonderMovie	*   action  = new DQActionPlayWonderMovie(which);
	DQItem				*item = new DQItem(action);

	m_itemQueue->AddTail(item);
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

	DQActionPlayVictoryMovie *  action  = new DQActionPlayVictoryMovie(reason);
	DQItem				*item = new DQItem(action);

	m_itemQueue->AddTail(item);
}

void DirectorImpl::AddMessage(const Message & message)
{
	DQActionMessage	*   action  = new DQActionMessage(message);
	DQItem				*item = new DQItem(action);

	m_itemQueue->AddTail(item);
}

void DirectorImpl::AddFaceoff(Unit &attacker, Unit &defender)
{
	if (!attacker.IsValid() || !attacker.GetActor() || !defender.IsValid() || !defender.GetActor())
		return;

	DQActionFaceoff	*   action  = new DQActionFaceoff(attacker.GetActor(), attacker.RetPos(),
			defender.GetActor(), defender.RetPos());
	DQItem						*item = new DQItem(action);

	m_itemQueue->AddTail(item);
}

void DirectorImpl::AddTerminateFaceoff(Unit &faceroffer)
{
	if (!faceroffer->GetActor())
		return;

	DQActionTerminateFaceOff	*action = new DQActionTerminateFaceOff(faceroffer->GetActor());
	DQItem						*item = new DQItem(action);

	m_itemQueue->AddTail(item);
}

void DirectorImpl::AddTerminateSound(Unit &unit)
{
	if (!unit.IsValid())
		return;

	DQActionTerminateSound *    action  = new DQActionTerminateSound(unit.m_id);
	DQItem						*item = new DQItem(action);

	m_itemQueue->AddTail(item);
}

void DirectorImpl::AddInvokeThroneRoom(void)
{
	DQActionInvokeThroneRoom *  action = new DQActionInvokeThroneRoom;
	DQItem						*item = new DQItem(action);

	m_itemQueue->AddTail(item);
}

void DirectorImpl::AddInvokeResearchAdvance(MBCHAR * message)
{
	if (!message)
		return;

	DQActionInvokeResearchAdvance * action = new DQActionInvokeResearchAdvance(message);
	DQItem						*item = new DQItem(action);

	m_itemQueue->AddTail(item);
}

void DirectorImpl::AddBeginScheduler(sint32 player)
{
	Assert(player == g_selected_item->GetCurPlayer());
	if(player != g_selected_item->GetCurPlayer())
		return;

	DPRINTF(k_DBG_GAMESTATE, ("Director::AddBeginScheduler(%d)\n", player));

	DQActionBeginScheduler *    action = new DQActionBeginScheduler(player);
	DQItem						*item = new DQItem(action);

	m_itemQueue->AddTail(item);
}


BOOL DirectorImpl::TileWillBeCompletelyVisible(sint32 x, sint32 y)
{
	RECT tempViewRect = *g_tiledMap->GetMapViewRect();

	m_itemWalker->SetList(m_itemQueue);
	while (m_itemWalker->IsValid()) {
		DQItem	*item = m_itemWalker->GetObj();
		if (item->m_action->GetType() == DQACTION_CENTERMAP) {
			DQActionCenterMap	*action = (DQActionCenterMap *)item->m_action;
			if (action) {
				action->CalculateCenteredMap(&tempViewRect);
			}
		}
		m_itemWalker->Next();
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
