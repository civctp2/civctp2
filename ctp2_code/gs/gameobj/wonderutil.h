//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ header
// Description  : Wonderutil game object
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
// - wonderutil_GetGoldPerBuildingAnywhere (E 5-27-2006)
//
//----------------------------------------------------------------------------

#ifndef WONDER_UTIL_H__
#define WONDER_UTIL_H__

class WonderRecord;

const WonderRecord *wonderutil_Get(sint32 wonder);

bool wonderutil_GetEmbassy(const uint64 builtWonders, const uint64 hisWonders);
bool wonderutil_GetCloseEmbassies(const uint64 builtWonders);
bool wonderutil_GetReformCities(const uint64 builtWonders);
bool wonderutil_GetFreeSlaves(const uint64 builtWonders);
bool wonderutil_GetGlobalRadar(const uint64 builtWonders);
bool wonderutil_GetSpiesEverywhere(const uint64 builtWonders);
bool wonderutil_GetFreeSpaceTransport(const uint64 builtWonders);
bool wonderutil_GetProtectFromBiologicalWarfare(const uint64 builtWonders);
bool wonderutil_GetPopulationMonitorEverywhere(const uint64 builtWonders);
bool wonderutil_GetParkRangersEnabled(const uint64 builtWonders);
bool wonderutil_GetAllCitizensContent(const uint64 builtWonders);
bool wonderutil_GetNukesEliminated(const uint64 builtWonders);
sint32 wonderutil_GetReadinessCostReduction(const uint64 builtWonders);
sint32 wonderutil_GetIncreaseGoldPercentage(const uint64 builtWonders);
sint32 wonderutil_GetDecreaseCrimePercentage(const uint64 builtWonders);
sint32 wonderutil_GetIncreaseKnowledgePercentage(const uint64 builtWonders);
sint32 wonderutil_GetDecreaseEmpireSize(const uint64 builtWonders);
sint32 wonderutil_GetIncreaseHappinessEmpire(const uint64 builtWonders);
sint32 wonderutil_GetIncreaseConvertedCitiesFeePercentage(const uint64 builtWonders);
sint32 wonderutil_GetGoldPerWaterTradeRoute(const uint64 builtWonders);
sint32 wonderutil_GetGoldPerTelevision(const uint64 builtWonders);
sint32 wonderutil_GetIncreaseHappiness(const uint64 builtWonders);
sint32 wonderutil_GetMakeGeneticsCheaper(const uint64 builtWonders);
sint32 wonderutil_GetGoldPerInternationalTradeRoute(const uint64 builtWonders);
sint32 wonderutil_GetBonusGold(const uint64 builtWonders);
sint32 wonderutil_GetPollutersToParks(const uint64 builtWonders);
sint32 wonderutil_GetReduceWorldPollution(const uint64 builtWonders);
sint32 wonderutil_GetPollutionBonus(const uint64 builtWonders);

sint32 wonderutil_GetPollutionCleaner(const uint64 builtWonders);

bool wonderutil_WormholeDetector(const uint64 builtWonders);
sint32 wonderutil_GetOvercrowdingReduction(const uint64 builtWonders);
bool wonderutil_GetAllBoatsDeepWater(const uint64 builtWonders);
sint32 wonderutil_GetIncreaseBoatMovement(const uint64 builtWonders);
bool wonderutil_GetFreeTradeRoutes(const uint64 builtWonders);
sint32 wonderutil_GetDecreaseTemples(const uint64 builtWonders);
sint32 wonderutil_GetDecreaseMaintenance(const uint64 builtWonders);
sint32 wonderutil_GetRandomAdvanceChance(const uint64 builtWonders);
sint32 wonderutil_GetIncreaseHP(const uint64 builtWonders);
sint32 wonderutil_GetMultiplyTradeRoutes(const uint64 builtWonders);
bool wonderutil_GetForcefieldEverywhere(const uint64 builtWonders);
bool wonderutil_GetRevoltingCitiesJoinPlayer(const uint64 builtWonders);
bool wonderutil_GetNoPollutionUnhappiness(const uint64 builtWonders);
bool wonderutil_GetEmbassiesEverywhereEvenAtWar(const uint64 builtWonders);
bool wonderutil_PreventConversion(const uint64 builtWonders);
bool wonderutil_GetProtectFromBarbarians(uint64 builtWonders);
bool wonderutil_GetStartGaiaController(uint64 builtWonders);
sint32 wonderutil_GetIncreaseScientists(const uint64 builtWonders);
sint32 wonderutil_GetOtherCivRandomAdvanceChance(const uint64 builtWonders);
sint32 wonderutil_GetIncreaseProduction(const uint64 builtWonders);
sint32 wonderutil_GetIncreaseFoodAllCities(const uint64 builtWonders);
double wonderutil_GetBreakDownChance(const uint64 builtWonders, sint32 &percentCities);
sint32 wonderutil_GetTemporaryFullHappiness(const uint64 builtWonders);
sint32 wonderutil_GetIncreaseSpecialists(const uint64 builtWonders);
sint32 wonderutil_GetIncreaseBrokerages(const uint64 builtWonders);
sint32 wonderutil_GetIncreaseCathedrals(const uint64 builtWonders);
sint32 wonderutil_GetIncreaseRegard(const uint64 builtWonders);
bool wonderutil_IsAvailable(sint32 wonder, sint32 player);
sint32 wonderutil_GetOwner(sint32 wonder);
void wonderutil_AddBuilt(sint32 wonder);
bool wonderutil_IsBuilt(sint32 wonder);
bool wonderutil_IsObsolete(sint32 wonder);
sint32 wonderutil_GetProductionCost(sint32 wonder);
sint32 wonderutil_GetGaiaIndex();
sint32 wonderutil_GetFobCityIndex();
sint32 wonderutil_GetGoldPerBuildingAnywhere(const uint64 builtWonders); //EMOD
sint32 wonderutil_GetEnablesPunativeAirstrikes(const uint64 builtWonders); //EMOD

#endif
