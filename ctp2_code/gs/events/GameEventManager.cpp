//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
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
//----------------------------------------------------------------------------
//
// Modifications from the original Activision code:
//
// - Improved slic event debugging. (7-Nov-2007 Martin Gühmann)
// - Events that should go into the event queue are only added if all their
//   arguments are valid. (7-Nov-2007 Martin Gühmann)
// - Events whose arguments became invalid between call and execution are not
//   executed and an error message is given. (7-Nov-2007 Martin Gühmann)
//
//----------------------------------------------------------------------------

#include "c3.h"
#include "GameEventManager.h"

#include "c3errors.h"
#include "c3debug.h"

#include "GameEventHook.h"
#include "GameEventDescription.h"
#include "GameEventArgList.h"
#include "GameEventArgument.h"
#include "GameEvent.h"

#include "Army.h"
#include "Unit.h"

#include "SlicEngine.h"
#include "TerrImprove.h"
#include "TradeRoute.h"

#include "director.h"
#include "SelItem.h"                // g_selected_item

GameEventManager *g_gevManager = NULL;

extern BOOL g_eventLog;

#define EVENTLOGNAME "eventlog.txt"

#define k_MAX_EVENT_HISTORY 10

void gameEventManager_Initialize()
{
    delete g_gevManager;
	g_gevManager = new GameEventManager();
}

void gameEventManager_Cleanup()
{
	delete g_gevManager;
	g_gevManager = NULL;
}

GameEventManager::GameEventManager()
:
	m_eventList         (new PointerList<GameEvent>),
#if defined(_DEBUG)
	m_eventHistory      (),
#endif
    // 	GameEventHook *m_hooks[GEV_MAX];
    m_processing        (false),
	m_processingEvent   (GEV_MAX),
	m_serial            (0),
    m_needUserInput     (false),
    m_pauseCount        (0)
{
#ifdef _DEBUG
	FILE *  f = fopen(EVENTLOGNAME, "w");
	fclose(f);
#endif

	std::fill(m_hooks, m_hooks + GEV_MAX, (GameEventHook *) NULL);
}

GameEventManager::~GameEventManager()
{
	if (m_eventList) 
    {
		m_eventList->DeleteAll();
		delete m_eventList;
	}

#ifdef _DEBUG
    for 
    (
        std::list<GameEvent *>::iterator    p = m_eventHistory.begin();
        p != m_eventHistory.end();
        ++p
    )
    {
		delete *p;
	}
    std::list<GameEvent *>().swap(m_eventHistory);
#endif

	for (size_t i = 0; i < GEV_MAX; ++i) 
    {
		delete m_hooks[i];
	}
}






GAME_EVENT_ERR GameEventManager::AddEvent(GAME_EVENT_INSERT insert,
										  GAME_EVENT type, ...)
{
	Assert((type >= (GAME_EVENT) 0) && (type < GEV_MAX));
	if(type < (GAME_EVENT)0 || type >= GEV_MAX)
		return GEV_ERR_BadEvent;
	
	
	
	
	
	
	
	
	if(g_slicEngine->AtBreak()) {
		return GEV_ERR_AtBreak;
	}

	if(m_processing) {
		EVENTLOG(("    "));
	}
	EVENTLOG(("AddEvent: %s(", g_eventDescriptions[type].name));

	va_list vl;
	va_start(vl, type);

	BOOL argsOk = VerifyArgs(type, &vl);
	va_end(vl);

	EVENTLOG(("): Serial: %d\n", m_serial));

	Assert(argsOk);
	if(!argsOk) {
		return GEV_ERR_WrongArguments;
	}

	va_start(vl, type);

	GameEventArgList *argList = new GameEventArgList(&vl, type);

	va_end(vl);

	return ArglistAddEvent(insert, type, argList);
}

GAME_EVENT_ERR GameEventManager::ArglistAddEvent(GAME_EVENT_INSERT insert,
												 GAME_EVENT type,
												 GameEventArgList *argList)
{
	Assert((type >= (GAME_EVENT) 0) && (type < GEV_MAX));
	if(type < (GAME_EVENT)0 || type >= GEV_MAX)
		return GEV_ERR_BadEvent;

	if(!argList->TestArgs(type))
	{
		delete argList;
		return GEV_ERR_ArgsInvalid;
	}

	GameEvent *newEvent = new GameEvent(type, argList, m_serial++, m_processingEvent);

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

	return Process();
}

GAME_EVENT_ERR GameEventManager::Process()
{
    if (m_processing)   // busy already?
    {
        return GEV_ERR_OK;
    }

    m_processing        = true;
	GAME_EVENT_ERR  err = GEV_ERR_OK;

    while ((GEV_ERR_OK == err) 
            && m_eventList->GetHead()
            && !g_slicEngine->AtBreak() 
            && !m_needUserInput
            && !m_pauseCount
          )
    {
        err             = ProcessHead();
    }
	m_processing        = false;

	if (GEV_ERR_NeedUserInput == err)
    {		
		m_needUserInput = true;
		return GEV_ERR_OK;
	}

	return err;
}

GAME_EVENT_ERR GameEventManager::ProcessHead()
{
	GameEvent *     event   = m_eventList->GetHead();
    
    // Processing busy
	m_processingEvent       = event->GetType();

	EVENTLOG(("ProcessEvent: %s Serial: %d\n", 
              g_eventDescriptions[m_processingEvent].name,
			  event->GetSerial()
            ));

	GAME_EVENT_ERR  err     = event->Process();

    // Processing ready
	m_processingEvent       = GEV_MAX;

	if (GEV_ERR_NeedUserInput != err) 
    {
		Assert(event == m_eventList->GetHead());
		m_eventList->RemoveHead();
		g_director->DecrementPendingGameActions();

#if defined(_DEBUG)
        // Debug version: keep the last k_MAX_EVENT_HISTORY handled events
        // to be able to find out how we got here.
		m_eventHistory.push_back(event);
		if (m_eventHistory.size() > k_MAX_EVENT_HISTORY)
        {
            // delete the oldest
			delete m_eventHistory.front();
            m_eventHistory.pop_front();
        }
#else
        // Release version: delete the handled event immediately
		delete event;
#endif
	} 

	return err;
}

GAME_EVENT_ERR GameEventManager::AddCallback(GAME_EVENT type, 
											 GAME_EVENT_PRIORITY pri,
											 GameEventHookCallback *cb)
{
	Assert((type >= (GAME_EVENT) 0) && (type < GEV_MAX));
	if(type < (GAME_EVENT)0 || type >= GEV_MAX)
		return GEV_ERR_BadEvent;

	if(!m_hooks[type])
		m_hooks[type] = new GameEventHook(type);

	m_hooks[type]->AddCallback(cb, pri);
	return GEV_ERR_OK;
}

void GameEventManager::RemoveCallback
(
    GAME_EVENT              type,
	GameEventHookCallback * cb
)
{
	Assert((type >= (GAME_EVENT) 0) && (type < GEV_MAX));

	if ((type >= (GAME_EVENT) 0) && (type < GEV_MAX) && m_hooks[type])
    {
		m_hooks[type]->RemoveCallback(cb);
    }
}

GAME_EVENT_ERR GameEventManager::ActivateHook
(
    GAME_EVENT          type, 
											  GameEventArgList *args,
	sint32              startIndex,
	sint32 &            resumeIndex
)
{
	Assert((type >= (GAME_EVENT) 0) && (type < GEV_MAX));
	if(type < (GAME_EVENT) 0 || (type >= GEV_MAX) || !m_hooks[type])
		return GEV_ERR_OK;

	return m_hooks[type]->Activate(args, 0, resumeIndex);
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

char *GameEventManager::ArgToName(GAME_EVENT_ARGUMENT want)
{
	switch(want) {
		case GEA_Army: return "GEA_Army";
		case GEA_Unit: return "GEA_Unit";
		case GEA_City: return "GEA_City";
		case GEA_Pop: return "GEA_Pop";
		case GEA_Gold: return "GEA_Gold";
		case GEA_Path: return "GEA_Path";
		case GEA_MapPoint: return "GEA_MapPoint";
		case GEA_Player: return "GEA_Player";
		case GEA_Int: return "GEA_Int";
		case GEA_Direction: return "GEA_Direction";
		case GEA_Advance: return "GEA_Advance";
		case GEA_Wonder: return "GEA_Wonder";
		case GEA_Improvement: return "GEA_Improvement";
		case GEA_TradeRoute: return "GEA_TradeRoute";
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

const char *GameEventManager::GetArgString(GAME_EVENT ev) const
{
	Assert(ev >= (GAME_EVENT)0);
	Assert(ev < GEV_MAX);
	if(ev < (GAME_EVENT)0 || ev >= GEV_MAX) {
		return "";
	}
	return g_eventDescriptions[ev].args;
}

bool GameEventManager::CheckArg(sint32 num, char got, char want)
{
	EVENTLOG(("%s ", ArgCharToName(got)));

	if(got != want) {
#ifdef _DEBUG
		c3errors_ErrorDialog("GameEventManager", "Argument %d should be of type %s.  Stack: %s", num, ArgCharToName(want), c3debug_StackTrace());
#endif
		return false;
	}
	return true;
}

char GameEventManager::ArgChar(GAME_EVENT type, size_t index) const
{
	Assert(type >= (GAME_EVENT)0);
	Assert(type < GEV_MAX);
	if(type < (GAME_EVENT)0 || type >= GEV_MAX)
		return '\0';

	size_t count = 0;

	for
	(
	    char const * argString = g_eventDescriptions[type].args; 
	    *argString; 
	    ++argString
	)
	{
		Assert(*argString == '%' || *argString == '&' || *argString == '$');
		if(*argString != '%' && *argString != '&' && *argString != '$')
			return '\0';

		++argString;
		Assert(*argString != '\0');
		if(*argString == '\0')
			return '\0';

		if (count == index)
			return *argString;
		
		++count;
	}

	Assert(index > count);
	return '\0';
}

size_t GameEventManager::GetNumArgs(GAME_EVENT type) const
{
    Assert((type >= (GAME_EVENT) 0) && (type < GEV_MAX));
    if ((type < (GAME_EVENT) 0) || (type >= GEV_MAX))
        return 0;

    size_t count = 0;

    for
    (
        char const * argString = g_eventDescriptions[type].args; 
        *argString;
        ++argString
    )
    {
        Assert(*argString == '%' || *argString == '&' || *argString == '$');
        if (*argString != '%' && *argString != '$')
        {
            break;
        }

        ++argString;
        Assert(*argString != '\0');
        if (*argString == '\0')
        {
            break;
        }

        ++count;
    }

    return count;
}


bool GameEventManager::VerifyArgs(GAME_EVENT type, va_list *vl)
{
	Assert(type >= (GAME_EVENT)0);
	Assert(type < GEV_MAX);
	if(type < (GAME_EVENT)0 || type >= GEV_MAX)
		return false;

	GameEventDescription *desc = &g_eventDescriptions[type];
	char *argString = desc->args;

	bool done = false;
	GAME_EVENT_ARGUMENT nextArg;

	sint32 argNum = 0;
	bool isOptional = false;

	while(!done) {
		nextArg = va_arg(*vl, GAME_EVENT_ARGUMENT);

		if(nextArg == GEA_End && *argString == 0)
			return true;

		
		
		
		
		
		if(!isOptional) {
			
			Assert(*argString == '%' || *argString == '&' || *argString == '$');
			if(*argString != '%' && *argString != '&' && *argString != '$')
				return false;
			
			if(*argString == '&')
				isOptional = true;
		} else {
			Assert(*argString == '&');
			if(*argString != '&')
				return false;
		}

		if(isOptional && nextArg == GEA_End) {
			
			
			return true;
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

		switch(nextArg) {
			case GEA_Army:
				if(!CheckArg(argNum, *argString, GEAC_ARMY)) return false;
				a = va_arg(*vl, Army);
				DG_PRINT(EVENTLOGNAME, "0x%lx, ", a.m_id);
				break;
			case GEA_Unit:
				if(!CheckArg(argNum, *argString, GEAC_UNIT)) return false;
				u = va_arg(*vl, Unit);
				DG_PRINT(EVENTLOGNAME, "0x%lx, ", u.m_id);
				break;
			case GEA_City:
				if(!CheckArg(argNum, *argString, GEAC_CITY)) return false;
				c = va_arg(*vl, Unit);
				DG_PRINT(EVENTLOGNAME, "0x%lx, ", c.m_id);
				break;
			case GEA_Gold:
				if(!CheckArg(argNum, *argString, GEAC_GOLD)) return false;
				value = va_arg(*vl, sint32);
				DG_PRINT(EVENTLOGNAME, "%d, ", value);
				break;
			case GEA_Path:
				if(!CheckArg(argNum, *argString, GEAC_PATH)) return false;
				path = va_arg(*vl, Path *);
				DG_PRINT(EVENTLOGNAME, "0x%lx, ", path);
				break;
			case GEA_MapPoint:
				if(!CheckArg(argNum, *argString, GEAC_MAPPOINT)) return false;
				pos = va_arg(*vl, MapPoint);
				EVENTLOG(("(%d,%d), ", pos.x, pos.y));
				break;
			case GEA_Player:
				if(!CheckArg(argNum, *argString, GEAC_PLAYER)) return false;
				value = va_arg(*vl, sint32);
				DG_PRINT(EVENTLOGNAME, "%d, ", value);
				break;
			case GEA_Int:
				if(!CheckArg(argNum, *argString, GEAC_INT)) return false;
				value = va_arg(*vl, sint32);
				DG_PRINT(EVENTLOGNAME, "%d, ", value);
				break;
			case GEA_Direction:
				if(!CheckArg(argNum, *argString, GEAC_DIRECTION)) return false;
				value = va_arg(*vl, sint32);
				DG_PRINT(EVENTLOGNAME, "%d, ", value);
				break;

			case GEA_Wonder:
				if(!CheckArg(argNum, *argString, GEAC_WONDER)) return false;
				value = va_arg(*vl, sint32);
				DG_PRINT(EVENTLOGNAME, "%d, ", value);
				break;
			case GEA_Advance:
				if(!CheckArg(argNum, *argString, GEAC_ADVANCE)) return false;
				value = va_arg(*vl, sint32);
				DG_PRINT(EVENTLOGNAME, "%d, ", value);
				break;
			case GEA_Improvement:
				if(!CheckArg(argNum, *argString, GEAC_IMPROVEMENT)) return false;
				imp = va_arg(*vl, TerrainImprovement);
				DG_PRINT(EVENTLOGNAME, "0x%lx, ", imp.m_id);
				break;
			case GEA_TradeRoute:
				if(!CheckArg(argNum, *argString, GEAC_TRADEROUTE)) return false;
				route = va_arg(*vl, TradeRoute);
				DG_PRINT(EVENTLOGNAME, "0x%lx, ", route.m_id);
				break;
			case GEA_End:
				if(*(argString) != 0) {
#ifdef _DEBUG
					c3errors_ErrorDialog("GameEventManager", "Not enough arguments.  Stack: %s", c3debug_StackTrace());
#endif
					return false;
				}
				return true;
			default:
				Assert(false);
				return false;
		}
		argString++;
	}
	return false;
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
			Assert(*argString == '%' || *argString == '&' || *argString == '$');
			
			
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

GAME_EVENT GameEventManager::GetEventIndex(const MBCHAR *name) const
{
	GAME_EVENT e;
	for(e = (GAME_EVENT)0; e < GEV_MAX; e = GAME_EVENT(sint32(e) + 1)) {
		if(!stricmp(g_eventDescriptions[e].name, name))
			return e;
	}
	return GEV_MAX;
}

const char *GameEventManager::GetEventName(GAME_EVENT ev) const
{
	Assert(ev >= (GAME_EVENT)0);
	Assert(ev < GEV_MAX);
	if(ev < (GAME_EVENT)0 || ev >= GEV_MAX) {
		return "";
	}

	return g_eventDescriptions[ev].name;
}

bool GameEventManager::EventsPending() const
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
