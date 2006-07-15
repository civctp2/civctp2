













#include "c3.h"
#include "astarpnt.h"

#if 0



sint32 AstarPoint::operator<(const AstarPoint &rhs) const
{ 
    if (m_total_cost < rhs.m_total_cost) {
        return TRUE;
    } else if ((m_total_cost - rhs.m_total_cost) > 0.000001) {
        return FALSE;
    } else {
        if (m_pos.x != rhs.m_pos.x) {
            return(m_pos.x < rhs.m_pos.x);
        } else if (m_pos.y != rhs.m_pos.y) {
            return(m_pos.y < rhs.m_pos.y);
        } else {
            return 0;
        }
    }
}
#endif

sint32  AstarPoint::Identical(const AstarPoint &rhs) const
{
    return m_pos == rhs.m_pos;
}


void AstarPoint::Clear()
{
    m_flags = (uint32) 0xcdcd;
    m_parent = (AstarPoint *) 0xcdcdcdcd; 
    m_next = (AstarPoint *)0xcdcdcdcd; 
	m_pos.x = (sint16)(0xcdcd); 
	m_past_cost = (float) 0xcdcdcdcd; 
    m_entry_cost = (float) 0xcdcdcdcd; 
    m_future_cost = (float) 0xcdcdcdcd; 
    m_total_cost = (float) 0xcdcdcdcd;
}
