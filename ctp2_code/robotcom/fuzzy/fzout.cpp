
#include "c3.h"

#include "globals.h"
#include "IC3GameState.h"
#include "Fuzzy_Variables.h"
#include "AiMain.h"

#include "FzOut.h"
#include "fliif.h"
#include "FliEngine.h"
#include "FliSymbol.h"

#include "DiplomaticTypes.h"

double fz_freight_reserve; 
double fz_personality_aggression;
double fz_personality_caution;
double fz_personality_expansion; 
double fz_personality_loyalty;
double fz_personality_green;
double fz_personality_smart; 

double fz_max_attack_strength; 
double fz_max_defense_strength; 

double fz_city_threat_decay;  

double fz_com_city_decay; 
double fz_com_city_scale; 

double fz_explore_near_civ; 
double fz_no_settling;


double fz_recruit_goal_threshold;  
double fz_attack_goal_threshold;  
double fz_settle_goal_threshold;  
double fz_no_settle; 
double fz_explore_goal_threshold;  

double fz_bid_recruit_uscale;
double fz_bid_recruit_attackers_uscale; 
double fz_bid_attack_uscale;
double fz_bid_defend_uscale; 
double fz_bid_settle_uscale;
double fz_bid_explore_uscale;
double fz_bid_enslave_uscale; 
double fz_bid_build_list_uscale; 
double fz_bid_patrol_uscale;
double fz_bid_harass_uscale;
double fz_bid_bombard_uscale;
double fz_bid_convert_city_uscale;
double fz_bid_pillage_uscale;
double fz_auto_pillage_chance; 

double fz_bid_pirate_uscale;
double fz_bid_counter_stealth_uscale;
double fz_bid_defuse_mines_uscale;
double fz_bid_bonus_food_uscale;

double fz_settler_packing; 
double fz_settle_patience; 
double fz_land_continents_full_of_friends;
double fz_land_continents_full;

double fz_settle_value_coef;
double fz_settle_bonus_space; 
double fz_settle_bonus_water; 
double fz_settle_bonus_island;

double fz_army_foreign_threat_decay; 
double fz_army_foreign_threat_falloff; 



double fz_reserve_flank_attack ;   
double fz_reserve_flank_defense;
double fz_reserve_range0_attack;     
double fz_reserve_range0_defense;  
double fz_reserve_range1_attack;    
double fz_reserve_range1_defense;



double fz_unexplored_ocean_threshold; 



double fz_inst_prod; 
double fz_inst_food; 
double fz_inst_move; 
double fz_inst_def; 
double fz_inst_vision_range; 
double fz_inst_airfield; 
double fz_inst_fort; 
double fz_inst_attack;
double fz_inst_firepower;
double fz_inst_threat_threshold;
double fz_inst_road_coef;
double fz_min_num_dirty_tiles;
double fz_max_num_dirty_tiles;

double fz_desired_farm_pw; 
double fz_desired_road_pw; 
double fz_desired_mine_pw; 
double fz_desired_transform_pw; 
double fz_desired_inst_pw; 



double fz_count; 
double fz_rand; 
double fz_time; 
double fz_diff_level;

double fz_freight_coef;
double fz_gold_2_production;


double fz_get_on_islands_coef; 
double fz_coef_fill_area_to_boat_explore; 


double fz_pop_food_max; 
double fz_pop_food_min; 
double fz_pop_production_max;
double fz_pop_production_min; 
double fz_min_food_factor;

double fz_pop_gold; 
double fz_pop_science;
double fz_pop_happiness;

double fz_min_happiness; 
double fz_max_crime; 



double fz_he_built_space;
double fz_he_built_wormhole;
double fz_he_built_undersea;

double fz_someone_built_space;
double fz_someone_built_wormhole;
double fz_someone_built_undersea;



double fz_production_tax; 
double fz_public_works;
double fz_current_military_readiness;
    




 

double fz_wgf_prod; 
double fz_wgf_gold; 
double fz_wgf_food; 
double fz_likely_available_food;
double fz_work_min; 
double fz_food_max; 
double fz_food_min; 
double fz_use_expectations;

double fz_desired_wages;
double fz_wage_setting_bonus;

double fz_wgf_prod_setting; 
double fz_wgf_gold_setting; 
double fz_wgf_food_setting; 



double fz_budget_income_new_blg; 
double fz_budget_income_wages; 
double fz_budget_income_science; 
double fz_budget_income_savings; 

double fz_budget_savings_diplomacy; 
double fz_budget_savings_overtime; 
double fz_budget_savings_reserve; 

double fz_rushbuy_max_utility;
double fz_my_current_savings;

double fz_base_gold_reserve_multiple;
double fz_special_action_gold_reserve_multiple;

double fz_percent_wages_needed_content; 

double fz_income_expense_cleric; 
double fz_income_expense_crime;
double fz_income_expense_maintenance;  
double fz_income_expense_wages; 
double fz_income_expense_science; 
double fz_income_expense_savings; 
double fz_gold_and_wage_percent_cost;


double fz_save_expense_overtime; 
double fz_save_expense_diplomacy; 

double fz_city_growth_ratio;
double fz_city_growth_min_percent;

double fz_city_prod_ratio;
double fz_city_prod_min_percent;

double fz_city_fp_ratio;
double fz_city_fp_min_percent;

double fz_city_delta;
double fz_num_cities; 
double fz_max_cities_before_penalty;
double fz_most_human_city_delta; 

double fz_unit_to_city_ratio;

double fz_wgf_stupid; 



double fz_blg_uscale; 
double fz_blg_effect_silo;
double fz_blg_effect_capitol;
double fz_blg_effect_defense_bonus;

double fz_blg_effect_defense_bonus_no_city_walls_attacker;
    
double fz_blg_effect_happy_increment;
double fz_blg_effect_content_aif;

double fz_blg_effect_double_televan;
double fz_blg_effect_reduce_waste;

double fz_blg_effect_production_type_production_increment;
double fz_blg_effect_production_type_food_increment;
double fz_blg_effect_production_type_gold_increment;
double fz_blg_effect_production_type_knowledge_increment;
double fz_blg_effect_production_type_entertainment_increment;
    
double fz_blg_effect_production_type_production_percent;
double fz_blg_effect_production_type_food_percent;
double fz_blg_effect_production_type_gold_percent;
double fz_blg_effect_production_type_knowledge_percent;
double fz_blg_effect_production_type_entertainment_percent;

double fz_blg_effect_pollution_bonus;
double fz_blg_effect_pollution_cleaner;

double fz_blg_effect_affective_pop_dec;
double fz_blg_effect_no_unhappy_people;
double fz_blg_effect_no_happiness;

double fz_blg_effect_lower_crime;
double fz_blg_effect_prevent_conver;
double fz_blg_effect_prevent_slavery;
double fz_blg_effect_lower_overcrowding;
double fz_blg_effect_allow_grunts;
double fz_blg_effect_lower_peace_move;
double fz_blg_effect_gold_per_citizen;
double fz_blg_effect_protect_from_nukes;
double fz_blg_effect_protect_from_bio;
double fz_blg_effect_protect_from_nano;
double fz_blg_effect_lower_pop_pollut;
double fz_blg_effect_television;
double fz_blg_effect_city_wall;
double fz_blg_effect_airport;
double fz_blg_effect_space_launch;



double fz_wonder_uscale; 
double fz_wonder_effect_embassy;
double fz_wonder_effect_close_embassy;
double fz_wonder_effect_reform_cities;
double fz_wonder_effect_free_slaves;
double fz_wonder_effect_global_radar;
double fz_wonder_effect_spies_everywhere;
double fz_wonder_effect_free_space_trans;
double fz_wonder_effect_stop_biowar;
double fz_wonder_effect_pop_monitor;
double fz_wonder_effect_enable_parkranger;
double fz_wonder_effect_all_pop_content;
double fz_wonder_effect_disable_nukes;

double fz_wonder_effect_reduce_readiness_cost;
double fz_wonder_effect_increase_gold;
double fz_wonder_effect_decrease_crime;
double fz_wonder_effect_increase_knowledge;
double fz_wonder_effect_decrease_empire_size;
double fz_wonder_effect_increase_happy_empire;
double fz_wonder_effect_increase_converted_city_fee;
double fz_wonder_effect_tax_water_routes;
double fz_wonder_effect_tax_television;
double fz_wonder_effect_increase_happy;
double fz_wonder_effect_make_genetics_cheap;
double fz_wonder_effect_tax_international_route;
double fz_wonder_effect_polluters_to_parks;
double fz_wonder_effect_reduce_world_pollution;
double fz_wonder_effect_pollution_bonus; 
double fz_wonder_effect_pollution_cleaner;



double fz_sci_unit_mil;
double fz_sci_unit_specialability ;
double fz_sci_unit_moveair;
double fz_sci_unit_movewater;
double fz_sci_unit_movespace;
double fz_sci_unit_settlewater;
double fz_sci_unit_settlespace;
double fz_sci_unit_nuke;
double fz_sci_unit_caravan;
double fz_sci_inst_move;
double fz_sci_inst_def;
double fz_sci_inst_food;
double fz_sci_inst_prod;
double fz_sci_inst_vis;
double fz_sci_blg_def;
double fz_sci_blg_food;
double fz_sci_blg_prod;
double fz_sci_blg_sci;
double fz_sci_blg_gold;
double fz_sci_blg_happy;
double fz_sci_wndr_isnew;
double fz_sci_wndr_obs_his;
double fz_sci_wndr_obs_mine;
double fz_sci_tech_end_game;

double fz_i_can_build_wormhole_probe;



double fz_sci_gov_workday;
double fz_sci_gov_wages;
double fz_sci_gov_rations;
double fz_sci_gov_sci;
double fz_sci_gov_poll;
double fz_sci_gov_poll_unhap;
double fz_sci_gov_emp_size;
double fz_sci_gov_crime;
double fz_sci_gov_martial_law;
double fz_sci_gov_combat_moral;
double fz_sci_gov_readiness; 
double fz_sci_gov_unit_dist;
double fz_sci_gov_gold;

double fz_sci_plantime; 
double fz_sci_ave_turns_to_advance;
double fz_sci_turns_to_current_advance;
double fz_max_time_to_research;

double fz_ratio_advance_cost_me_to_best_human;
double fz_ratio_advance_cost_me_to_best_other;

double fz_can_space_settle; 
double fz_can_sea_settle;




double fz_build_time_decay;

double fz_ave_turns_new_pop; 
double fz_min_turns_new_pop; 


double fz_his_message_tone;
double fz_accept_threshold;
double fz_his_coldwar_attacked_me;
double fz_his_hotwar_attacked_me;
double fz_send_threshold;
double fz_dip_send_coef;
double fz_send_tone;
double fz_contact_gained;
double fz_gold_gained;
double fz_gold_lost;
double fz_gold_utility;
double fz_military_incursions;
double fz_reduces_incursions;
double fz_my_military_incursions;
double fz_advances_skipped;
double fz_advance_cost;
double fz_demand_advance;
double fz_demand_city;
double fz_projected_food;
double fz_projected_production;
double fz_map_area_gained;
double fz_last_got_map;
double fz_diplomatic_persistence;
double fz_max_messages_considered_per_turn;
double fz_rejected_requests;
double fz_accepted_requests;
double fz_last_attacked_me;
double fz_unoccupied_humans;
double fz_most_human_adv_known_delta;

double fz_diplomatic_regard_towards_him;
double fz_diplomatic_regard_delta;
double fz_unit_regard_towards_him;
double fz_unit_regard_delta;
double fz_loyalty;
double fz_loyalty_modifier;

double fz_he_beat_me; 
double fz_he_beat_me_decay; 
double fz_he_beat_me_this_turn; 

double fz_enemy_of_my_enemy;
double fz_friend_of_my_enemy; 
double fz_has_pact_against_me;
double fz_has_pact_against_me_decay; 

double fz_highest_enemy_strength_ratio;
double fz_highest_human_enemy_strength_ratio;
double fz_total_enemy_strength_ratio;
double fz_allies_vs_enemies_strength_ratio;


double fz_war_with_him;
double fz_cease_fire_with_him;
double fz_alliance_with_him;
double fz_neutral_with_him;
double fz_player_type;
double fz_chase_the_rabbit;


double fz_dip_regard_pirate_delta ; 
double fz_dip_regard_attack_delta ; 
double fz_dip_regard_incursion_delta ; 
double fz_dip_regard_pillage_delta ; 
double fz_dip_regard_message_delta ; 
double fz_dip_regard_general_delta ; 
double fz_dip_regard_strength_delta ; 

double fz_my_strength;
double fz_his_strength;
double fz_third_party_strength;
double fz_third_party_relative_strength;
double fz_his_third_party_relative_strength;
double fz_third_party_regard;
double fz_his_trade_with_third_party;
double fz_stop_trade;
double fz_demand_attack_enemy;
double fz_demand_leave_our_lands;
double fz_send_strength;

double fz_i_agreed_to_stop_polluting ;
double fz_i_want_to_stop_others_who_pollute ;

double fz_his_pollution_level;
double fz_my_pollution_level;
double fz_global_pollution_level;
double fz_he_stops_polluting;
double fz_stop_polluting_others;
double fz_his_route_threat;
double fz_he_gains_advance;
double fz_advance_give;
double fz_offer_city_happiness_level;
double fz_projected_city_happiness;
double fz_he_gets_map;
double fz_gains_alliance;
double fz_gains_cease_fire;
double fz_end_pollution_pact;
double fz_cease_fire;
double fz_alliance;
double fz_gold_need;
double fz_he_gains_gold;
double fz_pact_end_pollution;
double fz_map_exchange;

double fz_food_utility;
double fz_production_utility;
double fz_trade_utility;
double fz_reduce_pollution_utility;
double fz_give_advance;
double fz_he_attacks_enemy;
double fz_at_war_with_third_party;
double fz_he_stops_trade_with_third_party;
double fz_relative_strength;
double fz_he_gains_city;
double fz_i_lose_city;

double fz_he_will_stop_piracy;
double fz_his_num_trade_routes;
double fz_his_relative_num_trade_routes;
double fz_my_num_trade_routes;


double fz_he_accepted_greetings;
double fz_he_accepted_gold_demand;
double fz_he_accepted_demand_map;
double fz_he_accepted_city_demand;
double fz_he_accepted_demand_advance;
double fz_he_accepted_demand_stop_trade;
double fz_he_accepted_demand_attack_enemy;
double fz_he_accepted_demand_leave_our_lands;
double fz_he_accepted_demand_reduce_pollution;
double fz_he_accepted_offer_advance;
double fz_he_accepted_offer_city;
double fz_he_accepted_offer_map;
double fz_he_accepted_offer_gold;
double fz_he_accepted_offer_cease_fire;
double fz_he_accepted_offer_alliance;
double fz_he_accepted_offer_capture_city;
double fz_he_accepted_offer_pact_end_pollution;
double fz_he_accepted_exchange_advance;
double fz_he_accepted_exchange_city;
double fz_he_accepted_exchange_map;
double fz_he_accepted_demand_no_piracy;


double fz_he_rejected_greetings;
double fz_he_rejected_gold_demand;
double fz_he_rejected_demand_map;
double fz_he_rejected_city_demand;
double fz_he_rejected_demand_advance;
double fz_he_rejected_demand_stop_trade;
double fz_he_rejected_demand_attack_enemy;
double fz_he_rejected_demand_leave_our_lands;
double fz_he_rejected_demand_reduce_pollution;
double fz_he_rejected_offer_advance;
double fz_he_rejected_offer_city;
double fz_he_rejected_offer_map;
double fz_he_rejected_offer_gold;
double fz_he_rejected_offer_cease_fire;
double fz_he_rejected_offer_alliance;
double fz_he_rejected_offer_capture_city;
double fz_he_rejected_offer_pact_end_pollution;
double fz_he_rejected_exchange_advance;
double fz_he_rejected_exchange_city;
double fz_he_rejected_exchange_map;
double fz_he_rejected_demand_no_piracy;

double fz_i_will_attack_third_party;
double fz_stops_trade_with_third_party;
double fz_i_will_leave_his_lands;
double fz_reduces_pollution;
double fz_i_will_stop_piracy;
double fz_i_gain_advance;
double fz_i_gain_city;
double fz_i_gain_gold;
double fz_i_gain_map;
double fz_diplomacy_rand_min, fz_diplomacy_rand_max;

double fz_diplomatic_coeff[REQUEST_TYPE_NULL];
double fz_diplomatic_receive_coeff[REQUEST_TYPE_NULL];

double fz_my_responses[REQUEST_TYPE_NULL];
double fz_my_responses_decay[REQUEST_TYPE_NULL];
double fz_his_responses[REQUEST_TYPE_NULL];
double fz_his_responses_decay[REQUEST_TYPE_NULL];

double fz_best_friend;
double fz_worst_enemy;

double fz_length_of_war_with_him;
double fz_he_shares_continent;
double fz_distance_to_his_capitol;
double fz_distance_to_nearest_human_capitol;
double fz_min_distance_to_capitol; 
double fz_num_he_is_at_war;
double fz_he_nuked_me_count;
double fz_i_use_mass_destruction;

double fz_he_has_space;
double fz_he_has_undersea;

double fz_his_relative_strength;
double fz_his_normalized_rank;
double fz_my_normalized_rank;
double fz_normalized_rank_diff_between_first_and_second_place;

double fz_unit_to_city_best_human_ratio;

double fz_num_players_in_game; 
double fz_at_war_count; 
double fz_longest_war;
double fz_shortest_peace;
double fz_total_military_incursions;
double fz_lowest_unit_regard;

double fz_size_of_world;
double fz_closest_capitol;

double fz_his_coldwar_attacked_me_decay;
double fz_his_hotwar_attacked_me_decay;
double fz_accepted_requests_decay;
double fz_rejected_requests_decay;
double fz_piracy_decay;
double fz_his_pirated_me;

double fz_violating_stop_trade;
double fz_violating_leave_our_lands;
double fz_violating_reduce_pollution;
double fz_violating_end_pollution;
double fz_violating_attack_enemy;

double fz_turns_to_disaster;
double fz_my_percent_pollution;
double fz_his_percent_pollution;

double fz_have_stop_trade;
double fz_have_leave_our_lands;
double fz_have_reduce_pollution;
double fz_have_end_pollution;
double fz_have_attack_enemy;
double fz_have_cease_fire;
double fz_have_alliance;

double fz_attacked_my_enemies;
double fz_attacked_my_friends;
double fz_attacked_my_enemies_decay;
double fz_attacked_my_friends_decay;
double fz_his_broken_treaties;
double fz_his_broken_treaties_decay;



double fz_gov_happy_workday; 
double fz_gov_happy_wage; 
double fz_gov_happy_rations; 
double fz_gov_happy_size;
double fz_gov_happy_martial;        
double fz_gov_happy_unitdist;
double fz_gov_happy_combat;
double fz_gov_pollution; 
double fz_gov_crime;
double fz_gov_gold; 
double fz_gov_production;  
double fz_gov_science;
double fz_gov_readiness; 
double fz_gov_number_city_growth;
double fz_i_want_this_gov[FZ_GOVERNMENT_MAX]; 
double fz_i_want_this_gov_uscale;
double fz_new_gov_discount;



























double fz_min_force_matching_percent;
double fz_max_force_matching_percent;




double fz_percent_cost_readiness;
double fz_desired_military_readiness;


double fz_total_pop_size;


extern void SetCurrentEngine(FliEngine *e);

BOOL InitFuzzyOut(IC3GameState *gs, AiMain *ai)
{ 
    Assert(ai->m_fliEngine==NULL); 
	ai->m_fliEngine = new FliEngine(gs, ai);
	

	
    ai->m_fliEngine->AddSymbol("personality_aggression", &fz_personality_aggression, 0.5, FLI_SYM_INPUT); 
    ai->m_fliEngine->AddSymbol("personality_caution", &fz_personality_caution, 0.5, FLI_SYM_INPUT); 
    ai->m_fliEngine->AddSymbol("personality_expansion", &fz_personality_expansion, 0.5, FLI_SYM_INPUT); 
    ai->m_fliEngine->AddSymbol("personality_loyalty", &fz_personality_loyalty, 0.5, FLI_SYM_INPUT); 
    ai->m_fliEngine->AddSymbol("personality_green", &fz_personality_green, 0.5, FLI_SYM_INPUT); 
    ai->m_fliEngine->AddSymbol("personality_smart", &fz_personality_smart, 0.5, FLI_SYM_INPUT); 
    
	
    ai->m_fliEngine->AddSymbol("total_pop_size", &fz_total_pop_size, 0.0, FLI_SYM_INPUT); 

	ai->m_fliEngine->AddSymbol("city_threat_decay", &fz_city_threat_decay, 0.2, FLI_SYM_OUTPUT); 
	
	ai->m_fliEngine->AddSymbol("max_attack_strength", &fz_max_attack_strength, 1000, FLI_SYM_OUTPUT); 
	ai->m_fliEngine->AddSymbol("max_defense_strength", &fz_max_defense_strength, 1000, FLI_SYM_OUTPUT); 
	
    ai->m_fliEngine->AddSymbol("com_city_scale", &fz_com_city_scale, 10, FLI_SYM_OUTPUT); 
    ai->m_fliEngine->AddSymbol("com_city_decay", &fz_com_city_decay, 0.9, FLI_SYM_OUTPUT);  
    ai->m_fliEngine->AddSymbol("explore_near_civ", &fz_explore_near_civ, 40.0, FLI_SYM_OUTPUT);

    ai->m_fliEngine->AddSymbol("recruit_goal_threshold", &fz_recruit_goal_threshold, 0.0, FLI_SYM_OUTPUT);  
    ai->m_fliEngine->AddSymbol("attack_goal_threshold", &fz_attack_goal_threshold, 0.0, FLI_SYM_OUTPUT);  
    ai->m_fliEngine->AddSymbol("settle_goal_threshold", &fz_settle_goal_threshold, 0.0, FLI_SYM_OUTPUT);  
    ai->m_fliEngine->AddSymbol("no_settle", &fz_no_settle, 0.0, FLI_SYM_OUTPUT); 
    ai->m_fliEngine->AddSymbol("explore_goal_threshold", &fz_explore_goal_threshold, 0.0, FLI_SYM_OUTPUT);  

    ai->m_fliEngine->AddSymbol("bid_recruit_uscale", &fz_bid_recruit_uscale, 5.0, FLI_SYM_OUTPUT);
    ai->m_fliEngine->AddSymbol("bid_recruit_attackers_uscale", &fz_bid_recruit_attackers_uscale, 1000.0, FLI_SYM_OUTPUT); 
    ai->m_fliEngine->AddSymbol("bid_attack_uscale", &fz_bid_attack_uscale, 10000.0, FLI_SYM_OUTPUT);
    ai->m_fliEngine->AddSymbol("bid_defend_uscale", &fz_bid_defend_uscale, 1.0, FLI_SYM_OUTPUT);
    ai->m_fliEngine->AddSymbol("bid_settle_uscale", &fz_bid_settle_uscale, 1.0, FLI_SYM_OUTPUT);
    ai->m_fliEngine->AddSymbol("bid_explore_uscale", &fz_bid_explore_uscale, 0.01, FLI_SYM_OUTPUT);
    ai->m_fliEngine->AddSymbol("bid_enslave_uscale", &fz_bid_enslave_uscale, 10.0, FLI_SYM_OUTPUT);
    ai->m_fliEngine->AddSymbol("bid_build_list_uscale", &fz_bid_build_list_uscale, 1.0, FLI_SYM_OUTPUT);
    ai->m_fliEngine->AddSymbol("bid_patrol_uscale", &fz_bid_patrol_uscale, 1.0, FLI_SYM_OUTPUT);
    ai->m_fliEngine->AddSymbol("bid_bombard_uscale", &fz_bid_bombard_uscale, 1.0, FLI_SYM_OUTPUT);
    ai->m_fliEngine->AddSymbol("bid_convert_city_uscale", &fz_bid_convert_city_uscale, 1.0, FLI_SYM_OUTPUT);
    ai->m_fliEngine->AddSymbol("bid_bombard_uscale", &fz_bid_bombard_uscale, 1.0, FLI_SYM_OUTPUT);
    ai->m_fliEngine->AddSymbol("bid_harass_uscale", &fz_bid_harass_uscale, 1.0, FLI_SYM_OUTPUT);
	ai->m_fliEngine->AddSymbol("bid_pillage_uscale", &fz_bid_pillage_uscale, 1.0, FLI_SYM_OUTPUT);
	ai->m_fliEngine->AddSymbol("auto_pillage_chance", &fz_auto_pillage_chance, 100.0, FLI_SYM_OUTPUT);    
	ai->m_fliEngine->AddSymbol("bid_pirate_uscale", &fz_bid_pirate_uscale, 1.0, FLI_SYM_OUTPUT);
	ai->m_fliEngine->AddSymbol("bid_counter_stealth_uscale", &fz_bid_counter_stealth_uscale, 1.0, FLI_SYM_OUTPUT);
	ai->m_fliEngine->AddSymbol("bid_defuse_mines_uscale", &fz_bid_defuse_mines_uscale, 1.0, FLI_SYM_OUTPUT);
	ai->m_fliEngine->AddSymbol("bid_bonus_food_uscale", &fz_bid_bonus_food_uscale, 1.0, FLI_SYM_OUTPUT);

    ai->m_fliEngine->AddSymbol("settler_packing", &fz_settler_packing, 1.0, FLI_SYM_OUTPUT);
    ai->m_fliEngine->AddSymbol("settle_patience", &fz_settle_patience, 100.0, FLI_SYM_OUTPUT);
    ai->m_fliEngine->AddSymbol("no_settling", &fz_no_settling, 0.0, FLI_SYM_OUTPUT);

    ai->m_fliEngine->AddSymbol("land_continents_full", &fz_land_continents_full, 0.0, FLI_SYM_INPUT);
    ai->m_fliEngine->AddSymbol("land_continents_full_of_friends", &fz_land_continents_full_of_friends, 0.0, FLI_SYM_INPUT);
    
    ai->m_fliEngine->AddSymbol("settle_value_coef", &fz_settle_value_coef, 100.0, FLI_SYM_OUTPUT); 
	ai->m_fliEngine->AddSymbol("settle_bonus_space", &fz_settle_bonus_space, 100000.0, FLI_SYM_OUTPUT);
    ai->m_fliEngine->AddSymbol("settle_bonus_water", &fz_settle_bonus_water, 50000.0, FLI_SYM_OUTPUT);
    ai->m_fliEngine->AddSymbol("settle_bonus_island", &fz_settle_bonus_island, 1000.0, FLI_SYM_OUTPUT);

    ai->m_fliEngine->AddSymbol("army_foreign_threat_decay", &fz_army_foreign_threat_decay, 0.9, FLI_SYM_OUTPUT);
    ai->m_fliEngine->AddSymbol("army_foreign_threat_falloff", &fz_army_foreign_threat_falloff, 5.0, FLI_SYM_OUTPUT); 

    ai->m_fliEngine->AddSymbol("reserve_flank_attack", &fz_reserve_flank_attack, 0.25, FLI_SYM_OUTPUT);   
    ai->m_fliEngine->AddSymbol("reserve_flank_defense", &fz_reserve_flank_defense, 0.37, FLI_SYM_OUTPUT);
    ai->m_fliEngine->AddSymbol("reserve_range0_attack", &fz_reserve_range0_attack, 0.25, FLI_SYM_OUTPUT);     
    ai->m_fliEngine->AddSymbol("reserve_range0_defense", &fz_reserve_range0_defense, 0.37, FLI_SYM_OUTPUT);  
    ai->m_fliEngine->AddSymbol("reserve_range1_attack", &fz_reserve_range1_attack, 0.25, FLI_SYM_OUTPUT);    
    ai->m_fliEngine->AddSymbol("reserve_range1_defense", &fz_reserve_range1_defense, 0.37, FLI_SYM_OUTPUT);

    ai->m_fliEngine->AddSymbol("unexplored_ocean_threshold", &fz_unexplored_ocean_threshold, 3, FLI_SYM_OUTPUT); 


    ai->m_fliEngine->AddSymbol("inst_prod", &fz_inst_prod, 1.0, FLI_SYM_OUTPUT); 
    ai->m_fliEngine->AddSymbol("inst_food", &fz_inst_food, 10.0, FLI_SYM_OUTPUT); 
    ai->m_fliEngine->AddSymbol("inst_move", &fz_inst_move, 0.01, FLI_SYM_OUTPUT); 
    ai->m_fliEngine->AddSymbol("inst_def", &fz_inst_def, 1.0, FLI_SYM_OUTPUT); 
    ai->m_fliEngine->AddSymbol("inst_vision_range", &fz_inst_vision_range, 1.0, FLI_SYM_OUTPUT); 
    ai->m_fliEngine->AddSymbol("inst_airfield", &fz_inst_airfield, 1.0, FLI_SYM_OUTPUT); 
    ai->m_fliEngine->AddSymbol("inst_fort", &fz_inst_fort, 1.0, FLI_SYM_OUTPUT); 
    ai->m_fliEngine->AddSymbol("inst_attack", &fz_inst_attack, 1.0, FLI_SYM_OUTPUT); 
    ai->m_fliEngine->AddSymbol("inst_firepower", &fz_inst_firepower, 1.0, FLI_SYM_OUTPUT);     
    ai->m_fliEngine->AddSymbol("inst_threat_threshold", &fz_inst_threat_threshold, 1000.0, FLI_SYM_OUTPUT);     
    ai->m_fliEngine->AddSymbol("inst_road_coef", &fz_inst_road_coef, 1.0, FLI_SYM_OUTPUT);     
    ai->m_fliEngine->AddSymbol("min_num_dirty_tiles", &fz_min_num_dirty_tiles, 5.0, FLI_SYM_OUTPUT);     
    ai->m_fliEngine->AddSymbol("max_num_dirty_tiles", &fz_max_num_dirty_tiles, 10.0, FLI_SYM_OUTPUT);     

    ai->m_fliEngine->AddSymbol("desired_farm_pw", &fz_desired_farm_pw, 0.0, FLI_SYM_INPUT);
    ai->m_fliEngine->AddSymbol("desired_road_pw", &fz_desired_road_pw, 0.0, FLI_SYM_INPUT);
    ai->m_fliEngine->AddSymbol("desired_mine_pw", &fz_desired_mine_pw, 0.0, FLI_SYM_INPUT); 
    ai->m_fliEngine->AddSymbol("desired_transform_pw", &fz_desired_transform_pw, 0.0, FLI_SYM_INPUT);
    ai->m_fliEngine->AddSymbol("desired_inst_pw", &fz_desired_inst_pw, 0.0, FLI_SYM_INPUT); 




    ai->m_fliEngine->AddSymbol("count", &fz_count, 0.5, FLI_SYM_INPUT); 
    ai->m_fliEngine->AddSymbol("rand", &fz_rand, 0.5, FLI_SYM_INPUT); 
	

    
	ai->m_fliEngine->AddSymbol("freight_reserve", &fz_freight_reserve, 1.0, FLI_SYM_OUTPUT); 

    ai->m_fliEngine->AddSymbol("freight_coef", &fz_freight_coef, 100.0, FLI_SYM_OUTPUT); 
    ai->m_fliEngine->AddSymbol("gold_2_production", &fz_gold_2_production, 0.5, FLI_SYM_OUTPUT); 

    ai->m_fliEngine->AddSymbol("get_on_islands_coef", &fz_get_on_islands_coef, 10.0, FLI_SYM_OUTPUT); 
    ai->m_fliEngine->AddSymbol("coef_fill_area_to_boat_explore", &fz_coef_fill_area_to_boat_explore, 0.05, FLI_SYM_OUTPUT); 

    ai->m_fliEngine->AddSymbol("time", &fz_time, 0.0, FLI_SYM_INPUT); 
    ai->m_fliEngine->AddSymbol("diff_level", &fz_diff_level, 0.0, FLI_SYM_INPUT); 

    ai->m_fliEngine->AddSymbol("pop_food_max", &fz_pop_food_max, 900.0, FLI_SYM_OUTPUT); 
    ai->m_fliEngine->AddSymbol("pop_food_min", &fz_pop_food_min, 100.0, FLI_SYM_OUTPUT); 
    ai->m_fliEngine->AddSymbol("pop_production_max", &fz_pop_production_max, 900.0, FLI_SYM_OUTPUT); 
    ai->m_fliEngine->AddSymbol("pop_production_min", &fz_pop_production_min, 100.0, FLI_SYM_OUTPUT); 
    ai->m_fliEngine->AddSymbol("min_food_factor", &fz_min_food_factor, 1.0, FLI_SYM_OUTPUT); 

    ai->m_fliEngine->AddSymbol("pop_gold", &fz_pop_gold, 100.0, FLI_SYM_OUTPUT); 
    ai->m_fliEngine->AddSymbol("pop_science", &fz_pop_science, 1.0, FLI_SYM_OUTPUT); 
    ai->m_fliEngine->AddSymbol("pop_happiness", &fz_pop_happiness, 1.0, FLI_SYM_OUTPUT); 
    ai->m_fliEngine->AddSymbol("min_happiness", &fz_min_happiness, 75.0, FLI_SYM_OUTPUT); 
    ai->m_fliEngine->AddSymbol("max_crime", &fz_max_crime, 75.0, FLI_SYM_OUTPUT); 

    ai->m_fliEngine->AddSymbol("he_built_space", &fz_he_built_space, 0.0, FLI_SYM_INPUT); 
    ai->m_fliEngine->AddSymbol("he_built_wormhole", &fz_he_built_wormhole, 0.0, FLI_SYM_INPUT); 
    ai->m_fliEngine->AddSymbol("he_built_undersea", &fz_he_built_undersea, 0.0, FLI_SYM_INPUT); 

    ai->m_fliEngine->AddSymbol("someone_built_space", &fz_someone_built_space, 0.0, FLI_SYM_INPUT); 
    ai->m_fliEngine->AddSymbol("someone_built_wormhole", &fz_someone_built_wormhole, 0.0, FLI_SYM_INPUT); 
    ai->m_fliEngine->AddSymbol("someone_built_undersea", &fz_someone_built_undersea, 0.0, FLI_SYM_INPUT); 
  
	ai->m_fliEngine->AddSymbol("production_tax", &fz_production_tax, 0.5, FLI_SYM_OUTPUT); 
    ai->m_fliEngine->AddSymbol("public_works", &fz_public_works, 0.0, FLI_SYM_INPUT); 
	ai->m_fliEngine->AddSymbol("current_military_readiness", &fz_current_military_readiness, 0.0, FLI_SYM_INPUT); 







    ai->m_fliEngine->AddSymbol("wgf_production", &fz_wgf_prod, 1.0, FLI_SYM_OUTPUT); 
    ai->m_fliEngine->AddSymbol("wgf_gold", &fz_wgf_gold, 1.0, FLI_SYM_OUTPUT);
    ai->m_fliEngine->AddSymbol("wgf_food", &fz_wgf_food, 1.0, FLI_SYM_OUTPUT); 
    ai->m_fliEngine->AddSymbol("likely_available_food", &fz_likely_available_food, 2.5, FLI_SYM_OUTPUT); 

	ai->m_fliEngine->AddSymbol("work_min", &fz_work_min, 0.0, FLI_SYM_OUTPUT); 
    ai->m_fliEngine->AddSymbol("food_max", &fz_food_max, 0.0, FLI_SYM_OUTPUT); 
    ai->m_fliEngine->AddSymbol("food_min", &fz_food_min, 0.0, FLI_SYM_OUTPUT); 

	ai->m_fliEngine->AddSymbol("use_expectations", &fz_use_expectations, 0.0, FLI_SYM_OUTPUT); 

	ai->m_fliEngine->AddSymbol("desired_wages", &fz_desired_wages, -3.0, FLI_SYM_OUTPUT); 
	ai->m_fliEngine->AddSymbol("wage_setting_bonus", &fz_wage_setting_bonus, 0.0, FLI_SYM_OUTPUT); 

    ai->m_fliEngine->AddSymbol("wgf_prod_setting", &fz_wgf_prod_setting, -1.0, FLI_SYM_INPUT); 
    ai->m_fliEngine->AddSymbol("wgf_gold_setting", &fz_wgf_gold_setting, -1.0, FLI_SYM_INPUT); 
    ai->m_fliEngine->AddSymbol("wgf_food_setting", &fz_wgf_food_setting, -1.0, FLI_SYM_INPUT); 


    ai->m_fliEngine->AddSymbol("budget_income_new_blg", &fz_budget_income_new_blg, 0.1, FLI_SYM_OUTPUT); 
    ai->m_fliEngine->AddSymbol("budget_income_wages", &fz_budget_income_wages, 0.40, FLI_SYM_OUTPUT); 
    ai->m_fliEngine->AddSymbol("budget_income_science", &fz_budget_income_science, 0.5, FLI_SYM_OUTPUT); 
    ai->m_fliEngine->AddSymbol("budget_income_savings", &fz_budget_income_savings, 0.0, FLI_SYM_OUTPUT); 
    ai->m_fliEngine->AddSymbol("budget_savings_diplomacy", &fz_budget_savings_diplomacy, 0.33, FLI_SYM_OUTPUT); 
    ai->m_fliEngine->AddSymbol("budget_savings_overtime", &fz_budget_savings_overtime, 0.33, FLI_SYM_OUTPUT); 
    ai->m_fliEngine->AddSymbol("budget_savings_reserve", &fz_budget_savings_reserve, 0.33, FLI_SYM_OUTPUT); 

    ai->m_fliEngine->AddSymbol("rushbuy_max_utility", &fz_rushbuy_max_utility, 150.0, FLI_SYM_OUTPUT); 
    ai->m_fliEngine->AddSymbol("my_current_savings", &fz_my_current_savings, 0.0, FLI_SYM_INPUT); 

    ai->m_fliEngine->AddSymbol("percent_wages_needed_content", &fz_percent_wages_needed_content, 0.5, FLI_SYM_INPUT); 

    ai->m_fliEngine->AddSymbol("income_expense_cleric", &fz_income_expense_cleric, 0.0, FLI_SYM_INPUT); 
    ai->m_fliEngine->AddSymbol("income_expense_crime", &fz_income_expense_crime, 0.0, FLI_SYM_INPUT); 
    ai->m_fliEngine->AddSymbol("income_expense_maintenance", &fz_income_expense_maintenance, 0.0, FLI_SYM_INPUT); 
    ai->m_fliEngine->AddSymbol("income_expense_wages", &fz_income_expense_wages, 0.0, FLI_SYM_INPUT); 
    ai->m_fliEngine->AddSymbol("income_expense_science", &fz_income_expense_science, 0.0, FLI_SYM_INPUT); 
    ai->m_fliEngine->AddSymbol("income_expense_savings", &fz_income_expense_savings, 0.0, FLI_SYM_INPUT); 
    ai->m_fliEngine->AddSymbol("gold_and_wage_percent_cost", &fz_gold_and_wage_percent_cost, 0.0, FLI_SYM_INPUT); 

    ai->m_fliEngine->AddSymbol("save_expense_overtime", &fz_save_expense_overtime, 0.0, FLI_SYM_INPUT); 
    ai->m_fliEngine->AddSymbol("save_expense_diplomacy", &fz_save_expense_diplomacy, 0.0, FLI_SYM_INPUT); 

    ai->m_fliEngine->AddSymbol("base_gold_reserve_multiple", &fz_base_gold_reserve_multiple, 0.0, FLI_SYM_INPUT); 
    ai->m_fliEngine->AddSymbol("special_action_gold_reserve_multiple", &fz_special_action_gold_reserve_multiple, 0.0, FLI_SYM_INPUT); 



    ai->m_fliEngine->AddSymbol("city_growth_ratio", &fz_city_growth_ratio, 0.5, FLI_SYM_OUTPUT); 
    ai->m_fliEngine->AddSymbol("city_growth_min_percent", &fz_city_growth_min_percent, 0.5, FLI_SYM_OUTPUT); 

    ai->m_fliEngine->AddSymbol("city_prod_ratio", &fz_city_prod_ratio, 0.5, FLI_SYM_OUTPUT); 
    ai->m_fliEngine->AddSymbol("city_prod_min_percent", &fz_city_prod_min_percent, 0.5, FLI_SYM_OUTPUT); 

    ai->m_fliEngine->AddSymbol("city_fp_ratio", &fz_city_fp_ratio, 0.5, FLI_SYM_OUTPUT); 
    ai->m_fliEngine->AddSymbol("city_fp_min_percent", &fz_city_fp_min_percent, 0.5, FLI_SYM_OUTPUT); 


    ai->m_fliEngine->AddSymbol("num_cities", &fz_num_cities, 0.0, FLI_SYM_INPUT); 
    ai->m_fliEngine->AddSymbol("max_cities_before_penalty", &fz_max_cities_before_penalty, 0.0, FLI_SYM_INPUT); 
    ai->m_fliEngine->AddSymbol("city_delta", &fz_city_delta, 0.0, FLI_SYM_INPUT); 
    ai->m_fliEngine->AddSymbol("most_human_city_delta", &fz_most_human_city_delta, 0.0, FLI_SYM_INPUT); 

    ai->m_fliEngine->AddSymbol("wgf_stupid", &fz_wgf_stupid, 50.0, FLI_SYM_OUTPUT); 

    
    ai->m_fliEngine->AddSymbol("blg_uscale", &fz_blg_uscale, 1.0, FLI_SYM_OUTPUT); 

    ai->m_fliEngine->AddSymbol("blg_effect_silo", &fz_blg_effect_silo, 1.0, FLI_SYM_OUTPUT); 
    ai->m_fliEngine->AddSymbol("blg_effect_capitol", &fz_blg_effect_capitol, 1.0, FLI_SYM_OUTPUT); 
    ai->m_fliEngine->AddSymbol("blg_effect_defense_bonus", &fz_blg_effect_defense_bonus, 1.0, FLI_SYM_OUTPUT); 

    ai->m_fliEngine->AddSymbol("blg_effect_defense_bonus_no_city_walls_attacker", &fz_blg_effect_defense_bonus_no_city_walls_attacker, 1.0, FLI_SYM_OUTPUT); 

    ai->m_fliEngine->AddSymbol("blg_effect_happy_increment", &fz_blg_effect_happy_increment, 1.0, FLI_SYM_OUTPUT); 

    ai->m_fliEngine->AddSymbol("blg_effect_content_aif", &fz_blg_effect_content_aif, 1.0, FLI_SYM_OUTPUT); 

    ai->m_fliEngine->AddSymbol("blg_effect_double_televan", &fz_blg_effect_double_televan, 1.0, FLI_SYM_OUTPUT); 
    ai->m_fliEngine->AddSymbol("blg_effect_reduce_waste", &fz_blg_effect_reduce_waste, 1.0, FLI_SYM_OUTPUT); 

    ai->m_fliEngine->AddSymbol("blg_effect_reduce_waste", &fz_blg_effect_reduce_waste, 1.0, FLI_SYM_OUTPUT); 

    ai->m_fliEngine->AddSymbol("blg_effect_production_type_production_increment", &fz_blg_effect_production_type_production_increment, 1.0, FLI_SYM_OUTPUT); 
    ai->m_fliEngine->AddSymbol("blg_effect_production_type_food_increment", &fz_blg_effect_production_type_food_increment, 1.0, FLI_SYM_OUTPUT); 
    ai->m_fliEngine->AddSymbol("blg_effect_production_type_gold_increment", &fz_blg_effect_production_type_gold_increment, 1.0, FLI_SYM_OUTPUT); 
    ai->m_fliEngine->AddSymbol("blg_effect_production_type_knowledge_increment", &fz_blg_effect_production_type_knowledge_increment, 1.0, FLI_SYM_OUTPUT); 
    ai->m_fliEngine->AddSymbol("blg_effect_production_type_entertainment_increment", &fz_blg_effect_production_type_entertainment_increment, 1.0, FLI_SYM_OUTPUT); 

    ai->m_fliEngine->AddSymbol("blg_effect_production_type_production_percent", &fz_blg_effect_production_type_production_percent, 1.0, FLI_SYM_OUTPUT); 
    ai->m_fliEngine->AddSymbol("blg_effect_production_type_food_percent", &fz_blg_effect_production_type_food_percent, 1.0, FLI_SYM_OUTPUT); 
    ai->m_fliEngine->AddSymbol("blg_effect_production_type_gold_percent", &fz_blg_effect_production_type_gold_percent, 1.0, FLI_SYM_OUTPUT); 
    ai->m_fliEngine->AddSymbol("blg_effect_production_type_knowledge_percent", &fz_blg_effect_production_type_knowledge_percent, 1.0, FLI_SYM_OUTPUT); 
    ai->m_fliEngine->AddSymbol("blg_effect_production_type_entertainment_percent", &fz_blg_effect_production_type_entertainment_percent, 1.0, FLI_SYM_OUTPUT); 



    ai->m_fliEngine->AddSymbol("blg_effect_pollution_bonus", &fz_blg_effect_pollution_bonus, 1.0, FLI_SYM_OUTPUT); 


    ai->m_fliEngine->AddSymbol("blg_effect_pollution_cleaner", &fz_blg_effect_pollution_cleaner, 1.0, FLI_SYM_OUTPUT); 

    ai->m_fliEngine->AddSymbol("blg_effect_affective_pop_dec", &fz_blg_effect_affective_pop_dec, 1.0, FLI_SYM_OUTPUT); 
    ai->m_fliEngine->AddSymbol("blg_effect_no_unhappy_people", &fz_blg_effect_no_unhappy_people, 1.0, FLI_SYM_OUTPUT); 

    ai->m_fliEngine->AddSymbol("blg_effect_no_happiness", &fz_blg_effect_no_happiness, 1.0, FLI_SYM_OUTPUT); 

    ai->m_fliEngine->AddSymbol("blg_effect_lower_crime", &fz_blg_effect_lower_crime, 1.0, FLI_SYM_OUTPUT); 

    ai->m_fliEngine->AddSymbol("blg_effect_prevent_conver", &fz_blg_effect_prevent_conver, 1.0, FLI_SYM_OUTPUT); 

    
    ai->m_fliEngine->AddSymbol("blg_effect_prevent_conver", &fz_blg_effect_prevent_conver, 1.0, FLI_SYM_OUTPUT); 

    ai->m_fliEngine->AddSymbol("blg_effect_prevent_slavery", &fz_blg_effect_prevent_slavery, 1.0, FLI_SYM_OUTPUT); 
    ai->m_fliEngine->AddSymbol("blg_effect_lower_overcrowding", &fz_blg_effect_lower_overcrowding, 1.0, FLI_SYM_OUTPUT); 
    ai->m_fliEngine->AddSymbol("blg_effect_allow_grunts", &fz_blg_effect_allow_grunts, 1.0, FLI_SYM_OUTPUT); 
    ai->m_fliEngine->AddSymbol("blg_effect_lower_peace_move", &fz_blg_effect_lower_peace_move, 1.0, FLI_SYM_OUTPUT); 
    ai->m_fliEngine->AddSymbol("blg_effect_gold_per_citizen", &fz_blg_effect_gold_per_citizen, 1.0, FLI_SYM_OUTPUT); 
    ai->m_fliEngine->AddSymbol("blg_effect_protect_from_nukes", &fz_blg_effect_protect_from_nukes, 1.0, FLI_SYM_OUTPUT); 
    ai->m_fliEngine->AddSymbol("blg_effect_protect_from_bio", &fz_blg_effect_protect_from_bio, 1.0, FLI_SYM_OUTPUT); 
 
    ai->m_fliEngine->AddSymbol("blg_effect_protect_from_nano", &fz_blg_effect_protect_from_nano, 1.0, FLI_SYM_OUTPUT); 
    ai->m_fliEngine->AddSymbol("blg_effect_lower_pop_pollut", &fz_blg_effect_lower_pop_pollut, 1.0, FLI_SYM_OUTPUT); 
    ai->m_fliEngine->AddSymbol("blg_effect_television", &fz_blg_effect_television, 1.0, FLI_SYM_OUTPUT); 
    ai->m_fliEngine->AddSymbol("blg_effect_city_wall", &fz_blg_effect_city_wall, 1.0, FLI_SYM_OUTPUT); 
    ai->m_fliEngine->AddSymbol("blg_effect_airport", &fz_blg_effect_airport, 1.0, FLI_SYM_OUTPUT); 
    ai->m_fliEngine->AddSymbol("blg_effect_space_launch", &fz_blg_effect_space_launch, 1.0, FLI_SYM_OUTPUT); 
    
	
    
    ai->m_fliEngine->AddSymbol("wonder_uscale", &fz_wonder_uscale, 1000.0, FLI_SYM_OUTPUT); 
    ai->m_fliEngine->AddSymbol("wonder_effect_embassy", &fz_wonder_effect_embassy, 1.0, FLI_SYM_OUTPUT); 
    ai->m_fliEngine->AddSymbol("wonder_effect_close_embassy", &fz_wonder_effect_close_embassy, 1.0, FLI_SYM_OUTPUT); 
    ai->m_fliEngine->AddSymbol("wonder_effect_reform_cities", &fz_wonder_effect_reform_cities, 1.0, FLI_SYM_OUTPUT); 
    ai->m_fliEngine->AddSymbol("wonder_effect_free_slaves", &fz_wonder_effect_free_slaves, 1.0, FLI_SYM_OUTPUT); 
    ai->m_fliEngine->AddSymbol("wonder_effect_global_radar", &fz_wonder_effect_global_radar, 1.0, FLI_SYM_OUTPUT); 
    ai->m_fliEngine->AddSymbol("wonder_effect_spies_everywhere", &fz_wonder_effect_spies_everywhere, 1.0, FLI_SYM_OUTPUT); 
    ai->m_fliEngine->AddSymbol("wonder_effect_free_space_trans", &fz_wonder_effect_free_space_trans, 1.0, FLI_SYM_OUTPUT); 
    ai->m_fliEngine->AddSymbol("wonder_effect_stop_biowar", &fz_wonder_effect_stop_biowar, 1.0, FLI_SYM_OUTPUT); 
    ai->m_fliEngine->AddSymbol("wonder_effect_pop_monitor", &fz_wonder_effect_pop_monitor, 1.0, FLI_SYM_OUTPUT); 
    ai->m_fliEngine->AddSymbol("wonder_effect_enable_parkranger", &fz_wonder_effect_enable_parkranger, 1.0, FLI_SYM_OUTPUT); 

    ai->m_fliEngine->AddSymbol("wonder_effect_disable_nukes", &fz_wonder_effect_disable_nukes, 1.0, FLI_SYM_OUTPUT); 
    ai->m_fliEngine->AddSymbol("wonder_effect_reduce_readiness_cost", &fz_wonder_effect_reduce_readiness_cost, 1.0, FLI_SYM_OUTPUT); 
    ai->m_fliEngine->AddSymbol("wonder_effect_increase_gold", &fz_wonder_effect_increase_gold, 1.0, FLI_SYM_OUTPUT); 
    ai->m_fliEngine->AddSymbol("wonder_effect_decrease_crime", &fz_wonder_effect_decrease_crime, 1.0, FLI_SYM_OUTPUT); 

    ai->m_fliEngine->AddSymbol("wonder_effect_increase_knowledge", &fz_wonder_effect_increase_knowledge, 1.0, FLI_SYM_OUTPUT); 
    ai->m_fliEngine->AddSymbol("wonder_effect_decrease_empire_size", &fz_wonder_effect_decrease_empire_size, 1.0, FLI_SYM_OUTPUT); 
    ai->m_fliEngine->AddSymbol("wonder_effect_increase_happy_empire", &fz_wonder_effect_increase_happy_empire, 1.0, FLI_SYM_OUTPUT); 
    ai->m_fliEngine->AddSymbol("wonder_effect_increase_converted_city_fee", &fz_wonder_effect_increase_converted_city_fee, 1.0, FLI_SYM_OUTPUT); 
    ai->m_fliEngine->AddSymbol("wonder_effect_tax_water_routes", &fz_wonder_effect_tax_water_routes, 1.0, FLI_SYM_OUTPUT); 
    ai->m_fliEngine->AddSymbol("wonder_effect_tax_television", &fz_wonder_effect_tax_television, 1.0, FLI_SYM_OUTPUT); 

    ai->m_fliEngine->AddSymbol("wonder_effect_increase_happy", &fz_wonder_effect_increase_happy, 1.0, FLI_SYM_OUTPUT); 
    ai->m_fliEngine->AddSymbol("wonder_effect_make_genetics_cheap", &fz_wonder_effect_make_genetics_cheap, 1.0, FLI_SYM_OUTPUT); 

    ai->m_fliEngine->AddSymbol("wonder_effect_tax_international_route", &fz_wonder_effect_tax_international_route, 1.0, FLI_SYM_OUTPUT); 
    ai->m_fliEngine->AddSymbol("wonder_effect_polluters_to_parks", &fz_wonder_effect_polluters_to_parks, 1.0, FLI_SYM_OUTPUT); 

    ai->m_fliEngine->AddSymbol("wonder_effect_reduce_world_pollution", &fz_wonder_effect_reduce_world_pollution, 1.0, FLI_SYM_OUTPUT); 


    ai->m_fliEngine->AddSymbol("wonder_effect_pollution_bonus", &fz_wonder_effect_pollution_bonus, 1.0, FLI_SYM_OUTPUT); 
    ai->m_fliEngine->AddSymbol("wonder_effect_pollution_cleaner", &fz_wonder_effect_pollution_cleaner, 1.0, FLI_SYM_OUTPUT); 
    
	

    
     ai->m_fliEngine->AddSymbol("sci_unit_mil", &fz_sci_unit_mil, 1.0, FLI_SYM_OUTPUT); 
     ai->m_fliEngine->AddSymbol("sci_unit_specialability ", &fz_sci_unit_specialability, 1.0, FLI_SYM_OUTPUT); 
     ai->m_fliEngine->AddSymbol("sci_unit_moveair", &fz_sci_unit_moveair, 1.0, FLI_SYM_OUTPUT); 
     ai->m_fliEngine->AddSymbol("sci_unit_movewater", &fz_sci_unit_movewater, 1.0, FLI_SYM_OUTPUT); 
     ai->m_fliEngine->AddSymbol("sci_unit_movespace", &fz_sci_unit_movespace, 1.0, FLI_SYM_OUTPUT); 
     ai->m_fliEngine->AddSymbol("sci_unit_settlewater", &fz_sci_unit_settlewater, 1.0, FLI_SYM_OUTPUT); 
     ai->m_fliEngine->AddSymbol("sci_unit_settlespace", &fz_sci_unit_settlespace, 1.0, FLI_SYM_OUTPUT); 
     ai->m_fliEngine->AddSymbol("sci_unit_nuke", &fz_sci_unit_nuke, 1.0, FLI_SYM_OUTPUT); 
     ai->m_fliEngine->AddSymbol("sci_unit_caravan", &fz_sci_unit_caravan, 1.0, FLI_SYM_OUTPUT); 

     ai->m_fliEngine->AddSymbol("sci_inst_move", &fz_sci_inst_move, 1.0, FLI_SYM_OUTPUT); 
     ai->m_fliEngine->AddSymbol("sci_inst_def", &fz_sci_inst_def, 1.0, FLI_SYM_OUTPUT); 
     ai->m_fliEngine->AddSymbol("sci_inst_food", &fz_sci_inst_food, 1.0, FLI_SYM_OUTPUT); 
     ai->m_fliEngine->AddSymbol("sci_inst_prod", &fz_sci_inst_prod, 1.0, FLI_SYM_OUTPUT); 
     ai->m_fliEngine->AddSymbol("sci_inst_vis", &fz_sci_inst_vis, 1.0, FLI_SYM_OUTPUT); 

     ai->m_fliEngine->AddSymbol("sci_blg_def", &fz_sci_blg_def, 1.0, FLI_SYM_OUTPUT); 
     ai->m_fliEngine->AddSymbol("sci_blg_food", &fz_sci_blg_food, 1.0, FLI_SYM_OUTPUT); 
     ai->m_fliEngine->AddSymbol("sci_blg_prod", &fz_sci_blg_prod, 1.0, FLI_SYM_OUTPUT); 
     ai->m_fliEngine->AddSymbol("sci_blg_sci", &fz_sci_blg_sci, 1.0, FLI_SYM_OUTPUT); 
     ai->m_fliEngine->AddSymbol("sci_blg_gold", &fz_sci_blg_gold, 1.0, FLI_SYM_OUTPUT); 
     ai->m_fliEngine->AddSymbol("sci_blg_happy", &fz_sci_blg_happy, 1.0, FLI_SYM_OUTPUT); 

     ai->m_fliEngine->AddSymbol("sci_wndr_obs_his", &fz_sci_wndr_obs_his, 1.0, FLI_SYM_OUTPUT); 
     ai->m_fliEngine->AddSymbol("sci_wndr_obs_mine", &fz_sci_wndr_obs_mine, 1.0, FLI_SYM_OUTPUT); 

     ai->m_fliEngine->AddSymbol("sci_tech_end_game", &fz_sci_tech_end_game, 1.0, FLI_SYM_OUTPUT); 
     ai->m_fliEngine->AddSymbol("i_can_build_wormhole_probe", &fz_i_can_build_wormhole_probe, 0.0, FLI_SYM_INPUT); 

     ai->m_fliEngine->AddSymbol("sci_gov_workday", &fz_sci_gov_workday, 1.0, FLI_SYM_OUTPUT); 
     ai->m_fliEngine->AddSymbol("sci_gov_wages", &fz_sci_gov_wages, 1.0, FLI_SYM_OUTPUT); 
     ai->m_fliEngine->AddSymbol("sci_gov_rations", &fz_sci_gov_rations, 1.0, FLI_SYM_OUTPUT); 
     ai->m_fliEngine->AddSymbol("sci_gov_sci", &fz_sci_gov_sci, 1.0, FLI_SYM_OUTPUT); 
     ai->m_fliEngine->AddSymbol("sci_gov_poll", &fz_sci_gov_poll, 1.0, FLI_SYM_OUTPUT); 
     ai->m_fliEngine->AddSymbol("sci_gov_poll_unhap", &fz_sci_gov_poll_unhap, 1.0, FLI_SYM_OUTPUT); 
     ai->m_fliEngine->AddSymbol("sci_gov_emp_size", &fz_sci_gov_emp_size, 1.0, FLI_SYM_OUTPUT); 
     ai->m_fliEngine->AddSymbol("sci_gov_crime", &fz_sci_gov_crime, 1.0, FLI_SYM_OUTPUT); 
     ai->m_fliEngine->AddSymbol("sci_gov_martial_law", &fz_sci_gov_martial_law, 1.0, FLI_SYM_OUTPUT); 
     ai->m_fliEngine->AddSymbol("sci_gov_combat_moral", &fz_sci_gov_combat_moral, 1.0, FLI_SYM_OUTPUT); 
     ai->m_fliEngine->AddSymbol("sci_gov_readiness", &fz_sci_gov_readiness, 1.0, FLI_SYM_OUTPUT); 

     ai->m_fliEngine->AddSymbol("sci_gov_unit_dist", &fz_sci_gov_unit_dist, 1.0, FLI_SYM_OUTPUT); 
     ai->m_fliEngine->AddSymbol("sci_gov_gold", &fz_sci_gov_gold, 1.0, FLI_SYM_OUTPUT); 

     ai->m_fliEngine->AddSymbol("sci_plantime", &fz_sci_plantime, 3.0, FLI_SYM_OUTPUT); 
     ai->m_fliEngine->AddSymbol("sci_ave_turns_to_advance", &fz_sci_ave_turns_to_advance, 1.0, FLI_SYM_INPUT); 
     ai->m_fliEngine->AddSymbol("sci_turns_to_current_advance", &fz_sci_turns_to_current_advance, 0.0, FLI_SYM_INPUT); 
     ai->m_fliEngine->AddSymbol("max_time_to_research", &fz_max_time_to_research, 100.0, FLI_SYM_OUTPUT); 

     ai->m_fliEngine->AddSymbol("ratio_advance_cost_me_to_best_human", &fz_ratio_advance_cost_me_to_best_human, 1.0, FLI_SYM_INPUT); 
     ai->m_fliEngine->AddSymbol("ratio_advance_cost_me_to_best_other", &fz_ratio_advance_cost_me_to_best_other, 1.0, FLI_SYM_INPUT); 

     ai->m_fliEngine->AddSymbol("can_space_settle", &fz_can_space_settle, 0.0, FLI_SYM_INPUT); 
     ai->m_fliEngine->AddSymbol("can_sea_settle", &fz_can_sea_settle, 0.0, FLI_SYM_INPUT); 

	

    ai->m_fliEngine->AddSymbol("build_time_decay", &fz_build_time_decay, 0.999, FLI_SYM_OUTPUT); 
    ai->m_fliEngine->AddSymbol("ave_turns_new_pop", &fz_ave_turns_new_pop, 1.0, FLI_SYM_OUTPUT); 
    ai->m_fliEngine->AddSymbol("min_turns_new_pop", &fz_min_turns_new_pop, 1.0, FLI_SYM_OUTPUT); 
	
	
	ai->m_fliEngine->AddSymbol("war_with_him", &fz_war_with_him, 0, FLI_SYM_INPUT);
	ai->m_fliEngine->AddSymbol("cease_fire_with_him", &fz_cease_fire_with_him, 0, FLI_SYM_INPUT);
	ai->m_fliEngine->AddSymbol("alliance_with_him", &fz_alliance_with_him, 0, FLI_SYM_INPUT);
	ai->m_fliEngine->AddSymbol("neutral_with_him", &fz_neutral_with_him, 0, FLI_SYM_INPUT);
	
	ai->m_fliEngine->AddSymbol("his_message_tone", &fz_his_message_tone, 2, FLI_SYM_INPUT);
	ai->m_fliEngine->AddSymbol("accept_threshold", &fz_accept_threshold, 500, FLI_SYM_OUTPUT);
	
	
	
	ai->m_fliEngine->AddSymbol("his_coldwar_attacked_me", &fz_his_coldwar_attacked_me, 0, FLI_SYM_INPUT);
	ai->m_fliEngine->AddSymbol("his_hotwar_attacked_me", &fz_his_hotwar_attacked_me, 0, FLI_SYM_INPUT);
	ai->m_fliEngine->AddSymbol("send_threshold", &fz_send_threshold, 0, FLI_SYM_OUTPUT);
	ai->m_fliEngine->AddSymbol("send_tone", &fz_send_tone, 0, FLI_SYM_OUTPUT);
	ai->m_fliEngine->AddSymbol("contact_gained", &fz_contact_gained, 0, FLI_SYM_INPUT);
	ai->m_fliEngine->AddSymbol("gold_gained", &fz_gold_gained, 0, FLI_SYM_INPUT);
	ai->m_fliEngine->AddSymbol("gold_lost", &fz_gold_lost, 0, FLI_SYM_INPUT);
	ai->m_fliEngine->AddSymbol("gold_utility", &fz_gold_utility, 0, FLI_SYM_OUTPUT);
	ai->m_fliEngine->AddSymbol("military_incursions", &fz_military_incursions, 0, FLI_SYM_INPUT);
	ai->m_fliEngine->AddSymbol("reduces_incursions", &fz_reduces_incursions, 0, FLI_SYM_INPUT);
	ai->m_fliEngine->AddSymbol("my_military_incursions", &fz_my_military_incursions, 0, FLI_SYM_INPUT);
	ai->m_fliEngine->AddSymbol("advances_skipped", &fz_advances_skipped, 0, FLI_SYM_INPUT);
	ai->m_fliEngine->AddSymbol("advance_cost", &fz_advance_cost, 0, FLI_SYM_INPUT);
	ai->m_fliEngine->AddSymbol("demand_advance", &fz_demand_advance, 0, FLI_SYM_OUTPUT);
	ai->m_fliEngine->AddSymbol("demand_city", &fz_demand_city, 0, FLI_SYM_OUTPUT);
	ai->m_fliEngine->AddSymbol("projected_food", &fz_projected_food, 0, FLI_SYM_INPUT);
	ai->m_fliEngine->AddSymbol("projected_production", &fz_projected_production, 0, FLI_SYM_INPUT);
	ai->m_fliEngine->AddSymbol("map_area_gained", &fz_map_area_gained, 0, FLI_SYM_INPUT);
	ai->m_fliEngine->AddSymbol("last_got_map", &fz_last_got_map, 0, FLI_SYM_INPUT);
	ai->m_fliEngine->AddSymbol("diplomatic_persistence", &fz_diplomatic_persistence, 0, FLI_SYM_OUTPUT);
	ai->m_fliEngine->AddSymbol("rarely_accept_message", &fz_max_messages_considered_per_turn, 23, FLI_SYM_OUTPUT);
	ai->m_fliEngine->AddSymbol("diplomatic_regard_towards_him", &fz_diplomatic_regard_towards_him, 50, FLI_SYM_INPUT);
	ai->m_fliEngine->AddSymbol("loyalty", &fz_loyalty, 50, FLI_SYM_INPUT);
	ai->m_fliEngine->AddSymbol("loyalty_modifier", &fz_loyalty_modifier, 1, FLI_SYM_OUTPUT);

	ai->m_fliEngine->AddSymbol("dip_regard_pirate_delta", &fz_dip_regard_pirate_delta, 0.0, FLI_SYM_OUTPUT);
	ai->m_fliEngine->AddSymbol("dip_regard_attack_delta", &fz_dip_regard_attack_delta, 0.0, FLI_SYM_OUTPUT); 
	ai->m_fliEngine->AddSymbol("dip_regard_incursion_delta", &fz_dip_regard_incursion_delta, 0.0, FLI_SYM_OUTPUT);
	ai->m_fliEngine->AddSymbol("dip_regard_pillage_delta", &fz_dip_regard_pillage_delta, 0.0, FLI_SYM_OUTPUT);
	ai->m_fliEngine->AddSymbol("dip_regard_message_delta", &fz_dip_regard_message_delta, 0.0, FLI_SYM_OUTPUT);
	ai->m_fliEngine->AddSymbol("dip_regard_general_delta", &fz_dip_regard_general_delta, 0.0, FLI_SYM_OUTPUT);
	ai->m_fliEngine->AddSymbol("dip_regard_strength_delta", &fz_dip_regard_strength_delta, 0.0, FLI_SYM_OUTPUT);
	
	ai->m_fliEngine->AddSymbol("player_type", &fz_player_type, 3, FLI_SYM_INPUT);
	ai->m_fliEngine->AddSymbol("chase_the_rabbit", &fz_chase_the_rabbit, 0, FLI_SYM_INPUT);
	ai->m_fliEngine->AddSymbol("unoccupied_humans", &fz_unoccupied_humans, 0, FLI_SYM_INPUT);

	ai->m_fliEngine->AddSymbol("unit_to_city_ratio", &fz_unit_to_city_ratio, 0, FLI_SYM_INPUT);

	ai->m_fliEngine->AddSymbol("most_human_adv_known_delta", &fz_most_human_adv_known_delta, 0, FLI_SYM_INPUT);

	

	ai->m_fliEngine->AddSymbol("he_accepted_greetings", &fz_he_accepted_greetings, 0, FLI_SYM_INPUT);
	ai->m_fliEngine->AddSymbol("he_accepted_gold_demand", &fz_he_accepted_gold_demand, 0, FLI_SYM_INPUT);
	ai->m_fliEngine->AddSymbol("he_accepted_demand_map", &fz_he_accepted_demand_map, 0, FLI_SYM_INPUT);
	ai->m_fliEngine->AddSymbol("he_accepted_city_demand", &fz_he_accepted_city_demand, 0, FLI_SYM_INPUT);
	ai->m_fliEngine->AddSymbol("he_accepted_demand_advance", &fz_he_accepted_demand_advance, 0, FLI_SYM_INPUT);
	ai->m_fliEngine->AddSymbol("he_accepted_demand_stop_trade", &fz_he_accepted_demand_stop_trade, 0, FLI_SYM_INPUT);
	ai->m_fliEngine->AddSymbol("he_accepted_demand_attack_enemy", &fz_he_accepted_demand_attack_enemy, 0, FLI_SYM_INPUT);
	ai->m_fliEngine->AddSymbol("he_accepted_demand_leave_our_lands", &fz_he_accepted_demand_leave_our_lands, 0, FLI_SYM_INPUT);
	ai->m_fliEngine->AddSymbol("he_accepted_demand_reduce_pollution", &fz_he_accepted_demand_reduce_pollution, 0, FLI_SYM_INPUT);
	ai->m_fliEngine->AddSymbol("he_accepted_offer_advance", &fz_he_accepted_offer_advance, 0, FLI_SYM_INPUT);
	ai->m_fliEngine->AddSymbol("he_accepted_offer_city", &fz_he_accepted_offer_city, 0, FLI_SYM_INPUT);
	ai->m_fliEngine->AddSymbol("he_accepted_offer_map", &fz_he_accepted_offer_map, 0, FLI_SYM_INPUT);
	ai->m_fliEngine->AddSymbol("he_accepted_offer_gold", &fz_he_accepted_offer_gold, 0, FLI_SYM_INPUT);
	ai->m_fliEngine->AddSymbol("he_accepted_offer_cease_fire", &fz_he_accepted_offer_cease_fire, 0, FLI_SYM_INPUT);
	ai->m_fliEngine->AddSymbol("he_accepted_offer_alliance", &fz_he_accepted_offer_alliance, 0, FLI_SYM_INPUT);
	ai->m_fliEngine->AddSymbol("he_accepted_offer_capture_city", &fz_he_accepted_offer_capture_city, 0, FLI_SYM_INPUT);
	ai->m_fliEngine->AddSymbol("he_accepted_offer_pact_end_pollution", &fz_he_accepted_offer_pact_end_pollution, 0, FLI_SYM_INPUT);

	ai->m_fliEngine->AddSymbol("he_accepted_exchange_advance", &fz_he_accepted_exchange_advance, 0, FLI_SYM_INPUT);
	ai->m_fliEngine->AddSymbol("he_accepted_exchange_city", &fz_he_accepted_exchange_city, 0, FLI_SYM_INPUT);
	ai->m_fliEngine->AddSymbol("he_accepted_exchange_map", &fz_he_accepted_exchange_map, 0, FLI_SYM_INPUT);
	ai->m_fliEngine->AddSymbol("he_accepted_demand_no_piracy", &fz_he_accepted_demand_no_piracy, 0, FLI_SYM_INPUT);


	ai->m_fliEngine->AddSymbol("he_rejected_greetings", &fz_he_rejected_greetings, 0, FLI_SYM_INPUT);
	ai->m_fliEngine->AddSymbol("he_rejected_gold_demand", &fz_he_rejected_gold_demand, 0, FLI_SYM_INPUT);
	ai->m_fliEngine->AddSymbol("he_rejected_demand_map", &fz_he_rejected_demand_map, 0, FLI_SYM_INPUT);
	ai->m_fliEngine->AddSymbol("he_rejected_city_demand", &fz_he_rejected_city_demand, 0, FLI_SYM_INPUT);
	ai->m_fliEngine->AddSymbol("he_rejected_demand_advance", &fz_he_rejected_demand_advance, 0, FLI_SYM_INPUT);
	ai->m_fliEngine->AddSymbol("he_rejected_demand_stop_trade", &fz_he_rejected_demand_stop_trade, 0, FLI_SYM_INPUT);
	ai->m_fliEngine->AddSymbol("he_rejected_demand_attack_enemy", &fz_he_rejected_demand_attack_enemy, 0, FLI_SYM_INPUT);
	ai->m_fliEngine->AddSymbol("he_rejected_demand_leave_our_lands", &fz_he_rejected_demand_leave_our_lands, 0, FLI_SYM_INPUT);
	ai->m_fliEngine->AddSymbol("he_rejected_demand_reduce_pollution", &fz_he_rejected_demand_reduce_pollution, 0, FLI_SYM_INPUT);
	ai->m_fliEngine->AddSymbol("he_rejected_offer_advance", &fz_he_rejected_offer_advance, 0, FLI_SYM_INPUT);
	ai->m_fliEngine->AddSymbol("he_rejected_offer_city", &fz_he_rejected_offer_city, 0, FLI_SYM_INPUT);
	ai->m_fliEngine->AddSymbol("he_rejected_offer_map", &fz_he_rejected_offer_map, 0, FLI_SYM_INPUT);
	ai->m_fliEngine->AddSymbol("he_rejected_offer_gold", &fz_he_rejected_offer_gold, 0, FLI_SYM_INPUT);
	ai->m_fliEngine->AddSymbol("he_rejected_offer_cease_fire", &fz_he_rejected_offer_cease_fire, 0, FLI_SYM_INPUT);
	ai->m_fliEngine->AddSymbol("he_rejected_offer_alliance", &fz_he_rejected_offer_alliance, 0, FLI_SYM_INPUT);
	ai->m_fliEngine->AddSymbol("he_rejected_offer_capture_city", &fz_he_rejected_offer_capture_city, 0, FLI_SYM_INPUT);
	ai->m_fliEngine->AddSymbol("he_rejected_offer_pact_end_pollution", &fz_he_rejected_offer_pact_end_pollution, 0, FLI_SYM_INPUT);

	ai->m_fliEngine->AddSymbol("he_rejected_exchange_advance", &fz_he_rejected_exchange_advance, 0, FLI_SYM_INPUT);
	ai->m_fliEngine->AddSymbol("he_rejected_exchange_city", &fz_he_rejected_exchange_city, 0, FLI_SYM_INPUT);
	ai->m_fliEngine->AddSymbol("he_rejected_exchange_map", &fz_he_rejected_exchange_map, 0, FLI_SYM_INPUT);
	ai->m_fliEngine->AddSymbol("he_rejected_demand_no_piracy", &fz_he_rejected_demand_no_piracy, 0, FLI_SYM_INPUT);

	ai->m_fliEngine->AddSymbol("rejected_requests", &fz_rejected_requests, 0, FLI_SYM_INPUT);
	ai->m_fliEngine->AddSymbol("accepted_requests", &fz_accepted_requests, 0, FLI_SYM_INPUT);
	ai->m_fliEngine->AddSymbol("last_attacked_me", &fz_last_attacked_me, 0, FLI_SYM_INPUT);
	ai->m_fliEngine->AddSymbol("diplomatic_regard_delta", &fz_diplomatic_regard_delta, 0.0, FLI_SYM_OUTPUT);
	ai->m_fliEngine->AddSymbol("my_strength", &fz_my_strength, 0, FLI_SYM_INPUT);
	ai->m_fliEngine->AddSymbol("his_strength", &fz_his_strength, 0, FLI_SYM_INPUT);
	ai->m_fliEngine->AddSymbol("third_party_strength", &fz_third_party_strength, 0, FLI_SYM_INPUT);
	ai->m_fliEngine->AddSymbol("third_party_relative_strength", &fz_third_party_relative_strength, 0, FLI_SYM_INPUT);
	ai->m_fliEngine->AddSymbol("his_third_party_relative_strength", &fz_his_third_party_relative_strength, 0, FLI_SYM_INPUT);
	ai->m_fliEngine->AddSymbol("third_party_regard", &fz_third_party_regard, 0, FLI_SYM_INPUT);
	ai->m_fliEngine->AddSymbol("at_war_with_third_party", &fz_at_war_with_third_party, 0.0, FLI_SYM_INPUT);

	ai->m_fliEngine->AddSymbol("his_trade_with_third_party", &fz_his_trade_with_third_party, 0, FLI_SYM_INPUT);
	ai->m_fliEngine->AddSymbol("stop_trade", &fz_stop_trade, 0, FLI_SYM_OUTPUT);
	ai->m_fliEngine->AddSymbol("demand_attack_enemy", &fz_demand_attack_enemy, 0, FLI_SYM_OUTPUT);
	ai->m_fliEngine->AddSymbol("demand_leave_our_lands", &fz_demand_leave_our_lands, 0, FLI_SYM_OUTPUT);

	ai->m_fliEngine->AddSymbol("send_strength", &fz_send_strength, 0, FLI_SYM_OUTPUT);

	ai->m_fliEngine->AddSymbol("i_agreed_to_stop_polluting", &fz_i_agreed_to_stop_polluting, 
		0, FLI_SYM_INPUT);
	ai->m_fliEngine->AddSymbol("i_want_to_stop_others_who_pollute", &fz_i_want_to_stop_others_who_pollute, 
		0, FLI_SYM_INPUT);
	
	
	ai->m_fliEngine->AddSymbol("his_pollution_level", &fz_his_pollution_level, 0, FLI_SYM_INPUT);
	ai->m_fliEngine->AddSymbol("my_pollution_level", &fz_my_pollution_level, 0, FLI_SYM_INPUT);
	ai->m_fliEngine->AddSymbol("global_pollution_level", &fz_global_pollution_level, 0, FLI_SYM_INPUT);
	ai->m_fliEngine->AddSymbol("he_stops_polluting", &fz_he_stops_polluting, 0, FLI_SYM_INPUT);
	ai->m_fliEngine->AddSymbol("stop_polluting_others", &fz_stop_polluting_others, 0, FLI_SYM_OUTPUT);
	ai->m_fliEngine->AddSymbol("his_route_threat", &fz_his_route_threat, 0, FLI_SYM_INPUT);
	ai->m_fliEngine->AddSymbol("he_gains_advance", &fz_he_gains_advance, 0, FLI_SYM_INPUT);
	ai->m_fliEngine->AddSymbol("advance_give", &fz_advance_give, 0, FLI_SYM_OUTPUT);
	ai->m_fliEngine->AddSymbol("offer_city_happiness_level", &fz_offer_city_happiness_level, 0, FLI_SYM_OUTPUT);
	ai->m_fliEngine->AddSymbol("projected_city_happiness", &fz_projected_city_happiness, 0, FLI_SYM_INPUT);
	ai->m_fliEngine->AddSymbol("he_gets_map", &fz_he_gets_map, 0, FLI_SYM_INPUT);
	ai->m_fliEngine->AddSymbol("he_gains_gold", &fz_he_gains_gold, 0, FLI_SYM_INPUT);
	ai->m_fliEngine->AddSymbol("gains_alliance", &fz_gains_alliance, 0, FLI_SYM_INPUT);
	ai->m_fliEngine->AddSymbol("gains_cease_fire", &fz_gains_cease_fire, 0, FLI_SYM_INPUT);
	ai->m_fliEngine->AddSymbol("end_pollution_pact", &fz_end_pollution_pact, 0, FLI_SYM_INPUT);

	ai->m_fliEngine->AddSymbol("cease_fire", &fz_cease_fire, 0, FLI_SYM_OUTPUT);
	ai->m_fliEngine->AddSymbol("alliance", &fz_alliance, 0, FLI_SYM_OUTPUT);
	ai->m_fliEngine->AddSymbol("gold_need", &fz_gold_need, 0, FLI_SYM_OUTPUT);
	ai->m_fliEngine->AddSymbol("he_gains_gold", &fz_he_gains_gold, 0, FLI_SYM_INPUT);
	ai->m_fliEngine->AddSymbol("i_gain_gold", &fz_i_gain_gold, 0, FLI_SYM_INPUT);
	ai->m_fliEngine->AddSymbol("pact_end_pollution", &fz_pact_end_pollution, 0, FLI_SYM_OUTPUT);

	ai->m_fliEngine->AddSymbol("map_exchange", &fz_map_exchange, 0, FLI_SYM_OUTPUT);
	ai->m_fliEngine->AddSymbol("food_utility", &fz_food_utility, 0, FLI_SYM_OUTPUT);
	ai->m_fliEngine->AddSymbol("production_utility", &fz_production_utility, 0, FLI_SYM_OUTPUT);
	ai->m_fliEngine->AddSymbol("trade_utility", &fz_trade_utility, 0, FLI_SYM_OUTPUT);
	ai->m_fliEngine->AddSymbol("reduce_pollution_utility", &fz_reduce_pollution_utility, 0, FLI_SYM_OUTPUT);
	ai->m_fliEngine->AddSymbol("give_advance", &fz_give_advance, 0, FLI_SYM_OUTPUT);
	ai->m_fliEngine->AddSymbol("he_attacks_enemy", &fz_he_attacks_enemy, 0, FLI_SYM_INPUT);
	ai->m_fliEngine->AddSymbol("he_stops_trade_with_third_party", &fz_he_stops_trade_with_third_party, 0, FLI_SYM_INPUT);
	ai->m_fliEngine->AddSymbol("he_gains_city", &fz_he_gains_city, 0, FLI_SYM_INPUT);
	ai->m_fliEngine->AddSymbol("i_lose_city", &fz_i_lose_city, 0, FLI_SYM_INPUT);
	ai->m_fliEngine->AddSymbol("relative_strength", &fz_relative_strength, 0, FLI_SYM_INPUT);


	ai->m_fliEngine->AddSymbol("i_will_attack_third_party", &fz_i_will_attack_third_party, 0, FLI_SYM_INPUT);
	ai->m_fliEngine->AddSymbol("stops_trade_with_third_party", &fz_stops_trade_with_third_party, 0, FLI_SYM_INPUT);
	ai->m_fliEngine->AddSymbol("i_will_leave_his_lands", &fz_i_will_leave_his_lands, 0, FLI_SYM_INPUT);
	ai->m_fliEngine->AddSymbol("reduces_pollution", &fz_reduces_pollution, 0, FLI_SYM_INPUT);
	ai->m_fliEngine->AddSymbol("i_will_stop_piracy", &fz_i_will_stop_piracy, 0, FLI_SYM_INPUT);
	ai->m_fliEngine->AddSymbol("i_gain_advance", &fz_i_gain_advance, 0, FLI_SYM_INPUT);
	ai->m_fliEngine->AddSymbol("i_gain_city", &fz_i_gain_city, 0, FLI_SYM_INPUT);
	ai->m_fliEngine->AddSymbol("gold_lost", &fz_gold_lost, 0, FLI_SYM_INPUT);
	ai->m_fliEngine->AddSymbol("i_gain_map", &fz_i_gain_map, 0, FLI_SYM_INPUT);

	ai->m_fliEngine->AddSymbol("he_will_stop_piracy", &fz_he_will_stop_piracy, 0, FLI_SYM_INPUT);
	ai->m_fliEngine->AddSymbol("his_num_trade_routes", &fz_his_num_trade_routes, 0, FLI_SYM_INPUT);
	ai->m_fliEngine->AddSymbol("his_relative_num_trade_routes", &fz_his_relative_num_trade_routes, 0, FLI_SYM_INPUT);
	ai->m_fliEngine->AddSymbol("my_num_trade_routes", &fz_my_num_trade_routes, 0, FLI_SYM_INPUT);

	
	
	
	ai->m_fliEngine->AddSymbol("GREETING_coeff", 
							   &fz_diplomatic_coeff[REQUEST_TYPE_GREETING], 
							   1, FLI_SYM_OUTPUT);
	ai->m_fliEngine->AddSymbol("DEMAND_ADVANCE_coeff", 
							   &fz_diplomatic_coeff[REQUEST_TYPE_DEMAND_ADVANCE], 
							   1, FLI_SYM_OUTPUT);
	ai->m_fliEngine->AddSymbol("DEMAND_CITY_coeff", 
							   &fz_diplomatic_coeff[REQUEST_TYPE_DEMAND_CITY], 
							   1, FLI_SYM_OUTPUT);
	ai->m_fliEngine->AddSymbol("DEMAND_MAP_coeff", 
							   &fz_diplomatic_coeff[REQUEST_TYPE_DEMAND_MAP], 
							   1, FLI_SYM_OUTPUT);
	ai->m_fliEngine->AddSymbol("DEMAND_GOLD_coeff", 
							   &fz_diplomatic_coeff[REQUEST_TYPE_DEMAND_GOLD], 
							   1, FLI_SYM_OUTPUT);
	ai->m_fliEngine->AddSymbol("DEMAND_STOP_TRADE_coeff", 
							   &fz_diplomatic_coeff[REQUEST_TYPE_DEMAND_STOP_TRADE], 
							   1, FLI_SYM_OUTPUT);
	ai->m_fliEngine->AddSymbol("DEMAND_ATTACK_ENEMY_coeff", 
							   &fz_diplomatic_coeff[REQUEST_TYPE_DEMAND_ATTACK_ENEMY], 
							   1, FLI_SYM_OUTPUT);
	ai->m_fliEngine->AddSymbol("DEMAND_LEAVE_OUR_LANDS_coeff", 
							   &fz_diplomatic_coeff[REQUEST_TYPE_DEMAND_LEAVE_OUR_LANDS], 
							   1, FLI_SYM_OUTPUT);
	ai->m_fliEngine->AddSymbol("DEMAND_REDUCE_POLLUTION_coeff", 
							   &fz_diplomatic_coeff[REQUEST_TYPE_DEMAND_REDUCE_POLLUTION], 
							   1, FLI_SYM_OUTPUT);
	ai->m_fliEngine->AddSymbol("DEMAND_LOWER_TARIFFS_coeff", 
							   &fz_diplomatic_coeff[REQUEST_TYPE_DEMAND_NO_PIRACY], 
							   1, FLI_SYM_OUTPUT);
	ai->m_fliEngine->AddSymbol("OFFER_ADVANCE_coeff", 
							   &fz_diplomatic_coeff[REQUEST_TYPE_OFFER_ADVANCE], 
							   1, FLI_SYM_OUTPUT);
	ai->m_fliEngine->AddSymbol("OFFER_CITY_coeff", 
							   &fz_diplomatic_coeff[REQUEST_TYPE_OFFER_CITY], 
							   1, FLI_SYM_OUTPUT);
	ai->m_fliEngine->AddSymbol("OFFER_MAP_coeff", 
							   &fz_diplomatic_coeff[REQUEST_TYPE_OFFER_MAP], 
							   1, FLI_SYM_OUTPUT);
	ai->m_fliEngine->AddSymbol("OFFER_GOLD_coeff", 
							   &fz_diplomatic_coeff[REQUEST_TYPE_OFFER_GOLD], 
							   1, FLI_SYM_OUTPUT);
	ai->m_fliEngine->AddSymbol("OFFER_CEASE_FIRE_coeff", 
							   &fz_diplomatic_coeff[REQUEST_TYPE_OFFER_CEASE_FIRE], 
							   1, FLI_SYM_OUTPUT);
	ai->m_fliEngine->AddSymbol("OFFER_PERMANENT_ALLIANCE_coeff", 
							   &fz_diplomatic_coeff[REQUEST_TYPE_OFFER_PERMANENT_ALLIANCE], 
							   1, FLI_SYM_OUTPUT);
	ai->m_fliEngine->AddSymbol("OFFER_PACT_CAPTURE_CITY_coeff", 
							   &fz_diplomatic_coeff[REQUEST_TYPE_OFFER_PACT_CAPTURE_CITY], 
							   1, FLI_SYM_OUTPUT);

	ai->m_fliEngine->AddSymbol("OFFER_PACT_END_POLLUTION_coeff", 
							   &fz_diplomatic_coeff[REQUEST_TYPE_OFFER_PACT_END_POLLUTION], 
							   1, FLI_SYM_OUTPUT);










	ai->m_fliEngine->AddSymbol("EXCHANGE_ADVANCE_coeff", 
							   &fz_diplomatic_coeff[REQUEST_TYPE_EXCHANGE_ADVANCE], 
							   1, FLI_SYM_OUTPUT);
	ai->m_fliEngine->AddSymbol("EXCHANGE_CITY_coeff", 
							   &fz_diplomatic_coeff[REQUEST_TYPE_EXCHANGE_CITY], 
							   1, FLI_SYM_OUTPUT);
	ai->m_fliEngine->AddSymbol("EXCHANGE_MAP_coeff", 
							   &fz_diplomatic_coeff[REQUEST_TYPE_EXCHANGE_MAP], 
							   1, FLI_SYM_OUTPUT);
	
	
	
	
	
	ai->m_fliEngine->AddSymbol("received_GREETING_coeff", 
							   &fz_diplomatic_receive_coeff[REQUEST_TYPE_GREETING], 
							   1, FLI_SYM_OUTPUT);
	ai->m_fliEngine->AddSymbol("received_DEMAND_ADVANCE_coeff", 
							   &fz_diplomatic_receive_coeff[REQUEST_TYPE_DEMAND_ADVANCE], 
							   1, FLI_SYM_OUTPUT);
	ai->m_fliEngine->AddSymbol("received_DEMAND_CITY_coeff", 
							   &fz_diplomatic_receive_coeff[REQUEST_TYPE_DEMAND_CITY], 
							   1, FLI_SYM_OUTPUT);
	ai->m_fliEngine->AddSymbol("received_DEMAND_MAP_coeff", 
							   &fz_diplomatic_receive_coeff[REQUEST_TYPE_DEMAND_MAP], 
							   1, FLI_SYM_OUTPUT);
	ai->m_fliEngine->AddSymbol("received_DEMAND_GOLD_coeff", 
							   &fz_diplomatic_receive_coeff[REQUEST_TYPE_DEMAND_GOLD], 
							   1, FLI_SYM_OUTPUT);
	ai->m_fliEngine->AddSymbol("received_DEMAND_STOP_TRADE_coeff", 
							   &fz_diplomatic_receive_coeff[REQUEST_TYPE_DEMAND_STOP_TRADE], 
							   1, FLI_SYM_OUTPUT);
	ai->m_fliEngine->AddSymbol("received_DEMAND_ATTACK_ENEMY_coeff", 
							   &fz_diplomatic_receive_coeff[REQUEST_TYPE_DEMAND_ATTACK_ENEMY], 
							   1, FLI_SYM_OUTPUT);
	ai->m_fliEngine->AddSymbol("received_DEMAND_LEAVE_OUR_LANDS_coeff", 
							   &fz_diplomatic_receive_coeff[REQUEST_TYPE_DEMAND_LEAVE_OUR_LANDS], 
							   1, FLI_SYM_OUTPUT);
	ai->m_fliEngine->AddSymbol("received_DEMAND_REDUCE_POLLUTION_coeff", 
							   &fz_diplomatic_receive_coeff[REQUEST_TYPE_DEMAND_REDUCE_POLLUTION], 
							   1, FLI_SYM_OUTPUT);
	ai->m_fliEngine->AddSymbol("received_DEMAND_LOWER_TARIFFS_coeff", 
							   &fz_diplomatic_receive_coeff[REQUEST_TYPE_DEMAND_NO_PIRACY], 
							   1, FLI_SYM_OUTPUT);
	ai->m_fliEngine->AddSymbol("received_OFFER_ADVANCE_coeff", 
							   &fz_diplomatic_receive_coeff[REQUEST_TYPE_OFFER_ADVANCE], 
							   1, FLI_SYM_OUTPUT);
	ai->m_fliEngine->AddSymbol("received_OFFER_CITY_coeff", 
							   &fz_diplomatic_receive_coeff[REQUEST_TYPE_OFFER_CITY], 
							   1, FLI_SYM_OUTPUT);
	ai->m_fliEngine->AddSymbol("received_OFFER_MAP_coeff", 
							   &fz_diplomatic_receive_coeff[REQUEST_TYPE_OFFER_MAP], 
							   1, FLI_SYM_OUTPUT);
	ai->m_fliEngine->AddSymbol("received_OFFER_GOLD_coeff", 
							   &fz_diplomatic_receive_coeff[REQUEST_TYPE_OFFER_GOLD], 
							   1, FLI_SYM_OUTPUT);
	ai->m_fliEngine->AddSymbol("received_OFFER_CEASE_FIRE_coeff", 
							   &fz_diplomatic_receive_coeff[REQUEST_TYPE_OFFER_CEASE_FIRE], 
							   1, FLI_SYM_OUTPUT);
	ai->m_fliEngine->AddSymbol("received_OFFER_PERMANENT_ALLIANCE_coeff", 
							   &fz_diplomatic_receive_coeff[REQUEST_TYPE_OFFER_PERMANENT_ALLIANCE], 
							   1, FLI_SYM_OUTPUT);
	ai->m_fliEngine->AddSymbol("received_OFFER_PACT_CAPTURE_CITY_coeff", 
							   &fz_diplomatic_receive_coeff[REQUEST_TYPE_OFFER_PACT_CAPTURE_CITY], 
							   1, FLI_SYM_OUTPUT);
	ai->m_fliEngine->AddSymbol("received_OFFER_PACT_END_POLLUTION_coeff", 
							   &fz_diplomatic_receive_coeff[REQUEST_TYPE_OFFER_PACT_END_POLLUTION], 
							   1, FLI_SYM_OUTPUT);













	ai->m_fliEngine->AddSymbol("received_EXCHANGE_ADVANCE_coeff", 
							   &fz_diplomatic_receive_coeff[REQUEST_TYPE_EXCHANGE_ADVANCE], 
							   1, FLI_SYM_OUTPUT);
	ai->m_fliEngine->AddSymbol("received_EXCHANGE_CITY_coeff", 
							   &fz_diplomatic_receive_coeff[REQUEST_TYPE_EXCHANGE_CITY], 
							   1, FLI_SYM_OUTPUT);
	ai->m_fliEngine->AddSymbol("received_EXCHANGE_MAP_coeff", 
							   &fz_diplomatic_receive_coeff[REQUEST_TYPE_EXCHANGE_MAP], 
							   1, FLI_SYM_OUTPUT);

	ai->m_fliEngine->AddSymbol("diplomacy_rand_min", &fz_diplomacy_rand_min,
							   0.75, FLI_SYM_OUTPUT);
	ai->m_fliEngine->AddSymbol("diplomacy_rand_max", &fz_diplomacy_rand_max,
							   1.25, FLI_SYM_OUTPUT);
	ai->m_fliEngine->AddSymbol("dip_send_coef", &fz_dip_send_coef,
							   1.0, FLI_SYM_OUTPUT);
	
	
	
	
	ai->m_fliEngine->AddSymbol("my_responses_DEMAND_ADVANCE", 
							   &fz_my_responses[REQUEST_TYPE_DEMAND_ADVANCE], 
							   0, FLI_SYM_INPUT);
	ai->m_fliEngine->AddSymbol("my_responses_DEMAND_CITY", 
							   &fz_my_responses[REQUEST_TYPE_DEMAND_CITY], 
							   0, FLI_SYM_INPUT);
	ai->m_fliEngine->AddSymbol("my_responses_DEMAND_MAP", 
							   &fz_my_responses[REQUEST_TYPE_DEMAND_MAP], 
							   0, FLI_SYM_INPUT);
	ai->m_fliEngine->AddSymbol("my_responses_DEMAND_GOLD", 
							   &fz_my_responses[REQUEST_TYPE_DEMAND_GOLD], 
							   0, FLI_SYM_INPUT);
	ai->m_fliEngine->AddSymbol("my_responses_DEMAND_STOP_TRADE", 
							   &fz_my_responses[REQUEST_TYPE_DEMAND_STOP_TRADE], 
							   0, FLI_SYM_INPUT);
	ai->m_fliEngine->AddSymbol("my_responses_DEMAND_ATTACK_ENEMY", 
							   &fz_my_responses[REQUEST_TYPE_DEMAND_ATTACK_ENEMY], 
							   0, FLI_SYM_INPUT);
	ai->m_fliEngine->AddSymbol("my_responses_DEMAND_LEAVE_OUR_LANDS", 
							   &fz_my_responses[REQUEST_TYPE_DEMAND_LEAVE_OUR_LANDS], 
							   0, FLI_SYM_INPUT);
	ai->m_fliEngine->AddSymbol("my_responses_DEMAND_REDUCE_POLLUTION", 
							   &fz_my_responses[REQUEST_TYPE_DEMAND_REDUCE_POLLUTION], 
							   0, FLI_SYM_INPUT);
	ai->m_fliEngine->AddSymbol("my_responses_DEMAND_LOWER_TARIFFS", 
							   &fz_my_responses[REQUEST_TYPE_DEMAND_NO_PIRACY], 
							   0, FLI_SYM_INPUT);
	ai->m_fliEngine->AddSymbol("my_responses_OFFER_ADVANCE", 
							   &fz_my_responses[REQUEST_TYPE_OFFER_ADVANCE], 
							   0, FLI_SYM_INPUT);
	ai->m_fliEngine->AddSymbol("my_responses_OFFER_CITY", 
							   &fz_my_responses[REQUEST_TYPE_OFFER_CITY], 
							   0, FLI_SYM_INPUT);
	ai->m_fliEngine->AddSymbol("my_responses_OFFER_MAP", 
							   &fz_my_responses[REQUEST_TYPE_OFFER_MAP], 
							   0, FLI_SYM_INPUT);
	ai->m_fliEngine->AddSymbol("my_responses_OFFER_GOLD", 
							   &fz_my_responses[REQUEST_TYPE_OFFER_GOLD], 
							   0, FLI_SYM_INPUT);
	ai->m_fliEngine->AddSymbol("my_responses_OFFER_CEASE_FIRE", 
							   &fz_my_responses[REQUEST_TYPE_OFFER_CEASE_FIRE], 
							   0, FLI_SYM_INPUT);
	ai->m_fliEngine->AddSymbol("my_responses_OFFER_PERMANENT_ALLIANCE", 
							   &fz_my_responses[REQUEST_TYPE_OFFER_PERMANENT_ALLIANCE], 
							   0, FLI_SYM_INPUT);
	ai->m_fliEngine->AddSymbol("my_responses_OFFER_PACT_CAPTURE_CITY", 
							   &fz_my_responses[REQUEST_TYPE_OFFER_PACT_CAPTURE_CITY], 
							   0, FLI_SYM_INPUT);
	ai->m_fliEngine->AddSymbol("my_responses_OFFER_PACT_END_POLLUTION", 
							   &fz_my_responses[REQUEST_TYPE_OFFER_PACT_END_POLLUTION], 
							   0, FLI_SYM_INPUT);
	ai->m_fliEngine->AddSymbol("my_responses_EXCHANGE_ADVANCE", 
							   &fz_my_responses[REQUEST_TYPE_EXCHANGE_ADVANCE], 
							   0, FLI_SYM_INPUT);
	ai->m_fliEngine->AddSymbol("my_responses_EXCHANGE_CITY", 
							   &fz_my_responses[REQUEST_TYPE_EXCHANGE_CITY], 
							   0, FLI_SYM_INPUT);
	ai->m_fliEngine->AddSymbol("my_responses_EXCHANGE_MAP", 
							   &fz_my_responses[REQUEST_TYPE_EXCHANGE_MAP], 
							   0, FLI_SYM_INPUT);

	
	
	

	
	
	
	
	
	
	
	ai->m_fliEngine->AddSymbol("my_responses_decay_DEMAND_ADVANCE", 
							   &fz_my_responses_decay[REQUEST_TYPE_DEMAND_ADVANCE], 
							   1, FLI_SYM_OUTPUT);
	ai->m_fliEngine->AddSymbol("my_responses_decay_DEMAND_CITY", 
							   &fz_my_responses_decay[REQUEST_TYPE_DEMAND_CITY], 
							   1, FLI_SYM_OUTPUT);
	ai->m_fliEngine->AddSymbol("my_responses_decay_DEMAND_MAP", 
							   &fz_my_responses_decay[REQUEST_TYPE_DEMAND_MAP], 
							   1, FLI_SYM_OUTPUT);
	ai->m_fliEngine->AddSymbol("my_responses_decay_DEMAND_GOLD", 
							   &fz_my_responses_decay[REQUEST_TYPE_DEMAND_GOLD], 
							   1, FLI_SYM_OUTPUT);
	ai->m_fliEngine->AddSymbol("my_responses_decay_DEMAND_STOP_TRADE", 
							   &fz_my_responses_decay[REQUEST_TYPE_DEMAND_STOP_TRADE], 
							   1, FLI_SYM_OUTPUT);
	ai->m_fliEngine->AddSymbol("my_responses_decay_DEMAND_ATTACK_ENEMY", 
							   &fz_my_responses_decay[REQUEST_TYPE_DEMAND_ATTACK_ENEMY], 
							   1, FLI_SYM_OUTPUT);
	ai->m_fliEngine->AddSymbol("my_responses_decay_DEMAND_LEAVE_OUR_LANDS", 
							   &fz_my_responses_decay[REQUEST_TYPE_DEMAND_LEAVE_OUR_LANDS], 
							   1, FLI_SYM_OUTPUT);
	ai->m_fliEngine->AddSymbol("my_responses_decay_DEMAND_REDUCE_POLLUTION", 
							   &fz_my_responses_decay[REQUEST_TYPE_DEMAND_REDUCE_POLLUTION], 
							   1, FLI_SYM_OUTPUT);
	ai->m_fliEngine->AddSymbol("my_responses_decay_DEMAND_LOWER_TARIFFS", 
							   &fz_my_responses_decay[REQUEST_TYPE_DEMAND_NO_PIRACY], 
							   1, FLI_SYM_OUTPUT);
	ai->m_fliEngine->AddSymbol("my_responses_decay_OFFER_ADVANCE", 
							   &fz_my_responses_decay[REQUEST_TYPE_OFFER_ADVANCE], 
							   1, FLI_SYM_OUTPUT);
	ai->m_fliEngine->AddSymbol("my_responses_decay_OFFER_CITY", 
							   &fz_my_responses_decay[REQUEST_TYPE_OFFER_CITY], 
							   1, FLI_SYM_OUTPUT);
	ai->m_fliEngine->AddSymbol("my_responses_decay_OFFER_MAP", 
							   &fz_my_responses_decay[REQUEST_TYPE_OFFER_MAP], 
							   1, FLI_SYM_OUTPUT);
	ai->m_fliEngine->AddSymbol("my_responses_decay_OFFER_GOLD", 
							   &fz_my_responses_decay[REQUEST_TYPE_OFFER_GOLD], 
							   1, FLI_SYM_OUTPUT);
	ai->m_fliEngine->AddSymbol("my_responses_decay_OFFER_CEASE_FIRE", 
							   &fz_my_responses_decay[REQUEST_TYPE_OFFER_CEASE_FIRE], 
							   1, FLI_SYM_OUTPUT);
	ai->m_fliEngine->AddSymbol("my_responses_decay_OFFER_PERMANENT_ALLIANCE", 
							   &fz_my_responses_decay[REQUEST_TYPE_OFFER_PERMANENT_ALLIANCE], 
							   1, FLI_SYM_OUTPUT);
	ai->m_fliEngine->AddSymbol("my_responses_decay_OFFER_PACT_CAPTURE_CITY", 
							   &fz_my_responses_decay[REQUEST_TYPE_OFFER_PACT_CAPTURE_CITY], 
							   1, FLI_SYM_OUTPUT);
	ai->m_fliEngine->AddSymbol("my_responses_decay_OFFER_PACT_END_POLLUTION", 
							   &fz_my_responses_decay[REQUEST_TYPE_OFFER_PACT_END_POLLUTION], 
							   1, FLI_SYM_OUTPUT);
	ai->m_fliEngine->AddSymbol("my_responses_decay_EXCHANGE_ADVANCE", 
							   &fz_my_responses_decay[REQUEST_TYPE_EXCHANGE_ADVANCE], 
							   1, FLI_SYM_OUTPUT);
	ai->m_fliEngine->AddSymbol("my_responses_decay_EXCHANGE_CITY", 
							   &fz_my_responses_decay[REQUEST_TYPE_EXCHANGE_CITY], 
							   1, FLI_SYM_OUTPUT);
	ai->m_fliEngine->AddSymbol("my_responses_decay_EXCHANGE_MAP", 
							   &fz_my_responses_decay[REQUEST_TYPE_EXCHANGE_MAP], 
							   1, FLI_SYM_OUTPUT);
	
	
	
	
	ai->m_fliEngine->AddSymbol("his_responses_GREETING", 
							   &fz_his_responses[REQUEST_TYPE_GREETING], 
							   0, FLI_SYM_INPUT);
	ai->m_fliEngine->AddSymbol("his_responses_DEMAND_ADVANCE", 
							   &fz_his_responses[REQUEST_TYPE_DEMAND_ADVANCE], 
							   0, FLI_SYM_INPUT);
	ai->m_fliEngine->AddSymbol("his_responses_DEMAND_CITY", 
							   &fz_his_responses[REQUEST_TYPE_DEMAND_CITY], 
							   0, FLI_SYM_INPUT);
	ai->m_fliEngine->AddSymbol("his_responses_DEMAND_MAP", 
							   &fz_his_responses[REQUEST_TYPE_DEMAND_MAP], 
							   0, FLI_SYM_INPUT);
	ai->m_fliEngine->AddSymbol("his_responses_DEMAND_GOLD", 
							   &fz_his_responses[REQUEST_TYPE_DEMAND_GOLD], 
							   0, FLI_SYM_INPUT);
	ai->m_fliEngine->AddSymbol("his_responses_DEMAND_STOP_TRADE", 
							   &fz_his_responses[REQUEST_TYPE_DEMAND_STOP_TRADE], 
							   0, FLI_SYM_INPUT);
	ai->m_fliEngine->AddSymbol("his_responses_DEMAND_ATTACK_ENEMY", 
							   &fz_his_responses[REQUEST_TYPE_DEMAND_ATTACK_ENEMY], 
							   0, FLI_SYM_INPUT);
	ai->m_fliEngine->AddSymbol("his_responses_DEMAND_LEAVE_OUR_LANDS", 
							   &fz_his_responses[REQUEST_TYPE_DEMAND_LEAVE_OUR_LANDS], 
							   0, FLI_SYM_INPUT);
	ai->m_fliEngine->AddSymbol("his_responses_DEMAND_REDUCE_POLLUTION", 
							   &fz_his_responses[REQUEST_TYPE_DEMAND_REDUCE_POLLUTION], 
							   0, FLI_SYM_INPUT);
	ai->m_fliEngine->AddSymbol("his_responses_DEMAND_LOWER_TARIFFS", 
							   &fz_his_responses[REQUEST_TYPE_DEMAND_NO_PIRACY], 
							   0, FLI_SYM_INPUT);
	ai->m_fliEngine->AddSymbol("his_responses_OFFER_ADVANCE", 
							   &fz_his_responses[REQUEST_TYPE_OFFER_ADVANCE], 
							   0, FLI_SYM_INPUT);
	ai->m_fliEngine->AddSymbol("his_responses_OFFER_CITY", 
							   &fz_his_responses[REQUEST_TYPE_OFFER_CITY], 
							   0, FLI_SYM_INPUT);
	ai->m_fliEngine->AddSymbol("his_responses_OFFER_MAP", 
							   &fz_his_responses[REQUEST_TYPE_OFFER_MAP], 
							   0, FLI_SYM_INPUT);
	ai->m_fliEngine->AddSymbol("his_responses_OFFER_GOLD", 
							   &fz_his_responses[REQUEST_TYPE_OFFER_GOLD], 
							   0, FLI_SYM_INPUT);
	ai->m_fliEngine->AddSymbol("his_responses_OFFER_CEASE_FIRE", 
							   &fz_his_responses[REQUEST_TYPE_OFFER_CEASE_FIRE], 
							   0, FLI_SYM_INPUT);
	ai->m_fliEngine->AddSymbol("his_responses_OFFER_PERMANENT_ALLIANCE", 
							   &fz_his_responses[REQUEST_TYPE_OFFER_PERMANENT_ALLIANCE], 
							   0, FLI_SYM_INPUT);
	ai->m_fliEngine->AddSymbol("his_responses_OFFER_PACT_CAPTURE_CITY", 
							   &fz_his_responses[REQUEST_TYPE_OFFER_PACT_CAPTURE_CITY], 
							   0, FLI_SYM_INPUT);
	ai->m_fliEngine->AddSymbol("his_responses_OFFER_PACT_END_POLLUTION", 
							   &fz_his_responses[REQUEST_TYPE_OFFER_PACT_END_POLLUTION], 
							   0, FLI_SYM_INPUT);
	ai->m_fliEngine->AddSymbol("his_responses_EXCHANGE_ADVANCE", 
							   &fz_his_responses[REQUEST_TYPE_EXCHANGE_ADVANCE], 
							   0, FLI_SYM_INPUT);
	ai->m_fliEngine->AddSymbol("his_responses_EXCHANGE_CITY", 
							   &fz_his_responses[REQUEST_TYPE_EXCHANGE_CITY], 
							   0, FLI_SYM_INPUT);
	ai->m_fliEngine->AddSymbol("his_responses_EXCHANGE_MAP", 
							   &fz_his_responses[REQUEST_TYPE_EXCHANGE_MAP], 
							   0, FLI_SYM_INPUT);
	
	
	
	
	
	
	
	
	ai->m_fliEngine->AddSymbol("his_responses_decay_GREETING", 
							   &fz_his_responses_decay[REQUEST_TYPE_GREETING], 
							   1, FLI_SYM_OUTPUT);
	ai->m_fliEngine->AddSymbol("his_responses_decay_DEMAND_ADVANCE", 
							   &fz_his_responses_decay[REQUEST_TYPE_DEMAND_ADVANCE], 
							   1, FLI_SYM_OUTPUT);
	ai->m_fliEngine->AddSymbol("his_responses_decay_DEMAND_CITY", 
							   &fz_his_responses_decay[REQUEST_TYPE_DEMAND_CITY], 
							   1, FLI_SYM_OUTPUT);
	ai->m_fliEngine->AddSymbol("his_responses_decay_DEMAND_MAP", 
							   &fz_his_responses_decay[REQUEST_TYPE_DEMAND_MAP], 
							   1, FLI_SYM_OUTPUT);
	ai->m_fliEngine->AddSymbol("his_responses_decay_DEMAND_GOLD", 
							   &fz_his_responses_decay[REQUEST_TYPE_DEMAND_GOLD], 
							   1, FLI_SYM_OUTPUT);
	ai->m_fliEngine->AddSymbol("his_responses_decay_DEMAND_STOP_TRADE", 
							   &fz_his_responses_decay[REQUEST_TYPE_DEMAND_STOP_TRADE], 
							   1, FLI_SYM_OUTPUT);
	ai->m_fliEngine->AddSymbol("his_responses_decay_DEMAND_ATTACK_ENEMY",
					   &fz_his_responses_decay[REQUEST_TYPE_DEMAND_ATTACK_ENEMY], 
					   1, FLI_SYM_OUTPUT);
	ai->m_fliEngine->AddSymbol("his_responses_decay_DEMAND_LEAVE_OUR_LANDS", 
							   &fz_his_responses_decay[REQUEST_TYPE_DEMAND_LEAVE_OUR_LANDS], 
							   1, FLI_SYM_OUTPUT);
	ai->m_fliEngine->AddSymbol("his_responses_decay_DEMAND_REDUCE_POLLUTION", 
							   &fz_his_responses_decay[REQUEST_TYPE_DEMAND_REDUCE_POLLUTION], 
							   1, FLI_SYM_OUTPUT);
	ai->m_fliEngine->AddSymbol("his_responses_decay_DEMAND_LOWER_TARIFFS", 
							   &fz_his_responses_decay[REQUEST_TYPE_DEMAND_NO_PIRACY], 
							   1, FLI_SYM_OUTPUT);
	ai->m_fliEngine->AddSymbol("his_responses_decay_OFFER_ADVANCE", 
							   &fz_his_responses_decay[REQUEST_TYPE_OFFER_ADVANCE], 
							   1, FLI_SYM_OUTPUT);
	ai->m_fliEngine->AddSymbol("his_responses_decay_OFFER_CITY", 
							   &fz_his_responses_decay[REQUEST_TYPE_OFFER_CITY], 
							   1, FLI_SYM_OUTPUT);
	ai->m_fliEngine->AddSymbol("his_responses_decay_OFFER_MAP", 
							   &fz_his_responses_decay[REQUEST_TYPE_OFFER_MAP], 
							   1, FLI_SYM_OUTPUT);
	ai->m_fliEngine->AddSymbol("his_responses_decay_OFFER_GOLD", 
							   &fz_his_responses_decay[REQUEST_TYPE_OFFER_GOLD], 
							   1, FLI_SYM_OUTPUT);
	ai->m_fliEngine->AddSymbol("his_responses_decay_OFFER_CEASE_FIRE", 
							   &fz_his_responses_decay[REQUEST_TYPE_OFFER_CEASE_FIRE], 
							   1, FLI_SYM_OUTPUT);
	ai->m_fliEngine->AddSymbol("his_responses_decay_OFFER_PERMANENT_ALLIANCE", 
							   &fz_his_responses_decay[REQUEST_TYPE_OFFER_PERMANENT_ALLIANCE], 
							   1, FLI_SYM_OUTPUT);
	ai->m_fliEngine->AddSymbol("his_responses_decay_OFFER_PACT_CAPTURE_CITY", 
							   &fz_his_responses_decay[REQUEST_TYPE_OFFER_PACT_CAPTURE_CITY], 
							   1, FLI_SYM_OUTPUT);
	ai->m_fliEngine->AddSymbol("his_responses_decay_OFFER_PACT_END_POLLUTION", 
							   &fz_his_responses_decay[REQUEST_TYPE_OFFER_PACT_END_POLLUTION], 
							   1, FLI_SYM_OUTPUT);
	ai->m_fliEngine->AddSymbol("his_responses_decay_EXCHANGE_ADVANCE", 
							   &fz_his_responses_decay[REQUEST_TYPE_EXCHANGE_ADVANCE], 
							   1, FLI_SYM_OUTPUT);
	ai->m_fliEngine->AddSymbol("his_responses_decay_EXCHANGE_CITY", 
							   &fz_his_responses_decay[REQUEST_TYPE_EXCHANGE_CITY], 
							   1, FLI_SYM_OUTPUT);
	ai->m_fliEngine->AddSymbol("his_responses_decay_EXCHANGE_MAP", 
							   &fz_his_responses_decay[REQUEST_TYPE_EXCHANGE_MAP], 
							   1, FLI_SYM_OUTPUT);
	
	
	
	
	
    
	ai->m_fliEngine->AddSymbol("best_friend", &fz_best_friend, 0.0, FLI_SYM_INPUT);
    ai->m_fliEngine->AddSymbol("worst_enemy", &fz_worst_enemy, 0.0, FLI_SYM_INPUT);

    ai->m_fliEngine->AddSymbol("turns_to_disaster", &fz_turns_to_disaster, 2000.0, FLI_SYM_INPUT); 
    ai->m_fliEngine->AddSymbol("my_percent_pollution", &fz_my_percent_pollution, 0.0, FLI_SYM_INPUT); 

    
	ai->m_fliEngine->AddSymbol("unit_regard_towards_him", &fz_unit_regard_towards_him, 50.0, FLI_SYM_INPUT);
    ai->m_fliEngine->AddSymbol("unit_regard_delta", &fz_unit_regard_delta, 0.0, FLI_SYM_OUTPUT);

    ai->m_fliEngine->AddSymbol("he_beat_me", &fz_he_beat_me, 0.0, FLI_SYM_INPUT);
    ai->m_fliEngine->AddSymbol("he_beat_me_decay", &fz_he_beat_me_decay, 0.9, FLI_SYM_OUTPUT);
    ai->m_fliEngine->AddSymbol("he_beat_me_this_turn", &fz_he_beat_me_this_turn, 0.0, FLI_SYM_INPUT);

    ai->m_fliEngine->AddSymbol("highest_enemy_strength_ratio", &fz_highest_enemy_strength_ratio, 0.8, FLI_SYM_INPUT);
    ai->m_fliEngine->AddSymbol("highest_human_enemy_strength_ratio", &fz_highest_human_enemy_strength_ratio, 0.8, FLI_SYM_INPUT);
    ai->m_fliEngine->AddSymbol("total_enemy_strength_ratio", &fz_total_enemy_strength_ratio, 0.8, FLI_SYM_INPUT);
    ai->m_fliEngine->AddSymbol("allies_vs_enemies_strength_ratio", &fz_allies_vs_enemies_strength_ratio, 0.8, FLI_SYM_INPUT);
    
	ai->m_fliEngine->AddSymbol("enemy_of_my_enemy", &fz_friend_of_my_enemy, 0.0, FLI_SYM_INPUT);
    ai->m_fliEngine->AddSymbol("friend_of_my_enemy", &fz_friend_of_my_enemy, 0.0, FLI_SYM_INPUT);
    ai->m_fliEngine->AddSymbol("has_pact_against_me", &fz_has_pact_against_me, 0.0, FLI_SYM_INPUT);
    ai->m_fliEngine->AddSymbol("has_pact_against_me_decay", &fz_has_pact_against_me_decay, 0.98, FLI_SYM_OUTPUT);

    ai->m_fliEngine->AddSymbol("length_of_war_with_him", &fz_length_of_war_with_him, 0.0, FLI_SYM_INPUT);
    ai->m_fliEngine->AddSymbol("he_shares_continent", &fz_he_shares_continent, 0.0, FLI_SYM_INPUT); 
    ai->m_fliEngine->AddSymbol("distance_to_his_capitol", &fz_distance_to_his_capitol, 0.0, FLI_SYM_INPUT); 
    ai->m_fliEngine->AddSymbol("min_distance_to_capitol", &fz_min_distance_to_capitol, 0.0, FLI_SYM_INPUT); 
    ai->m_fliEngine->AddSymbol("distance_to_nearest_human_capitol", &fz_distance_to_nearest_human_capitol, 2000.0, FLI_SYM_INPUT); 

	ai->m_fliEngine->AddSymbol("num_he_is_at_war", &fz_num_he_is_at_war, 0.0, FLI_SYM_INPUT);
    ai->m_fliEngine->AddSymbol("he_nuked_me_count", &fz_he_nuked_me_count, 0.0, FLI_SYM_INPUT); 
    ai->m_fliEngine->AddSymbol("i_use_mass_destruction", &fz_i_use_mass_destruction, 0.0, FLI_SYM_INPUT);     
    ai->m_fliEngine->AddSymbol("he_has_space", &fz_he_has_space, 0.0, FLI_SYM_INPUT); 
    ai->m_fliEngine->AddSymbol("he_has_undersea", &fz_he_has_undersea, 0.0, FLI_SYM_INPUT); 
    ai->m_fliEngine->AddSymbol("num_players_in_game", &fz_num_players_in_game, 0.0, FLI_SYM_INPUT);
    ai->m_fliEngine->AddSymbol("at_war_count", &fz_at_war_count, 0.0, FLI_SYM_INPUT);
    ai->m_fliEngine->AddSymbol("longest_war", &fz_longest_war, 0.0, FLI_SYM_INPUT);
    ai->m_fliEngine->AddSymbol("shortest_peace", &fz_shortest_peace, 0.0, FLI_SYM_INPUT);
    ai->m_fliEngine->AddSymbol("total_military_incursions", &fz_total_military_incursions, 0.0, FLI_SYM_INPUT);
	
	ai->m_fliEngine->AddSymbol("lowest_unit_regard", &fz_lowest_unit_regard, 0.0, FLI_SYM_INPUT);

    ai->m_fliEngine->AddSymbol("size_of_world", &fz_size_of_world, 1.0, FLI_SYM_INPUT); 
    ai->m_fliEngine->AddSymbol("closest_capitol", &fz_closest_capitol, 0.0, FLI_SYM_INPUT); 

    ai->m_fliEngine->AddSymbol("his_relative_strength", &fz_his_relative_strength, 0.5, FLI_SYM_INPUT); 
    ai->m_fliEngine->AddSymbol("his_normalized_rank", &fz_his_normalized_rank, 0.5, FLI_SYM_INPUT); 
    ai->m_fliEngine->AddSymbol("my_normalized_rank", &fz_my_normalized_rank, 0.0, FLI_SYM_INPUT); 
    ai->m_fliEngine->AddSymbol("normalized_rank_diff_between_first_and_second_place", 
		&fz_normalized_rank_diff_between_first_and_second_place, 0.0, FLI_SYM_INPUT); 

    ai->m_fliEngine->AddSymbol("unit_to_city_best_human_ratio", &fz_unit_to_city_best_human_ratio, 0.0, FLI_SYM_INPUT); 

	ai->m_fliEngine->AddSymbol("his_coldwar_attacked_me_decay", &fz_his_coldwar_attacked_me_decay, 1.0, FLI_SYM_OUTPUT);
	ai->m_fliEngine->AddSymbol("his_hotwar_attacked_me_decay", &fz_his_hotwar_attacked_me_decay, 1.0, FLI_SYM_OUTPUT);
	ai->m_fliEngine->AddSymbol("accepted_requests_decay", &fz_accepted_requests_decay, 1.0, FLI_SYM_OUTPUT);
	ai->m_fliEngine->AddSymbol("rejected_requests_decay", &fz_rejected_requests_decay, 1.0, FLI_SYM_OUTPUT);
	ai->m_fliEngine->AddSymbol("piracy_decay", &fz_piracy_decay, 1.0, FLI_SYM_OUTPUT);
	ai->m_fliEngine->AddSymbol("his_pirated_me", &fz_his_pirated_me, 0.0, FLI_SYM_INPUT);

	ai->m_fliEngine->AddSymbol("violating_stop_trade", &fz_violating_stop_trade, 0.0, FLI_SYM_INPUT);
	ai->m_fliEngine->AddSymbol("violating_leave_our_lands", &fz_violating_leave_our_lands, 0.0, FLI_SYM_INPUT);
	ai->m_fliEngine->AddSymbol("violating_reduce_pollution", &fz_violating_reduce_pollution, 0.0, FLI_SYM_INPUT);
	ai->m_fliEngine->AddSymbol("violating_end_pollution", &fz_violating_end_pollution, 0.0, FLI_SYM_INPUT);
	ai->m_fliEngine->AddSymbol("violating_attack_enemy", &fz_violating_attack_enemy, 0.0, FLI_SYM_INPUT);
	
	ai->m_fliEngine->AddSymbol("have_stop_trade", &fz_have_stop_trade, 0.0, FLI_SYM_INPUT);
	ai->m_fliEngine->AddSymbol("have_leave_our_lands", &fz_have_leave_our_lands, 0.0, FLI_SYM_INPUT);
	ai->m_fliEngine->AddSymbol("have_reduce_pollution", &fz_have_reduce_pollution, 0.0, FLI_SYM_INPUT);
	ai->m_fliEngine->AddSymbol("have_end_pollution", &fz_have_end_pollution, 0.0, FLI_SYM_INPUT);
	ai->m_fliEngine->AddSymbol("have_attack_enemy", &fz_have_attack_enemy, 0.0, FLI_SYM_INPUT);
	ai->m_fliEngine->AddSymbol("have_cease_fire", &fz_have_cease_fire, 0.0, FLI_SYM_INPUT);
	ai->m_fliEngine->AddSymbol("have_alliance", &fz_have_alliance, 0.0, FLI_SYM_INPUT);

	ai->m_fliEngine->AddSymbol("attacked_my_enemies", &fz_attacked_my_enemies, 0.0, FLI_SYM_INPUT);
	ai->m_fliEngine->AddSymbol("attacked_my_friends", &fz_attacked_my_friends, 0.0, FLI_SYM_INPUT);
	ai->m_fliEngine->AddSymbol("attacked_my_enemies_decay", &fz_attacked_my_enemies_decay, 0.0, FLI_SYM_OUTPUT);
	ai->m_fliEngine->AddSymbol("attacked_my_friends_decay", &fz_attacked_my_friends_decay, 0.0, FLI_SYM_OUTPUT);
	ai->m_fliEngine->AddSymbol("his_broken_treaties", &fz_his_broken_treaties,
							   0.0, FLI_SYM_INPUT);
	ai->m_fliEngine->AddSymbol("his_broken_treaties_decay", &fz_his_broken_treaties_decay,
							   0.0, FLI_SYM_OUTPUT);

    
	ai->m_fliEngine->AddSymbol("gov_happy_workday", &fz_gov_happy_workday, 1.0, FLI_SYM_OUTPUT);
	ai->m_fliEngine->AddSymbol("gov_happy_wage", &fz_gov_happy_wage, 1.0, FLI_SYM_OUTPUT);
	ai->m_fliEngine->AddSymbol("gov_happy_rations", &fz_gov_happy_rations, 1.0, FLI_SYM_OUTPUT);
	ai->m_fliEngine->AddSymbol("gov_happy_size", &fz_gov_happy_size, 1.0, FLI_SYM_OUTPUT);
	ai->m_fliEngine->AddSymbol("gov_happy_martial", &fz_gov_happy_martial, 1.0, FLI_SYM_OUTPUT);
	ai->m_fliEngine->AddSymbol("gov_happy_unitdist", &fz_gov_happy_unitdist, 1.0, FLI_SYM_OUTPUT);
	ai->m_fliEngine->AddSymbol("gov_happy_combat", &fz_gov_happy_combat, 1.0, FLI_SYM_OUTPUT);
	ai->m_fliEngine->AddSymbol("gov_pollution", &fz_gov_pollution, 1.0, FLI_SYM_OUTPUT);
	ai->m_fliEngine->AddSymbol("gov_crime", &fz_gov_crime, 1.0, FLI_SYM_OUTPUT);
	ai->m_fliEngine->AddSymbol("gov_gold", &fz_gov_gold, 1.0, FLI_SYM_OUTPUT);
	ai->m_fliEngine->AddSymbol("gov_production", &fz_gov_production, 1.0, FLI_SYM_OUTPUT);
	ai->m_fliEngine->AddSymbol("gov_science", &fz_gov_science, 1.5, FLI_SYM_OUTPUT);
	ai->m_fliEngine->AddSymbol("gov_readiness", &fz_gov_readiness, 1.0, FLI_SYM_OUTPUT);
	ai->m_fliEngine->AddSymbol("gov_number_city_growth", &fz_gov_number_city_growth, 1.0, FLI_SYM_OUTPUT);
	
	ai->m_fliEngine->AddSymbol("i_want_this_gov_TYRANNY", &fz_i_want_this_gov[FZ_GOVERNMENT_TYRANNY], 0.0, FLI_SYM_OUTPUT);
	ai->m_fliEngine->AddSymbol("i_want_this_gov_THEOCRACY", &fz_i_want_this_gov[FZ_GOVERNMENT_THEOCRACY], 0.0, FLI_SYM_OUTPUT);
	ai->m_fliEngine->AddSymbol("i_want_this_gov_ECOTOPIA", &fz_i_want_this_gov[FZ_GOVERNMENT_ECOTOPIA], 0.0, FLI_SYM_OUTPUT);
	ai->m_fliEngine->AddSymbol("i_want_this_gov_MONARCHY", &fz_i_want_this_gov[FZ_GOVERNMENT_MONARCHY], 0.0, FLI_SYM_OUTPUT);
	ai->m_fliEngine->AddSymbol("i_want_this_gov_REPUBLIC", &fz_i_want_this_gov[FZ_GOVERNMENT_REPUBLIC], 0.0, FLI_SYM_OUTPUT);
	ai->m_fliEngine->AddSymbol("i_want_this_gov_DEMOCRACY", &fz_i_want_this_gov[FZ_GOVERNMENT_DEMOCRACY], 0.0, FLI_SYM_OUTPUT);
	ai->m_fliEngine->AddSymbol("i_want_this_gov_FASCISM", &fz_i_want_this_gov[FZ_GOVERNMENT_FASCISM], 0.0, FLI_SYM_OUTPUT);
	ai->m_fliEngine->AddSymbol("i_want_this_gov_COMMUNISM", &fz_i_want_this_gov[FZ_GOVERNMENT_FASCISM], 0.0, FLI_SYM_OUTPUT);
	ai->m_fliEngine->AddSymbol("i_want_this_gov_MULTICORP", &fz_i_want_this_gov[FZ_GOVERNMENT_MULTICORP], 0.0, FLI_SYM_OUTPUT);
	ai->m_fliEngine->AddSymbol("i_want_this_gov_TECHNOCRACY", &fz_i_want_this_gov[FZ_GOVERNMENT_TECHNOCRACY], 0.0, FLI_SYM_OUTPUT);
	ai->m_fliEngine->AddSymbol("i_want_this_gov_VIRTUAL_D", &fz_i_want_this_gov[FZ_GOVERNMENT_VIRTUAL_D], 0.0, FLI_SYM_OUTPUT);
	

	ai->m_fliEngine->AddSymbol("i_want_this_gov_uscale", &fz_i_want_this_gov_uscale, 1.0, FLI_SYM_OUTPUT);
	ai->m_fliEngine->AddSymbol("new_gov_discount", &fz_new_gov_discount, 0.85, FLI_SYM_OUTPUT);

    

	
	















	
	ai->m_fliEngine->AddSymbol(
		"overall_threat", 
		&(ai->fuzzy_variables->overall_threat), 
		0.0, FLI_SYM_INPUT);  
	ai->m_fliEngine->AddSymbol(
		"power_ratio", 
		&(ai->fuzzy_variables->power_ratio), 
		0.0, FLI_SYM_INPUT);  

	ai->m_fliEngine->AddSymbol("min_force_matching_percent", &fz_min_force_matching_percent, 1.0, FLI_SYM_OUTPUT);
	ai->m_fliEngine->AddSymbol("max_force_matching_percent", &fz_max_force_matching_percent, 1.0, FLI_SYM_OUTPUT);

	


	ai->m_fliEngine->AddSymbol("percent_cost_readiness", &fz_percent_cost_readiness, 0.0, FLI_SYM_INPUT);  
	ai->m_fliEngine->AddSymbol("desired_military_readiness", &fz_desired_military_readiness, 0.0, FLI_SYM_OUTPUT);  


	
	SetCurrentEngine(ai->m_fliEngine); 
    BOOL r; 
    r = fli_RunParser(gs->GetDataPath(), ai->GetPersonalityFilename());
    Assert(r); 

    if (!r) {
        return FALSE; 
    }

	ai->m_fliEngine->Init();
    ai->m_fliEngine->FireRules(FLI_SECT_INIT); 
    ai->m_fliEngine->InitGraphics(); 

#ifdef DO_FZLOG
	ai->m_fliEngine->DumpFZInputHeaders(ai->m_my_player_id);
#endif

    return TRUE; 

} 


