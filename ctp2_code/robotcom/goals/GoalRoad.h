


#ifndef __GoalRoad_H__
#define __GoalRoad_H__ 1

class CityAgent; 
struct MapPointData; 

#include "GoalInst.h"

class GoalRoad : public GoalInstallation { 

    CityAgent *m_start; 
    CityAgent *m_dest; 

    sint32 m_bufSize; 
    MapPointData *m_buffer; 
    sint32 m_nPoints;
    sint32 m_road_type; 
    sint32 m_total_cost; 
public:

    GoalRoad(AiMain *ai, const sint32 rt, CityAgent *sa, CityAgent *da, 
        sint32 bufSize,  MapPointData *buffer, sint32 nPoints); 

    ~GoalRoad(); 
    BOOL Validate(AiMain *ai); 


    sint32 GetInstallationType() const; 
    void GetPos(MapPointData &pos);
    sint32 GetCost() const; 
    void Construct(AiMain *ai, sint32 &stored); 

};

#endif __GoalRoad_H__

