#include "c3.h"

#include "c3ui.h"

#include "LineDance.h"
#include "cellunitlist.h"

#include "battleview.h"
#include "bset.h"

extern UnitDatabase		*g_theUnitDB;
extern BattleViewWindow	*g_battleViewWindow;
extern C3UI				*g_c3ui;

LineDance::LineDance(CellUnitList &attackers, CellUnitList &defenders)
{
	SortAttackers(attackers);
	SortDefenders(defenders);

	m_startAttackersRangeSlot = (attackers.Num() + 1) / 2;
	m_startDefendersRangeSlot = (defenders.Num() + 1) / 2;
	sint32 i;

	if (Player::IsThisPlayerARobot(attackers.GetOwner()) &&
		Player::IsThisPlayerARobot(defenders.GetOwner())) return;

	if (attackers.Num() > 1 || defenders.Num() > 1) {
		if (g_battleViewWindow)
			g_c3ui->RemoveWindow(g_battleViewWindow->Id());

		BattleViewWindow::Initialize();

		if (g_battleViewWindow) {
			g_battleViewWindow->SetupBattle(m_numAttackers, m_attackers,
											m_numDefenders, m_defenders);
		}

		g_c3ui->AddWindow(g_battleViewWindow);
		g_battleViewWindow->AddBordersToUI();

	} else {
		if (g_battleViewWindow) {
			g_c3ui->RemoveWindow(g_battleViewWindow->Id());
			delete g_battleViewWindow;
		}
		g_battleViewWindow = NULL;
	}

}

LineDance::~LineDance()
{
}

void LineDance::SortAttackers(CellUnitList &attackers)
{
	sint32 i, j, n;
	sint32 maxassaultvalue;
	sint32 maxassault;
	const UnitRecord *rec;
	BOOL used[k_MAX_LINE_SIZE];
	for(i = 0; i < k_MAX_LINE_SIZE; i++) {
		used[i] = FALSE;
	}

	n = attackers.Num();
	m_numAttackers = n;
	for(j = 0; j < n; j++) {

		maxassaultvalue = -1;
		maxassault = -1;
		for(i = 0; i < n; i++) {

			if(used[i])
				continue;
			rec = attackers[i].GetDBRec();

			if(rec->GetAttack() > maxassaultvalue) {
				maxassaultvalue = (sint32)rec->GetAttack();
				maxassault = i;
			} else if(rec->GetAttack() == maxassaultvalue) {

				if(rec->GetZBRangeAttack() < attackers[maxassault].GetDBRec()->GetZBRangeAttack()) {
					maxassault = i;
				}
			}
		}
		Assert(maxassault >= 0);
		used[maxassault] = TRUE;
		m_attackers[j] = attackers[maxassault];
	}
}

void LineDance::SortDefenders(CellUnitList &defenders)
{
	sint32 i, j, n;
	sint32 maxdefensevalue;
	sint32 maxdefense;
	const UnitRecord *rec;
	BOOL used[k_MAX_LINE_SIZE];
	for(i = 0; i < k_MAX_LINE_SIZE; i++) {
		used[i] = FALSE;
	}

	n = defenders.Num();
	m_numDefenders = n;
	for(j = 0; j < n; j++) {

		maxdefensevalue = -1;
		maxdefense = -1;
		for(i = 0; i < n; i++) {

			if(used[i])
				continue;
			rec = defenders[i].GetDBRec();

			if(rec->GetAttack() > maxdefensevalue) {
				maxdefensevalue = (sint32)rec->GetDefense();
				maxdefense = i;
			} else if(rec->GetDefense() == maxdefensevalue) {

				if(rec->GetZBRangeAttack() < defenders[maxdefense].GetDBRec()->GetZBRangeAttack()) {
					maxdefense = i;
				}
			}
		}
		Assert(maxdefense >= 0);
		used[maxdefense] = TRUE;
		m_defenders[j] = defenders[maxdefense];
	}
}

Unit &LineDance::GetAttacker(sint32 index)
{
	Assert(index >= 0);
	Assert(index < m_numAttackers);
	if(index < 0 || index >= m_numAttackers)
		return Unit(0);

	return m_attackers[index];
}

Unit &LineDance::GetDefender(sint32 index)
{
	Assert(index >= 0);
	Assert(index < m_numAttackers);
	if(index < 0 || index >= m_numDefenders)
		return Unit(0);

	return m_defenders[index];
}

void LineDance::RemoveDeadDefender(sint32 index)
{
	sint32 i;

	m_dead_defenders->Insert(m_defenders[index]);

	for(i = index; i < m_numDefenders - 1; i++) {
		m_defenders[i] = m_defenders[i + 1];
	}

	m_numDefenders--;
	for(i = m_numDefenders; i < k_MAX_LINE_SIZE; i++) {
		m_defenders[i].m_id = (0);
	}
}

void LineDance::RemoveDeadAttacker(sint32 index)
{
	sint32 i;

	m_dead_attackers->Insert(m_attackers[index]);

	for(i = index; i < m_numAttackers - 1; i++) {
		m_attackers[i] = m_attackers[i+1];
	}
	m_numAttackers--;
	for(i = m_numAttackers; i < k_MAX_LINE_SIZE; i++) {
		m_attackers[i].m_id = (0);
	}
}

void LineDance::DoRangedAttackerAttacks()
{
	sint32 i, j;
	const UnitRecord *rec;

	for(i = 0; i < k_MAX_LINE_SIZE; i++) {
		m_attackerUsedRangeFire[i] = FALSE;
	}

	for(i = m_startAttackersRangeSlot; i < m_numAttackers; i++) {
		m_attackerUsedRangeFire[i] = FALSE;
		rec = m_attackers[i].GetDBRec();
		if(rec->GetZBRangeAttack() > 0) {

			m_attackerUsedRangeFire[i] = TRUE;
			for(j = 0 ; j < k_RANGED_ROUNDS; j++) {
				m_attackers[i].FightOneLineDanceRangedAttack(m_defenders[0]);
				if(m_defenders[0].GetHP() < 1) {

					if (g_battleViewWindow) {
						g_battleViewWindow->RegisterAttackerRangedCombat(0, i);
					}
					RemoveDeadDefender(0);
					if(m_numDefenders <= 0)
						return;




					break;
				}
			}
		}
	}


	for(i = 1; i < m_startAttackersRangeSlot; i++) {
		if(i >= m_numAttackers)
			return;
		rec = m_attackers[i].GetDBRec();
		if(rec->GetZBRangeAttack() > 0 &&
		   rec->GetZBRangeAttack() > rec->GetAttack()) {

			for(j = 0; j < k_RANGED_ROUNDS; j++) {
				m_attackers[i].FightOneLineDanceRangedAttack(m_defenders[0]);
				if(m_defenders[0].GetHP() < 1) {

					if (g_battleViewWindow) {
						g_battleViewWindow->RegisterAttackerRangedCombat(0, i);
					}
					RemoveDeadDefender(0);
					if(m_numDefenders <= 0)
						return;

					break;
				}
			}
		}
	}
}

void LineDance::DoRangedDefenderAttacks()
{
	sint32 i, j;
	const UnitRecord *rec;

	if(m_numDefenders <= 0 || m_numAttackers <= 0)
		return;

	for(i = m_startDefendersRangeSlot; i < m_numDefenders; i++) {
		rec = m_defenders[i].GetDBRec();
		if(rec->GetZBRangeAttack() > 0) {

			for(j = 0 ; j < k_RANGED_ROUNDS; j++) {
				m_defenders[i].FightOneLineDanceRangedAttack(m_attackers[0]);
				if(m_attackers[0].GetHP() < 1) {

					if (g_battleViewWindow) {
						g_battleViewWindow->RegisterDefenderRangedCombat(0, i);
					}
					RemoveDeadAttacker(0);

					if(m_numAttackers <= 0)
						return;




					break;
				}
			}
		}
	}


	for(i = 1; i < m_startDefendersRangeSlot; i++) {
		if(i >= m_numDefenders)
			return;

		rec = m_defenders[i].GetDBRec();
		if(rec->GetZBRangeAttack() > 0 &&
		   rec->GetZBRangeAttack() > rec->GetDefense()) {

			for(j = 0; j < k_RANGED_ROUNDS; j++) {
				m_defenders[i].FightOneLineDanceRangedAttack(m_attackers[0]);
				if(m_attackers[0].GetHP() < 1) {

					if (g_battleViewWindow) {
						g_battleViewWindow->RegisterDefenderRangedCombat(0, i);
					}

					RemoveDeadAttacker(0);
					if(m_numAttackers <= 0)
						return;

					break;
				}
			}
		}
	}
}

void LineDance::DoAssaults()
{
	sint32 i, j;
	const UnitRecord *rec;

	if(m_numAttackers <= 0 || m_numDefenders <= 0)
		return;

	sint32 numAttackersThatUsedRangeFireAndAreGenerallyNotInvolvedInThisAssaultPhase = 0;
	for(i = 0; i < k_MAX_LINE_SIZE; i++) {
		if(i < m_startAttackersRangeSlot && m_attackerUsedRangeFire[i]) {
			numAttackersThatUsedRangeFireAndAreGenerallyNotInvolvedInThisAssaultPhase++;
		}

		m_attackerIsDead[i] = FALSE;
		m_defenderIsDead[i] = FALSE;
	}

	sint32 numDeadAttackerAssaulters = 0;
	sint32 numDeadDefenderAssaulters = 0;





	while((numDeadAttackerAssaulters + numAttackersThatUsedRangeFireAndAreGenerallyNotInvolvedInThisAssaultPhase) < m_startAttackersRangeSlot &&
		numDeadDefenderAssaulters < m_startDefendersRangeSlot) {

		for(i = 0; i < m_startAttackersRangeSlot; i++) {
			if(i >= m_numAttackers)
				break;
			if(m_attackerIsDead[i])
				continue;

			rec = m_attackers[i].GetDBRec();
			if(!m_attackerUsedRangeFire[i] && rec->GetAttack() > 0) {
				for(j = 0; j < m_startDefendersRangeSlot; j++) {
					if(j >= m_numDefenders)
						break;
					if(m_defenderIsDead[j])
						continue;
					m_attackers[i].FightOneLineDanceAssault(m_defenders[j]);






					if(m_attackers[i].GetHP() < 1) {

						if (g_battleViewWindow) {
							g_battleViewWindow->RegisterDefenderAssaultCombat(i, j);
						}

						m_attackerIsDead[i] = TRUE;
						numDeadAttackerAssaulters++;

						break;
					}

					if(m_defenders[j].GetHP() < 1) {

						if (g_battleViewWindow) {
							g_battleViewWindow->RegisterAttackerAssaultCombat(j, i);
						}

						m_defenderIsDead[j] = TRUE;
						numDeadDefenderAssaulters++;

					}

					if(m_numAttackers <= 0 || m_numDefenders <= 0) {
						return;
					}
				}
			}
		}
	}

	for(i = m_startAttackersRangeSlot - 1; i >= 0; i--) {
		if(m_attackerIsDead[i]) {
			RemoveDeadAttacker(i);
			m_attackerIsDead[i] = FALSE;
		}
	}

	for(j = m_startDefendersRangeSlot - 1; j >= 0; j--) {
		if(m_defenderIsDead[j]) {
			RemoveDeadDefender(j);
			m_defenderIsDead[j] = FALSE;
		}
	}

	sint32 anum = 0, dnum = 0;
	for(i = 0; i < k_MAX_LINE_SIZE; i++) {
		m_attackerIsDead[i] = FALSE;
		if(m_attackers[i] != Unit(0))
			anum++;
		if(m_defenders[i] != Unit(0))
			dnum++;
	}
	m_startAttackersRangeSlot = (anum + 1) / 2;
	m_startDefendersRangeSlot = (dnum + 1) / 2;
}

BOOL LineDance::Resolve(CellUnitList &dead_attackers,
						CellUnitList &dead_defenders)
{
	m_dead_attackers = &dead_attackers;
	m_dead_defenders = &dead_defenders;

	sint32 i;
	while(m_numAttackers > 0 && m_numDefenders > 0) {
		double totalAttack = 0;
		double totalRangedAttack = 0;
		double totalDefenseRangedAttack = 0;
		for(i = 0; i < m_numAttackers; i++) {
			totalAttack += m_attackers[i].GetDBRec()->GetAttack();
		}
		if(totalAttack < 0.0001) {
			for(i = 1; i < m_numAttackers; i++) {
				totalRangedAttack += m_attackers[i].GetDBRec()->GetZBRangeAttack();
			}
			if(totalRangedAttack < 0.0001) {
				for(i = 1; i < m_numDefenders; i++) {
					totalDefenseRangedAttack += m_defenders[i].GetDBRec()->GetZBRangeAttack();
				}
				if(totalDefenseRangedAttack < 0.0001) {

					return FALSE;
				}
			}
		}

		DoRangedAttackerAttacks();
		DoRangedDefenderAttacks();


		if (g_battleViewWindow) {
			g_battleViewWindow->RegisterRoundEnd();
		}

		DoAssaults();

		if (g_battleViewWindow) {
			g_battleViewWindow->RegisterRoundEnd();
		}
	}
	return TRUE;
}
