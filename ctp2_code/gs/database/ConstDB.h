//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ header
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
// _MSC_VER		
// - When defined, allows Microsoft C++ extensions.
// - When not defined, generates standard C++.
//
// Note: For the blocks with _MSC_VER preprocessor directives, the following
//       is implied: the (_MSC_VER) preprocessor directive lines and the blocks 
//       between #else and #endif are modified Apolyton code. The blocks 
//       between #if and #else are the original Activision code.
//
//----------------------------------------------------------------------------
//
// Modifications from the original Activision code:
//
// - Exported MAX_MATCH_LIST_CYCLES and MIN_TURNS_BETWEEN_REVOLT to be 
//   modifiable in const.txt.
//
//----------------------------------------------------------------------------

#ifdef HAVE_PRAGMA_ONCE
#pragma once
#endif

#ifndef __CONSTDB_H__
#define __CONSTDB_H__ 1

#include "MapPoint.h"
#include "dbtypes.h"
#include "c3files.h"

#define k_MAX_CONST_DB 3

class CivArchive ;
class Token;

typedef class ConstDB { 
	
    
	
	double	m_land; 
	double	m_continent; 
	double	m_homogenous; 
	double	m_mountain; 
	double	m_hills; 
	sint32	m_mountain_spread; 
	sint32	m_mountain_length;
	sint32	m_glacier;
	sint32	m_volcano; 
	double	m_trench; 
	sint32	m_mount_cell; 
	sint32  m_forest; 
	sint32  m_grass;  
	sint32  m_plains; 
	sint32  m_desert; 
	sint32  m_whitePercent;
	sint32  m_brownPercent;
	sint32  m_temperatureRangeAdjust;
	
	sint32	m_merid[k_NUM_MERIDIANS]; 
	double	m_meridf[k_NUM_MERIDIANS];
	
	double	m_hlevelf[4];

	
	
	
	
	sint8	m_forestWet;
	sint8	m_forestDry;
	sint8	m_grassWet;
	sint8	m_grassDry;
	sint8	m_plainsWet;
	sint8	m_plainsDry;
	sint8	m_desertWet;
	sint8	m_desertDry;

	sint8	m_whiteWarm;
	sint8	m_whiteCold;
	sint8	m_brownWarm;
	sint8	m_brownCold;
	sint8	m_temperatureRangeAdjustWarm;
	sint8	m_temperatureRangeAdjustCold;

	sint8	m_richnessFewgoods;
	sint8	m_richnessManygoods;
	sint8	m_riverCellWidthFewgoods;
	sint8	m_riverCellWidthManygoods;
	sint8	m_riverCellHeightFewgoods;
	sint8	m_riverCellHeightManygoods;

	sint32	m_nice_r; 
	double	m_river;
	sint32	m_river_len;
	sint32	m_richness; 
	sint32	m_lost_at_sea;
	double	m_vet_coef; 
	
	sint32	m_average_pollution_turns ;
	
	sint32	m_revolution_level ;
	sint32	m_tariff_reduction,
		m_revolt_influence_distance ;
	


	double	m_discoveries_pollution_size_modifier ;
	double	m_discoveries_pollution_production_modifier ;
	
    double	m_max_empire_dist_cost;
    double	m_happiness_launch_cost;
	
    double	m_unit_workday;
    double  m_base_workday;
    double	m_unit_wages;
    double  m_base_wages;
    double	m_unit_rations;
    double  m_base_rations; 
	double	m_food_to_pollution_coef ;

	sint32	m_monopolyThreshold,
		m_veryHappyThreshold,									
		m_populationPollutionWarningThreshold,
		m_industrialPollutionWarningThreshold,
		m_pact_capture_city_expires,
		
		m_cease_fire_expires,
		m_short_cease_fire_expires,
		m_pact_end_pollution_expires,
		m_leave_our_lands_expires,
		m_reduce_pollution_expires,
		
		m_capture_city_for_gold_multiplier,
		m_patience_lost_per_request,
		m_patience_regained_per_round,
		m_patience_lost_threshold,
		m_end_of_game_year_early_warning,
		m_end_of_game_year ;
	

	uint32	m_pollution_forces_government_collapse ;
	
	double	m_patience_regain_probability ;
	
    uint32 m_goal_time_slice; 
    uint32 m_max_time_slice; 
    uint32 m_total_time_slice; 
	
	double m_entrenchment_bonus;
	uint32 m_paradrop_distance;
	uint32 m_paradrop_success_percentage;
	double m_elite_spy_bonus;
	double m_city_second_attempt_spy_bonus;
	
	sint32 m_assasination_happiness_effect;
	uint32 m_assasination_happiness_effect_timer;
	
	sint32 m_spied_upon_wariness_timer;
	sint32 m_maximum_party_cost;
	double m_maximum_party_chance;
	sint32 m_gossip_map_radius;
	double m_hear_gossip_chance;
	
	double m_franchise_effect;
	sint32	m_turns_to_sue_franchise;
	
	double	m_elite_slaver_bonus;
	double	m_slaver_elite_chance;
	
	double	m_elite_abolitionist_bonus,
		m_abolitionist_elite_chance;
	double	m_watchful_city_success_modifier,
		m_watchful_city_death_modifier;
	sint32	m_watchful_city_turns;
	sint32	m_bio_infection_turns;
	sint32	m_nano_infection_turns;
	double	m_bio_infection_spread_chance,
		m_nano_infection_spread_chance;
	
	double m_elite_terrorist_bonus, m_terrorist_elite_chance;
	double m_nuke_population_percentage;
	double m_bio_terror_kill_percentage;
	
	double m_special_action_move_cost;
	
	double m_cleric_conversion_factor;
	double m_televangelist_conversion_factor;
	
	double m_reformation_chance,
		m_reformation_death_chance;
	sint32 m_reformation_happiness_time,
		m_reformation_happiness_amount;
	sint32 m_unconverted_indulgence_gold, m_unconverted_indulgence_happiness,
		m_converted_indulgence_gold, m_converted_indulgence_happiness,
		m_other_faith_indulgence_gold, m_other_faith_indulgence_happiness;
	sint32 m_soothsay_happy_amount;
	
	sint32 m_hut_box_width, m_hut_box_height;
	double m_hut_chance_per_box;
	sint32 m_max_goody_hut_gold;
	sint32 m_max_goody_hut_advance;
	sint32 m_max_goody_hut_unit;
	
	double m_unit_rush_modifier, m_improvement_rush_modifier, m_wonder_rush_modifier;
    double m_change_currently_building_item_penalty; 
	double m_building_production_to_value_modifier;
	
	double m_city_growth_coefficient;
	double m_power_points_to_materials;
	sint32 m_max_airlift_stack_size;
	sint32 m_river_cell_width;
	sint32 m_river_cell_height;
	sint32 m_gold_from_piracy;
	sint32 m_no_piracy_expires;
	double m_space_launch_cost,
		m_space_land_cost;
	sint32 m_wormhole_orbit_height_percentage;
	sint32 m_wormhole_speed;
	sint32 m_wormhole_return_time;
	sint32 m_wormhole_visible_to_all_turns;
	sint32 m_max_government_change_turns;

	sint32 m_emancipation_unhappiness_turns;
	double m_emancipation_unhappiness_amount;
	double m_capture_city_advance_chance;

	double m_city_heal_rate;
	double m_normal_heal_rate;
	double m_local_pollution_level, m_local_pollution_chance;
	sint32 m_cityPopulationProducesPollutionThreshold ;
	sint32 m_cityProductionProducesPollutionThreshold ;

	double m_assault_destroy_building_chance;
	double m_bombard_destroy_building_chance;

	sint32 m_advance_choices_min, m_advance_choices_max;

	sint32 m_attack_converter_unhappiness_turns;
	double m_attack_converter_unhappiness_amount;

	double m_min_start_distance_coefficient, m_max_start_distance_coefficient;
	sint32 m_max_same_tiles;
	double m_combat_veteran_chance;

	sint32 m_stop_trade_rounds,
		m_leave_our_lands_rounds,
		m_reduce_pollution_rounds,
		m_capture_city_rounds,
		m_end_pollution_rounds,
		m_attack_enemy_rounds;

	sint32 m_min_continent_start_size;

	double m_bombard_kill_pop_chance,
		m_assault_kill_pop_chance,
		m_capture_kill_pop_chance;

	sint32 m_scaled_pop[5];

	sint32 m_piracy_kills_trader_chance;

	sint32 m_uprising_chance_per_unguarded_slave;

	sint32 m_max_disband_size;

	sint32 m_max_requests_per_player_per_turn;

	sint32 m_slaves_per_military_unit;

	sint32 m_min_absolute_start_distance;

	MapPoint	m_small_map_size;
	MapPoint	m_medium_map_size;
	MapPoint	m_large_map_size;
	MapPoint	m_gigantic_map_size;

	double m_rail_launch_pollution;

	sint32 m_space_fuel_cost;
	sint32 m_non_space_fuel_cost;

    sint32 m_starvation_warning_fudge_factor;
    sint32 m_maintenance_warning_fudge_factor;
    sint32 m_support_warning_fudge_factor;

	double m_relative_ai_production_cheat_coef;
	double m_relative_ai_science_cheat_coef;

	double m_incite_revolution_gold_coefficient;
	double m_incite_revolution_capitol_penalty;
	double m_incite_uprising_gold_coefficient;
	double m_incite_uprising_capitol_penalty;

	sint32 m_nuke_kill_tiles;

	sint32 m_bio_infection_unhappiness;

	sint32 m_min_eco_pact_violation_level;

	double m_nano_infection_terrorist_death_chance;
	double m_bio_infection_terrorist_death_chance;

	double m_flood_changes_coast_to_water_chance;

	double m_ai_cheat_eco_pact_min;
	double m_ai_cheat_eco_pact_max;

	sint32 m_max_absolute_growth_rate;

	double m_trade_distance_coefficient;
	sint32 m_min_like_tiles_for_good;

	double m_min_good_value, m_max_good_value;

	double m_piracy_waste_coefficient;

	sint32 m_border_int_radius;
	sint32 m_border_squared_radius;

	double m_bio_infection_production_coef;
	double m_nano_building_kill_percentage;

	sint32 m_base_starvation_protection;

	double m_caravan_coef;

	sint32 m_pollution_caused_by_nuke;

	
	sint32 m_riot_level;
	
	//added DWT 
	sint32 m_max_match_list_cycles;
	// Modified by kaan to address bug # 12
	sint32 m_min_turns_between_revolt;
	
	
	
	
public:
	
	
	ConstDB (); 
	ConstDB(CivArchive &archive) ;
    sint32 ParseConstDB(Token *const_token);
	sint32 ParseMapSize(Token *const_token, MapPoint &size);

	double GetValue(const sint32 m_index) const; 
	void SetValue(const sint32 m_index, const double v); 
	
	double PercentLand() const { return m_land; };
	void SetPercentLand(double percent) { m_land = percent; }
	
	double PercentContinent() const { return m_continent; };  
	void SetPercentContinent(double percent) { m_continent = percent; }
	
	void SetHomogenous(const sint32 x) { m_homogenous = double(x)/100.0f; }; 
	double PercentHomogenous() const { return m_homogenous;} ; 
	
	void SetMeridian(const sint32 i, const sint32 x, const sint32 s) { m_merid[i] = sint32( double(s) * double(x)/100.0f); } ;
	sint32 Meridian(const sint32 i) const { return m_merid[i]; }; 
	void y2meridian(const sint32 j, sint32 &k) const ;
	void CalcMer(sint32 i); 
	
	void SetMountains(const sint32 x) { m_mountain = double (x)/100.0f; };
	double PercentMountains() const { return m_mountain; }; 
	
	sint32 MountainCell() const { return m_mount_cell; } ; 
	
	
	void SetHills(const sint32 x) { m_hills = double(x)/100.0f; };
	double PercentHills() const { return m_hills; }; 
	
	void SetMountainSpread(const sint32 x) { m_mountain_spread = x; };
	sint32 PercentMountainSpread() const { return m_mountain_spread; }; 
	
	void SetMountainLength(const sint32 x) { m_mountain_length = x; };
	sint32 AveMountainLength() const { return m_mountain_length; } ; 
	
	void SetGlacierExtent (const sint32 x) { m_glacier = x; };
	sint32 GlacierExtent() const { return m_glacier; }; 
	
	void SetPercentVolcano(const sint32 x) { m_volcano = x; };
	sint32 PercentVolcano() const { return m_volcano; }; 
	
	double PercentTrench() const { return m_trench; }; 
	
	sint32 PercentForest() const { return m_forest; }
	void SetPercentForest( sint32 forest ) { m_forest = forest; }
	sint32 PercentGrass() const { return m_grass; }
	void SetPercentGrass( sint32 grass ) { m_grass = grass; }
	sint32 PercentPlains() const { return m_plains; }
	void SetPercentPlains( sint32 plains ) { m_plains = plains; }
	sint32 PercentDesert() const { return m_desert; }
	void SetPercentDesert( sint32 desert ) { m_desert = desert; }
	
	sint32 PercentWhite() const { return m_whitePercent; }
	void SetPercentWhite(sint32 percent) { m_whitePercent = percent; }
	
	sint32 PercentBrown() const { return m_brownPercent; }
	void SetPercentBrown(sint32 percent) { m_brownPercent = percent; }
	
	sint32 TemperatureRangeAdjust() const { return m_temperatureRangeAdjust; }
	void SetTemperatureRangeAdjust( sint32 temperatureRangeAdjust )
	{ m_temperatureRangeAdjust = temperatureRangeAdjust; }

	double	hlevel(sint32 i) const { return m_hlevelf[i]; };
	void	SetHLevel(sint32 i, double value) { m_hlevelf[i] = value; }
	
	sint8 ForestWet() const { return m_forestWet; }
	void SetForestWet( sint8 forestWet ) { m_forestWet = forestWet; }
	sint8 ForestDry() const { return m_forestDry; }
	void SetForestDry( sint8 forestDry ) { m_forestDry = forestDry; }

	sint8 GrassWet() const { return m_grassWet; }
	void SetGrassWet( sint8 grassWet ) { m_grassWet = grassWet; }
	sint8 GrassDry() const { return m_grassDry; }
	void SetGrassDry( sint8 grassDry ) { m_grassDry = grassDry; }

	sint8 PlainsWet() const { return m_plainsWet; }
	void SetPlainsWet( sint8 plainsWet ) { m_plainsWet = plainsWet; }
	sint8 PlainsDry() const { return m_plainsDry; }
	void SetPlainsDry( sint8 plainsDry ) { m_plainsDry = plainsDry; }

	sint8 DesertWet() const { return m_desertWet; }
	void SetDesertWet( sint8 desertWet ) { m_desertWet = desertWet; }
	sint8 DesertDry() const { return m_desertDry; }
	void SetDesertDry( sint8 desertDry ) { m_desertDry = desertDry; }

	sint8 WhiteWarm() const { return m_whiteWarm; }
	void SetWhiteWarm( sint8 whiteWarm ) { m_whiteWarm = whiteWarm; }
	sint8 WhiteCold() const { return m_whiteCold; }
	void SetWhiteCold( sint8 whiteCold ) { m_whiteCold = whiteCold; }

	sint8 BrownWarm() const { return m_brownWarm; }
	void SetBrownWarm( sint8 brownWarm ) { m_brownWarm = brownWarm; }
	sint8 BrownCold() const { return m_brownCold; }
	void SetBrownCold( sint8 brownCold ) { m_brownCold = brownCold; }

	sint8 TemperatureRangeAdjustWarm() const
	{ return m_temperatureRangeAdjustWarm; }
	void SetTemperatureRangeAdjustWarm( sint8 temperatureRangeAdjustWarm )
	{ m_temperatureRangeAdjustWarm = temperatureRangeAdjustWarm; }
	sint8 TemperatureRangeAdjustCold() const
	{ return m_temperatureRangeAdjustCold; }
	void SetTemperatureRangeAdjustCold( sint8 temperatureRangeAdjustCold )
	{ m_temperatureRangeAdjustCold = temperatureRangeAdjustCold; }

	sint8 RichnessFewgoods() const { return m_richnessFewgoods; }
	void SetRichnessFewgoods( sint8 richnessFewgoods )
	{ m_richnessFewgoods = richnessFewgoods; }
	sint8 RichnessManygoods() const { return m_richnessManygoods; }
	void SetRichnessManygoods( sint8 richnessManygoods )
	{ m_richnessManygoods = richnessManygoods; }

	sint8 RiverCellWidthFewgoods() const { return m_riverCellWidthFewgoods; }
	void SetRiverCellWidthFewgoods( sint8 riverCellWidthFewgoods )
	{ m_riverCellWidthFewgoods = riverCellWidthFewgoods; }
	sint8 RiverCellWidthManygoods() const { return m_riverCellWidthManygoods; }
	void SetRiverCellWidthManygoods( sint8 riverCellWidthManygoods )
	{ m_riverCellWidthManygoods = riverCellWidthManygoods; }

	sint8 RiverCellHeightFewgoods() const { return m_riverCellHeightFewgoods; }
	void SetRiverCellHeightFewgoods( sint8 riverCellHeightFewgoods )
	{ m_riverCellHeightFewgoods = riverCellHeightFewgoods; }
	sint8 RiverCellHeightManygoods() const { return m_riverCellHeightManygoods;}
	void SetRiverCellHeightManygoods( sint8 riverCellHeightManygoods )
	{ m_riverCellHeightManygoods = riverCellHeightManygoods; }


	sint32 NiceRadius() const { return m_nice_r; } ; 
	double PercentRiver() const { return m_river; };
	sint32 AveRiverLen () const { return m_river_len; };
	
	sint32 PercentRichness() const { return m_richness; }; 
	void SetPercentRichness(const sint32 percent) { m_richness = percent; }
	
	sint32 PercentLostAtSea() const { return m_lost_at_sea; };
	double GetVetCoef() const { return m_vet_coef; };
	


	double GetDiscoveriesPollutionSizeModifier(void) const { return (m_discoveries_pollution_size_modifier) ; }
	double GetDiscoveriesPollutionProductionModifier(void) const { return (m_discoveries_pollution_production_modifier) ; }
	sint32 GetRevolutionLevel(void) const { return (m_revolution_level) ; }

	uint32 GetPollutionCreatesAnarchy(void) const { return (m_pollution_forces_government_collapse) ; }
	sint32 GetPactCaptureCityExpires(void) const { return (m_pact_capture_city_expires) ; }
	
	sint32 GetCeaseFireExpires(void) const { return (m_cease_fire_expires) ; }
	sint32 GetShortCeaseFireExpires(void) const { return (m_short_cease_fire_expires) ; }
	sint32 GetCaptureCityForGoldMultiplier(void) const { return (m_capture_city_for_gold_multiplier) ; }
	sint32 GetPactEndPollutionExpires(void) const { return (m_pact_end_pollution_expires) ; }
	sint32 GetLeaveOurLandsExpires(void) const { return (m_leave_our_lands_expires) ; }
	sint32 GetReducePollutionExpires(void) const { return (m_reduce_pollution_expires) ; }
	double GetFoodToPollutionCoef(void) const { return (m_food_to_pollution_coef) ; }
	
	
    double MaxEmpireDistCost() const { return m_max_empire_dist_cost; }
    double GetHappinessLaunchCost() const { return m_happiness_launch_cost; } 
    double GetUnitWorkday() const { return m_unit_workday; }
    double GetUnitWages() const { return m_unit_wages; }
    double GetUnitRations() const { return m_unit_rations; }
	
    double GetBaseWorkday() const { return m_base_workday;}  
    double GetBaseWages() const { return m_base_wages; }  
    double GetBaseRations() const { return m_base_rations; }  
	
    double GetChangeCurrentlyBuildingItemPenalty() const {  return m_change_currently_building_item_penalty; }
	
    sint32 GetTariffReduction() const { return (m_tariff_reduction) ; }
	sint32 GetRevoltInfluenceDistance(void) const { return (m_revolt_influence_distance) ; }
	sint32 GetPatienceLostPerRequest(void) const { return (m_patience_lost_per_request) ; }
	sint32 GetPatienceRegainedPerRound(void) const { return (m_patience_regained_per_round) ; }
	sint32 GetPatienceLostThreshold(void) const { return (m_patience_lost_threshold) ; }
	double GetPatienceRegainProbability(void) const { return (m_patience_regain_probability) ; }
	
    uint32 GetGoalTimeSlice() const { return m_goal_time_slice; } 
    uint32 GetMaxTimeSlice() const { return m_max_time_slice; }
    uint32 GetTotalTimeSlice() const { return m_total_time_slice; } 
	
	double GetEntrenchmentBonus() const { return m_entrenchment_bonus; }
	uint32 ParadropDistance() const { return m_paradrop_distance; }
	uint32 ParadropSuccessPercentage() const { return m_paradrop_success_percentage; }
	double EliteSpyBonus() const { return m_elite_spy_bonus; }
	double CitySecondAttemptSpyBonus() const { return m_city_second_attempt_spy_bonus; }
	
	sint32 AssasinationHappinessEffect() const { return m_assasination_happiness_effect; }
	uint32 AssasinationHappinessEffectTimer() const { return m_assasination_happiness_effect_timer; }
	uint32 GetSpiedUponWarinessTimer() const { return m_spied_upon_wariness_timer; }
	sint32 GetMaximumPartyCost() const { return m_maximum_party_cost; }
	double GetMaximumPartyChance() const { return m_maximum_party_chance; }
	sint32 GossipMapRadius() const { return m_gossip_map_radius; }
	double HearGossipChance() const { return m_hear_gossip_chance; }
	double GetFranchiseEffect() const { return m_franchise_effect; }
	sint32 TurnsToSueFranchise() const { return m_turns_to_sue_franchise; }
	
	double SlaverEliteChance() const { return m_slaver_elite_chance; }
	double EliteSlaverBonus() const { return m_elite_slaver_bonus; }
	
	double AbolitionistEliteChance() const { return m_abolitionist_elite_chance; }
	double EliteAbolitionistBonus() const { return m_elite_abolitionist_bonus; }
	double WatchfulCitySuccessModifier() const { return m_watchful_city_success_modifier; }
	double WatchfulCityDeathModifier() const { return m_watchful_city_death_modifier; }
	
	sint32 WatchfulCityTurns() const { return m_watchful_city_turns; }
	
	sint32 BioInfectionTurns() const { return m_bio_infection_turns; }
	sint32 NanoInfectionTurns() const { return m_nano_infection_turns; }
	double BioInfectionSpreadChance() const { return m_bio_infection_spread_chance; }
	double NanoInfectionSpreadChance() const { return m_nano_infection_spread_chance; }
	double BioTerrorKillPercentage() const { return m_bio_terror_kill_percentage; }
	
	double EliteTerroristBonus() const { return m_elite_terrorist_bonus; }
	double TerroristEliteChance() const { return m_terrorist_elite_chance; }
	double NukePopulationPercentage() const { return m_nuke_population_percentage; }
	double SpecialActionMoveCost() const { return m_special_action_move_cost; }
	
	double ClericConversionFactor() const { return m_cleric_conversion_factor; }
	double TelevangelistConversionFactor() const { return m_televangelist_conversion_factor; }
	
	double ReformationChance() const { return m_reformation_chance; }
	double ReformationDeathChance() const { return m_reformation_death_chance; }
	sint32 ReformationHappinessTime() const { return m_reformation_happiness_time; }
	sint32 ReformationHappinessAmount() const { return m_reformation_happiness_amount;}
	
	sint32 UnconvertedIndulgenceGold() const { return m_unconverted_indulgence_gold; }
	sint32 UnconvertedIndulgenceHappiness() const { return m_unconverted_indulgence_happiness; }
	sint32 ConvertedIndulgenceGold() const { return m_converted_indulgence_gold; }
	sint32 ConvertedIndulgenceHappiness() const { return m_converted_indulgence_happiness; }
	sint32 OtherFaithIndulgenceGold() const { return m_other_faith_indulgence_gold;}
	sint32 OtherFaithIndulgenceHappiness() const { return m_other_faith_indulgence_happiness; }
	sint32 SoothsayHappinessAmount() const { return m_soothsay_happy_amount; }
	
	sint32 HutBoxWidth() const { return m_hut_box_width; }
	sint32 HutBoxHeight() const { return m_hut_box_height; }
	double HutChancePerBox() const { return m_hut_chance_per_box; }
	sint32 MaxGoodyHutGold() const { return m_max_goody_hut_gold; }
	sint32 MaxGoodyHutAdvance() const { return m_max_goody_hut_advance; }
	sint32 MaxGoodyHutUnit() const { return m_max_goody_hut_unit; }
	
	double UnitRushModifier() const { return m_unit_rush_modifier; }
	double ImprovementRushModifier() const { return m_improvement_rush_modifier; }
	double WonderRushModifier() const { return m_wonder_rush_modifier; }
	double BuildingProductionToValueModifier() const {
		return m_building_production_to_value_modifier; }
	
	sint32 GetMonopolyThreshold(void) const { return (m_monopolyThreshold) ; }
	sint32 GetPopulationPollutionThreshold(void) const { return (m_populationPollutionWarningThreshold) ; }
	sint32 GetIndustrialPollutionThreshold(void) const { return (m_industrialPollutionWarningThreshold) ; }
	sint32 GetVeryHappyThreshold(void) const { return (m_veryHappyThreshold) ; }
	double CityGrowthCoefficient(void) const { return m_city_growth_coefficient; }
	sint32 GetRiotLevel() const { return m_riot_level; }

	//added DWT
	sint32 GetMaxMatchListCycles() const { return m_max_match_list_cycles; }
	// Modified by kaan to address bug # 12
	sint32 GetMinTurnsBetweenRevolt() const { return m_min_turns_between_revolt; }

	double PowerPointsToMaterialsConversion() const { return m_power_points_to_materials; }
	sint32 MaxAirliftStackSize() const { return m_max_airlift_stack_size; }
	
	sint32 RiverCellWidth() const { return m_river_cell_width; }
	void SetRiverCellWidth( sint32 river_cell_width )
	{ m_river_cell_width = river_cell_width; }
	sint32 RiverCellHeight() const { return m_river_cell_height; }
	void SetRiverCellHeight( sint32 river_cell_height )
	{ m_river_cell_height = river_cell_height; }
	
	sint32 GoldFromPiracy() const { return m_gold_from_piracy; }
	sint32 GetNoPiracyExpires() const { return (m_no_piracy_expires) ; }
	
	double SpaceLaunchCost() const { return m_space_launch_cost; }
	double SpaceLandCost() const { return m_space_land_cost; }
	sint32 WormholeOrbitHeightPercentage() const { return m_wormhole_orbit_height_percentage; }
	sint32 GetEndOfGameYearEarlyWarning(void) const { return (m_end_of_game_year_early_warning) ; }
	sint32 GetEndOfGameYear(void) const { return (m_end_of_game_year) ; }
	
	sint32 WormholeReturnTime() const { return m_wormhole_return_time; }
	sint32 WormholeSpeed() const { return m_wormhole_speed; }
	sint32 WormholeVisibleToAllTurns() const { return m_wormhole_visible_to_all_turns; }
	
	sint32 GetPopulationProducesPollutionThreshold(void) const { return (m_cityPopulationProducesPollutionThreshold) ; }
	sint32 GetProductionProducesPollutionThreshold(void) const { return (m_cityProductionProducesPollutionThreshold) ; }
	sint32 MaxGovernmentChangeTurns() const { return m_max_government_change_turns; }
	sint32 EmancipationUnhappinessTurns() const { return m_emancipation_unhappiness_turns; }
	double EmancipationUnhappinessAmount() const { return m_emancipation_unhappiness_amount; }
	double CaptureCityAdvanceChance() const { return m_capture_city_advance_chance; }
	double CityHealRate() const { return m_city_heal_rate; }
	double NormalHealRate() const { return m_normal_heal_rate; }
	double LocalPollutionLevel() const { return m_local_pollution_level; }
	double LocalPollutionChance() const { return m_local_pollution_chance; }

	double AssaultDestroyBuildingChance() const { return m_assault_destroy_building_chance; }
	double BombardDestroyBuildingChance() const { return m_bombard_destroy_building_chance; }

	sint32 AdvanceChoicesMin() const { return m_advance_choices_min; }
	sint32 AdvanceChoicesMax() const { return m_advance_choices_max; }

	sint32 AttackConverterUnhappinessTurns() const { return m_attack_converter_unhappiness_turns; }
	double AttackConverterUnhappinessAmount() const { return m_attack_converter_unhappiness_amount; }

	double MinStartDistanceCoefficient() const { return m_min_start_distance_coefficient; }
	double MaxStartDistanceCoefficient() const { return m_max_start_distance_coefficient; }
	sint32 MaxSameTiles() const { return m_max_same_tiles; }
	double CombatVeteranChance() const { return m_combat_veteran_chance; }

	sint32 StopTradeRounds() const { return m_stop_trade_rounds; }
	sint32 LeaveOurLandsRounds() const { return m_leave_our_lands_rounds; }
	sint32 ReducePollutionRounds() const { return m_reduce_pollution_rounds; }
	sint32 CaptureCityRounds() const { return m_capture_city_rounds; }
	sint32 EndPollutionRounds() const { return m_end_pollution_rounds; }
	sint32 AttackEnemyRounds() const { return m_attack_enemy_rounds; }
	sint32 MinContinentStartSize() const { return m_min_continent_start_size; }

	double BombardKillPopChance() const { return m_bombard_kill_pop_chance; }
	double AssaultKillPopChance() const { return m_assault_kill_pop_chance; }
	double CaptureKillPopChance() const { return m_capture_kill_pop_chance; }

	sint32 GetScaledPop( sint32 index ) const { return m_scaled_pop[index]; }

	sint32 PiracyKillsTraderChance() const { return m_piracy_kills_trader_chance; }
	sint32 UprisingChancePerUnguardedSlave() const { return m_uprising_chance_per_unguarded_slave; }
	sint32 MaxDisbandSize() const { return m_max_disband_size; }

	sint32 MaxRequestsPerPlayerPerTurn() const { return m_max_requests_per_player_per_turn; }

	sint32 SlavesPerMilitaryUnit() const { return m_slaves_per_military_unit; }
	sint32 MinAbsoluteStartDistance() const { return m_min_absolute_start_distance; }

	sint32 AveragePollutionTurns() const { return m_average_pollution_turns; }

	void	GetSmallMapSize(MapPoint &size) { size = m_small_map_size; }
	void	GetMediumMapSize(MapPoint &size) { size = m_medium_map_size; }
	void	GetLargeMapSize(MapPoint &size) { size = m_large_map_size; }
	void	GetGiganticMapSize(MapPoint &size) { size = m_gigantic_map_size; }

	void	GetMapSizeMapPoint(MAPSIZE size, MapPoint &mapPoint);

	double  RailLaunchPollution() { return m_rail_launch_pollution; }
	
	sint32  SpaceFuelCost() { return m_space_fuel_cost; }
	sint32  NonSpaceFuelCost() { return m_non_space_fuel_cost; }

    sint32  StarvationWarningFudgeFactor() { return m_starvation_warning_fudge_factor; }
    sint32  MaintenanceWarningFudgeFactor() { return m_maintenance_warning_fudge_factor; }
    sint32  SupportWarningFudgeFactor() { return m_support_warning_fudge_factor; }

	double RelativeAIProductionCheatCoef() { return m_relative_ai_production_cheat_coef; }
	double RelativeAIScienceCheatCoef() { return m_relative_ai_science_cheat_coef; }

	double InciteRevolutionGoldCoefficient() const { return m_incite_revolution_gold_coefficient; }
	double InciteRevolutionCapitolPenalty() const { return m_incite_revolution_capitol_penalty; }

	double InciteUprisingGoldCoefficient() const { return m_incite_uprising_gold_coefficient; }
	double InciteUprisingCapitolPenalty() const { return m_incite_uprising_capitol_penalty; }

	sint32 NukeKillTiles() { return m_nuke_kill_tiles; }

	sint32 BioInfectionUnhappiness() { return m_bio_infection_unhappiness; }

	sint32 MinEcoPactViolationLevel() { return m_min_eco_pact_violation_level; }
	double NanoInfectionTerroristDeathChance() { return m_nano_infection_terrorist_death_chance; }
	double BioInfectionTerroristDeathChance() { return m_bio_infection_terrorist_death_chance; }
	double GetFloodChangesCoastToWaterChance() { return m_flood_changes_coast_to_water_chance; }

	double GetAICheatEcoPactMin() { return m_ai_cheat_eco_pact_min; }
	double GetAICheatEcoPactMax() { return m_ai_cheat_eco_pact_max; }

	sint32 GetMaxAbsoluteGrowthRate() { return m_max_absolute_growth_rate; }

	double GetTradeDistanceCoefficient() { return m_trade_distance_coefficient; }
	sint32 GetMinLikeTilesForGood() { return m_min_like_tiles_for_good; }

	double GetMinGoodValue() { return m_min_good_value; }
	double GetMaxGoodValue() { return m_max_good_value; }

	double GetPiracyWasteCoefficient() { return m_piracy_waste_coefficient; }

	sint32 GetBorderIntRadius() { return m_border_int_radius; }
	sint32 GetBorderSquaredRadius() { return m_border_squared_radius; }

	double GetBioInfectionProductionCoef() { return m_bio_infection_production_coef; }
	double GetNanoBuildingKillPercentage() { return m_nano_building_kill_percentage; }

	sint32 GetBaseStarvationProtection() { return m_base_starvation_protection;}

	double GetCaravanCoef() {return m_caravan_coef;}

	sint32 GetPollutionCausedByNuke() { return m_pollution_caused_by_nuke;}

	void Serialize(CivArchive &archive) ; 
} ConstDB; 

extern ConstDB *g_theConstDB;

extern sint32 ConstDB_Parse(char *filename, C3DIR dir);

#endif
