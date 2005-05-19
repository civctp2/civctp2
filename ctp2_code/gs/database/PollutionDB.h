#pragma once
#ifndef __POLLUTION_DB_H__
#define __POLLUTION_DB_H__

#include "DB.h"
#include "pollutionrecord.h"
#include "pollutiontoken.h"


class CivArchive ;
enum C3DIR;
enum MAPSIZE;

class PollutionDatabase : public Database <PollutionRecord>
	{
	public :

		PollutionDatabase() ;
		~PollutionDatabase() ;
    
		sint32 Initialise(char *filename, C3DIR dir) ;

		sint32 ParsePollutionDatabase(char *filename, C3DIR dir);
		sint32 ParseAPollution(Token *pollToken, TOKEN_POLLUTION which) ;

		sint32 NumTriggers(MAPSIZE size) const { return (m_rec[(sint32)size].GetNumTriggers()) ; }	
		
		sint32 GetTrigger(MAPSIZE size, const sint32 phase) const 
		{ 
			Assert((sint32)size >= 0 && (sint32)size < m_nRec);
			Assert((phase>=0) && (phase<m_rec[size].GetNumTriggers())) ; 
			return (m_rec[(sint32)size].GetTriggerLevel(phase)) ; 
		}
		
		sint32 GetDisasterType(MAPSIZE size, const sint32 phase) const 
		{ 
			Assert((sint32)size >= 0 && (sint32)size < m_nRec);
			Assert((phase>=0) && (phase<m_rec[size].GetNumTriggers())) ; 
			return (m_rec[(sint32)size].GetDisasterType(phase)) ; 
		}
		
		PollutionDatabase(CivArchive &archive) ;



	} ; 

#endif
