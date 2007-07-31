











#include "c3.h"
#include "c3errors.h"

#include "c3files.h"

#include "Token.h"
#include "ErrMsg.h"
#include "Globals.h"
#include "StrDB.h"
#ifndef __MAKESPR__
#include "AdvanceRecord.h"
#include "BuildingRecord.h"
#endif

sint32 g_parse_line; 
sint32 g_saved_parse_line;

extern sint32 g_abort_parse; 

extern StringDB *g_theStringDB;

TokenData g_allTokens [] = { 
	
	
	{TOKEN_STRING, "TOKEN_STRING"}, 
	{TOKEN_QUOTED_STRING,"TOKEN_QUOTED_STRING"},   
	{TOKEN_MISSING_QUOTE,"TOKEN_MISSING_QUOTE"},  
	{TOKEN_NUMBER, "TOKEN_NUMBER"},
	{TOKEN_EOF, "TOKEN_EOF"},
	{TOKEN_UNKNOWN, "TOKEN_UNKNOWN"},	
	
	{TOKEN_OPEN_BRACE, "{"}, 
	{TOKEN_CLOSE_BRACE, "}"}, 
	
	{TOKEN_IMPORT, "import"},
	
	
	
	{TOKEN_ENABLING_ADVANCE, "ENABLING_ADVANCE"}, 
	{TOKEN_OBSOLETE_ADVANCE, "OBSOLETE_ADVANCE"},
	
	
	{TOKEN_ADVANCE_POLLUTION_SIZE_MODIFIER, "ADVANCE_POLLUTION_SIZE_MODIFIER"},
	{TOKEN_ADVANCE_POLLUTION_PRODUCTION_MODIFIER, "ADVANCE_POLLUTION_PRODUCTION_MODIFIER"},
	{TOKEN_ADVANCE_IS_GENETIC, "IS_GENETIC"},
	{TOKEN_ADVANCE_ALLOWS_ROADS, "ALLOWS_ROADS"},
	{TOKEN_ADVANCE_ALLOWS_RAILROADS, "ALLOWS_RAILROADS"},
	{TOKEN_ADVANCE_ALLOWS_MAGLEV, "ALLOWS_MAGLEV"},
	{TOKEN_ADVANCE_ALLOWS_IRRIGATION1, "ALLOWS_IRRIGATION_1"},
	{TOKEN_ADVANCE_ALLOWS_IRRIGATION2, "ALLOWS_IRRIGATION_2"},
	{TOKEN_ADVANCE_ALLOWS_IRRIGATION3, "ALLOWS_IRRIGATION_3"},
	{TOKEN_ADVANCE_ALLOWS_MINE1, "ALLOWS_MINE_1"},
	{TOKEN_ADVANCE_ALLOWS_MINE2, "ALLOWS_MINE_2"},
	{TOKEN_ADVANCE_ALLOWS_MINE3, "ALLOWS_MINE_3"},
	{TOKEN_ADVANCE_ALLOWS_CANAL_TUNNEL, "ALLOWS_TUNNELS"},
	{TOKEN_ADVANCE_ALLOWS_TRANSFORM, "ALLOWS_TRANSFORM"},
	{TOKEN_ADVANCE_CIVILIZATION, "ADVANCE_CIVILIZATION"},
	{TOKEN_ADVANCE_COST, "ADVANCE_COST"},
	{TOKEN_ADVANCE_ALLOWS_DEEP_OCEAN, "ALLOWS_DEEP_OCEAN"},
	{TOKEN_ADVANCE_ALLOWS_CAPITALIZATION, "ALLOWS_CAPITALIZATION"},
	{TOKEN_ADVANCE_ALLOWS_INFRASTRUCTURE, "ALLOWS_INFRASTRUCTURE"},
	{TOKEN_ADVANCE_ALLOWS_ALIEN_LIFE, "ALLOWS_ALIEN_LIFE"},
	{TOKEN_ADVANCE_REQUIRES_PROBE_RECOVERY, "NEED_PROBE"},
	{TOKEN_ADVANCE_ALLOWS_NUKE,	"ALLOWS_NUKE"},

	{TOKEN_NULL, "NULL"},
	
	
	
	
	{TOKEN_SHIELD_COST, "SHIELD_COST"},
	{TOKEN_POWER_POINTS, "POWER_POINTS"},
	{TOKEN_ADVANCE_AGE_INDEX, "ADVANCE_AGE_INDEX"},
	
	{TOKEN_WONDER_POLLUTION_CLEANER, "POLLUTION_CLEANER"},
	
	{TOKEN_WONDER_POLLUTION_BONUS, "POLLUTION_BONUS"},
	
	
	
	{TOKEN_MAX_HP, "MAX_HP"}, 
	{TOKEN_ATTACK, "ATTACK"}, 
	{TOKEN_DEFENSE, "DEFENSE"}, 
	{TOKEN_FIREPOWER, "FIREPOWER"}, 
	{TOKEN_ZB_RANGE_ATTACK, "ZB_RANGE_ATTACK"},
	
	{TOKEN_BATTLEFIELD_RANGE, "BATTLEFIELD_RANGE"}, 
	{TOKEN_BATTLEFIELD_RADIUS, "BATTLEFIELD_RADIUS"}, 
	{TOKEN_VISION_RANGE, "VISION_RANGE"}, 
	{TOKEN_ACTIVE_DEFENSE_RANGE, "ACTIVE_DEFENSE_RANGE"}, 
	{TOKEN_ELECTRONIC_COMBAT_FACTOR, "ELECTRONIC_COMBAT_FACTOR"}, 
	{TOKEN_MAX_MOVEMENT, "MAX_MOVEMENT"}, 
	{TOKEN_FUEL, "FUEL"},
	
	{TOKEN_FOOD_HUNGER, "FOOD_HUNGER"}, 
	{TOKEN_SHIELD_HUNGER, "SHIELD_HUNGER"},
	{TOKEN_DEFAULT_SPRITE, "DEFAULT_SPRITE"},
	{TOKEN_DEFAULT_ICON, "DEFAULT_ICON"},
	{TOKEN_DESCRIPTION, "DESCRIPTION"},
	
	{TOKEN_SETTLE_CITY_TYPE, "SETTLE_CITY_TYPE"}, 
	
	
	{TOKEN_SOUND_SELECT1,	"SOUND_SELECT1"},
	{TOKEN_SOUND_SELECT2,	"SOUND_SELECT2"},
	{TOKEN_SOUND_MOVE,		"SOUND_MOVE"},
	{TOKEN_SOUND_ACKNOWLEDGE, "SOUND_ACKNOWLEDGE"},
	{TOKEN_SOUND_CANTMOVE,	"SOUND_CANTMOVE"},
	{TOKEN_SOUND_ATTACK,	"SOUND_ATTACK"},
	
	{TOKEN_SOUND_WORK,		"SOUND_WORK"},
	{TOKEN_SOUND_VICTORY,	"SOUND_VICTORY"},
	{TOKEN_SOUND_DEATH,		"SOUND_DEATH"},
	
	
	{TOKEN_MOVEMENT_TYPE_LAND, "MOVEMENT_TYPE_LAND"},
	{TOKEN_MOVEMENT_TYPE_WATER, "MOVEMENT_TYPE_WATER"},
	{TOKEN_MOVEMENT_TYPE_SPACE, "MOVEMENT_TYPE_SPACE"},
	{TOKEN_MOVEMENT_TYPE_AIR, "MOVEMENT_TYPE_AIR"}, 
	{TOKEN_MOVEMENT_TYPE_MOUNTAIN, "MOVEMENT_TYPE_MOUNTAIN"}, 
	{TOKEN_MOVEMENT_TYPE_TRADE, "MOVEMENT_TYPE_TRADE"},
	{TOKEN_MOVEMENT_TYPE_SHALLOW_WATER, "MOVEMENT_TYPE_SHALLOW_WATER"},
	
	
	{TOKEN_LOSS_MOVE_TO_DMG_NONE, "LOSS_MOVE_TO_DMG_NONE"},
	{TOKEN_LOSS_MOVE_TO_DMG_TWO, "LOSS_MOVE_TO_DMG_TWO_MAX"},
	{TOKEN_TRANSFORM_UNIT, "TRANSFORM_UNIT"},
	
	{TOKEN_NO_FUEL_THEN_CRASH,"NO_FUEL_THEN_CRASH"},
	
	{TOKEN_CAN_CARRY_SMALL_LAND,  "CAN_CARRY_SMALL_LAND"},  
	{TOKEN_CAN_CARRY_MED_LAND,  "CAN_CARRY_MED_LAND"},      
	{TOKEN_CAN_CARRY_LARGE_LAND, "CAN_CARRY_LARGE_LAND"},
	
	{TOKEN_CAN_CARRY_SMALL_AIR,  "CAN_CARRY_SMALL_AIR"},   
	{TOKEN_CAN_CARRY_MED_AIR, "CAN_CARRY_MED_AIR"},     
	{TOKEN_CAN_CARRY_LARGE_AIR,  "CAN_CARRY_LARGE_AIR"},   
	
	{TOKEN_CAN_CARRY_SMALL_WATER, "CAN_CARRY_SMALL_WATER"}, 
	{TOKEN_CAN_CARRY_MED_WATER, "CAN_CARRY_MED_WATER"},
	{TOKEN_CAN_CARRY_LARGE_WATER, "CAN_CARRY_LARGE_WATER"},
	
	{TOKEN_CAN_CARRY_SMALL_SPACE, "CAN_CARRY_SMALL_SPACE"}, 
	{TOKEN_CAN_CARRY_MED_SPACE, "CAN_CARRY_MED_SPACE"},
	{TOKEN_CAN_CARRY_LARGE_SPACE, "CAN_CARRY_LARGE_SPACE"},
	{TOKEN_CAN_LIFT_OFF, "CAN_LIFT_OFF"},
	
	{TOKEN_PRIMARY_PROJECTILE, "PRIMARY_PROJECTILE"},
	{TOKEN_SECONDARY_PROJECTILE, "SECONDARY_PROJECTILE"},
	{TOKEN_PROJECTILE_PATH, "PROJECTILE_PATH"}, 
	
	{TOKEN_SIZE_SMALL,  "SIZE_SMALL"},   
	{TOKEN_SIZE_MED, "SIZE_MED"}, 
	{TOKEN_SIZE_LARGE, "SIZE_LARGE"}, 
	
	{TOKEN_NCARGO, "TRANSPORT_CAPACITY"}, 
	
	
	{TOKEN_SETTLE_LAND, "SETTLE_LAND"}, 
	{TOKEN_SETTLE_MOUNTAINS, "SETTLE_MOUNTAINS"}, 
	{TOKEN_SETTLE_WATER, "SETTLE_WATER"}, 
	{TOKEN_SETTLE_SPACE, "SETTLE_SPACE"}, 
	
	{TOKEN_CANT_CAPTURE_CITY, "CANT_CAPTURE_CITY"},
	{TOKEN_HAS_POP_AND_CAN_BUILD, "HAS_POP_AND_CAN_BUILD"},
	{TOKEN_IS_TRADER, "IS_TRADER"},
	
	{TOKEN_LAUNCH_DESTROYS_OZONE, "LAUNCH_DESTROYS_OZONE"},
	{TOKEN_DEATH_INCREASES_GLOBAL_WARMING, "DEATH_INCREASES_GLOBAL_WARMING"},
	
	{TOKEN_VISIBILITY_CLASS_0, "VISIBILITY_CLASS_0"},
	{TOKEN_VISIBILITY_CLASS_1, "VISIBILITY_CLASS_1"},
	{TOKEN_VISIBILITY_CLASS_2, "VISIBILITY_CLASS_2"},
	{TOKEN_VISIBILITY_CLASS_3, "VISIBILITY_CLASS_3"},
	{TOKEN_VISIBILITY_CLASS_4, "VISIBILITY_CLASS_4"},
	{TOKEN_VISIBILITY_CLASS_5, "VISIBILITY_CLASS_5"},
	{TOKEN_VISIBILITY_CLASS_6, "VISIBILITY_CLASS_6"},
	{TOKEN_VISIBILITY_CLASS_7, "VISIBILITY_CLASS_7"},
	{TOKEN_VISIBILITY_CLASS_8, "VISIBILITY_CLASS_8"},
	
	{TOKEN_VISIBILITY_CAN_SEE_0, "VISIBILITY_CAN_SEE_0"},
	{TOKEN_VISIBILITY_CAN_SEE_1, "VISIBILITY_CAN_SEE_1"},
	{TOKEN_VISIBILITY_CAN_SEE_2, "VISIBILITY_CAN_SEE_2"},
	{TOKEN_VISIBILITY_CAN_SEE_3, "VISIBILITY_CAN_SEE_3"},
	{TOKEN_VISIBILITY_CAN_SEE_4, "VISIBILITY_CAN_SEE_4"},
	{TOKEN_VISIBILITY_CAN_SEE_5, "VISIBILITY_CAN_SEE_5"},
	{TOKEN_VISIBILITY_CAN_SEE_6, "VISIBILITY_CAN_SEE_6"},
	{TOKEN_VISIBILITY_CAN_SEE_7, "VISIBILITY_CAN_SEE_7"},
	{TOKEN_VISIBILITY_CAN_SEE_8, "VISIBILITY_CAN_SEE_8"},
	
	{TOKEN_IS_SPACE_LAUNCH, "IS_SPACE_LAUNCHER"}, 
	{TOKEN_IS_SPACE_LAND, "IS_SPACE_LANDER"},
	{TOKEN_EXERTS_MARTIAL_LAW, "EXERTS_MARTIAL_LAW"},
	{TOKEN_CANT_ENTRENCH, "CANT_ENTRENCH"},
	{TOKEN_CANT_PATROL, "CANT_PATROL"},
	{TOKEN_NEEDS_NO_SUPPORT, "NEEDS_NO_SUPPORT"}, 
	{TOKEN_CAN_EXPEL_POP, "CAN_EXPEL_POP"},
	{TOKEN_IS_FLANKER, "IS_FLANKER"},
	{TOKEN_DEATH_EFFECTS_HAPPY,  "DEATH_EFFECTS_HAPPY"},
	{TOKEN_CAN_BEACH_ASSAULT, "CAN_BEACH_ASSAULT"},
	
	
	{TOKEN_IGNORE_ZOC, "IGNORE_ZOC"},
	{TOKEN_HAS_NO_ZOC, "HAS_NO_ZOC"},
	
	{TOKEN_CAN_BE_EXPELLED, "CAN_BE_EXPELLED"},
	{TOKEN_CAN_BE_SUED, "CAN_BE_SUED"},
	{TOKEN_ATTACK_FROM_SPACESHIP, "ATTACK_FROM_SPACESHIP"},
	{TOKEN_CANT_BE_ATTACKED, "CANT_BE_ATTACKED"},
	{TOKEN_CAN_BOMBARD_AIR, "CAN_BOMBARD_AIR"},
	{TOKEN_CAN_BOMBARD_LAND, "CAN_BOMBARD_LAND"},
	{TOKEN_CAN_BOMBARD_WATER, "CAN_BOMBARD_WATER"},
	{TOKEN_CAN_BOMBARD_SPACE, "CAN_BOMBARD_SPACE"},
	{TOKEN_CAN_BOMBARD_MOUNTAIN, "CAN_BOMBARD_MOUNTAIN"},
	{TOKEN_BOMBARD_ROUNDS, "BOMBARD_ROUNDS"},
	
	{TOKEN_CAN_COUNTER_BOMBARD, "CAN_COUNTER_BOMBARD"},
	{TOKEN_DEFEND_LAND, "DEFEND_LAND"},
	{TOKEN_DEFEND_AIR, "DEFEND_AIR"},
	{TOKEN_DEFEND_WATER, "DEFEND_WATER"},
	{TOKEN_DEFEND_SPACE, "DEFEND_SPACE"},
	{TOKEN_DEFEND_MOUNTAIN, "DEFEND_MOUNTAIN"},
	
	{TOKEN_DEFEND_AGAINST_SPIES, "DEFEND_AGAINST_SPIES"},
	{TOKEN_IS_MOUNTED, "IS_MOUNTED"},
	{TOKEN_IS_CARGO_POD, "IS_CARGO_POD"},
	{TOKEN_IGNORE_CITY_WALLS, "IGNORE_CITY_WALLS"},
	{TOKEN_IS_SUBMARINE, "IS_SUBMARINE"},
	
	{TOKEN_NO_LAND_ATTACK, "NO_LAND_ATTACK"},
	{TOKEN_NO_SEA_ATTACK, "NO_SEA_ATTACK"},
	{TOKEN_NO_AIR_ATTACK, "NO_AIR_ATTACK"},
	{TOKEN_NO_MOUNTAIN_ATTACK, "NO_MOUNTAIN_ATTACK"},
	{TOKEN_NO_SPACE_ATTACK, "NO_SPACE_ATTACK"},
	{TOKEN_NO_SHALLOW_WATER_ATTACK, "NO_SHALLOW_WATER_ATTACK"},
	{TOKEN_NO_TRADE_ATTACK, "NO_TRADE_ATTACK"},
	{TOKEN_NO_UNDERWATER_ATTACK, "NO_UNDERWATER_ATTACK"},
	
	{TOKEN_LAND_ATTACK, "LAND_ATTACK"},
	{TOKEN_SEA_ATTACK, "SEA_ATTACK"},
	{TOKEN_AIR_ATTACK, "AIR_ATTACK"},
	{TOKEN_MOUNTAIN_ATTACK, "MOUNTAIN_ATTACK"},
	{TOKEN_SPACE_ATTACK, "SPACE_ATTACK"},
	{TOKEN_SHALLOW_WATER_ATTACK, "SHALLOW_WATER_ATTACK"},
	{TOKEN_TRADE_ATTACK, "TRADE_ATTACK"},
	{TOKEN_UNDERWATER_ATTACK, "UNDERWATER_ATTACK"},
	
	{TOKEN_PARATROOPER, "PARATROOPER"},
	{TOKEN_ASSISTED_DROPS, "ASSISTED_DROPS"},
	{TOKEN_BONUS_AGAINST_MOUNTED, "BONUS_AGAINST_MOUNTED"},
	{TOKEN_SINGLE_USE, "SINGLE_USE"},
	{TOKEN_BONUS_AIR_DEFENSE, "BONUS_AIR_DEFENSE"},
	
	{TOKEN_WOODEN_SHIP, "WOODEN_SHIP"},
	{TOKEN_ATTACK_WOODEN_SHIP_BONUS, "ATTACK_WOODEN_SHIP_BONUS"},
	{TOKEN_ATTACK_CITY_BONUS, "ATTACK_CITY_BONUS"},
	{TOKEN_ATTACK_BONUS_SUBMARINE, "ATTACK_BONUS_SUBMARINE"},
	
	{TOKEN_REPLACE_WITH_CARGO_MODULE, "REPLACE_WITH_CARGO_MODULE"},
	{TOKEN_VISIBILE_ATTACKING, "VISIBILE_ATTACKING"},
	
	{TOKEN_NUCLEAR_ATTACK, "NUCLEAR_ATTACK"},
	
	{TOKEN_CAN_SUE, "CAN_SUE"},
	{TOKEN_IS_STEALTHY, "IS_STEALTHY"},
	{TOKEN_SEE_SURFACE_FROM_SPACE, "SEE_SURFACE_FROM_SPACE"},
	
	{TOKEN_ESTABLISH_EMBASSY, "ESTABLISH_EMBASSY"},
	{TOKEN_THROW_PARTY, "THROW_PARTY"},
	{TOKEN_HEAR_GOSSIP, "HEAR_GOSSIP"},
	
	{TOKEN_INVESTIGATE_CITY, "INVESTIGATE_CITY"},
	{TOKEN_STEAL_TECHNOLOGY, "STEAL_TECHNOLOGY"},
	{TOKEN_INCITE_REVOLUTION, "INCITE_REVOLUTION"},
	{TOKEN_ASSASINATE_RULER, "ASSASINATE_RULER"},
	{TOKEN_INVESTIGATE_READINESS, "INVESTIGATE_READINESS"},
	{TOKEN_NULLIFY_CITY_WALLS, "NULLIFY_CITY_WALLS"},
	
	{TOKEN_CREATE_FRANCHISE, "CREATE_FRANCHISE"},
	{TOKEN_CAUSE_UNHAPPINESS, "CAUSE_UNHAPPINESS"},
	
	{TOKEN_CONDUCT_HITS, "CONDUCT_HITS"},
	{TOKEN_BIO_TERROR, "BIO_TERROR"},
	{TOKEN_NANO_TERROR, "NANO_TERROR"},
	
	{TOKEN_SLAVE_RAIDS, "SLAVE_RAIDS"},
	{TOKEN_SETTLER_SLAVE_RAIDS, "SETTLER_SLAVE_RAIDS"},
	{TOKEN_SLAVE_UPRISING, "SLAVE_UPRISING"},
	{TOKEN_UNDERGROUND_RAILWAY, "UNDERGROUND_RAILWAY"},
	{TOKEN_VICTORY_ENSLAVEMENT, "VICTORY_ENSLAVEMENT"},
	
	{TOKEN_DEFUSE_LAND_MINES, "SEE_LAND_MINES"},
	{TOKEN_DEFUSE_SEA_MINES, "SEA_SEA_MINES"},
	{TOKEN_DEFUSE_XRAY_MINES, "SEE_XRAY_MINES"},
	
	{TOKEN_INDULGENCE_SALES, "INDULGENCE_SALES"},
	{TOKEN_CONVERT_CITIES, "CONVERT_CITIES"},
	{TOKEN_BUILD_XRAY_MINES, "BUILD_XRAY_MINES"},
	
	{TOKEN_BUILD_WONDER, "BUILD_WONDER"},
	{TOKEN_GOVERNMENT_TYPE, "GOVERNMENT_TYPE"},
	{TOKEN_NO_SLAVES, "NO_SLAVES"},
	{TOKEN_CREATE_RIFT, "CREATE_RIFT"},
	{TOKEN_CAN_CLOAK, "CAN_CLOAK"},
	
	{TOKEN_PARATROOPER_TRANSPORT, "PARATROOPER_TRANSPORT"},
	{TOKEN_CANT_BUILD, "CANT_BUILD"},
	{TOKEN_PLANT_NUKE, "PLANT_NUKE"},
	{TOKEN_IS_TELEVANGELIST, "IS_TELEVANGELIST"},
	{TOKEN_CAN_SOOTHSAY, "CAN_SOOTHSAY"},
	{TOKEN_CAN_BE_RUSTLED, "CAN_BE_RUSTLED"},
	{TOKEN_CREATE_PARKS, "CREATE_PARKS"},
	{TOKEN_INJUNCTIONS, "INJUNCTIONS"},
	{TOKEN_LAND_CITY_CAN_BUILD, "LAND_CITY_CAN_BUILD"},
	{TOKEN_SEA_CITY_CAN_BUILD, "SEA_CITY_CAN_BUILD"},
	{TOKEN_SPACE_CITY_CAN_BUILD, "SPACE_CITY_CAN_BUILD"},
	{TOKEN_BOMBARD_RANGE, "BOMBARD_RANGE"},
	{TOKEN_IS_SPECIAL_FORCES, "IS_SPECIAL_FORCES"},
	{TOKEN_WORMHOLE_PROBE, "WORMHOLE_PROBE"},
	{TOKEN_NOT_AFFECTED_BY_LAND_MINES, "NOT_AFFECTED_BY_LAND_MINES"},
	{TOKEN_ENABLES_CARRIER_ACTIVE_DEFENSE, "ENABLES_CARRIER_ACTIVE_DEFENSE"},
	{TOKEN_ACTIVE_DEFENSE_ONLY_WHEN_CARRYING_ENABLERS, "ACTIVE_DEFENSE_ONLY_WHEN_CARRYING_ENABLERS"},
	{TOKEN_BUILDING_REMOVES_A_POP, "BUILDING_REMOVES_A_POP"},
	{TOKEN_UNIT_EXPLODES, "UNIT_EXPLODES"},
	{TOKEN_UNIT_ADVERTISES, "ADVERTISES"},
	{TOKEN_UNIT_CAN_RUSTLE, "CAN_RUSTLE"},
	{TOKEN_ONLY_BUILD_ONE, "ONLY_BUILD_ONE"},
	{TOKEN_CAN_REFORM, "CAN_REFORM"},
	{TOKEN_CAN_PILLAGE, "CAN_PILLAGE"},
	{TOKEN_CAN_PIRATE, "CAN_PIRATE"},
	{TOKEN_CAN_EXPEL, "CAN_EXPEL"},
	{TOKEN_CITY_GROWTH_MODIFIER, "CITY_GROWTH_MODIFIER"},
	{TOKEN_SET_USER_FLAG, "SET_USER_FLAG"},

	{TOKEN_REVOLUTION, "REVOLUTION"},

	
	{TOKEN_BONUS_TRADE, "BONUS_TRADE"}, 
	{TOKEN_BONUS_SHIELD, "BONUS_SHIELD"},
	{TOKEN_BONUS_FOOD, "BONUS_FOOD"}, 
	
	
	
	{TOKEN_HUNGER, "HUNGER"},
	{TOKEN_BONUS_GOLD, "BONUS_GOLD"},
	{TOKEN_BONUS_LUXURY, "BONUS_LUXURY"},
	{TOKEN_BONUS_SCIENCE, "BONUS_SCIENCE"},
	{TOKEN_KILL_PREF, "KILL_PREF"},
	{TOKEN_TYPE_PREF, "TYPE_PREF"},
	{TOKEN_MIN_SIZE, "MIN_SIZE"},
	{TOKEN_MAX_SIZE, "MAX_SIZE"},
	{TOKEN_IMPROVEMENT, "IMPROVEMENT"},
	
	{TOKEN_ATTITUDE_NO_EFFECT, "ATTITUDE_NO_EFFECT"},
	{TOKEN_CAN_WORK_FIELDS, "CAN_WORK_FIELDS"},
	{TOKEN_AFFECTS_CONTENTMENT_TOTAL, "AFFECTS_CONTENTMENT_TOTAL"},
	{TOKEN_IS_UNPAID, "IS_UNPAID"},
	{TOKEN_CAN_WORK_CITY, "CAN_WORK_CITY"},
	{TOKEN_GOVERNMENT, "GOVERNMENT"},
	{TOKEN_ADD_HAPPY_ADJUST, "ADD_HAPPY_ADJUST"},
	{TOKEN_REMOVE_HAPPY_ADJUST, "REMOVE_HAPPY_ADJUST"},
	{TOKEN_IS_SLAVE, "IS_SLAVE"},
	
	
	{TOKEN_BASE_CONTENTMENT, "BASE_CONTENTMENT"}, 
	{TOKEN_GRANARY_COEF, "GRANARY_COEF"},    
	{TOKEN_BIG_CITY_SCALE, "BIG_CITY_SCALE"},
	{TOKEN_BIG_CITY_OFFSET, "BIG_CITY_OFFSET"},
	{TOKEN_SCIENCE_HANDICAP, "SCIENCE_HANDICAP"},
	
	{TOKEN_DIFFICULTY_POLLUTION_MULTIPLIER, "POLLUTION_MULTIPLIER"},
	{TOKEN_DIFFICULTY_RIOT_CHANCE, "RIOT_CHANCE"},
	{TOKEN_DIFFICULTY_STARVATION_EFFECT, "STARVATION_EFFECT"},
	{TOKEN_DIFFICULTY_STARTING_GOLD, "STARTING_GOLD"},
	{TOKEN_DIFFICULTY_VISION_BONUS, "SUPER_STEALTH_FUN_FACTOR"},
	{TOKEN_DIFFICULTY_BASE_SCORE, "BASE_SCORE"},
	
	
	{TOKEN_TIME_SCALE,		"TIME_SCALE"},
	{TOKEN_START_YEAR,		"START_YEAR"},
	{TOKEN_NUM_PERIODS,		"NUM_PERIODS"},
	{TOKEN_PERIOD,			"PERIOD"},
	{TOKEN_START_TURN,		"START_TURN"},
	{TOKEN_YEARS_PER_TURN,	"YEARS_PER_TURN"},
    {TOKEN_NEGATIVE_YEAR_FORMAT, "NEGATIVE_YEAR_FORMAT"},
    {TOKEN_POSITIVE_YEAR_FORMAT, "POSITIVE_YEAR_FORMAT"},

	{TOKEN_CELEBRATION_FACTOR, "CELEBRATION_FACTOR"},
	{TOKEN_ADVANCES_FACTOR, "ADVANCES_FACTOR"},
	{TOKEN_WONDERS_FACTOR, "WONDERS_FACTOR"},
	{TOKEN_POPULATION_FACTOR, "POPULATION_FACTOR"},
	{TOKEN_CITY_FACTOR, "CITY_FACTOR"},
	{TOKEN_PEACE_FACTOR, "PEACE_FACTOR"},
	{TOKEN_TURN_FACTOR, "TURN_FACTOR"},
	{TOKEN_DIFFICULTY_BONUS, "DIFFICULTY_BONUS"},
	{TOKEN_MAP_FACTOR, "MAP_FACTOR"},
	{TOKEN_OPPONENTS_FACTOR, "OPPONENTS_FACTOR"},
	{TOKEN_WONDER_VICTORY_BONUS, "WONDER_VICTORY_BONUS"},
	{TOKEN_UNIT_FACTOR, "UNIT_FACTOR"},
	{TOKEN_RIOT_FACTOR, "RIOT_FACTOR"},
	{TOKEN_REVOLUTION_FACTOR, "REVOLUTION_FACTOR"},
	{TOKEN_POLLUTION_FACTOR, "POLLUTION_FACTOR"},
	
	{TOKEN_AI_START_UNITS, "AI_START_UNITS"},
	{TOKEN_AI_START_GOLD, "AI_START_GOLD"},
	{TOKEN_AI_START_ADVANCES, "AI_START_ADVANCES"},
	{TOKEN_AI_START_PUBLIC_WORKS, "AI_START_PUBLIC_WORKS"},
	{TOKEN_HUMAN_START_LOCATION, "HUMAN_START_LOCATION"},
	{TOKEN_AI_INTELLIGENCE_FACTOR, "AI_INTELLIGENCE_FACTOR"},
	{TOKEN_AI_GANG_UP_FACTOR, "AI_GANG_UP_FACTOR"},
	{TOKEN_DISTANCE_FROM_CAPITOL_ADJUSTMENT, "DISTANCE_FROM_CAPITOL_ADJUSTMENT"},
	{TOKEN_AI_DISTANCE_FROM_CAPITOL_ADJUSTMENT, "AI_DISTANCE_FROM_CAPITOL_ADJUSTMENT"},
	{TOKEN_POLLUTION_ADJUST, "POLLUTION_ADJUST"},

	{TOKEN_AI_MIN_BEHIND_PERCENT, "AI_MIN_BEHIND_PERCENT"},
	{TOKEN_AI_MAX_BEHIND_PERCENT, "AI_MAX_BEHIND_PERCENT"},
	{TOKEN_AI_MIN_AHEAD_PERCENT, "AI_MIN_AHEAD_PERCENT"},
	{TOKEN_AI_MAX_AHEAD_PERCENT, "AI_MAX_AHEAD_PERCENT"},

	{TOKEN_AI_MIN_BEHIND_TECHNOLOGY_COST, "AI_MIN_BEHIND_TECHNOLOGY_COST"},
	{TOKEN_AI_MAX_BEHIND_TECHNOLOGY_COST, "AI_MAX_BEHIND_TECHNOLOGY_COST"},
	{TOKEN_AI_MIN_BEHIND_PRODUCTION_COST_ADJUSTMENT, "AI_MIN_BEHIND_PRODUCTION_COST_ADJUSTMENT"},
	{TOKEN_AI_MAX_BEHIND_PRODUCTION_COST_ADJUSTMENT, "AI_MAX_BEHIND_PRODUCTION_COST_ADJUSTMENT"},
	{TOKEN_AI_MIN_BEHIND_GOLD_ADJUSTMENT, "AI_MIN_BEHIND_GOLD_ADJUSTMENT"},
	{TOKEN_AI_MAX_BEHIND_GOLD_ADJUSTMENT, "AI_MAX_BEHIND_GOLD_ADJUSTMENT"},

	{TOKEN_AI_MIN_AHEAD_TECHNOLOGY_COST, "AI_MIN_AHEAD_TECHNOLOGY_COST"},
	{TOKEN_AI_MAX_AHEAD_TECHNOLOGY_COST, "AI_MAX_AHEAD_TECHNOLOGY_COST"},
	{TOKEN_AI_MIN_AHEAD_PRODUCTION_COST_ADJUSTMENT, "AI_MIN_AHEAD_PRODUCTION_COST_ADJUSTMENT"},
	{TOKEN_AI_MAX_AHEAD_PRODUCTION_COST_ADJUSTMENT, "AI_MAX_AHEAD_PRODUCTION_COST_ADJUSTMENT"},
	{TOKEN_AI_MIN_AHEAD_GOLD_ADJUSTMENT, "AI_MIN_AHEAD_GOLD_ADJUSTMENT"},
	{TOKEN_AI_MAX_AHEAD_GOLD_ADJUSTMENT, "AI_MAX_AHEAD_GOLD_ADJUSTMENT"},

	{TOKEN_MAX_HUMAN_ADVANCES, "MAX_HUMAN_ADVANCES"},
	{TOKEN_MAX_AI_ADVANCES, "MAX_AI_ADVANCES"},
	{TOKEN_ADVANCE_CHANCES, "ADVANCE_CHANCES"},
	{TOKEN_HUMAN_SCIENCE_BONUS, "HUMAN_SCIENCE_BONUS"},
	{TOKEN_HUMAN_FOOD_BONUS, "HUMAN_FOOD_BONUS"},
	{TOKEN_EXTRA_SETTLER_CHANCE, "EXTRA_SETTLER_CHANCE"},
	{TOKEN_MAX_SCORE, "MAX_SCORE"},

	
	
	{TOKEN_UNIT_SPRITE,				"UNIT_SPRITE"},
	{TOKEN_PROJECTILE_SPRITE,		"PROJECTILE_SPRITE"},
	{TOKEN_EFFECT_SPRITE,			"EFFECT_SPRITE"},
	
	{TOKEN_SPRITE_NUM_FRAMES,		"SPRITE_NUM_FRAMES"},
	{TOKEN_SPRITE_FIRST_FRAME,		"SPRITE_FIRST_FRAME"},
	{TOKEN_SPRITE_WIDTH,			"SPRITE_WIDTH"},
	{TOKEN_SPRITE_HEIGHT,			"SPRITE_HEIGHT"},
	{TOKEN_SPRITE_HOT_POINT,			"SPRITE_HOT_POINT"},
	{TOKEN_SPRITE_HOT_POINTS,		"SPRITE_HOT_POINTS"},
	
	{TOKEN_UNIT_SPRITE_MOVE,			"UNIT_SPRITE_MOVE"},
	{TOKEN_UNIT_SPRITE_ATTACK,		"UNIT_SPRITE_ATTACK"},
	{TOKEN_UNIT_SPRITE_VICTORY,		"UNIT_SPRITE_VICTORY"},
	{TOKEN_UNIT_SPRITE_IDLE,			"UNIT_SPRITE_IDLE"},
	{TOKEN_UNIT_SPRITE_WORK,			"UNIT_SPRITE_WORK"},
	{TOKEN_UNIT_SPRITE_FIREPOINTS,	"UNIT_SPRITE_FIREPOINTS"},
	{TOKEN_UNIT_SPRITE_FIREPOINTS_WORK, "UNIT_SPRITE_FIREPOINTS_WORK"},
	{TOKEN_UNIT_SPRITE_MOVEOFFSETS,	"UNIT_SPRITE_MOVEOFFSETS"},
	{TOKEN_UNIT_SPRITE_SHIELDPOINTS, "UNIT_SPRITE_SHIELDPOINTS"},
	
	{TOKEN_UNIT_SPRITE_SHIELDPOINTS_MOVE,	"UNIT_SPRITE_SHIELDPOINTS_MOVE"},
	{TOKEN_UNIT_SPRITE_SHIELDPOINTS_ATTACK,	"UNIT_SPRITE_SHIELDPOINTS_ATTACK"},
	{TOKEN_UNIT_SPRITE_SHIELDPOINTS_IDLE,	"UNIT_SPRITE_SHIELDPOINTS_IDLE"},
	{TOKEN_UNIT_SPRITE_SHIELDPOINTS_VICTORY, "UNIT_SPRITE_SHIELDPOINTS_VICTORY"},
	{TOKEN_UNIT_SPRITE_SHIELDPOINTS_WORK,	"UNIT_SPRITE_SHIELDPOINTS_WORK"},
	
	{TOKEN_UNIT_SPRITE_IS_DEATH, "UNIT_SPRITE_IS_DEATH"},
	{TOKEN_UNIT_SPRITE_ATTACK_IS_DIRECTIONAL, "UNIT_SPRITE_ATTACK_IS_DIRECTIONAL"},
	
	{TOKEN_PROJECTILE_SPRITE_MOVE,	"PROJECTILE_SPRITE_MOVE"},
	{TOKEN_PROJECTILE_SPRITE_FIREPOINTS,	"PROJECTILE_SPRITE_FIREPOINTS"},
	{TOKEN_PROJECTILE_SPRITE_MOVEOFFSETS,	"PROJECTILE_SPRITE_MOVEOFFSETS"},
	
	{TOKEN_EFFECT_SPRITE_PLAY,	"EFFECT_SPRITE_PLAY"},
	{TOKEN_EFFECT_SPRITE_FLASH,	"EFFECT_SPRITE_FLASH"},
	{TOKEN_EFFECT_SPRITE_FIREPOINTS,	"EFFECT_SPRITE_FIREPOINTS"},
	{TOKEN_EFFECT_SPRITE_MOVEOFFSETS,	"EFFECT_SPRITE_MOVEOFFSETS"},
	
	
	
	{TOKEN_GOOD_SPRITE,					"GOOD_SPRITE"},
	
	{TOKEN_GOOD_SPRITE_IDLE,			"GOOD_SPRITE_IDLE"},
	{TOKEN_GOOD_GOLD_VALUE,         "GOOD_GOLD_VALUE"},
	{TOKEN_GOOD_SOUND_ID,			"GOOD_SOUND_ID"},
	
	
	{TOKEN_ANIM,						"ANIM"},
	{TOKEN_ANIM_TYPE,				"ANIM_TYPE"},
	{TOKEN_ANIM_NUM_FRAMES,			"ANIM_NUM_FRAMES"},
	{TOKEN_ANIM_PLAYBACK_TIME,		"ANIM_PLAYBACK_TIME"},
	{TOKEN_ANIM_DELAY,				"ANIM_DELAY"},
	{TOKEN_ANIM_FRAME_DATA,			"ANIM_FRAME_DATA"},
	{TOKEN_ANIM_MOVE_DELTAS,			"ANIM_MOVE_DELTAS"},
	{TOKEN_ANIM_TRANSPARENCIES,		"ANIM_TRANSPARENCIES"},
	
	
	
	{TOKEN_TILESET_TILE,			"TILESET_TILE"},
	{TOKEN_TILESET_TILE_BASE_TYPE,	"TILESET_TILE_BASE_TYPE"},
	{TOKEN_TILESET_TILE_TRANS_0,	"TILESET_TILE_TRANS_0"},
	{TOKEN_TILESET_TILE_TRANS_1,	"TILESET_TILE_TRANS_1"},
	{TOKEN_TILESET_TILE_TRANS_2,	"TILESET_TILE_TRANS_2"},
	{TOKEN_TILESET_TILE_TRANS_3,	"TILESET_TILE_TRANS_3"},
	{TOKEN_TILESET_MEGATILE,		"TILESET_MEGATILE"},
	{TOKEN_TILESET_MEGATILE_CONFIG,	"TILESET_MEGATILE_CONFIG"},
	{TOKEN_TILESET_MEGATILE_INFO,	"TILESET_MEGATILE_INFO"},
	
	{TOKEN_TILESET_TRANSFORM,		"TILESET_TRANSFORM"},
	{TOKEN_TILESET_TRANSFORM_TO,	"TILESET_TRANSFORM_TO"},
	{TOKEN_TILESET_TRANSFORM_TO_LIST,"TILESET_TRANSFORM_TO_LIST"},
	{TOKEN_TILESET_TRANSITION,		"TILESET_TRANSITION"},
	{TOKEN_TILESET_TRANSITION_FROM,	"TILESET_TRANSITION_FROM"},
	{TOKEN_TILESET_TRANSITION_TO,	"TILESET_TRANSITION_TO"},
	{TOKEN_TILESET_RIVER_TRANSFORM, "TILESET_RIVER_TRANSFORM"},
	{TOKEN_TILESET_RIVER_PIECE,		"TILESET_RIVER_PIECE"},
	
	{TOKEN_TILESET_END,				"TILESET_END"},
	
	{TOKEN_COLORSET,				"COLORSET"},
	{TOKEN_COLORSET_COLOR,			"COLORSET_COLOR"},
	
	
	{TOKEN_PRODUCTION_COST,         "PRODUCTION_COST"},
	{TOKEN_PRODUCTION_TIME,         "PRODUCTION_TIME"},
	{TOKEN_RADAR_RANGE,             "RADAR_RANGE"},
	{TOKEN_DEFENSE_BONUS,           "DEFENSE_BONUS"},
	
	
	{TOKEN_IS_AIRFIELD,             "IS_AIRFIELD"},
	{TOKEN_IS_FORT,                 "IS_FORT"},
	{TOKEN_IS_LAND_MINE,            "IS_LAND_MINE"},
	{TOKEN_IS_LISTENING_POST,       "IS_LISTENING_POST"},
	{TOKEN_IS_RADAR,                "IS_RADAR"},
	{TOKEN_IS_HEAL_UNITS,           "IS_HEAL_UNITS"},
	{TOKEN_INSTALLATION_LAND,       "INSTALLATION_LAND"},
	{TOKEN_INSTALLATION_WATER,      "INSTALLATION_WATER"},
	{TOKEN_INSTALLATION_SPACE,      "INSTALLATION_SPACE"},
	
	{TOKEN_FOREST_COST, "FOREST_COST"},
	{TOKEN_PLAINS_COST, "PLAINS_COST"},
	{TOKEN_TUNDRA_COST, "TUNDRA_COST"},
	{TOKEN_GLACIER_COST, "GLACIER_COST"},
	{TOKEN_GRASSLAND_COST, "GRASSLAND_COST"},
	{TOKEN_DESERT_COST, "DESERT_COST"},
	{TOKEN_SWAMP_COST, "SWAMP_COST"},
	{TOKEN_JUNGLE_COST, "JUNGLE_COST"},
	{TOKEN_MOUNTAIN_COST, "MOUNTAIN_COST"},
	{TOKEN_HILL_COST, "HILL_COST"},
	{TOKEN_SHALLOW_COST, "SHALLOW_COST"},
	{TOKEN_DEEP_COST, "DEEP_COST"},
	{TOKEN_RIFT_COST, "RIFT_COST"},
	{TOKEN_TRENCH_COST, "TRENCH_COST"},
	
	{TOKEN_BEACH_COST, "BEACH_COST"},
	{TOKEN_SHELF_COST, "SHELF_COST"},
	{TOKEN_VOLCANO_COST, "VOLCANO_COST"},
	{TOKEN_SPACE_COST, "SPACE_COST"},
	{TOKEN_DEAD_COST, "DEAD_COST"},
	{TOKEN_DEAD_HILL_COST, "DEAD_HILL_COST"},
	
	{TOKEN_BROWN_HILL_COST, "BROWN_HILL_COST"},
	{TOKEN_BROWN_MOUNTAIN_COST, "BROWN_MOUNTAIN_COST"},
	{TOKEN_WHITE_HILL_COST, "WHITE_HILL_COST"},
	{TOKEN_WHITE_MOUNTAIN_COST, "WHITE_MOUNTAIN_COST"},
	
	{TOKEN_BUILD_SOUND_ID,		"BUILD_SOUND_ID"},
	
	{TOKEN_WONDER_DEFAULT_ICON,		"WONDER_DEFAULT_ICON"},
	{TOKEN_WONDER_MOVIE,				"WONDER_MOVIE"},
	{TOKEN_WONDER_DESCRIPTION,		"WONDER_DESCRIPTION"},
	{TOKEN_IMPROVE_DEFAULT_ICON,		"IMPROVE_DEFAULT_ICON"},
	{TOKEN_IMPROVE_DESCRIPTION,		"IMPROVE_DESCRIPTION"},
	
	{TOKEN_ADVANCE_DEFAULT_ICON,		"ADVANCE_DEFAULT_ICON"},
	{TOKEN_CONCEPT_DEFAULT_ICON,		"CONCEPT_DEFAULT_ICON"},
	{TOKEN_TERRAIN_DEFAULT_ICON,		"TERRAIN_DEFAULT_ICON"},
	
	{TOKEN_TILEIMP_DEFAULT_ICON,		"TILEIMP_DEFAULT_ICON"},
	
	
	
	
	{TOKEN_BARBARIAN_HUT_CHANCE, "BARBARIAN_HUT_CHANCE"},
	{TOKEN_MAX_HUT_BARBARIANS, "MAX_HUT_BARBARIANS"},
	{TOKEN_GOLD_HUT_CHANCE, "GOLD_HUT_CHANCE"},
	{TOKEN_MIN_HUT_GOLD, "MIN_HUT_GOLD"},
	{TOKEN_MAX_HUT_GOLD, "MAX_HUT_GOLD"},
	{TOKEN_ADVANCE_HUT_CHANCE, "ADVANCE_HUT_CHANCE"},
	{TOKEN_MAX_HUT_ADVANCE_PREREQUISITES, "MAX_HUT_ADVANCE_PREREQUISITES"},
	{TOKEN_UNIT_HUT_CHANCE, "UNIT_HUT_CHANCE"},
	{TOKEN_MAX_UNIT_PREREQUISITES, "MAX_UNIT_PREREQUISITES"},
	{TOKEN_CITY_HUT_CHANCE, "CITY_HUT_CHANCE"},
	{TOKEN_SETTLER_HUT_CHANCE, "SETTLER_HUT_CHANCE"},
	
	{TOKEN_BARBARIAN_CHANCE, "BARBARIAN_CHANCE"},
	{TOKEN_BARBARIAN_RANK_MAX, "BARBARIAN_RANK_MAX"},
	{TOKEN_BARBARIAN_RANK_MIN, "BARBARIAN_RANK_MIN"},
	{TOKEN_MAX_SPONTANEOUS_BARBARIANS, "MAX_SPONTANEOUS_BARBARIANS"},
	{TOKEN_FIRST_BARBARIAN_TURN, "FIRST_BARBARIAN_TURN"},
	{TOKEN_LAST_BARBARIAN_TURN, "LAST_BARBARIAN_TURN"},
	{TOKEN_MINIMUM_BARBARIAN_DISTANCE, "MINIMUM_BARBARIAN_DISTANCE"},

	{TOKEN_ADVANCE_DEFAULT_BRANCH, "ADVANCE_DEFAULT_BRANCH"},
	{TOKEN_ENDGAME_DEFAULT_ICON, "ENDGAME_DEFAULT_ICON"},

	{TOKEN_NO_INDEX, "NO_INDEX"},
	{TOKEN_CHEAT_INDEX, "CHEAT_INDEX"},

	
	
	
	{TOKEN_FEATS_FACTOR, "FEATS_FACTOR"},
	
	
	{TOKEN_CITIES0TO30_FACTOR, "CITIES0TO30_FACTOR"},
	{TOKEN_CITIES30TO100_FACTOR,"CITIES30TO100_FACTOR"},
	{TOKEN_CITIES100TO500_FACTOR,"CITIES100TO500_FACTOR"},
	{TOKEN_CITIES500PLUS_FACTOR,"CITIES500PLUS_FACTOR"},
	{TOKEN_CITIES_RECAPTURED_FACTOR,"CITIES_RECAPTURED_FACTOR"},
	
	{TOKEN_RANK_FACTOR, "RANK_FACTOR"},
	{TOKEN_OPPONENTS_CONQUERED_FACTOR, "OPPONENTS_CONQUERED_FACTOR"},
	{TOKEN_ALLIES_FACTOR, "ALLIES_FACTOR"},
	
	{TOKEN_ALLIED_VICTORY_BONUS, "ALLIED_VICTORY_BONUS"},
	{TOKEN_SOLO_VICTORY_BONUS, "SOLO_VICTORY_BONUS"},

	{TOKEN_POLLUTION_START_PRODUCTION_LEVEL,	"POLLUTION_START_PRODUCTION_LEVEL"},
	{TOKEN_POLLUTION_START_POPULATION_LEVEL,	"POLLUTION_START_POPULATION_LEVEL"},
	{TOKEN_POLLUTION_PRODUCTION_RATIO,			"POLLUTION_PRODUCTION_RATIO"},
	{TOKEN_POLLUTION_POPULATION_RATIO,			"POLLUTION_POPULATION_RATIO"},

	{TOKEN_CONST_LAST,				"CONST_DO_NOT_USE_LAST"},
	{TOKEN_MAX,						"TOKEN_MAX_DO_NOT_USE"},
	
};

char *token_GetKeyword(const sint32 i)

{ 
	Assert(0 <= i); 
	return  g_allTokens[i].keyword; 
}

sint32 Token::ValidateAllTokens()

{  sint32 i; 

for (i=0; i<TOKEN_MAX; i++) { 
	if (g_allTokens[i].tok != i) { 
		return FALSE; 
	} 
} 
return TRUE;
}








Token::Token(
			 char *fn,
			 C3DIR	dir) 
			 
{
	Assert(ValidateAllTokens()); 
	
	strcpy(m_filename, fn);
	
	
	
	
	m_fin = c3files_fopen(dir, m_filename, "r");
	m_dir = dir;
	
	if (!m_fin) { 
		c3errors_ErrorDialog (ErrStr(), "Could not open %s", m_filename); 
		g_abort_parse = TRUE; 
		return;
	}
	
	m_index = 0; 
	m_len = 0; 
	g_parse_line = 0;
	g_saved_parse_line = 0;
	m_cur = ' '; 
	m_current_type = TOKEN_UNKNOWN; 
	m_num_it = 0; 
	m_imported_tokens = NULL;

	m_importFile = NULL;
	m_savedFin = NULL;

	Next(); 
	
}


Token::Token(
			 char *fn,
			 sint32 n, 
			 TokenData *it,
			 C3DIR	dir) 
			 
{
	Assert(ValidateAllTokens()); 
	
	strcpy(m_filename, fn);
	
	
	
	
	m_fin = c3files_fopen(dir, m_filename, "r");
	m_dir = dir;

	if (!m_fin) { 
		c3errors_ErrorDialog ("Token.cpp", "Could not open %s", m_filename); 
		g_abort_parse = TRUE; 
		return;
	}
	
	m_index = 0; 
	m_len = 0; 
	g_parse_line = 0;
	g_saved_parse_line = 0;
	m_cur = ' '; 
	m_current_type = TOKEN_UNKNOWN; 
	m_num_it = n; 
	m_imported_tokens = it;

	m_importFile = NULL;
	m_savedFin = NULL;

	Next(); 
	
}


Token::~Token()

{ 
	fclose (m_fin); 
}





















sint32 Token::IsNumber(char *str)
{
	
	
	
	if (*str == '-' || *str == '+')
		str++ ;
	
	
	if (!isdigit(*str++))
		return (FALSE) ;
	
	
	while (isdigit(*str))
		str++ ;
	
	
	if (*str == NULL)
		return (TRUE) ;
	
	
	if (*str++ != '.')
		return (FALSE) ;
	
	
	if (!isdigit(*str++))
		return (FALSE) ;
	
	
	while (isdigit(*str))
		str++ ;
	
	
	if (*str)
		return (FALSE) ;											
	
	return (TRUE) ;													
}


sint32 Token::GetType ()

{ 
	return m_current_type; 
} 


void Token::NextNumber()

{ 
	m_current_type = TOKEN_NUMBER; 
	m_val_number = atof(m_buf); 
}

sint32 Token::IsWhitespace(const char c) 

{   return (c == ' ') || (c == '\t') || (c == 13) || (c == 10); 
} 

void Token::NextString()

{ 
	sint32 i, searching = TRUE; 
	
	for (i=TOKEN_UNKNOWN+1; i<TOKEN_MAX && searching; i++) { 
		if (strcmp(m_buf, g_allTokens[i].keyword) == 0) { 
			m_current_type = i; 
			searching = FALSE;
		}
	}
	
	if ((searching) && (m_num_it)) { 
		for (i=0; i<m_num_it && searching ; i++) { 
			if (strcmp(m_buf, m_imported_tokens[i].keyword) == 0) { 
				m_current_type = m_imported_tokens[i].tok; 
				searching = FALSE;
			}
		}
	}
	
	if (searching) { 
		m_current_type = TOKEN_STRING; 
		strcpy (m_val_string, m_buf); 
		m_val_string_len = strlen(m_val_string);    
	}
} 

sint32 Token::HandleImport(void)
{
	MBCHAR		fileName[256];
	
	if (Next() != TOKEN_QUOTED_STRING) {
		Assert(FALSE);
		return 0;
	}
	
	GetString(fileName);
	
	g_saved_parse_line = g_parse_line;
	g_parse_line = 0;
	
	strcpy(m_savedFilename, m_filename);
	strcpy(m_filename, fileName);

	m_savedFin = m_fin;
	m_importFile = c3files_fopen(m_dir, fileName, "r");
	
	if (!m_importFile) { 
		c3errors_ErrorDialog ("Token.cpp", "Could not open import file '%s'", fileName); 
		g_abort_parse = TRUE; 
		return 0;
	}
	
	m_fin = m_importFile;

	return 0;
}

sint32 Token::CloseImport(void)
{
	Assert(m_importFile);

	fclose(m_importFile);

	m_fin = m_savedFin;
	m_importFile = NULL;
	strcpy(m_filename, m_savedFilename);
	g_parse_line = g_saved_parse_line;
	g_saved_parse_line = 0;

	return 0;
}

sint32 Token::Next() 

{
	
	while (1) { 
		
		
		for ( ; (m_cur != EOF) && IsWhitespace(m_cur) ; m_cur = getc(m_fin)) {  
			if ((m_cur == 13) || (m_cur == 10)) { 
				g_parse_line++;
			}
		} 
		
		
		if (m_cur == '#') { 
			for ( ; (m_cur != EOF) && (m_cur != 10); m_cur = getc(m_fin)) { 
				if ((m_cur == 13) || (m_cur == 10)) { 
					g_parse_line++;
				}
			} 
		} else { 
			break; 
		} 
	} 
	
	if (m_cur == EOF) {  
		
		if (m_importFile != NULL) {
			CloseImport();

			m_cur = getc(m_fin);
			
			return Next();
		} else {
			m_current_type = TOKEN_EOF; 
			m_cur = ' '; 
		}
	} else if (m_cur == '{') { 
		m_current_type = TOKEN_OPEN_BRACE; 
		m_cur = ' '; 
	} else if (m_cur == '}') { 
		m_current_type = TOKEN_CLOSE_BRACE; 
		m_cur = ' '; 
	} else if (m_cur == '\"') { // HACK - FIX ME - check if this can deal with international 
		// charater set. 
		m_current_type = TOKEN_QUOTED_STRING; 
		m_cur = getc(m_fin);
		m_val_string_len = 0; 
		while (m_cur != '\"' && m_val_string_len < k_MAX_TEXT_LEN) { 
			
			







			if (m_cur == EOF){ 
				m_current_type = TOKEN_MISSING_QUOTE; 
				m_val_string[m_val_string_len] = 0; 
				return m_current_type; 
			} else if ((m_cur == 10) || (m_cur == 13)){ 
				m_val_string[m_val_string_len] = ' '; 
				m_val_string_len++;        
				m_cur = getc(m_fin); 
				g_parse_line++;                
				
			} else if((m_cur == '\\')) {
				m_cur = getc(m_fin);
				switch(m_cur) {
				case 'n':
					m_val_string[m_val_string_len] = 10;
					break;
				case 'r':
					m_val_string[m_val_string_len] = 13;
					break;
				case 't':
					m_val_string[m_val_string_len] = 8;
					break;
				default:
					m_val_string[m_val_string_len] = m_cur;
					break;
				}
				m_val_string_len++;
				m_cur = getc(m_fin);
			} else {    
				if (k_MAX_TEXT_LEN <= m_val_string_len) { 
					m_current_type = TOKEN_MISSING_QUOTE; 
					m_val_string[m_val_string_len] = 0; 
					return m_current_type; 
				} 
				
				m_val_string[m_val_string_len] = m_cur; 
				m_val_string_len++;        
				m_cur =  getc(m_fin); 
			}
		}
		m_val_string[m_val_string_len] = 0; 
		m_cur = ' ';
	} else  { 
		m_buf[0] = m_cur;
		m_index = 0; 
		do { 
			m_index++;
			m_buf[m_index] = getc(m_fin); 
		} while ((m_buf[m_index] == '_') || (m_buf[m_index] == '.') || isalnum(m_buf[m_index])); 
		
		m_cur = m_buf[m_index]; 
		m_buf[m_index] = 0 ; 
		
		
		if (IsNumber(m_buf))										
			NextNumber() ;											
		else
			NextString() ;											
		
		
	}
	
	if (GetType() == TOKEN_IMPORT) {
		if (m_importFile != NULL) {
			c3errors_FatalDialog("Token", "Nested import is not supported.\n");
			return 0;
		} else {
			HandleImport();
			m_cur = getc(m_fin);
			return Next();
		}
	}
	
	return GetType(); 
}

void Token::GetString (char *str) 

{   sint32 i; 

if ((m_current_type != TOKEN_STRING) && (m_current_type != TOKEN_QUOTED_STRING)) { 
	c3errors_ErrorDialog (ErrStr(), "current type is not string");
	g_abort_parse = TRUE; 
	str[0] = 0; 
	return;
} 
for (i=0; i<m_val_string_len; i++) { 
	str[i] = m_val_string[i]; 
} 
str[i] = 0; 
} 

void Token::GetFloat(double &n)
{
	if (m_current_type != TOKEN_NUMBER)
	{
		c3errors_ErrorDialog (ErrStr(), "Token is not number"); 
		g_abort_parse = TRUE; 
	}
	
	n = m_val_number; 
}


void Token::GetNumber (sint32 &n)

{ 
	if (m_current_type != TOKEN_NUMBER) { 
		c3errors_ErrorDialog (ErrStr(), "Token is not number"); 
		g_abort_parse = TRUE; 
	} 
	n = (sint32)m_val_number; 
} 


char * Token::ErrStr()

{ 
	sprintf (m_estr, "%s line %d:", m_filename, g_parse_line); 
	return m_estr; 
} 

char * Token::GetKeyword(const sint32 tok) const

{  return g_allTokens[tok].keyword;
}



sint32 token_ParseKeywordNext(Token *aToken, const sint32 t) 

{
	if (aToken->Next() != t) { 
		return FALSE; 
	} else { 
		return TRUE; 
	}
}



sint32 token_ParseValNext(Token *aToken, const sint32 t, sint32 &val)

{
	if (!token_ParseKeywordNext(aToken, t)) { 
		
		if (TOKEN_MAX < t) { 
			c3errors_ErrorDialog(aToken->ErrStr(), "Expected keword %s not found", aToken->m_imported_tokens[t - (TOKEN_MAX + 1)].keyword);
		} else { 
			c3errors_ErrorDialog(aToken->ErrStr(), "Expected keword %s not found", g_allTokens[t].keyword);
		}
		g_abort_parse = TRUE; 
		return FALSE; 
	} else { 
		if (aToken->Next() != TOKEN_NUMBER) { 
			c3errors_ErrorDialog(aToken->ErrStr(), "Expected number not found");
			g_abort_parse = TRUE; 
			return FALSE;
		} else { 
			aToken->GetNumber(val); 
			return TRUE; 
		}
	}
}



sint32 token_ParseFloatNext(Token *aToken, const sint32 t, double &val)

{
	if (!token_ParseKeywordNext(aToken, t)) { 
		
		if (TOKEN_MAX < t) { 
			c3errors_ErrorDialog(aToken->ErrStr(), "Expected keword %s not found", aToken->m_imported_tokens[t - (TOKEN_MAX + 1)].keyword);
		} else { 
			c3errors_ErrorDialog(aToken->ErrStr(), "Expected keword %s not found", g_allTokens[t].keyword);
		}
		g_abort_parse = TRUE; 
		return FALSE; 
	} else { 
		if (aToken->Next() != TOKEN_NUMBER) { 
			c3errors_ErrorDialog(aToken->ErrStr(), "Expected number not found");
			g_abort_parse = TRUE; 
			return FALSE;
		} else { 
			aToken->GetFloat(val); 
			return TRUE; 
		}
	}
}



sint32 token_ParseAnOpenBraceNext(Token *aToken)

{
	if (aToken->Next() == TOKEN_OPEN_BRACE) { 
		return TRUE; 
	} else { 
		c3errors_ErrorDialog(aToken->ErrStr(), "Expected open brace missing");
		g_abort_parse = TRUE; 
		return FALSE; 
	} 
}



sint32 token_ParseAnCloseBraceNext(Token *aToken)

{
	if (aToken->Next() == TOKEN_CLOSE_BRACE) { 
		return TRUE; 
	} else { 
		c3errors_ErrorDialog(aToken->ErrStr(), "Expected close brace missing");
		g_abort_parse = TRUE; 
		return FALSE; 
	} 
}



sint32 token_ParseAnCloseBrace(Token *aToken)

{
	if (aToken->GetType() == TOKEN_CLOSE_BRACE) { 
		return TRUE; 
	} else { 
		c3errors_ErrorDialog(aToken->ErrStr(), "Expected close brace missing");
		g_abort_parse = TRUE; 
		return FALSE; 
	} 
}




POINT token_ParsePoint(Token *theToken)
{
	POINT		p = {-1, -1};
	sint32		tmp;
	
	if (theToken->Next() == TOKEN_NUMBER) theToken->GetNumber(tmp); 
	else return p;
	
	p.x = tmp;
	
	if (theToken->Next() == TOKEN_NUMBER) theToken->GetNumber(tmp); 
	else return p;
	
	p.y = tmp;
	
	return p;
}

#ifndef __TILETOOL__
#ifndef __MAKESPR__
sint32 token_ParseFieldAdvance(StringDB *g_theStringDB, 
							   CTPDatabase<AdvanceRecord> *theAdvanceDB, Token *aToken, const sint32 keyword, 
							   sint32 &a)
							   
{
	
	char str[k_MAX_NAME_LEN];
	StringId str_id; 
	
	if (aToken->Next() != keyword) { 
		c3errors_ErrorDialog  (aToken->ErrStr(), "Missing keyword %s", 
			aToken->GetKeyword(keyword)); 
		g_abort_parse = TRUE; 
		return FALSE; 
	} 
	
	aToken->Next();
	
	if (aToken->GetType() == TOKEN_NULL) { 
		a = -1; 
	} else {
		if (aToken->GetType() != TOKEN_STRING) { 
			c3errors_ErrorDialog  (aToken->ErrStr(), "Missing value for %s", aToken->GetKeyword(TOKEN_ENABLING_ADVANCE)); 
			g_abort_parse = TRUE; 
			return FALSE; 
		} else { 
			aToken->GetString(str); 
			if (!g_theStringDB->GetStringID(str, str_id)) { 
				c3errors_ErrorDialog (aToken->ErrStr(), "Could not find %s in string database", str); 
				g_abort_parse = TRUE; 
				return FALSE; 
			}
			if (!theAdvanceDB->GetNamedItem(str_id, a)) { 
				c3errors_ErrorDialog (aToken->ErrStr(), "%s not found in Advance database", str); 
				g_abort_parse = TRUE; 
				return FALSE; 
			}
		}
	}
	return TRUE;
}

sint32 token_ParseFieldImprovement(Token *aToken, const sint32 keyword, sint32 &imp)

{
	
	char str[k_MAX_NAME_LEN];
	StringId str_id; 
	
	if (aToken->Next() != keyword) { 
		c3errors_ErrorDialog  (aToken->ErrStr(), "Missing keyword %s", 
			aToken->GetKeyword(keyword)); 
		g_abort_parse = TRUE; 
		return FALSE; 
	} 
	
	aToken->Next();
	
	if (aToken->GetType() == TOKEN_NULL) { 
		imp = -1; 
	} else {
		if (aToken->GetType() != TOKEN_STRING) { 
			c3errors_ErrorDialog  (aToken->ErrStr(), "Missing value for %s", aToken->GetKeyword(TOKEN_ENABLING_ADVANCE)); 
			g_abort_parse = TRUE; 
			return FALSE; 
		} else { 
			aToken->GetString(str); 
			if (!g_theStringDB->GetStringID(str, str_id)) { 
				c3errors_ErrorDialog (aToken->ErrStr(), "Could not find %s in string database", str); 
				g_abort_parse = TRUE; 
				return FALSE; 
			}
		}
	}
	return TRUE;
}

#endif
#endif
