











#pragma once
#ifndef __UNITACTOR_H__
#define __UNITACTOR_H__

#include "Actor.h"
#include "pixelutils.h"
#include "Queue.h"
#include "XY_Coordinates.h"
#include "World.h"
#include "tech_wllist.h"
#include "Anim.h"
#include "UnitSpriteGroup.h"

enum SPECATTACK {
	SPECATTACK_NONE = -1,

	SPECATTACK_NUKE,
	SPECATTACK_HEARGOSSIP,
	SPECATTACK_THROWPARTY,
	SPECATTACK_ESTABLISHEMBASSY,
	SPECATTACK_INCITEREVOLUTION,
	SPECATTACK_BOMBCABINET,
	SPECATTACK_CREATEFRANCHISE,
	SPECATTACK_CAUSEUNHAPPINESS,
	SPECATTACK_CONDUCTHIT,
	SPECATTACK_BIOTERROR,
	SPECATTACK_NANOTERROR,
	SPECATTACK_SLAVERAID,
	SPECATTACK_ENSLAVESETTLER,
	SPECATTACK_SLAVEUPRISING,
	SPECATTACK_FREESLAVES,
	SPECATTACK_SELLINDULGENCE,
	SPECATTACK_CONVERTCITY,
	SPECATTACK_PLANTNUKE,
	SPECATTACK_SOOTHSAY,
	SPECATTACK_CREATEPARK,
	SPECATTACK_INJOIN,
	SPECATTACK_SPY,
	SPECATTACK_STEALTECH,
	SPECATTACK_REVOLUTION,
	SPECATTACK_REFORMCITY,

	SPECATTACK_MAX
};

class SpriteState;
class SpriteGroup;
class ProjectileActor;
class EffectActor;
class aui_Surface;
class ActorPath;
class Action;
class CivArchive;

class UnitActor : public Actor
{
public:
	UnitActor(SpriteState *ss, Unit id, sint32 type, const MapPoint &pos,
			  sint32 owner, BOOL isUnseenCellActor, double visionRange,
			  sint32 citySprite);
	UnitActor(CivArchive &archive);

	~UnitActor();

	void			GetIDAndType(sint32 owner, SpriteState *ss, Unit id, sint32 unitType, MapPoint &pos, sint32 *spriteID, GROUPTYPE *groupType);

	void			AddVision(void);
	void			RemoveVision(void);
	void			PositionActor(MapPoint &pos);
	void			Hide(void);
	void			Show(void);

	void			Initialize(void);

	void			ChangeImage(SpriteState *ss, sint32 type, Unit id);
	void			ChangeType(SpriteState *ss, sint32 type, Unit id, BOOL updateVision);

	void			SetSize(sint32 size) { m_size = size; }
	sint32			GetSize(void) { return m_size; }

	
	virtual void	Process(void);
	void			DumpAllActions(void);
	void			EndTurnProcess(void);
	Action 			*WillMorph(void);
	Action			*WillDie(void);
	void			AddAction(Action *actionObj);
	void			GetNextAction(BOOL isVisible = TRUE);
	void			AddIdle(BOOL NoIdleJustDelay = FALSE);
	void			ActionQueueUpIdle(BOOL NoIdleJustDelay = FALSE);

	Anim			*GetAnim(UNITACTION action);
	Anim			*MakeFakeDeath(void);
	Anim			*MakeFaceoff(void);

	BOOL			HasThisAnim(UNITACTION action) { if (!m_unitSpriteGroup) return FALSE; return (m_unitSpriteGroup->GetAnim((GAME_ACTION)action) != NULL); }

	
	void			DrawFortified(BOOL fogged);
	void			DrawFortifying(BOOL fogged);
	void			DrawCityWalls(BOOL fogged);
	void			DrawForceField(BOOL fogged);

	BOOL			Draw(BOOL fogged = FALSE);
	void			DrawHerald(void);
	void			DrawSelectionBrackets(void);
	void			DrawHealthBar(void);
	void			DrawStackingIndicator(sint32 x, sint32 y, sint32 stackSize);
	void			DrawText(sint32 x, sint32 y, MBCHAR *unitText);

	void			DrawDirect(aui_Surface *surf, sint32 x, sint32 y, double scale);

	
	BOOL			IsAnimating(void);

	MapPoint		GetPos(void) { return m_pos; }
	void			SetPos(MapPoint pnt) { m_pos = pnt; }
	MapPoint		GetSavedPos(void) { return m_savePos; }
	void			SetSavedPos(MapPoint pnt) { m_savePos = pnt; }
    void            GetPixelPos(sint32 &x, sint32 &y) { x = m_x; y = m_y; } 

	sint32			GetFacing(void) const { return m_facing; }

	uint16			GetWidth(void);
	uint16			GetHeight(void);

	uint32		    GetUnitID(void) { return m_unitID.m_id; }
	sint32			GetUnitDBIndex(void) { return m_unitDBIndex; }

	void			SetPlayerNum(sint32 playerNum) { m_playerNum = playerNum; }
	sint32			GetPlayerNum(void) { return m_playerNum;}

	Action			*GetCurAction(void) { return m_curAction; }

	Action			*LookAtNextAction(void) { return m_actionQueue.LookAtNextDeQueue(); }
	Action			*LookAtLastAction(void) { return m_actionQueue.LookAtLastDeQueue(); }
	uint32			GetActionQueueNumItems(void) { return m_actionQueue.GetNumItems(); }

	BOOL			HasDeath(void) { return m_unitSpriteGroup->HasDeath(); }
	BOOL			HasDirectional(void) { return m_unitSpriteGroup->HasDirectional(); }

	void			SetUnitVisibility(uint32 val) { m_unitSaveVisibility = m_unitVisibility = val; }
	void			SetUnitVisibility(uint32 val, BOOL bval) { m_unitSaveVisibility = m_unitVisibility; m_unitVisibility = val; m_bVisSpecial = TRUE; }
											
	void			SetUnitVisibility() { m_bVisSpecial = FALSE; }
	uint32			GetUnitVisibility(void) { return m_unitVisibility; }
	uint32			GetUnitSavedVisibility(void) { return m_unitSaveVisibility; }

	BOOL			GetVisSpecial(void) { return m_bVisSpecial; }
	void			SetVisSpecial(BOOL val) { m_bVisSpecial = val; }
	
	double			GetUnitVisionRange(void) { return m_unitVisionRange; }
	void            SetUnitVisionRange(double range) { m_unitVisionRange = range; }
	void            SetNewUnitVisionRange(double range) { m_newUnitVisionRange = range; }

	void			SetNeedsToDie(BOOL val) { m_needsToDie = val; }
	BOOL			GetNeedsToDie(void) { return m_needsToDie; }

	void			SetNeedsToVictor(BOOL val) { m_needsToVictor = val; }
	BOOL			GetNeedsToVictor(void) { return m_needsToVictor; }

	void			SetKillNow(void) { m_killNow = TRUE; }
	BOOL			GetKillNow(void) { return m_killNow; }

	void			SetRevealedActors(UnitActor **revealedActors) { m_revealedActors = revealedActors; }
	void			SaveRevealedActors(UnitActor **revealedActors) { m_savedRevealedActors = revealedActors; }
	UnitActor		**GetRevealedActors(void) { return m_revealedActors; }

	void			SetMoveActors(UnitActor **moveActors, sint32 numOActors) { m_moveActors = moveActors; m_numOActors = numOActors; }
	UnitActor		**GetMoveActors(void) { return m_moveActors; }
	sint32			GetNumOActors(void) { return m_numOActors; }

	BOOL			HiddenUnderStack(void) { return m_hiddenUnderStack; }
	void			SetHiddenUnderStack(BOOL val) { m_hiddenUnderStack = val; }

	void			SetIsTransported(sint32 val) { m_isTransported = val; }
	sint32			GetIsTransported() { return m_isTransported; }

	void			GetBoundingRect(RECT *rect);

	sint32			GetHoldingCurAnimPos(UNITACTION action) { return m_holdingCurAnimPos[action]; }
	sint32			GetHoldingCurAnimDelayEnd(UNITACTION action) { return m_holdingCurAnimDelayEnd[action]; }
	sint32			GetHoldingCurAnimElapsed(UNITACTION action) { return m_holdingCurAnimElapsed[action]; }
	sint32			GetHoldingCurAnimLastFrameTime(UNITACTION action) { return m_holdingCurAnimLastFrameTime[action]; }
	sint32			GetHoldingCurAnimSpecialDelayProcess(UNITACTION action) { return m_holdingCurAnimSpecialDelayProcess; }

	LOADTYPE		GetLoadType(void);

	void			FullLoad(UNITACTION action);
	void			DumpFullLoad(void);

	void			SetIsFortified(BOOL fort) { m_isFortified = fort; }
	void			SetIsFortifying(BOOL fort) { m_isFortifying = fort; }
	BOOL			IsFortified(void) { return m_isFortified; }
	BOOL			IsFortifying(void) { return m_isFortifying; }

	void			SetHasCityWalls(BOOL has) { m_hasCityWalls = has; }
	void			SetHasForceField(BOOL has) { m_hasForceField = has; }
	BOOL			HasCityWalls(void) { return m_hasCityWalls;}
	BOOL			HasForceField(void) { return m_hasForceField; }

	void			SetHealthPercent(double p) { m_healthPercent = p;}
	double			GetHealthPercent(void) { return m_healthPercent; }

	void			SetTempStackSize(sint32 i) { m_tempStackSize = i; }
	sint32			GetTempStackSize(void) { return m_tempStackSize; }


	BOOL			HitTest(POINT mousePt);

	
	
	
	
	void			AddActiveListRef(void) { m_activeListRef++; }
	sint32			ReleaseActiveListRef(void) { return --m_activeListRef;}
	sint32			GetActiveListRef(void) { return m_activeListRef; }

	void			Serialize(CivArchive &archive);

#ifdef _DEBUG
	void			DumpActor(void);
#endif
	sint32          m_refCount; 


	bool			ActionMove	       (Action *actionObj);
	bool			ActionAttack       (Action *actionObj,sint32 facing);
	bool			ActionSpecialAttack(Action *actionObj,sint32 facing);
	bool            TryAnimation       (Action *actionObj,UNITACTION action);


	void			TerminateLoopingSound	(uint32 sound_type); 
	void			AddSound				(uint32 sound_type, sint32 sound_id);
	void			AddLoopingSound			(uint32 sound_type, sint32 sound_id);

	
	void            HackSetSpriteID(sint32 spriteID) { m_spriteID = spriteID; }

protected:
	MapPoint			m_pos;
	MapPoint			m_savePos;
	Unit				m_unitID;
	sint32				m_unitDBIndex;
	sint32				m_playerNum;

	UnitSpriteGroup		*m_unitSpriteGroup;
	LOADTYPE			m_loadType;							

	sint32				m_facing;
	sint32				m_lastMoveFacing;
	sint32				m_frame;
	uint16				m_transparency;

	Action				*m_curAction;
	UNITACTION			m_curUnitAction;

	Queue<Action *>		m_actionQueue;
	
	RECT				m_heraldRect;

	uint32				m_unitVisibility;					
	uint32				m_unitSaveVisibility;				

	BOOL				m_directionalAttack;				
 	BOOL				m_needsToDie;						
 	BOOL				m_needsToVictor;					
	BOOL				m_killNow;							
	double				m_unitVisionRange;					
	double              m_newUnitVisionRange;				
	
	sint32				m_numRevealedActors;
	UnitActor			**m_revealedActors;					
	sint32				m_numSavedRevealedActors;
	UnitActor			**m_savedRevealedActors;			

	BOOL				m_bVisSpecial;						
	
	UnitActor			**m_moveActors;						
	sint32				m_numOActors;						
	BOOL				m_hidden;							
	BOOL				m_hiddenUnderStack;					
	sint32				m_isTransported;					
															

	sint32				m_holdingCurAnimPos[UNITACTION_MAX]; 
	sint32				m_holdingCurAnimDelayEnd[UNITACTION_MAX]; 
	sint32				m_holdingCurAnimElapsed[UNITACTION_MAX]; 
	sint32				m_holdingCurAnimLastFrameTime[UNITACTION_MAX]; 
	sint32				m_holdingCurAnimSpecialDelayProcess; 

	sint32				m_size;								
	BOOL                m_isUnseenCellActor;

	GROUPTYPE			m_type;								
	sint32				m_spriteID;							

	BOOL				m_isFortified;
	BOOL				m_isFortifying;
	BOOL				m_hasCityWalls;
	BOOL				m_hasForceField;


	uint32				m_shieldFlashOnTime;
	uint32				m_shieldFlashOffTime;

	sint32				m_activeListRef;
	double				m_healthPercent;	
	sint32				m_tempStackSize;

#ifdef _ACTOR_DRAW_OPTIMIZATION

	sint32				m_oldFacing;
	BOOL				m_oldIsFortified;
	BOOL				m_oldIsFortifying;
	BOOL				m_oldHasCityWalls;
	BOOL				m_oldHasForceField;
	BOOL				m_oldDrawShield;
	BOOL				m_oldDrawSelectionBrackets;
	uint16				m_oldFlags;

#endif
};


#endif
