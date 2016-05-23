#include "c3.h"
#include "AiBldQue.h"

#include "civarchive.h"

#include "bset.h"
#include "IMapPointData.h"
#include "globals.h"
#include "ic3GameState.h"
#include "aimain.h"
#include "ArmyAgent.h"
#include "bset.h"

AiBuildNode::AiBuildNode()
{
    m_cost = 0;
    m_category =  BQ_CATEGORY_END;
    m_data = NULL;
    m_next = NULL;
    m_id = NULL;
}

AiBuildNode::AiBuildNode(BQ_CATEGORY cat,  sint32 type, sint32 cost, Agent *aa,
     const BSetID &army_agent_id, const double utility)
{
    m_category = cat;
    m_type = type;
    m_cost = cost;
    m_utility = utility;
    m_data = aa;
  	m_next = NULL;
    m_id = new BSetID;
    *m_id = army_agent_id;
}

AiBuildNode::~AiBuildNode()

{
    Assert(m_id);
    NDELETE(m_id);
}

AiBuildQueue::AiBuildQueue()
{
    m_len=0;
	m_head = NULL;
	m_tail = NULL;
}

AiBuildQueue::~AiBuildQueue()
{
    AiBuildNode *ptr=NULL, *old=NULL;
    ptr = m_head;

#ifdef _DEBUG
    sint32 finite_loop=0;
#endif

    while (ptr) {
Assert(++finite_loop < 100000);
        old = ptr;
        ptr = ptr->m_next;

        NDELETE(old);
    }
}

AiBuildQueue::AiBuildQueue(CivArchive &archive)
{
    Serialize(archive);
}

void AiBuildQueue::Serialize(CivArchive &archive)
{
    AiBuildNode *p, **pptr;
    sint32 i;

    CHECKSERIALIZE

    if (archive.IsStoring()) {
        archive << m_len;
        for (p = m_head, i=0; i<m_len; i++, p = p->m_next) {
            archive.PutSINT32(p->m_category);
            archive << p->m_type;
            archive << p->m_cost;
            archive << p->m_utility;
            archive.PutUINT32(p->m_id->GetVal());
        }
    } else {
       archive >> m_len;

       if (0 < m_len) {
            pptr = &m_head;

            for (i=0; i<m_len; i++, pptr = &((*pptr)->m_next)) {
                *pptr = new AiBuildNode();
                m_tail = *pptr;
                (*pptr)->m_category = BQ_CATEGORY(archive.GetSINT32());
                archive >> (*pptr)->m_type;
                archive >> (*pptr)->m_cost;
                archive >> (*pptr)->m_utility;
                (*pptr)->m_id = new BSetID;
                *((*pptr)->m_id) = archive.GetUINT32();
                 (*pptr)->m_data = NULL;
            }

            *pptr = NULL;
       }  else {
           m_head = NULL;
           m_tail = NULL;
       }
    }
}

void AiBuildQueue::HookBldQueue(AiMain *ai)
{
    AiBuildNode *p;

    for (p = m_head; p; p = p->m_next) {
        if (p->m_category == BQ_CATEGORY_ARMY) {
            Assert(p->m_id->GetVal() != 0);
            p->m_data = ai->m_army_set->Find(*(p->m_id));
            Assert(p->m_data);
        }
    }
}

void AiBuildQueue::Enqueue(BQ_CATEGORY cat, sint32 type, sint32 cost,
     Agent *aa, double utility)

{
    AiBuildNode *b;

    Assert(BAD_UTILITY <= utility);

#ifdef _DEBUG
if (cat == BQ_CATEGORY_ARMY) {
    Assert(type != 0);
    Assert(type != 1);
}

#endif

    BSetID null_id(0);

    switch (cat) {
    case BQ_CATEGORY_ARMY:
        Assert(aa);
        b = new AiBuildNode (cat, type, cost, aa, aa->m_id, utility);
        break;
    case BQ_CATEGORY_BUILDING:
    case BQ_CATEGORY_FREIGHT:
    case BQ_CATEGORY_WONDER:
        Assert(aa == NULL);
        b = new AiBuildNode (cat, type, cost, NULL, null_id, utility);
        break;
    default:
        INSANE(UNKNOWN_BLG_ID);
    }

    if (m_head == NULL) {
        m_head = b;
        m_tail = b;
    } else {

INSANE(GOALS_SHOULD_NOT_BLD_QUEUE);






        m_tail->m_next = b;
        m_tail = b;
    }
    m_len++;
}

void AiBuildQueue::ChangeBuildHead(AiMain *ai, BQ_CATEGORY cat, sint32 item_type, sint32 cost)
{
    Assert(m_head);

    m_head->m_category = cat;
    m_head->m_type = item_type;
    m_head->m_cost = cost;

    switch (cat) {
    case BQ_CATEGORY_ARMY:
        ((ArmyAgent*)m_head->m_data)->ChangeBuildingType(ai, item_type);
        break;
    default:
        {
            BOOL CHANGE_BUILD_HEAD_UNKNOWN=0;
            Assert(CHANGE_BUILD_HEAD_UNKNOWN);
        }
        return;
    }
}

AiBuildNode* AiBuildQueue::Dequeue()

{

    AiBuildNode *tmp;

    tmp = m_head;

	if(!tmp)
		return NULL;

    m_head = m_head->m_next;
    if (m_head == NULL) {
        m_tail = NULL;
    }
    m_len--;
    return tmp;
}

BSetID AiBuildQueue::PeekHeadID() const
{
    if (m_head == NULL) {
        return BSetID(0);
    } else {
        Assert(m_head->m_data);
		#ifdef _DEBUG
		if (!m_head->m_data)
			return 0;
		#endif

        return m_head->m_data->GetID();
    }
}

sint32 AiBuildQueue::EstimateRoundsToFinished(AiMain *ai, BSetID id,
    const sint32 production_store, const sint32 net_production) const
{
    sint32 sum = 0, diff;
    AiBuildNode *p;

    if (m_head == NULL) {
        return 0;
    }

    diff = m_head->m_cost - production_store;
    if ( diff < 1) {
        sum = 1;
    } else {
        sum = int (0.5 + diff / net_production);
    }

    if (*(m_head->m_id) == id) {
        return sum;
    }

    for (p=m_head->m_next; p ; p = p->m_next) {
        sum += int(0.5 + p->m_cost / net_production);
        if (*(p->m_id) == id) {
            return sum;
        }
    }

    INSANE(ID_NOT_IN_QUEUE);
    return 1000000;
}

double AiBuildQueue::EstimatePercentItem(sint32 production_store)
{
    double cost = 0;
	double percent;

	if (m_head == NULL) {
        return 1;
    }

    cost = m_head->m_cost;

	if(cost == 0) {cost == 0.000001; }

	percent = production_store/cost;

	if(percent < 0 || percent > 1)
	{
		percent = 1.0;
	}
	return percent;
}

sint32 AiBuildQueue::TurnsToFinishFront(sint32 production_store, sint32 net_production)
{
    double diff = 0, sum;

	if (m_head == NULL)
	{
        return 0;
    }

    diff = m_head->m_cost - production_store;
    if ( diff < 1) {
        sum = 1;
    } else {
        sum = int (0.5 + diff / net_production);
    }

	return sint32(sum);

}

sint32 AiBuildQueue::EstimateRoundsToEmptyQueue(AiMain *ai,
    const sint32 production_store, sint32 net_production ) const
{
    sint32 sum=0;
    sint32 diff;
    AiBuildNode *p=NULL;

    if (m_head == NULL) {
        return 0;
    }

    if (net_production < 1 ) {
        net_production =1;
    }
    diff = m_head->m_cost - production_store;

    if ( diff < 1) {
        sum = 1;
    } else {
        sum = int (0.5 + diff / net_production);
    }

    for (p=m_head->m_next; p ; p = p->m_next) {
        sum += int(0.5 + (double(p->m_cost) / double(net_production)));
    }

    return sum;
}

BOOL AiBuildQueue::GetHead(BQ_CATEGORY &cat, sint32 &type)

{
    if (m_head) {
        cat = m_head->m_category;
        type = m_head->m_type;
        return TRUE;
    } else {
        return FALSE;
    }
}

double AiBuildQueue::GetFrontUtility() const

{
    Assert(m_head);

    if (m_head == NULL) {
        INSANE(NOTHING_BUILDING);
        return 0.0;
    }

    return m_head->m_utility;
}

BOOL AiBuildQueue::FrontIsImprovement()
{
	if(m_head)
	{
		if(m_head->m_category == BQ_CATEGORY_BUILDING)
		{
			return TRUE;
		}
	}
	return FALSE;
}
