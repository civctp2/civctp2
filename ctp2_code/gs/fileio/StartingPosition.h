#if defined(HAVE_PRAGMA_ONCE)
#pragma once
#endif

#ifndef STARTING_POSITION_H_
#define STARTING_POSITION_H_

struct StartingPosition;

enum STARTINFOTYPE {
	STARTINFOTYPE_NONE,
	STARTINFOTYPE_CIVSFIXED,      
	STARTINFOTYPE_POSITIONSFIXED, 
	STARTINFOTYPE_CIVS,           
	                              
	STARTINFOTYPE_NOLOCS,		
};

#include "ctp2_inttypes.h"
#include "MapPoint.h"

struct StartingPosition {
	MapPoint point;
	sint32 civIndex;
};

#define k_MAX_START_POINTS 64

#endif
