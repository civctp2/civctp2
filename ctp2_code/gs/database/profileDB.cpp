//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : Handling of user preferences.
// Id           : $Id$
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
// - _DEBUG
//
//----------------------------------------------------------------------------
//
// Modifications from the original Activision code:
//
// - You need two changes here for a profile option
//
// - Option added to enable viewing info on actions that are too expensive.
// - Option added to close a messagebox automatically on eyepoint clicking.
// - Option added to choose a color set.
// - Option added to select which order buttons are displayed for an army.
// - Option added to select message adding style (top or bottom).
// - Option added to include multiple data directories.
// - Replaced old civilisation database by new one. (Aug 20th 2005 Martin Gühmann)
// - Initialized local variables. (Sep 9th 2005 Martin Gühmann)
// - Option added to select whether an army is selected or a city is selected,
//   if both options are available. (Oct 8th 2005 Martin Gühmann)
// - DebugSlic and GoodAnim are now part of the advance options. (Oct 16th 2005 Martin Gühmann)
// - Added option to avoid an end turn if there are cities with empty build 
//   queues. (Oct. 22nd 2005 Martin Gühmann)
// - Added option to allow end turn if the game runs in the background,
//   useful for automatic AI testing. (Oct. 22nd 2005 Martin Gühmann)
// - Options CityClick, EndTurnWithEmptyBuildQueues and RunInBackground
//   removed from advance options since they do not work. (May 21st 2006 Martin Gühmann)
// - Made automatic treaty ending an option.
// - Option added to select between square and smooth borders. (Feb 4th 2007 Martin Gühmann)
// - Added additional options, most to be implemented later
// - Implemented NRG - option to ccalculate energy ratio affecting production and demand
// - Added DebugAI option
// - Made the upgrade option show up in the debug version. (19-May-2007 Martin Gühmann)
// - Added debug pathing option for the city astar. (17-Jan-2008 Martin Gühmann)
//
//----------------------------------------------------------------------------

#include "c3.h"
#include "profileDB.h"

#include "AgreementData.h"      // k_EXPIRATION_NEVER
#include "c3errors.h"
#include "Civilisation.h"
#include "CivPaths.h"           // g_civPaths
#include "Diffcly.h"
#include "DifficultyRecord.h"   // g_theDifficultyDB
#include "Diplomacy_Log.h"
#include "GameSettings.h"       // g_theGameSettings
#include "Globals.h"
#include "player.h"             // g_player
#include "soundmanager.h"       // g_soundManager
#include "StrDB.h"              // g_theStringDB
#include "Token.h"

extern Diplomacy_Log *      g_theDiplomacyLog; 

namespace
{
sint32 const                AUDIO_VOLUME_DEFAULT        = 8;
sint32 const                PLAYER_COUNT_DEFAULT        = 3;
sint32 const                PLAYER_COUNT_MAX_DEFAULT    = 16;
sint32 const                SLIDER_MIDDLE               = 5;

sint32 const                USE_UNKNOWN                 = 0;
}

ProfileDB::ProfileDB()
:
    m_nPlayers                          (PLAYER_COUNT_DEFAULT),
    m_ai_on                             (FALSE),
    m_use_nice_start                    (FALSE),
    m_use_map_plugin                    (FALSE),
    m_use_ipx                           (FALSE),
    m_setupRadius                       (0),
    m_powerPoints                       (0),
    m_difficulty                        (0),
    m_risklevel                         (0),
    m_genocide                          (FALSE),
    m_trade                             (FALSE),
    m_simplecombat                      (FALSE),
    m_pollution                         (FALSE),
    m_lineofsight                       (FALSE),
    m_unitAnim                          (FALSE),
    m_goodAnim                          (FALSE),
    m_tradeAnim                         (FALSE),
    m_waterAnim                         (FALSE),
    m_libraryAnim                       (FALSE),
    m_wonderMovies                      (FALSE),
    m_bounceMessage                     (FALSE),
    m_messageAdvice                     (FALSE),
    m_tutorialAdvice                    (FALSE),
    m_enemyMoves                        (TRUE),
    m_revoltWarning                     (FALSE),
    m_enemyIntrude                      (FALSE),
    m_unitLostWarning                   (FALSE),
    m_tradeLostWarning                  (FALSE),
    m_cityLostWarning                   (FALSE),
    m_autocenter                        (TRUE),
    m_fullScreenMovies                  (FALSE),
    m_showCityInfluence                 (FALSE),
    m_invulnerableTrade                 (FALSE),
    m_fogOfWar                          (FALSE),
    m_startType                         (FALSE),
    m_autoSave                          (FALSE),
    m_playerNumber                      (1),
    m_civIndex                          (PLAYER_COUNT_MAX_DEFAULT),
    m_gender                            (GENDER_MALE),
    m_isSaved                           (FALSE),
    m_isScenario                        (FALSE),
    m_noHumansOnHost                    (FALSE),
    m_logPlayerStats                    (FALSE),
    m_sfxVolume                         (AUDIO_VOLUME_DEFAULT),
    m_voiceVolume                       (AUDIO_VOLUME_DEFAULT),
    m_musicVolume                       (AUDIO_VOLUME_DEFAULT),
    m_xWrap                             (TRUE),
    m_yWrap                             (FALSE),
    m_autoGroup                         (FALSE),
    m_autoDeselect                      (FALSE),
    m_autoSelectNext                    (FALSE),
    m_autoSelectFirstUnit               (FALSE),
    m_autoTurnCycle                     (FALSE),
    m_combatLog                         (FALSE),
    m_useLeftClick                      (FALSE),
    m_showZoomedCombat                  (FALSE),
    m_useFingerprinting                 (FALSE),
    m_useRedbookAudio                   (FALSE),
    m_requireCD                         (FALSE),
    m_protected                         (TRUE),
    m_tryWindowsResolution              (TRUE),
    m_useDirectXBlitter                 (TRUE),
    m_screenResWidth                    (640),
    m_screenResHeight                   (480),
    m_zoomedCombatAlways                (FALSE),
    m_attackEveryone                    (FALSE),
    m_nonRandomCivs                     (FALSE),
    m_autoEndMultiple                   (FALSE),
    m_wetdry                            (SLIDER_MIDDLE), 
    m_warmcold                          (SLIDER_MIDDLE), 
    m_oceanland                         (SLIDER_MIDDLE), 
    m_islandcontinent                   (SLIDER_MIDDLE),
    m_homodiverse                       (SLIDER_MIDDLE), 
    m_goodcount                         (SLIDER_MIDDLE),
    m_throneRoom                        (FALSE),
    m_max_players                       (PLAYER_COUNT_MAX_DEFAULT),
    m_mapSize                           (MAPSIZE_MEDIUM),
    m_alienEndGame                      (TRUE),
    m_allow_ai_settle_move_cheat        (FALSE),
    m_unitCompleteMessages              (FALSE),
    m_nonContinuousUnitCompleteMessages (FALSE),
    m_debugSlic                         (FALSE),
    m_debugSlicEvents                   (TRUE),
    m_dontKillMessages                  (FALSE),
    m_aiPopCheat                        (TRUE),
    m_showCityNames                     (TRUE),
    m_showArmyNames                     (FALSE),  //emod
    m_showTradeRoutes                   (TRUE),
    m_unitSpeed                         (1),
    m_mouseSpeed                        (SLIDER_MIDDLE),
    m_leftHandedMouse                   (FALSE),
    m_cityBuiltMessage                  (TRUE),
    m_useAttackMessages                 (FALSE),
    m_useOldRegisterClick               (FALSE),
    m_useCTP2Mode                       (TRUE),
    m_is_diplomacy_log_on               (FALSE),
    m_cheat_age                         (0),
    m_autoSwitchTabs                    (TRUE),
    m_showPoliticalBorders              (TRUE),
    m_moveHoldTime                      (500),
    m_battleSpeed                       (SLIDER_MIDDLE),
    m_showEnemyHealth                   (FALSE),
    m_scrollDelay                       (3),
    m_autoRenameCities                  (FALSE),
    m_autoOpenCityWindow                (TRUE),
    m_endTurnSound                      (TRUE),
    m_enableLogs                        (FALSE),
    m_displayUnits                      (TRUE),
    m_displayCities                     (TRUE),
    m_displayBorders                    (TRUE),
    m_displayFilter                     (TRUE),
    m_displayTrade                      (TRUE),
    m_displayTerrain                    (TRUE),
    m_forest                            (USE_UNKNOWN),
    m_grass                             (USE_UNKNOWN),
    m_plains                            (USE_UNKNOWN),
    m_desert                            (USE_UNKNOWN),
    m_whitePercent                      (USE_UNKNOWN),
    m_brownPercent                      (USE_UNKNOWN),
    m_temperatureRangeAdjust            (USE_UNKNOWN),
    m_land                              (USE_UNKNOWN),
    m_continent                         (USE_UNKNOWN),
    m_homogenous                        (USE_UNKNOWN),
    m_richness                          (USE_UNKNOWN),
    m_closeEyepoint                     (FALSE),
    m_colorSet                          (0),
    m_showExpensive                     (FALSE),
    m_showOrderUnion                    (FALSE),
    m_recentAtTop                       (FALSE),
    m_cityClick                         (FALSE),
    m_dontSave                          (FALSE),
    m_endTurnWithEmptyBuildQueues       (FALSE),
    m_runInBackground                   (FALSE),
    m_autoExpireTreatyTurn              (k_EXPIRATION_NEVER),
    m_cityCaptureOptions                (FALSE),
    m_upgrade                           (FALSE),
    m_smoothBorders                     (FALSE),
    m_CivFlags                          (FALSE),
    m_NoAIProductionDeficit             (FALSE),
    m_NoAIGoldDeficit                   (FALSE),
    m_AICityDefenderBonus               (FALSE),
    m_BarbarianCities                   (FALSE),
    m_SectarianHappiness                (FALSE),
    m_RevoltCasualties                  (FALSE),
    m_RevoltInsurgents                  (FALSE),
    m_BarbarianCamps                    (FALSE),
    m_BarbarianSpawnsBarbarian          (FALSE),
    m_AINoSinking                       (FALSE),
    m_AINoCityLimit                     (FALSE),
    m_GoldPerUnitSupport                (FALSE),
    m_GoldPerCity                       (FALSE),
    m_AINoShieldHunger                  (FALSE),
    m_AINoGoldHunger                    (FALSE),
    m_AIFreeUpgrade                     (TRUE),
    m_AIMilitiaUnit                     (FALSE),
    m_OneCityChallenge                  (FALSE),
    m_NRG                               (FALSE),
    m_debugai                           (FALSE),
    m_ruin                              (FALSE),
    m_NoCityLimit                       (FALSE),
    m_DebugCityAstar                    (FALSE),
    m_newcombat                         (FALSE),
    // Add above this line new profile options
    m_vars                              (new PointerList<ProfileVar>),
    m_loadedFromTutorial                (FALSE)
{
	for (size_t player = 0; player < k_MAX_PLAYERS; ++player) 
	{
		m_ai_personality[player][0] = 0;
	}

	m_gameName[0]           = 0;
	m_leaderName[0]         = 0;
	m_civName[0]            = 0;
	m_saveNote[0]           = 0;
	m_ruleSets[0]           = 0;
	m_gameWatchDirectory[0] = 0;
	
	for (size_t map_pass = 0; map_pass < k_NUM_MAP_PASSES; ++map_pass)
	{
		m_map_plugin_name[map_pass][0]  = 0;
		m_map_settings[map_pass]        = NULL;
	};

	Var("NumPlayers"                 , PV_NUM   , &m_nPlayers                   , NULL, false);
	Var("AiOn"                       , PV_BOOL  , &m_ai_on                      , NULL, false);
	Var("UseNiceStart"               , PV_BOOL  , &m_use_nice_start             , NULL, false); 
	Var("UseMapPlugin"               , PV_BOOL  , &m_use_map_plugin             , NULL, false);
	
	Var("Difficulty"                 , PV_NUM   , &m_difficulty                 , NULL, false);
	Var("RiskLevel"                  , PV_NUM   , &m_risklevel                  , NULL, false);

	Var("Pollution"                  , PV_BOOL  , &m_pollution                  , NULL, false);
	Var("UnitAnim"                   , PV_BOOL  , &m_unitAnim                   , NULL);

	Var("GoodAnim"                   , PV_BOOL  , &m_goodAnim                   , NULL);
	Var("TradeAnim"                  , PV_BOOL  , &m_tradeAnim                  , NULL);
	Var("WaterAnim"                  , PV_BOOL  , &m_waterAnim                  , NULL, false);
	Var("LibraryAnim"                , PV_BOOL  , &m_libraryAnim                , NULL, false);
	Var("WonderMovies"               , PV_BOOL  , &m_wonderMovies               , NULL);
	Var("BounceMessage"              , PV_BOOL  , &m_bounceMessage              , NULL, false);
	Var("MessageAdvice"              , PV_BOOL  , &m_messageAdvice              , NULL, false);
	Var("TutorialAdvice"             , PV_BOOL  , &m_tutorialAdvice             , NULL, false);
	Var("EnemyMoves"                 , PV_BOOL  , &m_enemyMoves                 , NULL);
	Var("RevoltWarning"              , PV_BOOL  , &m_revoltWarning              , NULL, false);
	Var("EnemyIntrude"               , PV_BOOL  , &m_enemyIntrude               , NULL, false);
	Var("UnitLostWarning"            , PV_BOOL  , &m_unitLostWarning            , NULL, false);
	Var("TradeLostWarning"           , PV_BOOL  , &m_tradeLostWarning           , NULL, false);
	Var("CityLostWarning"            , PV_BOOL  , &m_cityLostWarning            , NULL, false);
	Var("AutoCenter"                 , PV_BOOL  , &m_autocenter                 , NULL);
	Var("FullScreenMovies"           , PV_BOOL  , &m_fullScreenMovies           , NULL, false);
	Var("AutoSave"                   , PV_BOOL  , &m_autoSave, NULL);
	Var("PlayerNumber"               , PV_NUM   , (sint32 *)&m_playerNumber     , NULL, false);
	
	Var("CivIndex"                   , PV_NUM   , (sint32 *)&m_civIndex         , NULL, false);

	Var("GameName"                   , PV_STRING, NULL, (char *)m_gameName            , false);
	Var("LeaderName"                 , PV_STRING, NULL, (char*)m_leaderName           , false);
	Var("CivName"                    , PV_STRING, NULL, (char*)m_civName              , false);
	Var("SaveNote"                   , PV_STRING, NULL, (char*)m_saveNote             , false);
	Var("Gender"                     , PV_NUM   , (sint32 *)&m_gender           , NULL, false);

	Var("NoHumansOnHost"             , PV_BOOL  , &m_noHumansOnHost             , NULL, false);
	Var("LogPlayerStats"             , PV_BOOL  , &m_logPlayerStats             , NULL, false);

	Var("SfxVolume"                  , PV_NUM   , &m_sfxVolume                  , NULL, false);
	Var("VoiceVolume"                , PV_NUM   , &m_voiceVolume                , NULL, false);
	Var("MusicVolume"                , PV_NUM   , &m_musicVolume                , NULL, false);

	Var("XWrap"                      , PV_BOOL  , &m_xWrap                      , NULL, false);
	Var("YWrap"                      , PV_BOOL  , &m_yWrap                      , NULL, false);
	Var("AutoGroup"                  , PV_BOOL  , &m_autoGroup                  , NULL, false);
	Var("AutoDeselect"               , PV_BOOL  , &m_autoDeselect               , NULL);
	Var("AutoSelectNext"             , PV_BOOL  , &m_autoSelectNext             , NULL);
	Var("AutoSelectFirstUnit"        , PV_BOOL  , &m_autoSelectFirstUnit        , NULL);
	Var("AutoTurnCycle"              , PV_BOOL  , &m_autoTurnCycle              , NULL);
	Var("CombatLog"                  , PV_BOOL  , &m_combatLog                  , NULL, false);

	Var("UseLeftClick"               , PV_BOOL  , &m_useLeftClick               , NULL, false);
	Var("ShowZoomedCombat"           , PV_BOOL  , &m_showZoomedCombat           , NULL);
	Var("UseFingerPrinting"          , PV_BOOL  , &m_useFingerprinting          , NULL, false);
	Var("UseRedbookAudio"            , PV_BOOL  , &m_useRedbookAudio            , NULL, false);
	Var("RequireCD"                  , PV_BOOL  , &m_requireCD                  , NULL, false);
	Var("Prophylaxis"                , PV_BOOL  , &m_protected                  , NULL, false);
	Var("TryWindowsResolution"       , PV_BOOL  , &m_tryWindowsResolution       , NULL, false);
	Var("UseDirectXBlitter"          , PV_BOOL  , &m_useDirectXBlitter          , NULL, false);
	Var("ScreenResWidth"             , PV_NUM   , &m_screenResWidth             , NULL, false);
	Var("ScreenResHeight"            , PV_NUM   , &m_screenResHeight            , NULL, false);

	Var("ZoomedCombatAlways"         , PV_BOOL  , &m_zoomedCombatAlways         , NULL);
	Var("AttackEveryone"             , PV_BOOL  , &m_attackEveryone             , NULL, false);
	Var("NonRandomCivs"              , PV_BOOL  , &m_nonRandomCivs              , NULL, false);
	Var("GameWatchDirectory"         , PV_STRING, NULL, (char*)m_gameWatchDirectory   , false);
	Var("AutoEndMultiple"            , PV_BOOL  , &m_autoEndMultiple            , NULL);

	Var("WetDry"                     , PV_NUM   , &m_wetdry                     , NULL, false);
	Var("WarmCold"                   , PV_NUM   , &m_warmcold                   , NULL, false);
	Var("OceanLand"                  , PV_NUM   , &m_oceanland                  , NULL, false);
	Var("IslandContinent"            , PV_NUM   , &m_islandcontinent            , NULL, false);
	Var("HomoDiverse"                , PV_NUM   , &m_homodiverse                , NULL, false);
	Var("GoodCount"                  , PV_NUM   , &m_goodcount                  , NULL, false);
	
	Var("ThroneRoom"                 , PV_BOOL  , &m_throneRoom                 , NULL, false);
	Var("MaxPlayers"                 , PV_NUM   , &m_max_players                , NULL, false);
	Var("MapSize"                    , PV_NUM   , (sint32 *)&m_mapSize          , NULL, false);
	
	Var("AlienEndGame"               , PV_BOOL  , &m_alienEndGame               , NULL, false);
	Var("UnitCompleteMessages"       , PV_BOOL  , &m_unitCompleteMessages       , NULL);
	Var("NonContinuousUnitCompleteMessages", PV_BOOL  , &m_nonContinuousUnitCompleteMessages, NULL);
	Var("DebugSlic"                  , PV_BOOL  , &m_debugSlic                  , NULL);
	Var("DebugSlicEvents"            , PV_BOOL  , &m_debugSlicEvents            , NULL);
	Var("DiplomacyLog"               , PV_BOOL  , &m_is_diplomacy_log_on        , NULL, false);
	Var("CheatAge"                   , PV_NUM   , &m_cheat_age                  , NULL, false);
	Var("DontKillMessages"           , PV_BOOL  , &m_dontKillMessages           , NULL, false);
	Var("AIPopCheat"                 , PV_BOOL  , &m_aiPopCheat                 , NULL, false);
	Var("ShowCityNames"              , PV_BOOL  , &m_showCityNames              , NULL);
	Var("ShowArmyNames"              , PV_BOOL  , &m_showArmyNames              , NULL);  //emod
	Var("ShowTradeRoutes"            , PV_BOOL  , &m_showTradeRoutes            , NULL);

	Var("UnitSpeed"                  , PV_NUM   , &m_unitSpeed                  , NULL);
	Var("MouseSpeed"                 , PV_NUM   , &m_mouseSpeed                 , NULL);
	Var("LeftHandedMouse"            , PV_BOOL  , &m_leftHandedMouse            , NULL);
	
	Var("CityBuiltMessage"           , PV_BOOL  , &m_cityBuiltMessage           , NULL, false);
	Var("UseAttackMessages"          , PV_BOOL  , &m_useAttackMessages          , NULL, false);

	Var("MapPlugin0"                 , PV_STRING, NULL, (char *)m_map_plugin_name[0]  , false);
	Var("MapPlugin1"                 , PV_STRING, NULL, (char *)m_map_plugin_name[1]  , false);
	Var("MapPlugin2"                 , PV_STRING, NULL, (char *)m_map_plugin_name[2]  , false);
	Var("MapPlugin3"                 , PV_STRING, NULL, (char *)m_map_plugin_name[3]  , false);

	Var("OldRegisterClick"           , PV_BOOL  , &m_useOldRegisterClick        , NULL, false);
	Var("CTP2Mode"                   , PV_BOOL  , &m_useCTP2Mode                , NULL, false);
	Var("MoveHoldTime"               , PV_NUM   , &m_moveHoldTime               , NULL, false);

	Var("BattleSpeed"                , PV_NUM   , &m_battleSpeed                , NULL);

	Var("ScrollDelay"                , PV_NUM   , &m_scrollDelay                , NULL);

	Var("AutoSwitchTabs"             , PV_BOOL  , &m_autoSwitchTabs             , NULL);
	Var("AutoRenameCities"           , PV_BOOL  , &m_autoRenameCities           , NULL, false);
	Var("AutoOpenCityWindow"         , PV_BOOL  , &m_autoOpenCityWindow         , NULL);

	Var("ShowEnemyHealth"            , PV_BOOL  , &m_showEnemyHealth            , NULL); //used

	Var("ShowCityInfluence"          , PV_BOOL  , &m_showCityInfluence          , NULL);
	Var("ShowPoliticalBorders"       , PV_BOOL  , &m_showPoliticalBorders       , NULL);

	Var("EndTurnSound"               , PV_BOOL  , &m_endTurnSound               , NULL);
	Var("EnableLogs"                 , PV_BOOL  , &m_enableLogs                 , NULL, false);
	Var("DisplayUnits"               , PV_BOOL  , &m_displayUnits               , NULL, false);
	Var("DisplayCities"              , PV_BOOL  , &m_displayCities              , NULL, false);
	Var("DisplayBorders"             , PV_BOOL  , &m_displayBorders             , NULL, false);
	Var("DisplayFilter"              , PV_BOOL  , &m_displayFilter              , NULL, false);
	Var("DisplayTrade"               , PV_BOOL  , &m_displayTrade               , NULL, false);
	Var("DisplayTerrain"             , PV_BOOL  , &m_displayTerrain             , NULL, false);

	Var("CloseOnEyepoint"            , PV_BOOL  , &m_closeEyepoint              , NULL);
	Var("ShowExpensive"              , PV_BOOL  , &m_showExpensive              , NULL);
	Var("ColorSet"                   , PV_NUM   , &m_colorSet                   , NULL, false);
	Var("ShowOrderUnion"             , PV_BOOL  , &m_showOrderUnion             , NULL);
	Var("RecentAtTop"                , PV_BOOL  , &m_recentAtTop                , NULL);
	Var("RuleSets"                   , PV_STRING, NULL, m_ruleSets                    , false);
	Var("CityClick"                  , PV_BOOL  , &m_cityClick                  , NULL, false);
	Var("EndTurnWithEmptyBuildQueues", PV_BOOL  , &m_endTurnWithEmptyBuildQueues, NULL, false);
	Var("RunInBackground"            , PV_BOOL  , &m_runInBackground            , NULL, false);
	Var("AutoExpireTreatyBase"       , PV_NUM   , &m_autoExpireTreatyTurn       , NULL, false);
	Var("CityCaptureOptions"         , PV_BOOL  , &m_cityCaptureOptions         , NULL, false); //used emod2
#if defined(_DEBUG)
	/// @todo Move this to the scenario editor
	Var("Upgrade"                    , PV_BOOL  , &m_upgrade                    , NULL);
#else
	Var("Upgrade"                    , PV_BOOL  , &m_upgrade                    , NULL, false);
#endif
	Var("SmoothBorders"              , PV_BOOL  , &m_smoothBorders              , NULL);
	// emod new profile flags // Please make sure that only those show up which are used.
	Var("CivFlags"                   , PV_BOOL  , &m_CivFlags                   , NULL); //used
	Var("NoAIProductionDeficit"      , PV_BOOL  , &m_NoAIProductionDeficit      , NULL, false);
	Var("NoAIGoldDeficit"            , PV_BOOL  , &m_NoAIGoldDeficit            , NULL, false);
	Var("AICityDefenderBonus"        , PV_BOOL  , &m_AICityDefenderBonus        , NULL, false);
	Var("BarbarianCities"            , PV_BOOL  , &m_BarbarianCities            , NULL, false);
	Var("SectarianHappiness"         , PV_BOOL  , &m_SectarianHappiness         , NULL, false);
	Var("RevoltCasualties"           , PV_BOOL  , &m_RevoltCasualties           , NULL, false);
	Var("RevoltInsurgents"           , PV_BOOL  , &m_RevoltInsurgents           , NULL, false);
	Var("BarbarianCamps"             , PV_BOOL  , &m_BarbarianCamps	            , NULL, false);
	Var("BarbarianSpawnsBarbarian"   , PV_BOOL  , &m_BarbarianSpawnsBarbarian   , NULL, false);
	Var("AINoSinking"                , PV_BOOL  , &m_AINoSinking                , NULL, false);
	Var("AINoCityLimit"              , PV_BOOL  , &m_AINoCityLimit              , NULL, false);
	Var("GoldPerUnitSupport"         , PV_BOOL  , &m_GoldPerUnitSupport         , NULL, false);
	Var("GoldPerCity"                , PV_BOOL  , &m_GoldPerCity                , NULL, false);
	Var("AINoShieldHunger"           , PV_BOOL  , &m_AINoShieldHunger           , NULL, false);
	Var("AINoGoldHunger"             , PV_BOOL  , &m_AINoGoldHunger             , NULL, false);
	Var("AIFreeUpgrade"              , PV_BOOL  , &m_AIFreeUpgrade              , NULL, false);
	Var("AIMilitiaUnit"              , PV_BOOL  , &m_AIMilitiaUnit              , NULL, false);
	Var("OneCityChallenge"           , PV_BOOL  , &m_OneCityChallenge           , NULL, false); //used
	Var("EnergySupply&DemandRatio"   , PV_BOOL  , &m_NRG                        , NULL, false); //used
	Var("ShowDebugAI"                , PV_BOOL  , &m_debugai                    , NULL);        //used //emod2 // Should go into the scenario editor
	Var("CitiesLeaveRuins"           , PV_BOOL  , &m_ruin                       , NULL, false); //used
	Var("NoCityLimit"                , PV_BOOL  , &m_NoCityLimit                , NULL, false);
	Var("DebugCityAstar"             , PV_BOOL  , &m_DebugCityAstar             , NULL);
	Var("NewCombat"                  , PV_BOOL  , &m_newcombat                  , NULL);
}

void ProfileDB::DefaultSettings(void)
{
	StringId    leaderNameId = g_theCivilisationDB->Get(m_civIndex)->GetLeaderNameMale();
	StringId    civNameId = g_theCivilisationDB->Get(m_civIndex)->GetPluralCivName();

	strcpy(m_leaderName, g_theStringDB->GetNameStr(leaderNameId));
	strcpy(m_civName, g_theStringDB->GetNameStr(civNameId));
}

ProfileDB::~ProfileDB()
{
	Save();

	if (m_vars) 
	{
		m_vars->DeleteAll();
		delete m_vars;
	}
}

BOOL ProfileDB::Init(BOOL forTutorial)
{
	MBCHAR profileName[_MAX_PATH];
	MBCHAR *profileTxtFile;

	if (forTutorial) 
	{
		m_loadedFromTutorial = TRUE;
		profileTxtFile = g_civPaths->FindFile(C3DIR_GAMEDATA, 
		                                      "tut_profile.txt", profileName);
	}
	else
	{
		profileTxtFile = g_civPaths->FindFile(C3DIR_DIRECT, "userprofile.txt",
		                                      profileName);
		if (!profileTxtFile || !c3files_PathIsValid(profileTxtFile)) 
		{
			profileTxtFile = g_civPaths->FindFile(C3DIR_GAMEDATA, 
			                                      "profile.txt", profileName);
		}
	}
	
	if (profileTxtFile) 
	{
		FILE * pro_file = c3files_fopen(C3DIR_DIRECT, profileTxtFile, "r");
			
		if (pro_file) 
		{
			sint32 const    saved_width     = m_screenResWidth;
			sint32 const    saved_height    = m_screenResHeight;
			BOOL const      res             = Parse(pro_file);
			fclose(pro_file);

			if (res) 
			{
				Save();
			}

			if (forTutorial) 
			{
				m_screenResWidth = saved_width;
				m_screenResHeight = saved_height;
			}
			
			return res;
		}
	}
	else 
	{
		m_nPlayers  = PLAYER_COUNT_DEFAULT; 
		m_ai_on     = FALSE;
	} 

	return FALSE;
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

		char *  name  = line;
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

		char *  value = name + 1;
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
	if (b == m_is_diplomacy_log_on)
	{
		// No action: keep current log status
	}
	else
	{
		delete g_theDiplomacyLog;
		g_theDiplomacyLog       = b ? new Diplomacy_Log : NULL;
		m_is_diplomacy_log_on   = b;
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
	if(x >= 0 && x < 7)
	{
		m_difficulty = x;
		if (g_player)
		{
			for (sint32 p = 0; p < k_MAX_PLAYERS; p++)
			{
				if (g_player[p])
				{
					delete g_player[p]->m_difficulty;
					g_player[p]->m_difficulty = 
					    new Difficulty(x,
					                   p,
					                   !g_player[p]->IsRobot()
					                  );
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

	FILE *file = c3files_fopen(C3DIR_DIRECT, "userprofile.txt", "w");
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

sint32 ProfileDB::GetValueByName(const char * name) const
{
	for
	(
	    PointerList<ProfileVar>::Walker walk(m_vars);
	    walk.IsValid();
	    walk.Next()
	)
	{
		ProfileVar *    var = walk.GetObj();
		if (stricmp(var->m_name, name) == 0)
		{
			if (var->m_type == PV_BOOL || var->m_type == PV_NUM)
			{
				return *var->m_numValue;
			}
			else
			{
				// This function only works for boolean or integer values
				return 0;
			}
		}
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
