#include "c3.h"
#include "c3errors.h"
#include "c3debug.h"

#include "GameEventManager.h"
#include "GameEventHook.h"
#include "GameEventDescription.h"
#include "GameEventArgList.h"
#include "GameEventArgument.h"
#include "GameEvent.h"

#include "pointerlist.h"

#include "Army.h"
#include "Unit.h"

#include "SlicEngine.h"
#include "TerrImprove.h"
#include "TradeRoute.h"

#include "director.h"

GameEventManager *g_gevManager = NULL;

extern BOOL g_eventLog;

#define EVENTLOGNAME "eventlog.txt"

#define k_MAX_EVENT_HISTORY 10

void gameEventManager_Initialize()
{
	g_gevManager = new GameEventManager();
}

void gameEventManager_Cleanup()
{
	if(g_gevManager) {
		delete g_gevManager;
		g_gevManager = NULL;
	}
}

GameEventManager::GameEventManager()
{
	m_eventList = new PointerList<GameEvent>;
	m_processing = false;
	m_processingEvent = GEV_MAX;
	m_needUserInput = false;
	m_pauseCount = 0;

#ifdef _DEBUG
	m_eventHistory = new PointerList<GameEvent>;

	FILE *f = fopen(EVENTLOGNAME, "w");
	fclose(f);
#endif

	sint32 i;
	for(i = 0; i < GEV_MAX; i++) {

		m_hooks[i] = NULL;
	}

	m_serial = 0;
}

GameEventManager::~GameEventManager()
{
	if(m_eventList) {
		m_eventList->DeleteAll();
		delete m_eventList;
		m_eventList = NULL;
	}

#ifdef _DEBUG
	if(m_eventHistory) {
		m_eventHistory->DeleteAll();
		delete m_eventHistory;
		m_eventHistory = NULL;
	}
#endif

	sint32 i;
	for(i = 0; i < GEV_MAX; i++) {
		if(m_hooks[i]) {
			delete m_hooks[i];
			m_hooks[i] = NULL;
		}
	}
}

GAME_EVENT_ERR GameEventManager::AddEvent(GAME_EVENT_INSERT insert, GAME_EVENT type,
				GAME_EVENT_ARGUMENT t1, int a1,
				GAME_EVENT_ARGUMENT t2)
{
	const GAME_EVENT_ARGUMENT argTypes[] = { t1, t2 };
	const void* args[] = { &a1 };
	return AddEvent(insert, type, argTypes, args);
}

GAME_EVENT_ERR GameEventManager::AddEvent(GAME_EVENT_INSERT insert, GAME_EVENT type,
				GAME_EVENT_ARGUMENT t1, int a1,
				GAME_EVENT_ARGUMENT t2, int a2,
				GAME_EVENT_ARGUMENT t3)
{
	const GAME_EVENT_ARGUMENT argTypes[] = { t1, t2, t3 };
	const void* args[] = { &a1, &a2 };
	return AddEvent(insert, type, argTypes, args);
}

GAME_EVENT_ERR GameEventManager::AddEvent(GAME_EVENT_INSERT insert, GAME_EVENT type,
				GAME_EVENT_ARGUMENT t1, int a1,
				GAME_EVENT_ARGUMENT t2, int a2,
				GAME_EVENT_ARGUMENT t3, int a3,
				GAME_EVENT_ARGUMENT t4)
{
	const GAME_EVENT_ARGUMENT argTypes[] = { t1, t2, t3, t4 };
	const void* args[] = { &a1, &a2, &a3 };
	return AddEvent(insert, type, argTypes, args);
}

GAME_EVENT_ERR GameEventManager::AddEvent(GAME_EVENT_INSERT insert, GAME_EVENT type,
				GAME_EVENT_ARGUMENT t1, int a1,
				GAME_EVENT_ARGUMENT t2, int a2,
				GAME_EVENT_ARGUMENT t3, int a3,
				GAME_EVENT_ARGUMENT t4, int a4,
				GAME_EVENT_ARGUMENT t5)
{
	const GAME_EVENT_ARGUMENT argTypes[] = { t1, t2, t3, t4, t5 };
	const void* args[] = { &a1, &a2, &a3, &a4 };
	return AddEvent(insert, type, argTypes, args);
}

GAME_EVENT_ERR GameEventManager::AddEvent(GAME_EVENT_INSERT insert, GAME_EVENT type,
				GAME_EVENT_ARGUMENT t1, int a1,
				GAME_EVENT_ARGUMENT t2, int a2,
				GAME_EVENT_ARGUMENT t3, int a3,
				GAME_EVENT_ARGUMENT t4, int a4,
				GAME_EVENT_ARGUMENT t5, int a5,
				GAME_EVENT_ARGUMENT t6)
{
	const GAME_EVENT_ARGUMENT argTypes[] = { t1, t2, t3, t4, t5, t6 };
	const void* args[] = { &a1, &a2, &a3, &a4, &a5 };
	return AddEvent(insert, type, argTypes, args);
}

GAME_EVENT_ERR GameEventManager::AddEvent(GAME_EVENT_INSERT insert, GAME_EVENT type,
				GAME_EVENT_ARGUMENT t1, const MapPoint& a1,
				GAME_EVENT_ARGUMENT t2)
{
	const GAME_EVENT_ARGUMENT argTypes[] = { t1, t2 };
	const void* args[] = { &a1 };
	return AddEvent(insert, type, argTypes, args);
}

GAME_EVENT_ERR GameEventManager::AddEvent(GAME_EVENT_INSERT insert, GAME_EVENT type,
				GAME_EVENT_ARGUMENT t1, int a1,
				GAME_EVENT_ARGUMENT t2, const MapPoint& a2,
				GAME_EVENT_ARGUMENT t3)
{
	const GAME_EVENT_ARGUMENT argTypes[] = { t1, t2, t3 };
	const void* args[] = { &a1, &a2 };
	return AddEvent(insert, type, argTypes, args);
}

GAME_EVENT_ERR GameEventManager::AddEvent(GAME_EVENT_INSERT insert, GAME_EVENT type,
				GAME_EVENT_ARGUMENT t1, int a1,
				GAME_EVENT_ARGUMENT t2, int a2,
				GAME_EVENT_ARGUMENT t3, const MapPoint& a3,
				GAME_EVENT_ARGUMENT t4)
{
	const GAME_EVENT_ARGUMENT argTypes[] = { t1, t2, t3, t4 };
	const void* args[] = { &a1, &a2, &a3 };
	return AddEvent(insert, type, argTypes, args);
}

GAME_EVENT_ERR GameEventManager::AddEvent(GAME_EVENT_INSERT insert, GAME_EVENT type,
				GAME_EVENT_ARGUMENT t1, int a1,
				GAME_EVENT_ARGUMENT t2, const MapPoint& a2,
				GAME_EVENT_ARGUMENT t3, int a3,
				GAME_EVENT_ARGUMENT t4)
{
	const GAME_EVENT_ARGUMENT argTypes[] = { t1, t2, t3, t4 };
	const void* args[] = { &a1, &a2, &a3 };
	return AddEvent(insert, type, argTypes, args);
}

GAME_EVENT_ERR GameEventManager::AddEvent(GAME_EVENT_INSERT insert, GAME_EVENT type,
				GAME_EVENT_ARGUMENT t1, int a1,
				GAME_EVENT_ARGUMENT t2, const MapPoint& a2,
				GAME_EVENT_ARGUMENT t3, const MapPoint& a3,
				GAME_EVENT_ARGUMENT t4)
{
	const GAME_EVENT_ARGUMENT argTypes[] = { t1, t2, t3, t4 };
	const void* args[] = { &a1, &a2, &a3 };
	return AddEvent(insert, type, argTypes, args);
}

GAME_EVENT_ERR GameEventManager::AddEvent(GAME_EVENT_INSERT insert, GAME_EVENT type,
				GAME_EVENT_ARGUMENT t1, int a1,
				GAME_EVENT_ARGUMENT t2, const MapPoint& a2,
				GAME_EVENT_ARGUMENT t3, int a3,
				GAME_EVENT_ARGUMENT t4, int a4,
				GAME_EVENT_ARGUMENT t5)
{
	const GAME_EVENT_ARGUMENT argTypes[] = { t1, t2, t3, t4, t5 };
	const void* args[] = { &a1, &a2, &a3, &a4 };
	return AddEvent(insert, type, argTypes, args);
}

GAME_EVENT_ERR GameEventManager::AddEvent(GAME_EVENT_INSERT insert, GAME_EVENT type,
				GAME_EVENT_ARGUMENT t1, int a1,
				GAME_EVENT_ARGUMENT t2, int a2,
				GAME_EVENT_ARGUMENT t3, const MapPoint& a3,
				GAME_EVENT_ARGUMENT t4, int a4,
				GAME_EVENT_ARGUMENT t5)
{
	const GAME_EVENT_ARGUMENT argTypes[] = { t1, t2, t3, t4, t5 };
	const void* args[] = { &a1, &a2, &a3, &a4 };
	return AddEvent(insert, type, argTypes, args);
}

GAME_EVENT_ERR GameEventManager::AddEvent(GAME_EVENT_INSERT insert, GAME_EVENT type,
				GAME_EVENT_ARGUMENT t1, int a1,
				GAME_EVENT_ARGUMENT t2, Path* a2,
				GAME_EVENT_ARGUMENT t3, const MapPoint& a3,
				GAME_EVENT_ARGUMENT t4, int a4,
				GAME_EVENT_ARGUMENT t5)
{
	const GAME_EVENT_ARGUMENT argTypes[] = { t1, t2, t3, t4, t5 };
	const void* args[] = { &a1, &a2, &a3, &a4 };
	return AddEvent(insert, type, argTypes, args);
}

GAME_EVENT_ERR GameEventManager::AddEvent(GAME_EVENT_INSERT insert, GAME_EVENT type,
				GAME_EVENT_ARGUMENT t1, int a1,
				GAME_EVENT_ARGUMENT t2, const MapPoint& a2,
				GAME_EVENT_ARGUMENT t3, int a3,
				GAME_EVENT_ARGUMENT t4, int a4,
				GAME_EVENT_ARGUMENT t5, int a5,
				GAME_EVENT_ARGUMENT t6)
{
	const GAME_EVENT_ARGUMENT argTypes[] = { t1, t2, t3, t4, t5, t6 };
	const void* args[] = { &a1, &a2, &a3, &a4, &a5 };
	return AddEvent(insert, type, argTypes, args);
}

GAME_EVENT_ERR GameEventManager::AddEvent(GAME_EVENT_INSERT insert, GAME_EVENT type,
				GAME_EVENT_ARGUMENT t1, int a1,
				GAME_EVENT_ARGUMENT t2, int a2,
				GAME_EVENT_ARGUMENT t3, int a3,
				GAME_EVENT_ARGUMENT t4, int a4,
				GAME_EVENT_ARGUMENT t5, const MapPoint& a5,
				GAME_EVENT_ARGUMENT t6)
{
	const GAME_EVENT_ARGUMENT argTypes[] = { t1, t2, t3, t4, t5, t6 };
	const void* args[] = { &a1, &a2, &a3, &a4, &a5 };
	return AddEvent(insert, type, argTypes, args);
}

GAME_EVENT_ERR GameEventManager::AddEvent(GAME_EVENT_INSERT insert, GAME_EVENT type,
				GAME_EVENT_ARGUMENT t1, int a1,
				GAME_EVENT_ARGUMENT t2, const MapPoint& a2,
				GAME_EVENT_ARGUMENT t3, int a3,
				GAME_EVENT_ARGUMENT t4, int a4,
				GAME_EVENT_ARGUMENT t5, int a5,
				GAME_EVENT_ARGUMENT t6, int a6,
				GAME_EVENT_ARGUMENT t7, int a7,
				GAME_EVENT_ARGUMENT t8)
{
	const GAME_EVENT_ARGUMENT argTypes[] = { t1, t2, t3, t4, t5, t6, t7, t8 };
	const void* args[] = { &a1, &a2, &a3, &a4, &a5, &a6, &a7 };
	return AddEvent(insert, type, argTypes, args);
}

GAME_EVENT_ERR GameEventManager::AddEvent(GAME_EVENT_INSERT insert, GAME_EVENT type,
	const GAME_EVENT_ARGUMENT* argTypes, const void** args)
{
	Assert(type >= (GAME_EVENT)0);
	Assert(type < GEV_MAX);
	if(type < (GAME_EVENT)0 || type >= GEV_MAX)
		return GEV_ERR_BadEvent;

	if(g_slicEngine->AtBreak()) {
		return GEV_ERR_AtBreak;
	}

	if(m_processing) {
		EVENTLOG(("    "));
	}
	EVENTLOG(("AddEvent: %s(", g_eventDescriptions[type].name));

	BOOL argsOk = VerifyArgs(type, argTypes, args);

	EVENTLOG(("): Serial: %d\n", m_serial));

	Assert(argsOk);
	if(!argsOk) {
		return GEV_ERR_WrongArguments;
	}

	GameEventArgList *argList = new GameEventArgList(argTypes, args);

	return ArglistAddEvent(insert, type, argList);
}

GAME_EVENT_ERR GameEventManager::ArglistAddEvent(GAME_EVENT_INSERT insert,
												 GAME_EVENT type,
												 GameEventArgList *argList)
{
	Assert(type >= (GAME_EVENT)0);
	Assert(type < GEV_MAX);
	if(type < (GAME_EVENT)0 || type >= GEV_MAX)
		return GEV_ERR_BadEvent;

	GameEvent *newEvent = new GameEvent(type, argList, m_serial++, m_processingEvent);

	newEvent->SetType(type);
	switch(insert) {
		case GEV_INSERT_Front:
			m_eventList->AddHead(newEvent);
			break;
		case GEV_INSERT_AfterCurrent:
			if(m_processing)
				m_eventList->InsertAt(m_eventList->GetHeadNode(), newEvent);
			else
				m_eventList->AddHead(newEvent);
			break;
		case GEV_INSERT_Tail:
			m_eventList->AddTail(newEvent);
			break;
		default:
			return GEV_ERR_BadInsert;
	}

	g_director->IncrementPendingGameActions();

	Process();

	return GEV_ERR_OK;
}

GAME_EVENT_ERR GameEventManager::Process()
{
	if(m_processing)

		return GEV_ERR_OK;

	if(!m_eventList->GetHead()) {
		return GEV_ERR_OK;
	}

	if(g_slicEngine->AtBreak() || m_needUserInput || m_pauseCount)
		return GEV_ERR_OK;

	EVENTLOG(("\nGameEventManager::Process()\n"));

	m_processing = true;

	GAME_EVENT_ERR err = GEV_ERR_OK;

	do {

		err = ProcessHead();
	} while(m_eventList->GetHead() && err == GEV_ERR_OK && !g_slicEngine->AtBreak() && !m_needUserInput && !m_pauseCount);

	switch(err) {
		case GEV_ERR_OK:
			err = GEV_ERR_OK;
			break;
		case GEV_ERR_NeedUserInput:

			err = GEV_ERR_OK;
			m_needUserInput = true;
			break;
		default:
			break;
	}
	m_processing = false;
	return err;
}

GAME_EVENT_ERR GameEventManager::ProcessHead()
{
	GameEvent *event = m_eventList->GetHead();
	BOOL complete;

	EVENTLOG(("ProcessEvent: %s Serial: %d\n", g_eventDescriptions[event->GetType()].name,
			event->GetSerial()));


	m_processingEvent = event->GetType();

	GAME_EVENT_ERR err = event->Process(complete);

	m_processingEvent = GEV_MAX;
	if(complete) {

		Assert(event == m_eventList->GetHead());
		m_eventList->RemoveHead();
		g_director->DecrementPendingGameActions();

#ifdef _DEBUG
		m_eventHistory->AddTail(event);
		if(m_eventHistory->GetCount() > k_MAX_EVENT_HISTORY)
			event = m_eventHistory->RemoveHead();
		else
			event = NULL;
#endif
		if(event)
			delete event;

	} else {
		err = GEV_ERR_NeedUserInput;
	}

	return err;
}

GAME_EVENT_ERR GameEventManager::AddCallback(GAME_EVENT type,
											 GAME_EVENT_PRIORITY pri,
											 GameEventHookCallback *cb)
{
	Assert(type >= (GAME_EVENT)0);
	Assert(type < GEV_MAX);
	if(type < (GAME_EVENT)0 || type >= GEV_MAX)
		return GEV_ERR_BadEvent;

	if(!m_hooks[type])
		m_hooks[type] = new GameEventHook(type);

	m_hooks[type]->AddCallback(cb, pri);
	return GEV_ERR_OK;
}

GAME_EVENT_ERR GameEventManager::RemoveCallback(GAME_EVENT type,
												GameEventHookCallback *cb)
{
	Assert(type >= (GAME_EVENT)0);
	Assert(type < GEV_MAX);
	if(type < (GAME_EVENT)0 || type >= GEV_MAX)
		return GEV_ERR_BadEvent;

	if(!m_hooks[type])
		return GEV_ERR_OK;

	m_hooks[type]->RemoveCallback(cb);
	return GEV_ERR_OK;
}

GAME_EVENT_ERR GameEventManager::ActivateHook(GAME_EVENT type,
											  GameEventArgList *args,
											  sint32 &resumeIndex)
{
	Assert(type < GEV_MAX);
	if((type >= GEV_MAX) || !m_hooks[type])

		return GEV_ERR_OK;

	return m_hooks[type]->Activate(args, resumeIndex);
}

GAME_EVENT_ERR GameEventManager::ResumeHook(GAME_EVENT type,
											GameEventArgList *args,
											sint32 startIndex,
											sint32 &resumeIndex)
{
	if(!m_hooks[type])

		return GEV_ERR_OK;

	return m_hooks[type]->Resume(args, startIndex, resumeIndex);
}

char *GameEventManager::ArgCharToName(char want)
{
	switch(want) {
		case GEAC_ARMY: return "GEA_Army";
		case GEAC_UNIT: return "GEA_Unit";
		case GEAC_CITY: return "GEA_City";
		case GEAC_POP: return "GEA_Pop";
		case GEAC_GOLD: return "GEA_Gold";
		case GEAC_PATH: return "GEA_Path";
		case GEAC_MAPPOINT: return "GEA_MapPoint";
		case GEAC_PLAYER: return "GEA_Player";
		case GEAC_INT: return "GEA_Int";
		case GEAC_DIRECTION: return "GEA_Direction";
		case GEAC_ADVANCE: return "GEA_Advance";
		case GEAC_WONDER: return "GEA_Wonder";
		case GEAC_IMPROVEMENT: return "GEA_Improvement";
		case GEAC_TRADEROUTE: return "GEA_TradeRoute";
		default: return "GEA_FixArgCharToNameDammit";
	}
}

GAME_EVENT_ARGUMENT GameEventManager::ArgCharToIndex(char arg)
{
	switch(arg) {
		case GEAC_ARMY: return GEA_Army;
		case GEAC_UNIT: return GEA_Unit;
		case GEAC_CITY: return GEA_City;
		case GEAC_POP: return GEA_Pop;
		case GEAC_GOLD: return GEA_Gold;
		case GEAC_PATH: return GEA_Path;
		case GEAC_MAPPOINT: return GEA_MapPoint;
		case GEAC_PLAYER: return GEA_Player;
		case GEAC_INT: return GEA_Int;
		case GEAC_DIRECTION: return GEA_Direction;
		case GEAC_ADVANCE: return GEA_Advance;
		case GEAC_WONDER:  return GEA_Wonder;
		case GEAC_IMPROVEMENT: return GEA_Improvement;
		case GEAC_TRADEROUTE: return GEA_TradeRoute;

		default: Assert(FALSE); return GEA_Null;
	}
}

const char *GameEventManager::GetArgString(GAME_EVENT ev)
{
	Assert(ev >= (GAME_EVENT)0);
	Assert(ev < GEV_MAX);
	if(ev < (GAME_EVENT)0 || ev >= GEV_MAX) {
		return "";
	}
	return g_eventDescriptions[ev].args;
}

BOOL GameEventManager::CheckArg(sint32 num, char got, char want)
{
	EVENTLOG(("%s ", ArgCharToName(got)));

	if(got != want) {
#if defined(_DEBUG) && defined(WIN32)
		c3errors_ErrorDialog("GameEventManager", "Argument %d should be of type %s.  Stack: %s", num, ArgCharToName(want), c3debug_StackTrace());
#endif
		return FALSE;
	}
	return TRUE;
}

char GameEventManager::ArgChar(GAME_EVENT type, sint32 index)
{
	Assert(type >= (GAME_EVENT)0);
	Assert(type < GEV_MAX);
	if(type < (GAME_EVENT)0 || type >= GEV_MAX)
		return '\0';

	GameEventDescription *desc = &g_eventDescriptions[type];
	char *argString = desc->args;
	sint32 count = 0;

	while(*argString) {
		Assert(*argString == '%' || *argString == '&');
		if(*argString != '%' && *argString != '&')
			return '\0';

		argString++;
		Assert(*argString != '\0');
		if(*argString == '\0')
			return '\0';

		if(count == index)
			return *argString;

		argString++;
		count++;
	}

	Assert(index > count);
	return '\0';
}

sint32 GameEventManager::GetNumArgs(GAME_EVENT type)
{
	Assert(type >= (GAME_EVENT)0);
	Assert(type < GEV_MAX);
	if(type < (GAME_EVENT)0 || type >= GEV_MAX)
		return -1;

	GameEventDescription *desc = &g_eventDescriptions[type];
	char *argString = desc->args;
	sint32 count = 0;

	while(*argString) {
		Assert(*argString == '%' || *argString == '&');
		if(*argString != '%') {

			return count;
		}

		argString++;
		Assert(*argString != '\0');
		if(*argString == '\0')
			return count;

		argString++;
		count++;
	}

	return count;
}

BOOL GameEventManager::VerifyArgs(GAME_EVENT type, const GAME_EVENT_ARGUMENT* argTypes, const void** args)
{
	Assert(type >= (GAME_EVENT)0);
	Assert(type < GEV_MAX);
	if(type < (GAME_EVENT)0 || type >= GEV_MAX)
		return FALSE;

	GameEventDescription *desc = &g_eventDescriptions[type];
	char *argString = desc->args;

	BOOL done = FALSE;
	GAME_EVENT_ARGUMENT nextArgType;

	sint32 argNum = 0; // argNum is the number of the expected argument we are looking for
	uint32 givenArgNum = 0; // givenArgNum is the number of the argument provided to us we are looking at
	bool isOptional = false;

	while(!done) {
		nextArgType = argTypes[givenArgNum];

		if(nextArgType == GEA_End && *argString == 0)
			return TRUE;

		if(!isOptional) {

			Assert(*argString == '%' || *argString == '&');
			if(*argString != '%' && *argString != '&')
				return FALSE;

			if(*argString == '&')
				isOptional = true;
		} else {
			Assert(*argString == '&');
			if(*argString != '&')
				return FALSE;
		}

		if(isOptional && nextArgType == GEA_End) {
			return TRUE;
		}

		argString++;
		argNum++;

		Assert(*argString);
		if(!(*argString))
			return FALSE;


		static Army a;
		static Unit u, c;
		static MapPoint pos;
		static TerrainImprovement imp;
		static TradeRoute route;

		sint32 value;
		Path *path;

#ifdef _DEBUG
#define DG_PRINT(fn, fmt, val) { EVENTLOG((fmt, val)); }
#else
#define DG_PRINT(fn, fmt, val)
#endif

		switch(nextArgType) {
			case GEA_Army:
				if(!CheckArg(argNum, *argString, GEAC_ARMY)) return FALSE;
				a = *(Army*)args[givenArgNum];
				DG_PRINT(EVENTLOGNAME, "0x%lx, ", a.m_id);
				break;
			case GEA_Unit:
				if(!CheckArg(argNum, *argString, GEAC_UNIT)) return FALSE;
				u = *(Unit*)args[givenArgNum];
				DG_PRINT(EVENTLOGNAME, "0x%lx, ", u.m_id);
				break;
			case GEA_City:
				if(!CheckArg(argNum, *argString, GEAC_CITY)) return FALSE;
				c = *(Unit*)args[givenArgNum];
				DG_PRINT(EVENTLOGNAME, "0x%lx, ", c.m_id);
				break;
			case GEA_Gold:
				if(!CheckArg(argNum, *argString, GEAC_GOLD)) return FALSE;
				value = *(sint32*)args[givenArgNum];
				DG_PRINT(EVENTLOGNAME, "%d, ", value);
				break;
			case GEA_Path:
				if(!CheckArg(argNum, *argString, GEAC_PATH)) return FALSE;
				path = *(Path**)args[givenArgNum];
				DG_PRINT(EVENTLOGNAME, "0x%lx, ", path);
				break;
			case GEA_MapPoint:
				if(!CheckArg(argNum, *argString, GEAC_MAPPOINT)) return FALSE;
				pos = *(MapPoint*)args[givenArgNum];
				EVENTLOG(("(%d,%d), ", pos.x, pos.y));
				break;
			case GEA_Player:
				if(!CheckArg(argNum, *argString, GEAC_PLAYER)) return FALSE;
				value = *(sint32*)args[givenArgNum];
				DG_PRINT(EVENTLOGNAME, "%d, ", value);
				break;
			case GEA_Int:
				if(!CheckArg(argNum, *argString, GEAC_INT)) return FALSE;
				value = *(sint32*)args[givenArgNum];
				DG_PRINT(EVENTLOGNAME, "%d, ", value);
				break;
			case GEA_Direction:
				if(!CheckArg(argNum, *argString, GEAC_DIRECTION)) return FALSE;
				value = *(sint32*)args[givenArgNum];
				DG_PRINT(EVENTLOGNAME, "%d, ", value);
				break;

			case GEA_Wonder:
				if(!CheckArg(argNum, *argString, GEAC_WONDER)) return FALSE;
				value = *(sint32*)args[givenArgNum];
				DG_PRINT(EVENTLOGNAME, "%d, ", value);
				break;
			case GEA_Advance:
				if(!CheckArg(argNum, *argString, GEAC_ADVANCE)) return FALSE;
				value = *(sint32*)args[givenArgNum];
				DG_PRINT(EVENTLOGNAME, "%d, ", value);
				break;
			case GEA_Improvement:
				if(!CheckArg(argNum, *argString, GEAC_IMPROVEMENT)) return FALSE;
				imp = *(TerrainImprovement*)args[givenArgNum];
				DG_PRINT(EVENTLOGNAME, "0x%lx, ", imp.m_id);
				break;
			case GEA_TradeRoute:
				if(!CheckArg(argNum, *argString, GEAC_TRADEROUTE)) return FALSE;
				route = *(TradeRoute*)args[givenArgNum];
				DG_PRINT(EVENTLOGNAME, "0x%lx, ", route.m_id);
				break;
			case GEA_End:
				if(*(argString) != 0) {
#if defined(_DEBUG) && defined(WIN32)
					c3errors_ErrorDialog("GameEventManager", "Not enough arguments.  Stack: %s", c3debug_StackTrace());
#endif
					return FALSE;
				}
				return TRUE;
			default:
				Assert(FALSE);
				return FALSE;
		}
		argString++;
		givenArgNum++;
	}
	return FALSE;
}

#ifdef _DEBUG
void GameEventManager::Log(const char *fmt, ...)
{

	if (g_eventLog == FALSE)
		return;

	char text[1024];
	va_list vl;
	va_start(vl, fmt);
	vsprintf(text, fmt, vl);
	va_end(vl);

	FILE *f = fopen(EVENTLOGNAME, "a");
	if(f) {
		fprintf(f, "%s", text);
		fclose(f);
	}

}

void GameEventManager::Dump()
{
	FILE *f = fopen("eventcb.txt", "w");

	GAME_EVENT ev;
	for(ev = (GAME_EVENT)0; ev < GEV_MAX; ev = GAME_EVENT((sint32)ev + 1)) {

		fprintf(f, "%d: GEV_%s(", (sint32)ev, g_eventDescriptions[ev].name);

		char *argString = g_eventDescriptions[ev].args;
		BOOL first = TRUE;


		while(*argString) {
			Assert(*argString == '%' || *argString == '&');

			BOOL  optional= *argString=='&';
			if(optional)
				{
				fprintf(f, " [");
				}


			argString++;
			if(!(*argString)) {
				Assert(FALSE);
				break;
			}
			if(!first) {
				fprintf(f, ", ");
			}
			first = FALSE;
			fprintf(f, "%s", ArgCharToName(*argString));
			argString++;

			if(optional)
				{
				fprintf(f, "]");
				}

		}

		fprintf(f, "): %s\n", g_eventDescriptions[ev].description);

		if(m_hooks[ev]) {
			m_hooks[ev]->Dump(f);
		} else {
			fprintf(f, "  <empty>\n");
		}
		fprintf(f, "\n");
	}
	fclose(f);
}
#endif

GAME_EVENT GameEventManager::GetEventIndex(const MBCHAR *name)
{
	GAME_EVENT e;
	for(e = (GAME_EVENT)0; e < GEV_MAX; e = GAME_EVENT(sint32(e) + 1)) {
		if(!stricmp(g_eventDescriptions[e].name, name))
			return e;
	}
	return GEV_MAX;
}

const char *GameEventManager::GetEventName(GAME_EVENT ev)
{
	Assert(ev >= (GAME_EVENT)0);
	Assert(ev < GEV_MAX);
	if(ev < (GAME_EVENT)0 || ev >= GEV_MAX) {
		return "";
	}

	return g_eventDescriptions[ev].name;
}

bool GameEventManager::EventsPending()
{
	return m_eventList->GetCount() > 0;
}

void GameEventManager::GotUserInput()
{
	m_needUserInput = false;
}

void GameEventManager::Pause()
{
	m_pauseCount++;
}

void GameEventManager::Resume()
{
	m_pauseCount--;
	Assert(m_pauseCount >= 0);
}

void GameEventManager::NotifyResync()
{
	m_eventList->DeleteAll();
	m_needUserInput = false;
}
