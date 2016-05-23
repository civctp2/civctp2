//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : The battleview
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
// - None
//
//----------------------------------------------------------------------------
//
// Modifications from the original Activision code:
//
// - Removed reference to outdated special effect database. (Aug 26th 2005 Martin Gühmann)
// - Removed unnecessary include files. (Aug 28th 2005 Martin Gühmann)
//
//----------------------------------------------------------------------------

#include "c3.h"
#include "battleview.h"

#include <algorithm>
#include "aui_factory.h"
#include "aui_blitter.h"
#include "c3ui.h"
#include "battleviewactor.h"
#include "battle.h"
#include "battleevent.h"
#include "GameEventManager.h"
#include "CTP2Combat.h"

extern C3UI					*g_c3ui;

namespace
{
    /// @todo Move to BattleEvent
    bool IsAfterAttack(BattleEvent const & a_Event)
    {
        BATTLE_EVENT_TYPE type  = a_Event.GetType();
	    return (type == BATTLE_EVENT_TYPE_PLACEMENT)  ||
			   (type == BATTLE_EVENT_TYPE_EXPLODE)    ||
			   (type == BATTLE_EVENT_TYPE_DEATH);
    };
}

BattleView::BattleView()
:
	m_battle                    (NULL),
//  m_battleViewRect
	m_battleSurface             (NULL),
	m_backgroundImage           (NULL),
	m_cityImage                 (NULL),
	m_numAttackers              (0),
	m_numDefenders              (0),
	m_eventQueue                (NULL),
	m_activeEvents              (new PointerList<BattleEvent>),
    m_walker                    (new PointerList<BattleEvent>::Walker),
	m_activeEvent               (NULL),
	m_cityBonus                 (0.0),
	m_citylandattackBonus       (0.0),
	m_cityairattackBonus        (0.0),
	m_cityseaattackBonus        (0.0),
	m_terrainBonus              (0.0),
	m_fortBonus                 (0.0),
	m_fortifiedBonus            (0.0)
{
    std::fill(m_attackers, m_attackers + k_MAX_UNITS_PER_SIDE,
              (BattleViewActor *) NULL
             );
    std::fill(m_defenders, m_defenders + k_MAX_UNITS_PER_SIDE,
              (BattleViewActor *) NULL
              );
}


BattleView::~BattleView()
{
	m_walker->SetList(m_eventQueue);
	while (m_walker->IsValid())
    {
		delete m_walker->Remove();
	}

	m_walker->SetList(m_activeEvents);
	while (m_walker->IsValid())
    {
		delete m_walker->Remove();
	}

    if (g_c3ui)
    {
        if (m_backgroundImage)
		    g_c3ui->UnloadImage(m_backgroundImage);

	    if (m_cityImage)
		    g_c3ui->UnloadImage(m_cityImage);
    }

	delete m_eventQueue;
    delete m_activeEvents;
	delete m_activeEvent;
	delete m_battleSurface;
	delete m_walker;

	for (int i = 0; i < k_MAX_UNITS_PER_SIDE; i++)
    {
		delete m_attackers[i];
		delete m_defenders[i];
	}
}

void BattleView::Initialize(RECT const & battleViewRect)
{
	AUI_ERRCODE	errcode;
	sint32      width   = battleViewRect.right - battleViewRect.left;
	sint32      height  = battleViewRect.bottom - battleViewRect.top;

	m_battleViewRect    = battleViewRect;
	delete m_battleSurface;
	m_battleSurface     = aui_Factory::new_Surface(errcode, width, height);
	Assert(m_battleSurface);
}

void BattleView::SetBattle(Battle *battle)
{

	m_numAttackers = battle->GetNumAttackers();
	m_numDefenders = battle->GetNumDefenders();
	for(sint32 index = 0; index < k_MAX_UNITS_PER_SIDE; index++) {
		m_attackers[index] = battle->GetAttacker(index);
		m_defenders[index] = battle->GetDefender(index);
	}


	delete m_eventQueue;
	m_eventQueue = battle->GrabEventQueue();

	m_battle = battle;
}

void BattleView::UpdateBattle(Battle *battle)
{

	delete m_eventQueue;
	m_eventQueue = battle->GrabEventQueue();
}


void BattleView::GetAttackerPos(sint32 column, sint32 row, sint32 *x, sint32 *y)
{
#define k_BV_ATT_00_X_POS 105
#define k_BV_ATT_00_Y_POS 75
#define k_BV_ATT_X_ROW_OFFSET (58)
#define k_BV_ATT_X_COL_OFFSET (-58)
#define k_BV_ATT_Y_ROW_OFFSET (34)
#define k_BV_ATT_Y_COL_OFFSET (34)

	*x = k_BV_ATT_00_X_POS + (row * k_BV_ATT_X_ROW_OFFSET) + (column * k_BV_ATT_X_COL_OFFSET);
	*y = k_BV_ATT_00_Y_POS + (row * k_BV_ATT_Y_ROW_OFFSET) + (column * k_BV_ATT_Y_COL_OFFSET);

}


void BattleView::GetDefenderPos(sint32 column, sint32 row, sint32 *x, sint32 *y)
{
#define k_BV_DEF_00_X_POS 163
#define k_BV_DEF_00_Y_POS 41
#define k_BV_DEF_X_ROW_OFFSET (58)
#define k_BV_DEF_X_COL_OFFSET (58)
#define k_BV_DEF_Y_ROW_OFFSET (34)
#define k_BV_DEF_Y_COL_OFFSET (-34)

	*x = k_BV_DEF_00_X_POS + (row * k_BV_DEF_X_ROW_OFFSET) + (column * k_BV_DEF_X_COL_OFFSET);
	*y = k_BV_DEF_00_Y_POS + (row * k_BV_DEF_Y_ROW_OFFSET) + (column * k_BV_DEF_Y_COL_OFFSET);

}












void BattleView::DrawExplosions(void)
{
	PointerList<BattleEvent>::Walker *walker = new PointerList<BattleEvent>::Walker(m_activeEvents);

	while (walker->IsValid()) {
		BattleEvent *event = walker->GetObj();
		if (event) {
			event->DrawExplosions(m_battleSurface);
		}
		walker->Next();
	}

	delete walker;
}


int battleview_AttackerSort( const void *arg1, const void *arg2 )
{
	SortedActor	*sa1 = (SortedActor *)arg1,
				*sa2 = (SortedActor *)arg2;

	if (sa1->y < sa2->y) {
		return -1;
	} else
	if (sa1->y == sa2->y) {
		if (sa1->x > sa2->x)
			return -1;
		else
			return 1;
	} else
		return 1;
}


void BattleView::DrawAttackers(void)
{
	sint32		i;
	sint32		x, y;

	SortedActor	sortedAttackers[k_MAX_UNITS_PER_SIDE];

	for (i=0; i<m_numAttackers; i++) {
		m_attackers[i]->GetPixelPos(x, y);
		sortedAttackers[i].x = x;
		sortedAttackers[i].y = y;
		sortedAttackers[i].actor = m_attackers[i];
	}

	qsort((void *)sortedAttackers, m_numAttackers, sizeof(SortedActor), battleview_AttackerSort);

	for (i=0; i<m_numAttackers; i++) {
		sortedAttackers[i].actor->DrawDirect(m_battleSurface, sortedAttackers[i].x, sortedAttackers[i].y);
	}







}


int battleview_DefenderSort( const void *arg1, const void *arg2 )
{
	SortedActor	*sa1 = (SortedActor *)arg1,
				*sa2 = (SortedActor *)arg2;

	if(sa1->x < sa2->x) {
		return 1;
	} else if(sa1->x == sa2->x) {
		if(sa1->y < sa2->y) {
			return -1;
		} else {
			return 1;
		}
	} else {
		return -1;
	}

#if 0

	if (sa1->y < sa2->y) {
		return -1;
	} else
	if (sa1->y == sa2->y) {
		if (sa1->x < sa2->x)
			return -1;
		else
			return 1;
	} else
		return 1;
#endif
}




void BattleView::DrawDefenders(void)
{
	sint32		i;
	sint32		x, y;

	SortedActor	sortedDefenders[k_MAX_UNITS_PER_SIDE];

	for (i=0; i<m_numDefenders; i++) {
		m_defenders[i]->GetPixelPos(x, y);
		sortedDefenders[i].x = x;
		sortedDefenders[i].y = y;
		sortedDefenders[i].actor = m_defenders[i];
	}

	qsort((void *)sortedDefenders, m_numDefenders, sizeof(SortedActor), battleview_DefenderSort);

	for (i=0; i<m_numDefenders; i++) {
		sortedDefenders[i].actor->DrawDirect(m_battleSurface, sortedDefenders[i].x, sortedDefenders[i].y);
	}







}


void BattleView::RemoveAttacker(sint32 index)
{
	if(m_battle) {
		m_battle->RemoveAttacker(m_attackers[index]);
	}

	delete m_attackers[index];

	for(sint32 i = index; i < m_numAttackers - 1; i++) {
		m_attackers[i] = m_attackers[i + 1];
	}

	m_attackers[m_numAttackers-1] = NULL;

	m_numAttackers--;
}


void BattleView::RemoveDefender(sint32 index)
{
	if(m_battle) {
		m_battle->RemoveDefender(m_defenders[index]);
	}

	delete m_defenders[index];

	for(sint32 i = index; i < m_numDefenders - 1; i++) {
		m_defenders[i] = m_defenders[i + 1];
	}

	m_defenders[m_numDefenders-1] = NULL;

	m_numDefenders--;
}


void BattleView::RemoveActor(BattleViewActor *actor)
{
	sint32 i;

	Assert(actor);
	if (actor == NULL) return;

	PointerList<BattleEvent>::Walker walk(m_eventQueue);
	for(; walk.IsValid(); walk.Next()) {
		walk.GetObj()->RemoveDeadActor(actor);
	}

	for(walk.SetList(m_activeEvents); walk.IsValid(); walk.Next()) {
		walk.GetObj()->RemoveDeadActor(actor);
	}

	for (i=0; i<m_numAttackers; i++) {
		if (actor == m_attackers[i]) {
			RemoveAttacker(i);
			return;
		}
	}

	for (i=0; i<m_numDefenders; i++) {
		if (actor == m_defenders[i]) {
			RemoveDefender(i);
			return;
		}
	}

	Assert(FALSE);
}


void BattleView::UpdateDisplay(void)
{

	if (!m_backgroundImage)
		return;

	RECT rect = {0, 0,
					m_backgroundImage->TheSurface()->Width(),
					m_backgroundImage->TheSurface()->Height()};

	g_c3ui->TheBlitter()->Blt(m_battleSurface, 0, 0, m_backgroundImage->TheSurface(),
							&rect, k_AUI_BLITTER_FLAG_COPY);

	if(m_cityImage) {
		RECT cityrect = {0, 0,
				m_cityImage->TheSurface()->Width(),
				m_cityImage->TheSurface()->Height() };

		g_c3ui->TheBlitter()->Blt(m_battleSurface, m_battleSurface->Width() - cityrect.right, 0,
								  m_cityImage->TheSurface(), &cityrect, k_AUI_BLITTER_FLAG_COPY);
	}

	Assert(m_numAttackers <= k_MAX_UNITS_PER_SIDE);
	Assert(m_numDefenders <= k_MAX_UNITS_PER_SIDE);


	DrawDefenders();
	DrawAttackers();




	DrawExplosions();
}

#define k_EVENT_FREQUENCY		1500

void BattleView::Process(void)
{

	Assert(m_eventQueue);
	if(!m_eventQueue)
		return;

	PointerList<BattleEvent>::PointerListNode *eventNode =
		m_eventQueue->GetHeadNode();

	while (eventNode)
    {
		BattleEvent *       event           = eventNode->GetObj();
		Assert(event);
		BattleViewActor *   actor           = event->GetActor();
		bool                addEvent        = true;
        bool                isAfterAttack   = IsAfterAttack(*event);
		PointerList<BattleEvent>::PointerListNode *
                            activeNode  = m_activeEvents->GetHeadNode();

		while (activeNode)
        {
            if (isAfterAttack &&
				(activeNode->GetObj()->GetType() == BATTLE_EVENT_TYPE_ATTACK)
               )
            {
				addEvent = false;
				break;
			}




			if((event->GetType() == BATTLE_EVENT_TYPE_PLACEMENT) &&
				(activeNode->GetObj()->GetType() != BATTLE_EVENT_TYPE_PLACEMENT)) {
				addEvent = false;
				break;
			}




			if(activeNode->GetObj()->GetActor() == actor) {
				addEvent = false;
				break;
			}

			activeNode = activeNode->GetNext();
		}

		if(addEvent) {

			PointerList<BattleEvent>::PointerListNode *addNode = eventNode;

			eventNode = eventNode->GetNext();

			m_eventQueue->Remove(addNode);

			m_activeEvents->AddTail(event);
		} else {

			eventNode = eventNode->GetNext();

		}
	}

	if(m_activeEvents->GetCount() > 0) {

		m_walker->SetList(m_activeEvents);


		while (m_walker->IsValid())
        {
			BattleEvent *   event = m_walker->GetObj();
			Assert(event);
			event->Process();

			if (event->IsFinished())
            {
				m_walker->Remove();
				delete event;

				if (!IsProcessing() && (!g_theCurrentBattle || !g_theCurrentBattle->IsDone()))
                {
					g_gevManager->GotUserInput();
                }
			}
            else
            {
				m_walker->Next();
            }
		}
	}
}
