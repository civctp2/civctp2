//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : Handling of user preferences.
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
//
//----------------------------------------------------------------------------
//
// Modifications from the original Activision code:
//
// - Option added to enable viewing info on actions that are too expensive.
// - Option added to close a messagebox automatically on eyepoint clicking.
// - Option added to choose a color set.
// - Option added to select which order buttons are displayed for an army.
// - Option added to select message adding style (top or bottom).
// - Option added to include multiple data directories.
//
//----------------------------------------------------------------------------

#include "c3.h"
#include "c3errors.h"
#include "profileDB.h"	
#include "MapPoint.h"
#include "Token.h"
#include "Globals.h"

#include "StrDB.h"
#include "CivilisationDB.h"
#include "Civilisation.h"
#include "SimpleDynArr.h"
#include "SlicEngine.h"
#include "civapp.h"
#include "screenutils.h"
#include "GameSettings.h"
#include "soundmanager.h"
#include "CivPaths.h"
#include "player.h"
#include "Diffcly.h"


#include "Action.h"

extern CivPaths				*g_civPaths;
extern StringDB				*g_theStringDB;
extern CivilisationDatabase *g_theCivilisationDB;
extern SlicEngine			*g_slicEngine;
extern CivApp				*g_civApp;
extern GameSettings			*g_theGameSettings;
extern SoundManager			*g_soundManager;
extern Player              **g_player;

#include "Diplomacy_Log.h"
extern Diplomacy_Log *g_theDiplomacyLog; 


ProfileDB::ProfileDB()
:	m_closeEyepoint(FALSE),
	m_colorSet(0),
	m_showExpensive(FALSE),
	m_showOrderUnion(FALSE),
	m_recentAtTop(FALSE)
{
	m_vars = new PointerList<ProfileVar>;

    m_autocenter = TRUE;

	m_mapSize = MAPSIZE_MEDIUM;

	sint32 i;
	for(i = 0; i < k_MAX_PLAYERS; i++) {
		m_ai_personality[i][0] = 0;
	}

	m_genocide = FALSE;
	m_trade = FALSE;
	m_simplecombat = FALSE;
	m_pollution = FALSE;
	m_lineofsight = FALSE;
	
	m_unitAnim = FALSE;

	m_goodAnim = FALSE;
	m_tradeAnim = FALSE;
	m_waterAnim = FALSE;
	m_libraryAnim = FALSE;
	m_wonderMovies = FALSE;
	m_bounceMessage = FALSE;
	m_messageAdvice = FALSE;
	m_tutorialAdvice = FALSE;
	m_enemyMoves = TRUE;
	m_revoltWarning = FALSE;
	m_enemyIntrude = FALSE;
	m_unitLostWarning = FALSE;
	m_tradeLostWarning = FALSE;
	m_cityLostWarning = FALSE; 
	m_invulnerableTrade = FALSE;
	m_fullScreenMovies = FALSE;
	m_showCityInfluence = FALSE;

	
	m_fogOfWar = FALSE;
	m_startType = FALSE;

	m_playerNumber = 1;

	
	
	m_civIndex = (CIV_INDEX)16; 

	m_gameName[0] = '\0';
	m_leaderName[0] = '\0';
	m_civName[0] = '\0';
	m_saveNote[0] = '\0';
	m_ruleSets[0] = '\0';
	m_isSaved = FALSE;
	m_isScenario = FALSE;
    m_gender = GENDER_MALE; 

    m_cheat_age = 0; 

	m_autoSave = FALSE;

	m_noHumansOnHost = FALSE;

	m_logPlayerStats = FALSE;

	m_sfxVolume = 8;
	m_musicVolume = 8;
	m_voiceVolume = 8;

	m_xWrap = FALSE;
	m_yWrap = FALSE;
	m_autoGroup = FALSE;
	m_autoDeselect = FALSE;
	m_autoSelectNext = FALSE;
	m_autoSelectFirstUnit = FALSE;
	m_autoTurnCycle = FALSE;
	m_combatLog = FALSE;

	m_useLeftClick = FALSE;

	m_showZoomedCombat = FALSE;
	m_useFingerprinting = FALSE;
	m_useRedbookAudio = FALSE;
	m_requireCD = FALSE;
	m_protected = TRUE;
	m_tryWindowsResolution = TRUE;
	m_useDirectXBlitter = TRUE;
	m_screenResWidth = 640;
	m_screenResHeight = 480;

	m_zoomedCombatAlways = FALSE;

	m_attackEveryone = FALSE;

	m_nonRandomCivs = FALSE;

	for(i = 0; i < k_NUM_MAP_PASSES; i++) {
		m_map_settings[i] = NULL;
	}

	m_gameWatchDirectory[0] = '\0';

	m_autoEndMultiple = FALSE;

	m_wetdry = 5;
	m_warmcold = 5;
	m_oceanland = 5;
	m_islandcontinent = 5;
	m_homodiverse = 5;
	m_goodcount = 5;
	
	m_throneRoom = FALSE;

	m_alienEndGame = TRUE;

	m_max_players = 16;

	m_allow_ai_settle_move_cheat = FALSE;
    m_is_diplomacy_log_on = FALSE; 
	m_unitCompleteMessages = FALSE;
	m_nonContinuousUnitCompleteMessages = FALSE;

	m_debugSlic = FALSE;

	m_loadedFromTutorial = FALSE;
	m_dontSave = FALSE;
	m_dontKillMessages = FALSE;
	m_aiPopCheat = TRUE;

	
	m_unitSpeed = 1;
	m_mouseSpeed = 5;
	m_showCityNames = TRUE;
	m_showTradeRoutes = TRUE;

	
	m_leftHandedMouse = FALSE;

	m_cityBuiltMessage = TRUE;
	m_useAttackMessages = FALSE;

	m_useOldRegisterClick = FALSE;
	m_useCTP2Mode = TRUE;
	m_moveHoldTime = 500;

	m_autoSwitchTabs = TRUE;
	m_showPoliticalBorders = TRUE;
	m_battleSpeed = 5;

	m_scrollDelay = 3;

	m_showEnemyHealth = FALSE;

	m_autoRenameCities = FALSE;
	m_autoOpenCityWindow = TRUE;

	m_endTurnSound = TRUE;

	m_enableLogs = FALSE;

	m_displayUnits=TRUE;
	m_displayCities=TRUE;
	m_displayBorders=TRUE;
	m_displayFilter=TRUE;
	m_displayTrade=TRUE;
	m_displayTerrain=TRUE;

	Var("NumPlayers", PV_NUM, &m_nPlayers, NULL, false);
	Var("AiOn", PV_BOOL, &m_ai_on, NULL, false);
    Var("UseNiceStart", PV_BOOL, &m_use_nice_start, NULL, false); 
	Var("UseMapPlugin", PV_BOOL, &m_use_map_plugin, NULL, false);
	
	Var("Difficulty", PV_NUM, &m_difficulty, NULL, false);
	Var("RiskLevel", PV_NUM, &m_risklevel, NULL, false);

	Var("Pollution", PV_BOOL, &m_pollution, NULL, false);
	Var("UnitAnim", PV_BOOL, &m_unitAnim, NULL);

	Var("GoodAnim", PV_BOOL, &m_goodAnim, NULL, false);
	Var("TradeAnim", PV_BOOL, &m_tradeAnim, NULL);
	Var("WaterAnim", PV_BOOL, &m_waterAnim, NULL, false);
	Var("LibraryAnim", PV_BOOL, &m_libraryAnim, NULL, false);
	Var("WonderMovies", PV_BOOL, &m_wonderMovies, NULL);
	Var("BounceMessage", PV_BOOL, &m_bounceMessage, NULL, false);
	Var("MessageAdvice", PV_BOOL, &m_messageAdvice, NULL, false);
	Var("TutorialAdvice", PV_BOOL, &m_tutorialAdvice, NULL, false);
	Var("EnemyMoves", PV_BOOL, &m_enemyMoves, NULL);
	Var("RevoltWarning", PV_BOOL, &m_revoltWarning, NULL, false);
	Var("EnemyIntrude", PV_BOOL, &m_enemyIntrude, NULL, false);
	Var("UnitLostWarning", PV_BOOL, &m_unitLostWarning, NULL, false);
	Var("TradeLostWarning", PV_BOOL, &m_tradeLostWarning, NULL, false);
	Var("CityLostWarning", PV_BOOL, &m_cityLostWarning, NULL, false);
	Var("AutoCenter", PV_BOOL, &m_autocenter, NULL);
	Var("FullScreenMovies", PV_BOOL, &m_fullScreenMovies, NULL, false);
	Var("AutoSave", PV_BOOL, &m_autoSave, NULL);
	Var("PlayerNumber", PV_NUM, (sint32 *)&m_playerNumber, NULL, false);
	
	Var("CivIndex", PV_NUM, (sint32 *)&m_civIndex, NULL, false);

	Var("GameName", PV_STRING, NULL, (char *)m_gameName, false);
	Var("LeaderName", PV_STRING, NULL, (char*)m_leaderName, false);
	Var("CivName", PV_STRING, NULL, (char*)m_civName, false);
	Var("SaveNote", PV_STRING, NULL, (char*)m_saveNote, false);
	Var("Gender", PV_NUM, (sint32 *)&m_gender, NULL, false);

	
	Var("NoHumansOnHost", PV_BOOL, &m_noHumansOnHost, NULL, false);
	Var("LogPlayerStats", PV_BOOL, &m_logPlayerStats, NULL, false);

	Var("SfxVolume", PV_NUM, &m_sfxVolume, NULL, false);
	Var("VoiceVolume", PV_NUM, &m_voiceVolume, NULL, false);
	Var("MusicVolume", PV_NUM, &m_musicVolume, NULL, false);

	Var("XWrap", PV_BOOL, &m_xWrap, NULL, false);
	Var("YWrap", PV_BOOL, &m_yWrap, NULL, false);
	Var("AutoGroup", PV_BOOL, &m_autoGroup, NULL, false);
	Var("AutoDeselect", PV_BOOL, &m_autoDeselect, NULL);
	Var("AutoSelectNext", PV_BOOL, &m_autoSelectNext, NULL);
	Var("AutoSelectFirstUnit", PV_BOOL, &m_autoSelectFirstUnit, NULL);
	Var("AutoTurnCycle", PV_BOOL, &m_autoTurnCycle, NULL);
	Var("CombatLog", PV_BOOL, &m_combatLog, NULL, false);

	Var("UseLeftClick", PV_BOOL, &m_useLeftClick, NULL, false);
	Var("ShowZoomedCombat", PV_BOOL, &m_showZoomedCombat, NULL);
	Var("UseFingerPrinting", PV_BOOL, &m_useFingerprinting, NULL, false);
	Var("UseRedbookAudio", PV_BOOL, &m_useRedbookAudio, NULL, false);
	Var("RequireCD", PV_BOOL, &m_requireCD, NULL, false);
	Var("Prophylaxis", PV_BOOL, &m_protected, NULL, false);
	Var("TryWindowsResolution", PV_BOOL, &m_tryWindowsResolution, NULL, false);
	Var("UseDirectXBlitter", PV_BOOL, &m_useDirectXBlitter, NULL, false);
	Var("ScreenResWidth", PV_NUM, &m_screenResWidth, NULL, false);
	Var("ScreenResHeight", PV_NUM, &m_screenResHeight, NULL, false);

	Var("ZoomedCombatAlways", PV_BOOL, &m_zoomedCombatAlways, NULL);
	Var("AttackEveryone", PV_BOOL, &m_attackEveryone, NULL, false);
	Var("NonRandomCivs", PV_BOOL, &m_nonRandomCivs, NULL, false);
	Var("GameWatchDirectory", PV_STRING, NULL, (char*)m_gameWatchDirectory, false);
	Var("AutoEndMultiple", PV_BOOL, &m_autoEndMultiple, NULL);

	Var("WetDry", PV_NUM, &m_wetdry, NULL, false);
	Var("WarmCold", PV_NUM, &m_warmcold, NULL, false);
	Var("OceanLand", PV_NUM, &m_oceanland, NULL, false);
	Var("IslandContinent", PV_NUM, &m_islandcontinent, NULL, false);
	Var("HomoDiverse", PV_NUM, &m_homodiverse, NULL, false);
	Var("GoodCount", PV_NUM, &m_goodcount, NULL, false);
	
	Var("ThroneRoom", PV_BOOL, &m_throneRoom, NULL, false);
	Var("MaxPlayers", PV_NUM, &m_max_players, NULL, false);
	Var("MapSize", PV_NUM, (sint32 *)&m_mapSize, NULL, false);
	
	Var("AlienEndGame", PV_BOOL, &m_alienEndGame, NULL, false);
	Var("UnitCompleteMessages", PV_BOOL, &m_unitCompleteMessages, NULL);
	Var("NonContinuousUnitCompleteMessages", PV_BOOL, &m_nonContinuousUnitCompleteMessages, NULL);
	Var("DebugSlic", PV_BOOL, &m_debugSlic, NULL, false);
	Var("DiplomacyLog", PV_BOOL, &m_is_diplomacy_log_on, NULL, false);
	Var("CheatAge", PV_NUM, &m_cheat_age, NULL, false);
	Var("DontKillMessages", PV_BOOL, &m_dontKillMessages, NULL, false);
	Var("AIPopCheat", PV_BOOL, &m_aiPopCheat, NULL, false);
	Var("ShowCityNames", PV_BOOL, &m_showCityNames, NULL);
	Var("ShowTradeRoutes", PV_BOOL, &m_showTradeRoutes, NULL);

	
	Var("UnitSpeed", PV_NUM, &m_unitSpeed, NULL);
	Var("MouseSpeed", PV_NUM, &m_mouseSpeed, NULL);

	
	Var("LeftHandedMouse", PV_BOOL, &m_leftHandedMouse, NULL);
	
	Var("CityBuiltMessage", PV_BOOL, &m_cityBuiltMessage, NULL, false);

	Var("UseAttackMessages", PV_BOOL, &m_useAttackMessages, NULL, false);

	Var("MapPlugin0", PV_STRING, NULL, (char *)m_map_plugin_name[0], false);
	Var("MapPlugin1", PV_STRING, NULL, (char *)m_map_plugin_name[1], false);
	Var("MapPlugin2", PV_STRING, NULL, (char *)m_map_plugin_name[2], false);
	Var("MapPlugin3", PV_STRING, NULL, (char *)m_map_plugin_name[3], false);

	Var("OldRegisterClick", PV_BOOL, &m_useOldRegisterClick, NULL, false);
	Var("CTP2Mode", PV_BOOL, &m_useCTP2Mode, NULL, false);
	Var("MoveHoldTime", PV_NUM, &m_moveHoldTime, NULL, false);

	Var("BattleSpeed", PV_NUM, &m_battleSpeed, NULL);

	Var("ScrollDelay", PV_NUM, &m_scrollDelay, NULL);

	Var("AutoSwitchTabs", PV_BOOL, &m_autoSwitchTabs, NULL);
	Var("AutoRenameCities", PV_BOOL, &m_autoRenameCities, NULL, false);
	Var("AutoOpenCityWindow", PV_BOOL, &m_autoOpenCityWindow, NULL);

	Var("ShowEnemyHealth", PV_BOOL, &m_showEnemyHealth, NULL);
	Var("ShowCityInfluence", PV_BOOL, &m_showCityInfluence, NULL);
	Var("ShowPoliticalBorders", PV_BOOL, &m_showPoliticalBorders, NULL);

	Var("EndTurnSound", PV_BOOL, &m_endTurnSound, NULL);
	Var("EnableLogs", PV_BOOL, &m_enableLogs, NULL, false);
	Var("DisplayUnits", PV_BOOL, &m_displayUnits, NULL, false);
	Var("DisplayCities", PV_BOOL, &m_displayCities, NULL, false);
	Var("DisplayBorders", PV_BOOL, &m_displayBorders, NULL, false);
	Var("DisplayFilter", PV_BOOL, &m_displayFilter, NULL, false);
	Var("DisplayTrade", PV_BOOL, &m_displayTrade, NULL, false);
	Var("DisplayTerrain", PV_BOOL, &m_displayTerrain, NULL, false);

	Var("CloseOnEyepoint", PV_BOOL, &m_closeEyepoint, NULL);
	Var("ShowExpensive", PV_BOOL, &m_showExpensive, NULL);
	Var("ColorSet", PV_NUM, &m_colorSet, NULL, false);
	Var("ShowOrderUnion", PV_BOOL, &m_showOrderUnion, NULL);
	Var("RecentAtTop", PV_BOOL, &m_recentAtTop, NULL);
	Var("RuleSets", PV_STRING, NULL, m_ruleSets, false);
}

void ProfileDB::DefaultSettings(void)
{
	
	

	
	StringId	leaderNameId = g_theCivilisationDB->GetLeaderName((CIV_INDEX)m_civIndex);
	StringId	civNameId = g_theCivilisationDB->GetPluralCivName((CIV_INDEX)m_civIndex);

	strcpy(m_leaderName, g_theStringDB->GetNameStr(leaderNameId));
	strcpy(m_civName, g_theStringDB->GetNameStr(civNameId));
}

ProfileDB::~ProfileDB()

{
	Save();

	if(m_vars) {
		m_vars->DeleteAll();
		delete m_vars;
	}
}

BOOL ProfileDB::Init(BOOL forTutorial)
{
	MBCHAR profileName[_MAX_PATH];
	MBCHAR *profileTxtFile;

	if(!forTutorial) {
		profileTxtFile = g_civPaths->FindFile(C3DIR_DIRECT, "userprofile.txt",
											  profileName);
		if(!profileTxtFile || !c3files_PathIsValid(profileTxtFile)) {
			profileTxtFile = g_civPaths->FindFile(C3DIR_GAMEDATA, 
												  "profile.txt", profileName);
		}
	} else {
		
		m_loadedFromTutorial = TRUE;
        profileTxtFile = g_civPaths->FindFile(C3DIR_GAMEDATA, 
                                              "tut_profile.txt", profileName);
	}
	
    if (profileTxtFile == NULL) { 
        m_nPlayers = 3; 
	    m_ai_on = FALSE;
		return FALSE;
    } else { 
		FILE *pro_file = c3files_fopen(C3DIR_DIRECT, profileTxtFile, "r");
		if(pro_file) {
			
			
			sint32 saved_width;
			sint32 saved_height;

			if (forTutorial) {
				saved_width = m_screenResWidth;
				saved_height = m_screenResHeight;
			}

			BOOL res = Parse(pro_file);
			fclose(pro_file);

			if(res) {
				Save();
			}

			if (forTutorial) {
				m_screenResWidth = saved_width;
				m_screenResHeight = saved_height;
			}
			
			return res;
		}
		return FALSE;
	}

}


BOOL ProfileDB::Parse(FILE *file)
{ 
	char line[k_MAX_NAME_LEN];
	sint32 linenum = 0;
	while(!feof(file)) {
		if(fgets(line, k_MAX_NAME_LEN, file) == NULL)
			return TRUE;
		linenum++;
		sint32 len = strlen(line);
		
		while(isspace(line[len - 1])) {
			line[len - 1] = 0;
			len--;
		}

		char *name, *value;
		name = line;
		while(isspace(*name) && *name != 0) {
			name++;
		}
		if(*name == 0 || *name == '#') {
			
			continue;
		}
		if(!isalpha(*name)) {
			c3errors_ErrorDialog("Profile", "Line %d: name must start with a letter", linenum);
			return FALSE;
		}
		value = name + 1;
		while(*value != '=' && *value != 0) {
			if(isspace(*value))
				*value = 0;
			value++;
		}
		if(*value != '=') {
			c3errors_ErrorDialog("Profile", "Line %d: no = found", linenum);
			return FALSE;
		}
		*value = 0;
		value++;
		while(isspace(*value) && *value != 0) {
			value++;
		}
		
		
		
		
		
		PointerList<ProfileVar>::Walker walk(m_vars);
		bool found = false;
		while(walk.IsValid() && !found) {
			ProfileVar *var = walk.GetObj();
			if(stricmp(var->m_name, name) == 0) {
				found = true;
				switch(var->m_type) {
					case PV_BOOL:
						if(*value == 0) {
							c3errors_ErrorDialog("Profile", "Line %d: no value found", linenum);
							return FALSE;
						}
						if(stricmp(value, "yes") == 0 ||
						   stricmp(value, "true") == 0 ||
						   stricmp(value, "1") == 0) {
							*var->m_numValue = 1;
						} else if(stricmp(value, "no") == 0 ||
								  stricmp(value, "false") == 0 ||
								  stricmp(value, "0") == 0) {
							*var->m_numValue = 0;
						} else {
							c3errors_ErrorDialog("Profile", "Line %d: %s is an illegal value for %s", value, var->m_name);
							return FALSE;
						}
						break;
					case PV_NUM:
						if(*value == 0) {
							c3errors_ErrorDialog("Profile", "Line %d: no value found", linenum);
							return FALSE;
						}
						*var->m_numValue = atoi(value);
						break;
					case PV_STRING:
						if(strlen(value) > k_MAX_NAME_LEN - 1) {
							c3errors_ErrorDialog("Profile", "Line %d: string too long");
							return FALSE;
						}
						strcpy(var->m_stringValue, value);
						break;
					default:
						Assert(FALSE);
				}
			}
			walk.Next();
		}
	}

    return TRUE; 
}

void ProfileDB::SetTutorialAdvice( BOOL val )
{
	m_tutorialAdvice = val;














}

void ProfileDB::SetDiplmacyLog(BOOL b)
{
    if (b) { 
        if (m_is_diplomacy_log_on) {
            Assert(g_theDiplomacyLog); 
        } else { 
            m_is_diplomacy_log_on = TRUE; 
            Assert(g_theDiplomacyLog); 
            g_theDiplomacyLog = new Diplomacy_Log;
        }
    } else { 
        if (m_is_diplomacy_log_on) { 
            m_is_diplomacy_log_on = FALSE; 
            Assert(g_theDiplomacyLog);
            delete g_theDiplomacyLog; 
            g_theDiplomacyLog=NULL; 
        } else { 
            Assert(g_theDiplomacyLog == NULL); 
        }
    }
}

void ProfileDB::SetPollutionRule( BOOL rule ) 
{
	m_pollution = rule;

	if ( g_theGameSettings ) {
		g_theGameSettings->SetPollution( m_pollution );
	}
}

void ProfileDB::SetSFXVolume(sint32 vol)	
{
	m_sfxVolume = vol; 
	if ( g_soundManager ) {
		g_soundManager->SetVolume( SOUNDTYPE_SFX, vol );
	}
}		

void ProfileDB::SetVoiceVolume(sint32 vol)				
{ 
	m_voiceVolume = vol; 
	if ( g_soundManager ) {
		g_soundManager->SetVolume( SOUNDTYPE_VOICE, vol );
	}
}

void ProfileDB::SetMusicVolume(sint32 vol)				
{ 
	m_musicVolume = vol; 
	if ( g_soundManager ) {
		g_soundManager->SetVolume( SOUNDTYPE_MUSIC, vol );
	}
}

void ProfileDB::SetDifficulty(uint32 x)
{ 
	Assert((x>=0) && (x<7)); 
	if(x >= 0 && x < 7) {
		m_difficulty = x;
		if(g_player) {
			sint32 p;
			for(p = 0; p < k_MAX_PLAYERS; p++) {
				if(g_player[p]) {
					delete g_player[p]->m_difficulty;
					g_player[p]->m_difficulty = new Difficulty(m_difficulty,
															   p,
															   g_player[p]->m_playerType != PLAYER_TYPE_ROBOT);
				}
			}
		}
	}
}

void ProfileDB::Var(char *name, PROF_VAR_TYPE type, sint32 *numValue,
					char *stringValue, bool visible)
{
	m_vars->AddTail(new ProfileVar(name, type, numValue, stringValue, visible));
}

void ProfileDB::Save()
{
	if(m_loadedFromTutorial || m_dontSave) {
		
		return;
	}

	FILE *file;
	file = c3files_fopen(C3DIR_DIRECT, "userprofile.txt", "w");
	if(file) {
		PointerList<ProfileVar>::Walker walk(m_vars);
		while(walk.IsValid()) {
			ProfileVar *var = walk.GetObj();
			fprintf(file, "%s=", var->m_name);

			switch(var->m_type) {
				case PV_BOOL:
					if(*var->m_numValue) {
						fprintf(file, "Yes\n");
					} else {
						fprintf(file, "No\n");
					}
					break;
				case PV_NUM:
					fprintf(file, "%d\n", *var->m_numValue);
					break;
				case PV_STRING:
					fprintf(file, "%s\n", var->m_stringValue);
					break;
			}
			walk.Next();
		}
		fclose(file);
	}
}

sint32 ProfileDB::GetValueByName(const char *name)
{
	PointerList<ProfileVar>::Walker walk(m_vars);
	while(walk.IsValid()) {
		ProfileVar *var = walk.GetObj();
		if(stricmp(var->m_name, name) == 0) {
			if(var->m_type == PV_BOOL || var->m_type == PV_NUM) {
				return *var->m_numValue;
			} else {
				return 0;
			}
		}
		walk.Next();
	}
	return 0;
}

void ProfileDB::SetValueByName(const char *name, sint32 value)
{
	PointerList<ProfileVar>::Walker walk(m_vars);
	while(walk.IsValid()) {
		ProfileVar *var = walk.GetObj();
		if(stricmp(var->m_name, name) == 0) {
			if(var->m_type == PV_BOOL || var->m_type == PV_NUM) {
				*var->m_numValue = value;
				return;
			} else {
				return;
			}
		}
		walk.Next();
	}
}


