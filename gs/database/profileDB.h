
#pragma once

#ifndef __PROFILE_DB_H__
#define __PROFILE_DB_H__ 1

#include "MapPoint.h"
#include "dbtypes.h"
#include "gstypes.h"

class Token;

class MapPoint; 

class CivArchive ;

#define k_MAX_PERSONALITY_LEN 20

enum CIV_INDEX;
enum GENDER;
enum MAPSIZE;

#define k_NUM_MAP_PASSES 4
#define k_NUM_MAP_SETTINGS 2

template <class T> class SimpleDynamicArray;
template <class T> class PointerList;
enum PROF_VAR_TYPE {
	PV_BOOL,
	PV_NUM,
	PV_STRING
};

class ProfileVar {
public:
	ProfileVar(char *name, PROF_VAR_TYPE type, sint32 *numValue,
			   char *stringValue, bool visible) {
		m_name = name;
		m_type = type;
		m_numValue = numValue;
		m_stringValue = stringValue;
		m_visible = visible;
	}
	
	char *m_name;
	PROF_VAR_TYPE m_type;
	sint32 *m_numValue;
	char *m_stringValue;
	bool m_visible;
};

class ProfileDB { 
	
    
	
	
	sint32 m_nPlayers; 

	sint32 m_ai_on; 
    sint32 m_use_nice_start; 
	sint32 m_use_map_plugin;
	sint32 m_use_ipx;
	
	sint32 m_setupRadius;
	sint32 m_powerPoints;
	sint32 m_difficulty;
	sint32 m_risklevel;
	
	sint32	m_genocide,
			m_trade,
			m_simplecombat,		
			m_pollution,
			m_lineofsight;

	sint32	m_unitAnim,	

			m_goodAnim,
			m_tradeAnim,

			m_waterAnim,		
			m_libraryAnim,

			m_wonderMovies,		
			m_bounceMessage,	
			m_messageAdvice,	
			m_tutorialAdvice,	
			m_enemyMoves,
			m_revoltWarning,	
			m_enemyIntrude,		
			m_unitLostWarning,	
			m_tradeLostWarning,	
			m_cityLostWarning,	
            m_autocenter;

	sint32	m_fullScreenMovies;	
	sint32	m_showCityInfluence;

	sint32	m_invulnerableTrade,			
			m_fogOfWar,						
			m_startType;					

	sint32	m_autoSave;						

	PLAYER_INDEX	m_playerNumber;			
	
	CIV_INDEX		m_civIndex;				

	MBCHAR	m_gameName[_MAX_PATH];			
	MBCHAR	m_leaderName[k_MAX_NAME_LEN];	
	MBCHAR	m_civName[k_MAX_NAME_LEN];		
	MBCHAR	m_saveNote[_MAX_PATH];			
	GENDER	m_gender;						

	sint32	m_isSaved;						
	
	sint32	m_isScenario;

	sint32    m_noHumansOnHost;
	
	sint32	m_logPlayerStats;

	
	sint32	m_sfxVolume;
	sint32	m_voiceVolume;
	sint32	m_musicVolume;

	sint32    m_xWrap;
	sint32    m_yWrap;
	sint32    m_autoGroup;
	sint32    m_autoDeselect;
	sint32    m_autoSelectNext;
	sint32    m_autoSelectFirstUnit;
	sint32    m_autoTurnCycle;
	sint32    m_combatLog;

	sint32	m_useLeftClick;

	
	sint32	m_showZoomedCombat;
	sint32	m_useFingerprinting;
	sint32	m_useRedbookAudio;
	sint32	m_requireCD;
	sint32	m_protected;
	sint32	m_tryWindowsResolution;
	sint32	m_useDirectXBlitter;
	sint32	m_screenResWidth;
	sint32	m_screenResHeight;

	sint32	m_zoomedCombatAlways;

	sint32    m_attackEveryone;
	sint32    m_nonRandomCivs;

	MBCHAR	m_gameWatchDirectory[_MAX_PATH];

	sint32    m_autoEndMultiple;

	sint32 m_wetdry, m_warmcold, m_oceanland, m_islandcontinent, 
		   m_homodiverse, m_goodcount;
	sint32   m_throneRoom;

	sint32 m_max_players;

	MAPSIZE	m_mapSize;

	sint32 m_alienEndGame;

	sint32 m_allow_ai_settle_move_cheat;

	sint32 m_unitCompleteMessages;
	sint32 m_nonContinuousUnitCompleteMessages;

	sint32 m_debugSlic;

	sint32 m_dontKillMessages;
	sint32 m_aiPopCheat;

	sint32 m_showCityNames;
	sint32 m_showTradeRoutes;

	sint32 m_unitSpeed;		
	sint32 m_mouseSpeed;

	sint32 m_leftHandedMouse;	

	sint32 m_cityBuiltMessage;
	sint32 m_useAttackMessages;

	sint32 m_useOldRegisterClick;
	sint32 m_useCTP2Mode;

	SimpleDynamicArray<double> *m_map_settings[k_NUM_MAP_PASSES];
	char m_ai_personality[k_MAX_PLAYERS][k_MAX_PERSONALITY_LEN+1];
	char m_map_plugin_name[k_NUM_MAP_PASSES][MAX_PATH];
    sint32 m_is_diplomacy_log_on;
    sint32 m_cheat_age; 

	sint32 m_autoSwitchTabs;
	sint32 m_showPoliticalBorders;
	sint32 m_moveHoldTime;

	sint32 m_battleSpeed;

	sint32 m_showEnemyHealth;
	sint32 m_scrollDelay;

	sint32 m_autoRenameCities;
	sint32 m_autoOpenCityWindow;
	
	sint32 m_endTurnSound;

	sint32 m_enableLogs;

	sint32 m_displayUnits;
	sint32 m_displayCities;
	sint32 m_displayBorders;
	sint32 m_displayFilter;
	sint32 m_displayTrade;
	sint32 m_displayTerrain;
	
	

	
	
	

	
	

	
	
	sint32 m_forest;
	sint32 m_grass;
	sint32 m_plains;
	sint32 m_desert;
	sint32 m_whitePercent;
	sint32 m_brownPercent;
	sint32 m_temperatureRangeAdjust;
	double m_land;
	double m_continent;
	double m_homogenous;
	sint32 m_richness;
	

	
	PointerList<ProfileVar> *m_vars;
	BOOL m_loadedFromTutorial;
	BOOL m_dontSave;

public: 
    ProfileDB();
    ~ProfileDB();
    BOOL Init(BOOL forTutorial);

	void DefaultSettings();

	PointerList<ProfileVar> *GetVars() { return m_vars; }

    BOOL IsAIOn() const { return m_ai_on; } 
	void SetAI(BOOL on) { m_ai_on = on; }

    BOOL IsUseNiceStart() const { return m_use_nice_start; } 
	BOOL UseMapPlugin() const { return m_use_map_plugin; }
	const char *MapPluginName(sint32 pass) const { return m_map_plugin_name[pass]; }
	BOOL UseIPX() const { return m_use_ipx; }
	sint32 SetupRadius() const { return m_setupRadius; }
	sint32 PowerPoints() const { return m_powerPoints; }

	const char *GetPersonality(sint32 m_ai_num);

	BOOL Parse(FILE *file);

	void Var(char *name, PROF_VAR_TYPE type, sint32 *numValue,
			 char *stringValue, bool visible = true);
	void Save();

	sint32 GetValueByName(const char *name);
	void SetValueByName(const char *name, sint32 value);

	

	
	
	
	void SetNPlayers(uint32 n)					{ Assert((n>2) && (n<17)); m_nPlayers = n; }
	
	void SetLeaderName(MBCHAR *name)			{ strcpy(m_leaderName, name); }
	void SetCivName(MBCHAR *name)				{ strcpy(m_civName, name); }
	void SetSaveNote(MBCHAR *note)				{ strcpy(m_saveNote, note); }
	void SetIsSaved(BOOL isSaved)				{ m_isSaved = isSaved; }
	void SetGameName(MBCHAR *name)				{ strcpy(m_gameName, name); }
	
	void SetCivIndex(CIV_INDEX civ)				{ m_civIndex = civ; }
	void SetPlayerIndex(PLAYER_INDEX index)		{ m_playerNumber = index; }
	void SetGender(GENDER gender)				{ m_gender = gender; }


	void SetMapSize(MAPSIZE size)				{ m_mapSize = size; }

	void SetDifficulty(uint32 x);
	void SetRiskLevel(uint32 x)					{ Assert((x>=0) && (x<5)); m_risklevel = x; }
	void SetPowerPoints(sint32 powerPoints)		{ m_powerPoints = powerPoints; }

	void SetGenocideRule(BOOL rule)				{ m_genocide = rule; }
	void SetTradeRule(BOOL rule)				{ m_trade = rule; }
	void SetSimpleCombatRule(BOOL rule)			{ m_simplecombat = rule; }
	void SetPollutionRule(BOOL rule);
	void SetLineOfSightRule(BOOL rule)			{ m_lineofsight = rule; }

	void SetUnitAnim(BOOL val)					{ m_unitAnim = val; }
	void SetGoodAnim(BOOL val)					{ m_goodAnim = val; }

	void SetTradeAnim(BOOL val)					{ m_tradeAnim = val; }

	void SetLibraryAnim(BOOL val)				{ m_libraryAnim = val; }
	void SetWonderMovies(BOOL val)				{ m_wonderMovies = val; }
	void SetBounceMessage(BOOL val)				{ m_bounceMessage = val; }
	void SetMessageAdvice(BOOL val)				{ m_messageAdvice = val; }

	void SetTutorialAdvice(BOOL val);
	void ProfileDB::SetEnemyMoves(BOOL val)		{ m_enemyMoves = val; }
	void SetRevoltWarning(BOOL val)				{ m_revoltWarning = val; }
	void SetEnemyIntrude(BOOL val)				{ m_enemyIntrude = val; }
	void SetUnitLostWarning(BOOL val)			{ m_unitLostWarning = val; }
	void SetTradeLostWarning(BOOL val)			{ m_tradeLostWarning = val; }
	void SetCityLostWarning(BOOL val)			{ m_cityLostWarning = val; }
    void SetAutoCenter(BOOL val)          { m_autocenter = val; } 

	void SetInvulnerableTrade(BOOL val)			{ m_invulnerableTrade = val; }
	void SetFogOfWar(BOOL val)					{ m_fogOfWar = val; }
	void SetStartType(BOOL val)					{ m_startType = val; }

	void SetFullScreenMovies(BOOL val)			{ m_fullScreenMovies = val; }
	void SetShowCityInfluence(BOOL val)			{ m_showCityInfluence = val; }
	void SetIsScenario(BOOL val)				{ m_isScenario = val; }

	void SetAutoSave(BOOL on)					{ m_autoSave = on; }
	void SetXWrap(BOOL on)                      { m_xWrap = on; }
	void SetYWrap(BOOL on)                      { m_yWrap = on; }
	void SetAutoGroup(BOOL on)                  { m_autoGroup = on; }
	void SetAutoDeselect(BOOL on)               { m_autoDeselect = on; }
	void SetAutoSelectNext(BOOL on)             { m_autoSelectNext = on; }

	void SetSFXVolume(sint32 vol);				
	void SetVoiceVolume(sint32 vol);			
	void SetMusicVolume(sint32 vol);			
	void SetCombatLog(BOOL on)                  { m_combatLog = on; }

	void SetUseLeftClick( BOOL on )				{ m_useLeftClick = on; }

	void SetShowZoomedCombat(BOOL show)			{ m_showZoomedCombat = show; }
	void SetUseFingerprinting(BOOL use)			{ m_useFingerprinting = use; }
	void SetUseRedbookAudio(BOOL use)			{ m_useRedbookAudio = use; }
	void SetRequireCD(BOOL require)				{ m_requireCD = require; }
	void SetProtected(BOOL pro)					{ m_protected = pro; }
	void SetTryWindowsResolution(BOOL tryIt)	{ m_tryWindowsResolution = tryIt; }
	void SetUseDirectXBlitter(BOOL tryIt)		{ m_useDirectXBlitter = tryIt; }
	void SetScreenResWidth(sint32 width)		{ m_screenResWidth = width; }
	void SetScreenResHeight(sint32 height)		{ m_screenResHeight = height; }

	void SetAttackEveryone(BOOL on)             { m_attackEveryone = on; }
	void SetAutoSelectFirstUnit(BOOL on)        { m_autoSelectFirstUnit = on; }
	void SetGameWatchDirectory(MBCHAR *dir)		{ strcpy(m_gameWatchDirectory, dir); }
	void SetAutoTurnCycle(BOOL on)              { m_autoTurnCycle = on; }
	void SetNonRandomCivs(BOOL on)              { m_nonRandomCivs = on; }

	void SetZoomedCombatAlways(BOOL on)			{ m_zoomedCombatAlways = on; }
	void SetAutoEndMultiple(BOOL on)            { m_autoEndMultiple = on; }
 
	void SetWetDry(sint32 x)                    { m_wetdry = x; }
	void SetWarmCold(sint32 x)                  { m_warmcold = x; }
	void SetOceanLand(sint32 x)                 { m_oceanland = x; }
	void SetIslandContinent(sint32 x)           { m_islandcontinent = x; }
	void SetHomoDiverse(sint32 x)               { m_homodiverse = x; }
	void SetGoodCount(sint32 x)                 { m_goodcount = x; }

	void SetThroneRoom(BOOL throneRoom)         { m_throneRoom = throneRoom; }

	void SetMaxPlayers(sint32 max)              { m_max_players = max; }
	void SetAlienEndGame(BOOL on)               { m_alienEndGame = on; }
	void SetAllowAISettleMoveCheat(BOOL on)     { m_allow_ai_settle_move_cheat = on; }
	void SetAllUnitCompleteMessages(BOOL on)     { m_unitCompleteMessages = on; }
	void SetNonContinuousUnitCompleteMessages(BOOL on) { m_nonContinuousUnitCompleteMessages = on; }

	void SetShowCityNames(BOOL on) { m_showCityNames = on; }
	void SetShowTradeRoutes(BOOL on) { m_showTradeRoutes = on; }

	void SetMoveHoldTime(sint32 t) { m_moveHoldTime = t; }

	
	void SetUnitSpeed(sint32 speed) { m_unitSpeed = speed; }
	void SetMouseSpeed(sint32 speed) { m_mouseSpeed = speed; }

	
	void SetLeftHandedMouse(BOOL lefty) { m_leftHandedMouse = lefty; }

	void SetCityBuiltMessage(BOOL on) { m_cityBuiltMessage = on; }

	
	void SetDontSave(BOOL on) { m_dontSave = on; }

	void SetAutoSwitchTabs(BOOL on) { m_autoSwitchTabs = on; }

	void SetShowPoliticalBorders(BOOL on) { m_showPoliticalBorders = on; }

	void SetEndTurnSound(BOOL on) { m_endTurnSound = on;}

	void SetDisplayUnits(BOOL on) { m_displayUnits = on;}
	void SetDisplayCities(BOOL on) { m_displayCities = on;}
	void SetDisplayBorders(BOOL on) { m_displayBorders = on;}
	void SetDisplayFilter(BOOL on) { m_displayFilter = on;}
	void SetDisplayTrade(BOOL on) { m_displayTrade = on;}
	void SetDisplayTerrain(BOOL on) { m_displayTerrain = on;}
	
	
	
    sint32		GetNPlayers() const				{ return m_nPlayers; }

	MAPSIZE		GetMapSize() const				{ return m_mapSize; } 
	sint32		GetDifficulty() const			{ return m_difficulty; }
	sint32		GetRiskLevel() const			{ return m_risklevel; }

	MBCHAR		*GetLeaderName()				{ return m_leaderName; }
	MBCHAR		*GetCivName()					{ return m_civName; }
	MBCHAR		*GetSaveNote()					{ return m_saveNote; }
	BOOL		IsSaved() const					{ return m_isSaved; }
	MBCHAR		*GetGameName()					{ return m_gameName; }
	CIV_INDEX		GetCivIndex() const				{ return m_civIndex; }
	
	PLAYER_INDEX	GetPlayerIndex() const		{ return m_playerNumber; }
	GENDER		GetGender() const				{ return m_gender; }

	sint32		GetSFXVolume() const			{ return m_sfxVolume; }
	sint32		GetVoiceVolume() const			{ return m_voiceVolume; }
	sint32		GetMusicVolume() const			{ return m_musicVolume; }

	BOOL		IsGenocideRule() const			{ return m_genocide; }
	BOOL		IsTradeRule() const				{ return m_trade; }
	BOOL		IsSimpleCombatRule() const		{ return m_simplecombat; }
	BOOL		IsPollutionRule() const			{ return m_pollution; }
	BOOL		IsLineOfSightRule() const		{ return m_lineofsight; }

	BOOL		IsUnitAnim() const				{ return m_unitAnim; }
	BOOL		IsGoodAnim() const				{ return m_goodAnim; }

	BOOL		IsTradeAnim() const				{ return m_tradeAnim; }
	BOOL		IsWaterAnim() const				{ return m_waterAnim; }
	BOOL		IsLibraryAnim() const			{ return m_libraryAnim; }
	BOOL		IsWonderMovies() const			{ return m_wonderMovies; }
	BOOL		IsBounceMessage() const			{ return m_bounceMessage; }
	BOOL		IsMessageAdvice() const			{ return m_messageAdvice; }
	BOOL		IsTutorialAdvice() const		{ return m_tutorialAdvice; }
	BOOL		IsEnemyMoves() const			{ return m_enemyMoves; }
	BOOL		IsRevoltWarning() const			{ return m_revoltWarning; }
	BOOL		IsEnemyIntrude() const			{ return m_enemyIntrude; }
	BOOL		IsUnitLostWarning() const		{ return m_unitLostWarning; }
	BOOL		IsTradeLostWarning() const		{ return m_tradeLostWarning; }
	BOOL		IsCityLostWarning() const		{ return m_cityLostWarning; }
    BOOL        IsAutoCenter()const             { return m_autocenter; } 

	BOOL		IsInvulnerableTrade(void) const	{ return m_invulnerableTrade; }
	BOOL		IsFogOfWar(void) const			{ return m_fogOfWar; }
	BOOL		IsStartType(void) const			{ return m_startType; }

	BOOL		IsFullScreenMovies(void) const	{ return m_fullScreenMovies; }
	BOOL		IsShowCityInfluence(void) const	{ return m_showCityInfluence; }

    void SetCheatAge(const sint32 age) 
    {
        m_cheat_age = age; 
    } 

    BOOL        GetCheatAge(sint32 &age)        
    {           
#ifdef _DEBUG 
        age = m_cheat_age; 
        return 0 < m_cheat_age; 
#else 
        age = 0; 
        return FALSE; 
#endif
    }
    
	BOOL		IsScenario() const				{ return m_isScenario; }

	BOOL		IsAutoSave() const				{ return m_autoSave;}
	BOOL        NoHumanPlayersOnHost() const    { return m_noHumansOnHost; }

	BOOL		LogPlayerStats() const			{ return m_logPlayerStats; }

	BOOL        IsYWrap() const                 { return m_yWrap; }
	BOOL        IsXWrap() const                 { return m_xWrap; }
	BOOL        IsAutoGroup() const             { return m_autoGroup; }
	BOOL        IsAutoDeselect() const          { return m_autoDeselect; }
	BOOL        IsAutoSelectNext() const        { return m_autoSelectNext; }
	BOOL        IsCombatLog() const             { return m_combatLog; }

	BOOL		IsUseLeftClick() const			{ return m_useLeftClick; }

	BOOL		IsShowZoomedCombat() const		{ return m_showZoomedCombat; }
	BOOL		IsUseFingerprinting() const		{ return m_useFingerprinting; }
	BOOL		IsUseRedbookAudio() const		{ return m_useRedbookAudio; }

#ifdef _BFR_
	BOOL		IsRequireCD() const				{ return TRUE; }
#else
	BOOL		IsRequireCD() const				{ return m_requireCD; }
#endif

#ifdef _BFR_
	BOOL		IsProtected() const				{ return TRUE;}
#else
	BOOL		IsProtected() const				{ return m_protected;}
#endif

	BOOL		IsTryWindowsResolution() const	{ return m_tryWindowsResolution; }
	BOOL		IsUseDirectXBlitter() const		{ return m_useDirectXBlitter; }
	sint32		GetScreenResWidth() const		{ return m_screenResWidth; }
	sint32		GetScreenResHeight() const		{ return m_screenResHeight; }

	BOOL        IsAttackEveryone() const        { return m_attackEveryone; }
	BOOL        IsAutoSelectFirstUnit() const   { return m_autoSelectFirstUnit; }
	BOOL        IsAutoTurnCycle() const         { return m_autoTurnCycle; }
	BOOL        IsNonRandomCivs() const         { return m_nonRandomCivs; }
	BOOL		IsZoomedCombatAlways() const	{ return m_zoomedCombatAlways; }
	BOOL        IsAutoEndMulitpleTurns() const  { return m_autoEndMultiple; }
	const double     *GetMapSettings(sint32 pass, sint32 &count);

	sint32		GetWorldShape( void )			{ return m_yWrap && m_xWrap; }

	MBCHAR		*GetGameWatchDirectory(void)	{ return m_gameWatchDirectory; }

	sint32      GetWetDry() { return m_wetdry; }
	sint32      GetWarmCold() { return m_warmcold; }
	sint32      GetOceanLand() { return m_oceanland; }
	sint32      GetIslandContinent() { return m_islandcontinent; }
	sint32      GetHomoDiverse() { return m_homodiverse; }
	sint32      GetGoodCount() { return m_goodcount; }

	BOOL        IsThroneRoom() { return m_throneRoom; }

	sint32      GetMaxPlayers() { return m_max_players; }

	BOOL        IsAlienEndGameOn() const { return m_alienEndGame; }
	BOOL        AllowAISettleMoveCheat() const { return m_allow_ai_settle_move_cheat; }
    BOOL        IsDiplomacyLogOn() const { return m_is_diplomacy_log_on; } 
    void        SetDiplmacyLog(BOOL b); 
	BOOL        IsAllUnitCompleteMessages() const { return m_unitCompleteMessages; }
	BOOL        IsNonContinuousUnitCompleteMessages() const { return m_nonContinuousUnitCompleteMessages; }

	BOOL        IsDebugSlic() const { return m_debugSlic; }

	BOOL        DontKillMessages() const { return m_dontKillMessages; }
	BOOL        AIPopCheat() const { return m_aiPopCheat; }

	BOOL        GetShowCityNames() { return m_showCityNames; }
	BOOL        GetShowTradeRoutes() { return m_showTradeRoutes; }

	
	sint32		GetUnitSpeed(void) { return m_unitSpeed; }
	sint32		GetMouseSpeed(void) { return m_mouseSpeed; }

	
	sint32		GetLeftHandedMouse(void) { return m_leftHandedMouse; }

	sint32      GetCityBuiltMessage(void) { return m_cityBuiltMessage; }

	BOOL        GetUseAttackMessages(void) { return m_useAttackMessages; }
	BOOL        GetUseOldRegisterClick(void) { return m_useOldRegisterClick; }
	BOOL        IsUseCTP2Mode(void) { return m_useCTP2Mode; }

	sint32      GetMoveHoldTime() { return m_moveHoldTime; }

	sint32      GetBattleSpeed() { return m_battleSpeed; }

	BOOL GetAutoSwitchTabs() { return m_autoSwitchTabs; }
	BOOL GetShowPoliticalBorders() { return m_showPoliticalBorders; }

	sint32 GetScrollDelay() { return m_scrollDelay; }
	BOOL GetShowEnemyHealth() { return m_showEnemyHealth; }

	BOOL GetAutoRenameCities() { return m_autoRenameCities; }
	BOOL GetAutoOpenCityWindow() { return m_autoOpenCityWindow; }
	BOOL GetEndTurnSound() { return m_endTurnSound; }

	BOOL GetDisplayUnits() { return m_displayUnits; }
	BOOL GetDisplayCities() { return m_displayCities; }
	BOOL GetDisplayBorders() { return m_displayBorders; }
	BOOL GetDisplayFilter() { return m_displayFilter; }
	BOOL GetDisplayTrade() { return m_displayTrade; }
	BOOL GetDisplayTerrain() { return m_displayTerrain; }
	BOOL GetEnableLogs() { return m_enableLogs; }

	void SetPercentForest(sint32 forest) { m_forest = forest; }
	void SetPercentGrass(sint32 grass) { m_grass = grass; }
	void SetPercentPlains(sint32 plains) { m_plains = plains; }
	void SetPercentDesert(sint32 desert) { m_desert = desert; }
	void SetPercentWhite(sint32 white) { m_whitePercent = white; }
	void SetPercentBrown(sint32 brown) { m_brownPercent = brown; }
	void SetTemperatureRangeAdjust(sint32 adjust) { m_temperatureRangeAdjust = adjust; }
	void SetPercentLand(double land) { m_land = land; }
	void SetPercentContinent(double continent) { m_continent = continent; }
	void SetHomogenous(sint32 homogenous) { m_homogenous = double(homogenous)/100.0f; }
	void SetPercentRichness(sint32 richness) { m_richness = richness; }
	
	sint32 PercentForest() { return m_forest; }
	sint32 PercentGrass() { return m_grass; }
	sint32 PercentPlains() { return m_plains; }
	sint32 PercentDesert() { return m_desert; }
	sint32 PercentWhite() { return m_whitePercent; }
	sint32 PercentBrown() { return m_brownPercent; }
	sint32 TemperatureRangeAdjust() { return m_temperatureRangeAdjust; }
	double PercentLand() { return m_land; }
	double PercentContinent() { return m_continent; }
	double PercentHomogenous() { return m_homogenous; }
	sint32 PercentRichness() { return m_richness; }
	
};

extern ProfileDB *g_theProfileDB;

#endif __PROFILE_DB_H__
