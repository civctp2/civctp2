#include "c3.h"

#include "Diffcly.h"
#include "DiffDB.h"
#include "civarchive.h"

extern DifficultyDB *g_theDifficultyDB;

Difficulty::Difficulty(sint32 diff_level, PLAYER_INDEX owner, BOOL isHuman)

{
    m_pad = 0;
	m_owner = owner;

    m_max_martial_law_units=0;
    m_martial_law_effect=0;

    m_content_in_the_field=0;
    m_in_the_field_effect=0;

    m_base_contentment = g_theDifficultyDB->GetBaseContentment(diff_level);

    m_big_city_scale = g_theDifficultyDB->GetBigCityScale (diff_level);
    m_big_city_offset = g_theDifficultyDB->GetBigCityOffset(diff_level);

	m_science_handicap = g_theDifficultyDB->GetScienceHandicap(diff_level);
	m_pollution_multiplier = g_theDifficultyDB->GetPollutionMultiplier(diff_level) ;
	m_riot_chance = g_theDifficultyDB->GetRiotChance(diff_level);
	m_starvation_effect = g_theDifficultyDB->GetStarvationEffect(diff_level);
	m_starting_gold = g_theDifficultyDB->GetStartingGold(diff_level);

	m_feats_factor = g_theDifficultyDB->GetFeatsFactor(diff_level);
	m_advances_factor = g_theDifficultyDB->GetAdvancesFactor(diff_level);
	m_wonders_factor = g_theDifficultyDB->GetWondersFactor(diff_level);
	m_cities0to30_factor = g_theDifficultyDB->GetCities0To30Factor(diff_level);
	m_cities30to100_factor = g_theDifficultyDB->GetCities30To100Factor(diff_level);
	m_cities100to500_factor = g_theDifficultyDB->GetCities100To500Factor(diff_level);
	m_cities500plus_factor = g_theDifficultyDB->GetCities500PlusFactor(diff_level);
	m_cities_recaptured_factor = g_theDifficultyDB->GetCitiesRecapturedFactor(diff_level);
	m_population_factor = g_theDifficultyDB->GetPopulationFactor(diff_level);
	m_rank_factor = g_theDifficultyDB->GetRankFactor(diff_level);
	m_allies_factor = g_theDifficultyDB->GetAlliesFactor(diff_level);
	m_opponents_conquered_factor = g_theDifficultyDB->GetOpponentsConqueredFactor(diff_level);
	m_wonder_victory_bonus = g_theDifficultyDB->GetWonderVictoryBonus(diff_level);
	m_allied_victory_bonus = g_theDifficultyDB->GetAlliedVictoryBonus(diff_level);
	m_solo_victory_bonus = g_theDifficultyDB->GetSoloVictoryBonus(diff_level);















	m_vision_bonus = g_theDifficultyDB->GetVisionBonus(diff_level);
	m_base_score = g_theDifficultyDB->GetBaseScore(diff_level);

	if(isHuman) {
		m_distance_from_capitol_adjustment = g_theDifficultyDB->Get(diff_level)->m_distance_from_capitol_adjustment;
	} else {
		m_distance_from_capitol_adjustment = g_theDifficultyDB->Get(diff_level)->m_ai_distance_from_capitol_adjustment;
	}
}












void Difficulty::Serialize(CivArchive &archive)
	{
	if (archive.IsStoring())
		archive.Store((uint8 *)this, sizeof(Difficulty)) ;
	else
		archive.Load((uint8 *)this, sizeof(Difficulty)) ;

	}














uint32 Diffcly_Difficulty_GetVersion(void)
	{
	return (k_DIFFICULTY_VERSION_MAJOR<<16 | k_DIFFICULTY_VERSION_MINOR) ;
	}
