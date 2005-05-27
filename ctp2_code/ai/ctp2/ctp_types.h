










#ifdef HAVE_PRAGMA_ONCE
#pragma once
#endif
#ifndef __CTP_TYPES_H__
#define __CTP_TYPES_H__

typedef signed char			schar;
typedef unsigned char		uchar;

typedef signed char			sint8; 
typedef unsigned char		uint8;
typedef signed short int	sint16;
typedef unsigned short int	uint16;
typedef signed long int		sint32;
typedef unsigned long int	uint32;
typedef signed __int64		sint64; 
typedef unsigned __int64	uint64; 


typedef char				MBCHAR;


#error "Do not use this"

extern sint32 g_turn;
extern sint32 g_numPlayers;

struct MapPointData {
	MapPointData(sint16 X=0, 
		sint16 Y=0, 
		sint16 Z=0): x(X), y(Y), z(Z) {}
	sint16 x,y,z;
};

class Player {
public:
	Player() : m_currentRound(0) {}
	static Player & GetPlayer(const sint32 &playerId)
		{ return s_players[playerId]; }
	sint32 m_currentRound;
private:
	static vector<Player> s_players;
};

typedef bool BOOL;

enum CAUSE_REMOVE_ARMY {
		CAUSE_REMOVE_ARMY_UNKNOWN,
		CAUSE_REMOVE_ARMY_FLOOD,
		
		CAUSE_REMOVE_ARMY_ATTACKED, 
		CAUSE_REMOVE_ARMY_CHEAT, 
		CAUSE_REMOVE_ARMY_TRANSPORTED,
		CAUSE_REMOVE_ARMY_GROUPING, 
		CAUSE_REMOVE_ARMY_UNGROUPING, 
		CAUSE_REMOVE_ARMY_RAIL_LAUNCH, 
		CAUSE_REMOVE_ARMY_PARADROP, 
		CAUSE_REMOVE_ARMY_SETTLE, 
		CAUSE_REMOVE_ARMY_STARVATION, 
		CAUSE_REMOVE_ARMY_PARKRANGER,     
		CAUSE_REMOVE_ARMY_NUKE, 
		CAUSE_REMOVE_ARMY_BOMBARD, 
		CAUSE_REMOVE_ARMY_SUE, 
		CAUSE_REMOVE_ARMY_POLLUTION, 
		CAUSE_REMOVE_ARMY_UPRISING,     
		CAUSE_REMOVE_ARMY_SLAVE_UPRISING, 
		CAUSE_REMOVE_ARMY_ENSLAVED,
		CAUSE_REMOVE_ARMY_DIPLOMACY, 
		CAUSE_REMOVE_ARMY_NO_MAT_SUPPORT,  
	
		
		CAUSE_REMOVE_ARMY_DIED_IN_ATTACK, 
		CAUSE_REMOVE_ARMY_DIED_IN_ATTACK_ON_TOP,
		CAUSE_REMOVE_ARMY_DIED_IN_SPYING, 
		CAUSE_REMOVE_ARMY_DIED_IN_SLAVERAID,
		CAUSE_REMOVE_ARMY_DIED_IN_UNDERGROUND_RR_RAID,
		CAUSE_REMOVE_ARMY_DIED_IN_CONVERT_RAID, 
		CAUSE_REMOVE_ARMY_DIED_IN_REFORMATION, 
		CAUSE_REMOVE_ARMY_COUNTERBOMBARD,
		CAUSE_REMOVE_ARMY_OUTOFFUEL,
		CAUSE_REMOVE_ARMY_REENTRY,
		CAUSE_REMOVE_ARMY_GOVERNMENT_CHANGE,
		CAUSE_REMOVE_ARMY_TOE,
		CAUSE_REMOVE_ARMY_RUSTLE,
		CAUSE_REMOVE_ARMY_LAND_MINES,
		CAUSE_REMOVE_ARMY_TRANSPORT_DIED,
		CAUSE_REMOVE_ARMY_DIDNT_NEED_IT_AFTER_ALL,
		CAUSE_REMOVE_ARMY_DISBANDED,
		
		CAUSE_REMOVE_ARMY_PARADROP_COMPLETE,
		CAUSE_REMOVE_ARMY_NUKES_ELIMINATED,
		CAUSE_REMOVE_ARMY_EMANCIPATION,
		CAUSE_REMOVE_ARMY_ILLEGAL_CELL,
		CAUSE_REMOVE_ARMY_EXPELLED_NO_CITIES, 
		CAUSE_REMOVE_ARMY_SCENARIO,
		CAUSE_REMOVE_ARMY_PROBE_RECOVERED,
};

struct GSObj {
	GSObj() {
		owner = id = attack = 1;
	}
	GSObj(const sint32 &o, 
		  const sint32 &i, 
		  const sint32 a) : owner(o), id(i), attack(a) {}
	sint32 owner;
	sint32 id;
	sint32 attack;
};

typedef GSObj Unit;
typedef GSObj Pop;
typedef GSObj City;
typedef GSObj Army;

class GameEventArgList {
public:
	BOOL SetArg(const sint32 & type, const sint32 & index, const GSObj & val)
		{ m_argLists[type][index] = val; return true; }
	BOOL SetArg(const sint32 & type, const sint32 & index, const sint32 & val)
		{ m_argLists[type][index].id = val; return true;}
	BOOL SetCity(const sint32 & index, const Unit &c) 
		{ return SetArg(0,index,c); }
	BOOL SetUnit(const sint32 & index, const Unit &u)
		{ return SetArg(1,index,u); }
	BOOL SetPop(const sint32 & index, const Pop &p)
		{ return SetArg(2,index,p); }
	BOOL setArmy(const sint32 & index, const Army &a)
		{ return SetArg(3,index,a); }
	BOOL SetInt(const sint32 & index, const sint32 &value)
		{ return SetArg(4,index,value); }
	BOOL SetPlayer(const sint32 & index, const PLAYER_INDEX &player)
		{ m_argLists[5][index].owner = (sint32) player; return true;}

	BOOL GetArg(const sint32 & type, const sint32 & index, GSObj &val)
		{ val = m_argLists[type][index]; return true;}
	BOOL GetArg(const sint32 & type, const sint32 & index, sint32 &val)
		{ val = m_argLists[type][index].id; return true;}
	BOOL GetCity(const sint32 & index, Unit &c)
		{ return GetArg(0,index,c); }
	BOOL GetUnit(const sint32 & index, Unit &u)
		{ return GetArg(1,index,u); }
	BOOL GetPop(const sint32 & index, Pop &p)
		{ return GetArg(2,index,p); }
	BOOL GetArmy(const sint32 & index, Army &a)
		{ return GetArg(3,index,a); }
	BOOL GetInt(const sint32 & index, sint32 &value)
		{ return GetArg(4,index,value); }
	BOOL GetPlayer(const sint32 & index, PLAYER_INDEX &player)
		{ player = (PLAYER_INDEX) m_argLists[5][index].owner; return true;}

private:
	GSObj m_argLists[6][4];
};

enum GOAL_TYPE {
	GOAL_DEFENSE,
	GOAL_ATTACK
};

class Scheduler {
public:
	Scheduler() {
	};

	
	static const Scheduler & GetScheduler(const PLAYER_INDEX playerId)	
		{ return s_scheduler[playerId]; }

	
	sint32 GetValueUnsatisfiedGoals(const GOAL_TYPE & goalType) const {
		return 10;
	};

	City GetCityUnsatisfiedGoals(const GOAL_TYPE & goalType) const {
		return City();
	}

private:
	static vector<Scheduler> s_scheduler;

};

































#endif

