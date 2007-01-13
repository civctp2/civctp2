











#pragma once
#ifndef __GOODACTOR_H__
#define __GOODACTOR_H__

#include "Actor.h"
#include "GoodSpriteGroup.h"
#include "pixelutils.h"
#include "Queue.h"
#include "XY_Coordinates.h"
#include "World.h"

class SpriteState;
class SpriteGroup;
class aui_Surface;
class ActorPath;
class Action;
class CivArchive;

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

	void			DumpAllActions();

	Anim			*GetAnim(GOODACTION action);

	void			DrawSelectionBrackets(void);
	BOOL			Draw(BOOL fogged = FALSE);
	void			DrawDirect(aui_Surface *surf, sint32 x, sint32 y, double scale);
	
	void			DrawText(sint32 x, sint32 y, MBCHAR *goodText);

	BOOL			IsAnimating(void);

	MapPoint		GetPos(void) { return m_pos; }
	void			SetPos(MapPoint &pos) { m_pos = pos; }
	uint16			GetWidth(void);
	uint16			GetHeight(void);

	POINT			GetHotpoint(void);

	void            Initialize(sint32 index, const MapPoint &pos);

	void			GetBoundingRect(RECT *rect);
	
	void            Serialize(CivArchive &archive);

	LOADTYPE		GetLoadType(void) { return m_loadType; }
	void			SetLoadType(LOADTYPE type) { m_loadType = type; }
	void			FullLoad();
	void			DumpFullLoad(void);

protected:
    
	
	sint32				m_facing;
	sint32				m_frame;
	uint16				m_transparency;
	sint32              m_index;
	
	

	
	
	
	MapPoint			m_pos;
	GoodSpriteGroup		*m_goodSpriteGroup;

	Action				*m_curAction;
	GOODACTION			m_curGoodAction;

	Queue<Action *>		m_actionQueue;
	LOADTYPE			m_loadType;
	
	


};

#endif
