











#pragma once

#ifndef __POLLUTION_H__
#define __POLLUTION_H__




#include "PollutionConst.h"

class CivArchive ;
class MapPoint;

#define k_TREND_DOWNWARD	-1
#define k_TREND_UPWARD		1
#define k_TREND_LEVEL		0
#define k_ROUNDS_BEFORE_DISASTER 10

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

#else

class Pollution ;

#endif 
