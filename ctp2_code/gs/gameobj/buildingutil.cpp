//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : Building data handling
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
// - Fixed buildingutil_GetOffenseBonusAir and buildingutil_GetOffenseBonusWater
// - Fix by NelsonAndBronte on 04-11-2003
// - buildingutil_GetGoldPerCity added to work like GoldPerPop (2-15-2006 E)
// - buildingutil_GetGoldPerUnit added to work like GoldPerPop (2-24-2006 E)
// - buildingutil_GetGoldPerUnitReadiness added to work like GoldPerPop (2-24-2006 E)
// - buildingutil_IsObsolete added so buildings can be obsolete like wonders (4-28-2006 E)
// - buildingutil_GetEmbassiesEverywhereEvenAtWar (E)
// - buildingutil_GetIncreaseHP (E)
// - buildingutil_GetTreasuryInterest (E 6.8.2006)
//
//----------------------------------------------------------------------------

#include "c3.h"
#include "buildingutil.h"

#include "BuildingRecord.h"
#include "c3errors.h"
#include "civarchive.h"
#include "ErrMsg.h"
#include "FeatTracker.h"
#include "GovernmentRecord.h"
#include "player.h"             // g_player    
#include "StrDB.h"              // g_theStringDB
#include "Unit.h"
#include "WonderRecord.h"
#include "WonderTracker.h"
#include "wonderutil.h"


#define shiftbit(i) uint64(uint64(0x01) << uint64(i))		

#define FOREACH_BUILT(recordFunc) \
const BuildingRecord *rec; \
sint32 i; \
uint64 cur_built; \
for (i=0,cur_built=built_improvements;cur_built!=0;cur_built>>=1,i++) \
	if ((cur_built&1) && \
		(rec = g_theBuildingDB->Get(i))->recordFunc())



void buildingutil_Initialize()
{
}

sint32 buildingutil_GetProductionCost(const sint32 building_type)
{
	const BuildingRecord * rec = g_theBuildingDB->Get(building_type);
	Assert(rec);
	
	return rec ? rec->GetProductionCost() : 0;
}

sint32 buildingutil_GetTotalUpkeep(const uint64 built_improvements,
										   sint32 wonderLevel, sint32 owner)
{
	//sint32 owner = 0;
	sint32 upkeep = 0;
	for(sint32 i = 0; i < g_theBuildingDB->NumRecords(); i++) {
		if(built_improvements & shiftbit(i)) {
			if(g_theBuildingDB->Get(i)->GetUpkeep() > wonderLevel) {
				upkeep += g_theBuildingDB->Get(i)->GetUpkeep();

				// EMOD added new Upkeep calculations (3-13-2006) restored 12-JULY-2006
				// This doesn't compile E and just adding some parentheses doesn't work either
				// so E do your work. ;)
				// And make the code more readable.
		//		upkeep += g_theBuildingDB->Get(i)->GetUpkeepPerUnitWagesReadiness * g_player[owner]->GetNumUnits() * g_player[owner]->m_readiness->GetSupportModifier(g_player[owner]->GetGovernmentType()) * g_player[owner]->GetWagesPerPerson();
		//		upkeep += g_theBuildingDB->Get(i)->GetUpkeepPerCity * g_player[owner]->GetNumCities() * g_theGovernmentDB->Get(g_player[owner]->GetGovernmentType())->GetTooManyCitiesThreshold();
		//		upkeep += g_theBuildingDB->Get(i)->GetUpkeepPerCitySq * g_player[owner]->GetNumCities() *  g_player[owner]->GetNumCities();
		//		upkeep += g_theBuildingDB->Get(i)->GetUpkeepPerUnit * g_player[owner]->GetNumUnits();
		//		upkeep += g_theBuildingDB->Get(i)->GetUpkeepPerUnitSupport * g_player[owner]->m_readiness->TotalUnitGoldSupport() * g_player[owner]->GetWagesPerPerson() * g_player[owner]->m_readiness->GetSupportModifier(g_player[owner]->GetGovernmentType());
				// End EMOD
			}
		}
	}
	return upkeep;
}

sint32 buildingutil_GetBlgUpkeep(const sint32 building_type)
{
	return g_theBuildingDB->Get(building_type)->GetUpkeep();
}


sint32 buildingutil_GetCheapestBuilding(const uint64 built_improvements, 
												sint32 wonderLevel)
{
	sint32 lowcost = 0x7fffffff;
	sint32 low = -1;

	for(sint32 i = 0; i < g_theBuildingDB->NumRecords(); i++) {
		if(built_improvements & shiftbit(i)) {
			if(g_theBuildingDB->Get(i)->GetUpkeep() > wonderLevel && g_theBuildingDB->Get(i)->GetUpkeep() < lowcost) {
				lowcost = g_theBuildingDB->Get(i)->GetUpkeep();
				low = i;
				// EMOD add new upkeep calucations?

			}
		}
	}
	return low;
}

bool buildingutil_GetDesignatesCapitol(const uint64 built_improvements)
{
	FOREACH_BUILT(GetCapitol) {
		return true;
	}
	return false;
}

void buildingutil_GetDefendersBonus(const uint64 built_improvements, 
     double &bonus)
{
	bonus = 0.0;
	FOREACH_BUILT(HasDefendersPercent) {
		double b;
		rec->GetDefendersPercent(b);
		bonus += b;
	}
}

void buildingutil_GetHappinessIncrement(const uint64 built_improvements, 
     sint32 &bonus, sint32 owner)
{
	double wonderCathedralIncrease = 0.0;
	if(g_player[owner]) {
		wonderCathedralIncrease = double(wonderutil_GetIncreaseCathedrals(
			g_player[owner]->m_builtWonders)) / 100.0;
	}

	bonus = 0;
	FOREACH_BUILT(HasHappyInc) {
		sint32 inc;
		rec->GetHappyInc(inc);
#if 0 // Sitting on the dock of the bay ... 
		if(rec->GetIsReligious())
      // sint32(double(whatever) * 0)?  
			inc -= sint32(double(inc) * 0); 
#endif
		if(rec->GetCathedral()) {
			inc += sint32(double(inc * wonderCathedralIncrease));
		}

		bonus += inc;
	}
}

bool buildingutil_GetDoubleTelevangelism(uint64 built_improvements)
{
	FOREACH_BUILT(GetDoubleTelevangelists) {
		return true;
	}

	return false;
}


bool buildingutil_GetNoUnhappyPeople(const uint64 built_improvements)
{
	FOREACH_BUILT(GetNoUnhappyPeople) {
		return true;
	}

	return false;
}


double buildingutil_GetLowerCrime(const uint64 built_improvements)
{
	double crimeMod = 0.0;
	FOREACH_BUILT(HasLowerCrime) {
		double lc;
		rec->GetLowerCrime(lc);
		crimeMod += lc;
	}
	return crimeMod;
}

double buildingutil_GetPreventConversion(const uint64 built_improvements)
{
	double theoMod = 0.0;
	FOREACH_BUILT(HasPreventConversion) {
		double mod;
		rec->GetPreventConversion(mod);
		theoMod += mod;
	}
	return theoMod;
}

double buildingutil_GetPreventSlavery(const uint64 built_improvements)
{
	double best = 0;
	FOREACH_BUILT(HasPreventSlavery) {
		double cur;
		if(rec->GetPreventSlavery(cur) && cur > best)
			best = cur;
	}
	return best;
}

double buildingutil_GetLowerPeaceMovement(const uint64 built_improvements)
{
	double peaceMod = 0;

	FOREACH_BUILT(HasLowerPeaceMovement) {
		double mod;
		rec->GetLowerPeaceMovement(mod);
		peaceMod += mod;
	}
	return peaceMod;
}

sint32 buildingutil_GetGoldPerCitizen(const uint64 built_improvements)
{
	sint32 goldMod = 0;
	FOREACH_BUILT(HasGoldPerCitizen) {
		sint32 mod;
		rec->GetGoldPerCitizen(mod);
		goldMod += mod;
	}
	return goldMod;
}

sint32 buildingutil_GetGoldPerCity(const uint64 built_improvements) // EMOD
{
	sint32 goldMod = 0;
	FOREACH_BUILT(HasGoldPerCity) {
		sint32 mod;
		rec->GetGoldPerCity(mod);
		goldMod += mod;
	}
	return goldMod;
}

sint32 buildingutil_GetGoldPerUnit(const uint64 built_improvements) // EMOD
{
	sint32 goldMod = 0;
	FOREACH_BUILT(HasGoldPerUnit) {
		sint32 mod;
		rec->GetGoldPerUnit(mod);
		goldMod += mod;
	}
	return goldMod;
}

sint32 buildingutil_GetGoldPerUnitReadiness(const uint64 built_improvements) // EMOD
{
	sint32 goldMod = 0;
	FOREACH_BUILT(HasGoldPerUnitReadiness) {
		sint32 mod;
		rec->GetGoldPerUnitReadiness(mod);
		goldMod += mod;
	}
	return goldMod;
}

sint32 buildingutil_GetGoldPerUnitSupport(const uint64 built_improvements) // EMOD
{
	sint32 goldMod = 0;
	FOREACH_BUILT(HasGoldPerUnitSupport) {
		sint32 mod;
		rec->GetGoldPerUnitSupport(mod);
		goldMod += mod;
	}
	return goldMod;
}

sint32 buildingutil_GetUpkeepPerCity(const uint64 built_improvements) // EMOD
{
	sint32 goldMod = 0;
	FOREACH_BUILT(HasUpkeepPerCity) {
		sint32 mod;
		rec->GetUpkeepPerCity(mod);
		goldMod += mod;
	}
	return goldMod;
}

sint32 buildingutil_GetUpkeepPerUnit(const uint64 built_improvements) // EMOD
{
	sint32 goldMod = 0;
	FOREACH_BUILT(HasUpkeepPerUnit) {
		sint32 mod;
		rec->GetUpkeepPerUnit(mod);
		goldMod += mod;
	}
	return goldMod;
}

sint32 buildingutil_GetUpkeepPerUnitWagesReadiness(const uint64 built_improvements) // EMOD
{
	sint32 goldMod = 0;
	FOREACH_BUILT(HasUpkeepPerUnitWagesReadiness) {
		sint32 mod;
		rec->GetUpkeepPerUnitWagesReadiness(mod);
		goldMod += mod;
	}
	return goldMod;
}

bool buildingutil_GetProtectFromNukes(const uint64 built_improvements)
{
	FOREACH_BUILT(GetProtectFromNukes) {
		return true;
	}
	return false;
}

double buildingutil_GetProtectFromBioAgents(const uint64 built_improvements)
{
	double best = 0;
	FOREACH_BUILT(HasProtectFromBioAgents) {
		double cur;
		if(rec->GetProtectFromBioAgents(cur) && cur > best)
			best = cur;
	}
	return best;
}

double buildingutil_GetProtectFromNanoVirus(const uint64 built_improvements)
{
	double best = 0;
	FOREACH_BUILT(HasProtectFromNanoVirus) {
		double cur;
		if(rec->GetProtectFromNanoVirus(cur) && cur > best)
			best = cur;
	}
	return best;
}

bool buildingutil_GetTelevision(const uint64 built_improvements)
{
	FOREACH_BUILT(GetTelevision) {
		return true;
	}
	return false;
}


bool buildingutil_GetCityWalls(const uint64 built_improvements)
{
	FOREACH_BUILT(GetCityWalls) {
		return true;
	}
	return false;
}

double buildingutil_GetCityWallsDefense(const uint64 built_improvements)
{
	double val=0.0, temp=0.0;
	FOREACH_BUILT(GetCityWalls) 
	{
		if(g_theBuildingDB->Get(i)->GetDefendersPercent(temp))
		{
			val+=temp;
		}
	}
	return val;
}

bool buildingutil_GetAirport(const uint64 built_improvements)
{
	FOREACH_BUILT(GetAirport) {
		return true;
	}
	return false;
}

bool buildingutil_HaveFoodVat(const uint64 built_improvements, 
                                    double &food_to_pollution_coef)
{
	bool atLeastOne = false;
	food_to_pollution_coef = 0;

	FOREACH_BUILT(HasFoodVat) {
		atLeastOne = true;
		double coef;
		rec->GetFoodVat(coef);
		food_to_pollution_coef += coef;
	}
	return atLeastOne;
}

bool buildingutil_NoRushBuyPenalty(const uint64 built_improvements)
{
	FOREACH_BUILT(GetNoRushBuyPenalty) {
		return true;
	}
	return false;
}

bool buildingutil_IsCathedral(const uint64 built_improvements)
{
	FOREACH_BUILT(GetCathedral) {
		return true;
	}
	return false;
}

bool buildingutil_IsBrokerage(const uint64 built_improvements)
{
	FOREACH_BUILT(GetBrokerage) {
		return true;
	}
	return false;
}

bool buildingutil_IsNuclearPlant(const uint64 built_improvements)
{
	FOREACH_BUILT(GetNuclearPlant) {
		return true;
	}
	return false;
}

double buildingutil_GetIncreaseSciencePerPop(const uint64 built_improvements)
{
	double sci = 0;
	FOREACH_BUILT(HasSciencePerPop) {
		double s;
		rec->GetSciencePerPop(s);
		sci += s;
	}
	return sci;
}


bool buildingutil_GetForceField(const uint64 built_improvements)
{
	FOREACH_BUILT(GetForceField) {
		return true;
	}
	return false;
}

bool buildingutil_GetProductionPercent(const uint64 built_improvements,
                                       double &percent)
{
	percent = 0;
	FOREACH_BUILT(HasProductionPercent) {
		double p;
		rec->GetProductionPercent(p);
		percent += p;
	}
	return percent > 0;
}

bool buildingutil_GetFoodPercent(const uint64 built_improvements,
                                 double &percent)
{
	percent = 0;
	FOREACH_BUILT(HasFoodPercent) {
		double p;
		rec->GetFoodPercent(p);
		percent += p;
	}
	return percent > 0;
}

bool buildingutil_GetCommercePercent(const uint64 built_improvements,
                                     double &percent, sint32 owner)
{
	double wonderBrokerageIncrease = 0;
	if(g_player[owner]) {
		wonderBrokerageIncrease = double(wonderutil_GetIncreaseBrokerages(
			g_player[owner]->m_builtWonders)) / 100.0;
	}

	percent = 0;
	FOREACH_BUILT(HasCommercePercent) {
		double p;
		rec->GetCommercePercent(p);

		if(rec->GetBrokerage()) {
			p += double(p * wonderBrokerageIncrease);
		}

		percent += p;
	}
	return percent > 0;
}

//EMOD for Gold treasury interest for Central Banking
bool buildingutil_GetTreasuryInterest(const uint64 built_improvements,
                                      double &percent, sint32 owner)
{

	percent = 0;
	FOREACH_BUILT(HasTreasuryInterest) {
		double p;
		rec->GetTreasuryInterest(p);

		percent += p;
	}
	return percent > 0;
}

bool buildingutil_GetSciencePercent(const uint64 built_improvements,
                                    double &percent)
{
	percent = 0;
	FOREACH_BUILT(HasSciencePercent) {
		double p;
		rec->GetSciencePercent(p);
		percent += p;
	}
	return percent > 0;
}

bool buildingutil_GetRaiseOvercrowdingLevel(const uint64 built_improvements,
                                            sint32 &level)
{
	level = 0;
	FOREACH_BUILT(HasRaiseOvercrowdingLevel) {
		sint32 l;
		rec->GetRaiseOvercrowdingLevel(l);
		level += l;
	}

	return level > 0;
}

bool buildingutil_GetRaiseMaxPopulation(const uint64 built_improvements,
                                        sint32 &level)
{
	level = 0;
	FOREACH_BUILT(HasRaiseMaxPopulation) {
		sint32 l;
		rec->GetRaiseMaxPopulation(l);
		level += l;
	}
	return level > 0;
}

sint32 buildingutil_GetStarvationProtection(const uint64 built_improvements)
{
	sint32 amt = 0;
	FOREACH_BUILT(HasStarvationProtection) {
		sint32 l;
		rec->GetStarvationProtection(l);
		amt += l;
	}
	return amt;
}

double buildingutil_GetOffenseBonusLand(const uint64 built_improvements)
{
	double best = 0;
	FOREACH_BUILT(HasOffenseBonusLand) {
		double cur;
		if(rec->GetOffenseBonusLand(cur) && cur > best)
			best = cur;
	}
	return best;
}

double buildingutil_GetOffenseBonusWater(const uint64 built_improvements)
{
	double best = 0;
	FOREACH_BUILT(HasOffenseBonusWater) {
		double cur;
		if(rec->GetOffenseBonusWater(cur) && cur > best)
			best = cur;
	}
	return best;
}

double buildingutil_GetOffenseBonusAir(const uint64 built_improvements)
{
	double best = 0;
	FOREACH_BUILT(HasOffenseBonusAir) {
		double cur;
		if(rec->GetOffenseBonusAir(cur) && cur > best)
			best = cur;
	}
	return best;
}

bool buildingutil_IsObsolete(sint32 building_type)
{
	const BuildingRecord* rec = g_theBuildingDB->Get(building_type);
	sint32 nObsolete = rec->GetNumObsoleteAdvance();
	if(nObsolete <= 0)
		return false;

	for(sint32 p = 0; p < k_MAX_PLAYERS; p++) {
		if(!g_player[p]) continue;

		for(sint32 o = 0; o < nObsolete; o++) {
			if(g_player[p]->HasAdvance(rec->GetObsoleteAdvanceIndex(o)))
				return true;
		}
	}
	return false;
}

bool buildingutil_GetEmbassiesEverywhereEvenAtWar(const uint64 built_improvements)
{
	FOREACH_BUILT(GetEmbassiesEverywhereEvenAtWar) {
		return true;
	}
	return false;
}

sint32 buildingutil_GetIncreaseHP(const uint64 built_improvements)
{
	sint32 amt = 0;
	FOREACH_BUILT(HasIncreaseHP) {
		sint32 hp;
		rec->GetIncreaseHP(hp);
		amt += hp;
	}
	return amt;
}


	//Bit(Int)   EnergyHunger
	//Bit(Int)   EnergyHungerPerPop
	//Bit(Int)   ProducesEnergy
	//Bit(Int)   ProducesEnergyPerPop
sint32 buildingutil_GetEnergyHunger(const uint64 built_improvements)
{
	sint32 e = 0;
	FOREACH_BUILT(HasEnergyHunger) {
		sint32 mod;
		rec->GetEnergyHunger(mod);
		e += mod;
	}
	return e;
}

sint32 buildingutil_GetEnergyHungerPerPop(const uint64 built_improvements)
{
	sint32 e = 0;
	FOREACH_BUILT(HasEnergyHungerPerPop) {
		sint32 mod;
		rec->GetEnergyHungerPerPop(mod);
		e += mod;
	}
	return e;
}

sint32 buildingutil_GetProducesEnergy(const uint64 built_improvements)
{
	sint32 e = 0;
	FOREACH_BUILT(HasProducesEnergy) {
		sint32 mod;
		rec->GetProducesEnergy(mod);
		e += mod;
	}
	return e;
}

sint32 buildingutil_GetProducesEnergyPerPop(const uint64 built_improvements)
{
	sint32 e = 0;
	FOREACH_BUILT(HasProducesEnergyPerPop) {
		sint32 mod;
		rec->GetProducesEnergyPerPop(mod);
		e += mod;
	}
	return e;
}