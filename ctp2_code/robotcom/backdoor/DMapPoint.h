

#pragma once

#ifndef __D_MAP_POINT_H__
#define __D_MAP_POINT_H__

#include "IMapPointData.h"

class CivArchive; 

class DMapPoint : public MapPointData { 

public:

    DMapPoint(); 
    ~DMapPoint(); 

    void Serialize(CivArchive &archive); 
    void Castrate() {}
    void DelPointers() {} 
};


#endif __D_MAP_POINT_H__