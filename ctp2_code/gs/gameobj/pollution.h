//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ header
// Description  : 
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


class Pollution;

#define k_TREND_DOWNWARD	-1
#define k_TREND_UPWARD		1
#define k_TREND_LEVEL		0

// A warning is triggered when the estimated number of rounds until disaster is 
// less than this number.
#define k_ROUNDS_BEFORE_DISASTER 10		

#include "Player.h"
#include "PollutionConst.h"

class CivArchive ;
class MapPoint;


class Pollution
	{ 
public:
	
	
	sint32	m_eventTriggerNextRound,

			m_eventTriggered,

			m_trend,												
			m_history[k_MAX_GLOBAL_POLLUTION_RECORD_TURNS],
			m_phase ;												
	sint32  m_gwPhase;


	
	
	
	

	sint32	m_next_level ;											

	// An arbitrary (large) number that will be returned by GetRoundsToNextDisaster
	// when there is no pollution at all.
	// It should be larger than k_ROUNDS_BEFORE_DISASTER, and larger than the values
	// that appear in FearPollution_MotivationEvent in MotivationEvent.cpp.
	static sint32 const	ROUNDS_COUNT_IMMEASURABLE;	
	
	static uint32 GetPollutionAtRound(const PLAYER_INDEX player, const sint32 round);

	Pollution() ;
	Pollution(CivArchive &archive) ;
	~Pollution() ;

	sint32 GetHistory(void) { return (m_history[0]) ; }
	void SetHistory(sint32 pollution) { m_history[0] = pollution ; }
	sint32 GetRoundsToNextDisaster(void) ;

	void Serialize(CivArchive &archive) ;
	void WarnPlayers() ;
	sint32 AtTriggerLevel(void) ;
	sint32 GetNextTrigger() ;
	sint32 GetTriggerLevel(sint32 phase) ;
	sint32 GetGlobalPollutionLevel();
	void SetGlobalPollutionLevel(sint32 requiredPollution);
	sint32 GetTrend(void) const ;
	void BeginTurn(void) ;
	void EndRound(void) ;

	
	sint32 GetPhase(void) { return m_phase; }

	void AddNukePollution(const MapPoint &cpos);

private:
	sint32 CalcTrend(sint32 level[], sint32 numPoints, double &offset, double &slope) ;
	void GotoNextLevel(void) ;

	};

#endif 
