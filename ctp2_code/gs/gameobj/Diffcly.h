










#pragma once
#ifndef __DIFFICULTY_H__
#define __DIFFICULTY_H__ 1

class CivArchive ;

typedef sint32 PLAYER_INDEX;

#define k_DIFFICULTY_VERSION_MAJOR	0									
#define k_DIFFICULTY_VERSION_MINOR	0									


class Difficulty { 

    
	
    double m_big_city_scale; 
    double m_big_city_offset;
	double m_pollution_multiplier ;
	double m_riot_chance;

	double m_feats_factor;
	double m_advances_factor;
	double m_wonders_factor;
	double m_population_factor;
	double m_rank_factor;
	double m_allies_factor;
	double m_opponents_conquered_factor;
	double m_cities0to30_factor;
	double m_cities30to100_factor;
	double m_cities100to500_factor;
	double m_cities500plus_factor;
	double m_cities_recaptured_factor;
	double m_allied_victory_bonus;
	double m_solo_victory_bonus;
	double m_wonder_victory_bonus;












	double m_distance_from_capitol_adjustment;

	double m_starvation_effect;


	PLAYER_INDEX m_owner;

	sint32 m_base_contentment; 

    
    sint32 m_max_martial_law_units;
    sint32 m_martial_law_effect; 

    sint32 m_content_in_the_field; 
    sint32 m_in_the_field_effect; 

	sint32 m_science_handicap;
	sint32 m_starting_gold;

	
	sint32 m_base_score;

	
	sint32 m_vision_bonus;
    sint32 m_pad; 
	
	

	
	
	

	
	

	friend class NetDifficulty;
	friend class Score;

public:

	Difficulty(sint32 diff_level, PLAYER_INDEX owner, BOOL isHuman);
    sint32 GetBaseContentment() const { return m_base_contentment; } 


   double GetBigCityScale() const { return m_big_city_scale; }
   double GetBigCityOffset() const { return m_big_city_offset; }

   double GetPollutionMultiplier() const { return (m_pollution_multiplier) ; }

   sint32 GetMaxMartialLawUnits() const { return m_max_martial_law_units; }
   sint32 GetMartialLawEffect() const { return m_martial_law_effect; }

   sint32 GetContentInTheField() const { return m_content_in_the_field; }
   sint32 GetInTheFieldEffect() const { return m_in_the_field_effect; }

	sint32 GetScienceHandicap() const { return m_science_handicap; }
	double GetRiotChance() const { return m_riot_chance; }
	double GetStarvationEffect() const { return m_starvation_effect; }

	sint32 GetStartingGold() const { return m_starting_gold; }
	sint32 GetVisionBonus() const {return m_vision_bonus; }
	sint32 GetBaseScore() const {return m_base_score; }
	double GetDistanceFromCapitolAdjustment() const { return m_distance_from_capitol_adjustment; }

	void Serialize(CivArchive &archive) ;

}; 

uint32 Diffcly_Difficulty_GetVersion(void) ;
#else 

class Difficulty; 

#endif
