//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ header file
// Description  :
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
//----------------------------------------------------------------------------
#ifdef HAVE_PRAGMA_ONCE
#pragma once
#endif
#ifndef GLOBALS_H
#define GLOBALS_H 1

#define k_GAME_OBJ_TYPE_UNIT                     1U
#define k_GAME_OBJ_TYPE_POP                      2U
#define k_GAME_OBJ_TYPE_IMPROVEMENT              3U
#define k_GAME_OBJ_TYPE_TRADE_ROUTE              4U
#define k_GAME_OBJ_TYPE_TRADE_OFFER              5U
#define k_GAME_OBJ_TYPE_TERRAIN_IMPROVEMENT      6U
#define k_GAME_OBJ_TYPE_DIPLOMATIC_REQUEST       7U
#define k_GAME_OBJ_TYPE_INSTALLATION             8U
#define k_GAME_OBJ_TYPE_WONDER                   9U
#define k_GAME_OBJ_TYPE_CIVILISATION            10U
#define k_GAME_OBJ_TYPE_AGREEMENT               11U
#define k_GAME_OBJ_TYPE_MESSAGE                 12U
#define k_GAME_OBJ_TYPE_ARMY                    13U
#define k_GAME_OBJ_TYPE_ENDGAME_OBJECT          14U
#define k_GAME_OBJ_TYPE_TERRAIN_IMPROVEMENT_DB  15U



#define k_GAME_OBJ_TYPE_CAPITALIZATION          16U
#define k_GAME_OBJ_TYPE_INFRASTRUCTURE          17U



#define k_MAX_CIVILISATIONS	64
#define k_MAP_WRAPAROUND 5


#ifdef _MSC_VER
#pragma warning (disable: 4661)
#endif

#include "gstypes.h"
#include "directions.h"

enum ATTITUDE { 
    ATTITUDE_NULL,
    ATTITUDE_HAPPY, 
    ATTITUDE_CONTENT,
    ATTITUDE_ANGRY, 
    ATTITUDE_NO_EFFECT
};


typedef sint32 PopDBIndex; 


enum MAPSIZE {
	MAPSIZE_SMALL,
	MAPSIZE_MEDIUM,
	MAPSIZE_LARGE,
	MAPSIZE_GIGANTIC
};

#endif
