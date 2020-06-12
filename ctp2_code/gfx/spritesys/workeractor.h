#ifdef HAVE_PRAGMA_ONCE
#pragma once
#endif
#ifndef __WORKERACTOR_H__
#define __WORKERACTOR_H__

#include "Actor.h"              // Actor
#include "ctp2_inttypes.h"      // sint32, uint16
#include "Queue.h"              // Queue
#include "UnitSpriteGroup.h"    // UNITACTION

class Action;
class Anim;
class aui_Surface;
// MBCHAR

class WorkerActor : public Actor
{
public:
	// Make position methods public
	using Actor::GetX;
	using Actor::GetY;

	WorkerActor(sint32 index, const MapPoint & pos, sint32 x = 0, sint32 y = 0);
	virtual ~WorkerActor();

	void   Process();
	void   AddAction(Action * action);

	Anim * CreateAnim(UNITACTION action) const;

	void   Draw() const;
	void   DrawDirect(aui_Surface * surf, sint32 x, sint32 y, double scale) const;

	uint16 GetWidth() const;
	uint16 GetHeight() const;

protected:
	void AddIdle();
	void GetNextAction();
	void Interrupt();

	void DrawText(sint32 x, sint32 y, const MBCHAR * goodText) const;

	sint32            m_facing;
	sint32            m_frame;
	uint16            m_transparency;
	sint32            m_index;
	MapPoint          m_pos;
	UnitSpriteGroup * m_unitSpriteGroup;
	Action          * m_curAction;
	UNITACTION        m_curUnitAction;
	Queue<Action *>   m_actionQueue;
};

#endif
