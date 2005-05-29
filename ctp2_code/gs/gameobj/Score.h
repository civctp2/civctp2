#ifdef HAVE_PRAGMA_ONCE
#pragma once
#endif
#ifndef __SCORE_H__
#define __SCORE_H__

class CivArchive;
class Player;
typedef sint32 PLAYER_INDEX;

enum SCORE_CATEGORY {
	SCORE_CAT_FEATS,
	SCORE_CAT_ADVANCES,
	SCORE_CAT_WONDERS,
	SCORE_CAT_POPULATION,
	SCORE_CAT_CITIES0TO30,
	SCORE_CAT_CITIES30TO100,
	SCORE_CAT_CITIES100TO500,
	SCORE_CAT_CITIES500PLUS,
	SCORE_CAT_CITIES_RECAPTURED,
	SCORE_CAT_OPPONENTS_CONQUERED,
	SCORE_CAT_TYPE_OF_VICTORY,
	SCORE_CAT_RANK,
	SCORE_CAT_MAX,

	SCORE_CAT_CELEBRATIONS = -1,
	SCORE_CAT_YEARS_AT_PEACE = -1,
	SCORE_CAT_YEAR_OF_VICTORY = -1,
	SCORE_CAT_DIFFICULTY_BONUS = -1,
	SCORE_CAT_MAP_SIZE = -1,
	SCORE_CAT_NUMBER_OF_OPPONENTS = -1,
	SCORE_CAT_UNITS_LOST = -1,
	SCORE_CAT_RIOTS = -1,
	SCORE_CAT_REVOLUTIONS = -1,
	SCORE_CAT_POLLUTION = -1,
	SCORE_CAT_CITIES = -1,
};

enum eScoreVictory
{
	kScoreGameInProgress,
	kScoreDefeat,
	kScoreSoloVictory,
	kScoreAlliedVictory,
	kScoreWonderVictory,
};

class Score
{
private:
	
	
	PLAYER_INDEX m_owner;
	sint32 m_cities_recaptured;
	sint32 m_opponents_conquered;
	sint32 m_finalScore;
	sint32 m_victory_type;	
	
	sint32 m_feats;			








	
	
	

	friend class NetPlayer;
	friend class NetScores;

	sint32 GetPartialScoreCount(SCORE_CATEGORY cat);
	double GetPartialScoreValue(SCORE_CATEGORY cat);
	static sint32 GetPlayerStrength(Player *pl);
public:
	Score(PLAYER_INDEX owner);
	Score(CivArchive &archive);
	void Serialize(CivArchive &archive);
	

	void SetFinalScore(sint32 score) { m_finalScore = score; }

	sint32 GetTotalScore();
	sint32 GetPartialScore(SCORE_CATEGORY cat);
	const MBCHAR *GetScoreString(SCORE_CATEGORY cat);
	const MBCHAR *GetPartialScoreItemized(SCORE_CATEGORY cat);

	void AddOpponentConquered();
	void AddCityRecaptured();
	void SetWonByWonder() {SetVictoryType(kScoreWonderVictory);}
	void SetVictoryType(eScoreVictory type);
	void AddFeat();


	void AddCelebration();
	void AddYearAtPeace();
	void AddUnitLost();
	void AddRiot();
	void AddRevolution();
};

#endif
