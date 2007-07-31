








#include "c3.h"
#include "c3errors.h"

#include "StrDB.h"
#include "ErrMsg.h"
#include "CivArchive.h"
#include "Player.h"
#include "MovieDB.h"
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

#define BOOL_WNDR(flag) FOREACH_WNDR(flag) { return TRUE; } return FALSE;
#define INT_WNDR(func) \
    sint32 amt = 0;\
    FOREACH_WNDR(func) {\
        sint32 add; \
	    g_theWonderDB->Get(wndr)->func (add);\
        amt += add;\
	}\
    return amt;

const WonderRecord *wonderutil_Get(sint32 wonder)
{
	return g_theWonderDB->Get(wonder);
}











BOOL wonderutil_GetEmbassy(const uint64 builtWonders, const uint64 hisWonders)
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

BOOL wonderutil_GetCloseEmbassies(const uint64 builtWonders)
{
	BOOL_WNDR(GetCloseEmbassies);
}

BOOL wonderutil_GetReformCities(const uint64 builtWonders)
{
	BOOL_WNDR(GetReformCities);
}

BOOL wonderutil_GetFreeSlaves(const uint64 builtWonders)
{
    BOOL_WNDR(GetFreeSlaves);
}

BOOL wonderutil_GetGlobalRadar(const uint64 builtWonders)
{
	BOOL_WNDR(GetGlobalRadar);
}

BOOL wonderutil_GetSpiesEverywhere(const uint64 builtWonders)
{
	BOOL_WNDR(GetSpiesEverywhere);
}

BOOL wonderutil_GetProtectFromBiologicalWarfare(const uint64 builtWonders)
{
    BOOL_WNDR(GetProtectFromBiologicalWarfare);
}

BOOL wonderutil_GetParkRangersEnabled(const uint64 builtWonders)
{
	BOOL_WNDR(GetEnableParkRangers);
}

BOOL wonderutil_GetAllCitizensContent(const uint64 builtWonders)
{
	BOOL_WNDR(GetAllCitizensContent);
}

BOOL wonderutil_GetNukesEliminated(const uint64 builtWonders)
{
	BOOL_WNDR(GetEliminateNukes);
}

sint32 wonderutil_GetReadinessCostReduction(const uint64 builtWonders)
{
	INT_WNDR(GetReduceReadinessCost);
}

sint32 wonderutil_GetDecreaseCrimePercentage(const uint64 builtWonders)
{
	INT_WNDR(GetDecCrimePercent);
}

sint32 wonderutil_GetIncreaseKnowledgePercentage(const uint64 builtWonders)
{
	INT_WNDR(GetIncKnowledgePercent);
}

sint32 wonderutil_GetDecreaseEmpireSize(const uint64 builtWonders)
{
	INT_WNDR(GetDecEmpireSize);
}

sint32 wonderutil_GetIncreaseHappinessEmpire(const uint64 builtWonders)
{
	INT_WNDR(GetIncHappinessEmpire);
}

sint32 wonderutil_GetIncreaseConvertedCitiesFeePercentage(const uint64 builtWonders)
{
	INT_WNDR(GetIncConvertedCitiesFeePercent);
}

sint32 wonderutil_GetGoldPerWaterTradeRoute(const uint64 builtWonders)
{
	INT_WNDR(GetGoldPerWaterTradeRoute);
}

sint32 wonderutil_GetGoldPerTelevision(const uint64 builtWonders)
{
	INT_WNDR(GetGoldPerTelevision);
}

sint32 wonderutil_GetGoldPerInternationalTradeRoute(const uint64 builtWonders)
{
	INT_WNDR(GetGoldPerInternationalTradeRoute);
}

sint32 wonderutil_GetBonusGold(const uint64 builtWonders)
{
	INT_WNDR(GetBonusGold);
}

sint32 wonderutil_GetPollutersToParks(const uint64 builtWonders)
{
	INT_WNDR(GetPollutersToParks);
}

sint32 wonderutil_GetReduceWorldPollution(const uint64 builtWonders)
{
	INT_WNDR(GetReduceWorldPollution);
}

BOOL wonderutil_GetAllBoatsDeepWater(const uint64 builtWonders)
{
	BOOL_WNDR(GetAllBoatsDeepWater);
}

sint32 wonderutil_GetIncreaseBoatMovement(const uint64 builtWonders)
{
	INT_WNDR(GetIncreaseBoatMovement);
}

BOOL wonderutil_GetFreeTradeRoutes(const uint64 builtWonders)
{
	BOOL_WNDR(GetFreeTradeRoutes);
}

sint32 wonderutil_GetDecreaseMaintenance(const uint64 builtWonders)
{
	INT_WNDR(GetDecreaseMaintenance);
}

sint32 wonderutil_GetRandomAdvanceChance(const uint64 builtWonders)
{
	INT_WNDR(GetRandomAdvanceChance);
}

sint32 wonderutil_GetIncreaseHP(const uint64 builtWonders)
{
	INT_WNDR(GetIncreaseHp);
}

sint32 wonderutil_GetMultiplyTradeRoutes(const uint64 builtWonders)
{
	INT_WNDR(GetMultiplyTradeRoutes);
}

BOOL wonderutil_GetForcefieldEverywhere(const uint64 builtWonders)
{
	BOOL_WNDR(GetForcefieldEverywhere);
}

BOOL wonderutil_GetRevoltingCitiesJoinPlayer(const uint64 builtWonders)
{
	BOOL_WNDR(GetRevoltingCitiesJoinPlayer);
}

BOOL wonderutil_GetNoPollutionUnhappiness(const uint64 builtWonders)
{
	BOOL_WNDR(GetNoPollutionUnhappiness);
}

BOOL wonderutil_GetEmbassiesEverywhereEvenAtWar(const uint64 builtWonders)
{
	BOOL_WNDR(GetEmbassiesEverywhereEvenAtWar);
}

BOOL wonderutil_PreventConversion(const uint64 builtWonders)
{
	BOOL_WNDR(GetPreventConversion);
}

BOOL wonderutil_GetProtectFromBarbarians(uint64 builtWonders)
{
	BOOL_WNDR(GetProtectFromBarbarians);
}

BOOL wonderutil_GetStartGaiaController(uint64 builtWonders)
{
	BOOL_WNDR(GetStartGaiaController);
}

sint32 wonderutil_GetIncreaseScientists(const uint64 builtWonders)
{
	INT_WNDR(GetIncreaseScientists);
}

sint32 wonderutil_GetOtherCivRandomAdvanceChance(const uint64 builtWonders)
{
	INT_WNDR(GetOtherCivRandomAdvanceChance);
}

sint32 wonderutil_GetIncreaseProduction(const uint64 builtWonders)
{
	INT_WNDR(GetIncreaseProduction);
}

sint32 wonderutil_GetIncreaseFoodAllCities(const uint64 builtWonders)
{
	INT_WNDR(GetIncreaseFoodAllCities);
}

sint32 wonderutil_GetTemporaryFullHappiness(const uint64 builtWonders)
{
	INT_WNDR(GetTemporaryFullHappiness);
}

sint32 wonderutil_GetIncreaseSpecialists(const uint64 builtWonders)
{
	INT_WNDR(GetIncreaseSpecialists);
}

sint32 wonderutil_GetIncreaseBrokerages(const uint64 builtWonders)
{
	INT_WNDR(GetIncreaseBrokerages);
}

sint32 wonderutil_GetIncreaseCathedrals(const uint64 builtWonders)
{
	INT_WNDR(GetIncreaseCathedrals);
}

sint32 wonderutil_GetIncreaseRegard(const uint64 builtWonders)
{
	INT_WNDR(GetIncreaseRegard);
}

BOOL wonderutil_IsAvailable(sint32 wonder, sint32 player)
{	
	if(g_theWonderTracker->HasWonderBeenBuilt(wonder)) {
		return FALSE;
	}

	const WonderRecord *rec = g_theWonderDB->Get(wonder);
	
	if(rec->GetEnableAdvanceIndex() >= 0 && 
	   !g_player[player]->HasAdvance(rec->GetEnableAdvanceIndex()))
		return FALSE;

	
	if(wonderutil_IsObsolete(wonder))
		return FALSE;

	
	if(rec->GetStartGaiaController() && !g_theGameSettings->GetAlienEndGame()) {
		return FALSE;
	}

	
	return TRUE;
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

BOOL wonderutil_IsBuilt(sint32 wonder)
{
	return g_theWonderTracker->HasWonderBeenBuilt(wonder);
}





BOOL wonderutil_IsObsolete(sint32 wonder)
{
	const WonderRecord *rec = g_theWonderDB->Get(wonder);
	sint32 nObsolete = rec->GetNumObsoleteAdvance();
	if(nObsolete <= 0)
		return FALSE;

	for(sint32 p = 0; p < k_MAX_PLAYERS; p++) {
		if(!g_player[p]) continue;

		for(sint32 o = 0; o < nObsolete; o++) {
			if(g_player[p]->HasAdvance(rec->GetObsoleteAdvanceIndex(o)))
				return TRUE;
		}
	}
	return FALSE;
}

sint32 wonderutil_GetProductionCost(sint32 wonder)
{
    const WonderRecord *rec = g_theWonderDB->Get(wonder);

    return rec->GetProductionCost();
}

sint32 wonderutil_GetGaiaIndex()
{
	for(sint32 i = 0; i < g_theWonderDB->NumRecords(); i++) {
		if(g_theWonderDB->Get(i)->GetStartGaiaController())
			return i;
	}
	return i;
}

sint32 wonderutil_GetFobCityIndex()
{
	for(sint32 i = 0; i < g_theWonderDB->NumRecords(); i++) {
		if(g_theWonderDB->Get(i)->GetCloseEmbassies()) 
			return i;
	}
	return i;
}

