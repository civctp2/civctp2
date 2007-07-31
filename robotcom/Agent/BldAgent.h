
#ifndef __BUILDING_AGENT_H__
#define __BUILDING_AGENT_H__ 1

enum BUILDING_STATE 
{
	BUILDING_STATE_UNBUILT, 
	BUILDING_STATE_BUILDING, 
	BUILDING_STATE_DONE
} ;

class CivArchive; 
class CityAgent;

class BuildingAgentFlat
{ 

protected:

    BUILDING_STATE m_state; 
    sint16 m_building_type; 
    void Serialize (CivArchive &archive); 
};

class BuildingAgent : public BuildingAgentFlat 
{ 

public:

    BuildingAgent(); 
    void Serialize (CivArchive &archive); 
    void SetType (sint16 t); 
    void SetState (const BUILDING_STATE s); 

    BOOL CanConstructBuilding(AiMain *ai);
    double CalcPayoff(AiMain *ai, CityAgent *ca); 

    void RegisterCreateBuilding(); 
    void RegisterLostBuilding(); 

    BUILDING_STATE GetState() const { return m_state; }
};

#endif __BUILDING_AGENT_H__
