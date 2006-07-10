//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C/C++ header
// Description  : Game event handler
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
// _DEBUG
// - Enable logging when set.
//
// HAVE_PRAGMA_ONCE
//
//----------------------------------------------------------------------------

#if defined(HAVE_PRAGMA_ONCE)
#pragma once
#endif

#ifndef __GAME_EVENT_MANAGER_H__
#define __GAME_EVENT_MANAGER_H__

//----------------------------------------------------------------------------
// Library dependencies
//----------------------------------------------------------------------------

#include <list>

//----------------------------------------------------------------------------
// Export overview
//----------------------------------------------------------------------------

class GameEventManager;

#ifdef _DEBUG
#define EVENTLOG(x) g_gevManager->Log x
#else
#define EVENTLOG(x)
#endif

//----------------------------------------------------------------------------
// Project dependencies
//----------------------------------------------------------------------------

#include "GameEventTypes.h"
#include "GameEventDescription.h"
#include "GameEventArgument.h"

class GameEvent;
template <class T> class PointerList;
class GameEventHookCallback;
class GameEventArgList;
class GameEventHook;

//----------------------------------------------------------------------------
// General declarations
//----------------------------------------------------------------------------

void gameEventManager_Initialize();
void gameEventManager_Cleanup();

extern GameEventManager *   g_gevManager;

//----------------------------------------------------------------------------
// Class declarations
//----------------------------------------------------------------------------

class GameEventManager 
{
public:
	GameEventManager();
	~GameEventManager();

	


	GAME_EVENT_ERR AddEvent(GAME_EVENT_INSERT insert, GAME_EVENT type, ...);

	
	GAME_EVENT_ERR ArglistAddEvent(GAME_EVENT_INSERT insert, GAME_EVENT type,
								   GameEventArgList *argList);

	
	GAME_EVENT_ERR Process();

	
	GAME_EVENT_ERR ProcessHead();

	
	GameEvent *FindFirstEvent(GAME_EVENT type);

	
	GameEvent *FindNextEvent(GameEvent *current, GAME_EVENT type);

	
	GAME_EVENT_ERR AddCallback(GAME_EVENT type, GAME_EVENT_PRIORITY pri,
							   GameEventHookCallback *cb);

	
	void RemoveCallback(GAME_EVENT type, GameEventHookCallback *cb);

	
	GAME_EVENT_ERR ActivateHook
    (
        GAME_EVENT          type, 
        GameEventArgList *  args, 
        sint32              startIndex, 
        sint32 &            resumeIndex
    );

	
	GAME_EVENT GetEventIndex(const MBCHAR *name) const;
	const char *GetEventName(GAME_EVENT ev) const;

	const char *GetArgString(GAME_EVENT ev) const;
	static GAME_EVENT_ARGUMENT ArgCharToIndex(char want);

	char ArgChar(GAME_EVENT type, size_t index) const;
	size_t GetNumArgs(GAME_EVENT type) const;

	
	void GotUserInput();

	
	void SetNeedUserInput() { m_needUserInput = true; }
	bool NeedUserInput() { return m_needUserInput; }

	void Pause();
	void Resume();

#ifdef _DEBUG
	void Log(const char *fmt, ...);
	void Dump();
#endif

	

	bool IsProcessing() const { return m_processing; }

	

	sint32 GetNextSerial() const { return m_serial; }

	
	bool EventsPending() const;

	


	static char * ArgCharToName(char want);

	void NotifyResync();

private:
	BOOL CheckArg(sint32 num, char got, char want);
	BOOL VerifyArgs(GAME_EVENT type, va_list *vl);

	/// Unhandled events
	PointerList<GameEvent> *m_eventList;

#ifdef _DEBUG
    /// History of recently handled events
    std::list<GameEvent*>   m_eventHistory;
#endif

	
	GameEventHook *m_hooks[GEV_MAX];

	
	bool m_processing;

	

	GAME_EVENT m_processingEvent;

	
	sint32 m_serial;

	bool m_needUserInput;

	sint32 m_pauseCount;
};

#endif
