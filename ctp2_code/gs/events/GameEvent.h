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

#if defined(HAVE_PRAGMA_ONCE)
#pragma once
#endif

#ifndef __GAME_EVENT_H__
#define __GAME_EVENT_H__

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

	sint32              GetSerial() const   { return m_serial; };
	GAME_EVENT          GetType() const     { return m_type; };

	GAME_EVENT_ERR      Process();

private:
	GAME_EVENT          m_type;
	GameEventArgList *  m_argList;
	sint32              m_resumeIndex;
	GAME_EVENT          m_addedDuring;
	sint32              m_serial;
};
	
#endif
