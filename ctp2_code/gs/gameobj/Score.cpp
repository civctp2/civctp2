

#include "c3.h"
#include "Score.h"
#include "Player.h"
#include "CivArchive.h"
#include "Diffcly.h"
#include "Advances.h"
#include "UnitDynArr.h"
#include "TurnCnt.h"
#include "XY_Coordinates.h"
#include "World.h"
#include "ProfileDB.h"
#include "StrDB.h"
#include "PointerList.h"
#include "GameSettings.h"
#include "Strengths.h"
#include "GameFile.h"
#include "FeatTracker.h"

extern Player **g_player;
extern PointerList<Player> *g_deadPlayer;
extern TurnCount *g_turn;
extern World *g_theWorld;
extern ProfileDB *g_theProfileDB;
extern StringDB *g_theStringDB;

Score::Score(PLAYER_INDEX owner)
{
	m_owner = owner;
	m_cities_recaptured = 0;
	m_opponents_conquered = 0;
	m_victory_type = kScoreGameInProgress;
	m_feats = 0;






}

Score::Score(CivArchive &archive)
{
	Serialize(archive);
}

void Score::Serialize(CivArchive &archive)
{
	if(archive.IsStoring()) {
		archive.StoreChunk((uint8*)&m_owner, (uint8*)&m_feats + sizeof(m_feats));
	} else {
		if(g_saveFileVersion >= 57)
		{
			archive.LoadChunk((uint8*)&m_owner, (uint8*)&m_feats + sizeof(m_feats));
		}
		else
		{
			archive.LoadChunk((uint8*)&m_owner, (uint8*)&m_victory_type + sizeof(m_victory_type));
		}
	}
}

void Score::AddCelebration()
{

}

void Score::AddYearAtPeace()
{

}

void Score::AddUnitLost()
{

}

void Score::AddRiot()
{

}

void Score::AddRevolution()
{

}

void Score::AddOpponentConquered()
{
	m_opponents_conquered++;
}

void Score::AddCityRecaptured()
{
	m_cities_recaptured++;
}

void Score::SetVictoryType(eScoreVictory type)
{
	m_victory_type = type;
}

void Score::AddFeat()
{
	m_feats++;
}

sint32 Score::GetTotalScore()
{
	if(!g_player[m_owner]) {
		return m_finalScore;
	}

	
	

	sint32 i;
	sint32 total = 0;
	for(i = 0; i < (sint32)SCORE_CAT_MAX; i++) {
		total += GetPartialScore((SCORE_CATEGORY)i);
	}
	return total;
}

sint32 Score::GetPlayerStrength(Player *pl)
{
	sint32 total = 0;
	total += pl->m_strengths->GetStrength(STRENGTH_CAT_UNITS);
	total += pl->m_strengths->GetStrength(STRENGTH_CAT_GOLD);
	total += pl->m_strengths->GetStrength(STRENGTH_CAT_BUILDINGS);
	total += pl->m_strengths->GetStrength(STRENGTH_CAT_WONDERS);
	total += pl->m_strengths->GetStrength(STRENGTH_CAT_PRODUCTION);
	return total;
}

sint32 Score::GetPartialScoreCount(SCORE_CATEGORY cat)
{
	Player *pl = g_player[m_owner];
	if(!pl) 
		pl = Player::GetDeadPlayer(m_owner);
	if(!pl)
		return 0;

	sint32 i, count = 0;
	switch (cat)
	{
		case SCORE_CAT_TYPE_OF_VICTORY:
		{
			count = 1;
			break;
		}
		case SCORE_CAT_RANK:
		{
			sint32 total = 0;
			for (i=1; i<k_MAX_PLAYERS; i++)
			{
				if (g_player[i])
					total += GetPlayerStrength(g_player[i]);
			}
			PointerList<Player>::Walker walk(g_deadPlayer);
			while (walk.IsValid()) 
			{
				total += GetPlayerStrength(walk.GetObj());
				walk.Next();
			}

			sint32 mystrength = GetPlayerStrength(pl);

			count = (total > 0) ? 100 * mystrength / total : 0;
			break;
		}
		case SCORE_CAT_OPPONENTS_CONQUERED:
		{
			count = m_opponents_conquered;
			break;
		}
		case SCORE_CAT_CITIES_RECAPTURED:
		{
			count = m_cities_recaptured;
			break;
		}
		case SCORE_CAT_FEATS:
		{
			
			
			count = m_feats;
			break;
		}

		case SCORE_CAT_ADVANCES:
		{
			count = pl->m_advances->GetDiscovered();
			break;
		}
		case SCORE_CAT_WONDERS:
		{
			uint64 built = pl->m_builtWonders;
			for(i = 0; i < 64; i++) 
			{
				if(((uint64)1 << (uint64)i) & built)
					count++;
			}
			break;
		}
		case SCORE_CAT_POPULATION:
		{
			sint32 totalPop = 0;
			
			for ( i = 0 ; i < k_MAX_PLAYERS ; i++ )
			{
				if (g_player[i] && (i != PLAYER_INDEX_VANDALS))
				{
					totalPop += g_player[i]->GetTotalPopulation();
				}
			}
			count = totalPop > 0 ? (pl->GetTotalPopulation() * 100) / totalPop : 0;
			break;
		}
		case SCORE_CAT_CITIES0TO30:
		{
			for(i = pl->m_all_cities->Num() - 1; i >= 0; i--) 
			{
				sint32 pop = pl->m_all_cities->Access(i).PopCount();
				if (pop < 30)
					count++;
			}
			break;
		}

		case SCORE_CAT_CITIES30TO100:
		{
			for(i = pl->m_all_cities->Num() - 1; i >= 0; i--) 
			{
				sint32 pop = pl->m_all_cities->Access(i).PopCount();
				if ((pop >= 30) && (pop < 100))
					count++;
			}
			break;
		}

		case SCORE_CAT_CITIES100TO500:
		{
			for(i = pl->m_all_cities->Num() - 1; i >= 0; i--) 
			{
				sint32 pop = pl->m_all_cities->Access(i).PopCount();
				if ((pop >= 100) && (pop < 500))
					count++;
			}
			break;
		}

		case SCORE_CAT_CITIES500PLUS:
		{
			for(i = pl->m_all_cities->Num() - 1; i >= 0; i--) 
			{
				sint32 pop = pl->m_all_cities->Access(i).PopCount();
				if (pop >= 500)
					count++;
			}
			break;
		}
	}
	if(count<0)	
	{
		count=0;
	}
	return count;
}

sint32 Score::GetPartialScore(SCORE_CATEGORY cat)
{
	sint32 count = GetPartialScoreCount(cat);
	double value = count * GetPartialScoreValue(cat);
	if(value<0)	
	{
		value=0;
	}

	switch (cat)
	{
		
		case SCORE_CAT_POPULATION:
		case SCORE_CAT_RANK:
		{
			value /= 100;
			break;
		}
	}
	return (sint32 )value;
}

double Score::GetPartialScoreValue(SCORE_CATEGORY cat)
{
	
	Player *pl = g_player[m_owner];
	if(!pl) 
		pl = Player::GetDeadPlayer(m_owner);
	if(!pl)
		return 0;

	
	

	Difficulty *diff = pl->m_difficulty;
	double ret = 0;

	switch(cat) 
	{
		case SCORE_CAT_FEATS:
		{
			ret = diff->m_feats_factor;
			break;
		}
		case SCORE_CAT_ADVANCES:
		{
			ret = diff->m_advances_factor;
			break;
		}
		case SCORE_CAT_WONDERS:
		{
			ret = diff->m_wonders_factor;
			break;
		}

		case SCORE_CAT_POPULATION:
		{
			ret = diff->m_population_factor;
			break;
		}
		case SCORE_CAT_RANK:
		{
			ret = diff->m_rank_factor;
			break;
		}

		case SCORE_CAT_CITIES0TO30:
		{
			ret = diff->m_cities0to30_factor;
			break;
		}
		case SCORE_CAT_CITIES30TO100:
		{
			ret = diff->m_cities30to100_factor;
			break;
		}
		case SCORE_CAT_CITIES100TO500:
		{
			ret = diff->m_cities100to500_factor;
			break;
		}
		case SCORE_CAT_CITIES500PLUS:
		{
			ret = diff->m_cities500plus_factor;
			break;
		}
		case SCORE_CAT_CITIES_RECAPTURED:
		{
			ret = diff->m_cities_recaptured_factor;
			break;
		}

		case SCORE_CAT_OPPONENTS_CONQUERED:
		{
			ret = diff->m_opponents_conquered_factor;
			break;
		}
		case SCORE_CAT_TYPE_OF_VICTORY:
		{
			switch (m_victory_type)
			{
				case kScoreGameInProgress:
				case kScoreDefeat:
					return 0;
					break;
				case kScoreSoloVictory:
					return diff->m_solo_victory_bonus;
					break;
				case kScoreAlliedVictory:
					return diff->m_allied_victory_bonus;
					break;
				case kScoreWonderVictory:
					return diff->m_wonder_victory_bonus;
					break;
			}
			break;
		}
	}
	return (sint32)ret;

}

extern void cpw_NumberToCommas( uint64 number, MBCHAR *s );

const MBCHAR *Score::GetPartialScoreItemized(SCORE_CATEGORY cat)
{
	static MBCHAR buffer[80];
	static MBCHAR commaValue[80];
	static MBCHAR commaCount[80];
	const MBCHAR *formatNormal = "%s x %s";
	const MBCHAR *formatPercent = "%s%% x %s";

	
	Player *pl = g_player[m_owner];
	if(!pl) 
		pl = Player::GetDeadPlayer(m_owner);
	if(!pl)
		return 0;

	
	

	Difficulty *diff = pl->m_difficulty;
	sint32 count = GetPartialScoreCount(cat);
	if(count<0)
		count=0;
	double value = GetPartialScoreValue(cat);
	const MBCHAR *formatStr = formatNormal;

	switch (cat) 
	{
		case SCORE_CAT_POPULATION:
		case SCORE_CAT_RANK:
			formatStr = formatPercent;
			break;
	}
	
	cpw_NumberToCommas((sint32 )value, commaValue);
	cpw_NumberToCommas(count, commaCount);
	sprintf(buffer, formatStr, commaCount, commaValue);

	return buffer;
}

const MBCHAR *Score::GetScoreString(SCORE_CATEGORY cat)
{
	switch(cat) {
		case SCORE_CAT_FEATS: return g_theStringDB->GetNameStr("SCORE_CAT_FEATS");
		case SCORE_CAT_ADVANCES: return g_theStringDB->GetNameStr("SCORE_CAT_ADVANCES");
		case SCORE_CAT_WONDERS: return g_theStringDB->GetNameStr("SCORE_CAT_WONDERS");
		case SCORE_CAT_POPULATION: return g_theStringDB->GetNameStr("SCORE_CAT_POPULATION");
		case SCORE_CAT_RANK: return g_theStringDB->GetNameStr("SCORE_CAT_RANK");
		case SCORE_CAT_CITIES0TO30: return g_theStringDB->GetNameStr("SCORE_CAT_CITIES0TO30");
		case SCORE_CAT_CITIES30TO100: return g_theStringDB->GetNameStr("SCORE_CAT_CITIES30TO100");
		case SCORE_CAT_CITIES100TO500: return g_theStringDB->GetNameStr("SCORE_CAT_CITIES100TO500");
		case SCORE_CAT_CITIES500PLUS: return g_theStringDB->GetNameStr("SCORE_CAT_CITIES500PLUS");
		case SCORE_CAT_CITIES_RECAPTURED: return g_theStringDB->GetNameStr("SCORE_CAT_CITIES_RECAPTURED");
		case SCORE_CAT_OPPONENTS_CONQUERED: return g_theStringDB->GetNameStr("SCORE_CAT_OPPONENTS_CONQUERED");
		case SCORE_CAT_TYPE_OF_VICTORY: 
		{
			switch (m_victory_type)
			{
			case kScoreDefeat: return g_theStringDB->GetNameStr("SCORE_CAT_DEFEAT");
			case kScoreGameInProgress: return g_theStringDB->GetNameStr("SCORE_CAT_IN_PROGRESS");
			case kScoreWonderVictory: return g_theStringDB->GetNameStr("SCORE_CAT_WONDER_VICTORY");
			case kScoreAlliedVictory: return g_theStringDB->GetNameStr("SCORE_CAT_ALLIED_VICTORY");
			case kScoreSoloVictory: return g_theStringDB->GetNameStr("SCORE_CAT_SOLO_VICTORY");
			}
		}












		default:
			Assert(FALSE);
			return g_theStringDB->GetNameStr("SCORE_CAT_UNKNOWN");
	}
}

