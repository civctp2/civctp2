#include "c3.h"

  
#include "FlatPtr.h"
#include "CivArchive.h"

#include "AiMain.h"

#include "GlTrans.h"


#include "MapPoint.h"


#include "GlSettl.h"

#include "AILOG.h"

ZEROMEM(GoalTransportFlat); 
ZEROMEM(GoalTransportPtr); 
ZERODEL(GoalTransportPtr); 
FLATSERIALIZE(GoalTransportFlat); 
PTRSERIALIZE(GoalTransportPtr); 

GoalTransport::GoalTransport()
{ 
}

GoalTransport::GoalTransport(GoalSettle *gs)
{ 

	what_goal = GOAL_TYPE_TRANSPORT; 
    m_value = gs->m_value; 
    m_satisfied = FALSE;  
	m_pos = new MapPointData;
    *m_pos = *(gs->m_pos);
}

GoalTransport::GoalTransport(AiMain *ai,CivArchive &archive)
{ 
	what_goal = GOAL_TYPE_TRANSPORT; 
    Serialize(ai, archive); 
} 

GoalTransport::~GoalTransport()
{ 
    delete m_pos; 
    m_pos = NULL; 
} 

void GoalTransportPtr::Store (CivArchive &archive)
{
}

void GoalTransportPtr::Load(CivArchive &archive, sint32 pcount, uint8 nmask[])
{
}

void GoalTransport::Serialize(AiMain *ai,CivArchive &archive)
{
    CHECKSERIALIZE 

    ArmyGoal::Serialize(ai, archive); 
    GoalTransportFlat::Serialize(archive); 
    GoalTransportPtr::Serialize(archive); 
}


BOOL GoalTransport::Validate(AiMain *ai)
{
    return ArmyGoal::Validate(ai); 
}

GOAL_TYPE GoalTransport::GetType() const

{ 
    return GOAL_TYPE_TRANSPORT; 
} 







void GoalTransport::Display_Goal_Type(AiMain *ai)
{
#ifdef DBGAILOG
	
	if (ai->AI_VERBOSITY >= LOG_MEDIUM)
		AILOG(( wstr,  "GOAL_TYPE_TRANSPORT"));
#endif DBGAILOG
}




