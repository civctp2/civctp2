//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ header
// Description  : Pollution handling
// Id           : $Id:$
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
// - Microsoft C++ extensions marked for future GCC compilation.
// - Do not trigger disaster warnings when there is no pollution at all.
//
//----------------------------------------------------------------------------

#if defined(HAVE_PRAGMA_ONCE)
#pragma once
#endif

#ifndef __POLLUTION_H__
#define __POLLUTION_H__

//----------------------------------------------------------------------------
// Library imports
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Exported names
//----------------------------------------------------------------------------

class CivArchive;
class MapPoint;
class Pollution;

#define k_TREND_DOWNWARD    -1
#define k_TREND_UPWARD       1
#define k_TREND_LEVEL        0

// A warning is triggered when the estimated number of rounds until disaster is
// less than this number.
#define k_ROUNDS_BEFORE_DISASTER 10

//----------------------------------------------------------------------------
// Project imports
//----------------------------------------------------------------------------

#include "Player.h"
#include "PollutionConst.h"

//----------------------------------------------------------------------------
// Class declarations
//----------------------------------------------------------------------------

class Pollution
{
public:

//----------------------------------------------------------------------------
// Do not change anything in the types or order of the following variable 
// declarations. Doing so will break reading in of save files.
// See the Serialize implementation for more details.
//----------------------------------------------------------------------------

	sint32      m_eventTriggerNextRound;
	sint32      m_eventTriggered;
	sint32      m_trend;
	sint32      m_history[k_MAX_GLOBAL_POLLUTION_RECORD_TURNS];
	sint32      m_phase;
	sint32      m_gwPhase;
	sint32      m_next_level;

//----------------------------------------------------------------------------
// Changing the order below this line should not break anything.
//----------------------------------------------------------------------------

	// An arbitrary (large) number that will be returned by GetRoundsToNextDisaster
	// when there is no pollution at all.
	// It should be larger than k_ROUNDS_BEFORE_DISASTER, and larger than the values
	// that appear in FearPollution_MotivationEvent in MotivationEvent.cpp.
	static sint32 const ROUNDS_COUNT_IMMEASURABLE;
	
	static uint32 GetPollutionAtRound(const PLAYER_INDEX player, const sint32 round);

	Pollution();
	Pollution(CivArchive &archive);
	~Pollution();

	sint32 GetHistory(void) { return (m_history[0]); }
	void   SetHistory(sint32 pollution) { m_history[0] = pollution; }
	sint32 GetRoundsToNextDisaster(void);

	void   Serialize(CivArchive &archive);
	void   WarnPlayers();
	sint32 AtTriggerLevel(void);
	sint32 GetNextTrigger();
	sint32 GetTriggerLevel(sint32 phase);
	sint32 GetGlobalPollutionLevel();
	void   SetGlobalPollutionLevel(sint32 requiredPollution);
	sint32 GetTrend(void) const;
	void   BeginTurn(void);
	void   EndRound(void);

	
	sint32 GetPhase(void) { return m_phase; }

	void   AddNukePollution(const MapPoint &cpos);

private:
	sint32 CalcTrend(sint32 level[], sint32 numPoints, double &offset, double &slope);
	void   GotoNextLevel(void);

};

#endif // __POLLUTION_H__
