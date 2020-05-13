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

#include "c3.h"
#include "Action.h"

#include "ActorPath.h"
#include "Anim.h"
#include "UnitActor.h"
#if 0
#include "Actor.h"
#include "debugmemory.h"
#include "director.h"       // g_director
#include "EffectActor.h"
#include "pixelutils.h"
#include "profileDB.h"      // g_theProfileDB
#include "spriteutils.h"
#include "tech_wllist.h"
#include "Unit.h"
#endif

#define STOMPCHECK()

Action::Action(sint32 actionType, ACTIONEND endCondition, sint32 startAnimPos, sint32 specialDelayProcess)
:
    m_actionType                (actionType),
    m_endCondition              (endCondition),
	m_curAnim                   (NULL),
	m_curPath                   (NULL),
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
	m_numRevealedActors         (0),
	m_revealedActors            (NULL),
	m_moveActors                (NULL),
	m_numOActors                (0),
    m_sequence                  (NULL)
{ ; }

Action::Action(Action const & a_Original)
:
    m_actionType                (a_Original.m_actionType),
    m_endCondition              (a_Original.m_endCondition),
	m_curAnim                   (NULL),
	m_curPath                   (NULL),
	m_maxActionCounter          (a_Original.m_maxActionCounter),
	m_curActionCounter          (a_Original.m_curActionCounter),
	m_animPos                   (a_Original.m_animPos),
	m_animDelayEnd              (a_Original.m_animDelayEnd),
	m_animElapsed               (a_Original.m_animElapsed),
	m_animLastFrameTime         (a_Original.m_animLastFrameTime),
	m_delay                     (a_Original.m_delay),
	m_itIsTimeToAct             (a_Original.m_itIsTimeToAct),
	m_finished                  (a_Original.m_finished),
	m_loopAnimFinished          (a_Original.m_loopAnimFinished),
	m_specialDelayProcess       (a_Original.m_specialDelayProcess),
    m_startMapPoint             (a_Original.m_startMapPoint),
	m_endMapPoint               (a_Original.m_endMapPoint),
	m_facing                    (a_Original.m_facing),
	m_sound_effect_id           (a_Original.m_sound_effect_id),
	m_unitsVisibility           (a_Original.m_unitsVisibility),
	m_unitVisionRange           (a_Original.m_unitVisionRange),
	m_numRevealedActors         (a_Original.m_numRevealedActors),
	m_revealedActors            (NULL),
	m_moveActors                (NULL),
	m_numOActors                (a_Original.m_numOActors),
    /// @todo Check copying of m_sequence (shallow copy in original code, not deleted in destructor)
    m_sequence                  (a_Original.m_sequence)
{
    if (a_Original.m_curAnim)
    {
        m_curAnim = new Anim(*a_Original.m_curAnim);
    }

    /// @todo Check copying of m_curPath, m_moveActors, m_revealedActors (NULLed in original code)
}

/// @todo Remove when no longer used
Action::Action(Action *copyme)
{
	*this = *copyme;

	if (copyme->GetAnim()) {
		m_curAnim = new Anim(*copyme->GetAnim());
	}

	m_curPath = NULL;
	m_moveActors = NULL;
	m_revealedActors = NULL;
}

Action::~Action(void)
{
	delete m_curPath;
	delete m_curAnim;
	delete[] m_moveActors;
	delete[] m_revealedActors;
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

void Action::Process(Action *pendingAction)
{
#ifndef _TEST
	STOMPCHECK();
#endif

	if(pendingAction != this )
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

	delete m_curPath;
	m_curPath = new ActorPath(x1, y1, x2, y2);

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
