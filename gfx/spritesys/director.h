











#pragma once
#ifndef __DIRECTOR_H__
#define __DIRECTOR_H__

#include "aui.h"
#include "aui_mouse.h"

#include "Queue.h"

class Unit; 


extern enum ROUTE_TYPE;
enum GAMESOUNDS;

class TradeRoute; 

#include "PointerList.h"

#include "DirectorActions.h"













#define k_MAX_DIRECTOR_QUEUE_ITEMS		2000
#define k_MAXFRAMERATE 20

#define k_FIRSTACTOR 0
#define k_NOPROJECTILE -1

#define k_TRANSPORTREMOVEONLY -1
#define k_TRANSPORTADDONLY -2

#define k_TIME_LOG_SIZE				30			
#define k_DEFAULT_FPS				10
#define k_ELAPSED_CEILING			100			

class UnitActor;
class ProjectileActor;
class EffectActor;
class TradeActor;
class aui_Surface;
class Battle;
enum SPECATTACK;

template <class T> class tech_WLList;

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

class Sequence;

typedef void (DQHandler)(DQAction *action, Sequence *seq, DHEXECUTE executeType);








class DQItem {
public:
	DQItem(DQITEM_TYPE type, DQAction *action, DQHandler *handler);
	~DQItem();

	void			SetOwner(sint32 owner) { m_owner = (sint8)owner; }
	sint32			GetOwner(void) { return (sint32) m_owner; }
public:
	DQITEM_TYPE		m_type;
	uint8			m_addedToSavedList;
	sint8			m_owner;
	uint16			m_round;
	DQAction		*m_action;
	DQHandler		*m_handler;
	Sequence		*m_sequence;
};








enum SEQ_ACTOR {
	SEQ_ACTOR_PRIMARY = 0,
	SEQ_ACTOR_SECONDARY = 1,

	SEQ_ACTOR_MAX
};

class Sequence {
public:
	Sequence() 
	{ 
		m_sequenceID = 0; 
		m_refCount = 0; 
		m_addedToActiveList[SEQ_ACTOR_PRIMARY] = FALSE;
		m_addedToActiveList[SEQ_ACTOR_SECONDARY] = FALSE;
	}

	Sequence(sint32 seqID) 
	{ 
		m_sequenceID = seqID; 
		m_refCount = 0; 
		m_addedToActiveList[SEQ_ACTOR_PRIMARY] = FALSE;
		m_addedToActiveList[SEQ_ACTOR_SECONDARY] = FALSE;
	}

	~Sequence() {}

	sint32 GetSequenceID(void) { return m_sequenceID; }
	void	SetSequenceID(sint32 seqID) { m_sequenceID = 0; }
	void	AddRef(void) { m_refCount++; }
	void	Release(void) { m_refCount--; }
	sint32	GetRefCount(void) { return m_refCount; }

	void	SetItem(DQItem *item) { m_item = item; }
	DQItem	*GetItem(void) { return m_item; }

	void	SetAddedToActiveList(SEQ_ACTOR which, BOOL added) { m_addedToActiveList[which] = added; }
	BOOL	GetAddedToActiveList(SEQ_ACTOR which) { return m_addedToActiveList[which]; }
private:
	sint32		m_sequenceID;
	sint32		m_refCount;
	DQItem		*m_item;
	BOOL		m_addedToActiveList[SEQ_ACTOR_MAX];
};







class Director
{
public:
	Director(void);
	~Director(void);

	Sequence		*NewSequence(void);

	
	void			UpdateTimingClock(void);
	void			Process(void);
	void			PauseDirector(BOOL pause);

#ifdef _DEBUG
	void			DumpItem(DQItem *item);
	void			DumpInfo(void);
#endif 

	void			HandleNextAction(void);

	void			ActionFinished(Sequence *seq);

	void			HandleFinishedItem(DQItem *item);
	void			SaveFinishedItem(DQItem *item);
	void			GarbageCollectItems(void);

	void			ProcessImmediately(DQItem *item);

	void			CatchUp(void);
	BOOL			CaughtUp(void);

	BOOL			TileIsVisibleToPlayer(MapPoint &pos);

	
	bool			IsProcessing();

	
	void			AddMoveProcess(UnitActor *top, UnitActor *dest, sint32 arraySize, UnitActor **moveActors, BOOL isTransported); 

	void			AddMove(Unit &mover, MapPoint &oldPos, MapPoint &newPos, sint32 numRevealed, UnitActor **revealedActors, 
						sint32 numRest, UnitActor **restOfStack, BOOL isTransported, sint32 soundID);

	void			AddTeleport(Unit &top, MapPoint &oldPos, MapPoint &newPos, 	sint32 numRevealed, UnitActor **revealedActors,
							sint32 arraySize, UnitActor **moveActors);

	void			AddAttack(Unit attacker, Unit attacked);
	void			AddAttackPos(Unit attacker, MapPoint &pos);
	void			AddSpecialAttack(Unit attacker, Unit attacked, SPECATTACK attack);
	void			AddWinnerLoser(Unit victor, Unit dead);
	void			AddDeath(Unit dead);
	void			AddDeathWithSound(Unit dead, sint32 soundID);
	void			AddProjectileAttack(Unit shooting, Unit target, SpriteState *projectile_state, SpriteState *projectileEnd_state, sint32 projectile_Path);
	void			AddSpecialEffect(MapPoint &pos, sint32 spriteID, sint32 soundID);
	void			AddMorphUnit(UnitActor *morphingActor, SpriteState *ss, sint32 type,  Unit id);
	void			AddHide(Unit hider);
	void			AddShow(Unit hider);
	void			AddWork(Unit worker);
	void			AddFastKill(Unit dead);
	void			AddRemoveVision(MapPoint &pos, double range);	
	void			AddAddVision(MapPoint &pos, double range);		
	void			AddSetVisibility(UnitActor *actor, uint32 visibility);	
	void			AddSetOwner(UnitActor *actor, sint32 owner);	
	void			AddSetVisionRange(UnitActor *actor, double range);
	void			AddCombatFlash(MapPoint &pos);
	void			AddCopyVision(void);
	void			AddCenterMap(const MapPoint &pos);
	void			AddSelectUnit(uint32 flags);
	void			AddEndTurn(void);
	void			AddBattle(Battle *battle);
	void			AddPlaySound(sint32 soundID, MapPoint &pos);
	void            AddGameSound(GAMESOUNDS sound);
	void			AddPlayWonderMovie(sint32 which);
	void			AddPlayVictoryMovie(GAME_OVER reason, BOOL previouslyWon, BOOL previouslyLost);
	void			AddMessage(const Message &message);
	void			AddFaceoff(Unit &attacker, Unit &defender);
	void			AddTerminateFaceoff(Unit &faceroffer);
	void			AddTerminateSound(Unit &unit);
	void			AddInvokeThroneRoom(void);
	void			AddInvokeResearchAdvance(MBCHAR *text);	
	void            AddBeginScheduler(sint32 player);

 
	void			ActiveUnitAdd(UnitActor *unitActor);
	void			ActiveUnitRemove(UnitActor *unitActor);


	void			ActiveEffectAdd(EffectActor *effectActor);
	void			ActiveEffectRemove(EffectActor *effectActor);
	void			TradeActorCreate(TradeRoute newRoute);
	void			TradeActorDestroy(TradeRoute routeToDestroy);




	uint32			KillAllActiveEffects(void);

	void			DrawActiveUnits(RECT *paintRect, sint32 layer);

	void			DrawActiveEffects(RECT *paintRect, sint32 layer);
	void			DrawTradeRouteAnimations(RECT *paintRect, sint32 layer);

	uint32			ProcessActiveUnits(void);

	uint32			ProcessActiveEffects(void);
	void			ProcessTradeRouteAnimations(void);

	void			OffsetActiveUnits(sint32 deltaX, sint32 deltaY);

	void			OffsetActiveEffects(sint32 deltaX, sint32 deltaY);
	void			OffsetTradeRouteAnimations(sint32 deltaX, sint32 deltaY);

	UnitActor		*GetClickedActiveUnit(aui_MouseEvent *mouse);

	void			NextPlayer(BOOL forcedUpdate = FALSE);

	
	uint32			GetMasterCurTime(void) {return m_masterCurTime;}
	void			SetMasterCurTime(uint32 val) {m_masterCurTime = val;}
	sint32			GetAverageFPS(void) const { return m_averageFPS; }

	
	void		Kill(UnitActor *actor);	
	void        FastKill(UnitActor *actor);

	void        FastKill(EffectActor *actor);

	BOOL		GetActionFinished(void) { return m_actionFinished; }
	void		SetActionFinished(BOOL finished = TRUE) { m_actionFinished = finished; }

	BOOL        TileWillBeCompletelyVisible(sint32 x, sint32 y);

	void        SetHoldSchedulerSequence(Sequence *seq) { m_holdSchedulerSequence = seq; }

	void IncrementPendingGameActions();
	void DecrementPendingGameActions();

	void ReloadAllSprites();

	void NotifyResync();



	tech_WLList<UnitActor *>		*m_activeUnitList;

	tech_WLList<EffectActor *>		*m_activeEffectList;
	tech_WLList<TradeActor *>		*m_tradeActorList; 

	BOOL							m_nextPlayer; 

	
	uint32							m_masterCurTime;			
	sint32							m_lastTickCount;			
	sint32							m_timeLog[k_TIME_LOG_SIZE];	
	sint32							m_timeLogIndex;				
	sint32							m_averageElapsed;			
	sint32							m_averageFPS;

	BOOL							m_actionFinished;			
																
	BOOL							m_paused;
	BOOL							m_processingActiveUnits;	
	BOOL							m_processingActiveEffects;	

	



	sint32							m_curSequenceID;
	sint32							m_lastSequenceID;

	PointerList<DQItem>				*m_dispatchedItems;
	PointerList<DQItem>				*m_savedItems;
	PointerList<DQItem>				*m_itemQueue;

	PointerList<DQItem>::Walker		*m_itemWalker;

	Sequence *m_holdSchedulerSequence;

	sint32 m_pendingGameActions;
	bool m_endTurnRequested;
};

extern Director *g_director;




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

#endif 
