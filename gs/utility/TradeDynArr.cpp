









#include "c3.h"

#include "TradeDynArr.h"
#include "MapPoint.h"



TradeDynamicArray::TradeDynamicArray() : DynamicArray<TradeRoute>()
{ 
}

TradeDynamicArray::TradeDynamicArray(const sint32 size) : 
	DynamicArray<TradeRoute>(size)
{ 
}


TradeDynamicArray::TradeDynamicArray (const DynamicArray<TradeRoute> &copyme) :
	DynamicArray<TradeRoute> (copyme)
{
}
