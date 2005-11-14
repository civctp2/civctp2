



#include "c3.h"
#include "GameEvent.h"
#include "GameEventArgList.h"
#include "GameEventManager.h"

GameEvent::GameEvent(GAME_EVENT type, GameEventArgList *args, sint32 serial, GAME_EVENT addedDuring)
{
	m_type = type;
	m_argList = args;
	m_serial = serial;
	m_resumeIndex = -1;
	m_addedDuring = addedDuring;
}

GameEvent::~GameEvent()
{
	delete m_argList;
}

GAME_EVENT_ERR GameEvent::Process(BOOL &complete)
{
	GAME_EVENT_ERR const    err = 
        g_gevManager->ActivateHook(m_type, m_argList, m_resumeIndex, m_resumeIndex);
    complete = (err != GEV_ERR_NeedUserInput);
	return err;
}

