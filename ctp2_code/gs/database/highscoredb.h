
#pragma once
#ifndef __HIGHSCOREDB_H__
#define __HIGHSCOREDB_H__

class CivArchive;
class Token;

#define k_MAX_HIGH_SCORES	10

struct HighScoreInfo {
	MBCHAR		m_name[256];
	sint32		m_score;
	HighScoreInfo	*m_next;
};

class HighScoreDB {

public:
	sint32		m_nHighScores;
	HighScoreInfo	*m_highScoreInfo;

	HighScoreDB();
	~HighScoreDB();

	void Initialize(void);
	void LoadHighScores( void );
	void SaveHighScores( void );

	sint32 AddHighScore(MBCHAR *name, sint32 score);
	sint32 CheckMaxScores( void );
	sint32 GetNumHighScores() const { return m_nHighScores; }
	HighScoreInfo *GetHighScoreInfo( sint32 index );
};

#endif
