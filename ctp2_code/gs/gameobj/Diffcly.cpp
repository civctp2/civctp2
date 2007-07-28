//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : Difficulty settings class and diffutils.
// Id           : $Id$
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
// - None
//
//----------------------------------------------------------------------------
//
// - Moved some functionality from DiffDB to create the diffutils. (April 29th 2006 Martin Gühmann)
// - Replaced old difficulty database by new one. (April 29th 2006 Martin Gühmann)
//
//----------------------------------------------------------------------------

#include "c3.h"

#include "Diffcly.h"
#include "DifficultyRecord.h"
#include "civarchive.h"
#include "mapanalysis.h"
#include "TurnYearStatus.h"
#include "StrDB.h"

Difficulty::Difficulty(sint32 diff_level, PLAYER_INDEX owner, BOOL isHuman)
{
	m_pad                        = 0;
	m_owner                      = owner;

	m_max_martial_law_units      = 0;
	m_martial_law_effect         = 0;

	m_content_in_the_field       = 0;
	m_in_the_field_effect        = 0;

	m_base_contentment =           g_theDifficultyDB->Get(diff_level)->GetBaseContentment();

	m_big_city_scale =             g_theDifficultyDB->Get(diff_level)->GetBigCityScale();
	m_big_city_offset =            g_theDifficultyDB->Get(diff_level)->GetBigCityOffset();

	m_science_handicap =           g_theDifficultyDB->Get(diff_level)->GetScienceHandicap();
	m_pollution_multiplier =       g_theDifficultyDB->Get(diff_level)->GetPollutionMultiplier();
	m_riot_chance =                g_theDifficultyDB->Get(diff_level)->GetRiotChance();
	m_starvation_effect =          g_theDifficultyDB->Get(diff_level)->GetStarvationEffect();
	m_starting_gold =              g_theDifficultyDB->Get(diff_level)->GetStartingGold();

	m_feats_factor =               g_theDifficultyDB->Get(diff_level)->GetFeatsFactor();
	m_advances_factor =            g_theDifficultyDB->Get(diff_level)->GetAdvancesFactor();
	m_wonders_factor =             g_theDifficultyDB->Get(diff_level)->GetWondersFactor();
	m_cities0to30_factor =         g_theDifficultyDB->Get(diff_level)->GetCities0To3Factor();
	m_cities30to100_factor =       g_theDifficultyDB->Get(diff_level)->GetCities3To10Factor();
	m_cities100to500_factor      = g_theDifficultyDB->Get(diff_level)->GetCities10To50Factor();
	m_cities500plus_factor       = g_theDifficultyDB->Get(diff_level)->GetCities50PlusFactor();
	m_cities_recaptured_factor   = g_theDifficultyDB->Get(diff_level)->GetCitiesRecapturedFactor();
	m_population_factor =          g_theDifficultyDB->Get(diff_level)->GetPopulationFactor();
	m_rank_factor =                g_theDifficultyDB->Get(diff_level)->GetRankFactor();
	m_allies_factor =              g_theDifficultyDB->Get(diff_level)->GetAlliesFactor();
	m_opponents_conquered_factor = g_theDifficultyDB->Get(diff_level)->GetOpponentsConqueredFactor();
	m_wonder_victory_bonus =       g_theDifficultyDB->Get(diff_level)->GetWonderVictoryBonus();
	m_allied_victory_bonus =       g_theDifficultyDB->Get(diff_level)->GetAlliedVictoryBonus();
	m_solo_victory_bonus =         g_theDifficultyDB->Get(diff_level)->GetSoloVictoryBonus();

	m_vision_bonus =               g_theDifficultyDB->Get(diff_level)->GetVisionBonus();
	m_base_score =                 g_theDifficultyDB->Get(diff_level)->GetBaseScore();

	if(isHuman) {
		m_distance_from_capitol_adjustment = g_theDifficultyDB->Get(diff_level)->GetDistanceFromCapitolAdjustment();
	} else {
		m_distance_from_capitol_adjustment = g_theDifficultyDB->Get(diff_level)->GetAIDistanceFromCapitolAdjustment();
	}
}

void Difficulty::Serialize(CivArchive &archive)
{
	if (archive.IsStoring())
		archive.Store((uint8 *)this, sizeof(Difficulty)) ;
	else
		archive.Load((uint8 *)this, sizeof(Difficulty)) ;

}

double diffutil_GetAiAdjustment(const sint32 diff,
                                const sint32 playerId,
                                const double & max_behind_value,
                                const double & min_behind_value,
                                const double &  max_ahead_value,
                                const double &  min_ahead_value
                               )
{
	const DifficultyRecord *rec = g_theDifficultyDB->Get(diff);
	double const	rank_percent		= 
		MapAnalysis::GetMapAnalysis().GetScienceHandicapRatio(playerId);
	float percent_spread;
	float value_spread;
	float rank_spread_percent;
	float value;

	if (rank_percent < rec->GetAIMinBehind())
	{
		if (rank_percent < rec->GetAIMaxBehind())
			value = max_behind_value;
		else
		{
			
			percent_spread = rec->GetAIMaxBehind() - rec->GetAIMinBehind();
			value_spread = max_behind_value - min_behind_value;
			rank_spread_percent = (rec->GetAIMinBehind() - rank_percent) / fabs(percent_spread);
			value = min_behind_value + (value_spread * rank_spread_percent);
		}
	}
	else if (rank_percent > rec->GetAIMaxAhead())
		value = max_ahead_value;
	else if (rank_percent < rec->GetAIMinAhead())
		value = min_ahead_value;
	else
	{
		
		percent_spread = rec->GetAIMaxAhead() - rec->GetAIMinAhead();
		value_spread =  max_ahead_value - min_ahead_value;
		rank_spread_percent = (rec->GetAIMinAhead() - rank_percent) / fabs(percent_spread);
		value = max_ahead_value + (value_spread * rank_spread_percent);
	}

	return value;
}

double diffutil_GetAiTechnologyCost(const sint32 diff, const sint32 playerId, const sint32 age)
{
	const DifficultyRecord *rec = g_theDifficultyDB->Get(diff);

	return diffutil_GetAiAdjustment(diff,
	                                playerId,
	                                rec->GetAIMaxBehindTechnologyCost(age),
	                                rec->GetAIMinBehindTechnologyCost(age),
	                                rec->GetAIMaxAheadTechnologyCost(age),
	                                rec->GetAIMinAheadTechnologyCost(age));

}

double diffutil_GetAiProductionCostAdjustment(const sint32 diff, const sint32 playerId, const sint32 age)
{
	const DifficultyRecord *rec = g_theDifficultyDB->Get(diff);

	return diffutil_GetAiAdjustment(diff,
	                                playerId,
	                                rec->GetAIMaxBehindProductionCostAdjustment(age),
	                                rec->GetAIMinBehindProductionCostAdjustment(age),
	                                rec->GetAIMaxAheadProductionCostAdjustment(age),
	                                rec->GetAIMinAheadProductionCostAdjustment(age));

}

double diffutil_GetAiGoldAdjustment(const sint32 diff, const sint32 playerId, const sint32 age)
{
	const DifficultyRecord *rec = g_theDifficultyDB->Get(diff);

	return diffutil_GetAiAdjustment(diff,
	                                playerId,
	                                rec->GetAIMaxBehindGoldCostAdjustment(age),
	                                rec->GetAIMinBehindGoldCostAdjustment(age),
	                                rec->GetAIMaxAheadGoldCostAdjustment(age),
	                                rec->GetAIMinAheadGoldCostAdjustment(age));

}

sint32 diffutil_GetYearFromTurn(const sint32 diff, const sint32 turn)
{
	const DifficultyRecord *rec = g_theDifficultyDB->Get(diff);


	sint32 period = -1;

	const DifficultyRecord::TimeScale *tsrec = rec->GetTimeScale();
	sint32 i;
	for(i = tsrec->GetNumPeriod() - 1; i >= 0; i--){
		if(turn >= tsrec->GetPeriod(i)->GetStartTurn()){
			period = i;
			break;
		}
	}

	Assert(period >= 0);
	if (period < 0) return 0;

	sint32			curYear = tsrec->GetStartYear();
	sint32			turnsLeft = turn;

	for(i = 0; i <= period; i++){
		if(i < period){
			sint32 totalTurns = tsrec->GetPeriod(i+1)->GetStartTurn() -
								tsrec->GetPeriod(i)->GetStartTurn();
			curYear += (totalTurns * tsrec->GetPeriod(i)->GetYearsPerTurn());
			
			turnsLeft -= totalTurns;
		} else {
			
			curYear += (turnsLeft * tsrec->GetPeriod(i)->GetYearsPerTurn());
		}
	}

	return curYear;
}

sint32 diffutil_GetYearIncrementFromTurn(sint32 diff, sint32 turn)
{
	const DifficultyRecord *rec = g_theDifficultyDB->Get(diff);

	const DifficultyRecord::TimeScale *tsrec = rec->GetTimeScale();
	sint32 i;
	for(i = tsrec->GetNumPeriod() - 1; i >= 0; i--){
		if(turn >= tsrec->GetPeriod(i)->GetStartTurn()){
			return tsrec->GetPeriod(i)->GetYearsPerTurn();
		}
	}

	Assert(FALSE);
	return 0;
}

const char *diffutil_GetYearStringFromTurn(sint32 diff, sint32 turn)
{
	return TurnYearStatus::GetCurrentYear();
}


uint32 Diffcly_Difficulty_GetVersion(void)
{
	return (k_DIFFICULTY_VERSION_MAJOR<<16 | k_DIFFICULTY_VERSION_MINOR) ;
}
