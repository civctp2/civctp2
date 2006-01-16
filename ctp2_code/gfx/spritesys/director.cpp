//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : Handling of the action on the screen
// Id           : $Id:$
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
// - Otcommented some unreachable code. (Sep 9th 2005 Martin Gühmann)
//
//----------------------------------------------------------------------------


#include "c3.h"
#include "c3errors.h"

#include "tech_wllist.h"

#include "Globals.h"
#include "player.h"
#include "dynarr.h"
#include "UnitPool.h"

#include "pixelutils.h"
#include "spriteutils.h"
#include "tileutils.h"
#include "primitives.h"
#include "colorset.h"

#include "SelItem.h"

#include "SpriteState.h"
#include "Actor.h"
#include "Unit.h"

#include "radarmap.h"
#include "director.h"
#include "UnitActor.h"

#include "EffectActor.h"
#include "TradeActor.h"
#include "Action.h"
#include "Anim.h"
#include "maputils.h"
#include "tiledmap.h"

#include "battleviewwindow.h"
#include "network.h"
#include "soundmanager.h"
#include "network.h"
#include "debugmemory.h"
#include "TurnCnt.h"
#include "c3ui.h"

#include "wondermoviewin.h"
#include "victorymoviewin.h"
#include "messagewin.h"

#include "background.h"
#include "backgroundwin.h"

#include "screenutils.h"
#include "sci_advancescreen.h"
#include "UnitRecord.h"
#include "GameEventManager.h"
#include "gamesounds.h"
#include "network.h"
#include "net_info.h"
#include "UnitDynArr.h"
#include "SpriteRecord.h"

#include "cursormanager.h"

#include "citydata.h" //PFT 29 mar 05, show # turns until city next grows a pop
#include "UnitData.h" // "

#include "SpriteGroupList.h"
extern SpriteGroupList		*g_unitSpriteGroupList;

extern Director			*g_director;
extern double			g_ave_frame_rate;
extern double			g_ave_frame_time;
extern RadarMap			*g_radarMap;
extern TiledMap			*g_tiledMap;
extern UnitPool			*g_theUnitPool;
extern Background		*g_background;
extern ColorSet			*g_colorSet;

extern SelectedItem		*g_selected_item;
extern SoundManager		*g_soundManager;

extern sint32			g_fog_toggle;
extern sint32			g_god;
extern TurnCount		*g_turn;
extern C3UI				*g_c3ui;
extern BattleViewWindow	*g_battleViewWindow;
extern Player			**g_player;

#include "profileDB.h"
extern ProfileDB		*g_theProfileDB;

#include "ArmyPool.h"
extern ArmyPool			*g_theArmyPool;

#include "Cell.h"
#include "cellunitlist.h"
#include "World.h"
extern World			*g_theWorld;

#include "MessagePool.h"
extern MessagePool		*g_theMessagePool;

#ifdef _DEBUG
extern BOOL				g_doingFastRounds;
#endif


BOOL					g_zoomedBattlefieldOn = FALSE;
BOOL					g_useHandler = FALSE;

DQItem::DQItem(DQITEM_TYPE type, DQAction *action, DQHandler *handler)
:
m_type(type),
m_addedToSavedList(FALSE),
m_owner(-1),
m_action(action),
m_handler(handler)
{
	m_sequence = g_director->NewSequence();
	m_sequence->SetItem(this);

	m_round = (uint16)g_turn->GetRound();
}

DQItem::~DQItem()
{
	//DPRINTF(k_DBG_GAMESTATE, ("Deleting item @ %lx, type=%d\n", this, m_type));
	if (m_sequence)
		delete m_sequence;

	if (m_action)
		delete m_action;
}

#define k_MAX_DISPATCHED_QUEUE_ITEMS		1000
#define k_MAX_SAVED_SEQUENCES				1000

Director::Director(void)
{
	m_activeUnitList = new tech_WLList<UnitActor *>;

	m_activeEffectList = new tech_WLList<EffectActor *>;
	m_tradeActorList = new tech_WLList<TradeActor *>;

	
	m_masterCurTime = 0;

	

	m_nextPlayer = FALSE;
	m_paused = FALSE;

	m_lastTickCount = 0;
	m_timeLogIndex = 0;
	m_averageElapsed = 0;
	m_averageFPS = k_DEFAULT_FPS;

	m_processingActiveUnits = FALSE;
	m_processingActiveEffects = FALSE;

	m_itemQueue = new PointerList<DQItem>;


	m_dispatchedItems = new PointerList<DQItem>;
	
	m_curSequenceID = 0;
	m_lastSequenceID = 0;
	

	m_savedItems = new PointerList<DQItem>;

	m_itemWalker = new PointerList<DQItem>::Walker;

	m_actionFinished = TRUE;
	m_holdSchedulerSequence = NULL;
	m_pendingGameActions = 0;
	m_endTurnRequested = false;
}


Director::~Director(void)
{
	if (m_dispatchedItems)
		delete m_dispatchedItems;

	if (m_itemQueue)
		delete m_itemQueue;

	if (m_savedItems)
		delete m_savedItems;

	if (m_itemWalker)
		delete m_itemWalker;

	if (m_activeUnitList) delete m_activeUnitList;
	m_activeUnitList = NULL;




	if (m_activeEffectList) delete m_activeEffectList;
		m_activeEffectList = NULL;

	if (m_tradeActorList) delete m_tradeActorList;
	m_tradeActorList = NULL;
	
	m_masterCurTime = 0;
}

void Director::Kill(UnitActor *actor)
{
	ListPos		pos;

	
	actor->DumpAllActions();

	pos = m_activeUnitList->Find(actor, m_activeUnitList->GetHeadPosition());
	if (pos) 
	{
		if (m_processingActiveUnits) {
			actor->SetKillNow();
		} else {
			m_activeUnitList->DeleteAt(pos);
			delete actor;
			actor = NULL;
		}
	}
}


void Director::FastKill(UnitActor *actor)
{
	ListPos		pos;

	
	actor->DumpAllActions();

	pos = m_activeUnitList->Find(actor, m_activeUnitList->GetHeadPosition());
	if (pos) 
	{
		if (m_processingActiveUnits) {
			actor->SetKillNow();
		} else {
			m_activeUnitList->DeleteAt(pos);
			delete actor;
			actor = NULL;
		}
	} else {
		
		delete actor;
		actor = NULL;
	}
}
















void Director::FastKill(EffectActor *actor)
{
	ListPos		pos;

	pos = m_activeEffectList->Find(actor, m_activeEffectList->GetHeadPosition());
	if (pos) 
	{
		if (m_processingActiveEffects) {
			actor->SetKillNow();
		} else {
			m_activeEffectList->DeleteAt(pos);
			delete actor;
			actor = NULL;
		}
	}

}

void Director::UpdateTimingClock(void)
{
	sint32				elapsed;

	if (m_lastTickCount == 0) {
		elapsed = 1000/k_DEFAULT_FPS;
	} else {
		elapsed = GetTickCount() - m_lastTickCount;
	}

	
	
	if (elapsed > k_ELAPSED_CEILING)
		elapsed = k_ELAPSED_CEILING;

	m_lastTickCount = GetTickCount();

	
	m_timeLog[m_timeLogIndex] = elapsed;
	m_timeLogIndex++;

	
	if (m_timeLogIndex >= k_TIME_LOG_SIZE) {
		
		sint32		timeSum=0;
		for (sint32 i=0; i<k_TIME_LOG_SIZE; i++) 
			timeSum += m_timeLog[i];

		
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

void Director::Process(void)
{
	UpdateTimingClock();

	static uint32 nextTime = 0;

	if (GetTickCount() > nextTime) {
		
		
		
		
		HandleNextAction();

		

		ProcessActiveUnits();
		ProcessActiveEffects();

		ProcessTradeRouteAnimations();
		if(g_tiledMap)
			g_tiledMap->ProcessLayerSprites(g_tiledMap->GetMapViewRect(), 0);

		nextTime = GetTickCount() + 75;	
	}
}

void Director::PauseDirector(BOOL pause)
{
	m_paused = pause;
}

#ifdef _DEBUG

void Director::DumpItem(DQItem *item)
{
	switch(item->m_type) {
	case DQITEM_MOVE: {
		DQActionMove *action = (DQActionMove *)item->m_action;

		DPRINTF(k_DBG_UI, ("Move\n"));
		DPRINTF(k_DBG_UI, ("  move_actor         :%#.8lx\n", action->move_actor));
		DPRINTF(k_DBG_UI, ("  move_oldPos        :%d,%d\n", action->move_oldPos.x, action->move_oldPos.y));
		DPRINTF(k_DBG_UI, ("  move_newPos        :%d,%d\n", action->move_newPos.x, action->move_newPos.y));
		DPRINTF(k_DBG_UI, ("  moveArraySize      :%d\n", action->moveArraySize));
		DPRINTF(k_DBG_UI, ("  moveActors         :"));
		if (action->moveArraySize > 0) {
			for (sint32 i=0; i<action->moveArraySize; i++) {
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
	case DQITEM_MOVEPROJECTILE:{
		DQActionMoveProjectile *action = (DQActionMoveProjectile *)item->m_action;

		DPRINTF(k_DBG_UI, ("Move Projectile\n"));
		DPRINTF(k_DBG_UI, ("  pshooting_actor    :%#.8lx\n", action->pshooting_actor));
		DPRINTF(k_DBG_UI, ("  ptarget_actor      :%#.8lx\n", action->ptarget_actor));
		DPRINTF(k_DBG_UI, ("  pmove_oldPos       :%d,%d\n", action->pmove_oldPos.x, action->pmove_oldPos.y));
		DPRINTF(k_DBG_UI, ("  pmove_newPos       :%d,%d\n", action->pmove_newPos.x, action->pmove_newPos.y));
		DPRINTF(k_DBG_UI, ("  move_projectile    :%#.8lx\n", action->move_projectile));
		DPRINTF(k_DBG_UI, ("  end_projectile     :%#.8lx\n", action->end_projectile));
		DPRINTF(k_DBG_UI, ("  projectile_path    :%d\n", action->projectile_path));
	  }
		break;
	case DQITEM_SPECEFFECT : {
		DQActionSpecialEffect *action = (DQActionSpecialEffect *)item->m_action;

		DPRINTF(k_DBG_UI, ("Special Effect\n"));
		DPRINTF(k_DBG_UI, ("  speceffect_pos       :%d,%d\n", action->speceffect_pos.x, action->speceffect_pos.y));
		DPRINTF(k_DBG_UI, ("  speceffect_spriteID    :%d\n", action->speceffect_spriteID));
		DPRINTF(k_DBG_UI, ("  speceffect_soundID    :%d\n", action->speceffect_soundID));
		}
		break;
	case DQITEM_ATTACK: {
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
	case DQITEM_ATTACKPOS: {
		DQActionAttackPos *action = (DQActionAttackPos *)item->m_action;

		DPRINTF(k_DBG_UI, ("Attack Pos\n"));
		DPRINTF(k_DBG_UI, ("  attackpos_attacker     :%#.8lx\n", action->attackpos_attacker));
		DPRINTF(k_DBG_UI, ("  attackpos_attacker_pos :%d,%d\n", action->attackpos_attacker_pos.x, action->attackpos_attacker_pos.y));
		DPRINTF(k_DBG_UI, ("  attackpos_target_pos   :%d,%d\n", action->attackpos_target_pos.x, action->attackpos_target_pos.y));
		DPRINTF(k_DBG_UI, ("  attackpos_soundID      :%d\n", action->attackpos_soundID));
	  }
		break;
	case DQITEM_SPECATTACK: {
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
	case DQITEM_DEATH: {
		DQActionDeath *action = (DQActionDeath *)item->m_action;

		DPRINTF(k_DBG_UI, ("Death\n"));
		DPRINTF(k_DBG_UI, ("  death_dead         :%#.8lx\n", action->death_dead));
		DPRINTF(k_DBG_UI, ("  death_victor       :%#.8lx\n", action->death_victor));
		DPRINTF(k_DBG_UI, ("  victor_Pos         :%d,%d\n", action->victor_Pos.x, action->victor_Pos.y));
		DPRINTF(k_DBG_UI, ("  dead_Pos           :%d,%d\n", action->dead_Pos.x, action->dead_Pos.y));
		DPRINTF(k_DBG_UI, ("  dead_soundID       :%d\n", action->dead_soundID));
		DPRINTF(k_DBG_UI, ("  victor_soundID     :%d\n", action->victor_soundID));
	  }
		break;
	case DQITEM_MORPH: {
		DQActionMorph *action = (DQActionMorph *)item->m_action;

		DPRINTF(k_DBG_UI, ("Morph\n"));
		DPRINTF(k_DBG_UI, ("  morphing_actor     :%#.8lx\n", action->morphing_actor));
		DPRINTF(k_DBG_UI, ("  ss                 :%#.8lx (%d)\n", action->ss, action->ss->GetIndex()));
		DPRINTF(k_DBG_UI, ("  type               :%d\n", action->type));
		DPRINTF(k_DBG_UI, ("  id                 :%#.8lx\n", action->id.m_id));
	  }
		break;
	case DQITEM_HIDE: {
		DQActionHideShow *action = (DQActionHideShow *)item->m_action;
		
		DPRINTF(k_DBG_UI, ("Hide\n"));
		DPRINTF(k_DBG_UI, ("  hiding_actor       :%#.8lx\n", action->hiding_actor));
		DPRINTF(k_DBG_UI, ("  hiding_pos         :%d,%d\n", action->hiding_pos.x, action->hiding_pos.y));
	  }
		break;
	case DQITEM_SHOW: {
		DQActionHideShow *action = (DQActionHideShow *)item->m_action;
		
		DPRINTF(k_DBG_UI, ("Show\n"));
		DPRINTF(k_DBG_UI, ("  hiding_actor       :%#.8lx\n", action->hiding_actor));
		DPRINTF(k_DBG_UI, ("  hiding_pos         :%d,%d\n", action->hiding_pos.x, action->hiding_pos.y));
	  }
		break;
	case DQITEM_WORK: {
		DQActionWork *action = (DQActionWork *)item->m_action;
		
		DPRINTF(k_DBG_UI, ("Work\n"));
		DPRINTF(k_DBG_UI, ("  working_actor      :%#.8lx\n", action->working_actor));
		DPRINTF(k_DBG_UI, ("  working_pos        :%d,%d\n", action->working_pos.x, action->working_pos.y));
		DPRINTF(k_DBG_UI, ("  working_soundID    :%d\n", action->working_soundID));
	  }
		break;
	case DQITEM_FASTKILL: {
		DQActionFastKill *action = (DQActionFastKill *)item->m_action;
		
		DPRINTF(k_DBG_UI, ("Fast Kill\n"));
		DPRINTF(k_DBG_UI, ("  dead               :%#.8lx\n", action->dead));
	  }
		break;
	case DQITEM_ADDVISION: {
		DQActionVision *action = (DQActionVision *)item->m_action;
		
		DPRINTF(k_DBG_UI, ("Add Vision\n"));
		DPRINTF(k_DBG_UI, ("  vision_pos         :%d,%d\n", action->vision_pos.x, action->vision_pos.y));
		DPRINTF(k_DBG_UI, ("  vision_range       :%#.2f\n", action->vision_range));
	  }
		break;
	case DQITEM_REMOVEVISION: {
		DQActionVision *action = (DQActionVision *)item->m_action;
		
		DPRINTF(k_DBG_UI, ("Remove Vision\n"));
		DPRINTF(k_DBG_UI, ("  vision_pos         :%d,%d\n", action->vision_pos.x, action->vision_pos.y));
		DPRINTF(k_DBG_UI, ("  vision_range       :%#.2f\n", action->vision_range));
	  }
		break;
	case DQITEM_SETOWNER: {
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
		DQActionCopyVision *action = (DQActionCopyVision *)item->m_action;
		
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
		DQActionEndTurn *action = (DQActionEndTurn *)item->m_action;
		
		DPRINTF(k_DBG_UI, ("End Turn from Gamestate\n"));
	  }
		break;
	case DQITEM_BATTLE : {
		DQActionBattle *action = (DQActionBattle *)item->m_action;
		
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
		DQActionMessage *action = (DQActionMessage *)item->m_action;
		
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

void Director::DumpInfo(void)
{
	sint32		i;

	DPRINTF(k_DBG_UI, (" ------------------\n"));		
	DPRINTF(k_DBG_UI, ("Director Dump:\n"));		

	DPRINTF(k_DBG_UI, (" m_curSequenceID  :%d\n", m_curSequenceID));		
	DPRINTF(k_DBG_UI, (" m_lastSequenceID :%d\n", m_lastSequenceID));		

	DPRINTF(k_DBG_UI, (" ------------------\n"));		
	DPRINTF(k_DBG_UI, (" Dispatched Items:\n"));		
	DPRINTF(k_DBG_UI, (" Count:%d\n", m_dispatchedItems->GetCount()));		
	DPRINTF(k_DBG_UI, (" ------------------\n"));		
	
	m_itemWalker->SetList(m_dispatchedItems);
	while (m_itemWalker->IsValid()) {
		DQItem *item = m_itemWalker->GetObj();

		DumpItem(item);

		m_itemWalker->Next();
	}







	
	DPRINTF(k_DBG_UI, (" ------------------\n"));		
	DPRINTF(k_DBG_UI, (" Active Units:\n"));		
	DPRINTF(k_DBG_UI, (" Count:%d\n", m_activeUnitList->L()));		
	DPRINTF(k_DBG_UI, (" ------------------\n"));		
	ListPos		pos;
	UnitActor	*actor;
	pos = m_activeUnitList->GetHeadPosition();
	for (i=0; i<(sint32)m_activeUnitList->L(); i++) {
		actor = m_activeUnitList->GetNext(pos);
		actor->DumpActor();
	}
	DPRINTF(k_DBG_UI, (" ------------------\n"));		

	DPRINTF(k_DBG_UI, (" Queued Items:\n"));		
	DPRINTF(k_DBG_UI, (" Count:%d\n", m_itemQueue->GetCount()));		
	DPRINTF(k_DBG_UI, (" ------------------\n"));		
	m_itemWalker->SetList(m_itemQueue);
	while (m_itemWalker->IsValid()) {
		DQItem	*item = m_itemWalker->GetObj();

		DumpItem(item);

		m_itemWalker->Next();
	}
































	DPRINTF(k_DBG_UI, (" ------------------\n"));		
}
#endif

void Director::HandleNextAction(void)
{
	DQItem			*item;
	static DQItem	*lastItem;

	if (m_paused) return;

	
	
	while (m_itemQueue->GetCount() > 0 && GetActionFinished()) {
		
		item = m_itemQueue->RemoveHead();

		Assert(item->m_handler != NULL);
		if (item->m_handler != NULL) 
		{
			
			SetActionFinished(FALSE);

			
			
			
			
			m_dispatchedItems->AddTail(item);

			
			





			lastItem = item;

			DHEXECUTE		executeType = DHEXECUTE_NORMAL;

			if ((item->m_round < g_turn->GetRound() - 1) ||
				(!g_theProfileDB->IsEnemyMoves() && item->GetOwner() != -1 && 
					item->GetOwner() != g_selected_item->GetVisiblePlayer())) {
				
				executeType = DHEXECUTE_IMMEDIATE;
			}

			item->m_handler(item->m_action, item->m_sequence, executeType);

			
			if (item->m_type == DQITEM_ADDVISION ||
				item->m_type == DQITEM_REMOVEVISION)
				HandleNextAction();
		}
	}
}

void Director::ActionFinished(Sequence *seq)
{
	static DQItem	*lastItem;

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
		sint32 NoDispatchedItem = 0;


		return;
	}

	if (!item->m_sequence) {
#ifdef _DEBUG
	DPRINTF(k_DBG_UI, (" ------------------\n"));		
	DPRINTF(k_DBG_UI, ("*** Null Sequence Finished\n"));		
	DumpItem(item);
	DPRINTF(k_DBG_UI, (" ------------------\n"));		
#endif
		return;
	}

	if (item->m_sequence->GetSequenceID() == seq->GetSequenceID()) {
		m_lastSequenceID = seq->GetSequenceID();
		seq->Release();
		if (seq->GetRefCount() <= 0) {
			SaveFinishedItem(item);
			SetActionFinished(TRUE);
		}
	} else {
		if (item->m_sequence->GetSequenceID() < seq->GetSequenceID()) {
			
			
			
			sint32 OutOfOrder_TooEarly = 0;


			m_lastSequenceID = seq->GetSequenceID();
			DQItem *temp = m_dispatchedItems->RemoveHead();
			delete temp;
			SetActionFinished(TRUE);
		} else {
			
			
			sint32 OutOfOrder_SequenceSkipped = 0;


			SetActionFinished(TRUE);
		}
	}

	lastItem = item;
}






Sequence *Director::NewSequence(void)
{
	Sequence *seq = new Sequence(++m_curSequenceID);

	return seq;
}












void Director::HandleFinishedItem(DQItem *item)
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

	switch (item->m_type) {
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
	case DQITEM_MOVEPROJECTILE:
		break;
	case DQITEM_SPECEFFECT:
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
	case DQITEM_ATTACK:
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
			if (action && action->death_dead) {
				ActiveUnitRemove(action->death_dead);
				if (!action->death_dead->IsActive())
					g_soundManager->TerminateLoopingSound(SOUNDTYPE_SFX, action->dead_id);
			}
		}
		if (removeSecondaryFromActiveList) {
			if (action && action->death_victor) {
				ActiveUnitRemove(action->death_victor);
				if (!action->death_victor->IsActive())
					g_soundManager->TerminateLoopingSound(SOUNDTYPE_SFX, action->victor_id);
			}
		}
		}
		break;
	case DQITEM_MORPH:
		break;
	case DQITEM_HIDE:
		break;
	case DQITEM_SHOW:
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
	case DQITEM_FASTKILL:
		break;
	case DQITEM_ADDVISION:
		break;
	case DQITEM_REMOVEVISION:
		break;
	case DQITEM_SETOWNER:
		break;
	case DQITEM_SETVISIBILITY:
		break;
	case DQITEM_SETVISIONRANGE:
		break;
	case DQITEM_COMBATFLASH:
		break;
	case DQITEM_TELEPORT:
		break;
	case DQITEM_COPYVISION:
		break;
	case DQITEM_CENTERMAP:
		break;
	case DQITEM_SELECTUNIT:
		break;
	case DQITEM_ENDTURN:
		break;
	case DQITEM_BATTLE:
		break;
	case DQITEM_PLAYSOUND:
		break;
	case DQITEM_PLAYWONDERMOVIE:
		break;
	case DQITEM_PLAYVICTORYMOVIE:
		break;
	case DQITEM_MESSAGE:
		break;
	case DQITEM_FACEOFF:
		break;
	case DQITEM_TERMINATE_FACEOFF:
		break;
	}

	delete item;
}









void Director::SaveFinishedItem(DQItem *item)
{


	if (item->m_addedToSavedList) return;

	item->m_addedToSavedList = TRUE;

	m_savedItems->AddTail(item);
}








void Director::GarbageCollectItems(void)
{
	while (m_savedItems->GetCount() > 0) {
		DQItem		*item = m_savedItems->RemoveHead();
		HandleFinishedItem(item);
	}
}



void Director::ProcessImmediately(DQItem *item)
{
	item->m_handler(item->m_action, item->m_sequence, DHEXECUTE_IMMEDIATE);
}
 


void Director::CatchUp(void)
{
	
	
	ListPos pos, actorPos;
	UnitActor *actor;

	pos = m_activeUnitList->GetHeadPosition();

	if (pos) {
		do {
			actorPos = pos;
			actor = m_activeUnitList->GetNext(pos);

			if (actor) {
				if (actor->WillDie()) {
					m_activeUnitList->DeleteAt(actorPos);
					delete actor;
				} else {
					actor->EndTurnProcess();
				}
			}
		} while (pos);
	}

	
	while(m_itemQueue->GetCount()) {
		DQItem *item;
		item = m_itemQueue->RemoveHead();

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





BOOL Director::CaughtUp(void)
{
	if (!m_itemQueue) return TRUE;

	return (m_itemQueue->GetCount() == 0);
}




BOOL Director::TileIsVisibleToPlayer(MapPoint &pos)
{
#ifdef _DEBUG
	if (g_doingFastRounds) return FALSE;
	if (g_god) return TRUE;
	if (g_fog_toggle) return TRUE;
	if(!g_tiledMap) return FALSE;



	return g_tiledMap->GetLocalVision()->IsVisible(pos);
#else
	if (g_god) return TRUE;
	return g_tiledMap->GetLocalVision()->IsVisible(pos);
#endif
}



void Director::ActiveUnitAdd(UnitActor *unitActor)
{
	if(unitActor==NULL)
		return;

	if(!unitActor->IsActive())
		m_activeUnitList->AddHead(unitActor);
	
	unitActor->SetActive(true);

	unitActor->AddActiveListRef();

















}



void Director::ActiveUnitRemove(UnitActor *unitActor)
{
	if(unitActor==NULL)
	  return;

	ListPos		pos;

	pos = m_activeUnitList->Find(unitActor, m_activeUnitList->GetHeadPosition());
	
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




























void Director::ActiveEffectAdd(EffectActor *effectActor)
{
	ListPos		pos;

	Assert(effectActor);

	
	Assert((uint32)effectActor > (uint32)0x00010000);

	pos = m_activeEffectList->Find(effectActor, m_activeEffectList->GetHeadPosition());
	if(!pos)
		pos = m_activeEffectList->AddHead(effectActor);
}



void Director::ActiveEffectRemove(EffectActor *effectActor)
{
	ListPos		pos;

	pos = m_activeEffectList->Find(effectActor, m_activeEffectList->GetHeadPosition());
	if (pos) 
	{
		m_activeEffectList->DeleteAt(pos);
	} 
}



void Director::TradeActorCreate(TradeRoute newRoute)
{
	ListPos			pos, foundPos;
	TradeActor		*tActor;

	foundPos = pos = m_tradeActorList->GetHeadPosition();
	for (uint32 i=0; i<m_tradeActorList->L(); i++) 
	{
		tActor = m_tradeActorList->GetNext(pos);
		
		Assert(tActor != NULL);
		if(tActor->GetRouteID() == newRoute)
			break;
		else
			foundPos = pos;
	}
	
	
	if(!foundPos)
	{
		tActor = new TradeActor(newRoute);
		pos = m_tradeActorList->AddHead(tActor);
	}
}

void Director::TradeActorDestroy(TradeRoute routeToDestroy)
{
	ListPos			pos, foundPos;
	TradeActor		*tActor = NULL;

	foundPos = pos = m_tradeActorList->GetHeadPosition();
	for (uint32 i=0; i<m_tradeActorList->L(); i++) 
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
BOOL Director::IsActive(UnitActor *unitActor)
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
uint32 Director::ProcessActiveUnits(void)
{
	ListPos			pos, actorPos;
	UnitActor		*actor;

	if (m_activeUnitList->IsEmpty()) return 0;

	m_processingActiveUnits = TRUE;

	
	pos = m_activeUnitList->GetHeadPosition();

	for (sint32 i=m_activeUnitList->L(); i; i--) 
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
	for (size_t i=m_activeUnitList->L(); i; i--) {
		actorPos = pos;
		actor = m_activeUnitList->GetNext(pos);
		if (actor) {
			if (actor->GetKillNow())
			{
				actor->DumpAllActions();
				m_activeUnitList->DeleteAt(actorPos);
				delete actor;
			}
		}
	}

	return 0;
}































uint32 Director::ProcessActiveEffects(void)
{
	ListPos				pos, actorPos;
	EffectActor			*actor;

	if (m_activeEffectList->IsEmpty()) return 0;

	m_processingActiveEffects = TRUE;

	pos = m_activeEffectList->GetHeadPosition();
	for (sint32 i=m_activeEffectList->L(); i; i--) {
		actorPos = pos;

		actor = m_activeEffectList->GetNext(pos);
		if (actor) {
			actor->Process();
		}
	}

	m_processingActiveEffects = FALSE;

	pos = m_activeEffectList->GetHeadPosition();
	for (size_t i=m_activeEffectList->L(); i; i--) {
		actorPos = pos;

		actor = m_activeEffectList->GetNext(pos);
		if (actor && actor->GetKillNow()) {
			m_activeEffectList->DeleteAt(actorPos);
			delete actor;
		}
	}

	return 0;
}

void Director::ProcessTradeRouteAnimations(void)
{
	ListPos			pos;
	TradeActor		*tActor;
	size_t			numToProcess;

	if (!g_theProfileDB->IsTradeAnim()) return;

	if (m_tradeActorList->IsEmpty()) return;

	pos = m_tradeActorList->GetHeadPosition();
	numToProcess = m_tradeActorList->L();
	for (size_t i=0; i<numToProcess; i++) 
	{
		tActor = m_tradeActorList->GetNext(pos);
		Assert(tActor != NULL);
		if (tActor != NULL)
		{
			
			tActor->Process();
		}
	}
}

void Director::OffsetActiveUnits(sint32 deltaX, sint32 deltaY)
{
	ListPos			pos;
	UnitActor		*actor;
	uint32			numToProcess;

	pos = m_activeUnitList->GetHeadPosition();
	numToProcess = m_activeUnitList->L();
	for (uint32 i=0; i<numToProcess; i++) {
		actor = m_activeUnitList->GetNext(pos);
		Assert(actor != NULL);
		if (actor != NULL) {
			actor->SetX(actor->GetX() + deltaX);
			actor->SetY(actor->GetY() + deltaY);
		}
	}
}






















void Director::OffsetActiveEffects(sint32 deltaX, sint32 deltaY)
{
	ListPos				pos;
	EffectActor			*actor;
	uint32				numToProcess;

	pos = m_activeEffectList->GetHeadPosition();
	numToProcess = m_activeEffectList->L();
	for (uint32 i=0; i<numToProcess; i++) {
		actor = m_activeEffectList->GetNext(pos);
		Assert(actor != NULL);
		if (actor != NULL) {
			actor->SetX(actor->GetX() + deltaX);
			actor->SetY(actor->GetY() + deltaY);
		}
	}
}

void Director::OffsetTradeRouteAnimations(sint32 deltaX, sint32 deltaY)
{
	ListPos			pos;
	TradeActor		*tActor;
	uint32			numToProcess;

	pos = m_tradeActorList->GetHeadPosition();
	numToProcess = m_tradeActorList->L();
	for (uint32 i=0; i<numToProcess; i++) 
	{
		tActor = m_tradeActorList->GetNext(pos);
		Assert(tActor != NULL);
		if (tActor != NULL)
		{
			tActor->SetX(tActor->GetX() + deltaX);
			tActor->SetY(tActor->GetY() + deltaY);
		}
	}
}
























uint32 Director::KillAllActiveEffects()
{
	ListPos				pos, actorPos;
	EffectActor			*actor;

	if (m_activeEffectList->IsEmpty()) return 0;

	pos = m_activeEffectList->GetHeadPosition();

	do {
		actorPos = pos;
		actor = m_activeEffectList->GetNext(pos);

		if (actor) {
			m_activeEffectList->DeleteAt(actorPos);
			delete actor;
		}
	} while (pos);

	return 0;
}
 
void Director::NextPlayer(BOOL forcedUpdate)
{ 
#ifdef _DEBUG
	if (!g_doingFastRounds &&
		(!g_network.IsActive() || (g_player[g_selected_item->GetVisiblePlayer()]->m_playerType == PLAYER_TYPE_ROBOT))) {
		return;
	}
#else
	return; // Next code isn't used, should it be used?
#endif
	m_nextPlayer = TRUE; 
	

	
	
	ListPos pos, actorPos;
	UnitActor *actor;

	pos = m_activeUnitList->GetHeadPosition();

	if (pos) {
		do {
			actorPos = pos;
			actor = m_activeUnitList->GetNext(pos);

			if (actor && (!g_network.IsActive() || forcedUpdate)) {
				if (actor->WillDie()) {
					m_activeUnitList->DeleteAt(actorPos);
					delete actor;
				} else {
					actor->EndTurnProcess();
				}
			}
		} while (pos);
	}

	
	while(m_itemQueue->GetCount()) {
		DQItem *item;
		item = m_itemQueue->RemoveHead();

		Assert(item->m_handler != NULL);
		if (item->m_handler != NULL) 
		{
			
			
			
			
			m_dispatchedItems->AddTail(item);

			
			





 			item->m_handler(item->m_action, item->m_sequence, DHEXECUTE_NORMAL);
		}
	}





























	pos = m_activeUnitList->GetHeadPosition();
	if (pos) {
		do {
			actorPos = pos;

			actor = m_activeUnitList->GetNext(pos);

			if (!g_network.IsActive() || forcedUpdate) {
				if (actor->WillDie()) {
					m_activeUnitList->DeleteAt(actorPos);
					delete actor;
				} else {
					actor->EndTurnProcess();
				}
			}
		} while (pos);
	}


	if (!g_network.IsActive()) {

		if(g_tiledMap)
			g_tiledMap->NextPlayer(); 


		KillAllActiveEffects();
	
		if (g_soundManager)
			g_soundManager->TerminateAllLoopingSounds(SOUNDTYPE_SFX);
	}
}


void Director::DrawActiveUnits(RECT *paintRect, sint32 layer)
{
	ListPos			pos;
	UnitActor		*actor;
	uint32			numToProcess;

	m_nextPlayer = FALSE;

	pos = m_activeUnitList->GetHeadPosition();
	numToProcess = m_activeUnitList->L();
	uint32 visPlayerBit = (1 << g_selected_item->GetVisiblePlayer());
	for (uint32 i=0; i<numToProcess; i++) 
	{
		actor = m_activeUnitList->GetNext(pos);
		Assert(actor != NULL);
		if (actor != NULL)
		{
			MapPoint pos = actor->GetPos();

			sint32		tileX;

			maputils_MapX2TileX(pos.x, pos.y, &tileX);

			if (!maputils_TilePointInTileRect(tileX, pos.y, paintRect))
				continue;

			if(( actor->GetUnitVisibility() & (1 << g_selected_item->GetVisiblePlayer())))
			{
				
							
				BOOL fog = FALSE;

				
				g_tiledMap->PaintUnitActor(actor, fog);

			}
			else
			{
				
				
			}
		}
	}
}





















































void Director::DrawActiveEffects(RECT *paintRect, sint32 layer)
{
	ListPos				pos;
	EffectActor			*actor;
	uint32				numToProcess;

	pos = m_activeEffectList->GetHeadPosition();
	numToProcess = m_activeEffectList->L();
	for (uint32 i=0; i<numToProcess; i++) 
	{
		actor = m_activeEffectList->GetNext(pos);
		Assert(actor != NULL);
		if (actor != NULL)
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

void Director::DrawTradeRouteAnimations(RECT *paintRect, sint32 layer)
{
	ListPos			pos;
	TradeActor		*tActor;
	uint32			numToProcess;
	RECT			tempRect;

	if (!g_theProfileDB->IsTradeAnim()) return;

	pos = m_tradeActorList->GetHeadPosition();
	numToProcess = m_tradeActorList->L();
	for (uint32 i=0; i<numToProcess; i++) 
	{
		tActor = m_tradeActorList->GetNext(pos);

		Assert(tActor != NULL);
		if (tActor != NULL)
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


UnitActor *Director::GetClickedActiveUnit(aui_MouseEvent *data)
{
	ListPos			pos;
	UnitActor		*actor;
	POINT			mouse;

	mouse = data->position;

	pos = m_activeUnitList->GetHeadPosition();
	for (uint32 i=0; i<m_activeUnitList->L(); i++) 
	{
		actor = m_activeUnitList->GetNext(pos);
		Assert(actor != NULL);
		if (actor != NULL)
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

	return NULL;
}


bool Director::IsProcessing()
{
	
	return (m_itemQueue->GetCount() > 0);
}


void Director::AddMoveProcess(UnitActor *top, UnitActor *dest, sint32 arraySize, UnitActor **moveActors, BOOL isTransported)
{
























}

void Director::AddMove(Unit &mover, MapPoint &oldPos, MapPoint &newPos, sint32 numRevealed, UnitActor **revealedActors,
					   sint32 numRest, UnitActor **restOfStack, BOOL isTransported, sint32 soundID)
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

	if (actor->GetIsTransported() == FALSE && isTransported == TRUE) {
		
		
		
		actor->SetIsTransported(k_TRANSPORTREMOVEONLY);
	} else
	if (actor->GetIsTransported() == k_TRANSPORTREMOVEONLY && isTransported == FALSE) {
		
		
		actor->SetIsTransported(k_TRANSPORTADDONLY);
	}

	DQActionMove		*action = new DQActionMove;
	DQItem				*item = new DQItem(DQITEM_MOVE, action, dh_move);
	
	item->SetOwner(mover.GetOwner());




	action->move_actor = actor;
	action->move_oldPos = oldPos;
	action->move_newPos = newPos;
	action->move_soundID = soundID;

	
	action->moveArraySize = numRest;
	if(numRest <= 0)
		action->moveActors = NULL;
	else
		action->moveActors = restOfStack;

	
	action->numRevealed = numRevealed;
	action->revealedActors = revealedActors;




	m_itemQueue->AddTail(item);
}

void Director::AddTeleport(Unit &top, MapPoint &oldPos, MapPoint &newPos, 
							sint32 numRevealed, UnitActor **revealedActors,
							sint32 arraySize, UnitActor **moveActors)
{
	DQActionMove		*action = new DQActionMove;
	DQItem				*item	= new DQItem(DQITEM_TELEPORT, action, dh_teleport);
	item->SetOwner(top.GetOwner());




	action->move_actor = top.GetActor();
	action->move_oldPos = oldPos;
	action->move_newPos = newPos;
	action->moveArraySize = arraySize;
	action->moveActors = moveActors;
	action->numRevealed = numRevealed;
	action->revealedActors = revealedActors;
	



	m_itemQueue->AddTail(item);
}

void Director::AddProjectileAttack(Unit shooting, Unit target, SpriteState *projectile_state, 
									SpriteState *projectileEnd_state, sint32 projectile_Path)
{
	DQActionMoveProjectile	*action = new DQActionMoveProjectile;
	DQItem					*item = new DQItem(DQITEM_MOVEPROJECTILE, action, dh_projectileMove);
	item->SetOwner(shooting.GetOwner());

    ProjectileActor *projectile = NULL;
    EffectActor *projectileEnd = new EffectActor(projectileEnd_state, target.RetPos());




	action->pshooting_actor = shooting.GetActor();
	action->ptarget_actor = target.GetActor();
	action->move_projectile = projectile;
	action->end_projectile = projectileEnd;
	action->projectile_path = projectile_Path;
	action->pmove_oldPos = shooting.RetPos();
	action->pmove_newPos = target.RetPos();




	m_itemQueue->AddTail(item);
}

void Director::AddSpecialEffect(MapPoint &pos, sint32 spriteID, sint32 soundID)
{
	DQActionSpecialEffect	*action = new DQActionSpecialEffect;
	DQItem					*item = new DQItem(DQITEM_SPECEFFECT, action, dh_speceffect);

	action->speceffect_pos = pos;
	action->speceffect_spriteID = spriteID;
	action->speceffect_soundID = soundID;

	m_itemQueue->AddTail(item);
}

void Director::AddCombatFlash(const MapPoint &pos)
{
	DQActionCombatFlash		*action = new DQActionCombatFlash;
	DQItem				*item = new DQItem(DQITEM_COMBATFLASH, action, dh_combatflash);

	action->flash_pos = pos;

	m_itemQueue->AddTail(item);
}

void Director::AddCopyVision(void)
{
	DQActionCopyVision	*action = new DQActionCopyVision;
	DQItem				*item = new DQItem(DQITEM_COPYVISION, action, dh_copyVision);



	action->copyVision = TRUE;





	m_itemQueue->AddTail(item);
}

void Director::AddCenterMap(const MapPoint &pos)
{
	DQActionCenterMap	*action = new DQActionCenterMap;
	DQItem				*item = new DQItem(DQITEM_CENTERMAP, action, dh_centerMap);




	action->centerMap_pos = pos;





	m_itemQueue->AddTail(item);
}

void Director::AddSelectUnit(uint32 flags)
{
	DQActionUnitSelection	*action = new DQActionUnitSelection;
	DQItem				*item = new DQItem(DQITEM_SELECTUNIT, action, dh_selectUnit);



	action->flags = flags;




	m_itemQueue->AddTail(item);
}

void Director::AddEndTurn(void)
{	
	DPRINTF(k_DBG_GAMESTATE, ("Director::AddEndTurn, curPlayer = %d\n", g_selected_item->GetCurPlayer()));
	
	if(g_selected_item->GetCurPlayer() == g_selected_item->GetVisiblePlayer()) { 
		static sint32 last_turn_processed = -1;
		if(last_turn_processed != g_player[g_selected_item->GetCurPlayer()]->m_current_round) {
			last_turn_processed = g_player[g_selected_item->GetCurPlayer()]->m_current_round;

			
			g_gevManager->Pause();

			Player *p = g_player[g_selected_item->GetCurPlayer()];
			p->m_endingTurn = TRUE;  
			
			sint32 i;
			for(i = 0; i < p->m_all_armies->Num(); i++) {
				IncrementPendingGameActions();
				g_gevManager->AddEvent(GEV_INSERT_Tail, GEV_BeginTurnExecute,
									   GEA_Army, p->m_all_armies->Access(i).m_id,
									   GEA_End);
			}
			
			
			

			g_player[g_selected_item->GetCurPlayer()]->m_endingTurn = FALSE;
			g_gevManager->Resume();
		}
	}

	if(m_pendingGameActions > 0 || m_endTurnRequested) {
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
	if(g_player[lastPlayer]) {
		lastRound = g_player[lastPlayer]->m_current_round;
	} else {
		lastRound = -1;
	}

	DQActionEndTurn		*action = new DQActionEndTurn;
	DQItem				*item = new DQItem(DQITEM_ENDTURN, action, dh_endTurn);

	action->endTurn = TRUE;

	m_itemQueue->AddTail(item);
}


void Director::AddAttack(Unit attacker, Unit defender)
{
	
	
	UnitActor	*attackerActor = NULL;
	UnitActor	*defenderActor = NULL;

	DQActionAttack		*action = new DQActionAttack;
	DQItem				*item = new DQItem(DQITEM_ATTACK, action, dh_attack);
	item->SetOwner(attacker.GetOwner());

	


	action->defender_IsCity = FALSE;

	static Army army;

	if (attacker != Unit(0)) 
	{
		attackerActor = attacker.GetActor();
		action->attacker = attackerActor;
		action->attacker_Pos = attacker.RetPos();
		action->attacker_ID		= attacker.GetAttackSoundID();
		action->attacker_IsCity = attacker.IsCity();

		attackerActor->SetHealthPercent(attacker.GetHP() / attacker.GetDBRec()->GetMaxHP());

		army = attacker.GetArmy();
		if (g_theArmyPool->IsValid(army)) {
			attackerActor->SetTempStackSize(army.Num());
		}
	}

	if (defender != Unit(0)) 
	{
		defenderActor			= defender.GetActor();
		action->defender		= defenderActor;
		action->defender_Pos    = defender.RetPos();
		action->defender_ID		= defender.GetAttackSoundID();
		action->defender_IsCity = defender.IsCity();
	
		defenderActor->SetHealthPercent(defender.GetHP() / defender.GetDBRec()->GetMaxHP());

		CellUnitList *unitList;
		unitList = g_theWorld->GetCell(defender.RetPos())->UnitArmy();
		sint32 num = 1;
		if (unitList != NULL)
			num = unitList->Num();
		defenderActor->SetTempStackSize(num);
	}


	m_itemQueue->AddTail(item);

	if (g_player[g_selected_item->GetVisiblePlayer()] &&
		g_player[g_selected_item->GetVisiblePlayer()]->IsVisible(attacker.RetPos()))
		if (attacker.m_id != 0) {
			AddCombatFlash(attacker.RetPos());
		}

	if (g_player[g_selected_item->GetVisiblePlayer()] &&
		g_player[g_selected_item->GetVisiblePlayer()]->IsVisible(defender.RetPos()))
		if (defender.m_id != 0) {
			AddCombatFlash(defender.RetPos());
		}
}

void Director::AddAttackPos(Unit attacker, const MapPoint &pos)
{
	DQActionAttackPos	*action = new DQActionAttackPos;
	DQItem				*item = new DQItem(DQITEM_ATTACKPOS, action, dh_attackpos);
	item->SetOwner(attacker.GetOwner());

	action->attackpos_attacker = attacker.GetActor();
	action->attackpos_attacker_pos = attacker.RetPos();
	action->attackpos_target_pos = pos;
	action->attackpos_soundID = attacker.GetAttackSoundID();

	m_itemQueue->AddTail(item);

	if (g_player[g_selected_item->GetVisiblePlayer()] &&
		g_player[g_selected_item->GetVisiblePlayer()]->IsVisible(pos))
		if (attacker.m_id != 0) {
			AddCombatFlash(pos);
		}
}

void Director::AddSpecialAttack(Unit attacker, Unit attacked, SPECATTACK attack)
{
	UnitActor	*attackerActor = attacker.IsValid() ? attacker.GetActor() : NULL;
	UnitActor	*defenderActor = attacked.IsValid() ? attacked.GetActor() : NULL;

	DQActionAttack	*action = new DQActionAttack;
	DQItem					*item = new DQItem(DQITEM_SPECATTACK, action, dh_specialAttack);
	item->SetOwner(attacker.GetOwner());




	action->attacker = attackerActor;
	action->defender = defenderActor;
	action->attacker_Pos = attacker.RetPos();
	action->defender_Pos = attacked.RetPos();
	action->attacker_IsCity = attacker.IsCity();
	action->defender_IsCity = attacked.IsCity();

	sint32 soundID, spriteID;

	if (!attacker.GetSpecialAttackInfo(attack, &soundID, &spriteID)) 
	{
		delete item;
		delete action;

		return;
	}

	action->attacker_ID  = soundID;
	action->defender_ID  = spriteID;






	m_itemQueue->AddTail(item);

	

	if (g_player[g_selected_item->GetVisiblePlayer()] &&
		g_player[g_selected_item->GetVisiblePlayer()]->IsVisible(attacked.RetPos())) {

		SpriteState *ss = new SpriteState(spriteID);
		AddProjectileAttack(attacker, attacked, NULL, ss, 0);
	}
}


void Director::AddWinnerLoser(Unit victor, Unit dead)
{
	Assert(victor.GetActor());
	Assert(dead.GetActor());

	DQActionDeath		*action = new DQActionDeath;
	DQItem				*item = new DQItem(DQITEM_DEATH, action, dh_death);
	item->SetOwner(victor.GetOwner());
	



	action->death_victor = victor.GetActor();
	action->victor_id = victor.m_id;
	action->death_dead = dead.GetActor();
	action->dead_id = dead.m_id;
	action->victor_Pos = victor.RetPos();
	action->dead_Pos = dead.RetPos();
	
	action->dead_soundID = dead.GetDeathSoundID();
	action->victor_soundID = victor.GetVictorySoundID();






	m_itemQueue->AddTail(item);
}

void Director::AddDeath(Unit dead)
{
	Assert(dead.GetActor());

	DQActionDeath		*action = new DQActionDeath;
	DQItem				*item = new DQItem(DQITEM_DEATH, action, dh_death);
	item->SetOwner(dead.GetOwner());




	action->death_dead	= dead.GetActor();
	action->death_victor= NULL;
	action->dead_id		= dead.m_id;
	action->victor_id	= 0;
	action->victor_Pos	= dead.RetPos();
	action->dead_Pos	= dead.RetPos();
	action->dead_soundID	= dead.GetDeathSoundID();
	action->victor_soundID	= 0;




	ActiveUnitAdd(dead.GetActor());

	
	
	dead.SetActor(NULL);


	m_itemQueue->AddTail(item);
}

void Director::AddDeathWithSound(Unit dead, sint32 soundID) 
{
	Assert(dead.GetActor());

	DQActionDeath		*action = new DQActionDeath;
	DQItem				*item = new DQItem(DQITEM_DEATH, action, dh_death);
	item->SetOwner(dead.GetOwner());




	action->death_dead = dead.GetActor();
	action->dead_id = dead.m_id;
	action->death_victor = NULL;
	action->victor_id = 0;
	action->victor_Pos = dead.RetPos();
	action->dead_Pos = dead.RetPos();
	action->dead_soundID = soundID;
	action->victor_soundID = 0;




	
	
	dead.SetActor(NULL);


	m_itemQueue->AddTail(item);
}

void Director::AddMorphUnit(UnitActor *morphingActor, SpriteState *ss, sint32 type,  Unit id)
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

void Director::AddHide(Unit hider)
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

void Director::AddShow(Unit hider)
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

void Director::AddWork(Unit worker)
{
	UnitActor	*actor = worker.GetActor();
	Assert(actor);
	if (!actor) return;

	DQActionWork	*action = new DQActionWork;
	DQItem			*item = new DQItem(DQITEM_WORK, action, dh_work);
	item->SetOwner(worker.GetOwner());




	action->working_actor = actor;
	action->working_pos = worker.RetPos();
	action->working_soundID = worker.GetWorkSoundID();






	m_itemQueue->AddTail(item);
}

void Director::AddFastKill(Unit dead)
{
	UnitActor *actor = dead.GetActor();
	Assert(actor);
	if (!actor) return;

	DQActionFastKill	*action = new DQActionFastKill;
	DQItem				*item = new DQItem(DQITEM_FASTKILL, action, dh_fastkill);



	
	action->dead = actor;




	
	
	dead.SetActor(NULL);



	m_itemQueue->AddTail(item);
}





void Director::AddRemoveVision(MapPoint &pos, double range)
{
	DQActionVision		*action = new DQActionVision;
	DQItem				*item = new DQItem(DQITEM_REMOVEVISION, action, dh_removeVision);




	action->vision_pos = pos;
	action->vision_range = range;






	m_itemQueue->AddTail(item);
}





void Director::AddAddVision(MapPoint &pos, double range)
{
	DQActionVision		*action = new DQActionVision;
	DQItem				*item = new DQItem(DQITEM_ADDVISION, action, dh_addVision);




	action->vision_pos = pos;
	action->vision_range = range;






	m_itemQueue->AddTail(item);
}





void Director::AddSetVisibility(UnitActor *actor, uint32 visibility)
{
	Assert(actor);
	if (!actor) return;

	DQActionSetVisibility	*action = new DQActionSetVisibility;
	DQItem					*item = new DQItem(DQITEM_SETVISIBILITY, action, dh_setVisibility);




	action->setvisibility_actor = actor;
	action->visibilityFlag = visibility;






	m_itemQueue->AddTail(item);
}





void Director::AddSetOwner(UnitActor *actor, sint32 owner)
{
	Assert(actor);
	if (!actor) return;

	DQActionSetOwner	*action = new DQActionSetOwner;
	DQItem				*item = new DQItem(DQITEM_SETOWNER, action, dh_setOwner);




	action->setowner_actor = actor;
	action->owner = owner;






	m_itemQueue->AddTail(item);
}





void Director::AddSetVisionRange(UnitActor *actor, double range)
{
	Assert(actor);
	if (!actor) return;

	DQActionSetVisionRange	*action = new DQActionSetVisionRange;
	DQItem					*item = new DQItem(DQITEM_SETVISIONRANGE, action, dh_setVisionRange);




	action->setvisionrange_actor = actor;
	action->range = range;






	m_itemQueue->AddTail(item);
}

void Director::AddBattle(Battle *battle)
{
	Assert(battle);
	if (!battle) return;

	DQActionBattle	*action = new DQActionBattle;
	DQItem			*item = new DQItem(DQITEM_BATTLE, action, dh_battle);




	action->battle = battle;






	m_itemQueue->AddTail(item);
}

void Director::AddPlaySound(sint32 soundID, const MapPoint &pos)
{
	if (soundID <= 0) return;

	DQActionPlaySound	*action = new DQActionPlaySound;
	DQItem			*item = new DQItem(DQITEM_PLAYSOUND, action, dh_playSound);

	action->playsound_soundID = soundID;
	action->playsound_pos = pos;

	m_itemQueue->AddTail(item);
}

void Director::AddGameSound(GAMESOUNDS sound)
{
	sint32 soundID = gamesounds_GetGameSoundID((sint32)sound);
	static MapPoint pos(0,0);
	AddPlaySound(soundID, pos);
}


void Director::AddPlayWonderMovie(sint32 which)
{
	Assert(which >= 0);
	if (which < 0) return;

	DQActionPlayWonderMovie	*action = new DQActionPlayWonderMovie;
	DQItem					*item = new DQItem(DQITEM_PLAYWONDERMOVIE, action, dh_playWonderMovie);




	action->playwondermovie_which = which;
	





	m_itemQueue->AddTail(item);
}

void Director::AddPlayVictoryMovie(GAME_OVER reason, BOOL previouslyWon, BOOL previouslyLost)
{
	PLAYER_INDEX	player = g_selected_item->GetVisiblePlayer();

	
	
	if (previouslyWon || previouslyLost) {
		
		
		if(g_player[player] && !g_player[player]->m_isDead) {
			return;
		}
	}

	DQActionPlayVictoryMovie	*action = new DQActionPlayVictoryMovie;
	DQItem						*item = new DQItem(DQITEM_PLAYVICTORYMOVIE, action, dh_playVictoryMovie);




	action->playvictorymovie_reason = reason;
	





	m_itemQueue->AddTail(item);
}

void Director::AddMessage(const Message &message)
{
	DQActionMessage	*action = new DQActionMessage;
	DQItem			*item = new DQItem(DQITEM_MESSAGE, action, dh_message);




	action->message = message;
	





	m_itemQueue->AddTail(item);
}

void Director::AddFaceoff(Unit &attacker, Unit &defender)
{
	DQActionFaceoff	*action = new DQActionFaceoff;
	DQItem			*item = new DQItem(DQITEM_FACEOFF, action, dh_faceoff);




	if (attacker.m_id != 0) {
		action->faceoff_attacker = attacker.GetActor();
		action->faceoff_attacker_pos = attacker.RetPos();
	} else {
		action->faceoff_attacker = NULL;
	}

	if (defender.m_id != 0) {
		action->faceoff_attacked = defender.GetActor();
		action->faceoff_attacked_pos = defender.RetPos();
	} else {
		action->faceoff_attacked = NULL;
	}






	m_itemQueue->AddTail(item);
}


void Director::AddTerminateFaceoff(Unit &faceroffer)
{
	DQActionTerminateFaceOff	*action = new DQActionTerminateFaceOff;
	DQItem						*item = new DQItem(DQITEM_TERMINATE_FACEOFF, action, dh_terminateFaceoff);
	item->SetOwner(faceroffer.GetOwner());

	if (faceroffer.m_id != 0) {
		action->faceroffer = faceroffer.GetActor();
	} else {
		action->faceroffer = NULL;
	}

	m_itemQueue->AddTail(item);
}

void Director::AddTerminateSound(Unit &unit)
{
	DQActionTerminateSound	*action = new DQActionTerminateSound;
	DQItem					*item = new DQItem(DQITEM_TERMINATE_SOUND, action, dh_terminateSound);




	if (unit.m_id != 0) {
		action->terminate_sound_unit = unit;
	} else {
		delete item;
		delete action;
		return;
	}



	


	m_itemQueue->AddTail(item);
}

void Director::AddInvokeThroneRoom(void)
{
	DQActionInvokeThroneRoom	*action = new DQActionInvokeThroneRoom;
	DQItem						*item = new DQItem(DQITEM_INVOKE_THRONE_ROOM, action, dh_invokeThroneRoom);

	m_itemQueue->AddTail(item);
}



void Director::AddInvokeResearchAdvance(MBCHAR *message)
{
	DQActionInvokeResearchAdvance *action = new DQActionInvokeResearchAdvance;
	DQItem							*item = new DQItem(DQITEM_INVOKE_RESEARCH_ADVANCE, action, dh_invokeResearchAdvance);

	if (message) {
		MBCHAR *mess = new MBCHAR[strlen(message)+1];
		strcpy(mess, message);
		action->message = mess;
	} else {
		action->message = NULL;
	}

	m_itemQueue->AddTail(item);
}

void Director::AddBeginScheduler(sint32 player)
{
	Assert(player == g_selected_item->GetCurPlayer());
	if(player != g_selected_item->GetCurPlayer())
		return;

	DPRINTF(k_DBG_GAMESTATE, ("Director::AddBeginScheduler(%d)\n", player));

	DQActionBeginScheduler *action = new DQActionBeginScheduler;
	DQItem                   *item = new DQItem(DQITEM_BEGIN_SCHEDULER, action, dh_beginScheduler);
	action->player = player;

	m_itemQueue->AddTail(item);
}



BOOL Director::TileWillBeCompletelyVisible(sint32 x, sint32 y)
{
	
	
	
	
	RECT tempViewRect;

	tempViewRect = *g_tiledMap->GetMapViewRect();

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

void Director::IncrementPendingGameActions()
{
	m_pendingGameActions++;
}

void Director::DecrementPendingGameActions()
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

void Director::ReloadAllSprites()
{
	sint32 p, i;
	for(p = 0; p < k_MAX_PLAYERS; p++) {
		if(!g_player[p]) continue;
        //PFT  29 mar 05
		//cycle through human players' cities
        if(g_player[g_selected_item->GetVisiblePlayer()]->m_playerType == PLAYER_TYPE_HUMAN){
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

void Director::NotifyResync()
{
	m_endTurnRequested = false;
	m_pendingGameActions = 0;
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
		g_director->ActionFinished(seq);
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
		g_director->ActionFinished(seq);
		return;
	}
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	

   

   visible	=  	g_director->TileIsVisibleToPlayer(oldP)||
				g_director->TileIsVisibleToPlayer(newP);

   
   
		
		
		
	   
	   
	   
		
		
	   
	
	
	
		
		


		
	 
	 
			
		  
	 
	 
	 
			
	  
	  
	


	
	
	
	if (visible && executeType == DHEXECUTE_NORMAL) 
	{
		seq->SetAddedToActiveList(SEQ_ACTOR_PRIMARY, TRUE);
	  	g_director->ActiveUnitAdd(theActor);

		
		
		if (g_selected_item->GetVisiblePlayer()!= theActor->GetPlayerNum()
			&& !g_tiledMap->TileIsVisible(theActor->GetPos().x, theActor->GetPos().y)) 
		{
			g_radarMap->CenterMap(theActor->GetPos());
			g_tiledMap->Refresh();
			g_tiledMap->InvalidateMap();
			g_tiledMap->InvalidateMix();
			background_draw_handler(g_background);
		}
	} 
	else 
	{ 
		if (theActor->WillDie()) 
			g_director->FastKill(theActor);
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

	g_director->ActionFinished(seq);
}



void dh_projectileMove(DQAction *itemAction, Sequence *seq, DHEXECUTE executeType)
{
	DQActionMoveProjectile	*action = (DQActionMoveProjectile *)itemAction;

	ProjectileActor		*projectile = action->move_projectile;
	EffectActor			*projectileEnd = action->end_projectile;
	UnitActor			*shootingActor = action->pshooting_actor;
	UnitActor			*targetActor = action->ptarget_actor;
	MapPoint			startPos = action->pmove_oldPos;
	MapPoint			endPos = action->pmove_newPos;

	
	Assert(shootingActor != NULL && targetActor != NULL);

	if (shootingActor == NULL || targetActor == NULL) 
		return;

	
	if(projectileEnd && g_director->TileIsVisibleToPlayer(startPos))
	{
		Action *actionObj= NULL;

		
		
		
		Anim *anim = projectileEnd->GetAnim(EFFECTACTION_PLAY);
		if (anim == NULL) 
		{
			
			anim = projectileEnd->GetAnim(EFFECTACTION_FLASH);
			Assert(anim != NULL);
			if (anim == NULL) 
			{
				g_director->ActionFinished(seq);
				return;
			}
			else
			{
				actionObj = new Action(EFFECTACTION_FLASH, ACTIONEND_PATHEND);
				Assert(actionObj != NULL);
				if (actionObj == NULL) {
					g_director->ActionFinished(seq);
					return;
				}
			}
		}
		else
		{
			actionObj = new Action(EFFECTACTION_PLAY, ACTIONEND_PATHEND);
			Assert(actionObj != NULL);
			if (actionObj == NULL) {
				g_director->ActionFinished(seq);
				return;
			}
		}


		
		
		actionObj->SetAnim(anim);

		
		projectileEnd->AddAction(actionObj);

		
		g_director->ActiveEffectAdd(projectileEnd);
	}

	g_director->ActionFinished(seq);
}
 

void dh_attack(DQAction *itemAction, Sequence *seq, DHEXECUTE executeType)
{
	DQActionAttack	*action = (DQActionAttack *)itemAction;

	
	BOOL  defenderVisible=FALSE;
	BOOL  attackerVisible=FALSE;
	BOOL  battleVisible  =FALSE;
	BOOL  playerInvolved =FALSE;

	
	UnitActor	*theAttacker = action->attacker;
	UnitActor	*theDefender = action->defender;
	
	
	Assert(theAttacker != NULL);
	Assert(theDefender != NULL);

	if ((theAttacker == NULL)||(theDefender == NULL)) return;

	BOOL defenderIsAttackable = !action->defender_IsCity;

	Action *ActionObj = NULL;

	
	attackerVisible = g_director->TileIsVisibleToPlayer(action->attacker_Pos);
	defenderVisible = g_director->TileIsVisibleToPlayer(action->defender_Pos);
	
	
	playerInvolved  = (theDefender->GetPlayerNum() == g_selected_item->GetVisiblePlayer()) ||
					  (theAttacker->GetPlayerNum() == g_selected_item->GetVisiblePlayer());

	
	sint32 facingIndex;
	POINT  AttackerPoints, DefenderPoints;

	
	maputils_MapXY2PixelXY(action->attacker_Pos.x, action->attacker_Pos.y, &(AttackerPoints.x), &(AttackerPoints.y));
	maputils_MapXY2PixelXY(action->defender_Pos.x, action->defender_Pos.y, &(DefenderPoints.x), &(DefenderPoints.y));

	
	sint32  deltax=DefenderPoints.x-AttackerPoints.x;
	sint32  deltay=DefenderPoints.y-AttackerPoints.y;


	
	facingIndex=spriteutils_DeltaToFacing(deltax,deltay);

	
	ActionObj = new Action();

	
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
	   	g_director->ActiveUnitAdd(theAttacker);
	}
	else 
	{
		if (theAttacker->WillDie()) 
			g_director->FastKill(theAttacker);
		else 
			theAttacker->EndTurnProcess();
	}

	
	if (defenderIsAttackable) 
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
		   defenderVisible = TRUE;
	 
		
		if(defenderVisible && (executeType == DHEXECUTE_NORMAL)) 
		{
			seq->SetAddedToActiveList(SEQ_ACTOR_SECONDARY, TRUE);
		    g_director->ActiveUnitAdd(theDefender);
		}
		else 
		{
			if (theDefender->WillDie()) 
				g_director->FastKill(theDefender);
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
		g_director->ActionFinished(seq);
		return;
	}
	
	sint32 facingIndex;
	POINT  AttackerPoints, DefenderPoints;

	
	maputils_MapXY2PixelXY(action->attacker_Pos.x, action->attacker_Pos.y, &(AttackerPoints.x), &(AttackerPoints.y));
	maputils_MapXY2PixelXY(action->defender_Pos.x, action->defender_Pos.y, &(DefenderPoints.x), &(DefenderPoints.y));

	
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
		   g_director->ActionFinished(seq);
		   return;
		}
	  
		
		if (g_director->TileIsVisibleToPlayer(action->attacker_Pos) && executeType == DHEXECUTE_NORMAL) 
		{
			seq->SetAddedToActiveList(SEQ_ACTOR_PRIMARY, TRUE);
			g_director->ActiveUnitAdd(theAttacker);
		} 
		else 
		{
			if (theAttacker->WillDie()) 
				g_director->FastKill(theAttacker);
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
		   g_director->ActionFinished(seq);
		   return;
		}

		
		if (g_director->TileIsVisibleToPlayer(action->defender_Pos) && executeType == DHEXECUTE_NORMAL) 
		{
			seq->SetAddedToActiveList(SEQ_ACTOR_SECONDARY, TRUE);
			g_director->ActiveUnitAdd(theDefender);
		} 
		else 
		{
			if (theDefender->WillDie()) 
				g_director->FastKill(theDefender);
			else 
				theDefender->EndTurnProcess();
		}
	}
}


void dh_death(DQAction *itemAction, Sequence *seq, DHEXECUTE executeType)
{
	DQActionDeath	*action = (DQActionDeath *)itemAction;

	
	
	
	
	
	

	UnitActor	*theDead = action->death_dead;
	UnitActor	*theVictor = action->death_victor;
	Anim		*deathAnim = NULL;
	Anim		*victorAnim = NULL;
	uint32		deathActionType = (unsigned) UNITACTION_NONE, 
				victorActionType = (unsigned) UNITACTION_NONE;

	
	
	

	
	
	

	
	
	if (theDead != NULL && !theDead->GetNeedsToDie())
	{
		theDead->SetNeedsToDie(TRUE);


		
		
		
		if(theDead->HasDeath() && theDead->GetAnim(UNITACTION_VICTORY))
		{
			
			
			if (theDead->GetLoadType() != LOADTYPE_FULL) 
				theDead->FullLoad(UNITACTION_VICTORY);

			deathActionType = UNITACTION_VICTORY;
			deathAnim = theDead->GetAnim((UNITACTION)deathActionType);
		} 
		else
		{
			
			deathActionType = (unsigned) UNITACTION_FAKE_DEATH;
			deathAnim = theDead->MakeFakeDeath();
		}
	}
	else
	{
		
		theDead = NULL;
	}

	
	
	if(theVictor != NULL && !theVictor->GetNeedsToDie())
	{
		g_director->ActiveUnitRemove(theVictor);

		theVictor->SetNeedsToVictor(TRUE);
		
		if(theVictor->HasDeath())
		{
			
		} 
		else
		{
			
			
			if (theVictor->GetLoadType() != LOADTYPE_FULL) {
				theVictor->FullLoad(UNITACTION_VICTORY);
			}
			
			victorActionType = UNITACTION_VICTORY;

			victorAnim = theVictor->GetAnim((UNITACTION)victorActionType);
			if (victorAnim == NULL) 
			{
				theVictor = NULL;
			}
		}
	}

	if(theDead != NULL )
	{
		theDead->SetHealthPercent(-1.0);
		theDead->SetTempStackSize(0);

		Action *deadActionObj = new Action(	(UNITACTION)deathActionType, ACTIONEND_ANIMEND);
		Assert(deadActionObj != NULL);
		if (deadActionObj == NULL) 
		{
			c3errors_ErrorDialog("Director", "Internal Failure to create death action");
			delete deathAnim;
			delete victorAnim;
			return;
		}

		deadActionObj->SetSequence(seq);
		seq->AddRef();
		
		deadActionObj->SetStartMapPoint(action->dead_Pos);
		deadActionObj->SetEndMapPoint(action->dead_Pos);

		deadActionObj->SetAnim(deathAnim);

		deadActionObj->SetUnitVisionRange(theDead->GetUnitVisionRange());
		deadActionObj->SetUnitsVisibility(theDead->GetUnitVisibility());
		deadActionObj->SetFacing(theDead->GetFacing());

		
		if (g_soundManager)
			g_soundManager->TerminateLoopingSound(SOUNDTYPE_SFX, (uint32)theDead->GetUnitID());

		if (g_soundManager) {
			sint32 visiblePlayer = g_selected_item->GetVisiblePlayer();
			if ((visiblePlayer == theDead->GetPlayerNum()) ||
				(theDead->GetUnitVisibility() & (1 << visiblePlayer))) {
				g_soundManager->AddSound(SOUNDTYPE_SFX, (uint32)theDead->GetUnitID(), action->dead_soundID,
									theDead->GetPos().x, theDead->GetPos().y);
			}
		}

		theDead->AddAction(deadActionObj);

		
		
		if (g_director->TileIsVisibleToPlayer(action->dead_Pos) 
				&& executeType == DHEXECUTE_NORMAL) {
			seq->SetAddedToActiveList(SEQ_ACTOR_PRIMARY, TRUE);
			g_director->ActiveUnitAdd(theDead);
		} else {
			if (theDead->WillDie()) {
				g_director->FastKill(theDead);
			} else {
				theDead->EndTurnProcess();
			}
		}
	}
	
	
	if(theVictor != NULL)
	{
		theVictor->SetHealthPercent(-1.0);
		theVictor->SetTempStackSize(0);

		if(theVictor->HasDeath() || victorAnim == NULL)
		{
			theVictor->ActionQueueUpIdle();
		}
		else
		{

			
			Action *victorActionObj = new Action((UNITACTION)victorActionType, ACTIONEND_ANIMEND);
			Assert(victorActionObj != NULL);
			if (victorActionObj == NULL) 
			{
				c3errors_ErrorDialog("Director", "Internal Failure to create victory action");
				delete deathAnim;
				delete victorAnim;
				return;
			}
			victorActionObj->SetSequence(seq);
			seq->AddRef();
			victorActionObj->SetStartMapPoint(action->victor_Pos);
			victorActionObj->SetEndMapPoint(action->victor_Pos);
			
			victorActionObj->SetAnim(victorAnim);

			victorActionObj->SetUnitVisionRange(theVictor->GetUnitVisionRange());
			victorActionObj->SetUnitsVisibility(theVictor->GetUnitVisibility());
			
			
			if (g_soundManager)
				g_soundManager->TerminateLoopingSound(SOUNDTYPE_SFX, (uint32)theVictor->GetUnitID());
			if (g_soundManager) {
				sint32 visiblePlayer = g_selected_item->GetVisiblePlayer();
				if ((visiblePlayer == theVictor->GetPlayerNum()) ||
					(theVictor->GetUnitVisibility() & (1 << visiblePlayer))) {
					g_soundManager->AddSound(SOUNDTYPE_SFX, (uint32)theVictor->GetUnitID(), action->victor_soundID,
											theVictor->GetPos().x, theVictor->GetPos().y);
				}
			}

			theVictor->AddAction(victorActionObj);
		}
		
		if (g_director->TileIsVisibleToPlayer(action->victor_Pos) && executeType == DHEXECUTE_NORMAL) {
			seq->SetAddedToActiveList(SEQ_ACTOR_SECONDARY, TRUE);
			g_director->ActiveUnitAdd(theVictor);
		} else {
			if (theVictor->WillDie()) {
				g_director->FastKill(theVictor);
			} else {
				theVictor->EndTurnProcess();
			}
		}
	}
	
}

void dh_morphUnit(DQAction *itemAction, Sequence *seq, DHEXECUTE executeType)
{
	DQActionMorph	*action = (DQActionMorph *)itemAction;

	UnitActor		*theActor = action->morphing_actor;

	Assert(theActor != NULL);
	if (theActor == NULL) {
		g_director->ActionFinished(seq);
		return;
	}

	
	theActor->ChangeType(action->ss,  action->type,	action->id, FALSE);	

	g_director->ActionFinished(seq);
}

void dh_hide(DQAction *itemAction, Sequence *seq, DHEXECUTE executeType)
{
	DQActionHideShow	*action = (DQActionHideShow *)itemAction;

	Assert(action);
	if (!action) return;

	UnitActor		*actor;
	actor = action->hiding_actor;

	Assert(actor);
	if (!actor) return;

	actor->Hide();


	g_director->ActionFinished(seq);
}

void dh_show(DQAction *itemAction, Sequence *seq, DHEXECUTE executeType)
{
	DQActionHideShow	*action = (DQActionHideShow *)itemAction;

	Assert(action);
	if (!action) return;

	UnitActor		*actor;
	actor = action->hiding_actor;

	Assert(actor);
	if (!actor) return;

	actor->PositionActor(action->hiding_pos);
	actor->Show();


	g_director->ActionFinished(seq);
}

void dh_work(DQAction *itemAction, Sequence *seq, DHEXECUTE executeType)
{
	DQActionWork	*action = (DQActionWork *)itemAction;

	Assert(action);
	if (!action) return;

	UnitActor *actor;
	actor = action->working_actor;

	Assert(actor);
	if (!actor) return;

	Action *actionObj = new Action(UNITACTION_WORK, ACTIONEND_ANIMEND);

	Assert(actionObj != NULL);
	if (actionObj == NULL) return;

	actionObj->SetStartMapPoint(action->working_pos);
	actionObj->SetEndMapPoint(action->working_pos);

	
	
	if (actor->GetLoadType() != LOADTYPE_FULL)
		actor->FullLoad(UNITACTION_WORK);

	
	Anim	*anim = actor->GetAnim(UNITACTION_WORK);
	if (anim == NULL) 
	{
		anim = actor->GetAnim(UNITACTION_MOVE);

		if (!anim) {
			delete actionObj;
			g_director->ActionFinished(seq);
			return;
		}
	}

	actionObj->SetSequence(seq);
	seq->AddRef();

	actionObj->SetAnim(anim);

	actor->AddAction(actionObj);

	
	if (g_soundManager) {
		sint32 visiblePlayer = g_selected_item->GetVisiblePlayer();
		if ((visiblePlayer == actor->GetPlayerNum()) ||
			(actor->GetUnitVisibility() & (1 << visiblePlayer))) {
			g_soundManager->AddSound(SOUNDTYPE_SFX, (uint32)actor->GetUnitID(), action->working_soundID,
									actor->GetPos().x, actor->GetPos().y);
		}
	}

	if (g_director->TileIsVisibleToPlayer(action->working_pos) && executeType == DHEXECUTE_NORMAL) {
		seq->SetAddedToActiveList(SEQ_ACTOR_PRIMARY, TRUE);
		g_director->ActiveUnitAdd(actor);
	} else {
		if (actor->WillDie()) {
			g_director->FastKill(actor);
		} else {
			actor->EndTurnProcess();
		}
	}
}

void dh_fastkill(DQAction *itemAction, Sequence *seq, DHEXECUTE executeType)
{
	DQActionFastKill	*action = (DQActionFastKill *)itemAction;

	Assert(action);
	if (!action) return;

	UnitActor *actor;
	actor = action->dead;
	Assert(actor);
	if (!actor) return;


	g_director->FastKill(actor);
	g_director->ActionFinished(seq);
}

void dh_removeVision(DQAction *itemAction, Sequence *seq, DHEXECUTE executeType)
{
	DQActionVision	*action = (DQActionVision *)itemAction;

	Assert(action);
	if (!action) return;

	if(g_tiledMap)
		g_tiledMap->GetLocalVision()->RemoveVisible(action->vision_pos, action->vision_range);

	g_director->ActionFinished(seq);
}

void dh_addVision(DQAction *itemAction, Sequence *seq, DHEXECUTE executeType)
{
	DQActionVision	*action = (DQActionVision *)itemAction;

	Assert(action);
	if (!action) return;

	BOOL revealedUnexplored;

	g_tiledMap->GetLocalVision()->AddVisible(action->vision_pos, action->vision_range, 
												revealedUnexplored);

	g_director->ActionFinished(seq);

}

void dh_setVisibility(DQAction *itemAction, Sequence *seq, DHEXECUTE executeType)
{
	DQActionSetVisibility	*action = (DQActionSetVisibility *)itemAction;

	UnitActor	*actor;
	actor = action->setvisibility_actor;
	Assert(actor);
	if (!actor) return;

	actor->SetUnitVisibility(action->visibilityFlag);

	g_director->ActionFinished(seq);
}

void dh_setOwner(DQAction *itemAction, Sequence *seq, DHEXECUTE executeType)
{
	DQActionSetOwner	*action = (DQActionSetOwner *)itemAction;

	UnitActor	*actor;
	actor = action->setowner_actor;
	Assert(actor);
	if (!actor) return;

	actor->SetPlayerNum(action->owner);

	g_director->ActionFinished(seq);

}

void dh_setVisionRange(DQAction *itemAction, Sequence *seq, DHEXECUTE executeType)
{
	DQActionSetVisionRange	*action = (DQActionSetVisionRange *)itemAction;

	UnitActor *actor;
	actor = action->setvisionrange_actor;
	Assert(actor);

	if (!actor) return;

	actor->SetUnitVisionRange(action->range);

	g_director->ActionFinished(seq);
}

void dh_combatflash(DQAction *itemAction, Sequence *seq, DHEXECUTE executeType)
{
	DQActionCombatFlash	*action = (DQActionCombatFlash *)itemAction;

	SpriteState		*ss = new SpriteState(99);
	EffectActor		*flash = new EffectActor(ss, action->flash_pos);

	Action *actionObj= NULL;

	Anim *anim = flash->GetAnim(EFFECTACTION_PLAY);
	if (anim == NULL) 
	{
		
		anim = flash->GetAnim(EFFECTACTION_FLASH);
		Assert(anim != NULL);
	}

	if (!anim) {
		g_director->ActionFinished(seq);
		return;
	}

	actionObj = new Action(EFFECTACTION_FLASH, ACTIONEND_PATHEND);
	Assert(actionObj != NULL);
	if (actionObj == NULL) {
		g_director->ActionFinished(seq);
		return;
	}

	
	actionObj->SetAnim(anim);

	
	flash->AddAction(actionObj);

	g_director->ActiveEffectAdd(flash);

	g_director->ActionFinished(seq);
}

void dh_copyVision(DQAction *itemAction, Sequence *seq, DHEXECUTE executeType)
{
	DQActionCopyVision	*action = (DQActionCopyVision *)itemAction;

	g_tiledMap->CopyVision();
	g_radarMap->Update();

	g_director->ActionFinished(seq);
}

void dh_centerMap(DQAction *itemAction, Sequence *seq, DHEXECUTE executeType)
{
	DQActionCenterMap	*action = (DQActionCenterMap *)itemAction;

	if(!g_selected_item->GetIsPathing()) {
		
		g_radarMap->CenterMap(action->centerMap_pos);
		
		g_tiledMap->Refresh();
		g_tiledMap->InvalidateMap();
		g_tiledMap->InvalidateMix();
		
		background_draw_handler(g_background);
	}

	g_director->ActionFinished(seq);
}

void dh_selectUnit(DQAction *itemAction, Sequence *seq, DHEXECUTE executeType)
{
	DQActionUnitSelection	*action = (DQActionUnitSelection *)itemAction;

	g_selected_item->DirectorUnitSelection(action->flags);

	g_director->ActionFinished(seq);
}

void dh_endTurn(DQAction *itemAction, Sequence *seq, DHEXECUTE executeType)
{
	DQActionEndTurn	*action = (DQActionEndTurn *)itemAction;

	g_director->ActionFinished(seq);

	g_gevManager->AddEvent(GEV_INSERT_Tail, GEV_EndTurn,
						   GEA_Player, g_selected_item->GetCurPlayer(),
						   GEA_End);
	
}

void dh_battle(DQAction *itemAction, Sequence *seq, DHEXECUTE executeType)
{
	
	DQActionBattle	*action = (DQActionBattle *)itemAction;

	
	
	if(g_battleViewWindow) {
		BattleViewWindow::Cleanup();
	}

	
	BattleViewWindow::Initialize(seq);

	
	if(g_battleViewWindow) {
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

	g_director->ActionFinished(seq);
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

	g_director->ActionFinished(seq);
}

void dh_faceoff(DQAction *itemAction, Sequence *seq, DHEXECUTE executeType) 
{
	DQActionFaceoff	*action = (DQActionFaceoff *)itemAction;

	Assert(action);
	if (!action) return;

	UnitActor	*theAttacker = action->faceoff_attacker;
	UnitActor	*theAttacked = action->faceoff_attacked;
	
	BOOL		attackedIsAttackable = TRUE;

	Assert(theAttacker != NULL);
	if (theAttacker == NULL) return;

	Assert(theAttacked != NULL);
	if (theAttacked == NULL) return;



	Action *AttackerActionObj = NULL;
	Action *AttackedActionObj = NULL;

	
	AttackerActionObj = new Action(UNITACTION_FACE_OFF, ACTIONEND_INTERRUPT);
	Assert(AttackerActionObj != NULL);
	if (AttackerActionObj == NULL) return;

	AttackerActionObj->SetSequence(NULL);

	if (attackedIsAttackable) {
		
		AttackedActionObj = new Action(UNITACTION_FACE_OFF, ACTIONEND_INTERRUPT);
		Assert(AttackedActionObj != NULL);
		if (AttackedActionObj == NULL) return;

		AttackedActionObj->SetSequence(NULL);
	}

	AttackerActionObj->SetStartMapPoint(action->faceoff_attacker_pos);
	AttackerActionObj->SetEndMapPoint(action->faceoff_attacker_pos);

	if (attackedIsAttackable) {
		AttackedActionObj->SetStartMapPoint(action->faceoff_attacked_pos);
		AttackedActionObj->SetEndMapPoint(action->faceoff_attacked_pos);
	}

	Anim	*AttackerAnim = NULL;
	Anim	*AttackedAnim = NULL;

	AttackerAnim = theAttacker->MakeFaceoff();
	if (AttackerAnim == NULL) 
	{
		theAttacker->AddIdle(TRUE);
		delete AttackerActionObj;

		if (AttackedActionObj) 
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
							&(AttackerPoints.x), &(AttackerPoints.y));
	maputils_MapXY2PixelXY(action->faceoff_attacked_pos.x, 
							action->faceoff_attacked_pos.y, &(AttackedPoints.x), &(AttackedPoints.y));

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

	
	BOOL attackedVisible = TRUE;

	
	if (attackedIsAttackable) {
		if(AttackedAnim != NULL)
		{
			AttackedActionObj->SetUnitVisionRange(theAttacked->GetUnitVisionRange());
			
			
			AttackedActionObj->SetUnitsVisibility(theAttacker->GetUnitVisibility());
			theAttacked->AddAction(AttackedActionObj);
		}

		
		
		attackedVisible = g_director->TileIsVisibleToPlayer(action->faceoff_attacked_pos);

		if (theAttacker->GetPlayerNum() == g_selected_item->GetVisiblePlayer() ||
			theAttacked->GetPlayerNum() == g_selected_item->GetVisiblePlayer())
			attackedVisible = TRUE;

		if(attackedVisible && executeType == DHEXECUTE_NORMAL) {
			seq->SetAddedToActiveList(SEQ_ACTOR_SECONDARY, TRUE);
			g_director->ActiveUnitAdd(theAttacked);
		} else {
			if (theAttacked->WillDie()) {
				g_director->FastKill(theAttacked);
			} else {
				theAttacked->EndTurnProcess();
			}
		}
	}

	BOOL attackerVisible = g_director->TileIsVisibleToPlayer(action->faceoff_attacker_pos);

	if (theAttacked->GetPlayerNum() == g_selected_item->GetVisiblePlayer() ||
		theAttacker->GetPlayerNum() == g_selected_item->GetVisiblePlayer()) {
		attackerVisible = TRUE;
		attackedVisible = TRUE;
	}
	if (attackerVisible && attackedVisible && executeType == DHEXECUTE_NORMAL) {
		seq->SetAddedToActiveList(SEQ_ACTOR_PRIMARY, TRUE);
		g_director->ActiveUnitAdd(theAttacker);
	} else {
		if (theAttacker->WillDie()) {
			g_director->FastKill(theAttacker);
		} else {
			theAttacker->EndTurnProcess();
		}
	}

	g_director->ActionFinished(seq);
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

		g_director->ActiveUnitRemove(facerOffer);
	}

	g_director->ActionFinished(seq);
}

void dh_terminateSound(DQAction *itemAction, Sequence *seq, DHEXECUTE executeType)
{
	DQActionTerminateSound	*action = (DQActionTerminateSound *)itemAction;

	Assert(action);
	if (!action) return;

	if (g_soundManager)
		g_soundManager->TerminateLoopingSound(SOUNDTYPE_SFX, action->terminate_sound_unit.m_id);

	g_director->ActionFinished(seq);
}

void dh_speceffect(DQAction *itemAction, Sequence *seq, DHEXECUTE executeType)
{
	DQActionSpecialEffect *action = (DQActionSpecialEffect *)itemAction;

	Assert(action);
	if (!action) {
		g_director->ActionFinished(seq);
		return;
	}

	MapPoint		pos = action->speceffect_pos;
	sint32			soundID = action->speceffect_soundID;
	sint32			spriteID = action->speceffect_spriteID;

	if (!g_tiledMap->GetLocalVision()->IsVisible(pos)) {
		g_director->ActionFinished(seq);
		return;
	}

	SpriteState		*ss = new SpriteState(spriteID);
	EffectActor		*effectActor = new EffectActor(ss, pos);

	Action *actionObj= NULL;

	
	
	
	Anim *anim = effectActor->GetAnim(EFFECTACTION_PLAY);
	if (anim == NULL) 
	{
		g_director->ActionFinished(seq);
		return;
	}

	actionObj = new Action(EFFECTACTION_PLAY, ACTIONEND_PATHEND);
	Assert(actionObj != NULL);
	if (actionObj == NULL) {
		g_director->ActionFinished(seq);
		return;
	}

	
	actionObj->SetAnim(anim);

	
	effectActor->AddAction(actionObj);

	
	g_director->ActiveEffectAdd(effectActor);

	
	if (g_soundManager)
		g_soundManager->AddSound(SOUNDTYPE_SFX, 0, soundID, pos.x, pos.y);


	
	g_director->ActionFinished(seq);
}

void dh_attackpos(DQAction *itemAction, Sequence *seq, DHEXECUTE executeType)
{
	DQActionAttackPos	*action = (DQActionAttackPos *)itemAction;

	Assert(action);
	if (!action) return;

	
	UnitActor	*theAttacker = action->attackpos_attacker;

	Assert(theAttacker != NULL);
	if (theAttacker == NULL || theAttacker->GetNeedsToDie()) {
		g_director->ActionFinished(seq);
		return;
	}
	Action *AttackerActionObj = NULL;

	
	AttackerActionObj = new Action(UNITACTION_ATTACK, ACTIONEND_ANIMEND, 
										theAttacker->GetHoldingCurAnimPos(UNITACTION_ATTACK), 
										theAttacker->GetHoldingCurAnimSpecialDelayProcess(UNITACTION_ATTACK));
	Assert(AttackerActionObj != NULL);
	if (AttackerActionObj == NULL) {
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

	AttackerAnim = theAttacker->GetAnim(UNITACTION_ATTACK);

	if (AttackerAnim == NULL) 
		AttackerAnim = theAttacker->GetAnim(UNITACTION_IDLE);

	AttackerActionObj->SetAnim(AttackerAnim);

	
	POINT AttackerPoints, AttackedPoints;

	
	maputils_MapXY2PixelXY(action->attackpos_attacker_pos.x, action->attackpos_attacker_pos.y, 
							&(AttackerPoints.x), &(AttackerPoints.y));
	maputils_MapXY2PixelXY(action->attackpos_target_pos.x, action->attackpos_target_pos.y, 
							&(AttackedPoints.x), &(AttackedPoints.y));

	AttackerActionObj->SetFacing(spriteutils_DeltaToFacing(AttackedPoints.x - AttackerPoints.x,
		AttackedPoints.y - AttackerPoints.y));

	AttackerActionObj->SetUnitVisionRange(theAttacker->GetUnitVisionRange());
	AttackerActionObj->SetUnitsVisibility(theAttacker->GetUnitVisibility());

	
	if (g_soundManager)
		g_soundManager->TerminateLoopingSound(SOUNDTYPE_SFX, (uint32)theAttacker->GetUnitID());

	if (g_soundManager) {
		sint32 visiblePlayer = g_selected_item->GetVisiblePlayer();
		if ((visiblePlayer == theAttacker->GetPlayerNum()) ||
			(theAttacker->GetUnitVisibility() & (1 << visiblePlayer))) {
			g_soundManager->AddSound(SOUNDTYPE_SFX, (uint32)theAttacker->GetUnitID(), 
								action->attackpos_soundID, 
								theAttacker->GetPos().x, theAttacker->GetPos().y);
		}
	}

	theAttacker->AddAction(AttackerActionObj);

	BOOL attackerVisible = g_director->TileIsVisibleToPlayer(action->attackpos_attacker_pos);

	if (attackerVisible && executeType == DHEXECUTE_NORMAL) {
		seq->SetAddedToActiveList(SEQ_ACTOR_PRIMARY, TRUE);
		g_director->ActiveUnitAdd(theAttacker);
	} else {
		if (theAttacker->WillDie()) {
			g_director->FastKill(theAttacker);
		} else {
			theAttacker->EndTurnProcess();
		}
	}
}

void dh_invokeThroneRoom(DQAction *itemAction, Sequence *seq, DHEXECUTE executeType)
{
	
	
	g_director->ActionFinished(seq);
}

void dh_invokeResearchAdvance(DQAction *itemAction, Sequence *seq, DHEXECUTE executeType)
{
	DQActionInvokeResearchAdvance *action = (DQActionInvokeResearchAdvance *)itemAction;

	if (!action) {
		g_director->ActionFinished(seq);
		return;
	}

	sci_advancescreen_displayMyWindow(action->message, 0, seq);

	delete action->message;
	action->message = NULL;
}

void dh_beginScheduler(DQAction *itemAction, Sequence *seq, DHEXECUTE executeType)
{
	DQActionBeginScheduler *action = (DQActionBeginScheduler *)itemAction;

	if(!action) {
		g_director->ActionFinished(seq);
		return;
	}

	
#ifdef _DEBUG
	static bool first = true;
	if (first) {
		int a;
		Assert(a=(action->player == g_selected_item->GetCurPlayer()));
		if (!a) first=false;
	}
#endif

	if(g_network.IsHost()) {
		g_network.Enqueue(new NetInfo(NET_INFO_CODE_BEGIN_SCHEDULER, action->player));
	}

	
	Assert(g_director->m_holdSchedulerSequence == NULL);
	if(!g_network.IsActive() || g_network.IsLocalPlayer(action->player)) {
		g_director->SetHoldSchedulerSequence(seq);
	} else {
		g_director->SetHoldSchedulerSequence(NULL);
	}

	
	g_gevManager->Pause(); 
	g_gevManager->AddEvent(GEV_INSERT_Tail, GEV_BeginScheduler,
						   GEA_Player, action->player,
						   GEA_End);
	g_gevManager->Resume();

	if(g_director->m_holdSchedulerSequence == NULL) {
		g_director->ActionFinished(seq);
	}
}
