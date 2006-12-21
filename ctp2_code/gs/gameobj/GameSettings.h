#ifdef HAVE_PRAGMA_ONCE
#pragma once
#endif
#ifndef __GAME_SETTINGS_H__
#define __GAME_SETTINGS_H__

class CivArchive;

class GameSettings {
private:
	
	sint32 m_difficulty;
	sint32 m_risk;
	BOOL m_alienEndGame;
	BOOL m_keepScore;
	BOOL m_cityCaptureOptions;

	sint32 m_startingAge, m_endingAge; 
	
	BOOL m_pollution;
	

	friend class NetGameSettings;

public:
	GameSettings();
	GameSettings(CivArchive &archive);

	void SetKeepScore( BOOL keepScore );
	void SetPollution( BOOL pollution );
	void SetStartingAge(sint32 age) { m_startingAge = age; }
	void SetEndingAge(sint32 age) { m_endingAge = age; }
	void SetAlienEndGameWon(sint32 player);

	sint32 GetDifficulty() const { return m_difficulty; }
	sint32 GetRisk() const { return m_risk; }
	BOOL GetAlienEndGame() const;
	BOOL GetKeeppScore() const { return m_keepScore; }
	BOOL GetPollution() const { return m_pollution; }
	sint32 GetStartingAge() const { return m_startingAge; }
	sint32 GetEndingAge() const { return m_endingAge; }

	void Serialize(CivArchive &archive);
};

extern GameSettings *g_theGameSettings;
#endif
