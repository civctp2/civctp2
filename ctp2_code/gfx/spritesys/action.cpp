









#include "c3.h"

#include "pixelutils.h"
#include "spriteutils.h"

#include "Actor.h"
#include "Unit.h"
#include "Anim.h"
#include "ActorPath.h"
#include "Action.h"
#include "director.h"
#include "UnitActor.h"
#include "tech_wllist.h"

#include "EffectActor.h"
#include "profileDB.h"
#include "debugmemory.h"

extern ProfileDB		*g_theProfileDB;
extern Director			*g_director;
extern double			g_ave_frame_rate;
extern double			g_ave_frame_time;







#define STOMPCHECK() ;





Action::Action(sint32 actionType, ACTIONEND endCondition, sint32 startAnimPos, BOOL specialDelayProcess)
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
	m_itIsTimeToAct             (FALSE), 
	m_finished                  (FALSE), 
	m_loopAnimFinished          (FALSE), 
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
    m_specialUnitEffectsAction  (), 
    m_sequence                  (NULL)
{ ; }


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
        SetFinished(TRUE);
        return;
    }
	
	m_animPos = m_curAnim->GetNextPosition(m_animPos);
	m_animDelayEnd = m_curAnim->GetDelayEnd();
	m_animElapsed = m_curAnim->GetElapsed();
	m_animLastFrameTime = m_curAnim->GetLastFrameTime();

	
	
	
	






	
	
	if(m_curAnim->GetWeAreInDelay() && m_actionType == UNITACTION_IDLE)
	{
		m_animPos = 0;
		m_specialDelayProcess = TRUE;
	}
	else
	{
		m_specialDelayProcess = FALSE;
	}

	m_curActionCounter++;


	
	

	if (m_curActionCounter > m_maxActionCounter || m_curAnim->Finished()) 
	{
		m_curActionCounter = m_maxActionCounter;
		m_loopAnimFinished = TRUE;

		if ( m_endCondition == ACTIONEND_ANIMEND ||
			m_endCondition == ACTIONEND_PATHEND )
		{
			SetFinished(TRUE);
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
		pendingAction->SetItIsTimeToAct(TRUE);
		SetFinished(TRUE);
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

	anim->SetFinished(FALSE);













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

	if (m_curPath != NULL) 
		delete m_curPath;

	ActorPath	*path = new ActorPath(x1, y1, x2, y2);

	m_curPath = path;



	m_maxActionCounter = k_MAX_UNIT_MOVEMENT_ITERATIONS;

#ifndef _TEST
	STOMPCHECK();
#endif

}

POINT Action::GetPosition(void)
{
#ifndef _TEST

#endif

	POINT pos = {0,0};

	if (m_curPath == NULL)
		return pos;

	m_curPath->CalcPosition(0, m_maxActionCounter, m_curActionCounter, &pos);
	return pos;
}

uint16 Action::GetSpriteFrame(void)
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

uint16 Action::GetTransparency(void)
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


