#ifdef HAVE_PRAGMA_ONCE
#pragma once
#endif
#ifndef __TRADEACTOR_H__
#define __TRADEACTOR_H__

#include "Actor.h"
#include "GoodSpriteGroup.h"
#include "TradeRoute.h"

class ActorPath;
class Action;

class TradeActor : public Actor
{
public:
	TradeActor(TradeRoute newRoute);
	virtual ~TradeActor();

	void       Process();

	void       Draw(const RECT & paintRect) const;

	TradeRoute GetRouteID() const { return m_routeID; }

protected:
	void     AddIdle();
	MapPoint GetNextPos();
	MapPoint LookAtNextPos() const;

	Anim   * CreateAnim(GOODACTION action) const;

	void     Draw(const Vision * tileLocalVision, const POINT & drawPos) const;
	void     DrawText(sint32 x, sint32 y, MBCHAR * goodText) const;
	uint16   GetWidth() const;
	uint16   GetHeight() const;
	void     GetBoundingRect(RECT * rect, const POINT & drawPos) const;

	void     LoadSprites();

	TradeRoute                     m_routeID;
	sint32                         m_spriteID;

	const DynamicArray<MapPoint> * m_routePath;
	uint32                         m_sourcePosID;
	uint32                         m_currentPosID;
	uint32                         m_destPosID;
	MapPoint                       m_currentPos;
	MapPoint                       m_nextPos;

	GoodSpriteGroup              * m_goodSpriteGroup;

	sint32                         m_facing;

	Anim                         * m_idleAnim;
	Action                       * m_curAction;
	GOODACTION                     m_curGoodAction;
	LOADTYPE                       m_loadType;
	uint32                         m_nextGoodAnimation;
};

#endif
