//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ header
// Description  : Tile set
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
// HAVE_UNISTD_H
// HAVE_SYS_STAT_H
// HAVE_SYS_TYPES_H
// LINUX
// WIN32
//
//----------------------------------------------------------------------------
//
// Modifications from the original Activision code:
//
// - Added new map icon database. (3-Mar-2007 Martin Gühmann)
// - Increased the number of possible tile improvement graphics in the
//   tile to 1024. (28-Feb-2008 Martin Gühmann)
// - Added show city production icons. (25-Jun-2009 Maq)
//
//----------------------------------------------------------------------------

#ifdef HAVE_PRAGMA_ONCE 
#pragma once
#endif
#ifndef __TILESET_H__
#define __TILESET_H__

class TileSet;

#define k_TILE_SKIP_PIXEL			0x0000
#define k_TILE_COPY_RUN_ID			0x0C
#define k_TILE_SKIP_RUN_ID			0x0A
#define k_TILE_SHADOW_RUN_ID		0x0D
#define k_TILE_COLORIZE_RUN_ID		0x0B
#define k_TILE_EOLN_ID				0xE0

#define k_32_BIT_SHADOW_PIXEL		0x00FF00FF
#define k_32_BIT_SKIP_PIXEL			0x00000000
#define k_32_BIT_COLORIZE_PIXEL		0x00FFFF00
#define k_32_BIT_RGB_MASK			0x00FFFFFF


#define k_16_BIT_SHADOW_PIXEL		0x7C1F
#define k_16_BIT_SKIP_PIXEL			0x0000
#define k_16_BIT_COLORIZE_PIXEL		0x03FF
#define k_16_BIT_RGB_MASK			0x7FFF


#define k_TILE_GRID_HEIGHT		72
#define k_TILE_GRID_WIDTH		94

#define k_TILE_PIXEL_WIDTH		94
#define k_TILE_PIXEL_HEIGHT		48
#define k_TILE_PIXEL_HEADROOM	24


#define k_MAX_TRANSFORMS		300

#define k_TRANSFORM_SIZE		15		
#define k_TRANSFORM_TO_INDEX	9
#define	k_TRANSFORM_TO_LIST_FIRST		10
#define k_TRANSFORM_TO_LIST_ID			-1
#define k_MAX_TRANSFORM_TO_LIST	5


#define k_MAX_BASE_TILES		3000
#define k_TRANSITIONS_PER_TILE	4
#define k_FIRST_VARIATION		600
#define k_FIRST_MEGATILE		700


#define k_MAX_RIVERS			16
#define k_MAX_RIVER_TRANSFORMS	32
#define k_RIVER_TRANSFORM_SIZE	10


#define k_MAX_MEGATILES			50
#define k_MAX_MEGATILE_STEPS	5
#define k_MEGATILE_DIRECTION_X	0
#define k_MEGATILE_DIRECTION_N	1
#define k_MEGATILE_DIRECTION_E	2
#define k_MEGATILE_DIRECTION_S	3
#define k_MEGATILE_DIRECTION_W	4

#define k_MEGATILE_DIRECTION_CHAR_X		'X'
#define k_MEGATILE_DIRECTION_CHAR_N		'N'
#define k_MEGATILE_DIRECTION_CHAR_E		'E'
#define k_MEGATILE_DIRECTION_CHAR_S		'S'
#define k_MEGATILE_DIRECTION_CHAR_W		'W'

struct MegaTileStep{
	uint32		direction:6,		
				terrainType:10,		
				tileNum:16;			
};


#define k_MAX_IMPROVEMENTS			1024

enum MAPICON { 
	
	MAPICON_BIODISEASE,       //upc001.rim
	MAPICON_NANODISEASE,
	MAPICON_CONVERSION,
	MAPICON_FRANCHISE,
	MAPICON_INJUNCTION,
	MAPICON_HERALD,	          //upc006
	MAPICON_UNHAPPY,          //upc007.rim

	MAPICON_BRACKET1,
	MAPICON_BRACKET2,
	MAPICON_BRACKET3,  
	
	MAPICON_BRACKET4,

	MAPICON_HERALD2,          //upc012.rim
	MAPICON_HERALD3,
	MAPICON_HERALD4,
	MAPICON_HERALD5,
	MAPICON_HERALD6,
	MAPICON_HERALD7,
	MAPICON_HERALD8,
	MAPICON_HERALD9,

	MAPICON_RESOURCE1,        //upc020.rim
	
	MAPICON_RESOURCE2,
	MAPICON_RESOURCE3,

	MAPICON_FLAG,             //upc023.rim

	MAPICON_CIRCLE1,
	MAPICON_CIRCLE2,
	MAPICON_CIRCLE3,

	MAPICON_UNKNOWN1,         //upc027.rim  arch
	MAPICON_UNKNOWN2,    
	MAPICON_UNKNOWN3,
	MAPICON_UNKNOWN4,         //upc030.rim 
	
	MAPICON_UNKNOWN5,
	MAPICON_UNKNOWN6,         //house
	MAPICON_UNKNOWN7,         //earth
	MAPICON_UNKNOWN8,         //person/slave
	MAPICON_UNKNOWN9,         //upc035.rim  city
	MAPICON_UNKNOWN10,        //house
	MAPICON_UNKNOWN11,        //earth
	MAPICON_UNKNOWN12,        //person/slave

	MAPICON_SLAVE,
	MAPICON_SLEEPINGUNITS,
	
	MAPICON_AIRPORT,
	MAPICON_UPRISING,
	MAPICON_WATCHFUL,         //upc043.rim

	MAPICON_POLLUTION,        //upc044.rim factory

	MAPICON_CARGO,
	MAPICON_ARMY,             //upc046.rim star
	MAPICON_VETERAN,


	MAPICON_TARGET,
	MAPICON_HERALD10,
	MAPICON_HERALD11,
	MAPICON_HERALD12,         //upc051.rim
//EMOD NEW ICONS
	MAPICON_CAPITOL,	      //upc052.tga
//new borders
	MAPICON_POLBORDERNW,      //upc053.tga
	MAPICON_POLBORDERNE,      //upc054.tga
	MAPICON_POLBORDERSW,      //upc055.tga
	MAPICON_POLBORDERSE,      //upc056.tga
	MAPICON_ELITE,            //upc057.tga
//Show city production icons
	MAPICON_PRODUCTIONNEW,	  //upc154.tga
	MAPICON_PRODUCTIONNEWBIG, //upc155.tga
//religions
/*
	MAPICON_RELIGION1,	//upc057.tga  //judaism
	MAPICON_RELIGION2,	//58 buhddism
	MAPICON_RELIGION3,	//59 islam
	MAPICON_RELIGION4,	//60 taoism
	MAPICON_RELIGION5,	//61 christianity
	MAPICON_RELIGION6,	//62 confuscianism
	MAPICON_RELIGION7,	//63 hinduism
	MAPICON_RELIGION8,  //64orthodox christianity?  communism?
	MAPICON_RELIGION9,	//65shia islam?
	MAPICON_RELIGION10,	//66paganism?  
//civilization flags 

	MAPICON_CIVFLAG_0,		//67
	MAPICON_CIVFLAG_1,
	MAPICON_CIVFLAG_2,
	MAPICON_CIVFLAG_3,
	MAPICON_CIVFLAG_4,
	MAPICON_CIVFLAG_5,
	MAPICON_CIVFLAG_6,
	MAPICON_CIVFLAG_7,
	MAPICON_CIVFLAG_8,
	MAPICON_CIVFLAG_9,
	MAPICON_CIVFLAG_10,
	MAPICON_CIVFLAG_11,
	MAPICON_CIVFLAG_12,
	MAPICON_CIVFLAG_13,		//80
	MAPICON_CIVFLAG_14,
	MAPICON_CIVFLAG_15,
	MAPICON_CIVFLAG_16,
	MAPICON_CIVFLAG_17,	
	MAPICON_CIVFLAG_18,
	MAPICON_CIVFLAG_19,
	MAPICON_CIVFLAG_20,
	MAPICON_CIVFLAG_21,
	MAPICON_CIVFLAG_22,
	MAPICON_CIVFLAG_23,
	MAPICON_CIVFLAG_24,
	MAPICON_CIVFLAG_25,
	MAPICON_CIVFLAG_26,
	MAPICON_CIVFLAG_27,
	MAPICON_CIVFLAG_28,
	MAPICON_CIVFLAG_29,
	MAPICON_CIVFLAG_30,
	MAPICON_CIVFLAG_31,
	MAPICON_CIVFLAG_32,
	MAPICON_CIVFLAG_33,		//upc100
	MAPICON_CIVFLAG_34,
	MAPICON_CIVFLAG_35,
	MAPICON_CIVFLAG_36,
	MAPICON_CIVFLAG_37,
	MAPICON_CIVFLAG_38,
	MAPICON_CIVFLAG_39,
	MAPICON_CIVFLAG_40,
	MAPICON_CIVFLAG_41,
	MAPICON_CIVFLAG_42,
	MAPICON_CIVFLAG_43,
	MAPICON_CIVFLAG_44,
	MAPICON_CIVFLAG_45,
	MAPICON_CIVFLAG_46,
	MAPICON_CIVFLAG_47,
	MAPICON_CIVFLAG_48,
	MAPICON_CIVFLAG_49,
	MAPICON_CIVFLAG_50,
	MAPICON_CIVFLAG_51,
	MAPICON_CIVFLAG_52,
	MAPICON_CIVFLAG_53,		//upc120
	MAPICON_CIVFLAG_54,
	MAPICON_CIVFLAG_55,
	MAPICON_CIVFLAG_56,
	MAPICON_CIVFLAG_57,
	MAPICON_CIVFLAG_58,
	MAPICON_CIVFLAG_59,
	MAPICON_CIVFLAG_60,
	MAPICON_CIVFLAG_61,
	MAPICON_CIVFLAG_62,
	MAPICON_CIVFLAG_63,
	MAPICON_CIVFLAG_64,
	MAPICON_CIVFLAG_65,
	MAPICON_CIVFLAG_66,
	MAPICON_CIVFLAG_67,
	MAPICON_CIVFLAG_68,
	MAPICON_CIVFLAG_69,
	MAPICON_CIVFLAG_70,		//137


//city expansion?
	MAPICON_URBANANC,
	MAPICON_URBANMED,
	MAPICON_URBANMOD,
	MAPICON_URBANSLUM,
//visible wonders
	MAPICON_WONDER1,
	MAPICON_WONDER2,	
	MAPICON_WONDER3,	
	MAPICON_WONDER4,	
	MAPICON_WONDER5,	
	MAPICON_WONDER6,	
	MAPICON_WONDER7,
	MAPICON_WONDER8,
	MAPICON_WONDER9,
	MAPICON_WONDER10,
	MAPICON_WONDER11,
	MAPICON_WONDER12,
	MAPICON_WONDER13,
	MAPICON_WONDER14,
	MAPICON_WONDER15,
	MAPICON_WONDER16,  //only need 16 for call2civ mod
	MAPICON_WONDER17,
	MAPICON_WONDER18,
	MAPICON_WONDER19,
	MAPICON_WONDER21,
	MAPICON_WONDER22,
	MAPICON_WONDER23,
	MAPICON_WONDER24,
	MAPICON_WONDER25,
	MAPICON_WONDER26,
	MAPICON_WONDER27,
	MAPICON_WONDER28,
	MAPICON_WONDER29,
	MAPICON_WONDER30,
	MAPICON_WONDER31,
	MAPICON_WONDER32, //total number of wonders in ctp2 default game
	MAPICON_WONDER33,
	MAPICON_WONDER34,
	MAPICON_WONDER35,
//end emod
*/
	MAPICON_MAX   //this returns the total number or Icons
};

#include "gstypes.h"
#include "pixeltypes.h"     // Pixel16


class BaseTile;

class TileSet
{
public:
	TileSet();
	~TileSet();

	void			Cleanup(void);
	void			CleanupQuick(void);
	void			CleanupMapped(void);

	void			LoadBaseTiles(FILE *file);
	void			LoadTransitions(FILE *file);
	void			LoadTransforms(FILE *file);
	void			LoadRiverTransforms(FILE *file);
	void			LoadImprovements(FILE *file);
	void			LoadMegaTiles(FILE *file);

	void			LoadMapIcons(void);
	//Pixel16			ConvertMapIcons(const MBCHAR *name);  //EMOD

	void			Load(void);

	Pixel16			*GetRiverData(uint16 riverNum) const { return m_riverData[riverNum]; }

	sint16			*GetTransformData(uint16 transformNum) const { return m_transforms[transformNum]; }
	sint16			GetTransform(uint16 transformNum, uint16 index) { return m_transforms[transformNum][index]; }
	uint16			GetNumTransforms(void) const { return m_numTransforms; }

	BaseTile		*GetBaseTile(uint16 baseTileNum) const { return m_baseTiles[baseTileNum]; }
	Pixel16			*GetTransitionData(uint16 from, uint16 to, uint16 which) const { return m_transitions[from][to][which]; }

	uint16			GetNumRiverTransforms(void) const { return m_numRiverTransforms; }
	sint16			GetRiverTransform(uint16 transformNum, uint16 index) { return m_riverTransforms[transformNum][index];}

	Pixel16			*GetImprovementData(uint16 improveNum) const { return m_improvementData[improveNum]; }
	Pixel16			*GetRoadData(uint16 roadNum) const { return m_improvementData[roadNum]; } // Now the same as GetImprovementData

	uint16			GetNumMegaTiles(void) const { return m_numMegaTiles; }
	uint16			GetMegaTileLength(sint32 megaTileNum) const { return m_megaTileLengths[megaTileNum]; }
	MegaTileStep	GetMegaTileStep(sint32 megaTileNum, sint32 stepNum) const { return m_megaTileData[megaTileNum][stepNum]; }

	Pixel16			*GetMapIconData(sint32 icon) { return m_mapIcons[icon]; }
	POINT			GetMapIconDimensions(MAPICON icon) { return m_mapIconDimensions[icon]; }

	uint8			ReverseDirection(sint32 dir);
	
	void			QuickLoadTransforms(uint8 **dataPtr);
	void			QuickLoadTransitions(uint8 **dataPtr);
	void			QuickLoadBaseTiles(uint8 **dataPtr);

	void			QuickLoadRiverTransforms(uint8 **dataPtr);
	void			QuickLoadImprovements(uint8 **dataPtr);
	void			QuickLoadMegaTiles(uint8 **dataPtr);

	void			QuickLoad(void);
	void			QuickLoadMapped(void);

	BOOL			QuickLoaded(void) { return m_quick; }


private:
	uint16			m_numTransforms;
	sint16			**m_transforms;

	uint16			m_numRiverTransforms;
	sint16			**m_riverTransforms;
	Pixel16			**m_riverData;

	BaseTile		*m_baseTiles[k_MAX_BASE_TILES];
	Pixel16			*m_transitions[TERRAIN_MAX][TERRAIN_MAX][k_TRANSITIONS_PER_TILE];

	Pixel16			*m_improvementData[k_MAX_IMPROVEMENTS];

	uint8			*m_tileSetData;

	uint16			m_numMegaTiles;
	uint16			m_megaTileLengths[k_MAX_MEGATILES];
	MegaTileStep	m_megaTileData[k_MAX_MEGATILES][k_MAX_MEGATILE_STEPS];
	
	Pixel16			**m_mapIcons;
	POINT			*m_mapIconDimensions;

	BOOL			m_quick;
	BOOL			m_mapped;

#ifdef WIN32
	HANDLE			m_fileHandle;
	HANDLE			m_mappedFileHandle;
#else
	int			m_fd;
	size_t			m_MMapSize;
#endif
};


#endif
