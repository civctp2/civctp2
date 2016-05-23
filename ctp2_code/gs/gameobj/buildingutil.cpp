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
// - added buildingutil_DesigatesOnePerCiv (E 6.4.2007)
// - added buildingutil_IsReligious (E 6.4.2007)
// - added buildingutil_HasReligionIcon (E 6.4.2007)
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

#define FOREACH_BUILT(recordFunc, owner) \
const BuildingRecord *rec; \
sint32 i; \
uint64 cur_built; \
for (i=0,cur_built=built_improvements;cur_built!=0;cur_built>>=1,i++) \
	if ((cur_built&1) && \
		(rec = buildingutil_Get(i, owner))->recordFunc())


void buildingutil_Initialize()
{
}

sint32 buildingutil_GetProductionCost(const sint32 building_type, const sint32 owner)
{
	const BuildingRecord * rec = buildingutil_Get(building_type, owner);
	Assert(rec);

	return rec ? rec->GetProductionCost() : 0;
}

sint32 buildingutil_GetTotalUpkeep(const uint64 built_improvements,
										   sint32 wonderLevel, const sint32 owner)
{
	//sint32 owner = 0;
	sint32 upkeep = 0;
	for(sint32 i = 0; i < g_theBuildingDB->NumRecords(); i++)
	{
		if(built_improvements & shiftbit(i))
		{
			if(buildingutil_Get(i, owner)->GetUpkeep() > wonderLevel)
			{
				upkeep += buildingutil_Get(i, owner)->GetUpkeep();

				// EMOD added new Upkeep calculations (3-13-2006) restored 12-JULY-2006
				// This doesn't compile E and just adding some parentheses doesn't work either
				// so E do your work. ;)
				// And make the code more readable.
		//		upkeep += buildingutil_Get(i, owner)->GetUpkeepPerUnitWagesReadiness * g_player[owner]->GetNumUnits() * g_player[owner]->m_readiness->GetSupportModifier(g_player[owner]->GetGovernmentType()) * g_player[owner]->GetWagesPerPerson();
		//		upkeep += buildingutil_Get(i, owner)->GetUpkeepPerCity * g_player[owner]->GetNumCities() * g_theGovernmentDB->Get(g_player[owner]->GetGovernmentType())->GetTooManyCitiesThreshold();
		//		upkeep += buildingutil_Get(i, owner)->GetUpkeepPerCitySq * g_player[owner]->GetNumCities() *  g_player[owner]->GetNumCities();
		//		upkeep += buildingutil_Get(i, owner)->GetUpkeepPerUnit * g_player[owner]->GetNumUnits();
		//		upkeep += buildingutil_Get(i, owner)->GetUpkeepPerUnitSupport * g_player[owner]->m_readiness->TotalUnitGoldSupport() * g_player[owner]->GetWagesPerPerson() * g_player[owner]->m_readiness->GetSupportModifier(g_player[owner]->GetGovernmentType());
				// End EMOD
			}
		}
	}
	return upkeep;
}

sint32 buildingutil_GetBlgUpkeep(const sint32 building_type, const sint32 owner)
{
	return buildingutil_Get(building_type, owner)->GetUpkeep();
}

sint32 buildingutil_GetCheapestBuilding(const uint64 built_improvements,
												sint32 wonderLevel, const sint32 owner)
{
	sint32 lowcost = 0x7fffffff;
	sint32 low = -1;

	for(sint32 i = 0; i < g_theBuildingDB->NumRecords(); i++)
	{
		if(built_improvements & shiftbit(i))
		{
			if(buildingutil_Get(i, owner)->GetUpkeep() > wonderLevel && buildingutil_Get(i, owner)->GetUpkeep() < lowcost)
			{
				lowcost = buildingutil_Get(i, owner)->GetUpkeep();
				low = i;
				// EMOD add new upkeep calucations?
			}
		}
	}

	return low;
}

bool buildingutil_GetDesignatesCapitol(const uint64 built_improvements, const sint32 owner)
{
	FOREACH_BUILT(GetCapitol, owner)
	{
		return true;
	}
	return false;
}

void buildingutil_GetDefendersBonus(const uint64 built_improvements,
     double &bonus, const sint32 owner)
{
	bonus = 0.0;
	FOREACH_BUILT(HasDefendersPercent, owner)
	{
		double b;
		rec->GetDefendersPercent(b);
		bonus += b;
	}
}

void buildingutil_GetHappinessIncrement(const uint64 built_improvements,
     sint32 &bonus, const sint32 owner)
{
	double wonderCathedralIncrease = 0.0;
	if(g_player[owner])
	{
		wonderCathedralIncrease = double(wonderutil_GetIncreaseCathedrals(
			g_player[owner]->m_builtWonders)) / 100.0;
	}

	bonus = 0;
	FOREACH_BUILT(HasHappyInc, owner)
	{
		sint32 inc;
		rec->GetHappyInc(inc);
#if 0 // Sitting on the dock of the bay ...
		if(rec->GetIsReligious())
      // sint32(double(whatever) * 0)?
			inc -= sint32(double(inc) * 0);
#endif
		if(rec->GetCathedral())
		{
			inc += sint32(double(inc * wonderCathedralIncrease));
		}

		bonus += inc;
	}
}

bool buildingutil_GetDoubleTelevangelism(uint64 built_improvements, const sint32 owner)
{
	FOREACH_BUILT(GetDoubleTelevangelists, owner)
	{
		return true;
	}

	return false;
}

bool buildingutil_GetNoUnhappyPeople(const uint64 built_improvements, const sint32 owner)
{
	FOREACH_BUILT(GetNoUnhappyPeople, owner)
	{
		return true;
	}

	return false;
}

double buildingutil_GetLowerCrime(const uint64 built_improvements, const sint32 owner)
{
	double crimeMod = 0.0;
	FOREACH_BUILT(HasLowerCrime, owner)
	{
		double lc;
		rec->GetLowerCrime(lc);
		crimeMod += lc;
	}

	return crimeMod;
}

double buildingutil_GetPreventConversion(const uint64 built_improvements, const sint32 owner)
{
	double theoMod = 0.0;
	FOREACH_BUILT(HasPreventConversion, owner)
	{
		double mod;
		rec->GetPreventConversion(mod);
		theoMod += mod;
	}

	return theoMod;
}

double buildingutil_GetPreventSlavery(const uint64 built_improvements, const sint32 owner)
{
	double best = 0;
	FOREACH_BUILT(HasPreventSlavery, owner)
	{
		double cur;
		if(rec->GetPreventSlavery(cur) && cur > best)
			best = cur;
	}

	return best;
}

double buildingutil_GetLowerPeaceMovement(const uint64 built_improvements, const sint32 owner)
{
	double peaceMod = 0;

	FOREACH_BUILT(HasLowerPeaceMovement, owner)
	{
		double mod;
		rec->GetLowerPeaceMovement(mod);
		peaceMod += mod;
	}

	return peaceMod;
}

sint32 buildingutil_GetGoldPerCitizen(const uint64 built_improvements, const sint32 owner)
{
	sint32 goldMod = 0;
	FOREACH_BUILT(HasGoldPerCitizen, owner)
	{
		sint32 mod;
		rec->GetGoldPerCitizen(mod);
		goldMod += mod;
	}

	return goldMod;
}

sint32 buildingutil_GetGoldPerCity(const uint64 built_improvements, const sint32 owner) // EMOD
{
	sint32 goldMod = 0;
	FOREACH_BUILT(HasGoldPerCity, owner)
	{
		sint32 mod;
		rec->GetGoldPerCity(mod);
		goldMod += mod;
	}

	return goldMod;
}

sint32 buildingutil_GetGoldPerUnit(const uint64 built_improvements, const sint32 owner) // EMOD
{
	sint32 goldMod = 0;
	FOREACH_BUILT(HasGoldPerUnit, owner)
	{
		sint32 mod;
		rec->GetGoldPerUnit(mod);
		goldMod += mod;
	}

	return goldMod;
}

sint32 buildingutil_GetGoldPerUnitReadiness(const uint64 built_improvements, const sint32 owner) // EMOD
{
	sint32 goldMod = 0;
	FOREACH_BUILT(HasGoldPerUnitReadiness, owner)
	{
		sint32 mod;
		rec->GetGoldPerUnitReadiness(mod);
		goldMod += mod;
	}

	return goldMod;
}

sint32 buildingutil_GetGoldPerUnitSupport(const uint64 built_improvements, const sint32 owner) // EMOD
{
	sint32 goldMod = 0;
	FOREACH_BUILT(HasGoldPerUnitSupport, owner)
	{
		sint32 mod;
		rec->GetGoldPerUnitSupport(mod);
		goldMod += mod;
	}

	return goldMod;
}

sint32 buildingutil_GetUpkeepPerCity(const uint64 built_improvements, const sint32 owner) // EMOD
{
	sint32 goldMod = 0;
	FOREACH_BUILT(HasUpkeepPerCity, owner)
	{
		sint32 mod;
		rec->GetUpkeepPerCity(mod);
		goldMod += mod;
	}

	return goldMod;
}

sint32 buildingutil_GetUpkeepPerUnit(const uint64 built_improvements, const sint32 owner) // EMOD
{
	sint32 goldMod = 0;
	FOREACH_BUILT(HasUpkeepPerUnit, owner)
	{
		sint32 mod;
		rec->GetUpkeepPerUnit(mod);
		goldMod += mod;
	}

	return goldMod;
}

sint32 buildingutil_GetUpkeepPerUnitWagesReadiness(const uint64 built_improvements, const sint32 owner) // EMOD
{
	sint32 goldMod = 0;
	FOREACH_BUILT(HasUpkeepPerUnitWagesReadiness, owner)
	{
		sint32 mod;
		rec->GetUpkeepPerUnitWagesReadiness(mod);
		goldMod += mod;
	}

	return goldMod;
}

bool buildingutil_GetProtectFromNukes(const uint64 built_improvements, const sint32 owner)
{
	FOREACH_BUILT(GetProtectFromNukes, owner)
	{
		return true;
	}

	return false;
}

double buildingutil_GetProtectFromBioAgents(const uint64 built_improvements, const sint32 owner)
{
	double best = 0;
	FOREACH_BUILT(HasProtectFromBioAgents, owner)
	{
		double cur;
		if(rec->GetProtectFromBioAgents(cur) && cur > best)
			best = cur;
	}

	return best;
}

double buildingutil_GetProtectFromNanoVirus(const uint64 built_improvements, const sint32 owner)
{
	double best = 0;
	FOREACH_BUILT(HasProtectFromNanoVirus, owner)
	{
		double cur;
		if(rec->GetProtectFromNanoVirus(cur) && cur > best)
			best = cur;
	}

	return best;
}

bool buildingutil_GetTelevision(const uint64 built_improvements, const sint32 owner)
{
	FOREACH_BUILT(GetTelevision, owner)
	{
		return true;
	}

	return false;
}

bool buildingutil_GetCityWalls(const uint64 built_improvements, const sint32 owner)
{
	FOREACH_BUILT(GetCityWalls, owner)
	{
		return true;
	}

	return false;
}

double buildingutil_GetCityWallsDefense(const uint64 built_improvements, const sint32 owner)
{
	double val=0.0, temp=0.0;
	FOREACH_BUILT(GetCityWalls, owner)
	{
		if(buildingutil_Get(i, owner)->GetDefendersPercent(temp))
		{
			val+=temp;
		}
	}

	return val;
}

bool buildingutil_GetAirport(const uint64 built_improvements, const sint32 owner)
{
	FOREACH_BUILT(GetAirport, owner)
	{
		return true;
	}

	return false;
}

bool buildingutil_HaveFoodVat(const uint64 built_improvements,
                                    double &food_to_pollution_coef, const sint32 owner)
{
	bool atLeastOne = false;
	food_to_pollution_coef = 0;

	FOREACH_BUILT(HasFoodVat, owner)
	{
		atLeastOne = true;
		double coef;
		rec->GetFoodVat(coef);
		food_to_pollution_coef += coef;
	}

	return atLeastOne;
}

bool buildingutil_NoRushBuyPenalty(const uint64 built_improvements, const sint32 owner)
{
	FOREACH_BUILT(GetNoRushBuyPenalty, owner)
	{
		return true;
	}
	return false;
}

bool buildingutil_IsCathedral(const uint64 built_improvements, const sint32 owner)
{
	FOREACH_BUILT(GetCathedral, owner)
	{
		return true;
	}

	return false;
}

bool buildingutil_IsBrokerage(const uint64 built_improvements, const sint32 owner)
{
	FOREACH_BUILT(GetBrokerage, owner)
	{
		return true;
	}

	return false;
}

bool buildingutil_IsNuclearPlant(const uint64 built_improvements, const sint32 owner)
{
	FOREACH_BUILT(GetNuclearPlant, owner)
	{
		return true;
	}

	return false;
}

double buildingutil_GetIncreaseSciencePerPop(const uint64 built_improvements, const sint32 owner)
{
	double sci = 0;
	FOREACH_BUILT(HasSciencePerPop, owner)
	{
		double s;
		rec->GetSciencePerPop(s);
		sci += s;
	}

	return sci;
}

bool buildingutil_GetForceField(const uint64 built_improvements, const sint32 owner)
{
	FOREACH_BUILT(GetForceField, owner)
	{
		return true;
	}

	return false;
}

bool buildingutil_GetProductionPercent(const uint64 built_improvements,
                                       double &percent, const sint32 owner)
{
	percent = 0;
	FOREACH_BUILT(HasProductionPercent, owner)
	{
		double p;
		rec->GetProductionPercent(p);
		percent += p;
	}
	return percent > 0;
}

bool buildingutil_GetFoodPercent(const uint64 built_improvements,
                                 double &percent, const sint32 owner)
{
	percent = 0.0;
	FOREACH_BUILT(HasFoodPercent, owner)
	{
		double p;
		rec->GetFoodPercent(p);
		percent += p;
	}
	return percent > 0.0;
}

bool buildingutil_GetCommercePercent(const uint64 built_improvements,
                                     double &percent, const sint32 owner)
{
	double wonderBrokerageIncrease = 0;
	if(g_player[owner])
	{
		wonderBrokerageIncrease = double(wonderutil_GetIncreaseBrokerages(
			g_player[owner]->m_builtWonders)) / 100.0;
	}

	percent = 0;
	FOREACH_BUILT(HasCommercePercent, owner)
	{
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
	FOREACH_BUILT(HasTreasuryInterest, owner)
	{
		double p;
		rec->GetTreasuryInterest(p);

		percent += p;
	}

	return percent > 0;
}

bool buildingutil_GetSciencePercent(const uint64 built_improvements,
                                    double &percent, const sint32 owner)
{
	percent = 0;
	FOREACH_BUILT(HasSciencePercent, owner)
	{
		double p;
		rec->GetSciencePercent(p);
		percent += p;
	}

	return percent > 0;
}

bool buildingutil_GetRaiseOvercrowdingLevel(const uint64 built_improvements,
                                            sint32 &level, const sint32 owner)
{
	level = 0;
	FOREACH_BUILT(HasRaiseOvercrowdingLevel, owner)
	{
		sint32 l;
		rec->GetRaiseOvercrowdingLevel(l);
		level += l;
	}

	return level > 0;
}

bool buildingutil_GetRaiseMaxPopulation(const uint64 built_improvements,
                                        sint32 &level, const sint32 owner)
{
	level = 0;
	FOREACH_BUILT(HasRaiseMaxPopulation, owner)
	{
		sint32 l;
		rec->GetRaiseMaxPopulation(l);
		level += l;
	}

	return level > 0;
}

sint32 buildingutil_GetStarvationProtection(const uint64 built_improvements, const sint32 owner)
{
	sint32 amt = 0;
	FOREACH_BUILT(HasStarvationProtection, owner)
	{
		sint32 l;
		rec->GetStarvationProtection(l);
		amt += l;
	}

	return amt;
}

double buildingutil_GetOffenseBonusLand(const uint64 built_improvements, const sint32 owner)
{
	double best = 0;
	FOREACH_BUILT(HasOffenseBonusLand, owner)
	{
		double cur;
		if(rec->GetOffenseBonusLand(cur) && cur > best)
			best = cur;
	}

	return best;
}

double buildingutil_GetOffenseBonusWater(const uint64 built_improvements, const sint32 owner)
{
	double best = 0;
	FOREACH_BUILT(HasOffenseBonusWater, owner)
	{
		double cur;
		if(rec->GetOffenseBonusWater(cur) && cur > best)
			best = cur;
	}

	return best;
}

double buildingutil_GetOffenseBonusAir(const uint64 built_improvements, const sint32 owner)
{
	double best = 0;
	FOREACH_BUILT(HasOffenseBonusAir, owner)
	{
		double cur;
		if(rec->GetOffenseBonusAir(cur) && cur > best)
			best = cur;
	}

	return best;
}

bool buildingutil_IsObsolete(sint32 building_type, const sint32 owner)
{
	const BuildingRecord* rec = buildingutil_Get(building_type, owner);
	sint32 nObsolete = rec->GetNumObsoleteAdvance();
	if(nObsolete <= 0)
		return false;

	for(sint32 p = 0; p < k_MAX_PLAYERS; p++)
	{
		if(!g_player[p]) continue;

		for(sint32 o = 0; o < nObsolete; o++)
		{
			if(g_player[p]->HasAdvance(rec->GetObsoleteAdvanceIndex(o)))
				return true;
		}
	}

	return false;
}

bool buildingutil_GetEmbassiesEverywhereEvenAtWar(const uint64 built_improvements, const sint32 owner)
{
	FOREACH_BUILT(GetEmbassiesEverywhereEvenAtWar, owner)
	{
		return true;
	}

	return false;
}

sint32 buildingutil_GetIncreaseHP(const uint64 built_improvements, const sint32 owner)
{
	sint32 amt = 0;
	FOREACH_BUILT(HasIncreaseHP, owner)
	{
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
sint32 buildingutil_GetEnergyHunger(const uint64 built_improvements, const sint32 owner)
{
	sint32 e = 0;
	FOREACH_BUILT(HasEnergyHunger, owner)
	{
		sint32 mod;
		rec->GetEnergyHunger(mod);
		e += mod;
	}

	return e;
}

sint32 buildingutil_GetEnergyHungerPerPop(const uint64 built_improvements, const sint32 owner)
{
	sint32 e = 0;
	FOREACH_BUILT(HasEnergyHungerPerPop, owner)
	{
		sint32 mod;
		rec->GetEnergyHungerPerPop(mod);
		e += mod;
	}

	return e;
}

sint32 buildingutil_GetProducesEnergy(const uint64 built_improvements, const sint32 owner)
{
	sint32 e = 0;
	FOREACH_BUILT(HasProducesEnergy, owner)
	{
		sint32 mod;
		rec->GetProducesEnergy(mod);
		e += mod;
	}

	return e;
}

sint32 buildingutil_GetProducesEnergyPerPop(const uint64 built_improvements, const sint32 owner)
{
	sint32 e = 0;
	FOREACH_BUILT(HasProducesEnergyPerPop, owner)
	{
		sint32 mod;
		rec->GetProducesEnergyPerPop(mod);
		e += mod;
	}

	return e;
}

bool buildingutil_GetDesignatesOnePerCiv(const uint64 built_improvements, const sint32 owner)
{
	FOREACH_BUILT(GetOnePerCiv, owner)
	{
		return true;
	}

	return false;
}
bool buildingutil_GetHasReligionIcon(const uint64 built_improvements, const sint32 owner)
{
	FOREACH_BUILT(HasIsReligionIcon, owner)
	{
		return true;
	}

	return false;
}
bool buildingutil_GetIsReligious(const uint64 built_improvements, const sint32 owner)
{
	FOREACH_BUILT(GetIsReligious, owner)
	{
		return true;
	}

	return false;
}
bool buildingutil_GetShowCityIconTop(const uint64 built_improvements, const sint32 owner)
{
	FOREACH_BUILT(HasShowCityIconTop, owner)
	{
		return true;
	}

	return false;
}

const BuildingRecord * buildingutil_Get(const sint32 type, const sint32 playerId)
{
	if(Player * player = g_player[playerId])
	{
		return g_theBuildingDB->Get(type, player->GetGovernmentType());
	}
	else
	{
		return g_theBuildingDB->Get(type);
	}
}
