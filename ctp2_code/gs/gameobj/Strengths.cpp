//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : Player strength history
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
// - Crash prevented.
// - Duplicate code removed.
//
//----------------------------------------------------------------------------

#include "c3.h"
#include "Strengths.h"
#include "Player.h"
#include "CivArchive.h"
#include "UnitDynArr.h"
#include "BuildingRecord.h"
#include "WonderRecord.h"
#include "Unit.h"
#include "UnitData.h"
#include "CityData.h"
#include "Gold.h"
#include "TurnCnt.h"
#include "UnitRecord.h"
#include "wonderutil.h"
#include "NewTurnCount.h"
#include "SelItem.h"

Strengths::Strengths(sint32 owner)
{
	m_owner = owner;

	if (g_turn==NULL)
		return;

	sint32 const	curRound = NewTurnCount::GetCurrentRound();
	
	sint32 c, y;
	for(y = 1; y < curRound; y++) {
		for(c = 0; c < sint32(STRENGTH_CAT_MAX); c++) {
			m_strengthRecords[c].Insert(0);
		}
	}
}

Strengths::Strengths(CivArchive &archive)
{
	Serialize(archive);
}

Strengths::~Strengths()
{
}

void Strengths::Serialize(CivArchive &archive)
{
	sint32 i;
	if(archive.IsStoring()) {
		archive << m_owner;
	} else {
		archive >> m_owner;
	}

	for(i = STRENGTH_CAT_NONE + 1; i < STRENGTH_CAT_MAX; i++) {
		m_strengthRecords[i].Serialize(archive);
	}
}

void Strengths::Calculate()
{
	sint32 i;
	for(i = STRENGTH_CAT_NONE + 1; i < STRENGTH_CAT_MAX; i++) {
		switch(i) {
			case STRENGTH_CAT_KNOWLEDGE:
				m_strengthRecords[i].Insert(g_player[m_owner]->GetKnowledgeStrength());
				break;
			case STRENGTH_CAT_MILITARY:
				m_strengthRecords[i].Insert(g_player[m_owner]->GetMilitaryStrength());
				break;
			case STRENGTH_CAT_POLLUTION:
				m_strengthRecords[i].Insert(g_player[m_owner]->GetCurrentPollution()) ;
				break;
			case STRENGTH_CAT_TRADE:
				m_strengthRecords[i].Insert(g_player[m_owner]->GetTradeStrength());
				break;
			case STRENGTH_CAT_GOLD:
				m_strengthRecords[i].Insert(g_player[m_owner]->m_gold->GetIncome());
				break;
			case STRENGTH_CAT_POPULATION:
				m_strengthRecords[i].Insert(g_player[m_owner]->GetTotalPopulation());
				break;
			case STRENGTH_CAT_CITIES:
				m_strengthRecords[i].Insert(g_player[m_owner]->m_all_cities->Num());
				break;
			case STRENGTH_CAT_GEOGRAPHICAL:
				m_strengthRecords[i].Insert(g_player[m_owner]->GetLandArea());
				break;
			case STRENGTH_CAT_SPACE:
				m_strengthRecords[i].Insert(g_player[m_owner]->GetSpaceStrength());
				break;
			case STRENGTH_CAT_UNDERSEA:
				m_strengthRecords[i].Insert(g_player[m_owner]->GetUnderseaStrength());
				break;

			case STRENGTH_CAT_UNITS:
				m_strengthRecords[i].Insert(GetTotalUnitCost());
				break;
			case STRENGTH_CAT_BUILDINGS:
				m_strengthRecords[i].Insert(GetTotalBuildingCost());
				break;
			case STRENGTH_CAT_WONDERS:
				m_strengthRecords[i].Insert(GetTotalWonderCost());
				break;
			case STRENGTH_CAT_PRODUCTION:
				m_strengthRecords[i].Insert(GetTotalProduction());
				break;
			default:
				Assert(FALSE);
				break;
		}
	}
}

sint32 Strengths::GetStrength(STRENGTH_CAT category)
{
	return m_strengthRecords[category].Num() 
		   ? m_strengthRecords[category].GetLast()
		   : 0;
}

sint32 Strengths::GetTurnStrength(STRENGTH_CAT category, sint32 turn) 
{
	if (!m_strengthRecords[category].Num()) return 0;
	
	
	
	
	
	if (turn >= m_strengthRecords[category].Num()) return 0;

	return m_strengthRecords[category][turn];
}

sint32 Strengths::GetTotalUnitCost()
{
	sint32 i;
	sint32 c = 0;
	UnitDynamicArray *units = g_player[m_owner]->m_all_units;
	for(i = units->Num() - 1; i >= 0; i--) {
		c += units->Access(i).GetDBRec()->GetShieldCost();
	}
	return c;
}

sint32 Strengths::GetTotalBuildingCost()
{
	sint32 i, j;
	sint32 c = 0;
	UnitDynamicArray *cities = g_player[m_owner]->m_all_cities;
	for(i = cities->Num() - 1; i >= 0; i--) {
		uint64 builtImprovements = cities->Access(i).GetImprovements();
		for(j = g_theBuildingDB->NumRecords() - 1; j >= 0; j--) {
			if(builtImprovements & ((uint64)1 << (uint64)j)) {
				c += g_theBuildingDB->Get(j)->GetProductionCost();
			}
		}
	}
	return c;
}

sint32 Strengths::GetTotalWonderCost()
{
	sint32 i;
	sint32 c = 0;
	for(i = g_theWonderDB->NumRecords() - 1; i >= 0; i--) {
		if(g_player[m_owner]->m_builtWonders & (uint64(1) << uint64(i))) {
			c += wonderutil_Get(i)->GetProductionCost();
		}
	}
	return c;
}

sint32 Strengths::GetTotalProduction()
{
	sint32 i;
	sint32 c = 0;
	UnitDynamicArray *cities = g_player[m_owner]->m_all_cities;
	for(i = cities->Num() - 1; i >= 0; i--) {
		c += cities->Access(i).GetData()->GetCityData()->GetNetCityProduction();
	}
	return c;
}
