

#include "c3.h"
#include "GameSettings.h"
#include "ProfileDB.h"
#include "CivArchive.h"
#include "network.h"
#include "Player.h"
#include "endgame.h"

extern ProfileDB *g_theProfileDB;
extern Player **g_player;

GameSettings *g_theGameSettings = NULL;

GameSettings::GameSettings()
{
	m_difficulty = g_theProfileDB->GetDifficulty();
	m_risk = g_theProfileDB->GetRiskLevel();
	m_alienEndGame = g_theProfileDB->IsAlienEndGameOn();
	m_pollution = g_theProfileDB->IsPollutionRule();
	m_keepScore = TRUE;

	m_startingAge = g_network.GetStartingAge();
	m_endingAge = g_network.GetEndingAge();
}

GameSettings::GameSettings(CivArchive &archive)
{
	Serialize(archive);
}

void GameSettings::SetKeepScore( BOOL keepScore )
{
	m_keepScore = keepScore;
}

void GameSettings::SetPollution( BOOL pollution )
{
	m_pollution = pollution;
}

void GameSettings::Serialize(CivArchive &archive)
{
	if(archive.IsStoring()) {
		archive.StoreChunk((uint8*)&m_difficulty, (uint8*)&m_pollution + sizeof(m_pollution));
	} else {
		archive.LoadChunk((uint8*)&m_difficulty, (uint8*)&m_pollution + sizeof(m_pollution));
	}
}

void GameSettings::SetAlienEndGameWon(sint32 player)
{


	


	







}

BOOL GameSettings::GetAlienEndGame() const
{
	if(g_network.IsActive())
		return FALSE;
	return m_alienEndGame;
}
