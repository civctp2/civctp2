










#ifdef HAVE_PRAGMA_ONCE
#pragma once
#endif
#ifndef __GOODACTOR_H__
#define __GOODACTOR_H__

class GoodActor;

#include "Actor.h"              // Actor
#include "civarchive.h"         // CivArchive
#include "ctp2_inttypes.h"      // sint32, uint16
#include "GoodSpriteGroup.h"    // GOODACTION
#include "Queue.h"              // Queue
#include "SpriteGroup.h"        // LOADTYPE
#include "World.h"              // MapPoint

class Action;
class Anim;
class aui_Surface;


class GoodActor : public Actor
{
public:
	GoodActor(sint32 index, const MapPoint &pos);
	GoodActor(GoodActor *copy);
	GoodActor(CivArchive &archive);
	virtual ~GoodActor();

	virtual void	Process(void);

	void			PositionActor(MapPoint &pos);
	void			AddAction(Action *actionObj);
	void			GetNextAction(void);
	void			AddIdle(void);

	Anim *          CreateAnim(GOODACTION action);

	void			DrawSelectionBrackets(void);
	bool			Draw(bool fogged = false);
	void			DrawDirect(aui_Surface *surf, sint32 x, sint32 y, double scale);
	
	void			DrawText(sint32 x, sint32 y, MBCHAR const * goodText);

	bool			IsAnimating(void) const;

	MapPoint		GetPos(void) const { return m_pos; }
	void			SetPos(MapPoint const & pos) { m_pos = pos; }
	uint16			GetWidth(void) const;
	uint16			GetHeight(void) const;
	POINT			GetHotpoint(void) const;
	void			GetBoundingRect(RECT *rect) const;
	
	void            Serialize(CivArchive &archive);

	LOADTYPE		GetLoadType(void) const { return m_loadType; }
	void			SetLoadType(LOADTYPE type) { m_loadType = type; }
	void			FullLoad();
	void			DumpFullLoad(void);

protected:
	sint32				m_facing;
	sint32				m_frame;
	uint16				m_transparency;
	sint32              m_index;
	MapPoint			m_pos;
	GoodSpriteGroup	*   m_goodSpriteGroup;
	Action *            m_curAction;
	GOODACTION			m_curGoodAction;
	Queue<Action *>		m_actionQueue;
	LOADTYPE			m_loadType;
};

#endif
