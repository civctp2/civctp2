#ifndef __BATTLE_H__
#define __BATTLE_H__

#include "dbtypes.h"
#include "battleview.h"
#include "colorset.h"

class Battle {
public:
	Battle();
	~Battle();

	void Initialize(const Army &attackers, CellUnitList &defenders);







	void MakeAttackers(sint32 numAttackers, const Army &attackers);
	void MakeDefenders(sint32 numDefenders, CellUnitList &defenders);

	BattleViewActor			*ActorFromUnit(BOOL isDefender, Unit theUnit);

	void					GetAttackerPos(sint32 column, sint32 row, sint32 *x, sint32 *y);
	void					GetDefenderPos(sint32 column, sint32 row, sint32 *x, sint32 *y);

	void					PositionUnit(BattleEvent *event, BOOL isDefender, Unit theUnit, sint32 column, sint32 row, bool initial);
	void					AddUnitAttack(BattleEvent *event, BOOL isDefender, Unit theUnit);
	void					AddUnitExplosion(BattleEvent *event, BOOL isDefender, Unit theUnit);
	void					AddUnitDeath(BattleEvent *event, BOOL isDefender, Unit theUnit);

	void					AddEvent(BattleEvent *event);
	void					ShowEvent(BattleEvent *event);

	double					GetCityBonus(void) { return m_cityBonus; }
	sint32					GetCityImage(void) { return m_cityImage; }
	MBCHAR					*GetCityName(void) { return m_cityName; }
	double					GetTerrainBonus(void) { return m_terrainBonus; }
	sint32					GetTerrainType(void) { return m_terrainType; }
	sint32                  GetAttackersTerrainType() { return m_attackerTerrainType; }

	double					GetFortBonus(void) { return m_fortBonus; }
	double					GetFortifiedBonus(void) { return m_fortifiedBonus; }

	COLOR					GetAttackersColor(void) { return m_attackersColor; }
	COLOR					GetDefendersColor(void) { return m_defendersColor; }
	void					GetAttackersName(MBCHAR *s) { strcpy(s, m_attackersName); }
	void					GetDefendersName(MBCHAR *s) { strcpy(s, m_defendersName); }

	sint32					GetNumAttackers(void) { return m_numAttackers; }
	BattleViewActor			*GetAttacker(sint32 index) { return m_attackers[index]; }
	sint32					GetNumDefenders(void) { return m_numDefenders; }
	BattleViewActor			*GetDefender(sint32 index) { return m_defenders[index]; }


	void RemoveAttacker(BattleViewActor *actor);
	void RemoveDefender(BattleViewActor *actor);





	PointerList<BattleEvent> *GrabEventQueue();

private:
	sint32						m_numAttackers;
	BattleViewActor				*m_attackers[k_MAX_UNITS_PER_SIDE];
	COLOR						m_attackersColor;

	sint32						m_numDefenders;
	BattleViewActor				*m_defenders[k_MAX_UNITS_PER_SIDE];
	COLOR						m_defendersColor;

	PointerList<BattleEvent>	*m_eventQueue;

	double						m_cityBonus;
	sint32						m_cityImage;
	MBCHAR						m_cityName[k_MAX_NAME_LEN];

	double						m_terrainBonus;
	sint32						m_terrainType;
	sint32                      m_attackerTerrainType;
	double						m_fortBonus;
	double						m_fortifiedBonus;

	MBCHAR						m_attackersName[k_MAX_NAME_LEN];
	MBCHAR						m_defendersName[k_MAX_NAME_LEN];

};

#endif
