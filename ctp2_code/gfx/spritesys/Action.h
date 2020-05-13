//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ header
// Description  : Action on the screen
// Id           : $Id$
//
//----------------------------------------------------------------------------
//
// Disclaimer
//
// THIS FILE IS NOT GENERATED OR SUPPORTED BY ACTIVISION.
//
// This material has been developed at apolyton.net by the Apolyton CtP2
// Source Code Project. Contact the authors at ctp2source@apolyton.net.
//
//----------------------------------------------------------------------------
//
// Compiler flags
//
// None
//
//----------------------------------------------------------------------------
//
// Modifications from the original Activision code:
//
//----------------------------------------------------------------------------
///
/// \file   Action.h
/// \brief  Action on the screen

#if defined(HAVE_PRAGMA_ONCE)
#pragma once
#endif

#ifndef ACTION_H__
#define ACTION_H__

//----------------------------------------------------------------------------
//
// Library imports
//
//----------------------------------------------------------------------------

// none

//----------------------------------------------------------------------------
//
// Exported names
//
//----------------------------------------------------------------------------

class Action;

enum ACTIONEND
{
	ACTIONEND_PATHEND,
	ACTIONEND_ANIMEND,
	ACTIONEND_INTERRUPT,

	ACTIONEND_MAX
};

enum  GAME_ACTION
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

#define ACTION_VISIBLE                  0x00000001
#define ACTION_CENTER                   0x00000002

#define k_DEFAULTSPRITEFACING           3
#define k_MAX_UNIT_MOVEMENT_ITERATIONS	10

//----------------------------------------------------------------------------
//
// Project imports
//
//----------------------------------------------------------------------------

#include "ctp2_inttypes.h"      // sintNN, uintNN
#include "MapPoint.h"           // MapPoint

class ActorPath;
class Anim;
// POINT
class Sequence;
class UnitActor;

//----------------------------------------------------------------------------
//
// Declarations
//
//----------------------------------------------------------------------------

class Action
{
public:
	Action(sint32 actionType=0, ACTIONEND endCondition=ACTIONEND_PATHEND, sint32 startAnimPos = 0, sint32 specialDelayProcess = 0);
    Action(Action const & a_Original);
	Action(Action *copyme); // @todo Remove when no longer used

    virtual ~Action(void);

	sint32			GetActionType(void) const { return (m_actionType > 0 ) ? m_actionType : 0; }
	void			SetActionType(sint32 action) { m_actionType=action;};
	bool			GetIsSpecialActionType(void) const { return (m_actionType < 0 ); }

	Anim *          GetAnim(void) const { return m_curAnim; }
	sint32			GetAnimPos(void) const { return m_animPos; }
	void			SetAnimPos(sint32 pos) { m_animPos=pos; }
	sint32			GetAnimDelayEnd(void) const { return m_animDelayEnd; }
	sint32			GetAnimElapsed(void) const { return m_animElapsed; }
	sint32			GetAnimLastFrameTime(void) const { return m_animLastFrameTime; }

	ActorPath *     GetPath(void) const { return m_curPath; }

	virtual void	Process(void);
	void			Process(Action *pendingAction);

	void			SetAnim(Anim *anim);
	void			CreatePath(sint32 x1, sint32 y1, sint32 x2, sint32 y2);

	sint32			GetMaxActionCounter(void) const { return m_maxActionCounter; }
	void			SetMaxActionCounter(sint32 count) { m_maxActionCounter = count; }

	sint32			GetCurActionCounter(void) const { return m_curActionCounter; }
	void			SetCurActionCounter(sint32 count) { m_curActionCounter = count; }

	POINT			GetPosition(void) const;
	uint16			GetSpriteFrame(void) const;
	uint16			GetTransparency(void) const;

	sint32			GetFacing(void); // not quite const
	void			SetFacing(sint32 facing) { m_facing = facing; }
	sint32			SpecialDelayProcess(void) const {return m_specialDelayProcess; }
	void			SetSpecialDelayProcess(sint32 val) {m_specialDelayProcess = val; }

	void			SetDelay(sint32 delay) { m_delay = delay; }
	sint32			GetDelay(void) const { return m_delay; }

	bool			Finished(void) const { return m_finished; }
	void			SetFinished(bool fin) { m_finished = fin; }

	void			SetStartMapPoint(MapPoint &point) { m_startMapPoint = point;}
	void			GetStartMapPoint(MapPoint &point) const { point = m_startMapPoint; }

	void			SetEndMapPoint(MapPoint &point) { m_endMapPoint = point; }
	void			GetEndMapPoint(MapPoint &point) const { point = m_endMapPoint; }

	ACTIONEND		GetCurrentEndCondition(void) const { return m_endCondition; }
	void			SetCurrentEndCondition(ACTIONEND end_condition) { m_endCondition = end_condition; }
	bool			LoopAnimHasCycled(void) const { return m_loopAnimFinished; }
	void			ResetAnimLoop(void) { m_loopAnimFinished = false; }

	void			SetItIsTimeToAct(bool act) { m_itIsTimeToAct = act; }
	bool			GetItIsTimeToAct() const { return m_itIsTimeToAct; }

	void			SetRevealedActors(UnitActor **revealedActors) { m_revealedActors = revealedActors; }
	UnitActor		**GetRevealedActors(void) const { return m_revealedActors; }
	void			SetNumRevealedActors(sint32 num) { m_numRevealedActors = num; }
	sint32			GetNumRevealedActors(void) const { return m_numRevealedActors; }

	void			SetMoveActors(UnitActor **moveActors, sint32 numOActors)
    {
        m_moveActors = moveActors;
        m_numOActors = numOActors;
    }
	UnitActor		**GetMoveActors(void) const { return m_moveActors; }

	void			SetNumOActors(sint32 val) { m_numOActors = val; }
	sint32			GetNumOActors(void) const { return m_numOActors; }

	void			SetUnitsVisibility(uint32 unitsVis) { m_unitsVisibility = unitsVis; }
	uint32			GetUnitsVisibility(void) const { return m_unitsVisibility; }

	void			SetUnitVisionRange(double visRange) { m_unitVisionRange = visRange; }
	double			GetUnitVisionRange(void) const { return m_unitVisionRange; }

	void			SetSequence(Sequence *seq) { m_sequence = seq; }
	Sequence		*GetSequence(void) const { return m_sequence; }

	void			SetSoundEffect(sint32 sound_id) { m_sound_effect_id = sound_id; }
	sint32  		GetSoundEffect(void) const { return m_sound_effect_id; }

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

	bool				m_itIsTimeToAct;
	bool				m_finished;
	bool				m_loopAnimFinished;
	sint32  			m_specialDelayProcess;

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

	Sequence			*m_sequence;
};

#endif
