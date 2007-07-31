













#pragma once
#ifndef __AI_CELL_H__
#define __AI_CELL_H__ 1




#define k_TERRRAIN_TYPE_MASK 0xf8000000  
#define k_TERRRAIN_TYPE_SHIFT 27

#define k_SHIFTED_TERRAIN_FOREST (TERRAIN_FOREST<<k_TERRRAIN_TYPE_SHIFT)
#define k_SHIFTED_TERRAIN_PLAINS (TERRAIN_PLAINS<<k_TERRRAIN_TYPE_SHIFT)
#define k_SHIFTED_TERRAIN_TUNDRA (TERRAIN_TUNDRA<<k_TERRRAIN_TYPE_SHIFT)
#define k_SHIFTED_TERRAIN_GLACIER (TERRAIN_GLACIER<<k_TERRRAIN_TYPE_SHIFT)
#define k_SHIFTED_TERRAIN_GRASSLAND (TERRAIN_GRASSLAND<<k_TERRRAIN_TYPE_SHIFT)
#define k_SHIFTED_TERRAIN_DESERT (TERRAIN_DESERT<<k_TERRRAIN_TYPE_SHIFT)
#define k_SHIFTED_TERRAIN_SWAMP (TERRAIN_SWAMP<<k_TERRRAIN_TYPE_SHIFT)
#define k_SHIFTED_TERRAIN_JUNGLE (TERRAIN_JUNGLE<<k_TERRRAIN_TYPE_SHIFT)
#define k_SHIFTED_TERRAIN_MOUNTAIN (TERRAIN_MOUNTAIN<<k_TERRRAIN_TYPE_SHIFT)
#define k_SHIFTED_TERRAIN_HILL (TERRAIN_HILL<<k_TERRRAIN_TYPE_SHIFT)
#define k_SHIFTED_TERRAIN_WATER_SHALLOW (TERRAIN_WATER_SHALLOW<<k_TERRRAIN_TYPE_SHIFT)
#define k_SHIFTED_TERRAIN_WATER_DEEP (TERRAIN_WATER_DEEP<<k_TERRRAIN_TYPE_SHIFT)
#define k_SHIFTED_TERRAIN_WATER_VOLCANO (TERRAIN_WATER_VOLCANO<<k_TERRRAIN_TYPE_SHIFT)
#define k_SHIFTED_TERRAIN_SPACE (TERRAIN_SPACE<<k_TERRRAIN_TYPE_SHIFT)
#define k_SHIFTED_TERRAIN_WATER_BEACH (TERRAIN_WATER_BEACH<<k_TERRRAIN_TYPE_SHIFT)
#define k_SHIFTED_TERRAIN_WATER_SHELF (TERRAIN_WATER_SHELF<<k_TERRRAIN_TYPE_SHIFT)
#define k_SHIFTED_TERRAIN_WATER_TRENCH (TERRAIN_WATER_TRENCH<<k_TERRRAIN_TYPE_SHIFT)
#define k_SHIFTED_TERRAIN_WATER_RIFT (TERRAIN_WATER_RIFT<<k_TERRRAIN_TYPE_SHIFT)
#define k_SHIFTED_TERRAIN_DEAD (TERRAIN_DEAD<<k_TERRRAIN_TYPE_SHIFT)
#define k_SHIFTED_TERRAIN_UNEXPLORED (TERRAIN_UNEXPLORED<<k_TERRRAIN_TYPE_SHIFT)   


enum GRADIENT_BITS { 
   GRADIENT_0_FIVE, 
   GRADIENT_1_SIX, 
   GRADIENT_2_SEVEN, 
   GRADIENT_3_SEVEN, 
   GRADIENT_4_SEVEN, 
   NUM_GRADIENT
};


struct MapPointData; 
class AiMain; 
class CivArchive; 
class AiCell; 


class AiCellPtr { 
    
    sint16 x, y;
    sint16 m_queue_idx; 

public:

    AiCellPtr();
    ~AiCellPtr();
    void Init(); 

    
    void* operator new(size_t); 
    void operator delete(void *);

    AiCellPtr& operator= (AiCell* set_me);
    AiCellPtr& operator= (AiCellPtr &set_me);

    sint32 operator<(const AiCellPtr &rhs) const;

    friend class AiMap;

    sint32 GetPriorityQueueIndex() const { return m_queue_idx; } 
    void SetPriorityQueueIndex(const sint32 i) { 
        Assert(-1 <= i); 
        Assert(i < SHRT_MAX); 
        m_queue_idx = sint16(i); }

};


class AiCell { 

    
    sint16 m_current_settle_value; 
    sint16 m_dynamic_settle_value; 
    AiCellPtr * m_settle_ptr;
    

public:

	AiCell(); 
    
    void Serialize(CivArchive &archive); 

    friend class AiMap;
    friend class AiCellPtr; 

};



#endif
