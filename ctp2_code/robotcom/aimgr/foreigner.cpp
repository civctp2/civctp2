#include "c3.h"
#include "IRobot.h"
#include "civarchive.h"
#include "Agreement.h"

#include "ic3GameState.h"
#include "aimain.h"
#include "Foreigner.h"
#include "AiMap.h"
#include "ic3world.h"

#include "ForeignAgent.h"
#include "ForeignCity.h"
#include "MapPoint.h"
#include "CityAgent.h"

#include "FzOut.h"

#include "ic3player.h"
#include "diplomatictypes.h"

#include "airndcnt.h"
#include "Cont.h"

#include "C3Player.h"

extern double fz_com_city_scale;
extern double fz_com_city_decay;

extern double fz_i_use_mass_destruction;

extern double fz_my_responses[REQUEST_TYPE_NULL];
extern double fz_his_responses[REQUEST_TYPE_NULL];
extern double fz_my_responses_decay[REQUEST_TYPE_NULL];
extern double fz_his_responses_decay[REQUEST_TYPE_NULL];







Foreigner::Foreigner(PLAYER_INDEX player_index)
{

    BadValue();

	m_player_index = player_index;

    m_visible_units = new BSet<ForeignAgent>;
    m_known_cities = new BSet<ForeignCity>;
    m_center_of_mass = new MapPointData;
    m_center_of_mass->x = 20;
    m_center_of_mass->y = 20;
    m_center_of_mass->z = 0;

	m_hotwar_attacked_me = 0;
	m_coldwar_attacked_me = 0;
	m_attacked_by_me = 0;
	m_sent_greetings = FALSE;
	m_he_accepted_greetings = FALSE;
	m_estimated_gold = 100;
	m_have_contact = FALSE;
	m_got_map_from = -100;
	m_last_attacked_me = -1000;
    m_military_incursions = FALSE;

	m_sent_requests = new sint32[REQUEST_TYPE_NULL];
	m_considered_sending_requests = new sint32[REQUEST_TYPE_NULL];
	for(sint32 i = 0; i < sint32(REQUEST_TYPE_NULL) ; i++) {
		m_sent_requests[i] = -1000000;
		m_considered_sending_requests[i] = -1000000;
	}

	m_my_responses = new double[REQUEST_TYPE_NULL];
	m_his_responses = new double[REQUEST_TYPE_NULL];
	m_my_responses_decay = new double[REQUEST_TYPE_NULL];
	m_his_responses_decay = new double[REQUEST_TYPE_NULL];
	for(i=0; i< sint32(REQUEST_TYPE_NULL); i++)
		{
			m_my_responses[i] = 0;
			m_his_responses[i] = 0;
			m_my_responses_decay[i] = 1.0;
			m_his_responses_decay[i] = 1.0;
		}

	m_diplomatic_regard = 50;
	m_unit_regard = 30;
	m_current_bonus_regard = 0;
	m_static_bonus_regard = 0;
	m_loyalty = 50;
	m_hate_you_insanely = FALSE;

	m_accepted_requests = 0;
	m_rejected_requests = 0;

	m_pollution_level = 0;

	m_trade_threat = 0;

    m_his_raw_strength = 0.0;
    m_his_relative_strength = 0.5;
    m_his_normalized_rank = 0.5;

    m_war_began = -1.0;
    m_peace_began = 0.0;
	m_closest_capitol =0.0;
	m_he_shares_continent = 0.0;
    m_distance_to_his_capitol = 0.0;
    m_num_he_is_at_war = 0.0;
    m_he_nuked_me_count = 0.0;
    m_his_total_nuke_count = 0.0;
    m_he_has_space = 0.0;
    m_he_has_undersea = 0.0;

    m_he_built_underwater_units = 0.0;
    m_he_built_space_units = 0.0;
    m_he_built_wormhole_units = 0.0;
    m_pirated_this_turn = 0.0;
    m_piracy_count = 0.0;
	m_decayed_piracy = 0.0;

    m_total_production = 0.0;
    m_total_unit_cost = 0.0;

	m_piracy_decay = 1.0;
	m_accepted_requests_decay = 1.0;
	m_rejected_requests_decay = 1.0;
	m_his_coldwar_attacked_me_decay = 1.0;
	m_his_hotwar_attacked_me_decay = 1.0;

	m_violating_stop_trade = 0.0;
	m_violating_leave_our_lands = 0.0;
	m_violating_reduce_pollution = 0.0;
	m_violating_capture_city = 0.0;
	m_violating_end_pollution = 0.0;
	m_violating_attack_enemy = 0.0;

	m_trade_persistence = 0;

	m_attacked_my_friends = 0;
	m_attacked_my_enemies = 0;
	m_attacked_my_friends_decay = 1;
	m_attacked_my_enemies_decay = 1;
	m_broken_treaties = 0;
	m_broken_treaties_decay = 1.0;

    m_he_beat_me = 0.0;
    m_he_beat_me_this_turn = 0.0;

    m_enemy_of_my_enemy = 0.0;
    m_friend_of_my_enemy= 0.0;
    m_has_pact_against_me= 0.0;
	m_is_robot = FALSE;
    m_leave_our_lands = FALSE;
}







Foreigner::~Foreigner()
{
    delete m_center_of_mass;
    m_center_of_mass = NULL;

    delete m_visible_units;
    m_visible_units = NULL;

    delete m_known_cities;
    m_known_cities = NULL;

	delete [] m_sent_requests;
    m_sent_requests = NULL;

	delete [] m_considered_sending_requests;
    m_considered_sending_requests  = NULL;

	delete [] m_my_responses;
	m_my_responses = NULL;
	delete [] m_his_responses;
	m_his_responses = NULL;
	delete [] m_my_responses_decay;
	m_my_responses_decay = NULL;
	delete [] m_his_responses_decay;
	m_his_responses_decay = NULL;
}







Foreigner::Foreigner(CivArchive &archive, AiMain *init_ai)
{
    BadValue();

    Serialize(archive);

    BSetID h_id;
    ForeignAgent *him=NULL;
    for (him = m_visible_units->First(h_id); m_visible_units->Last(); him =  m_visible_units->Next(h_id)) {
        him->Hook(init_ai);
    }

    ForeignCity *ca=NULL;
    for (ca = m_known_cities->First(h_id); m_known_cities->Last(); ca =  m_known_cities->Next(h_id)) {
        ca->Hook(init_ai);
    }

}

void Foreigner::BadValue()
{
	m_player_index = 0xcdcdcdcd;

    m_center_of_mass = NULL;

    m_visible_units = NULL;
    m_known_cities = NULL;

	m_coldwar_attacked_me = 0xcdcdcdcd;
	m_hotwar_attacked_me = 0xcdcdcdcd;
    m_attacked_by_me = 0xcdcdcdcd;
	m_last_attacked_me = 0xcdcdcdcd;
	m_estimated_gold = 0xcdcdcdcd;
	m_have_contact = 0xcdcdcdcd;
	m_military_incursions = 0xcdcdcdcd;
	m_got_map_from = 0xcdcdcdcd;
	m_rejected_requests = 0xcdcdcdcd;
	m_accepted_requests = 0xcdcdcdcd;
	m_sent_requests = NULL;
	m_considered_sending_requests = NULL;

	m_my_responses = NULL;
	m_his_responses = NULL;
	m_my_responses_decay = NULL;
	m_his_responses_decay = NULL;

	m_diplomatic_regard = 0xcdcdcdcd;
    m_unit_regard = 0xcdcdcdcd;
	m_current_bonus_regard = 0xcdcdcdcd;
    m_static_bonus_regard = 0xcdcdcdcd;
	m_pollution_level = 0xcdcdcdcd;

	m_trade_threat = 0xcdcdcdcd;
	m_sent_greetings = 0xcdcdcdcd;
	m_he_accepted_greetings = 0xcdcdcdcd;
}







void Foreigner::Serialize(CivArchive &archive)
{
    CHECKSERIALIZE

    if (archive.IsStoring()) {
		archive << m_player_index;

        archive << m_center_of_mass->x;
        archive << m_center_of_mass->y;
        archive << m_center_of_mass->z;

		archive << m_attacked_by_me;
		archive << m_hotwar_attacked_me;
		archive << m_coldwar_attacked_me;
		archive << m_sent_greetings;
		archive << m_he_accepted_greetings;
		archive << m_estimated_gold;
		archive << m_have_contact;
		archive << m_military_incursions;
		archive << m_got_map_from;
		archive << m_rejected_requests;
		archive << m_accepted_requests;
		archive << m_last_attacked_me;

        Assert(0.0 <= m_diplomatic_regard);
        Assert(m_diplomatic_regard <= 100.0);
        Assert(0.0 <= m_unit_regard);
        Assert(m_unit_regard <= 100.0);
        archive << m_diplomatic_regard;
        archive << m_unit_regard;

        archive << m_current_bonus_regard;
        archive << m_static_bonus_regard;

        archive << m_he_beat_me;
        archive << m_he_beat_me_this_turn;

        archive << m_enemy_of_my_enemy;
        archive << m_friend_of_my_enemy;
        archive << m_has_pact_against_me;

	    archive << m_pollution_level;

	    archive << m_trade_threat;
		archive << m_loyalty;

        archive << m_unit_regard;
        archive << m_his_raw_strength;
        archive << m_his_relative_strength;
        archive << m_his_normalized_rank;

        archive << m_war_began;
        archive << m_peace_began;
        archive << m_he_shares_continent;
        archive << m_distance_to_his_capitol;
        archive << m_closest_capitol;
        archive << m_num_he_is_at_war;
        archive << m_he_nuked_me_count;
        archive << m_his_total_nuke_count;
        archive << m_he_has_space;
        archive << m_he_has_undersea;

        archive << m_he_built_underwater_units;
        archive << m_he_built_space_units;
        archive << m_he_built_wormhole_units;
        archive << m_pirated_this_turn;
        archive << m_piracy_count;

        archive << m_total_production;
        archive << m_total_unit_cost;

		archive << m_his_coldwar_attacked_me_decay;
		archive << m_his_hotwar_attacked_me_decay;
		archive << m_accepted_requests_decay;
		archive << m_rejected_requests_decay;
		archive << m_piracy_decay;
		archive << m_decayed_piracy;

		archive << m_violating_stop_trade;
		archive << m_violating_leave_our_lands;
		archive << m_violating_reduce_pollution;
		archive << m_violating_capture_city;
		archive << m_violating_end_pollution;
		archive << m_violating_attack_enemy;
		archive << m_trade_persistence;

        archive << m_loyalty;

		archive << m_attacked_my_enemies;
		archive << m_attacked_my_enemies_decay;
		archive << m_attacked_my_friends;
		archive << m_attacked_my_friends_decay;

		archive << m_broken_treaties;
		archive << m_broken_treaties_decay;

		archive << m_hate_you_insanely;

        archive.PutSINT32(m_leave_our_lands);

		archive.Store((uint8*)m_sent_requests, sizeof(sint32) * REQUEST_TYPE_NULL);
		archive.Store((uint8*)m_considered_sending_requests, sizeof(sint32) * REQUEST_TYPE_NULL);

		archive.Store((uint8*)m_my_responses, sizeof(double) * REQUEST_TYPE_NULL);
		archive.Store((uint8*)m_his_responses, sizeof(double) * REQUEST_TYPE_NULL);
		archive.Store((uint8*)m_my_responses_decay, sizeof(double) * REQUEST_TYPE_NULL);
		archive.Store((uint8*)m_his_responses_decay, sizeof(double) * REQUEST_TYPE_NULL);

        m_visible_units->Serialize(((IC3CivArchive *)&archive));
        m_known_cities->Serialize(((IC3CivArchive *)&archive));

    } else {

		archive >> m_player_index;

        m_center_of_mass = new MapPointData();
        archive >> m_center_of_mass->x;
        archive >> m_center_of_mass->y;
        archive >> m_center_of_mass->z;

		archive >> m_attacked_by_me;
		archive >> m_hotwar_attacked_me;
		archive >> m_coldwar_attacked_me;
		archive >> m_sent_greetings;
		archive >> m_he_accepted_greetings;
		archive >> m_estimated_gold;
		archive >> m_have_contact;
		archive >> m_military_incursions;
		archive >> m_got_map_from;
		archive >> m_rejected_requests;
		archive >> m_accepted_requests;
		archive >> m_last_attacked_me;

        archive >> m_diplomatic_regard;
        archive >> m_unit_regard;
        Assert(0.0 <= m_diplomatic_regard);
        Assert(m_diplomatic_regard <= 100.0);
        Assert(0.0 <= m_unit_regard);
        Assert(m_unit_regard <= 100.0);

        archive >> m_current_bonus_regard;
        archive >> m_static_bonus_regard;

        archive >> m_he_beat_me;
        archive >> m_he_beat_me_this_turn;

        archive >> m_enemy_of_my_enemy;
        archive >> m_friend_of_my_enemy;
        archive >> m_has_pact_against_me;

        archive >> m_pollution_level;

	    archive >> m_trade_threat;
		archive >> m_loyalty;

        archive >> m_unit_regard;
        archive >> m_his_raw_strength;
        archive >> m_his_relative_strength;
        archive >> m_his_normalized_rank;

        archive >> m_war_began;
        archive >> m_peace_began;
        archive >> m_he_shares_continent;
        archive >> m_distance_to_his_capitol;
        archive >> m_closest_capitol;
        archive >> m_num_he_is_at_war;
        archive >> m_he_nuked_me_count;
        archive >> m_his_total_nuke_count;
        archive >> m_he_has_space;
        archive >> m_he_has_undersea;

        archive >> m_he_built_underwater_units;
        archive >> m_he_built_space_units;
        archive >> m_he_built_wormhole_units;

        archive >> m_pirated_this_turn;
        archive >> m_piracy_count;

        archive >> m_total_production;
        archive >> m_total_unit_cost;

		archive >> m_his_coldwar_attacked_me_decay;
		archive >> m_his_hotwar_attacked_me_decay;
		archive >> m_accepted_requests_decay;
		archive >> m_rejected_requests_decay;
		archive >> m_piracy_decay;
		archive >> m_decayed_piracy;

		archive >> m_violating_stop_trade;
		archive >> m_violating_leave_our_lands;
		archive >> m_violating_reduce_pollution;
		archive >> m_violating_capture_city;
		archive >> m_violating_end_pollution;
		archive >> m_violating_attack_enemy;
		archive >> m_trade_persistence;

        archive >> m_loyalty;

		archive >> m_attacked_my_enemies;
		archive >> m_attacked_my_enemies_decay;
		archive >> m_attacked_my_friends;
		archive >> m_attacked_my_friends_decay;

		archive >> m_broken_treaties;
		archive >> m_broken_treaties_decay;

 		archive >> m_hate_you_insanely;

		m_leave_our_lands = archive.GetSINT32();

        m_sent_requests = new sint32[REQUEST_TYPE_NULL];
		archive.Load((uint8*)m_sent_requests, sizeof(sint32) * REQUEST_TYPE_NULL);
		m_considered_sending_requests = new sint32[REQUEST_TYPE_NULL];
		archive.Load((uint8*)m_considered_sending_requests, sizeof(sint32) * REQUEST_TYPE_NULL);

	    m_my_responses = new double[REQUEST_TYPE_NULL];
		archive.Load((uint8*)m_my_responses, sizeof(double) * REQUEST_TYPE_NULL);
		m_his_responses = new double[REQUEST_TYPE_NULL];
		archive.Load((uint8*)m_his_responses, sizeof(double) * REQUEST_TYPE_NULL);
		m_my_responses_decay = new double[REQUEST_TYPE_NULL];
		archive.Load((uint8*)m_my_responses_decay, sizeof(double) * REQUEST_TYPE_NULL);
		m_his_responses_decay = new double[REQUEST_TYPE_NULL];
		archive.Load((uint8*)m_his_responses_decay, sizeof(double) * REQUEST_TYPE_NULL);

        m_visible_units = new BSet<ForeignAgent>;
        m_visible_units->Serialize(((IC3CivArchive *)&archive));
        m_known_cities = new BSet<ForeignCity>;
        m_known_cities->Serialize(((IC3CivArchive *)&archive));
    }
}









void Foreigner::BeginTurn(AiMain *ai)

{
    UpdateCenterOfMass();
	UpdateMiscInfo(ai);
}


void Foreigner::UpdateCenterOfMass()
{
    ForeignAgent *him;
    ForeignCity *ca;
    BSetID h_id;
    double count = 1.0;
    MapPointData pos, sum;
    sum = *m_center_of_mass;

    for (him = m_visible_units->First(h_id); m_visible_units->Last(); him =  m_visible_units->Next(h_id)) {
        him->GetPos(pos);
        sum.x += pos.x;
        sum.y += pos.y;
        sum.z += pos.z;
        count += 1.0;
    }

    for (ca = m_known_cities->First(h_id); m_known_cities->Last(); ca =  m_known_cities->Next(h_id)) {
        ca->GetPos(pos);
        sum.x += sint16(pos.x * fz_com_city_scale);
        sum.y += sint16(pos.y * fz_com_city_scale);
        sum.z += sint16(pos.z * fz_com_city_scale);
        count += fz_com_city_scale;
    }

    m_center_of_mass->x = int(fz_com_city_decay * (double(sum.x) / count) + (1.0 - fz_com_city_decay) * m_center_of_mass->x);
    m_center_of_mass->y = int(fz_com_city_decay * (double(sum.y) / count) + (1.0 - fz_com_city_decay) * m_center_of_mass->y);
    m_center_of_mass->z = int(fz_com_city_decay * (double(sum.z) / count) + (1.0 - fz_com_city_decay) * m_center_of_mass->z);

}







void Foreigner::GetCenterOfMass(MapPointData &pos)
{
    pos = *m_center_of_mass;
}







void Foreigner::ResetCenterOfMass(MapPointData &pos)
{
    *m_center_of_mass = pos;
}








void Foreigner::UpdateMiscInfo(AiMain *ai)
{
	m_estimated_gold = ai->m_gs->GetGold(m_player_index);
	m_pollution_level = ai->m_gs->GetPollutionLevel(m_player_index);

    ForeignAgent *him;
    BSetID h_id;
    double count = 1.0;
    MapPointData pos;
	m_trade_threat = 0;
	sint32 dist;

    for (him = m_visible_units->First(h_id); m_visible_units->Last(); him =  m_visible_units->Next(h_id)) {
		him->GetPos(pos);
		dist = ai->m_player->NearTradeRoute(&pos);

		if(dist > 0) {
			if(dist > 1) {
				m_trade_threat += 1;
			} else {
				m_trade_threat += 2;
			}
		}
	}

    if (HeAgreed(ai, AGREEMENT_TYPE_CEASE_FIRE) ||
        (0.0 < ai->m_player->GetAlliance(m_player_index, ai->m_my_player_id))) {
        m_current_bonus_regard = m_static_bonus_regard;
    }

	m_is_robot = ai->m_player->IsRobotPlayer(m_player_index);

	m_leave_our_lands = HeAgreed(ai, AGREEMENT_TYPE_DEMAND_LEAVE_OUR_LANDS);
	if (m_leave_our_lands)
		ai->SetLeaveOurLands(TRUE);
}













void Foreigner::ClearVision(AiMain *ai)
{

    int cur_round = ai->m_round_count->GetRound();
    ForeignAgent *him;
    BSetID h_id, del_id;
    MapPointData pos;
	MapPointData XYpos;
    BOOL is_unknown_id;
    DynamicArray<BSetID> kill_me_list(16);
	sint32 remember_until_round;

    del_id = BSetID(0);
    for (him = m_visible_units->First(h_id); m_visible_units->Last(); him =  m_visible_units->Next(h_id))
    {
		if (him->IsStealthy(ai))
			remember_until_round = (cur_round -
									ai->m_planner->the_AIP.memory_rounds_stealth);
		else
			remember_until_round = (cur_round -
									ai->m_planner->the_AIP.memory_rounds);
        if (him->GetLastSeen() < remember_until_round)
            kill_me_list.Insert(h_id);
        else {

			sint32 army_count = him->GetUnitNum();

            ai->m_player->GetArmyXYPos(m_player_index, h_id.GetVal(), &is_unknown_id, &XYpos);

			Assert(is_unknown_id == FALSE);
			if (is_unknown_id)
			{
				Assert(him->GetUnitNum() == 0);
				kill_me_list.Insert(h_id);
			}
			else
			{

				ai->m_player->XY2Norm(XYpos, pos);

				him->SetCurPos(pos);
				him->SetCurXYPos(XYpos);

				if (ai->m_world->IsCityHere(&pos))
					kill_me_list.Insert(h_id);

				Assert(pos.z == SPACE_Z || pos.z == GROUND_Z);
			}
        }
    }

    sint32 unit_idx, unit_num = kill_me_list.Num();
    for (unit_idx=0; unit_idx<unit_num; unit_idx++) {
        m_visible_units->Del(kill_me_list[unit_idx]);
    }

}

void Foreigner::RegisterHisArmyTeleported (AiMain *ai, uint32 i_moved,
        MapPointData *new_pos)
{
    BSetID b_id(i_moved);

    ForeignAgent *his_army = m_visible_units->Find(b_id);
    MapPointData XY_pos;

    if (his_army) {
        his_army->SetCurPos(*new_pos);
        ai->m_player->Norm2XY(*new_pos, XY_pos);
        his_army->SetCurXYPos(XY_pos);
    }
}









void Foreigner::CanSeeArmy(AiMain *ai, MapPointData &pos,
    const uint32 u_id, const sint32 top_unit_type, const sint32 unit_num)
{
	MapPointData XYpos;
    BOOL is_unknown_id;
	BSetID a_id(u_id);

	Assert(top_unit_type != 0);
	if (top_unit_type == 0)
		return;

    ForeignAgent *him = m_visible_units->Find(a_id);

    if (him == NULL)
    {
        him = new ForeignAgent(m_player_index, ai, a_id);

        BSetNode<ForeignAgent> *node = new BSetNode<ForeignAgent>(a_id, him);
        m_visible_units->InsertNode(node);
    }

    ai->m_player->GetArmyXYPos(m_player_index, a_id.GetVal(), &is_unknown_id, &XYpos);

    Assert(is_unknown_id == FALSE);

    him->SetCurPos(pos);
    him->SetCurXYPos(XYpos);
    him->SetTopUnit(top_unit_type);
    him->SetUnitNum(unit_num);
    him->SetLastSeen(ai->m_round_count->GetRound());
}








void Foreigner::CanSeeCity(AiMain *ai, MapPointData &pos, uint32 u_id)
{
    BSetID c_id(u_id);

    ForeignCity *him;

    if (m_known_cities->Find(c_id) == NULL) {

        him = new ForeignCity(ai, c_id, m_player_index);

        him->SetPos(pos);
        BSetNode<ForeignCity> *node = new BSetNode<ForeignCity>(c_id, him);
        m_known_cities->InsertNode(node);

    }
}







void Foreigner::RemoveArmy(AiMain *ai, uint32 u_id)
{
    BSetID c_id(u_id);
    ForeignAgent *bad_unit= NULL;

    bad_unit = m_visible_units->Find(c_id);

    if (bad_unit) {
	    m_visible_units->Del(c_id);
    }

}







void Foreigner::RemoveCity(AiMain *ai, uint32 u_id)
{
	BSetID c_id(u_id);
    ForeignCity *bad_city= NULL;

    bad_city = m_known_cities->Find(c_id);

    if (bad_city) {
	    m_known_cities->Del(c_id);
    }
}













BOOL Foreigner::IsAtHotWar()
{

    return (m_unit_regard  + m_current_bonus_regard) <= k_REGARD_HOTWAR;

}








BOOL Foreigner::IsAtColdWar()
{
    return (m_unit_regard + m_current_bonus_regard) <= k_REGARD_COLDWAR;
}







void Foreigner::AttackedByMe()
{
	m_attacked_by_me++;

    if (k_REGARD_HOTWAR <= m_diplomatic_regard)
    	m_diplomatic_regard = k_REGARD_HOTWAR-1;

    if (k_REGARD_HOTWAR <= m_unit_regard)
        m_unit_regard = k_REGARD_HOTWAR-1;

    Assert(0.0 <= m_diplomatic_regard);
    Assert(m_diplomatic_regard <= 100.0);
    Assert(0.0 <= m_unit_regard);
    Assert(m_unit_regard <= 100.0);

    m_leave_our_lands = FALSE;
}







void Foreigner::ColdwarAttackedMe(AiMain *ai, const double delta)
{
	m_coldwar_attacked_me += delta;
	m_last_attacked_me = ai->m_round_count->GetRound();
}

void Foreigner::HotwarAttackedMe(AiMain *ai, const double delta)
{
	m_hotwar_attacked_me += delta;
	m_last_attacked_me = ai->m_round_count->GetRound();


    if (k_REGARD_HOTWAR <= m_diplomatic_regard)
    	m_diplomatic_regard = k_REGARD_HOTWAR-1;

    if (k_REGARD_HOTWAR <= m_unit_regard)
        m_unit_regard = k_REGARD_HOTWAR-1;

    Assert(0.0 <= m_diplomatic_regard);
    Assert(m_diplomatic_regard <= 100.0);
    Assert(0.0 <= m_unit_regard);
    Assert(m_unit_regard <= 100.0);

    m_leave_our_lands = FALSE;

    m_current_bonus_regard = 0.0;
    m_static_bonus_regard = 0.0;
}

void Foreigner::RegisterFightWinner(sint32 lost_units, sint32 winner)
{
    if (winner == m_player_index) {
        m_he_beat_me += 1.0;
        m_he_beat_me_this_turn += 1.0;
    } else {
        m_he_beat_me -= 1.0;
        m_he_beat_me_this_turn -= 1.0;
    }
}

void Foreigner::BeginTurnHeBeatMe(const double m_he_beat_me_decay)
{
    m_he_beat_me *= m_he_beat_me_decay;

}

void Foreigner::EndTurnClear_HeBeatMeThisTurn()
{
	m_he_beat_me_this_turn = 0.0;
}







void Foreigner::RegisterPlayerInfo(C3PlayerInfo *pi)
{
	m_estimated_gold = pi->m_gold;
}







sint32 Foreigner::LastSentRequest(REQUEST_TYPE r)
{
	return m_sent_requests[r];
}







void Foreigner::RegisterSentRequestTo(AiMain *ai, REQUEST_TYPE r)
{
	m_sent_requests[r] = ai->m_round_count->GetRound();
}







sint32 Foreigner::LastConsideredSendingRequest(REQUEST_TYPE r)
{
	return m_considered_sending_requests[r];
}







void Foreigner::RegisterConsideredSendingRequest(AiMain *ai, REQUEST_TYPE r)
{
	Assert(r < REQUEST_TYPE_MAX);
	Assert(r >= 0);
	if(r >= 0 && r < REQUEST_TYPE_NULL)
		m_considered_sending_requests[r] = ai->m_round_count->GetRound();
}








double Foreigner::GetMyResponses(REQUEST_TYPE r)
{
	return m_my_responses[r];
}








void Foreigner::SetMyResponses(REQUEST_TYPE r, int answer)
{
	Assert(r < REQUEST_TYPE_MAX);
	Assert(r >= 0);

	if (r < 0 || r >= REQUEST_TYPE_NULL) return;

	if(answer == 1)
	{
		m_my_responses[r]++;
	}
}







void Foreigner::UpdateMyResponses()
{
	for(sint32 r = 0; r < REQUEST_TYPE_NULL; r ++)
	{
		fz_my_responses[(REQUEST_TYPE) r] = m_my_responses[(REQUEST_TYPE) r];

	}
}








void Foreigner::DecayMyResponses(REQUEST_TYPE r)
{
	Assert(r < REQUEST_TYPE_MAX);
	Assert(r >= 0);

	if (r < 0 || r >= REQUEST_TYPE_NULL) return;

	m_my_responses[r] = m_my_responses[r] * m_my_responses_decay[r];
	if(m_my_responses[r] < 0.1) { m_my_responses[r] = 0.0; }

}







void Foreigner::SetMyResponsesDecay(REQUEST_TYPE r,double decay)
{
	Assert (r < REQUEST_TYPE_MAX && r>=0);
	if (r < 0 || r >= REQUEST_TYPE_NULL) return;

	if(decay >= 0 && decay <= 1.0)
	{
		m_my_responses_decay[r] = decay;
	}
}








double Foreigner::GetHisResponses(REQUEST_TYPE r)
{
	Assert (r < REQUEST_TYPE_MAX && r>=0);
	if (r < 0 || r >= REQUEST_TYPE_NULL) return 0.0;

	return m_his_responses[(REQUEST_TYPE) r];
}








void Foreigner::SetHisResponses(REQUEST_TYPE r, int answer)
{
	Assert(r < REQUEST_TYPE_MAX);
	Assert(r >= 0);

	if (r < 0 || r >= REQUEST_TYPE_NULL) return;

	if(answer == 0)
	{
		if(m_his_responses[r] < 0)
		{
			m_his_responses[r]--;
		}else
		{
			m_his_responses[r] = -1;
		}
	}
	if(answer == 1)
		if(m_his_responses[r] > 0)
		{
			m_his_responses[r]++;
		}else
		{
			m_his_responses[r] = 1;
		}
}







void Foreigner::UpdateHisResponses()
{
	for(sint32 r = 0; r <REQUEST_TYPE_NULL; r++)
	{
		fz_his_responses[(REQUEST_TYPE) r] = m_his_responses[(REQUEST_TYPE) r];
	}
}







void Foreigner::SetHisResponsesDecay(REQUEST_TYPE r,double decay)
{
	Assert(r >= 0 && r < REQUEST_TYPE_MAX);

	if (r < 0 || r >= REQUEST_TYPE_NULL) return;

	if(decay >= 0 && decay <= 1.0)
	{
		m_his_responses_decay[r] = decay;
	}
}








void Foreigner::DecayHisResponses(REQUEST_TYPE r)
{
	Assert(r < REQUEST_TYPE_MAX && r >= 0);
	if (r < 0 || r >= REQUEST_TYPE_NULL) return;

	m_his_responses[r] = m_his_responses[r] * m_his_responses_decay[r];

	if(m_his_responses[r] < 0.1 && m_his_responses[r] > -0.1) {
		m_his_responses[r] = 0.0;
	}
}







sint32 Foreigner::GetTradeWith(AiMain *ai, PLAYER_INDEX third_party)
{
	return ai->GetTradeWith(m_player_index, third_party);
}







double Foreigner::GetMilitaryThreat() const
{

	return 5;
}







double Foreigner::GetDefense() const
{

	return 5;
}







double Foreigner::HeDefeatedMeRecently() const
{

	return 1;
}







double Foreigner::IDefeatedHimRecently() const
{

	return 1;
}

double Foreigner::GetLoyalty() const
{
	return m_loyalty;
}










void Foreigner::RegisterRegardIncrease(sint32 delta)
{
    m_static_bonus_regard += delta;
    if (m_static_bonus_regard < 0)  {
        m_static_bonus_regard = 0;
    }

    m_current_bonus_regard += delta;
    if (m_current_bonus_regard < 0)  {
        m_current_bonus_regard = 0;
    }
}

void Foreigner::GetFZRegard(sint32 &diplomatic, sint32 &unit, sint32 &bonus)
{
    diplomatic = sint32(m_diplomatic_regard);
    unit = sint32(m_unit_regard);
    bonus = sint32(m_current_bonus_regard);
}

void Foreigner::SetFZRegard (sint32 diplomatic, sint32 unit, sint32 bonus)
{
    m_diplomatic_regard = diplomatic;
    m_unit_regard = unit;
    m_current_bonus_regard = bonus;
    m_static_bonus_regard = bonus;

    Assert(0.0 <= m_diplomatic_regard);
    Assert(m_diplomatic_regard <= 100.0);
    Assert(0.0 <= m_unit_regard);
    Assert(m_unit_regard <= 100.0);

}

void Foreigner::HeBrokeCeaseFire()
{

	m_loyalty -= 10.0;
    if (k_REGARD_HOTWAR <= m_diplomatic_regard)
    	m_diplomatic_regard = k_REGARD_HOTWAR - 5;

    if (k_REGARD_HOTWAR < m_unit_regard)
        m_unit_regard = k_REGARD_HOTWAR - 5;

    Assert(0.0 <= m_diplomatic_regard);
    Assert(m_diplomatic_regard <= 100.0);
    Assert(0.0 <= m_unit_regard);
    Assert(m_unit_regard <= 100.0);

    m_leave_our_lands = FALSE;

    m_current_bonus_regard = 0;
}

void Foreigner::HeBrokeAlliance()
{

	m_loyalty -= 10;
    if (k_REGARD_HOTWAR <= m_diplomatic_regard)
    	m_diplomatic_regard = k_REGARD_HOTWAR-1;

    if (k_REGARD_HOTWAR <= m_unit_regard)
        m_unit_regard = k_REGARD_HOTWAR-1;

    Assert(0.0 <= m_diplomatic_regard);
    Assert(m_diplomatic_regard <= 100.0);
    Assert(0.0 <= m_unit_regard);
    Assert(m_unit_regard <= 100.0);

     m_leave_our_lands = FALSE;

}

double Foreigner::UpdateCeaseFire(AiMain *ai, sint32 player_idx)
{
	return HeAgreed(ai, AGREEMENT_TYPE_CEASE_FIRE);
}

double Foreigner::UpdateAlliance(AiMain *ai, sint32 player_idx)
{

	return ai->m_player->GetAlliance(m_player_index, player_idx);
}

double Foreigner::UpdateWar(AiMain *ai, sint32 player_idx)
{

	return ai->m_player->GetWar(player_idx, m_player_index, player_idx);
}

double Foreigner::UpdateNeutrality(AiMain *ai, sint32 player_idx)
{


	if (!HeAgreed(ai, AGREEMENT_TYPE_CEASE_FIRE) &&
		(0.0 <= ai->m_player->GetAlliance(m_player_index, ai->m_my_player_id)) &&
		(0.0 <= ai->m_player->GetWar(player_idx, m_player_index, player_idx)))
		return 1.0;
	else
		return 0.0;
}

PLAYER_INDEX Foreigner::GetPlayerType(AiMain *ai, Foreigner *foreigner)
{
	sint32 foreign_player = foreigner->m_player_index;
	return (PLAYER_INDEX) ai->m_player->GetPlayerType(foreign_player);
}

void Foreigner::UpdateDiplomaticState(AiMain *ai)
{

   m_piracy_count = 0.0;





	m_num_he_is_at_war = double(ai->m_player->GetNumAtWarWith(m_player_index));

	sint32 m_other_player = GetPlayerIndex();
	sint32 my_index = ai->my_player_index;

	if (UpdateWar(ai, my_index))
	{

		m_peace_began = -1;


		if(m_war_began == -1)
		{
			m_war_began = ai->m_round_count->GetRound();

		}
	} else
	{

		m_war_began = -1;

		if(m_peace_began == -1)
		{
			m_peace_began = ai->m_round_count->GetRound();
		}
	}

   BOOL use_max_dist=FALSE;
   MapPointData my_cap_pos;
   MapPointData his_cap_pos;
   CityAgent *the_capitol;

   if (CAPITOL_STATE_HASIT == ai->m_capitol_state) {
       the_capitol = ai->m_city_set->Find(*(ai->m_capitol_id));
       Assert(the_capitol);
       the_capitol->GetPos(my_cap_pos);
   } else {
       use_max_dist = TRUE;
   }

   if (!ai->m_player->GetCapitolPos(m_player_index, &his_cap_pos)) {
       use_max_dist = TRUE;
   }

   if (use_max_dist) {
       m_distance_to_his_capitol = 200.0;
   } else {
       m_distance_to_his_capitol = ai->m_map->WrappedDistance(my_cap_pos, his_cap_pos);
   }

   ComputeLandContinentShared(ai, m_he_shares_continent);

   m_total_production = ai->m_player->GetTotalProduction(m_player_index);
   m_total_unit_cost = ai->m_player->GetTotalUnitCost(m_player_index);

   m_his_raw_strength = k_UNIT_STRENGTH_COEF * m_total_unit_cost +
       k_PRODUCTION_STRENGTH_COEF * m_total_production;

   double ranking_strength =
	   ai->m_gs->GetStrength(m_player_index);
   m_his_raw_strength += ranking_strength * 0.1;

   if (m_his_raw_strength <= 0.0) {
       m_his_relative_strength = -10.0;
   } else if (ai->m_my_raw_strength <= 0.0) {
       m_his_relative_strength = 10.0;
   } else if (ai->m_my_raw_strength <= m_his_raw_strength) {
       m_his_relative_strength = (m_his_raw_strength - ai->m_my_raw_strength) / ai->m_my_raw_strength;
   } else {
       m_his_relative_strength =  -(ai->m_my_raw_strength - m_his_raw_strength) / m_his_raw_strength;
   }
}
double Foreigner::GetLengthOfWarWithHim(AiMain *ai)
{
	if( m_war_began == -1)
	{
		return 0;
	}
	if(m_war_began > 0)
	{
		return ai->m_round_count->GetRound() - m_war_began;
	}
	return 0;
}
double Foreigner::GetLengthOfPeaceWithHim(AiMain *ai)
{
	if( m_peace_began == -1)
	{
		return 0;
	}
	if(m_peace_began >= 0)
	{
		return ai->m_round_count->GetRound() - m_peace_began;
	}
	return 0;
}

void Foreigner::RegisterHeNuked(const PLAYER_INDEX my_player_id,
                                const PLAYER_INDEX his_target)
{




    if (my_player_id == his_target) {
        if ((k_REGARD_INSANE_HATRED) <= m_diplomatic_regard) {
            m_diplomatic_regard = k_REGARD_INSANE_HATRED;
        }

        if ((k_REGARD_INSANE_HATRED) < m_unit_regard) {
            m_unit_regard = (k_REGARD_INSANE_HATRED);
        }

        m_he_nuked_me_count += 1.0;
    }

    Assert(0.0 <= m_diplomatic_regard);
    Assert(m_diplomatic_regard <= 100.0);
    Assert(0.0 <= m_unit_regard);
    Assert(m_unit_regard <= 100.0);

    m_his_total_nuke_count += 1.0;
}

void Foreigner::RegisterHeBuiltNewAgeUnit(NEW_AGE unit_type)
{
    switch (unit_type) {
    case NEW_AGE_UNDERWATER:
        m_he_built_underwater_units = 1.0;
        break;
    case NEW_AGE_SPACE:
        m_he_built_space_units = 1.0;
        break;
    case NEW_AGE_WORMHOLE:
        m_he_built_wormhole_units = 1.0;
        break;
    default:
        INSANE(UNIT_TYPE_UNKNOWN);
    }
}

void Foreigner::RegisterPiracy()
{
    m_pirated_this_turn = 1.0;
    m_piracy_count += 1.0;
	m_decayed_piracy += 1.0;
}

void Foreigner::GetUnitAge(double &underwater,
                           double &space,
                           double &wormhole)
{
    underwater = m_he_built_underwater_units;
    space = m_he_built_space_units;
    wormhole = m_he_built_wormhole_units;
}

void Foreigner::ComputeLandContinentShared(AiMain *ai,
    double &continents_shared)
{
    if (NULL == m_known_cities) {
        continents_shared = 0.0;
    }

    sint32 cont_num = ai->m_continents->GetNumLandCont();
    sint32 cont_idx;
    double *city_count = new double[cont_num];

    for (cont_idx=0; cont_idx<cont_num; cont_idx++) {
        city_count[cont_idx] = 0.0;
    }

    BSetID id;
    CityAgent *ca;
    sint32 city_cont;
    BOOL city_is_land;

    MapPointData pos;

    double my_city_total = 0.0;
    for (ca = ai->m_city_set->First(id); ai->m_city_set->Last(); ca = ai->m_city_set->Next(id)) {

        ca->GetPos(pos);

        if (SPACE_Z == pos.z) continue;

        if (!ai->m_continents->GetContinent(ai, pos, city_cont, city_is_land)) {
            INSANE(CAN_NOT_FIND_CONT);
        }

        if (city_is_land) {
            city_count[city_cont] += 1.0;
            my_city_total += 1.0;
        }
    }


   double *foreign_city_count = new double[cont_num];

   for (cont_idx=0; cont_idx<cont_num; cont_idx++) {
       foreign_city_count[cont_idx] = 0.0;
   }

   BSet<ForeignCity> *known_cities=NULL;
   ForeignCity *fc=NULL;
   double foreign_city_total = 0.0;

   for (fc = m_known_cities->First(id); m_known_cities->Last(); fc = m_known_cities->Next(id)) {

           fc->GetPos(pos);

           if (SPACE_Z == pos.z) continue;

           if (!ai->m_continents->GetContinent(ai, pos, city_cont, city_is_land)) {
                INSANE(CAN_NOT_FIND_CONT);
           }

      if (city_is_land) {
          foreign_city_count[city_cont] += 1.0;
          foreign_city_total += 1.0;
      }
   }

   double percent_my_cont;
   double percent_his_cont;
   continents_shared = 0.0;
   for (cont_idx=0; cont_idx<cont_num; cont_idx++) {
       if ((0.0 < city_count[cont_idx]) && (0.0 < foreign_city_count[cont_idx])) {
           percent_my_cont = city_count[cont_idx]/my_city_total;
           percent_his_cont = foreign_city_count[cont_idx]/foreign_city_total;


           continents_shared += max(percent_my_cont, percent_his_cont);

       }
   }

   delete[] foreign_city_count;
   foreign_city_count = NULL;
   delete[] city_count;
   city_count = NULL;

   if (1.0 < continents_shared) {
       continents_shared = 1.0;
   }
}

void Foreigner::SetDiplomaticRegard(AiMain *ai, double reg)
{

	m_diplomatic_regard = reg;

    Assert(0.0 <= m_diplomatic_regard);
    Assert(m_diplomatic_regard <= 100.0);
    Assert(0.0 <= m_unit_regard);
    Assert(m_unit_regard <= 100.0);

	ai->m_player->SetRegard(m_player_index, sint32(GetDiplomaticRegard()));
}

BOOL Foreigner::HeAgreed(AiMain *ai, AGREEMENT_TYPE agreement)
{

	return ai->m_player->HaveActiveAgreement(m_player_index, agreement);
}

BOOL Foreigner::CanUseMassDestructionOnHim() const
{
    if (0 < m_he_nuked_me_count) {
        return TRUE;
    } else if (0.5 < fz_i_use_mass_destruction) {
        return TRUE;
    }  else {
        return FALSE;
    }
}

void Foreigner::RegisterAgreementAgainstMe(double str)
{
    m_has_pact_against_me += str;

    if (k_REGARD_HOTWAR <= m_diplomatic_regard)
    	m_diplomatic_regard = k_REGARD_HOTWAR-1;

    if (k_REGARD_HOTWAR <= m_unit_regard)
        m_unit_regard = k_REGARD_HOTWAR-1;

    Assert(0.0 <= m_diplomatic_regard);
    Assert(m_diplomatic_regard <= 100.0);
    Assert(0.0 <= m_unit_regard);
    Assert(m_unit_regard <= 100.0);

    m_leave_our_lands = FALSE;

}
