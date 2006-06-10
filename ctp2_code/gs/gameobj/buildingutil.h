//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ header
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
// - buildingutil_GetGoldPerCity added to work like GoldPerPop (2-15-2006 E)
// - buildingutil_GetGoldPerUnit added to work like GoldPerPop (2-24-2006 E)
// - buildingutil_GetGoldPerUnitReadiness added to work like GoldPerPop (2-24-2006 E)
// - buildingutil_IsObsolete added so buildings can be obsolete like wonders (4-28-2006 E)
// - buildingutil_GetEmbassiesEverywhereEvenAtWar (E)
// - buildingutil_GetIncreaseHP (E)
//
//----------------------------------------------------------------------------

#ifndef __BUILDING_UTIL_H__
#define __BUILDING_UTIL_H__

void buildingutil_Initialize();
sint32 buildingutil_GetProductionCost(const sint32 building_type);
sint32 buildingutil_GetTotalUpkeep(const uint64 built_improvements,
										   sint32 wonderLevel);
sint32 buildingutil_GetBlgUpkeep(const sint32 building_type);
sint32 buildingutil_GetCheapestBuilding(const uint64 built_improvements, 
												sint32 wonderLevel);
sint32 buildingutil_GetDesignatesCapitol(const uint64 built_improvements);
void buildingutil_GetDefendersBonus(const uint64 built_improvements, 
     double &bonus);

void buildingutil_GetHappinessIncrement(const uint64 built_improvements, 
     sint32 &bonus, sint32 owner);

void buildingutil_GetDoubleTelevangelism(uint64 built_improvements, sint32 &is_dbl_tel);
sint32 buildingutil_GetNoUnhappyPeople(const uint64 built_improvements);
double buildingutil_GetLowerCrime(const uint64 built_improvements);
double buildingutil_GetPreventConversion(const uint64 built_improvements);
double buildingutil_GetPreventSlavery(const uint64 built_improvements);
double buildingutil_GetLowerPeaceMovement(const uint64 built_improvements);
sint32 buildingutil_GetGoldPerCitizen(const uint64 built_improvements);
sint32 buildingutil_GetUpkeepPerCity(const uint64 built_improvements); // EMOD
sint32 buildingutil_GetUpkeepPerUnit(const uint64 built_improvements); // EMOD
sint32 buildingutil_GetUpkeepPerUnitWagesReadiness(const uint64 built_improvements); // EMOD
sint32 buildingutil_GetGoldPerCity(const uint64 built_improvements); // EMOD
sint32 buildingutil_GetGoldPerUnitSupport(const uint64 built_improvements); // EMOD
sint32 buildingutil_GetGoldPerUnit(const uint64 built_improvements); // EMOD
sint32 buildingutil_GetGoldPerUnitReadiness(const uint64 built_improvements); // EMOD
BOOL buildingutil_GetProtectFromNukes(const uint64 built_improvements);
double buildingutil_GetProtectFromBioAgents(const uint64 built_improvements);
double buildingutil_GetProtectFromNanoVirus(const uint64 built_improvements);
BOOL buildingutil_GetTelevision(const uint64 built_improvements);
BOOL buildingutil_GetCityWalls(const uint64 built_improvements);
double buildingutil_GetCityWallsDefense(const uint64 built_improvements);
BOOL buildingutil_GetAirport(const uint64 built_improvements);
BOOL buildingutil_HaveFoodVat(const uint64 built_improvements, 
									  double &food_to_pollution_coef);
BOOL buildingutil_NoRushBuyPenalty(const uint64 built_improvements);
BOOL buildingutil_IsCathedral(const uint64 built_improvements);
BOOL buildingutil_IsBrokerage(const uint64 built_improvements);
BOOL buildingutil_IsNuclearPlant(const uint64 built_improvements);
double buildingutil_GetIncreaseSciencePerPop(const uint64 built_improvements);
BOOL buildingutil_GetForceField(const uint64 built_improvements);
BOOL buildingutil_GetProductionPercent(const uint64 built_improvements,
									   double &percent);
BOOL buildingutil_GetFoodPercent(const uint64 built_improvements,
								 double &percent);
BOOL buildingutil_GetCommercePercent(const uint64 built_improvements,
									 double &percent, sint32 owner);
BOOL buildingutil_GetSciencePercent(const uint64 built_improvements,
									 double &percent);
BOOL buildingutil_GetRaiseOvercrowdingLevel(const uint64 built_improvements,
											sint32 &level);
BOOL buildingutil_GetRaiseMaxPopulation(const uint64 built_improvements,
										sint32 &level);
sint32 buildingutil_GetStarvationProtection(const uint64 built_improvements);

double buildingutil_GetOffenseBonusLand(const uint64 built_improvements);
double buildingutil_GetOffenseBonusWater(const uint64 built_improvements);
double buildingutil_GetOffenseBonusAir(const uint64 built_improvements);

BOOL buildingutil_IsObsolete(sint32 building_type); //EMOD 4-28-2006
BOOL buildingutil_GetEmbassiesEverywhereEvenAtWar(const uint64 built_improvements); //EMOD 4-28-2006
sint32 buildingutil_GetIncreaseHP(const uint64 built_improvements); //EMOD 5-27-2006
BOOL buildingutil_GetTreasuryInterest(const uint64 built_improvements,
									 double &percent, sint32 owner);

#endif
