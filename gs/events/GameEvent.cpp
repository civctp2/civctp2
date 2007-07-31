



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
	if(m_argList) {
		delete m_argList;
		m_argList = NULL;
	}
}

GAME_EVENT_ERR GameEvent::Process(BOOL &complete)
{
	complete = FALSE;

	GAME_EVENT_ERR err;
	if(m_resumeIndex >= 0)
		err = g_gevManager->ResumeHook(m_type, m_argList, m_resumeIndex, m_resumeIndex);
	else
		err = g_gevManager->ActivateHook(m_type, m_argList, m_resumeIndex);

	if(err != GEV_ERR_NeedUserInput)
		complete = TRUE;

	return err;
}

