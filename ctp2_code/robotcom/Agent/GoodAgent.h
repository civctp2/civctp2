

#ifndef __GOOD_AGENT_H__
#define __GOOD_AGENT_H__ 1

#include "BSet.h"

class CivArchive; 
struct MapPointData; 

class GoodAgentFlat { 

protected:
    sint16 m_type_good;
    BOOL m_is_connected; 
    PLAYER_INDEX m_dest_owner; 

public:
    GoodAgentFlat(); 
    void Serialize(CivArchive &archive); 
}; 


class GoodAgentPtr { 
protected:
    MapPointData *m_pos; 
    BSetID m_dest_id; 

public:
    GoodAgentPtr(); 
    void Serialize(CivArchive &archive); 
    void Store(CivArchive &archive);
    void Load(CivArchive &archive, sint32 pcount, uint8 nmask[]);
}; 


class GoodAgent : public GoodAgentFlat, public GoodAgentPtr { 

public:

    GoodAgent (MapPointData &pos, sint32 type_good); 
    ~GoodAgent(); 
    GoodAgent (CivArchive &archive); 

    void Serialize(CivArchive &archive); 

    BOOL IsConnected(); 
    void SetIsConnected(BOOL c, sint32 dest_owner, BSetID dest_id);
    BOOL OwnsGoodAt(MapPointData &pos);
    sint32 GetType() const { return m_type_good; }
    void GetPos(MapPointData &pos) const;

    BOOL MatchesRoute(const PLAYER_INDEX dest_owner, const uint32 dest_id, 
        const sint32 resource) const;

    BOOL UnconnectedMatchesRoute(const sint32 resource) const;

    void CutRoute();
    sint32 GetDestOwner() const { return m_dest_owner; } 
}; 

#endif  __GOOD_AGENT_H__

