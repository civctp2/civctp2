//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : Battle event player
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
// - None
//
//----------------------------------------------------------------------------
//
// Modifications from the original Activision code:
//
// - Initialized local variables. (Sep 9th 2005 Martin Gühmann)
//
//----------------------------------------------------------------------------

#include "c3.h"

#include "battleviewactor.h"
#include "EffectActor.h"
#include "battleviewwindow.h"

#include "battleevent.h"

#include "soundmanager.h"

extern SoundManager		*g_soundManager;
extern BattleViewWindow	*g_battleViewWindow;



BattleEvent::BattleEvent(BATTLE_EVENT_TYPE type)
{
	Initialize();

	m_type = type;
}



void BattleEvent::Initialize(void)
{
	m_type = BATTLE_EVENT_TYPE_NONE;
	m_finished = FALSE;
	m_animating = FALSE;				
	m_dataList = new PointerList<BattleEventData>;
	m_walker = new PointerList<BattleEventData>::Walker(m_dataList);
}



BattleEvent::~BattleEvent()
{
	
	if (m_dataList) {
		m_dataList->DeleteAll();
		delete m_dataList;
	}

	if (m_walker) {
		delete m_walker;
	}
}



BOOL BattleEvent::HasActor(BattleViewActor *actor)
{
	m_walker->SetList(m_dataList);

	while (m_walker->IsValid()) {
		if (m_walker->GetObj()->actor == actor)
			return TRUE;
		m_walker->Next();
	}

	return FALSE;
}



void BattleEvent::AddPositionData(BattleViewActor *actor, sint32 column, sint32 row, 
									sint32 facing, double hp, BOOL isDefender)
{
	if (HasActor(actor)) return;

	BattleEventData		*data = new BattleEventData;

	data->positionActor = actor;
	data->positionColumn = column;
	data->positionRow = row;
	data->positionFacing = facing;
	data->positionHP = hp;
	data->positionIsDefender = isDefender;

	m_dataList->AddTail(data);
}



void BattleEvent::AddAttackData(BattleViewActor *actor, sint32 soundID, double hp)
{
	if (HasActor(actor)) return;

	BattleEventData		*data = new BattleEventData;

	data->attackActor = actor;
	data->attackSoundID	= soundID;
	data->attackHP = hp;

	m_dataList->AddTail(data);
}



void BattleEvent::AddExplosionData(BattleViewActor *actor, EffectActor *explodeActor, sint32 soundID,
								   double hp)
{
	if (HasActor(actor)) return;

	BattleEventData		*data = new BattleEventData;

	data->explodeVictim = actor;
	data->explodeActor = explodeActor;
	data->explodeSoundID = soundID;
	data->explodeHP = hp;

	m_dataList->AddTail(data);
}



void BattleEvent::AddDeathData(BattleViewActor *actor, sint32 soundID, double hp)
{
	if (HasActor(actor)) return;

	BattleEventData		*data = new BattleEventData;

	data->deathVictim = actor;
	data->deathSoundID = soundID;
	data->deathHP = hp;

	m_dataList->AddTail(data);
}



void BattleEvent::ProcessPlacement(void)
{
	BattleEventData		*data;

	m_walker->SetList(m_dataList);
	while (m_walker->IsValid()) {
		data = m_walker->GetObj();
		Assert(data);
		if (data) {
			if(data->positionActor) {

				sint32		x, y;
				if (data->positionIsDefender) {
					g_battleViewWindow->GetDefenderPos(data->positionColumn, data->positionRow,
													   &x, &y);
				} else {
					g_battleViewWindow->GetAttackerPos(data->positionColumn, data->positionRow,
													   &x, &y);
				}
				DPRINTF(k_DBG_GAMESTATE, ("Process placement for actor %lx (unit %lx)\n", data->positionActor, data->positionActor->GetUnitID().m_id));
				
				data->positionActor->SetX(x);
				data->positionActor->SetY(y);
				data->positionActor->SetFacing(data->positionFacing);
				data->positionActor->SetHitPoints(data->positionHP);
			}

			


			delete m_walker->Remove();
		} else {
			Assert(FALSE);
			m_walker->Next();
		}
	}	

	m_finished = TRUE;
}



void BattleEvent::ProcessAttack(void)
{
	BattleEventData		*data;

	if (m_dataList->GetCount() <= 0) {
		m_finished = TRUE;
		return;
	}

	BOOL	nowAnimating = FALSE;

	m_walker->SetList(m_dataList);
	while (m_walker->IsValid()) {
		data = m_walker->GetObj();
		Assert(data);
		if (data) {
			BattleViewActor	*actor = data->attackActor;

			Assert(actor);
			if (!actor) {


				delete m_walker->Remove();
			} else {
				BOOL finished = FALSE;

				if (!m_animating) {
					
					Assert(actor);

					Anim *  anim = actor->CreateAnim(UNITACTION_ATTACK);
					if (anim) {
						Action			*action = new Action(UNITACTION_ATTACK, ACTIONEND_ANIMEND);

						action->SetAnim(anim);
						actor->AddAction(action);
					} else {
						
						finished = TRUE;
					}

					
					if (g_soundManager && data->attackSoundID >= 0)
						g_soundManager->AddSound(SOUNDTYPE_SFX, (uint32)actor->GetUnitID(), 
												data->attackSoundID);
					
					nowAnimating = TRUE;
				} else {
					
					actor->Process();

					
					if (actor->GetCurAction()) {
						if (actor->GetCurAction()->GetActionType() != UNITACTION_ATTACK) {
							finished = TRUE;
						}
					}
				}

				
				if (finished) {
					actor->SetHitPoints(data->attackHP);


					delete m_walker->Remove();
				} else {
					m_walker->Next();
				}
			}
		}
	}	
	if (nowAnimating)
		m_animating = TRUE;
}



void BattleEvent::ProcessExplode(void)
{
	BattleEventData		*data;

	if (m_dataList->GetCount() <= 0) {
		m_finished = TRUE;
		return;
	}

	BOOL	nowAnimating = FALSE;

	m_walker->SetList(m_dataList);
	while (m_walker->IsValid()) {
		data = m_walker->GetObj();
		Assert(data);
		if (data) {
			EffectActor			*actor = data->explodeActor;

			BOOL finished = FALSE;

			
			if (!m_animating) {

				if (actor) {
					Action		*action = NULL;

					Anim *  anim = actor->CreateAnim(EFFECTACTION_PLAY);
					if (anim == NULL) {
						anim = actor->CreateAnim(EFFECTACTION_FLASH);
						if (anim)
                        {
							action = new Action(EFFECTACTION_FLASH, ACTIONEND_ANIMEND);
                        }
						else
                        {
							Assert(FALSE);
                        }
					} else {
						action = new Action(EFFECTACTION_PLAY, ACTIONEND_ANIMEND);
					}

					action->SetAnim(anim);
					actor->AddAction(action);
					actor->Process();

					if(data->explodeVictim) {
						actor->SetX(data->explodeVictim->GetX());
						actor->SetY(data->explodeVictim->GetY());
					}
				}
				
				if (g_soundManager)
					g_soundManager->AddSound(SOUNDTYPE_SFX, (uint32)0, data->explodeSoundID);

				nowAnimating = TRUE;
			} else {
				if (actor) {
					actor->Process();
					
					if(data->explodeVictim) {
						actor->SetX(data->explodeVictim->GetX());
						actor->SetY(data->explodeVictim->GetY());
					}

					if (actor->GetKillNow())
						finished = TRUE;
				} else {
					finished = TRUE;
				}
			}

			
			if (finished) {
				if(data->explodeVictim) {
					data->explodeVictim->SetHitPoints(data->explodeHP);
				}


				delete m_walker->Remove();
				delete actor;
			} else {
				m_walker->Next();
			}
		}
	}

	if (nowAnimating)
		m_animating = TRUE;
}



void BattleEvent::ProcessDeath(void)
{
	BattleEventData		*data;

	if (m_dataList->GetCount() <= 0) {
		m_finished = TRUE;
		return;
	}

	BOOL	nowAnimating = FALSE;

	m_walker->SetList(m_dataList);
	while (m_walker->IsValid()) {
		data = m_walker->GetObj();
		Assert(data);
		if (data) {
			BattleViewActor		*actor = data->deathVictim;
			Assert(actor);
			BOOL finished = TRUE;
			if(actor) {
				finished = FALSE;

				
				if (!m_animating) {
					Action		*action;
					Anim		*anim;

					
					

					if (!actor->HasDeath() || (!actor->HasThisAnim(UNITACTION_VICTORY))) {
						
						action = new Action(UNITACTION_FAKE_DEATH, ACTIONEND_ANIMEND);
						anim = actor->MakeFakeDeath();
					} else {
						action = new Action(UNITACTION_VICTORY, ACTIONEND_ANIMEND);
						anim = actor->CreateAnim(UNITACTION_VICTORY);
					}

					action->SetAnim(anim);
					actor->AddAction(action);

					
					if (g_soundManager)
						g_soundManager->AddSound(SOUNDTYPE_SFX, (uint32)actor->GetUnitID(), 
												 data->deathSoundID);

					actor->Process();

					actor->SetHitPoints(data->deathHP);

					nowAnimating = TRUE;
				} else {
					actor->Process();

					if (actor->GetCurAction() && 
						actor->GetCurAction()->m_actionType != UNITACTION_VICTORY && 
						actor->GetCurAction()->m_actionType != UNITACTION_FAKE_DEATH) {
						
						finished = TRUE;
					}
				}

			}

			
			if (finished) {
				


				delete m_walker->Remove();
				if(actor) {
					g_battleViewWindow->RemoveActor(actor);
				}
			} else {
				m_walker->Next();
			}
		}
	}	
	if (nowAnimating)
		m_animating = TRUE;
}



void BattleEvent::Process(void)
{
	switch (m_type) {
	case BATTLE_EVENT_TYPE_PLACEMENT:	ProcessPlacement();
		break;
	case BATTLE_EVENT_TYPE_ATTACK:		ProcessAttack();
		break;
	case BATTLE_EVENT_TYPE_EXPLODE:		ProcessExplode();
		break;
	case BATTLE_EVENT_TYPE_DEATH:		ProcessDeath();
		break;
	default:
		Assert(FALSE);
		break;
	}
}



void BattleEvent::DrawExplosions(aui_Surface *surface)
{
	if (m_type != BATTLE_EVENT_TYPE_EXPLODE) return;
	
	if (m_dataList->GetCount() <= 0) return;

	BattleEventData *data;

	m_walker->SetList(m_dataList);
	while (m_walker->IsValid()) {
		data = m_walker->GetObj();
		Assert(data);
		if (data) {
			EffectActor		*actor = data->explodeActor;
			if (actor) {
				sint32			x, y;
				actor->GetPixelPos(x, y);
				actor->DrawDirectWithFlags(surface, x, y, k_DRAWFLAGS_NORMAL | k_BIT_DRAWFLAGS_ADDITIVE);
			}
		}
		m_walker->Next();
	}	
}



BattleViewActor *BattleEvent::GetActor(void)
{
	if (m_dataList->GetCount() <= 0) return NULL;

	PointerList<BattleEventData>::PointerListNode	*node = NULL;
	BattleEventData									*eventData = NULL;
	BattleViewActor									*actor = NULL;

	node = m_dataList->GetHeadNode();
	Assert(node);

	eventData = node->GetObj();
	Assert(eventData);

	switch (m_type) {
	case BATTLE_EVENT_TYPE_PLACEMENT:
		actor = eventData->positionActor;
		break;
	case BATTLE_EVENT_TYPE_ATTACK:
		actor = eventData->attackActor;
		break;
	case BATTLE_EVENT_TYPE_EXPLODE:
		actor = eventData->explodeVictim;
		break;
	case BATTLE_EVENT_TYPE_DEATH:
		actor = eventData->deathVictim;
		break;
	}

	return actor;
}

void BattleEvent::RemoveDeadActor(BattleViewActor *deadActor)
{
	
	PointerList<BattleEventData>::Walker walk(m_dataList);
	for(; walk.IsValid(); walk.Next()) {
		BattleEventData *data = walk.GetObj();
		
		if(deadActor == data->actor) {
			
			data->actor = NULL;
		}
	}
}
