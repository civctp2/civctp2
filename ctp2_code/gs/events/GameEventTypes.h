//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C/C++ header
// Description  : Game event types
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
// - Added GEV_ERR_ArgsInvalid to GAME_EVENT_ERR. (7-Nov-2007 Martin Gühmann)
//
//----------------------------------------------------------------------------

#ifdef HAVE_PRAGMA_ONCE
#pragma once
#endif
#ifndef GAME_EVENT_TYPES_H_
#define GAME_EVENT_TYPES_H_

#include "GameEventDescription.h"

enum GAME_EVENT_INSERT {
	GEV_INSERT_Front,
	GEV_INSERT_AfterCurrent,
	GEV_INSERT_Tail
};

enum GAME_EVENT_ERR {
	GEV_ERR_OK,
	GEV_ERR_Internal,
	GEV_ERR_BadInsert,
	GEV_ERR_BadEvent,
	GEV_ERR_NeedUserInput,
	GEV_ERR_WrongArguments,
	GEV_ERR_StopProcessing,
	GEV_ERR_AtBreak,
	GEV_ERR_ArgsInvalid
};

enum GAME_EVENT_HOOK_DISPOSITION {
	GEV_HD_Continue,
	GEV_HD_Stop,
	GEV_HD_NeedUserInput,

	GEV_HD_MAX
};

enum GAME_EVENT_ARGUMENT {
	GEA_Null,

	GEA_Army,
	GEA_Unit,
	GEA_City,
	GEA_Pop,
	GEA_Gold,
	GEA_Path,
	GEA_MapPoint,
	GEA_Player,
	GEA_Int,
	GEA_Direction,
	GEA_Advance,
	GEA_Wonder,
	GEA_Improvement,
	GEA_TradeRoute,

	GEA_End
};








#define GEAC_ARMY 'a'
#define GEAC_UNIT 'u'
#define GEAC_CITY 'c'
#define GEAC_POP 'C'
#define GEAC_GOLD 'g'
#define GEAC_PATH 'p'
#define GEAC_MAPPOINT 'l'
#define GEAC_PLAYER 'P'
#define GEAC_INT 'i'
#define GEAC_DIRECTION 'd'

#define GEAC_ADVANCE 'A'
#define GEAC_WONDER 'w'
#define GEAC_IMPROVEMENT 'I'
#define GEAC_TRADEROUTE 't'

/// Execution stage of an event
enum GAME_EVENT_PRIORITY {
	/// SLIC 'pre' handler stage, executed before the event has taken place.
	/// At this stage, the actual processing can be prevented by returning a
	/// stop code.
	GEV_PRI_Pre,
	/// Source code handler stage, doing the actual processing.
	GEV_PRI_Primary,
	/// SLIC 'post' handler stage, signalling that the event has been executed.
	GEV_PRI_Post
};

#endif
