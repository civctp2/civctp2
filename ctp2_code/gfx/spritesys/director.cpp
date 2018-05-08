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

#include "ctp/c3.h"
#include "gfx/spritesys/Director.h"

#include <inttypes.h>

#include <algorithm>  // std::fill
#include <utility>

#include "ctp/ctp2_utils/c3errors.h"
#include "ctp/debugtools/debugmemory.h"
#include "gfx/gfx_utils/colorset.h"
#include "gfx/gfx_utils/pixelutils.h"
#include "gfx/spritesys/Action.h"
#include "gfx/spritesys/Actor.h"
#include "gfx/spritesys/Anim.h"
#include "gfx/spritesys/EffectActor.h"
#include "gfx/spritesys/SpriteState.h"
#include "gfx/spritesys/TradeActor.h"
#include "gfx/spritesys/UnitActor.h"
#include "gfx/tilesys/maputils.h"
#include "gfx/tilesys/tiledmap.h"  // g_tiledMap
#include "gfx/tilesys/tileutils.h"
#include "gs/database/profileDB.h"  // g_theProfileDB
#include "gs/events/GameEventManager.h"
#include "gs/gameobj/Army.h"
#include "gs/gameobj/Player.h"  // g_player
#include "gs/gameobj/Unit.h"
#include "gs/gameobj/UnitData.h"
#include "gs/gameobj/citydata.h"
#include "gs/newdb/SpriteRecord.h"
#include "gs/newdb/UnitRecord.h"
#include "gs/utility/Globals.h"
#include "gs/utility/TurnCnt.h"  // g_turn
#include "gs/utility/UnitDynArr.h"
#include "gs/world/Cell.h"
#include "gs/world/World.h"  // g_theWorld
#include "gs/world/cellunitlist.h"
#include "net/general/network.h"
#include "robot/aibackdoor/dynarr.h"
#include "sound/gamesounds.h"
#include "sound/soundmanager.h"  // g_soundManager
#include "ui/aui_common/tech_wllist.h"
#include "ui/aui_ctp2/SelItem.h"   // g_selected_item
#include "ui/aui_ctp2/radarmap.h"  // g_radarMap
#include "ui/aui_utils/primitives.h"
#include "ui/interface/cursormanager.h"

#ifdef _PLAYTEST
extern BOOL g_doingFastRounds;
#endif

DQItem::DQItem(DQITEM_TYPE type, DQAction* action, DQHandler* handler)
    : m_type(type),
      m_addedToSavedList(FALSE),
      m_owner(PLAYER_UNASSIGNED),
      m_round(0),
      m_action(action),
      m_handler(handler) {
  if (g_turn) {
    m_round = static_cast<uint16>(g_turn->GetRound());
  }
}

DQItemPtr DQItem::CreatePtr(DQITEM_TYPE type,
                            DQAction* action,
                            DQHandler* handler) {
  DQItemPtr item(new DQItem(type, action, handler));

  if (g_director) {
    item->m_sequence = g_director->NewSequence();
    item->m_sequence->SetItem(item);
  }

  return item;
}

DQItem::~DQItem() {
  // DPRINTF(k_DBG_GAMESTATE, ("Deleting item @ %lx, type=%d\n", this, m_type));
  delete m_action;
}

SequenceWeakPtr DQItem::getSequence() {
  return m_sequence;
}

#define k_MAX_DISPATCHED_QUEUE_ITEMS 1000
#define k_MAX_SAVED_SEQUENCES 1000

Director::Director(void)
    : m_nextPlayer(FALSE),
      m_masterCurTime(0),
      m_lastTickCount(0),
      m_timeLogIndex(0),
      m_averageElapsed(0),
      m_averageFPS(k_DEFAULT_FPS),
      m_actionFinished(TRUE),
      m_paused(FALSE),
      m_processingActiveUnits(FALSE),
      m_processingActiveEffects(FALSE),
      m_curSequenceID(0),
      m_lastSequenceID(0),
      m_pendingGameActions(0),
      m_endTurnRequested(false) {
  std::fill(m_timeLog, m_timeLog + k_TIME_LOG_SIZE, 0);
}

Director::~Director(void) {}

void Director::Kill(UnitActorPtr actor) {
  actor->DumpAllActions();

  UnitActorList::iterator actorIt =
      std::find_if(m_activeUnitList.begin(), m_activeUnitList.end(),
                   [&](UnitActorPtr& a) { return a == actor; });

  if (actorIt != m_activeUnitList.end()) {
    if (m_processingActiveUnits) {
      actor->SetKillNow();
    } else {
      m_activeUnitList.erase(actorIt);
    }
  }
}

void Director::FastKill(UnitActorPtr actor) {
  actor->DumpAllActions();

  UnitActorList::iterator actorIt =
      std::find_if(m_activeUnitList.begin(), m_activeUnitList.end(),
                   [&](UnitActorPtr& a) { return a == actor; });

  if (actorIt != m_activeUnitList.end()) {
    if (m_processingActiveUnits) {
      actor->SetKillNow();
    } else {
      m_activeUnitList.erase(actorIt);
    }
  }
}

void Director::FastKill(EffectActor* actor) {
  EffectActorList::iterator actorIt =
      std::find(m_activeEffectList.begin(), m_activeEffectList.end(), actor);

  if (actorIt != m_activeEffectList.end()) {
    if (m_processingActiveEffects) {
      actor->SetKillNow();
    } else {
      m_activeEffectList.erase(actorIt);
      delete actor;
    }
  }
}

void Director::UpdateTimingClock(void) {
  sint32 elapsed;

  if (m_lastTickCount == 0) {
    elapsed = 1000 / k_DEFAULT_FPS;
  } else {
    elapsed = GetTickCount() - m_lastTickCount;
  }

  if (elapsed > k_ELAPSED_CEILING)
    elapsed = k_ELAPSED_CEILING;

  m_lastTickCount = GetTickCount();

  m_timeLog[m_timeLogIndex++] = elapsed;

  if (m_timeLogIndex >= k_TIME_LOG_SIZE) {
    sint32 timeSum = 0;
    for (sint32 i = 0; i < k_TIME_LOG_SIZE; i++)
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

void Director::Process(void) {
  UpdateTimingClock();

  static uint32 nextTime = 0;

  if (GetTickCount() > nextTime) {
    HandleNextAction();

    ProcessActiveUnits();
    ProcessActiveEffects();

    ProcessTradeRouteAnimations();
    if (g_tiledMap)
      g_tiledMap->ProcessLayerSprites(g_tiledMap->GetMapViewRect(), 0);

    nextTime = GetTickCount() + 75;
  }
}

void Director::PauseDirector(BOOL pause) {
  m_paused = pause;
}

#ifdef _DEBUG

void Director::DumpItem(DQItem* item) {
  switch (item->m_type) {
    case DQITEM_MOVE: {
      DQActionMove* action = (DQActionMove*)item->m_action;

      DPRINTF(k_DBG_UI, ("Move\n"));
      DPRINTF(k_DBG_UI, ("  move_actor         :%#.8lx\n", action->move_actor));
      DPRINTF(k_DBG_UI, ("  move_oldPos        :%d,%d\n", action->move_oldPos.x,
                         action->move_oldPos.y));
      DPRINTF(k_DBG_UI, ("  move_newPos        :%d,%d\n", action->move_newPos.x,
                         action->move_newPos.y));
      DPRINTF(k_DBG_UI, ("  moveArraySize      :%" PRIu64 "\n",
                         action->moveActors.size()));
      DPRINTF(k_DBG_UI, ("  moveActors         :"));
      for (UnitActorWeakPtr a : action->moveActors) {
        DPRINTF(k_DBG_UI, ("%#.8lx  ", a.lock().get()));
      }
      DPRINTF(k_DBG_UI, ("\n"));
      DPRINTF(k_DBG_UI, ("  numRevealed        :%" PRIu64 "\n",
                         action->revealedActors.size()));
      DPRINTF(k_DBG_UI, ("  revealedActors     :"));
      for (UnitActorWeakPtr a : action->revealedActors) {
        DPRINTF(k_DBG_UI, ("%#.8lx  ", a.lock().get()));
      }
      DPRINTF(k_DBG_UI, ("\n"));
      DPRINTF(k_DBG_UI, ("  move_soundID       :%d\n", action->move_soundID));
    } break;
    case DQITEM_MOVEPROJECTILE: {
      DQActionMoveProjectile* action = (DQActionMoveProjectile*)item->m_action;

      DPRINTF(k_DBG_UI, ("Move Projectile\n"));
      DPRINTF(k_DBG_UI,
              ("  pshooting_actor    :%#.8lx\n", action->pshooting_actor));
      DPRINTF(k_DBG_UI,
              ("  ptarget_actor      :%#.8lx\n", action->ptarget_actor));
      DPRINTF(k_DBG_UI, ("  pmove_oldPos       :%d,%d\n",
                         action->pmove_oldPos.x, action->pmove_oldPos.y));
      DPRINTF(k_DBG_UI, ("  pmove_newPos       :%d,%d\n",
                         action->pmove_newPos.x, action->pmove_newPos.y));
      DPRINTF(k_DBG_UI,
              ("  end_projectile     :%#.8lx\n", action->end_projectile));
      DPRINTF(k_DBG_UI,
              ("  projectile_path    :%d\n", action->projectile_path));
    } break;
    case DQITEM_SPECEFFECT: {
      DQActionSpecialEffect* action = (DQActionSpecialEffect*)item->m_action;

      DPRINTF(k_DBG_UI, ("Special Effect\n"));
      DPRINTF(k_DBG_UI, ("  speceffect_pos       :%d,%d\n",
                         action->speceffect_pos.x, action->speceffect_pos.y));
      DPRINTF(k_DBG_UI,
              ("  speceffect_spriteID    :%d\n", action->speceffect_spriteID));
      DPRINTF(k_DBG_UI,
              ("  speceffect_soundID    :%d\n", action->speceffect_soundID));
    } break;
    case DQITEM_ATTACK: {
      DQActionAttack* action = (DQActionAttack*)item->m_action;

      DPRINTF(k_DBG_UI, ("Attack\n"));
      DPRINTF(k_DBG_UI,
              ("  attacker			:%#.8lx\n", action->attacker));
      DPRINTF(k_DBG_UI,
              ("  defender			:%#.8lx\n", action->defender));
      DPRINTF(k_DBG_UI, ("  attacker_Pos      :%d,%d\n", action->attacker_Pos.x,
                         action->attacker_Pos.y));
      DPRINTF(k_DBG_UI, ("  defender_Pos      :%d,%d\n", action->defender_Pos.x,
                         action->defender_Pos.y));
      DPRINTF(k_DBG_UI, ("  attacker_soundID  :%d\n", action->attacker_ID));
      DPRINTF(k_DBG_UI, ("  defender_soundID  :%d\n", action->defender_ID));
    } break;
    case DQITEM_ATTACKPOS: {
      DQActionAttackPos* action = (DQActionAttackPos*)item->m_action;

      DPRINTF(k_DBG_UI, ("Attack Pos\n"));
      DPRINTF(k_DBG_UI, ("  attackpos_attacker     :%#.8lx\n",
                         action->attackpos_attacker));
      DPRINTF(k_DBG_UI, ("  attackpos_attacker_pos :%d,%d\n",
                         action->attackpos_attacker_pos.x,
                         action->attackpos_attacker_pos.y));
      DPRINTF(k_DBG_UI,
              ("  attackpos_target_pos   :%d,%d\n",
               action->attackpos_target_pos.x, action->attackpos_target_pos.y));
      DPRINTF(k_DBG_UI,
              ("  attackpos_soundID      :%d\n", action->attackpos_soundID));
    } break;
    case DQITEM_SPECATTACK: {
      DQActionAttack* action = (DQActionAttack*)item->m_action;

      DPRINTF(k_DBG_UI, ("Special Attack\n"));
      DPRINTF(k_DBG_UI, ("  sa_attacker        ::%#.8lx\n", action->attacker));
      DPRINTF(k_DBG_UI, ("  sa_attacked        ::%#.8lx\n", action->defender));
      DPRINTF(k_DBG_UI, ("  sa_attacker_pos    ::%d,%d\n",
                         action->attacker_Pos.x, action->attacker_Pos.y));
      DPRINTF(k_DBG_UI, ("  sa_attacked_pos    ::%d,%d\n",
                         action->defender_Pos.x, action->defender_Pos.y));
      DPRINTF(k_DBG_UI, ("  sa_soundID         ::%d\n", action->attacker_ID));
      DPRINTF(k_DBG_UI, ("  sa_spriteID        ::%d\n", action->defender_ID));
    } break;
    case DQITEM_DEATH: {
      DQActionDeath* action = (DQActionDeath*)item->m_action;

      DPRINTF(k_DBG_UI, ("Death\n"));
      DPRINTF(k_DBG_UI, ("  death_dead         :%#.8lx\n", action->death_dead));
      DPRINTF(k_DBG_UI,
              ("  death_victor       :%#.8lx\n", action->death_victor));
      DPRINTF(k_DBG_UI, ("  victor_Pos         :%d,%d\n", action->victor_Pos.x,
                         action->victor_Pos.y));
      DPRINTF(k_DBG_UI, ("  dead_Pos           :%d,%d\n", action->dead_Pos.x,
                         action->dead_Pos.y));
      DPRINTF(k_DBG_UI, ("  dead_soundID       :%d\n", action->dead_soundID));
      DPRINTF(k_DBG_UI, ("  victor_soundID     :%d\n", action->victor_soundID));
    } break;
    case DQITEM_MORPH: {
      DQActionMorph* action = (DQActionMorph*)item->m_action;

      DPRINTF(k_DBG_UI, ("Morph\n"));
      DPRINTF(k_DBG_UI,
              ("  morphing_actor     :%#.8lx\n", action->morphing_actor));
      DPRINTF(k_DBG_UI, ("  ss                 :%#.8lx (%d)\n", action->ss,
                         action->ss->GetIndex()));
      DPRINTF(k_DBG_UI, ("  type               :%d\n", action->type));
      DPRINTF(k_DBG_UI, ("  id                 :%#.8lx\n", action->id));
    } break;
    case DQITEM_HIDE: {
      DQActionHideShow* action = (DQActionHideShow*)item->m_action;

      DPRINTF(k_DBG_UI, ("Hide\n"));
      DPRINTF(k_DBG_UI,
              ("  hiding_actor       :%#.8lx\n", action->hiding_actor));
      DPRINTF(k_DBG_UI, ("  hiding_pos         :%d,%d\n", action->hiding_pos.x,
                         action->hiding_pos.y));
    } break;
    case DQITEM_SHOW: {
      DQActionHideShow* action = (DQActionHideShow*)item->m_action;

      DPRINTF(k_DBG_UI, ("Show\n"));
      DPRINTF(k_DBG_UI,
              ("  hiding_actor       :%#.8lx\n", action->hiding_actor));
      DPRINTF(k_DBG_UI, ("  hiding_pos         :%d,%d\n", action->hiding_pos.x,
                         action->hiding_pos.y));
    } break;
    case DQITEM_WORK: {
      DQActionWork* action = (DQActionWork*)item->m_action;

      DPRINTF(k_DBG_UI, ("Work\n"));
      DPRINTF(k_DBG_UI,
              ("  working_actor      :%#.8lx\n", action->working_actor));
      DPRINTF(k_DBG_UI, ("  working_pos        :%d,%d\n", action->working_pos.x,
                         action->working_pos.y));
      DPRINTF(k_DBG_UI,
              ("  working_soundID    :%d\n", action->working_soundID));
    } break;
    case DQITEM_FASTKILL: {
      DQActionFastKill* action = (DQActionFastKill*)item->m_action;

      DPRINTF(k_DBG_UI, ("Fast Kill\n"));
      DPRINTF(k_DBG_UI, ("  dead               :%#.8lx\n", action->dead.get()));
    } break;
    case DQITEM_ADDVISION: {
      DQActionVision* action = (DQActionVision*)item->m_action;

      DPRINTF(k_DBG_UI, ("Add Vision\n"));
      DPRINTF(k_DBG_UI, ("  vision_pos         :%d,%d\n", action->vision_pos.x,
                         action->vision_pos.y));
      DPRINTF(k_DBG_UI,
              ("  vision_range       :%#.2f\n", action->vision_range));
    } break;
    case DQITEM_REMOVEVISION: {
      DQActionVision* action = (DQActionVision*)item->m_action;

      DPRINTF(k_DBG_UI, ("Remove Vision\n"));
      DPRINTF(k_DBG_UI, ("  vision_pos         :%d,%d\n", action->vision_pos.x,
                         action->vision_pos.y));
      DPRINTF(k_DBG_UI,
              ("  vision_range       :%#.2f\n", action->vision_range));
    } break;
    case DQITEM_SETOWNER: {
      DQActionSetOwner* action = (DQActionSetOwner*)item->m_action;

      DPRINTF(k_DBG_UI, ("Set Owner\n"));
      DPRINTF(k_DBG_UI,
              ("  setowner_actor     :%#.8lx\n", action->setowner_actor));
      DPRINTF(k_DBG_UI, ("  owner              :%d\n", action->owner));
    } break;
    case DQITEM_SETVISIBILITY: {
      DQActionSetVisibility* action = (DQActionSetVisibility*)item->m_action;

      DPRINTF(k_DBG_UI, ("Set Visibility\n"));
      DPRINTF(k_DBG_UI,
              ("  setowner_actor     :%#.8lx\n", action->setvisibility_actor));
      DPRINTF(k_DBG_UI,
              ("  owner              :%#.8lx\n", action->visibilityFlag));
    } break;
    case DQITEM_SETVISIONRANGE: {
      DQActionSetVisionRange* action = (DQActionSetVisionRange*)item->m_action;

      DPRINTF(k_DBG_UI, ("Set Vision Range\n"));
      DPRINTF(k_DBG_UI,
              ("  setvisibility_actor:%#.8lx\n", action->setvisionrange_actor));
      DPRINTF(k_DBG_UI, ("  visibilityFlag     :%#.2f\n", action->range));
    } break;
    case DQITEM_COMBATFLASH: {
      DQActionCombatFlash* action = (DQActionCombatFlash*)item->m_action;

      DPRINTF(k_DBG_UI, ("Combat Flash\n"));
      DPRINTF(k_DBG_UI, ("  flash_pos         :%d,%d\n", action->flash_pos.x,
                         action->flash_pos.y));
    } break;
    case DQITEM_TELEPORT: {
      DQActionMove* action = (DQActionMove*)item->m_action;

      DPRINTF(k_DBG_UI, ("Move\n"));
      DPRINTF(k_DBG_UI,
              ("  teleport_actor         :%#.8lx\n", action->move_actor));
      DPRINTF(k_DBG_UI, ("  teleport_oldPos        :%d,%d\n",
                         action->move_oldPos.x, action->move_oldPos.y));
      DPRINTF(k_DBG_UI, ("  teleport_newPos        :%d,%d\n",
                         action->move_newPos.x, action->move_newPos.y));
      DPRINTF(k_DBG_UI, ("  teleport_moveArraySize      :%" PRIu64 "\n",
                         action->moveActors.size()));
      DPRINTF(k_DBG_UI, ("  teleport_moveActors         :"));
      for (auto a : action->moveActors) {
        DPRINTF(k_DBG_UI, ("%#.8lx  ", a.lock().get()));
      }
      DPRINTF(k_DBG_UI, ("\n"));
      DPRINTF(k_DBG_UI, ("  teleport_numRevelead        :%" PRIu64 "\n",
                         action->revealedActors.size()));
      DPRINTF(k_DBG_UI, ("  teleport_revealedActors     :"));

      for (auto a : action->revealedActors) {
        DPRINTF(k_DBG_UI, ("%#.8lx  ", a.lock().get()));
      }
      DPRINTF(k_DBG_UI, ("\n"));
    } break;
    case DQITEM_COPYVISION: {
      //		DQActionCopyVision *action = (DQActionCopyVision
      //*)item->m_action;

      DPRINTF(k_DBG_UI, ("Copy Vision from Gamestate\n"));
    } break;
    case DQITEM_CENTERMAP: {
      DQActionCenterMap* action = (DQActionCenterMap*)item->m_action;

      DPRINTF(k_DBG_UI, ("Center Map\n"));
      DPRINTF(k_DBG_UI, ("  centerMap_pos    :%d,%d\n", action->centerMap_pos.x,
                         action->centerMap_pos.y));
    } break;
    case DQITEM_SELECTUNIT: {
      DQActionUnitSelection* action = (DQActionUnitSelection*)item->m_action;

      DPRINTF(k_DBG_UI, ("Select Unit\n"));
      DPRINTF(k_DBG_UI, ("  flags     :%#.8lx\n", action->flags));
    } break;
    case DQITEM_ENDTURN: {
      //		DQActionEndTurn *action = (DQActionEndTurn
      //*)item->m_action;

      DPRINTF(k_DBG_UI, ("End Turn from Gamestate\n"));
    } break;
    case DQITEM_BATTLE: {
      //		DQActionBattle *action = (DQActionBattle
      //*)item->m_action;

      DPRINTF(k_DBG_UI, ("Battle\n"));
    } break;
    case DQITEM_PLAYSOUND: {
      DQActionPlaySound* action = (DQActionPlaySound*)item->m_action;

      DPRINTF(k_DBG_UI, ("Play Sound\n"));
      DPRINTF(k_DBG_UI,
              ("  playsound_soundID    :%d\n", action->playsound_soundID));
      DPRINTF(k_DBG_UI, ("  playsound_pos        :%d,%d\n",
                         action->playsound_pos.x, action->playsound_pos.y));
    } break;
    case DQITEM_PLAYWONDERMOVIE: {
      DQActionPlayWonderMovie* action =
          (DQActionPlayWonderMovie*)item->m_action;

      DPRINTF(k_DBG_UI, ("Play Wonder Movie\n"));
      DPRINTF(k_DBG_UI,
              ("  which                :%ld\n", action->playwondermovie_which));
    } break;
    case DQITEM_PLAYVICTORYMOVIE: {
      DQActionPlayVictoryMovie* action =
          (DQActionPlayVictoryMovie*)item->m_action;

      DPRINTF(k_DBG_UI, ("Play Victory Movie\n"));
      DPRINTF(k_DBG_UI, ("  reason                :%ld\n",
                         action->playvictorymovie_reason));
    } break;
    case DQITEM_MESSAGE: {
      //		DQActionMessage *action = (DQActionMessage
      //*)item->m_action;

      DPRINTF(k_DBG_UI, ("Message from Gamestate\n"));
    } break;
    case DQITEM_FACEOFF: {
      DQActionFaceoff* action = (DQActionFaceoff*)item->m_action;

      DPRINTF(k_DBG_UI, ("Faceoff\n"));
      DPRINTF(k_DBG_UI,
              ("  faceoff_attacker   :%#.8lx\n", action->faceoff_attacker));
      DPRINTF(k_DBG_UI,
              ("  faceoff_attacked   :%#.8lx\n", action->faceoff_attacked));
      DPRINTF(k_DBG_UI,
              ("  faceoff_attacker_pos :%d,%d\n",
               action->faceoff_attacker_pos.x, action->faceoff_attacker_pos.y));
      DPRINTF(k_DBG_UI,
              ("  faceoff_attacked_pos :%d,%d\n",
               action->faceoff_attacked_pos.x, action->faceoff_attacked_pos.y));
    } break;
    case DQITEM_TERMINATE_FACEOFF: {
      DQActionTerminateFaceOff* action =
          (DQActionTerminateFaceOff*)item->m_action;

      DPRINTF(k_DBG_UI, ("Terminate Faceoff\n"));
      DPRINTF(k_DBG_UI, ("  faceroffer   :%#.8lx\n", action->faceroffer));
    } break;
    case DQITEM_TERMINATE_SOUND: {
      DQActionTerminateSound* action = (DQActionTerminateSound*)item->m_action;

      DPRINTF(k_DBG_UI, ("Terminate Sound\n"));
      DPRINTF(k_DBG_UI, ("  terminate_sound_unit    :%#.8lx\n",
                         action->terminate_sound_unit));
      break;
    }
    case DQITEM_BEGIN_SCHEDULER: {
      DQActionBeginScheduler* action = (DQActionBeginScheduler*)item->m_action;
      DPRINTF(k_DBG_UI, ("Begin Scheduler\n"));
      DPRINTF(k_DBG_UI, ("  player: %d\n", action->player));
      break;
    }
  }
}

#endif

#ifdef _DEBUG

void Director::DumpInfo(void) {
  DPRINTF(k_DBG_UI, (" ------------------\n"));
  DPRINTF(k_DBG_UI, ("Director Dump:\n"));
  DPRINTF(k_DBG_UI, (" m_curSequenceID  :%d\n", m_curSequenceID));
  DPRINTF(k_DBG_UI, (" m_lastSequenceID :%d\n", m_lastSequenceID));
  DPRINTF(k_DBG_UI, (" ------------------\n"));
  DPRINTF(k_DBG_UI, (" Dispatched Items:\n"));
  DPRINTF(k_DBG_UI, (" Count:%" PRIu64 "\n", m_dispatchedItems.size()));
  DPRINTF(k_DBG_UI, (" ------------------\n"));

  for (const DQItemPtr& i : m_dispatchedItems) {
    DumpItem(i.get());
  }

  DPRINTF(k_DBG_UI, (" ------------------\n"));
  DPRINTF(k_DBG_UI, (" Active Units:\n"));
  DPRINTF(k_DBG_UI, (" Count:%" PRIu64 "\n", m_activeUnitList.size()));
  DPRINTF(k_DBG_UI, (" ------------------\n"));

  for (UnitActorPtr a : m_activeUnitList) {
    a->DumpActor();
  }

  DPRINTF(k_DBG_UI, (" ------------------\n"));
  DPRINTF(k_DBG_UI, (" Queued Items:\n"));
  DPRINTF(k_DBG_UI, (" Count:%" PRIu64 "\n", m_itemQueue.size()));
  DPRINTF(k_DBG_UI, (" ------------------\n"));

  for (const DQItemPtr& i : m_itemQueue) {
    DumpItem(i.get());
  }

  DPRINTF(k_DBG_UI, (" ------------------\n"));
}
#endif

void Director::HandleNextAction(void) {
  if (m_paused)
    return;

  while (!m_itemQueue.empty() && GetActionFinished()) {
    DQItemPtr item = m_itemQueue.front();
    m_itemQueue.pop_front();

    SetActionFinished(FALSE);
    m_dispatchedItems.push_back(item);
    DHEXECUTE executeType = DHEXECUTE_NORMAL;

    if (item->m_round < g_turn->GetRound() - 1 ||
        (!g_theProfileDB->IsEnemyMoves() && item->GetOwner() != -1 &&
         item->GetOwner() != g_selected_item->GetVisiblePlayer()) ||
        (!g_theProfileDB->IsUnitAnim() && item->GetOwner() != -1 &&
         g_player[item->GetOwner()] != NULL &&
         g_player[item->GetOwner()]->IsRobot())) {
      executeType = DHEXECUTE_IMMEDIATE;
    }

    Assert(item->m_handler != NULL);
    item->m_handler(item->m_action, item->getSequence(), executeType);

    if (item->m_type == DQITEM_ADDVISION || item->m_type == DQITEM_REMOVEVISION)
      HandleNextAction();
  }
}

void Director::ActionFinished(SequenceWeakPtr seq) {
  if (seq.expired()) {
    if (!m_dispatchedItems.empty()) {
      DQItemPtr item = m_dispatchedItems.front();
      m_dispatchedItems.pop_front();
      Assert(false);
    }

    SetActionFinished(TRUE);
    return;
  }

  SequencePtr sequence(seq);

  if (sequence->GetSequenceID() == m_lastSequenceID) {
    sequence->Release();
    if (sequence->GetRefCount() <= 0) {
      SaveFinishedItem(sequence->GetItem());
      SetActionFinished(TRUE);
    }
    return;
  }

  DQItemPtr item;
  if (!m_dispatchedItems.empty()) {
    item = m_dispatchedItems.front();
    m_dispatchedItems.pop_front();
  } else {
    return;
  }

  if (item->getSequence().lock()->GetSequenceID() ==
      sequence->GetSequenceID()) {
    m_lastSequenceID = sequence->GetSequenceID();
    sequence->Release();
    if (sequence->GetRefCount() <= 0) {
      SaveFinishedItem(item);
      SetActionFinished(TRUE);
    }
  } else {
    if (item->getSequence().lock()->GetSequenceID() <
        sequence->GetSequenceID()) {
      m_lastSequenceID = sequence->GetSequenceID();
      // @TODO check if we really want do this
      // delete m_dispatchedItems->RemoveHead();
      m_dispatchedItems.pop_front();
    }

    SetActionFinished(TRUE);
  }
}

SequencePtr Director::NewSequence(void) {
  return std::make_shared<Sequence>(++m_curSequenceID);
}

void Director::HandleFinishedItem(DQItemPtr item) {
  Assert(item);

  SequencePtr seq = item->getSequence().lock();

  const bool removePrimaryFromActiveList =
      seq->GetAddedToActiveList(SEQ_ACTOR_PRIMARY);
  const bool removeSecondaryFromActiveList =
      seq->GetAddedToActiveList(SEQ_ACTOR_SECONDARY);

  switch (item->m_type) {
    case DQITEM_MOVE: {
      DQActionMove* action = (DQActionMove*)item->m_action;
      if (removePrimaryFromActiveList)
        if (action && !action->move_actor.expired()) {
          ActiveUnitRemove(action->move_actor.lock());
          if (!action->move_actor.lock()->IsActive())
            g_soundManager->TerminateLoopingSound(
                SOUNDTYPE_SFX, action->move_actor.lock()->GetUnitID());
        }
    } break;
    case DQITEM_ATTACKPOS: {
      DQActionAttackPos* action = (DQActionAttackPos*)item->m_action;
      if (removePrimaryFromActiveList)
        if (action && !action->attackpos_attacker.expired()) {
          ActiveUnitRemove(action->attackpos_attacker.lock());
          if (!action->attackpos_attacker.lock()->IsActive())
            g_soundManager->TerminateLoopingSound(
                SOUNDTYPE_SFX, action->attackpos_attacker.lock()->GetUnitID());
        }
    } break;
    case DQITEM_SPECATTACK: {
      DQActionAttack* action = (DQActionAttack*)item->m_action;
      if (removePrimaryFromActiveList) {
        if (action && !action->attacker.expired()) {
          ActiveUnitRemove(action->attacker.lock());
          if (!action->attacker.lock()->IsActive())
            g_soundManager->TerminateLoopingSound(
                SOUNDTYPE_SFX, action->attacker.lock()->GetUnitID());
        }
      }
      if (removeSecondaryFromActiveList) {
        if (action && !action->defender.expired()) {
          ActiveUnitRemove(action->defender.lock());
          if (!action->defender.lock()->IsActive())
            g_soundManager->TerminateLoopingSound(
                SOUNDTYPE_SFX, action->defender.lock()->GetUnitID());
        }
      }
    } break;
    case DQITEM_DEATH: {
      DQActionDeath* action = (DQActionDeath*)item->m_action;
      if (removePrimaryFromActiveList) {
        if (action && action->death_dead) {
          ActiveUnitRemove(action->death_dead);
          if (!action->death_dead->IsActive())
            g_soundManager->TerminateLoopingSound(SOUNDTYPE_SFX,
                                                  action->dead_id);
        }
      }
      if (removeSecondaryFromActiveList) {
        if (action && !action->death_victor.expired()) {
          ActiveUnitRemove(action->death_victor.lock());
          if (!action->death_victor.lock()->IsActive())
            g_soundManager->TerminateLoopingSound(SOUNDTYPE_SFX,
                                                  action->victor_id);
        }
      }
    } break;
    case DQITEM_WORK: {
      DQActionWork* action = (DQActionWork*)item->m_action;
      if (removePrimaryFromActiveList)
        if (action && !action->working_actor.expired()) {
          ActiveUnitRemove(action->working_actor.lock());
          if (!action->working_actor.lock()->IsActive())
            g_soundManager->TerminateLoopingSound(
                SOUNDTYPE_SFX, action->working_actor.lock()->GetUnitID());
        }
    } break;
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
    default: {
    }
  }
}

void Director::SaveFinishedItem(DQItemPtr item) {
  if (item->m_addedToSavedList)
    return;

  item->m_addedToSavedList = TRUE;

  m_savedItems.push_back(item);
}

void Director::GarbageCollectItems(void) {
  while (!m_savedItems.empty()) {
    DQItemPtr item = m_savedItems.front();
    m_savedItems.pop_front();
    HandleFinishedItem(item);
  }
}

void Director::ProcessImmediately(DQItemPtr item) {
  item->m_handler(item->m_action, item->getSequence(), DHEXECUTE_IMMEDIATE);
}

void Director::CatchUp(void) {
  m_activeUnitList.erase(
      std::remove_if(m_activeUnitList.begin(), m_activeUnitList.end(),
                     [](UnitActorPtr a) { return a->WillDie(); }),
      m_activeUnitList.end());

  for (UnitActorPtr& a : m_activeUnitList) {
    a->EndTurnProcess();
  }

  while (!m_itemQueue.empty()) {
    DQItemPtr item = m_itemQueue.front();
    m_itemQueue.pop_front();

    m_dispatchedItems.push_back(item);

    Assert(item->m_handler != NULL);
    item->m_handler(item->m_action, item->getSequence(), DHEXECUTE_IMMEDIATE);
  }

  m_activeUnitList.erase(
      std::remove_if(m_activeUnitList.begin(), m_activeUnitList.end(),
                     [](UnitActorPtr a) { return a->WillDie(); }),
      m_activeUnitList.end());

  for (UnitActorPtr& a : m_activeUnitList) {
    a->EndTurnProcess();
  }

  GarbageCollectItems();

  if (!g_network.IsActive()) {
    KillAllActiveEffects();
    if (g_soundManager)
      g_soundManager->TerminateAllLoopingSounds(SOUNDTYPE_SFX);
  }
}

bool Director::CaughtUp(void) {
  return m_itemQueue.empty();
}

bool Director::TileIsVisibleToPlayer(MapPoint& pos) {
#if defined(_PLAYTEST)
  if (g_doingFastRounds)
    return false;
#endif

  return g_tiledMap && g_tiledMap->GetLocalVision()->IsVisible(pos);
}

void Director::ActiveUnitAdd(UnitActorPtr unitActor) {
  if (unitActor == NULL)
    return;

  if (!unitActor->IsActive())
    m_activeUnitList.push_front(unitActor);

  unitActor->SetActive(true);

  unitActor->AddActiveListRef();
}

void Director::ActiveUnitRemove(UnitActorPtr unitActor) {
  if (!unitActor)
    return;

  UnitActorList::const_iterator removeIt =
      std::find(m_activeUnitList.begin(), m_activeUnitList.end(), unitActor);

  if (removeIt != m_activeUnitList.end() &&
      unitActor->ReleaseActiveListRef() <= 0) {
    m_activeUnitList.erase(removeIt);
    unitActor->SetActive(false);
    // Prevent getting stuck waiting for a deleted item.
    if (m_activeUnitList.empty()) {
      SetActionFinished(TRUE);
    }
  }
}

void Director::ActiveEffectAdd(EffectActor* effectActor) {
  Assert(effectActor);

  if (std::find(m_activeEffectList.begin(), m_activeEffectList.end(),
                effectActor) == m_activeEffectList.end()) {
    m_activeEffectList.push_front(effectActor);
  }
}

void Director::ActiveEffectRemove(EffectActor* effectActor) {
  EffectActorList::const_iterator it = std::find(
      m_activeEffectList.begin(), m_activeEffectList.end(), effectActor);
  if (it != m_activeEffectList.end()) {
    m_activeEffectList.erase(it);
  }
}

void Director::TradeActorCreate(TradeRoute newRoute) {
  if (std::find_if(m_tradeActorList.begin(), m_tradeActorList.end(),
                   [&](TradeActor* t) {
                     return t->GetRouteID() == newRoute;
                   }) == m_tradeActorList.end()) {
    m_tradeActorList.push_front(new TradeActor(newRoute));
  }
}

void Director::TradeActorDestroy(TradeRoute routeToDestroy) {
  TradeActorList::const_iterator removeIt = std::find_if(
      m_tradeActorList.begin(), m_tradeActorList.end(),
      [&](TradeActor* t) { return t->GetRouteID() == routeToDestroy; });

  if (removeIt != m_tradeActorList.end()) {
    delete (*removeIt);
    m_tradeActorList.erase(removeIt);
  }
}

#if 0
BOOL Director::IsActive(UnitActorPtr unitActor)
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

uint32 Director::ProcessActiveUnits(void) {
  if (m_activeUnitList.empty())
    return 0;

  m_processingActiveUnits = TRUE;

  for (UnitActorPtr& uActor : m_activeUnitList) {
    uActor->Process();
  }

  m_processingActiveUnits = FALSE;

  UnitActorList toRemove;
  std::copy_if(m_activeUnitList.begin(), m_activeUnitList.end(),
               std::back_inserter(toRemove),
               [](UnitActorPtr& u) { return u->GetKillNow(); });

  m_activeUnitList.erase(
      std::remove_if(m_activeUnitList.begin(), m_activeUnitList.end(),
                     [](UnitActorPtr& u) { return u->GetKillNow(); }),
      m_activeUnitList.end());

  for (UnitActorPtr& i : toRemove) {
    i->DumpAllActions();
  }

  return 0;
}

uint32 Director::ProcessActiveEffects(void) {
  if (m_activeEffectList.empty())
    return 0;

  m_processingActiveEffects = TRUE;

  for (EffectActor* eActor : m_activeEffectList) {
    eActor->Process();
  }

  m_processingActiveEffects = FALSE;

  EffectActorList::iterator toRemoveIt =
      std::remove_if(m_activeEffectList.begin(), m_activeEffectList.end(),
                     [](EffectActor* e) { return e->GetKillNow(); });

  for (EffectActorList::iterator i = toRemoveIt; i != m_activeEffectList.end();
       ++i) {
    delete *i;
  }

  m_activeEffectList.erase(toRemoveIt, m_activeEffectList.end());

  return 0;
}

void Director::ProcessTradeRouteAnimations(void) {
  if (!g_theProfileDB->IsTradeAnim())
    return;

  for (TradeActor* tActor : m_tradeActorList) {
    tActor->Process();
  }
}

void Director::OffsetActiveUnits(sint32 deltaX, sint32 deltaY) {
  for (UnitActorPtr& uActor : m_activeUnitList) {
    uActor->SetX(uActor->GetX() + deltaX);
    uActor->SetY(uActor->GetY() + deltaY);
  }
}

void Director::OffsetActiveEffects(sint32 deltaX, sint32 deltaY) {
  for (EffectActor* eActor : m_activeEffectList) {
    eActor->SetX(eActor->GetX() + deltaX);
    eActor->SetY(eActor->GetY() + deltaY);
  }
}

void Director::OffsetTradeRouteAnimations(sint32 deltaX, sint32 deltaY) {
  for (TradeActor* tActor : m_tradeActorList) {
    tActor->SetX(tActor->GetX() + deltaX);
    tActor->SetY(tActor->GetY() + deltaY);
  }
}

uint32 Director::KillAllActiveEffects() {
  for (EffectActor* eActor : m_activeEffectList) {
    delete eActor;
  }
  m_activeEffectList.clear();

  return 0;
}

void Director::NextPlayer(BOOL forcedUpdate) {
#ifdef _PLAYTEST
  if (!g_doingFastRounds &&
      (!g_network.IsActive() ||
       g_player[g_selected_item->GetVisiblePlayer()]->IsRobot())) {
    return;
  }
#else
  return;  // Next code isn't used, should it be used?
#endif
  m_nextPlayer = TRUE;

  m_activeUnitList.erase(
      std::remove_if(m_activeUnitList.begin(), m_activeUnitList.end(),
                     [](UnitActorPtr a) { return a->WillDie(); }),
      m_activeUnitList.end());

  for (UnitActorPtr& a : m_activeUnitList) {
    a->EndTurnProcess();
  }

  while (!m_itemQueue.empty()) {
    DQItemPtr item = m_itemQueue.front();
    m_itemQueue.pop_front();

    m_dispatchedItems.push_back(item);

    Assert(item->m_handler != NULL);
    item->m_handler(item->m_action, item->getSequence(), DHEXECUTE_NORMAL);
  }

  if (!g_network.IsActive() || forcedUpdate) {
    m_activeUnitList.erase(
        std::remove_if(m_activeUnitList.begin(), m_activeUnitList.end(),
                       [](UnitActorPtr a) { return a->WillDie(); }),
        m_activeUnitList.end());

    for (UnitActorPtr& a : m_activeUnitList) {
      a->EndTurnProcess();
    }
  }

  if (!g_network.IsActive()) {
    if (g_tiledMap)
      g_tiledMap->NextPlayer();

    KillAllActiveEffects();

    if (g_soundManager)
      g_soundManager->TerminateAllLoopingSounds(SOUNDTYPE_SFX);
  }
}

void Director::DrawActiveUnits(RECT* paintRect, sint32 layer) {
  m_nextPlayer = FALSE;

  for (UnitActorPtr& uActor : m_activeUnitList) {
    MapPoint pos = uActor->GetPos();

    sint32 tileX;
    maputils_MapX2TileX(pos.x, pos.y, &tileX);

    if (maputils_TilePointInTileRect(tileX, pos.y, paintRect)) {
      if (uActor->GetUnitVisibility() &
          (1 << g_selected_item->GetVisiblePlayer())) {
        g_tiledMap->PaintUnitActor(uActor);
      }
    }
  }
}

void Director::DrawActiveEffects(RECT* paintRect, sint32 layer) {
  EffectActorList::iterator toRemoveIt = std::remove_if(
      m_activeEffectList.begin(), m_activeEffectList.end(), [](EffectActor* e) {
        return e->GetActionQueueNumItems() <= 0 && e->GetKillNow();
      });

  for (EffectActorList::iterator i = toRemoveIt; i != m_activeEffectList.end();
       ++i) {
    delete *i;
  }

  m_activeEffectList.erase(toRemoveIt, m_activeEffectList.end());

  for (EffectActor* actor : m_activeEffectList) {
    MapPoint pos = actor->GetPos();
    sint32 tileX = 0;

    maputils_MapX2TileX(pos.x, pos.y, &tileX);

    if (!maputils_TilePointInTileRect(tileX, pos.y, paintRect))
      continue;

    if (actor->GetCurAction())
      g_tiledMap->PaintEffectActor(actor);
  }
}

void Director::DrawTradeRouteAnimations(RECT* paintRect, sint32 layer) {
  if (!g_theProfileDB->IsTradeAnim())
    return;

  for (TradeActor* tActor : m_tradeActorList) {
    MapPoint pos = tActor->GetCurrentPos();
    sint32 tileX = 0;

    maputils_MapX2TileX(pos.x, pos.y, &tileX);

    if (!maputils_TilePointInTileRect(tileX, pos.y, paintRect))
      continue;

    tActor->Draw(g_tiledMap->GetLocalVision());

    RECT tempRect;
    tActor->GetBoundingRect(&tempRect);
    g_tiledMap->AddDirtyRectToMix(tempRect);
  }
}

UnitActorPtr Director::GetClickedActiveUnit(aui_MouseEvent* data) {
  POINT mouse = data->position;

  for (UnitActorPtr& actor : m_activeUnitList) {
    actor->Process();
    MapPoint pos = actor->GetPos();
    if (g_tiledMap->TileIsVisible(pos.x, pos.y)) {
      RECT actorRect;

      SetRect(&actorRect, actor->GetX(), actor->GetY(),
              actor->GetX() + (sint32)actor->GetWidth(),
              actor->GetY() + (sint32)actor->GetHeight());

      if (PtInRect(&actorRect, mouse)) {
        return actor;
      }
    }
  }

  return UnitActorPtr();
}

bool Director::IsProcessing() {
  return (!m_itemQueue.empty());
}

void Director::AddMoveProcess(UnitActorPtr top,
                              UnitActorPtr dest,
                              sint32 arraySize,
                              UnitActor** moveActors,
                              BOOL isTransported) {}

void Director::AddMove(Unit mover,
                       MapPoint const& oldPos,
                       MapPoint const& newPos,
                       const UnitActorVec& revealedActors,
                       const UnitActorVec& restOfStack,
                       bool isTransported,
                       sint32 soundID) {
  UnitActorPtr actor = mover.GetActor();

  Assert(!mover.IsCity());

  Assert(actor->GetUnitID() == mover.m_id);

  if (g_theProfileDB->IsEnemyMoves() &&
      mover.GetOwner() != g_selected_item->GetVisiblePlayer() &&
      (mover.GetVisibility() & (1 << g_selected_item->GetVisiblePlayer())) &&
      !TileWillBeCompletelyVisible(newPos.x, newPos.y)) {
    AddCenterMap(newPos);
  }

  actor->SetHiddenUnderStack(FALSE);

  if (actor->GetIsTransported() == FALSE && isTransported) {
    actor->SetIsTransported(k_TRANSPORTREMOVEONLY);
  } else if (actor->GetIsTransported() == k_TRANSPORTREMOVEONLY &&
             !isTransported) {
    actor->SetIsTransported(k_TRANSPORTADDONLY);
  }

  DQActionMove* action = new DQActionMove;
  DQItemPtr item(DQItem::CreatePtr(DQITEM_MOVE, action, dh_move));

  item->SetOwner(mover.GetOwner());

  action->move_actor = actor;
  action->move_oldPos = oldPos;
  action->move_newPos = newPos;
  action->move_soundID = soundID;

  if (!restOfStack.empty()) {
    action->moveActors = restOfStack;
  }
  action->revealedActors = revealedActors;

  m_itemQueue.push_back(item);
}

void Director::AddTeleport(Unit top,
                           MapPoint const& oldPos,
                           MapPoint const& newPos,
                           const UnitActorVec& revealedActors,
                           const UnitActorVec& moveActors) {
  DQActionMove* action = new DQActionMove;
  DQItemPtr item(DQItem::CreatePtr(DQITEM_TELEPORT, action, dh_teleport));
  item->SetOwner(top.GetOwner());

  action->move_actor = top.GetActor();
  action->move_oldPos = oldPos;
  action->move_newPos = newPos;
  action->moveActors = moveActors;
  action->revealedActors = revealedActors;

  m_itemQueue.push_back(item);
}

void Director::AddProjectileAttack(Unit shooting,
                                   Unit target,
                                   SpriteStatePtr projectile_state,
                                   SpriteStatePtr projectileEnd_state,
                                   sint32 projectile_Path) {
  DQActionMoveProjectile* action = new DQActionMoveProjectile;
  DQItemPtr item(
      DQItem::CreatePtr(DQITEM_MOVEPROJECTILE, action, dh_projectileMove));
  item->SetOwner(shooting.GetOwner());

  EffectActor* projectileEnd =
      new EffectActor(projectileEnd_state, target.RetPos());

  action->pshooting_actor = shooting.GetActor();
  action->ptarget_actor = target.GetActor();
  action->end_projectile = projectileEnd;
  action->projectile_path = projectile_Path;
  action->pmove_oldPos = shooting.RetPos();
  action->pmove_newPos = target.RetPos();

  m_itemQueue.push_back(item);
}

void Director::AddSpecialEffect(MapPoint& pos,
                                sint32 spriteID,
                                sint32 soundID) {
  DQActionSpecialEffect* action = new DQActionSpecialEffect;
  DQItemPtr item(DQItem::CreatePtr(DQITEM_SPECEFFECT, action, dh_speceffect));

  action->speceffect_pos = pos;
  action->speceffect_spriteID = spriteID;
  action->speceffect_soundID = soundID;

  m_itemQueue.push_back(item);
}

void Director::AddCombatFlash(MapPoint const& pos) {
  DQActionCombatFlash* action = new DQActionCombatFlash;
  action->flash_pos = pos;

  DQItemPtr item(DQItem::CreatePtr(DQITEM_COMBATFLASH, action, dh_combatflash));
  m_itemQueue.push_back(item);
}

void Director::AddCopyVision(void) {
  DQActionCopyVision* action = new DQActionCopyVision;
  DQItemPtr item(DQItem::CreatePtr(DQITEM_COPYVISION, action, dh_copyVision));

  action->copyVision = TRUE;

  m_itemQueue.push_back(item);
}

void Director::AddCenterMap(const MapPoint& pos) {
  DQActionCenterMap* action = new DQActionCenterMap;
  DQItemPtr item(DQItem::CreatePtr(DQITEM_CENTERMAP, action, dh_centerMap));

  action->centerMap_pos = pos;

  m_itemQueue.push_back(item);
}

void Director::AddSelectUnit(uint32 flags) {
  DQActionUnitSelection* action = new DQActionUnitSelection;
  DQItemPtr item(DQItem::CreatePtr(DQITEM_SELECTUNIT, action, dh_selectUnit));

  action->flags = flags;

  m_itemQueue.push_back(item);
}

void Director::AddEndTurn(void) {
  DPRINTF(k_DBG_GAMESTATE, ("Director::AddEndTurn, curPlayer = %d\n",
                            g_selected_item->GetCurPlayer()));

  if (g_selected_item->GetCurPlayer() == g_selected_item->GetVisiblePlayer()) {
    static sint32 last_turn_processed = -1;
    if (last_turn_processed !=
        g_player[g_selected_item->GetCurPlayer()]->m_current_round) {
      last_turn_processed =
          g_player[g_selected_item->GetCurPlayer()]->m_current_round;

      g_gevManager->Pause();

      Player* p = g_player[g_selected_item->GetCurPlayer()];
      p->m_endingTurn = TRUE;

      for (sint32 i = 0; i < p->m_all_armies->Num(); i++) {
        //				IncrementPendingGameActions();
        g_gevManager->AddEvent(GEV_INSERT_Tail, GEV_BeginTurnExecute, GEA_Army,
                               p->m_all_armies->Access(i).m_id, GEA_End);
      }

      g_player[g_selected_item->GetCurPlayer()]->m_endingTurn = FALSE;
      g_gevManager->Resume();
    }
  }

  if (m_pendingGameActions > 0 || m_endTurnRequested) {
    DPRINTF(k_DBG_GAMESTATE,
            ("Director::AddEndTurn, but there are %d pending actions (or it "
             "was already requested), not doing it yet.\n",
             m_pendingGameActions));
    m_endTurnRequested = true;
    return;
  }

  static sint32 lastPlayer = -1;
  static sint32 lastRound = -1;

  if (g_selected_item->GetCurPlayer() == lastPlayer && g_player[lastPlayer] &&
      g_player[lastPlayer]->m_current_round == lastRound) {
    for (DQItemPtr& item : m_itemQueue) {
      if (item->m_type == DQITEM_ENDTURN) {
        DPRINTF(k_DBG_GAMESTATE, ("Skipping duplicate end turn for %d,%d\n",
                                  lastPlayer, lastRound));
        return;
      }
    }
  }

  lastPlayer = g_selected_item->GetCurPlayer();
  if (g_player[lastPlayer]) {
    lastRound = g_player[lastPlayer]->m_current_round;
  } else {
    lastRound = -1;
  }

  DQActionEndTurn* action = new DQActionEndTurn;
  DQItemPtr item(DQItem::CreatePtr(DQITEM_ENDTURN, action, dh_endTurn));

  action->endTurn = TRUE;

  m_itemQueue.push_back(item);
}

void Director::AddAttack(Unit attacker, Unit defender) {
  UnitActorPtr attackerActor;
  UnitActorPtr defenderActor;

  DQActionAttack* action = new DQActionAttack;
  DQItemPtr item(DQItem::CreatePtr(DQITEM_ATTACK, action, dh_attack));
  item->SetOwner(attacker.GetOwner());

  action->defender_IsCity = FALSE;

  if (attacker != Unit()) {
    attackerActor = attacker.GetActor();
    action->attacker = attackerActor;
    action->attacker_Pos = attacker.RetPos();
    action->attacker_ID = attacker.GetAttackSoundID();
    action->attacker_IsCity = attacker.IsCity();

    attackerActor->SetHealthPercent(attacker.GetHP() /
                                    attacker->CalculateTotalHP());

    Army army = attacker.GetArmy();
    if (army.IsValid()) {
      attackerActor->SetTempStackSize(army.Num());
    }
  }

  if (defender != Unit()) {
    defenderActor = defender.GetActor();
    action->defender = defenderActor;
    action->defender_Pos = defender.RetPos();
    action->defender_ID = defender.GetAttackSoundID();
    action->defender_IsCity = defender.IsCity();

    defenderActor->SetHealthPercent(defender.GetHP() /
                                    defender->CalculateTotalHP());

    CellUnitList* unitList;
    unitList = g_theWorld->GetCell(defender.RetPos())->UnitArmy();
    sint32 num = 1;
    if (unitList != NULL)
      num = unitList->Num();
    defenderActor->SetTempStackSize(num);
  }

  m_itemQueue.push_back(item);

  Player* visiblePlayer = g_player[g_selected_item->GetVisiblePlayer()];
  if (visiblePlayer && visiblePlayer->IsVisible(attacker.RetPos())) {
    if (attacker.m_id != 0) {
      AddCombatFlash(attacker.RetPos());
    }
  }

  if (visiblePlayer && visiblePlayer->IsVisible(defender.RetPos())) {
    if (defender.m_id != 0) {
      AddCombatFlash(defender.RetPos());
    }
  }
}

void Director::AddAttackPos(Unit attacker, MapPoint const& pos) {
  DQActionAttackPos* action = new DQActionAttackPos;
  action->attackpos_attacker = attacker.GetActor();
  action->attackpos_attacker_pos = attacker.RetPos();
  action->attackpos_target_pos = pos;
  action->attackpos_soundID = attacker.GetAttackSoundID();

  DQItemPtr item(DQItem::CreatePtr(DQITEM_ATTACKPOS, action, dh_attackpos));
  item->SetOwner(attacker.GetOwner());
  m_itemQueue.push_back(item);

  if (g_player[g_selected_item->GetVisiblePlayer()] &&
      g_player[g_selected_item->GetVisiblePlayer()]->IsVisible(pos)) {
    AddCombatFlash(pos);
  }
}

void Director::AddSpecialAttack(Unit attacker,
                                Unit attacked,
                                SPECATTACK attack) {
  sint32 soundID;
  sint32 spriteID;
  if (!attacker.GetSpecialAttackInfo(attack, &soundID, &spriteID)) {
    return;
  }

  UnitActorPtr attackerActor = attacker.IsValid() ? attacker.GetActor() : NULL;
  UnitActorPtr defenderActor = attacked.IsValid() ? attacked.GetActor() : NULL;

  DQActionAttack* action = new DQActionAttack;
  action->attacker = attackerActor;
  action->defender = defenderActor;
  action->attacker_Pos = attacker.RetPos();
  action->defender_Pos = attacked.RetPos();
  action->attacker_IsCity = attacker.IsCity();
  action->defender_IsCity = attacked.IsCity();
  action->attacker_ID = soundID;
  action->defender_ID = spriteID;

  DQItemPtr item(
      DQItem::CreatePtr(DQITEM_SPECATTACK, action, dh_specialAttack));
  item->SetOwner(attacker.GetOwner());

  m_itemQueue.push_back(item);

  if (g_player[g_selected_item->GetVisiblePlayer()] &&
      g_player[g_selected_item->GetVisiblePlayer()]->IsVisible(
          attacked.RetPos())) {
    AddProjectileAttack(attacker, attacked, NULL,
                        SpriteStatePtr(new SpriteState(spriteID)), 0);
  }
}

void Director::AddWinnerLoser(Unit victor, Unit dead) {
  Assert(victor.GetActor());
  Assert(dead.GetActor());

  DQActionDeath* action = new DQActionDeath;
  DQItemPtr item(DQItem::CreatePtr(DQITEM_DEATH, action, dh_death));
  item->SetOwner(victor.GetOwner());

  action->death_victor = victor.GetActor();
  action->victor_id = victor.m_id;
  action->death_dead = dead.GetActor();
  action->dead_id = dead.m_id;
  action->victor_Pos = victor.RetPos();
  action->dead_Pos = dead.RetPos();

  action->dead_soundID = dead.GetDeathSoundID();
  action->victor_soundID = victor.GetVictorySoundID();

  m_itemQueue.push_back(item);
}

void Director::AddDeath(Unit dead) {
  Assert(dead.GetActor());

  DQActionDeath* action = new DQActionDeath;
  DQItemPtr item(DQItem::CreatePtr(DQITEM_DEATH, action, dh_death));
  item->SetOwner(dead.GetOwner());

  action->death_dead = dead.GetActor();
  action->dead_id = dead.m_id;
  action->victor_id = 0;
  action->victor_Pos = dead.RetPos();
  action->dead_Pos = dead.RetPos();
  action->dead_soundID = dead.GetDeathSoundID();
  action->victor_soundID = 0;

  ActiveUnitAdd(dead.GetActor());

  dead.ResetActor(); // Now action owns dead's actor
  m_itemQueue.push_back(item);
}

void Director::AddDeathWithSound(Unit dead, sint32 soundID) {
  Assert(dead.GetActor());

  DQActionDeath* action = new DQActionDeath;
  DQItemPtr item(DQItem::CreatePtr(DQITEM_DEATH, action, dh_death));
  item->SetOwner(dead.GetOwner());

  action->death_dead = dead.GetActor();
  action->dead_id = dead.m_id;
  action->victor_id = 0;
  action->victor_Pos = dead.RetPos();
  action->dead_Pos = dead.RetPos();
  action->dead_soundID = soundID;
  action->victor_soundID = 0;

  dead.ResetActor();  // Now action owns dead's actor
  m_itemQueue.push_back(item);
}

void Director::AddMorphUnit(UnitActorPtr morphingActor,
                            SpriteStatePtr ss,
                            sint32 type,
                            Unit id) {
  if (morphingActor == NULL)
    return;

  DQActionMorph* action = new DQActionMorph;
  DQItemPtr item(DQItem::CreatePtr(DQITEM_MORPH, action, dh_morphUnit));

  action->morphing_actor = morphingActor;
  action->ss = ss;
  action->type = type;
  action->id = id;

  m_itemQueue.push_back(item);
}

void Director::AddHide(Unit hider) {
  UnitActorPtr actor = hider.GetActor();
  Assert(actor);
  if (!actor)
    return;

  DQActionHideShow* action = new DQActionHideShow;
  DQItemPtr item(DQItem::CreatePtr(DQITEM_HIDE, action, dh_hide));

  action->hiding_actor = actor;
  action->hiding_pos = hider.RetPos();

  m_itemQueue.push_back(item);
}

void Director::AddShow(Unit hider) {
  UnitActorPtr actor = hider.GetActor();
  Assert(actor);
  if (!actor)
    return;

  DQActionHideShow* action = new DQActionHideShow;
  DQItemPtr item(DQItem::CreatePtr(DQITEM_SHOW, action, dh_show));

  action->hiding_actor = actor;
  action->hiding_pos = hider.RetPos();

  m_itemQueue.push_back(item);
}

void Director::AddWork(Unit worker) {
  UnitActorPtr actor = worker.GetActor();
  Assert(actor);
  if (!actor)
    return;

  DQActionWork* action = new DQActionWork;
  DQItemPtr item(DQItem::CreatePtr(DQITEM_WORK, action, dh_work));
  item->SetOwner(worker.GetOwner());

  action->working_actor = actor;
  action->working_pos = worker.RetPos();
  action->working_soundID = worker.GetWorkSoundID();

  m_itemQueue.push_back(item);
}

void Director::AddFastKill(Unit dead) {
  UnitActorPtr actor = dead.GetActor();
  Assert(actor);
  if (!actor)
    return;

  DQActionFastKill* action = new DQActionFastKill;
  DQItemPtr item(DQItem::CreatePtr(DQITEM_FASTKILL, action, dh_fastkill));

  action->dead = actor;

  dead.ResetActor();
  m_itemQueue.push_back(item);  // Now action owns dead's actor
}

void Director::AddRemoveVision(const MapPoint& pos, double range) {
  DQActionVision* action = new DQActionVision;
  DQItemPtr item(
      DQItem::CreatePtr(DQITEM_REMOVEVISION, action, dh_removeVision));

  action->vision_pos = pos;
  action->vision_range = range;

  m_itemQueue.push_back(item);
}

void Director::AddAddVision(const MapPoint& pos, double range) {
  DQActionVision* action = new DQActionVision;
  action->vision_pos = pos;
  action->vision_range = range;

  m_itemQueue.push_back(
      DQItem::CreatePtr(DQITEM_ADDVISION, action, dh_addVision));
}

void Director::AddSetVisibility(UnitActorPtr actor, uint32 visibility) {
  Assert(actor);
  if (!actor)
    return;

  DQActionSetVisibility* action = new DQActionSetVisibility;
  action->setvisibility_actor = actor;
  action->visibilityFlag = visibility;

  m_itemQueue.push_back(
      DQItem::CreatePtr(DQITEM_SETVISIBILITY, action, dh_setVisibility));
}

void Director::AddSetOwner(UnitActorPtr actor, sint32 owner) {
  Assert(actor);
  if (!actor)
    return;

  DQActionSetOwner* action = new DQActionSetOwner;
  action->setowner_actor = actor;
  action->owner = owner;

  m_itemQueue.push_back(
      DQItem::CreatePtr(DQITEM_SETOWNER, action, dh_setOwner));
}

void Director::AddSetVisionRange(UnitActorPtr actor, double range) {
  Assert(actor);
  if (!actor)
    return;

  DQActionSetVisionRange* action = new DQActionSetVisionRange;
  action->setvisionrange_actor = actor;
  action->range = range;

  m_itemQueue.push_back(
      DQItem::CreatePtr(DQITEM_SETVISIONRANGE, action, dh_setVisionRange));
}

void Director::AddBattle(Battle* battle) {
  Assert(battle);
  if (!battle)
    return;

  DQActionBattle* action = new DQActionBattle;
  action->battle = battle;

  m_itemQueue.push_back(DQItem::CreatePtr(DQITEM_BATTLE, action, dh_battle));
}

void Director::AddPlaySound(sint32 soundID, MapPoint const& pos) {
  if (soundID <= 0)
    return;

  DQActionPlaySound* action = new DQActionPlaySound;
  action->playsound_soundID = soundID;
  action->playsound_pos = pos;

  m_itemQueue.push_back(
      DQItem::CreatePtr(DQITEM_PLAYSOUND, action, dh_playSound));
}

void Director::AddGameSound(GAMESOUNDS sound) {
  AddPlaySound(gamesounds_GetGameSoundID((sint32)sound), MapPoint());
}

void Director::AddPlayWonderMovie(sint32 which) {
  Assert(which >= 0);
  if (which < 0)
    return;

  DQActionPlayWonderMovie* action = new DQActionPlayWonderMovie;
  action->playwondermovie_which = which;

  m_itemQueue.push_back(
      DQItem::CreatePtr(DQITEM_PLAYWONDERMOVIE, action, dh_playWonderMovie));
}

void Director::AddPlayVictoryMovie(GAME_OVER reason,
                                   BOOL previouslyWon,
                                   BOOL previouslyLost) {
  if (previouslyWon || previouslyLost) {
    PLAYER_INDEX player = g_selected_item->GetVisiblePlayer();

    if (g_player[player] && !g_player[player]->m_isDead) {
      return;
    }
  }

  DQActionPlayVictoryMovie* action = new DQActionPlayVictoryMovie;
  action->playvictorymovie_reason = reason;

  m_itemQueue.push_back(
      DQItem::CreatePtr(DQITEM_PLAYVICTORYMOVIE, action, dh_playVictoryMovie));
}

void Director::AddMessage(const Message& message) {
  DQActionMessage* action = new DQActionMessage;
  action->message = message;

  m_itemQueue.push_back(DQItem::CreatePtr(DQITEM_MESSAGE, action, dh_message));
}

void Director::AddFaceoff(Unit& attacker, Unit& defender) {
  DQActionFaceoff* action = new DQActionFaceoff;

  if (attacker.IsValid()) {
    action->faceoff_attacker = attacker.GetActor();
    action->faceoff_attacker_pos = attacker.RetPos();
  }

  if (defender.IsValid()) {
    action->faceoff_attacked = defender.GetActor();
    action->faceoff_attacked_pos = defender.RetPos();
  }

  m_itemQueue.push_back(DQItem::CreatePtr(DQITEM_FACEOFF, action, dh_faceoff));
}

void Director::AddTerminateFaceoff(Unit& faceroffer) {
  DQActionTerminateFaceOff* action = new DQActionTerminateFaceOff;
  DQItemPtr item(
      DQItem::CreatePtr(DQITEM_TERMINATE_FACEOFF, action, dh_terminateFaceoff));

  if (faceroffer.IsValid()) {
    action->faceroffer = faceroffer.GetActor();
    item->SetOwner(faceroffer.GetOwner());
  }

  m_itemQueue.push_back(item);
}

void Director::AddTerminateSound(Unit& unit) {
  if (unit.IsValid()) {
    DQActionTerminateSound* action = new DQActionTerminateSound;
    action->terminate_sound_unit = unit;

    m_itemQueue.push_back(
        DQItem::CreatePtr(DQITEM_TERMINATE_SOUND, action, dh_terminateSound));
  }
}

void Director::AddInvokeThroneRoom(void) {
  DQActionInvokeThroneRoom* action = new DQActionInvokeThroneRoom;

  m_itemQueue.push_back(DQItem::CreatePtr(DQITEM_INVOKE_THRONE_ROOM, action,
                                          dh_invokeThroneRoom));
}

void Director::AddInvokeResearchAdvance(MBCHAR* message) {
  DQActionInvokeResearchAdvance* action = new DQActionInvokeResearchAdvance;
  if (message) {
    MBCHAR* mess = new MBCHAR[strlen(message) + 1];
    strcpy(mess, message);
    action->message = mess;
  }

  m_itemQueue.push_back(DQItem::CreatePtr(DQITEM_INVOKE_RESEARCH_ADVANCE,
                                          action, dh_invokeResearchAdvance));
}

void Director::AddBeginScheduler(sint32 player) {
  Assert(player == g_selected_item->GetCurPlayer());
  if (player != g_selected_item->GetCurPlayer())
    return;

  DPRINTF(k_DBG_GAMESTATE, ("Director::AddBeginScheduler(%d)\n", player));

  DQActionBeginScheduler* action = new DQActionBeginScheduler;
  action->player = player;

  m_itemQueue.push_back(
      DQItem::CreatePtr(DQITEM_BEGIN_SCHEDULER, action, dh_beginScheduler));
}

BOOL Director::TileWillBeCompletelyVisible(sint32 x, sint32 y) {
  RECT tempViewRect = *g_tiledMap->GetMapViewRect();

  for (DQItemPtr item : m_itemQueue) {
    if (item->m_type == DQITEM_CENTERMAP) {
      DQActionCenterMap* action = (DQActionCenterMap*)item->m_action;
      if (action) {
        MapPoint pos = action->centerMap_pos;
        g_radarMap->ComputeCenteredMap(pos, &tempViewRect);
      }
    }
  }

  return g_tiledMap->TileIsCompletelyVisible(x, y, &tempViewRect);
}

void Director::IncrementPendingGameActions() {
  m_pendingGameActions++;
}

void Director::DecrementPendingGameActions() {
  m_pendingGameActions--;
  Assert(m_pendingGameActions >= 0);

  if (m_pendingGameActions <= 0) {
    m_pendingGameActions = 0;
    if (m_endTurnRequested) {
      Player* pl = g_player[g_selected_item->GetCurPlayer()];
      if (pl && (!g_network.IsActive() ||
                 (g_network.IsLocalPlayer(g_selected_item->GetCurPlayer())))) {
        m_endTurnRequested = false;
        DPRINTF(k_DBG_GAMESTATE,
                ("Adding from DecrementPendingGameActions, %d\n",
                 g_selected_item->GetCurPlayer()));
        AddEndTurn();
      }
    }
  }
}

void Director::ReloadAllSprites() {
  sint32 p, i;
  for (p = 0; p < k_MAX_PLAYERS; p++) {
    if (!g_player[p])
      continue;
    // PFT  29 mar 05
    // cycle through human players' cities
    if (g_player[g_selected_item->GetVisiblePlayer()]->IsHuman()) {
      for (i = 0; i < g_player[p]->m_all_cities->Num(); i++) {
        Unit u = g_player[p]->m_all_cities->Access(i);

        // recover the number of turns until the city next produces a pop from
        // it's current state  CityData *cityData = u.GetData()->GetCityData();
        // cityData->TurnsToNextPop();

        UnitActorPtr actor = u.GetActor();
        u.GetSpriteState()->SetIndex(
            u.GetDBRec()->GetDefaultSprite()->GetValue());
        actor->ChangeImage(u.GetSpriteState(), u.GetType(), u);
      }
    }
    for (i = 0; i < g_player[p]->m_all_units->Num(); i++) {
      Unit u = g_player[p]->m_all_units->Access(i);
      UnitActorPtr actor = u.GetActor();
      u.GetSpriteState()->SetIndex(
          u.GetDBRec()->GetDefaultSprite()->GetValue());
      actor->ChangeImage(u.GetSpriteState(), u.GetType(), u);
    }
  }
}

void Director::NotifyResync() {
  m_endTurnRequested = false;
  m_pendingGameActions = 0;
}
