#ifndef __BATTLEEVENT_H__
#define __BATTLEEVENT_H__

#include "pointerlist.h"






enum BATTLE_EVENT_TYPE {
	BATTLE_EVENT_TYPE_NONE  = -1,

	BATTLE_EVENT_TYPE_PLACEMENT,
	BATTLE_EVENT_TYPE_ATTACK,
	BATTLE_EVENT_TYPE_EXPLODE,
	BATTLE_EVENT_TYPE_DEATH,

	BATTLE_EVENT_TYPE_MAX
};

class BattleViewActor;
class EffectActor;
class aui_Surface;

union BattleEventData {

	struct {
		BattleViewActor *actor;
	};

	struct {
		BattleViewActor *positionActor;
		sint32			positionColumn;
		sint32			positionRow;
		sint32			positionFacing;
		double			positionHP;
		BOOL			positionIsDefender;
	};

	struct {
		BattleViewActor *attackActor;
		sint32			attackSoundID;
		double			attackHP;
	};

	struct {
		BattleViewActor *explodeVictim;
		EffectActor		*explodeActor;
		sint32			explodeSoundID;
		double			explodeHP;
	};

	struct {
		BattleViewActor *deathVictim;
		sint32			deathSoundID;
		double			deathHP;
	};
};

class BattleEvent {
public:
	BattleEvent(BATTLE_EVENT_TYPE type);
	~BattleEvent();

	void Initialize(void);

	void ProcessPlacement(void);
	void ProcessAttack(void);
	void ProcessExplode(void);
	void ProcessDeath(void);
	void Process(void);

	void DrawExplosions(aui_Surface *surface);

	BOOL					IsFinished(void) const { return m_finished; }
	void					SetFinished(BOOL finished) { m_finished = TRUE; }

	void					SetType(BATTLE_EVENT_TYPE type) { m_type = type; }

	BATTLE_EVENT_TYPE		GetType(void) const { return m_type; }

	BOOL					HasActor(BattleViewActor *actor);

	void					AddPositionData(BattleViewActor *actor, sint32 column, sint32 row,
												sint32 facing, double hp, BOOL isDefender);
	void					AddAttackData(BattleViewActor *actor, sint32 soundID, double hp);
	void					AddExplosionData(BattleViewActor *actor, EffectActor *explodeActor, sint32 soundID, double hp);
	void					AddDeathData(BattleViewActor *actor, sint32 soundID, double hp);

	BattleViewActor			*GetActor(void);
	PointerList<BattleEventData>	*GetDataList(void) { return m_dataList; }

	void RemoveDeadActor(BattleViewActor *actor);
private:
	BATTLE_EVENT_TYPE						m_type;
	PointerList<BattleEventData>			*m_dataList;
	PointerList<BattleEventData>::Walker	*m_walker;
	BOOL									m_animating;
	BOOL									m_finished;
};

#endif
