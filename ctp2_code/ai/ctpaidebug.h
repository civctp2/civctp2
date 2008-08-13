//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ header
// Description  : Debug Ai File
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
// _PLAYTEST
// - If defined allows extensive logging
//
// USE_LOGGING
// - Enable logging when set, even when not a debug version. This is not
//   original Activision code.
//
// _DEBUG
// - Generate debug version
//
//----------------------------------------------------------------------------
//
// Modifications from the original Activision code:
//
// - USE_LOGGING now works in a final version. (30-Jun-2008 Martin Gühmann)
//
//----------------------------------------------------------------------------

#pragma once
#ifndef __CTP_AI_DEBUG_H__
#define __CTP_AI_DEBUG_H__

#if defined(_PLAYTEST) || defined(USE_LOGGING)

#include "cellunitlist.h"

#if defined(_DEBUG) || defined(USE_LOGGING)
	#define AI_DPRINTF(mask, player, goal_type, army_id, x) { if (CtpAiDebug::DebugLogCheck(player, goal_type, army_id)) DPRINTF(mask,x); }
#else
	#define AI_DPRINTF(mask, player, goal_type, army_id, x)
#endif

class CtpAiDebug
{
public:

	static bool DebugLogCheck(const sint32 player, const sint32 goal_type, const sint32 army_id);


	static bool IsDebugGoalTypeSet();


	static bool IsDebugArmyIdSet();


	static void SetDebugPlayer(const sint32 debug_player);


	static void SetDebugGoalType(const sint32 goal_type);


	static void SetDebugArmies(const CellUnitList & unit_list);

private:

	static PLAYER_INDEX s_debugPlayer;
	static sint32 s_debugGoalType;
	static sint32 s_debugArmies[k_MAX_ARMY_SIZE];

};
#else
	#define AI_DPRINTF(mask, player, goal_type, army_id, x);
#endif // _DEBUG

#endif // __CTP_AI_DEBUG_H__
