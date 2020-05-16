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

// TODO: DQAction: improve member names
// TODO: DQAction: dump write actor unit-id as well
// TODO: Director: introduce callbacks (window) when action is done
// TODO: verify that all EffectActors are deleted

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
#include "SpriteGroupList.h"
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

extern SpriteGroupList	*g_unitSpriteGroupList;
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

enum DHEXECUTE {
	DHEXECUTE_NONE = -1,

	DHEXECUTE_NORMAL,
	DHEXECUTE_IMMEDIATE,

	DHEXECUTE_MAX
};

enum SEQ_ACTOR {
	SEQ_ACTOR_PRIMARY = 0,
	SEQ_ACTOR_SECONDARY = 1,

	SEQ_ACTOR_MAX
};

class DQAction; // forward
typedef void (DQHandler)(DQAction *action, Sequence *seq, DHEXECUTE executeType);

class DQItem {
public:
	DQItem(DQAction *action, DQHandler *handler);
	~DQItem();

	void			SetOwner(sint32 owner) { m_owner = (sint8)owner; }
	sint32			GetOwner(void) { return (sint32) m_owner; }

	uint8			m_addedToSavedList;
	sint8			m_owner;
	uint16			m_round;
	DQAction		*m_action;
	DQHandler		*m_handler;
	Sequence		*m_sequence;
};

class Sequence {
public:
	Sequence(sint32 seqID, DQItem *item)
			:
			m_sequenceID    (seqID),
			m_refCount      (0),
			m_item          (item)
	{
		m_addedToActiveList[SEQ_ACTOR_PRIMARY]      = false;
		m_addedToActiveList[SEQ_ACTOR_SECONDARY]    = false;
	}

	~Sequence() {}

	sint32 GetSequenceID(void) { return m_sequenceID; }
	void	AddRef(void) { m_refCount++; }
	void	Release(void) { m_refCount--; }
	sint32	GetRefCount(void) { return m_refCount; }

	DQItem	*GetItem(void) { return m_item; }

	void	SetAddedToActiveList(SEQ_ACTOR which, bool added) { m_addedToActiveList[which] = added; }
	bool	GetAddedToActiveList(SEQ_ACTOR which) { return m_addedToActiveList[which]; }

private:
	sint32	m_sequenceID;
	sint32	m_refCount;
	DQItem	*m_item;
	bool	m_addedToActiveList[SEQ_ACTOR_MAX];
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

	// DirectorEvent
	virtual Sequence *GetHoldSchedulerSequence() { return m_holdSchedulerSequence; }
	virtual void SetHoldSchedulerSequence(Sequence *seq) { m_holdSchedulerSequence = seq; }

	// GameEventManager
	virtual void IncrementPendingGameActions();
	virtual void DecrementPendingGameActions();

	// TiledMap
	virtual UnitActor *GetClickedActiveUnit(aui_MouseEvent *mouse);

	// TradePool
	virtual void TradeActorCreate(TradeRoute newRoute);
	virtual void TradeActorDestroy(TradeRoute routeToDestroy);

	// Unit
	virtual void FastKill(UnitActor *actor);

	// UnitActor && DirectorEvent
	virtual void ActionFinished(Sequence *seq);

	// UnitActor
	#ifdef _DEBUG
		virtual void DumpSequence(Sequence *seq);
	#endif

	// UnseenCell
	virtual void ActiveUnitRemove(UnitActor *unitActor);

	// Used locally
	Sequence	*NewSequence(DQItem* item);
	void		EndTurn();
	void		HandleActor(bool immediate, UnitActor* actor, bool actionEnabled, Sequence *seq, SEQ_ACTOR seqActor);
	bool 		TileIsVisibleToPlayer(MapPoint &pos);
	void		ActiveEffectAdd(EffectActor *effectActor);
	void		CenterMap(const MapPoint &pos);

private:
	bool    CanStartNextAction() { return m_itemQueue->GetCount() > 0 && GetActionFinished(); }
	void	HandleNextAction();
	void	HandleFinishedItem(DQItem *item);
	void	SaveFinishedItem(DQItem *item);
	void	GarbageCollectItems();

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
	uint32	ProcessActiveUnits();
	uint32	ProcessActiveEffects();
	void	ProcessTradeRouteAnimations();

	void	ActiveUnitAdd(UnitActor *unitActor);
	void	FinalizeActor(UnitActor *unitActor);
	void	FastKill(EffectActor *actor);
	uint32	KillAllActiveEffects();

	bool	GetActionFinished() { return m_actionFinished; }
	void	SetActionFinished(bool finished = true) { m_actionFinished = finished; }

	void	UpdateStandbyUnits();

#ifdef _DEBUG
	void	DumpItem(DQItem *item);
#endif

	// Unused
	void	ProcessImmediately(DQItem *item);
	bool	IsProcessing();
	void	ActiveEffectRemove(EffectActor *effectActor);

	static DirectorImpl		*m_instance;
	tech_WLList<UnitActor *>	*m_activeUnitList;

	tech_WLList<EffectActor *>	*m_activeEffectList;
	tech_WLList<TradeActor *>	*m_tradeActorList;

	bool						m_nextPlayer;

	static const int            k_TIME_LOG_SIZE = 30;
	uint32						m_masterCurTime;
	sint32						m_lastTickCount;
	sint32						m_timeLog[k_TIME_LOG_SIZE];
	sint32						m_timeLogIndex;
	sint32						m_averageElapsed;
	sint32						m_averageFPS;

	bool						m_actionFinished;

	bool						m_paused;
	bool						m_processingActiveUnits;
	bool						m_processingActiveEffects;

	sint32						m_curSequenceID;
	sint32						m_lastSequenceID;
	Sequence					*m_holdSchedulerSequence;

	PointerList<DQItem>			*m_dispatchedItems;
	PointerList<DQItem>			*m_savedItems;
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

class DQAction {
public:
	DQAction() {}
	virtual ~DQAction() {}
	virtual DQACTION_TYPE GetType() = 0;

	virtual void Execute(Sequence *sequence) = 0;
	virtual void ExecuteImmediate(Sequence *sequence) = 0;
	virtual void Finalize(bool primary, bool secondary) {}

	virtual void Dump() = 0;
};

class DQActionImmediate : public DQAction {
public:
	DQActionImmediate()
	: DQAction()
	{}
	virtual ~DQActionImmediate() {}

	virtual void Execute(Sequence *sequence) { ExecuteImmediate(sequence); }
	virtual void ExecuteImmediate(Sequence *sequence)
	{
		DoExecuteImmediate();
		DirectorImpl::Instance()->ActionFinished(sequence);
	}

protected:
	virtual void DoExecuteImmediate() = 0;
};

class DQActionMove : public DQAction
{
public:
	DQActionMove(
			UnitActor		*moveActor,
			const MapPoint	&startPos,
			const MapPoint	&endPos,
			sint32			moveArraySize,
			UnitActor		**moveActors,
			sint32			numRevealed,
			UnitActor		**revealedActors,
			sint32			soundID)
	: DQAction(),
	moveActor		(moveActor),
	startPos		(startPos),
	endPos			(endPos),
	moveArraySize	(moveArraySize),
	moveActors		(moveActors),
	numRevealed		(numRevealed),
	revealedActors	(revealedActors),
	soundID			(soundID)
	{}
	virtual ~DQActionMove() {
		delete [] moveActors;
	}
	virtual DQACTION_TYPE GetType() { return DQACTION_MOVE; }

	virtual void Execute(Sequence *sequence)
	{
		DoExecute(sequence, false);
	}

	virtual void ExecuteImmediate(Sequence *sequence)
	{
		DoExecute(sequence, true);
	}

	virtual void Finalize(bool primary, bool secondary) {
		if (primary) {
			MoveActorsToEndPosition();
			DirectorImpl::Instance()->ActiveUnitRemove(moveActor);
			if (!moveActor->IsActive()) {
				g_soundManager->TerminateLoopingSound(SOUNDTYPE_SFX, moveActor->GetUnitID());
			}
		}
	}

	virtual void Dump()
	{
		DPRINTF(k_DBG_UI, ("%s\n", GetName()));
		DPRINTF(k_DBG_UI, ("  actor                  :%#.8lx\n", moveActor));
		DPRINTF(k_DBG_UI, ("  startPos               :%d,%d\n", startPos.x, startPos.y));
		DPRINTF(k_DBG_UI, ("  endPos                 :%d,%d\n", endPos.x, endPos.y));
		DPRINTF(k_DBG_UI, ("  moveArraySize               :%d\n", moveArraySize));
		DPRINTF(k_DBG_UI, ("  moveActors                  :"));
		for (int i = 0; i < moveArraySize; i++) {
			DPRINTF(k_DBG_UI, ("%#.8lx  ", moveActors[i]));
		}
		DPRINTF(k_DBG_UI, ("\n"));
		DPRINTF(k_DBG_UI, ("  numRevealed                 :%d\n", numRevealed));
		DPRINTF(k_DBG_UI, ("  revealedActors              :"));
		for (int i = 0; i < numRevealed; i++) {
			DPRINTF(k_DBG_UI, ("%#.8lx  ", revealedActors[i]));
		}
		DPRINTF(k_DBG_UI, ("\n"));
		DPRINTF(k_DBG_UI, ("  soundID            :%d\n", soundID));
	}

	// TODO: make protected
	UnitActor	*moveActor;

protected:
	virtual MBCHAR const * const GetName() { return "Move"; }

	virtual void DoExecute(Sequence *sequence, bool immediate)
	{
		if (g_theUnitPool && !g_theUnitPool->IsValid(moveActor->GetUnitID())) {
			DirectorImpl::Instance()->ActionFinished(sequence);
			return;
		}

		Action *action = new Action();

		action->SetSequence(sequence);
		sequence->AddRef();

		action->SetStartMapPoint(startPos);
		action->SetEndMapPoint  (endPos);
		action->CreatePath(startPos.x, startPos.y, endPos.x, endPos.y);

		moveActor->PositionActor(startPos);

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
			DirectorImpl::Instance()->ActionFinished(sequence);
			return;
		}

		for (int i = 0; i < moveArraySize; i++) {
			moveActors[i]->SetHiddenUnderStack(TRUE);
		}

		bool visible = DirectorImpl::Instance()->TileIsVisibleToPlayer(startPos)
				|| DirectorImpl::Instance()->TileIsVisibleToPlayer(endPos);

		if (visible && !immediate)
		{
			if (g_selected_item->GetVisiblePlayer()!= moveActor->GetPlayerNum()
				&& !g_tiledMap->TileIsVisible(moveActor->GetPos().x, moveActor->GetPos().y))
			{
				DirectorImpl::Instance()->CenterMap(moveActor->GetPos());
			}
		}
		DirectorImpl::Instance()->HandleActor(immediate, moveActor, visible, sequence,SEQ_ACTOR_PRIMARY);
	}

	MapPoint	startPos;
	MapPoint	endPos;
	sint32		moveArraySize;
	UnitActor	**moveActors;
	sint32		numRevealed;
	UnitActor	**revealedActors;
	sint32		soundID;
private:
	void MoveActorsToEndPosition()
	{
		moveActor->PositionActor(endPos);

		for (int i = 0; i < moveArraySize; i++)
		{
			moveActors[i]->PositionActor(endPos);
		}
	}
};

class DQActionTeleport : public DQActionMove
{
public:
	DQActionTeleport(
			UnitActor		*moveActor,
			const MapPoint	&startPos,
			const MapPoint	&endPos,
			sint32			moveArraySize,
			UnitActor		**moveActors,
			sint32			numRevealed,
			UnitActor		**revealedActors)
	: DQActionMove(
			moveActor,
			startPos,
			endPos,
			moveArraySize,
			moveActors,
			numRevealed,
			revealedActors,
			CTPRecord::INDEX_INVALID)
	{}
	virtual ~DQActionTeleport() {}
	virtual DQACTION_TYPE GetType() { return DQACTION_TELEPORT; }

protected:
	virtual MBCHAR const * const GetName() { return "Teleport"; }

	virtual void DoExecute(Sequence *sequence, bool immediate)
	{
		// immediate is ignored as both executions are immediate for teleport.
		for (int i = 0; i < numRevealed; i++)
		{
			UnitActor *tempActor = revealedActors[i];
			tempActor->SetVisSpecial(TRUE);
		}

		moveActor->PositionActor(endPos);

		for (int i = 0; i < moveArraySize; i++)
		{
			UnitActor *moveActor = moveActors[i];
			moveActor->PositionActor(endPos);
		}

		DirectorImpl::Instance()->ActionFinished(sequence);
	}
};

class DQActionMoveProjectile : public DQActionImmediate
{
public:
	DQActionMoveProjectile(SpriteState *projectileEndState, const MapPoint &startPos, const MapPoint &endPos)
	: DQActionImmediate(),
	projectileEndState	(projectileEndState),
	startPos			(startPos),
	endPos				(endPos)
	{}
	virtual ~DQActionMoveProjectile() {}
	virtual DQACTION_TYPE GetType() { return DQACTION_MOVEPROJECTILE; }

protected:
	virtual void DoExecuteImmediate()
	{
		if (DirectorImpl::Instance()->TileIsVisibleToPlayer(startPos))
		{
			EffectActor *projectileEnd = new EffectActor(projectileEndState, endPos);
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
		DPRINTF(k_DBG_UI, ("  startPos           :%d,%d\n", startPos.x, startPos.y));
		DPRINTF(k_DBG_UI, ("  endPos             :%d,%d\n", endPos.x, endPos.y));
		DPRINTF(k_DBG_UI, ("  projectileEndState :%#.8lx\n", projectileEndState));
	}
protected:
	SpriteState	*projectileEndState;
	MapPoint	startPos;
	MapPoint	endPos;
};

class DQActionAttack : public DQAction
{
public:
	DQActionAttack(
			UnitActor		*attacker,
			const MapPoint	&attackerPos,
			bool			attackerIsCity,
			UnitActor		*defender,
			const MapPoint	&defenderPos,
			bool			defenderIsCity
			)
	: DQAction(),
	attacker		(attacker),
	attackerPos		(attackerPos),
	attackerIsCity	(attackerIsCity),
	defender		(defender),
	defenderPos		(defenderPos),
	defenderIsCity	(defenderIsCity)
	{}
	virtual ~DQActionAttack() {}
	virtual DQACTION_TYPE GetType() { return DQACTION_ATTACK; }

	virtual void Execute(Sequence *sequence) {
		DoExecute(sequence, false);
	}

	virtual void ExecuteImmediate(Sequence *sequence) {
		DoExecute(sequence, true);
	}

	virtual void Dump()
	{
		DPRINTF(k_DBG_UI, ("%s\n", GetName()));
		DPRINTF(k_DBG_UI, ("  attacker			:%#.8lx\n", attacker));
		DPRINTF(k_DBG_UI, ("  attackerPos       :%d,%d\n", attackerPos.x, attackerPos.y));
		DPRINTF(k_DBG_UI, ("  attackerIsCity    :%s\n", attackerIsCity ? "true" : "false"));
		DPRINTF(k_DBG_UI, ("  defender			:%#.8lx\n", defender));
		DPRINTF(k_DBG_UI, ("  defenderPos       :%d,%d\n", defenderPos.x, defenderPos.y));
		DPRINTF(k_DBG_UI, ("  defenderIsCity    :%s\n", defenderIsCity ? "true" : "false"));
	}
protected:
	virtual MBCHAR const * const GetName() { return "Attack"; }
	virtual void DoExecute(Sequence *sequence, bool immediate) {

		//	bool attackerVisible = DirectorImpl::Instance()->TileIsVisibleToPlayer(attackerPos);
		bool defenderVisible = DirectorImpl::Instance()->TileIsVisibleToPlayer(defenderPos);

		bool playerInvolved  = (defender->GetPlayerNum() == g_selected_item->GetVisiblePlayer()) ||
							   (attacker->GetPlayerNum() == g_selected_item->GetVisiblePlayer());

		POINT  AttackerPoints, DefenderPoints;

		maputils_MapXY2PixelXY(attackerPos.x, attackerPos.y, AttackerPoints);
		maputils_MapXY2PixelXY(defenderPos.x, defenderPos.y, DefenderPoints);

		sint32  deltax = DefenderPoints.x - AttackerPoints.x;
		sint32  deltay = DefenderPoints.y - AttackerPoints.y;

		sint32 facingIndex = spriteutils_DeltaToFacing(deltax, deltay);

		Action *action = new Action();
		action->SetSequence(sequence);
		sequence->AddRef();

		action->SetStartMapPoint(attackerPos);
		action->SetEndMapPoint  (attackerPos);

		attacker->ActionAttack(action, facingIndex);

		DirectorImpl::Instance()->HandleActor(immediate, attacker, playerInvolved, sequence,SEQ_ACTOR_PRIMARY);

		if (!defenderIsCity)
		{
			facingIndex = spriteutils_DeltaToFacing(-deltax, -deltay);

			action = new Action();
			action->SetSequence(sequence);
			sequence->AddRef();

			action->SetStartMapPoint(defenderPos);
			action->SetEndMapPoint  (defenderPos);

			defender->ActionAttack(action, facingIndex);

			if (playerInvolved) {
				defenderVisible = true;
			}
			DirectorImpl::Instance()->HandleActor(immediate, defender, defenderVisible, sequence, SEQ_ACTOR_SECONDARY);
		}
	}

	UnitActor	*attacker;
	MapPoint	attackerPos;
	bool		attackerIsCity;
	UnitActor	*defender;
	MapPoint	defenderPos;
	bool		defenderIsCity;
};

class DQActionSpecialAttack : public DQActionAttack
{
public:
	DQActionSpecialAttack(
			UnitActor		*attacker,
			const MapPoint	&attackerPos,
			bool			attackerIsCity,
			sint32			attackerSoundID,
			UnitActor		*defender,
			const MapPoint	&defenderPos,
			bool			defenderIsCity)
			: DQActionAttack(attacker, attackerPos, attackerIsCity, defender, defenderPos, defenderIsCity),
			  attackerSoundID	(attackerSoundID)
	{}
	virtual ~DQActionSpecialAttack() {}
	virtual DQACTION_TYPE GetType() { return DQACTION_SPECATTACK; }

	virtual void Finalize(bool primary, bool secondary)
	{
		if (primary) {
			DirectorImpl::Instance()->ActiveUnitRemove(attacker);
			if (!attacker->IsActive()) {
				g_soundManager->TerminateLoopingSound(SOUNDTYPE_SFX, attacker->GetUnitID());
			}
		}
		if (secondary)
		{
			DirectorImpl::Instance()->ActiveUnitRemove(defender);
			if (!defender->IsActive()) {
				g_soundManager->TerminateLoopingSound(SOUNDTYPE_SFX, defender->GetUnitID());
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

	virtual void DoExecute(Sequence *sequence, bool immediate)
	{
		BOOL attackerCanAttack = !attackerIsCity;
		BOOL defenderIsAttackable = !defenderIsCity;

		if (!attackerCanAttack && !defenderIsAttackable)
		{
			DirectorImpl::Instance()->ActionFinished(sequence);
			return;
		}

		sint32 facingIndex;
		POINT  AttackerPoints, DefenderPoints;
		maputils_MapXY2PixelXY(attackerPos.x, attackerPos.y, AttackerPoints);
		maputils_MapXY2PixelXY(defenderPos.x, defenderPos.y, DefenderPoints);

		sint32  deltax = DefenderPoints.x - AttackerPoints.x;
		sint32  deltay = DefenderPoints.y - AttackerPoints.y;

		if (attackerSoundID >= 0) {
			g_soundManager->AddSound(SOUNDTYPE_SFX, (uint32)0, attackerSoundID, 0, 0);
		}

		if (attackerCanAttack)
		{
			Action *attackerAction = new Action();
			attackerAction->SetStartMapPoint(attackerPos);
			attackerAction->SetEndMapPoint  (attackerPos);

			facingIndex = spriteutils_DeltaToFacing(deltax, deltay);

			attackerAction->SetSequence(sequence);
			sequence->AddRef();

			if(!attacker->ActionSpecialAttack(attackerAction, facingIndex))
			{
				delete attackerAction;
				DirectorImpl::Instance()->ActionFinished(sequence);
				return;
			}

			bool visible = DirectorImpl::Instance()->TileIsVisibleToPlayer(attackerPos);
			DirectorImpl::Instance()->HandleActor(immediate, attacker, visible, sequence,SEQ_ACTOR_SECONDARY);
		}

		if (defenderIsAttackable)
		{
			Action *defenderAction = new Action();

			defenderAction->SetStartMapPoint(defenderPos);
			defenderAction->SetEndMapPoint  (defenderPos);

			facingIndex=spriteutils_DeltaToFacing(-deltax, -deltay);

			defenderAction->SetSequence(sequence);
			sequence->AddRef();

			if(!defender->ActionSpecialAttack(defenderAction, facingIndex))
			{
				delete defenderAction;
				DirectorImpl::Instance()->ActionFinished(sequence);
				return;
			}

			bool visible = DirectorImpl::Instance()->TileIsVisibleToPlayer(defenderPos);
			DirectorImpl::Instance()->HandleActor(immediate, defender, visible, sequence, SEQ_ACTOR_SECONDARY);
		}
	}

	sint32	attackerSoundID;
};

class DQActionAttackPos : public DQAction
{
public:
	DQActionAttackPos(UnitActor *attacker, const MapPoint &attackerPos, const MapPoint &targetPos, sint32 soundID)
	: DQAction(),
	attacker	(attacker),
	attackerPos	(attackerPos),
	targetPos	(targetPos),
	soundID		(soundID)
	{}
	virtual ~DQActionAttackPos() {}
	virtual DQACTION_TYPE GetType() { return DQACTION_ATTACKPOS; }

	virtual void Execute(Sequence *sequence)
	{
		DoExecute(sequence, false);
	}

	virtual void ExecuteImmediate(Sequence* sequence)
	{
		DoExecute(sequence, true);
	}

	virtual void Finalize(bool primary, bool secondary)
	{
		if (primary) {
			DirectorImpl::Instance()->ActiveUnitRemove(attacker);
			if (!attacker->IsActive()) {
				g_soundManager->TerminateLoopingSound(SOUNDTYPE_SFX, attacker->GetUnitID());
			}
		}
	}

	virtual void Dump()
	{
		DPRINTF(k_DBG_UI, ("Attack Pos\n"));
		DPRINTF(k_DBG_UI, ("  attackpos_attacker     :%#.8lx\n", attacker));
		DPRINTF(k_DBG_UI, ("  attackpos_attacker_pos :%d,%d\n", attackerPos.x, attackerPos.y));
		DPRINTF(k_DBG_UI, ("  attackpos_target_pos   :%d,%d\n", targetPos.x, targetPos.y));
		DPRINTF(k_DBG_UI, ("  attackpos_soundID      :%d\n", soundID));
	}
protected:
	UnitActor	*attacker;
	MapPoint	attackerPos;
	MapPoint	targetPos;
	sint32		soundID;
private:
	void DoExecute(Sequence *sequence, bool immediate)
	{
		if (attacker->GetNeedsToDie()) {
			DirectorImpl::Instance()->ActionFinished(sequence);
			return;
		}
		Action *action = new Action(UNITACTION_ATTACK, ACTIONEND_ANIMEND,
									   attacker->GetHoldingCurAnimPos(UNITACTION_ATTACK),
									   attacker->GetHoldingCurAnimSpecialDelayProcess(UNITACTION_ATTACK));
		action->SetSequence(sequence);
		sequence->AddRef();

		action->SetStartMapPoint(attackerPos);
		action->SetEndMapPoint(attackerPos);

		if (attacker->GetLoadType() != LOADTYPE_FULL) {
			attacker->FullLoad(UNITACTION_ATTACK);
		}

		Anim	*attackerAnim = attacker->CreateAnim(UNITACTION_ATTACK);
		if (!attackerAnim) {
			attackerAnim = attacker->CreateAnim(UNITACTION_IDLE);
		}
		action->SetAnim(attackerAnim);

		POINT AttackerPoints, AttackedPoints;
		maputils_MapXY2PixelXY(attackerPos.x, attackerPos.y, AttackerPoints);
		maputils_MapXY2PixelXY(targetPos.x, targetPos.y, AttackedPoints);

		action->SetFacing(spriteutils_DeltaToFacing(AttackedPoints.x - AttackerPoints.x,
															   AttackedPoints.y - AttackerPoints.y));

		action->SetUnitVisionRange(attacker->GetUnitVisionRange());
		action->SetUnitsVisibility(attacker->GetUnitVisibility());

		if (g_soundManager) {
			g_soundManager->TerminateLoopingSound(SOUNDTYPE_SFX, attacker->GetUnitID());

			sint32 visiblePlayer = g_selected_item->GetVisiblePlayer();
			if ((visiblePlayer == attacker->GetPlayerNum()) ||
				(attacker->GetUnitVisibility() & (1 << visiblePlayer)))
			{
				g_soundManager->AddSound(SOUNDTYPE_SFX, attacker->GetUnitID(), soundID,
										 attackerPos.x, attackerPos.y);
			}
		}

		attacker->AddAction(action);

		bool attackerVisible = DirectorImpl::Instance()->TileIsVisibleToPlayer(attackerPos);
		DirectorImpl::Instance()->HandleActor(immediate, attacker, attackerVisible, sequence, SEQ_ACTOR_PRIMARY);
	}
};

class DQActionDeath : public DQAction
{
public:
	DQActionDeath(UnitActor *dead, const MapPoint &deadPos, sint32 deadSoundID)
	: DQAction(),
	dead		(dead),
	deadPos		(deadPos),
	deadSoundID	(deadSoundID)
	{}
	virtual ~DQActionDeath() {}
	virtual DQACTION_TYPE GetType() { return DQACTION_DEATH; }

	virtual void Execute(Sequence *sequence)
	{
		DoExecute(sequence, false);
	}

	virtual void ExecuteImmediate(Sequence *sequence)
	{
		DoExecute(sequence, true);
	}

	virtual void Finalize(bool primary, bool secondary)
	{
		if (primary) {
			DirectorImpl::Instance()->ActiveUnitRemove(dead);
			if (!dead->IsActive()) {
				g_soundManager->TerminateLoopingSound(SOUNDTYPE_SFX, dead->GetUnitID());
			}
		}
	}

	virtual void Dump()
	{
		DPRINTF(k_DBG_UI, ("Death\n"));
		DPRINTF(k_DBG_UI, ("  dead               :%#.8lx\n", dead));
		DPRINTF(k_DBG_UI, ("  dead_Pos           :%d,%d\n", deadPos.x, deadPos.y));
		DPRINTF(k_DBG_UI, ("  dead_soundID       :%d\n", deadSoundID));
	}
protected:
	UnitActor	*dead;
	MapPoint	deadPos;
	sint32		deadSoundID;
private:
	void DoExecute(Sequence *sequence, bool immediate)
	{
		if (!dead->GetNeedsToDie())
		{
			dead->SetNeedsToDie(TRUE);

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

			action->SetSequence(sequence);
			sequence->AddRef();

			action->SetStartMapPoint(deadPos);
			action->SetEndMapPoint(deadPos);

			action->SetAnim(deathAnim);

			action->SetUnitVisionRange(dead->GetUnitVisionRange());
			action->SetUnitsVisibility(dead->GetUnitVisibility());
			action->SetFacing(dead->GetFacing());

			if (g_soundManager) {
				g_soundManager->TerminateLoopingSound(SOUNDTYPE_SFX, dead->GetUnitID());
			}

			if (g_soundManager) {
				sint32 visiblePlayer = g_selected_item->GetVisiblePlayer();
				if ((visiblePlayer == dead->GetPlayerNum()) ||
					(dead->GetUnitVisibility() & (1 << visiblePlayer))) {
					g_soundManager->AddSound(SOUNDTYPE_SFX, (uint32)dead->GetUnitID(), deadSoundID,
											 deadPos.x, deadPos.y);
				}
			}

			dead->AddAction(action);

			bool visible = DirectorImpl::Instance()->TileIsVisibleToPlayer(deadPos);
			DirectorImpl::Instance()->HandleActor(immediate, dead, visible, sequence, SEQ_ACTOR_PRIMARY);
		} else {
			DirectorImpl::Instance()->ActionFinished(sequence);
		}
	}
};

class DQActionWork : public DQAction
{
public:
	DQActionWork(UnitActor *workActor, const MapPoint &workPos, sint32 workSoundID)
	: DQAction(),
	workActor	(workActor),
	workPos		(workPos),
	workSoundID	(workSoundID)
	{}
	virtual ~DQActionWork() {}
	virtual DQACTION_TYPE GetType() { return DQACTION_WORK; }

	virtual void Execute(Sequence *sequence)
	{
		DoExecute(sequence, false);
	}

	virtual void ExecuteImmediate(Sequence* sequence)
	{
		DoExecute(sequence, true);
	}

	virtual void Finalize(bool primary, bool secondary)
	{
		if (primary) {
			DirectorImpl::Instance()->ActiveUnitRemove(workActor);
			if (!workActor->IsActive()) {
				g_soundManager->TerminateLoopingSound(SOUNDTYPE_SFX, workActor->GetUnitID());
			}
		}
	}

	virtual void Dump()
	{
		DPRINTF(k_DBG_UI, ("Work\n"));
		DPRINTF(k_DBG_UI, ("  working_actor      :%#.8lx\n", workActor));
		DPRINTF(k_DBG_UI, ("  working_pos        :%d,%d\n", workPos.x, workPos.y));
		DPRINTF(k_DBG_UI, ("  working_soundID    :%d\n", workSoundID));
	}
protected:
	UnitActor	*workActor;
	MapPoint	workPos;
	sint32		workSoundID;
private:
	void DoExecute(Sequence *sequence, bool immediate)
	{
		Action *action = new Action(UNITACTION_WORK, ACTIONEND_ANIMEND);

		action->SetStartMapPoint(workPos);
		action->SetEndMapPoint(workPos);

		if (workActor->GetLoadType() != LOADTYPE_FULL) {
			workActor->FullLoad(UNITACTION_WORK);
		}

		Anim *anim = workActor->CreateAnim(UNITACTION_WORK);
		if (!anim)
		{
			anim = workActor->CreateAnim(UNITACTION_MOVE);

			if (!anim) {
				delete action;
				DirectorImpl::Instance()->ActionFinished(sequence);
				return;
			}
		}

		action->SetSequence(sequence);
		sequence->AddRef();

		action->SetAnim(anim);

		workActor->AddAction(action);

		if (g_soundManager)
		{
			sint32 visiblePlayer = g_selected_item->GetVisiblePlayer();
			if ((visiblePlayer == workActor->GetPlayerNum()) ||
				(workActor->GetUnitVisibility() & (1 << visiblePlayer))) {
				g_soundManager->AddSound(SOUNDTYPE_SFX, workActor->GetUnitID(), workSoundID, workPos.x, workPos.y);
			}
		}

		bool visible = DirectorImpl::Instance()->TileIsVisibleToPlayer(workPos);
		DirectorImpl::Instance()->HandleActor(immediate, workActor, visible, sequence, SEQ_ACTOR_PRIMARY);
	}
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
		DPRINTF(k_DBG_UI, ("  morphing_actor     :%#.8lx\n", morphingActor));
		DPRINTF(k_DBG_UI, ("  ss                 :%#.8lx (%d)\n", spriteState, spriteState->GetIndex()));
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
		DPRINTF(k_DBG_UI, ("  hiding_actor       :%#.8lx\n", hidingActor));
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
	DQActionShow(UnitActor *hidingActor, const MapPoint &hidingPos)
	: DQActionHideShow(hidingActor),
	hidingPos	(hidingPos)
	{}
	virtual ~DQActionShow() {}
	virtual DQACTION_TYPE GetType() { return DQACTION_SHOW; }

	virtual void Dump() {
		DPRINTF(k_DBG_UI, ("Show\n"));
		DPRINTF(k_DBG_UI, ("  hiding_actor       :%#.8lx\n", hidingActor));
		DPRINTF(k_DBG_UI, ("  hiding_pos         :%d,%d\n", hidingPos.x, hidingPos.y));
	}
protected:
	virtual void DoExecuteImmediate()
	{
		hidingActor->PositionActor(hidingPos);
		hidingActor->Show();
	}
	MapPoint hidingPos;
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
		DPRINTF(k_DBG_UI, ("  dead               :%#.8lx\n", dead));
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
	DQActionVision(const MapPoint &visionPos, double visionRange)
	: DQActionImmediate(),
	visionPos	(visionPos),
	visionRange	(visionRange)
	{}
	virtual ~DQActionVision() {}

protected:
	MapPoint	visionPos;
	double		visionRange;
};

class DQActionRemoveVision : public DQActionVision
{
public:
	DQActionRemoveVision(const MapPoint &visionPos, double visionRange)
	: DQActionVision(visionPos, visionRange)
	{}
	virtual ~DQActionRemoveVision() {}
	virtual DQACTION_TYPE GetType() { return DQACTION_REMOVEVISION; }

	virtual void Dump()
	{
		DPRINTF(k_DBG_UI, ("Remove Vision\n"));
		DPRINTF(k_DBG_UI, ("  vision_pos         :%d,%d\n", visionPos.x, visionPos.y));
		DPRINTF(k_DBG_UI, ("  vision_range       :%#.2f\n", visionRange));
	}
protected:
	virtual void DoExecuteImmediate()
	{
		if(g_tiledMap) {
			g_tiledMap->RemoveVisible(visionPos, visionRange);
		}
	}
};

class DQActionAddVision : public DQActionVision {
public:
	DQActionAddVision(const MapPoint &visionPos, double visionRange)
	: DQActionVision(visionPos, visionRange)
	{}
	virtual ~DQActionAddVision() {}
	virtual DQACTION_TYPE GetType() { return DQACTION_ADDVISION; }

	virtual void Dump()
	{
		DPRINTF(k_DBG_UI, ("Add Vision\n"));
		DPRINTF(k_DBG_UI, ("  vision_pos         :%d,%d\n", visionPos.x, visionPos.y));
		DPRINTF(k_DBG_UI, ("  vision_range       :%#.2f\n", visionRange));
	}
protected:
	virtual void DoExecuteImmediate()
	{
		if(g_tiledMap) {
			g_tiledMap->AddVisible(visionPos, visionRange);
		}
	}
};

class DQActionSetVisibility : public DQActionImmediate
{
public:
	DQActionSetVisibility(UnitActor *setVisibilityActor, uint32 visibilityFlag)
	: DQActionImmediate(),
	setVisibilityActor	(setVisibilityActor),
	visibilityFlag		(visibilityFlag)
	{}
	virtual ~DQActionSetVisibility() {}
	virtual DQACTION_TYPE GetType() { return DQACTION_SETVISIBILITY; }

	virtual void Dump()
	{
		DPRINTF(k_DBG_UI, ("Set Visibility\n"));
		DPRINTF(k_DBG_UI, ("  setVisibilityActor :%#.8lx\n", setVisibilityActor));
		DPRINTF(k_DBG_UI, ("  owner              :%#.8lx\n", visibilityFlag));
	}
protected:
	virtual void DoExecuteImmediate()
	{
		setVisibilityActor->SetUnitVisibility(visibilityFlag);
	}

	UnitActor	*setVisibilityActor;
	uint32		visibilityFlag;
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
		DPRINTF(k_DBG_UI, ("  setowner_actor     :%#.8lx\n", setOwnerActor));
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
	DQActionSetVisionRange(UnitActor *setVisionRangeActor, double range)
	: DQActionImmediate(),
	setVisionRangeActor	(setVisionRangeActor),
	range				(range)
	{}
	virtual ~DQActionSetVisionRange() {}
	virtual DQACTION_TYPE GetType() { return DQACTION_SETVISIONRANGE; }

	virtual void Dump()
	{
		DPRINTF(k_DBG_UI, ("Set Vision Range\n"));
		DPRINTF(k_DBG_UI, ("  setVisionRangeActor:%#.8lx\n", setVisionRangeActor));
		DPRINTF(k_DBG_UI, ("  range     :%#.2f\n", range));
	}
protected:
	virtual void DoExecuteImmediate()
	{
		setVisionRangeActor->SetUnitVisionRange(range);
	}

	UnitActor	*setVisionRangeActor;
	double		range;
};

class DQActionCenterMap : public DQActionImmediate
{
public:
	DQActionCenterMap(const MapPoint &centerMapPos)
	: DQActionImmediate(),
	centerMapPos	(centerMapPos)
	{}
	virtual ~DQActionCenterMap() {}
	virtual DQACTION_TYPE GetType() { return DQACTION_CENTERMAP; }

	void CalculateCenteredMap(RECT *viewRect)
	{
		g_radarMap->ComputeCenteredMap(centerMapPos, viewRect);
	}

	virtual void Dump()
	{
		DPRINTF(k_DBG_UI, ("Center Map\n"));
		DPRINTF(k_DBG_UI, ("  centerMap_pos    :%d,%d\n", centerMapPos.x, centerMapPos.y));
	}
protected:
	virtual void DoExecuteImmediate()
	{
		if(!g_selected_item->GetIsPathing()) {
			DirectorImpl::Instance()->CenterMap(centerMapPos);
		}
	}

	MapPoint	centerMapPos;
};

class DQActionCombatFlash : public DQActionImmediate
{
public:
	DQActionCombatFlash(const MapPoint &flashPos)
	: DQActionImmediate(),
	flashPos	(flashPos)
	{}
	virtual ~DQActionCombatFlash() {}
	virtual DQACTION_TYPE GetType() { return DQACTION_COMBATFLASH; }

	virtual void Dump()
	{
		DPRINTF(k_DBG_UI, ("Combat Flash\n"));
		DPRINTF(k_DBG_UI, ("  flash_pos         :%d,%d\n", flashPos.x, flashPos.y));
	}
protected:
	virtual void DoExecuteImmediate()
	{
		SpriteState	*spriteState = new SpriteState(99);
		EffectActor	*flash = new EffectActor(spriteState, flashPos);

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

	MapPoint	flashPos;
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
		DPRINTF(k_DBG_UI, ("Copy Vision from Gamestate\n"));
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
		DPRINTF(k_DBG_UI, ("End Turn from Gamestate\n"));
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
	DQActionPlaySound(sint32 soundID, const MapPoint &pos)
	: DQActionImmediate(),
	soundID	(soundID),
	pos		(pos)
	{}
	virtual ~DQActionPlaySound() {}
	virtual DQACTION_TYPE GetType() { return DQACTION_PLAYSOUND; }

	virtual void Dump()
	{
		DPRINTF(k_DBG_UI, ("Play Sound\n"));
		DPRINTF(k_DBG_UI, ("  playsound_soundID    :%d\n", soundID));
		DPRINTF(k_DBG_UI, ("  playsound_pos        :%d,%d\n", pos.x, pos.y));
	}
protected:
	virtual void DoExecuteImmediate()
	{
		if (g_soundManager) {
			g_soundManager->AddSound(SOUNDTYPE_SFX, 0, soundID, pos.x, pos.y);
		}
	}

	sint32		soundID;
	MapPoint	pos;
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
		DPRINTF(k_DBG_UI, ("Message from Gamestate\n"));
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

class DQActionFaceoff : public DQAction
{
public:
	DQActionFaceoff(UnitActor *attacker, const MapPoint &attackerPos, UnitActor *attacked, const MapPoint &attackedPos)
	: DQAction(),
	attacker	(attacker),
	attackerPos	(attackerPos),
	attacked	(attacked),
	attackedPos	(attackedPos)
	{}
	virtual ~DQActionFaceoff() {}
	virtual DQACTION_TYPE GetType() { return DQACTION_FACEOFF; }

	virtual void Execute(Sequence *sequence)
	{
		DoExecute(sequence, false);
	}

	virtual void ExecuteImmediate(Sequence *sequence)
	{
		DoExecute(sequence, true);
	}

	virtual void Dump()
	{
		DPRINTF(k_DBG_UI, ("Faceoff\n"));
		DPRINTF(k_DBG_UI, ("  faceoff_attacker   :%#.8lx\n", attacker));
		DPRINTF(k_DBG_UI, ("  faceoff_attacked   :%#.8lx\n", attacked));
		DPRINTF(k_DBG_UI, ("  faceoff_attacker_pos :%d,%d\n", attackerPos.x, attackerPos.y));
		DPRINTF(k_DBG_UI, ("  faceoff_attacked_pos :%d,%d\n", attackedPos.x, attackedPos.y));
	}
protected:
	UnitActor	*attacker;
	MapPoint	attackerPos;
	UnitActor	*attacked;
	MapPoint	attackedPos;
private:
	void DoExecute(Sequence *sequence, bool immediate)
	{
		Action *attackerAction = new Action(UNITACTION_FACE_OFF, ACTIONEND_INTERRUPT);
		attackerAction->SetSequence(NULL);
		attackerAction->SetStartMapPoint(attackerPos);
		attackerAction->SetEndMapPoint(attackerPos);

		Action *attackedAction = new Action(UNITACTION_FACE_OFF, ACTIONEND_INTERRUPT);
		attackedAction->SetSequence(NULL);
		attackedAction->SetStartMapPoint(attackedPos);
		attackedAction->SetEndMapPoint(attackedPos);

		Anim * attackerAnim = attacker->MakeFaceoff();
		if (!attackerAnim)
		{
			attacker->AddIdle(TRUE);
			delete attackerAction;
			delete attackedAction;
			DirectorImpl::Instance()->ActionFinished(sequence);
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

		maputils_MapXY2PixelXY(attackerPos.x, attackerPos.y, AttackerPoints);
		maputils_MapXY2PixelXY(attackedPos.x, attackedPos.y, AttackedPoints);

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

		bool attackedVisible = DirectorImpl::Instance()->TileIsVisibleToPlayer(attackedPos);
		if (attacker->GetPlayerNum() == g_selected_item->GetVisiblePlayer() ||
			attacked->GetPlayerNum() == g_selected_item->GetVisiblePlayer()) {
			attackedVisible = true;
		}

		DirectorImpl::Instance()->HandleActor(immediate, attacked, attackedVisible, sequence, SEQ_ACTOR_SECONDARY);

		bool attackerVisible = DirectorImpl::Instance()->TileIsVisibleToPlayer(attackerPos);
		if (attacked->GetPlayerNum() == g_selected_item->GetVisiblePlayer() ||
			attacker->GetPlayerNum() == g_selected_item->GetVisiblePlayer()) {
			attackerVisible = true;
			attackedVisible = true;
		}
		DirectorImpl::Instance()->HandleActor(
				immediate, attacker, attackerVisible && attackedVisible, sequence, SEQ_ACTOR_PRIMARY);
		DirectorImpl::Instance()->ActionFinished(sequence);
	}
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
		DPRINTF(k_DBG_UI, ("  faceroffer   :%#.8lx\n", faceOffer));
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
	DQActionSpecialEffect(const MapPoint &pos, sint32 spriteID, sint32 soundID)
	: DQActionImmediate(),
	pos			(pos),
	spriteID	(spriteID),
	soundID		(soundID)
	{}
	virtual ~DQActionSpecialEffect() {}
	virtual DQACTION_TYPE GetType() { return DQACTION_SPECEFFECT; }

	virtual void Dump()
	{
		DPRINTF(k_DBG_UI, ("Special Effect\n"));
		DPRINTF(k_DBG_UI, ("  speceffect_pos       :%d,%d\n", pos.x, pos.y));
		DPRINTF(k_DBG_UI, ("  speceffect_spriteID    :%d\n", spriteID));
		DPRINTF(k_DBG_UI, ("  speceffect_soundID    :%d\n", soundID));
	}
protected:
	virtual void DoExecuteImmediate()
	{
		if (g_tiledMap->GetLocalVision()->IsVisible(pos)) {
			SpriteState *spriteState = new SpriteState(spriteID);
			EffectActor *effectActor = new EffectActor(spriteState, pos);

			Anim *anim = effectActor->CreateAnim(EFFECTACTION_PLAY);

			if (anim) {
				Action *action = new Action(EFFECTACTION_PLAY, ACTIONEND_PATHEND);
				action->SetAnim(anim);
				effectActor->AddAction(action);
				DirectorImpl::Instance()->ActiveEffectAdd(effectActor);

				if (g_soundManager) {
					g_soundManager->AddSound(SOUNDTYPE_SFX, 0, soundID, pos.x, pos.y);
				}
			}
		}
	}

	MapPoint	pos;
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

class DQActionInvokeResearchAdvance : public DQAction
{
public:
	DQActionInvokeResearchAdvance(MBCHAR const * const message)
	: DQAction(),
	message	(strdup(message))
	{}
	virtual ~DQActionInvokeResearchAdvance()
	{
		free(message);
	}
	virtual DQACTION_TYPE GetType() { return DQACTION_INVOKE_RESEARCH_ADVANCE; }

	virtual void Execute(Sequence *sequence)
	{
		sci_advancescreen_displayMyWindow(message, 0, sequence);
	}

	virtual void ExecuteImmediate(Sequence *sequence)
	{
		// No immediate execution available
		Execute(sequence);
	}

	virtual void Dump()
	{
		DPRINTF(k_DBG_UI, ("Invoke Research advance\n"));
		DPRINTF(k_DBG_UI, ("  message              :%s\n", message));
	}
protected:
	MBCHAR	*message;
};

class DQActionBattle : public DQAction
{
public:
	DQActionBattle(Battle *battle)
	: DQAction(),
	battle	(battle)
	{}
	virtual ~DQActionBattle() {}
	virtual DQACTION_TYPE GetType() { return DQACTION_BATTLE; }

	virtual void Execute(Sequence *sequence)
	{
		if(g_battleViewWindow)
		{
			BattleViewWindow::Cleanup();
		}

		BattleViewWindow::Initialize(sequence);

		if(g_battleViewWindow)
		{
			g_battleViewWindow->SetupBattle(battle);
			g_c3ui->AddWindow(g_battleViewWindow);
			g_cursorManager->SetCursor(CURSORINDEX_DEFAULT);
		}
	}

	virtual void ExecuteImmediate(Sequence *sequence)
	{
		// No immediate execution available
		Execute(sequence);
	}

	virtual void Dump()
	{
		DPRINTF(k_DBG_UI, ("Battle\n"));
	}
protected:
	Battle	*battle;
};

class DQActionPlayWonderMovie : public DQAction
{
public:
	DQActionPlayWonderMovie(sint32 wonderMovieID)
	: DQAction(),
	wonderMovieID	(wonderMovieID)
	{}
	virtual ~DQActionPlayWonderMovie() {}
	virtual DQACTION_TYPE GetType() { return DQACTION_PLAYWONDERMOVIE; }

	virtual void Execute(Sequence *sequence)
	{
		wondermoviewin_Initialize(sequence);
		wondermoviewin_DisplayWonderMovie(wonderMovieID);
	}

	virtual void ExecuteImmediate(Sequence *sequence)
	{
		// No immediate execution available
		Execute(sequence);
	}

	virtual void Dump()
	{
		DPRINTF(k_DBG_UI, ("Play Wonder Movie\n"));
		DPRINTF(k_DBG_UI, ("  wonderMovieID        :%ld\n", wonderMovieID));
	}
protected:
	sint32	wonderMovieID;
};

class DQActionPlayVictoryMovie : public DQAction
{
public:
	DQActionPlayVictoryMovie(GAME_OVER reason)
	: DQAction(),
	reason	(reason)
	{}
	virtual ~DQActionPlayVictoryMovie() {}
	virtual DQACTION_TYPE GetType() { return DQACTION_PLAYVICTORYMOVIE; }

	virtual void Execute(Sequence *sequence)
	{
		victorymoviewin_Initialize(sequence);
		victorymoviewin_DisplayVictoryMovie(reason);
	}

	virtual void ExecuteImmediate(Sequence *sequence)
	{
		// No immediate execution available
		Execute(sequence);
	}

	virtual void Dump()
	{
		DPRINTF(k_DBG_UI, ("Play Victory Movie\n"));
		DPRINTF(k_DBG_UI, ("  reason                :%ld\n", reason));
	}
protected:
	GAME_OVER reason;
};

class DQActionBeginScheduler : public DQAction
{
public:
	DQActionBeginScheduler(sint32 player)
	: DQAction(),
	player	(player)
	{}
	virtual ~DQActionBeginScheduler() {}
	virtual DQACTION_TYPE GetType() { return DQACTION_BEGIN_SCHEDULER; }

	virtual void Execute(Sequence *sequence)
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

		Assert(!DirectorImpl::Instance()->GetHoldSchedulerSequence());
		if(!g_network.IsActive() || g_network.IsLocalPlayer(player))
		{
			DirectorImpl::Instance()->SetHoldSchedulerSequence(sequence);
		}
		else
		{
			DirectorImpl::Instance()->SetHoldSchedulerSequence(NULL);
		}

		g_gevManager->Pause();
		g_gevManager->AddEvent(GEV_INSERT_Tail, GEV_BeginScheduler,
							   GEA_Player, player,
							   GEA_End);
		g_gevManager->Resume();

		if(!DirectorImpl::Instance()->GetHoldSchedulerSequence()) {
			DirectorImpl::Instance()->ActionFinished(sequence);
		}
	}

	virtual void ExecuteImmediate(Sequence *sequence)
	{
		// No immediate execution available
		Execute(sequence);
	}

	virtual void Dump()
	{
		DPRINTF(k_DBG_UI, ("Begin Scheduler\n"));
		DPRINTF(k_DBG_UI, ("  player: %d\n", player));
	}
protected:
	sint32 player;
};

void dh_move(DQAction *itemAction, Sequence *seq, DHEXECUTE executeType)
{
	if (executeType == DHEXECUTE_NORMAL) {
		itemAction->Execute(seq);
	} else {
		itemAction->ExecuteImmediate(seq);
	}
}

void dh_teleport(DQAction *itemAction, Sequence *seq, DHEXECUTE executeType)
{
	itemAction->Execute(seq);
}

void dh_projectileMove(DQAction *itemAction, Sequence *seq, DHEXECUTE executeType)
{
	itemAction->Execute(seq);
}

void dh_attack(DQAction *itemAction, Sequence *seq, DHEXECUTE executeType)
{
	if (executeType == DHEXECUTE_NORMAL) {
		itemAction->Execute(seq);
	} else {
		itemAction->ExecuteImmediate(seq);
	}
}

void dh_specialAttack(DQAction *itemAction, Sequence *seq, DHEXECUTE executeType)
{
	if (executeType == DHEXECUTE_NORMAL) {
		itemAction->Execute(seq);
	} else {
		itemAction->ExecuteImmediate(seq);
	}
}

void dh_death(DQAction *itemAction, Sequence *seq, DHEXECUTE executeType)
{
	if (executeType == DHEXECUTE_NORMAL) {
		itemAction->Execute(seq);
	} else {
		itemAction->ExecuteImmediate(seq);
	}
}

void dh_morphUnit(DQAction *itemAction, Sequence *seq, DHEXECUTE executeType)
{
	itemAction->Execute(seq);
}

void dh_hide(DQAction *itemAction, Sequence *seq, DHEXECUTE executeType)
{
	itemAction->Execute(seq);
}

void dh_show(DQAction *itemAction, Sequence *seq, DHEXECUTE executeType)
{
	itemAction->Execute(seq);
}

void dh_work(DQAction *itemAction, Sequence *seq, DHEXECUTE executeType)
{
	if (executeType == DHEXECUTE_NORMAL) {
		itemAction->Execute(seq);
	} else {
		itemAction->ExecuteImmediate(seq);
	}
}

void dh_fastkill(DQAction *itemAction, Sequence *seq, DHEXECUTE executeType)
{
	itemAction->Execute(seq);
}

void dh_removeVision(DQAction *itemAction, Sequence *seq, DHEXECUTE executeType)
{
	itemAction->Execute(seq);
}

void dh_addVision(DQAction *itemAction, Sequence *seq, DHEXECUTE executeType)
{
	itemAction->Execute(seq);
}

void dh_setVisibility(DQAction *itemAction, Sequence *seq, DHEXECUTE executeType)
{
	itemAction->Execute(seq);
}

void dh_setOwner(DQAction *itemAction, Sequence *seq, DHEXECUTE executeType)
{
	itemAction->Execute(seq);
}

void dh_setVisionRange(DQAction *itemAction, Sequence *seq, DHEXECUTE executeType)
{
	itemAction->Execute(seq);
}

void dh_combatflash(DQAction *itemAction, Sequence *seq, DHEXECUTE executeType)
{
	itemAction->Execute(seq);
}

void dh_copyVision(DQAction *itemAction, Sequence *seq, DHEXECUTE executeType)
{
	itemAction->Execute(seq);
}

void dh_centerMap(DQAction *itemAction, Sequence *seq, DHEXECUTE executeType)
{
	itemAction->Execute(seq);
}

void dh_selectUnit(DQAction *itemAction, Sequence *seq, DHEXECUTE executeType)
{
	itemAction->Execute(seq);
}

void dh_endTurn(DQAction *itemAction, Sequence *seq, DHEXECUTE executeType)
{
	itemAction->Execute(seq);
}

void dh_battle(DQAction *itemAction, Sequence *seq, DHEXECUTE executeType)
{
	itemAction->Execute(seq);
}

void dh_playSound(DQAction *itemAction, Sequence *seq, DHEXECUTE executeType)
{
	itemAction->Execute(seq);
}

void dh_playWonderMovie(DQAction *itemAction, Sequence *seq, DHEXECUTE executeType)
{
	itemAction->Execute(seq);
}

void dh_playVictoryMovie(DQAction *itemAction, Sequence *seq, DHEXECUTE executeType)
{
	itemAction->Execute(seq);
}

void dh_message(DQAction *itemAction, Sequence *seq, DHEXECUTE executeType)
{
	itemAction->Execute(seq);
}

void dh_faceoff(DQAction *itemAction, Sequence *seq, DHEXECUTE executeType)
{
	if (executeType == DHEXECUTE_NORMAL) {
		itemAction->Execute(seq);
	} else {
		itemAction->ExecuteImmediate(seq);
	}
}

void dh_terminateFaceoff(DQAction *itemAction, Sequence *seq, DHEXECUTE executeType)
{
	itemAction->Execute(seq);
}

void dh_terminateSound(DQAction *itemAction, Sequence *seq, DHEXECUTE executeType)
{
	itemAction->Execute(seq);
}

void dh_speceffect(DQAction *itemAction, Sequence *seq, DHEXECUTE executeType)
{
	itemAction->Execute(seq);
}

void dh_attackpos(DQAction *itemAction, Sequence *seq, DHEXECUTE executeType)
{
	if (executeType == DHEXECUTE_NORMAL) {
		itemAction->Execute(seq);
	} else {
		itemAction->ExecuteImmediate(seq);
	}
}

void dh_invokeThroneRoom(DQAction *itemAction, Sequence *seq, DHEXECUTE executeType)
{
	itemAction->Execute(seq);
}

void dh_invokeResearchAdvance(DQAction *itemAction, Sequence *seq, DHEXECUTE executeType)
{
	itemAction->Execute(seq);
}

void dh_beginScheduler(DQAction *itemAction, Sequence *seq, DHEXECUTE executeType)
{
	itemAction->Execute(seq);
}

DQItem::DQItem(DQAction *action, DQHandler *handler)
:
	m_addedToSavedList  (FALSE),
	m_owner             (PLAYER_UNASSIGNED),
	m_round             (0),
	m_action            (action),
	m_handler           (handler),
	m_sequence          (NULL)
{
	m_sequence = DirectorImpl::Instance()->NewSequence(this);

	if (g_turn)
	{
		m_round = static_cast<uint16>(g_turn->GetRound());
	}
}

DQItem::~DQItem()
{
	//DPRINTF(k_DBG_GAMESTATE, ("Deleting item @ %lx, type=%d\n", this, m_type));
	delete m_sequence;
	delete m_action;
}

#define k_MAX_DISPATCHED_QUEUE_ITEMS		1000
#define k_MAX_SAVED_SEQUENCES				1000

DirectorImpl *DirectorImpl::m_instance = NULL;
DirectorImpl::DirectorImpl(void)
:
	m_activeUnitList            (new tech_WLList<UnitActor *>),
	m_activeEffectList          (new tech_WLList<EffectActor *>),
	m_tradeActorList            (new tech_WLList<TradeActor *>),
	m_nextPlayer                (FALSE),
	m_masterCurTime             (0),
	m_lastTickCount             (0),
	m_timeLogIndex              (0),
	m_averageElapsed            (0),
	m_averageFPS                (k_DEFAULT_FPS),
	m_actionFinished            (TRUE),
	m_paused                    (FALSE),
	m_processingActiveUnits     (FALSE),
	m_processingActiveEffects   (FALSE),
	m_curSequenceID             (0),
	m_lastSequenceID            (0),
	m_dispatchedItems           (new PointerList<DQItem>),
	m_savedItems                (new PointerList<DQItem>),
	m_itemQueue                 (new PointerList<DQItem>),
	m_itemWalker                (new PointerList<DQItem>::Walker),
	m_holdSchedulerSequence     (NULL),
	m_pendingGameActions        (0),
	m_endTurnRequested          (false)
{
	std::fill(m_timeLog, m_timeLog + k_TIME_LOG_SIZE, 0);

	delete m_instance;
	m_instance = this;
}

DirectorImpl::~DirectorImpl(void)
{
	delete m_dispatchedItems;
	delete m_itemQueue;
	delete m_savedItems;
	delete m_itemWalker;
	delete m_activeUnitList;
	delete m_activeEffectList;
	delete m_tradeActorList;
	m_instance = NULL;
}

void DirectorImpl::FastKill(UnitActor *actor)
{
	m_dyingActors.erase(actor);
	actor->DumpAllActions();
	ListPos		pos = m_activeUnitList->Find(actor, m_activeUnitList->GetHeadPosition());
	if (pos)
	{
		if (m_processingActiveUnits)
		{
			actor->SetKillNow();
		}
		else
		{
			m_activeUnitList->DeleteAt(pos);
			m_standbyActors.erase(actor);
			delete actor;
		}
	}
	else
	{
		m_standbyActors.erase(actor);
		delete actor;
	}
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
		switch (walk.GetObj()->m_action->GetType()) {
			case DQACTION_MOVE:
			case DQACTION_TELEPORT: {
				UnitActor *actor = ((DQActionMove *) (walk.GetObj()->m_action))->moveActor;
				if (!actor->IsActive()) {
					m_standbyActors.insert(actor);
				}
				break;
			}
			default:
				break;
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
		ProcessActiveUnits();
		ProcessActiveEffects();
		ProcessTradeRouteAnimations();

		HandleNextAction();
		GarbageCollectItems();

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

void DirectorImpl::DumpSequence(Sequence *seq) {
	DPRINTF(k_DBG_UI, ("  m_sequence->m_sequenceID     :%ld\n", seq->GetSequenceID()));
	DumpItem(seq->GetItem());
}

void DirectorImpl::DumpItem(DQItem *item)
{
	item->m_action->Dump();
}

#endif

#ifdef _DEBUG

void DirectorImpl::DumpInfo(void)
{
	DPRINTF(k_DBG_UI, (" ------------------\n"));
	DPRINTF(k_DBG_UI, ("Director Dump:\n"));
	DPRINTF(k_DBG_UI, (" m_curSequenceID  :%d\n", m_curSequenceID));
	DPRINTF(k_DBG_UI, (" m_lastSequenceID :%d\n", m_lastSequenceID));
	DPRINTF(k_DBG_UI, (" ------------------\n"));
	DPRINTF(k_DBG_UI, (" Dispatched Items:\n"));
	DPRINTF(k_DBG_UI, (" Count:%d\n", m_dispatchedItems->GetCount()));
	DPRINTF(k_DBG_UI, (" ------------------\n"));

	for (
		m_itemWalker->SetList(m_dispatchedItems);
		m_itemWalker->IsValid();
		m_itemWalker->Next()
		)
	{
		DumpItem(m_itemWalker->GetObj());
	}

	DPRINTF(k_DBG_UI, (" ------------------\n"));
	DPRINTF(k_DBG_UI, (" Active Units:\n"));
	DPRINTF(k_DBG_UI, (" Count:%d\n", m_activeUnitList->L()));
	DPRINTF(k_DBG_UI, (" ------------------\n"));

	ListPos		pos = m_activeUnitList->GetHeadPosition();
	for (size_t i = 0; i < m_activeUnitList->L(); ++i)
	{
		m_activeUnitList->GetNext(pos)->DumpActor();
	}

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

	for (std::unordered_set<UnitActor*>::iterator it=m_dyingActors.begin(); it != m_dyingActors.end();) {
		UnitActor *actor = *it;
		// Move iterator forward before killing the actor as it will remove the actor from the set.
		it++;
		FastKill(actor);
	}
}

void DirectorImpl::HandleActor(bool immediate, UnitActor *actor, bool enableAction, Sequence *seq, SEQ_ACTOR seqActor)
{
	if (enableAction && !immediate) {
		seq->SetAddedToActiveList(seqActor, true);
		ActiveUnitAdd(actor);
	} else {
		FinalizeActor(actor);
	}
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
		DQItem *item = m_itemQueue->RemoveHead();

		Assert(item->m_handler);
		if (item->m_handler)
		{
			SetActionFinished(FALSE);

			Assert(m_dispatchedItems->GetCount() == 0);
			m_dispatchedItems->AddTail(item);

			DHEXECUTE executeType = DHEXECUTE_NORMAL;

			if (
				item->m_round < g_turn->GetRound() - 1
				||
				(
					!g_theProfileDB->IsEnemyMoves()
					&&  item->GetOwner() != -1
					&&  item->GetOwner() != g_selected_item->GetVisiblePlayer()
				)
				||
				(
					!g_theProfileDB->IsUnitAnim()
					&&  item->GetOwner() != -1
					&&  g_player[item->GetOwner()] != NULL
					&&  g_player[item->GetOwner()]->IsRobot()
				)
				)
			{
				executeType = DHEXECUTE_IMMEDIATE;
			}

			item->m_handler(item->m_action, item->m_sequence, executeType);
		}
	}
}

void DirectorImpl::ActionFinished(Sequence *seq)
{
	Assert(m_dispatchedItems->GetCount() <= 1);

	DQItem			*item;

	if (!seq) {
		if (m_dispatchedItems->GetCount() > 0) {
			item = m_dispatchedItems->RemoveHead();
			Assert(!item->m_sequence);

			delete item;
		}

		SetActionFinished(TRUE);
		return;
	}

	if (seq->GetSequenceID() == m_lastSequenceID) {
		seq->Release();
		if (seq->GetRefCount() <= 0) {
			SaveFinishedItem(seq->GetItem());

			SetActionFinished(TRUE);
		}
		return;
	}

	if (m_dispatchedItems->GetCount() > 0) {
		item = m_dispatchedItems->RemoveHead();
	} else {
//		sint32 NoDispatchedItem = 0;

		return;
	}

	if (!item->m_sequence)
	{
#ifdef _DEBUG
		DPRINTF(k_DBG_UI, (" ------------------\n"));
		DPRINTF(k_DBG_UI, ("*** Null Sequence Finished\n"));
		DumpItem(item);
		DPRINTF(k_DBG_UI, (" ------------------\n"));
#endif
		return;
	}

	if (item->m_sequence->GetSequenceID() == seq->GetSequenceID())
	{
		m_lastSequenceID = seq->GetSequenceID();
		seq->Release();
		if (seq->GetRefCount() <= 0)
		{
			SaveFinishedItem(item);
			SetActionFinished(TRUE);
		}
	}
	else
	{
		if (item->m_sequence->GetSequenceID() < seq->GetSequenceID())
		{
			m_lastSequenceID = seq->GetSequenceID();
			delete m_dispatchedItems->RemoveHead();
		}

		SetActionFinished(TRUE);
	}
}

Sequence *DirectorImpl::NewSequence(DQItem *item)
{
	return new Sequence(++m_curSequenceID, item);
}

void DirectorImpl::HandleFinishedItem(DQItem *item)
{
	Assert(item);
	if (!item) return;

	Sequence *seq = item->m_sequence;

	bool	removePrimaryFromActiveList = false;
	bool	removeSecondaryFromActiveList = false;

	if (seq) {
		removePrimaryFromActiveList = seq->GetAddedToActiveList(SEQ_ACTOR_PRIMARY);
		removeSecondaryFromActiveList = seq->GetAddedToActiveList(SEQ_ACTOR_SECONDARY);
	}

	item->m_action->Finalize(removePrimaryFromActiveList, removeSecondaryFromActiveList);

	delete item;
}

void DirectorImpl::SaveFinishedItem(DQItem *item)
{
	if (item->m_addedToSavedList) return;

	item->m_addedToSavedList = TRUE;

	m_savedItems->AddTail(item);
}

void DirectorImpl::GarbageCollectItems(void)
{
	while (m_savedItems->GetCount() > 0)
	{
		HandleFinishedItem(m_savedItems->RemoveHead());
	}
}

void DirectorImpl::ProcessImmediately(DQItem *item)
{
	item->m_handler(item->m_action, item->m_sequence, DHEXECUTE_IMMEDIATE);
}

void DirectorImpl::CatchUp(void)
{
	ListPos pos = m_activeUnitList->GetHeadPosition();
	while (pos) {
		UnitActor *actor = m_activeUnitList->GetNext(pos);
		FinalizeActor(actor);
	}

	while(m_itemQueue->GetCount())
	{
		DQItem *    item    = m_itemQueue->RemoveHead();

		Assert(item->m_handler);
		if (item->m_handler)
		{
			m_dispatchedItems->AddTail(item);

			item->m_handler(item->m_action, item->m_sequence, DHEXECUTE_IMMEDIATE);
		}
	}

	pos = m_activeUnitList->GetHeadPosition();
	while (pos) {
		UnitActor *actor = m_activeUnitList->GetNext(pos);
		FinalizeActor(actor);
	}

	GarbageCollectItems();

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
	ProcessStandbyUnits();
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

void DirectorImpl::ActiveUnitAdd(UnitActor *unitActor)
{
	if(!unitActor)
		return;

	if(!unitActor->IsActive())
		m_activeUnitList->AddHead(unitActor);

	unitActor->SetActive(true);

	unitActor->AddActiveListRef();
}

void DirectorImpl::ActiveUnitRemove(UnitActor *unitActor)
{
	if(!unitActor)
		return;

	ListPos		pos =
		m_activeUnitList->Find(unitActor, m_activeUnitList->GetHeadPosition());

	if (pos)
	{
		if (unitActor->ReleaseActiveListRef() <= 0)
		{
			m_activeUnitList->DeleteAt(pos);
			unitActor->SetActive(false);
			// Prevent getting stuck waiting for a deleted item.
			if (m_activeUnitList->IsEmpty())
			{
				SetActionFinished(TRUE);
			}
		}
	}
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

#if 0
BOOL DirectorImpl::IsActive(UnitActor *unitActor)
{
	ListPos		pos;
	UnitActor	*actor;

	pos = m_activeUnitList->Find(unitActor, m_activeUnitList->GetHeadPosition());
	if (pos)
	{


 		actor = m_activeUnitList->GetNext(pos);
		Assert(actor);
		if (actor)
		{

			return TRUE;
		}
		else
		{
			return FALSE;
		}
	}
	else
	{
		return FALSE;
	}
}
#endif

void DirectorImpl::ProcessStandbyUnits()
{
	m_standbyActors.clear();
	UpdateStandbyUnits();

	for (std::set<UnitActor*>::iterator it=m_standbyActors.begin(); it != m_standbyActors.end(); ++it) {
		(*it)->Process();
	}
}

uint32 DirectorImpl::ProcessActiveUnits(void)
{
	if (m_activeUnitList->IsEmpty()) return 0;

	m_processingActiveUnits = TRUE;
	UnitActor		*actor;

	ListPos			pos = m_activeUnitList->GetHeadPosition();

	for (size_t i = m_activeUnitList->L(); i > 0; --i)
	{
		Assert(pos);
		if (pos) {
			actor = m_activeUnitList->GetNext(pos);

			if (actor)
				actor->Process();
		}
	}

	m_processingActiveUnits = FALSE;

	pos = m_activeUnitList->GetHeadPosition();
	for (size_t n = m_activeUnitList->L(); n > 0; --n)
	{
		ListPos actorPos = pos;
		actor = m_activeUnitList->GetNext(pos);
		if (actor && actor->GetKillNow()) {
			FastKill(actor);
		}
	}

	return 0;
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
	ListPos             pos             = m_activeUnitList->GetHeadPosition();
	size_t const        numToProcess    = m_activeUnitList->L();

	for (size_t i = 0; i < numToProcess; ++i)
	{
		UnitActor *     actor = m_activeUnitList->GetNext(pos);
		if (actor)
		{
			actor->SetX(actor->GetX() + deltaX);
			actor->SetY(actor->GetY() + deltaY);
		}
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
	m_nextPlayer = TRUE;

	if (!g_network.IsActive()) {
		ListPos pos = m_activeUnitList->GetHeadPosition();
		while (pos) {
			UnitActor *actor = m_activeUnitList->GetNext(pos);
			FinalizeActor(actor);
		}
	}

	while(m_itemQueue->GetCount())
	{
		DQItem *    item    = m_itemQueue->RemoveHead();

		Assert(item->m_handler);
		if (item->m_handler)
		{
			m_dispatchedItems->AddTail(item);

			item->m_handler(item->m_action, item->m_sequence, DHEXECUTE_NORMAL);
		}
	}

	if(!g_network.IsActive()) {
		ListPos pos = m_activeUnitList->GetHeadPosition();
		while (pos) {
			UnitActor *actor = m_activeUnitList->GetNext(pos);
			FinalizeActor(actor);
		}
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
	m_nextPlayer = FALSE;

	ListPos     pos             = m_activeUnitList->GetHeadPosition();
	size_t      numToProcess    = m_activeUnitList->L();

	for (size_t i = 0; i < numToProcess; i++)
	{
		UnitActor* actor = m_activeUnitList->GetNext(pos);
		DrawUnitActor(paintRect, actor, false);
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

UnitActor *DirectorImpl::GetClickedActiveUnit(aui_MouseEvent *data)
{
	POINT   mouse   = data->position;
	ListPos pos     = m_activeUnitList->GetHeadPosition();

	for (size_t i = 0; i < m_activeUnitList->L(); i++)
	{
		UnitActor * actor = m_activeUnitList->GetNext(pos);

		if (actor)
		{
			actor->Process();
			MapPoint pos = actor->GetPos();
			if (g_tiledMap->TileIsVisible(pos.x, pos.y))
			{
				RECT	actorRect;

				SetRect(&actorRect, actor->GetX(), actor->GetY(),
						actor->GetX() + (sint32)actor->GetWidth(),
						actor->GetY() + (sint32)actor->GetHeight());

				if (PtInRect(&actorRect, mouse))
				{
					return actor;
				}
			}
		}
	}

	return NULL;
}

bool DirectorImpl::IsProcessing()
{
	return (m_itemQueue->GetCount() > 0);
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
	DQItem				*item = new DQItem(action, dh_move);
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

	DQActionMove		*action = new DQActionTeleport(
			top->GetActor(),
			oldPos,
			newPos,
			arraySize,
			moveActors,
			numRevealed,
			revealedActors);
	DQItem				*item	= new DQItem(action, dh_teleport);

	m_itemQueue->AddTail(item);
}

void DirectorImpl::AddProjectileAttack(Unit shooting, Unit target, SpriteState *projectile_state,
									   SpriteState *projectileEnd_state, sint32 projectile_Path)
{
	DQActionMoveProjectile	*action = new DQActionMoveProjectile(
			projectileEnd_state, shooting.RetPos(), target.RetPos());
	DQItem					*item = new DQItem(action, dh_projectileMove);

	m_itemQueue->AddTail(item);
}

void DirectorImpl::AddSpecialEffect(MapPoint &pos, sint32 spriteID, sint32 soundID)
{
	DQActionSpecialEffect	*action = new DQActionSpecialEffect(pos, spriteID, soundID);
	DQItem					*item = new DQItem(action, dh_speceffect);

	m_itemQueue->AddTail(item);
}

void DirectorImpl::AddCombatFlash(MapPoint const & pos)
{
	DQActionCombatFlash	*   action  = new DQActionCombatFlash(pos);
	DQItem *                item    = new DQItem(action, dh_combatflash);

	m_itemQueue->AddTail(item);
}

void DirectorImpl::AddCopyVision(void)
{
	DQActionCopyVision	*action = new DQActionCopyVision();
	DQItem				*item = new DQItem(action, dh_copyVision);

	m_itemQueue->AddTail(item);
}

void DirectorImpl::AddCenterMap(const MapPoint &pos)
{
	DQActionCenterMap	*action = new DQActionCenterMap(pos);
	DQItem				*item = new DQItem(action, dh_centerMap);

	m_itemQueue->AddTail(item);
}

void DirectorImpl::AddSelectUnit(uint32 flags)
{
	DQActionUnitSelection	*action = new DQActionUnitSelection(flags);
	DQItem				*item = new DQItem(action, dh_selectUnit);

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
	DQItem				*item = new DQItem(action, dh_endTurn);

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
	DQItem				*item = new DQItem(action, dh_attack);
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
	DQItem *item = new DQItem(action, dh_attackpos);
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

	DQItem *            item            = new DQItem(action, dh_specialAttack);
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
	DQItem			*item = new DQItem(action, dh_death);
	item->SetOwner(dead->GetPlayerNum());

	m_dyingActors.insert(dead);
	m_itemQueue->AddTail(item);
}

void DirectorImpl::AddMorphUnit(UnitActor *morphingActor, SpriteState *ss, sint32 type, Unit id)
{
	if (!morphingActor)
		return;

	DQActionMorph	*action = new DQActionMorph(morphingActor, ss, type, id);
	DQItem			*item = new DQItem(action, dh_morphUnit);
	m_itemQueue->AddTail(item);
}

void DirectorImpl::AddHide(Unit hider)
{
	UnitActor	*actor = hider.GetActor();
	Assert(actor);
	if (!actor) return;

	DQActionHideShow	*action = new DQActionHide(actor);
	DQItem				*item = new DQItem(action, dh_hide);

	m_itemQueue->AddTail(item);
}

void DirectorImpl::AddShow(Unit hider)
{
	UnitActor	*actor = hider.GetActor();
	Assert(actor);
	if (!actor) return;

	DQActionHideShow	*action = new DQActionShow(actor, hider.RetPos());
	DQItem				*item = new DQItem(action, dh_show);

	m_itemQueue->AddTail(item);
}

void DirectorImpl::AddWork(Unit worker)
{
	if (!worker.GetActor())
		return;

	DQActionWork	*action = new DQActionWork(worker.GetActor(), worker.RetPos(), worker.GetWorkSoundID());
	DQItem			*item   = new DQItem(action, dh_work);
	item->SetOwner(worker.GetOwner());

	m_itemQueue->AddTail(item);
}

void DirectorImpl::AddFastKill(UnitActor *dead)
{
	Assert(dead);
	if (!dead) return;

	DQActionFastKill	*action = new DQActionFastKill(dead);
	DQItem				*item = new DQItem(action, dh_fastkill);

	m_itemQueue->AddTail(item);
}

void DirectorImpl::AddRemoveVision(const MapPoint &pos, double range)
{
	DQActionVision		*action = new DQActionRemoveVision(pos, range);
	DQItem				*item = new DQItem(action, dh_removeVision);

	m_itemQueue->AddTail(item);
}

void DirectorImpl::AddAddVision(const MapPoint &pos, double range)
{
	DQActionVision *    action  = new DQActionAddVision(pos, range);
	DQItem				*item = new DQItem(action, dh_addVision);

	m_itemQueue->AddTail(item);
}

void DirectorImpl::AddSetVisibility(UnitActor *actor, uint32 visibility)
{
	Assert(actor);
	if (!actor) return;

	DQActionSetVisibility * action  = new DQActionSetVisibility(actor, visibility);
	DQItem				*item = new DQItem(action, dh_setVisibility);

	m_itemQueue->AddTail(item);
}

void DirectorImpl::AddSetOwner(UnitActor *actor, sint32 owner)
{
	Assert(actor);
	if (!actor) return;

	DQActionSetOwner *  action  = new DQActionSetOwner(actor, owner);
	DQItem				*item = new DQItem(action, dh_setOwner);

	m_itemQueue->AddTail(item);
}

void DirectorImpl::AddSetVisionRange(UnitActor *actor, double range)
{
	Assert(actor);
	if (!actor) return;

	DQActionSetVisionRange *    action  = new DQActionSetVisionRange(actor, range);
	DQItem				*item = new DQItem(action, dh_setVisionRange);

	m_itemQueue->AddTail(item);
}

void DirectorImpl::AddBattle(Battle *battle)
{
	if (!battle)
		return;

	DQActionBattle *    action  = new DQActionBattle(battle);
	DQItem				*item = new DQItem(action, dh_battle);

	m_itemQueue->AddTail(item);
}

void DirectorImpl::AddPlaySound(sint32 soundID, MapPoint const & pos)
{
	if (soundID <= 0) return;

	DQActionPlaySound * action  = new DQActionPlaySound(soundID, pos);
	DQItem				*item = new DQItem(action, dh_playSound);

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
	DQItem				*item = new DQItem(action, dh_playWonderMovie);

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
	DQItem				*item = new DQItem(action, dh_playVictoryMovie);

	m_itemQueue->AddTail(item);
}

void DirectorImpl::AddMessage(const Message & message)
{
	DQActionMessage	*   action  = new DQActionMessage(message);
	DQItem				*item = new DQItem(action, dh_message);

	m_itemQueue->AddTail(item);
}

void DirectorImpl::AddFaceoff(Unit &attacker, Unit &defender)
{
	if (!attacker.IsValid() || !attacker.GetActor() || !defender.IsValid() || !defender.GetActor())
		return;

	DQActionFaceoff	*   action  = new DQActionFaceoff(attacker.GetActor(), attacker.RetPos(),
			defender.GetActor(), defender.RetPos());
	DQItem						*item = new DQItem(action, dh_faceoff);

	m_itemQueue->AddTail(item);
}

void DirectorImpl::AddTerminateFaceoff(Unit &faceroffer)
{
	if (!faceroffer->GetActor())
		return;

	DQActionTerminateFaceOff	*action = new DQActionTerminateFaceOff(faceroffer->GetActor());
	DQItem						*item = new DQItem(action, dh_terminateFaceoff);

	m_itemQueue->AddTail(item);
}

void DirectorImpl::AddTerminateSound(Unit &unit)
{
	if (!unit.IsValid())
		return;

	DQActionTerminateSound *    action  = new DQActionTerminateSound(unit.m_id);
	DQItem						*item = new DQItem(action, dh_terminateSound);

	m_itemQueue->AddTail(item);
}

void DirectorImpl::AddInvokeThroneRoom(void)
{
	DQActionInvokeThroneRoom *  action = new DQActionInvokeThroneRoom;
	DQItem						*item = new DQItem(action, dh_invokeThroneRoom);

	m_itemQueue->AddTail(item);
}

void DirectorImpl::AddInvokeResearchAdvance(MBCHAR * message)
{
	if (!message)
		return;

	DQActionInvokeResearchAdvance * action = new DQActionInvokeResearchAdvance(message);
	DQItem						*item = new DQItem(action, dh_invokeResearchAdvance);

	m_itemQueue->AddTail(item);
}

void DirectorImpl::AddBeginScheduler(sint32 player)
{
	Assert(player == g_selected_item->GetCurPlayer());
	if(player != g_selected_item->GetCurPlayer())
		return;

	DPRINTF(k_DBG_GAMESTATE, ("Director::AddBeginScheduler(%d)\n", player));

	DQActionBeginScheduler *    action = new DQActionBeginScheduler(player);
	DQItem						*item = new DQItem(action, dh_beginScheduler);

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
