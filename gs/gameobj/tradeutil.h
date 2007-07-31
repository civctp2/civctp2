
#ifndef TRADEUTIL_H__
#define TRADEUTIL_H__

class Unit;

sint32 tradeutil_GetTradeValue(const sint32 owner, Unit &destination, sint32 resource);
sint32 tradeutil_GetAccurateTradeDistance(Unit &source, Unit &destination);
sint32 tradeutil_GetTradeDistance(Unit &source, Unit &destination);

#endif
