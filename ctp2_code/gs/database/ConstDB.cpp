//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : Game constants database
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
// - Exported MAX_MATCH_LIST_CYCLES and MIN_TURNS_BETWEEN_REVOLT to be 
//   modifiable in const.txt.
// - Added compatibility handling to solve database out of synch problems.
// - Removed temporary compatibility, to force const.txt cleanup.
//
//----------------------------------------------------------------------------

#include "c3.h"

#include "Token.h"
#include "civarchive.h"
#include "pollution.h"
#include "ConstDB.h"
#include "c3errors.h"
#include "globals.h"

extern ConstDB *g_theConstDB; 

extern sint32		g_abort_parse;
extern TokenData	g_const_token_data[];

namespace
{

// Default (original game) values when not present in const.txt.

int const	DEFAULT_MAX_MATCH_LIST_CYCLES		= 6;
int const	DEFAULT_MIN_TURNS_BETWEEN_REVOLT	= 1;

//----------------------------------------------------------------------------
//
// Name       : ParseOptional
//
// Description: Parse an optional element from the input file
//
// Parameters : a_Token			: current token pointer
//				t				: token to look for
//				defaultVal		: value to return when the token is not found
//
// Globals    : g_abort_parse	: set when a parsing error occurs
//
// Returns    : bool			: set when the token was read from the file
//				val				: value (from file or default)
//
// Remark(s)  : Modified version of token_ParseValNext
//
//----------------------------------------------------------------------------

bool ParseOptional
(
	Token *			aToken, 
	sint32 const	t, 
	sint32 &		val, 
	sint32 const	defaultVal
)
{
	if ((aToken->GetType() == t) || (aToken->Next() == t))
	{
		// Keyword found: the number has to be valid
		if (aToken->Next() == TOKEN_NUMBER)
		{
			aToken->GetNumber(val);
			return true;
		}
		else
		{ 
			c3errors_ErrorDialog(aToken->ErrStr(), "Expected number not found");
			g_abort_parse = TRUE; 
		} 
	} 

	val = defaultVal; 
	return false;
}


} // namespace

ConstDB::ConstDB ()

{ 
	memset(this, 0, sizeof(*this));

	sint32 i ;

	for (i=0; i<k_NUM_MERIDIANS; i++) { 
		m_merid[i] = 11 * (i + 1) ;
	}

	m_leave_our_lands_expires = 0 ;
	m_reduce_pollution_expires = 0 ;

	m_average_pollution_turns = 0 ;
	m_patience_lost_per_request = 0 ;
	m_patience_regained_per_round = 0 ;
	m_patience_lost_threshold = 0 ;
	m_patience_regain_probability = 0.0 ;
	m_entrenchment_bonus = 0.0;
	m_elite_spy_bonus = 0;
	m_city_second_attempt_spy_bonus = 0;
	m_tariff_reduction = 0 ;
	m_revolt_influence_distance = 6 ;
	m_monopolyThreshold = 0 ;
	m_veryHappyThreshold = 0 ;


	m_discoveries_pollution_size_modifier=0.0 ;
	m_discoveries_pollution_production_modifier=0.0 ;
    m_base_workday = 0.0; 
    m_base_rations = 0.0; 
    m_base_wages = 0.0; 
	m_no_piracy_expires=0;
	m_end_of_game_year_early_warning = 0 ;
	m_end_of_game_year = 0 ;

	m_pollution_forces_government_collapse = 0 ;
	m_food_to_pollution_coef = 0.0 ;
	m_cityPopulationProducesPollutionThreshold=0 ;
	m_cityProductionProducesPollutionThreshold=0 ;
	m_piracy_kills_trader_chance = 0;
	m_max_requests_per_player_per_turn = 3;
	m_min_absolute_start_distance = 5;
	m_max_absolute_growth_rate = 5000;
	m_trade_distance_coefficient = 1;
	m_min_like_tiles_for_good = 3;
	m_piracy_waste_coefficient = 1.0;
	m_border_int_radius = 4;
	m_border_squared_radius = 20;
	m_caravan_coef = 0.02;
	m_max_match_list_cycles		= DEFAULT_MAX_MATCH_LIST_CYCLES;
	m_min_turns_between_revolt	= DEFAULT_MIN_TURNS_BETWEEN_REVOLT;
	} 












ConstDB::ConstDB(CivArchive &archive)
	{
	Serialize(archive) ;
	}

//----------------------------------------------------------------------------
//
// Name       : ConstDB::Serialize
//
// Description: Serialize/unserialize the database
//
// Parameters : archive	: archive to serialize to or from
//
// Globals    : -
//
// Returns    : -
//
// Remark(s)  : This function is only used for multiplayer games, to verify
//              the consistency between the databases of the players.
//              The values are never stored in the save game, nor transmitted.
//              The values are read from const.txt, and the multiplayer
//              verification is based on a CRC over the data.
//
//----------------------------------------------------------------------------

void ConstDB::Serialize(CivArchive &archive)
	{
    CHECKSERIALIZE

#define CONSTDB_MAGIC 0x00FE7987
	if (archive.IsStoring())
		{
		archive.PerformMagic(CONSTDB_MAGIC) ;
	archive.PutDoubleString(	m_land); 
	archive.PutDoubleString(	m_continent); 
	archive.PutDoubleString(	m_homogenous); 
	archive.PutDoubleString(	m_mountain); 
	archive.PutDoubleString(	m_hills); 
	archive <<	m_mountain_spread; 
	archive <<	m_mountain_length;
	archive <<	m_glacier;
	archive <<	m_volcano; 
	archive.PutDoubleString(	m_trench); 
	archive <<	m_mount_cell; 
	archive <<  m_forest; 
	archive <<  m_grass;  
	archive <<  m_plains; 
	archive <<  m_desert; 
	archive <<  m_whitePercent;
	archive <<  m_brownPercent;
	archive <<  m_temperatureRangeAdjust;
	
	archive.StoreArray(m_merid, k_NUM_MERIDIANS);
	 archive.StoreArrayString(m_meridf, k_NUM_MERIDIANS);
	
	 archive.StoreArrayString(m_hlevelf, 4);
	
	archive <<	m_forestWet;
	archive <<	m_forestDry;
	archive <<	m_grassWet;
	archive <<	m_grassDry;
	archive <<	m_plainsWet;
	archive <<	m_plainsDry;
	archive <<	m_desertWet;
	archive <<	m_desertDry;

	archive <<	m_whiteWarm;
	archive <<	m_whiteCold;
	archive <<	m_brownWarm;
	archive <<	m_brownCold;
	archive <<	m_temperatureRangeAdjustWarm;
	archive <<	m_temperatureRangeAdjustCold;

	archive <<	m_richnessFewgoods;
	archive <<	m_richnessManygoods;
	archive <<	m_riverCellWidthFewgoods;
	archive <<	m_riverCellWidthManygoods;
	archive <<	m_riverCellHeightFewgoods;
	archive <<	m_riverCellHeightManygoods;

	archive <<	m_nice_r; 
	archive.PutDoubleString(	m_river);
	archive <<	m_river_len;
	archive <<	m_richness; 
	archive <<	m_lost_at_sea;
	archive.PutDoubleString(	m_vet_coef); 
	
	archive <<	m_average_pollution_turns ;
	
	archive <<	m_revolution_level ;
	archive <<	m_tariff_reduction <<
		m_revolt_influence_distance ;
	


	archive.PutDoubleString(	m_discoveries_pollution_size_modifier );
	archive.PutDoubleString(	m_discoveries_pollution_production_modifier );
	
    archive.PutDoubleString(	m_max_empire_dist_cost);
    archive.PutDoubleString(	m_happiness_launch_cost);
	
    archive.PutDoubleString(	m_unit_workday);
    archive.PutDoubleString(  m_base_workday);
    archive.PutDoubleString(	m_unit_wages);
    archive.PutDoubleString(  m_base_wages);
    archive.PutDoubleString(	m_unit_rations);
    archive.PutDoubleString(  m_base_rations); 
	archive.PutDoubleString(	m_food_to_pollution_coef );

	archive <<	m_monopolyThreshold <<
		m_veryHappyThreshold <<									
		m_populationPollutionWarningThreshold <<
		m_industrialPollutionWarningThreshold <<
		m_pact_capture_city_expires <<
		
		m_cease_fire_expires <<
		m_short_cease_fire_expires <<
		m_pact_end_pollution_expires <<
		m_leave_our_lands_expires <<
		m_reduce_pollution_expires <<
		
		m_capture_city_for_gold_multiplier <<
		m_patience_lost_per_request <<
		m_patience_regained_per_round <<
		m_patience_lost_threshold <<
		m_end_of_game_year_early_warning <<
		m_end_of_game_year ;
	

	archive <<	m_pollution_forces_government_collapse ;
	
	archive.PutDoubleString(	m_patience_regain_probability );
	
    archive << m_goal_time_slice; 
    archive << m_max_time_slice; 
    archive << m_total_time_slice; 
	
	archive.PutDoubleString( m_entrenchment_bonus);
	archive << m_paradrop_distance;
	archive << m_paradrop_success_percentage;
	archive.PutDoubleString( m_elite_spy_bonus);
	archive.PutDoubleString( m_city_second_attempt_spy_bonus);
	
	archive << m_assasination_happiness_effect;
	archive << m_assasination_happiness_effect_timer;
	
	archive << m_spied_upon_wariness_timer;
	archive << m_maximum_party_cost;
	archive.PutDoubleString( m_maximum_party_chance);
	archive << m_gossip_map_radius;
	archive.PutDoubleString( m_hear_gossip_chance);
	
	archive.PutDoubleString( m_franchise_effect);
	archive <<	m_turns_to_sue_franchise;
	
	archive.PutDoubleString(	m_elite_slaver_bonus);
	archive.PutDoubleString(	m_slaver_elite_chance);
	
	archive.PutDoubleString(m_elite_abolitionist_bonus);
	archive.PutDoubleString(m_abolitionist_elite_chance);
	archive.PutDoubleString(m_watchful_city_success_modifier);
	archive.PutDoubleString(m_watchful_city_death_modifier);
	archive <<	m_watchful_city_turns;
	archive <<	m_bio_infection_turns;
	archive <<	m_nano_infection_turns;
	archive.PutDoubleString(m_bio_infection_spread_chance);
	archive.PutDoubleString(m_nano_infection_spread_chance);
	
	archive.PutDoubleString( m_elite_terrorist_bonus);
	archive.PutDoubleString( m_terrorist_elite_chance);
	archive.PutDoubleString( m_nuke_population_percentage);
	archive.PutDoubleString( m_bio_terror_kill_percentage);
	
	archive.PutDoubleString( m_special_action_move_cost);
	
	archive.PutDoubleString( m_cleric_conversion_factor);
	archive.PutDoubleString( m_televangelist_conversion_factor);
	
	archive.PutDoubleString(m_reformation_chance);
	archive.PutDoubleString(m_reformation_death_chance);
	archive.PutDoubleString(m_reformation_happiness_time);
	archive.PutDoubleString(m_reformation_happiness_amount);

	archive << m_unconverted_indulgence_gold << m_unconverted_indulgence_happiness <<
		m_converted_indulgence_gold << m_converted_indulgence_happiness <<
		m_other_faith_indulgence_gold << m_other_faith_indulgence_happiness;
	archive << m_soothsay_happy_amount;
	
	archive << m_hut_box_width << m_hut_box_height;
	archive.PutDoubleString( m_hut_chance_per_box);
	archive << m_max_goody_hut_gold;
	archive << m_max_goody_hut_advance;
	archive << m_max_goody_hut_unit;
	
	archive.PutDoubleString( m_unit_rush_modifier);
	archive.PutDoubleString( m_improvement_rush_modifier);
	archive.PutDoubleString( m_wonder_rush_modifier);

    archive.PutDoubleString( m_change_currently_building_item_penalty); 
	archive.PutDoubleString( m_building_production_to_value_modifier);
	
	archive.PutDoubleString( m_city_growth_coefficient);
	archive.PutDoubleString( m_power_points_to_materials);
	archive << m_max_airlift_stack_size;
	archive << m_river_cell_width;
	archive << m_river_cell_height;
	archive << m_gold_from_piracy;
	archive << m_no_piracy_expires;
	archive.PutDoubleString(m_space_launch_cost);
	archive.PutDoubleString(m_space_land_cost);
	archive << m_wormhole_orbit_height_percentage;
	archive << m_wormhole_speed;
	archive << m_wormhole_return_time;
	archive << m_wormhole_visible_to_all_turns;
	archive << m_max_government_change_turns;

	archive << m_emancipation_unhappiness_turns;
	archive.PutDoubleString( m_emancipation_unhappiness_amount);
	archive.PutDoubleString( m_capture_city_advance_chance);

	archive.PutDoubleString( m_city_heal_rate);
	archive.PutDoubleString( m_normal_heal_rate);
	archive.PutDoubleString( m_local_pollution_level);
	archive.PutDoubleString( m_local_pollution_chance);
	archive << m_cityPopulationProducesPollutionThreshold ;
	archive << m_cityProductionProducesPollutionThreshold ;

	archive.PutDoubleString( m_assault_destroy_building_chance);
	archive.PutDoubleString( m_bombard_destroy_building_chance);

	archive << m_advance_choices_min << m_advance_choices_max;

	archive << m_attack_converter_unhappiness_turns;
	archive.PutDoubleString( m_attack_converter_unhappiness_amount);

	archive.PutDoubleString( m_min_start_distance_coefficient);
	archive.PutDoubleString( m_max_start_distance_coefficient);
	archive << m_max_same_tiles;
	archive.PutDoubleString( m_combat_veteran_chance);

	archive << m_stop_trade_rounds <<
		m_leave_our_lands_rounds <<
		m_reduce_pollution_rounds <<
		m_capture_city_rounds <<
		m_end_pollution_rounds <<
		m_attack_enemy_rounds;

	archive << m_min_continent_start_size;

	archive.PutDoubleString(m_bombard_kill_pop_chance);
	archive.PutDoubleString(m_assault_kill_pop_chance);
	archive.PutDoubleString(m_capture_kill_pop_chance);

	archive.StoreArray(m_scaled_pop, 5);

	archive << m_piracy_kills_trader_chance;

	archive << m_uprising_chance_per_unguarded_slave;

	archive << m_max_disband_size;

	archive << m_max_requests_per_player_per_turn;

	archive << m_slaves_per_military_unit;

	archive << m_min_absolute_start_distance;

	MapPoint	m_small_map_size;
	MapPoint	m_medium_map_size;
	MapPoint	m_large_map_size;
	MapPoint	m_gigantic_map_size;

	archive.PutDoubleString( m_rail_launch_pollution);

	archive << m_space_fuel_cost;
	archive << m_non_space_fuel_cost;

    archive << m_starvation_warning_fudge_factor;
    archive << m_maintenance_warning_fudge_factor;
    archive << m_support_warning_fudge_factor;

	archive.PutDoubleString( m_relative_ai_production_cheat_coef);
	archive.PutDoubleString( m_relative_ai_science_cheat_coef);

	archive.PutDoubleString( m_incite_revolution_gold_coefficient);
	archive.PutDoubleString( m_incite_revolution_capitol_penalty);
	archive.PutDoubleString( m_incite_uprising_gold_coefficient);
	archive.PutDoubleString( m_incite_uprising_capitol_penalty);

	archive << m_nuke_kill_tiles;

	archive << m_bio_infection_unhappiness;

	archive << m_min_eco_pact_violation_level;

	archive.PutDoubleString( m_nano_infection_terrorist_death_chance);
	archive.PutDoubleString( m_bio_infection_terrorist_death_chance);

	archive.PutDoubleString( m_flood_changes_coast_to_water_chance);

	archive.PutDoubleString( m_ai_cheat_eco_pact_min);
	archive.PutDoubleString( m_ai_cheat_eco_pact_max);
	
	archive << m_riot_level;
	if ((DEFAULT_MAX_MATCH_LIST_CYCLES == m_max_match_list_cycles) &&
		(DEFAULT_MIN_TURNS_BETWEEN_REVOLT == m_min_turns_between_revolt)
       )
	{
		// No action, to keep compatibility with the original patch.
	}
	else
	{
		// Modified by kaan to address bug # 12
		archive << m_min_turns_between_revolt;
		archive << m_max_match_list_cycles;
	}
		}
	else
		{
		}

	}


void ConstDB::y2meridian(const sint32 j, sint32 &k) const

{
   sint32 i; 

   for (i=0; i<k_NUM_MERIDIANS; i++) { 
      if (j < m_merid[i]) { 
         k = i;
         return;
      } 
   } 
   k = i;
   return; 
}

void ConstDB::CalcMer(sint32 v) 

{  sint32 i; 
   for (i=0; i<k_NUM_MERIDIANS; i++) { 
      m_merid[i] = sint32 (m_meridf[i] * double(v)); 
   }
} 



enum TOKEN_CONST { 
    TOKEN_CONST_FIRST = TOKEN_MAX+1, 
    TOKEN_CONST_PERCENT_LAND, 
    TOKEN_CONST_PERCENT_CONTINENT,  
            
    TOKEN_CONST_PERCENT_HOMOGENOUS,
    TOKEN_CONST_WET_LEVEL, 
    TOKEN_CONST_DRY_LEVEL, 
    TOKEN_CONST_MERIDIAN0,
    TOKEN_CONST_MERIDIAN1, 
    TOKEN_CONST_MERIDIAN2,
    TOKEN_CONST_MERIDIAN3,
    TOKEN_CONST_MERIDIAN4,
    TOKEN_CONST_MERIDIAN5,
            
    TOKEN_CONST_HLEVELA, 
    TOKEN_CONST_HLEVELB, 
    TOKEN_CONST_HLEVELC, 
    TOKEN_CONST_HLEVELD, 
            
	TOKEN_CONST_FORESTWET,
	TOKEN_CONST_FORESTDRY,
	TOKEN_CONST_GRASSWET,
	TOKEN_CONST_GRASSDRY,
	TOKEN_CONST_PLAINSWET,
	TOKEN_CONST_PLAINSDRY,
	TOKEN_CONST_DESERTWET,
	TOKEN_CONST_DESERTDRY,

	TOKEN_CONST_WHITEWARM,
	TOKEN_CONST_WHITECOLD,
	TOKEN_CONST_BROWNWARM,
	TOKEN_CONST_BROWNCOLD,
	TOKEN_CONST_TEMPERATURERANGEADJUSTWARM,
	TOKEN_CONST_TEMPERATURERANGEADJUSTCOLD,

	TOKEN_CONST_RICHNESSFEWGOODS,
	TOKEN_CONST_RICHNESSMANYGOODS,
	TOKEN_CONST_RIVERCELLWIDTHFEWGOODS,
	TOKEN_CONST_RIVERCELLWIDTHMANYGOODS,
	TOKEN_CONST_RIVERCELLHEIGHTFEWGOODS,
	TOKEN_CONST_RIVERCELLHEIGHTMANYGOODS,

    TOKEN_CONST_PERCENT_MOUNTAIN,
    TOKEN_CONST_MOUNTAIN_CELL,
    TOKEN_CONST_PERCENT_HILLS,
    TOKEN_CONST_MOUNTAIN_SPREAD,
    TOKEN_CONST_MOUNTAIN_LENGTH,
    TOKEN_CONST_GLACIER_EXTENT,
    TOKEN_CONST_PERCENT_VOLCANO, 
    TOKEN_CONST_PERCENT_TRENCH,
	TOKEN_CONST_PERCENT_FOREST,
	TOKEN_CONST_PERCENT_GRASS,
	TOKEN_CONST_PERCENT_PLAINS,
	TOKEN_CONST_PERCENT_DESERT,
	TOKEN_CONST_PERCENT_WHITE,
	TOKEN_CONST_PERCENT_BROWN,
	TOKEN_CONST_TEMPERATURE_RANGE_ADJUST,
    TOKEN_CONST_NICE_RADIUS, 
    TOKEN_CONST_PERCENT_RIVER,
    TOKEN_CONST_RIVER_LEN,
	TOKEN_CONST_RIVER_CELL_WIDTH,
	TOKEN_CONST_RIVER_CELL_HEIGHT,
    TOKEN_CONST_RICHNESS, 
    TOKEN_CONST_CHANCE_LOST_AT_SEA, 
    TOKEN_CONST_VET_COEF, 


	TOKEN_CONST_DISCOVERIES_POLLUTION_SIZE_MODIFIER,
	TOKEN_CONST_DISCOVERIES_POLLUTION_PRODUCTION_MODIFIER,
	TOKEN_CONST_AVERAGE_POLLUTION_TURNS,
	TOKEN_CONST_REVOLUTION_LEVEL, 
	TOKEN_CONST_REVOLT_INFLUENCE_DISTANCE,
	TOKEN_CONST_TARIFF_REDUCTION,

    TOKEN_CONST_MAX_EMPIRE_DIST_COST,
    TOKEN_CONST_HAPPINESS_LAUNCH_COST, 
    TOKEN_CONST_UNIT_WORKDAY,
    TOKEN_CONST_UNIT_WAGES,
    TOKEN_CONST_UNIT_RATIONS,

    TOKEN_CONST_BASE_WORKDAY, 
    TOKEN_CONST_BASE_WAGES, 
    TOKEN_CONST_BASE_RATIONS, 

	TOKEN_CONST_POPULATION_PRODUCES_POLLUTION_THRESHOLD,
	TOKEN_CONST_PRODUCTION_PRODUCES_POLLUTION_THRESHOLD,

    TOKEN_CHANGE_CURRENTLY_BUILDING_ITEM_PENALTY,

	TOKEN_CONST_PACT_CAPTURE_CITY_EXPIRES,

	TOKEN_CONST_CEASE_FIRE_EXPIRES,
	TOKEN_CONST_SHORT_CEASE_FIRE_EXPIRES,
	TOKEN_CONST_END_OF_GAME_YEAR_EARLY_WARNING,
	TOKEN_CONST_END_OF_GAME_YEAR,
	TOKEN_CONST_CAPTURE_CITY_FOR_GOLD_MULTIPLIER,
	TOKEN_CONST_PACT_END_POLLUTION_EXPIRES,
	TOKEN_CONST_LEAVE_OUR_LANDS_EXPIRES,
	TOKEN_CONST_REDUCE_POLLUTION_EXPIRES,


	TOKEN_CONST_PATIENCE_LOST_PER_REQUEST,
	TOKEN_CONST_PATIENCE_REGAINED_PER_ROUND,
	TOKEN_CONST_REGAIN_PATIENCE_PROBABILITY,
	TOKEN_CONST_PATIENCE_LOST_THRESHOLD,

    TOKEN_GOAL_TIME_SLICE,     
    TOKEN_MAX_TIME_SLICE,
    TOKEN_TOTAL_TIME_SLICE,

	TOKEN_ENTRENCHMENT_BONUS,
	TOKEN_PARADROP_DISTANCE,
	TOKEN_PARADROP_SUCCESS_PERCENT,

	TOKEN_ELITE_SPY_BONUS,
	TOKEN_CITY_SECOND_ATTEMPT_SPY_BONUS,
	TOKEN_ASSASINATION_HAPPINESS_EFFECT,
	TOKEN_ASSASINATION_HAPPINESS_EFFECT_TIMER,
	TOKEN_SPIED_UPON_WARINESS_TIMER,

	TOKEN_MAXIMUM_PARTY_COST,
	TOKEN_MAXIMUM_PARTY_CHANCE,
	TOKEN_GOSSIP_MAP_RADIUS,
	TOKEN_HEAR_GOSSIP_CHANCE,

	TOKEN_FRANCHISE_EFFECT,
	TOKEN_TURNS_TO_SUE_FRANCHISE,

	TOKEN_SLAVER_ELITE_CHANCE,
	TOKEN_ELITE_SLAVER_BONUS,

	TOKEN_ABOLITIONIST_ELITE_CHANCE,
	TOKEN_ELITE_ABOLITIONIST_BONUS,
	TOKEN_WATCHFUL_CITY_SUCCESS_MODIFIER,
	TOKEN_WATCHFUL_CITY_DEATH_MODIFIER,
	TOKEN_WATCHFUL_CITY_TURNS,


	TOKEN_BIO_INFECTION_TURNS,
	TOKEN_NANO_INFECTION_TURNS,
	TOKEN_BIO_INFECTION_SPREAD_CHANCE,
	TOKEN_NANO_INFECTION_SPREAD_CHANCE,
	TOKEN_BIO_TERROR_KILL_PERCENTAGE,

	TOKEN_ELITE_TERRORIST_BONUS,
	TOKEN_TERRORIST_ELITE_CHANCE,
	
	TOKEN_NUKE_POPULATION_PERCENTAGE,

	TOKEN_SPECIAL_ACTION_MOVE_COST,
	TOKEN_CLERIC_CONVERSION_FACTOR,
	TOKEN_TELEVANGELIST_CONVERSION_FACTOR,

	TOKEN_REFORMATION_CHANCE,
	TOKEN_REFORMATION_DEATH_CHANCE,
	TOKEN_REFORMATION_HAPPINESS_TIME,
	TOKEN_REFORMATION_HAPPINESS_AMOUNT,

	TOKEN_UNCONVERTED_INDULGENCE_GOLD,
	TOKEN_UNCONVERTED_INDULGENCE_HAPPINESS,
	TOKEN_CONVERTED_INDULGENCE_GOLD,
	TOKEN_CONVERTED_INDULGENCE_HAPPINESS,
	TOKEN_OTHER_FAITH_INDULGENCE_GOLD,
	TOKEN_OTHER_FAITH_INDULGENCE_HAPPINESS,
	
	TOKEN_SOOTHSAY_HAPPY_AMOUNT,

	TOKEN_RUINS_BOX_WIDTH,
	TOKEN_RUINS_BOX_HEIGHT,
	TOKEN_RUINS_CHANCE_PER_BOX,
	
	TOKEN_MAX_RUINS_GOLD,
	TOKEN_MAX_RUINS_ADVANCE,
	TOKEN_MAX_RUINS_UNIT,

	TOKEN_UNIT_RUSH_MODIFIER,
	TOKEN_IMPROVEMENT_RUSH_MODIFIER,
	TOKEN_WONDER_RUSH_MODIFIER,

	TOKEN_BUILDING_PRODUCTION_TO_VALUE_MODIFIER,

	TOKEN_MONOPOLY_THRESHOLD,

	TOKEN_POPULATION_POLLUTION_WARNING_THRESHOLD,
	TOKEN_INDUSTRIAL_POLLUTION_WARNING_THRESHOLD,
	TOKEN_VERY_HAPPY_THRESHOLD,
	TOKEN_CITY_GROWTH_COEFFICIENT,
	TOKEN_RIOT_LEVEL,

	TOKEN_POWER_POINTS_TO_MATERIALS,

	TOKEN_MAX_AIRLIFT_STACK_SIZE,
	TOKEN_GOLD_FROM_PIRACY,
	TOKEN_NO_PIRACY_EXPIRES,
	TOKEN_SPACE_LAUNCH_COST,
	TOKEN_SPACE_LAND_COST,
	TOKEN_WORMHOLE_ORBIT_HEIGHT_PERCENTAGE,
	TOKEN_WORMHOLE_SPEED,
	TOKEN_WORMHOLE_RETURN_TIME,
	TOKEN_WORMHOLE_VISIBLE_TO_ALL_TURNS,
	TOKEN_MAX_GOVERNMENT_CHANGE_TURNS,

	TOKEN_POLLUTION_FORCES_ANARCHY,
	TOKEN_FOOD_TO_POLLUTION_COEF,

	TOKEN_EMANCIPATION_UNHAPPINESS_TURNS,
	TOKEN_EMANCIPATION_UNHAPPINESS_AMOUNT,

	TOKEN_CAPTURE_CITY_ADVANCE_CHANCE,
	TOKEN_CITY_HEAL_RATE,
	TOKEN_NORMAL_HEAL_RATE,
	TOKEN_LOCAL_POLLUTION_LEVEL,
	TOKEN_LOCAL_POLLUTION_CHANCE,
	TOKEN_ASSAULT_DESTROY_BUILDING_CHANCE,
	TOKEN_BOMBARD_DESTROY_BUILDING_CHANCE,
	TOKEN_ADVANCE_CHOICES_MIN,
	TOKEN_ADVANCE_CHOICES_MAX,

	TOKEN_ATTACK_CONVERTER_UNHAPPINESS_TURNS,
	TOKEN_ATTACK_CONVERTER_UNHAPPINESS_AMOUNT,

	TOKEN_MIN_START_DISTANCE_COEFFICIENT,
	TOKEN_MAX_START_DISTANCE_COEFFICIENT,
	TOKEN_MAX_SAME_TILES,
	TOKEN_COMBAT_VETERAN_CHANCE,

	TOKEN_STOP_TRADE_ROUNDS,
	TOKEN_LEAVE_OUR_LANDS_ROUNDS,
	TOKEN_REDUCE_POLLUTION_ROUNDS,
	TOKEN_CAPTURE_CITY_ROUNDS,
	TOKEN_END_POLLUTION_ROUNDS,
	TOKEN_ATTACK_ENEMY_ROUNDS,

	TOKEN_MIN_CONTINENT_START_SIZE,

	TOKEN_BOMBARD_KILL_POP_CHANCE,
	TOKEN_ASSAULT_KILL_POP_CHANCE,
	TOKEN_CAPTURE_KILL_POP_CHANCE,

	TOKEN_SCALED_POP_ANCIENT,
	TOKEN_SCALED_POP_RENAISSANCE,
	TOKEN_SCALED_POP_MODERN,
	TOKEN_SCALED_POP_GENETIC,
	TOKEN_SCALED_POP_DIAMOND,
	TOKEN_PIRACY_KILLS_TRADER_CHANCE,
	TOKEN_UPRISING_CHANCE_PER_UNGUARDED_SLAVE,
	TOKEN_MAX_DISBAND_SIZE,
	TOKEN_MAX_REQUESTS_PER_PLAYER_PER_TURN,

	TOKEN_SLAVES_PER_MILITARY_UNIT,
	TOKEN_MIN_ABSOLUTE_START_DISTANCE,

	TOKEN_MAP_SIZE_SMALL,
	TOKEN_MAP_SIZE_MEDIUM,
	TOKEN_MAP_SIZE_LARGE,
	TOKEN_MAP_SIZE_GIGANTIC,

	TOKEN_RAIL_LAUNCH_POLLUTION,
	TOKEN_SPACE_FUEL_COST,
	TOKEN_NON_SPACE_FUEL_COST,

	TOKEN_STARVATION_WARNING_FUDGE_FACTOR,
	TOKEN_MAINTENANCE_WARNING_FUDGE_FACTOR,
	TOKEN_SUPPORT_WARNING_FUDGE_FACTOR,

	TOKEN_RELATIVE_AI_PRODUCTION_CHEAT_COEF,
	TOKEN_RELATIVE_AI_SCIENCE_CHEAT_COEF,

	TOKEN_INCITE_REVOLUTION_GOLD_COEFFICIENT,
	TOKEN_INCITE_REVOLUTION_CAPITOL_PENALTY,

	TOKEN_NUKE_KILL_TILES,

	TOKEN_INCITE_UPRISING_GOLD_COEFFICIENT,
	TOKEN_INCITE_UPRISING_CAPITOL_PENALTY,

	TOKEN_BIO_INFECTION_UNHAPPINESS,
	TOKEN_MIN_ECO_PACT_VIOLATION_LEVEL,

	TOKEN_NANO_INFECTION_TERRORIST_DEATH_CHANCE,
	TOKEN_BIO_INFECTION_TERRORIST_DEATH_CHANCE,

	TOKEN_FLOOD_CHANGES_COAST_TO_WATER_CHANCE,

	TOKEN_AI_CHEAT_ECO_PACT_MIN,
	TOKEN_AI_CHEAT_ECO_PACT_MAX,

	TOKEN_MAX_ABSOLUTE_GROWTH_RATE,
	TOKEN_TRADE_DISTANCE_COEFFICIENT,
	TOKEN_MIN_LIKE_TILES_FOR_GOOD,

	TOKEN_MIN_GOOD_VALUE,
	TOKEN_MAX_GOOD_VALUE,

	TOKEN_PIRACY_WASTE_COEFFICIENT,

	TOKEN_BORDER_INT_RADIUS,
	TOKEN_BORDER_SQUARED_RADIUS,

	TOKEN_BIO_INFECTION_PRODUCTION_COEF,
	TOKEN_NANO_BUILDING_KILL_PERCENTAGE,

	TOKEN_BASE_STARVATION_PROTECTION,
	TOKEN_CARAVAN_COEF,

	TOKEN_POLLUTION_CAUSED_BY_NUKE,
	TOKEN_MAX_MATCH_LIST_CYCLES, // added DWT
	// Modified by kaan to address bug # 12
	TOKEN_MIN_TURNS_BETWEEN_REVOLT,

    TOKEN_CONST_MAX 

};



TokenData g_const_token_data [] = { 
	{TOKEN_CONST_FIRST, "CONST_DO_NOT_USE_FIRST"},
	{TOKEN_CONST_PERCENT_LAND, "PERCENT_LAND"}, 
	{TOKEN_CONST_PERCENT_CONTINENT, "PERCENT_CONTINENT"}, 

	{TOKEN_CONST_PERCENT_HOMOGENOUS,"PERCENT_HOMOGENOUS"},
	{TOKEN_CONST_WET_LEVEL, "WET_LEVEL"}, 
	{TOKEN_CONST_DRY_LEVEL, "DRY_LEVEL"}, 
	{TOKEN_CONST_MERIDIAN0,"MERIDIANA"},
	{TOKEN_CONST_MERIDIAN1, "MERIDIANB"}, 
	{TOKEN_CONST_MERIDIAN2, "MERIDIANC"},
	{TOKEN_CONST_MERIDIAN3, "MERIDIAND"},
	{TOKEN_CONST_MERIDIAN4, "MERIDIANE"},
	{TOKEN_CONST_MERIDIAN5, "MERIDIANF"},

	{TOKEN_CONST_HLEVELA, "HLEVELA"}, 
	{TOKEN_CONST_HLEVELB, "HLEVELB"}, 
	{TOKEN_CONST_HLEVELC, "HLEVELC"}, 
	{TOKEN_CONST_HLEVELD, "HLEVELD"}, 

	{TOKEN_CONST_FORESTWET, "FORESTWET"}, 
	{TOKEN_CONST_FORESTDRY, "FORESTDRY"}, 
	{TOKEN_CONST_GRASSWET, "GRASSWET"}, 
	{TOKEN_CONST_GRASSDRY, "GRASSDRY"}, 
	{TOKEN_CONST_PLAINSWET, "PLAINSWET"}, 
	{TOKEN_CONST_PLAINSDRY, "PLAINSDRY"}, 
	{TOKEN_CONST_DESERTWET, "DESERTWET"}, 
	{TOKEN_CONST_DESERTDRY, "DESERTDRY"}, 

	{TOKEN_CONST_WHITEWARM, "WHITEWARM"}, 
	{TOKEN_CONST_WHITECOLD, "WHITECOLD"}, 
	{TOKEN_CONST_BROWNWARM, "BROWNWARM"}, 
	{TOKEN_CONST_BROWNCOLD, "BROWNCOLD"}, 
	{TOKEN_CONST_TEMPERATURERANGEADJUSTWARM, "TEMPERATURERANGEADJUSTWARM"}, 
	{TOKEN_CONST_TEMPERATURERANGEADJUSTCOLD, "TEMPERATURERANGEADJUSTCOLD"}, 

	{TOKEN_CONST_RICHNESSFEWGOODS, "RICHNESSFEWGOODS"}, 
	{TOKEN_CONST_RICHNESSMANYGOODS, "RICHNESSMANYGOODS"}, 
	{TOKEN_CONST_RIVERCELLWIDTHFEWGOODS, "RIVERCELLWIDTHFEWGOODS"}, 
	{TOKEN_CONST_RIVERCELLWIDTHMANYGOODS, "RIVERCELLWIDTHMANYGOODS"}, 
	{TOKEN_CONST_RIVERCELLHEIGHTFEWGOODS, "RIVERCELLHEIGHTFEWGOODS"}, 
	{TOKEN_CONST_RIVERCELLHEIGHTMANYGOODS, "RIVERCELLHEIGHTMANYGOODS"}, 

	{TOKEN_CONST_PERCENT_MOUNTAIN, "PERCENT_MOUNTAIN"},
	{TOKEN_CONST_MOUNTAIN_CELL, "MOUNTAIN_CELL"},

	{TOKEN_CONST_PERCENT_HILLS, "PERCENT_HILLS"},
	{TOKEN_CONST_MOUNTAIN_SPREAD, "MOUNTAIN_SPREAD"},
	{TOKEN_CONST_MOUNTAIN_LENGTH, "MOUNTAIN_LENGTH"},
	{TOKEN_CONST_GLACIER_EXTENT, "GLACIER_EXTENT"},
	{TOKEN_CONST_PERCENT_VOLCANO, "PERCENT_VOLCANO"}, 
	{TOKEN_CONST_PERCENT_TRENCH, "PERCENT_TRENCH"}, 
	{TOKEN_CONST_PERCENT_FOREST, "PERCENT_FOREST"},
	{TOKEN_CONST_PERCENT_GRASS, "PERCENT_GRASS"},
	{TOKEN_CONST_PERCENT_PLAINS, "PERCENT_PLAINS"},
	{TOKEN_CONST_PERCENT_DESERT, "PERCENT_DESERT"},
	{TOKEN_CONST_PERCENT_WHITE, "PERCENT_WHITE"},
	{TOKEN_CONST_PERCENT_BROWN, "PERCENT_BROWN"},
	{TOKEN_CONST_TEMPERATURE_RANGE_ADJUST, "TEMPERATURE_RANGE_ADJUST"},
	{TOKEN_CONST_NICE_RADIUS, "NICE_RADIUS"}, 
	{TOKEN_CONST_PERCENT_RIVER, "PERCENT_RIVER"},
	{TOKEN_CONST_RIVER_LEN, "RIVER_LENGTH"},
	{TOKEN_CONST_RIVER_CELL_WIDTH, "RIVER_CELL_WIDTH"},
	{TOKEN_CONST_RIVER_CELL_HEIGHT, "RIVER_CELL_HEIGHT"},
	{TOKEN_CONST_RICHNESS, "RICHNESS"},
	{TOKEN_CONST_CHANCE_LOST_AT_SEA, "CHANCE_LOST_AT_SEA"},
	{TOKEN_CONST_VET_COEF, "VETERAN_COEF"},



	{TOKEN_CONST_DISCOVERIES_POLLUTION_SIZE_MODIFIER, "DISCOVERIES_POLLUTION_SIZE_MODIFIER"},
	{TOKEN_CONST_DISCOVERIES_POLLUTION_PRODUCTION_MODIFIER, "DISCOVERIES_POLLUTION_PRODUCTION_MODIFIER"},
	{TOKEN_CONST_AVERAGE_POLLUTION_TURNS, "AVERAGE_POLLUTION_TURNS"},
	{TOKEN_CONST_REVOLUTION_LEVEL, "REVOLUTION_LEVEL"},
	{TOKEN_CONST_TARIFF_REDUCTION, "TARIFF_REDUCTION"},
	{TOKEN_CONST_REVOLT_INFLUENCE_DISTANCE, "REVOLT_INFLUENCE_DISTANCE"},		

	{TOKEN_CONST_MAX_EMPIRE_DIST_COST, "MAX_EMPIRE_DIST_COST"},
	{TOKEN_CONST_HAPPINESS_LAUNCH_COST, "HAPPINESS_LAUNCH_COST"}, 
	{TOKEN_CONST_UNIT_WORKDAY, "UNIT_WORKDAY"},
	{TOKEN_CONST_UNIT_WAGES, "UNIT_WAGES"},
	{TOKEN_CONST_UNIT_RATIONS,"UNIT_RATIONS"},

    {TOKEN_CONST_BASE_WORKDAY, "BASE_WORKDAY"}, 
    {TOKEN_CONST_BASE_WAGES, "BASE_WAGES"}, 
    {TOKEN_CONST_BASE_RATIONS, "BASE_RATIONS"}, 

	{TOKEN_CONST_POPULATION_PRODUCES_POLLUTION_THRESHOLD, "POPULATION_PRODUCES_POLLUTION_THRESHOLD"},
	{TOKEN_CONST_PRODUCTION_PRODUCES_POLLUTION_THRESHOLD, "PRODUCTION_PRODUCES_POLLUTION_THRESHOLD"},

    {TOKEN_CHANGE_CURRENTLY_BUILDING_ITEM_PENALTY, "CHANGE_CURRENTLY_BUILDING_ITEM_PENALTY" }, 

	{TOKEN_CONST_PACT_CAPTURE_CITY_EXPIRES, "PACT_CAPTURE_CITY_EXPIRES"},

	{TOKEN_CONST_CEASE_FIRE_EXPIRES, "CEASE_FIRE_EXPIRES"},
	{TOKEN_CONST_SHORT_CEASE_FIRE_EXPIRES, "SHORT_CEASE_FIRE_EXPIRES"},
	{TOKEN_CONST_END_OF_GAME_YEAR_EARLY_WARNING, "END_OF_GAME_YEAR_EARLY_WARNING"},
	{TOKEN_CONST_END_OF_GAME_YEAR, "END_OF_GAME_YEAR"},
	{TOKEN_CONST_CAPTURE_CITY_FOR_GOLD_MULTIPLIER, "CAPTURE_CITY_FOR_GOLD_MULTIPLIER"},
	{TOKEN_CONST_PACT_END_POLLUTION_EXPIRES, "PACT_END_POLLUTION_EXPIRES"},
	{TOKEN_CONST_LEAVE_OUR_LANDS_EXPIRES, "LEAVE_OUR_LANDS_EXPIRES"},
	{TOKEN_CONST_REDUCE_POLLUTION_EXPIRES, "REDUCE_POLLUTION_EXPIRES"},

	{TOKEN_CONST_PATIENCE_LOST_PER_REQUEST, "PATIENCE_LOST_PER_REQUEST"},
	{TOKEN_CONST_PATIENCE_REGAINED_PER_ROUND, "PATIENCE_REGAINED_PER_ROUND"},
	{TOKEN_CONST_REGAIN_PATIENCE_PROBABILITY, "REGAIN_PATIENCE_PROBABILITY"},
	{TOKEN_CONST_PATIENCE_LOST_THRESHOLD, "PATIENCE_LOST_THRESHOLD"},

    {TOKEN_GOAL_TIME_SLICE, "AI_GOAL_TIME_SLICE"},     
    {TOKEN_MAX_TIME_SLICE, "AI_MAX_TIME_SLICE"},
    {TOKEN_TOTAL_TIME_SLICE, "AI_TOTAL_TIME_SLICE"},

	{TOKEN_ENTRENCHMENT_BONUS, "ENTRENCHMENT_BONUS"},
	{TOKEN_PARADROP_DISTANCE, "PARADROP_DISTANCE"},
	{TOKEN_PARADROP_SUCCESS_PERCENT, "PARADROP_SUCCESS_PERCENT"},

	{TOKEN_ELITE_SPY_BONUS, "ELITE_SPY_BONUS"},
	{TOKEN_CITY_SECOND_ATTEMPT_SPY_BONUS, "CITY_SECOND_ATTEMPT_SPY_BONUS"},
	{TOKEN_ASSASINATION_HAPPINESS_EFFECT, "ASSASINATION_HAPPINESS_EFFECT"},
	{TOKEN_ASSASINATION_HAPPINESS_EFFECT_TIMER, "ASSASINATION_HAPPINESS_EFFECT_TIMER"},
	{TOKEN_SPIED_UPON_WARINESS_TIMER, "SPIED_UPON_WARINESS_TIMER"},

	{TOKEN_MAXIMUM_PARTY_COST, "MAX_PARTY_COST"},
	{TOKEN_MAXIMUM_PARTY_CHANCE, "MAX_PARTY_CHANCE"},
	{TOKEN_GOSSIP_MAP_RADIUS, "GOSSIP_MAP_RADIUS"},
	{TOKEN_HEAR_GOSSIP_CHANCE, "HEAR_GOSSIP_CHANCE"},

	{TOKEN_FRANCHISE_EFFECT, "FRANCHISE_EFFECT"},
	{TOKEN_TURNS_TO_SUE_FRANCHISE, "TURNS_TO_SUE_FRANCHISE"},

	{TOKEN_SLAVER_ELITE_CHANCE, "SLAVER_ELITE_CHANCE"},
	{TOKEN_ELITE_SLAVER_BONUS, "ELITE_SLAVER_BONUS"},

	{TOKEN_ABOLITIONIST_ELITE_CHANCE, "ABOLITIONIST_ELITE_CHANCE"},
	{TOKEN_ELITE_ABOLITIONIST_BONUS, "ELITE_ABOLITIONIST_BONUS"},
	{TOKEN_WATCHFUL_CITY_SUCCESS_MODIFIER, "WATCHFUL_CITY_SUCCESS_MODIFIER"},
	{TOKEN_WATCHFUL_CITY_DEATH_MODIFIER, "WATCHFUL_CITY_DEATH_MODIFIER"},
	{TOKEN_WATCHFUL_CITY_TURNS, "WATCHFUL_CITY_TURNS"},

	{TOKEN_BIO_INFECTION_TURNS, "BIO_INFECTION_TURNS"},
	{TOKEN_NANO_INFECTION_TURNS, "NANO_INFECTION_TURNS"},
	{TOKEN_BIO_INFECTION_SPREAD_CHANCE, "BIO_INFECTION_SPREAD_CHANCE"},
	{TOKEN_NANO_INFECTION_SPREAD_CHANCE, "NANO_INFECTION_SPREAD_CHANCE"},
	{TOKEN_BIO_TERROR_KILL_PERCENTAGE, "PLAGUE_KILL_PERCENTAGE"},

	{TOKEN_ELITE_TERRORIST_BONUS, "ELITE_TERRORIST_BONUS"},
	{TOKEN_TERRORIST_ELITE_CHANCE, "TERRORIST_ELITE_CHANCE"},
	{TOKEN_NUKE_POPULATION_PERCENTAGE, "NUKE_POPULATION_PERCENTAGE"},
	{TOKEN_SPECIAL_ACTION_MOVE_COST, "SPECIAL_ACTION_MOVE_COST"},

	{TOKEN_CLERIC_CONVERSION_FACTOR, "CLERIC_CONVERSION_FACTOR"},
	{TOKEN_TELEVANGELIST_CONVERSION_FACTOR, "TELEVANGELIST_CONVERSION_FACTOR"},

	{TOKEN_REFORMATION_CHANCE, "REFORMATION_CHANCE"},
	{TOKEN_REFORMATION_DEATH_CHANCE, "REFORMATION_DEATH_CHANCE"},
	{TOKEN_REFORMATION_HAPPINESS_TIME, "REFORMATION_HAPPINESS_TIME"},
	{TOKEN_REFORMATION_HAPPINESS_AMOUNT, "REFORMATION_HAPPINESS_AMOUNT"},

	{TOKEN_UNCONVERTED_INDULGENCE_GOLD, "UNCONVERTED_INDULGENCE_GOLD"},
	{TOKEN_UNCONVERTED_INDULGENCE_HAPPINESS, "UNCONVERTED_INDULGENCE_HAPPINESS"},
	{TOKEN_CONVERTED_INDULGENCE_GOLD, "CONVERTED_INDULGENCE_GOLD"},
	{TOKEN_CONVERTED_INDULGENCE_HAPPINESS, "CONVERTED_INDULGENCE_HAPPINESS"},
	{TOKEN_OTHER_FAITH_INDULGENCE_GOLD, "OTHER_FAITH_INDULGENCE_GOLD"},
	{TOKEN_OTHER_FAITH_INDULGENCE_HAPPINESS, "OTHER_FAITH_INDULGENCE_HAPPINESS"},
	{TOKEN_SOOTHSAY_HAPPY_AMOUNT, "SOOTHSAY_HAPPY_AMOUNT"},

	{TOKEN_RUINS_BOX_WIDTH, "RUINS_BOX_WIDTH"},
	{TOKEN_RUINS_BOX_HEIGHT, "RUINS_BOX_HEIGHT"},
	{TOKEN_RUINS_CHANCE_PER_BOX, "RUINS_CHANCE_PER_BOX"},

	{TOKEN_MAX_RUINS_GOLD, "MAX_RUINS_GOLD"},
	{TOKEN_MAX_RUINS_ADVANCE, "MAX_RUINS_ADVANCE"},
	{TOKEN_MAX_RUINS_UNIT, "MAX_RUINS_UNIT"},

	{TOKEN_UNIT_RUSH_MODIFIER, "UNIT_RUSH_MODIFIER"},
	{TOKEN_IMPROVEMENT_RUSH_MODIFIER, "IMPROVEMENT_RUSH_MODIFIER"},
	{TOKEN_WONDER_RUSH_MODIFIER, "WONDER_RUSH_MODIFIER"},
	{TOKEN_BUILDING_PRODUCTION_TO_VALUE_MODIFIER, "BUILDING_PRODUCTION_TO_VALUE_MODIFIER"},
	
	{TOKEN_MONOPOLY_THRESHOLD, "MONOPOLY_THRESHOLD"},
	{TOKEN_POPULATION_POLLUTION_WARNING_THRESHOLD, "POPULATION_POLLUTION_WARNING_THRESHOLD"},
	{TOKEN_INDUSTRIAL_POLLUTION_WARNING_THRESHOLD, "INDUSTRIAL_POLLUTION_WARNING_THRESHOLD"},
	{TOKEN_VERY_HAPPY_THRESHOLD, "VERY_HAPPY_THRESHOLD"},
	{TOKEN_CITY_GROWTH_COEFFICIENT, "CITY_GROWTH_COEFFICIENT"},
	{TOKEN_RIOT_LEVEL, "RIOT_LEVEL"},
	{TOKEN_POWER_POINTS_TO_MATERIALS, "POWER_POINTS_TO_MATERIALS"},
	{TOKEN_MAX_AIRLIFT_STACK_SIZE, "MAX_AIRLIFT_STACK_SIZE"},
	{TOKEN_GOLD_FROM_PIRACY, "GOLD_FROM_PIRACY"},
	{TOKEN_NO_PIRACY_EXPIRES, "NO_PIRACY_EXPIRES"},
	{TOKEN_SPACE_LAUNCH_COST, "SPACE_LAUNCH_COST"},
	{TOKEN_SPACE_LAND_COST, "SPACE_LAND_COST"},
	{TOKEN_WORMHOLE_ORBIT_HEIGHT_PERCENTAGE, "WORMHOLE_ORBIT_HEIGHT_PERCENTAGE"},
	{TOKEN_WORMHOLE_SPEED, "WORMHOLE_SPEED"},
	{TOKEN_WORMHOLE_RETURN_TIME, "WORMHOLE_RETURN_TIME"},
	{TOKEN_WORMHOLE_VISIBLE_TO_ALL_TURNS, "WORMHOLE_VISIBLE_TO_ALL_TURNS"},
	{TOKEN_MAX_GOVERNMENT_CHANGE_TURNS, "MAX_GOVERNMENT_CHANGE_TURNS"},

	{TOKEN_POLLUTION_FORCES_ANARCHY, "POLLUTION_FORCES_ANARCHY"},
	{TOKEN_FOOD_TO_POLLUTION_COEF, "FOOD_TO_POLLUTION_COEF"},
	{TOKEN_EMANCIPATION_UNHAPPINESS_TURNS, "EMANCIPATION_UNHAPPINESS_TURNS"},
	{TOKEN_EMANCIPATION_UNHAPPINESS_AMOUNT, "EMANCIPATION_UNHAPPINESS_AMOUNT"},
	{TOKEN_CAPTURE_CITY_ADVANCE_CHANCE, "CAPTURE_CITY_ADVANCE_CHANCE"},
	{TOKEN_CITY_HEAL_RATE, "CITY_HEAL_RATE"},
	{TOKEN_NORMAL_HEAL_RATE, "NORMAL_HEAL_RATE"},
	{TOKEN_LOCAL_POLLUTION_LEVEL, "LOCAL_POLLUTION_LEVEL"},
	{TOKEN_LOCAL_POLLUTION_CHANCE, "LOCAL_POLLUTION_CHANCE"},
	{TOKEN_ASSAULT_DESTROY_BUILDING_CHANCE, "ASSAULT_DESTROY_BUILDING_CHANCE"},
	{TOKEN_BOMBARD_DESTROY_BUILDING_CHANCE, "BOMBARD_DESTROY_BUILDING_CHANCE"},
	{TOKEN_ADVANCE_CHOICES_MIN, "ADVANCE_CHOICES_MIN"},
	{TOKEN_ADVANCE_CHOICES_MAX, "ADVANCE_CHOICES_MAX"},
	{TOKEN_ATTACK_CONVERTER_UNHAPPINESS_TURNS, "ATTACK_CONVERTER_UNHAPPINESS_TURNS"},
	{TOKEN_ATTACK_CONVERTER_UNHAPPINESS_AMOUNT, "ATTACK_CONVERTER_UNHAPPINESS_AMOUNT"},
	{TOKEN_MIN_START_DISTANCE_COEFFICIENT, "MIN_START_DISTANCE_COEFFICIENT"},
	{TOKEN_MAX_START_DISTANCE_COEFFICIENT, "MAX_START_DISTANCE_COEFFICIENT"},
	{TOKEN_MAX_SAME_TILES, "MAX_SAME_TILES"},
	{TOKEN_COMBAT_VETERAN_CHANCE, "COMBAT_VETERAN_CHANCE"},

	{TOKEN_STOP_TRADE_ROUNDS, "STOP_TRADE_ROUNDS"},
	{TOKEN_LEAVE_OUR_LANDS_ROUNDS, "LEAVE_OUR_LANDS_ROUNDS"},
	{TOKEN_REDUCE_POLLUTION_ROUNDS, "REDUCE_POLLUTION_ROUNDS"},
	{TOKEN_CAPTURE_CITY_ROUNDS, "CAPTURE_CITY_ROUNDS"},
	{TOKEN_END_POLLUTION_ROUNDS, "END_POLLUTION_ROUNDS"},
	{TOKEN_ATTACK_ENEMY_ROUNDS, "ATTACK_ENEMY_ROUNDS"},
	{TOKEN_MIN_CONTINENT_START_SIZE, "MIN_CONTINENT_START_SIZE"},

	{TOKEN_BOMBARD_KILL_POP_CHANCE, "BOMBARD_KILL_POP_CHANCE"},
	{TOKEN_ASSAULT_KILL_POP_CHANCE, "ASSAULT_KILL_POP_CHANCE"},
	{TOKEN_CAPTURE_KILL_POP_CHANCE, "CAPTURE_KILL_POP_CHANCE"},

	{TOKEN_SCALED_POP_ANCIENT, "SCALED_POP_ANCIENT"},
	{TOKEN_SCALED_POP_RENAISSANCE, "SCALED_POP_RENAISSANCE"},
	{TOKEN_SCALED_POP_MODERN, "SCALED_POP_MODERN"},
	{TOKEN_SCALED_POP_GENETIC, "SCALED_POP_GENETIC"},
	{TOKEN_SCALED_POP_DIAMOND, "SCALED_POP_DIAMOND"},
	{TOKEN_PIRACY_KILLS_TRADER_CHANCE, "PIRACY_KILLS_TRADER_CHANCE"},
	{TOKEN_UPRISING_CHANCE_PER_UNGUARDED_SLAVE, "UPRISING_CHANCE_PER_UNGUARDED_SLAVE"},
	{TOKEN_MAX_DISBAND_SIZE, "MAX_DISBAND_SIZE"},
	{TOKEN_MAX_REQUESTS_PER_PLAYER_PER_TURN, "MAX_REQUESTS_PER_PLAYER_PER_TURN"},
	{TOKEN_SLAVES_PER_MILITARY_UNIT, "SLAVES_PER_MILITARY_UNIT"},
	{TOKEN_MIN_ABSOLUTE_START_DISTANCE, "MIN_ABSOLUTE_START_DISTANCE"},

	{TOKEN_MAP_SIZE_SMALL, "MAP_SIZE_SMALL"},
	{TOKEN_MAP_SIZE_MEDIUM, "MAP_SIZE_MEDIUM"},
	{TOKEN_MAP_SIZE_LARGE, "MAP_SIZE_LARGE"},
	{TOKEN_MAP_SIZE_GIGANTIC, "MAP_SIZE_GIGANTIC"},
	{TOKEN_RAIL_LAUNCH_POLLUTION, "RAIL_LAUNCH_POLLUTION"},
	{TOKEN_SPACE_FUEL_COST, "SPACE_FUEL_COST"},
	{TOKEN_NON_SPACE_FUEL_COST, "NON_SPACE_FUEL_COST"},
	{TOKEN_STARVATION_WARNING_FUDGE_FACTOR, "STARVATION_WARNING_FUDGE_FACTOR"},
	{TOKEN_MAINTENANCE_WARNING_FUDGE_FACTOR, "MAINTENANCE_WARNING_FUDGE_FACTOR"},
	{TOKEN_SUPPORT_WARNING_FUDGE_FACTOR, "SUPPORT_WARNING_FUDGE_FACTOR"},
	{TOKEN_RELATIVE_AI_PRODUCTION_CHEAT_COEF, "RELATIVE_AI_PRODUCTION_CHEAT_COEF"},
	{TOKEN_RELATIVE_AI_SCIENCE_CHEAT_COEF, "RELATIVE_AI_SCIENCE_CHEAT_COEF"},
	{TOKEN_INCITE_REVOLUTION_GOLD_COEFFICIENT, "INCITE_REVOLUTION_GOLD_COEFFICIENT"},
	{TOKEN_INCITE_REVOLUTION_CAPITOL_PENALTY, "INCITE_REVOLUTION_CAPITOL_PENALTY"},

	{TOKEN_NUKE_KILL_TILES, "NUKE_KILL_TILES"},
	{TOKEN_INCITE_UPRISING_GOLD_COEFFICIENT, "INCITE_UPRISING_GOLD_COEFFICIENT"},
	{TOKEN_INCITE_UPRISING_CAPITOL_PENALTY, "INCITE_UPRISING_CAPITOL_PENALTY"},
	{TOKEN_BIO_INFECTION_UNHAPPINESS, "BIO_INFECTION_UNHAPPINESS"},
	{TOKEN_MIN_ECO_PACT_VIOLATION_LEVEL, "MIN_ECO_PACT_VIOLATION_LEVEL"},
	{TOKEN_NANO_INFECTION_TERRORIST_DEATH_CHANCE, "NANO_INFECTION_TERRORIST_DEATH_CHANCE"},
	{TOKEN_BIO_INFECTION_TERRORIST_DEATH_CHANCE, "BIO_INFECTION_TERRORIST_DEATH_CHANCE"},
	{TOKEN_FLOOD_CHANGES_COAST_TO_WATER_CHANCE, "FLOOD_CHANGES_COAST_TO_WATER_CHANCE"},

	{TOKEN_AI_CHEAT_ECO_PACT_MIN, "AI_CHEAT_ECO_PACT_MIN"},
	{TOKEN_AI_CHEAT_ECO_PACT_MAX, "AI_CHEAT_ECO_PACT_MAX"},

    {TOKEN_MAX_ABSOLUTE_GROWTH_RATE, "MAX_ABSOLUTE_GROWTH_RATE"},
    {TOKEN_TRADE_DISTANCE_COEFFICIENT, "TRADE_DISTANCE_COEFFICIENT"},
    {TOKEN_MIN_LIKE_TILES_FOR_GOOD, "MIN_LIKE_TILES_FOR_GOOD"},
    {TOKEN_MIN_GOOD_VALUE, "MIN_GOOD_VALUE"},
    {TOKEN_MAX_GOOD_VALUE, "MAX_GOOD_VALUE"},
    {TOKEN_PIRACY_WASTE_COEFFICIENT, "PIRACY_WASTE_COEFFICIENT"},
    {TOKEN_BORDER_INT_RADIUS, "BORDER_INT_RADIUS"},
    {TOKEN_BORDER_SQUARED_RADIUS, "BORDER_SQUARED_RADIUS"},
    {TOKEN_BIO_INFECTION_PRODUCTION_COEF, "BIO_INFECTION_PRODUCTION_COEF"},
    {TOKEN_NANO_BUILDING_KILL_PERCENTAGE, "NANO_BUILDING_KILL_PERCENTAGE"},
    {TOKEN_BASE_STARVATION_PROTECTION, "BASE_STARVATION_PROTECTION"},
    {TOKEN_CARAVAN_COEF, "CARAVAN_COEF"},
    {TOKEN_POLLUTION_CAUSED_BY_NUKE, "POLLUTION_CAUSED_BY_NUKE"},
	{TOKEN_MAX_MATCH_LIST_CYCLES, "MAX_MATCH_LIST_CYCLES"}, // added DWT
	// Modified by kaan to address bug # 12
	{TOKEN_MIN_TURNS_BETWEEN_REVOLT, "MIN_TURNS_BETWEEN_REVOLT"},
};
	































sint32 ConstDB::ParseConstDB(Token *const_token)
{ 
	
	Assert(const_token); 	
	sint32 tmp; 
	
	if (const_token->GetType() != TOKEN_CONST_PERCENT_LAND) { 
		c3errors_ErrorDialog  (const_token->ErrStr(), "Expected a value");
		return FALSE; 
	} else { 
		if (const_token->Next() != TOKEN_NUMBER) {  
			c3errors_ErrorDialog  (const_token->ErrStr(), "Expected a value"); 
			return FALSE; 
		} else { 
			const_token->GetNumber(tmp); 
		} 
	} 

    if ((tmp < 0) || (100 < tmp)) c3errors_ErrorDialog  (const_token->ErrStr(), "invalid range for percent") ;	
	m_land = tmp * 0.01f;
	
	if (!token_ParseValNext(const_token, TOKEN_CONST_PERCENT_CONTINENT, tmp)) return FALSE;    
    if ((tmp < 0) || (100 < tmp)) c3errors_ErrorDialog  (const_token->ErrStr(), "invalid range for percent") ;
	m_continent = tmp * 0.01f;
	
	
	if (!token_ParseValNext(const_token, TOKEN_CONST_PERCENT_HOMOGENOUS, tmp)) return FALSE;    
    if ((tmp < 0) || (100 < tmp)) c3errors_ErrorDialog  (const_token->ErrStr(), "invalid range for percent") ;
    m_homogenous = tmp * 0.01f;
	
	
	if (!token_ParseValNext(const_token, TOKEN_CONST_MERIDIAN0, tmp)) return FALSE;    
    if ((tmp < 0) || (100 < tmp)) c3errors_ErrorDialog  (const_token->ErrStr(), "invalid range for percent") ;
	m_meridf[0] = tmp * 0.01f;

	if (!token_ParseValNext(const_token, TOKEN_CONST_MERIDIAN1, tmp)) return FALSE;    
    if ((tmp < 0) || (100 < tmp)) c3errors_ErrorDialog  (const_token->ErrStr(), "invalid range for percent") ;
    m_meridf[1] = tmp * 0.01f;
	if (!token_ParseValNext(const_token, TOKEN_CONST_MERIDIAN2, tmp)) return FALSE;    
    if ((tmp < 0) || (100 < tmp)) c3errors_ErrorDialog  (const_token->ErrStr(), "invalid range for percent") ;
	m_meridf[2] = tmp * 0.01f;
	if (!token_ParseValNext(const_token, TOKEN_CONST_MERIDIAN3, tmp)) return FALSE;    
    if ((tmp < 0) || (100 < tmp)) c3errors_ErrorDialog  (const_token->ErrStr(), "invalid range for percent") ;
	m_meridf[3] = tmp * 0.01f;
	if (!token_ParseValNext(const_token, TOKEN_CONST_MERIDIAN4, tmp)) return FALSE;            
    if ((tmp < 0) || (100 < tmp)) c3errors_ErrorDialog  (const_token->ErrStr(), "invalid range for percent") ;
	m_meridf[4] = tmp * 0.01f;
	if (!token_ParseValNext(const_token, TOKEN_CONST_MERIDIAN5, tmp)) return FALSE;    
    if ((tmp < 0) || (100 < tmp)) c3errors_ErrorDialog  (const_token->ErrStr(), "invalid range for percent") ;
    m_meridf[5] = tmp * 0.01f;
	
	
	if (!token_ParseValNext(const_token, TOKEN_CONST_HLEVELA, tmp)) return FALSE;    
    if ((tmp < 0) || (100 < tmp)) c3errors_ErrorDialog  (const_token->ErrStr(), "invalid range for percent") ;
    m_hlevelf[0] = tmp * 0.01f;
	if (!token_ParseValNext(const_token, TOKEN_CONST_HLEVELB, tmp)) return FALSE;    
    if ((tmp < 0) || (100 < tmp)) c3errors_ErrorDialog  (const_token->ErrStr(), "invalid range for percent") ;
    m_hlevelf[1] = tmp * 0.01f;
	if (!token_ParseValNext(const_token, TOKEN_CONST_HLEVELC, tmp)) return FALSE;    
    if ((tmp < 0) || (100 < tmp)) c3errors_ErrorDialog  (const_token->ErrStr(), "invalid range for percent") ;
    m_hlevelf[2] = tmp * 0.01f;
	if (!token_ParseValNext(const_token, TOKEN_CONST_HLEVELD, tmp)) return FALSE;    
    if ((tmp < 0) || (100 < tmp)) c3errors_ErrorDialog  (const_token->ErrStr(), "invalid range for percent") ;
    m_hlevelf[3] = tmp * 0.01f;
	
	if (!token_ParseValNext(const_token, TOKEN_CONST_FORESTWET, tmp)) return FALSE;    
    if ((tmp < 0) || (100 < tmp)) c3errors_ErrorDialog  (const_token->ErrStr(), "invalid range for percent") ;
    m_forestWet = (sint8)tmp;
	if (!token_ParseValNext(const_token, TOKEN_CONST_GRASSWET, tmp)) return FALSE;    
    if ((tmp < 0) || (100 < tmp)) c3errors_ErrorDialog  (const_token->ErrStr(), "invalid range for percent") ;
    m_grassWet = (sint8)tmp;
	if (!token_ParseValNext(const_token, TOKEN_CONST_PLAINSWET, tmp)) return FALSE;    
    if ((tmp < 0) || (100 < tmp)) c3errors_ErrorDialog  (const_token->ErrStr(), "invalid range for percent") ;
    m_plainsWet = (sint8)tmp;
	if (!token_ParseValNext(const_token, TOKEN_CONST_DESERTWET, tmp)) return FALSE;    
    if ((tmp < 0) || (100 < tmp)) c3errors_ErrorDialog  (const_token->ErrStr(), "invalid range for percent") ;
    m_desertWet = (sint8)tmp;
	if (!token_ParseValNext(const_token, TOKEN_CONST_FORESTDRY, tmp)) return FALSE;    
    if ((tmp < 0) || (100 < tmp)) c3errors_ErrorDialog  (const_token->ErrStr(), "invalid range for percent") ;
    m_forestDry = (sint8)tmp;
	if (!token_ParseValNext(const_token, TOKEN_CONST_GRASSDRY, tmp)) return FALSE;    
    if ((tmp < 0) || (100 < tmp)) c3errors_ErrorDialog  (const_token->ErrStr(), "invalid range for percent") ;
    m_grassDry = (sint8)tmp;
	if (!token_ParseValNext(const_token, TOKEN_CONST_PLAINSDRY, tmp)) return FALSE;    
    if ((tmp < 0) || (100 < tmp)) c3errors_ErrorDialog  (const_token->ErrStr(), "invalid range for percent") ;
    m_plainsDry = (sint8)tmp;
	if (!token_ParseValNext(const_token, TOKEN_CONST_DESERTDRY, tmp)) return FALSE;    
    if ((tmp < 0) || (100 < tmp)) c3errors_ErrorDialog  (const_token->ErrStr(), "invalid range for percent") ;
    m_desertDry = (sint8)tmp;

	if (!token_ParseValNext(const_token, TOKEN_CONST_WHITEWARM, tmp)) return FALSE;    
    if ((tmp < 0) || (100 < tmp)) c3errors_ErrorDialog  (const_token->ErrStr(), "invalid range for percent") ;
    m_whiteWarm = (sint8)tmp;
	if (!token_ParseValNext(const_token, TOKEN_CONST_BROWNWARM, tmp)) return FALSE;    
    if ((tmp < 0) || (100 < tmp)) c3errors_ErrorDialog  (const_token->ErrStr(), "invalid range for percent") ;
    m_brownWarm = (sint8)tmp;
	if (!token_ParseValNext(const_token, TOKEN_CONST_TEMPERATURERANGEADJUSTWARM, tmp)) return FALSE;    


    m_temperatureRangeAdjustWarm = (sint8)tmp;
	if (!token_ParseValNext(const_token, TOKEN_CONST_WHITECOLD, tmp)) return FALSE;    
    if ((tmp < 0) || (100 < tmp)) c3errors_ErrorDialog  (const_token->ErrStr(), "invalid range for percent") ;
    m_whiteCold = (sint8)tmp;
	if (!token_ParseValNext(const_token, TOKEN_CONST_BROWNCOLD, tmp)) return FALSE;    
    if ((tmp < 0) || (100 < tmp)) c3errors_ErrorDialog  (const_token->ErrStr(), "invalid range for percent") ;
    m_brownCold = (sint8)tmp;
	if (!token_ParseValNext(const_token, TOKEN_CONST_TEMPERATURERANGEADJUSTCOLD, tmp)) return FALSE;    


    m_temperatureRangeAdjustCold = (sint8)tmp;

	if (!token_ParseValNext(const_token, TOKEN_CONST_RICHNESSFEWGOODS, tmp)) return FALSE;    


    m_richnessFewgoods = (sint8)tmp;
	if (!token_ParseValNext(const_token, TOKEN_CONST_RIVERCELLWIDTHFEWGOODS, tmp)) return FALSE;    


    m_riverCellWidthFewgoods = (sint8)tmp;
	if (!token_ParseValNext(const_token, TOKEN_CONST_RIVERCELLHEIGHTFEWGOODS, tmp)) return FALSE;    


    m_riverCellHeightFewgoods = (sint8)tmp;
	if (!token_ParseValNext(const_token, TOKEN_CONST_RICHNESSMANYGOODS, tmp)) return FALSE;    


    m_richnessManygoods = (sint8)tmp;
	if (!token_ParseValNext(const_token, TOKEN_CONST_RIVERCELLWIDTHMANYGOODS, tmp)) return FALSE;    


    m_riverCellWidthManygoods = (sint8)tmp;
	if (!token_ParseValNext(const_token, TOKEN_CONST_RIVERCELLHEIGHTMANYGOODS, tmp)) return FALSE;    


    m_riverCellHeightManygoods = (sint8)tmp;

	if (!token_ParseValNext(const_token, TOKEN_CONST_PERCENT_MOUNTAIN, tmp)) return FALSE;    
    if ((tmp < 0) || (100 < tmp)) c3errors_ErrorDialog  (const_token->ErrStr(), "invalid range for percent") ;
    m_mountain = tmp * 0.01f;
	
	if (!token_ParseValNext(const_token, TOKEN_CONST_MOUNTAIN_CELL, m_mount_cell)) return FALSE;    
	
	if (!token_ParseValNext(const_token, TOKEN_CONST_PERCENT_HILLS, tmp)) return FALSE;    
    if ((tmp < 0) || (100 < tmp)) c3errors_ErrorDialog  (const_token->ErrStr(), "invalid range for percent") ;
	m_hills = tmp * 0.01;
	
	if (!token_ParseValNext(const_token, TOKEN_CONST_MOUNTAIN_SPREAD, m_mountain_spread)) return FALSE;    
	
	if (!token_ParseValNext(const_token, TOKEN_CONST_MOUNTAIN_LENGTH, m_mountain_length)) return FALSE;    
	if (!token_ParseValNext(const_token, TOKEN_CONST_GLACIER_EXTENT, m_glacier)) return FALSE;    
	if (!token_ParseValNext(const_token, TOKEN_CONST_PERCENT_VOLCANO, m_volcano)) return FALSE;    
	if (!token_ParseValNext(const_token, TOKEN_CONST_PERCENT_TRENCH, tmp)) return FALSE;    
    if ((tmp < 0) || (100 < tmp)) c3errors_ErrorDialog  (const_token->ErrStr(), "invalid range for percent") ;
	m_trench = tmp * 0.01;
	if(!token_ParseValNext(const_token, TOKEN_CONST_PERCENT_FOREST, m_forest)) return FALSE;
	if(!token_ParseValNext(const_token, TOKEN_CONST_PERCENT_GRASS, m_grass)) return FALSE;
	if(!token_ParseValNext(const_token, TOKEN_CONST_PERCENT_PLAINS, m_plains)) return FALSE;
	if(!token_ParseValNext(const_token, TOKEN_CONST_PERCENT_DESERT, m_desert)) return FALSE;
	if(!token_ParseValNext(const_token, TOKEN_CONST_PERCENT_WHITE, m_whitePercent)) return FALSE;
	if(!token_ParseValNext(const_token, TOKEN_CONST_PERCENT_BROWN, m_brownPercent)) return FALSE;
	if(!token_ParseValNext(const_token, TOKEN_CONST_TEMPERATURE_RANGE_ADJUST, m_temperatureRangeAdjust)) return FALSE;
	
	
	if (!token_ParseValNext(const_token, TOKEN_CONST_NICE_RADIUS, m_nice_r)) return FALSE;    
	if (!token_ParseValNext(const_token, TOKEN_CONST_PERCENT_RIVER, tmp)) return FALSE;    
    if ((tmp < 0) || (100 < tmp)) c3errors_ErrorDialog  (const_token->ErrStr(), "invalid range for percent") ;
    m_river = tmp * 0.01f;
	if (!token_ParseValNext(const_token, TOKEN_CONST_RIVER_LEN, m_river_len)) return FALSE;    
	
	if(!token_ParseValNext(const_token, TOKEN_CONST_RIVER_CELL_WIDTH, m_river_cell_width)) return FALSE;
	if(!token_ParseValNext(const_token, TOKEN_CONST_RIVER_CELL_HEIGHT, m_river_cell_height)) return FALSE;

	if (!token_ParseValNext(const_token, TOKEN_CONST_RICHNESS, m_richness)) return FALSE;    
	
	if (!token_ParseValNext(const_token, TOKEN_CONST_CHANCE_LOST_AT_SEA, m_lost_at_sea)) return FALSE;    
	if (!token_ParseValNext(const_token, TOKEN_CONST_VET_COEF, tmp)) return FALSE;    
	m_vet_coef = tmp * 0.01f;

	
	if (!token_ParseValNext(const_token, TOKEN_CONST_REVOLUTION_LEVEL, m_revolution_level)) return FALSE ;

	


	
	
	if (!token_ParseFloatNext(const_token, TOKEN_CONST_DISCOVERIES_POLLUTION_SIZE_MODIFIER, m_discoveries_pollution_size_modifier)) return FALSE ;
	if (!token_ParseFloatNext(const_token, TOKEN_CONST_DISCOVERIES_POLLUTION_PRODUCTION_MODIFIER, m_discoveries_pollution_production_modifier)) return FALSE ;

	if (!token_ParseValNext(const_token, TOKEN_CONST_AVERAGE_POLLUTION_TURNS, tmp))
		return (FALSE) ;

	if (tmp < 0 || tmp > k_MAX_EVENT_POLLUTION_TURNS)
		{
        c3errors_ErrorDialog  (const_token->ErrStr(), "invalid range for AVERAGE_POLLUTION_TURNS") ;
        return (FALSE) ;
		}

	m_average_pollution_turns = tmp ;

	if (!token_ParseValNext(const_token, TOKEN_CONST_POPULATION_PRODUCES_POLLUTION_THRESHOLD, m_cityPopulationProducesPollutionThreshold)) return (FALSE) ;
	if (!token_ParseValNext(const_token, TOKEN_CONST_PRODUCTION_PRODUCES_POLLUTION_THRESHOLD, m_cityProductionProducesPollutionThreshold)) return (FALSE) ;
    if (!token_ParseFloatNext(const_token, TOKEN_CONST_MAX_EMPIRE_DIST_COST, m_max_empire_dist_cost)) return FALSE ;
    if (!token_ParseFloatNext(const_token, TOKEN_CONST_HAPPINESS_LAUNCH_COST, m_happiness_launch_cost)) return FALSE ;
    if (!token_ParseFloatNext(const_token, TOKEN_CONST_UNIT_WORKDAY, m_unit_workday)) return FALSE ;
    if (!token_ParseFloatNext(const_token, TOKEN_CONST_BASE_WORKDAY, m_base_workday)) return FALSE ;        

    if (!token_ParseFloatNext(const_token, TOKEN_CONST_UNIT_WAGES, m_unit_wages)) return FALSE ;
    if (!token_ParseFloatNext(const_token, TOKEN_CONST_BASE_WAGES, m_base_wages)) return FALSE ;        

    if (!token_ParseFloatNext(const_token, TOKEN_CONST_UNIT_RATIONS, m_unit_rations)) return FALSE ;        
    if (!token_ParseFloatNext(const_token, TOKEN_CONST_BASE_RATIONS, m_base_rations)) return FALSE ;        

    if (!token_ParseFloatNext(const_token, TOKEN_CHANGE_CURRENTLY_BUILDING_ITEM_PENALTY, m_change_currently_building_item_penalty)) return FALSE ;

    m_change_currently_building_item_penalty = min(1.0, max(0.0, 1.0 - m_change_currently_building_item_penalty * 0.01)); 

	
	if (!token_ParseValNext(const_token, TOKEN_CONST_TARIFF_REDUCTION, m_tariff_reduction)) return FALSE ;
	
	if (!token_ParseValNext(const_token, TOKEN_CONST_REVOLT_INFLUENCE_DISTANCE, m_revolt_influence_distance)) return FALSE ;

	
	if (!token_ParseValNext(const_token, TOKEN_CONST_PACT_CAPTURE_CITY_EXPIRES, m_pact_capture_city_expires)) return (FALSE) ;

	if (!token_ParseValNext(const_token, TOKEN_CONST_CEASE_FIRE_EXPIRES, m_cease_fire_expires)) return (FALSE) ;
	if (!token_ParseValNext(const_token, TOKEN_CONST_SHORT_CEASE_FIRE_EXPIRES, m_short_cease_fire_expires)) return (FALSE) ;
	if (!token_ParseValNext(const_token, TOKEN_CONST_END_OF_GAME_YEAR_EARLY_WARNING, m_end_of_game_year_early_warning)) return (FALSE) ;
	if (!token_ParseValNext(const_token, TOKEN_CONST_END_OF_GAME_YEAR, m_end_of_game_year)) return (FALSE) ;
	if (!token_ParseValNext(const_token, TOKEN_CONST_CAPTURE_CITY_FOR_GOLD_MULTIPLIER, m_capture_city_for_gold_multiplier)) return (FALSE) ;
	if (!token_ParseValNext(const_token, TOKEN_CONST_PACT_END_POLLUTION_EXPIRES, m_pact_end_pollution_expires)) return (FALSE) ;
	if (!token_ParseValNext(const_token, TOKEN_CONST_LEAVE_OUR_LANDS_EXPIRES, m_leave_our_lands_expires)) return (FALSE) ;
	if (!token_ParseValNext(const_token, TOKEN_CONST_REDUCE_POLLUTION_EXPIRES, m_reduce_pollution_expires)) return (FALSE) ;


	if (!token_ParseValNext(const_token, TOKEN_CONST_PATIENCE_LOST_PER_REQUEST, m_patience_lost_per_request)) return (FALSE) ;
	if (!token_ParseValNext(const_token, TOKEN_CONST_PATIENCE_REGAINED_PER_ROUND, m_patience_regained_per_round)) return (FALSE) ;
	if (!token_ParseFloatNext(const_token, TOKEN_CONST_REGAIN_PATIENCE_PROBABILITY, m_patience_regain_probability)) return (FALSE) ;
	if (!token_ParseValNext(const_token, TOKEN_CONST_PATIENCE_LOST_THRESHOLD, m_patience_lost_threshold)) return (FALSE) ;

    sint32 utmp; 

  	if (!token_ParseValNext(const_token, TOKEN_GOAL_TIME_SLICE, utmp)) return (FALSE) ;
    m_goal_time_slice = uint32(utmp); 
   	if (!token_ParseValNext(const_token, TOKEN_MAX_TIME_SLICE, utmp)) return (FALSE) ;
    m_max_time_slice = uint32(utmp); 
   	if (!token_ParseValNext(const_token, TOKEN_TOTAL_TIME_SLICE, utmp)) return (FALSE) ;
    m_total_time_slice = uint32(utmp); 

	if(!token_ParseFloatNext(const_token, TOKEN_ENTRENCHMENT_BONUS, m_entrenchment_bonus)) return (FALSE);
	
	if(!token_ParseValNext(const_token, TOKEN_PARADROP_DISTANCE, utmp)) return (FALSE);
	m_paradrop_distance = uint32(utmp);
	
	if(!token_ParseValNext(const_token, TOKEN_PARADROP_SUCCESS_PERCENT, utmp)) return FALSE;
	m_paradrop_success_percentage = uint32(utmp);

	if(!token_ParseFloatNext(const_token, TOKEN_ELITE_SPY_BONUS, m_elite_spy_bonus)) return FALSE;

	if(!token_ParseFloatNext(const_token, TOKEN_CITY_SECOND_ATTEMPT_SPY_BONUS, m_city_second_attempt_spy_bonus)) return FALSE;

	if(!token_ParseValNext(const_token, TOKEN_ASSASINATION_HAPPINESS_EFFECT, tmp)) return FALSE;
	m_assasination_happiness_effect = tmp;

	if(!token_ParseValNext(const_token, TOKEN_ASSASINATION_HAPPINESS_EFFECT_TIMER, utmp)) return FALSE;
	m_assasination_happiness_effect_timer = utmp;

	if(!token_ParseValNext(const_token, TOKEN_SPIED_UPON_WARINESS_TIMER, tmp)) return FALSE;
	m_spied_upon_wariness_timer = tmp;

	if(!token_ParseValNext(const_token, TOKEN_MAXIMUM_PARTY_COST, tmp)) return FALSE;
	m_maximum_party_cost = tmp;

	if(!token_ParseFloatNext(const_token, TOKEN_MAXIMUM_PARTY_CHANCE, m_maximum_party_chance)) return FALSE;

	if(!token_ParseValNext(const_token, TOKEN_GOSSIP_MAP_RADIUS, tmp)) return FALSE;
	m_gossip_map_radius = tmp;

	if(!token_ParseFloatNext(const_token, TOKEN_HEAR_GOSSIP_CHANCE, m_hear_gossip_chance)) return FALSE;

	if(!token_ParseFloatNext(const_token, TOKEN_FRANCHISE_EFFECT, m_franchise_effect)) return FALSE;

	if(!token_ParseValNext(const_token, TOKEN_TURNS_TO_SUE_FRANCHISE,
						   m_turns_to_sue_franchise)) return FALSE;

	if(!token_ParseFloatNext(const_token, TOKEN_SLAVER_ELITE_CHANCE,
							 m_slaver_elite_chance))
		return FALSE;

	if(!token_ParseFloatNext(const_token, TOKEN_ELITE_SLAVER_BONUS,
							 m_elite_slaver_bonus))
		return FALSE;

	if(!token_ParseFloatNext(const_token, TOKEN_ABOLITIONIST_ELITE_CHANCE,
							 m_abolitionist_elite_chance))
		return FALSE;

	if(!token_ParseFloatNext(const_token, TOKEN_ELITE_ABOLITIONIST_BONUS,
							 m_elite_abolitionist_bonus))
		return FALSE;

	if(!token_ParseFloatNext(const_token, TOKEN_WATCHFUL_CITY_SUCCESS_MODIFIER,
							 m_watchful_city_success_modifier))
		return FALSE;

	if(!token_ParseFloatNext(const_token, TOKEN_WATCHFUL_CITY_DEATH_MODIFIER,
							 m_watchful_city_death_modifier))
		return FALSE;

	if(!token_ParseValNext(const_token, TOKEN_WATCHFUL_CITY_TURNS,
						   m_watchful_city_turns)) return FALSE;

	if(!token_ParseValNext(const_token, TOKEN_BIO_INFECTION_TURNS,
						   m_bio_infection_turns)) return FALSE;

	if(!token_ParseValNext(const_token, TOKEN_NANO_INFECTION_TURNS,
						   m_nano_infection_turns)) return FALSE;
	
	if(!token_ParseFloatNext(const_token, TOKEN_BIO_INFECTION_SPREAD_CHANCE,
							 m_bio_infection_spread_chance)) return FALSE;
	if(!token_ParseFloatNext(const_token, TOKEN_NANO_INFECTION_SPREAD_CHANCE,
							 m_nano_infection_spread_chance)) return FALSE;
	if(!token_ParseFloatNext(const_token, TOKEN_BIO_TERROR_KILL_PERCENTAGE,
							 m_bio_terror_kill_percentage)) return FALSE;

	if(!token_ParseFloatNext(const_token, TOKEN_ELITE_TERRORIST_BONUS,
							 m_elite_terrorist_bonus)) return FALSE;
	if(!token_ParseFloatNext(const_token, TOKEN_TERRORIST_ELITE_CHANCE,
							 m_terrorist_elite_chance)) return FALSE;

	if(!token_ParseFloatNext(const_token, TOKEN_NUKE_POPULATION_PERCENTAGE,
							 m_nuke_population_percentage)) return FALSE;

	if(!token_ParseFloatNext(const_token, TOKEN_SPECIAL_ACTION_MOVE_COST,
						   m_special_action_move_cost)) return FALSE;

	if(!token_ParseFloatNext(const_token, TOKEN_CLERIC_CONVERSION_FACTOR,
							 m_cleric_conversion_factor)) return FALSE;
	if(!token_ParseFloatNext(const_token, TOKEN_TELEVANGELIST_CONVERSION_FACTOR,
							 m_televangelist_conversion_factor)) return FALSE;

	if(!token_ParseFloatNext(const_token, TOKEN_REFORMATION_CHANCE,
							 m_reformation_chance)) return FALSE;
	if(!token_ParseFloatNext(const_token, TOKEN_REFORMATION_DEATH_CHANCE,
							 m_reformation_death_chance)) return FALSE;

	if(!token_ParseValNext(const_token, TOKEN_REFORMATION_HAPPINESS_TIME,
						   m_reformation_happiness_time)) return FALSE;
	if(!token_ParseValNext(const_token, TOKEN_REFORMATION_HAPPINESS_AMOUNT,
						   m_reformation_happiness_amount)) return FALSE;

	if(!token_ParseValNext(const_token, TOKEN_UNCONVERTED_INDULGENCE_GOLD,
						   m_unconverted_indulgence_gold)) return FALSE;
	if(!token_ParseValNext(const_token, TOKEN_UNCONVERTED_INDULGENCE_HAPPINESS,
						   m_unconverted_indulgence_happiness)) return FALSE;

	if(!token_ParseValNext(const_token, TOKEN_CONVERTED_INDULGENCE_GOLD,
						   m_converted_indulgence_gold)) return FALSE;
	if(!token_ParseValNext(const_token, TOKEN_CONVERTED_INDULGENCE_HAPPINESS,
						   m_converted_indulgence_happiness)) return FALSE;

	if(!token_ParseValNext(const_token, TOKEN_OTHER_FAITH_INDULGENCE_GOLD,
						   m_other_faith_indulgence_gold)) return FALSE;
	if(!token_ParseValNext(const_token, TOKEN_OTHER_FAITH_INDULGENCE_HAPPINESS,
						   m_other_faith_indulgence_happiness)) return FALSE;

	if(!token_ParseValNext(const_token, TOKEN_SOOTHSAY_HAPPY_AMOUNT,
						   m_soothsay_happy_amount)) return FALSE;

	if(!token_ParseValNext(const_token, TOKEN_RUINS_BOX_WIDTH,
						   m_hut_box_width)) return FALSE;
	if(!token_ParseValNext(const_token, TOKEN_RUINS_BOX_HEIGHT,
						   m_hut_box_height)) return FALSE;
	if(!token_ParseFloatNext(const_token, TOKEN_RUINS_CHANCE_PER_BOX,
							 m_hut_chance_per_box)) return FALSE;

	if(!token_ParseValNext(const_token, TOKEN_MAX_RUINS_GOLD,
						   m_max_goody_hut_gold)) return FALSE;
	if(!token_ParseValNext(const_token, TOKEN_MAX_RUINS_ADVANCE,
						   m_max_goody_hut_advance)) return FALSE;
	if(!token_ParseValNext(const_token, TOKEN_MAX_RUINS_UNIT,
						   m_max_goody_hut_unit)) return FALSE;

	if(!token_ParseFloatNext(const_token, TOKEN_UNIT_RUSH_MODIFIER,
							 m_unit_rush_modifier)) return FALSE;
	if(!token_ParseFloatNext(const_token, TOKEN_IMPROVEMENT_RUSH_MODIFIER,
							 m_improvement_rush_modifier)) return FALSE;
	if(!token_ParseFloatNext(const_token, TOKEN_WONDER_RUSH_MODIFIER,
							 m_wonder_rush_modifier)) return FALSE;


	if(!token_ParseFloatNext(const_token, TOKEN_BUILDING_PRODUCTION_TO_VALUE_MODIFIER,
							 m_building_production_to_value_modifier)) return FALSE;

	if(!token_ParseValNext(const_token, TOKEN_MONOPOLY_THRESHOLD, m_monopolyThreshold))
		return (FALSE) ;

	if(!token_ParseValNext(const_token, TOKEN_POPULATION_POLLUTION_WARNING_THRESHOLD, m_populationPollutionWarningThreshold))
		return (FALSE) ;

	if(!token_ParseValNext(const_token, TOKEN_INDUSTRIAL_POLLUTION_WARNING_THRESHOLD, m_industrialPollutionWarningThreshold))
		return (FALSE) ;


	if (!token_ParseValNext(const_token, TOKEN_VERY_HAPPY_THRESHOLD, m_veryHappyThreshold))
		return (FALSE) ;

	if (!token_ParseFloatNext(const_token, TOKEN_CITY_GROWTH_COEFFICIENT,
							  m_city_growth_coefficient)) return FALSE;
	if (!token_ParseValNext(const_token, TOKEN_RIOT_LEVEL,
							m_riot_level)) return FALSE;

	if (!token_ParseFloatNext
            (const_token, 
             TOKEN_POWER_POINTS_TO_MATERIALS, 
			 m_power_points_to_materials
            )
       )
	{
		return FALSE;
	}
	if(!token_ParseValNext(const_token, TOKEN_MAX_AIRLIFT_STACK_SIZE,
						   m_max_airlift_stack_size)) return FALSE;

	if(!token_ParseValNext(const_token, TOKEN_GOLD_FROM_PIRACY,
						   m_gold_from_piracy)) return FALSE;
	
	if(!token_ParseValNext(const_token, TOKEN_NO_PIRACY_EXPIRES, m_no_piracy_expires))
		return (FALSE) ;

	if(!token_ParseFloatNext(const_token, TOKEN_SPACE_LAUNCH_COST,
							 m_space_launch_cost)) return FALSE;
	if(!token_ParseFloatNext(const_token, TOKEN_SPACE_LAND_COST,
							 m_space_land_cost)) return FALSE;

	if(!token_ParseValNext(const_token, TOKEN_WORMHOLE_ORBIT_HEIGHT_PERCENTAGE,
						   m_wormhole_orbit_height_percentage)) return FALSE;
	if(!token_ParseValNext(const_token, TOKEN_WORMHOLE_SPEED,
						   m_wormhole_speed)) return FALSE;
	if(!token_ParseValNext(const_token, TOKEN_WORMHOLE_RETURN_TIME,
						   m_wormhole_return_time)) return FALSE;
	if(!token_ParseValNext(const_token, TOKEN_WORMHOLE_VISIBLE_TO_ALL_TURNS,
						   m_wormhole_visible_to_all_turns)) return FALSE;
	if(!token_ParseValNext(const_token, TOKEN_MAX_GOVERNMENT_CHANGE_TURNS,
						   m_max_government_change_turns)) return FALSE;






	if (!token_ParseValNext(const_token, TOKEN_POLLUTION_FORCES_ANARCHY, utmp))
		return (FALSE) ;

	m_pollution_forces_government_collapse = utmp ;


	if (!token_ParseFloatNext(const_token, TOKEN_FOOD_TO_POLLUTION_COEF, m_food_to_pollution_coef))
		return (FALSE) ;


	if(!token_ParseValNext(const_token, TOKEN_EMANCIPATION_UNHAPPINESS_TURNS,
							 m_emancipation_unhappiness_turns))
		return FALSE;
	if(!token_ParseFloatNext(const_token, TOKEN_EMANCIPATION_UNHAPPINESS_AMOUNT,
							 m_emancipation_unhappiness_amount))
		return FALSE;

	if(!token_ParseFloatNext(const_token, TOKEN_CAPTURE_CITY_ADVANCE_CHANCE,
							  m_capture_city_advance_chance))
		return FALSE;


	if(!token_ParseFloatNext(const_token, TOKEN_CITY_HEAL_RATE,
							 m_city_heal_rate))
		return FALSE;

	if(!token_ParseFloatNext(const_token, TOKEN_NORMAL_HEAL_RATE,
							 m_normal_heal_rate))
		return FALSE;

	if(!token_ParseFloatNext(const_token, TOKEN_LOCAL_POLLUTION_LEVEL,
							 m_local_pollution_level))
		return FALSE;

	if(!token_ParseFloatNext(const_token, TOKEN_LOCAL_POLLUTION_CHANCE,
							 m_local_pollution_chance))
		return FALSE;

	if(!token_ParseFloatNext(const_token, TOKEN_ASSAULT_DESTROY_BUILDING_CHANCE,
							 m_assault_destroy_building_chance))
		return FALSE;

	if(!token_ParseFloatNext(const_token, TOKEN_BOMBARD_DESTROY_BUILDING_CHANCE,
							 m_bombard_destroy_building_chance))
		return FALSE;

	if(!token_ParseValNext(const_token, TOKEN_ADVANCE_CHOICES_MIN, m_advance_choices_min))
		return FALSE;

	if(!token_ParseValNext(const_token, TOKEN_ADVANCE_CHOICES_MAX, m_advance_choices_max))
		return FALSE;

	if(!token_ParseValNext(const_token, TOKEN_ATTACK_CONVERTER_UNHAPPINESS_TURNS, m_attack_converter_unhappiness_turns))
		return FALSE;

	if(!token_ParseFloatNext(const_token, TOKEN_ATTACK_CONVERTER_UNHAPPINESS_AMOUNT, m_attack_converter_unhappiness_amount))
		return FALSE;

	if(!token_ParseFloatNext(const_token, TOKEN_MIN_START_DISTANCE_COEFFICIENT, m_min_start_distance_coefficient))
		return FALSE;
	if(!token_ParseFloatNext(const_token, TOKEN_MAX_START_DISTANCE_COEFFICIENT, m_max_start_distance_coefficient))
		return FALSE;
	if(!token_ParseValNext(const_token, TOKEN_MAX_SAME_TILES, m_max_same_tiles))
		return FALSE;
	if(!token_ParseFloatNext(const_token, TOKEN_COMBAT_VETERAN_CHANCE, m_combat_veteran_chance))
		return FALSE;

	if(!token_ParseValNext(const_token, TOKEN_STOP_TRADE_ROUNDS, m_stop_trade_rounds))
		return FALSE;
	if(!token_ParseValNext(const_token, TOKEN_LEAVE_OUR_LANDS_ROUNDS, m_leave_our_lands_rounds))
		return FALSE;
	if(!token_ParseValNext(const_token, TOKEN_REDUCE_POLLUTION_ROUNDS, m_reduce_pollution_rounds))
		return FALSE;
	if(!token_ParseValNext(const_token, TOKEN_CAPTURE_CITY_ROUNDS, m_capture_city_rounds))
		return FALSE;
	if(!token_ParseValNext(const_token, TOKEN_END_POLLUTION_ROUNDS, m_end_pollution_rounds))
		return FALSE;
	if(!token_ParseValNext(const_token, TOKEN_ATTACK_ENEMY_ROUNDS, m_attack_enemy_rounds))
		return FALSE;
	if(!token_ParseValNext(const_token, TOKEN_MIN_CONTINENT_START_SIZE, m_min_continent_start_size))
		return FALSE;
	if(!token_ParseFloatNext(const_token, TOKEN_BOMBARD_KILL_POP_CHANCE, m_bombard_kill_pop_chance))
		return FALSE;
	if(!token_ParseFloatNext(const_token, TOKEN_ASSAULT_KILL_POP_CHANCE, m_assault_kill_pop_chance))
		return FALSE;
	if(!token_ParseFloatNext(const_token, TOKEN_CAPTURE_KILL_POP_CHANCE, m_capture_kill_pop_chance))
		return FALSE;

	if(!token_ParseValNext(const_token, TOKEN_SCALED_POP_ANCIENT, m_scaled_pop[0]))
		return FALSE;
	if(!token_ParseValNext(const_token, TOKEN_SCALED_POP_RENAISSANCE, m_scaled_pop[1]))
		return FALSE;
	if(!token_ParseValNext(const_token, TOKEN_SCALED_POP_MODERN, m_scaled_pop[2]))
		return FALSE;
	if(!token_ParseValNext(const_token, TOKEN_SCALED_POP_GENETIC, m_scaled_pop[3]))
		return FALSE;
	if(!token_ParseValNext(const_token, TOKEN_SCALED_POP_DIAMOND, m_scaled_pop[4]))
		return FALSE;
	if(!token_ParseValNext(const_token, TOKEN_PIRACY_KILLS_TRADER_CHANCE, m_piracy_kills_trader_chance))
		return FALSE;
	if(!token_ParseValNext(const_token, TOKEN_UPRISING_CHANCE_PER_UNGUARDED_SLAVE, m_uprising_chance_per_unguarded_slave))
		return FALSE;
	if(!token_ParseValNext(const_token, TOKEN_MAX_DISBAND_SIZE, m_max_disband_size))
		return FALSE;
	if(!token_ParseValNext(const_token, TOKEN_MAX_REQUESTS_PER_PLAYER_PER_TURN, m_max_requests_per_player_per_turn))
		return FALSE;

	if(!token_ParseValNext(const_token, TOKEN_SLAVES_PER_MILITARY_UNIT,
						   m_slaves_per_military_unit))
		return FALSE;

	if(!token_ParseValNext(const_token, TOKEN_MIN_ABSOLUTE_START_DISTANCE, 
							m_min_absolute_start_distance))
		return FALSE;

	
	if (const_token->Next() != TOKEN_MAP_SIZE_SMALL) return FALSE;
	if (!ParseMapSize(const_token, m_small_map_size)) return FALSE;

	if (const_token->Next() != TOKEN_MAP_SIZE_MEDIUM) return FALSE;
	if (!ParseMapSize(const_token, m_medium_map_size)) return FALSE;

	if (const_token->Next() != TOKEN_MAP_SIZE_LARGE) return FALSE;
	if (!ParseMapSize(const_token, m_large_map_size)) return FALSE;

	if (const_token->Next() != TOKEN_MAP_SIZE_GIGANTIC) return FALSE;
	if (!ParseMapSize(const_token, m_gigantic_map_size)) return FALSE;

	if(!token_ParseFloatNext(const_token, TOKEN_RAIL_LAUNCH_POLLUTION,
							 m_rail_launch_pollution)) return FALSE;

	if(!token_ParseValNext(const_token, TOKEN_SPACE_FUEL_COST,
						   m_space_fuel_cost)) return FALSE;
	if(!token_ParseValNext(const_token, TOKEN_NON_SPACE_FUEL_COST,
						   m_non_space_fuel_cost)) return FALSE;

	if(!token_ParseValNext(const_token, TOKEN_STARVATION_WARNING_FUDGE_FACTOR,
						   m_starvation_warning_fudge_factor)) return FALSE;
	if(!token_ParseValNext(const_token, TOKEN_MAINTENANCE_WARNING_FUDGE_FACTOR,
						   m_maintenance_warning_fudge_factor)) return FALSE;
	if(!token_ParseValNext(const_token, TOKEN_SUPPORT_WARNING_FUDGE_FACTOR,
						   m_support_warning_fudge_factor)) return FALSE;

	if(!token_ParseFloatNext(const_token, TOKEN_RELATIVE_AI_PRODUCTION_CHEAT_COEF,
							 m_relative_ai_production_cheat_coef)) return FALSE;
	if(!token_ParseFloatNext(const_token, TOKEN_RELATIVE_AI_SCIENCE_CHEAT_COEF,
							 m_relative_ai_science_cheat_coef)) return FALSE;

	if(!token_ParseValNext(const_token, TOKEN_NUKE_KILL_TILES,
						   m_nuke_kill_tiles)) return FALSE;

	if(!token_ParseFloatNext(const_token, TOKEN_INCITE_REVOLUTION_GOLD_COEFFICIENT,
							 m_incite_revolution_gold_coefficient)) return FALSE;
	if(!token_ParseFloatNext(const_token, TOKEN_INCITE_REVOLUTION_CAPITOL_PENALTY,
							 m_incite_revolution_capitol_penalty)) return FALSE;
	if(!token_ParseFloatNext(const_token, TOKEN_INCITE_UPRISING_GOLD_COEFFICIENT,
							 m_incite_uprising_gold_coefficient)) return FALSE;
	if(!token_ParseFloatNext(const_token, TOKEN_INCITE_UPRISING_CAPITOL_PENALTY,
							 m_incite_uprising_capitol_penalty)) return FALSE;

	if(!token_ParseValNext(const_token, TOKEN_BIO_INFECTION_UNHAPPINESS,
						   m_bio_infection_unhappiness)) return FALSE;

	if(!token_ParseValNext(const_token, TOKEN_MIN_ECO_PACT_VIOLATION_LEVEL,
						   m_min_eco_pact_violation_level)) return FALSE;
	if(!token_ParseFloatNext(const_token, TOKEN_NANO_INFECTION_TERRORIST_DEATH_CHANCE,
						   m_nano_infection_terrorist_death_chance)) return FALSE;
	if(!token_ParseFloatNext(const_token, TOKEN_BIO_INFECTION_TERRORIST_DEATH_CHANCE,
							 m_bio_infection_terrorist_death_chance)) return FALSE;

	if(!token_ParseFloatNext(const_token, TOKEN_FLOOD_CHANGES_COAST_TO_WATER_CHANCE,
							 m_flood_changes_coast_to_water_chance)) return FALSE;

	if(!token_ParseFloatNext(const_token, TOKEN_AI_CHEAT_ECO_PACT_MIN,
							 m_ai_cheat_eco_pact_min)) return FALSE;
	if(!token_ParseFloatNext(const_token, TOKEN_AI_CHEAT_ECO_PACT_MAX,
							 m_ai_cheat_eco_pact_max)) return FALSE;

	if(!token_ParseValNext(const_token, TOKEN_MAX_ABSOLUTE_GROWTH_RATE,
						   m_max_absolute_growth_rate)) return FALSE;
	if(!token_ParseFloatNext(const_token, TOKEN_TRADE_DISTANCE_COEFFICIENT,
						   m_trade_distance_coefficient)) return FALSE;
	if(!token_ParseValNext(const_token, TOKEN_MIN_LIKE_TILES_FOR_GOOD,
						   m_min_like_tiles_for_good)) return FALSE;

	if(!token_ParseFloatNext(const_token, TOKEN_MIN_GOOD_VALUE,
						   m_min_good_value)) return FALSE;
	if(!token_ParseFloatNext(const_token, TOKEN_MAX_GOOD_VALUE,
						   m_max_good_value)) return FALSE;
	if(!token_ParseFloatNext(const_token, TOKEN_PIRACY_WASTE_COEFFICIENT,
							 m_piracy_waste_coefficient)) return FALSE;
	if(!token_ParseValNext(const_token, TOKEN_BORDER_INT_RADIUS,
						   m_border_int_radius)) return FALSE;
	if(!token_ParseValNext(const_token, TOKEN_BORDER_SQUARED_RADIUS,
						   m_border_squared_radius)) return FALSE;

	if(!token_ParseFloatNext(const_token, TOKEN_BIO_INFECTION_PRODUCTION_COEF,
							 m_bio_infection_production_coef)) return FALSE;
	if(!token_ParseFloatNext(const_token, TOKEN_NANO_BUILDING_KILL_PERCENTAGE,
							 m_nano_building_kill_percentage)) return FALSE;

	if(!token_ParseValNext(const_token, TOKEN_BASE_STARVATION_PROTECTION,
						   m_base_starvation_protection)) return FALSE;

	if(!token_ParseFloatNext(const_token, TOKEN_CARAVAN_COEF,
							 m_caravan_coef)) return FALSE;

	if(!token_ParseValNext(const_token, TOKEN_POLLUTION_CAUSED_BY_NUKE,
						   m_pollution_caused_by_nuke)) return FALSE;

	(void) ParseOptional(const_token, 
		                 TOKEN_MAX_MATCH_LIST_CYCLES, 
		                 m_max_match_list_cycles, 
                         DEFAULT_MAX_MATCH_LIST_CYCLES
					    );
    (void) ParseOptional(const_token, 
		        		 TOKEN_MIN_TURNS_BETWEEN_REVOLT,
				         m_min_turns_between_revolt,
					     DEFAULT_MIN_TURNS_BETWEEN_REVOLT
					    );

	return TRUE; 	
}

sint32 ConstDB::ParseMapSize(Token *const_token, MapPoint &size)
{
	sint32 x, y, z;

	if (const_token->Next() != TOKEN_NUMBER) return FALSE;
	const_token->GetNumber(x);
	if (const_token->Next() != TOKEN_NUMBER) return FALSE;
	const_token->GetNumber(y);
	if (const_token->Next() != TOKEN_NUMBER) return FALSE;
	const_token->GetNumber(z);

	size.x = sint16(x);
	size.y = sint16(y);

	return TRUE;
}

void ConstDB::GetMapSizeMapPoint(MAPSIZE size, MapPoint &mapPoint)
{
	switch (size) {
	case MAPSIZE_SMALL :
		GetSmallMapSize(mapPoint);
		break;
	case MAPSIZE_MEDIUM :
		GetMediumMapSize(mapPoint);
		break;
	case MAPSIZE_LARGE :
		GetLargeMapSize(mapPoint);
		break;
	case MAPSIZE_GIGANTIC :
		GetGiganticMapSize(mapPoint);
		break;
	default :
		Assert(FALSE);
	}
}

sint32 ConstDB_Parse(char *filename, C3DIR dir)
{
    Token *const_token = new Token(filename, TOKEN_CONST_MAX - TOKEN_MAX, g_const_token_data, dir);
    sint32 r = g_theConstDB->ParseConstDB(const_token); 
    delete const_token; 
    return r; 
}

