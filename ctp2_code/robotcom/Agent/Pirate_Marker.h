
#pragma once

#ifndef __Pirate_Marker__
#define __Pirate_Marker__ 1

#include "bset.h"

class CityAgent; 
class CivArchive; 
class CityAgentPtr;

class Pirate_Marker { 

    sint32 m_other_owner;
	BSetID m_other_city; 
	sint32 m_when_pirated; 
	sint32 m_no_trade_period; 
    Pirate_Marker *m_next; 

public:

  	friend CityAgentPtr;
	friend CityAgent;

	Pirate_Marker(const sint32 other_owner, const uint32 other_city, 
        const sint32 round); 

    Pirate_Marker(CivArchive &archive);

	~Pirate_Marker(); 

    void Serialize(CivArchive &archive); 

    void RegisterPiracy(const sint32 when_pirated);

    BOOL IsSafe(const sint32 current_time); 
};

#endif __Pirate_Marker__
