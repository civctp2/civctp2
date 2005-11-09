










#ifdef HAVE_PRAGMA_ONCE
#pragma once
#endif
#ifndef __ACTION_H__
#define __ACTION_H__

#include "MapPoint.h"
#include "Anim.h"
#include "director.h"


class ProjectileActor;
class EffectActor;
class ActorPath;

#define k_DEFAULTSPRITEFACING 3

#define k_MAX_UNIT_MOVEMENT_ITERATIONS		10


enum GAME_ACTION
{
  ACTION_0=0,
  ACTION_1,
  ACTION_2,
  ACTION_3,
  ACTION_4,
  ACTION_5,
  ACTION_6,
  ACTION_7,
  ACTION_8,
  ACTION_9,
  ACTION_10,
  ACTION_11,
  ACTION_12,
  ACTION_13,
  ACTION_14,
  ACTION_15,
  ACTION_MAX
};


#define  ACTION_VISIBLE 0x00000001
#define  ACTION_CENTER  0x00000002

template <class T> class tech_WLList;











enum ACTIONEND 
{
	ACTIONEND_PATHEND,
	ACTIONEND_ANIMEND,
	ACTIONEND_INTERRUPT,

	ACTIONEND_MAX
};

class Action
{
public:

	Action(sint32 actionType=0, ACTIONEND endCondition=ACTIONEND_PATHEND, sint32 startAnimPos = 0, BOOL specialDelayProcess = FALSE);
	Action(Action *copyme);

	virtual ~Action(void);

	sint32			GetActionType(void) { return (m_actionType > 0 ) ? m_actionType : 0; }
	void			SetActionType(sint32 action) { m_actionType=action;};
	BOOL			GetIsSpecialActionType(void) { return (m_actionType < 0 ); } 

	Anim			*GetAnim(void) { return m_curAnim; }
	sint32			GetAnimPos(void) { return m_animPos; }
	void			SetAnimPos(sint32 pos) { m_animPos=pos; }
	sint32			GetAnimDelayEnd(void) { return m_animDelayEnd; }
	sint32			GetAnimElapsed(void) { return m_animElapsed; }
	sint32			GetAnimLastFrameTime(void) { return m_animLastFrameTime; }

	ActorPath		*GetPath(void) { return m_curPath; }

	virtual void	Process(void);
	void			Process(Action *pendingAction);

	void			SetAnim(Anim *anim);
	void			CreatePath(sint32 x1, sint32 y1, sint32 x2, sint32 y2);


	sint32			GetMaxActionCounter(void) { return m_maxActionCounter; }
	void			SetMaxActionCounter(sint32 count) { m_maxActionCounter = count; }

	sint32			GetCurActionCounter(void) { return m_curActionCounter; }
	void			SetCurActionCounter(sint32 count) { m_curActionCounter = count; }

	POINT			GetPosition(void);
	uint16			GetSpriteFrame(void);
	uint16			GetTransparency(void);

	
	sint32			GetFacing(void);
	void			SetFacing(sint32 facing) { m_facing = facing; }
	BOOL			SpecialDelayProcess(void) {return m_specialDelayProcess; }
	void			SetSpecialDelayProcess(BOOL val) {m_specialDelayProcess = val; }

	void			SetDelay(sint32 delay) { m_delay = delay; }
	sint32			GetDelay(void) { return m_delay; }

	BOOL			Finished(void) { return m_finished; }
	void			SetFinished(BOOL fin) { m_finished = fin; }

	void			SetStartMapPoint(MapPoint &point) { m_startMapPoint = point;}
	void			GetStartMapPoint(MapPoint &point) { point = m_startMapPoint; }

	void			SetEndMapPoint(MapPoint &point) { m_endMapPoint = point; }
	void			GetEndMapPoint(MapPoint &point) { point = m_endMapPoint; }

	ACTIONEND		GetCurrentEndCondition(void) { return m_endCondition; }
	void			SetCurrentEndCondition(ACTIONEND end_condition) { m_endCondition=end_condition; }
	BOOL			LoopAnimHasCycled(void) { return m_loopAnimFinished; } 
	void			ResetAnimLoop(void) { m_loopAnimFinished = FALSE; }

	void			SetItIsTimeToAct(BOOL act) { m_itIsTimeToAct = act; }
	uint32			GetItIsTimeToAct() { return m_itIsTimeToAct; }

	void			SetRevealedActors(UnitActor **revealedActors) { m_revealedActors = revealedActors; }
	UnitActor		**GetRevealedActors(void) { return m_revealedActors; }
	void			SetNumRevealedActors(sint32 num) { m_numRevealedActors = num; }
	sint32			GetNumRevealedActors(void) { return m_numRevealedActors; }

	void			SetMoveActors(UnitActor **moveActors, sint32 numOActors) { m_moveActors = moveActors; m_numOActors = numOActors; }
	UnitActor		**GetMoveActors(void) { return m_moveActors; }

	void			SetNumOActors(sint32 val) { m_numOActors = val; }
	sint32			GetNumOActors(void) { return m_numOActors; }

	void			SetUnitsVisibility(uint32 unitsVis) { m_unitsVisibility = unitsVis; } 
	uint32			GetUnitsVisibility(void) { return m_unitsVisibility; } 

	void			SetUnitVisionRange(double visRange) { m_unitVisionRange = visRange; } 
	double			GetUnitVisionRange(void) { return m_unitVisionRange; } 

	void			SetSpecialUnitEffectsAction(DQAction action) { m_specialUnitEffectsAction = action; }
	DQAction		GetSpecialUnitEffectsAction(void) { return m_specialUnitEffectsAction; }
	
	void			SetSequence(Sequence *seq) { m_sequence = seq; }
	Sequence		*GetSequence(void) { return m_sequence; }

	
	void			SetSoundEffect(sint32 sound_id) { m_sound_effect_id = sound_id; }
	sint32  		GetSoundEffect(void) { return m_sound_effect_id; }

	sint32				m_actionType;
protected:
	ACTIONEND			m_endCondition;

	Anim				*m_curAnim;
	ActorPath			*m_curPath;

	sint32				m_maxActionCounter;
	sint32				m_curActionCounter;
	sint32				m_animPos; 
	sint32				m_animDelayEnd;
	sint32				m_animElapsed;
	sint32				m_animLastFrameTime;
	
	sint32				m_delay; 

	BOOL				m_itIsTimeToAct; 
	BOOL				m_finished; 
	
	BOOL				m_loopAnimFinished; 
	BOOL				m_specialDelayProcess; 

	MapPoint			m_startMapPoint;
	MapPoint			m_endMapPoint;

	sint32				m_facing;
	sint32              m_sound_effect_id; 

	uint32				m_unitsVisibility; 

	double				m_unitVisionRange; 

	sint32				m_numRevealedActors;
	UnitActor			**m_revealedActors;
	UnitActor			**m_moveActors; 
	sint32				m_numOActors; 

	DQAction			m_specialUnitEffectsAction; 

	Sequence			*m_sequence;
};

#endif
