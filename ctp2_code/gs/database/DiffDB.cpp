

#include "c3.h"

#include "DiffDB.h"

#include "CivArchive.h"

#include "Token.h"
#include "StrDB.h"
#include "PointerList.h"
#include "AdvanceRecord.h"
#include "c3files.h"
#include "MapAnalysis.h"
#include "TurnYearStatus.h"

extern StringDB *g_theStringDB;

extern sint32 g_abort_parse;

void DiffAdvanceChance::Serialize(CivArchive &archive)
{
	if(archive.IsStoring()) {
		archive << m_adv;
		archive << m_humanChance;
		archive << m_aiChance;
	} else {
	}
}

DifficultyDBRecord::DifficultyDBRecord()

{

	memset((uint8*)&m_base_contentment, 0, (uint8*)&m_advanceChances - (uint8*)&m_base_contentment + sizeof(m_advanceChances));

   m_base_contentment= 0; 
   m_granary_coef = 0;
   m_big_city_scale = 0.0; 
   m_big_city_offset = 0.0;

   m_pollution_multiplier = 0.0 ;

   m_science_handicap = 0;
   m_riot_chance = 0.0;
   m_starvation_effect = 0.0;
   m_starting_gold = 0;
   m_vision_bonus = 0;
   m_base_score = 0;

   m_advanceChances = new PointerList<DiffAdvanceChance>;
}

DifficultyDBRecord::~DifficultyDBRecord()
{
	if(m_advanceChances) {
		m_advanceChances->DeleteAll();
		delete m_advanceChances;
	}
}

void DifficultyDBRecord::Serialize(CivArchive &archive)

{
	sint32 count;
	sint32 i;
  	if (archive.IsStoring()) {
	archive << m_base_contentment; 
    archive << m_granary_coef;

    archive.PutDoubleString( m_big_city_scale); 
    archive.PutDoubleString( m_big_city_offset);
	archive.PutDoubleString( m_pollution_multiplier );
	archive << m_science_handicap;
	archive.PutDoubleString( m_riot_chance);
	archive << m_starting_gold;
	archive << m_vision_bonus;
	archive << m_base_score;

	
	archive << m_time_scale.m_startYear;
	archive << m_time_scale.m_numPeriods;
	for(i = 0; i < m_time_scale.m_numPeriods; i++) {
		archive << m_time_scale.m_periods[i].m_startTurn;
		archive << m_time_scale.m_periods[i].m_yearsPerTurn;
	}
	
	archive.PutDoubleString( m_feats_factor);
	archive.PutDoubleString( m_advances_factor);
	archive.PutDoubleString( m_wonders_factor);
	archive.PutDoubleString( m_cities0to30_factor);
	archive.PutDoubleString( m_cities30to100_factor);
	archive.PutDoubleString( m_cities100to500_factor);
	archive.PutDoubleString( m_cities500plus_factor);
	archive.PutDoubleString( m_cities_recaptured_factor);
	archive.PutDoubleString( m_population_factor);
	archive.PutDoubleString( m_rank_factor);
	archive.PutDoubleString( m_opponents_conquered_factor);
	archive.PutDoubleString( m_allies_factor);
	archive.PutDoubleString( m_solo_victory_bonus);
	archive.PutDoubleString( m_allied_victory_bonus);
	archive.PutDoubleString( m_wonder_victory_bonus);













	archive << m_ai_start_units;
	archive << m_ai_start_gold;
	archive << m_ai_start_advances;
	archive << m_ai_start_public_works;
	archive << m_human_start_location;
	archive.PutDoubleString( m_ai_intelligence_factor);
	archive.PutDoubleString( m_ai_gang_up_factor);
	archive.PutDoubleString( m_distance_from_capitol_adjustment);
	archive.PutDoubleString( m_ai_distance_from_capitol_adjustment);
	archive.PutDoubleString( m_pollution_adjust);

	
	
	
	

	
	archive << m_ai_min_behind_percent;
	archive << m_ai_max_behind_percent;
	archive << m_ai_min_ahead_percent;
	archive << m_ai_max_ahead_percent;
	archive.StoreArray(m_ai_min_behind_technology_cost, k_MAX_AGES);
	archive.StoreArray(m_ai_max_behind_technology_cost, k_MAX_AGES);
	archive.StoreArray(m_ai_min_behind_production_cost, k_MAX_AGES);
	archive.StoreArray(m_ai_max_behind_production_cost, k_MAX_AGES);
	archive.StoreArray(m_ai_min_behind_gold_adjustment, k_MAX_AGES);
	archive.StoreArray(m_ai_max_behind_gold_adjustment, k_MAX_AGES);
	archive.StoreArray(m_ai_min_ahead_technology_cost, k_MAX_AGES);
	archive.StoreArray(m_ai_max_ahead_technology_cost, k_MAX_AGES);
	archive.StoreArray(m_ai_min_ahead_production_cost, k_MAX_AGES);
	archive.StoreArray(m_ai_max_ahead_production_cost, k_MAX_AGES);
	archive.StoreArray(m_ai_min_ahead_gold_adjustment, k_MAX_AGES);
	archive.StoreArray(m_ai_max_ahead_gold_adjustment, k_MAX_AGES);

	archive << m_max_human_advances;
	archive << m_max_ai_advances;

	archive.PutDoubleString( m_human_science_bonus);
	archive.PutDoubleString( m_human_food_bonus);
	archive.PutDoubleString( m_extra_settler_chance);

	archive << m_max_score;

	archive.PutDoubleString( m_starvation_effect);

		count = m_advanceChances->GetCount();
		archive << count;
		PointerList<DiffAdvanceChance>::Walker walk(m_advanceChances);
		while(walk.IsValid()) {
			walk.GetObj()->Serialize(archive);
			walk.Next();
		}
	} else {
	}
}

DifficultyDB::DifficultyDB()
{
}

 











DifficultyDB::DifficultyDB(CivArchive &archive)
	{
	Serialize(archive) ;
	}












void DifficultyDB::Serialize(CivArchive &archive)
	{
	sint32	i ;

    CHECKSERIALIZE

#define DIFFICULTYDB_MAGIC 0x89898989
	if (archive.IsStoring())
		{
		archive.PerformMagic(DIFFICULTYDB_MAGIC) ;
		for (i=0; i<LEVELS_OF_DIFFICULTY; i++)
            m_table[i].Serialize(archive);

		}
	else
		{
		archive.TestMagic(DIFFICULTYDB_MAGIC) ;
		for (i=0; i<LEVELS_OF_DIFFICULTY; i++)
             m_table[i].Serialize(archive);

		}

	}


sint32 DifficultyDB::GetYearFromTurn(sint32 diff, sint32 turn)
{
	DifficultyDBRecord		*rec;

	Assert(diff < LEVELS_OF_DIFFICULTY);
	if (diff >= LEVELS_OF_DIFFICULTY) return 0;

	rec = Get(diff);
	Assert(rec);

	sint32 i;

	sint32 period = -1;

	for (i=rec->m_time_scale.m_numPeriods-1; i>=0; i--) {
		if ( turn >= rec->m_time_scale.m_periods[i].m_startTurn) {
			period = i;
			break;
		}
	}

	Assert(period >= 0);
	if (period < 0) return 0;

	sint32			curYear = rec->m_time_scale.m_startYear;
	sint32			turnsLeft = turn;

	for (i=0; i<=period; i++) {
		if (i < period) {
			sint32 totalTurns = rec->m_time_scale.m_periods[i+1].m_startTurn -
								rec->m_time_scale.m_periods[i].m_startTurn;
			curYear += (totalTurns * rec->m_time_scale.m_periods[i].m_yearsPerTurn);
			
			turnsLeft -= totalTurns;
		} else {
			
			curYear += (turnsLeft * rec->m_time_scale.m_periods[i].m_yearsPerTurn);
		}
	}

	return curYear;
}


sint32 DifficultyDB::GetYearIncrementFromTurn(sint32 diff, sint32 turn)
{
	DifficultyDBRecord		*rec;

	Assert(diff < LEVELS_OF_DIFFICULTY);
	if (diff >= LEVELS_OF_DIFFICULTY) return 0;

	rec = Get(diff);
	Assert(rec);

	for (sint32 i=rec->m_time_scale.m_numPeriods-1; i>=0; i--) {
		if ( turn >= rec->m_time_scale.m_periods[i].m_startTurn) {
			return rec->m_time_scale.m_periods[i].m_yearsPerTurn;
		}
	}

	Assert(FALSE);
	return 0;
}

const char *DifficultyDB::GetYearStringFromTurn(sint32 diff, sint32 turn)
{







	
	static char buf[k_MAX_NAME_LEN];
	if(g_useCustomYear && g_pTurnLengthOverride) {
		if(turn >= g_turnLengthOverrideSize) {
			turn = g_turnLengthOverrideSize - 1;
		}
		strcpy(buf, g_pTurnLengthOverride[turn].text);
	} else {
		sint32 year = GetYearFromTurn(diff, turn);
		if(year == 0) {
			
			year = 1;
		}

		sprintf(buf, "%d %s", abs(year), ((year < 0) ?
												 g_theStringDB->GetNameStr("str_tbl_ldl_BC") :
												 g_theStringDB->GetNameStr("str_tbl_ldl_AD")));
	}

	return buf;
}
		

sint32 DifficultyDB::ParseAdvanceChances(Token *diffToken, DifficultyDBRecord *rec)
{
	if(!token_ParseKeywordNext(diffToken, TOKEN_ADVANCE_CHANCES)) return FALSE;
	if(!token_ParseAnOpenBraceNext(diffToken)) return FALSE;

	sint32 tok;
	char str[k_MAX_NAME_LEN];
	StringId str_id;
	AdvanceType adv;
	sint32 humanChance, aiChance;

	while((tok = diffToken->Next()) != TOKEN_CLOSE_BRACE) {
		if(tok != TOKEN_STRING) return FALSE;
		diffToken->GetString(str);
		if(!g_theStringDB->GetStringID(str, str_id)) {
			c3errors_ErrorDialog(diffToken->ErrStr(), "Couldn't find %s in string database", str);
			return FALSE;
		}

		if(!g_theAdvanceDB->GetNamedItem(str_id, adv)) {
			c3errors_ErrorDialog(diffToken->ErrStr(), "%s is not an advance", str);
			return FALSE;
		}
		
		if(diffToken->Next() != TOKEN_NUMBER) {
			c3errors_ErrorDialog(diffToken->ErrStr(), "Each advance requires a human chance and an AI chance");
			return FALSE;
		}
		diffToken->GetNumber(humanChance);

		if(diffToken->Next() != TOKEN_NUMBER) {
			c3errors_ErrorDialog(diffToken->ErrStr(), "Each advance requires a human chance and an AI chance");
			return FALSE;
		}
		diffToken->GetNumber(aiChance);
		rec->m_advanceChances->AddTail(new DiffAdvanceChance(adv, humanChance, aiChance));
	}
	return TRUE;
}

sint32 DifficultyDB::ParseATimeScale(Token *diffToken, DifficultyDBRecord *rec)
{
	sint32		tmp;

	if (!token_ParseKeywordNext(diffToken, TOKEN_TIME_SCALE)) return FALSE;
	if (!token_ParseAnOpenBraceNext(diffToken)) return FALSE;
    
	
	if (!token_ParseValNext(diffToken, TOKEN_START_YEAR, tmp)) return FALSE;
	rec->m_time_scale.m_startYear = tmp;

	
    if (!token_ParseValNext(diffToken, TOKEN_NUM_PERIODS, tmp)) return FALSE;        
	rec->m_time_scale.m_numPeriods = tmp;

	if (tmp > k_MAX_PERIODS) {
		c3errors_FatalDialog("DifficultyDB", "%d periods defined, %d periods allowed.", tmp, k_MAX_PERIODS);
		return FALSE;
	}
	
	for (sint32 i=0; i<rec->m_time_scale.m_numPeriods; i++) {
		if (!token_ParseKeywordNext(diffToken, TOKEN_PERIOD)) return FALSE;
		if (!token_ParseAnOpenBraceNext(diffToken)) return FALSE;

		
		if (!token_ParseValNext(diffToken, TOKEN_START_TURN, tmp)) return FALSE;
		rec->m_time_scale.m_periods[i].m_startTurn = tmp;

		
		if (!token_ParseValNext(diffToken, TOKEN_YEARS_PER_TURN, tmp)) return FALSE;        
		rec->m_time_scale.m_periods[i].m_yearsPerTurn = tmp;

		if (!token_ParseAnCloseBraceNext(diffToken)) return FALSE;
	}

	if(!token_ParseKeywordNext(diffToken, TOKEN_NEGATIVE_YEAR_FORMAT)) return FALSE;
	if(diffToken->Next() != TOKEN_STRING) return FALSE;
	char strFmt[k_MAX_NAME_LEN];
	diffToken->GetString(strFmt);
	if(!g_theStringDB->GetStringID(strFmt, rec->m_negativeYearFormat)) return FALSE;

	if(!token_ParseKeywordNext(diffToken, TOKEN_POSITIVE_YEAR_FORMAT)) return FALSE;
	if(diffToken->Next() != TOKEN_STRING) return FALSE;
	diffToken->GetString(strFmt);
	if(!g_theStringDB->GetStringID(strFmt, rec->m_positiveYearFormat)) return FALSE;

	if (!token_ParseAnCloseBraceNext(diffToken)) return FALSE;

	return TRUE;
}

sint32 DifficultyDB::ParseADiff(Token *diffToken, DifficultyDBRecord *rec)
{    
	sint32 i;

    if (diffToken->GetType() != TOKEN_OPEN_BRACE) { 
        c3errors_ErrorDialog (diffToken->ErrStr(), "Missing expected open brace");
        g_abort_parse = TRUE;
        return FALSE; 
    } 
    if (!token_ParseValNext(diffToken, TOKEN_BASE_CONTENTMENT, rec->m_base_contentment)) return FALSE;        
    if (!token_ParseValNext(diffToken, TOKEN_GRANARY_COEF, rec->m_granary_coef)) return FALSE;    
    if (!token_ParseFloatNext(diffToken, TOKEN_BIG_CITY_SCALE, rec->m_big_city_scale)) return FALSE;    
    if (!token_ParseFloatNext(diffToken, TOKEN_BIG_CITY_OFFSET, rec->m_big_city_offset)) return FALSE;      
	if (!token_ParseValNext(diffToken, TOKEN_SCIENCE_HANDICAP, rec->m_science_handicap)) return FALSE;

    if (!token_ParseFloatNext(diffToken, TOKEN_DIFFICULTY_POLLUTION_MULTIPLIER, rec->m_pollution_multiplier )) return FALSE;
	if(!token_ParseFloatNext(diffToken, TOKEN_DIFFICULTY_RIOT_CHANCE, rec->m_riot_chance)) return FALSE;
	if(!token_ParseFloatNext(diffToken, TOKEN_DIFFICULTY_STARVATION_EFFECT, rec->m_starvation_effect)) return FALSE;
	if(!token_ParseValNext(diffToken, TOKEN_DIFFICULTY_STARTING_GOLD, rec->m_starting_gold)) return FALSE;
	if(!token_ParseValNext(diffToken, TOKEN_DIFFICULTY_VISION_BONUS, rec->m_vision_bonus)) return FALSE;
	if(!token_ParseValNext(diffToken, TOKEN_DIFFICULTY_BASE_SCORE, rec->m_base_score)) return FALSE;

	if (!ParseATimeScale(diffToken, rec)) return FALSE;

	if(!token_ParseFloatNext(diffToken, TOKEN_FEATS_FACTOR, rec->m_feats_factor)) return FALSE;
	if(!token_ParseFloatNext(diffToken, TOKEN_ADVANCES_FACTOR, rec->m_advances_factor)) return FALSE;
	if(!token_ParseFloatNext(diffToken, TOKEN_WONDERS_FACTOR, rec->m_wonders_factor)) return FALSE;
	if(!token_ParseFloatNext(diffToken, TOKEN_CITIES0TO30_FACTOR, rec->m_cities0to30_factor)) return FALSE;
	if(!token_ParseFloatNext(diffToken, TOKEN_CITIES30TO100_FACTOR, rec->m_cities30to100_factor)) return FALSE;
	if(!token_ParseFloatNext(diffToken, TOKEN_CITIES100TO500_FACTOR, rec->m_cities100to500_factor)) return FALSE;
	if(!token_ParseFloatNext(diffToken, TOKEN_CITIES500PLUS_FACTOR, rec->m_cities500plus_factor)) return FALSE;
	if(!token_ParseFloatNext(diffToken, TOKEN_CITIES_RECAPTURED_FACTOR, rec->m_cities_recaptured_factor)) return FALSE;
	if(!token_ParseFloatNext(diffToken, TOKEN_POPULATION_FACTOR, rec->m_population_factor)) return FALSE;
	if(!token_ParseFloatNext(diffToken, TOKEN_RANK_FACTOR, rec->m_rank_factor)) return FALSE;
	if(!token_ParseFloatNext(diffToken, TOKEN_OPPONENTS_CONQUERED_FACTOR, rec->m_opponents_conquered_factor)) return FALSE;
	if(!token_ParseFloatNext(diffToken, TOKEN_ALLIES_FACTOR, rec->m_allies_factor)) return FALSE;
	if(!token_ParseFloatNext(diffToken, TOKEN_WONDER_VICTORY_BONUS, rec->m_wonder_victory_bonus)) return FALSE;
	if(!token_ParseFloatNext(diffToken, TOKEN_SOLO_VICTORY_BONUS, rec->m_solo_victory_bonus)) return FALSE;
	if(!token_ParseFloatNext(diffToken, TOKEN_ALLIED_VICTORY_BONUS, rec->m_allied_victory_bonus)) return FALSE;












	
	if(!token_ParseValNext(diffToken, TOKEN_AI_START_UNITS, rec->m_ai_start_units)) return FALSE;
	if(!token_ParseValNext(diffToken, TOKEN_AI_START_GOLD, rec->m_ai_start_gold)) return FALSE;
	if(!token_ParseValNext(diffToken, TOKEN_AI_START_ADVANCES, rec->m_ai_start_advances)) return FALSE;
	if(!token_ParseValNext(diffToken, TOKEN_AI_START_PUBLIC_WORKS, rec->m_ai_start_public_works)) return FALSE;
	if(!token_ParseValNext(diffToken, TOKEN_HUMAN_START_LOCATION, rec->m_human_start_location)) return FALSE;

	if(!token_ParseFloatNext(diffToken, TOKEN_AI_INTELLIGENCE_FACTOR, rec->m_ai_intelligence_factor)) return FALSE;
	if(!token_ParseFloatNext(diffToken, TOKEN_AI_GANG_UP_FACTOR, rec->m_ai_gang_up_factor)) return FALSE;
	if(!token_ParseFloatNext(diffToken, TOKEN_DISTANCE_FROM_CAPITOL_ADJUSTMENT, rec->m_distance_from_capitol_adjustment)) return FALSE;
	if(!token_ParseFloatNext(diffToken, TOKEN_AI_DISTANCE_FROM_CAPITOL_ADJUSTMENT, rec->m_ai_distance_from_capitol_adjustment)) return FALSE;
	if(!token_ParseFloatNext(diffToken, TOKEN_POLLUTION_ADJUST, rec->m_pollution_adjust)) return FALSE;

	
	if(!token_ParseFloatNext(diffToken, TOKEN_AI_MIN_BEHIND_PERCENT, rec->m_ai_min_behind_percent)) return FALSE;
	if(!token_ParseFloatNext(diffToken, TOKEN_AI_MAX_BEHIND_PERCENT, rec->m_ai_max_behind_percent)) return FALSE;

	
	if(!token_ParseFloatNext(diffToken, TOKEN_AI_MIN_AHEAD_PERCENT, rec->m_ai_min_ahead_percent)) return FALSE;
	if(!token_ParseFloatNext(diffToken, TOKEN_AI_MAX_AHEAD_PERCENT, rec->m_ai_max_ahead_percent)) return FALSE;

	

	if(diffToken->Next() != TOKEN_AI_MIN_BEHIND_TECHNOLOGY_COST) {
		c3errors_ErrorDialog(diffToken->ErrStr(), "Expected AI_MIN_BEHIND_TECHNOLOGY_COST");
		return FALSE;
	}

	for(i = 0; i < k_MAX_AGES; i++) {
		if(diffToken->Next() != TOKEN_NUMBER) {
			c3errors_ErrorDialog(diffToken->ErrStr(), "Expected technology coefficient (Need %d total)", k_MAX_AGES);
			return FALSE;
		}
		diffToken->GetFloat(rec->m_ai_min_behind_technology_cost[i]);
	}

	if(diffToken->Next() != TOKEN_AI_MAX_BEHIND_TECHNOLOGY_COST) {
		c3errors_ErrorDialog(diffToken->ErrStr(), "Expected AI_MAX_BEHIND_TECHNOLOGY_COST");
		return FALSE;
	}

	for(i = 0; i < k_MAX_AGES; i++) {
		if(diffToken->Next() != TOKEN_NUMBER) {
			c3errors_ErrorDialog(diffToken->ErrStr(), "Expected technology coefficient (Need %d total)", k_MAX_AGES);
			return FALSE;
		}
		diffToken->GetFloat(rec->m_ai_max_behind_technology_cost[i]);
	}

	if(diffToken->Next() != TOKEN_AI_MIN_BEHIND_PRODUCTION_COST_ADJUSTMENT) {
		c3errors_ErrorDialog(diffToken->ErrStr(), "Expected AI_MIN_BEHIND_PRODUCTION_COST_ADJUSTMENT");
		return FALSE;
	}

	for(i = 0; i < k_MAX_AGES; i++) {
		if(diffToken->Next() != TOKEN_NUMBER) {
			c3errors_ErrorDialog(diffToken->ErrStr(), "Expected production coefficient (Need %d total)", k_MAX_AGES);
			return FALSE;
		}
		diffToken->GetFloat(rec->m_ai_min_behind_production_cost[i]);
	}

	if(diffToken->Next() != TOKEN_AI_MAX_BEHIND_PRODUCTION_COST_ADJUSTMENT) {
		c3errors_ErrorDialog(diffToken->ErrStr(), "Expected AI_MAX_BEHIND_PRODUCTION_COST_ADJUSTMENT");
		return FALSE;
	}

	for(i = 0; i < k_MAX_AGES; i++) {
		if(diffToken->Next() != TOKEN_NUMBER) {
			c3errors_ErrorDialog(diffToken->ErrStr(), "Expected production coefficient (Need %d total)", k_MAX_AGES);
			return FALSE;
		}
		diffToken->GetFloat(rec->m_ai_max_behind_production_cost[i]);
	}

	if(diffToken->Next() != TOKEN_AI_MIN_BEHIND_GOLD_ADJUSTMENT) {
		c3errors_ErrorDialog(diffToken->ErrStr(), "Expected AI_MIN_BEHIND_GOLD_ADJUSTMENT");
		return FALSE;
	}

	for(i = 0; i < k_MAX_AGES; i++) {
		if(diffToken->Next() != TOKEN_NUMBER) {
			c3errors_ErrorDialog(diffToken->ErrStr(), "Expected gold adjustment coefficient (Need %d total)", k_MAX_AGES);
			return FALSE;
		}
		diffToken->GetFloat(rec->m_ai_min_behind_gold_adjustment[i]);
	}

	if(diffToken->Next() != TOKEN_AI_MAX_BEHIND_GOLD_ADJUSTMENT) {
		c3errors_ErrorDialog(diffToken->ErrStr(), "Expected AI_MAX_BEHIND_GOLD_ADJUSTMENT");
		return FALSE;
	}

	for(i = 0; i < k_MAX_AGES; i++) {
		if(diffToken->Next() != TOKEN_NUMBER) {
			c3errors_ErrorDialog(diffToken->ErrStr(), "Expected gold adjustment coefficient (Need %d total)", k_MAX_AGES);
			return FALSE;
		}
		diffToken->GetFloat(rec->m_ai_max_behind_gold_adjustment[i]);
	}

	

	if(diffToken->Next() != TOKEN_AI_MIN_AHEAD_TECHNOLOGY_COST) {
		c3errors_ErrorDialog(diffToken->ErrStr(), "Expected AI_MIN_AHEAD_TECHNOLOGY_COST");
		return FALSE;
	}

	for(i = 0; i < k_MAX_AGES; i++) {
		if(diffToken->Next() != TOKEN_NUMBER) {
			c3errors_ErrorDialog(diffToken->ErrStr(), "Expected technology coefficient (Need %d total)", k_MAX_AGES);
			return FALSE;
		}
		diffToken->GetFloat(rec->m_ai_min_ahead_technology_cost[i]);
	}

	if(diffToken->Next() != TOKEN_AI_MAX_AHEAD_TECHNOLOGY_COST) {
		c3errors_ErrorDialog(diffToken->ErrStr(), "Expected AI_MAX_AHEAD_TECHNOLOGY_COST");
		return FALSE;
	}

	for(i = 0; i < k_MAX_AGES; i++) {
		if(diffToken->Next() != TOKEN_NUMBER) {
			c3errors_ErrorDialog(diffToken->ErrStr(), "Expected technology coefficient (Need %d total)", k_MAX_AGES);
			return FALSE;
		}
		diffToken->GetFloat(rec->m_ai_max_ahead_technology_cost[i]);
	}

	if(diffToken->Next() != TOKEN_AI_MIN_AHEAD_PRODUCTION_COST_ADJUSTMENT) {
		c3errors_ErrorDialog(diffToken->ErrStr(), "Expected AI_MIN_AHEAD_PRODUCTION_COST_ADJUSTMENT");
		return FALSE;
	}

	for(i = 0; i < k_MAX_AGES; i++) {
		if(diffToken->Next() != TOKEN_NUMBER) {
			c3errors_ErrorDialog(diffToken->ErrStr(), "Expected production coefficient (Need %d total)", k_MAX_AGES);
			return FALSE;
		}
		diffToken->GetFloat(rec->m_ai_min_ahead_production_cost[i]);
	}

	if(diffToken->Next() != TOKEN_AI_MAX_AHEAD_PRODUCTION_COST_ADJUSTMENT) {
		c3errors_ErrorDialog(diffToken->ErrStr(), "Expected AI_MAX_AHEAD_PRODUCTION_COST_ADJUSTMENT");
		return FALSE;
	}

	for(i = 0; i < k_MAX_AGES; i++) {
		if(diffToken->Next() != TOKEN_NUMBER) {
			c3errors_ErrorDialog(diffToken->ErrStr(), "Expected production coefficient (Need %d total)", k_MAX_AGES);
			return FALSE;
		}
		diffToken->GetFloat(rec->m_ai_max_ahead_production_cost[i]);
	}

	if(diffToken->Next() != TOKEN_AI_MIN_AHEAD_GOLD_ADJUSTMENT) {
		c3errors_ErrorDialog(diffToken->ErrStr(), "Expected AI_MIN_AHEAD_GOLD_ADJUSTMENT");
		return FALSE;
	}

	for(i = 0; i < k_MAX_AGES; i++) {
		if(diffToken->Next() != TOKEN_NUMBER) {
			c3errors_ErrorDialog(diffToken->ErrStr(), "Expected gold adjustment coefficient (Need %d total)", k_MAX_AGES);
			return FALSE;
		}
		diffToken->GetFloat(rec->m_ai_min_ahead_gold_adjustment[i]);
	}

	if(diffToken->Next() != TOKEN_AI_MAX_AHEAD_GOLD_ADJUSTMENT) {
		c3errors_ErrorDialog(diffToken->ErrStr(), "Expected AI_MAX_AHEAD_GOLD_ADJUSTMENT");
		return FALSE;
	}

	for(i = 0; i < k_MAX_AGES; i++) {
		if(diffToken->Next() != TOKEN_NUMBER) {
			c3errors_ErrorDialog(diffToken->ErrStr(), "Expected gold adjustment coefficient (Need %d total)", k_MAX_AGES);
			return FALSE;
		}
		diffToken->GetFloat(rec->m_ai_max_ahead_gold_adjustment[i]);
	}

	if(!token_ParseValNext(diffToken, TOKEN_MAX_HUMAN_ADVANCES, rec->m_max_human_advances)) return FALSE;
	if(!token_ParseValNext(diffToken, TOKEN_MAX_AI_ADVANCES, rec->m_max_ai_advances)) return FALSE;

	if(!token_ParseFloatNext(diffToken, TOKEN_HUMAN_SCIENCE_BONUS, rec->m_human_science_bonus)) return FALSE;
	if(!token_ParseFloatNext(diffToken, TOKEN_HUMAN_FOOD_BONUS, rec->m_human_food_bonus)) return FALSE;
	if(!token_ParseFloatNext(diffToken, TOKEN_EXTRA_SETTLER_CHANCE, rec->m_extra_settler_chance)) return FALSE;
	

	if(!ParseAdvanceChances(diffToken, rec)) return FALSE;

	if(!token_ParseValNext(diffToken, TOKEN_POLLUTION_START_PRODUCTION_LEVEL, rec->m_pollution_start_production_level)) return FALSE;
	if(!token_ParseValNext(diffToken, TOKEN_POLLUTION_START_POPULATION_LEVEL, rec->m_pollution_start_population_level)) return FALSE;
	if(!token_ParseFloatNext(diffToken, TOKEN_POLLUTION_PRODUCTION_RATIO, rec->m_pollution_production_ratio)) return FALSE;
	if(!token_ParseFloatNext(diffToken, TOKEN_POLLUTION_POPULATION_RATIO, rec->m_pollution_population_ratio)) return FALSE;

	if (diffToken->Next() != TOKEN_CLOSE_BRACE) { 
        ("Missing expected close brace");
        return FALSE; 
    } 
    diffToken->Next();
    
    return TRUE;
}

sint32 DifficultyDB::Parse(char *filename)

{
    int i; 
	Token *diffToken = new Token(filename, C3DIR_GAMEDATA); 

    for (i=0; i<LEVELS_OF_DIFFICULTY; i++) { 
        if (!ParseADiff(diffToken, Get(i))) {
			delete diffToken;
			return FALSE; 
		}
    }

	
	delete diffToken;

    return TRUE;
}

double DifficultyDB::GetFeatsFactor(sint32 diff)
{
    Assert(diff >= 0);
    Assert(diff < LEVELS_OF_DIFFICULTY);
    return m_table[diff].m_feats_factor;
}

double DifficultyDB::GetAdvancesFactor(sint32 diff)
{
    Assert(diff >= 0);
    Assert(diff < LEVELS_OF_DIFFICULTY);
    return m_table[diff].m_advances_factor;
}

double DifficultyDB::GetWondersFactor(sint32 diff)
{
    Assert(diff >= 0);
    Assert(diff < LEVELS_OF_DIFFICULTY);
    return m_table[diff].m_wonders_factor;
}

double DifficultyDB::GetPopulationFactor(sint32 diff)
{
    Assert(diff >= 0);
    Assert(diff < LEVELS_OF_DIFFICULTY);
    return m_table[diff].m_population_factor;
}

double DifficultyDB::GetRankFactor(sint32 diff)
{
    Assert(diff >= 0);
    Assert(diff < LEVELS_OF_DIFFICULTY);
    return m_table[diff].m_rank_factor;
}

double DifficultyDB::GetCities0To30Factor(sint32 diff)
{
    Assert(diff >= 0);
    Assert(diff < LEVELS_OF_DIFFICULTY);
    return m_table[diff].m_cities0to30_factor;
}

double DifficultyDB::GetCities30To100Factor(sint32 diff)
{
    Assert(diff >= 0);
    Assert(diff < LEVELS_OF_DIFFICULTY);
    return m_table[diff].m_cities30to100_factor;
}

double DifficultyDB::GetCities100To500Factor(sint32 diff)
{
    Assert(diff >= 0);
    Assert(diff < LEVELS_OF_DIFFICULTY);
    return m_table[diff].m_cities100to500_factor;
}

double DifficultyDB::GetCities500PlusFactor(sint32 diff)
{
    Assert(diff >= 0);
    Assert(diff < LEVELS_OF_DIFFICULTY);
    return m_table[diff].m_cities500plus_factor;
}

double DifficultyDB::GetCitiesRecapturedFactor(sint32 diff)
{
    Assert(diff >= 0);
    Assert(diff < LEVELS_OF_DIFFICULTY);
    return m_table[diff].m_cities_recaptured_factor;
}

double DifficultyDB::GetAlliesFactor(sint32 diff)
{
    Assert(diff >= 0);
    Assert(diff < LEVELS_OF_DIFFICULTY);
    return m_table[diff].m_allies_factor;
}

double DifficultyDB::GetSoloVictoryBonus(sint32 diff)
{
    Assert(diff >= 0);
    Assert(diff < LEVELS_OF_DIFFICULTY);
    return m_table[diff].m_solo_victory_bonus;
}

double DifficultyDB::GetOpponentsConqueredFactor(sint32 diff)
{
    Assert(diff >= 0);
    Assert(diff < LEVELS_OF_DIFFICULTY);
    return m_table[diff].m_opponents_conquered_factor;
}

double DifficultyDB::GetWonderVictoryBonus(sint32 diff)
{
    Assert(diff >= 0);
    Assert(diff < LEVELS_OF_DIFFICULTY);
    return m_table[diff].m_wonder_victory_bonus;
}

double DifficultyDB::GetAlliedVictoryBonus(sint32 diff)
{
    Assert(diff >= 0);
    Assert(diff < LEVELS_OF_DIFFICULTY);
    return m_table[diff].m_allied_victory_bonus;
}

double DifficultyDBRecord::GetAiTechnologyCost(const sint32 playerId, const sint32 age) const
{
	float rank_percent = MapAnalysis::GetMapAnalysis().GetScienceHandicapRatio(playerId);
	float percent_spread;
	float value_spread;
	float rank_spread_percent;
	float value;
	const double & max_behind_value = m_ai_max_behind_technology_cost[age];
	const double & min_behind_value = m_ai_min_behind_technology_cost[age];
	const double & max_ahead_value = m_ai_max_ahead_technology_cost[age];
	const double & min_ahead_value = m_ai_min_ahead_technology_cost[age];

	if (rank_percent < m_ai_min_behind_percent)
	{
		if (rank_percent < m_ai_max_behind_percent)
			value = max_behind_value;
		else
		{
			
			percent_spread = m_ai_max_behind_percent - m_ai_min_behind_percent;
			value_spread = max_behind_value - min_behind_value;
			rank_spread_percent = (m_ai_min_behind_percent - rank_percent) / fabs(percent_spread);
			value = min_behind_value + (value_spread * rank_spread_percent);
		}
	}
	else if (rank_percent > m_ai_max_ahead_percent)
		value = max_ahead_value;
	else if (rank_percent < m_ai_min_ahead_percent)
		value = min_ahead_value;
	else
	{
		
		percent_spread = m_ai_max_ahead_percent - m_ai_min_ahead_percent;
		value_spread =  max_ahead_value - min_ahead_value;
		rank_spread_percent = (m_ai_min_ahead_percent - rank_percent) / fabs(percent_spread);
		value = max_ahead_value + (value_spread * rank_spread_percent);
	}

	return value;
}

double DifficultyDBRecord::GetAiProductionCostAdjustment(const sint32 playerId, const sint32 age) const
{
	float rank_percent = MapAnalysis::GetMapAnalysis().GetProductionHandicapRatio(playerId);
	float percent_spread;
	float value_spread;
	float rank_spread_percent;
	float value;
	const double & max_behind_value = m_ai_max_behind_production_cost[age];
	const double & min_behind_value = m_ai_min_behind_production_cost[age];
	const double & max_ahead_value = m_ai_max_ahead_production_cost[age];
	const double & min_ahead_value = m_ai_min_ahead_production_cost[age];

	if (rank_percent < m_ai_min_behind_percent)
	{
		if (rank_percent < m_ai_max_behind_percent)
			value = max_behind_value;
		else
		{
			
			percent_spread = m_ai_max_behind_percent - m_ai_min_behind_percent;
			value_spread = max_behind_value - min_behind_value;
			rank_spread_percent = (m_ai_min_behind_percent - rank_percent) / fabs(percent_spread);
			value = min_behind_value + (value_spread * rank_spread_percent);
		}
	}
	else if (rank_percent > m_ai_max_ahead_percent)
		value = max_ahead_value;
	else if (rank_percent < m_ai_min_ahead_percent)
		value = min_ahead_value;
	else
	{
		
		percent_spread = m_ai_max_ahead_percent - m_ai_min_ahead_percent;
		value_spread =  max_ahead_value - min_ahead_value;
		rank_spread_percent = (m_ai_min_ahead_percent - rank_percent) / fabs(percent_spread);
		value = max_ahead_value + (value_spread * rank_spread_percent);
	}

	return value;
}

double DifficultyDBRecord::GetAiGoldAdjustment(const sint32 playerId, const sint32 age) const
{
	float rank_percent = MapAnalysis::GetMapAnalysis().GetGoldHandicapRatio(playerId);
	float percent_spread;
	float value_spread;
	float rank_spread_percent;
	float value;
	const double & max_behind_value = m_ai_max_behind_gold_adjustment[age];
	const double & min_behind_value = m_ai_min_behind_gold_adjustment[age];
	const double & max_ahead_value = m_ai_max_ahead_gold_adjustment[age];
	const double & min_ahead_value = m_ai_min_ahead_gold_adjustment[age];

	if (rank_percent < m_ai_min_behind_percent)
	{
		if (rank_percent < m_ai_max_behind_percent)
			value = max_behind_value;
		else
		{
			
			percent_spread = m_ai_max_behind_percent - m_ai_min_behind_percent;
			value_spread = max_behind_value - min_behind_value;
			rank_spread_percent = (m_ai_min_behind_percent - rank_percent) / fabs(percent_spread);
			value = min_behind_value + (value_spread * rank_spread_percent);
		}
	}
	else if (rank_percent > m_ai_max_ahead_percent)
		value = max_ahead_value;
	else if (rank_percent < m_ai_min_ahead_percent)
		value = min_ahead_value;
	else
	{
		
		percent_spread = m_ai_max_ahead_percent - m_ai_min_ahead_percent;
		value_spread =  max_ahead_value - min_ahead_value;
		rank_spread_percent = (m_ai_min_ahead_percent - rank_percent) / fabs(percent_spread);
		value = max_ahead_value + (value_spread * rank_spread_percent);
	}

	return value;
}
