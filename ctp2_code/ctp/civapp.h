#ifdef HAVE_PRAGMA_ONCE
#pragma once
#endif
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
	void 		InitializeAppUI(void);

	BOOL		m_appLoaded;
	BOOL		m_dbLoaded;
	BOOL		m_gameLoaded;
	BOOL		m_saveDBInGameFile;
	BOOL		m_aiFinishedThisTurn;
	BOOL		m_inBackground;

	BOOL		m_isKeyboardScrolling;
	sint32		m_keyboardScrollingKey;
};


AUI_ACTION_BASIC(EndGameAction);
AUI_ACTION_BASIC(QuitToSPShellAction);
AUI_ACTION_BASIC(QuitToLobbyAction);
AUI_ACTION_BASIC(RestartGameAction);
AUI_ACTION_BASIC(RestartGameSameMapAction);
AUI_ACTION_BASIC(StartGameAction);
AUI_ACTION_BASIC(SpriteTestAction);

class LoadSaveGameAction : public aui_Action
{
public:
	LoadSaveGameAction(MBCHAR const * name = NULL) 
    :   aui_Action  () 
    { 
        if (name)
        {
            strncpy(m_filename, name, k_AUI_LDL_MAXBLOCK);
        }
        else
        {
            m_filename[0] = 0;
        }
    };

	virtual void	Execute
	(
		aui_Control	*	control,
		uint32			action,
		uint32			data
	);

private:
	MBCHAR m_filename[k_AUI_LDL_MAXBLOCK + 1];
};


class LoadSaveGameMapAction : public aui_Action
{
public:
	LoadSaveGameMapAction(MBCHAR const * name = NULL) 
    :   aui_Action  () 
    { 
        if (name)
        {
            strncpy(m_filename, name, k_AUI_LDL_MAXBLOCK);
        }
        else
        {
            m_filename[0] = 0;
        }
    };

	virtual void	Execute
	(
		aui_Control	*	control,
		uint32			action,
		uint32			data
	);

private:
	MBCHAR m_filename[k_AUI_LDL_MAXBLOCK + 1];
};




class LoadScenarioGameAction : public aui_Action
{
public:
	LoadScenarioGameAction(MBCHAR const * name) 
    :   aui_Action  () 
    { 
        if (name)
        {
            strncpy(m_filename, name, _MAX_PATH); 
        }
        else
        {
            m_filename[0] = 0;
        }
    };

	virtual void	Execute
	(
		aui_Control	*	control,
		uint32			action,
		uint32			data
	);

private:
	MBCHAR m_filename[_MAX_PATH];
};

#endif
