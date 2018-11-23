#include "c3.h"

#include "globals.h"
#include "ic3player.h"

#include "GlOvertime.h"

#include "bset.h"
#include "civarchive.h"
#include "FlatPtr.h"

#include "ic3GameState.h"
#include "ic3world.h"
#include "aimain.h"
#include "gold.h"

#include "CityAgent.h"

GoalOvertime::GoalOvertime(AiMain *init_ai, CityAgent *ca)
{
	m_home_city = ca;
	m_city_id = new BSetID;
	*m_city_id = m_home_city->GetID();

	MapPointData pos;
	m_home_city->GetPos(pos);


























	sint32 rounds_to_completion = m_home_city->
		EstimateRoundsToEmptyQueue(init_ai);

	if(m_home_city->FrontIsImprovement())
		m_utility = 1000.0;
	else
		m_utility = 1.0;


	if (rounds_to_completion > 5)
		m_utility *= 100;

	if (pos.z == SPACE_Z)
		{
			m_utility *= 1.5;
		}

	sint32 terrain;
	init_ai->m_world->GetTerrainType(&pos, &terrain);
	switch (terrain)
		{
		  case TERRAIN_WATER_SHALLOW:
		  case TERRAIN_WATER_DEEP:
		  case TERRAIN_WATER_VOLCANO:
		  case TERRAIN_WATER_BEACH:
		  case TERRAIN_WATER_SHELF:
		  case TERRAIN_WATER_TRENCH:
		  case TERRAIN_WATER_RIFT:
			m_utility *= 1.25;
			break;
		}

	sint32 gross_production = m_home_city->GetGrossProduction();
	m_utility -= gross_production;

    Assert(BAD_UTILITY < m_utility);

    BOOL is_unknown_id;
    m_cost = init_ai->m_player->CityGetOvertimeCost(&is_unknown_id, m_city_id->GetVal());

    Assert(0.0 < m_cost);

    Assert(is_unknown_id == FALSE);
    m_satisfied = FALSE;
}

ZEROMEM(GoalOvertimeFlat);
FLATSERIALIZE(GoalOvertimeFlat);
ZEROMEM(GoalOvertimePtr);
PTRSERIALIZE(GoalOvertimePtr);

GoalOvertime::GoalOvertime(BSet<CityAgent>*bs, CivArchive &archive)
{
	Serialize(bs,archive);
}

GoalOvertime::~GoalOvertime()
{
	NDELETE(m_city_id);
}

void GoalOvertimePtr::Store(CivArchive &archive)
{
	archive.PutUINT32(m_city_id->GetVal());
}

void GoalOvertimePtr::Load(CivArchive &archive, sint32 pcount, uint8 nmask[])
{
    sint32 v;
	v = archive.GetUINT32();
	m_city_id = new BSetID;
	*m_city_id = v;
	m_home_city = (CityAgent*)1;
}

void GoalOvertime::Serialize(BSet<CityAgent>*bs, CivArchive &archive)
{

    CHECKSERIALIZE

    GoalOvertimeFlat::Serialize(archive);
    GoalOvertimePtr::Serialize(archive);

    if (!archive.IsStoring()) {
        m_home_city = bs->Find(*m_city_id);
    }

	Assert(m_home_city);
}

void GoalOvertime::Spend(AiMain *ai, double &gold, double &cost)
{
    BOOL is_unknown_id;
    BOOL rush_buy_success;

    Assert(0.0 < m_cost);

    if (m_cost <= gold &&
		ai->m_gold->CanSpendRushBuy(ai, (sint32) m_cost)) {
        rush_buy_success =  ai->m_player->CityBuyFront(&is_unknown_id, m_city_id->GetVal());
        Assert(rush_buy_success);

        Assert(is_unknown_id == FALSE);
        ai->m_gold->Deduct(sint32(m_cost));
        ai->m_gold->RegisterOvertime(sint32(m_cost));
    }

	m_satisfied = TRUE;

	gold -= m_cost;
	cost = m_cost;
}

CityAgent *GoalOvertime::GetHomeCity()
{
    Assert(m_home_city);
    return m_home_city;
}
