//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : Battle view actor handling
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
// - Removed unnecessary include files. (Aug 28th 2005 Martin Gühmann)
//
//----------------------------------------------------------------------------

#include "c3.h"

#include "Army.h"
#include "World.h"
#include "Cell.h"
#include "cellunitlist.h"
#include "player.h"
#include "colorset.h"

#include "battleevent.h"
#include "battle.h"
#include "battleviewactor.h"
#include "EffectActor.h"
#include "soundmanager.h"
#include "SpriteState.h"

#include "UnitRecord.h"
#include "SpecialEffectRecord.h"

#include "terrainutil.h"
#include "unitutil.h"
#include "ArmyData.h"

extern World		*g_theWorld;
extern ColorSet		*g_colorSet;
extern Player		**g_player;


Battle::Battle() :
m_eventQueue(new PointerList<BattleEvent>),
m_cityBonus(0.0),
m_cityImage(-1),
m_terrainBonus(0.0),
m_terrainType(0),
m_fortBonus(0.0),
m_fortifiedBonus(0.0)
{
	
	m_attackersName[0] = '\0';
	m_defendersName[0] = '\0';

	
	for(sint32 index=0; index < k_MAX_UNITS_PER_SIDE; index++) {
		m_attackers[index] = NULL;
		m_defenders[index] = NULL;
	}
}


Battle::~Battle()
{
	
	
	
	
	
	if(m_eventQueue)
		delete m_eventQueue;
}






PointerList<BattleEvent> *Battle::GrabEventQueue()
{
	
	PointerList<BattleEvent> *eventQueue = m_eventQueue;

	
	m_eventQueue = new PointerList<BattleEvent>;

	
	return(eventQueue);
}


void Battle::Initialize(const Army &attackers, CellUnitList &defenders)
{
	m_numAttackers = attackers.Num();
	m_numDefenders = defenders.Num();

	
	MakeAttackers(m_numAttackers, attackers);
	MakeDefenders(m_numDefenders, defenders);

	
	MapPoint		defPos;
	defenders.GetPos(defPos);

	terrainutil_GetDefenseBonus(defPos, m_terrainBonus, m_fortBonus);

	unitutil_GetAverageDefenseBonus(defPos, attackers, defenders, m_cityBonus, m_fortifiedBonus);
	unitutil_GetCityInfo(defPos, m_cityName, m_cityImage);

	
	MapPoint pos = defenders[0].RetPos();
	Cell *cell = g_theWorld->GetCell(pos);
	m_terrainType = cell->GetTerrainType();

	m_attackerTerrainType = g_theWorld->GetCell(attackers->RetPos())->GetTerrainType();

	m_attackersColor = g_colorSet->ComputePlayerColor(attackers[0].GetOwner());
	m_defendersColor = g_colorSet->ComputePlayerColor(defenders[0].GetOwner());

	g_player[attackers[0].GetOwner()]->GetPluralCivName(m_attackersName);
	g_player[defenders[0].GetOwner()]->GetPluralCivName(m_defendersName);

}



























































































































void Battle::MakeAttackers(sint32 numAttackers, const Army &attackers)
{
	sint32			i;
	MapPoint		pos;

	pos.x = pos.y = 0;

	for (i=0; i<numAttackers; i++) {
		m_attackers[i] = new BattleViewActor(const_cast<Unit &>(attackers[i]).GetSpriteState(), attackers[i],
										attackers[i].GetType(), pos,  attackers[i].GetOwner());
		m_attackers[i]->SetFacing(k_BATTLEVIEW_DEFAULT_ATTACKER_FACING);

		DPRINTF(k_DBG_FIX, ("MakeAttackers: Actor with Unit id: %.8lx\n", m_attackers[i]->GetUnitID().m_id));
	}
}



void Battle::MakeDefenders(sint32 numDefenders, CellUnitList &defenders)
{
	sint32			i;
	MapPoint		pos;

	pos.x = pos.y = 0;

	for (i=0; i<numDefenders; i++) {
		m_defenders[i] = new BattleViewActor(defenders[i].GetSpriteState(), defenders[i],
										defenders[i].GetType(), pos,  defenders[i].GetOwner());
		m_defenders[i]->SetFacing(k_BATTLEVIEW_DEFAULT_DEFENDER_FACING);
		m_defenders[i]->SetFortified(defenders[i].IsEntrenched());
		DPRINTF(k_DBG_FIX, ("MakeDefenders: Actor with Unit id: %.8lx\n", m_defenders[i]->GetUnitID().m_id));
	}
}



BattleViewActor *Battle::ActorFromUnit(BOOL isDefender, Unit theUnit)
{
	BattleViewActor		*actor = NULL;
	sint32				i;

	if (isDefender) {
		for (i=0; i<m_numDefenders; i++) {
			if (m_defenders[i] && m_defenders[i]->GetUnitID() == theUnit) {
				actor = m_defenders[i];
				break;
			}
		}
	} else {
		for (i=0; i<m_numAttackers; i++) {
			if (m_attackers[i] && m_attackers[i]->GetUnitID() == theUnit) {
				actor = m_attackers[i];
				break;
			}
		}
	}
	
	Assert(actor);

	if (!actor) {
		DPRINTF(k_DBG_FIX, ("ActorFromUnit: Unit %.8lx not found in Actor lists\n", theUnit.m_id));
		for (i=0; i<m_numAttackers; i++) {
			DPRINTF(k_DBG_FIX, ("ActorFromUnit: Attacker Actor with id: %.8lx\n", 
			        m_attackers[i] ? m_attackers[i]->GetUnitID().m_id : 0xabcdef12));
		}
		for (i=0; i<m_numDefenders; i++) {
			DPRINTF(k_DBG_FIX, ("ActorFromUnit: Defender Actor with id: %.8lx\n", 
			        m_defenders[i] ? m_defenders[i]->GetUnitID().m_id : 0xfedcba21));
		}
	}

	return actor;
}

void Battle::PositionUnit(BattleEvent *event, BOOL isDefender, Unit theUnit, sint32 column, sint32 row, bool sethp)
{
	BattleViewActor		*actor;

	actor = ActorFromUnit(isDefender, theUnit);
	Assert(actor);
	if (!actor) return;

	if(sethp) {
		actor->SetHitPoints(theUnit.GetHP());
		actor->SetHitPointsMax(theUnit.GetDBRec()->GetMaxHP());
	}

	if (isDefender) {
		event->AddPositionData(actor, column, row, k_BATTLEVIEW_DEFAULT_DEFENDER_FACING, 
								theUnit.GetHP(), isDefender);
	} else {
		event->AddPositionData(actor, column, row, k_BATTLEVIEW_DEFAULT_ATTACKER_FACING, 
								theUnit.GetHP(), isDefender);
	}
}



void Battle::AddUnitAttack(BattleEvent *event, BOOL isDefender, Unit theUnit)
{
	BattleViewActor		*actor;

	DPRINTF(k_DBG_FIX, ("Add Attack for %s %d (%d)\n", theUnit.GetName(), theUnit.GetOwner(), int(theUnit) & 0xFFF));

	actor = ActorFromUnit(isDefender, theUnit);
	Assert(actor);
	if (!actor) return;

	sint32 soundId = theUnit.IsValid() ? theUnit.GetAttackSoundID() : -1;
	event->AddAttackData(actor, soundId, actor->GetHitPoints());
}



void Battle::AddUnitExplosion(BattleEvent *event, BOOL isDefender, Unit theUnit)
{
	BattleViewActor		*actor;

	DPRINTF(k_DBG_FIX, ("Add Explosion for %s %d (%d)\n", theUnit.GetName(), theUnit.GetOwner(), int(theUnit) & 0xFFF));

	actor = ActorFromUnit(isDefender, theUnit);
	Assert(actor);
	if (!actor) return;

	if(theUnit.IsValid() && !theUnit.GetDBRec()->GetExplodes())
		return;

	

	EffectActor		*explodeActor = NULL;
	SpriteState		*explosionState = new SpriteState(g_theSpecialEffectDB->Get(
																				g_theSpecialEffectDB->FindTypeIndex("SPECEFFECT_EXPLOSION_LAND_1"))->GetValue());

	if (explosionState && explosionState->GetIndex() != -1) {
		MapPoint		pos = theUnit.RetPos();

		explodeActor = new EffectActor(explosionState, pos);
	}

	event->AddExplosionData(actor, explodeActor, gamesounds_GetGameSoundID(GAMESOUNDS_EXPLOSION), 
							theUnit.GetHP());
}



void Battle::AddUnitDeath(BattleEvent *event, BOOL isDefender, Unit theUnit)
{
	BattleViewActor		*actor;

	DPRINTF(k_DBG_FIX, ("Add Death for %s %d (%d)\n", theUnit.GetName(), theUnit.GetOwner(), int(theUnit) & 0xFFF));

	actor = ActorFromUnit(isDefender, theUnit);
	
	Assert(actor);
	if (!actor) return;

	event->AddDeathData(actor, theUnit.GetDeathSoundID(), theUnit.GetHP());
}



void Battle::AddEvent(BattleEvent *event)
{
	Assert(event);
	if (!event) return;



	

	Assert(m_eventQueue);
	if (m_eventQueue)
		m_eventQueue->AddTail(event);
}



void Battle::ShowEvent(BattleEvent *event)
{
	MBCHAR		*s;

	switch (event->GetType()) {
	case BATTLE_EVENT_TYPE_PLACEMENT :
			s = "Placement";
		break;
	case BATTLE_EVENT_TYPE_ATTACK :
			s = "Attack";
		break;
	case BATTLE_EVENT_TYPE_EXPLODE :
			s = "Explode";
		break;
	case BATTLE_EVENT_TYPE_DEATH :
			s = "Death";
		break;
	}

	DPRINTF(k_DBG_FIX, ("BattleEvent: %s\n", s));

	PointerList<BattleEventData>	*list = event->GetDataList();

	PointerList<BattleEventData>::Walker *walker = new PointerList<BattleEventData>::Walker(list);

	DPRINTF(k_DBG_FIX, ("  %d data elements.\n", list->GetCount()));
	sint32		index = 0;
	while (walker->IsValid()) {
		BattleEventData		*data = walker->GetObj();
		DPRINTF(k_DBG_FIX, ("    Data element %d:\n", index));

		switch (event->GetType()) {
		case BATTLE_EVENT_TYPE_PLACEMENT :
			DPRINTF(k_DBG_FIX, ("      positionActor  :  %#.8lx\n", data->positionActor));
			DPRINTF(k_DBG_FIX, ("      positionColumn :  %d\n", data->positionColumn));
			DPRINTF(k_DBG_FIX, ("      positionRow    :  %d\n", data->positionRow));
			DPRINTF(k_DBG_FIX, ("      positionFacing :  %d\n", data->positionFacing));
			DPRINTF(k_DBG_FIX, ("      positionHP     :  %#.2f\n", data->positionHP));
			DPRINTF(k_DBG_FIX, ("      positionIsDefen:  %d\n", data->positionIsDefender));
			break;
		case BATTLE_EVENT_TYPE_ATTACK :
			DPRINTF(k_DBG_FIX, ("      attackActor    :  %#.8lx\n", data->attackActor));
			DPRINTF(k_DBG_FIX, ("      attackSoundID  :  %d\n", data->attackSoundID));
			DPRINTF(k_DBG_FIX, ("      attackHP       :  %#.2f\n", data->attackHP));
			break;
		case BATTLE_EVENT_TYPE_EXPLODE :
			DPRINTF(k_DBG_FIX, ("      explodeVictim  :  %#.8lx\n", data->explodeVictim));
			DPRINTF(k_DBG_FIX, ("      explodeActor   :  %#.8lx\n", data->explodeActor));
			DPRINTF(k_DBG_FIX, ("      explodeSoundID :  %d\n", data->explodeSoundID));
			DPRINTF(k_DBG_FIX, ("      explodeHP      :  %#.2f\n", data->explodeHP));
			break;
		case BATTLE_EVENT_TYPE_DEATH :
			DPRINTF(k_DBG_FIX, ("      deathVictim    :  %#.8lx\n", data->deathVictim));
			DPRINTF(k_DBG_FIX, ("      deathSoundID   :  %d\n", data->deathSoundID));
			DPRINTF(k_DBG_FIX, ("      deathHP        :  %#.2f\n", data->deathHP));
			break;
		}
		DPRINTF(k_DBG_FIX, ("--\n"));

		walker->Next();

		index++;
	}
}



void Battle::RemoveAttacker(BattleViewActor *actor)
{
	sint32 i;
	for(i = 0; i < m_numAttackers; i++) {
		if(m_attackers[i] == actor)
			m_attackers[i] = NULL;
	}
}

void Battle::RemoveDefender(BattleViewActor *actor)
{
	sint32 i;
	for(i = 0; i < m_numDefenders; i++) {
		if(m_defenders[i] == actor)
			m_defenders[i] = NULL;
	}
}

