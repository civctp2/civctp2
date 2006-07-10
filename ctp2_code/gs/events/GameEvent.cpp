//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : Game event
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

#include "c3.h"
#include "GameEvent.h"

#include "GameEventArgList.h"
#include "GameEventManager.h"   // g_gevManager

GameEvent::GameEvent
(
    GAME_EVENT          type, 
    GameEventArgList *  args, 
    sint32              serial, 
    GAME_EVENT          addedDuring
)
:
    m_type              (type),
	m_argList           (args),
	m_resumeIndex       (-1),   // handling not started yet
	m_addedDuring       (addedDuring),
	m_serial            (serial)
{ }

GameEvent::~GameEvent()
{
	delete m_argList;
}

GAME_EVENT_ERR GameEvent::Process()
{
	return g_gevManager->ActivateHook
                (m_type, m_argList, m_resumeIndex, m_resumeIndex);
}

