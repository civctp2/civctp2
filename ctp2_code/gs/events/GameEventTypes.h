#ifdef HAVE_PRAGMA_ONCE
#pragma once
#endif
#ifndef __GAME_EVENT_TYPES_H__
#define __GAME_EVENT_TYPES_H__


#include "GameEventDescription.h"

enum GAME_EVENT_INSERT {
	GEV_INSERT_Front,
	GEV_INSERT_AfterCurrent,
	GEV_INSERT_Tail
};

enum GAME_EVENT_ERR {
	GEV_ERR_OK,
	GEV_ERR_Internal,
	GEV_ERR_BadInsert,
	GEV_ERR_BadEvent,
	GEV_ERR_NeedUserInput,
	GEV_ERR_WrongArguments,
	GEV_ERR_StopProcessing,
	GEV_ERR_AtBreak,
};

enum GAME_EVENT_HOOK_DISPOSITION {
	GEV_HD_Continue,      
	GEV_HD_Stop,          
	GEV_HD_NeedUserInput, 

	GEV_HD_MAX
};

enum GAME_EVENT_ARGUMENT {
	GEA_Null, 

	GEA_Army,
	GEA_Unit,
	GEA_City,
	GEA_Pop,
	GEA_Gold,
	GEA_Path,
	GEA_MapPoint,
	GEA_Player,
	GEA_Int,
	GEA_Direction,
	GEA_Advance,
	GEA_Wonder,
	GEA_Improvement,
	GEA_TradeRoute,

	GEA_End, 
};








#define GEAC_ARMY 'a'
#define GEAC_UNIT 'u'
#define GEAC_CITY 'c'
#define GEAC_POP 'C' 
#define GEAC_GOLD 'g'
#define GEAC_PATH 'p'
#define GEAC_MAPPOINT 'l' 
#define GEAC_PLAYER 'P'
#define GEAC_INT 'i'
#define GEAC_DIRECTION 'd'

#define GEAC_ADVANCE 'A'
#define GEAC_WONDER 'w'
#define GEAC_IMPROVEMENT 'I'
#define GEAC_TRADEROUTE 't'

enum GAME_EVENT_PRIORITY {
	GEV_PRI_Pre,
	GEV_PRI_Primary,
	GEV_PRI_Post
};

#endif
