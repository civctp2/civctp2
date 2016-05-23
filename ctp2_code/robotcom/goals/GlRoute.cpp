#include "c3.h"
#include "globals.h"
#include "ic3player.h"

#include "GlRoute.h"
#include "globals.h"
#include "civarchive.h"
#include "FlatPtr.h"
#include "bset.h"
#include "ic3TerrDB.h"

#include "ic3GameState.h"
#include "aimain.h"
#include "airndcnt.h"
#include "FzOut.h"

#include "TradeRouteData.h"

#include "CityAgent.h"


extern double fz_gold_2_production;

GoalRoute::GoalRoute(CityAgent *ca,  sint32 good_type)
{
    m_city_agent = ca;
    m_city_id = new BSetID;
    *m_city_id = m_city_agent->GetID();

    m_dest_city=NULL;
    m_dest_id = new BSetID;
    *m_dest_id = 0;


    m_good_type = good_type;

    m_utility = BAD_UTILITY;
    m_cost = 0.0;

    m_is_satisfied = FALSE;
}

GoalRouteFlat::~GoalRouteFlat()
{ }

GoalRoutePtr::~GoalRoutePtr()
{
    NDELETE(m_city_id);
    NDELETE(m_dest_id);
}

GoalRoute::~GoalRoute()
{
    return;
}

ZEROMEM(GoalRouteFlat);
FLATSERIALIZE(GoalRouteFlat);
ZEROMEM(GoalRoutePtr);
PTRSERIALIZE(GoalRoutePtr);

void GoalRoutePtr::Store(CivArchive &archive)
{
    BSetID id;

    archive.PutUINT32(m_city_id->GetVal());
    archive.PutUINT32(m_dest_id->GetVal());

#ifdef _DEBUG
    if (m_dest_id->GetVal() == 0) {
        Assert(m_dest_city == NULL);
    } else {
        Assert(m_dest_city != NULL);
    }
#endif
}

void GoalRoutePtr::Load(CivArchive &archive, sint32 pcount, uint8 nmask[])
{
    uint32 v;

    v = archive.GetUINT32();
    m_city_id = new BSetID(v);
    m_city_agent = (class CityAgent*)1;
    v = archive.GetUINT32();
    m_dest_id = new BSetID(v);
    if (v == 0) {
        m_dest_city = NULL;
    } else {
        m_dest_city = (class CityAgent*)1;
    }

}

GoalRoute::GoalRoute (BSet<CityAgent> *bs, CivArchive &archive)
{
	Serialize(bs, archive);
}

void GoalRoute::Serialize(BSet<CityAgent> *bs, CivArchive &archive)
{
    CHECKSERIALIZE

  	GoalRouteFlat::Serialize(archive);
	GoalRoutePtr::Serialize(archive);

    if (!archive.IsStoring()) {
        m_city_agent = bs->Find(*m_city_id);

        if (m_dest_city) {
            m_dest_city = bs->Find(*m_dest_id);
        }
    }
}

void GoalRoute::EstimateRouteBenifit(AiMain *ai, CityAgent *ca, double &utility,
   double &cost)
{

    double v, n, g;
    BOOL is_unknown_id;
    double dest1, dest2, src1, src2;
    sint32 local_good_count;
    sint32 total_good_count;

    if (ca == m_city_agent) {
       utility = BAD_UTILITY;
    } else {
        MapPointData dest_pos;
        ca->GetPos(dest_pos);
        if (SPACE_Z == dest_pos.z) {
            utility = BAD_UTILITY;
            cost = 10000;
            return;
        }

        v = ai->m_terrainDB->GetGoodGoldValue(m_good_type);

        ai->m_player->GetGoodCount(&is_unknown_id, m_city_agent->m_id.GetVal(),
            m_good_type, &local_good_count, &total_good_count);

        n = double(total_good_count);

        if (n <1) {
            utility = BAD_UTILITY;
            cost = 10000;
            return;
        }
        src1 = (n+1) * (n) * 0.5;
        src2 = (n) * (n-1) * 0.5;

        ai->m_player->GetGoodCount(&is_unknown_id, ca->m_id.GetVal(),
            m_good_type, &local_good_count, &total_good_count);

        n = double(total_good_count);
        dest1 = (n+1) * (n) * 0.5;
        dest2 = (n+2) * (n+1) * 0.5;

        g =  fz_gold_2_production * v * ((dest2 - dest1) + (src2 - src1));
        utility = g * ai->m_round_count->EstimateRoundsToEnd();

        cost = ai->m_player->GetRouteCost(&is_unknown_id,
            m_city_agent->GetID().GetVal(), ca->GetID().GetVal());

        Assert(is_unknown_id == FALSE);

    }
}

void GoalRoute::ConnectRoute(AiMain *ai, sint32 &freight)
{
    BOOL is_unknown_id;

    sint32 in_num;
    sint32 in_max;
    sint32 out_num;
    sint32 out_max;

    if (0 <  freight) {

        ai->m_player->GetCityNumTradeRoutes (ai->m_my_player_id, m_city_agent->GetID().GetVal(), &is_unknown_id,
                &in_num, &in_max, &out_num, &out_max);
        Assert(is_unknown_id == FALSE);
        if (out_num >= out_max) return;

        ai->m_player->GetCityNumTradeRoutes (ai->m_my_player_id, m_dest_city->GetID().GetVal(), &is_unknown_id,
                &in_num, &in_max, &out_num, &out_max);
        Assert(is_unknown_id == FALSE);
        if (in_num >= in_max) return;

        sint32 connect_success = ai->m_player->CreateTradeRoute(&is_unknown_id, m_city_agent->GetID().GetVal(),
           	ROUTE_TYPE_RESOURCE,  m_good_type, m_dest_city->GetID().GetVal());

        Assert(connect_success);

        if (!connect_success) return;

        Assert(is_unknown_id == FALSE);


    }

    freight--;

    m_is_satisfied = TRUE;

}

void GoalRoute::SetDest(CityAgent *dest_city, double max_utility, double max_cost)
{

    m_dest_city = dest_city;
    *m_dest_id = dest_city->GetID();
    m_cost = max_cost;
    m_utility = max_utility;
}

void GoalRoute::SetIsSatisfied(sint32 s)
{
    m_is_satisfied = s;
}

BOOL GoalRoute::IsSatisfied() const
{
    return (m_utility < 1) || m_is_satisfied;
}

double GoalRoute::GetCost() const
{
    return m_cost;
}

sint32 GoalRoute::GetGoodType() const
{
    return m_good_type;
}
