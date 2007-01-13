










#ifdef HAVE_PRAGMA_ONCE 
#pragma once
#endif
#ifndef __TILESET_H__
#define __TILESET_H__

typedef uint16		TILEINDEX;


#include "gstypes.h"


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


#define k_MAX_IMPROVEMENTS			600
#define k_IMPROVEMENT_ROAD_BASE		200
#define k_IMPROVEMENT_RR_BASE		300
#define K_IMPROVEMENT_MAGLEV_BASE	400

enum MAPICON { 
	
	MAPICON_BIODISEASE,
	MAPICON_NANODISEASE,
	MAPICON_CONVERSION,
	MAPICON_FRANCHISE,
	MAPICON_INJUNCTION,
	MAPICON_HERALD,
	MAPICON_UNHAPPY,

	MAPICON_BRACKET1,
	MAPICON_BRACKET2,
	MAPICON_BRACKET3,
	
	MAPICON_BRACKET4,

	MAPICON_HERALD2,
	MAPICON_HERALD3,
	MAPICON_HERALD4,
	MAPICON_HERALD5,
	MAPICON_HERALD6,
	MAPICON_HERALD7,
	MAPICON_HERALD8,
	MAPICON_HERALD9,

	MAPICON_RESOURCE1,
	
	MAPICON_RESOURCE2,
	MAPICON_RESOURCE3,

	MAPICON_FLAG,

	MAPICON_CIRCLE1,
	MAPICON_CIRCLE2,
	MAPICON_CIRCLE3,

	MAPICON_UNKNOWN1,
	MAPICON_UNKNOWN2,
	MAPICON_UNKNOWN3,
	MAPICON_UNKNOWN4,
	
	MAPICON_UNKNOWN5,
	MAPICON_UNKNOWN6,
	MAPICON_UNKNOWN7,
	MAPICON_UNKNOWN8,
	MAPICON_UNKNOWN9,
	MAPICON_UNKNOWN10,
	MAPICON_UNKNOWN11,
	MAPICON_UNKNOWN12,

	MAPICON_SLAVE,
	MAPICON_SLEEPINGUNITS,
	
	MAPICON_AIRPORT,
	MAPICON_UPRISING,
	MAPICON_WATCHFUL,

	MAPICON_POLLUTION,

	MAPICON_CARGO,
	MAPICON_ARMY,
	MAPICON_VETERAN,

	MAPICON_TARGET,
	MAPICON_HERALD10,
	MAPICON_HERALD11,
	MAPICON_HERALD12,

	MAPICON_MAX
};

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
	Pixel16			*GetRoadData(uint16 roadNum) const { return m_improvementData[k_IMPROVEMENT_ROAD_BASE + roadNum]; }

	uint16			GetNumMegaTiles(void) const { return m_numMegaTiles; }
	uint16			GetMegaTileLength(sint32 megaTileNum) const { return m_megaTileLengths[megaTileNum]; }
	MegaTileStep	GetMegaTileStep(sint32 megaTileNum, sint32 stepNum) const { return m_megaTileData[megaTileNum][stepNum]; }

	Pixel16			*GetMapIconData(MAPICON icon) { return m_mapIcons[icon]; }
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
	
	Pixel16			*m_mapIcons[MAPICON_MAX];
	POINT			m_mapIconDimensions[MAPICON_MAX];

	BOOL			m_quick;
	BOOL			m_mapped;

	HANDLE			m_fileHandle;
	HANDLE			m_mappedFileHandle;
};


#endif
