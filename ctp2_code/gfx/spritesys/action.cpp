//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
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
/// \file   Action.cpp
/// \brief  Action on the screen

#include "ctp/c3.h"
#include "gfx/spritesys/Action.h"

#include "gfx/spritesys/ActorPath.h"
#include "gfx/spritesys/Anim.h"
#include "gfx/spritesys/UnitActor.h"
#if 0
#include "gfx/spritesys/Actor.h"
#include "ctp/debugtools/debugmemory.h"
#include "gfx/spritesys/director.h"       // g_director
#include "gfx/spritesys/EffectActor.h"
#include "gfx/gfx_utils/pixelutils.h"
#include "gs/database/profileDB.h"      // g_theProfileDB
#include "gfx/spritesys/spriteutils.h"
#include "ui/aui_common/tech_wllist.h"
#include "gs/gameobj/Unit.h"
#endif

#define STOMPCHECK()

Action::Action(sint32 actionType, ACTIONEND endCondition, sint32 startAnimPos, sint32 specialDelayProcess) :
    m_actionType                (actionType),
    m_endCondition              (endCondition),
	  m_curAnim                   (NULL),
	  m_maxActionCounter          (0),
	  m_curActionCounter          (0),
	  m_animPos                   (startAnimPos),
	  m_animDelayEnd              (0),
	  m_animElapsed               (0),
	  m_animLastFrameTime         (0),
	  m_delay                     (0),
	  m_itIsTimeToAct             (false),
	  m_finished                  (false),
	  m_loopAnimFinished          (false),
	  m_specialDelayProcess       (specialDelayProcess),
    m_startMapPoint             (),
	  m_endMapPoint               (),
	  m_facing                    (k_DEFAULTSPRITEFACING),
	  m_sound_effect_id           (-1),
	  m_unitsVisibility           (0),
	  m_unitVisionRange           (0),
    m_numOActors(0),
    m_specialUnitEffectsAction  (),
    m_sequence                  (NULL) {
}

Action::Action(const Action &rhs):
  m_actionType                (rhs.m_actionType),
  m_endCondition              (rhs.m_endCondition),
	m_curAnim                   (NULL),
	m_maxActionCounter          (rhs.m_maxActionCounter),
	m_curActionCounter          (rhs.m_curActionCounter),
	m_animPos                   (rhs.m_animPos),
	m_animDelayEnd              (rhs.m_animDelayEnd),
	m_animElapsed               (rhs.m_animElapsed),
	m_animLastFrameTime         (rhs.m_animLastFrameTime),
	m_delay                     (rhs.m_delay),
	m_itIsTimeToAct             (rhs.m_itIsTimeToAct),
	m_finished                  (rhs.m_finished),
	m_loopAnimFinished          (rhs.m_loopAnimFinished),
	m_specialDelayProcess       (rhs.m_specialDelayProcess),
  m_startMapPoint             (rhs.m_startMapPoint),
	m_endMapPoint               (rhs.m_endMapPoint),
	m_facing                    (rhs.m_facing),
	m_sound_effect_id           (rhs.m_sound_effect_id),
	m_unitsVisibility           (rhs.m_unitsVisibility),
	m_unitVisionRange           (rhs.m_unitVisionRange),
  m_numOActors                (0),
  m_specialUnitEffectsAction  (rhs.m_specialUnitEffectsAction),
    /// @todo Check copying of m_sequence (shallow copy in original code, not deleted in destructor)
    /// done : Action dows not own sequence object, shallow copy is OK
    m_sequence                  (rhs.m_sequence)
{
    if (rhs.m_curAnim)
    {
        m_curAnim = new Anim(*rhs.m_curAnim);
    }

    /// @todo Check copying of m_curPath, m_moveActors, m_revealedActors (NULLed in original code)
}

Action::~Action(void)
{
	delete m_curAnim;
  m_curAnim = NULL;
}

void Action::Process(void)
{
#ifndef _TEST
	STOMPCHECK();
#endif

	if (!m_curAnim)
    {
        SetFinished(true);
        return;
    }

	m_animPos = m_curAnim->GetNextPosition(m_animPos);
	m_animDelayEnd = m_curAnim->GetDelayEnd();
	m_animElapsed = m_curAnim->GetElapsed();
	m_animLastFrameTime = m_curAnim->GetLastFrameTime();

	m_specialDelayProcess = m_curAnim->GetWeAreInDelay() && (m_actionType == UNITACTION_IDLE);
	if (m_specialDelayProcess)
    {
		m_animPos = 0;
	}

	m_curActionCounter++;

	if (m_curActionCounter > m_maxActionCounter || m_curAnim->Finished())
	{
		m_curActionCounter = m_maxActionCounter;
		m_loopAnimFinished = true;

		if ( m_endCondition == ACTIONEND_ANIMEND ||
			m_endCondition == ACTIONEND_PATHEND )
		{
			SetFinished(true);
		}
	}

}

void Action::Process(ActionPtr pendingAction)
{
#ifndef _TEST
	STOMPCHECK();
#endif

	if(pendingAction.get() != this )
		Process();

	if (m_endCondition == ACTIONEND_INTERRUPT || Finished())
	{
		pendingAction->SetItIsTimeToAct(true);
		SetFinished(true);
	}

#ifndef _TEST
	STOMPCHECK();
#endif
}

void Action::SetAnim(Anim *anim)
{
#ifndef _TEST
	STOMPCHECK();
#endif

//	Assert(anim != NULL);
	if (anim == NULL) return;

	anim->SetFinished(false);

    m_maxActionCounter = anim->GetNumFrames();

	m_curAnim = anim;

#ifndef _TEST
	STOMPCHECK();
#endif
}

void Action::CreatePath(sint32 x1, sint32 y1, sint32 x2, sint32 y2)
{
#ifndef _TEST
	STOMPCHECK();
#endif

	m_curPath.reset(new ActorPath(x1, y1, x2, y2));

	m_maxActionCounter = k_MAX_UNIT_MOVEMENT_ITERATIONS;

#ifndef _TEST
	STOMPCHECK();
#endif

}

POINT Action::GetPosition(void) const
{
	POINT pos = {0,0};

	if (m_curPath)
    {
    	m_curPath->CalcPosition(0, m_maxActionCounter, m_curActionCounter, &pos);
    }

	return pos;
}

uint16 Action::GetSpriteFrame(void) const
{
	uint16 frame;

	if (m_curAnim) {
		if (m_loopAnimFinished && m_curActionCounter == m_maxActionCounter) {
			frame = 0;
		} else {
			frame = m_curAnim->GetFrame(m_animPos);
		}

	} else {
		frame = 0;
	}

	return frame;
}

sint32 Action::GetFacing(void)
{
#ifndef _TEST
	STOMPCHECK();
#endif

	if (m_curPath)
  {
		m_facing = m_curPath->CalcFacing(0, m_maxActionCounter, m_curActionCounter);
	}
    // else: No action: keep current facing

	return m_facing;
}

uint16 Action::GetTransparency(void) const
{
#ifndef _TEST
	STOMPCHECK();
#endif

	uint16	trans = 15;
	uint16	*transparencies;

	if (m_curAnim != NULL && m_animPos < m_curAnim->GetNumFrames()) {
		transparencies = m_curAnim->GetTransparencies();
		if (transparencies) {
			trans = transparencies[m_animPos];
		}

	}

	return trans;
}

void Action::SetMoveActors(const std::vector<std::weak_ptr<UnitActor> > &moveActors) {
  m_moveActors = moveActors;
  m_numOActors = m_moveActors.size();
}

void Action::SetRevealedActors(const std::vector<std::weak_ptr<UnitActor> > &revealedActors) {
  m_revealedActors = revealedActors; 
}
