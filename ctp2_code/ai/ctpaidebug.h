











#pragma once
#ifndef __CTP_AI_DEBUG_H__
#define __CTP_AI_DEBUG_H__

#ifdef _PLAYTEST

#include "CellUnitList.h"

#ifdef _DEBUG
	#define AI_DPRINTF(mask, player, goal_type, army_id, x) { if (CtpAiDebug::DebugLogCheck(player, goal_type, army_id)) DPRINTF(mask,x); }
#else
	#define AI_DPRINTF(mask, player, goal_type, army_id, x)
#endif

class CtpAiDebug {
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
	#define AI_DPRINTF(mask, player, goal_type, army_id, x)	;
#endif _DEBUG

#endif __CTP_AI_DEBUG_H__
