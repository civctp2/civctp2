#include "c3.h"
#include "Globals.h"

#include "GoodAgent.h"

#include "CivArchive.h"
#include "FlatPtr.h"
#include "MapPoint.h"

#include "IC3GameState.h"
#include "AiMain.h"



GoodAgent::GoodAgent (MapPointData &pos, sint16 tg)
{
    m_type_good = tg; 
    m_is_connected = FALSE; 
    m_pos = new MapPointData; 
     *m_pos = pos; 

    m_dest_owner = -1; 
    m_dest_id = 0; 
}

GoodAgent::~GoodAgent()
{ 
    NDELETE (m_pos); 
} 


GoodAgent::GoodAgent (CivArchive &archive)
{
    Serialize(archive); 
}

ZEROMEM(GoodAgentFlat); 
FLATSERIALIZE(GoodAgentFlat); 
ZEROMEM(GoodAgentPtr); 
PTRSERIALIZE(GoodAgentPtr); 

void GoodAgentPtr::Store(CivArchive &archive)
{
    archive << m_pos->x; 
    archive << m_pos->y; 
    archive << m_pos->z; 
    archive.PutUINT32(m_dest_id.GetVal()); 
}

void GoodAgentPtr::Load(CivArchive &archive, sint32 pcount, uint8 nmask[])
{
    m_pos = new MapPointData; 

    archive >> m_pos->x; 
    archive >> m_pos->y; 
    archive >> m_pos->z; 

    m_dest_id = archive.GetUINT32(); 
}

void GoodAgent::Serialize(CivArchive &archive)
{    
    CHECKSERIALIZE

    GoodAgentFlat::Serialize(archive); 
    GoodAgentPtr::Serialize(archive); 
}

BOOL GoodAgent::IsConnected()
{ 
    return m_is_connected; 
}

void GoodAgent::SetIsConnected(BOOL c, PLAYER_INDEX dest_owner, BSetID dest_id)
{
    m_is_connected = c; 
    m_dest_owner = dest_owner; 
    m_dest_id = dest_id; 
}

BOOL GoodAgent::OwnsGoodAt(MapPointData &pos)
{
    return ((m_pos->x == pos.x) &&
        (m_pos->y == pos.y) &&
        (m_pos->z == pos.z)); 
}


void GoodAgent::GetPos(MapPointData &pos) const
{ 
    pos = *m_pos; 
}

BOOL GoodAgent::MatchesRoute(const PLAYER_INDEX dest_owner, const uint32 dest_id, 
                             const sint32 resource) const 
{
    return (m_dest_owner == dest_owner) &&
            (m_type_good == resource) && 
            (dest_id == m_dest_id.GetVal());
}

BOOL GoodAgent::UnconnectedMatchesRoute(const sint32 resource) const
{ 
    return !m_is_connected && (m_type_good == resource); 
} 

void GoodAgent::CutRoute()
{

    m_dest_owner = -1; 
    m_dest_id = 0; 
}


