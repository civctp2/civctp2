//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : Logging
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
//----------------------------------------------------------------------------
//
// Modifications from the original Activision code:
//
// - Use the same science percentage everywhere.
//
//----------------------------------------------------------------------------

#include "c3.h"
#include "GSLogs.h"
#include "player.h"
#include "StrDB.h"
#include "citydata.h"
#include "UnitDynArr.h"
#include "Unit.h"
#include "UnitRecord.h"
#include "AdvanceRecord.h"
#include "Advances.h"
#include "UnitData.h"
#include "buildingutil.h"
#include "wonderutil.h"
#include "Sci.h"
#include "TaxRate.h"
#include "PlayHap.h"
#include "profileDB.h"
#include "c3math.h"		// AsPercentage

static bool s_initialized		= false;
static bool s_dip_initialized	= false;
static int	s_populationHack[k_MAX_PLAYERS];

void gslog_print(char *fmt, ...)
{
#ifndef _BFR_
	if(!g_theProfileDB->GetEnableLogs())
		return;

    FILE * f = fopen("logs" FILE_SEP "gslog.txt", (s_initialized) ? "a" : "w");
	if (!s_initialized)
	{
		std::fill(s_populationHack, s_populationHack + k_MAX_PLAYERS, 0);
		s_initialized = true;
	}

	char buf[k_MAX_NAME_LEN];
	va_list vl;
	va_start(vl, fmt);
	vsprintf(buf, fmt, vl);
	va_end(vl);

	if(f) {
		fputs(buf, f);
		fclose(f);
	}

	DPRINTF(k_DBG_GAMESTATE, ("%s", buf));
#endif
}

void gslog_dipprint(const char *fmt, ...)
{
#ifndef _BFR_
	if (!g_theProfileDB->GetEnableLogs())
		return;

	FILE * f = fopen("logs" FILE_SEP "diplog.txt", (s_dip_initialized) ? "a" : "w");
	s_dip_initialized = true;

	char buf[k_MAX_NAME_LEN];
	va_list vl;
	va_start(vl, fmt);
	vsprintf(buf, fmt, vl);
	va_end(vl);

	if(f) {
		fputs(buf, f);
		fclose(f);
	}
#endif
}

void gslog_LogPlayerStats(sint32 player)
{
#ifndef _BFR_
	if(!g_theProfileDB->GetEnableLogs())
		return;

	Player *pl = g_player[player];

	if (!pl) return;


	gslog_print("[Player %d] [Turn %d]\n", player, pl->m_current_round);


	double totalFood = 0, totalFoodCrime = 0, totalFoodConsumed = 0;

	UnitDynamicArray *cityList = pl->GetAllCitiesList();
	sint32 cityIndex;


	for (cityIndex = 0; cityIndex < cityList->Num(); cityIndex++)
	{
		CityData * cityData = (*cityList)[cityIndex].GetData()->GetCityData();

		double foodCrime = cityData->GetFoodCrime();

		totalFood           += cityData->GetGrossCityFood();
		totalFoodCrime      += foodCrime;
		totalFoodConsumed   += cityData->GetConsumedFood();
	}

	sint32 percentFoodCrime = static_cast<sint32>(totalFood ?
		((totalFoodCrime * 100.0) / totalFood) : 0.0);
	sint32 percentFoodConsumed = static_cast<sint32>(totalFood ?
		((totalFoodConsumed * 100.0) / totalFood) : 0.0);
	sint32 percentFoodStored = static_cast<sint32>(100.0 - (percentFoodCrime + percentFoodConsumed));

	gslog_print("  Food Total: %d\n", totalFood);
	gslog_print("  Food Crime: %d (%d%%):\n", totalFoodCrime, percentFoodCrime);
	gslog_print("  Food Consumed: %d (%d%%)\n", totalFoodConsumed, percentFoodConsumed);
	gslog_print("  Food Stored: %d (%d%%)\n", totalFood - (totalFoodConsumed + totalFoodCrime), percentFoodStored);

		sint32 totalProduction = 0, totalProductionCrime = 0,
		totalProductionUnitUpkeep = 0, totalProductionPublicWorks = 0;

	for (cityIndex = 0; cityIndex < cityList->Num(); cityIndex++)
	{
		CityData *cityData = (*cityList)[cityIndex].GetData()->GetCityData();

		totalProduction             += cityData->GetGrossCityProduction();
		totalProductionCrime        += cityData->GetProdCrime();
		totalProductionPublicWorks  += cityData->ComputeMaterialsPaid(pl->m_materialsTax);
	}

	totalProductionUnitUpkeep = pl->GetReadinessCost();

	sint32 percentProductionCrime = totalProduction ?
		((totalProductionCrime * 100) / totalProduction) : 0;
	sint32 percentProductionUnitUpkeep = totalProduction ?
		((totalProductionUnitUpkeep * 100) / totalProduction) : 0;
	sint32 percentProductionPublicWorks = totalProduction ?
		((totalProductionPublicWorks * 100) / totalProduction) : 0;
#if 0   // Unused
	sint32 percentProductionCityUse = 100 - (percentProductionCrime +
		percentProductionUnitUpkeep + totalProductionPublicWorks);
#endif

	gslog_print("  Total Production: %d\n", totalProduction);
	gslog_print("  Production Crime: %d (%d%%)\n", totalProductionCrime, percentProductionCrime);
	gslog_print("  Production Unit Upkeep: %d (%d%%)\n", totalProductionUnitUpkeep, percentProductionUnitUpkeep);
	gslog_print("  Production PW: %d (%d%%)\n", totalProductionPublicWorks, percentProductionPublicWorks);

	sint32 totalCommerce = 0, totalCommerceCrime = 0,
		totalCommerceWages = 0, totalCommerceBuildingUpkeep = 0,
		totalCommerceScience = 0, totalNetGold = 0, totalTrade = 0;


	sint32  wonderReduction = wonderutil_GetDecreaseMaintenance(pl->GetBuiltWonders());
	for (cityIndex = 0; cityIndex < cityList->Num(); cityIndex++)
	{
		CityData *cityData = (*cityList)[cityIndex].GetData()->GetCityData();

		sint32 commerceBuildingUpkeep = buildingutil_GetTotalUpkeep
		    (cityData->GetImprovements(), wonderReduction, cityData->GetOwner()); //EMOD added owner

		totalCommerce               += cityData->GetGrossCityGold();
		totalCommerceCrime          += cityData->GetGoldCrime();
		totalCommerceWages          += cityData->CalcWages();
		totalCommerceBuildingUpkeep += commerceBuildingUpkeep;
		totalCommerceScience        += cityData->GetScience();
		totalNetGold                += cityData->GetNetCityGold();
		totalTrade                  += cityData->CalculateGoldFromResources();
	}

	sint32 percentCommerceCrime = totalCommerce ?
		((totalCommerceCrime * 100) / totalCommerce) : 0;
	sint32 percentCommerceWages = totalCommerce ?
		((totalCommerceWages * 100) / totalCommerce) : 0;
	sint32 percentCommerceBuildingUpkeep = totalCommerce ?
		((totalCommerceBuildingUpkeep * 100) / totalCommerce) : 0;
	sint32 percentCommerceScience = totalCommerce ?
		((totalCommerceScience * 100) / totalCommerce) : 0;
	sint32 percentCommerceSavings = 100 - (percentCommerceCrime +
		percentCommerceWages + percentCommerceBuildingUpkeep +
		percentCommerceScience);

	gslog_print("  Total Commerce: %d\n", totalCommerce);
	gslog_print("  Total Commerce After Science: %d\n", totalCommerce - totalCommerceScience);
	gslog_print("  Commerce Crime: %d (%d%%)\n", totalCommerceCrime, percentCommerceCrime);
	gslog_print("  Commerce Wages: %d (%d%%)\n", totalCommerceWages, percentCommerceWages);
	gslog_print("  Commerce Building Upkeep: %d (%d%%)\n", totalCommerceBuildingUpkeep, percentCommerceBuildingUpkeep);
	gslog_print("  Commerce Science: %d (%d%%):\n", totalCommerceScience, percentCommerceScience);
	gslog_print("  Commerce Savings %%: %d\n", percentCommerceSavings);
	gslog_print("  Net Gold: %d\n", totalNetGold);
	gslog_print("  Total Trade Profit: %d\n", totalTrade);


	gslog_print("  Total accumulated science: %d\n", pl->m_science->GetLevel());

	sint32 riot, content, happy;
	sint32 totalCities = g_player[player]->CountCityHappiness(riot, content, happy);
	gslog_print("  Total cities: %d\n", totalCities);
	gslog_print("    Rioting: %d\n", riot);
	gslog_print("    Content: %d\n", content);
	gslog_print("    Happy: %d\n", happy);

	sint32 pop = 0;
	sint32 partialPop = 0;
	sint32 i;
	for(i = 0; i < g_player[player]->m_all_cities->Num(); i++) {
		pop += g_player[player]->m_all_cities->Access(i).PopCount();
		partialPop += g_player[player]->m_all_cities->Access(i).CD()->GetPartialPopulation();
	}

	sint32 totalPartialPop = (pop * k_PEOPLE_PER_POPULATION) + partialPop;
	gslog_print("  Total population: %d/%d, +%d\n", pop, totalPartialPop, totalPartialPop - s_populationHack[player]);
	s_populationHack[player] = totalPartialPop;

	sint32 totalUnits = g_player[player]->m_all_units->Num();
	sint32 totalMilUnits = 0;
	sint32 totalOffense = 0;

	for(i = 0; i < totalUnits; i++) {
		const UnitRecord *rec = g_player[player]->m_all_units->Access(i).GetDBRec();
		if(rec->GetAttack() > 0.001) {
			totalMilUnits++;
			totalOffense += (sint32)rec->GetAttack();
		}
	}
	gslog_print("  Total Units: %d\n", totalUnits);
	gslog_print("  Total Military Units: %d\n", totalMilUnits);
	gslog_print("  Total Attack Strength: %d\n", totalOffense);

	double s;
	g_player[player]->m_tax_rate->GetScienceTaxRate(s);
	gslog_print("  Settings:\n");
	gslog_print("     Science Tax: %d\n", AsPercentage(s));
	gslog_print("              PW: %d\n", sint32(g_player[player]->m_materialsTax * 100));
	gslog_print("         Workday: %d\n", g_player[player]->m_global_happiness->GetUnitlessWorkday());
	gslog_print("           Wages: %d\n", g_player[player]->m_global_happiness->GetUnitlessWages());
	gslog_print("         Rations: %d\n", g_player[player]->m_global_happiness->GetUnitlessRations());

	gslog_print("  Current research: %s\n", g_theAdvanceDB->Get(g_player[player]->m_advances->GetResearching())->GetNameText());

#endif
}
