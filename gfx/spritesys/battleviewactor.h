











#pragma once
#ifndef __BATTLEVIEWACTOR_H__
#define __BATTLEVIEWACTOR_H__

#include "Actor.h"
#include "UnitSpriteGroup.h"
#include "pixelutils.h"
#include "Queue.h"
#include "XY_Coordinates.h"
#include "World.h"
#include "tech_wllist.h"
#include "Anim.h"

class SpriteState;
class SpriteGroup;
class ProjectileActor;
class EffectActor;
class aui_Surface;
class ActorPath;
class Action;
class CivArchive;

class BattleViewActor : public Actor
{
public:
	BattleViewActor(SpriteState *ss, Unit id, sint32 type, const MapPoint &pos,
			  sint32 owner);

	~BattleViewActor();

	void			GetIDAndType(sint32 owner, SpriteState *ss, Unit id, sint32 unitType, MapPoint &pos, uint32 *spriteID, GROUPTYPE *groupType);
	
	void			Initialize(void);

	
	virtual void	Process(void);
	void			DumpAllActions(void);
	void			AddAction(Action *actionObj);
	void			GetNextAction(BOOL isVisible = TRUE);
	void			AddIdle(BOOL NoIdleJustDelay = FALSE);

	Anim			*GetAnim(UNITACTION action);
	
	BOOL			HasThisAnim(UNITACTION action) { if (!m_unitSpriteGroup) return FALSE; return (m_unitSpriteGroup->GetAnim((GAME_ACTION)action) != NULL); }
	Anim			*MakeFakeDeath(void);

	
	void			Draw(BOOL fogged = FALSE);
	void			DrawDirect(aui_Surface *surf, sint32 x, sint32 y);

	void			DrawHealthBar(aui_Surface *surf);

	MapPoint		GetPos(void) { return m_pos; }
	void			SetPos(MapPoint pnt) { m_pos = pnt; }
    void            GetPixelPos(sint32 &x, sint32 &y) { x = m_x; y = m_y; } 
	void			SetPixelPos(sint32 x, sint32 y) { m_x = x; m_y = y; }

	sint32			GetFacing(void) const { return m_facing; }
	void			SetFacing(sint32 facing) { m_facing = facing; }

	uint16			GetWidth(void);
	uint16			GetHeight(void);
	Unit			GetUnitID(void) { return m_unitID; }

	Action			*GetCurAction(void) { return m_curAction; }

	Action			*LookAtNextAction(void) { return m_actionQueue.LookAtNextDeQueue(); }
	Action			*LookAtLastAction(void) { return m_actionQueue.LookAtLastDeQueue(); }
	uint32			GetActionQueueNumItems(void) { return m_actionQueue.GetNumItems(); }

	BOOL			HasDeath(void) { return m_unitSpriteGroup->HasDeath(); }
	BOOL			HasDirectional(void) { return m_unitSpriteGroup->HasDirectional(); }

	void			GetBoundingRect(RECT *rect);

	double			GetHitPoints(void) { return m_hitPoints; }
	double			GetHitPointsMax(void) { return m_hitPointsMax; }
	void			SetHitPoints(double points) { m_hitPoints = points; }
	void			SetHitPointsMax(double points) { m_hitPointsMax = points; }


	void			SetFortified(BOOL fortified) { m_isFortified = fortified; }
	BOOL			GetFortified(void) { return m_isFortified; }

protected:
	MapPoint			m_pos;
	Unit				m_unitID;
	sint32  			m_unitDBIndex;
	sint32				m_playerNum;

	UnitSpriteGroup		*m_unitSpriteGroup;
	sint32				m_facing;
	sint32				m_frame;
	uint16				m_transparency;

	Action				*m_curAction;
	UNITACTION			m_curUnitAction;

	Queue<Action *>		m_actionQueue;
	GROUPTYPE			m_type;			
	uint32				m_spriteID;		

	double				m_hitPoints;
	double				m_hitPointsMax;

	BOOL				m_isFortified;
};


#endif
