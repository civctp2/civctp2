
#pragma once
#ifndef __UNIT_DYNAMIC_ARRAY_H__
#define __UNIT_DYNAMIC_ARRAY_H__ 1

#include "DynArr.h"
#include "Unit.h"

class UnitDynamicArray : public DynamicArray<Unit> {


public:

    UnitDynamicArray(); 
    UnitDynamicArray(const sint32 size); 
    UnitDynamicArray (const DynamicArray<Unit> &copyme); 

};

#endif
