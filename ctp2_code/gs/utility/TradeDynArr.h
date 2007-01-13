









#pragma once
#ifndef _TRADEDYNARR_H_
#define _TRADEDYNARR_H_

#include "dynarr.h"
#include "TradeRoute.h"

class TradeDynamicArray : public DynamicArray<TradeRoute>
{
public:
	TradeDynamicArray();
	TradeDynamicArray(const sint32 size);
	TradeDynamicArray(const DynamicArray<TradeRoute> &copyme);
};

#endif
