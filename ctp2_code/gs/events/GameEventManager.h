
#ifndef __GAME_EVENT_MANAGER_H__
#define __GAME_EVENT_MANAGER_H__

#include "GameEventTypes.h"
#include "GameEventDescription.h"
#include "GameEventArgument.h"

class GameEvent;
template <class T> class PointerList;
class GameEventHookCallback;
class GameEventArgList;
class GameEventHook;

void gameEventManager_Initialize();
void gameEventManager_Cleanup();

#ifdef _DEBUG
#define EVENTLOG(x) g_gevManager->Log x
#else
#define EVENTLOG(x)
#endif

class GameEventManager {
public:
	GameEventManager();
	~GameEventManager();

//	GAME_EVENT_ERR AddEvent(GAME_EVENT_INSERT insert, GAME_EVENT type, ...);
	GAME_EVENT_ERR AddEvent(GAME_EVENT_INSERT insert, GAME_EVENT type,
				GAME_EVENT_ARGUMENT, int,
				GAME_EVENT_ARGUMENT);
	GAME_EVENT_ERR AddEvent(GAME_EVENT_INSERT insert, GAME_EVENT type,
				GAME_EVENT_ARGUMENT, int,
				GAME_EVENT_ARGUMENT, int,
				GAME_EVENT_ARGUMENT);
	GAME_EVENT_ERR AddEvent(GAME_EVENT_INSERT insert, GAME_EVENT type,
				GAME_EVENT_ARGUMENT, int,
				GAME_EVENT_ARGUMENT, int,
				GAME_EVENT_ARGUMENT, int,
				GAME_EVENT_ARGUMENT);
	GAME_EVENT_ERR AddEvent(GAME_EVENT_INSERT insert, GAME_EVENT type,
				GAME_EVENT_ARGUMENT, int,
				GAME_EVENT_ARGUMENT, int,
				GAME_EVENT_ARGUMENT, int,
				GAME_EVENT_ARGUMENT, int,
				GAME_EVENT_ARGUMENT);
	GAME_EVENT_ERR AddEvent(GAME_EVENT_INSERT insert, GAME_EVENT type,
				GAME_EVENT_ARGUMENT, int,
				GAME_EVENT_ARGUMENT, int,
				GAME_EVENT_ARGUMENT, int,
				GAME_EVENT_ARGUMENT, int,
				GAME_EVENT_ARGUMENT, int,
				GAME_EVENT_ARGUMENT);
	GAME_EVENT_ERR AddEvent(GAME_EVENT_INSERT insert, GAME_EVENT type,
				GAME_EVENT_ARGUMENT, const MapPoint&,
				GAME_EVENT_ARGUMENT);
	GAME_EVENT_ERR AddEvent(GAME_EVENT_INSERT insert, GAME_EVENT type,
				GAME_EVENT_ARGUMENT, int,
				GAME_EVENT_ARGUMENT, const MapPoint&,
				GAME_EVENT_ARGUMENT);
	GAME_EVENT_ERR AddEvent(GAME_EVENT_INSERT insert, GAME_EVENT type,
				GAME_EVENT_ARGUMENT, int,
				GAME_EVENT_ARGUMENT, int,
				GAME_EVENT_ARGUMENT, const MapPoint&,
				GAME_EVENT_ARGUMENT);
	GAME_EVENT_ERR AddEvent(GAME_EVENT_INSERT insert, GAME_EVENT type,
				GAME_EVENT_ARGUMENT, int,
				GAME_EVENT_ARGUMENT, const MapPoint&,
				GAME_EVENT_ARGUMENT, int,
				GAME_EVENT_ARGUMENT);
	GAME_EVENT_ERR AddEvent(GAME_EVENT_INSERT insert, GAME_EVENT type,
				GAME_EVENT_ARGUMENT, int,
				GAME_EVENT_ARGUMENT, const MapPoint&,
				GAME_EVENT_ARGUMENT, const MapPoint&,
				GAME_EVENT_ARGUMENT);
	GAME_EVENT_ERR AddEvent(GAME_EVENT_INSERT insert, GAME_EVENT type,
				GAME_EVENT_ARGUMENT, int,
				GAME_EVENT_ARGUMENT, const MapPoint&,
				GAME_EVENT_ARGUMENT, int,
				GAME_EVENT_ARGUMENT, int,
				GAME_EVENT_ARGUMENT);
	GAME_EVENT_ERR AddEvent(GAME_EVENT_INSERT insert, GAME_EVENT type,
				GAME_EVENT_ARGUMENT, int,
				GAME_EVENT_ARGUMENT, int,
				GAME_EVENT_ARGUMENT, const MapPoint&,
				GAME_EVENT_ARGUMENT, int,
				GAME_EVENT_ARGUMENT);
	GAME_EVENT_ERR AddEvent(GAME_EVENT_INSERT insert, GAME_EVENT type,
				GAME_EVENT_ARGUMENT, int,
				GAME_EVENT_ARGUMENT, Path*,
				GAME_EVENT_ARGUMENT, const MapPoint&,
				GAME_EVENT_ARGUMENT, int,
				GAME_EVENT_ARGUMENT);
	GAME_EVENT_ERR AddEvent(GAME_EVENT_INSERT insert, GAME_EVENT type,
				GAME_EVENT_ARGUMENT, int,
				GAME_EVENT_ARGUMENT, const MapPoint&,
				GAME_EVENT_ARGUMENT, int,
				GAME_EVENT_ARGUMENT, int,
				GAME_EVENT_ARGUMENT, int,
				GAME_EVENT_ARGUMENT);
	GAME_EVENT_ERR AddEvent(GAME_EVENT_INSERT insert, GAME_EVENT type,
				GAME_EVENT_ARGUMENT, int,
				GAME_EVENT_ARGUMENT, int,
				GAME_EVENT_ARGUMENT, int,
				GAME_EVENT_ARGUMENT, int,
				GAME_EVENT_ARGUMENT, const MapPoint&,
				GAME_EVENT_ARGUMENT);
	GAME_EVENT_ERR AddEvent(GAME_EVENT_INSERT insert, GAME_EVENT type,
				GAME_EVENT_ARGUMENT, int,
				GAME_EVENT_ARGUMENT, const MapPoint&,
				GAME_EVENT_ARGUMENT, int,
				GAME_EVENT_ARGUMENT, int,
				GAME_EVENT_ARGUMENT, int,
				GAME_EVENT_ARGUMENT, int,
				GAME_EVENT_ARGUMENT, int,
				GAME_EVENT_ARGUMENT);


	
	GAME_EVENT_ERR ArglistAddEvent(GAME_EVENT_INSERT insert, GAME_EVENT type,
								   GameEventArgList *argList);

	
	GAME_EVENT_ERR Process();

	
	GAME_EVENT_ERR ProcessHead();

	
	GameEvent *FindFirstEvent(GAME_EVENT type);

	
	GameEvent *FindNextEvent(GameEvent *current, GAME_EVENT type);

	
	GAME_EVENT_ERR AddCallback(GAME_EVENT type, GAME_EVENT_PRIORITY pri,
							   GameEventHookCallback *cb);

	
	GAME_EVENT_ERR RemoveCallback(GAME_EVENT type, GameEventHookCallback *cb);

	
	GAME_EVENT_ERR ActivateHook(GAME_EVENT type, GameEventArgList *args, sint32 &resumeIndex);
	GAME_EVENT_ERR ResumeHook(GAME_EVENT type, GameEventArgList *args, sint32 startIndex, sint32 &resumeIndex);

	
	GAME_EVENT GetEventIndex(const MBCHAR *name);
	const char *GetEventName(GAME_EVENT ev);

	const char *GetArgString(GAME_EVENT ev);
	GAME_EVENT_ARGUMENT ArgCharToIndex(char want);

	char ArgChar(GAME_EVENT type, sint32 index);
	sint32 GetNumArgs(GAME_EVENT type);

	
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

	
	bool EventsPending();

	


	char *ArgCharToName(char want);

	void NotifyResync();

private:
	BOOL CheckArg(sint32 num, char got, char want);
	BOOL VerifyArgs(GAME_EVENT type, va_list *vl);

	
	PointerList<GameEvent> *m_eventList;

#ifdef _DEBUG
	PointerList<GameEvent> *m_eventHistory;
#endif

	
	GameEventHook *m_hooks[GEV_MAX];

	
	bool m_processing;

	

	GAME_EVENT m_processingEvent;

	
	sint32 m_serial;

	bool m_needUserInput;

	sint32 m_pauseCount;
};

extern GameEventManager *g_gevManager;

#endif
