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
#include "directoractions.h"
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

enum DQITEM_TYPE {
	DQITEM_MOVE,
	DQITEM_MOVEPROJECTILE,
	DQITEM_SPECEFFECT,
	DQITEM_ATTACK,
	DQITEM_ATTACKPOS,
	DQITEM_SPECATTACK,
	DQITEM_DEATH,
	DQITEM_MORPH,
	DQITEM_HIDE,
	DQITEM_SHOW,
	DQITEM_WORK,
	DQITEM_FASTKILL,
	DQITEM_ADDVISION,
	DQITEM_REMOVEVISION,
	DQITEM_SETOWNER,
	DQITEM_SETVISIBILITY,
	DQITEM_SETVISIONRANGE,
	DQITEM_COMBATFLASH,
	DQITEM_TELEPORT,
	DQITEM_COPYVISION,
	DQITEM_CENTERMAP,
	DQITEM_SELECTUNIT,
	DQITEM_ENDTURN,
	DQITEM_BATTLE,
	DQITEM_PLAYSOUND,
	DQITEM_PLAYWONDERMOVIE,
	DQITEM_PLAYVICTORYMOVIE,
	DQITEM_MESSAGE,
	DQITEM_FACEOFF,
	DQITEM_TERMINATE_FACEOFF,
	DQITEM_TERMINATE_SOUND,

	DQITEM_INVOKE_THRONE_ROOM,

	DQITEM_INVOKE_RESEARCH_ADVANCE,

	DQITEM_BEGIN_SCHEDULER,

	DQITEM_MAX
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

typedef void (DQHandler)(DQAction *action, Sequence *seq, DHEXECUTE executeType);

class DQItem {
public:
	DQItem(DQITEM_TYPE type, DQAction *action, DQHandler *handler);
	~DQItem();

	void			SetOwner(sint32 owner) { m_owner = (sint8)owner; }
	sint32			GetOwner(void) { return (sint32) m_owner; }

	DQITEM_TYPE		m_type;
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
		m_addedToActiveList[SEQ_ACTOR_PRIMARY]      = FALSE;
		m_addedToActiveList[SEQ_ACTOR_SECONDARY]    = FALSE;
	}

	~Sequence() {}

	sint32 GetSequenceID(void) { return m_sequenceID; }
	void	AddRef(void) { m_refCount++; }
	void	Release(void) { m_refCount--; }
	sint32	GetRefCount(void) { return m_refCount; }

	DQItem	*GetItem(void) { return m_item; }

	void	SetAddedToActiveList(SEQ_ACTOR which, BOOL added) { m_addedToActiveList[which] = added; }
	BOOL	GetAddedToActiveList(SEQ_ACTOR which) { return m_addedToActiveList[which]; }

private:
	sint32	m_sequenceID;
	sint32	m_refCount;
	DQItem	*m_item;
	BOOL	m_addedToActiveList[SEQ_ACTOR_MAX];
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
	bool 		TileIsVisibleToPlayer(MapPoint &pos);
	void		ActiveUnitAdd(UnitActor *unitActor);
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
	PointerList<DQItem>::Walker	*m_itemWalker;

	sint32						m_pendingGameActions;
	bool						m_endTurnRequested;
};

Director* Director::CreateDirector() {
	return new DirectorImpl();
}

void dh_move(DQAction *itemAction, Sequence *seq, DHEXECUTE executeType)
{
	DQActionMove	*action				= (DQActionMove *)itemAction;
	UnitActor		*theActor			= action->move_actor;
	uint32			maxActionCounter	= 1;
	sint32			speed				= g_theProfileDB->GetUnitSpeed();
	BOOL			visible				= FALSE;

	Assert(theActor != NULL);

	if (theActor==NULL)
	{
		DirectorImpl::Instance()->ActionFinished(seq);
		return;
	}

	if (g_theUnitPool && !g_theUnitPool->IsValid(theActor->GetUnitID())) {
		DirectorImpl::Instance()->ActionFinished(seq);
		return;
	}

	Action *actionObj = new Action();

	Assert(actionObj != NULL);

	if (actionObj == NULL)
		return;

	MapPoint  oldP=action->move_oldPos;
	MapPoint  newP=action->move_newPos;

	actionObj->SetSequence(seq);
	seq->AddRef();

	actionObj->SetStartMapPoint(oldP);
	actionObj->SetEndMapPoint  (newP);

	actionObj->CreatePath(oldP.x,oldP.y,newP.x,newP.y);

	theActor->PositionActor(oldP);

	if (g_theProfileDB->IsUnitAnim())
		maxActionCounter = k_MAX_UNIT_MOVEMENT_ITERATIONS - speed;

	actionObj->SetMaxActionCounter(maxActionCounter);
	actionObj->SetCurActionCounter(0);

	actionObj->SetSoundEffect(action->move_soundID);

	actionObj->SetMoveActors(action->moveActors, action->moveArraySize);

	if(!theActor->ActionMove(actionObj))
	{
		DirectorImpl::Instance()->ActionFinished(seq);
		return;
	}

	visible =	DirectorImpl::Instance()->TileIsVisibleToPlayer(oldP)||
				DirectorImpl::Instance()->TileIsVisibleToPlayer(newP);

	if (visible && executeType == DHEXECUTE_NORMAL)
	{
		seq->SetAddedToActiveList(SEQ_ACTOR_PRIMARY, TRUE);
		DirectorImpl::Instance()->ActiveUnitAdd(theActor);

		if (g_selected_item->GetVisiblePlayer()!= theActor->GetPlayerNum()
			&& !g_tiledMap->TileIsVisible(theActor->GetPos().x, theActor->GetPos().y))
		{
			DirectorImpl::Instance()->CenterMap(theActor->GetPos());
		}
	}
	else
	{
		if (theActor->WillDie())
			DirectorImpl::Instance()->FastKill(theActor);
		else
			theActor->EndTurnProcess();
	}
}

void dh_teleport(DQAction *itemAction, Sequence *seq, DHEXECUTE executeType)
{
	DQActionMove	*action = (DQActionMove *)itemAction;

	UnitActor		*theActor = action->move_actor;
	sint32			i;

	if (theActor == NULL) return;

	if( action->revealedActors != NULL)
	{
		UnitActor		*tempActor;

		for (i=0; i<action->numRevealed; i++)
		{
			tempActor = action->revealedActors[i];
			if (tempActor)
				tempActor->SetVisSpecial(TRUE);
		}
	}

	theActor->PositionActor(action->move_newPos);

	for (i=0; i<action->moveArraySize; i++)
	{
		UnitActor *moveActor = action->moveActors[i];
		moveActor->PositionActor(action->move_newPos);
	}

	DirectorImpl::Instance()->ActionFinished(seq);
}

void dh_projectileMove(DQAction *itemAction, Sequence *seq, DHEXECUTE executeType)
{
	DQActionMoveProjectile	*action = (DQActionMoveProjectile *)itemAction;

	EffectActor			*projectileEnd = action->end_projectile;
	UnitActor			*shootingActor = action->pshooting_actor;
	UnitActor			*targetActor = action->ptarget_actor;
	MapPoint			startPos = action->pmove_oldPos;
	MapPoint			endPos = action->pmove_newPos;

	Assert(shootingActor != NULL && targetActor != NULL);

	if (shootingActor == NULL || targetActor == NULL)
		return;

	if (projectileEnd && DirectorImpl::Instance()->TileIsVisibleToPlayer(startPos))
	{
		Action *actionObj= NULL;




		Anim * anim = projectileEnd->CreateAnim(EFFECTACTION_PLAY);
		if (anim == NULL)
		{

			anim = projectileEnd->CreateAnim(EFFECTACTION_FLASH);
			Assert(anim != NULL);
			if (anim == NULL)
			{
				DirectorImpl::Instance()->ActionFinished(seq);
				return;
			}
			else
			{
				actionObj = new Action(EFFECTACTION_FLASH, ACTIONEND_PATHEND);
			}
		}
		else
		{
			actionObj = new Action(EFFECTACTION_PLAY, ACTIONEND_PATHEND);
		}

		Assert(actionObj);
		if (actionObj)
		{
			actionObj->SetAnim(anim);
			projectileEnd->AddAction(actionObj);
			DirectorImpl::Instance()->ActiveEffectAdd(projectileEnd);

			// Management taken over by director, no longer managed by item queue.
			action->end_projectile = NULL;
		}
		else
		{
			delete anim;
		}
	}

	DirectorImpl::Instance()->ActionFinished(seq);
}

void dh_attack(DQAction *itemAction, Sequence *seq, DHEXECUTE executeType)
{
	DQActionAttack	*action = (DQActionAttack *)itemAction;

	UnitActor	*theAttacker = action->attacker;
	UnitActor	*theDefender = action->defender;

	Assert(theAttacker != NULL);
	Assert(theDefender != NULL);

	if ((theAttacker == NULL)||(theDefender == NULL)) return;

//	bool attackerVisible = DirectorImpl::Instance()->TileIsVisibleToPlayer(action->attacker_Pos);
	bool defenderVisible = DirectorImpl::Instance()->TileIsVisibleToPlayer(action->defender_Pos);

	bool playerInvolved  = (theDefender->GetPlayerNum() == g_selected_item->GetVisiblePlayer()) ||
						   (theAttacker->GetPlayerNum() == g_selected_item->GetVisiblePlayer());

	POINT  AttackerPoints, DefenderPoints;

	maputils_MapXY2PixelXY(action->attacker_Pos.x, action->attacker_Pos.y, AttackerPoints);
	maputils_MapXY2PixelXY(action->defender_Pos.x, action->defender_Pos.y, DefenderPoints);

	sint32  deltax=DefenderPoints.x-AttackerPoints.x;
	sint32  deltay=DefenderPoints.y-AttackerPoints.y;

	sint32 facingIndex=spriteutils_DeltaToFacing(deltax,deltay);

	Action * ActionObj = new Action();

	Assert(ActionObj != NULL);
	if (ActionObj == NULL)
	{
		c3errors_ErrorDialog("Director", "Could not allocate attacker action object");
		return;
	}

	ActionObj->SetSequence(seq);
	seq->AddRef();

	ActionObj->SetStartMapPoint(action->attacker_Pos);
	ActionObj->SetEndMapPoint  (action->attacker_Pos);

	theAttacker->ActionAttack(ActionObj,facingIndex);

	if (playerInvolved && (executeType == DHEXECUTE_NORMAL))
	{
		seq->SetAddedToActiveList(SEQ_ACTOR_PRIMARY, TRUE);
		DirectorImpl::Instance()->ActiveUnitAdd(theAttacker);
	}
	else
	{
		if (theAttacker->WillDie())
			DirectorImpl::Instance()->FastKill(theAttacker);
		else
			theAttacker->EndTurnProcess();
	}

	if (!action->defender_IsCity)
	{
		facingIndex=spriteutils_DeltaToFacing(-deltax,-deltay);

		ActionObj = new Action();

		Assert(ActionObj != NULL);

		if (ActionObj == NULL)
		{
			c3errors_ErrorDialog("Director", "Could not allocate defender action object");
			return;
		}

		ActionObj->SetSequence(seq);
		seq->AddRef();

		ActionObj->SetStartMapPoint(action->defender_Pos);
		ActionObj->SetEndMapPoint  (action->defender_Pos);

		theDefender->ActionAttack(ActionObj,facingIndex);

		if(playerInvolved)
			defenderVisible = true;

		if(defenderVisible && (executeType == DHEXECUTE_NORMAL))
		{
			seq->SetAddedToActiveList(SEQ_ACTOR_SECONDARY, TRUE);
			DirectorImpl::Instance()->ActiveUnitAdd(theDefender);
		}
		else
		{
			if (theDefender->WillDie())
				DirectorImpl::Instance()->FastKill(theDefender);
			else
				theDefender->EndTurnProcess();
		}
	}
}

void dh_specialAttack(DQAction *itemAction, Sequence *seq, DHEXECUTE executeType)
{
	DQActionAttack	*action = (DQActionAttack *)itemAction;

	UnitActor	*theAttacker = action->attacker;
	UnitActor	*theDefender = action->defender;

	Assert(theAttacker != NULL);
	Assert(theDefender != NULL);

	if (theAttacker == NULL) return;
	if (theDefender == NULL) return;

	BOOL attackerCanAttack = !action->attacker_IsCity;
	BOOL defenderIsAttackable = !action->defender_IsCity;

	if (!attackerCanAttack && !defenderIsAttackable)
	{
		DirectorImpl::Instance()->ActionFinished(seq);
		return;
	}

	sint32 facingIndex;
	POINT  AttackerPoints, DefenderPoints;

	maputils_MapXY2PixelXY(action->attacker_Pos.x,
						   action->attacker_Pos.y,
						   AttackerPoints
	);
	maputils_MapXY2PixelXY(action->defender_Pos.x,
						   action->defender_Pos.y,
						   DefenderPoints
	);

	sint32  deltax=DefenderPoints.x-AttackerPoints.x;
	sint32  deltay=DefenderPoints.y-AttackerPoints.y;

	if(action->attacker_ID >= 0) {

		g_soundManager->AddSound(SOUNDTYPE_SFX, (uint32)0, action->attacker_ID, 0, 0);
	}

	if (attackerCanAttack)
	{
		Action *AttackerActionObj = new Action();

		Assert(AttackerActionObj != NULL);
		if (AttackerActionObj == NULL)
		{
			c3errors_ErrorDialog("Director", "Could not allocate attacker action object");
			return;
		}

		AttackerActionObj->SetStartMapPoint(action->attacker_Pos);
		AttackerActionObj->SetEndMapPoint  (action->attacker_Pos);

		facingIndex=spriteutils_DeltaToFacing(deltax,deltay);

		AttackerActionObj->SetSequence(seq);
		seq->AddRef();

		if(!theAttacker->ActionSpecialAttack(AttackerActionObj,facingIndex))
		{
			delete AttackerActionObj;
			DirectorImpl::Instance()->ActionFinished(seq);
			return;
		}

		if (DirectorImpl::Instance()->TileIsVisibleToPlayer(action->attacker_Pos) && executeType == DHEXECUTE_NORMAL)
		{
			seq->SetAddedToActiveList(SEQ_ACTOR_PRIMARY, TRUE);
			DirectorImpl::Instance()->ActiveUnitAdd(theAttacker);
		}
		else
		{
			if (theAttacker->WillDie())
				DirectorImpl::Instance()->FastKill(theAttacker);
			else
				theAttacker->EndTurnProcess();
		}
	}

	if (defenderIsAttackable)
	{
		Action *DefenderActionObj = new Action();

		Assert(DefenderActionObj != NULL);
		if (DefenderActionObj == NULL)
		{
			c3errors_ErrorDialog("Director", "Could not allocate defender action object");
			return;
		}

		DefenderActionObj->SetStartMapPoint(action->defender_Pos);
		DefenderActionObj->SetEndMapPoint  (action->defender_Pos);

		facingIndex=spriteutils_DeltaToFacing(-deltax,-deltay);

		DefenderActionObj->SetSequence(seq);
		seq->AddRef();

		if(!theDefender->ActionSpecialAttack(DefenderActionObj,facingIndex))
		{
			delete DefenderActionObj;
			DirectorImpl::Instance()->ActionFinished(seq);
			return;
		}

		if (DirectorImpl::Instance()->TileIsVisibleToPlayer(action->defender_Pos) && executeType == DHEXECUTE_NORMAL)
		{
			seq->SetAddedToActiveList(SEQ_ACTOR_SECONDARY, TRUE);
			DirectorImpl::Instance()->ActiveUnitAdd(theDefender);
		}
		else
		{
			if (theDefender->WillDie())
				DirectorImpl::Instance()->FastKill(theDefender);
			else
				theDefender->EndTurnProcess();
		}
	}
}

void dh_death(DQAction *itemAction, Sequence *seq, DHEXECUTE executeType)
{
	DQActionDeath * action			= (DQActionDeath *)itemAction;
	UnitActor *     dead			= action->dead;
	Anim *          deathAnim		= NULL;
	sint32		    deathActionType	= UNITACTION_NONE;

	Assert(dead);
	if (!dead->GetNeedsToDie())
	{
		dead->SetNeedsToDie(TRUE);

		if(dead->HasDeath())
		{
			if (dead->GetLoadType() != LOADTYPE_FULL)
				dead->FullLoad(UNITACTION_VICTORY);

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

		Action *deadActionObj = new Action(	(UNITACTION)deathActionType, ACTIONEND_ANIMEND);
		Assert(deadActionObj != NULL);
		if (deadActionObj == NULL)
		{
			c3errors_ErrorDialog("Director", "Internal Failure to create death action");
			delete deathAnim;
			return;
		}

		deadActionObj->SetSequence(seq);
		seq->AddRef();

		deadActionObj->SetStartMapPoint(action->dead_pos);
		deadActionObj->SetEndMapPoint(action->dead_pos);

		deadActionObj->SetAnim(deathAnim);

		deadActionObj->SetUnitVisionRange(dead->GetUnitVisionRange());
		deadActionObj->SetUnitsVisibility(dead->GetUnitVisibility());
		deadActionObj->SetFacing(dead->GetFacing());

		if (g_soundManager)
			g_soundManager->TerminateLoopingSound(SOUNDTYPE_SFX, (uint32)dead->GetUnitID());

		if (g_soundManager) {
			sint32 visiblePlayer = g_selected_item->GetVisiblePlayer();
			if ((visiblePlayer == dead->GetPlayerNum()) ||
				(dead->GetUnitVisibility() & (1 << visiblePlayer))) {
				g_soundManager->AddSound(SOUNDTYPE_SFX, (uint32)dead->GetUnitID(), action->dead_soundID,
										 dead->GetPos().x, dead->GetPos().y);
			}
		}

		dead->AddAction(deadActionObj);

		if (DirectorImpl::Instance()->TileIsVisibleToPlayer(action->dead_pos)
			&& executeType == DHEXECUTE_NORMAL) {
			seq->SetAddedToActiveList(SEQ_ACTOR_PRIMARY, TRUE);
			DirectorImpl::Instance()->ActiveUnitAdd(dead);
		} else {
			if (dead->WillDie()) {
				DirectorImpl::Instance()->FastKill(dead);
			} else {
				dead->EndTurnProcess();
			}
			DirectorImpl::Instance()->ActionFinished(seq);
		}
	} else {
		DirectorImpl::Instance()->ActionFinished(seq);
	}
}

void dh_morphUnit(DQAction *itemAction, Sequence *seq, DHEXECUTE executeType)
{
	DQActionMorph   *action = (DQActionMorph *)itemAction;

	UnitActor       *theActor = action->morphing_actor;

	Assert(theActor != NULL);
	if (theActor)
	{
		theActor->ChangeType(action->ss, action->type, action->id, FALSE);
	}

	DirectorImpl::Instance()->ActionFinished(seq);
}

void dh_hide(DQAction *itemAction, Sequence *seq, DHEXECUTE executeType)
{
	DQActionHideShow	*action = (DQActionHideShow *)itemAction;

	Assert(action);
	if (!action) return;

	UnitActor * actor = action->hiding_actor;

	Assert(actor);
	if (!actor) return;

	actor->Hide();

	DirectorImpl::Instance()->ActionFinished(seq);
}

void dh_show(DQAction *itemAction, Sequence *seq, DHEXECUTE executeType)
{
	DQActionHideShow	*action = (DQActionHideShow *)itemAction;

	Assert(action);
	if (!action) return;

	UnitActor * actor = action->hiding_actor;

	Assert(actor);
	if (!actor) return;

	actor->PositionActor(action->hiding_pos);
	actor->Show();

	DirectorImpl::Instance()->ActionFinished(seq);
}

void dh_work(DQAction *itemAction, Sequence *seq, DHEXECUTE executeType)
{
	DQActionWork	*action = (DQActionWork *)itemAction;

	Assert(action);
	if (!action) return;

	UnitActor * actor = action->working_actor;

	Assert(actor);
	if (!actor) return;

	Action *actionObj = new Action(UNITACTION_WORK, ACTIONEND_ANIMEND);

	Assert(actionObj != NULL);
	if (actionObj == NULL) return;

	actionObj->SetStartMapPoint(action->working_pos);
	actionObj->SetEndMapPoint(action->working_pos);

	if (actor->GetLoadType() != LOADTYPE_FULL)
		actor->FullLoad(UNITACTION_WORK);

	Anim	*anim = actor->CreateAnim(UNITACTION_WORK);
	if (anim == NULL)
	{
		anim = actor->CreateAnim(UNITACTION_MOVE);

		if (!anim) {
			delete actionObj;
			DirectorImpl::Instance()->ActionFinished(seq);
			return;
		}
	}

	actionObj->SetSequence(seq);
	seq->AddRef();

	actionObj->SetAnim(anim);

	actor->AddAction(actionObj);

	if (g_soundManager)
	{
		sint32 visiblePlayer = g_selected_item->GetVisiblePlayer();
		if ((visiblePlayer == actor->GetPlayerNum()) ||
			(actor->GetUnitVisibility() & (1 << visiblePlayer))) {
			g_soundManager->AddSound(SOUNDTYPE_SFX, (uint32)actor->GetUnitID(), action->working_soundID,
									 actor->GetPos().x, actor->GetPos().y);
		}
	}

	if (DirectorImpl::Instance()->TileIsVisibleToPlayer(action->working_pos) && executeType == DHEXECUTE_NORMAL)
	{
		seq->SetAddedToActiveList(SEQ_ACTOR_PRIMARY, TRUE);
		DirectorImpl::Instance()->ActiveUnitAdd(actor);
	}
	else
	{
		if (actor->WillDie())
		{
			DirectorImpl::Instance()->FastKill(actor);
		}
		else
		{
			actor->EndTurnProcess();
		}
	}
}

void dh_fastkill(DQAction *itemAction, Sequence *seq, DHEXECUTE executeType)
{
	DQActionFastKill	*action = (DQActionFastKill *)itemAction;

	Assert(action);
	if (!action) return;

	UnitActor * actor = action->dead;
	Assert(actor);
	if (!actor) return;

	DirectorImpl::Instance()->FastKill(actor);
	DirectorImpl::Instance()->ActionFinished(seq);
}

void dh_removeVision(DQAction *itemAction, Sequence *seq, DHEXECUTE executeType)
{
	DQActionVision	*action = (DQActionVision *)itemAction;

	Assert(action);
	if (!action) return;

	if(g_tiledMap)
		g_tiledMap->RemoveVisible(action->vision_pos, action->vision_range);

	DirectorImpl::Instance()->ActionFinished(seq);
}

void dh_addVision(DQAction *itemAction, Sequence *seq, DHEXECUTE executeType)
{
	DQActionVision	*action = (DQActionVision *)itemAction;

	Assert(action);
	if (!action) return;

	g_tiledMap->AddVisible(action->vision_pos, action->vision_range);

	DirectorImpl::Instance()->ActionFinished(seq);
}

void dh_setVisibility(DQAction *itemAction, Sequence *seq, DHEXECUTE executeType)
{
	DQActionSetVisibility	*action = (DQActionSetVisibility *)itemAction;

	UnitActor	* actor = action ? action->setvisibility_actor : NULL;
	Assert(actor);
	if (!actor) return;

	actor->SetUnitVisibility(action->visibilityFlag);

	DirectorImpl::Instance()->ActionFinished(seq);
}

void dh_setOwner(DQAction *itemAction, Sequence *seq, DHEXECUTE executeType)
{
	DQActionSetOwner	*action = (DQActionSetOwner *)itemAction;

	UnitActor	* actor = action ? action->setowner_actor : NULL;
	Assert(actor);
	if (!actor) return;

	actor->SetPlayerNum(action->owner);

	DirectorImpl::Instance()->ActionFinished(seq);

}

void dh_setVisionRange(DQAction *itemAction, Sequence *seq, DHEXECUTE executeType)
{
	DQActionSetVisionRange	*action = (DQActionSetVisionRange *)itemAction;

	UnitActor * actor = action ? action->setvisionrange_actor : NULL;
	Assert(actor);
	if (!actor) return;

	actor->SetUnitVisionRange(action->range);

	DirectorImpl::Instance()->ActionFinished(seq);
}

void dh_combatflash(DQAction *itemAction, Sequence *seq, DHEXECUTE executeType)
{
	DQActionCombatFlash	*action = (DQActionCombatFlash *)itemAction;

	SpriteState		*ss = new SpriteState(99);
	EffectActor		*flash = new EffectActor(ss, action->flash_pos);

	Anim *anim = flash->CreateAnim(EFFECTACTION_PLAY);
	if (anim == NULL)
	{
		anim = flash->CreateAnim(EFFECTACTION_FLASH);
		Assert(anim != NULL);
	}

	if (anim)
	{
		Action * actionObj = new Action(EFFECTACTION_FLASH, ACTIONEND_PATHEND);
		actionObj->SetAnim(anim);
		flash->AddAction(actionObj);
		DirectorImpl::Instance()->ActiveEffectAdd(flash);
	}

	DirectorImpl::Instance()->ActionFinished(seq);
}

void dh_copyVision(DQAction *itemAction, Sequence *seq, DHEXECUTE executeType)
{
//	DQActionCopyVision	*action = (DQActionCopyVision *)itemAction;

	g_tiledMap->CopyVision();
	g_radarMap->Update();
	DirectorImpl::Instance()->ActionFinished(seq);
}

void dh_centerMap(DQAction *itemAction, Sequence *seq, DHEXECUTE executeType)
{
	DQActionCenterMap	*action = (DQActionCenterMap *)itemAction;

	if(!g_selected_item->GetIsPathing()) {
		DirectorImpl::Instance()->CenterMap(action->centerMap_pos);
	}

	DirectorImpl::Instance()->ActionFinished(seq);
}

void dh_selectUnit(DQAction *itemAction, Sequence *seq, DHEXECUTE executeType)
{
	DQActionUnitSelection	*action = (DQActionUnitSelection *)itemAction;

	g_selected_item->DirectorUnitSelection(action->flags);

	DirectorImpl::Instance()->ActionFinished(seq);
}

void dh_endTurn(DQAction *itemAction, Sequence *seq, DHEXECUTE executeType)
{
//	DQActionEndTurn	*action = (DQActionEndTurn *)itemAction;

	DirectorImpl::Instance()->ActionFinished(seq);

	g_gevManager->AddEvent(GEV_INSERT_Tail, GEV_EndTurn,
						   GEA_Player, g_selected_item->GetCurPlayer(),
						   GEA_End);
}

void dh_battle(DQAction *itemAction, Sequence *seq, DHEXECUTE executeType)
{
	DQActionBattle	*action = (DQActionBattle *)itemAction;

	if(g_battleViewWindow)
	{
		BattleViewWindow::Cleanup();
	}

	BattleViewWindow::Initialize(seq);

	if(g_battleViewWindow)
	{
		g_battleViewWindow->SetupBattle(action->battle);
		g_c3ui->AddWindow(g_battleViewWindow);
		g_cursorManager->SetCursor(CURSORINDEX_DEFAULT);
	}
}

void dh_playSound(DQAction *itemAction, Sequence *seq, DHEXECUTE executeType)
{
	DQActionPlaySound	*action = (DQActionPlaySound *)itemAction;

	if (!g_soundManager) return;

	g_soundManager->AddSound(SOUNDTYPE_SFX, 0, action->playsound_soundID,
							 action->playsound_pos.x, action->playsound_pos.y);

	DirectorImpl::Instance()->ActionFinished(seq);
}

void dh_playWonderMovie(DQAction *itemAction, Sequence *seq, DHEXECUTE executeType)
{
	DQActionPlayWonderMovie	*action = (DQActionPlayWonderMovie *)itemAction;

	Assert(action);
	if (!action) return;

	sint32		which = action->playwondermovie_which;

	wondermoviewin_Initialize(seq);
	wondermoviewin_DisplayWonderMovie(which);
}

void dh_playVictoryMovie(DQAction *itemAction, Sequence *seq, DHEXECUTE executeType)
{
	DQActionPlayVictoryMovie	*action = (DQActionPlayVictoryMovie *)itemAction;

	Assert(action);
	if (!action) return;

	GAME_OVER		reason = action->playvictorymovie_reason;

	victorymoviewin_Initialize(seq);
	victorymoviewin_DisplayVictoryMovie(reason);
}

void dh_message(DQAction *itemAction, Sequence *seq, DHEXECUTE executeType)
{
	DQActionMessage	*action = (DQActionMessage *)itemAction;

	Assert(action);
	if (!action) return;

	if (g_theMessagePool->IsValid(action->message)) {
		if(action->message.IsAlertBox()) {
			if(!messagewin_IsModalMessageDisplayed()) {
				messagewin_CreateModalMessage(action->message);
			}
		} else {
			if(!action->message.AccessData()->GetMessageWindow()) {
				messagewin_CreateMessage( action->message );
			}
			if(action->message.IsInstantMessage()
			   // JJB added this to prevent instant messages showing
			   // out of turn in hotseat games.
			   // With the existing behaviour they would show immediately
			   // which would often mean that they show on the wrong players
			   // turn.
			   && g_selected_item->GetVisiblePlayer() == action->message.GetOwner()
					) {
				action->message.Show();
			}
		}
	}

	DirectorImpl::Instance()->ActionFinished(seq);
}

void dh_faceoff(DQAction *itemAction, Sequence *seq, DHEXECUTE executeType)
{
	DQActionFaceoff	*action = (DQActionFaceoff *)itemAction;

	Assert(action);
	if (!action) return;

	UnitActor	*theAttacker = action->faceoff_attacker;
	UnitActor	*theAttacked = action->faceoff_attacked;

	bool		attackedIsAttackable = true;

	Assert(theAttacker != NULL);
	if (theAttacker == NULL) return;

	Assert(theAttacked != NULL);
	if (theAttacked == NULL) return;

	Action *AttackedActionObj = NULL;

	Action * AttackerActionObj = new Action(UNITACTION_FACE_OFF, ACTIONEND_INTERRUPT);
	Assert(AttackerActionObj != NULL);
	if (AttackerActionObj == NULL) return;

	AttackerActionObj->SetSequence(NULL);

	if (attackedIsAttackable)
	{
		AttackedActionObj = new Action(UNITACTION_FACE_OFF, ACTIONEND_INTERRUPT);
		Assert(AttackedActionObj != NULL);
		if (AttackedActionObj == NULL) return;

		AttackedActionObj->SetSequence(NULL);
	}

	AttackerActionObj->SetStartMapPoint(action->faceoff_attacker_pos);
	AttackerActionObj->SetEndMapPoint(action->faceoff_attacker_pos);

	if (attackedIsAttackable)
	{
		AttackedActionObj->SetStartMapPoint(action->faceoff_attacked_pos);
		AttackedActionObj->SetEndMapPoint(action->faceoff_attacked_pos);
	}

	Anim	*AttackedAnim = NULL;

	Anim * AttackerAnim = theAttacker->MakeFaceoff();
	if (AttackerAnim == NULL)
	{
		theAttacker->AddIdle(TRUE);
		delete AttackerActionObj;
		delete AttackedActionObj;

		return;
	}
	AttackerActionObj->SetAnim(AttackerAnim);

	if (attackedIsAttackable)
	{
		if (theAttacked->GetLoadType() != LOADTYPE_FULL)
			theAttacked->FullLoad(UNITACTION_IDLE);

		AttackedAnim = theAttacked->MakeFaceoff();

		if (AttackedAnim == NULL)
		{
			theAttacked->AddIdle(TRUE);
			delete AttackedActionObj;
			AttackedActionObj = NULL;
		}
	}

	POINT AttackerPoints, AttackedPoints;

	maputils_MapXY2PixelXY(action->faceoff_attacker_pos.x,
						   action->faceoff_attacker_pos.y,
						   AttackerPoints
	);
	maputils_MapXY2PixelXY(action->faceoff_attacked_pos.x,
						   action->faceoff_attacked_pos.y,
						   AttackedPoints
	);

	AttackerActionObj->SetFacing(spriteutils_DeltaToFacing(AttackedPoints.x - AttackerPoints.x,
														   AttackedPoints.y - AttackerPoints.y));

	if(AttackedAnim != NULL)
	{
		AttackedActionObj->SetAnim(AttackedAnim);
		AttackedActionObj->SetFacing(spriteutils_DeltaToFacing(AttackerPoints.x - AttackedPoints.x,
															   AttackerPoints.y - AttackedPoints.y));
	}

	AttackerActionObj->SetUnitVisionRange(theAttacker->GetUnitVisionRange());
	AttackerActionObj->SetUnitsVisibility(theAttacker->GetUnitVisibility());

	theAttacker->AddAction(AttackerActionObj);

	bool attackedVisible = true;

	if (attackedIsAttackable)
	{
		if(AttackedAnim != NULL)
		{
			AttackedActionObj->SetUnitVisionRange(theAttacked->GetUnitVisionRange());

			AttackedActionObj->SetUnitsVisibility(theAttacker->GetUnitVisibility());
			theAttacked->AddAction(AttackedActionObj);
		}

		attackedVisible = DirectorImpl::Instance()->TileIsVisibleToPlayer(action->faceoff_attacked_pos);

		if (theAttacker->GetPlayerNum() == g_selected_item->GetVisiblePlayer() ||
			theAttacked->GetPlayerNum() == g_selected_item->GetVisiblePlayer())
			attackedVisible = TRUE;

		if(attackedVisible && executeType == DHEXECUTE_NORMAL)
		{
			seq->SetAddedToActiveList(SEQ_ACTOR_SECONDARY, TRUE);
			DirectorImpl::Instance()->ActiveUnitAdd(theAttacked);
		}
		else
		{
			if (theAttacked->WillDie())
			{
				DirectorImpl::Instance()->FastKill(theAttacked);
			}
			else
			{
				theAttacked->EndTurnProcess();
			}
		}
	}

	BOOL attackerVisible = DirectorImpl::Instance()->TileIsVisibleToPlayer(action->faceoff_attacker_pos);

	if (theAttacked->GetPlayerNum() == g_selected_item->GetVisiblePlayer() ||
		theAttacker->GetPlayerNum() == g_selected_item->GetVisiblePlayer()) {
		attackerVisible = TRUE;
		attackedVisible = TRUE;
	}
	if (attackerVisible && attackedVisible && executeType == DHEXECUTE_NORMAL)
	{
		seq->SetAddedToActiveList(SEQ_ACTOR_PRIMARY, TRUE);
		DirectorImpl::Instance()->ActiveUnitAdd(theAttacker);
	}
	else
	{
		if (theAttacker->WillDie())
		{
			DirectorImpl::Instance()->FastKill(theAttacker);
		}
		else
		{
			theAttacker->EndTurnProcess();
		}
	}

	DirectorImpl::Instance()->ActionFinished(seq);
}

void dh_terminateFaceoff(DQAction *itemAction, Sequence *seq, DHEXECUTE executeType)
{
	DQActionTerminateFaceOff	*action = (DQActionTerminateFaceOff *)itemAction;

	Assert(action);
	if (!action) return;

	UnitActor		*facerOffer;

	facerOffer = action->faceroffer;

	if (facerOffer) {

		facerOffer->SetHealthPercent(-1.0);
		facerOffer->SetTempStackSize(0);

		// This is commented out as currently AddFaceoff is not called and thereby ActiveUnitAdd is not called.
		// DirectorImpl::Instance()->ActiveUnitRemove(facerOffer);
	}

	DirectorImpl::Instance()->ActionFinished(seq);
}

void dh_terminateSound(DQAction *itemAction, Sequence *seq, DHEXECUTE executeType)
{
	DQActionTerminateSound	*action = (DQActionTerminateSound *)itemAction;

	Assert(action);
	if (!action) return;

	if (g_soundManager)
		g_soundManager->TerminateLoopingSound(SOUNDTYPE_SFX, action->terminate_sound_unit.m_id);

	DirectorImpl::Instance()->ActionFinished(seq);
}

void dh_speceffect(DQAction *itemAction, Sequence *seq, DHEXECUTE executeType)
{
	DQActionSpecialEffect *action = (DQActionSpecialEffect *)itemAction;

	Assert(action);
	if (!action) {
		DirectorImpl::Instance()->ActionFinished(seq);
		return;
	}

	MapPoint		pos = action->speceffect_pos;
	sint32			soundID = action->speceffect_soundID;
	sint32			spriteID = action->speceffect_spriteID;

	if (!g_tiledMap->GetLocalVision()->IsVisible(pos))
	{
		DirectorImpl::Instance()->ActionFinished(seq);
		return;
	}

	SpriteState		*ss = new SpriteState(spriteID);
	EffectActor		*effectActor = new EffectActor(ss, pos);

	Anim *  anim = effectActor->CreateAnim(EFFECTACTION_PLAY);

	if (anim)
	{
		Action * actionObj = new Action(EFFECTACTION_PLAY, ACTIONEND_PATHEND);
		actionObj->SetAnim(anim);
		effectActor->AddAction(actionObj);
		DirectorImpl::Instance()->ActiveEffectAdd(effectActor);

		if (g_soundManager)
		{
			g_soundManager->AddSound(SOUNDTYPE_SFX, 0, soundID, pos.x, pos.y);
		}
	}

	DirectorImpl::Instance()->ActionFinished(seq);
}

void dh_attackpos(DQAction *itemAction, Sequence *seq, DHEXECUTE executeType)
{
	DQActionAttackPos	*action = (DQActionAttackPos *)itemAction;

	Assert(action);
	if (!action) return;

	UnitActor	*theAttacker = action->attackpos_attacker;

	Assert(theAttacker != NULL);
	if (theAttacker == NULL || theAttacker->GetNeedsToDie()) {
		DirectorImpl::Instance()->ActionFinished(seq);
		return;
	}
	Action *AttackerActionObj = NULL;

	AttackerActionObj = new Action(UNITACTION_ATTACK, ACTIONEND_ANIMEND,
								   theAttacker->GetHoldingCurAnimPos(UNITACTION_ATTACK),
								   theAttacker->GetHoldingCurAnimSpecialDelayProcess(UNITACTION_ATTACK));
	Assert(AttackerActionObj != NULL);
	if (AttackerActionObj == NULL)
	{
		c3errors_ErrorDialog("Director", "Could not allocate attacker action object");
		return;
	}

	AttackerActionObj->SetSequence(seq);
	seq->AddRef();

	AttackerActionObj->SetStartMapPoint(action->attackpos_attacker_pos);
	AttackerActionObj->SetEndMapPoint(action->attackpos_attacker_pos);

	Anim	*AttackerAnim = NULL;

	if (theAttacker->GetLoadType() != LOADTYPE_FULL)
		theAttacker->FullLoad(UNITACTION_ATTACK);

	AttackerAnim = theAttacker->CreateAnim(UNITACTION_ATTACK);

	if (AttackerAnim == NULL)
		AttackerAnim = theAttacker->CreateAnim(UNITACTION_IDLE);

	AttackerActionObj->SetAnim(AttackerAnim);

	POINT AttackerPoints, AttackedPoints;

	maputils_MapXY2PixelXY(action->attackpos_attacker_pos.x,
						   action->attackpos_attacker_pos.y,
						   AttackerPoints
	);
	maputils_MapXY2PixelXY(action->attackpos_target_pos.x,
						   action->attackpos_target_pos.y,
						   AttackedPoints
	);

	AttackerActionObj->SetFacing(spriteutils_DeltaToFacing(AttackedPoints.x - AttackerPoints.x,
														   AttackedPoints.y - AttackerPoints.y));

	AttackerActionObj->SetUnitVisionRange(theAttacker->GetUnitVisionRange());
	AttackerActionObj->SetUnitsVisibility(theAttacker->GetUnitVisibility());

	if (g_soundManager)
		g_soundManager->TerminateLoopingSound(SOUNDTYPE_SFX, (uint32)theAttacker->GetUnitID());

	if (g_soundManager)
	{
		sint32 visiblePlayer = g_selected_item->GetVisiblePlayer();
		if ((visiblePlayer == theAttacker->GetPlayerNum()) ||
			(theAttacker->GetUnitVisibility() & (1 << visiblePlayer)))
		{
			g_soundManager->AddSound(SOUNDTYPE_SFX, (uint32)theAttacker->GetUnitID(),
									 action->attackpos_soundID,
									 theAttacker->GetPos().x, theAttacker->GetPos().y);
		}
	}

	theAttacker->AddAction(AttackerActionObj);

	bool attackerVisible = DirectorImpl::Instance()->TileIsVisibleToPlayer(action->attackpos_attacker_pos);

	if (attackerVisible && executeType == DHEXECUTE_NORMAL)
	{
		seq->SetAddedToActiveList(SEQ_ACTOR_PRIMARY, TRUE);
		DirectorImpl::Instance()->ActiveUnitAdd(theAttacker);
	}
	else
	{
		if (theAttacker->WillDie())
		{
			DirectorImpl::Instance()->FastKill(theAttacker);
		}
		else
		{
			theAttacker->EndTurnProcess();
		}
	}
}

void dh_invokeThroneRoom(DQAction *itemAction, Sequence *seq, DHEXECUTE executeType)
{
	DirectorImpl::Instance()->ActionFinished(seq);
}

void dh_invokeResearchAdvance(DQAction *itemAction, Sequence *seq, DHEXECUTE executeType)
{
	DQActionInvokeResearchAdvance *action = (DQActionInvokeResearchAdvance *)itemAction;

	if (!action)
	{
		DirectorImpl::Instance()->ActionFinished(seq);
		return;
	}

	sci_advancescreen_displayMyWindow(action->message, 0, seq);

	delete action->message;
	action->message = NULL;
}

void dh_beginScheduler(DQAction *itemAction, Sequence *seq, DHEXECUTE executeType)
{
	DQActionBeginScheduler *action = (DQActionBeginScheduler *)itemAction;

	if(!action)
	{
		DirectorImpl::Instance()->ActionFinished(seq);
		return;
	}

#ifdef _DEBUG
	static bool isCurrentPlayerOk = true; // static, to report the error only once
	if (isCurrentPlayerOk)
	{
		isCurrentPlayerOk = action->player == g_selected_item->GetCurPlayer();
		Assert(isCurrentPlayerOk);
	}
#endif

	if(g_network.IsHost())
	{
		g_network.Enqueue(new NetInfo(NET_INFO_CODE_BEGIN_SCHEDULER, action->player));
	}

	Assert(DirectorImpl::Instance()->GetHoldSchedulerSequence() == NULL);
	if(!g_network.IsActive() || g_network.IsLocalPlayer(action->player))
	{
		DirectorImpl::Instance()->SetHoldSchedulerSequence(seq);
	}
	else
	{
		DirectorImpl::Instance()->SetHoldSchedulerSequence(NULL);
	}

	g_gevManager->Pause();
	g_gevManager->AddEvent(GEV_INSERT_Tail, GEV_BeginScheduler,
						   GEA_Player, action->player,
						   GEA_End);
	g_gevManager->Resume();

	if(DirectorImpl::Instance()->GetHoldSchedulerSequence() == NULL) {
		DirectorImpl::Instance()->ActionFinished(seq);
	}
}

DQItem::DQItem(DQITEM_TYPE type, DQAction *action, DQHandler *handler)
:
	m_type              (type),
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
			actor = NULL;
		}
	}
	else
	{
		m_standbyActors.erase(actor);
		delete actor;
		actor = NULL;
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
		switch (walk.GetObj()->m_type) {
			case DQITEM_MOVE:
			case DQITEM_TELEPORT: {
				UnitActor *actor = ((DQActionMove *) (walk.GetObj()->m_action))->move_actor;
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

		nextTime = GetTickCount() + 75;
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
	switch(item->m_type)
	{
		case DQITEM_MOVE:
		{
			DQActionMove *action = (DQActionMove *)item->m_action;

			DPRINTF(k_DBG_UI, ("Move\n"));
			DPRINTF(k_DBG_UI, ("  move_actor         :%#.8lx\n", action->move_actor));
			DPRINTF(k_DBG_UI, ("  move_oldPos        :%d,%d\n", action->move_oldPos.x, action->move_oldPos.y));
			DPRINTF(k_DBG_UI, ("  move_newPos        :%d,%d\n", action->move_newPos.x, action->move_newPos.y));
			DPRINTF(k_DBG_UI, ("  moveArraySize      :%d\n", action->moveArraySize));
			DPRINTF(k_DBG_UI, ("  moveActors         :"));
			if (action->moveArraySize > 0)
			{
				for (sint32 i=0; i<action->moveArraySize; i++)
				{
					DPRINTF(k_DBG_UI, ("%#.8lx  ", action->moveActors[i]));
				}
			}
			DPRINTF(k_DBG_UI, ("\n"));
			DPRINTF(k_DBG_UI, ("  numRevelaed        :%d\n", action->numRevealed));
			DPRINTF(k_DBG_UI, ("  revealedActors     :"));
			if (action->moveArraySize > 0) {
				for (sint32 i=0; i<action->numRevealed; i++) {
					DPRINTF(k_DBG_UI, ("%#.8lx  ", action->revealedActors[i]));
				}
			}
			DPRINTF(k_DBG_UI, ("\n"));
			DPRINTF(k_DBG_UI, ("  move_soundID       :%d\n", action->move_soundID));
		}
			break;
		case DQITEM_MOVEPROJECTILE:
		{
			DQActionMoveProjectile *action = (DQActionMoveProjectile *)item->m_action;

			DPRINTF(k_DBG_UI, ("Move Projectile\n"));
			DPRINTF(k_DBG_UI, ("  pshooting_actor    :%#.8lx\n", action->pshooting_actor));
			DPRINTF(k_DBG_UI, ("  ptarget_actor      :%#.8lx\n", action->ptarget_actor));
			DPRINTF(k_DBG_UI, ("  pmove_oldPos       :%d,%d\n", action->pmove_oldPos.x, action->pmove_oldPos.y));
			DPRINTF(k_DBG_UI, ("  pmove_newPos       :%d,%d\n", action->pmove_newPos.x, action->pmove_newPos.y));
			DPRINTF(k_DBG_UI, ("  end_projectile     :%#.8lx\n", action->end_projectile));
			DPRINTF(k_DBG_UI, ("  projectile_path    :%d\n", action->projectile_path));
		}
			break;
		case DQITEM_SPECEFFECT :
		{
			DQActionSpecialEffect *action = (DQActionSpecialEffect *)item->m_action;

			DPRINTF(k_DBG_UI, ("Special Effect\n"));
			DPRINTF(k_DBG_UI, ("  speceffect_pos       :%d,%d\n", action->speceffect_pos.x, action->speceffect_pos.y));
			DPRINTF(k_DBG_UI, ("  speceffect_spriteID    :%d\n", action->speceffect_spriteID));
			DPRINTF(k_DBG_UI, ("  speceffect_soundID    :%d\n", action->speceffect_soundID));
		}
			break;
		case DQITEM_ATTACK:
		{
			DQActionAttack *action = (DQActionAttack *)item->m_action;

			DPRINTF(k_DBG_UI, ("Attack\n"));
			DPRINTF(k_DBG_UI, ("  attacker			:%#.8lx\n", action->attacker));
			DPRINTF(k_DBG_UI, ("  defender			:%#.8lx\n", action->defender));
			DPRINTF(k_DBG_UI, ("  attacker_Pos      :%d,%d\n", action->attacker_Pos.x, action->attacker_Pos.y));
			DPRINTF(k_DBG_UI, ("  defender_Pos      :%d,%d\n", action->defender_Pos.x, action->defender_Pos.y));
			DPRINTF(k_DBG_UI, ("  attacker_soundID  :%d\n", action->attacker_ID));
			DPRINTF(k_DBG_UI, ("  defender_soundID  :%d\n", action->defender_ID));
		}
			break;
		case DQITEM_ATTACKPOS:
		{
			DQActionAttackPos *action = (DQActionAttackPos *)item->m_action;

			DPRINTF(k_DBG_UI, ("Attack Pos\n"));
			DPRINTF(k_DBG_UI, ("  attackpos_attacker     :%#.8lx\n", action->attackpos_attacker));
			DPRINTF(k_DBG_UI, ("  attackpos_attacker_pos :%d,%d\n", action->attackpos_attacker_pos.x, action->attackpos_attacker_pos.y));
			DPRINTF(k_DBG_UI, ("  attackpos_target_pos   :%d,%d\n", action->attackpos_target_pos.x, action->attackpos_target_pos.y));
			DPRINTF(k_DBG_UI, ("  attackpos_soundID      :%d\n", action->attackpos_soundID));
		}
			break;
		case DQITEM_SPECATTACK:
		{
			DQActionAttack *action = (DQActionAttack *)item->m_action;

			DPRINTF(k_DBG_UI, ("Special Attack\n"));
			DPRINTF(k_DBG_UI, ("  sa_attacker        ::%#.8lx\n"  , action->attacker));
			DPRINTF(k_DBG_UI, ("  sa_attacked        ::%#.8lx\n"  , action->defender));
			DPRINTF(k_DBG_UI, ("  sa_attacker_pos    ::%d,%d\n", action->attacker_Pos.x, action->attacker_Pos.y));
			DPRINTF(k_DBG_UI, ("  sa_attacked_pos    ::%d,%d\n", action->defender_Pos.x, action->defender_Pos.y));
			DPRINTF(k_DBG_UI, ("  sa_soundID         ::%d\n",       action->attacker_ID));
			DPRINTF(k_DBG_UI, ("  sa_spriteID        ::%d\n",       action->defender_ID));
		}
			break;
		case DQITEM_DEATH:
		{
			DQActionDeath *action = (DQActionDeath *)item->m_action;

			DPRINTF(k_DBG_UI, ("Death\n"));
			DPRINTF(k_DBG_UI, ("  dead               :%#.8lx\n", action->dead));
			DPRINTF(k_DBG_UI, ("  dead_Pos           :%d,%d\n", action->dead_pos.x, action->dead_pos.y));
			DPRINTF(k_DBG_UI, ("  dead_soundID       :%d\n", action->dead_soundID));
		}
			break;
		case DQITEM_MORPH:
		{
			DQActionMorph *action = (DQActionMorph *)item->m_action;

			DPRINTF(k_DBG_UI, ("Morph\n"));
			DPRINTF(k_DBG_UI, ("  morphing_actor     :%#.8lx\n", action->morphing_actor));
			DPRINTF(k_DBG_UI, ("  ss                 :%#.8lx (%d)\n", action->ss, action->ss->GetIndex()));
			DPRINTF(k_DBG_UI, ("  type               :%d\n", action->type));
			DPRINTF(k_DBG_UI, ("  id                 :%#.8lx\n", action->id.m_id));
		}
			break;
		case DQITEM_HIDE:
		{
			DQActionHideShow *action = (DQActionHideShow *)item->m_action;

			DPRINTF(k_DBG_UI, ("Hide\n"));
			DPRINTF(k_DBG_UI, ("  hiding_actor       :%#.8lx\n", action->hiding_actor));
			DPRINTF(k_DBG_UI, ("  hiding_pos         :%d,%d\n", action->hiding_pos.x, action->hiding_pos.y));
		}
			break;
		case DQITEM_SHOW:
		{
			DQActionHideShow *action = (DQActionHideShow *)item->m_action;

			DPRINTF(k_DBG_UI, ("Show\n"));
			DPRINTF(k_DBG_UI, ("  hiding_actor       :%#.8lx\n", action->hiding_actor));
			DPRINTF(k_DBG_UI, ("  hiding_pos         :%d,%d\n", action->hiding_pos.x, action->hiding_pos.y));
		}
			break;
		case DQITEM_WORK:
		{
			DQActionWork *action = (DQActionWork *)item->m_action;

			DPRINTF(k_DBG_UI, ("Work\n"));
			DPRINTF(k_DBG_UI, ("  working_actor      :%#.8lx\n", action->working_actor));
			DPRINTF(k_DBG_UI, ("  working_pos        :%d,%d\n", action->working_pos.x, action->working_pos.y));
			DPRINTF(k_DBG_UI, ("  working_soundID    :%d\n", action->working_soundID));
		}
			break;
		case DQITEM_FASTKILL:
		{
			DQActionFastKill *action = (DQActionFastKill *)item->m_action;

			DPRINTF(k_DBG_UI, ("Fast Kill\n"));
			DPRINTF(k_DBG_UI, ("  dead               :%#.8lx\n", action->dead));
		}
			break;
		case DQITEM_ADDVISION:
		{
			DQActionVision *action = (DQActionVision *)item->m_action;

			DPRINTF(k_DBG_UI, ("Add Vision\n"));
			DPRINTF(k_DBG_UI, ("  vision_pos         :%d,%d\n", action->vision_pos.x, action->vision_pos.y));
			DPRINTF(k_DBG_UI, ("  vision_range       :%#.2f\n", action->vision_range));
		}
			break;
		case DQITEM_REMOVEVISION:
		{
			DQActionVision *action = (DQActionVision *)item->m_action;

			DPRINTF(k_DBG_UI, ("Remove Vision\n"));
			DPRINTF(k_DBG_UI, ("  vision_pos         :%d,%d\n", action->vision_pos.x, action->vision_pos.y));
			DPRINTF(k_DBG_UI, ("  vision_range       :%#.2f\n", action->vision_range));
		}
			break;
		case DQITEM_SETOWNER:
		{
			DQActionSetOwner *action = (DQActionSetOwner *)item->m_action;

			DPRINTF(k_DBG_UI, ("Set Owner\n"));
			DPRINTF(k_DBG_UI, ("  setowner_actor     :%#.8lx\n", action->setowner_actor));
			DPRINTF(k_DBG_UI, ("  owner              :%d\n", action->owner));
		}
			break;
		case DQITEM_SETVISIBILITY: {
			DQActionSetVisibility *action = (DQActionSetVisibility *)item->m_action;

			DPRINTF(k_DBG_UI, ("Set Visibility\n"));
			DPRINTF(k_DBG_UI, ("  setowner_actor     :%#.8lx\n", action->setvisibility_actor));
			DPRINTF(k_DBG_UI, ("  owner              :%#.8lx\n", action->visibilityFlag));
		}
			break;
		case DQITEM_SETVISIONRANGE: {
			DQActionSetVisionRange *action = (DQActionSetVisionRange *)item->m_action;

			DPRINTF(k_DBG_UI, ("Set Vision Range\n"));
			DPRINTF(k_DBG_UI, ("  setvisibility_actor:%#.8lx\n", action->setvisionrange_actor));
			DPRINTF(k_DBG_UI, ("  visibilityFlag     :%#.2f\n", action->range));
		}
			break;
		case DQITEM_COMBATFLASH: {
			DQActionCombatFlash *action = (DQActionCombatFlash *)item->m_action;

			DPRINTF(k_DBG_UI, ("Combat Flash\n"));
			DPRINTF(k_DBG_UI, ("  flash_pos         :%d,%d\n", action->flash_pos.x, action->flash_pos.y));
		}
			break;
		case DQITEM_TELEPORT: {

			DQActionMove *action = (DQActionMove *)item->m_action;

			DPRINTF(k_DBG_UI, ("Move\n"));
			DPRINTF(k_DBG_UI, ("  teleport_actor         :%#.8lx\n", action->move_actor));
			DPRINTF(k_DBG_UI, ("  teleport_oldPos        :%d,%d\n", action->move_oldPos.x, action->move_oldPos.y));
			DPRINTF(k_DBG_UI, ("  teleport_newPos        :%d,%d\n", action->move_newPos.x, action->move_newPos.y));
			DPRINTF(k_DBG_UI, ("  teleport_moveArraySize      :%d\n", action->moveArraySize));
			DPRINTF(k_DBG_UI, ("  teleport_moveActors         :"));
			if (action->moveArraySize > 0)
			{
				for (sint32 i=0; i<action->moveArraySize; i++) {
					DPRINTF(k_DBG_UI, ("%#.8lx  ", action->moveActors[i]));
				}
			}
			DPRINTF(k_DBG_UI, ("\n"));
			DPRINTF(k_DBG_UI, ("  teleport_numRevelead        :%d\n", action->numRevealed));
			DPRINTF(k_DBG_UI, ("  teleport_revealedActors     :"));

			if (action->moveArraySize > 0)
			{
				for (sint32 i=0; i<action->numRevealed; i++) {
					DPRINTF(k_DBG_UI, ("%#.8lx  ", action->revealedActors[i]));
				}
			}
			DPRINTF(k_DBG_UI, ("\n"));
		}




			break;
		case DQITEM_COPYVISION: {
//		DQActionCopyVision *action = (DQActionCopyVision *)item->m_action;

			DPRINTF(k_DBG_UI, ("Copy Vision from Gamestate\n"));
		}
			break;
		case DQITEM_CENTERMAP : {
			DQActionCenterMap *action = (DQActionCenterMap *)item->m_action;

			DPRINTF(k_DBG_UI, ("Center Map\n"));
			DPRINTF(k_DBG_UI, ("  centerMap_pos    :%d,%d\n", action->centerMap_pos.x, action->centerMap_pos.y));
		}
			break;
		case DQITEM_SELECTUNIT : {
			DQActionUnitSelection *action = (DQActionUnitSelection *)item->m_action;

			DPRINTF(k_DBG_UI, ("Select Unit\n"));
			DPRINTF(k_DBG_UI, ("  flags     :%#.8lx\n", action->flags));
		}
			break;
		case DQITEM_ENDTURN : {
//		DQActionEndTurn *action = (DQActionEndTurn *)item->m_action;

			DPRINTF(k_DBG_UI, ("End Turn from Gamestate\n"));
		}
			break;
		case DQITEM_BATTLE : {
//		DQActionBattle *action = (DQActionBattle *)item->m_action;

			DPRINTF(k_DBG_UI, ("Battle\n"));
		}
			break;
		case DQITEM_PLAYSOUND : {
			DQActionPlaySound *action = (DQActionPlaySound *)item->m_action;

			DPRINTF(k_DBG_UI, ("Play Sound\n"));
			DPRINTF(k_DBG_UI, ("  playsound_soundID    :%d\n", action->playsound_soundID));
			DPRINTF(k_DBG_UI, ("  playsound_pos        :%d,%d\n", action->playsound_pos.x, action->playsound_pos.y));
		}
			break;
		case DQITEM_PLAYWONDERMOVIE : {
			DQActionPlayWonderMovie *action = (DQActionPlayWonderMovie *)item->m_action;

			DPRINTF(k_DBG_UI, ("Play Wonder Movie\n"));
			DPRINTF(k_DBG_UI, ("  which                :%ld\n", action->playwondermovie_which));
		}
			break;
		case DQITEM_PLAYVICTORYMOVIE : {
			DQActionPlayVictoryMovie *action = (DQActionPlayVictoryMovie *)item->m_action;

			DPRINTF(k_DBG_UI, ("Play Victory Movie\n"));
			DPRINTF(k_DBG_UI, ("  reason                :%ld\n", action->playvictorymovie_reason));
		}
			break;
		case DQITEM_MESSAGE : {
//		DQActionMessage *action = (DQActionMessage *)item->m_action;

			DPRINTF(k_DBG_UI, ("Message from Gamestate\n"));
		}
			break;
		case DQITEM_FACEOFF : {
			DQActionFaceoff *action = (DQActionFaceoff *)item->m_action;

			DPRINTF(k_DBG_UI, ("Faceoff\n"));
			DPRINTF(k_DBG_UI, ("  faceoff_attacker   :%#.8lx\n", action->faceoff_attacker));
			DPRINTF(k_DBG_UI, ("  faceoff_attacked   :%#.8lx\n", action->faceoff_attacked));
			DPRINTF(k_DBG_UI, ("  faceoff_attacker_pos :%d,%d\n", action->faceoff_attacker_pos.x, action->faceoff_attacker_pos.y));
			DPRINTF(k_DBG_UI, ("  faceoff_attacked_pos :%d,%d\n", action->faceoff_attacked_pos.x, action->faceoff_attacked_pos.y));
		}
			break;
		case DQITEM_TERMINATE_FACEOFF : {
			DQActionTerminateFaceOff *action = (DQActionTerminateFaceOff *)item->m_action;

			DPRINTF(k_DBG_UI, ("Terminate Faceoff\n"));
			DPRINTF(k_DBG_UI, ("  faceroffer   :%#.8lx\n", action->faceroffer));
		}
			break;
		case DQITEM_TERMINATE_SOUND : {
			DQActionTerminateSound *action = (DQActionTerminateSound *)item->m_action;

			DPRINTF(k_DBG_UI, ("Terminate Sound\n"));
			DPRINTF(k_DBG_UI, ("  terminate_sound_unit    :%#.8lx\n", action->terminate_sound_unit.m_id));
			break;
		}
		case DQITEM_BEGIN_SCHEDULER:
		{
			DQActionBeginScheduler *action = (DQActionBeginScheduler*)item->m_action;
			DPRINTF(k_DBG_UI, ("Begin Scheduler\n"));
			DPRINTF(k_DBG_UI, ("  player: %d\n", action->player));
			break;
		}
	}
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

void DirectorImpl::HandleNextAction(void)
{
	if (m_paused) return;

	while (CanStartNextAction())
	{
		DQItem *    item        = m_itemQueue->RemoveHead();

		Assert(item->m_handler != NULL);
		if (item->m_handler != NULL)
		{
			SetActionFinished(FALSE);

			m_dispatchedItems->AddTail(item);

			DHEXECUTE		executeType = DHEXECUTE_NORMAL;

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
	DQItem			*item;

	if (!seq) {
		if (m_dispatchedItems->GetCount() > 0) {
			item = m_dispatchedItems->RemoveHead();
			Assert(item->m_sequence == NULL);

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

	BOOL	removePrimaryFromActiveList = FALSE;
	BOOL	removeSecondaryFromActiveList = FALSE;

	if (seq) {
		removePrimaryFromActiveList = seq->GetAddedToActiveList(SEQ_ACTOR_PRIMARY);
		removeSecondaryFromActiveList = seq->GetAddedToActiveList(SEQ_ACTOR_SECONDARY);
	}

	switch (item->m_type)
	{
		default:
//  case DQITEM_ADDVISION:
//  case DQITEM_ATTACK:
//  case DQITEM_BATTLE:
//  case DQITEM_CENTERMAP:
//  case DQITEM_COMBATFLASH:
//  case DQITEM_COPYVISION:
//  case DQITEM_ENDTURN:
//  case DQITEM_FACEOFF:
//  case DQITEM_FASTKILL:
//  case DQITEM_HIDE:
//  case DQITEM_MESSAGE:
//  case DQITEM_MORPH:
//  case DQITEM_MOVEPROJECTILE:
//  case DQITEM_PLAYSOUND:
//  case DQITEM_PLAYVICTORYMOVIE:
//  case DQITEM_PLAYWONDERMOVIE:
//  case DQITEM_REMOVEVISION:
//  case DQITEM_SPECEFFECT:
//  case DQITEM_SELECTUNIT:
//  case DQITEM_SETOWNER:
//  case DQITEM_SETVISIBILITY:
//  case DQITEM_SETVISIONRANGE:
//  case DQITEM_SHOW:
//  case DQITEM_TELEPORT:
//  case DQITEM_TERMINATE_FACEOFF:
			// Does not contain actions that have to be removed
			break;

		case DQITEM_MOVE: {
			DQActionMove	*action = (DQActionMove *)item->m_action;
			if (removePrimaryFromActiveList)
				if (action && action->move_actor)
				{
					ActiveUnitRemove(action->move_actor);
					if (!action->move_actor->IsActive())
						g_soundManager->TerminateLoopingSound(SOUNDTYPE_SFX, action->move_actor->GetUnitID());
				}
		}
			break;
		case DQITEM_ATTACKPOS:
		{
			DQActionAttackPos	*action = (DQActionAttackPos *)item->m_action;
			if (removePrimaryFromActiveList)
				if (action && action->attackpos_attacker)
				{
					ActiveUnitRemove(action->attackpos_attacker);
					if (!action->attackpos_attacker->IsActive())
						g_soundManager->TerminateLoopingSound(SOUNDTYPE_SFX, action->attackpos_attacker->GetUnitID());
				}
		}
			break;
		case DQITEM_SPECATTACK:
		{
			DQActionAttack	*action = (DQActionAttack *)item->m_action;
			if (removePrimaryFromActiveList) {
				if (action && action->attacker)
				{
					ActiveUnitRemove(action->attacker);
					if (!action->attacker->IsActive())
						g_soundManager->TerminateLoopingSound(SOUNDTYPE_SFX, action->attacker->GetUnitID());
				}
			}
			if (removeSecondaryFromActiveList)
			{
				if (action && action->defender)
				{
					ActiveUnitRemove(action->defender);
					if (!action->defender->IsActive())
						g_soundManager->TerminateLoopingSound(SOUNDTYPE_SFX, action->defender->GetUnitID());
				}
			}
		}
			break;
		case DQITEM_DEATH: {
			DQActionDeath	*action = (DQActionDeath *)item->m_action;
			if (removePrimaryFromActiveList) {
				if (action && action->dead) {
					ActiveUnitRemove(action->dead);
					if (!action->dead->IsActive())
						g_soundManager->TerminateLoopingSound(SOUNDTYPE_SFX, action->dead->GetUnitID());
				}
			}
		}
			break;
		case DQITEM_WORK: {
			DQActionWork	*action = (DQActionWork *)item->m_action;
			if (removePrimaryFromActiveList)
				if (action && action->working_actor) {
					ActiveUnitRemove(action->working_actor);
					if (!action->working_actor->IsActive())
						g_soundManager->TerminateLoopingSound(SOUNDTYPE_SFX, action->working_actor->GetUnitID());
				}
		}
			break;
	}

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

	ListPos     actorPos;
	UnitActor * actor;

	ListPos     pos = m_activeUnitList->GetHeadPosition();

	if (pos) {
		do {
			actorPos = pos;
			actor = m_activeUnitList->GetNext(pos);

			if (actor) {
				if (actor->WillDie()) {
					m_standbyActors.erase(actor);
					m_activeUnitList->DeleteAt(actorPos);
					delete actor;
				} else {
					actor->EndTurnProcess();
				}
			}
		} while (pos);
	}

	while(m_itemQueue->GetCount())
	{
		DQItem *    item    = m_itemQueue->RemoveHead();

		Assert(item->m_handler != NULL);
		if (item->m_handler != NULL)
		{
			m_dispatchedItems->AddTail(item);

			item->m_handler(item->m_action, item->m_sequence, DHEXECUTE_IMMEDIATE);
		}
	}

	pos = m_activeUnitList->GetHeadPosition();
	if (pos) {
		do {
			actorPos = pos;

			actor = m_activeUnitList->GetNext(pos);

			if (actor) {
				if (actor->WillDie()) {
					m_standbyActors.erase(actor);
					m_activeUnitList->DeleteAt(actorPos);
					delete actor;
				} else {
					actor->EndTurnProcess();
				}
			}
		} while (pos);
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
	if(unitActor==NULL)
		return;

	if(!unitActor->IsActive())
		m_activeUnitList->AddHead(unitActor);

	unitActor->SetActive(true);

	unitActor->AddActiveListRef();
}

void DirectorImpl::ActiveUnitRemove(UnitActor *unitActor)
{
	if(unitActor==NULL)
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

		Assert(tActor != NULL);
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
		Assert(actor != NULL);
		if (actor != NULL)
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
		if (actor) {
			if (actor->GetKillNow())
			{
				actor->DumpAllActions();
				m_standbyActors.erase(actor);
				m_activeUnitList->DeleteAt(actorPos);
				delete actor;
			}
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

void DirectorImpl::NextPlayer()
{
#ifdef _PLAYTEST
	if (!g_doingFastRounds &&
		(!g_network.IsActive() || g_player[g_selected_item->GetVisiblePlayer()]->IsRobot())) {
		return;
	}
#else
	return; // Next code isn't used, should it be used?
#endif
	m_nextPlayer = TRUE;

	UnitActor * actor;
	ListPos     actorPos;
	ListPos     pos = m_activeUnitList->GetHeadPosition();

	if(pos)
	{
		do
		{
			actorPos = pos;
			actor    = m_activeUnitList->GetNext(pos);

			if (actor && !g_network.IsActive())
			{
				if (actor->WillDie())
				{
					m_standbyActors.erase(actor);
					m_activeUnitList->DeleteAt(actorPos);
					delete actor;
				}
				else
				{
					actor->EndTurnProcess();
				}
			}
		} while (pos);
	}

	while(m_itemQueue->GetCount())
	{
		DQItem *    item    = m_itemQueue->RemoveHead();

		Assert(item->m_handler != NULL);
		if (item->m_handler != NULL)
		{
			m_dispatchedItems->AddTail(item);

			item->m_handler(item->m_action, item->m_sequence, DHEXECUTE_NORMAL);
		}
	}

	pos = m_activeUnitList->GetHeadPosition();
	if(pos)
	{
		do
		{
			actorPos = pos;

			actor = m_activeUnitList->GetNext(pos);

			if(!g_network.IsActive())
			{
				if(actor->WillDie())
				{
					m_standbyActors.erase(actor);
					m_activeUnitList->DeleteAt(actorPos);
					delete actor;
				}
				else
				{
					actor->EndTurnProcess();
				}
			}
		} while (pos);
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
	UnitActor		*actor = mover.GetActor();

	Assert(!mover.IsCity());

	Assert(actor->GetUnitID() == mover.m_id);

	if(g_theProfileDB->IsEnemyMoves() &&
	   mover.GetOwner() != g_selected_item->GetVisiblePlayer() &&
	   (mover.GetVisibility() & (1 << g_selected_item->GetVisiblePlayer())) &&
	   !TileWillBeCompletelyVisible(newPos.x, newPos.y)) {
		AddCenterMap(newPos);
	}

	actor->SetHiddenUnderStack(FALSE);

	DQActionMove		*action = new DQActionMove;
	DQItem				*item = new DQItem(DQITEM_MOVE, action, dh_move);

	item->SetOwner(mover.GetOwner());

	action->move_actor    = actor;
	action->move_oldPos   = oldPos;
	action->move_newPos   = newPos;
	action->move_soundID  = soundID;
	action->moveArraySize = numRest;

	if(numRest <= 0)
		action->moveActors = NULL;
	else
		action->moveActors = restOfStack;

	action->numRevealed = numRevealed;
	action->revealedActors = revealedActors;

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
	DQActionMove		*action = new DQActionMove;
	DQItem				*item	= new DQItem(DQITEM_TELEPORT, action, dh_teleport);
	item->SetOwner(top.GetOwner());

	action->move_actor     = top.GetActor();
	action->move_oldPos    = oldPos;
	action->move_newPos    = newPos;
	action->moveArraySize  = arraySize;
	action->moveActors     = moveActors;
	action->numRevealed    = numRevealed;
	action->revealedActors = revealedActors;

	m_itemQueue->AddTail(item);
}

void DirectorImpl::AddProjectileAttack(Unit shooting, Unit target, SpriteState *projectile_state,
									   SpriteState *projectileEnd_state, sint32 projectile_Path)
{
	DQActionMoveProjectile	*action = new DQActionMoveProjectile;
	DQItem					*item = new DQItem(DQITEM_MOVEPROJECTILE, action, dh_projectileMove);
	item->SetOwner(shooting.GetOwner());

	EffectActor *projectileEnd = new EffectActor(projectileEnd_state, target.RetPos());

	action->pshooting_actor = shooting.GetActor();
	action->ptarget_actor = target.GetActor();
	action->end_projectile = projectileEnd;
	action->projectile_path = projectile_Path;
	action->pmove_oldPos = shooting.RetPos();
	action->pmove_newPos = target.RetPos();

	m_itemQueue->AddTail(item);
}

void DirectorImpl::AddSpecialEffect(MapPoint &pos, sint32 spriteID, sint32 soundID)
{
	DQActionSpecialEffect	*action = new DQActionSpecialEffect;
	DQItem					*item = new DQItem(DQITEM_SPECEFFECT, action, dh_speceffect);

	action->speceffect_pos = pos;
	action->speceffect_spriteID = spriteID;
	action->speceffect_soundID = soundID;

	m_itemQueue->AddTail(item);
}

void DirectorImpl::AddCombatFlash(MapPoint const & pos)
{
	DQActionCombatFlash	*   action  = new DQActionCombatFlash;
	action->flash_pos = pos;

	DQItem *                item    =
		new DQItem(DQITEM_COMBATFLASH, action, dh_combatflash);
	m_itemQueue->AddTail(item);
}

void DirectorImpl::AddCopyVision(void)
{
	DQActionCopyVision	*action = new DQActionCopyVision;
	DQItem				*item = new DQItem(DQITEM_COPYVISION, action, dh_copyVision);

	action->copyVision = TRUE;

	m_itemQueue->AddTail(item);
}

void DirectorImpl::AddCenterMap(const MapPoint &pos)
{
	DQActionCenterMap	*action = new DQActionCenterMap;
	DQItem				*item = new DQItem(DQITEM_CENTERMAP, action, dh_centerMap);

	action->centerMap_pos = pos;

	m_itemQueue->AddTail(item);
}

void DirectorImpl::AddSelectUnit(uint32 flags)
{
	DQActionUnitSelection	*action = new DQActionUnitSelection;
	DQItem				*item = new DQItem(DQITEM_SELECTUNIT, action, dh_selectUnit);

	action->flags = flags;

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
			if(walk.GetObj()->m_type == DQITEM_ENDTURN) {
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

	DQActionEndTurn		*action = new DQActionEndTurn;
	DQItem				*item = new DQItem(DQITEM_ENDTURN, action, dh_endTurn);

	action->endTurn = TRUE;

	m_itemQueue->AddTail(item);
}

void DirectorImpl::AddAttack(Unit attacker, Unit defender)
{
	UnitActor	*attackerActor = NULL;
	UnitActor	*defenderActor = NULL;

	DQActionAttack		*action = new DQActionAttack;
	DQItem				*item = new DQItem(DQITEM_ATTACK, action, dh_attack);
	item->SetOwner(attacker.GetOwner());

	action->defender_IsCity = FALSE;

	if (attacker != Unit())
	{
		attackerActor = attacker.GetActor();
		action->attacker = attackerActor;
		action->attacker_Pos = attacker.RetPos();
		action->attacker_ID		= attacker.GetAttackSoundID();
		action->attacker_IsCity = attacker.IsCity();

		attackerActor->SetHealthPercent(attacker.GetHP() / attacker->CalculateTotalHP());

		Army army = attacker.GetArmy();
		if (army.IsValid())
		{
			attackerActor->SetTempStackSize(army.Num());
		}
	}

	if (defender != Unit())
	{
		defenderActor			= defender.GetActor();
		action->defender		= defenderActor;
		action->defender_Pos    = defender.RetPos();
		action->defender_ID		= defender.GetAttackSoundID();
		action->defender_IsCity = defender.IsCity();

		defenderActor->SetHealthPercent(defender.GetHP() / defender->CalculateTotalHP());

		CellUnitList *unitList;
		unitList = g_theWorld->GetCell(defender.RetPos())->UnitArmy();
		sint32 num = 1;
		if (unitList != NULL)
			num = unitList->Num();
		defenderActor->SetTempStackSize(num);
	}

	m_itemQueue->AddTail(item);

	Player *    visiblePlayer   = g_player[g_selected_item->GetVisiblePlayer()];
	if (visiblePlayer && visiblePlayer->IsVisible(attacker.RetPos()))
	{
		if (attacker.m_id != 0)
		{
			AddCombatFlash(attacker.RetPos());
		}
	}

	if (visiblePlayer && visiblePlayer->IsVisible(defender.RetPos()))
	{
		if (defender.m_id != 0) {
			AddCombatFlash(defender.RetPos());
		}
	}
}

void DirectorImpl::AddAttackPos(Unit attacker, MapPoint const & pos)
{
	DQActionAttackPos *     action  = new DQActionAttackPos;
	action->attackpos_attacker      = attacker.GetActor();
	action->attackpos_attacker_pos  = attacker.RetPos();
	action->attackpos_target_pos    = pos;
	action->attackpos_soundID       = attacker.GetAttackSoundID();

	DQItem *                item    =
		new DQItem(DQITEM_ATTACKPOS, action, dh_attackpos);
	item->SetOwner(attacker.GetOwner());
	m_itemQueue->AddTail(item);

	if (g_player[g_selected_item->GetVisiblePlayer()] &&
		g_player[g_selected_item->GetVisiblePlayer()]->IsVisible(pos)
		)
	{
		AddCombatFlash(pos);
	}
}

void DirectorImpl::AddSpecialAttack(Unit attacker, Unit attacked, SPECATTACK attack)
{
	sint32              soundID;
	sint32              spriteID;
	if (!attacker.GetSpecialAttackInfo(attack, &soundID, &spriteID))
	{
		return;
	}

	UnitActor *         attackerActor   = attacker.IsValid() ? attacker.GetActor() : NULL;
	UnitActor *         defenderActor   = attacked.IsValid() ? attacked.GetActor() : NULL;

	DQActionAttack	*   action          = new DQActionAttack;
	action->attacker        = attackerActor;
	action->defender        = defenderActor;
	action->attacker_Pos    = attacker.RetPos();
	action->defender_Pos    = attacked.RetPos();
	action->attacker_IsCity = attacker.IsCity();
	action->defender_IsCity = attacked.IsCity();
	action->attacker_ID     = soundID;
	action->defender_ID     = spriteID;

	DQItem *            item            = new DQItem(DQITEM_SPECATTACK, action, dh_specialAttack);
	item->SetOwner(attacker.GetOwner());

	m_itemQueue->AddTail(item);

	if (g_player[g_selected_item->GetVisiblePlayer()] &&
		g_player[g_selected_item->GetVisiblePlayer()]->IsVisible(attacked.RetPos())
		)
	{
		AddProjectileAttack(attacker, attacked, NULL, new SpriteState(spriteID), 0);
	}
}

void DirectorImpl::AddDeath(UnitActor *dead, const MapPoint &deadPos, sint32 deadSoundID)
{
	Assert(dead);

	DQActionDeath	*action = new DQActionDeath;
	DQItem			*item = new DQItem(DQITEM_DEATH, action, dh_death);
	item->SetOwner(dead->GetPlayerNum());

	action->dead = dead;
	action->dead_pos = deadPos;
	action->dead_soundID = deadSoundID;

	m_itemQueue->AddTail(item);
}

void DirectorImpl::AddMorphUnit(UnitActor *morphingActor, SpriteState *ss, sint32 type, Unit id)
{
	if (morphingActor == NULL) return;

	DQActionMorph	*action = new DQActionMorph;
	DQItem			*item = new DQItem(DQITEM_MORPH, action, dh_morphUnit);

	action->morphing_actor = morphingActor;
	action->ss = ss;
	action->type = type;
	action->id = id;

	m_itemQueue->AddTail(item);
}

void DirectorImpl::AddHide(Unit hider)
{
	UnitActor	*actor = hider.GetActor();
	Assert(actor);
	if (!actor) return;

	DQActionHideShow	*action = new DQActionHideShow;
	DQItem				*item = new DQItem(DQITEM_HIDE, action, dh_hide);

	action->hiding_actor = actor;
	action->hiding_pos = hider.RetPos();

	m_itemQueue->AddTail(item);
}

void DirectorImpl::AddShow(Unit hider)
{
	UnitActor	*actor = hider.GetActor();
	Assert(actor);
	if (!actor) return;

	DQActionHideShow	*action = new DQActionHideShow;
	DQItem				*item = new DQItem(DQITEM_SHOW, action, dh_show);

	action->hiding_actor = actor;
	action->hiding_pos = hider.RetPos();

	m_itemQueue->AddTail(item);
}

void DirectorImpl::AddWork(Unit worker)
{
	UnitActor	*actor = worker.GetActor();
	Assert(actor);
	if (!actor) return;

	DQActionWork	*action = new DQActionWork;
	DQItem			*item   = new DQItem(DQITEM_WORK, action, dh_work);
	item->SetOwner(worker.GetOwner());

	action->working_actor = actor;
	action->working_pos = worker.RetPos();
	action->working_soundID = worker.GetWorkSoundID();

	m_itemQueue->AddTail(item);
}

void DirectorImpl::AddFastKill(UnitActor *dead)
{
	Assert(dead);
	if (!dead) return;

	DQActionFastKill	*action = new DQActionFastKill;
	DQItem				*item = new DQItem(DQITEM_FASTKILL, action, dh_fastkill);
	action->dead = dead;

	m_itemQueue->AddTail(item);
}

void DirectorImpl::AddRemoveVision(const MapPoint &pos, double range)
{
	DQActionVision		*action = new DQActionVision;
	DQItem				*item = new DQItem(DQITEM_REMOVEVISION, action, dh_removeVision);

	action->vision_pos = pos;
	action->vision_range = range;

	m_itemQueue->AddTail(item);
}

void DirectorImpl::AddAddVision(const MapPoint &pos, double range)
{
	DQActionVision *    action  = new DQActionVision;
	action->vision_pos          = pos;
	action->vision_range        = range;

	m_itemQueue->AddTail(new DQItem(DQITEM_ADDVISION, action, dh_addVision));
}

void DirectorImpl::AddSetVisibility(UnitActor *actor, uint32 visibility)
{
	Assert(actor);
	if (!actor) return;

	DQActionSetVisibility * action  = new DQActionSetVisibility;
	action->setvisibility_actor     = actor;
	action->visibilityFlag          = visibility;

	m_itemQueue->AddTail(new DQItem(DQITEM_SETVISIBILITY, action, dh_setVisibility));
}

void DirectorImpl::AddSetOwner(UnitActor *actor, sint32 owner)
{
	Assert(actor);
	if (!actor) return;

	DQActionSetOwner *  action  = new DQActionSetOwner;
	action->setowner_actor      = actor;
	action->owner               = owner;

	m_itemQueue->AddTail(new DQItem(DQITEM_SETOWNER, action, dh_setOwner));
}

void DirectorImpl::AddSetVisionRange(UnitActor *actor, double range)
{
	Assert(actor);
	if (!actor) return;

	DQActionSetVisionRange *    action  = new DQActionSetVisionRange;
	action->setvisionrange_actor        = actor;
	action->range                       = range;

	m_itemQueue->AddTail(new DQItem(DQITEM_SETVISIONRANGE, action, dh_setVisionRange));
}

void DirectorImpl::AddBattle(Battle *battle)
{
	Assert(battle);
	if (!battle) return;

	DQActionBattle *    action  = new DQActionBattle;
	action->battle              = battle;

	m_itemQueue->AddTail(new DQItem(DQITEM_BATTLE, action, dh_battle));
}

void DirectorImpl::AddPlaySound(sint32 soundID, MapPoint const & pos)
{
	if (soundID <= 0) return;

	DQActionPlaySound * action  = new DQActionPlaySound;
	action->playsound_soundID   = soundID;
	action->playsound_pos       = pos;

	m_itemQueue->AddTail(new DQItem(DQITEM_PLAYSOUND, action, dh_playSound));
}

void DirectorImpl::AddGameSound(GAMESOUNDS sound)
{
	AddPlaySound(gamesounds_GetGameSoundID((sint32) sound), MapPoint());
}

void DirectorImpl::AddPlayWonderMovie(sint32 which)
{
	Assert(which >= 0);
	if (which < 0) return;

	DQActionPlayWonderMovie	*   action  = new DQActionPlayWonderMovie;
	action->playwondermovie_which       = which;

	m_itemQueue->AddTail(new DQItem(DQITEM_PLAYWONDERMOVIE, action, dh_playWonderMovie));
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

	DQActionPlayVictoryMovie *  action  = new DQActionPlayVictoryMovie;
	action->playvictorymovie_reason     = reason;

	m_itemQueue->AddTail(new DQItem(DQITEM_PLAYVICTORYMOVIE, action, dh_playVictoryMovie));
}

void DirectorImpl::AddMessage(const Message & message)
{
	DQActionMessage	*   action  = new DQActionMessage;
	action->message             = message;

	m_itemQueue->AddTail(new DQItem(DQITEM_MESSAGE, action, dh_message));
}

void DirectorImpl::AddFaceoff(Unit &attacker, Unit &defender)
{
	DQActionFaceoff	*   action  = new DQActionFaceoff;

	if (attacker.IsValid())
	{
		action->faceoff_attacker        = attacker.GetActor();
		action->faceoff_attacker_pos    = attacker.RetPos();
	}

	if (defender.IsValid())
	{
		action->faceoff_attacked        = defender.GetActor();
		action->faceoff_attacked_pos    = defender.RetPos();
	}

	m_itemQueue->AddTail(new DQItem(DQITEM_FACEOFF, action, dh_faceoff));
}

void DirectorImpl::AddTerminateFaceoff(Unit &faceroffer)
{
	DQActionTerminateFaceOff	*action = new DQActionTerminateFaceOff;
	DQItem						*item = new DQItem(DQITEM_TERMINATE_FACEOFF, action, dh_terminateFaceoff);

	if (faceroffer.IsValid())
	{
		action->faceroffer = faceroffer.GetActor();
		item->SetOwner(faceroffer.GetOwner());
	}

	m_itemQueue->AddTail(item);
}

void DirectorImpl::AddTerminateSound(Unit &unit)
{
	if (unit.IsValid())
	{
		DQActionTerminateSound *    action  = new DQActionTerminateSound;
		action->terminate_sound_unit        = unit;

		m_itemQueue->AddTail(new DQItem(DQITEM_TERMINATE_SOUND, action, dh_terminateSound));
	}
}

void DirectorImpl::AddInvokeThroneRoom(void)
{
	DQActionInvokeThroneRoom *  action = new DQActionInvokeThroneRoom;

	m_itemQueue->AddTail(new DQItem(DQITEM_INVOKE_THRONE_ROOM, action, dh_invokeThroneRoom));
}

void DirectorImpl::AddInvokeResearchAdvance(MBCHAR * message)
{
	DQActionInvokeResearchAdvance * action = new DQActionInvokeResearchAdvance;
	if (message)
	{
		MBCHAR * mess = new MBCHAR[strlen(message)+1];
		strcpy(mess, message);
		action->message = mess;
	}

	m_itemQueue->AddTail
		(new DQItem(DQITEM_INVOKE_RESEARCH_ADVANCE, action, dh_invokeResearchAdvance));
}

void DirectorImpl::AddBeginScheduler(sint32 player)
{
	Assert(player == g_selected_item->GetCurPlayer());
	if(player != g_selected_item->GetCurPlayer())
		return;

	DPRINTF(k_DBG_GAMESTATE, ("Director::AddBeginScheduler(%d)\n", player));

	DQActionBeginScheduler *    action = new DQActionBeginScheduler;
	action->player = player;

	m_itemQueue->AddTail(new DQItem(DQITEM_BEGIN_SCHEDULER, action, dh_beginScheduler));
}


BOOL DirectorImpl::TileWillBeCompletelyVisible(sint32 x, sint32 y)
{
	RECT tempViewRect = *g_tiledMap->GetMapViewRect();

	m_itemWalker->SetList(m_itemQueue);
	while (m_itemWalker->IsValid()) {
		DQItem	*item = m_itemWalker->GetObj();
		if (item->m_type == DQITEM_CENTERMAP) {
			DQActionCenterMap	*action = (DQActionCenterMap *)item->m_action;
			if (action) {
				MapPoint pos = action->centerMap_pos;
				g_radarMap->ComputeCenteredMap(pos, &tempViewRect);
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
