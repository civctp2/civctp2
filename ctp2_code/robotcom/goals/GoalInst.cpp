#include "c3.h"

#include "MapPoint.h"
#include "ic3InstDB.h"
#include "civarchive.h"
#include "FlatPtr.h"

#include "ic3GameState.h"
#include "aimain.h"


#include "CityAgent.h"

#include "GoalInst.h"
#include "AiMap.h"

GoalInstallation::GoalInstallation()
{ 
    m_type = -1; 

	m_pos = NULL;
    m_home_city_agent = NULL; 
    m_next = NULL; 

    m_cost = 0; 
    m_utility = 0.0; 
}

GoalInstallation::GoalInstallation(sint32 t, sint32 extra_data, MapPointData &pos, 
	double u, CityAgent *ca, sint32 cost)
{
    m_type = t; 
    m_pos = new MapPointData; 
    *m_pos = pos; 
    m_utility = u; 
    m_home_city_agent = ca; 
    m_next = NULL; 
    m_cost = cost; 
    m_extra_data = extra_data;
}

GoalInstallation::GoalInstallation (BSet<CityAgent> *bs, CivArchive &archive)
{
    Serialize(bs, archive); 
}


GoalInstallation::~GoalInstallation()
{

    if (m_pos) { 
    	delete m_pos;
        m_pos = NULL; 
    } 

    return;
}

ZEROMEM(GoalInstallationFlat); 
FLATSERIALIZE(GoalInstallationFlat);

void GoalInstallation::Serialize(BSet<CityAgent> *bs, CivArchive &archive)
{
    CHECKSERIALIZE

    GoalInstallationFlat::Serialize(archive); 
    
    
    BSetID id;
    uint32 v; 

    CHECKSERIALIZE

    if (archive.IsStoring()) {
		BOOL	hasPos = (m_pos != NULL);

		archive.PutUINT8((uint8)hasPos);

		if (hasPos) {
			archive << m_pos->x; 
			archive << m_pos->y; 
			archive << m_pos->z;
		}

		BOOL hasAgent = (m_home_city_agent != NULL);
		
		archive.PutUINT8((uint8)hasAgent);
		
		if (hasAgent) {
			id = m_home_city_agent->GetID(); 
			archive << id.GetVal(); 
		}
    } else {
		BOOL	hasPos;

		hasPos = (BOOL)archive.GetUINT8();
		if (hasPos) {
			m_pos = new MapPointData; 
			archive >> m_pos->x; 
			archive >> m_pos->y; 
			archive >> m_pos->z;
		} else {
			m_pos = NULL;
		}

		BOOL	hasAgent;

		hasAgent = (BOOL)archive.GetUINT8();
		if (hasAgent) {
			archive >> v; 
			id = v; 
			m_home_city_agent = bs->Find(id); 
			Assert(m_home_city_agent); 
		} else {
			m_home_city_agent = NULL;
		}
    }
}

sint32 GoalInstallation::GetInstallationType() const
{
    return m_type;
} 

double GoalInstallation::GetUtility() const
{ 
    return m_utility; 
} 

void GoalInstallation::SetUtility(const double u)
{ 
    m_utility = u;
} 

void GoalInstallation::GetPos(MapPointData &pos) const

{ 
     pos = *m_pos; 
} 

sint32 GoalInstallation::GetCost() const 
{ 
    return m_cost;
} 

void GoalInstallation::Construct(AiMain *ai, sint32 &stored)
{
    BOOL construct_ok=FALSE; 

    if (m_cost <= stored) { 
        construct_ok = ai->m_installationDB->ConstructInstallation(m_type, m_extra_data, m_pos);
        Assert(construct_ok); 
#ifdef _DEBUG
        if (!construct_ok) { 
            construct_ok = ai->m_installationDB->ConstructInstallation(m_type, m_extra_data, m_pos);
        } 
#endif
    } 
    stored -= m_cost; 

    switch (m_type) { 
    case INSTALLATION_LISTENING_POSTS: 
        ai->m_map->IncGuardtowerCellCount(*m_pos); 
        break; 
    case INSTALLATION_FORTIFICATIONS:
        ai->m_map->IncFortCellCount(*m_pos); 
        break;
    case INSTALLATION_AIR_BASES:  
        ai->m_map->IncAirfieldCellCount(*m_pos); 
        break;
    default:
        break; 
    } 

}

CityAgent *GoalInstallation::GetHomeCity()
{  
    return m_home_city_agent; 
}

BOOL GoalInstallation::SamePosition(GoalInstallation *test_goal)
{
    return ((m_pos->x == test_goal->m_pos->x) &&
        (m_pos->y == test_goal->m_pos->y) &&
        (m_pos->z == test_goal->m_pos->z)) ;
}



