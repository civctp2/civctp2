












#pragma once
#ifndef __POLLUTION_RECORD_H__
#define __POLLUTION_RECORD_H__

#include "Rec.h"

class Token;

enum POLLUTION_DISASTER_TYPE
	{
	POLLUTION_DISASTER_TYPE_OZONE,
	POLLUTION_DISASTER_TYPE_FLOOD,
	POLLUTION_DISASTER_TYPE_WARNING,
	POLLUTION_DISASTER_TYPE_INVALID,
	POLLUTION_DISASTER_TYPE_MAX_VAL,
	} ;

#define MAX_POLLUTION_TRIGGERS 20



class CivArchive ;

class Token ;

class PollutionRecord : public Record
	{
	public :
		
		
		
		sint32	m_numTriggers;
		sint32	m_triggerLevel[MAX_POLLUTION_TRIGGERS];
		sint32	m_triggerType[MAX_POLLUTION_TRIGGERS];

		

		
		
		

		
		

		PollutionRecord() ;

		sint32 GetNumTriggers(void) const { return m_numTriggers; }
		sint32 GetTriggerLevel(sint32 trigger) const { return (m_triggerLevel[trigger]) ; }
		sint32 GetDisasterType(sint32 trigger) const { return (m_triggerType[trigger]) ; }

		sint32 ParsePollutionRecord(Token *token) ;
		void Serialize(CivArchive &archive) ;

	} ;

#endif
