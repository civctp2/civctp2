//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
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
// - wonderutil_IsObsolete now has two new checks GovernmentType and 
//   ObsoleteGovernmentType switching from or two these obsoletes your wonder (E 5-27-2006)
// - wonderutil_GetGoldPerBuildingAnywhere (E 5-27-2006)
//
//----------------------------------------------------------------------------

#include "c3.h"
#include "c3errors.h"

#include "StrDB.h"
#include "ErrMsg.h"
#include "civarchive.h"
#include "player.h"
#include "moviedb.h"
#include "AgeRecord.h"

#include "WonderTracker.h"

#include "AdvanceRecord.h"
#include "AgeRecord.h"

#include "WonderRecord.h"
#include "wonderutil.h"

#include "GameSettings.h"

#define shiftbit(i) uint64(uint64(0x01) << uint64(i))
#define FOREACH_WNDR(func) \
uint64 forEachBuilt = builtWonders & (shiftbit(g_theWonderDB->NumRecords())-1); \
for(sint32 wndr=0;forEachBuilt>0;wndr++,forEachBuilt>>=1) \
	if((forEachBuilt&1) && \
	g_theWonderDB->Get(wndr)->func() && !wonderutil_IsObsolete(wndr)) 

#define BOOL_WNDR(flag) FOREACH_WNDR(flag) { return true; } return false;
#define INT_WNDR(func, getfunc)\
    sint32 amt = 0;\
    FOREACH_WNDR(func) {\
        sint32 add;\
        g_theWonderDB->Get(wndr)->getfunc(add);\
        amt += add;\
    }\
    return amt;

const WonderRecord *wonderutil_Get(sint32 wonder)
{
	return g_theWonderDB->Get(wonder);
}











bool wonderutil_GetEmbassy(const uint64 builtWonders, const uint64 hisWonders)
{

	
	
	uint64 wonders = hisWonders & (shiftbit(g_theWonderDB->NumRecords())-1);
	int j = 0;
	while (wonders>0)
	{
		if ((wonders&1) && g_theWonderDB->Get(j)->GetCloseEmbassies()) 
			return FALSE;
		wonders>>=1;
		j++;
	}

	BOOL_WNDR(GetEmbassiesEverywhere);
}

bool wonderutil_GetCloseEmbassies(const uint64 builtWonders)
{
	BOOL_WNDR(GetCloseEmbassies);
}

bool wonderutil_GetReformCities(const uint64 builtWonders)
{
	BOOL_WNDR(GetReformCities);
}

bool wonderutil_GetFreeSlaves(const uint64 builtWonders)
{
	BOOL_WNDR(GetFreeSlaves);
}

bool wonderutil_GetGlobalRadar(const uint64 builtWonders)
{
	BOOL_WNDR(GetGlobalRadar);
}

bool wonderutil_GetSpiesEverywhere(const uint64 builtWonders)
{
	BOOL_WNDR(GetSpiesEverywhere);
}

bool wonderutil_GetProtectFromBiologicalWarfare(const uint64 builtWonders)
{
	BOOL_WNDR(GetProtectFromBiologicalWarfare);
}

bool wonderutil_GetParkRangersEnabled(const uint64 builtWonders)
{
	BOOL_WNDR(GetEnableParkRangers);
}

bool wonderutil_GetAllCitizensContent(const uint64 builtWonders)
{
	BOOL_WNDR(GetAllCitizensContent);
}

bool wonderutil_GetNukesEliminated(const uint64 builtWonders)
{
	BOOL_WNDR(GetEliminateNukes);
}

sint32 wonderutil_GetReadinessCostReduction(const uint64 builtWonders)
{
	INT_WNDR(HasReduceReadinessCost, GetReduceReadinessCost);
}

sint32 wonderutil_GetDecreaseCrimePercentage(const uint64 builtWonders)
{
	INT_WNDR(HasDecCrimePercent, GetDecCrimePercent);
}

sint32 wonderutil_GetIncreaseKnowledgePercentage(const uint64 builtWonders)
{
	INT_WNDR(HasIncKnowledgePercent, GetIncKnowledgePercent);
}

sint32 wonderutil_GetDecreaseEmpireSize(const uint64 builtWonders)
{
	INT_WNDR(HasDecEmpireSize, GetDecEmpireSize);
}

sint32 wonderutil_GetIncreaseHappinessEmpire(const uint64 builtWonders)
{
	INT_WNDR(HasIncHappinessEmpire, GetIncHappinessEmpire);
}

sint32 wonderutil_GetIncreaseConvertedCitiesFeePercentage(const uint64 builtWonders)
{
	INT_WNDR(HasIncConvertedCitiesFeePercent, GetIncConvertedCitiesFeePercent);
}

sint32 wonderutil_GetGoldPerWaterTradeRoute(const uint64 builtWonders)
{
	INT_WNDR(HasGoldPerWaterTradeRoute, GetGoldPerWaterTradeRoute);
}

sint32 wonderutil_GetGoldPerTelevision(const uint64 builtWonders)
{
	INT_WNDR(HasGoldPerTelevision, GetGoldPerTelevision);
}

sint32 wonderutil_GetGoldPerInternationalTradeRoute(const uint64 builtWonders)
{
	INT_WNDR(HasGoldPerInternationalTradeRoute, GetGoldPerInternationalTradeRoute);
}

sint32 wonderutil_GetBonusGold(const uint64 builtWonders)
{
	INT_WNDR(HasBonusGold, GetBonusGold);
}

sint32 wonderutil_GetPollutersToParks(const uint64 builtWonders)
{
	INT_WNDR(HasPollutersToParks, GetPollutersToParks);
}

sint32 wonderutil_GetReduceWorldPollution(const uint64 builtWonders)
{
	INT_WNDR(HasReduceWorldPollution, GetReduceWorldPollution);
}

bool wonderutil_GetAllBoatsDeepWater(const uint64 builtWonders)
{
	BOOL_WNDR(GetAllBoatsDeepWater);
}

sint32 wonderutil_GetIncreaseBoatMovement(const uint64 builtWonders)
{
	INT_WNDR(HasIncreaseBoatMovement, GetIncreaseBoatMovement);
}

bool wonderutil_GetFreeTradeRoutes(const uint64 builtWonders)
{
	BOOL_WNDR(GetFreeTradeRoutes);
}

sint32 wonderutil_GetDecreaseMaintenance(const uint64 builtWonders)
{
	INT_WNDR(HasDecreaseMaintenance, GetDecreaseMaintenance);
}

sint32 wonderutil_GetRandomAdvanceChance(const uint64 builtWonders)
{
	INT_WNDR(HasRandomAdvanceChance, GetRandomAdvanceChance);
}

sint32 wonderutil_GetIncreaseHP(const uint64 builtWonders)
{
	INT_WNDR(HasIncreaseHp, GetIncreaseHp);
}

sint32 wonderutil_GetMultiplyTradeRoutes(const uint64 builtWonders)
{
	INT_WNDR(HasMultiplyTradeRoutes, GetMultiplyTradeRoutes);
}

bool wonderutil_GetForcefieldEverywhere(const uint64 builtWonders)
{
	BOOL_WNDR(GetForcefieldEverywhere);
}

bool wonderutil_GetRevoltingCitiesJoinPlayer(const uint64 builtWonders)
{
	BOOL_WNDR(GetRevoltingCitiesJoinPlayer);
}

bool wonderutil_GetNoPollutionUnhappiness(const uint64 builtWonders)
{
	BOOL_WNDR(GetNoPollutionUnhappiness);
}

bool wonderutil_GetEmbassiesEverywhereEvenAtWar(const uint64 builtWonders)
{
	BOOL_WNDR(GetEmbassiesEverywhereEvenAtWar);
}

bool wonderutil_PreventConversion(const uint64 builtWonders)
{
	BOOL_WNDR(GetPreventConversion);
}

bool wonderutil_GetProtectFromBarbarians(uint64 builtWonders)
{
	BOOL_WNDR(GetProtectFromBarbarians);
}

bool wonderutil_GetStartGaiaController(uint64 builtWonders)
{
	BOOL_WNDR(GetStartGaiaController);
}

sint32 wonderutil_GetIncreaseScientists(const uint64 builtWonders)
{
	INT_WNDR(HasIncreaseScientists, GetIncreaseScientists);
}

sint32 wonderutil_GetOtherCivRandomAdvanceChance(const uint64 builtWonders)
{
	INT_WNDR(HasOtherCivRandomAdvanceChance, GetOtherCivRandomAdvanceChance);
}

sint32 wonderutil_GetIncreaseProduction(const uint64 builtWonders)
{
	INT_WNDR(HasIncreaseProduction, GetIncreaseProduction);
}

sint32 wonderutil_GetIncreaseFoodAllCities(const uint64 builtWonders)
{
	INT_WNDR(HasIncreaseFoodAllCities, GetIncreaseFoodAllCities);
}

sint32 wonderutil_GetTemporaryFullHappiness(const uint64 builtWonders)
{
	INT_WNDR(HasTemporaryFullHappiness, GetTemporaryFullHappiness);
}

sint32 wonderutil_GetIncreaseSpecialists(const uint64 builtWonders)
{
	INT_WNDR(HasIncreaseSpecialists, GetIncreaseSpecialists);
}

sint32 wonderutil_GetIncreaseBrokerages(const uint64 builtWonders)
{
	INT_WNDR(HasIncreaseBrokerages, GetIncreaseBrokerages);
}

sint32 wonderutil_GetIncreaseCathedrals(const uint64 builtWonders)
{
	INT_WNDR(HasIncreaseCathedrals, GetIncreaseCathedrals);
}

sint32 wonderutil_GetIncreaseRegard(const uint64 builtWonders)
{
	INT_WNDR(HasIncreaseRegard, GetIncreaseRegard);
}

bool wonderutil_IsAvailable(sint32 wonder, sint32 player)
{
	if(g_theWonderTracker->HasWonderBeenBuilt(wonder)) {
		return false;
	}

	const WonderRecord *rec = g_theWonderDB->Get(wonder);
	
	if(rec->GetEnableAdvanceIndex() >= 0 && 
	   !g_player[player]->HasAdvance(rec->GetEnableAdvanceIndex()))
		return false;

	
	if(wonderutil_IsObsolete(wonder))
		return false;

	
	if(rec->GetStartGaiaController() && !g_theGameSettings->GetAlienEndGame()) {
		return false;
	}

	
	return true;
}














PLAYER_INDEX wonderutil_GetOwner(sint32 wonder)
{
	PLAYER_INDEX who = g_theWonderTracker->WhoOwnsWonder(wonder);
	if(who < 0)
		return PLAYER_INDEX_INVALID;
	return who;
}


void wonderutil_AddBuilt(sint32 wonder)
{
	g_theWonderTracker->AddBuilt(wonder);
}

bool wonderutil_IsBuilt(sint32 wonder)
{
	return g_theWonderTracker->HasWonderBeenBuilt(wonder);
}





bool wonderutil_IsObsolete(sint32 wonder)
{

	sint32 m_owner = wonderutil_GetOwner(wonder);
	const WonderRecord *rec = g_theWonderDB->Get(wonder);
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

	sint32 i;
	// EMOD if you change from the govt that allows for 
	// the wonder to be built you make it obsolete
	for(i = 0; i < rec->GetNumGovernmentType(); i++) {
		if(rec->GetGovernmentTypeIndex(i) != g_player[m_owner]->GetGovernmentType()) {
			return true;
		}
	}

	// EMOD if you switch to this govt it will obsolete 
	// the wonder 9ie communism for religious wonders)
	for(i = 0; i < rec->GetNumObsoleteGovernmentType(); i++) {
		if(rec->GetObsoleteGovernmentTypeIndex(i) != g_player[m_owner]->GetGovernmentType()) {
			return true;
		}
	}

	return false;
}

sint32 wonderutil_GetProductionCost(sint32 wonder)
{
	const WonderRecord *rec = g_theWonderDB->Get(wonder);

	return rec->GetProductionCost();
}

sint32 wonderutil_GetGaiaIndex()
{
	sint32 i;
	for(i = 0; i < g_theWonderDB->NumRecords(); i++) {
		if(g_theWonderDB->Get(i)->GetStartGaiaController())
			return i;
	}
	return i;
}

sint32 wonderutil_GetFobCityIndex()
{
	sint32 i;
	for(i = 0; i < g_theWonderDB->NumRecords(); i++) {
		if(g_theWonderDB->Get(i)->GetCloseEmbassies()) 
			return i;
	}
	return i;
}

//EMOD
sint32 wonderutil_GetGoldPerBuildingAnywhere(const uint64 builtWonders)
{
	INT_WNDR(HasGoldPerBuildingAnywhere, GetGoldPerBuildingAnywhere);
}

sint32 wonderutil_GetEnablesPunativeAirstrikes(const uint64 builtWonders)
{
	BOOL_WNDR(GetEnablesPunativeAirstrikes);
}
	//Bit(Int)   EnergyHunger
	//Bit(Int)   EnergyHungerPerPop
	//Bit(Int)   ProducesEnergy
	//Bit(Int)   ProducesEnergyPerPop
sint32 wonderutil_GetEnergyHunger(const uint64 builtWonders)
{
	INT_WNDR(HasEnergyHunger, GetEnergyHunger);
}

sint32 wonderutil_GetEnergyHungerPerPop(const uint64 builtWonders)
{
	INT_WNDR(HasEnergyHungerPerPop, GetEnergyHungerPerPop);
}

sint32 wonderutil_GetProducesEnergy(const uint64 builtWonders)
{
	INT_WNDR(HasProducesEnergy, GetProducesEnergy);
}

sint32 wonderutil_GetProducesEnergyPerPop(const uint64 builtWonders)
{
	INT_WNDR(HasProducesEnergyPerPop, GetProducesEnergyPerPop);
}