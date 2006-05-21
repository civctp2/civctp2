//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ header file
// Description  : Type definitions
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
// - None
//
//----------------------------------------------------------------------------

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
#pragma once
#endif
#ifndef GS_TYPES_H__
#define GS_TYPES_H__

typedef sint32 PLAYER_INDEX;

#define PLAYER_INDEX_VANDALS 0
#define PLAYER_INDEX_0 0
#define PLAYER_INDEX_INVALID 32

#define k_MAX_PLAYERS 32 

#if 0

enum TERRAIN_TYPES{ 
TERRAIN_NULL = -10000,           
TERRAIN_FOREST =0,       
TERRAIN_PLAINS,         
TERRAIN_TUNDRA,         
TERRAIN_GLACIER,        
TERRAIN_GRASSLAND,      
TERRAIN_DESERT,         
TERRAIN_SWAMP,          
TERRAIN_JUNGLE,         
TERRAIN_MOUNTAIN,       
TERRAIN_HILL,           
TERRAIN_WATER_SHALLOW,  
TERRAIN_WATER_DEEP,     
TERRAIN_WATER_VOLCANO,  
TERRAIN_SPACE,          
TERRAIN_WATER_BEACH,	
TERRAIN_WATER_SHELF,	
TERRAIN_WATER_TRENCH,   
TERRAIN_WATER_RIFT,     
TERRAIN_DEAD,			
TERRAIN_BROWN_HILL,     
TERRAIN_BROWN_MOUNTAIN, 
TERRAIN_WHITE_HILL,     
TERRAIN_WHITE_MOUNTAIN, 
TERRAIN_DEAD_HILL,		
TERRAIN_MAX,
TERRAIN_UNEXPLORED = 31         
};
#endif



#define TERRAIN_MAX 32

typedef sint32 TERRAIN_TYPES;


#define k_BASE_TERRAIN_TYPES (TERRAIN_MAX)

#define k_MAX_ARMY_SIZE  12

#endif
