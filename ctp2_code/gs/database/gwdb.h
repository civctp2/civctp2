












#pragma once
#ifndef __GW_DB_H__
#define __GW_DB_H__

#include "DB.h"
#include "GWRecord.h"
#include "gstypes.h"

class CivArchive ;
enum C3DIR;

class GlobalWarmingDatabase : public Database <GWRecord>
	{
public:

	GlobalWarmingDatabase() ;
    ~GlobalWarmingDatabase() ;
    
    sint32 Initialise(char *filename, C3DIR dir) ;

    sint32 ParseGlobalWarmingDatabase(char *filename, C3DIR dir);
    sint32 ParseAGlobalWarming(Token *pollToken) ;
	sint32 NumTriggers(void) const { return (m_nRec) ; }			
	sint32 NumChanges(void) const { return (m_rec[0].m_changes) ; }
	double NextToWaterBonus(void) const { return (m_rec[0].m_next_to_ocean_bonus) ; }
	double ChangeProbability(const TERRAIN_TYPES terrain, TERRAIN_TYPES &newtype) const ;
	TERRAIN_TYPES Change(const double baseProb, const TERRAIN_TYPES terrain) const ;
    GlobalWarmingDatabase(CivArchive &archive) ;



	} ; 

#endif
