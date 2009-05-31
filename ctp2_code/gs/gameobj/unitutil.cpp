//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : Unit utilities
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
// - Added unitutil_GetSmallCityMaxSize to figure out the maximum population
//   size a ring one city. - Oct. 6th 2004 Martin Gühmann
// - EMOD TO DO add check for buildings and wonder for ring size
// - Added function to compare unit type quality, based on unit cargo capacity
//   or on the units statistics like attack, defense and range. (19-May-2007 Martin Gühmann)
// - Replaced old const database by new one. (5-Aug-2007 Martin Gühmann)
// - Added GetCityLandAttackBonus, GetCityAirAttackBonus and GetCitySeaAttackBonus
//   for battleview window. Moved leader bonus from positiondefense, 
//   to unitdata::getdefense. (07-Mar-2009 Maq)
//
//----------------------------------------------------------------------------

#include "c3.h"
#include "Unit.h"
#include "unitutil.h"

#include "UnitRecord.h"
#include "World.h"
#include "SpecialAttackInfoRecord.h"
#include "UnitActor.h"
#include "Cell.h"
#include "Army.h"
#include "cellunitlist.h"
#include "UnitData.h"
#include "ConstRecord.h"
#include "GameEventUser.h"
#include "player.h"
#include "AICause.h"
#include "FeatTracker.h"
#include "buildingutil.h"
#include "terrainutil.h"

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
	city_bonus = 0.0;
	entrenched_bonus = 0.0;
	const UnitRecord *rec;
	sint32 i;
	
	const Cell *    cell = g_theWorld->GetCell(pos);
	if (cell->GetCity().IsValid()) 
    {
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
			entrenched_bonus += g_theConstDB->Get(0)->GetEntrenchmentBonus();
		}
	}

	city_bonus /= attackers.Num();
	entrenched_bonus /= defenders.Num();
}

void unitutil_GetCityLandAttackBonus(const MapPoint &pos, double & city_landatk_bonus)
{
	const CityData *cityData;
	city_landatk_bonus = 0.0;

	const Cell *    cell = g_theWorld->GetCell(pos);
	if (cell->GetCity().IsValid()) 
    {
		cityData = cell->GetCity().GetData()->GetCityData();
		Assert(cityData);
		city_landatk_bonus += cityData->GetCityLandAttackBonus();
	}
}

void unitutil_GetCityAirAttackBonus(const MapPoint &pos, double & city_airatk_bonus)
{
	const CityData *cityData;
	city_airatk_bonus = 0.0;

	const Cell *    cell = g_theWorld->GetCell(pos);
	if (cell->GetCity().IsValid()) 
    {
		cityData = cell->GetCity().GetData()->GetCityData();
		Assert(cityData);
		city_airatk_bonus += cityData->GetCityAirAttackBonus();
	}
}

void unitutil_GetCitySeaAttackBonus(const MapPoint &pos, double & city_seaatk_bonus)
{
	const CityData *cityData;
	city_seaatk_bonus = 0.0;

	const Cell *    cell = g_theWorld->GetCell(pos);
	if (cell->GetCity().IsValid()) 
    {
		cityData = cell->GetCity().GetData()->GetCityData();
		Assert(cityData);
		city_seaatk_bonus += cityData->GetCitySeaAttackBonus();
	}
}

bool unitutil_GetCityInfo(MapPoint &pos, char * city_name, sint32 & image_index)
{
	const CityData *cityData;
	image_index = -1;

	const Cell *    cell = g_theWorld->GetCell(pos);

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
	
	if( unit.GetDBRec()->HasNuclearAttack() && 
		unit->GetTargetCity().IsValid() &&
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

bool unitutil_IsUnitBetterThan(sint32 type1, sint32 type2, sint32 gov)
{
	if(type2 <= -1) return true;
	if(type1 <= -1) return false;
	const UnitRecord* unitRec1 = g_theUnitDB->Get(type1, gov);
	const UnitRecord* unitRec2 = g_theUnitDB->Get(type2, gov);

	// Transport units should always be bigger:
	const UnitRecord::CargoData* cargo1 = unitRec1->GetCargoDataPtr();
	const UnitRecord::CargoData* cargo2 = unitRec2->GetCargoDataPtr();
	if(cargo1 && cargo2){
		if(cargo1->GetMaxCargo() > cargo2->GetMaxCargo())
		{
			return true;
		}
		else if(cargo1->GetMaxCargo() < cargo2->GetMaxCargo())
		{
			return false;
		}
	}

	// Equal transport strength or just one a transporter or none a transporter:
	// Test the battle strength

	// Attack difference
	double const attack_cpr  = (unitRec1->GetAttack()         - unitRec2->GetAttack());
	// Defense difference
	double const defense_cpr = (unitRec1->GetDefense()        - unitRec2->GetDefense());
	// ranged difference
	double const ranged_cpr  = (unitRec1->GetZBRangeAttack()  - unitRec2->GetZBRangeAttack());

	double const battle_cpr  = attack_cpr + defense_cpr + ranged_cpr;
	if (battle_cpr > 0)
	{
		return true;
	}
	else if (battle_cpr < 0)
	{
		return false;
	}

	return false;
}

double unitutil_GetPositionDefense(const UnitRecord * rec, const bool isEntrenched, const MapPoint pos, const Unit &attacker)
{
	if (g_theWorld->IsWater(pos) && 
	    !(rec->GetMovementTypeSea() || rec->GetMovementTypeShallowWater())
	   )
	{
		return 1.0;
	}

	Cell *          cell    = g_theWorld->GetCell(pos);
	double const    basedef = rec->GetDefense();
	double          def     = basedef;

	if(cell->GetCity().m_id != (0))
	{
		const CityData *cityData = 
			cell->GetCity().GetData()->GetCityData();
		Assert(cityData);
		def += cityData->GetDefendersBonus();

		if (cityData->HasCityWalls() && attacker.IsValid())
		{
			if(g_featTracker->GetAdditiveEffect(FEAT_EFFECT_REDUCE_CITY_WALLS, attacker.GetOwner()) > 0)
			{
				def += buildingutil_GetCityWallsDefense(cityData->GetEffectiveBuildings(), cityData->GetOwner());
			}
//			double deductwall;
//			deductwall = myRec->GetReducesDefensesBonus();
//			if(deductwall)
//			{
//				def -= deductwall;
//			}
		}
	}

//	if(Flag(k_UDF_IS_ENTRENCHED))
	if(isEntrenched)
	{
		def += basedef * g_theConstDB->Get(0)->GetEntrenchmentBonus();
	}

	double terrain_bonus = 0.0;
	double fort_bonus = 0.0;
	terrainutil_GetDefenseBonus(pos, terrain_bonus, fort_bonus);

	def += (basedef * fort_bonus);

	
	if(terrain_bonus > 0 && 
		(rec->GetMovementTypeLand() && g_theWorld->IsLand(pos)) ||
		(rec->GetMovementTypeMountain() && g_theWorld->IsMountain(pos)) ||
		(rec->GetMovementTypeSea() && g_theWorld->IsWater(pos)) ||
		(rec->GetMovementTypeSpace() && g_theWorld->IsSpace(pos))) 
	{
		def += basedef * terrain_bonus;
	}

	return def;
}
