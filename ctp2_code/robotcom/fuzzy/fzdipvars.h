#pragma once
#ifndef __FZDIPVARS_H__
#define __FZDIPVARS_H__

#include "DiplomaticTypes.h"


extern double fz_war_with_him;
extern double fz_cease_fire_with_him;
extern double fz_alliance_with_him;
extern double fz_neutral_with_him;


extern double fz_turns_to_disaster;


extern double fz_dip_regard_pirate_delta ; 
extern double fz_dip_regard_attack_delta ; 
extern double fz_dip_regard_incursion_delta ; 
extern double fz_dip_regard_pillage_delta ; 
extern double fz_dip_regard_message_delta ; 
extern double fz_dip_regard_general_delta ; 
extern double fz_dip_regard_strength_delta ; 

extern double fz_accept_threshold;
extern double fz_his_coldwar_attacked_me;
extern double fz_his_hotwar_attacked_me;
extern double fz_send_threshold;
extern double fz_dip_send_coef;
extern double fz_send_tone;
extern double fz_contact_gained;
extern double fz_gold_gained;
extern double fz_gold_gained;
extern double fz_gold_utility;
extern double fz_military_incursions;
extern double fz_reduces_incursions;
extern double fz_my_military_incursions;
extern double fz_advances_skipped;
extern double fz_advance_cost;
extern double fz_demand_advance;
extern double fz_projected_food;
extern double fz_projected_production;
extern double fz_demand_city;
extern double fz_map_area_gained;
extern double fz_last_got_map;
extern double fz_diplomatic_persistence;
extern double fz_max_messages_considered_per_turn;
extern double fz_length_of_war_with_him;
extern double fz_num_he_is_at_war;
extern double fz_he_shares_continent; 
extern double fz_closest_capitol; 

extern double fz_he_built_space;
extern double fz_he_built_wormhole;
extern double fz_he_built_undersea;


extern double fz_player_type;
extern double fz_chase_the_rabbit;
extern double fz_distance_to_his_capitol;
extern double fz_unoccupied_humans;

extern double fz_rejected_requests;
extern double fz_accepted_requests;
extern double fz_last_attacked_me;
extern double fz_he_beat_me;
extern double fz_he_beat_me_this_turn;

extern double fz_diplomatic_regard_towards_him;
extern double fz_diplomatic_regard_delta;
extern double fz_unit_regard_towards_him;
extern double fz_unit_regard_delta;

extern double fz_best_friend; 
extern double fz_worst_enemy; 
extern double fz_my_strength;

extern double fz_his_strength;
extern double fz_third_party_strength;
extern double fz_at_war_with_third_party;
extern double fz_third_party_relative_strength;
extern double fz_his_third_party_relative_strength;
extern double fz_third_party_regard;
extern double fz_his_trade_with_third_party;
extern double fz_stop_trade;
extern double fz_demand_attack_enemy;
extern double fz_demand_leave_our_lands;
extern double fz_send_strength;
extern double fz_my_percent_pollution;
extern double fz_his_percent_pollution;
extern double fz_his_pollution_level;
extern double fz_my_pollution_level;
extern double fz_global_pollution_level;
extern double fz_he_stops_polluting;
extern double fz_stop_polluting_others;
extern double fz_his_route_threat;
extern double fz_he_gains_advance;
extern double fz_advance_give;
extern double fz_offer_city_happiness_level;
extern double fz_projected_city_happiness;
extern double fz_he_gets_map;
extern double fz_he_gains_gold;
extern double fz_gains_alliance;
extern double fz_gains_cease_fire;
extern double fz_end_pollution_pact;
extern double fz_cease_fire;
extern double fz_alliance;
extern double fz_gold_need;
extern double fz_he_gains_gold;
extern double fz_pact_end_pollution;
extern double fz_map_exchange;
extern double fz_food_utility;
extern double fz_production_utility;
extern double fz_trade_utility;
extern double fz_reduce_pollution_utility;
extern double fz_give_advance;
extern double fz_he_attacks_enemy;
extern double fz_he_stops_trade_with_third_party;
extern double fz_he_gains_city;
extern double fz_i_lose_city;
extern double fz_relative_strength;
extern double fz_loyalty;
extern double fz_loyalty_modifier;


extern double fz_he_accepted_greetings;
extern double fz_he_accepted_gold_demand;
extern double fz_he_accepted_demand_map;
extern double fz_he_accepted_city_demand;
extern double fz_he_accepted_demand_advance;
extern double fz_he_accepted_demand_stop_trade;
extern double fz_he_accepted_demand_attack_enemy;
extern double fz_he_accepted_demand_leave_our_lands;
extern double fz_he_accepted_demand_reduce_pollution;
extern double fz_he_accepted_offer_advance;
extern double fz_he_accepted_offer_city;
extern double fz_he_accepted_offer_map;
extern double fz_he_accepted_offer_gold;
extern double fz_he_accepted_offer_cease_fire;
extern double fz_he_accepted_offer_alliance;
extern double fz_he_accepted_offer_capture_city;
extern double fz_he_accepted_offer_pact_end_pollution;
extern double fz_he_accepted_exchange_advance;
extern double fz_he_accepted_exchange_city;
extern double fz_he_accepted_exchange_map;
extern double fz_he_accepted_demand_no_piracy;


extern double fz_he_rejected_greetings;
extern double fz_he_rejected_gold_demand;
extern double fz_he_rejected_demand_map;
extern double fz_he_rejected_city_demand;
extern double fz_he_rejected_demand_advance;
extern double fz_he_rejected_demand_stop_trade;
extern double fz_he_rejected_demand_attack_enemy;
extern double fz_he_rejected_demand_leave_our_lands;
extern double fz_he_rejected_demand_reduce_pollution;
extern double fz_he_rejected_offer_advance;
extern double fz_he_rejected_offer_city;
extern double fz_he_rejected_offer_map;
extern double fz_he_rejected_offer_gold;
extern double fz_he_rejected_offer_cease_fire;
extern double fz_he_rejected_offer_alliance;
extern double fz_he_rejected_offer_capture_city;
extern double fz_he_rejected_offer_pact_end_pollution;
extern double fz_he_rejected_exchange_advance;
extern double fz_he_rejected_exchange_city;
extern double fz_he_rejected_exchange_map;
extern double fz_he_rejected_demand_no_piracy;

extern double fz_i_will_attack_third_party;
extern double fz_stops_trade_with_third_party;
extern double fz_i_will_leave_his_lands;
extern double fz_reduces_pollution;
extern double fz_i_will_stop_piracy;
extern double fz_i_gain_advance;
extern double fz_i_gain_city;
extern double fz_i_gain_gold;
extern double fz_gold_lost;
extern double fz_i_gain_map;

extern double fz_diplomacy_rand_min, fz_diplomacy_rand_max;

extern double fz_diplomatic_coeff[REQUEST_TYPE_NULL];
extern double fz_diplomatic_receive_coeff[REQUEST_TYPE_NULL];

extern double fz_my_responses[REQUEST_TYPE_NULL];
extern double fz_my_responses_decay[REQUEST_TYPE_NULL];
extern double fz_his_responses[REQUEST_TYPE_NULL];
extern double fz_his_responses_decay[REQUEST_TYPE_NULL];

extern double fz_his_coldwar_attacked_me_decay;
extern double fz_his_hotwar_attacked_me_decay;
extern double fz_accepted_requests_decay;
extern double fz_rejected_requests_decay;
extern double fz_piracy_decay;
extern double fz_his_pirated_me;

extern double fz_he_will_stop_piracy;
extern double fz_my_num_trade_routes;
extern double fz_his_num_trade_routes;
extern double fz_his_relative_num_trade_routes;

extern double fz_violating_stop_trade;
extern double fz_violating_leave_our_lands;
extern double fz_violating_reduce_pollution;
extern double fz_violating_end_pollution;
extern double fz_violating_capture_city;
extern double fz_violating_attack_enemy;

extern double fz_have_stop_trade;
extern double fz_have_leave_our_lands;
extern double fz_have_reduce_pollution;
extern double fz_have_end_pollution;
extern double fz_have_attack_enemy;
extern double fz_have_alliance;
extern double fz_have_cease_fire;
extern double fz_his_relative_strength;
extern double fz_his_normalized_rank;
extern double fz_attacked_my_enemies;
extern double fz_attacked_my_friends;
extern double fz_attacked_my_enemies_decay;
extern double fz_attacked_my_friends_decay;
extern double fz_his_broken_treaties;
extern double fz_his_broken_treaties_decay;
#endif
