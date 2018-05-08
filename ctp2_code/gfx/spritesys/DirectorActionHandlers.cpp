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
// Extracted from Director.cpp
//
//----------------------------------------------------------------------------

#include "ctp/c3.h"
#include "gfx/spritesys/DirectorActionHandlers.h"

#include "gfx/spritesys/Director.h"
#include "gfx/spritesys/EffectActor.h"
#include "gfx/spritesys/SpriteGroupList.h"
#include "gfx/spritesys/UnitActor.h"
#include "gfx/spritesys/spriteutils.h"
#include "gfx/tilesys/maputils.h"
#include "gfx/tilesys/tiledmap.h"  // g_tiledMap

#include "gs/database/profileDB.h"       // g_theProfileDB
#include "gs/events/GameEventManager.h"  // g_gevManager
#include "gs/gameobj/MessagePool.h"

#include "net/general/net_info.h"
#include "net/general/network.h"

#include "sound/soundmanager.h"  // g_soundManager

#include "ui/aui_ctp2/background.h"
#include "ui/aui_ctp2/c3ui.h"
#include "ui/aui_ctp2/radarmap.h"  // g_radarMap

#include "ui/interface/backgroundwin.h"
#include "ui/interface/battleviewwindow.h"  // g_battleViewWindow
#include "ui/interface/messagewin.h"
#include "ui/interface/sci_advancescreen.h"
#include "ui/interface/screenutils.h"
#include "ui/interface/victorymoviewin.h"
#include "ui/interface/wondermoviewin.h"

extern Background* g_background;
extern C3UI* g_c3ui;
extern MessagePool* g_theMessagePool;
extern SpriteGroupList* g_unitSpriteGroupList;

void dh_move(DQAction* itemAction,
             SequenceWeakPtr weakSeq,
             DHEXECUTE executeType) {
  Assert(!weakSeq.expired());
  SequencePtr seq = weakSeq.lock();

  DQActionMove* action = (DQActionMove*)itemAction;

  Assert(!action->move_actor.expired());
  if (action->move_actor.expired()) {
    g_director->ActionFinished(seq);
    return;
  }
  UnitActorPtr theActor = action->move_actor.lock();

  uint32 maxActionCounter = 1;
  sint32 speed = g_theProfileDB->GetUnitSpeed();
  BOOL visible = FALSE;

  ActionPtr actionObj(new Action());

  MapPoint oldP = action->move_oldPos;
  MapPoint newP = action->move_newPos;

  actionObj->SetSequence(seq);
  seq->AddRef();

  actionObj->SetStartMapPoint(oldP);
  actionObj->SetEndMapPoint(newP);

  actionObj->CreatePath(oldP.x, oldP.y, newP.x, newP.y);

  theActor->PositionActor(oldP);

  if (g_theProfileDB->IsUnitAnim())
    maxActionCounter = k_MAX_UNIT_MOVEMENT_ITERATIONS - speed;

  actionObj->SetMaxActionCounter(maxActionCounter);
  actionObj->SetCurActionCounter(0);

  actionObj->SetSoundEffect(action->move_soundID);

  actionObj->SetMoveActors(action->moveActors);

  if (!theActor->ActionMove(std::move(actionObj))) {
    g_director->ActionFinished(seq);
    return;
  }

  visible = g_director->TileIsVisibleToPlayer(oldP) ||
            g_director->TileIsVisibleToPlayer(newP);

  if (visible && executeType == DHEXECUTE_NORMAL) {
    seq->SetAddedToActiveList(SEQ_ACTOR_PRIMARY, TRUE);
    g_director->ActiveUnitAdd(theActor);

    if (g_selected_item->GetVisiblePlayer() != theActor->GetPlayerNum() &&
        !g_tiledMap->TileIsVisible(theActor->GetPos().x,
                                   theActor->GetPos().y)) {
      g_radarMap->CenterMap(theActor->GetPos());
      g_tiledMap->Refresh();
      g_tiledMap->InvalidateMap();
      g_tiledMap->InvalidateMix();
      background_draw_handler(g_background);
    }
  } else {
    if (theActor->WillDie())
      g_director->FastKill(theActor);
    else
      theActor->EndTurnProcess();
  }
}

void dh_teleport(DQAction* itemAction,
                 SequenceWeakPtr weakSeq,
                 DHEXECUTE executeType) {
  Assert(!weakSeq.expired());
  SequencePtr seq = weakSeq.lock();

  DQActionMove* action = (DQActionMove*)itemAction;

  UnitActorPtr theActor = action->move_actor.lock();

  if (theActor == NULL)
    return;

  if (!action->revealedActors.empty()) {
    for (UnitActorWeakPtr a : action->revealedActors) {
      if (!a.expired())
        a.lock()->SetVisSpecial(TRUE);
    }
  }

  theActor->PositionActor(action->move_newPos);

  for (auto moveActor : action->moveActors) {
    moveActor.lock()->PositionActor(action->move_newPos);
  }

  g_director->ActionFinished(seq);
}

void dh_projectileMove(DQAction* itemAction,
                       SequenceWeakPtr weakSeq,
                       DHEXECUTE executeType) {
  Assert(!weakSeq.expired());
  SequencePtr seq = weakSeq.lock();

  DQActionMoveProjectile* action = (DQActionMoveProjectile*)itemAction;

  EffectActor* projectileEnd = action->end_projectile;
  UnitActorPtr shootingActor = action->pshooting_actor.lock();
  UnitActorPtr targetActor = action->ptarget_actor.lock();
  MapPoint startPos = action->pmove_oldPos;
  MapPoint endPos = action->pmove_newPos;

  Assert(shootingActor != NULL && targetActor != NULL);

  if (shootingActor == NULL || targetActor == NULL)
    return;

  if (projectileEnd && g_director->TileIsVisibleToPlayer(startPos)) {
    ActionPtr actionObj;

    Anim* anim = projectileEnd->CreateAnim(EFFECTACTION_PLAY);
    if (anim == NULL) {
      anim = projectileEnd->CreateAnim(EFFECTACTION_FLASH);
      Assert(anim != NULL);
      if (anim == NULL) {
        g_director->ActionFinished(seq);
        return;
      } else {
        actionObj.reset(new Action(EFFECTACTION_FLASH, ACTIONEND_PATHEND));
      }
    } else {
      actionObj.reset(new Action(EFFECTACTION_PLAY, ACTIONEND_PATHEND));
    }

    Assert(actionObj);
    if (actionObj) {
      actionObj->SetAnim(anim);
      projectileEnd->AddAction(std::move(actionObj));
      g_director->ActiveEffectAdd(projectileEnd);

      // Management taken over by director, no longer managed by item queue.
      action->end_projectile = NULL;
    } else {
      delete anim;
    }
  }

  g_director->ActionFinished(seq);
}

void dh_attack(DQAction* itemAction,
               SequenceWeakPtr weakSeq,
               DHEXECUTE executeType) {
  Assert(!weakSeq.expired());
  SequencePtr seq = weakSeq.lock();

  DQActionAttack* action = (DQActionAttack*)itemAction;

  UnitActorPtr theAttacker = action->attacker.lock();
  UnitActorPtr theDefender = action->defender.lock();

  Assert(theAttacker != NULL);
  Assert(theDefender != NULL);

  if ((theAttacker == NULL) || (theDefender == NULL))
    return;

  //	bool attackerVisible =
  // g_director->TileIsVisibleToPlayer(action->attacker_Pos);
  bool defenderVisible =
      g_director->TileIsVisibleToPlayer(action->defender_Pos);

  bool playerInvolved =
      (theDefender->GetPlayerNum() == g_selected_item->GetVisiblePlayer()) ||
      (theAttacker->GetPlayerNum() == g_selected_item->GetVisiblePlayer());

  POINT AttackerPoints, DefenderPoints;

  maputils_MapXY2PixelXY(action->attacker_Pos.x, action->attacker_Pos.y,
                         AttackerPoints);
  maputils_MapXY2PixelXY(action->defender_Pos.x, action->defender_Pos.y,
                         DefenderPoints);

  sint32 deltax = DefenderPoints.x - AttackerPoints.x;
  sint32 deltay = DefenderPoints.y - AttackerPoints.y;

  sint32 facingIndex = spriteutils_DeltaToFacing(deltax, deltay);

  ActionPtr ActionObj(new Action());

  ActionObj->SetSequence(seq);
  seq->AddRef();

  ActionObj->SetStartMapPoint(action->attacker_Pos);
  ActionObj->SetEndMapPoint(action->attacker_Pos);

  theAttacker->ActionAttack(std::move(ActionObj), facingIndex);

  if (playerInvolved && (executeType == DHEXECUTE_NORMAL)) {
    seq->SetAddedToActiveList(SEQ_ACTOR_PRIMARY, TRUE);
    g_director->ActiveUnitAdd(theAttacker);
  } else {
    if (theAttacker->WillDie())
      g_director->FastKill(theAttacker);
    else
      theAttacker->EndTurnProcess();
  }

  if (!action->defender_IsCity) {
    facingIndex = spriteutils_DeltaToFacing(-deltax, -deltay);

    ActionObj.reset(new Action());

    ActionObj->SetSequence(seq);
    seq->AddRef();

    ActionObj->SetStartMapPoint(action->defender_Pos);
    ActionObj->SetEndMapPoint(action->defender_Pos);

    theDefender->ActionAttack(std::move(ActionObj), facingIndex);

    if (playerInvolved)
      defenderVisible = true;

    if (defenderVisible && (executeType == DHEXECUTE_NORMAL)) {
      seq->SetAddedToActiveList(SEQ_ACTOR_SECONDARY, TRUE);
      g_director->ActiveUnitAdd(theDefender);
    } else {
      if (theDefender->WillDie())
        g_director->FastKill(theDefender);
      else
        theDefender->EndTurnProcess();
    }
  }
}

void dh_specialAttack(DQAction* itemAction,
                      SequenceWeakPtr weakSeq,
                      DHEXECUTE executeType) {
  Assert(!weakSeq.expired());
  SequencePtr seq = weakSeq.lock();

  DQActionAttack* action = (DQActionAttack*)itemAction;

  UnitActorPtr theAttacker = action->attacker.lock();
  UnitActorPtr theDefender = action->defender.lock();

  Assert(theAttacker != NULL);
  Assert(theDefender != NULL);

  if (theAttacker == NULL)
    return;
  if (theDefender == NULL)
    return;

  BOOL attackerCanAttack = !action->attacker_IsCity;
  BOOL defenderIsAttackable = !action->defender_IsCity;

  if (!attackerCanAttack && !defenderIsAttackable) {
    g_director->ActionFinished(seq);
    return;
  }

  sint32 facingIndex;
  POINT AttackerPoints, DefenderPoints;

  maputils_MapXY2PixelXY(action->attacker_Pos.x, action->attacker_Pos.y,
                         AttackerPoints);
  maputils_MapXY2PixelXY(action->defender_Pos.x, action->defender_Pos.y,
                         DefenderPoints);

  sint32 deltax = DefenderPoints.x - AttackerPoints.x;
  sint32 deltay = DefenderPoints.y - AttackerPoints.y;

  if (action->attacker_ID >= 0) {
    g_soundManager->AddSound(SOUNDTYPE_SFX, (uint32)0, action->attacker_ID, 0,
                             0);
  }

  if (attackerCanAttack) {
    ActionPtr AttackerActionObj(new Action());

    AttackerActionObj->SetStartMapPoint(action->attacker_Pos);
    AttackerActionObj->SetEndMapPoint(action->attacker_Pos);

    facingIndex = spriteutils_DeltaToFacing(deltax, deltay);

    AttackerActionObj->SetSequence(seq);
    seq->AddRef();

    if (!theAttacker->ActionSpecialAttack(AttackerActionObj, facingIndex)) {
      g_director->ActionFinished(seq);
      return;
    }

    if (g_director->TileIsVisibleToPlayer(action->attacker_Pos) &&
        executeType == DHEXECUTE_NORMAL) {
      seq->SetAddedToActiveList(SEQ_ACTOR_PRIMARY, TRUE);
      g_director->ActiveUnitAdd(theAttacker);
    } else {
      if (theAttacker->WillDie())
        g_director->FastKill(theAttacker);
      else
        theAttacker->EndTurnProcess();
    }
  }

  if (defenderIsAttackable) {
    ActionPtr DefenderActionObj(new Action());

    DefenderActionObj->SetStartMapPoint(action->defender_Pos);
    DefenderActionObj->SetEndMapPoint(action->defender_Pos);

    facingIndex = spriteutils_DeltaToFacing(-deltax, -deltay);

    DefenderActionObj->SetSequence(seq);
    seq->AddRef();

    if (!theDefender->ActionSpecialAttack(std::move(DefenderActionObj),
                                          facingIndex)) {
      g_director->ActionFinished(seq);
      return;
    }

    if (g_director->TileIsVisibleToPlayer(action->defender_Pos) &&
        executeType == DHEXECUTE_NORMAL) {
      seq->SetAddedToActiveList(SEQ_ACTOR_SECONDARY, TRUE);
      g_director->ActiveUnitAdd(theDefender);
    } else {
      if (theDefender->WillDie())
        g_director->FastKill(theDefender);
      else
        theDefender->EndTurnProcess();
    }
  }
}

void dh_death(DQAction* itemAction,
              SequenceWeakPtr weakSeq,
              DHEXECUTE executeType) {
  Assert(!weakSeq.expired());
  SequencePtr seq = weakSeq.lock();

  DQActionDeath* action = (DQActionDeath*)itemAction;
  UnitActorPtr theDead = action->death_dead;
  UnitActorPtr theVictor = action->death_victor.lock();
  Anim* deathAnim = NULL;
  Anim* victorAnim = NULL;
  sint32 deathActionType = UNITACTION_NONE;
  sint32 victorActionType = UNITACTION_NONE;

  if (theDead != NULL && !theDead->GetNeedsToDie()) {
    theDead->SetNeedsToDie(TRUE);

    if (theDead->HasDeath()) {
      if (theDead->GetLoadType() != LOADTYPE_FULL)
        theDead->FullLoad(UNITACTION_VICTORY);

      deathAnim = theDead->CreateAnim(
          (UNITACTION)deathActionType);  // deathAnim must be deleted

      if (deathAnim) {
        deathActionType = UNITACTION_VICTORY;
      } else {
        deathActionType = UNITACTION_FAKE_DEATH;
        deathAnim = theDead->MakeFakeDeath();
      }
    } else {
      deathActionType = UNITACTION_FAKE_DEATH;
      deathAnim = theDead->MakeFakeDeath();
    }
  } else {
    theDead = NULL;
  }

  if (theVictor != NULL && !theVictor->GetNeedsToDie()) {
    g_director->ActiveUnitRemove(theVictor);

    theVictor->SetNeedsToVictor(TRUE);

    if (theVictor->HasDeath()) {
    } else {
      if (theVictor->GetLoadType() != LOADTYPE_FULL) {
        theVictor->FullLoad(UNITACTION_VICTORY);
      }

      victorActionType = UNITACTION_VICTORY;

      victorAnim = theVictor->CreateAnim((UNITACTION)victorActionType);
      if (victorAnim == NULL) {
        theVictor = NULL;
      }
    }
  }

  if (theDead != NULL) {
    theDead->SetHealthPercent(-1.0);
    theDead->SetTempStackSize(0);

    ActionPtr deadActionObj(
        new Action((UNITACTION)deathActionType, ACTIONEND_ANIMEND));
    Assert(deadActionObj != NULL);
    if (deadActionObj == NULL) {
      c3errors_ErrorDialog("Director",
                           "Internal Failure to create death action");
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
      g_soundManager->TerminateLoopingSound(SOUNDTYPE_SFX,
                                            (uint32)theDead->GetUnitID());

    if (g_soundManager) {
      sint32 visiblePlayer = g_selected_item->GetVisiblePlayer();
      if ((visiblePlayer == theDead->GetPlayerNum()) ||
          (theDead->GetUnitVisibility() & (1 << visiblePlayer))) {
        g_soundManager->AddSound(SOUNDTYPE_SFX, (uint32)theDead->GetUnitID(),
                                 action->dead_soundID, theDead->GetPos().x,
                                 theDead->GetPos().y);
      }
    }

    theDead->AddAction(std::move(deadActionObj));

    if (g_director->TileIsVisibleToPlayer(action->dead_Pos) &&
        executeType == DHEXECUTE_NORMAL) {
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

  if (theVictor != NULL) {
    theVictor->SetHealthPercent(-1.0);
    theVictor->SetTempStackSize(0);

    if (theVictor->HasDeath() || victorAnim == NULL) {
      theVictor->ActionQueueUpIdle();
    } else {
      ActionPtr victorActionObj(
          new Action((UNITACTION)victorActionType, ACTIONEND_ANIMEND));
      if (victorActionObj == NULL) {
        c3errors_ErrorDialog("Director",
                             "Internal Failure to create victory action");
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
        g_soundManager->TerminateLoopingSound(SOUNDTYPE_SFX,
                                              (uint32)theVictor->GetUnitID());
      if (g_soundManager) {
        sint32 visiblePlayer = g_selected_item->GetVisiblePlayer();
        if ((visiblePlayer == theVictor->GetPlayerNum()) ||
            (theVictor->GetUnitVisibility() & (1 << visiblePlayer))) {
          g_soundManager->AddSound(
              SOUNDTYPE_SFX, (uint32)theVictor->GetUnitID(),
              action->victor_soundID, theVictor->GetPos().x,
              theVictor->GetPos().y);
        }
      }

      theVictor->AddAction(std::move(victorActionObj));
    }

    if (g_director->TileIsVisibleToPlayer(action->victor_Pos) &&
        executeType == DHEXECUTE_NORMAL) {
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

void dh_morphUnit(DQAction* itemAction,
                  SequenceWeakPtr weakSeq,
                  DHEXECUTE executeType) {
  Assert(!weakSeq.expired());
  SequencePtr seq = weakSeq.lock();

  DQActionMorph* action = (DQActionMorph*)itemAction;

  UnitActorPtr theActor = action->morphing_actor.lock();

  Assert(theActor != NULL);
  if (theActor) {
    theActor->ChangeType(action->ss, action->type, action->id, FALSE);
  }

  g_director->ActionFinished(seq);
}

void dh_hide(DQAction* itemAction,
             SequenceWeakPtr weakSeq,
             DHEXECUTE executeType) {
  Assert(!weakSeq.expired());
  SequencePtr seq = weakSeq.lock();

  DQActionHideShow* action = (DQActionHideShow*)itemAction;

  Assert(action);
  if (!action)
    return;

  UnitActorPtr actor = action->hiding_actor.lock();

  Assert(actor);
  if (!actor)
    return;

  actor->Hide();

  g_director->ActionFinished(seq);
}

void dh_show(DQAction* itemAction,
             SequenceWeakPtr weakSeq,
             DHEXECUTE executeType) {
  Assert(!weakSeq.expired());
  SequencePtr seq = weakSeq.lock();

  DQActionHideShow* action = (DQActionHideShow*)itemAction;

  Assert(action);
  if (!action)
    return;

  UnitActorPtr actor = action->hiding_actor.lock();

  Assert(actor);
  if (!actor)
    return;

  actor->PositionActor(action->hiding_pos);
  actor->Show();

  g_director->ActionFinished(seq);
}

void dh_work(DQAction* itemAction,
             SequenceWeakPtr weakSeq,
             DHEXECUTE executeType) {
  Assert(!weakSeq.expired());
  SequencePtr seq = weakSeq.lock();

  DQActionWork* action = (DQActionWork*)itemAction;

  Assert(action);
  if (!action)
    return;

  UnitActorPtr actor = action->working_actor.lock();

  Assert(actor);
  if (!actor)
    return;

  ActionPtr actionObj(new Action(UNITACTION_WORK, ACTIONEND_ANIMEND));

  Assert(actionObj);
  if (actionObj)
    return;

  actionObj->SetStartMapPoint(action->working_pos);
  actionObj->SetEndMapPoint(action->working_pos);

  if (actor->GetLoadType() != LOADTYPE_FULL)
    actor->FullLoad(UNITACTION_WORK);

  Anim* anim = actor->CreateAnim(UNITACTION_WORK);
  if (anim == NULL) {
    anim = actor->CreateAnim(UNITACTION_MOVE);

    if (!anim) {
      actionObj.reset();
      g_director->ActionFinished(seq);
      return;
    }
  }

  actionObj->SetSequence(seq);
  seq->AddRef();

  actionObj->SetAnim(anim);

  actor->AddAction(std::move(actionObj));

  if (g_soundManager) {
    sint32 visiblePlayer = g_selected_item->GetVisiblePlayer();
    if ((visiblePlayer == actor->GetPlayerNum()) ||
        (actor->GetUnitVisibility() & (1 << visiblePlayer))) {
      g_soundManager->AddSound(SOUNDTYPE_SFX, (uint32)actor->GetUnitID(),
                               action->working_soundID, actor->GetPos().x,
                               actor->GetPos().y);
    }
  }

  if (g_director->TileIsVisibleToPlayer(action->working_pos) &&
      executeType == DHEXECUTE_NORMAL) {
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

void dh_fastkill(DQAction* itemAction,
                 SequenceWeakPtr weakSeq,
                 DHEXECUTE executeType) {
  Assert(!weakSeq.expired());
  SequencePtr seq = weakSeq.lock();

  DQActionFastKill* action = (DQActionFastKill*)itemAction;

  Assert(action);
  if (!action) {
    return;
  }

  g_director->FastKill(action->dead);

  g_director->ActionFinished(seq);
}

void dh_removeVision(DQAction* itemAction,
                     SequenceWeakPtr weakSeq,
                     DHEXECUTE executeType) {
  Assert(!weakSeq.expired());
  SequencePtr seq = weakSeq.lock();

  DQActionVision* action = (DQActionVision*)itemAction;

  Assert(action);
  if (!action)
    return;

  if (g_tiledMap)
    g_tiledMap->RemoveVisible(action->vision_pos, action->vision_range);

  g_director->ActionFinished(seq);
}

void dh_addVision(DQAction* itemAction,
                  SequenceWeakPtr weakSeq,
                  DHEXECUTE executeType) {
  Assert(!weakSeq.expired());
  SequencePtr seq = weakSeq.lock();

  DQActionVision* action = (DQActionVision*)itemAction;

  Assert(action);
  if (!action)
    return;

  g_tiledMap->AddVisible(action->vision_pos, action->vision_range);

  g_director->ActionFinished(seq);
}

void dh_setVisibility(DQAction* itemAction,
                      SequenceWeakPtr weakSeq,
                      DHEXECUTE executeType) {
  Assert(!weakSeq.expired());
  SequencePtr seq = weakSeq.lock();

  DQActionSetVisibility* action = (DQActionSetVisibility*)itemAction;

  UnitActorPtr actor = action ? action->setvisibility_actor.lock() : NULL;
  Assert(actor);
  if (!actor)
    return;

  actor->SetUnitVisibility(action->visibilityFlag);

  g_director->ActionFinished(seq);
}

void dh_setOwner(DQAction* itemAction,
                 SequenceWeakPtr weakSeq,
                 DHEXECUTE executeType) {
  Assert(!weakSeq.expired());
  SequencePtr seq = weakSeq.lock();

  DQActionSetOwner* action = (DQActionSetOwner*)itemAction;

  UnitActorPtr actor(action ? action->setowner_actor.lock() : NULL);
  Assert(actor);
  if (!actor)
    return;

  actor->SetPlayerNum(action->owner);

  g_director->ActionFinished(seq);
}

void dh_setVisionRange(DQAction* itemAction,
                       SequenceWeakPtr weakSeq,
                       DHEXECUTE executeType) {
  Assert(!weakSeq.expired());
  SequencePtr seq = weakSeq.lock();

  DQActionSetVisionRange* action = (DQActionSetVisionRange*)itemAction;

  UnitActorPtr actor = action ? action->setvisionrange_actor.lock() : NULL;
  Assert(actor);
  if (!actor)
    return;

  actor->SetUnitVisionRange(action->range);

  g_director->ActionFinished(seq);
}

void dh_combatflash(DQAction* itemAction,
                    SequenceWeakPtr weakSeq,
                    DHEXECUTE executeType) {
  Assert(!weakSeq.expired());
  SequencePtr seq = weakSeq.lock();

  DQActionCombatFlash* action = (DQActionCombatFlash*)itemAction;

  SpriteStatePtr ss(new SpriteState(99));
  EffectActor* flash = new EffectActor(ss, action->flash_pos);

  Anim* anim = flash->CreateAnim(EFFECTACTION_PLAY);
  if (anim == NULL) {
    anim = flash->CreateAnim(EFFECTACTION_FLASH);
    Assert(anim != NULL);
  }

  if (anim) {
    ActionPtr actionObj(new Action(EFFECTACTION_FLASH, ACTIONEND_PATHEND));
    actionObj->SetAnim(anim);
    flash->AddAction(std::move(actionObj));
    g_director->ActiveEffectAdd(flash);
  }

  g_director->ActionFinished(seq);
}

void dh_copyVision(DQAction* itemAction,
                   SequenceWeakPtr weakSeq,
                   DHEXECUTE executeType) {
  Assert(!weakSeq.expired());
  SequencePtr seq = weakSeq.lock();

  //	DQActionCopyVision	*action = (DQActionCopyVision *)itemAction;

  g_tiledMap->CopyVision();
  g_radarMap->Update();
  g_director->ActionFinished(seq);
}

void dh_centerMap(DQAction* itemAction,
                  SequenceWeakPtr weakSeq,
                  DHEXECUTE executeType) {
  Assert(!weakSeq.expired());
  SequencePtr seq = weakSeq.lock();

  DQActionCenterMap* action = (DQActionCenterMap*)itemAction;

  if (!g_selected_item->GetIsPathing()) {
    g_radarMap->CenterMap(action->centerMap_pos);

    g_tiledMap->Refresh();
    g_tiledMap->InvalidateMap();
    g_tiledMap->InvalidateMix();

    background_draw_handler(g_background);
  }

  g_director->ActionFinished(seq);
}

void dh_selectUnit(DQAction* itemAction,
                   SequenceWeakPtr weakSeq,
                   DHEXECUTE executeType) {
  Assert(!weakSeq.expired());
  SequencePtr seq = weakSeq.lock();

  DQActionUnitSelection* action = (DQActionUnitSelection*)itemAction;

  g_selected_item->DirectorUnitSelection(action->flags);

  g_director->ActionFinished(seq);
}

void dh_endTurn(DQAction* itemAction,
                SequenceWeakPtr weakSeq,
                DHEXECUTE executeType) {
  Assert(!weakSeq.expired());
  SequencePtr seq = weakSeq.lock();

  //	DQActionEndTurn	*action = (DQActionEndTurn *)itemAction;

  g_director->ActionFinished(seq);

  g_gevManager->AddEvent(GEV_INSERT_Tail, GEV_EndTurn, GEA_Player,
                         g_selected_item->GetCurPlayer(), GEA_End);
}

void dh_battle(DQAction* itemAction,
               SequenceWeakPtr weakSeq,
               DHEXECUTE executeType) {
  Assert(!weakSeq.expired());
  SequencePtr seq = weakSeq.lock();

  DQActionBattle* action = (DQActionBattle*)itemAction;

  if (g_battleViewWindow) {
    BattleViewWindow::Cleanup();
  }

  BattleViewWindow::Initialize(seq);

  if (g_battleViewWindow) {
    g_battleViewWindow->SetupBattle(action->battle);
    g_c3ui->AddWindow(g_battleViewWindow);
    g_cursorManager->SetCursor(CURSORINDEX_DEFAULT);
  }
}

void dh_playSound(DQAction* itemAction,
                  SequenceWeakPtr weakSeq,
                  DHEXECUTE executeType) {
  Assert(!weakSeq.expired());
  SequencePtr seq = weakSeq.lock();

  DQActionPlaySound* action = (DQActionPlaySound*)itemAction;

  if (!g_soundManager)
    return;

  g_soundManager->AddSound(SOUNDTYPE_SFX, 0, action->playsound_soundID,
                           action->playsound_pos.x, action->playsound_pos.y);

  g_director->ActionFinished(seq);
}

void dh_playWonderMovie(DQAction* itemAction,
                        SequenceWeakPtr weakSeq,
                        DHEXECUTE executeType) {
  Assert(!weakSeq.expired());
  SequencePtr seq = weakSeq.lock();

  DQActionPlayWonderMovie* action = (DQActionPlayWonderMovie*)itemAction;

  Assert(action);
  if (!action)
    return;

  sint32 which = action->playwondermovie_which;

  wondermoviewin_Initialize(seq);
  wondermoviewin_DisplayWonderMovie(which);
}

void dh_playVictoryMovie(DQAction* itemAction,
                         SequenceWeakPtr weakSeq,
                         DHEXECUTE executeType) {
  Assert(!weakSeq.expired());
  SequencePtr seq = weakSeq.lock();

  DQActionPlayVictoryMovie* action = (DQActionPlayVictoryMovie*)itemAction;

  Assert(action);
  if (!action)
    return;

  GAME_OVER reason = action->playvictorymovie_reason;

  victorymoviewin_Initialize(seq);
  victorymoviewin_DisplayVictoryMovie(reason);
}

void dh_message(DQAction* itemAction,
                SequenceWeakPtr weakSeq,
                DHEXECUTE executeType) {
  Assert(!weakSeq.expired());
  SequencePtr seq = weakSeq.lock();

  DQActionMessage* action = (DQActionMessage*)itemAction;

  Assert(action);
  if (!action)
    return;

  if (g_theMessagePool->IsValid(action->message)) {
    if (action->message.IsAlertBox()) {
      if (!messagewin_IsModalMessageDisplayed()) {
        messagewin_CreateModalMessage(action->message);
      }
    } else {
      if (!action->message.AccessData()->GetMessageWindow()) {
        messagewin_CreateMessage(action->message);
      }
      if (action->message.IsInstantMessage()
          // JJB added this to prevent instant messages showing
          // out of turn in hotseat games.
          // With the existing behaviour they would show immediately
          // which would often mean that they show on the wrong players
          // turn.
          &&
          g_selected_item->GetVisiblePlayer() == action->message.GetOwner()) {
        action->message.Show();
      }
    }
  }

  g_director->ActionFinished(seq);
}

void dh_faceoff(DQAction* itemAction,
                SequenceWeakPtr weakSeq,
                DHEXECUTE executeType) {
  Assert(!weakSeq.expired());
  SequencePtr seq = weakSeq.lock();

  DQActionFaceoff* action = (DQActionFaceoff*)itemAction;

  Assert(action);
  if (!action)
    return;

  UnitActorPtr theAttacker = action->faceoff_attacker.lock();
  UnitActorPtr theAttacked = action->faceoff_attacked.lock();

  bool attackedIsAttackable = true;

  Assert(theAttacker != NULL);
  if (theAttacker == NULL)
    return;

  Assert(theAttacked != NULL);
  if (theAttacked == NULL)
    return;

  ActionPtr AttackedActionObj;

  ActionPtr AttackerActionObj(
      new Action(UNITACTION_FACE_OFF, ACTIONEND_INTERRUPT));

  AttackerActionObj->SetSequence(SequenceWeakPtr());

  if (attackedIsAttackable) {
    AttackedActionObj.reset(
        new Action(UNITACTION_FACE_OFF, ACTIONEND_INTERRUPT));
    AttackedActionObj->SetSequence(SequenceWeakPtr());
  }

  AttackerActionObj->SetStartMapPoint(action->faceoff_attacker_pos);
  AttackerActionObj->SetEndMapPoint(action->faceoff_attacker_pos);

  if (attackedIsAttackable) {
    AttackedActionObj->SetStartMapPoint(action->faceoff_attacked_pos);
    AttackedActionObj->SetEndMapPoint(action->faceoff_attacked_pos);
  }

  Anim* AttackedAnim = NULL;

  Anim* AttackerAnim = theAttacker->MakeFaceoff();
  if (AttackerAnim == NULL) {
    theAttacker->AddIdle(TRUE);
    return;
  }
  AttackerActionObj->SetAnim(AttackerAnim);

  if (attackedIsAttackable) {
    if (theAttacked->GetLoadType() != LOADTYPE_FULL)
      theAttacked->FullLoad(UNITACTION_IDLE);

    AttackedAnim = theAttacked->MakeFaceoff();

    if (AttackedAnim == NULL) {
      theAttacked->AddIdle(TRUE);
    }
  }

  POINT AttackerPoints, AttackedPoints;

  maputils_MapXY2PixelXY(action->faceoff_attacker_pos.x,
                         action->faceoff_attacker_pos.y, AttackerPoints);
  maputils_MapXY2PixelXY(action->faceoff_attacked_pos.x,
                         action->faceoff_attacked_pos.y, AttackedPoints);

  AttackerActionObj->SetFacing(
      spriteutils_DeltaToFacing(AttackedPoints.x - AttackerPoints.x,
                                AttackedPoints.y - AttackerPoints.y));

  if (AttackedAnim != NULL) {
    AttackedActionObj->SetAnim(AttackedAnim);
    AttackedActionObj->SetFacing(
        spriteutils_DeltaToFacing(AttackerPoints.x - AttackedPoints.x,
                                  AttackerPoints.y - AttackedPoints.y));
  }

  AttackerActionObj->SetUnitVisionRange(theAttacker->GetUnitVisionRange());
  AttackerActionObj->SetUnitsVisibility(theAttacker->GetUnitVisibility());

  theAttacker->AddAction(std::move(AttackerActionObj));

  bool attackedVisible = true;

  if (attackedIsAttackable) {
    if (AttackedAnim != NULL) {
      AttackedActionObj->SetUnitVisionRange(theAttacked->GetUnitVisionRange());

      AttackedActionObj->SetUnitsVisibility(theAttacker->GetUnitVisibility());
      theAttacked->AddAction(std::move(AttackedActionObj));
    }

    attackedVisible =
        g_director->TileIsVisibleToPlayer(action->faceoff_attacked_pos);

    if (theAttacker->GetPlayerNum() == g_selected_item->GetVisiblePlayer() ||
        theAttacked->GetPlayerNum() == g_selected_item->GetVisiblePlayer())
      attackedVisible = TRUE;

    if (attackedVisible && executeType == DHEXECUTE_NORMAL) {
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

  BOOL attackerVisible =
      g_director->TileIsVisibleToPlayer(action->faceoff_attacker_pos);

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

void dh_terminateFaceoff(DQAction* itemAction,
                         SequenceWeakPtr weakSeq,
                         DHEXECUTE executeType) {
  Assert(!weakSeq.expired());
  SequencePtr seq = weakSeq.lock();

  DQActionTerminateFaceOff* action = (DQActionTerminateFaceOff*)itemAction;

  Assert(action);
  if (!action)
    return;

  UnitActorPtr facerOffer = action->faceroffer.lock();

  if (facerOffer) {
    facerOffer->SetHealthPercent(-1.0);
    facerOffer->SetTempStackSize(0);

    g_director->ActiveUnitRemove(facerOffer);
  }

  g_director->ActionFinished(seq);
}

void dh_terminateSound(DQAction* itemAction,
                       SequenceWeakPtr weakSeq,
                       DHEXECUTE executeType) {
  Assert(!weakSeq.expired());
  SequencePtr seq = weakSeq.lock();

  DQActionTerminateSound* action = (DQActionTerminateSound*)itemAction;

  Assert(action);
  if (!action)
    return;

  if (g_soundManager)
    g_soundManager->TerminateLoopingSound(SOUNDTYPE_SFX,
                                          action->terminate_sound_unit.m_id);

  g_director->ActionFinished(seq);
}

void dh_speceffect(DQAction* itemAction,
                   SequenceWeakPtr weakSeq,
                   DHEXECUTE executeType) {
  Assert(!weakSeq.expired());
  SequencePtr seq = weakSeq.lock();

  DQActionSpecialEffect* action = (DQActionSpecialEffect*)itemAction;

  Assert(action);
  if (!action) {
    g_director->ActionFinished(seq);
    return;
  }

  MapPoint pos = action->speceffect_pos;
  sint32 soundID = action->speceffect_soundID;
  sint32 spriteID = action->speceffect_spriteID;

  if (!g_tiledMap->GetLocalVision()->IsVisible(pos)) {
    g_director->ActionFinished(seq);
    return;
  }

  SpriteStatePtr ss(new SpriteState(spriteID));
  EffectActor* effectActor = new EffectActor(ss, pos);

  Anim* anim = effectActor->CreateAnim(EFFECTACTION_PLAY);

  if (anim) {
    ActionPtr actionObj(new Action(EFFECTACTION_PLAY, ACTIONEND_PATHEND));
    actionObj->SetAnim(anim);
    effectActor->AddAction(std::move(actionObj));
    g_director->ActiveEffectAdd(effectActor);

    if (g_soundManager) {
      g_soundManager->AddSound(SOUNDTYPE_SFX, 0, soundID, pos.x, pos.y);
    }
  }

  g_director->ActionFinished(seq);
}

void dh_attackpos(DQAction* itemAction,
                  SequenceWeakPtr weakSeq,
                  DHEXECUTE executeType) {
  Assert(!weakSeq.expired());
  SequencePtr seq = weakSeq.lock();

  DQActionAttackPos* action = (DQActionAttackPos*)itemAction;

  Assert(action);
  if (!action)
    return;

  UnitActorPtr theAttacker = action->attackpos_attacker.lock();

  Assert(theAttacker != NULL);
  if (theAttacker == NULL || theAttacker->GetNeedsToDie()) {
    g_director->ActionFinished(seq);
    return;
  }
  ActionPtr AttackerActionObj(new Action(
      UNITACTION_ATTACK, ACTIONEND_ANIMEND,
      theAttacker->GetHoldingCurAnimPos(UNITACTION_ATTACK),
      theAttacker->GetHoldingCurAnimSpecialDelayProcess(UNITACTION_ATTACK)));

  AttackerActionObj->SetSequence(seq);
  seq->AddRef();

  AttackerActionObj->SetStartMapPoint(action->attackpos_attacker_pos);
  AttackerActionObj->SetEndMapPoint(action->attackpos_attacker_pos);

  Anim* AttackerAnim = NULL;

  if (theAttacker->GetLoadType() != LOADTYPE_FULL)
    theAttacker->FullLoad(UNITACTION_ATTACK);

  AttackerAnim = theAttacker->CreateAnim(UNITACTION_ATTACK);

  if (AttackerAnim == NULL)
    AttackerAnim = theAttacker->CreateAnim(UNITACTION_IDLE);

  AttackerActionObj->SetAnim(AttackerAnim);

  POINT AttackerPoints, AttackedPoints;

  maputils_MapXY2PixelXY(action->attackpos_attacker_pos.x,
                         action->attackpos_attacker_pos.y, AttackerPoints);
  maputils_MapXY2PixelXY(action->attackpos_target_pos.x,
                         action->attackpos_target_pos.y, AttackedPoints);

  AttackerActionObj->SetFacing(
      spriteutils_DeltaToFacing(AttackedPoints.x - AttackerPoints.x,
                                AttackedPoints.y - AttackerPoints.y));

  AttackerActionObj->SetUnitVisionRange(theAttacker->GetUnitVisionRange());
  AttackerActionObj->SetUnitsVisibility(theAttacker->GetUnitVisibility());

  if (g_soundManager)
    g_soundManager->TerminateLoopingSound(SOUNDTYPE_SFX,
                                          (uint32)theAttacker->GetUnitID());

  if (g_soundManager) {
    sint32 visiblePlayer = g_selected_item->GetVisiblePlayer();
    if ((visiblePlayer == theAttacker->GetPlayerNum()) ||
        (theAttacker->GetUnitVisibility() & (1 << visiblePlayer))) {
      g_soundManager->AddSound(SOUNDTYPE_SFX, (uint32)theAttacker->GetUnitID(),
                               action->attackpos_soundID,
                               theAttacker->GetPos().x,
                               theAttacker->GetPos().y);
    }
  }

  theAttacker->AddAction(std::move(AttackerActionObj));

  bool attackerVisible =
      g_director->TileIsVisibleToPlayer(action->attackpos_attacker_pos);

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

void dh_invokeThroneRoom(DQAction* itemAction,
                         SequenceWeakPtr weakSeq,
                         DHEXECUTE executeType) {
  Assert(!weakSeq.expired());
  SequencePtr seq = weakSeq.lock();

  g_director->ActionFinished(seq);
}

void dh_invokeResearchAdvance(DQAction* itemAction,
                              SequenceWeakPtr weakSeq,
                              DHEXECUTE executeType) {
  Assert(!weakSeq.expired());
  SequencePtr seq = weakSeq.lock();

  DQActionInvokeResearchAdvance* action =
      (DQActionInvokeResearchAdvance*)itemAction;

  if (!action) {
    g_director->ActionFinished(seq);
    return;
  }

  sci_advancescreen_displayMyWindow(action->message, 0, seq);

  delete action->message;
  action->message = NULL;
}

void dh_beginScheduler(DQAction* itemAction,
                       SequenceWeakPtr weakSeq,
                       DHEXECUTE executeType) {
  Assert(!weakSeq.expired());
  SequencePtr seq = weakSeq.lock();

  DQActionBeginScheduler* action = (DQActionBeginScheduler*)itemAction;

  if (!action) {
    g_director->ActionFinished(seq);
    return;
  }

#ifdef _DEBUG
  static bool isCurrentPlayerOk =
      true;  // static, to report the error only once
  if (isCurrentPlayerOk) {
    isCurrentPlayerOk = action->player == g_selected_item->GetCurPlayer();
    Assert(isCurrentPlayerOk);
  }
#endif

  if (g_network.IsHost()) {
    g_network.Enqueue(
        new NetInfo(NET_INFO_CODE_BEGIN_SCHEDULER, action->player));
  }

  Assert(g_director->m_holdSchedulerSequence.expired());
  if (!g_network.IsActive() || g_network.IsLocalPlayer(action->player)) {
    g_director->SetHoldSchedulerSequence(seq);
  } else {
    g_director->SetHoldSchedulerSequence(SequenceWeakPtr());
  }

  g_gevManager->Pause();
  g_gevManager->AddEvent(GEV_INSERT_Tail, GEV_BeginScheduler, GEA_Player,
                         action->player, GEA_End);
  g_gevManager->Resume();

  if (g_director->m_holdSchedulerSequence.expired()) {
    g_director->ActionFinished(seq);
  }
}
