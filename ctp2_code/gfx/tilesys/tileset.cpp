










 
#include "c3.h"

#include "XY_Coordinates.h"
#include "World.h"

#include "c3errors.h"

#include "pixelutils.h"
#include "tileutils.h"
#include "tiffutils.h"
#include "BaseTile.h"

#include "tiledmap.h"
#include "tileset.h"

#include "CivPaths.h"
#include "rimutils.h"

#include "prjfile.h"
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#ifdef HAVE_SYS_STAT_H
#include <sys/stat.h>
#endif
#ifdef HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif
#include <fcntl.h>
#ifdef LINUX
#include <sys/mman.h>
#endif

extern ProjectFile *g_ImageMapPF;


extern CivPaths		*g_civPaths;
extern TiledMap		*g_tiledMap;
extern sint32		g_is565Format;

TileSet::TileSet()
{
	sint32		i,j,k;
	
	
	m_numTransforms = 0;
	m_transforms = NULL;

	m_numRiverTransforms = 0;
	m_riverTransforms = NULL;
	m_riverData = NULL;

	
	for (i=0; i<TERRAIN_MAX; i++) {
		for (j=0; j<TERRAIN_MAX; j++) {
			for(k=0; k<k_TRANSITIONS_PER_TILE; k++) {
				m_transitions[i][j][k] = NULL;
			}
		}
	}

	
	for (i=0; i<k_MAX_BASE_TILES; i++) {
		m_baseTiles[i] = NULL;
	}

	
	for (i=0; i<MAPICON_MAX; i++) {
		m_mapIcons[i] = NULL;
	}

	for (i=0; i<k_MAX_IMPROVEMENTS; i++) {
		m_improvementData[i] = NULL;
	}

	m_numMegaTiles = 0;
	for (i=0; i<k_MAX_MEGATILES; i++) {
		for (j=0; j<k_MAX_MEGATILE_STEPS; j++) {
			m_megaTileData[i][j].direction = 0;
			m_megaTileData[i][j].terrainType = 0;
			m_megaTileData[i][j].tileNum = 0;
		}
		m_megaTileLengths[i] = 0;
	}

	m_tileSetData = NULL;

	m_quick = FALSE;
	m_mapped = FALSE;
}

TileSet::~TileSet()
{
	Cleanup();
}

void TileSet::CleanupQuick(void)
{
	if (m_transforms) {
		delete[] m_transforms;
		m_transforms = 0;
		m_numTransforms = 0;
	}

	if (m_riverTransforms) {
		delete[] m_riverTransforms;
		m_riverTransforms = NULL;
	}

	if (m_riverData) {
		delete[] m_riverData;
		m_riverData = NULL;
	}

	sint32 i;
	
	for (i=0; i<k_MAX_BASE_TILES; i++) {
		if (m_baseTiles[i] != NULL) {
			delete m_baseTiles[i];
			m_baseTiles[i] = NULL;
		}
	}

	
	for (i=0; i<MAPICON_MAX; i++) {
		delete m_mapIcons[i];
		m_mapIcons[i] = NULL;
	}

	delete[] m_tileSetData;
	m_tileSetData = NULL;
}

void TileSet::CleanupMapped(void)
{
	if (m_transforms) {
		delete[] m_transforms;
		m_transforms = 0;
		m_numTransforms = 0;
	}

	if (m_riverTransforms) {
		delete[] m_riverTransforms;
		m_riverTransforms = NULL;
	}

	if (m_riverData) {
		delete[] m_riverData;
		m_riverData = NULL;
	}

	sint32 i;

	
	for (i=0; i<k_MAX_BASE_TILES; i++) {
		if (m_baseTiles[i] != NULL) {
			delete m_baseTiles[i];
			m_baseTiles[i] = NULL;
		}
	}

	
	for (i=0; i<MAPICON_MAX; i++) {
		delete m_mapIcons[i];
		m_mapIcons[i] = NULL;
	}

#ifdef WIN32
	UnmapViewOfFile(m_tileSetData);

	CloseHandle(m_mappedFileHandle);
	CloseHandle(m_fileHandle);
#else
	munmap(m_tileSetData, m_MMapSize);
	close(m_fd);
#endif
}

void TileSet::Cleanup(void)
{
	sint32		i,j,k;

	if (m_mapped)
		CleanupMapped();
	else
	if (m_quick)
		CleanupQuick();
	else {
		
		if (m_transforms != NULL) {
			for (i=0; i<m_numTransforms; i++) {
				if (m_transforms[i] != NULL)
					delete[] m_transforms[i];
			}

			delete[] m_transforms;
			m_transforms = 0;
			m_numTransforms = 0;
		}


		
		if (m_riverTransforms != NULL) {
			for (i=0; i<m_numRiverTransforms; i++) {
				if (m_riverTransforms[i] != NULL)
					delete[] m_riverTransforms[i];
				if (m_riverData[i] != NULL)
					delete[] m_riverData[i];
			}

			delete[] m_riverTransforms;
			m_riverTransforms = NULL;
			m_numRiverTransforms = 0;

			delete[] m_riverData;
			m_riverData = NULL;
		}

		
		for (i=0; i<TERRAIN_MAX; i++) {
			for (j=0; j<TERRAIN_MAX; j++) {
				if (m_transitions[i][j][0] != NULL) {
					for(k=0; k<k_TRANSITIONS_PER_TILE; k++) {
						delete[] m_transitions[i][j][k];
						m_transitions[i][j][k] = NULL;
					}
				}
			}
		}

		
		for (i=0; i<k_MAX_BASE_TILES; i++) {
			if (m_baseTiles[i] != NULL) {
				delete m_baseTiles[i];
				m_baseTiles[i] = NULL;
			}
		}
	}
}

void TileSet::LoadBaseTiles(FILE *file)
{
	uint16			i;
	BaseTile		*baseTile;
	uint32			baseTileCount;

	c3files_fread((void *)&baseTileCount, 1, sizeof(baseTileCount), file);

	for (i=0; i<baseTileCount; i++) 
	{
		baseTile = new BaseTile();

		baseTile->Read(file);

		m_baseTiles[baseTile->GetTileNum()] = baseTile;

	}
}



void TileSet::LoadTransitions(FILE *file)
{
	sint32		i,k;
	uint32		transitionCount = 0;
	uint32		transitionSize = 0;
	Pixel16		*xData;
	size_t		count;

	count = c3files_fread((void *)&transitionCount, 1, sizeof(transitionCount), file);
	if (count != sizeof(transitionCount)) goto Error;

	count = c3files_fread((void *)&transitionSize, 1, sizeof(transitionSize), file);
	if (count != sizeof(transitionSize)) goto Error;

	sint16	from, to;

	for (i=0; i<(sint32)transitionCount; i++) {
		count = c3files_fread((void *)&from, 1, sizeof(from), file);
		if (count != sizeof(from)) goto Error;
		count = c3files_fread((void *)&to, 1, sizeof(to), file);
		if (count != sizeof(to)) goto Error;

		for(k=0; k<k_TRANSITIONS_PER_TILE; k++) {
			xData = new Pixel16[transitionSize/2];
			count = c3files_fread((void *)xData, 1, transitionSize, file);
			if (count != transitionSize) goto Error;







			m_transitions[from][to][k] = xData;
		}
	}

	return;

Error:
	
	c3errors_FatalDialog("TileSet", "Could not load Transitions");
}

void TileSet::LoadTransforms(FILE *file)
{
	MBCHAR		ageChar = 'f';
	uint16		numTransforms;
	sint16		*transform;
	sint32		i;

	if (file) {
		c3files_fread((void *)&numTransforms, 1, sizeof(numTransforms), file);
		
		m_transforms = new sint16*[numTransforms];
		m_numTransforms = numTransforms;

		for (i=0; i<numTransforms; i++) {
			transform = new sint16[k_TRANSFORM_SIZE];
			c3files_fread((void *)transform, 1, sizeof(sint16)*k_TRANSFORM_SIZE, file);
			m_transforms[i] = transform;
		}
	}
}

void TileSet::LoadRiverTransforms(FILE *file)
{
	MBCHAR		ageChar = 'f';
	uint16		numRiverTransforms;
	sint16		*transform;
	sint32		i;
	uint32		len;
	Pixel16		*riverData;

	if (file) {
		c3files_fread((void *)&numRiverTransforms, 1, sizeof(numRiverTransforms), file);
		
		if (numRiverTransforms > 0) {
			m_riverTransforms = new sint16*[numRiverTransforms];
			m_numRiverTransforms = numRiverTransforms;
			m_riverData = new Pixel16*[numRiverTransforms];

			for (i=0; i<numRiverTransforms; i++) {
				transform = new sint16[k_RIVER_TRANSFORM_SIZE];
				
				
				c3files_fread((void *)transform, 1, sizeof(sint16)*k_RIVER_TRANSFORM_SIZE, file);
				m_riverTransforms[i] = transform;

				
				c3files_fread((void *)&len, 1, sizeof(uint32), file);

				if (len > 0) {
					riverData = new Pixel16[len/2];
					c3files_fread((void *)riverData, 1, len, file);

					


					m_riverData[i] = riverData;
				} else {
					m_riverData[i] = NULL;
				}
			}
		}
	}
}

void TileSet::LoadImprovements(FILE *file)
{
	MBCHAR		ageChar = 'f';
	uint16		numImprovements;
	sint32		i;
	uint32		len;

	if (file) {
		c3files_fread((void *)&numImprovements, 1, sizeof(numImprovements), file);
		
		if (numImprovements > 0) {
			for (i=0; i<numImprovements; i++) {
				uint16		impNum;
				Pixel16		*impData;

				
				c3files_fread((void *)&impNum, 1, sizeof(uint16), file);

				
				c3files_fread((void *)&len, 1, sizeof(uint32), file);

				if (len > 0) {
					impData = new Pixel16[len/2];
					c3files_fread((void *)impData, 1, len, file);

					


					m_improvementData[impNum] = impData;
				} else {
					m_improvementData[impNum] = NULL;
				}
			}
		}
	}
}

void TileSet::LoadMegaTiles(FILE *file)
{
	MBCHAR		ageChar = 'f';
	sint32		i;

	if (file) {
		c3files_fread((void *)&m_numMegaTiles, 1, sizeof(m_numMegaTiles), file);
		
		if (m_numMegaTiles > 0) {
			for (i=0; i<m_numMegaTiles; i++) {
				uint16			megaLen;

				
				c3files_fread((void *)&megaLen, 1, sizeof(uint16), file);

				m_megaTileLengths[i] = megaLen;

				if (megaLen > 0) {
					c3files_fread((void *)&m_megaTileData[i], 1, megaLen * sizeof(MegaTileStep), file);
				}
			}
		}
	}
}

uint8 TileSet::ReverseDirection(sint32 dir)
{
	switch (dir) {
	case k_MEGATILE_DIRECTION_X	: return k_MEGATILE_DIRECTION_X;
	case k_MEGATILE_DIRECTION_N : return k_MEGATILE_DIRECTION_S;
	case k_MEGATILE_DIRECTION_E	: return k_MEGATILE_DIRECTION_W;
	case k_MEGATILE_DIRECTION_S	: return k_MEGATILE_DIRECTION_N;
	case k_MEGATILE_DIRECTION_W	: return k_MEGATILE_DIRECTION_E;
	default:
		Assert(FALSE);
		return (unsigned) -1;
	}

	return (unsigned) -1;
}

void TileSet::LoadMapIcons(void)
{
	MBCHAR		name[_MAX_PATH];
	MBCHAR		path[_MAX_PATH];
	uint16		width, height;
	uint32		len;
	Pixel16		*tga;
	Pixel16		*data;
	sint32		i;
 
	for (i=0; i<MAPICON_MAX; i++) {
		
		sprintf(name, "UPC%.3d.TGA", i+1);

		if (g_civPaths->FindFile(C3DIR_PICTURES, name, path, TRUE, FALSE) == NULL) {
			Pixel16 *image;
			RIMHeader *rhead;
			sprintf(path, "upc%.3d.rim", i+1);
			uint8 *buf = (uint8 *) g_ImageMapPF->getData(path, &len);
			if (buf == NULL) {
				c3errors_ErrorDialog("TileSet", "'%s not found in asset tree.", name);
				continue;
			}
			len -= sizeof(RIMHeader);
			rhead = (RIMHeader *)buf;
			width = rhead->width;
			height = rhead->height;
			image = (Pixel16 *)(buf + sizeof(RIMHeader));
			data = (Pixel16 *)tileutils_EncodeTile16(image, width, height, &len, rhead->pitch);
			if (data) {
				m_mapIcons[i] = data;
				POINT pt = {width, height};
				m_mapIconDimensions[i] = pt;

				tileutils_ConvertPixelFormatFrom555(data);
			}
			continue;
		}

            
		tga = tileutils_TGA2mem(path, &width, &height);
		if (tga) {
			data = (Pixel16 *)tileutils_EncodeTile16(tga, width, height, &len);
			delete[] tga;
			tga = NULL;

			if (data) {
				
				tileutils_ConvertPixelFormatFrom555(data);

				m_mapIcons[i] = data;
				POINT pt = {width, height};
				m_mapIconDimensions[i] = pt;
			}
		}

	}
}


void TileSet::Load(void)
{
	MBCHAR		filename[_MAX_PATH];
	FILE		*file;
	
	if (g_is565Format)
		sprintf(filename, "gtset565.til");
	else
		sprintf(filename, "gtset555.til");







	file = c3files_fopen(C3DIR_TILES, filename, "rb");

	if (file) 
	{
		LoadTransforms(file);
		
		LoadTransitions(file);
		
		LoadBaseTiles(file);

		LoadRiverTransforms(file);

		LoadImprovements(file);

		LoadMegaTiles(file);

		c3files_fclose(file);

		LoadMapIcons();

		m_quick = FALSE;
	}
}

void TileSet::QuickLoadTransforms(uint8 **dataPtr)
{
	MBCHAR		ageChar = 'f';
	uint16		numTransforms;
	sint32		i;

	if (dataPtr) {
		numTransforms = *(uint16 *)(*dataPtr);
		(*dataPtr) += sizeof(uint16);
		
		m_transforms = new sint16*[numTransforms];
		m_numTransforms = numTransforms;

		for (i=0; i<numTransforms; i++) {
			m_transforms[i] = (sint16 *)(*dataPtr);
			(*dataPtr) += sizeof(sint16)*k_TRANSFORM_SIZE;
		}
	}
}

void TileSet::QuickLoadTransitions(uint8 **dataPtr)
{
	sint32		i,k;
	uint32		transitionCount = 0;
	uint32		transitionSize = 0;
	Pixel16		*xData;

	transitionCount = *(uint32 *)(*dataPtr);
	(*dataPtr) += sizeof(uint32);

	transitionSize = *(uint32 *)(*dataPtr);
	(*dataPtr) += sizeof(uint32);

	sint16	from, to;
	for (i=0; i<(sint32)transitionCount; i++) {
		from = *(sint16 *)(*dataPtr);
		(*dataPtr) += sizeof(sint16);

		to = *(sint16 *)(*dataPtr);
		(*dataPtr) += sizeof(sint16);

		for(k=0; k<k_TRANSITIONS_PER_TILE; k++) {
			xData = (Pixel16 *)(*dataPtr);
			(*dataPtr) += transitionSize;








			m_transitions[from][to][k] = xData;
		}
	}
}

void TileSet::QuickLoadBaseTiles(uint8 **dataPtr)
{
	uint16			i;
	BaseTile		*baseTile;
	uint32			baseTileCount;

	baseTileCount = *(uint32 *)(*dataPtr);
	(*dataPtr) += sizeof(uint32);

	for (i=0; i<baseTileCount; i++) {
		baseTile = new BaseTile();

		baseTile->QuickRead(dataPtr, m_mapped);

		m_baseTiles[baseTile->GetTileNum()] = baseTile;
	}
}

void TileSet::QuickLoadRiverTransforms(uint8 **dataPtr)
{
	uint16		numRiverTransforms;
	sint32		i;
	uint32		len;
	Pixel16		*riverData;

	numRiverTransforms = *(uint16 *)(*dataPtr);
	(*dataPtr) += sizeof(uint16);
	
	if (numRiverTransforms > 0) {
		m_riverTransforms = new sint16*[numRiverTransforms];
		m_numRiverTransforms = numRiverTransforms;
		m_riverData = new Pixel16*[numRiverTransforms];

		for (i=0; i<numRiverTransforms; i++) {				
			
			m_riverTransforms[i] = (sint16 *)(*dataPtr);
			(*dataPtr) += (sizeof(sint16)*k_RIVER_TRANSFORM_SIZE);

			
			len = *(uint32 *)(*dataPtr);
			(*dataPtr) += sizeof(uint32);

			if (len > 0) {
				riverData = (Pixel16 *)(*dataPtr);
				(*dataPtr) += len;

				if (!m_mapped) {
					

				}

				m_riverData[i] = riverData;
			} else {
				m_riverData[i] = NULL;
			}
		}
	}
}

void TileSet::QuickLoadImprovements(uint8 **dataPtr)
{
	uint16		numImprovements;
	sint32		i;
	uint32		len;

	numImprovements = *(uint16 *)(*dataPtr);
	(*dataPtr) += sizeof(numImprovements);


	
	if (numImprovements > 0) {
		for (i=0; i<numImprovements; i++) {
			uint16		impNum;
			Pixel16		*impData;

			
			impNum = *(uint16 *)(*dataPtr);
			(*dataPtr) += sizeof(uint16);

			
			len = *(uint32 *)(*dataPtr);
			(*dataPtr) += sizeof(uint32);

			if (len > 0) {
			
				impData = (Pixel16 *)(*dataPtr);
				(*dataPtr) += len;

				if (!m_mapped) {
					

				}
				m_improvementData[impNum] = impData;
			} else {
				m_improvementData[impNum] = NULL;
			}
		}
	}
}

void TileSet::QuickLoadMegaTiles(uint8 **dataPtr)
{
	sint32		i;

	m_numMegaTiles = *(uint16 *)(*dataPtr);
	(*dataPtr) += sizeof(m_numMegaTiles);

	if (m_numMegaTiles > 0) {
		for (i=0; i<m_numMegaTiles; i++) {
			uint16			megaLen;

			megaLen = *(uint16 *)(*dataPtr);
			(*dataPtr) += sizeof(megaLen);

			m_megaTileLengths[i] = megaLen;

			if (megaLen > 0) {
				memcpy(m_megaTileData[i], (MegaTileStep *)(*dataPtr), sizeof(MegaTileStep) * megaLen);
				
				(*dataPtr) += (megaLen * sizeof(MegaTileStep));
			}
		}
	}
}

void TileSet::QuickLoad(void)
{
	MBCHAR		filename[_MAX_PATH];
	FILE		*file;
	fpos_t		pos;
	size_t		fileSize;
	uint8		*dataPtr;
	size_t		count;

	if (g_is565Format)
		sprintf(filename, "gtset565.til");
	else
		sprintf(filename, "gtset555.til");


	file = c3files_fopen(C3DIR_TILES, filename, "rb");
	Assert(file != NULL);
	if (file) {
		if (m_tileSetData == NULL) {
			
			if (c3files_fseek(file, 0, SEEK_END)) goto Error;
			
			
			if (c3files_fgetpos(file, &pos)) goto Error;

#ifndef LINUX
			fileSize = (uint32)pos;
#else
			fileSize = pos.__pos;
#endif

			
			if (c3files_fseek(file, 0, SEEK_SET)) goto Error;
			
			m_tileSetData = new uint8[fileSize];
		} else {
			
			Cleanup();
		}

		count = c3files_fread((void *)m_tileSetData, 1, fileSize, file);
		if (count != (size_t)fileSize) goto Error;
		dataPtr = m_tileSetData;

		c3files_fclose(file);
	}

	
	QuickLoadTransforms(&dataPtr);
	QuickLoadTransitions(&dataPtr);
	QuickLoadBaseTiles(&dataPtr);
	
	QuickLoadRiverTransforms(&dataPtr);
	QuickLoadImprovements(&dataPtr);
	QuickLoadMegaTiles(&dataPtr);

    
	LoadMapIcons();

	m_quick = TRUE;

	return;

Error:
	
	if (file != NULL)
		fclose(file);

	if (m_tileSetData != NULL)
		delete[] m_tileSetData;

	c3errors_FatalDialog("Tile Set", "Unable to load tileset.");
}

void TileSet::QuickLoadMapped(void)
{
	MBCHAR		filename[_MAX_PATH],
			path[_MAX_PATH];
	uint8		*dataPtr;

	if (g_is565Format)
		sprintf(filename, "gtset565.til");
	else
		sprintf(filename, "gtset555.til");

	

	g_civPaths->FindFile(C3DIR_TILES, filename, path);
#ifdef WIN32
	m_fileHandle = CreateFile(path, 
						GENERIC_READ,
						FILE_SHARE_READ,
						NULL,
						OPEN_EXISTING,
						FILE_ATTRIBUTE_NORMAL,
						NULL);
	if (m_fileHandle == INVALID_HANDLE_VALUE) {
		c3errors_FatalDialog("Tile Set", "Unable to load tileset.");
		return;
	}

	sint32	size;

	size = GetFileSize(m_fileHandle, NULL);
	if (size <= 0) {
		c3errors_FatalDialog("Tile Set", "Unable to load tileset.");
		return;
	}

	m_mappedFileHandle = CreateFileMapping(m_fileHandle, 
						NULL,
						PAGE_READONLY,
						0,
						0,
						NULL);

	if (m_mappedFileHandle == INVALID_HANDLE_VALUE) {
		CloseHandle(m_fileHandle);
		c3errors_FatalDialog("Tile Set", "Unable to load tileset.");
		return;
	}

	m_tileSetData = (uint8 *)MapViewOfFile(m_mappedFileHandle,
						FILE_MAP_READ,
						0,
						0,
						0);
#else
	struct stat st;
	int rc = stat(filename, &st);
	if (0 != rc) {
		c3errors_FatalDialog("Tile Set", "Unable to load tileset.");
		return;
	}
	m_MMapSize = st.st_size;
	m_fd = open(filename, O_RDONLY);
	if (m_fd < 0) {
		c3errors_FatalDialog("Tile Set", "Unable to load tileset.");
		return;
	}
	m_tileSetData = (uint8 *)mmap(0, m_MMapSize, PROT_READ, MAP_PRIVATE, m_fd, 0);
#endif

	if (m_tileSetData == NULL) {
#ifdef WIN32
		CloseHandle(m_fileHandle);
		CloseHandle(m_mappedFileHandle);
#else
		close(m_fd);
#endif
		c3errors_FatalDialog("Tile Set", "Unable to load tileset.");
		return;
	}

	m_mapped = TRUE;

	dataPtr = m_tileSetData;

	
	QuickLoadTransforms(&dataPtr);
	QuickLoadTransitions(&dataPtr);
	QuickLoadBaseTiles(&dataPtr);
	
	QuickLoadRiverTransforms(&dataPtr);
	QuickLoadImprovements(&dataPtr);
	QuickLoadMegaTiles(&dataPtr);

	LoadMapIcons();

	m_quick = TRUE;

	return;
}
