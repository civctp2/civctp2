










#ifdef HAVE_PRAGMA_ONCE
#pragma once
#endif
#ifndef __TRADEACTOR_H__
#define __TRADEACTOR_H__

#include "Actor.h"
#include "GoodSpriteGroup.h"
#include "Queue.h"
#include "XY_Coordinates.h"
#include "World.h"
#include "TradeRoute.h"

class SpriteState;
class SpriteGroup;
class aui_Surface;
class ActorPath;
class Action;

class TradeActor : public Actor
{
public:
	TradeActor(TradeRoute newRoute);
	TradeActor(TradeActor *copy);
	~TradeActor();

	virtual void	Process(void);

	void			AddAction(Action *actionObj);
	void			GetNextAction(void);
	void			AddIdle(void);

	Anim *          CreateAnim(GOODACTION action);

	void			Draw(Vision *tileLocalVision);
	void			DrawText(sint32 x, sint32 y, MBCHAR *goodText);

	BOOL			IsAnimating(void);

	TradeRoute		GetRouteID(void) { return m_routeID; }

	MapPoint		GetCurrentPos(void) { return m_currentPos; }
	MapPoint		GetSourcePos(void) { return m_sourcePos; }
	MapPoint		GetDestPos(void) { return m_destPos; }
	MapPoint		GetNextPos(void); 
	MapPoint		LookAtNextPos(void); 

	uint16			GetWidth(void);
	uint16			GetHeight(void);

	void			GetBoundingRect(RECT *rect);

protected:
	uint32							m_currentPosID;
	uint32							m_sourcePosID;
	uint32							m_destPosID;

	MapPoint						m_currentPos;
	MapPoint						m_sourcePos;
	MapPoint						m_destPos;

	TradeRoute						m_routeID;
	const DynamicArray<MapPoint>*	m_routePath;

	ROUTE_TYPE						m_routeType;
	sint32							m_routeResource;

	GoodSpriteGroup					*m_goodSpriteGroup;
	sint32							m_facing;
	sint32							m_frame;
	uint16							m_transparency;
	
	Action							*m_curAction;
	GOODACTION						m_curGoodAction;

	Queue<Action *>					m_actionQueue;


};

#endif
