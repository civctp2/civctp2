#ifdef HAVE_PRAGMA_ONCE
#pragma once
#endif
#ifndef __DIFF_DB_H__
#define __DIFF_DB_H__ 1

#define LEVELS_OF_DIFFICULTY 6

class CivArchive ;
class Token;
typedef sint32 AdvanceType;

#define k_MAX_PERIODS			10

#define k_MAX_AGES              5

struct PeriodData {
	sint32 m_startTurn;
	sint32 m_yearsPerTurn;
};

struct TimeScaleData {
	sint32		m_startYear;
	sint32		m_numPeriods;
	PeriodData	m_periods[k_MAX_PERIODS];
};

class DiffAdvanceChance {
public:
	AdvanceType m_adv;
	sint32 m_humanChance;
	sint32 m_aiChance;

	DiffAdvanceChance(AdvanceType adv, sint32 human, sint32 ai)
	{
		m_adv = adv;
		m_humanChance = human;
		m_aiChance = ai;
	}
	DiffAdvanceChance(CivArchive &archive) { Serialize(archive); }
	void Serialize(CivArchive &archive);
};

template <class T> class PointerList;

class DifficultyDBRecord {
public:
	sint32 m_base_contentment;
    sint32 m_granary_coef;

    double m_big_city_scale;
    double m_big_city_offset;
	double m_pollution_multiplier ;
	sint32 m_science_handicap;
	double m_riot_chance;
	sint32 m_starting_gold;
	sint32 m_vision_bonus;
	sint32 m_base_score;

	TimeScaleData	m_time_scale;

	double m_feats_factor;
	double m_advances_factor;
	double m_wonders_factor;
	double m_cities0to30_factor;
	double m_cities30to100_factor;
	double m_cities100to500_factor;
	double m_cities500plus_factor;
	double m_cities_recaptured_factor;
	double m_population_factor;
	double m_rank_factor;
	double m_opponents_conquered_factor;
	double m_allies_factor;
	double m_wonder_victory_bonus;
	double m_allied_victory_bonus;
	double m_solo_victory_bonus;










	sint32 m_ai_start_units;
	sint32 m_ai_start_gold;
	sint32 m_ai_start_advances;
	sint32 m_ai_start_public_works;
	sint32 m_human_start_location;
	double m_ai_intelligence_factor;
	double m_ai_gang_up_factor;
	double m_distance_from_capitol_adjustment;
	double m_ai_distance_from_capitol_adjustment;
	double m_pollution_adjust;







	double GetAiTechnologyCost(const sint32 playerId, const sint32 age) const;
	double GetAiProductionCostAdjustment(const sint32 playerId, const sint32 age) const;
	double GetAiGoldAdjustment(const sint32 playerId, const sint32 age)const;

	sint32 m_max_human_advances;
	sint32 m_max_ai_advances;

	double m_human_science_bonus;
	double m_human_food_bonus;
	double m_extra_settler_chance;

	sint32 m_pollution_start_production_level;
	sint32 m_pollution_start_population_level;
	double m_pollution_production_ratio;
	double m_pollution_population_ratio;

	sint32 m_max_score;

	double m_starvation_effect;

	sint32 m_negativeYearFormat, m_positiveYearFormat;

	PointerList<DiffAdvanceChance> *m_advanceChances;

    DifficultyDBRecord();
	~DifficultyDBRecord();
    void Serialize(CivArchive &archive);

	double m_ai_min_behind_percent;

	double m_ai_max_behind_percent;

	double m_ai_min_ahead_percent;

	double m_ai_max_ahead_percent;




	double m_ai_min_behind_technology_cost[k_MAX_AGES];
	double m_ai_max_behind_technology_cost[k_MAX_AGES];
	double m_ai_min_behind_production_cost[k_MAX_AGES];
	double m_ai_max_behind_production_cost[k_MAX_AGES];
	double m_ai_min_behind_gold_adjustment[k_MAX_AGES];
	double m_ai_max_behind_gold_adjustment[k_MAX_AGES];

	double m_ai_min_ahead_technology_cost[k_MAX_AGES];
	double m_ai_max_ahead_technology_cost[k_MAX_AGES];
	double m_ai_min_ahead_production_cost[k_MAX_AGES];
	double m_ai_max_ahead_production_cost[k_MAX_AGES];
	double m_ai_min_ahead_gold_adjustment[k_MAX_AGES];
	double m_ai_max_ahead_gold_adjustment[k_MAX_AGES];
};

class DifficultyDB {

	DifficultyDBRecord m_table[LEVELS_OF_DIFFICULTY];

public:

	DifficultyDB();
	DifficultyDB(CivArchive &archive) ;

	DifficultyDBRecord * Get(sint32 diff) {
		Assert(0<=diff);
		Assert(diff<LEVELS_OF_DIFFICULTY);
		return &m_table[diff];
	}
	sint32 GetBaseContentment(sint32 diff) {
		Assert(0<=diff);
		Assert(diff<LEVELS_OF_DIFFICULTY);
		return m_table[diff].m_base_contentment;
	}

	sint32 GetYearFromTurn(sint32 diff, sint32 turn);
	const char *GetYearStringFromTurn(sint32 diff, sint32 turn);
	sint32 GetYearIncrementFromTurn(sint32 diff, sint32 turn);

	double GetBigCityScale (sint32 diff) {
		Assert(0<=diff);
		Assert(diff<LEVELS_OF_DIFFICULTY);
		return m_table[diff].m_big_city_scale;
	}

	double GetBigCityOffset(sint32 diff) {
		Assert(0<=diff);
		Assert(diff<LEVELS_OF_DIFFICULTY);
		return m_table[diff].m_big_city_offset;
	}

	double GetPollutionMultiplier(sint32 diff)
	{
		Assert(0<=diff) ;
		Assert(diff<LEVELS_OF_DIFFICULTY) ;
		return (m_table[diff].m_pollution_multiplier) ;
	}

	sint32 GetScienceHandicap(sint32 diff) {
		Assert(diff >= 0);
		Assert(diff < LEVELS_OF_DIFFICULTY);
		return m_table[diff].m_science_handicap;
	}

	double GetRiotChance(sint32 diff) {
		Assert(diff >= 0);
		Assert(diff < LEVELS_OF_DIFFICULTY);
		return m_table[diff].m_riot_chance;
	}

	double GetStarvationEffect(sint32 diff) {
		Assert(diff >= 0);
		Assert(diff < LEVELS_OF_DIFFICULTY);
		return m_table[diff].m_starvation_effect;
	}

	sint32 GetStartingGold(sint32 diff) {
		Assert(diff >= 0);
		Assert(diff < LEVELS_OF_DIFFICULTY);
		return m_table[diff].m_starting_gold;
	}

	sint32 GetVisionBonus(sint32 diff) {
		Assert(diff >= 0);
		Assert(diff < LEVELS_OF_DIFFICULTY);
		return m_table[diff].m_vision_bonus;
	}

	sint32 GetBaseScore(sint32 diff) {
		Assert(diff >= 0);
		Assert(diff < LEVELS_OF_DIFFICULTY);
		return m_table[diff].m_base_score;
	}

	sint32 GetPollutionStartProductionLevel(sint32 diff)
	{
		Assert(diff >= 0);
		Assert(diff < LEVELS_OF_DIFFICULTY);
		return m_table[diff].m_pollution_start_production_level;
	}

	sint32 GetPollutionStartPopulationLevel(sint32 diff)
	{
		Assert(diff >= 0);
		Assert(diff < LEVELS_OF_DIFFICULTY);
		return m_table[diff].m_pollution_start_population_level;
	}

	double GetPollutionProductionRatio(sint32 diff)
	{
		Assert(diff >= 0);
		Assert(diff < LEVELS_OF_DIFFICULTY);
		return m_table[diff].m_pollution_production_ratio;
	}

	double GetPollutionPopulationRatio(sint32 diff)
	{
		Assert(diff >= 0);
		Assert(diff < LEVELS_OF_DIFFICULTY);
		return m_table[diff].m_pollution_population_ratio;
	}

	double GetFeatsFactor(sint32 diff);
	double GetAdvancesFactor(sint32 diff);
	double GetWondersFactor(sint32 diff);
	double GetPopulationFactor(sint32 diff);
	double GetRankFactor(sint32 diff);
	double GetWonderVictoryBonus(sint32 diff);
	double GetSoloVictoryBonus(sint32 diff);
	double GetAlliedVictoryBonus(sint32 diff);
	double GetAlliesFactor(sint32 diff);
	double GetOpponentsConqueredFactor(sint32 diff);
	double GetCities0To30Factor(sint32 diff);
	double GetCities30To100Factor(sint32 diff);
	double GetCities100To500Factor(sint32 diff);
	double GetCities500PlusFactor(sint32 diff);
	double GetCitiesRecapturedFactor(sint32 diff);












	void Serialize(CivArchive &archive) ;

	sint32 ParseAdvanceChances(Token *diffToken, DifficultyDBRecord *rec);
	sint32 ParseATimeScale(Token *diffToken, DifficultyDBRecord *rec);
	sint32 ParseADiff(Token *diffToken, DifficultyDBRecord *rec);
	sint32 Parse(char *filename);

};

#else

class DifficultyDB;

#endif
