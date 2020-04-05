//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ header
// Description  : Main application
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
//----------------------------------------------------------------------------
///
/// \file   civapp.h
/// \brief  Main application

#if defined(HAVE_PRAGMA_ONCE)
#pragma once
#endif

#ifndef CIVAPP_H__
#define CIVAPP_H__

//----------------------------------------------------------------------------
//
// Library imports
//
//----------------------------------------------------------------------------

#include "windows.h"            // HINSTANCE, MBCHAR

//----------------------------------------------------------------------------
//
// Exported names
//
//----------------------------------------------------------------------------

class CivApp;

//----------------------------------------------------------------------------
//
// Project imports
//
//----------------------------------------------------------------------------

#include "aui_ldl.h"
#include "aui_action.h"     // aui_Action
#include "ctp2_inttypes.h"  // sint32, uint32
class CivArchive;

//----------------------------------------------------------------------------
//
// Declarations
//
//----------------------------------------------------------------------------

class CivApp
{
public:
	CivApp();

	void		AutoSave(sint32 player, bool isQuickSave = false);
	void		BeginKeyboardScrolling(sint32 key);
	void		CleanupApp(void);
	void		CleanupAppDB(void);
	sint32		EndGame(void);

	sint32		GetKeyboardScrollingKey(void) const
    {
        return m_keyboardScrollingKey;
    };

    sint32		InitializeApp(HINSTANCE hInstance, int iCmdShow);
	bool		InitializeAppDB(void);
	sint32		InitializeAppDB(CivArchive &archive);
	sint32		InitializeGame(CivArchive *archive);

   	bool		IsGameLoaded(void) const
    {
        return m_gameLoaded;
    };

	bool		IsInBackground(void) const
    {
        return m_inBackground;
    };

	bool		IsKeyboardScrolling(void) const
    {
        return m_isKeyboardScrolling;
    };

	sint32		LoadSavedGame(MBCHAR const * name);
	sint32		LoadSavedGameMap(MBCHAR const * name);
	sint32		LoadScenarioGame(MBCHAR const * name);
	void		PostEndGameAction(void);
	void        PostLoadQuickSaveAction(sint32 player);
	void		PostLoadSaveGameAction(MBCHAR const *);
	void		PostLoadScenarioGameAction(MBCHAR const * name);
	void		PostQuitToLobbyAction(void);
	void		PostQuitToSPShellAction(void);
	void		PostRestartGameAction(void);
	void		PostRestartGameSameMapAction(void);
	void		PostSpriteTestAction(void);
	void		PostStartGameAction(void);
	sint32		Process(void);
	void		ProcessGraphicsCallback(void);
	sint32		QuickInit(HINSTANCE hInstance, int iCmdShow);
	void		QuitGame(void);
	sint32		QuitToLobby(void);
	sint32		QuitToSPShell(void);
	sint32		RestartGame(void);
	sint32		RestartGameSameMap(void);

	void		SetInBackground(bool in = true)
	{
		m_inBackground = in;
	}
	sint32		StartSpriteEditor(void);
	sint32		StartGame(void);
	void		StopKeyboardScrolling(sint32 key);

private:
	void		CleanupAppUI(void);
	void		CleanupGame(bool keepScenInfo);
	void		CleanupGameUI(void);
	void 		InitializeAppUI(void);
	sint32  	InitializeGameUI(void);
	sint32		InitializeSpriteEditor(CivArchive *archive);
	void		PostLoadSaveGameMapAction(MBCHAR const *);
	sint32      ProcessAI();
	sint32      ProcessNet(const uint32 target_milliseconds, uint32 &used_milliseconds);
	sint32		ProcessProfile(void);
	sint32      ProcessRobot(const uint32 target_milliseconds, uint32 &used_milliseconds);
	sint32      ProcessSLIC(void);
	sint32		ProcessUI(const uint32 target_milliseconds, uint32 &used_milliseconds);
	void		RestoreAutoSave(sint32 player);
	void        StartMessageSystem();

	bool		m_appLoaded;
	bool		m_dbLoaded;
	bool		m_gameLoaded;
	bool		m_aiFinishedThisTurn;
	bool		m_inBackground;
	bool		m_isKeyboardScrolling;
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

/*   // never used
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
*/

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
