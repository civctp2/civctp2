











#pragma once
#ifndef __TILEDRAWROAD_H__
#define __TILEDRAWROAD_H__

#define k_ROAD_TYPE_1		0
#define k_ROAD_TYPE_2		1
#define k_ROAD_TYPE_3		2
#define k_ROAD_TYPE_4		3

#define k_ROAD_OFFSET_1		0
#define k_ROAD_OFFSET_2		100
#define k_ROAD_OFFSET_3		200
#define k_ROAD_OFFSET_4		300

#define k_BIT_SOUTH			1 
#define k_BIT_SOUTHWEST		1 << 1
#define k_BIT_WEST			1 << 2
#define k_BIT_NORTHWEST		1 << 3
#define k_BIT_NORTH			1 << 4
#define k_BIT_NORTHEAST		1 << 5
#define k_BIT_EAST			1 << 6
#define k_BIT_SOUTHEAST		1 << 7

#define k_MASK_8			0x45
#define k_MASK_9			0x8a
#define k_MASK_10			0x15
#define k_MASK_11			0x2a
#define k_MASK_12			0x54
#define k_MASK_13			0xa8
#define k_MASK_14			0x51
#define k_MASK_15			0xa2
#define k_MASK_16			0xaa
#define k_MASK_17			0x55

#define k_T_INTERSECT		3
#define k_X_INTERSECT		4

enum ROAD 
{
	ROAD_DEFAULT,
	ROAD_SOUTH = 0,
	ROAD_SOUTHWEST,
	ROAD_WEST,
	ROAD_NORTHWEST,
	ROAD_NORTH,
	ROAD_NORTHEAST,
	ROAD_EAST,
	ROAD_SOUTHEAST,
	ROAD_T_SOUTH,
	ROAD_T_SOUTHWEST,
	ROAD_T_WEST,
	ROAD_T_NORTHWEST,
	ROAD_T_NORTH,
	ROAD_T_NORTHEAST,
	ROAD_T_EAST,
	ROAD_T_SOUTHEAST,
	ROAD_X_DIAGONAL,
	ROAD_X_STRAIGHT,
	ROAD_MAGLEVPOST
};

#endif

