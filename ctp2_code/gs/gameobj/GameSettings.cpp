//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : Game settings
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
//----------------------------------------------------------------------------
//
// Modifications from the original Activision code:
//
// - Use the difficulty and barbarian risk as selected by the user.
// - removed new rules attempt - E 12.27.2006
//
//----------------------------------------------------------------------------

#include "c3.h"
#include "GameSettings.h"
#include "profileDB.h"
#include "civarchive.h"
#include "network.h"
#include "player.h"
#include "EndGame.h"

extern ProfileDB *g_theProfileDB;
extern Player **g_player;
extern BOOL			g_setDifficultyUponLaunch;
extern sint32		g_difficultyToSetUponLaunch;
extern BOOL			g_setBarbarianRiskUponLaunch;
extern sint32		g_barbarianRiskUponLaunch;

GameSettings *g_theGameSettings = NULL;

GameSettings::GameSettings()
{
	if (g_setDifficultyUponLaunch) 
	{
		m_difficulty	= g_difficultyToSetUponLaunch;
	}
	else
	{
		m_difficulty	= g_theProfileDB->GetDifficulty();
	}

	if (g_setBarbarianRiskUponLaunch) 
	{
		m_risk			= g_barbarianRiskUponLaunch;
	}
	else
	{
		m_risk			= g_theProfileDB->GetRiskLevel();
	}
	m_alienEndGame = g_theProfileDB->IsAlienEndGameOn();
	m_pollution = g_theProfileDB->IsPollutionRule();
	m_keepScore = TRUE;

	m_startingAge = g_network.GetStartingAge();
	m_endingAge = g_network.GetEndingAge();
}

GameSettings::GameSettings(CivArchive &archive)
{
	Serialize(archive);

	if (g_setDifficultyUponLaunch) 
	{
		m_difficulty	= g_difficultyToSetUponLaunch;
	}

	if (g_setBarbarianRiskUponLaunch) 
	{
		m_risk			= g_barbarianRiskUponLaunch;
	}
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
