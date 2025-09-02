//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ header
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

#ifdef HAVE_PRAGMA_ONCE
#pragma once
#endif
#ifndef __GAME_SETTINGS_H__
#define __GAME_SETTINGS_H__

class CivArchive;

// m_flags0, max are 32 flags
#define k_GameSettings_AlienEndGame_Bit              0x00000001

// m_flags1, max are 32 flags
#define k_GameSettings_KeepStore_Bit                 0x00000001
#define k_GameSettings_Genocide_Bit                  0x00000002

// m_flags2, max are 32 flags
#define k_GameSettings_Pollution_Bit                 0x00000001
#define k_GameSettings_CityCaptureOptions_Bit        0x00000002
#define k_GameSettings_OneCityChallenge_Bit          0x00000004
#define k_GameSettings_RevoltInsurgents_Bit          0x00000008
#define k_GameSettings_RevoltCasualties_Bit          0x00000010
#define k_GameSettings_BarbarianSpawnsBarbarian_Bit  0x00000020
#define k_GameSettings_Upgrade_Bit                   0x00000040
#define k_GameSettings_NewCombat_Bit                 0x00000080
#define k_GameSettings_GoldPerUnitSupport_Bit        0x00000100
#define k_GameSettings_GoldPerCity_Bit               0x00000200
#define k_GameSettings_NoCityLimit_Bit               0x00000400
#define k_GameSettings_Unused_Bit                    0x00000800

class GameSettings
{
private:

	sint32 m_difficulty;
	sint32 m_risk;
	sint32 m_flags0;
	sint32 m_flags1;

	sint32 m_startingAge;  // Unused in SP
	sint32 m_endingAge;    // Unused in SP

	sint32 m_flags2;

	void    AddFlag0(sint32 flag)       { m_flags0 |=  flag; }
	void RemoveFlag0(sint32 flag)       { m_flags0 &= ~flag; }
	bool    HasFlag0(sint32 flag) const { return (m_flags0 & flag) != 0; }

	void    AddFlag1(sint32 flag)       { m_flags1 |=  flag; }
	void RemoveFlag1(sint32 flag)       { m_flags1 &= ~flag; }
	bool    HasFlag1(sint32 flag) const { return (m_flags1 & flag) != 0; }

	void    AddFlag2(sint32 flag)       { m_flags2 |=  flag; }
	void RemoveFlag2(sint32 flag)       { m_flags2 &= ~flag; }
	bool    HasFlag2(sint32 flag) const { return (m_flags2 & flag) != 0; }

	friend class NetGameSettings;

public:
	GameSettings();
	GameSettings(CivArchive &archive);

	void SetAlienEndGameOn           (bool on)          { if(on)          AddFlag0(k_GameSettings_AlienEndGame_Bit);             else RemoveFlag0(k_GameSettings_AlienEndGame_Bit);             }

	void SetKeepScore                (bool keepScore)   { if(keepScore)   AddFlag1(k_GameSettings_KeepStore_Bit);                else RemoveFlag1(k_GameSettings_KeepStore_Bit);                }
	void SetGenocide                 (bool genocide)    { if(genocide)    AddFlag1(k_GameSettings_Genocide_Bit);                 else RemoveFlag1(k_GameSettings_Genocide_Bit);                 }

	void SetPollution                (bool pollution)   { if(pollution)   AddFlag2(k_GameSettings_Pollution_Bit);                else RemoveFlag2(k_GameSettings_Pollution_Bit);                }
	void SetCityCaptureOptions       (bool options)     { if(options)     AddFlag2(k_GameSettings_CityCaptureOptions_Bit);       else RemoveFlag2(k_GameSettings_CityCaptureOptions_Bit);       }
	void SetOneCityChallenge         (bool challange)   { if(challange)   AddFlag2(k_GameSettings_OneCityChallenge_Bit);         else RemoveFlag2(k_GameSettings_OneCityChallenge_Bit);         }
	void SetRevoltInsurgents         (bool insurgents)  { if(insurgents)  AddFlag2(k_GameSettings_RevoltInsurgents_Bit);         else RemoveFlag2(k_GameSettings_RevoltInsurgents_Bit);         }
	void SetRevoltCasualties         (bool casualties)  { if(casualties)  AddFlag2(k_GameSettings_RevoltCasualties_Bit);         else RemoveFlag2(k_GameSettings_RevoltCasualties_Bit);         }
	void SetBarbarianSpawnsBarbarian (bool barbs)       { if(barbs)       AddFlag2(k_GameSettings_BarbarianSpawnsBarbarian_Bit); else RemoveFlag2(k_GameSettings_BarbarianSpawnsBarbarian_Bit); }
	void SetUpgrade                  (bool upgrade)     { if(upgrade)     AddFlag2(k_GameSettings_Upgrade_Bit);                  else RemoveFlag2(k_GameSettings_Upgrade_Bit);                  }
	void SetNewCombat                (bool newCombat)   { if(newCombat)   AddFlag2(k_GameSettings_NewCombat_Bit);                else RemoveFlag2(k_GameSettings_NewCombat_Bit);                }
	void SetGoldPerUnitSupport       (bool goldSupport) { if(goldSupport) AddFlag2(k_GameSettings_GoldPerUnitSupport_Bit);       else RemoveFlag2(k_GameSettings_GoldPerUnitSupport_Bit);       }
	void SetGoldPerCity              (bool goldCity)    { if(goldCity)    AddFlag2(k_GameSettings_GoldPerCity_Bit);              else RemoveFlag2(k_GameSettings_GoldPerCity_Bit);              }
	void SetNoCityLimit              (bool noCityLimit) { if(noCityLimit) AddFlag2(k_GameSettings_NoCityLimit_Bit);              else RemoveFlag2(k_GameSettings_NoCityLimit_Bit);              }

	void SetStartingAge              (sint32 age)       { m_startingAge = age; }
	void SetEndingAge                (sint32 age)       { m_endingAge   = age; }

	void SaveToProfile() const;

	sint32 GetDifficulty()            const { return m_difficulty;                                          }
	sint32 GetRisk()                  const { return m_risk;                                                }
	sint32 GetStartingAge()           const { return m_startingAge;                                         }
	sint32 GetEndingAge()             const { return m_endingAge;                                           }

	bool IsAlienEndGame()             const { return HasFlag0(k_GameSettings_AlienEndGame_Bit);             }

	bool IsKeepScore()                const { return HasFlag1(k_GameSettings_KeepStore_Bit);                }
	bool IsGenocide()                 const { return HasFlag1(k_GameSettings_Genocide_Bit);                 }

	bool IsPollution()                const { return HasFlag2(k_GameSettings_Pollution_Bit);                }
	bool IsCityCaptureOptions()       const { return HasFlag2(k_GameSettings_CityCaptureOptions_Bit);       }
	bool IsOneCityChallenge()         const { return HasFlag2(k_GameSettings_OneCityChallenge_Bit);         }
	bool IsRevoltInsurgents()         const { return HasFlag2(k_GameSettings_RevoltInsurgents_Bit);         }
	bool IsRevoltCasualties()         const { return HasFlag2(k_GameSettings_RevoltCasualties_Bit);         }
	bool IsBarbarianSpawnsBarbarian() const { return HasFlag2(k_GameSettings_BarbarianSpawnsBarbarian_Bit); }
	bool IsUpgrade()                  const { return HasFlag2(k_GameSettings_Upgrade_Bit);                  }
	bool IsNewCombat()                const { return HasFlag2(k_GameSettings_NewCombat_Bit);                }
	bool IsGoldPerUnitSupport()       const { return HasFlag2(k_GameSettings_GoldPerUnitSupport_Bit);       }
	bool IsGoldPerCity()              const { return HasFlag2(k_GameSettings_GoldPerCity_Bit);              }
	bool IsNoCityLimit()              const { return HasFlag2(k_GameSettings_NoCityLimit_Bit);              }

	void Serialize(CivArchive &archive);

#if 0
	void PrintSizeOfClass()
	{
		DPRINTF(k_DBG_AI, ("\n"));
		DPRINTF(k_DBG_AI, ("Size of GameSettings class:\n"));
		DPRINTF(k_DBG_AI, ("GameSettings: %d\n", sizeof(GameSettings)));
		DPRINTF(k_DBG_AI, ("m_difficulty: %d\n", sizeof(m_difficulty)));
		DPRINTF(k_DBG_AI, ("m_risk: %d\n", sizeof(m_risk)));
		DPRINTF(k_DBG_AI, ("m_flags0: %d\n", sizeof(m_flags0)));
		DPRINTF(k_DBG_AI, ("m_flags1: %d\n", sizeof(m_flags1)));
		DPRINTF(k_DBG_AI, ("m_flags2: %d\n", sizeof(m_flags2)));
		DPRINTF(k_DBG_AI, ("m_startingAge: %d\n", sizeof(m_startingAge)));
		DPRINTF(k_DBG_AI, ("m_endingAge: %d\n", sizeof(m_endingAge)));
		DPRINTF(k_DBG_AI, ("\n"));
	}

	void PrintData()
	{
		DPRINTF(k_DBG_AI, ("\n"));
		DPRINTF(k_DBG_AI, ("Data of GameSettings class:\n"));
		DPRINTF(k_DBG_AI, ("m_difficulty: %d\n", m_difficulty));
		DPRINTF(k_DBG_AI, ("m_risk: %d\n", m_risk));
		DPRINTF(k_DBG_AI, ("m_flags0: %d\n", m_flags0));
		DPRINTF(k_DBG_AI, ("m_flags1: %d\n", m_flags1));
		DPRINTF(k_DBG_AI, ("m_flags2: %d\n", m_flags2));
		DPRINTF(k_DBG_AI, ("m_startingAge: %d\n", m_startingAge));
		DPRINTF(k_DBG_AI, ("m_endingAge: %d\n", m_endingAge));
		DPRINTF(k_DBG_AI, ("\n"));
	}
#endif
};

extern GameSettings *g_theGameSettings;
#endif
