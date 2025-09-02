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
#include <map>

extern Background  *g_background;
extern C3UI        *g_c3ui;
extern MessagePool *g_theMessagePool;

#ifdef _PLAYTEST
extern bool g_doingFastRounds;
#endif

#define k_DEFAULT_FPS     10
#define k_ELAPSED_CEILING 100

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
	DQACTION_SPACE_LAUNCH
};

class DQActiveLoopingSound {
public:
	DQActiveLoopingSound(uint32 unitID, sint32 soundID) :
		m_unitID (unitID)
	{
		if (g_soundManager) {
			g_soundManager->AddLoopingSound(SOUNDTYPE_SFX, m_unitID, soundID);
		}
	}
	~DQActiveLoopingSound()
	{
		if (g_soundManager) {
			g_soundManager->TerminateLoopingSound(SOUNDTYPE_SFX, m_unitID);
		}
	}

	uint32 GetUnitID() { return m_unitID; }
private:
	uint32 m_unitID;
};

class DQAnimation {
public:
	DQAnimation() {}
	virtual ~DQAnimation() {}

	virtual void Process() = 0;
	virtual void Draw(const RECT & paintRect) = 0;
	virtual void Offset(sint32 deltaX, sint32 deltaY) = 0;

	virtual void Dump() = 0;
};

class DQAnimationTrade : public DQAnimation {
public:
	DQAnimationTrade() : DQAnimation() {}
	virtual ~DQAnimationTrade()
	{
		Clear();
	}

	virtual void Process()
	{
		if (g_theProfileDB->IsTradeAnim())
		{
			for (auto actorReference : m_tradeActors) {
				actorReference.second->Process();
			}
		}
	}

	virtual void Draw(const RECT & paintRect)
	{
		if (g_theProfileDB->IsTradeAnim())
		{
			for (auto actorReference : m_tradeActors) {
				actorReference.second->Draw(paintRect);
			}
		}
	}

	virtual void Offset(sint32 deltaX, sint32 deltaY) {}

	virtual void Dump() {
		DPRINTF(k_DBG_UI, (" ------------------\n"));
		DPRINTF(k_DBG_UI, (" Trade actors:\n"));
		for (auto actorReference : m_tradeActors)
		{
			TradeActor *actor = actorReference.second;
			DPRINTF(k_DBG_UI, ("  trade-actor        :%#x (%#.8lx)\n", actor->GetRouteID().m_id, actor));
		}
	}

	void AddTradeRoute(const TradeRoute &tradeRoute)
	{
		auto current = m_tradeActors.find(tradeRoute.m_id);
		if (current == m_tradeActors.end()) {
			m_tradeActors.insert({tradeRoute.m_id, new TradeActor(tradeRoute)});
		}
	}

	void RemoveTradeRoute(const TradeRoute &tradeRoute)
	{
		auto current = m_tradeActors.find(tradeRoute.m_id);
		if (current != m_tradeActors.end())
		{
			delete current->second;
			m_tradeActors.erase(current);
		}
	}

	void Clear()
	{
		for (auto actorReference : m_tradeActors) {
			delete actorReference.second;
		}
		m_tradeActors.clear();
	}

private:
	std::map<uint32, TradeActor *> m_tradeActors;
};

class DQAnimationStandby : public DQAnimation {
public:
	DQAnimationStandby() : DQAnimation() {}
	virtual ~DQAnimationStandby()
	{
		Clear();
	}

	virtual void Process()
	{
		for (auto actorReference : m_standbyActors) {
			UnitActor *actor = actorReference.first;
			if (!actor->IsActive()) {
				actorReference.first->Process();
			}
		}
	}

	virtual void Draw(const RECT & paintRect)
	{
		for (auto actorReference : m_standbyActors) {
			UnitActor *actor = actorReference.first;
			// Do not draw standby units over cities
			if (!actor->IsActive() && !g_theWorld->IsCity(actor->GetMapPos())) {
				actor->Draw(paintRect);
			}
		}
	}

	virtual void Offset(sint32 deltaX, sint32 deltaY)
	{
		for (auto actorReference : m_standbyActors) {
			UnitActor *actor = actorReference.first;
			if (!actor->IsActive())
			{
				actor->SetX(actor->GetX() + deltaX);
				actor->SetY(actor->GetY() + deltaY);
			}
		}
	}

	void AddActor(UnitActor *actor)
	{
		auto current = m_standbyActors.find(actor);
		if (current == m_standbyActors.end()) {
			m_standbyActors.insert({actor, 1});
		} else {
			current->second++;
		}
	}

	void RemoveActor(UnitActor *actor)
	{
		auto current = m_standbyActors.find(actor);
		Assert(current != m_standbyActors.end());
		current->second--;
		if (current->second <= 0) {
			m_standbyActors.erase(current);
		}
	}

	virtual void Dump() {
		DPRINTF(k_DBG_UI, (" ------------------\n"));
		DPRINTF(k_DBG_UI, (" Standby-actors:\n"));
		for (auto actorReference : m_standbyActors) {
			UnitActor *actor = actorReference.first;
			DPRINTF(k_DBG_UI, ("  unit-actor         :%#x (%#.8lx)\n", actor->GetUnitID(), actor));
		}
	}

	void Clear() {
		m_standbyActors.clear();
	}

private:
	std::map<UnitActor*, uint32> m_standbyActors;
};

/**
 * DQAction: implements two types of behaviour:
 * - Lock:      lock the scheduler while executing this action
 * - Animation: animate this action
 *
 * The combination of the two behaviours will create four kind of actions:
 *  - No Lock && No Animation -> immediate
 *  - No Lock && Animation    -> effect
 *  - Lock    && No Animation -> external
 *  - Lock    && Animation    -> active
 *
 * The action is done (IsDone) which means it can be deleted, when both behaviours are finished, i.e.
 *   IsUnlocked and IsAnimationFinished both return true.
 *
 * Generic-behaviour:
 *  -> ProcessLoopingSound handles the continuation of a looping sound
 *  -> Execute runs the execute phase which is always run (unconditionally)
 *  -> IsDone returns true when the action is done and can be deleted
 *
 * Lock-behaviour:
 *  -> Unlock unlocks the action
 * 	-> IsUnlocked returns true when the action is unlocked
 *
 * Animation-behaviour:
 *  -> Process runs animation
 *  -> Draw draws actors
 *  -> Offset moves actors
 *  -> IsAnimationFinished determines if animation is finished
 */
class DQAction {
public:
	DQAction() {}
	virtual ~DQAction() {}
	virtual DQACTION_TYPE GetType() = 0;

	// Generic behaviour
	virtual void ProcessLoopingSound(DQActiveLoopingSound* &activeLoopingSound) = 0;
	virtual void Execute() = 0;
	bool IsDone() { return IsUnlocked() && IsAnimationFinished(); }
	virtual void Dump() = 0;

	// Lock behaviour
	virtual void Unlock() = 0;
	virtual bool IsUnlocked() = 0;

	// Animation behaviour
	virtual void Process() = 0;
	virtual void Draw(const RECT & paintRect) = 0;
	virtual void Offset(sint32 deltaX, sint32 deltaY) = 0;
	virtual bool IsAnimationFinished() = 0;

protected:
	static bool TileIsVisibleToPlayer(const MapPoint &pos)
	{
		#if defined(_PLAYTEST)
			if (g_doingFastRounds) return false;
		#endif

		return g_tiledMap && g_tiledMap->GetLocalVision()->IsVisible(pos); // should this be g_player[g_selected_item->GetVisiblePlayer()]->IsVisible(pos)
	}

	static void CenterMap(const MapPoint &pos)
	{
		g_radarMap->CenterMap(pos);
		g_tiledMap->Refresh();
		g_tiledMap->InvalidateMap();
		g_tiledMap->InvalidateMix();
		background_draw_handler(g_background);
	}
};

/**
 * DQActionImmediate: No Lock and No Animation
 *   -> LoopingSound is ignored
 *   -> Execute implements the functionality of the action
 *   -> IsUnlocked and IsAnimationFinished both return true
 */
class DQActionImmediate : public DQAction {
public:
	DQActionImmediate() : DQAction() {}
	virtual ~DQActionImmediate() {}

	// Generic behaviour
	virtual void ProcessLoopingSound(DQActiveLoopingSound* &activeLoopingSound) {}
	virtual void Execute() = 0;

	// Lock behaviour
	virtual void Unlock() {}
	virtual bool IsUnlocked() { return true; }

	// Animation behaviour
	virtual void Process() {}
	virtual void Draw(const RECT & paintRect) {}
	virtual void Offset(sint32 deltaX, sint32 deltaY) {}
	virtual bool IsAnimationFinished() { return true; }
};

/**
 * DQActionEffect: No Lock and Animation
 *   -> LoopingSound is ignored
 *   -> IsUnlocked returns true
 *   -> IsAnimationFinished returns true when animation is done
 */
class DQActionEffect : public DQAction {
public:
	DQActionEffect(SpriteState *spriteState, const MapPoint &pos)
		: DQAction(),
		m_activeActor (new EffectActor(spriteState, pos))
	{}
	DQActionEffect(sint32 spriteID, const MapPoint &pos)
		: DQAction(),
		m_activeActor (new EffectActor(new SpriteState(spriteID), pos))
	{}
	virtual ~DQActionEffect() {
		delete m_activeActor;
	}

	// Generic behaviour
	virtual void ProcessLoopingSound(DQActiveLoopingSound* &activeLoopingSound) {}
	virtual void Execute() {}

	// Lock behaviour
	virtual void Unlock() {}
	virtual bool IsUnlocked() { return true; }

	// Animation behaviour
	virtual void Process()
	{
		m_activeActor->Process();
	}

	virtual void Draw(const RECT & paintRect)
	{
		m_activeActor->Draw(paintRect);
	}

	virtual void Offset(sint32 deltaX, sint32 deltaY) {}

	virtual bool IsAnimationFinished()
	{
		return m_activeActor->IsActionFinished();
	}

protected:
	EffectActor *m_activeActor;
};

/**
 * DQActionExternal: Lock, and No Animation
 *   -> LoopingSound is terminated
 *   -> Execute initiates the external process
 *   -> IsUnlocked returns false till Unlock has been called
 *   -> IsAnimationFinished returns true
 */
class DQActionExternal : public DQAction
{
public:
	DQActionExternal()
		: DQAction(),
		m_unlocked (false)
	{}
	virtual ~DQActionExternal() {}

	// Generic behaviour
	virtual void ProcessLoopingSound(DQActiveLoopingSound* &activeLoopingSound)
	{
		delete activeLoopingSound;
		activeLoopingSound = NULL;
	}

	virtual void Execute() {}

	// Lock behaviour
	virtual void Unlock() { m_unlocked = true; }
	virtual bool IsUnlocked() { return m_unlocked; }

	// Animation behaviour
	virtual void Process() {}
	virtual void Draw(const RECT & paintRect) {}
	virtual void Offset(sint32 deltaX, sint32 deltaY) {}
	virtual bool IsAnimationFinished() { return true; }

protected:
	bool m_unlocked;
};

/**
 * DQActionActive: Lock and Animation
 *   -> LoopingSound is processed; normally it is terminated; in case it is needed it will keep looping
 *   -> IsAnimationFinished returns true when animation is done
 *   -> IsUnlocked returns true when animation is done
 */
class DQActionActive : public DQAction
{
public:
	DQActionActive(sint8 owner)
		: DQAction(),
		m_owner	(owner),
		m_round	(0)
	{
		if (g_turn) {
			m_round = static_cast<uint16>(g_turn->GetRound());
		}
	}
	virtual ~DQActionActive() {}

	// Generic behaviour
	virtual void ProcessLoopingSound(DQActiveLoopingSound* &activeLoopingSound)
	{
		delete activeLoopingSound;
		activeLoopingSound = NULL;
	}

	virtual void Execute() {
		if (!SkipAnimation()) {
			PrepareAnimation();
		}
	}

	// Lock behaviour
	virtual void Unlock() {}
	virtual bool IsUnlocked() { return IsAnimationFinished(); }

	// Animation behaviour
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

	virtual void Draw(const RECT & paintRect)
	{
		for (const auto &activeActor : m_activeActors) {
			activeActor->Draw(paintRect);
		}
	}

	virtual void Offset(sint32 deltaX, sint32 deltaY)
	{
		for (const auto &activeActor : m_activeActors) {
			activeActor->SetX(activeActor->GetX() + deltaX);
			activeActor->SetY(activeActor->GetY() + deltaY);
		}
	}

	virtual bool IsAnimationFinished()
	{
		return m_activeActors.empty();
	}

protected:
	virtual void PrepareAnimation() = 0;
	virtual bool ForceAnimation() { return false; }
	virtual bool DoSkipAnimation() { return false; }

	void AddActiveActor(UnitActor *actor) {
		m_activeActors.insert(actor);
		actor->SetActive(true);
	}

	bool SkipAnimation()
	{
		return !ForceAnimation()
			&& (SkipPreviousRounds() || SkipEnemyMoves() || SkipRobotUnitAnimations() || DoSkipAnimation());
	}

private:
	bool SkipPreviousRounds()
	{
		return (m_round < g_turn->GetRound() - 1);
	}

	bool SkipEnemyMoves()
	{
		return (!g_theProfileDB->IsEnemyMoves() && m_owner != -1 && m_owner != g_selected_item->GetVisiblePlayer());
	}

	bool SkipRobotUnitAnimations()
	{
		return (!g_theProfileDB->IsUnitAnim() && m_owner != -1 && g_player[m_owner] && g_player[m_owner]->IsRobot());
	}

	sint8                          m_owner;
	uint16                         m_round;
	std::unordered_set<UnitActor*> m_activeActors;
};

// Forward definition; used to keep track of latest DQActionCenterMap so that TileWillBeCompletelyVisible is fast
class DQActionCenterMap;
class DirectorImpl : public Director {
public:
	DirectorImpl();
	virtual ~DirectorImpl();
	static DirectorImpl *Instance() { Assert(m_instance); return m_instance; }

	virtual void Clear();
	virtual void PauseDirector(bool pause);

	virtual void Process();
	virtual void Draw(const RECT & paintRect, sint32 layer);
	virtual void OffsetActors(sint32 deltaX, sint32 deltaY);

	virtual void ExternalActionFinished(DEACTION_TYPE externalActionType);

	virtual void NextPlayer();

	virtual void CatchUp();
	virtual bool CaughtUp();

#ifdef _DEBUG
	virtual void DumpInfo();
#endif

	virtual void AddMove(
		const Unit     &mover,
		MapPoint const &startPos,
		MapPoint const &endPos,
		sint32         numberOfMoveActors,
		UnitActor      **moveActors,
		sint32         soundID
	);
	virtual void AddTeleport(
		const Unit     &mover,
		MapPoint const &startPos,
		MapPoint const &endPos,
		sint32         numberOfMoveActors,
		UnitActor      **moveActors
	);
	virtual void AddSpaceLaunch(const Unit & launcher, const MapPoint & destinationPos);
	virtual void AddAttack(const Unit& attacker, const Unit& attacked);
	virtual void AddAttackPos(const Unit& attacker, const MapPoint &pos);
	virtual void AddSpecialAttack(const Unit& attacker, const Unit& attacked, SPECATTACK attack);
	virtual void AddDeath(UnitActor *dead, const MapPoint &deadPos, sint32 deadSoundID, PLAYER_INDEX killedBy);
	virtual void AddProjectileAttack(
		const Unit  &shooting,
		const Unit  &target,
		SpriteState *projectileState,
		SpriteState *projectileEndState,
		sint32      projectilePath
	);
	virtual void AddSpecialEffect(const MapPoint &pos, sint32 spriteID, sint32 soundID);
	virtual void AddMorphUnit(UnitActor *morphingActor, SpriteState *spriteState, sint32 type, const Unit &id);
	virtual void AddHide(const Unit &hider);
	virtual void AddShow(const Unit &hider);
	virtual void AddWork(const Unit &worker);
	virtual void AddFastKill(UnitActor *dead);
	virtual void AddRemoveVision(const MapPoint &pos, double range);
	virtual void AddAddVision(const MapPoint &pos, double range);
	virtual void AddSetVisibility(UnitActor *actor, uint32 visibility);
	virtual void AddSetOwner(UnitActor *actor, sint32 owner);
	virtual void AddCombatFlash(const MapPoint &pos);
	virtual void AddCopyVision();
	virtual void AddCenterMap(const MapPoint &pos);
	virtual void AddSelectUnit(uint32 flags);
	virtual void AddEndTurn();
	virtual void AddBattle(Battle *battle);
	virtual void AddPlaySound(sint32 soundID, const MapPoint &pos);
	virtual void AddGameSound(GAMESOUNDS sound);
	virtual void AddPlayWonderMovie(sint32 which);
	virtual void AddPlayVictoryMovie(GAME_OVER reason, bool previouslyWon, bool previouslyLost);
	virtual void AddMessage(const Message &message);
	virtual void AddFaceoff(const Unit &attacker, const Unit &defender);
	virtual void AddTerminateFaceoff(const Unit &faceroffer);
	virtual void AddTerminateSound(const Unit &unit);
	virtual void AddInvokeThroneRoom();
	virtual void AddInvokeResearchAdvance(MBCHAR *text);
	virtual void AddBeginScheduler(sint32 player);

	// Animations
	virtual void AddTradeRouteAnimation(const TradeRoute &tradeRoute);
	virtual void RemoveTradeRouteAnimation(const TradeRoute &tradeRoute);

	// Anim
	virtual uint32 GetMasterCurTime() { return m_masterCurTime; }
	// battleviewwindow
	virtual void UpdateTimingClock();

	// ArmyData and others
	virtual bool TileWillBeCompletelyVisible(sint32 x, sint32 y);

	// Used locally
	void AddStandbyActor(UnitActor *actor);
	void RemoveStandbyActor(UnitActor *actor);

private:
	bool CanStartNextAction();
	void HandleNextAction();
	void FinalizeAction(DQAction *action);
	void FinalizeAnimatingActions();

	void ProcessActions();
	void ProcessAnimations();

	void DrawActions(const RECT & paintRect);
	void DrawAnimations(const RECT & paintRect);

	void OffsetActions(sint32 deltaX, sint32 deltaY);
	void OffsetAnimations(sint32 deltaX, sint32 deltaY);

#ifdef _DEBUG
	void DumpAction(DQAction *action);
#endif

	static DirectorImpl               *m_instance;
	PointerList<DQAction>             *m_actionQueue;
	DQAction                          *m_lockingAction;
	DQActiveLoopingSound              *m_activeLoopingSound;
	std::unordered_set<DQAction *>    m_animatingActions;
	DQAnimationTrade                  *m_tradeAnimations;
	DQAnimationStandby                *m_standbyAnimations;
	uint32                            m_nextProcessTime;
	// Optimization: keep track of latest added DQActionCenterMap
	DQActionCenterMap                 *m_latestCenterMap;

	static const int                  k_TIME_LOG_SIZE = 30;
	uint32                            m_masterCurTime;
	sint32                            m_lastTickCount;
	sint32                            m_timeLog[k_TIME_LOG_SIZE];
	sint32                            m_timeLogIndex;
	sint32                            m_averageElapsed;
	sint32                            m_averageFPS;

	bool                              m_paused;

	sint32                            m_lastPlayer;
	sint32                            m_lastRound;
};

Director* Director::CreateDirector() {
	return new DirectorImpl();
}

void Director::ReloadAllSprites()
{
	for (int player = 0; player < k_MAX_PLAYERS; player++)
	{
		if (!g_player[player]) {
			continue;
		}

		//PFT  29 mar 05
		//cycle through human players' cities
		if (g_player[g_selected_item->GetVisiblePlayer()]->IsHuman())
		{
			for (int i = 0; i < g_player[player]->m_all_cities->Num(); i++)
			{
				Unit city = g_player[player]->m_all_cities->Access(i);

				//recover the number of turns until the city next produces a pop from it's current state
				//CityData *cityData = city.GetData()->GetCityData();
				//cityData->TurnsToNextPop();

				UnitActor *actor = city.GetActor();
				city.GetSpriteState()->SetIndex(city.GetDBRec()->GetDefaultSprite()->GetValue());
				actor->ChangeImage(city.GetSpriteState(), city.GetType(), city);
			}
		}

		for (int i = 0; i < g_player[player]->m_all_units->Num(); i++)
		{
			Unit unit = g_player[player]->m_all_units->Access(i);
			UnitActor *actor = unit.GetActor();
			unit.GetSpriteState()->SetIndex(unit.GetDBRec()->GetDefaultSprite()->GetValue());
			actor->ChangeImage(unit.GetSpriteState(), unit.GetType(), unit);
		}
	}
}

class DQActionTeleport : public DQActionImmediate
{
public:
	DQActionTeleport(
			UnitActor      *moveActor,
			const MapPoint &startPos,
			const MapPoint &endPos,
			sint32         numberOfMoveActors,
			UnitActor      **moveActors)
		: DQActionImmediate(),
		moveActor              (moveActor),
		startPos               (startPos),
		endPos                 (endPos),
		numberOfMoveActors     (numberOfMoveActors),
		moveActors             (moveActors)
	{
		DirectorImpl::Instance()->AddStandbyActor(moveActor);
	}
	virtual ~DQActionTeleport() {
		moveActor->PositionActor(endPos);

		for (int i = 0; i < numberOfMoveActors; i++)
		{
			moveActors[i]->PositionActor(endPos);
		}

		DirectorImpl::Instance()->RemoveStandbyActor(moveActor);
		delete[] moveActors;
	}
	virtual DQACTION_TYPE GetType() { return DQACTION_TELEPORT; }

	virtual void Execute()
	{
		moveActor->PositionActor(endPos);

		for (int i = 0; i < numberOfMoveActors; i++)
		{
			UnitActor *moveActor = moveActors[i];
			moveActor->PositionActor(endPos);
		}
	}

	virtual void Dump()
	{
		DPRINTF(k_DBG_UI, ("Teleport\n"));
		DPRINTF(k_DBG_UI, ("  actor                  :%#x (%#.8lx)\n", moveActor->GetUnitID(), moveActor));
		DPRINTF(k_DBG_UI, ("  startPosition          :%d,%d\n", startPos.x, startPos.y));
		DPRINTF(k_DBG_UI, ("  endPosition            :%d,%d\n", endPos.x, endPos.y));
		DPRINTF(k_DBG_UI, ("  numberOfMoveActors     :%d\n", numberOfMoveActors));
		if (numberOfMoveActors > 0)
		{
			DPRINTF(k_DBG_UI, ("  moveActors\n"));
			for (int i = 0; i < numberOfMoveActors; i++) {
				DPRINTF(k_DBG_UI, ("    %d.                  :%#x (%#.8lx)\n", i, moveActors[i]->GetUnitID(),
						moveActors[i]));
			}
		}
	}

protected:
	UnitActor *moveActor;
	MapPoint  startPos;
	MapPoint  endPos;
	sint32    numberOfMoveActors;
	UnitActor **moveActors;
};

class DQActionMorph : public DQActionImmediate
{
public:
	DQActionMorph(UnitActor *morphingActor, SpriteState *spriteState, sint32 type, const Unit &id)
		: DQActionImmediate(),
		morphingActor (morphingActor),
		spriteState   (spriteState),
		type          (type),
		id            (id)
	{}
	virtual ~DQActionMorph() {}
	virtual DQACTION_TYPE GetType() { return DQACTION_MORPH; }

	virtual void Execute()
	{
		morphingActor->ChangeType(spriteState, type, id);
	}

	virtual void Dump()
	{
		DPRINTF(k_DBG_UI, ("Morph\n"));
		DPRINTF(k_DBG_UI, ("  morphingActor          :%#x (%#.8lx)\n", morphingActor->GetUnitID(), morphingActor));
		DPRINTF(k_DBG_UI, ("  spriteState            :%#.8lx (%d)\n", spriteState, spriteState->GetIndex()));
		DPRINTF(k_DBG_UI, ("  type                   :%d\n", type));
		DPRINTF(k_DBG_UI, ("  id                     :%#.8lx\n", id.m_id));
	}
protected:
	UnitActor   *morphingActor;
	SpriteState *spriteState;
	sint32      type;
	Unit        id;
};

class DQActionHideShow : public DQActionImmediate
{
public:
	DQActionHideShow(UnitActor *hidingActor)
		: DQActionImmediate(),
		hidingActor (hidingActor)
	{}
	virtual ~DQActionHideShow() {}

protected:
	UnitActor *hidingActor;
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
		DPRINTF(k_DBG_UI, ("  hidingActor            :%#x (%#.8lx)\n", hidingActor->GetUnitID(), hidingActor));
	}
};

class DQActionShow : public DQActionHideShow
{
public:
	DQActionShow(UnitActor *hidingActor, const MapPoint &hidingPos)
		: DQActionHideShow(hidingActor),
		hidingPos (hidingPos)
	{}
	virtual ~DQActionShow() {}
	virtual DQACTION_TYPE GetType() { return DQACTION_SHOW; }

	virtual void Execute()
	{
		hidingActor->PositionActor(hidingPos);
		hidingActor->Show();
	}

	virtual void Dump() {
		DPRINTF(k_DBG_UI, ("Show\n"));
		DPRINTF(k_DBG_UI, ("  hidingActor            :%#x (%#.8lx)\n", hidingActor->GetUnitID(), hidingActor));
		DPRINTF(k_DBG_UI, ("  hidingPosition         :%d,%d\n", hidingPos.x, hidingPos.y));
	}
protected:
	MapPoint hidingPos;
};

class DQActionFastKill : public DQActionImmediate
{
public:
	DQActionFastKill(UnitActor *dead)
		: DQActionImmediate(),
		dead (dead)
	{}
	virtual ~DQActionFastKill()
	{
		delete dead;
	}
	virtual DQACTION_TYPE GetType() { return DQACTION_FASTKILL; }

	virtual void Execute() {}

	virtual void Dump()
	{
		DPRINTF(k_DBG_UI, ("Fast Kill\n"));
		DPRINTF(k_DBG_UI, ("  dead                   :%#x (%#.8lx)\n", dead->GetUnitID(), dead));
	}
private:
	UnitActor *dead;
};

class DQActionVision : public DQActionImmediate
{
public:
	DQActionVision(const MapPoint &visionPos, double visionRange)
		: DQActionImmediate(),
		visionPos   (visionPos),
		visionRange (visionRange)
	{}
	virtual ~DQActionVision() {}

protected:
	MapPoint visionPos;
	double   visionRange;
};

class DQActionRemoveVision : public DQActionVision
{
public:
	DQActionRemoveVision(const MapPoint &visionPos, double visionRange)
		: DQActionVision(visionPos, visionRange)
	{}
	virtual ~DQActionRemoveVision() {}
	virtual DQACTION_TYPE GetType() { return DQACTION_REMOVEVISION; }

	virtual void Execute()
	{
		if(g_tiledMap) {
			g_tiledMap->RemoveVisible(visionPos, visionRange);
		}
	}

	virtual void Dump()
	{
		DPRINTF(k_DBG_UI, ("Remove Vision\n"));
		DPRINTF(k_DBG_UI, ("  visionPosition         :%d,%d\n", visionPos.x, visionPos.y));
		DPRINTF(k_DBG_UI, ("  visionRange            :%#.2f\n", visionRange));
	}
};

class DQActionAddVision : public DQActionVision {
public:
	DQActionAddVision(const MapPoint &visionPos, double visionRange)
		: DQActionVision(visionPos, visionRange)
	{}
	virtual ~DQActionAddVision() {}
	virtual DQACTION_TYPE GetType() { return DQACTION_ADDVISION; }

	virtual void Execute()
	{
		if(g_tiledMap) {
			g_tiledMap->AddVisible(visionPos, visionRange);
		}
	}

	virtual void Dump()
	{
		DPRINTF(k_DBG_UI, ("Add Vision\n"));
		DPRINTF(k_DBG_UI, ("  visionPosition         :%d,%d\n", visionPos.x, visionPos.y));
		DPRINTF(k_DBG_UI, ("  visionRange            :%#.2f\n", visionRange));
	}
};

class DQActionSetVisibility : public DQActionImmediate
{
public:
	DQActionSetVisibility(UnitActor *actor, uint32 flag)
		: DQActionImmediate(),
		actor (actor),
		flag  (flag)
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
		DPRINTF(k_DBG_UI, ("  actor                  :%#x (%#.8lx)\n", actor->GetUnitID(), actor));
		DPRINTF(k_DBG_UI, ("  flag                   :%%#.8lx\n", flag));
	}
protected:
	UnitActor *actor;
	uint32    flag;
};

class DQActionSetOwner : public DQActionImmediate
{
public:
	DQActionSetOwner(UnitActor *setOwnerActor, sint32 owner)
		: DQActionImmediate(),
		setOwnerActor (setOwnerActor),
		owner         (owner)
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
		DPRINTF(k_DBG_UI, ("  setOwnerActor          :%#x (%#.8lx)\n", setOwnerActor->GetUnitID(), setOwnerActor));
		DPRINTF(k_DBG_UI, ("  owner                  :%d\n", owner));
	}
protected:
	UnitActor *setOwnerActor;
	sint32    owner;
};

class DQActionCenterMap : public DQActionImmediate
{
public:
	DQActionCenterMap(const MapPoint &centerMapPos)
		: DQActionImmediate(),
		centerMapPos (centerMapPos)
	{}
	virtual ~DQActionCenterMap() {}
	virtual DQACTION_TYPE GetType() { return DQACTION_CENTERMAP; }

	virtual void Execute()
	{
		if(!g_selected_item->GetIsPathing()) {
			CenterMap(centerMapPos);
		}
	}

	virtual void Dump()
	{
		DPRINTF(k_DBG_UI, ("Center Map\n"));
		DPRINTF(k_DBG_UI, ("  centerMapPosition      :%d,%d\n", centerMapPos.x, centerMapPos.y));
	}

	bool TileIsCompletelyVisible(sint32 x, sint32 y)
	{
		RECT tempViewRect = g_tiledMap->GetMapViewRect();
		g_radarMap->ComputeCenteredMap(centerMapPos, &tempViewRect);
		return g_tiledMap->TileIsCompletelyVisible(x, y, &tempViewRect);
	}

protected:
	MapPoint centerMapPos;
};

class DQActionCopyVision : public DQActionImmediate
{
public:
	DQActionCopyVision() : DQActionImmediate() {}
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
		flags (flags)
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
		DPRINTF(k_DBG_UI, ("  flags                  :%#x\n", flags));
	}
protected:
	uint32 flags;
};

class DQActionEndTurn : public DQActionImmediate
{
public:
	DQActionEndTurn() : DQActionImmediate() {}
	virtual ~DQActionEndTurn() {}
	virtual DQACTION_TYPE GetType() { return DQACTION_ENDTURN; }

	virtual void Execute()
	{
		g_gevManager->AddEvent(GEV_INSERT_Tail, GEV_EndTurn,
							   GEA_Player, g_selected_item->GetCurPlayer(),
							   GEA_End);
	}

	virtual void Dump()
	{
		DPRINTF(k_DBG_UI, ("End Turn\n"));
	}
};

class DQActionPlaySound : public DQActionImmediate
{
public:
	DQActionPlaySound(sint32 soundID, const MapPoint &pos)
		: DQActionImmediate(),
		soundID (soundID),
		pos     (pos)
	{}
	virtual ~DQActionPlaySound() {}
	virtual DQACTION_TYPE GetType() { return DQACTION_PLAYSOUND; }

	virtual void Execute()
	{
		if (g_soundManager) {
			g_soundManager->AddSound(SOUNDTYPE_SFX, 0, soundID, pos.x, pos.y);
		}
	}

	virtual void Dump()
	{
		DPRINTF(k_DBG_UI, ("Play Sound\n"));
		DPRINTF(k_DBG_UI, ("  soundID                :%d\n", soundID));
		DPRINTF(k_DBG_UI, ("  position               :%d,%d\n", pos.x, pos.y));
	}
protected:
	sint32   soundID;
	MapPoint pos;
};

class DQActionMessage : public DQActionImmediate
{
public:
	DQActionMessage(const Message &message)
		: DQActionImmediate(),
		message (message)
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
	Message message;
};

class DQActionTerminateFaceOff : public DQActionImmediate
{
public:
	DQActionTerminateFaceOff(UnitActor *faceOffer)
		: DQActionImmediate(),
		faceOffer (faceOffer)
	{}
	virtual ~DQActionTerminateFaceOff() {}
	virtual DQACTION_TYPE GetType() { return DQACTION_TERMINATE_FACEOFF; }

	virtual void Execute()
	{
		faceOffer->SetHealthPercent(-1.0);
		faceOffer->SetTempStackSize(0);
	}

	virtual void Dump()
	{
		DPRINTF(k_DBG_UI, ("Terminate Faceoff\n"));
		DPRINTF(k_DBG_UI, ("  faceroffer             :%#x (%#.8lx)\n", faceOffer->GetUnitID(), faceOffer));
	}
protected:
	UnitActor *faceOffer;
};

class DQActionTerminateSound : public DQActionImmediate
{
public:
	DQActionTerminateSound(uint32 terminateSoundUnitID)
		: DQActionImmediate(),
		terminateSoundUnitID (terminateSoundUnitID)
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
		DPRINTF(k_DBG_UI, ("  terminate_sound_unit   :%#.8lx\n", terminateSoundUnitID));
	}
protected:
	uint32 terminateSoundUnitID;
};

class DQActionInvokeThroneRoom : public DQActionImmediate
{
public:
	DQActionInvokeThroneRoom() : DQActionImmediate() {}
	virtual ~DQActionInvokeThroneRoom() {}
	virtual DQACTION_TYPE GetType() { return DQACTION_INVOKE_THRONE_ROOM; }

	virtual void Execute() {}

	virtual void Dump()
	{
		DPRINTF(k_DBG_UI, ("Invoke Throne-room\n"));
	}
};

class DQActionMoveProjectile : public DQActionEffect
{
public:
	DQActionMoveProjectile(SpriteState *projectileEndState, const MapPoint &startPos, const MapPoint &endPos)
		: DQActionEffect(projectileEndState, endPos),
		startPos (startPos),
		endPos   (endPos)
	{}
	virtual ~DQActionMoveProjectile() {}
	virtual DQACTION_TYPE GetType() { return DQACTION_MOVEPROJECTILE; }

	virtual void Execute()
	{
		if (TileIsVisibleToPlayer(startPos))
		{
			EFFECTACTION effectAction;
			Anim * animation = m_activeActor->CreatePlayElseFlashAnim(effectAction);
			Assert(animation);
			if (animation) {
				Action * action = Action::CreateEffectAction(effectAction, animation, startPos, endPos);
				m_activeActor->SetAction(action);
			}
		}
	}

	virtual void Dump()
	{
		DPRINTF(k_DBG_UI, ("Move Projectile\n"));
		DPRINTF(k_DBG_UI, ("  startPosition          :%d,%d\n", startPos.x, startPos.y));
		DPRINTF(k_DBG_UI, ("  endPosition            :%d,%d\n", endPos.x, endPos.y));
	}
protected:
	MapPoint startPos;
	MapPoint endPos;
};

class DQActionCombatFlash : public DQActionEffect
{
public:
	DQActionCombatFlash(const MapPoint &flashPos)
		: DQActionEffect(99, flashPos)
	{}
	virtual ~DQActionCombatFlash() {}
	virtual DQACTION_TYPE GetType() { return DQACTION_COMBATFLASH; }

	virtual void Execute()
	{
		EFFECTACTION effectAction;
		Anim *animation = m_activeActor->CreatePlayElseFlashAnim(effectAction);
		Assert(animation);
		if (animation)
		{
			Action * action = Action::CreateEffectAction(effectAction, animation);
			m_activeActor->SetAction(action);
		}
	}

	virtual void Dump()
	{
		DPRINTF(k_DBG_UI, ("Combat Flash\n"));
		const MapPoint pos = m_activeActor->GetMapPos();
		DPRINTF(k_DBG_UI, ("  flashPosition          :%d,%d\n", pos.x, pos.y));
	}
};

class DQActionSpecialEffect : public DQActionEffect
{
public:
	DQActionSpecialEffect(const MapPoint &pos, sint32 spriteID, sint32 soundID)
		: DQActionEffect(spriteID, pos),
		spriteID (spriteID),
		soundID  (soundID)
	{}
	virtual ~DQActionSpecialEffect() {}
	virtual DQACTION_TYPE GetType() { return DQACTION_SPECEFFECT; }

	virtual void Execute()
	{
	        const MapPoint pos = m_activeActor->GetMapPos();
		if(g_player[g_selected_item->GetVisiblePlayer()]->IsVisible(pos)){ // only play animation and sound if pos is generally visible to current player
			Anim *animation = m_activeActor->CreatePlayAnim();
			if (animation)
			{
				Action *action = Action::CreateEffectAction(EFFECTACTION_PLAY, animation);
				m_activeActor->SetAction(action);

				if (g_soundManager)
				{
					g_soundManager->AddSound(SOUNDTYPE_SFX, 0, soundID, pos.x, pos.y);
				}
			}
		}
	}

	virtual void Dump()
	{
		DPRINTF(k_DBG_UI, ("Special Effect\n"));
		const MapPoint pos = m_activeActor->GetMapPos();
		DPRINTF(k_DBG_UI, ("  position               :%d,%d\n", pos.x, pos.y));
		DPRINTF(k_DBG_UI, ("  spriteID               :%d\n", spriteID));
		DPRINTF(k_DBG_UI, ("  soundID                :%d\n", soundID));
	}
protected:
	sint32 spriteID;
	sint32 soundID;
};

class DQActionMove : public DQActionActive
{
public:
	DQActionMove(
			sint8          owner,
			UnitActor      *moveActor,
			const MapPoint &startPos,
			const MapPoint &endPos,
			sint32         numberOfMoveActors,
			UnitActor      **moveActors,
			sint32         soundID)
		: DQActionActive(owner),
		moveActor              (moveActor),
		startPos               (startPos),
		endPos                 (endPos),
		numberOfMoveActors     (numberOfMoveActors),
		moveActors             (moveActors),
		soundID                (soundID)
	{
		DirectorImpl::Instance()->AddStandbyActor(moveActor);
	}
	virtual ~DQActionMove()
	{
		moveActor->PositionActor(endPos);

		for (int i = 0; i < numberOfMoveActors; i++)
		{
			moveActors[i]->PositionActor(endPos);
		}

		DirectorImpl::Instance()->RemoveStandbyActor(moveActor);
		delete [] moveActors;
	}
	virtual DQACTION_TYPE GetType() { return DQACTION_MOVE; }

	virtual void ProcessLoopingSound(DQActiveLoopingSound* &activeLoopingSound)
	{
		if (!activeLoopingSound || activeLoopingSound->GetUnitID() != moveActor->GetUnitID()) {
			delete activeLoopingSound;
			activeLoopingSound = NULL;

			if (!SkipAnimation() && g_soundManager && soundID >= 0) {
				sint32 visiblePlayer = g_selected_item->GetVisiblePlayer();
				if ((visiblePlayer == moveActor->GetPlayerNum()) ||
						(moveActor->GetUnitVisibility() & (1 << visiblePlayer)))
				{
					activeLoopingSound = new DQActiveLoopingSound(moveActor->GetUnitID(), soundID);
				}
			}
		}
	}

	virtual void PrepareAnimation()
	{
		Anim  * animation = moveActor->CreateMoveAnim();
		if (!animation) {
			return;
		}

		Action *action = Action::CreateUnitAction(UNITACTION_MOVE, animation, startPos, endPos);
		moveActor->PositionActor(startPos);
		moveActor->SetIsFortifying(false);
		moveActor->SetIsFortified (false);
		moveActor->AddAction(action);

		for (int i = 0; i < numberOfMoveActors; i++) {
			moveActors[i]->SetHiddenUnderStack(true);
		}

		const MapPoint & actorPosition = moveActor->GetMapPos();
		if (g_selected_item->GetVisiblePlayer() != moveActor->GetPlayerNum()
			&& !g_tiledMap->TileIsVisible(actorPosition.x, actorPosition.y))
		{
			CenterMap(actorPosition);
		}
		AddActiveActor(moveActor);
	}

	virtual void Dump()
	{
		DPRINTF(k_DBG_UI, ("Move\n"));
		DPRINTF(k_DBG_UI, ("  actor                  :%#x (%#.8lx)\n", moveActor->GetUnitID(), moveActor));
		DPRINTF(k_DBG_UI, ("  startPosition          :%d,%d\n", startPos.x, startPos.y));
		DPRINTF(k_DBG_UI, ("  endPosition            :%d,%d\n", endPos.x, endPos.y));
		DPRINTF(k_DBG_UI, ("  numberOfMoveActors     :%d\n", numberOfMoveActors));
		if (numberOfMoveActors > 0)
		{
			DPRINTF(k_DBG_UI, ("  moveActors\n"));
			for (int i = 0; i < numberOfMoveActors; i++) {
				DPRINTF(k_DBG_UI, ("    %d.                  :%#x (%#.8lx)\n", i, moveActors[i]->GetUnitID(),
						moveActors[i]));
			}
		}
		DPRINTF(k_DBG_UI, ("  soundID                :%d\n", soundID));
	}

protected:
	virtual bool DoSkipAnimation()
	{
		// Unit invisible
		return !TileIsVisibleToPlayer(startPos) && !TileIsVisibleToPlayer(endPos);
	}

	UnitActor *moveActor;
	MapPoint  startPos;
	MapPoint  endPos;
	sint32    numberOfMoveActors;
	UnitActor **moveActors;
	sint32    soundID;
};

class DQActionSpaceLaunch : public DQActionActive
{
public:
	DQActionSpaceLaunch(
			sint8            owner,
			UnitActor      * launchActor,
			const MapPoint & launchPos,
			const MapPoint & destinationPos,
			sint32           soundID)
		: DQActionActive(owner),
		launchActor         (launchActor),
		launchPos           (launchPos),
		directionalNeighbor (DetermineDirectionalNeighbor(launchPos, destinationPos)),
		soundID             (soundID)
	{
		DirectorImpl::Instance()->AddStandbyActor(launchActor);
	}
	virtual ~DQActionSpaceLaunch()
	{
		DirectorImpl::Instance()->RemoveStandbyActor(launchActor);
	}
	virtual DQACTION_TYPE GetType() { return DQACTION_SPACE_LAUNCH; }

	virtual void PrepareAnimation()
	{
		Anim * animation = launchActor->CreateMoveAnim();
		if (!animation) {
			return;
		}

		Action * action = Action::CreateUnitAction(UNITACTION_MOVE, animation, launchPos, directionalNeighbor);
		launchActor->PositionActor(launchPos);
		launchActor->SetIsFortifying(false);
		launchActor->SetIsFortified (false);
		launchActor->AddAction(action);

		AddActiveActor(launchActor);

		if (g_soundManager) {
			g_soundManager->AddSound(SOUNDTYPE_SFX, launchActor->GetUnitID(), soundID, launchPos.x, launchPos.y);
		}
	}

	virtual void Dump()
	{
		DPRINTF(k_DBG_UI, ("Space launch\n"));
		DPRINTF(k_DBG_UI, ("  actor                  :%#x (%#.8lx)\n", launchActor->GetUnitID(), launchActor));
		DPRINTF(k_DBG_UI, ("  launchPosition         :%d,%d\n", launchPos.x, launchPos.y));
		DPRINTF(k_DBG_UI, ("  directionNeighbor      :%d,%d\n", directionalNeighbor.x, directionalNeighbor.y));
		DPRINTF(k_DBG_UI, ("  soundID                :%d\n", soundID));
	}

protected:
	virtual bool DoSkipAnimation()
	{
		// Unit invisible
		return !TileIsVisibleToPlayer(launchPos) && !TileIsVisibleToPlayer(directionalNeighbor);
	}

	UnitActor * launchActor;
	MapPoint    launchPos;
	MapPoint    directionalNeighbor;
	sint32      soundID;

private:
	static MapPoint DetermineDirectionalNeighbor(const MapPoint & startPos, const MapPoint & endPos)
	{
		WORLD_DIRECTION neighborDirection = NOWHERE;
		MapPointData delta = startPos.NormalizedSubtract(endPos);
		if (delta.y == 0) {
			neighborDirection = delta.x > 0 ? EAST : WEST;
		}
		else
		{
			if (delta.x == 0) {
				neighborDirection = delta.y > 0 ? SOUTH : NORTH;
			}
			else {
				double directionRatio = ((double) delta.x) / delta.y;
				if (directionRatio < -2.0 || directionRatio > 2.0) // x-dominant
				{
					neighborDirection = delta.x > 0 ? EAST : WEST;
				}
				else if (directionRatio > -0.5 && directionRatio < 0.5) // y-dominant
				{
					neighborDirection = delta.y > 0 ? SOUTH : NORTH;
				}
				else // no dominance
				{
					if (delta.y < 0) {
						neighborDirection = delta.x > 0 ? NORTHEAST : NORTHWEST;
					} else {
						neighborDirection = delta.x > 0 ? SOUTHEAST : SOUTHWEST;
					}
				}
			}
		}
		MapPoint directionalNeighbor;
		startPos.GetNeighborPosition(neighborDirection, directionalNeighbor);
		return directionalNeighbor;
	}
};


class DQActionAttack : public DQActionActive
{
public:
	DQActionAttack(
			sint8          owner,
			UnitActor      *attacker,
			const MapPoint &attackerPos,
			bool           attackerIsCity,
			UnitActor      *defender,
			const MapPoint &defenderPos,
			bool           defenderIsCity)
		: DQActionActive(owner),
		attacker       (attacker),
		attackerPos    (attackerPos),
		attackerIsCity (attackerIsCity),
		defender       (defender),
		defenderPos    (defenderPos),
		defenderIsCity (defenderIsCity)
	{}
	virtual ~DQActionAttack() {}
	virtual DQACTION_TYPE GetType() { return DQACTION_ATTACK; }

	virtual void Dump()
	{
		DPRINTF(k_DBG_UI, ("%s\n", GetName()));
		DPRINTF(k_DBG_UI, ("  attacker               :%#x (%#.8lx)\n", attacker->GetUnitID(), attacker));
		DPRINTF(k_DBG_UI, ("  attackerPosition       :%d,%d\n", attackerPos.x, attackerPos.y));
		DPRINTF(k_DBG_UI, ("  attackerIsCity         :%s\n", attackerIsCity ? "true" : "false"));
		DPRINTF(k_DBG_UI, ("  defender               :%#x (%#.8lx)\n", defender->GetUnitID(), defender));
		DPRINTF(k_DBG_UI, ("  defenderPosition       :%d,%d\n", defenderPos.x, defenderPos.y));
		DPRINTF(k_DBG_UI, ("  defenderIsCity         :%s\n", defenderIsCity ? "true" : "false"));
	}
protected:
	virtual MBCHAR const * const GetName() { return "Attack"; }

	virtual void PrepareAnimation()
	{
		POINT  attackerPoints, defenderPoints;
		maputils_MapXY2PixelXY(attackerPos.x, attackerPos.y, attackerPoints);
		maputils_MapXY2PixelXY(defenderPos.x, defenderPos.y, defenderPoints);

		sint32  deltaX = defenderPoints.x - attackerPoints.x;
		sint32  deltaY = defenderPoints.y - attackerPoints.y;

		bool playerInvolved = (defender->GetPlayerNum() == g_selected_item->GetVisiblePlayer())
							  || (attacker->GetPlayerNum() == g_selected_item->GetVisiblePlayer());
		if (playerInvolved)
		{
			sint32 facing = spriteutils_DeltaToFacing(deltaX, deltaY);
			Action *action = Action::CreateUnitAction(UNITACTION_ATTACK, attacker->CreateAttackAnim(), attackerPos,
					facing);
			action->PutUnitsVisibility(attacker->GetUnitVisibility());
			attacker->AddAction(action);

			AddActiveActor(attacker);
		}

		bool defenderVisible = TileIsVisibleToPlayer(defenderPos);
		if (!defenderIsCity && (playerInvolved || defenderVisible))
		{
			sint32 facing = spriteutils_DeltaToFacing(-deltaX, -deltaY);
			Action *action = Action::CreateUnitAction(UNITACTION_ATTACK, defender->CreateAttackAnim(), defenderPos,
					facing);
			action->PutUnitsVisibility(defender->GetUnitVisibility());
			defender->AddAction(action);

			AddActiveActor(defender);
		}
	}

	UnitActor *attacker;
	MapPoint  attackerPos;
	bool      attackerIsCity;
	UnitActor *defender;
	MapPoint  defenderPos;
	bool      defenderIsCity;
};

class DQActionSpecialAttack : public DQActionAttack
{
public:
	DQActionSpecialAttack(
			sint8          owner,
			UnitActor      *attacker,
			const MapPoint &attackerPos,
			bool           attackerIsCity,
			sint32         attackerSoundID,
			UnitActor      *defender,
			const MapPoint &defenderPos,
			bool           defenderIsCity)
		: DQActionAttack(owner, attacker, attackerPos, attackerIsCity, defender, defenderPos, defenderIsCity),
		attackerSoundID (attackerSoundID)
	{}
	virtual ~DQActionSpecialAttack() {}
	virtual DQACTION_TYPE GetType() { return DQACTION_SPECATTACK; }

	virtual void Dump()
	{
		DQActionAttack::Dump();
		DPRINTF(k_DBG_UI, ("  attackerSoundID        :%d\n", attackerSoundID));
	}

protected:
	virtual MBCHAR const * const GetName() { return "Special attack"; }

	virtual bool DoSkipAnimation() {
		return attackerIsCity && defenderIsCity;
	}

	virtual void PrepareAnimation()
	{
		POINT attackerPoints, defenderPoints;
		maputils_MapXY2PixelXY(attackerPos.x, attackerPos.y, attackerPoints);
		maputils_MapXY2PixelXY(defenderPos.x, defenderPos.y, defenderPoints);

		sint32 deltaX = defenderPoints.x - attackerPoints.x;
		sint32 deltaY = defenderPoints.y - attackerPoints.y;

		bool attackerVisible = TileIsVisibleToPlayer(attackerPos);
		bool defenderVisible = TileIsVisibleToPlayer(defenderPos);

		if (attackerSoundID >= 0 && defenderVisible) { // removed || attackerVisible to suppress sound if e.g. invisible slaver is not in FOW of visiblePlayer but his target is in FOW
			g_soundManager->AddSound(SOUNDTYPE_SFX, 0, attackerSoundID, 0, 0);
		}

		if (!attackerIsCity && attackerVisible)
		{
			Anim * animation = attacker->CreateSpecialAttackAnim();
			if (animation) {
				sint32 facing = spriteutils_DeltaToFacing(deltaX, deltaY);
				Action *attackerAction = Action::CreateUnitAction(UNITACTION_ATTACK, animation, attackerPos, facing);
				attackerAction->PutUnitsVisibility(attacker->GetUnitVisibility());
				attacker->AddAction(attackerAction);
			}
		}

		if (!defenderIsCity && defenderVisible)
		{
			Anim * animation = defender->CreateSpecialAttackAnim();
			if (animation) {
				sint32 facing = spriteutils_DeltaToFacing(-deltaX, -deltaY);
				Action *defenderAction = Action::CreateUnitAction(UNITACTION_ATTACK, animation, defenderPos, facing);
				defenderAction->PutUnitsVisibility(defender->GetUnitVisibility());
				defender->AddAction(defenderAction);
			}
		}
	}

	sint32 attackerSoundID;
};

class DQActionAttackPos : public DQActionActive
{
public:
	DQActionAttackPos(
			sint8          owner,
			UnitActor      *attacker,
			const MapPoint &attackerPos,
			const MapPoint &targetPos,
			sint32         soundID)
		: DQActionActive(owner),
		attacker    (attacker),
		attackerPos (attackerPos),
		targetPos   (targetPos),
		soundID     (soundID)
	{}
	virtual ~DQActionAttackPos() {}
	virtual DQACTION_TYPE GetType() { return DQACTION_ATTACKPOS; }

	virtual void Dump()
	{
		DPRINTF(k_DBG_UI, ("Attack Pos\n"));
		DPRINTF(k_DBG_UI, ("  attacker               :%#x (%#.8lx)\n", attacker->GetUnitID(), attacker));
		DPRINTF(k_DBG_UI, ("  attackerPosition       :%d,%d\n", attackerPos.x, attackerPos.y));
		DPRINTF(k_DBG_UI, ("  targetPosition         :%d,%d\n", targetPos.x, targetPos.y));
		DPRINTF(k_DBG_UI, ("  soundID                :%d\n", soundID));
	}
protected:
	virtual bool DoSkipAnimation()
	{
		// Unit invisible
		return !TileIsVisibleToPlayer(attackerPos);
	}

	virtual void PrepareAnimation()
	{
		POINT attackerPoints, attackedPoints;
		maputils_MapXY2PixelXY(attackerPos.x, attackerPos.y, attackerPoints);
		maputils_MapXY2PixelXY(targetPos.x, targetPos.y, attackedPoints);
		sint32 facing = spriteutils_DeltaToFacing(
				attackedPoints.x - attackerPoints.x,
				attackedPoints.y - attackerPoints.y);

		Action *action = Action::CreateUnitAction(UNITACTION_ATTACK, attacker->CreateAttackAnim(), attackerPos, facing);
		action->PutUnitsVisibility(attacker->GetUnitVisibility());
		attacker->AddAction(action);
		AddActiveActor(attacker);

		if (g_soundManager) {
			sint32 visiblePlayer = g_selected_item->GetVisiblePlayer();
			if ((visiblePlayer == attacker->GetPlayerNum()) ||
				(attacker->GetUnitVisibility() & (1 << visiblePlayer)))
			{
				g_soundManager->AddSound(SOUNDTYPE_SFX, attacker->GetUnitID(), soundID,
										 attackerPos.x, attackerPos.y);
			}
		}
	}

	UnitActor *attacker;
	MapPoint  attackerPos;
	MapPoint  targetPos;
	sint32    soundID;
};

class DQActionDeath : public DQActionActive
{
public:
	DQActionDeath(sint8 owner, UnitActor *dead, const MapPoint &deadPos, sint32 deadSoundID, PLAYER_INDEX killedBy)
		: DQActionActive(owner),
		dead        (dead),
		deadPos     (deadPos),
		deadSoundID (deadSoundID),
		killedBy    (killedBy)
	{
		DirectorImpl::Instance()->AddStandbyActor(dead);
	}
	virtual ~DQActionDeath()
	{
		DirectorImpl::Instance()->RemoveStandbyActor(dead);
		delete dead;
	}
	virtual DQACTION_TYPE GetType() { return DQACTION_DEATH; }

	virtual void Dump()
	{
		DPRINTF(k_DBG_UI, ("Death\n"));
		DPRINTF(k_DBG_UI, ("  dead                   :%#x (%#.8lx)\n", dead->GetUnitID(), dead));
		DPRINTF(k_DBG_UI, ("  deadPosition           :%d,%d\n", deadPos.x, deadPos.y));
		DPRINTF(k_DBG_UI, ("  deadSoundID            :%d\n", deadSoundID));
	}
protected:
	virtual bool ForceAnimation()
	{
		return killedBy == g_selected_item->GetVisiblePlayer();
	}

	virtual bool DoSkipAnimation()
	{
		// Unit invisible
		return !TileIsVisibleToPlayer(deadPos);
	}

	virtual void PrepareAnimation()
	{
		UNITACTION deathActionType;
		Anim * deathAnimation = dead->CreateDeadAnim(deathActionType);

		dead->SetHealthPercent(-1.0);
		dead->SetTempStackSize(0);

		Action *action = Action::CreateUnitAction(deathActionType, deathAnimation, deadPos);
		dead->AddAction(action);
		AddActiveActor(dead);

		if (g_soundManager) {
			sint32 visiblePlayer = g_selected_item->GetVisiblePlayer();
			if ((visiblePlayer == dead->GetPlayerNum()) ||
				(dead->GetUnitVisibility() & (1 << visiblePlayer))) {
				g_soundManager->AddSound(SOUNDTYPE_SFX, (uint32)dead->GetUnitID(), deadSoundID,
										 deadPos.x, deadPos.y);
			}
		}
	}

	UnitActor    * dead;
	MapPoint       deadPos;
	sint32         deadSoundID;
	PLAYER_INDEX   killedBy;
};

class DQActionWork : public DQActionActive
{
public:
	DQActionWork(sint8 owner, UnitActor *workActor, const MapPoint &workPos, sint32 workSoundID)
		: DQActionActive(owner),
		workActor   (workActor),
		workPos     (workPos),
		workSoundID (workSoundID)
	{}
	virtual ~DQActionWork() {}
	virtual DQACTION_TYPE GetType() { return DQACTION_WORK; }

	virtual void Dump()
	{
		DPRINTF(k_DBG_UI, ("Work\n"));
		DPRINTF(k_DBG_UI, ("  workActor              :%#x (%#.8lx)\n", workActor->GetUnitID(), workActor));
		DPRINTF(k_DBG_UI, ("  workPosition           :%d,%d\n", workPos.x, workPos.y));
		DPRINTF(k_DBG_UI, ("  workSoundID            :%d\n", workSoundID));
	}
protected:
	virtual bool DoSkipAnimation()
	{
		// Unit invisible
		return !TileIsVisibleToPlayer(workPos);
	}

	virtual void PrepareAnimation()
	{
		Anim * animation = workActor->CreateWorkAnim();
		if (!animation) {
			return;
		}
		Action *action = Action::CreateUnitAction(UNITACTION_WORK, animation, workPos);
		workActor->AddAction(action);
		AddActiveActor(workActor);

		if (g_soundManager)
		{
			sint32 visiblePlayer = g_selected_item->GetVisiblePlayer();
			if ((visiblePlayer == workActor->GetPlayerNum()) ||
				(workActor->GetUnitVisibility() & (1 << visiblePlayer)))
			{
				g_soundManager->AddSound(SOUNDTYPE_SFX, workActor->GetUnitID(), workSoundID,
						workPos.x, workPos.y);
			}
		}
	}

	UnitActor *workActor;
	MapPoint  workPos;
	sint32    workSoundID;
};

class DQActionFaceoff : public DQActionActive
{
public:
	DQActionFaceoff(
			sint8          owner,
			UnitActor      *attacker,
			const MapPoint &attackerPos,
			UnitActor      *attacked,
			const MapPoint &attackedPos)
		: DQActionActive(owner),
		attacker    (attacker),
		attackerPos (attackerPos),
		attacked    (attacked),
		attackedPos (attackedPos)
	{}
	virtual ~DQActionFaceoff() {}
	virtual DQACTION_TYPE GetType() { return DQACTION_FACEOFF; }

	virtual void Dump()
	{
		DPRINTF(k_DBG_UI, ("Faceoff\n"));
		DPRINTF(k_DBG_UI, ("  attacker               :%#x (%#.8lx)\n", attacker->GetUnitID(), attacker));
		DPRINTF(k_DBG_UI, ("  attackerPosition       :%d,%d\n", attackerPos.x, attackerPos.y));
		DPRINTF(k_DBG_UI, ("  attacked               :%#x (%#.8lx)\n", attacked->GetUnitID(), attacked));
		DPRINTF(k_DBG_UI, ("  attackedPosition       :%d,%d\n", attackedPos.x, attackedPos.y));
	}
protected:
	virtual void PrepareAnimation()
	{
		Anim * attackerAnim = Anim::MakeFaceoff();
		POINT attackerPoints, attackedPoints;
		maputils_MapXY2PixelXY(attackerPos.x, attackerPos.y, attackerPoints);
		maputils_MapXY2PixelXY(attackedPos.x, attackedPos.y, attackedPoints);
		sint32 attackerFacing = spriteutils_DeltaToFacing(
				attackedPoints.x - attackerPoints.x,
				attackedPoints.y - attackerPoints.y);

		Action * attackerAction = Action::CreateUnitAction(UNITACTION_FACE_OFF, attackerAnim, attackerPos,
				attackerFacing);
		attacker->AddAction(attackerAction);

		Anim * attackedAnim = Anim::MakeFaceoff();
		sint32 attackedFacing = spriteutils_DeltaToFacing(
				attackerPoints.x - attackedPoints.x,
				attackerPoints.y - attackedPoints.y);
		Action *attackedAction = Action::CreateUnitAction(UNITACTION_FACE_OFF, attackedAnim, attackedPos,
				attackedFacing);
		attacked->AddAction(attackedAction);

		bool attackedVisible = TileIsVisibleToPlayer(attackedPos);
		if (attacker->GetPlayerNum() == g_selected_item->GetVisiblePlayer() ||
			attacked->GetPlayerNum() == g_selected_item->GetVisiblePlayer())
		{
			attackedVisible = true;
		}
		if (attackedVisible) {
			AddActiveActor(attacked);
		}

		bool attackerVisible = TileIsVisibleToPlayer(attackerPos);
		if (attacked->GetPlayerNum() == g_selected_item->GetVisiblePlayer() ||
			attacker->GetPlayerNum() == g_selected_item->GetVisiblePlayer())
		{
			attackerVisible = true;
			attackedVisible = true;
		}
		if (attackerVisible && attackedVisible) {
			AddActiveActor(attacker);
		}
	}

	UnitActor *attacker;
	MapPoint  attackerPos;
	UnitActor *attacked;
	MapPoint  attackedPos;
};

class DQActionInvokeResearchAdvance : public DQActionExternal
{
public:
	DQActionInvokeResearchAdvance(MBCHAR const * const message)
		: DQActionExternal(),
		message (strdup(message))
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
		DPRINTF(k_DBG_UI, ("  message                :%s\n", message));
	}
protected:
	MBCHAR *message;
};

class DQActionBattle : public DQActionExternal
{
public:
	DQActionBattle(Battle *battle)
		: DQActionExternal(),
		battle (battle)
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
	Battle *battle;
};

class DQActionPlayWonderMovie : public DQActionExternal
{
public:
	DQActionPlayWonderMovie(sint32 wonderMovieID)
		: DQActionExternal(),
		wonderMovieID (wonderMovieID)
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
		DPRINTF(k_DBG_UI, ("  wonderMovieID          :%ld\n", wonderMovieID));
	}
protected:
	sint32 wonderMovieID;
};

class DQActionPlayVictoryMovie : public DQActionExternal
{
public:
	DQActionPlayVictoryMovie(GAME_OVER reason)
		: DQActionExternal(),
		reason (reason)
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
		DPRINTF(k_DBG_UI, ("  reason                 :%ld\n", reason));
	}
protected:
	GAME_OVER reason;
};

class DQActionBeginScheduler : public DQActionExternal
{
public:
	DQActionBeginScheduler(sint32 player)
		: DQActionExternal(),
		player (player)
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
		DPRINTF(k_DBG_UI, ("  player                 :%d\n", player));
	}
protected:
	sint32 player;
};

DirectorImpl *DirectorImpl::m_instance = NULL;
DirectorImpl::DirectorImpl()
:
		m_actionQueue        (new PointerList<DQAction>),
		m_lockingAction      (NULL),
		m_activeLoopingSound (NULL),
		m_animatingActions   (),
		m_tradeAnimations    (new DQAnimationTrade()),
		m_standbyAnimations  (new DQAnimationStandby()),
		m_latestCenterMap    (NULL),
		m_nextProcessTime    (0),
		m_masterCurTime      (0),
		m_lastTickCount      (0),
		m_timeLogIndex       (0),
		m_averageElapsed     (0),
		m_averageFPS         (k_DEFAULT_FPS),
		m_paused             (false),
		m_lastPlayer         (-1),
		m_lastRound          (-1)
{
	std::fill(m_timeLog, m_timeLog + k_TIME_LOG_SIZE, 0);

	delete m_instance;
	m_instance = this;
}

DirectorImpl::~DirectorImpl()
{
	Clear();
	delete m_tradeAnimations;
	delete m_standbyAnimations;
	delete m_actionQueue;
	m_instance = NULL;
}

void DirectorImpl::Clear() {
	for (auto animatingAction : m_animatingActions) {
		if (animatingAction == m_lockingAction) {
			m_lockingAction = NULL;
		}
		delete animatingAction;
	}
	m_animatingActions.clear();

	if (m_lockingAction) {
		delete m_lockingAction;
		m_lockingAction = NULL;
	}

	m_actionQueue->DeleteAll();

	m_tradeAnimations->Clear();
	m_standbyAnimations->Clear();

	delete m_activeLoopingSound;
	m_activeLoopingSound = NULL;

	m_latestCenterMap = NULL;
}

void DirectorImpl::UpdateTimingClock()
{
	sint32 elapsed;

	if (m_lastTickCount == 0) {
		elapsed = 1000/k_DEFAULT_FPS;
	} else {
		elapsed = GetTickCount() - m_lastTickCount;
	}

	if (elapsed > k_ELAPSED_CEILING) {
		elapsed = k_ELAPSED_CEILING;
	}

	m_lastTickCount = GetTickCount();

	m_timeLog[m_timeLogIndex++] = elapsed;

	if (m_timeLogIndex >= k_TIME_LOG_SIZE)
	{
		sint32 timeSum = 0;
		for (sint32 i = 0; i < k_TIME_LOG_SIZE; i++) {
			timeSum += m_timeLog[i];
		}

		m_averageElapsed = (timeSum / k_TIME_LOG_SIZE);

		if (m_averageElapsed > 0) {
			m_averageFPS = 1000 / m_averageElapsed;
		} else {
			m_averageFPS = k_DEFAULT_FPS;
		}

		m_timeLogIndex = 0;
	}

	m_masterCurTime += elapsed;
}

void DirectorImpl::AddStandbyActor(UnitActor *actor)
{
	m_standbyAnimations->AddActor(actor);
}

void DirectorImpl::RemoveStandbyActor(UnitActor *actor)
{
	m_standbyAnimations->RemoveActor(actor);
}

void DirectorImpl::Process()
{
	UpdateTimingClock();

	if (GetTickCount() > m_nextProcessTime)
	{
		ProcessActions();

		HandleNextAction();
		// To ensure smooth animations this needs to be done after HandleNextAction
		ProcessAnimations();

		if (g_tiledMap) {
			g_tiledMap->ProcessLayerSprites(g_tiledMap->GetMapViewRect(), 0);
		}

		m_nextProcessTime = GetTickCount() + 75;
	}
}

void DirectorImpl::PauseDirector(bool pause)
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

void DirectorImpl::DumpInfo()
{
	DPRINTF(k_DBG_UI, (" ------------------\n"));
	DPRINTF(k_DBG_UI, ("Director Dump:\n"));
	DPRINTF(k_DBG_UI, (" ------------------\n"));
	DPRINTF(k_DBG_UI, (" Current Action:\n"));
	DumpAction(m_lockingAction);
	DPRINTF(k_DBG_UI, ("-------------------\n"));
	DPRINTF(k_DBG_UI, (" Animating actions:\n"));
	DPRINTF(k_DBG_UI, (" Count:%d\n", m_animatingActions.size()));
	for (auto action : m_animatingActions) {
		DumpAction(action);
	}
	DPRINTF(k_DBG_UI, (" ------------------\n"));
	DPRINTF(k_DBG_UI, (" Queued actions:\n"));
	DPRINTF(k_DBG_UI, (" Count:%d\n", m_actionQueue->GetCount()));

	for (PointerList<DQAction>::Walker walker(m_actionQueue); walker.IsValid(); walker.Next()) {
		DumpAction(walker.GetObj());
	}
	m_tradeAnimations->Dump();
	m_standbyAnimations->Dump();
	DPRINTF(k_DBG_UI, (" ------------------\n"));
}
#endif

bool DirectorImpl::CanStartNextAction() {
	return !m_actionQueue->IsEmpty() && (!m_lockingAction || m_lockingAction->IsUnlocked());
}

void DirectorImpl::HandleNextAction()
{
	if (m_paused) {
		return;
	}

	while (CanStartNextAction())
	{
		if (m_lockingAction && m_lockingAction->IsDone()) {
			FinalizeAction(m_lockingAction);
		}
		m_lockingAction = m_actionQueue->RemoveHead();
	 	m_lockingAction->ProcessLoopingSound(m_activeLoopingSound);
	 	m_lockingAction->Execute();
		if (m_lockingAction && !m_lockingAction->IsAnimationFinished()) {
			m_animatingActions.insert(m_lockingAction);
		}
	}

	if (m_actionQueue->IsEmpty()) {
		delete m_activeLoopingSound;
		m_activeLoopingSound = NULL;
	}
}

void DirectorImpl::ProcessActions()
{
	for (auto it = m_animatingActions.begin(); it != m_animatingActions.end(); )
	{
		DQAction *action = *it;
		action->Process();
		if (action->IsAnimationFinished()) {
			it = m_animatingActions.erase(it);
			if (action->IsDone()) {
				FinalizeAction(action);
			}
		} else {
			++it;
		}
	}
}

void DirectorImpl::ProcessAnimations()
{
	m_tradeAnimations->Process();
	m_standbyAnimations->Process();
}

void DirectorImpl::FinalizeAction(DQAction *action)
{
	if (action == m_latestCenterMap) {
		m_latestCenterMap = NULL;
	}

	m_animatingActions.erase(action);
	if (m_lockingAction == action) {
		m_lockingAction = NULL;
	}
	delete action;
}

void DirectorImpl::FinalizeAnimatingActions()
{
	for (auto it = m_animatingActions.begin(); it != m_animatingActions.end(); ) {
		DQAction *action = *it;
		it = m_animatingActions.erase(it);
		FinalizeAction(action);
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

	if (m_lockingAction && m_lockingAction->GetType() == actionType) {
		m_lockingAction->Unlock();
	}
}

void DirectorImpl::CatchUp() {
	if (m_lockingAction) {
		FinalizeAction(m_lockingAction);
	}
	FinalizeAnimatingActions();

	while (!m_actionQueue->IsEmpty())
	{
		DQAction *action = m_actionQueue->RemoveHead();
		action->Execute();
		FinalizeAction(action);
	}

	if (!g_network.IsActive())
	{
		if (g_soundManager) {
			g_soundManager->TerminateAllLoopingSounds(SOUNDTYPE_SFX);
		}
	}
}

bool DirectorImpl::CaughtUp()
{
	return !m_actionQueue || m_actionQueue->IsEmpty();
}

void DirectorImpl::AddTradeRouteAnimation(const TradeRoute &tradeRoute)
{
	m_tradeAnimations->AddTradeRoute(tradeRoute);
}

void DirectorImpl::RemoveTradeRouteAnimation(const TradeRoute& tradeRoute)
{
	m_tradeAnimations->RemoveTradeRoute(tradeRoute);
}

void DirectorImpl::OffsetActors(sint32 deltaX, sint32 deltaY)
{
	OffsetAnimations(-deltaX, -deltaY);
	OffsetActions(-deltaX, -deltaY);
}

void DirectorImpl::OffsetActions(sint32 deltaX, sint32 deltaY)
{
	for (auto action : m_animatingActions) {
		action->Offset(deltaX, deltaY);
	}
}

void DirectorImpl::OffsetAnimations(sint32 deltaX, sint32 deltaY)
{
	m_standbyAnimations->Offset(deltaX, deltaY);
	m_tradeAnimations->Offset(deltaX, deltaY);
}

void DirectorImpl::NextPlayer() {
#ifdef _PLAYTEST
	if (!g_doingFastRounds &&
		(!g_network.IsActive() || g_player[g_selected_item->GetVisiblePlayer()]->IsRobot()))
	{
		return;
	}
#else
	return; // Next code isn't used, should it be used?
#endif
	if (!g_network.IsActive()) {
		if (m_lockingAction) {
			FinalizeAction(m_lockingAction);
		}
		FinalizeAnimatingActions();
	}

	while (!m_actionQueue->IsEmpty())
	{
		DQAction *action = m_actionQueue->RemoveHead();
		action->Execute();
		FinalizeAction(action);
	}

	if (!g_network.IsActive())
	{
		if (g_tiledMap) {
			g_tiledMap->NextPlayer();
		}

		if (g_soundManager) {
			g_soundManager->TerminateAllLoopingSounds(SOUNDTYPE_SFX);
		}
	}
}

void DirectorImpl::Draw(const RECT & paintRect, sint32 layer)
{
	DrawAnimations(paintRect);
	DrawActions(paintRect);
}

void DirectorImpl::DrawActions(const RECT & paintRect)
{
	for (auto action : m_animatingActions) {
		action->Draw(paintRect);
	}
}

void DirectorImpl::DrawAnimations(const RECT & paintRect)
{
	m_tradeAnimations->Draw(paintRect);
	m_standbyAnimations->Draw(paintRect);
}

void DirectorImpl::AddMove (
		const Unit     &mover,
		const MapPoint &startPos,
		const MapPoint &endPos,
		sint32         numberOfMoveActors,
		UnitActor      **moveActors,
		sint32         soundID)
{
	Assert(!mover.IsCity());

	UnitActor *actor = mover.GetActor();
	if (!actor) {
		return;
	}
	Assert(actor->GetUnitID() == mover.m_id);

	if (g_theProfileDB->IsEnemyMoves() &&
		mover.GetOwner() != g_selected_item->GetVisiblePlayer() &&
		(mover.GetVisibility() & (1 << g_selected_item->GetVisiblePlayer())) &&
		!TileWillBeCompletelyVisible(endPos.x, endPos.y))
	{
		AddCenterMap(endPos);
	}

	actor->SetHiddenUnderStack(false);

	DQActionMove *action = new DQActionMove(
			mover.GetOwner(),
			actor,
			startPos,
			endPos,
			numberOfMoveActors,
			numberOfMoveActors > 0 ? moveActors : NULL,
			soundID);
	m_actionQueue->AddTail(action);
}

void DirectorImpl::AddTeleport (
		const Unit     &mover,
		const MapPoint &startPos,
		const MapPoint &endPos,
		sint32         numberOfMoveActors,
		UnitActor      **moveActors)
{
	if (!mover->GetActor()) {
		return;
	}

	DQActionTeleport *action = new DQActionTeleport(
			mover->GetActor(),
			startPos,
			endPos,
			numberOfMoveActors,
			moveActors);
	m_actionQueue->AddTail(action);
}

void DirectorImpl::AddProjectileAttack(
		const Unit  &shooting,
		const Unit  &target,
		SpriteState *projectileState,
		SpriteState *projectileEndState,
		sint32      projectilePath)
{
	DQActionMoveProjectile *action = new DQActionMoveProjectile(projectileEndState, shooting.RetPos(), target.RetPos());
	m_actionQueue->AddTail(action);
}

void DirectorImpl::AddSpecialEffect(const MapPoint &pos, sint32 spriteID, sint32 soundID)
{
	if(g_selected_item->IsAutoCenterOn() 
	    && !TileWillBeCompletelyVisible(pos.x, pos.y)
	    && g_player[g_selected_item->GetVisiblePlayer()]->IsVisible(pos)
	    ){ // center on pos if generally visible but not in current view
	    AddCenterMap(pos);
	    }
	DQActionSpecialEffect *action = new DQActionSpecialEffect(pos, spriteID, soundID);
	m_actionQueue->AddTail(action);
}

void DirectorImpl::AddCombatFlash(const MapPoint &pos)
{
	DQActionCombatFlash	*action = new DQActionCombatFlash(pos);
	m_actionQueue->AddTail(action);
}

void DirectorImpl::AddCopyVision()
{
	DQActionCopyVision *action = new DQActionCopyVision();
	m_actionQueue->AddTail(action);
}

void DirectorImpl::AddCenterMap(const MapPoint &pos)
{
	DQActionCenterMap *action = new DQActionCenterMap(pos);
	m_actionQueue->AddTail(action);
	m_latestCenterMap = action;
}

void DirectorImpl::AddSelectUnit(uint32 flags)
{
	DQActionUnitSelection *action = new DQActionUnitSelection(flags);
	m_actionQueue->AddTail(action);
}

void DirectorImpl::AddEndTurn()
{
	DPRINTF(k_DBG_GAMESTATE, ("Director::AddEndTurn, curPlayer = %d\n", g_selected_item->GetCurPlayer()));

	if (g_selected_item->GetCurPlayer() == m_lastPlayer &&
		g_player[m_lastPlayer] && g_player[m_lastPlayer]->m_current_round == m_lastRound)
	{
		for (PointerList<DQAction>::Walker walker(m_actionQueue); walker.IsValid(); walker.Next())
		{
			if (walker.GetObj()->GetType() == DQACTION_ENDTURN) {
				DPRINTF(k_DBG_GAMESTATE, ("Skipping duplicate end turn for %d,%d\n", m_lastPlayer, m_lastRound));
				return;
			}
		}
	}

	m_lastPlayer = g_selected_item->GetCurPlayer();
	m_lastRound  = g_player[m_lastPlayer] ? g_player[m_lastPlayer]->m_current_round : -1;

	DQActionEndTurn *action = new DQActionEndTurn();
	m_actionQueue->AddTail(action);
}

void DirectorImpl::AddAttack(const Unit &attacker, const Unit &defender)
{
	if (!attacker.GetActor() || !defender.GetActor() || attacker == Unit() || defender == Unit()) {
		return;
	}

	UnitActor *attackerActor = attacker.GetActor();
	attackerActor->SetHealthPercent(attacker.GetHP() / attacker->CalculateTotalHP());

	Army army = attacker.GetArmy();
	if (army.IsValid()) {
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
	if (visiblePlayer && visiblePlayer->IsVisible(attacker.RetPos())) {
		AddCombatFlash(attacker.RetPos());
	}

	if (visiblePlayer && visiblePlayer->IsVisible(defender.RetPos())) {
		AddCombatFlash(defender.RetPos());
	}
}

void DirectorImpl::AddSpaceLaunch(const Unit & launcher, const MapPoint & destinationPos)
{
	if (!launcher.GetActor()) {
		return;
	}

	bool playerInvolved = (launcher->GetOwner() == g_selected_item->GetVisiblePlayer());
	bool visibleEnemyUnit = g_theProfileDB->IsEnemyMoves()
	                        && (launcher->GetVisibility() & (1 << g_selected_item->GetVisiblePlayer()));
	if ((playerInvolved || visibleEnemyUnit)
		&& !TileWillBeCompletelyVisible(launcher.RetPos().x, launcher.RetPos().y)) {
		AddCenterMap(launcher.RetPos());
	}

	DQActionSpaceLaunch *action = new DQActionSpaceLaunch(
			launcher.GetOwner(),
			launcher.GetActor(),
			launcher.RetPos(),
			destinationPos,
			gamesounds_GetGameSoundID(GAMESOUNDS_SPACE_LAUNCH)
	);
	m_actionQueue->AddTail(action);
}

void DirectorImpl::AddAttackPos(const Unit &attacker, const MapPoint &pos)
{
	if (!attacker.GetActor()) {
		return;
	}

	DQActionAttackPos *action = new DQActionAttackPos(
			attacker.GetOwner(), attacker.GetActor(), attacker.RetPos(), pos, attacker.GetAttackSoundID());
	m_actionQueue->AddTail(action);

	if (g_player[g_selected_item->GetVisiblePlayer()] &&
		g_player[g_selected_item->GetVisiblePlayer()]->IsVisible(pos))
	{
		AddCombatFlash(pos);
	}
}

void DirectorImpl::AddSpecialAttack(const Unit& attacker, const Unit &attacked, SPECATTACK attack)
{
	sint32 soundID;
	sint32 spriteID;
	if (!attacker.GetSpecialAttackInfo(attack, &soundID, &spriteID)) {
		return;
	}
	if (!attacker.IsValid() || !attacker.GetActor() || !attacked.IsValid() || !attacked->GetActor()) {
		return;
	}

	if (g_player[g_selected_item->GetVisiblePlayer()] &&
		g_player[g_selected_item->GetVisiblePlayer()]->IsVisible(attacked.RetPos()))
	{
		MapPoint attackPosition;
		if (attacker->GetVisibility() & (1 << g_selected_item->GetVisiblePlayer())) {
			attackPosition = attacker.RetPos();
		} else { // attacker is invisible; do not give away its position by the animation
			attackPosition = attacked.RetPos();
		}
		DQActionMoveProjectile * moveProjectileAction = new DQActionMoveProjectile(new SpriteState(spriteID),
				attackPosition, attacked.RetPos());
		m_actionQueue->AddTail(moveProjectileAction);
		if(g_selected_item->IsAutoCenterOn() 
		    && !TileWillBeCompletelyVisible(attackPosition.x, attackPosition.y)
		    && g_player[g_selected_item->GetVisiblePlayer()]->IsVisible(attackPosition)
		    ){ // center on pos if generally visible but not in current view
		    AddCenterMap(attackPosition);
		    }
	}
	
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
}

void DirectorImpl::AddDeath(UnitActor *dead, const MapPoint &deadPos, sint32 deadSoundID, PLAYER_INDEX killedBy)
{
	Assert(dead);

	bool playerInvolved = (dead->GetPlayerNum() == g_selected_item->GetVisiblePlayer())
			|| (killedBy == g_selected_item->GetVisiblePlayer());
	bool visibleEnemyUnit = g_theProfileDB->IsEnemyMoves()
							&& dead->GetPlayerNum() != g_selected_item->GetVisiblePlayer()
							&& (dead->GetUnitVisibility() & (1 << g_selected_item->GetVisiblePlayer()));
	if ((playerInvolved || visibleEnemyUnit) && !TileWillBeCompletelyVisible(deadPos.x, deadPos.y) && g_theProfileDB->IsAutoCenter()) {
		AddCenterMap(deadPos);
	}
	dead->SetHiddenUnderStack(false);

	DQActionDeath * action = new DQActionDeath(dead->GetPlayerNum(), dead, deadPos, deadSoundID, killedBy);
	m_actionQueue->AddTail(action);
}

void DirectorImpl::AddMorphUnit(UnitActor *morphingActor, SpriteState *spriteState, sint32 type, const Unit &id)
{
	if (!morphingActor) {
		return;
	}

	DQActionMorph *action = new DQActionMorph(morphingActor, spriteState, type, id);
	m_actionQueue->AddTail(action);
}

void DirectorImpl::AddHide(const Unit &hider)
{
	UnitActor *actor = hider.GetActor();
	Assert(actor);
	if (!actor) {
		return;
	}

	DQActionHideShow *action = new DQActionHide(actor);
	m_actionQueue->AddTail(action);
}

void DirectorImpl::AddShow(const Unit &hider)
{
	UnitActor *actor = hider.GetActor();
	Assert(actor);
	if (!actor) {
		return;
	}

	DQActionHideShow *action = new DQActionShow(actor, hider.RetPos());
	m_actionQueue->AddTail(action);
}

void DirectorImpl::AddWork(const Unit &worker)
{
	if (!worker.GetActor()) {
		return;
	}

	DQActionWork *action = new DQActionWork(
			worker.GetOwner(), worker.GetActor(), worker.RetPos(), worker.GetWorkSoundID());
	m_actionQueue->AddTail(action);
}

void DirectorImpl::AddFastKill(UnitActor *dead)
{
	Assert(dead);
	if (!dead) {
		return;
	}

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
	if (!actor) {
		return;
	}

	DQActionSetVisibility *action = new DQActionSetVisibility(actor, visibility);
	m_actionQueue->AddTail(action);
}

void DirectorImpl::AddSetOwner(UnitActor *actor, sint32 owner)
{
	Assert(actor);
	if (!actor) {
		return;
	}

	DQActionSetOwner *action = new DQActionSetOwner(actor, owner);
	m_actionQueue->AddTail(action);
}

void DirectorImpl::AddBattle(Battle *battle)
{
	if (!battle) {
		return;
	}

	DQActionBattle *action = new DQActionBattle(battle);
	m_actionQueue->AddTail(action);
}

void DirectorImpl::AddPlaySound(sint32 soundID, const MapPoint &pos)
{
	if (soundID <= 0) {
		return;
	}

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
	if (which < 0) {
		return;
	}

	DQActionPlayWonderMovie *action = new DQActionPlayWonderMovie(which);
	m_actionQueue->AddTail(action);
}

void DirectorImpl::AddPlayVictoryMovie(GAME_OVER reason, bool previouslyWon, bool previouslyLost)
{
	if (previouslyWon || previouslyLost)
	{
		PLAYER_INDEX player = g_selected_item->GetVisiblePlayer();
		if (g_player[player] && !g_player[player]->m_isDead) {
			return;
		}
	}

	DQActionPlayVictoryMovie *action = new DQActionPlayVictoryMovie(reason);
	m_actionQueue->AddTail(action);
}

void DirectorImpl::AddMessage(const Message &message)
{
	DQActionMessage *action = new DQActionMessage(message);
	m_actionQueue->AddTail(action);
}

void DirectorImpl::AddFaceoff(const Unit &attacker, const Unit &defender)
{
	if (!attacker.IsValid() || !attacker.GetActor() || !defender.IsValid() || !defender.GetActor()) {
		return;
	}

	DQActionFaceoff *action = new DQActionFaceoff(
			attacker.GetOwner(), attacker.GetActor(), attacker.RetPos(), defender.GetActor(), defender.RetPos());
	m_actionQueue->AddTail(action);
}

void DirectorImpl::AddTerminateFaceoff(const Unit &faceroffer)
{
	if (!faceroffer->GetActor()) {
		return;
	}

	DQActionTerminateFaceOff *action = new DQActionTerminateFaceOff(faceroffer->GetActor());
	m_actionQueue->AddTail(action);
}

void DirectorImpl::AddTerminateSound(const Unit &unit)
{
	if (!unit.IsValid()) {
		return;
	}

	DQActionTerminateSound *action = new DQActionTerminateSound(unit.m_id);
	m_actionQueue->AddTail(action);
}

void DirectorImpl::AddInvokeThroneRoom()
{
	DQActionInvokeThroneRoom *action = new DQActionInvokeThroneRoom;
	m_actionQueue->AddTail(action);
}

void DirectorImpl::AddInvokeResearchAdvance(MBCHAR *message)
{
	if (!message) {
		return;
	}

	DQActionInvokeResearchAdvance *action = new DQActionInvokeResearchAdvance(message);
	m_actionQueue->AddTail(action);
}

void DirectorImpl::AddBeginScheduler(sint32 player)
{
	Assert(player == g_selected_item->GetCurPlayer());
	if (player != g_selected_item->GetCurPlayer()) {
		return;
	}

	DPRINTF(k_DBG_GAMESTATE, ("Director::AddBeginScheduler(%d)\n", player));

	DQActionBeginScheduler *action = new DQActionBeginScheduler(player);
	m_actionQueue->AddTail(action);
}


bool DirectorImpl::TileWillBeCompletelyVisible(sint32 x, sint32 y)
{
	if (m_latestCenterMap) {
		return m_latestCenterMap->TileIsCompletelyVisible(x, y);
	}
	return g_tiledMap->TileIsCompletelyVisible(x, y);
}
