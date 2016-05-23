//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C/C++ header
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
// HAVE_PRAGMA_ONCE
//
//----------------------------------------------------------------------------
//
// Modifications from the original Activision code:
//
// - Improved slic event debugging. (7-Nov-2007 Martin Gühmann)
//
//----------------------------------------------------------------------------

#if defined(HAVE_PRAGMA_ONCE)
#pragma once
#endif

#ifndef GAME_EVENT_H_
#define GAME_EVENT_H_

//----------------------------------------------------------------------------
// Library dependencies
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Export overview
//----------------------------------------------------------------------------

class GameEvent;

//----------------------------------------------------------------------------
// Project dependencies
//----------------------------------------------------------------------------

#include "ctp2_inttypes.h"          // sint32
#include "GameEventDescription.h"   // GAME_EVENT
#include "GameEventTypes.h"         // GAME_EVENT_ERR

class GameEventArgList;

//----------------------------------------------------------------------------
// General declarations
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Class declarations
//----------------------------------------------------------------------------

class GameEvent
{
public:
	GameEvent
	(
	    GAME_EVENT          type,
	    GameEventArgList *  args,
	    sint32              serial,
	    GAME_EVENT          addedDuring
	);
	~GameEvent();

	sint32              GetSerial()      const   { return m_serial;      };
	GAME_EVENT          GetType()        const   { return m_type;        };
	sint32              GetLine()        const   { return m_line;        };
	char *              GetFile()        const   { return m_file;        };
	char *              GetContextName() const   { return m_contextName; };
	GAME_EVENT          AddedDuring()    const   { return m_addedDuring; };

	GAME_EVENT_ERR      Process();

	/// Check whether an event type is valid
	/// \remarks GEA_End is considered invalid
	static bool IsTypeValid(GAME_EVENT a_Type)
	{
		return (a_Type >= (GAME_EVENT) 0) && (a_Type < GEV_MAX);
	}

private:
	GAME_EVENT          m_type;
	GameEventArgList *  m_argList;
	sint32              m_resumeIndex;
	GAME_EVENT          m_addedDuring;
	sint32              m_serial;
	sint32              m_line;
	char *              m_file;
	char *              m_contextName;
};

#endif
