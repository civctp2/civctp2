#include "c3.h"

#include "RouteUtilityList.h"

#include "globals.h"
#include "civarchive.h"
#include "bset.h"

#include "ic3GameState.h"
#include "aimain.h"

#include "CityAgent.h"

#include "GlRoute.h"

RouteUtilityList::RouteUtilityList()
{
    m_num_goals = 0;
    m_max_goals = 8;
    m_goal = new GoalRoute*[m_max_goals];

    sint32 i;
    for (i=0; i<m_max_goals; i++) {
        m_goal[i] = NULL;
    }
}

RouteUtilityList::RouteUtilityList(BSet<CityAgent> *bs, CivArchive &archive)
{
    Serialize(bs, archive);
}

RouteUtilityList::~RouteUtilityList()
{
    sint32 i;

    Assert(m_goal);
    for (i=0; i<m_num_goals; i++) {
        Assert(m_goal[i]);
        NDELETE(m_goal[i]);
    }
    delete[] m_goal;
    m_goal=NULL;
}

void RouteUtilityList::Serialize(BSet<CityAgent> *bs, CivArchive &archive)
{

    if (archive.IsStoring()) {
        archive << m_num_goals;
        archive << m_max_goals;

        sint32 i;
        for (i=0; i<m_num_goals; i++) {
            m_goal[i]->Serialize(bs, archive);
        }
    } else {
        archive >> m_num_goals;
        archive >> m_max_goals;

        sint32 i;
        m_goal = new GoalRoute*[m_max_goals];

        for (i=0; i<m_num_goals; i++) {
            m_goal[i] = new GoalRoute(bs, archive);
        }

        for (i=m_num_goals; i<m_max_goals; i++) {
            m_goal[i] = NULL;
        }
    }
}

void RouteUtilityList::DoubleGoals()
{
    GoalRoute **tmp;

    tmp = new GoalRoute*[2*m_max_goals];
    memcpy(tmp, m_goal, m_num_goals * sizeof(GoalRoute*));
    delete[] m_goal;
    m_goal = tmp;
    m_max_goals *= 2;
}

void RouteUtilityList::InsertGoal(GoalRoute *g)
{
    if (m_max_goals <= m_num_goals) {
        DoubleGoals();
    }

    m_goal[m_num_goals] = g;
    m_num_goals++;
}

GoalRoute * RouteUtilityList::GetGoal(sint32 i)
{
    Assert(0 <= i);
    Assert(i <= m_num_goals);
    return m_goal[i];
}

sint32 RouteUtilityList::GetNumGoals()
{
    return m_num_goals;
}

void RouteUtilityList::Clear()
{
    sint32 i;

    for (i=0; i<m_num_goals; i++) {
        NDELETE(m_goal[i]);
    }
    m_num_goals = 0;
}
