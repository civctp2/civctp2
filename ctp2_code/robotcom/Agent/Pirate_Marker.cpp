
#include "c3.h"

#include "CivArchive.h"
#include "BSet.h"
#include "Pirate_Marker.h"


Pirate_Marker::Pirate_Marker(const sint32 owner, const uint32 id, 
   const sint32 round)
{

    m_other_owner = owner;
    m_other_city = id; 
    m_when_pirated = round; 
    m_no_trade_period = 10; 
    m_next = NULL; 
}

Pirate_Marker::~Pirate_Marker()
{

}

Pirate_Marker::Pirate_Marker(CivArchive &archive)
{ 
    Serialize(archive); 
} 

void Pirate_Marker::Serialize(CivArchive &archive)
{
    m_other_city.Serialize(archive); 

    if (archive.IsStoring()) { 
        archive << m_other_owner; 
        archive << m_when_pirated; 
        archive << m_no_trade_period; 
    } else { 
        archive >> m_other_owner; 
        archive >> m_when_pirated; 
        archive >> m_no_trade_period; 
    }
}


void Pirate_Marker::RegisterPiracy(const sint32 when_pirated)
{
    m_when_pirated = when_pirated; 
	m_no_trade_period += 10; 
}

BOOL Pirate_Marker::IsSafe(const sint32 current_time)
{
    return (m_when_pirated + m_no_trade_period) < current_time; 
}
