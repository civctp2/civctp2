

#pragma once

#ifndef __BATTLEVIEW_H__
#define __BATTLEVIEW_H__

#include "PointerList.h"

#define k_BATTLEVIEW_DEFAULT_ATTACKER_FACING		1
#define k_BATTLEVIEW_DEFAULT_DEFENDER_FACING		5

class BattleViewActor;
class aui_DirectSurface;
class aui_Control;
class c3_Button;
class c3_Static;
class BattleEvent;
class ProjectileActor;
class EffectActor;
class aui_Image;
class c3_Icon;
class Battle;





#define k_MAX_UNITS_PER_SIDE		12
struct SortedActor {
	sint32			x;
	sint32			y;
	BattleViewActor	*actor;
};


void battleview_ExitButtonActionCallback( aui_Control *control, uint32 action, 
											uint32 data, void *cookie );

class BattleView {
public:
	
	BattleView();

	
	~BattleView();

	
	
	
	bool IsProcessing() const
	{ return(!m_activeEvents->IsEmpty() || !m_eventQueue->IsEmpty()); }

	
	
	
	bool IsCurrentBattle(const Battle *battle) const
		{ 
			
			return(m_battle == battle); 
		}

	
	void Initialize(RECT *battleViewRect);
	void SetBattle(Battle *battle);
	void UpdateBattle(Battle *battle);
	void SetBackgroundImage(aui_Image *image) { m_backgroundImage = image; }
	aui_Image *GetBackgroundImage(void) { return m_backgroundImage; }

	void SetCityImage(aui_Image *image) { m_cityImage = image; }
	aui_Image *GetCityImage() { return m_cityImage; }

	void GetAttackerPos(sint32 column, sint32 row, sint32 *x, sint32 *y);
	void GetDefenderPos(sint32 column, sint32 row, sint32 *x, sint32 *y);
	void UpdateDisplay(void);
	void RemoveAttacker(sint32 index);
	void RemoveDefender(sint32 index);
	void RemoveActor(BattleViewActor *actor);
	void Process(void);
	void DrawProjectiles(void);
	void DrawExplosions(void);
	void DrawAttackers(void);
	void DrawDefenders(void);
	aui_DirectSurface		*GetBattleSurface(void) { return m_battleSurface; }
	void RemoveDeadActorsFromEvents(BattleEvent *deathEvent);

private:
	
	
	
	
	Battle		*m_battle;

	RECT						m_battleViewRect;
	aui_DirectSurface			*m_battleSurface;
	aui_Image					*m_backgroundImage;
	aui_Image                   *m_cityImage;
	sint32						m_numAttackers;
	BattleViewActor				*m_attackers[k_MAX_UNITS_PER_SIDE];
	sint32						m_numDefenders;
	BattleViewActor				*m_defenders[k_MAX_UNITS_PER_SIDE];
	PointerList<BattleEvent>	*m_eventQueue;
	PointerList<BattleEvent>	*m_activeEvents;
	PointerList<BattleEvent>::Walker	*m_walker;
	BattleEvent					*m_activeEvent;
	
	double						m_cityBonus;
	double						m_terrainBonus;
	double						m_fortBonus;
	double						m_fortifiedBonus;
};

int battleview_AttackerSort( const void *arg1, const void *arg2 );
int battleview_DefenderSort( const void *arg1, const void *arg2 );

#endif
