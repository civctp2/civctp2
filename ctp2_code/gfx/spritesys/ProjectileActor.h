#ifdef HAVE_PRAGMA_ONCE
#pragma once
#endif

#ifndef __PROJECTILEACTOR_H__
#define __PROJECTILEACTOR_H__

#include "Actor.h"
#include "ProjectileSpriteGroup.h"
#include "pixelutils.h"
#include "Queue.h"
#include "XY_Coordinates.h"
#include "World.h"
#include "tech_wllist.h"

class SpriteState;
class SpriteGroup;
class aui_Surface;
class ActorPath;
class Action;

typedef sint32 UnitDBIndex;

class ProjectileActor : public Actor
{
public:
	ProjectileActor(SpriteState *ss, const MapPoint &pos);
	~ProjectileActor();
	
	void ChangeType(SpriteState *ss, sint32 type, Unit id);

	virtual void	Process(void);
	void			EndTurnProcess(void);

	void			AddAction(Action *actionObj);
	void			GetNextAction(BOOL isVisible = TRUE);

	Anim			*GetAnim(PROJECTILEACTION action);

	void			Draw(void);

	void			DrawText(sint32 x, sint32 y, MBCHAR *ProjectileText);

	BOOL			IsAnimating(void);

	MapPoint		GetPos(void) { return m_pos; }
	void			SetPos(MapPoint pnt) { m_pos = pnt; }
	MapPoint		GetSavedPos(void) { return m_savePos; }
	void			SetSavedPos(MapPoint pnt) { m_savePos = pnt; }
    void            GetPixelPos(sint32 &x, sint32 &y) { x = m_x; y = m_y; } 

	uint16			GetWidth(void);
	uint16			GetHeight(void);

	void			SetPlayerNum(sint32 playerNum) { m_playerNum = playerNum; }

	Action			*GetCurAction(void) { return m_curAction; }
	Action			*LookAtNextAction(void) { return m_actionQueue.LookAtNextDeQueue(); }
	uint32			GetActionQueueNumItems(void) { return m_actionQueue.GetNumItems(); }

	BOOL			HasDeath(void) { return m_projectileSpriteGroup->HasDeath(); }
	BOOL			HasDirectional(void) { return m_projectileSpriteGroup->HasDirectional(); }

	void			SetGenerateDeath(BOOL val) { m_generateDeath = val; }
	BOOL			GetGenerateDeath(void) { return m_generateDeath; }

	void			SetDieAtTick(uint32 val) { m_dieAtTick = val; }
	uint32			GetDieAtTick(void) { return m_dieAtTick; }

	void			SetProjectileVisibility(uint32 val) { m_projectileVisibility = val; }
	void			SetProjectileVisibility(uint32 val, BOOL bval) { m_projectileSaveVisibility = m_projectileVisibility; m_projectileVisibility = val; m_bVisSpecial = TRUE; }
											
	void			SetProjectileVisibility() { m_bVisSpecial = FALSE; }
	uint32			GetProjectileVisibility(void) { return m_projectileVisibility; }
	BOOL			GetVisSpecial(void) { return m_bVisSpecial; }

	void			SetNeedsToDie(BOOL val) { m_needsToDie = val; }
	BOOL			GetNeedsToDie(void) { return m_needsToDie; }

	void			SetKillNow(void) { m_killNow = TRUE; }
	BOOL			GetKillNow(void) { return m_killNow; }

	void			GetBoundingRect(RECT *rect);

protected:
	MapPoint					m_pos;
	MapPoint					m_savePos;

	sint32						m_shX; 
	sint32						m_shY; 

	ProjectileSpriteGroup		*m_projectileSpriteGroup;
	sint32						m_facing;
	sint32						m_lastMoveFacing;
	sint32						m_frame;
	uint16						m_transparency;

	Action						*m_curAction;
	PROJECTILEACTION			m_curProjectileAction;

	Queue<Action *>				m_actionQueue;
	
	sint32						m_playerNum;
	uint32						m_projectileVisibility; 
	uint32						m_projectileSaveVisibility; 

	uint32						m_dieAtTick;

	BOOL						m_directionalAttack; 
 	BOOL						m_needsToDie; 
	BOOL						m_killNow; 
	BOOL						m_generateDeath;  

	BOOL						m_bVisSpecial; 
};

#endif
