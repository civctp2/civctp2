#include "ctp/c3.h"

#include "gs/utility/UnitDynArr.h"
#include "gs/world/MapPoint.h"


UnitDynamicArray::UnitDynamicArray() : DynamicArray<Unit>()

{
    return;
}

UnitDynamicArray::UnitDynamicArray(const sint32 size) : DynamicArray<Unit>(size)

{
    return;
}

UnitDynamicArray::UnitDynamicArray (const DynamicArray<Unit> &copyme) :
DynamicArray<Unit> (copyme)

{
    return;
}
