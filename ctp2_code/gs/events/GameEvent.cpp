//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : Game event
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
// - None
//
//----------------------------------------------------------------------------
//
// Modifications from the original Activision code:
//
// - Improved slic event debugging. (7-Nov-2007 Martin Gühmann)
// - An event is not executed if its arguments became invalid during
//   code execution. (7-Nov-2007 Martin Gühmann)
// - Corrected delete operator use in destructor.
//
//----------------------------------------------------------------------------

#include "c3.h"
#include "GameEvent.h"

#include "GameEventArgList.h"
#include "GameEventManager.h"   // g_gevManager

#include "SlicEngine.h"
#include "SlicObject.h"
#include "SlicFrame.h"
#include "SlicSegment.h"

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
	m_resumeIndex       (-1),   // Handling not started yet
	m_addedDuring       (addedDuring),
	m_serial            (serial),
	m_line              (-1),   // Line invalid
	m_file              (NULL),
	m_contextName       (NULL)
{
	/// @todo Improve code style
	// Instead of accessing global variables (which may or may not exist),
	// better pass the data as arguments.

	if(g_slicEngine->GetContext())
	{
		m_line = g_slicEngine->GetContext()->GetFrame()->GetCurrentLine();

		const char * file = g_slicEngine->GetContext()->GetFrame()->GetSlicSegment()->GetFilename();
		const char * name = g_slicEngine->GetContext()->GetFrame()->GetSlicSegment()->GetName();

		m_file        = new char[strlen(file) + 1];
		m_contextName = new char[strlen(name) + 1];

		strcpy(m_file       , file);
		strcpy(m_contextName, name);
	}
	else if(g_gevManager->GetHeadEvent()
	&&      g_gevManager->GetHeadEvent()->GetLine() >= 0
	){
		GameEvent * event = g_gevManager->GetHeadEvent();

		m_line = event->GetLine();

		const char * file = event->GetFile();
		const char * name = event->GetContextName();

		m_file        = new char[strlen(file) + 1];
		m_contextName = new char[strlen(name) + 1];

		strcpy(m_file       , file);
		strcpy(m_contextName, name);
	}
}

GameEvent::~GameEvent()
{
	delete m_argList;
	delete [] m_file;
	delete [] m_contextName;
}

GAME_EVENT_ERR GameEvent::Process()
{
	/// @todo Check whether there are valid situations when the data became invalid.
	// When a unit is killed or consumed in the main execution phase of the event,
	// its ID may have become invalid when reaching the 'post' execution phase.
	if(m_argList->TestArgs(m_type, this))
	{
		return g_gevManager->ActivateHook
		            (m_type, m_argList, m_resumeIndex, m_resumeIndex);
	}
	else
	{
		return GEV_ERR_ArgsInvalid;
	}
}
