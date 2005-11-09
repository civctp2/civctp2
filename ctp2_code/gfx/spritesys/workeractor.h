










#ifdef HAVE_PRAGMA_ONCE
#pragma once
#endif
#ifndef __WORKERACTOR_H__
#define __WORKERACTOR_H__

#include "Actor.h"
#include "UnitSpriteGroup.h"
#include "pixelutils.h"
#include "Queue.h"
#include "XY_Coordinates.h"
#include "World.h"

class SpriteState;
class SpriteGroup;
class aui_Surface;
class ActorPath;
class Action;

class WorkerActor : public Actor
{
public:
	WorkerActor(sint32 index, const MapPoint &pos);
	WorkerActor(sint32 index, const MapPoint &pos, sint32 x, sint32 y);
	WorkerActor(WorkerActor *copy);
	~WorkerActor();

	virtual void	Process(void);

	void			AddAction(Action *actionObj);
	void			GetNextAction(void);
	void			AddIdle(void);

	Anim *          CreateAnim(UNITACTION action);

	void			Draw(void);
	void			DrawDirect(aui_Surface *surf, sint32 x, sint32 y, double scale);

	void			DrawText(sint32 x, sint32 y, MBCHAR *goodText);

	BOOL			IsAnimating(void);

	MapPoint		GetPos(void) { return m_pos; }
	uint16			GetWidth(void);
	uint16			GetHeight(void);

	void			SetFrame(sint32 frame) { m_frame = frame; }

	void            Initialize(sint32 index, const MapPoint &pos);

protected:
    
	
	sint32				m_facing;
	sint32				m_frame;
	uint16				m_transparency;
	sint32              m_index;
	
	

	
	
	
	MapPoint			m_pos;
	UnitSpriteGroup		*m_unitSpriteGroup;

	Action				*m_curAction;
	UNITACTION			m_curUnitAction;

	Queue<Action *>		m_actionQueue;
	
	


};

#endif
