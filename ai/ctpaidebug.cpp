











#include "c3.h"

#include "CtpAiDebug.h"
#include "Army.h"

#ifdef _PLAYTEST

PLAYER_INDEX CtpAiDebug::s_debugPlayer = 2;
sint32 CtpAiDebug::s_debugGoalType = -1;
sint32 CtpAiDebug::s_debugArmies[k_MAX_ARMY_SIZE];


bool CtpAiDebug::DebugLogCheck(const sint32 player, const sint32 goal_type, const sint32 army_id)
{
	if (army_id != -1)
	{
		bool found = false;
		for (sint32 i = 0; i < k_MAX_ARMY_SIZE; i++)
		{
			if (s_debugArmies[i] == army_id)
				return true;
			
			
			if (s_debugArmies[i] != -1)
				found = true;
		}
		
		if (found)
			return false;
	}

	
	if (goal_type != -1 && s_debugGoalType != -1)
	{
		if (goal_type != s_debugGoalType)
			return false;
	}

	
	if (player != -1 && s_debugPlayer != -1)
	{
		if (player != s_debugPlayer)
			return false;
	}

	
	return true;
}


bool CtpAiDebug::IsDebugGoalTypeSet()
{
	return (s_debugGoalType != -1);
}


bool CtpAiDebug::IsDebugArmyIdSet()
{
	bool found = false;
	for (sint32 i = 0; i < k_MAX_ARMY_SIZE; i++)
	{
		if (s_debugArmies[i] != -1)
			return true;
	}
	return false;
}


void CtpAiDebug::SetDebugPlayer(const sint32 debug_player)
{
	s_debugPlayer = debug_player;
}


void CtpAiDebug::SetDebugGoalType(const sint32 goal_type)
{
	s_debugGoalType = goal_type;
}


void CtpAiDebug::SetDebugArmies(const CellUnitList & unit_list)
{
	for (sint32 i = 0; i < k_MAX_ARMY_SIZE; i++)
		{
			if (i < unit_list.Num())
				s_debugArmies[i] = unit_list.Get(i).GetArmy().m_id;
			else
				s_debugArmies[i] = -1;
		}
}

#endif _DEBUG
