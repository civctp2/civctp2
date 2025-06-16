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
// - None
//
//----------------------------------------------------------------------------
//
// Modifications from the original Activision code:
//
// - Use the difficulty and barbarian risk as selected by the user.
// - Removed new rules attempt - E 12.27.2006
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

	// m_flags0 group
	SetAlienEndGameOn           (g_theProfileDB->IsAlienEndGameOn()           != 0);

	// m_flags1 group
	SetKeepScore                (true);
	SetGenocide                 (g_theProfileDB->IsGenocideRule()             != 0);

	// m_flags2 group
	SetPollution                (g_theProfileDB->IsPollutionRule()            != 0);
	SetCityCaptureOptions       (g_theProfileDB->IsCityCaptureOptions()       != 0);
	SetOneCityChallenge         (g_theProfileDB->IsOneCityChallenge()         != 0);
	SetRevoltInsurgents         (g_theProfileDB->IsRevoltInsurgents()         != 0);
	SetRevoltCasualties         (g_theProfileDB->IsRevoltCasualties()         != 0);
	SetBarbarianSpawnsBarbarian (g_theProfileDB->IsBarbarianSpawnsBarbarian() != 0);
	SetUpgrade                  (g_theProfileDB->IsUpgrade()                  != 0);
	SetNewCombat                (g_theProfileDB->IsNewCombat()                != 0);
	SetGoldPerUnitSupport       (g_theProfileDB->IsGoldPerUnitSupport()       != 0);
	SetGoldPerCity              (g_theProfileDB->IsGoldPerCity()              != 0);
	SetNoCityLimit              (g_theProfileDB->IsNoCityLimit()              != 0);

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

void GameSettings::Serialize(CivArchive &archive)
{
	if(archive.IsStoring()) {
		archive.StoreChunk((uint8*)&m_difficulty, (uint8*)&m_flags2 + sizeof(m_flags2));
	} else {
		archive.LoadChunk((uint8*)&m_difficulty, (uint8*)&m_flags2 + sizeof(m_flags2));
	}
}

void GameSettings::SaveToProfile() const
{
	g_theProfileDB->SetDifficulty(GetDifficulty());
	g_theProfileDB->SetRiskLevel(GetRisk());
	g_theProfileDB->SetSPStartingAge(GetStartingAge());
	g_theProfileDB->SetSPEndingAge(GetEndingAge());

	g_theProfileDB->SetAlienEndGame(IsAlienEndGame());

	g_theProfileDB->SetGenocideRule(IsGenocide());

	g_theProfileDB->SetPollutionRule(IsPollution());
	g_theProfileDB->SetCityCaptureOptions(IsCityCaptureOptions());
	g_theProfileDB->SetOneCity(IsOneCityChallenge());
	g_theProfileDB->SetRevoltInsurgents(IsRevoltInsurgents());
	g_theProfileDB->SetRevoltCasualties(IsRevoltCasualties());
	g_theProfileDB->SetBarbarianSpawnsBarbarian(IsBarbarianSpawnsBarbarian());
	g_theProfileDB->SetUpgrade(IsUpgrade());
	g_theProfileDB->SetNewCombat(IsNewCombat());
	g_theProfileDB->SetGoldPerUnitSupport(IsGoldPerUnitSupport());
	g_theProfileDB->SetGoldPerCity(IsGoldPerCity());
	g_theProfileDB->SetNoCityLimit(IsNoCityLimit());
}
