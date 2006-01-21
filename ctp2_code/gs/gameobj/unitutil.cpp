//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : Unit utilities
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
// - Added unitutil_GetSmallCityMaxSize to figure out the maximum population
//   size a ring one city. - Oct. 6th 2004 Martin Gühmann
// - EMOD TO DO add check for buildings and wonder for ring size
//
//----------------------------------------------------------------------------

#include "c3.h"
#include "UnitRecord.h"
#include "unitutil.h"
#include "World.h"
#include "SpecialAttackInfoRecord.h"
#include "UnitActor.h"
#include "Cell.h"
#include "Army.h"
#include "cellunitlist.h"
#include "UnitData.h"
#include "ConstDB.h"
#include "GameEventUser.h"
#include "player.h"
#include "AICause.h"
#include "FeatTracker.h"

#include "UnitPool.h"
extern UnitPool *g_theUnitPool;

static sint32 s_maxDefenseRange;
static sint32 s_maxVisionRange;

#include "CitySizeRecord.h"

static sint32 s_smallCityMaxSize;
static sint32 s_maxCitySquaredRadius;

static const SpecialAttackInfoRecord *s_specialAttackMap[SPECATTACK_MAX];

static const SpecialAttackInfoRecord *unitutil_GetSpecialAttackByName(const char *name);

void unitutil_Initialize()
{
	sint32 i;
	s_maxDefenseRange = 0;
	s_maxVisionRange = 0;

	for(i = 0; i < g_theUnitDB->NumRecords(); i++) {
		const UnitRecord *rec = g_theUnitDB->Get(i);
		if(rec->GetActiveDefenseRange() > s_maxDefenseRange)
			s_maxDefenseRange = rec->GetActiveDefenseRange();

		if(rec->GetVisionRange() > s_maxVisionRange)
			s_maxVisionRange = rec->GetVisionRange();
	}
	
	
	for(i = 0; i < SPECATTACK_MAX; i++) {
		s_specialAttackMap[i] = NULL;
	}

	s_specialAttackMap[SPECATTACK_NUKE] = unitutil_GetSpecialAttackByName("NUKE");
	s_specialAttackMap[SPECATTACK_HEARGOSSIP] = unitutil_GetSpecialAttackByName("HEARGOSSIP");
	s_specialAttackMap[SPECATTACK_THROWPARTY] = unitutil_GetSpecialAttackByName("THROWPARTY");
	s_specialAttackMap[SPECATTACK_ESTABLISHEMBASSY] = unitutil_GetSpecialAttackByName("ESTABLISHEMBASSY");
	s_specialAttackMap[SPECATTACK_INCITEREVOLUTION] = unitutil_GetSpecialAttackByName("INCITEREVOLUTION");
	s_specialAttackMap[SPECATTACK_BOMBCABINET] = unitutil_GetSpecialAttackByName("BOMBCABINET");
	s_specialAttackMap[SPECATTACK_CREATEFRANCHISE] = unitutil_GetSpecialAttackByName("CREATEFRANCHISE");
	s_specialAttackMap[SPECATTACK_CAUSEUNHAPPINESS] = unitutil_GetSpecialAttackByName("CAUSEUNHAPPINESS");
	s_specialAttackMap[SPECATTACK_CONDUCTHIT] = unitutil_GetSpecialAttackByName("CONDUCTHIT");
	s_specialAttackMap[SPECATTACK_BIOTERROR] = unitutil_GetSpecialAttackByName("BIOTERROR");
	s_specialAttackMap[SPECATTACK_NANOTERROR] = unitutil_GetSpecialAttackByName("NANOTERROR");
	s_specialAttackMap[SPECATTACK_SLAVERAID] = unitutil_GetSpecialAttackByName("SLAVERAID");
	s_specialAttackMap[SPECATTACK_ENSLAVESETTLER] = unitutil_GetSpecialAttackByName("ENSLAVESETTLER");
	s_specialAttackMap[SPECATTACK_SLAVEUPRISING] = unitutil_GetSpecialAttackByName("SLAVEUPRISING");
	s_specialAttackMap[SPECATTACK_FREESLAVES] = unitutil_GetSpecialAttackByName("FREESLAVES");
	s_specialAttackMap[SPECATTACK_SELLINDULGENCE] = unitutil_GetSpecialAttackByName("SELLINDULGENCE");
	s_specialAttackMap[SPECATTACK_CONVERTCITY] = unitutil_GetSpecialAttackByName("CONVERTCITY");
	s_specialAttackMap[SPECATTACK_PLANTNUKE] = unitutil_GetSpecialAttackByName("PLANTNUKE");
	s_specialAttackMap[SPECATTACK_SOOTHSAY] = unitutil_GetSpecialAttackByName("SOOTHSAY");
	s_specialAttackMap[SPECATTACK_CREATEPARK] = unitutil_GetSpecialAttackByName("CREATEPARK");
	s_specialAttackMap[SPECATTACK_INJOIN] = unitutil_GetSpecialAttackByName("INJOIN");
	s_specialAttackMap[SPECATTACK_SPY] = unitutil_GetSpecialAttackByName("SPY");
	s_specialAttackMap[SPECATTACK_STEALTECH] = unitutil_GetSpecialAttackByName("STEALTECH");
	s_specialAttackMap[SPECATTACK_REVOLUTION] = unitutil_GetSpecialAttackByName("REVOLUTION");
	s_specialAttackMap[SPECATTACK_REFORMCITY] = unitutil_GetSpecialAttackByName("REFORMCITY");

#ifdef _DEBUG
	for(i = 0; i < g_theUnitDB->NumRecords(); i++) {
		DPRINTF(k_DBG_GAMESTATE, ("Unit %d: %s\n", i, g_theUnitDB->Get(i)->GetNameText()));
	}
#endif
	sint32 min = 0x7fffffff;
	sint32 candidate;
	s_smallCityMaxSize = 0x7fffffff;
	s_maxCitySquaredRadius = 0;

	for(i = 0; i < g_theCitySizeDB->NumRecords(); ++i){
		candidate = g_theCitySizeDB->Get(i)->GetPopulation();
		if(candidate < min){
			s_smallCityMaxSize = min;
			min = candidate;
		}
		else if(candidate >= min && candidate < s_smallCityMaxSize){
			s_smallCityMaxSize = candidate;
		}
		if(s_maxCitySquaredRadius < g_theCitySizeDB->Get(i)->GetSquaredRadius()){
			s_maxCitySquaredRadius = g_theCitySizeDB->Get(i)->GetSquaredRadius();
		}
		// EMOD add else if or if here? to check built buildings and wonders and get squared Radius


	}
}

sint32 unitutil_GetSmallCityMaxSize()
{
	return s_smallCityMaxSize;
}
sint32 unitutil_GetMaxRadius(){
	return s_maxCitySquaredRadius;
}

sint32 unitutil_MaxActiveDefenseRange()
{
	return s_maxDefenseRange;
}

sint32 unitutil_GetMaxVisionRange()
{
	return s_maxVisionRange;
}

sint32 unitutil_GetLandCity()
{
	sint32 i;
	for(i = 0; i < g_theUnitDB->NumRecords(); i++) {
		if(g_theUnitDB->Get(i)->GetMovementTypeLand() &&
		   g_theUnitDB->Get(i)->GetHasPopAndCanBuild()) {
			return i;
		}
	}
	Assert(FALSE);

	return 0;
}

sint32 unitutil_GetSeaCity()
{
	sint32 i;
	for(i = 0; i < g_theUnitDB->NumRecords(); i++) {
		if(g_theUnitDB->Get(i)->GetMovementTypeSea() &&
		   g_theUnitDB->Get(i)->GetHasPopAndCanBuild()) {
			return i;
		}
	}
	Assert(FALSE);

	return 0;
}
	
sint32 unitutil_GetCityTypeFor(const MapPoint &pos)
{
	if(g_theWorld->IsLand(pos))
		return unitutil_GetLandCity();
	else
		return unitutil_GetSeaCity();
}

static const SpecialAttackInfoRecord *unitutil_GetSpecialAttackByName(const char *name)
{
	sint32 index;
	if(g_theSpecialAttackInfoDB->GetNamedItem(name, index))
		return g_theSpecialAttackInfoDB->Get(index);
	return NULL;
}

const SpecialAttackInfoRecord *unitutil_GetSpecialAttack(SPECATTACK attack)
{
	return s_specialAttackMap[attack];
}

void unitutil_GetAverageDefenseBonus(const MapPoint &pos, const Army &attackers, const CellUnitList &defenders, double & city_bonus, double & entrenched_bonus)
{
	const CityData *cityData;
	const Cell *cell;
	city_bonus = 0.0;
	entrenched_bonus = 0.0;
	const UnitRecord *rec;
	sint32 i;
	
	cell = g_theWorld->GetCell(pos);
	if(cell->GetCity().m_id != (0)) {
		cityData = cell->GetCity().GetData()->GetCityData();
		Assert(cityData);

		for (i=0; i<attackers.Num(); i++) {
			rec = attackers[i].GetDBRec();
			if(rec->GetIgnoreCityWalls()) {
				city_bonus += cityData->GetDefendersBonusNoWalls();
			} else {
				city_bonus += cityData->GetDefendersBonus();

				
				double wallval=g_featTracker->GetAdditiveEffect(FEAT_EFFECT_REDUCE_CITY_WALLS, attackers.GetOwner());
				if(wallval)
				{
					city_bonus += wallval;
				}

			}
		}
	}

	for (i=0; i < defenders.Num(); i++) {
		if(defenders[i].IsEntrenched()) {
			entrenched_bonus += g_theConstDB->GetEntrenchmentBonus();
		}
	}

	city_bonus /= attackers.Num();
	entrenched_bonus /= defenders.Num();
}

bool unitutil_GetCityInfo(MapPoint &pos, char * city_name, sint32 & image_index)
{
	const CityData *cityData;
	const Cell *cell;
	image_index = -1;

	cell = g_theWorld->GetCell(pos);

	if(cell->GetCity().m_id != (0)) {
		cityData = cell->GetCity().GetData()->GetCityData();
		Assert(cityData);

		BOOL hasWalls = cityData->HasCityWalls();
		BOOL hasForceField = cityData->HasForceField();

		
		BOOL futureAge = 0; 

		if (futureAge) {
			
			if (hasForceField) {
				image_index = 3;
			} else {
				if (hasWalls) {
					
					image_index = 2;
				} else {
					image_index = 2;
				}
			}
		} else {
			
			if (hasWalls) {
				image_index = 1;
			} else {
				image_index = 0;
			}
		}

		Assert(city_name);
		strcpy(city_name, cell->GetCity().GetName());
		return true;
	}
	return false;
}


void unitutil_ExecuteMadLaunch(Unit & unit)
{
	
	if( unit.GetDBRec()->GetNuclearAttack() && 
		g_theUnitPool->IsValid(unit->GetTargetCity()) &&
		!unit.Flag(k_UDF_MAD_LAUNCHED)) {
		
		
		
		
		if (unit.IsBeingTransported())
		{
			Unit transport = unit.GetTransport();
			Army debark = g_player[unit.GetOwner()]->GetNewArmy(CAUSE_NEW_ARMY_TRANSPORTED);
			transport.UnloadCargo(transport.RetPos(), debark, TRUE, unit);
		} else {
			unit->CreateOwnArmy();
		}
		
		unit.SetFlag(k_UDF_MAD_LAUNCHED);
		unit.SetMovementPoints(unit.GetDBRec()->GetMaxMovePoints());
		g_gevManager->AddEvent(GEV_INSERT_AfterCurrent, GEV_MADLaunch,
			GEA_Unit, unit.m_id,
			GEA_End);
	}
}
