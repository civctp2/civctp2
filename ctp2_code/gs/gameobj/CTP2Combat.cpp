//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : Combat handling
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
//----------------------------------------------------------------------------
//
// Modifications from the original Activision code:
//
// - Veteran effect added.
//
//----------------------------------------------------------------------------

#include "c3.h"

#ifndef TEST_APP

#include "cellunitlist.h"
#include "UnitRecord.h"
#include "UnitData.h"
#include "GameEventManager.h"
#include "AICause.h"
#include "RandGen.h"

#include "battleevent.h"
#include "battleview.h"
#include "battle.h"
#include "battleviewwindow.h"
#include "director.h"

#include "SelItem.h"
#include "player.h"
#include "profileDB.h"


#include "World.h"
#include "Cell.h"

#else

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#endif

#include "debugmemory.h"

#include "CTP2Combat.h"

CTP2Combat *g_theCurrentBattle = NULL;


extern BattleViewWindow *g_battleViewWindow;

#define k_COMBAT_DEBUG_VERBOSE 2
#define k_COMBAT_DEBUG_NORMAL 1
#define k_COMBAT_DEBUG_CRITICAL 0

static bool s_somethingDied = false;

#ifdef TEST_APP

#define Assert(x) { if(!(x)) { combat_print(k_COMBAT_DEBUG_CRITICAL, "%s:%d: Assert '%s'\n", __FILE__, __LINE__, #x); }}

FILE *s_output;

sint32 g_debugLevel = 1;

#define COMBATRAND(x) (rand() % (x))

static void combat_print(sint32 level, const char *fmt, ...)
{
	if(level > g_debugLevel)
		return;

	char text[1024];
	va_list vl;
	va_start(vl, fmt);
	vsprintf(text, fmt, vl);
	va_end(vl);

	fprintf(s_output, "%s", text);
}

#define k_MAX_TRACKED_UNITS 255

uint32 s_unitId = 0;

class DamageTracker {
public:
	DamageTracker() { m_DamageInflicted = 0; }
	CombatUnit m_unit;
	double m_DamageInflicted;
};

DamageTracker s_trackedUnits[k_MAX_TRACKED_UNITS];

#else 

#define COMBATRAND(x) (g_rand->Next(x))

static void combat_print(sint32 level, const char *fmt, ...)
{
	
		
		char buf[1024];
		va_list vl;
		va_start(vl, fmt);
		vsprintf(buf, fmt, vl);
		va_end(vl);

		DPRINTF(k_DBG_GAMESTATE, ("%s", buf));
		
}

#endif

#ifdef TEST_APP
CombatUnit::CombatUnit(double offense, double defense,
		   double strength, double armor,
		   double ranged, double hp,
		   UNIT_TYPE type,
		   bool const	isVeteran) :
	m_offense(offense),
	m_defense(defense),
	m_strength(strength),
	m_armor(armor),
	m_ranged(ranged),
	m_hp(hp),
	m_isVeteran(isVeteran),
	m_type(type)
{
	m_valid = true;

	m_id = s_unitId++;
	s_trackedUnits[m_id].m_unit = *this;
}

#else 

CombatUnit::CombatUnit(double offense, double defense,
					   double strength, double armor,
					   double ranged, double hp,
					   Unit &u) :
	m_offense(offense),
	m_defense(defense),
	m_strength(strength),
	m_armor(armor),
	m_ranged(ranged),
	m_hp(hp),
	// Using a silly ?:-construction to prevent a compiler warning. 
	// TODO: make Unit::IsVeteran return bool.
	m_isVeteran(u.IsVeteran() ? true : false),
	m_unit(u)
{
	m_valid = true;

	const UnitRecord *rec = m_unit.GetDBRec();
	if(rec->GetMovementTypeAir() &&
	   ((rec->GetMovementType() & ~(k_Unit_MovementType_Air_Bit)) == 0)) {
		
		m_type = UNIT_TYPE_AIR;
	} else if((rec->GetMovementTypeSea() || rec->GetMovementTypeShallowWater()) &&
			  ((rec->GetMovementType() & ~(k_Unit_MovementType_Sea_Bit |
										   k_Unit_MovementType_ShallowWater_Bit)) == 0)) {
		
		m_type = UNIT_TYPE_NAVAL;
	} else if(rec->GetIsFlanker()) {
		m_type = UNIT_TYPE_FLANKER;
	} else if(rec->GetMovementTypeLand() || rec->GetMovementTypeMountain()) {
		m_type = UNIT_TYPE_GROUND;
	}

	if((m_offense < 0.999) && (m_defense < 0.999) && (m_ranged < 0.999)) {
		m_type = UNIT_TYPE_NONCOMBAT;
	}
}

void CombatUnit::DeductHP(double amt)
{
	Assert(m_unit.IsValid());
	Assert(amt > 0);
	m_hp -= amt; 

	m_unit->SetHP(m_hp);

	if(m_hp < 0.99999) {
		m_hp = 0;
		g_theCurrentBattle->AddDeadUnit(m_unit);
	}
}

#endif


CombatField::CombatField(sint32 width, sint32 height, bool isOffense)
{
	m_field = new CombatUnit *[width];
	sint32 i;
	for(i = 0; i < width; i++) {
		m_field[i] = new CombatUnit[height];
	}

	m_width = width;
	m_height = height;
	m_isOffense = isOffense;
}




CombatField::~CombatField()
{
	for (int i = 0; i < m_width; i++)
	{
		delete m_field[i];
	}

	delete m_field;
}


CombatUnit &CombatField::GetUnit(sint32 x, sint32 y)
{
	Assert(x < m_width);
	Assert(y < m_height);

	return m_field[x][y];
}

void CombatField::SetUnit(sint32 x, sint32 y, CombatUnit *u)
{
	Assert(x < m_width);
	Assert(y < m_height);
	
	m_field[x][y] = *u;
}

void CombatField::StartRound()
{
	sint32 x, y;

	for(x = 0; x < m_width; x++) {
		for(y = 0; y < m_height; y++) {
			if(m_field[x][y].IsActive()) {
				
				m_field[x][y].m_alreadyExploded = false;
				m_field[x][y].m_alreadyAttacked = false;
			}
		}
	}
}

sint32 CombatField::NumAlive()
{
	sint32 x, y;
	sint32 count = 0;

	for(x = 0; x < m_width; x++) {
		for(y = 0; y < m_height; y++) {
			if(m_field[x][y].IsActive()) {
				count++;
			}
		}
	}
	return count;
}



bool CombatField::FindTargetForRangedAttackFrom(sint32 x, sint32 y,
												 sint32 *dx, sint32 *dy)
{	
	sint32 yc, xc;

	*dx = *dy = -1;

	sint32 ydelta;
	
	for(xc = k_FRONT_COL; xc < m_width; xc++) {		
		
		for(ydelta = 0; ydelta < m_height; ydelta++) {
			if(((y + ydelta) < m_height) && (m_field[xc][y + ydelta].IsActive())) {
				*dx = xc;
				*dy = y + ydelta;
				return true;
			}

			if((ydelta != 0) && ((y - ydelta) >= 0) && (m_field[xc][y - ydelta].IsActive())) {
				*dx = xc;
				*dy = y - ydelta;
				return true;
			}
		}
	}

	
	
	
	
	for(xc = 0; xc < m_width; xc++) {
		for(yc = 0; yc < m_height; yc++) {
			if(m_field[xc][yc].IsActive()) {
				*dx = xc;
				*dy = yc;
				return true;
			}
		}
	}

	
	return false;
}

bool CombatField::FindTargetForAttackFrom(sint32 x, sint32 y,
										  sint32 *dx, sint32 *dy,
										  bool isFlanker)
{
	
	Assert(x == 0);

	*dx = *dy = -1;

	
	if(m_field[x][y].IsActive()) {
		*dx = x;
		*dy = y;
		return true;
	}

	if(isFlanker) {
		
		
		sint32 ydelta;
		for(ydelta = 1; ydelta < m_height; ydelta++) {
			
			if(y + ydelta < m_height) {
				if(m_field[x][y + ydelta].IsActive()) {
					*dx = x;
					*dy = y + ydelta;
					return true;
				}
			}
			
			
			if(y - ydelta >= 0) {
				if(m_field[x][y - ydelta].IsActive()) {
					*dx = x;
					*dy = y - ydelta;
					return true;
				}
			}
		}
	}

	return false;
}

void CombatField::Move()
{
	
	sint32 x, y, y2;
	sint32 frontCount = CountColumn(k_FRONT_COL);
	for(x = 1; x < m_width; x++) {
		for(y = 0; y < m_height; y++) {
			if(!m_field[x][y].IsActive())
				
				continue;

			if(m_field[0][y].IsActive())
				
				continue;

			
			for(y2 = 0; y2 < m_height; y2++) {
				if(m_field[x][y2].IsActive())
					
					continue;

				if(!m_field[0][y2].IsActive())
					
					continue;

				
				m_field[x][y2] = m_field[x][y];
				m_field[x][y].Invalidate();
				break;
			}
		}
	}

	
	
	CombatUnit *newRow = new CombatUnit[m_width];
	sint32 rowPos = 0;

	for(y = 0; y < m_height; y++) {
		rowPos = 0;
		
		for(x = 0; x < m_width; x++) {
			if(m_field[x][y].IsActive()) {
				newRow[rowPos++] = m_field[x][y];
			}
		}

		
		for(x = 0; x < rowPos; x++) {
			m_field[x][y] = newRow[x];
		}

		
		for(; x < m_width; x++) {
			m_field[x][y].Invalidate();
		}
	}

	delete [] newRow;
}

void CombatField::Sort()
{
	sint32 x, y;
	sint32 dbgcount = 0;
	for(x = 0; x < m_width; x++) {
		for(y = 0; y < m_height; y++) {
			if(!m_field[x][y].IsActive())
				continue;
			dbgcount++;

			CombatUnit *u = &m_field[x][y];
			if(u->GetCombatType() == UNIT_TYPE_AIR) {
				u->SetPreferredCol(k_AIR_COL);
				u->SetPriority(sint32(u->GetOffense() + u->GetRangedAttack()));				
			} else if((u->GetOffense() <= 0) && (u->GetDefense() <= 10) && (u->GetRangedAttack() <= 0)) {
				
				
				u->SetPreferredCol(k_NONCOMBAT_COL);
				u->SetPriority(0);
			} else {
				sint32 primaryStat;
				if(m_isOffense) {
					primaryStat = (sint32)u->GetOffense();
				} else {
					primaryStat = (sint32)u->GetDefense();
				}

				if(m_field[x][y].GetRangedAttack() > primaryStat) {
					u->SetPreferredCol(k_RANGED_COL);
					u->SetPriority(sint32((u->GetRangedAttack() * 2) + primaryStat));
				} else {
					u->SetPreferredCol(k_FRONT_COL);
					u->SetPriority(primaryStat);
				}
			}

			
			
			if(u->GetCombatType() != UNIT_TYPE_FLANKER) {
				u->SetPriority(u->GetPriority() + k_NON_FLANKER_PRIORITY);
			}
		}
	}
	Assert(dbgcount);

	CombatField sorted(m_width, m_height, m_isOffense);
	for(x = 0; x < m_width; x++) {
		for(y = 0; y < m_height; y++) {
			if(m_field[x][y].IsActive()) {
				dbgcount++;
				sorted.InsertInPreferredCol(&m_field[x][y]);
			}
		}
	}
	Assert(dbgcount);
	dbgcount = 0;

	
	
	
	
	
	
	
	
	
	
	for(x = 0; x < m_width; x++) {
		for(y = 0; y < m_height; y++) {
			m_field[x][y].Invalidate();
		}
	}

#ifdef TEST_APP
	sorted.Dump(false);
#endif

	for(x = 0; x < m_width; x++) {
		for(y = 0; y < m_height; y++) {
			sint32 my = m_height/2;
			if(y & 1) {
				my += ((y + 1)/2);
			} else {
				my -= ((y + 1) / 2);
			}
			m_field[x][my] = sorted.m_field[x][y];
			if(m_field[x][my].IsActive())
				dbgcount++;
		}
	}
	Assert(dbgcount);
}

void CombatField::InsertInPreferredCol(CombatUnit *u)
{
	sint32 col = u->GetPreferredCol();
	Assert(col >= 0);
	Assert(col < m_width);

	sint32 y,y2;
	bool full = true;
	col--;
	do {
		col++;
		if(col >= m_width)
			col = 0;

		for(y = 0; y < m_height; y++) {
			if(!m_field[col][y].IsActive()) {
				full = false;
				break;
			}
		}
	} while(full);

	for(y = 0; y < m_height; y++) {
		if((u->GetPriority() > m_field[col][y].GetPriority()) ||
		   (!m_field[col][y].IsActive())) {
			for(y2 = m_height - 1; y2 > y; y2--) {
				m_field[col][y2] = m_field[col][y2 - 1];
			}
			m_field[col][y] = *u;
			break;
		}
	}
}

sint32 CombatField::CountColumn(sint32 col)
{
	sint32 y, count=0;
	for(y = 0; y < m_height; y++) {
		if(m_field[col][y].IsActive())
			count++;
	}
	return count;
}

void CombatField::MoveRowForward(sint32 row)
{
	sint32 x;
	
	for(x = k_FRONT_COL + 1; x < m_width; x++) {
		if(m_field[x][row].IsActive() && 
		   m_field[x][row].GetCombatType() != UNIT_TYPE_NONCOMBAT) {
			
			if(m_field[x][row].IsActive()) {
				combat_print(k_COMBAT_DEBUG_VERBOSE, "MOVEH: %s@%d,%d -> %d,%d\n", 
							 m_isOffense ? "Attacker" : "Defender",
							 x, row, k_FRONT_COL, row);
			}
			m_field[k_FRONT_COL][row] = m_field[x][row];
			m_field[x][row].Invalidate();
			break;
		}
	}
}

void CombatField::MoveUnitsToFront(sint32 num)
{
	Assert((CountColumn(k_FRONT_COL) + num) <= m_height);

	
	sint32 ydelta, ycenter;
	ycenter = m_height / 2;
	for(ydelta = 0; ydelta <= (m_height / 2); ydelta++) {
		if(num <= 0)
			break;

		if(!m_field[k_FRONT_COL][ycenter + ydelta].IsActive()) {
			
			MoveRowForward(ycenter + ydelta);
			if(m_field[k_FRONT_COL][ycenter + ydelta].IsActive())
				num--;
		}

		if(ydelta != 0 && num > 0) {
            if(!m_field[k_FRONT_COL][ycenter - ydelta].IsActive()) {
				
				MoveRowForward(ycenter - ydelta);
				if(m_field[k_FRONT_COL][ycenter - ydelta].IsActive())
					num--;
			}
		}
	}
}

void CombatField::FindOpponents(CombatField &opposite)
{
	
	sint32 y;
	for(y = 0; y < m_height; y++) {
		if(!m_field[k_FRONT_COL][y].IsActive())
			
			continue;

		if(opposite.m_field[k_FRONT_COL][y].IsActive())
			
			continue;

		sint32 yo;
		
		for(yo = 0; yo < m_height; yo++) {
			if(m_field[k_FRONT_COL][y].IsActive())
				
				continue;
			if(opposite.m_field[k_FRONT_COL][y].IsActive())
				
				continue;

			
			m_field[k_FRONT_COL][yo] = m_field[k_FRONT_COL][y];
			m_field[k_FRONT_COL][y].Invalidate();
			break;
		}
	}
}

void CombatField::FillHoles()
{
	
	sint32 ydelta = 0;
	sint32 ycenter = m_height / 2;
	sint32 x;
	sint32 yc;
	for(x = 0; x < m_width; x++) {
		ydelta = 0; 
		if(!m_field[x][ycenter].IsActive()) {
			
			double plusOffense = -1;
			sint32 plusDelta = 0; 
			for(yc = ycenter + 1; yc < m_height; yc++) {
				plusDelta++;
				if(m_field[x][yc].IsActive()) {
					plusOffense = m_field[x][yc].GetOffense();
					break;
				}
			}
			double minusOffense = -1;
			sint32 minusDelta = 0; 
			for(yc = ycenter - 1; yc >= 0; yc--) {
				minusDelta++;
				if(m_field[x][yc].IsActive()) {
					minusOffense = m_field[x][yc].GetOffense();
					break;
				}
			}
			if(plusOffense >= 0 || minusOffense >= 0) {
				
				if(plusOffense > minusOffense) {
					
					for(yc = ycenter+plusDelta; yc < m_height; yc++) {
						if(m_field[x][yc].IsActive()) {
							combat_print(k_COMBAT_DEBUG_VERBOSE, "MOVEV: %s@%d,%d -> %d,%d\n", 
										 m_isOffense ? "Attacker" : "Defender",
										 x, yc, x, yc - 1);
						}
						m_field[x][yc - plusDelta] = m_field[x][yc];
						m_field[x][yc].Invalidate();
					}
				} else {
					
					for(yc = ycenter - minusDelta; yc >= 0; yc--) {
						if(m_field[x][yc].IsActive()) {
							combat_print(k_COMBAT_DEBUG_VERBOSE, "MOVEV: %s@%d,%d -> %d,%d\n", 
										 m_isOffense ? "Attacker" : "Defender",
										 x, yc, x, yc + 1);
						}
						m_field[x][yc + minusDelta] = m_field[x][yc];
						m_field[x][yc].Invalidate();
					}
				}

			}
					
		}
		for(ydelta = 1; ydelta <= (m_height / 2); ydelta++) {
			if(!m_field[x][ycenter + ydelta].IsActive()) {
				for(yc = ycenter + ydelta + 1; yc < m_height; yc++) {
					if(m_field[x][yc].IsActive()) {
						combat_print(k_COMBAT_DEBUG_VERBOSE, "MOVEV: %s@%d,%d -> %d,%d\n", 
									 m_isOffense ? "Attacker" : "Defender",
									 x, yc, x, yc - 1);
					}
					m_field[x][yc - 1] = m_field[x][yc];
					m_field[x][yc].Invalidate();
				}
			}
			
			if(!m_field[x][ycenter - ydelta].IsActive()) {
				for(yc = ycenter - ydelta - 1; yc >= 0; yc--) {
					if(m_field[x][yc].IsActive()) {
						combat_print(k_COMBAT_DEBUG_VERBOSE, "MOVEV: %s@%d,%d -> %d,%d\n", 
									 m_isOffense ? "Attacker" : "Defender",
									 x, yc, x, yc + 1);
					}
					m_field[x][yc + 1] = m_field[x][yc];
					m_field[x][yc].Invalidate();
				}
			}
		}
	}
}

#ifndef TEST_APP
void CombatField::ReportUnits(Battle *battle, BattleEvent *event, bool initial)
{
	sint32 x, y;
	for(x = 0; x < m_width; x++) {
		for(y = 0; y < m_height; y++) {
			CombatUnit cu = GetUnit(x, y);
			if(cu.IsActive()) {
				if(battle) {
					DPRINTF(k_DBG_GAMESTATE, ("Positioning unit %lx (%lf HP)\n", cu.m_unit, cu.GetHP()));
					battle->PositionUnit(event, !m_isOffense, cu.m_unit, x, y, initial);
				}
			}
		}
	}
}
#endif


#ifdef TEST_APP
void CombatField::Dump(bool flip)
{
	
	sint32 x, y;

	sint32 xs = flip ? m_width - 1 : 0;
	sint32 xe = flip ? -1 : m_width;
	sint32 xi = flip ? -1 : 1;

	for(x = xs; x != xe; x += xi) {
		combat_print(k_COMBAT_DEBUG_VERBOSE, "%3d: ", x);
		for(y = 0; y < m_height; y++) {
			if(m_field[x][y].IsValid()) {
				combat_print(k_COMBAT_DEBUG_VERBOSE, "%3d ", (sint32)m_field[x][y].GetHP());
			} else {
				combat_print(k_COMBAT_DEBUG_VERBOSE, "--- ");
			}
		}
		combat_print(k_COMBAT_DEBUG_VERBOSE, "\n");
	}
}

void error_exit(char *s, ...)
{
	char text[1024];
	va_list vl;
	va_start(vl, s);
	vsprintf(text, s, vl);
	va_end(vl);

	combat_print(k_COMBAT_DEBUG_CRITICAL, "%s", text);

	exit(0);
}
#endif

#ifdef TEST_APP
CTP2Combat::CTP2Combat(sint32 w, sint32 h, FILE *input) :
	m_width(w),
	m_height(h),
	m_attackers(w, h, true),
	m_defenders(w, h, false)
{
	
	sint32 state = 0;
	sint32 lineNum = 0;
	m_retreated = false;
	m_retreating = false;
	m_noAttacksPossible = false;

	while(!feof(input)) {

		
		char line[1024];
		if(!fgets(line, 1024, input))
			break;

		lineNum++;

		if(line[0] == '#') {
			
			continue;
		} else if(strnicmp(line, "[attackers]", 11) == 0) {
			
			state = 1;
		} else if(strnicmp(line, "[defenders]", 11) == 0) {
			
			state = 2;
		} else {
			switch(state) {
				case 0:
					error_exit("Expected [attackers] or [defenders] first\n");
					break;

				case 1: 
				case 2: 
				{
					sint32 x, y;
					double offense, defense, strength, armor, ranged, hp;
					char typeStr[256];
					
					if(sscanf(line, "%d,%d %lf %lf %lf %lf %lf %lf %s\n",
							  &x, &y, &offense, &defense, &strength, &armor, &ranged, &hp, typeStr) != 9) {
						error_exit("Couldn't parse '%s' at line %d", line, lineNum);
					}

					UNIT_TYPE type;
					if(stricmp(typeStr, "ground") == 0) {
						type = UNIT_TYPE_GROUND;
					} else if(stricmp(typeStr, "naval") == 0) {
						type = UNIT_TYPE_NAVAL;
					} else if(stricmp(typeStr, "air") == 0) {
						type = UNIT_TYPE_AIR;
					} else if(stricmp(typeStr, "flanker") == 0) {
						type = UNIT_TYPE_FLANKER;
					} else if(stricmp(typeStr, "noncombat") == 0) {
						type = UNIT_TYPE_NONCOMBAT;
					} else {
						Assert(0);
						type = UNIT_TYPE_GROUND;
					}

					CombatUnit newUnit(offense, defense, strength, armor, ranged, hp, type);
					if(state == 1) {
						m_attackers.SetUnit(x, y, &newUnit);
					} else {
						m_defenders.SetUnit(x, y, &newUnit);
					}

					break;
				}
				default:
					Assert(0);
			}
		}
	}

	m_defenders.Sort();
	m_attackers.Sort();

	Balance();

	m_round = 0;
}

#else 

void CombatField::FillFrom(CellUnitList &units)
{
	sint32 i;
	sint32 x = 0, y = 0;
	for(i = 0; i < units.Num(); i++) {
		Unit u = units[i];
		const UnitRecord *rec = u.GetDBRec();

		CombatUnit newUnit(rec->GetAttack(), rec->GetDefense(), rec->GetFirepower(),
						   rec->GetArmor(), rec->GetZBRangeAttack(), u->GetHP(),
						   u);
		SetUnit(x, y, &newUnit);

		x++;
		if(x >= m_width) {
			x = 0;
			y++;
		}			
	}
}

CTP2Combat::CTP2Combat(sint32 w, sint32 h,
					   CellUnitList &attackers,
					   CellUnitList &defenders) :
	m_attackers(w, h, true),
	m_defenders(w, h, false),
	m_width(w),
	m_height(h),
	m_retreated(false),
	m_retreating(false),
	m_noAttacksPossible(false),
	m_battle(NULL)
{
	m_attackers.FillFrom(attackers);
	m_defenders.FillFrom(defenders);

	m_defenders.Sort();
	m_attackers.Sort();

	Balance();

	m_round = 0;

	m_attacker = attackers.GetOwner();
	m_defender = defenders.GetOwner();
	m_army_id = attackers[0].GetArmy().m_id;
	defenders.GetPos(m_defenderPos);

	combat_print(k_COMBAT_DEBUG_VERBOSE, "Attacker: %lx, Defender: (%d,%d)\n", attackers[0].GetArmy().m_id,
				 defenders[0].RetPos().x, defenders[0].RetPos().y);
	sint32 i;
	for(i = 0; i < attackers.Num(); i++) {
		combat_print(k_COMBAT_DEBUG_VERBOSE, "attacker[%d]: %lx @ %lfHP\n", i, attackers[i].m_id, attackers[i].GetHP());
	}
	for(i = 0; i < defenders.Num(); i++) {
		combat_print(k_COMBAT_DEBUG_VERBOSE, "defender[%d]: %lx @ %lfHP, army=%lx\n", i, defenders[i].m_id, defenders[i].GetHP(), defenders[i].GetArmy().m_id);
	}

	
	Assert(g_player[m_attacker]);
	Assert(g_player[m_defender]);
	if (g_player[m_attacker] &&
		g_player[m_defender] &&
		g_player[m_attacker]->GetPlayerType() == PLAYER_TYPE_ROBOT &&
		g_player[m_defender]->GetPlayerType() == PLAYER_TYPE_ROBOT)
		return;

	m_roundsSinceUpdate = 0x7ffffffe;  

	
	m_attackers.StartRound();
	m_defenders.StartRound();

	
	if (g_theProfileDB->IsShowZoomedCombat() == FALSE)
		return;

	
	
	if((m_attacker == g_selected_item->GetVisiblePlayer()) ||
	   (m_defender == g_selected_item->GetVisiblePlayer())) {
		
		m_battle = new Battle();

		
		m_battle->Initialize(attackers[0].GetArmy(), defenders);

		
		BattleEvent *placementEvent = new BattleEvent(BATTLE_EVENT_TYPE_PLACEMENT);
		Assert(placementEvent);
		m_attackers.ReportUnits(m_battle, placementEvent, true);
		m_defenders.ReportUnits(m_battle, placementEvent, true);
		m_battle->AddEvent(placementEvent);

		
		
		
		g_director->AddBattle(m_battle);
	}

	Assert(!IsDone());
}


CTP2Combat::~CTP2Combat()
{
	
	if(m_battle) {
		
		if(g_battleViewWindow)
			g_battleViewWindow->EndBattle();

		
		delete m_battle;
	}
}
#endif


#ifdef TEST_APP
void CTP2Combat::DumpState()
{
	
	combat_print(k_COMBAT_DEBUG_VERBOSE, "%d remaining defenders\n", m_defenders.NumAlive());
	m_defenders.Dump(true);
	combat_print(k_COMBAT_DEBUG_VERBOSE, "--front line--\n");
	m_attackers.Dump(false);
	combat_print(k_COMBAT_DEBUG_VERBOSE, "%d remaining attackers\n", m_attackers.NumAlive());
}

#endif

void CTP2Combat::Balance()
{
	
	
	

	m_attackers.FillHoles();
	m_defenders.FillHoles();

	sint32 attackFront = m_attackers.CountColumn(k_FRONT_COL);
	sint32 defendFront = m_defenders.CountColumn(k_FRONT_COL);

	if(attackFront < defendFront) {
		m_defenders.FindOpponents(m_attackers);
		m_attackers.MoveUnitsToFront(defendFront - attackFront);
	} else if(defendFront < attackFront) {
		m_attackers.FindOpponents(m_defenders);
		m_defenders.MoveUnitsToFront(attackFront - defendFront);
	}

	attackFront = m_attackers.CountColumn(k_FRONT_COL);
	defendFront = m_defenders.CountColumn(k_FRONT_COL);

	
	if(attackFront < defendFront) {
		m_defenders.FindOpponents(m_attackers);
	} else {
		m_attackers.FindOpponents(m_defenders);
	}

	m_attackers.FillHoles();
	m_defenders.FillHoles();
}

void CTP2Combat::ExecuteRangedAttack(CombatField *attacker, sint32 attX, sint32 attY,
									 CombatField *defender, sint32 defX, sint32 defY)
{
	m_noAttacksPossible = false;

	
	
	CombatUnit *att = &attacker->GetUnit(attX, attY);
	CombatUnit *def = &defender->GetUnit(defX, defY);

	
	if(m_battle && !att->m_alreadyAttacked) {
		BattleEvent *attackEvent = new BattleEvent(BATTLE_EVENT_TYPE_ATTACK);
		m_battle->AddUnitAttack(attackEvent,
			(attacker == &m_defenders), att->m_unit);
		m_battle->AddEvent(attackEvent);
		att->m_alreadyAttacked = true;
	}

	const char *attackString = attacker == &m_attackers ? "Attacker" : "Defender";
	const char *defenseString = defender == &m_defenders ? "Defender" : "Attacker";

	
	double hitChance = att->GetRangedAttack() /
		(att->GetRangedAttack() + def->m_unit->GetDefense(att->m_unit));

	if(sint32(COMBATRAND(1000)) < sint32(hitChance * 1000.0)) {
		

		
		double damage = att->GetStrength() / def->GetArmor();
		combat_print(k_COMBAT_DEBUG_VERBOSE, "RANGED: %s@(%2d,%2d) -HIT-  %s@(%2d,%2d) for %.2lf", attackString, attX, attY, 
					 defenseString, defX, defY, damage);

#ifdef TEST_APP
		s_trackedUnits[att->m_id].m_DamageInflicted += damage;
#endif
		
		def->DeductHP(damage);

		if(!def->IsAlive()) {
			combat_print(k_COMBAT_DEBUG_VERBOSE, ", killing %s.\n", defenseString);

			
			if(m_battle) {
				BattleEvent *deathEvent = new BattleEvent(BATTLE_EVENT_TYPE_DEATH);
				DPRINTF(k_DBG_GAMESTATE, ("Adding death for %lx\n", def->m_unit));
				m_battle->AddUnitDeath(deathEvent,
					(defender == &m_defenders), def->m_unit);
				m_battle->AddEvent(deathEvent);
			}
			s_somethingDied = true;
		} else {
			combat_print(k_COMBAT_DEBUG_VERBOSE, ", %s has %.2lfHP.\n", defenseString, def->GetHP());

			
			if(m_battle) {
				if(!def->m_alreadyExploded) {
					BattleEvent *explodeEvent = new BattleEvent(BATTLE_EVENT_TYPE_EXPLODE);
					m_battle->AddUnitExplosion(explodeEvent,
											   (defender == &m_defenders), def->m_unit);
					m_battle->AddEvent(explodeEvent);
					def->m_alreadyExploded = true;
				}
			}
		}
	} else {
		combat_print(k_COMBAT_DEBUG_VERBOSE, "RANGED: %s@(%2d,%2d) MISSED %s@(%2d,%2d).\n", attackString, attX, attY, 
					 defenseString, defX, defY);
	}
}

void CTP2Combat::DoRangedAttacks(CombatField *attacker, CombatField *defender)
{
	sint32 x, y;

	
	for(x = 1; x < m_width; x++) {
		for(y = 0; y < m_height; y++) {
			CombatUnit *att = &attacker->GetUnit(x, y);
			if(att->IsActive() &&
			   att->GetRangedAttack() > 0.001) {
				sint32 dx, dy;
				if(!defender->FindTargetForRangedAttackFrom(x, y, &dx, &dy)) {
					Assert(defender->NumAlive() == 0);
				} else {
					Assert(dx >= 0 && dy >= 0);
					ExecuteRangedAttack(attacker, x, y, defender, dx, dy);
				}
			}
		}
	}
}

void CTP2Combat::DoRangedAttacks()
{
	DoRangedAttacks(&m_attackers, &m_defenders);
}

void CTP2Combat::DoRangedCounterAttacks()
{
	DoRangedAttacks(&m_defenders, &m_attackers);
}

void CTP2Combat::ExecuteAttack(CombatField *attacker, sint32 attX, sint32 attY,
							   CombatField *defender, sint32 defX, sint32 defY)
{
	m_noAttacksPossible = false;

	
	CombatUnit *att = &attacker->GetUnit(attX, attY);
	CombatUnit *def = &defender->GetUnit(defX, defY);

	
	if(m_battle && !att->m_alreadyAttacked) {
		BattleEvent *attackEvent = new BattleEvent(BATTLE_EVENT_TYPE_ATTACK);
		m_battle->AddUnitAttack(attackEvent,
			(attacker == &m_defenders), att->m_unit);
		m_battle->AddEvent(attackEvent);
		att->m_alreadyAttacked = true;
	}

	const char *attackString = attacker == &m_attackers ? "Attacker" : "Defender";
	const char *defenseString = defender == &m_defenders ? "Defender" : "Attacker";

	
	double hitChance = att->m_unit->GetOffense(def->m_unit) /
		(att->m_unit->GetOffense(def->m_unit) + def->m_unit->GetDefense(att->m_unit));
	combat_print(k_COMBAT_DEBUG_VERBOSE, "NORMAL: %s@(%2d,%2d) -Attacking-  %s@(%2d,%2d) -  HitChance: %.2lf\n", attackString, attX, attY,
				 defenseString, defX, defY, hitChance);

	if(sint32(COMBATRAND(1000)) < sint32(hitChance * 1000.0)) {
		
		double damage = att->GetStrength() / def->GetArmor();
		combat_print(k_COMBAT_DEBUG_VERBOSE, "NORMAL: %s@(%2d,%2d) -HIT-  %s@(%2d,%2d) for %.2lf", attackString, attX, attY,
					 defenseString, defX, defY, damage);
#ifdef TEST_APP
		s_trackedUnits[att->m_id].m_DamageInflicted += damage;
#endif

		def->DeductHP(damage);

		if(!def->IsAlive()) {
			combat_print(k_COMBAT_DEBUG_VERBOSE, ", killing %s.\n", defenseString);

			
			if(m_battle) {
				BattleEvent *deathEvent = new BattleEvent(BATTLE_EVENT_TYPE_DEATH);
				DPRINTF(k_DBG_GAMESTATE, ("Adding death for %lx\n", def->m_unit));
				m_battle->AddUnitDeath(deathEvent,
					(defender == &m_defenders), def->m_unit);
				m_battle->AddEvent(deathEvent);
			}
			s_somethingDied = true;
		} else {
			combat_print(k_COMBAT_DEBUG_VERBOSE, ", %s has %.2lfHP.\n", defenseString, def->GetHP());

			
			if(m_battle) {
				if(!def->m_alreadyExploded) {
					BattleEvent *explodeEvent = new BattleEvent(BATTLE_EVENT_TYPE_EXPLODE);
					m_battle->AddUnitExplosion(explodeEvent,
											   (defender == &m_defenders), def->m_unit);
					m_battle->AddEvent(explodeEvent);
					def->m_alreadyExploded = true;
				}
			}
		}
	} else {
		combat_print(k_COMBAT_DEBUG_VERBOSE, "NORMAL: %s@(%2d,%2d) MISSED %s@(%2d,%2d).\n", attackString, attX, attY, 
					 defenseString, defX, defY);
	}
}


void CTP2Combat::DoAttacks(CombatField *attacker, CombatField *defender)
{
	
	sint32 x = 0, y;
	
	for(y = 0; y < m_height; y++) {
		CombatUnit *att = &attacker->GetUnit(x, y);
		if(att->IsActive() &&
		   att->GetOffense() > 0.001) {
			sint32 dx, dy;
			if(!defender->FindTargetForAttackFrom(x, y, &dx, &dy, att->GetCombatType() == UNIT_TYPE_FLANKER)) {
				
			} else {
				Assert(dx >= 0 && dy >= 0);
				ExecuteAttack(attacker, x, y, defender, dx, dy);
			}
		}
	}
}

void CTP2Combat::DoAttacks()
{
	DoAttacks(&m_attackers, &m_defenders);
}

void CTP2Combat::DoCounterAttacks()
{
	DoAttacks(&m_defenders, &m_attackers);
}

void CTP2Combat::DoMovement()
{
	
	

	
	
	

	Balance();

	m_attackers.Move();
	m_defenders.Move();

	
	if(m_battle) {
		BattleEvent *placementEvent = new BattleEvent(BATTLE_EVENT_TYPE_PLACEMENT);
		Assert(placementEvent);
		m_attackers.ReportUnits(m_battle, placementEvent, false);
		m_defenders.ReportUnits(m_battle, placementEvent, false);
		m_battle->AddEvent(placementEvent);
	}
}

bool CTP2Combat::ResolveOneRound()
{
	s_somethingDied = false;

	
	if(m_attackers.NumAlive() > 0 && m_defenders.NumAlive() > 0) {

		
		m_noAttacksPossible = true;

		
		combat_print(k_COMBAT_DEBUG_VERBOSE, "ROUND %d: (Rand calls: %d)\n", m_round, g_rand->CallCount());
#ifdef TEST_APP
		DumpState();
#endif

		
		
		if(!m_retreating) {
			
			DoRangedAttacks();
		}

		
		DoRangedCounterAttacks();

		
		
		if(!m_retreating) {
			
			DoAttacks();
		}

		
		DoCounterAttacks();
	
		
		DoMovement();

		
		m_round++;
	}

	
	bool playAnimations = true;
	if(m_battle && (m_round > 1)) {
		if(IsDone() || s_somethingDied || ++m_roundsSinceUpdate >= g_theProfileDB->GetBattleSpeed()) {
			if(g_battleViewWindow && m_battle) {
				g_battleViewWindow->UpdateBattle(m_battle);
			}
			m_roundsSinceUpdate = 0;
			
			
			m_attackers.StartRound();
			m_defenders.StartRound();
		} else {
			playAnimations = false;
		}
	}

	
	
	if(m_retreating) {
		m_retreated = true;
	}

	
	
	
	return playAnimations || IsDone();
}

bool CTP2Combat::IsDone()
{
	return m_attackers.NumAlive() <= 0 || m_defenders.NumAlive() <= 0 ||
		m_retreated || m_noAttacksPossible;
}

void CTP2Combat::Retreat()
{
	if(!this) return;
	m_retreating = true;
}

void CTP2Combat::KillUnits()
{
	sint32 i;
	for(i = 0; i < m_deadUnits.Num(); i++) {
		Unit u = m_deadUnits[i];
		sint32 attacker;
		if(u.GetOwner() == GetAttacker()) {
			attacker = GetDefender();
		} else {
			attacker = GetAttacker();
		}

		g_gevManager->AddEvent(GEV_INSERT_AfterCurrent,
							   GEV_KillUnit,
							   GEA_Unit, u,
							   GEA_Int, CAUSE_REMOVE_ARMY_ATTACKED,
							   GEA_Player, attacker,
							   GEA_End);
	}
}

#ifndef TEST_APP

Army CTP2Combat::GetAttackerArmy()
{
	return Army(m_army_id);
}
#endif

#ifdef TEST_APP
int main(int argc, char **argv)
{
	srand(time(0));

	s_output = stdout;

	if(argc != 4) {
		error_exit("Usage: %s <num_runs> <debuglevel> <inputfile>\n", argv[0]);
	}

	sint32 fileArg = 3;
	sint32 numRuns = atoi(argv[1]);
	g_debugLevel = atoi(argv[2]);
	
	sint32 offenseVictory = 0;
	sint32 defenseVictory = 0;

	sint32 n;
	for(n = 0; n < numRuns; n++) {
		s_unitId = 0;
		FILE *inputfile = fopen(argv[fileArg], "r");
		if(!inputfile) {
			error_exit("Can't open %s\n", argv[0]);
		}
		
		sint32 w, h;
		if(fscanf(inputfile, "%d,%d\n", &w, &h) != 2) {
			error_exit("First line must hold width,height\n");
		}
		
		CTP2Combat combat(w, h, inputfile);

		while(!combat.IsDone()) {
			combat.ResolveOneRound();
		}

		fclose(inputfile);

		if(combat.GetNumLivingAttackers() > 0) {
			combat_print(k_COMBAT_DEBUG_VERBOSE, "***ATTACKER WINS\n");
			offenseVictory++;
		} else {
			combat_print(k_COMBAT_DEBUG_VERBOSE, "***DEFENDER WINS\n");
			defenseVictory++;
		}
	}

	uint32 i;
	for(i = 0; i < s_unitId; i++) {
		combat_print(k_COMBAT_DEBUG_NORMAL, "Unit %2d inflicted %8lf damage\n", i, s_trackedUnits[i].m_DamageInflicted);
	}

	combat_print(k_COMBAT_DEBUG_NORMAL, "Totals: Attackers won: %d, defenders won %d\n", offenseVictory, defenseVictory);

	return 0;
}
#endif
