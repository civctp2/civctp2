#ifdef HAVE_PRAGMA_ONCE
#pragma once
#endif
#ifndef _TRADEDYNARR_H_
#define _TRADEDYNARR_H_

#include "robot/aibackdoor/dynarr.h"
#include "gs/gameobj/TradeRoute.h"

class TradeDynamicArray : public DynamicArray<TradeRoute>
{
public:
	TradeDynamicArray();
	TradeDynamicArray(const sint32 size);
	TradeDynamicArray(const DynamicArray<TradeRoute> &copyme);
};

#endif
