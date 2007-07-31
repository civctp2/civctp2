#pragma once

#ifndef __CIVAPP_H__
#define __CIVAPP_H__

#include "aui_ldl.h"
#include "aui_action.h"
#include "player.h"

class CivArchive;

class CivApp {
public:
	CivApp();
	~CivApp();

	
	sint32		InitializeAppUI(void);
	sint32		InitializeAppDB(CivArchive &archive);
	sint32		InitializeApp(HINSTANCE hInstance, int iCmdShow);
	sint32		QuickInit(HINSTANCE hInstance, int iCmdShow);
	sint32		CleanupAppUI(void);
	sint32		CleanupAppDB(void);
	sint32		CleanupApp(void);

	
	sint32		InitializeGameUI(void);
	sint32		InitializeGame(CivArchive &archive);
	sint32		InitializeSpriteEditor(CivArchive &archive);
	sint32		CleanupGameUI(void);
	sint32		CleanupGame(bool keepScenInfo);
	sint32      StartMessageSystem();

	void		ProcessGraphicsCallback(void);

	void		BeginKeyboardScrolling(sint32 key);
	void		StopKeyboardScrolling(sint32 key);

	BOOL		IsKeyboardScrolling(void) { return m_isKeyboardScrolling; }
	sint32		GetKeyboardScrollingKey(void) { return m_keyboardScrollingKey; }

	
	sint32      ProcessNet(const uint32 target_milliseconds, uint32 &used_milliseconds);
	sint32      ProcessAI();
	sint32      ProcessRobot(const uint32 target_milliseconds, uint32 &used_milliseconds);
	sint32		ProcessUI(const uint32 target_milliseconds, uint32 &used_milliseconds);
	sint32      ProcessSLIC(void);

	sint32		ProcessProfile(void);
	sint32		Process(void);

	
	sint32		StartSpriteEditor(void);
	sint32		StartGame(void);
	sint32		EndGame(void);

	sint32		LoadSavedGame(MBCHAR *name);
	sint32		LoadSavedGameMap(MBCHAR *name); 
	sint32		LoadScenarioGame(MBCHAR *name);
	sint32		RestartGame(void);

	
	sint32		RestartGameSameMap(void);
	sint32		QuitToSPShell(void);
	sint32		QuitToLobby(void);

	sint32		QuitGame(void);

	void		AutoSave(PLAYER_INDEX player, bool isQuickSave = false);
	void		RestoreAutoSave(PLAYER_INDEX player);

	
	BOOL		SaveDBInGameFile(void) { return m_saveDBInGameFile; }

	
	void		PostStartGameAction(void);
	void		PostSpriteTestAction(void);
	void		PostLoadSaveGameAction(MBCHAR *);
	void        PostLoadQuickSaveAction(PLAYER_INDEX player);

	void		PostLoadSaveGameMapAction(MBCHAR *); 
	void		PostRestartGameAction(void);

	
	void		PostRestartGameSameMapAction(void);
	void		PostQuitToSPShellAction(void);
	void		PostQuitToLobbyAction(void);

	void		PostEndGameAction(void);
	void		PostLoadScenarioGameAction(MBCHAR *name);

	BOOL		IsAppLoaded(void) const { return m_appLoaded; }
	BOOL		IsDBLoaded(void) const { return m_dbLoaded; }
	BOOL		IsGameLoaded(void) const { return m_gameLoaded; }

	BOOL		IsAIFinishedThisTurn(void) const { return m_aiFinishedThisTurn; }
	void		SetAIFinishedThisTurn(BOOL finished) { m_aiFinishedThisTurn = finished; }

	BOOL		IsInBackground(void) { return m_inBackground; }
	void		SetInBackground(BOOL in) { m_inBackground = in; }
private:
	BOOL		m_appLoaded;
	BOOL		m_dbLoaded;
	BOOL		m_gameLoaded;
	BOOL		m_saveDBInGameFile;
	BOOL		m_aiFinishedThisTurn;
	BOOL		m_inBackground;

	BOOL		m_isKeyboardScrolling;
	sint32		m_keyboardScrollingKey;
};


class StartGameAction : public aui_Action
{
public:
	virtual ActionCallback Execute;
};




class SpriteTestAction : public aui_Action
{
public:
	virtual ActionCallback Execute;
};



class LoadSaveGameAction : public aui_Action
{
	MBCHAR m_filename[ k_AUI_LDL_MAXBLOCK + 1 ];
public:
	
	LoadSaveGameAction() : aui_Action() {}
	LoadSaveGameAction(MBCHAR *name) : aui_Action() { strncpy(m_filename,name,k_AUI_LDL_MAXBLOCK );}
	virtual ActionCallback Execute;
};


class LoadSaveGameMapAction : public aui_Action
{
	MBCHAR m_filename[ k_AUI_LDL_MAXBLOCK + 1 ];
public:
	
	LoadSaveGameMapAction() : aui_Action() {}
	LoadSaveGameMapAction(MBCHAR *name) : aui_Action() { strncpy(m_filename,name,k_AUI_LDL_MAXBLOCK );}
	virtual ActionCallback Execute;
};

class RestartGameAction : public aui_Action
{
public:
	virtual ActionCallback Execute;
};


class RestartGameSameMapAction : public aui_Action
{
public:
	virtual ActionCallback Execute;
};
class QuitToSPShellAction : public aui_Action
{
public:
	virtual ActionCallback Execute;
};
class QuitToLobbyAction : public aui_Action
{
public:
	virtual ActionCallback Execute;
};

class EndGameAction : public aui_Action
{
public:
	virtual ActionCallback Execute;
};

class LoadScenarioGameAction : public aui_Action
{
public:
	LoadScenarioGameAction(MBCHAR *name) : aui_Action() { strncpy(m_filename, name, _MAX_PATH); }
	virtual ActionCallback Execute;

	MBCHAR m_filename[_MAX_PATH];
};

#endif
