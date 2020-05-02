//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : Director events
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
// - Special attack centers only if the auto center option on units and
//   cities is set. (23-Feb-2008 Martin Gühmann)
// - Stopped centering map on stealth units that you can't see. (12-Apr-2009 Maq)
//
//----------------------------------------------------------------------------

#include "c3.h"
#include "GameEventUser.h"
#include "Events.h"

#include "Army.h"
#include "ArmyData.h"
#include "director.h"
#include "tiledmap.h"
#include "SelItem.h"
#include "UnitActor.h"
#include "UnitData.h"
#include "unitutil.h"
#include "UnitPool.h"
#include "player.h"

#include "soundmanager.h"

#include "SoundRecord.h"
#include "SpecialEffectRecord.h"
#include "SpecialAttackInfoRecord.h"

#include "network.h"
#include "profileDB.h"

STDEHANDLER(DirectorMoveUnitsEvent)
{
	Army a;
	MapPoint from, to;

	if(!args->GetArmy(0, a)) return GEV_HD_Continue;
	if(!args->GetPos(0, from)) return GEV_HD_Continue;
	if(!args->GetPos(1, to)) return GEV_HD_Continue;

	if(a.Num() <= 0) return GEV_HD_Continue;

	if(a->IsStealth()
		&& !a->IsVisible(g_selected_item->GetPlayerOnScreen())) return GEV_HD_Continue;

//	BOOL theTileIsVisible = g_tiledMap->TileIsCompletelyVisible(to.x, to.y);

	if (g_selected_item->GetPlayerOnScreen() != -1 &&
		g_selected_item->GetPlayerOnScreen() != g_selected_item->GetVisiblePlayer() &&
		!g_network.IsActive())

			return GEV_HD_Continue;

	Unit top_src = a->GetTopVisibleUnit(g_selected_item->GetVisiblePlayer());
	if (top_src.m_id == 0)
		top_src = a[0];

	sint32 numRevealed = 0;

	UnitActor **restOfStack = NULL;
	sint32 numRest = a->Num() - 1;

	if (numRest > 0) {
		restOfStack = new (UnitActor* [numRest]);
		a->GetActors(top_src, restOfStack);
	}

	UnitActor	**revealedActors = NULL;

	if (numRevealed > 0) {

		// Something missing here.

	} else {
		revealedActors = NULL;
	}

	MapPoint newPos = to;

	if(g_selected_item->IsAutoCenterOn()
	&& !g_director->TileWillBeCompletelyVisible(newPos.x, newPos.y)
	&& (top_src.GetVisibility() & (1 << g_selected_item->GetVisiblePlayer()))
	&& (   g_theProfileDB->IsEnemyMoves()
	    || top_src.GetOwner() == g_selected_item->GetVisiblePlayer()
	   )
	){
		g_director->AddCenterMap(newPos);
	}

	if (!to.IsNextTo(from)) {
		g_director->AddTeleport(top_src, from, newPos, numRevealed, revealedActors,
								numRest, restOfStack);
	} else {
		g_director->AddMove(top_src, from, newPos, numRevealed, revealedActors,
							numRest, restOfStack, FALSE, top_src.GetMoveSoundID());

	}

	if (top_src.GetData()->HasLeftMap())
		g_director->AddHide(top_src);

	return GEV_HD_Continue;
}

STDEHANDLER(DirectorActionSuccessful)
{
	Unit unit, c;
	if(!args->GetUnit(0, unit)) return GEV_HD_Continue;

	MapPoint pos = unit.RetPos();
	static MapPoint attackPos;

	if(!args->GetCity(0,c)) {
		if(!args->GetUnit(1, c)) {
			if(!args->GetPos(0, attackPos)) {
				attackPos = pos;
			}
		} else {
			attackPos = c.RetPos();
		}
	} else {
		attackPos = c.RetPos();
	}

	SPECATTACK attack;
	switch(gameEventType)
	{
		case GEV_InciteRevolutionUnit: attack = SPECATTACK_INCITEREVOLUTION; break;
		case GEV_AssassinateRulerUnit: attack = SPECATTACK_BOMBCABINET; break;
		case GEV_MakeFranchise: attack = SPECATTACK_CREATEFRANCHISE; break;
		case GEV_PlantNukeUnit: attack = SPECATTACK_PLANTNUKE; break;
		case GEV_SlaveRaidCity: attack = SPECATTACK_SLAVERAID; break;
		case GEV_NukeCityUnit: attack = SPECATTACK_NUKE; break;
		case GEV_NukeLocationUnit: attack = SPECATTACK_NUKE; break;
		case GEV_EnslaveSettler: attack = SPECATTACK_ENSLAVESETTLER; break;
		case GEV_InciteUprisingUnit: attack = SPECATTACK_SLAVEUPRISING; break;
		case GEV_EstablishEmbassyUnit: attack = SPECATTACK_ESTABLISHEMBASSY; break;
		case GEV_BioInfectCityUnit: attack = SPECATTACK_BIOTERROR; break;
		case GEV_NanoInfectCityUnit: attack = SPECATTACK_NANOTERROR; break;
		case GEV_ConvertCityUnit: attack = SPECATTACK_CONVERTCITY; break;
		case GEV_ReformCityUnit: attack = SPECATTACK_REFORMCITY; break;
		case GEV_IndulgenceSaleMade: attack = SPECATTACK_SELLINDULGENCE; break;
		case GEV_CreateParkUnit: attack = SPECATTACK_CREATEPARK; break;
		case GEV_InjoinUnit: attack = SPECATTACK_INJOIN; break;


		case GEV_Lawsuit: attack = SPECATTACK_NONE; break;
		case GEV_RemoveFranchise: attack = SPECATTACK_NONE; break;
		case GEV_ExpelUnits: attack = SPECATTACK_NONE; break;
		case GEV_UndergroundRailwayUnit: attack = SPECATTACK_NONE; break;
		case GEV_PillageUnit: attack = SPECATTACK_NONE; break;

		default:
			attack = SPECATTACK_NONE;
	}

	sint32 soundID, spriteID;
	if(attack != SPECATTACK_NONE)
	{
		const SpecialAttackInfoRecord *rec;
		rec = unitutil_GetSpecialAttack(attack);
		soundID = rec->GetSoundIDIndex();
		spriteID = rec->GetSpriteID()->GetValue();
		if (spriteID != -1 && soundID != -1)
		{
			if(g_selected_item->IsAutoCenterOn())
			{
				if((((unit.GetOwner() == g_selected_item->GetVisiblePlayer()) ||
					 (unit.GetVisibility() & (1 << g_selected_item->GetVisiblePlayer()))) ||
					g_theUnitPool->IsValid(c) &&
					((c.GetOwner() == g_selected_item->GetVisiblePlayer()) ||
					 (c.GetVisibility() & (1 << g_selected_item->GetVisiblePlayer()))))) {

					g_director->AddCenterMap(attackPos);
				}
			}

			if(c.IsValid())
				g_director->AddSpecialAttack(unit, c, attack);
			else
				g_director->AddSpecialEffect(attackPos, spriteID, soundID);

		} else {
			if (soundID != -1) {
				sint32 visiblePlayer = g_selected_item->GetVisiblePlayer();
				if ((visiblePlayer == unit.GetOwner()) ||
					(unit.GetVisibility() & (1 << visiblePlayer))) {

					g_soundManager->AddSound(SOUNDTYPE_SFX, (uint32)0, 	soundID, attackPos.x, attackPos.y);
				}
			}
		}
	}
	else
	{
		if(g_player[g_selected_item->GetVisiblePlayer()]->m_vision->IsVisible(attackPos))
		{
			spriteID = g_theSpecialEffectDB->Get(g_theSpecialEffectDB->FindTypeIndex("SPECEFFECT_GENERAL_SUCCESS"))->GetValue();
			soundID  = g_theSoundDB->FindTypeIndex("SOUND_ID_GENERALSUCCEED");
			if(g_selected_item->IsAutoCenterOn())
			{
				g_director->AddCenterMap(attackPos);
			}

			g_director->AddSpecialEffect(attackPos, spriteID, soundID);
		}
	}
	return GEV_HD_Continue;
}

STDEHANDLER(DirectorReallyBeginScheduler)
{
	if(g_director->GetHoldSchedulerSequence() != NULL) {
		g_director->ActionFinished(g_director->GetHoldSchedulerSequence());
		g_director->SetHoldSchedulerSequence(NULL);
	}
	return GEV_HD_Continue;
}

void directorevent_Initialize()
{
	g_gevManager->AddCallback(GEV_MoveUnits, GEV_PRI_Post, &s_DirectorMoveUnitsEvent);

	g_gevManager->AddCallback(GEV_InciteRevolutionUnit, GEV_PRI_Post, &s_DirectorActionSuccessful);
	g_gevManager->AddCallback(GEV_AssassinateRulerUnit, GEV_PRI_Post, &s_DirectorActionSuccessful);
	g_gevManager->AddCallback(GEV_MakeFranchise, GEV_PRI_Post, &s_DirectorActionSuccessful);
	g_gevManager->AddCallback(GEV_PlantNukeUnit, GEV_PRI_Post, &s_DirectorActionSuccessful);
	g_gevManager->AddCallback(GEV_SlaveRaidCity, GEV_PRI_Post, &s_DirectorActionSuccessful);
	g_gevManager->AddCallback(GEV_NukeCity, GEV_PRI_Post, &s_DirectorActionSuccessful);
	g_gevManager->AddCallback(GEV_EnslaveSettler, GEV_PRI_Post, &s_DirectorActionSuccessful);
	g_gevManager->AddCallback(GEV_InciteUprisingUnit, GEV_PRI_Post, &s_DirectorActionSuccessful);
	g_gevManager->AddCallback(GEV_EstablishEmbassyUnit, GEV_PRI_Post, &s_DirectorActionSuccessful);
	g_gevManager->AddCallback(GEV_BioInfectCityUnit, GEV_PRI_Post, &s_DirectorActionSuccessful);
	g_gevManager->AddCallback(GEV_NanoInfectCityUnit, GEV_PRI_Post, &s_DirectorActionSuccessful);
	g_gevManager->AddCallback(GEV_ConvertCityUnit, GEV_PRI_Post, &s_DirectorActionSuccessful);
	g_gevManager->AddCallback(GEV_ReformCityUnit, GEV_PRI_Post, &s_DirectorActionSuccessful);
	g_gevManager->AddCallback(GEV_IndulgenceSaleMade, GEV_PRI_Post, &s_DirectorActionSuccessful);
	g_gevManager->AddCallback(GEV_CreateParkUnit, GEV_PRI_Post, &s_DirectorActionSuccessful);
	g_gevManager->AddCallback(GEV_PillageUnit, GEV_PRI_Post, &s_DirectorActionSuccessful);
	g_gevManager->AddCallback(GEV_InjoinUnit, GEV_PRI_Post, &s_DirectorActionSuccessful);
	g_gevManager->AddCallback(GEV_Lawsuit, GEV_PRI_Post, &s_DirectorActionSuccessful);
	g_gevManager->AddCallback(GEV_RemoveFranchise, GEV_PRI_Post, &s_DirectorActionSuccessful);
	g_gevManager->AddCallback(GEV_ExpelUnits, GEV_PRI_Post, &s_DirectorActionSuccessful);
	g_gevManager->AddCallback(GEV_UndergroundRailwayUnit, GEV_PRI_Post, &s_DirectorActionSuccessful);
	g_gevManager->AddCallback(GEV_NukeCityUnit, GEV_PRI_Pre, &s_DirectorActionSuccessful);
	g_gevManager->AddCallback(GEV_NukeLocationUnit, GEV_PRI_Pre, &s_DirectorActionSuccessful);

	g_gevManager->AddCallback(GEV_BeginScheduler, GEV_PRI_Post, &s_DirectorReallyBeginScheduler);
}

void directorevent_Cleanup()
{
}
