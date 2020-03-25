//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
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
//
//----------------------------------------------------------------------------

#include "c3.h"
#include "tileset.h"

#include "World.h"

#include "c3errors.h"

#include "pixelutils.h"
#include "tileutils.h"
#include "tiffutils.h"
#include "BaseTile.h"

#include "tiledmap.h"   // g_tiledMap

#include "CivPaths.h"   // g_civPaths
#include "rimutils.h"
#include "MapIconRecord.h"

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

extern ProjectFile *    g_ImageMapPF;
extern sint32		g_is565Format;

namespace
{
    uint8 const     DIRECTION_INVALID   = static_cast<uint8>(-1);   // 255

    char const      TILESETFILE_555[]   = "gtset555.til";
    char const      TILESETFILE_565[]   = "gtset565.til";

    char const *    TileSetFile(void)
    {
        return (g_is565Format) ? TILESETFILE_565 : TILESETFILE_555;
    }
}

TileSet::TileSet()
:
	m_numTransforms         (0),
	m_transforms            (NULL),
    m_numRiverTransforms    (0),
	m_riverTransforms       (NULL),
	m_riverData             (NULL),
	m_tileSetData           (NULL),
	m_numMegaTiles          (0),
    m_quick                 (false),
	m_mapped                (false),
	m_mapIcons              (NULL),
	m_mapIconDimensions     (NULL),
#ifdef WIN32
	m_fileHandle            (INVALID_HANDLE_VALUE),
	m_mappedFileHandle      (INVALID_HANDLE_VALUE)
#else
	m_fd                    (-1),
	m_MMapSize              (0)
#endif
{
	sint32		i,j,k;

	// Data from the tile file
	for (i=0; i<TERRAIN_MAX; i++) {
		for (j=0; j<TERRAIN_MAX; j++) {
			for(k=0; k<k_TRANSITIONS_PER_TILE; k++) {
				m_transitions[i][j][k] = NULL;
			}
		}
	}

	// Data from the tile file
	for (i=0; i<k_MAX_BASE_TILES; i++) {
		m_baseTiles[i] = NULL;
	}

	// Data from the tile file
	for (i=0; i<k_MAX_IMPROVEMENTS; i++) {
		m_improvementData[i] = NULL;
	}

	// Data from the tile file
	for (i=0; i<k_MAX_MEGATILES; i++) {
		for (j=0; j<k_MAX_MEGATILE_STEPS; j++) {
			m_megaTileData[i][j].direction = 0;
			m_megaTileData[i][j].terrainType = 0;
			m_megaTileData[i][j].tileNum = 0;
		}
		m_megaTileLengths[i] = 0;
	}
}

TileSet::~TileSet()
{
	Cleanup();
}

// ToDo: Merge the comon parts of CleanupQuick and CleanupMapped

void TileSet::CleanupQuick(void)
{
	delete[] m_transforms;
	m_transforms = NULL;
	m_numTransforms = 0;

	delete[] m_riverTransforms;
	m_riverTransforms = NULL;

	delete[] m_riverData;
	m_riverData = NULL;

	sint32 i;

	for (i=0; i<k_MAX_BASE_TILES; i++)
	{
		delete m_baseTiles[i];
		m_baseTiles[i] = NULL;
	}

	for (i=0; i < g_theMapIconDB->NumRecords(); i++)
	{
		delete m_mapIcons[i];
	}
	delete[] m_mapIcons;
	m_mapIcons = NULL;
	delete[] m_mapIconDimensions;
	m_mapIconDimensions = NULL;

	delete[] m_tileSetData;
	m_tileSetData = NULL;
}

void TileSet::CleanupMapped(void)
{
	delete[] m_transforms;
	m_transforms = NULL;
	m_numTransforms = 0;

	delete[] m_riverTransforms;
	m_riverTransforms = NULL;

	delete[] m_riverData;
	m_riverData = NULL;

	sint32 i;

	for (i=0; i<k_MAX_BASE_TILES; i++)
	{
		delete m_baseTiles[i];
		m_baseTiles[i] = NULL;
	}

	for (i=0; i < g_theMapIconDB->NumRecords(); i++)
	{
		delete m_mapIcons[i];
	}
	delete[] m_mapIcons;
	m_mapIcons = NULL;
	delete[] m_mapIconDimensions;
	m_mapIconDimensions = NULL;

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
	if (m_mapped)
    {
		CleanupMapped();
    }
	else if (m_quick)
    {
		CleanupQuick();
    }
	else
    {
	    sint32		i,j,k;

		if (m_transforms)
        {
			for (i = 0; i < m_numTransforms; i++)
            {
				delete [] m_transforms[i];
			}
			delete [] m_transforms;
			m_transforms    = NULL;
			m_numTransforms = 0;
		}


		if (m_riverTransforms)
        {
			for (i = 0; i<m_numRiverTransforms; i++)
            {
				delete [] m_riverTransforms[i];
				delete [] m_riverData[i];
			}

			delete [] m_riverTransforms;
			m_riverTransforms = NULL;
			m_numRiverTransforms = 0;

			delete [] m_riverData;
			m_riverData = NULL;
		}

		for (i=0; i<TERRAIN_MAX; i++) {
			for (j=0; j<TERRAIN_MAX; j++) {
				if (m_transitions[i][j][0]) {
					for(k=0; k < k_TRANSITIONS_PER_TILE; k++) {
						delete [] m_transitions[i][j][k];
						m_transitions[i][j][k] = NULL;
					}
				}
			}
		}

		for (i=0; i<k_MAX_BASE_TILES; i++)
        {
			delete m_baseTiles[i];
			m_baseTiles[i] = NULL;
		}
	}
}

void TileSet::LoadBaseTiles(FILE *file)
{
	uint32			baseTileCount;
	c3files_fread((void *)&baseTileCount, 1, sizeof(baseTileCount), file);

	for (uint32 i = 0; i < baseTileCount; ++i)
	{
		BaseTile *  baseTile = new BaseTile();
		baseTile->Read(file);
		m_baseTiles[baseTile->GetTileNum()] = baseTile;
	}
}


void TileSet::LoadTransitions(FILE *file)
{
	uint32		transitionCount = 0;
	uint32		transitionSize = 0;
	size_t		count;
    uint32      i;

	count = c3files_fread((void *)&transitionCount, 1, sizeof(transitionCount), file);
	if (count != sizeof(transitionCount)) goto Error;

	count = c3files_fread((void *)&transitionSize, 1, sizeof(transitionSize), file);
	if (count != sizeof(transitionSize)) goto Error;

	sint16	from, to;

	for (i = 0; i < transitionCount; ++i)
    {
		count = c3files_fread((void *)&from, 1, sizeof(from), file);
		if (count != sizeof(from)) goto Error;
		count = c3files_fread((void *)&to, 1, sizeof(to), file);
		if (count != sizeof(to)) goto Error;

		for (size_t k = 0; k < k_TRANSITIONS_PER_TILE; ++k)
        {
	        Pixel16	* xData = new Pixel16[transitionSize/2];
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
	if (file)
    {
	    uint16		numTransforms;
		c3files_fread((void *)&numTransforms, 1, sizeof(numTransforms), file);

		m_transforms    = new sint16*[numTransforms];
		m_numTransforms = numTransforms;

		for (uint16 i = 0; i < numTransforms; ++i)
        {
	        sint16 * transform = new sint16[k_TRANSFORM_SIZE];
			c3files_fread((void *)transform, 1, sizeof(sint16)*k_TRANSFORM_SIZE, file);
			m_transforms[i] = transform;
		}
	}
}

void TileSet::LoadRiverTransforms(FILE *file)
{
	if (file)
    {
	    uint16		numRiverTransforms;
		c3files_fread((void *)&numRiverTransforms, 1, sizeof(numRiverTransforms), file);

		if (numRiverTransforms > 0) {
			m_riverTransforms = new sint16*[numRiverTransforms];
			m_numRiverTransforms = numRiverTransforms;
			m_riverData = new Pixel16*[numRiverTransforms];

			for (uint16 i = 0; i < numRiverTransforms; ++i)
            {
	            sint16 *    transform = new sint16[k_RIVER_TRANSFORM_SIZE];
				c3files_fread((void *)transform, 1, sizeof(sint16)*k_RIVER_TRANSFORM_SIZE, file);
				m_riverTransforms[i] = transform;

	            uint32		len = 0;
				c3files_fread((void *)&len, 1, sizeof(uint32), file);

				if (len > 0)
                {
	                Pixel16	* riverData = new Pixel16[len/2];
					c3files_fread((void *)riverData, 1, len, file);
					m_riverData[i] = riverData;
				}
                else
                {
					m_riverData[i] = NULL;
				}
			}
		}
	}
}

void TileSet::LoadImprovements(FILE *file)
{
	if (file)
    {
	    uint16		numImprovements;
		c3files_fread((void *)&numImprovements, 1, sizeof(numImprovements), file);

		for (uint16 i = 0; i < numImprovements; ++i)
        {
			uint16		impNum;
			c3files_fread((void *)&impNum, 1, sizeof(uint16), file);

	        uint32		len;
			c3files_fread((void *)&len, 1, sizeof(uint32), file);

			if (len > 0)
            {
				Pixel16	*   impData = new Pixel16[len/2];
				c3files_fread((void *)impData, 1, len, file);
				m_improvementData[impNum] = impData;
			}
            else
            {
				m_improvementData[impNum] = NULL;
			}
		}
	}
}

void TileSet::LoadMegaTiles(FILE *file)
{
	if (file)
    {
		c3files_fread((void *)&m_numMegaTiles, 1, sizeof(m_numMegaTiles), file);

		for (uint16 i = 0; i < m_numMegaTiles; ++i)
        {
			uint16			megaLen;
			c3files_fread((void *)&megaLen, 1, sizeof(uint16), file);
			m_megaTileLengths[i] = megaLen;

			if (megaLen > 0)
            {
				c3files_fread((void *)&m_megaTileData[i], 1, megaLen * sizeof(MegaTileStep), file);
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
		break;
	}

	return DIRECTION_INVALID;
}

void TileSet::LoadMapIcons(void)
{
	MBCHAR		name[_MAX_PATH];
	MBCHAR		path[_MAX_PATH];
	uint16		width, height;
	uint32		len;
	Pixel16		*tga;
	Pixel16		*data;

	m_mapIcons          = new Pixel16*[g_theMapIconDB->NumRecords()];
	m_mapIconDimensions = new POINT[g_theMapIconDB->NumRecords()];
	for (sint32 i = 0; i < g_theMapIconDB->NumRecords(); ++i)
	{

		sprintf(name, g_theMapIconDB->Get(i)->GetValue());

		if (g_civPaths->FindFile(C3DIR_PICTURES, name, path, TRUE, FALSE) == NULL) {

			sprintf(path, "%s", name);
			char * lastDot = strrchr(path, '.');
			if (lastDot)
			{
				++lastDot;
				sprintf(lastDot, "rim");
			}
			else
			{
				sprintf(path, "%s.rim", path);
			}

			size_t  testlen = 0;
			uint8 * buf = reinterpret_cast<uint8 *>(g_ImageMapPF->getData(path, testlen));
			len = testlen;
			if (buf == NULL) {
				c3errors_ErrorDialog("TileSet", "'%s not found in asset tree.", name);
				continue;
			}
			len -= sizeof(RIMHeader);
			RIMHeader * rhead = (RIMHeader *)buf;
			width = rhead->width;
			height = rhead->height;
			Pixel16 *   image = (Pixel16 *)(buf + sizeof(RIMHeader));
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

/*
Pixel16 TileSet::ConvertMapIcons(const MBCHAR *name)  //EMOD
{
	//MBCHAR		name[_MAX_PATH];
	MBCHAR		path[_MAX_PATH];
	uint16		width, height;
	uint32		len;
	Pixel16		*tga;
	Pixel16		*data;

	//for (int i = 0; i < MAPICON_MAX; ++i)
    //{
	//	sprintf(name, "UPC%.3d.TGA", i+1);

		if (g_civPaths->FindFile(C3DIR_PICTURES, name, path, TRUE, FALSE) == NULL) {
			//sprintf(path, "upc%.3d.rim", i+1);
            size_t  testlen = 0;
			uint8 * buf = reinterpret_cast<uint8 *>(g_ImageMapPF->getData(path, testlen));
            len = testlen;
			if (buf == NULL) {
				c3errors_ErrorDialog("TileSet", "'%s not found in asset tree.", name);
				//continue;
			}
			len -= sizeof(RIMHeader);
			RIMHeader * rhead = (RIMHeader *)buf;
			width = rhead->width;
			height = rhead->height;
			Pixel16 *   image = (Pixel16 *)(buf + sizeof(RIMHeader));
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
	//}
}
*/

void TileSet::Load(void)
{
	FILE *  file = c3files_fopen(C3DIR_TILES, TileSetFile(), "rb");

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
	if (dataPtr) {
	    m_numTransforms = *(uint16 *)(*dataPtr);
		(*dataPtr) += sizeof(uint16);

		m_transforms = new sint16*[m_numTransforms];

		for (uint16 i = 0; i < m_numTransforms; ++i)
        {
			m_transforms[i] = (sint16 *)(*dataPtr);
			(*dataPtr) += sizeof(sint16)*k_TRANSFORM_SIZE;
		}
	}
}

void TileSet::QuickLoadTransitions(uint8 **dataPtr)
{

	uint32		transitionCount = *(uint32 *)(*dataPtr);
	(*dataPtr) += sizeof(uint32);

	uint32		transitionSize  = *(uint32 *)(*dataPtr);
	(*dataPtr) += sizeof(uint32);

	for (uint32 i = 0; i < transitionCount; ++i)
    {
		sint16 from = *(sint16 *)(*dataPtr);
		(*dataPtr) += sizeof(sint16);

		sint16 to = *(sint16 *)(*dataPtr);
		(*dataPtr) += sizeof(sint16);

		for (size_t k = 0; k < k_TRANSITIONS_PER_TILE; ++k)
        {
			m_transitions[from][to][k] = (Pixel16 *)(*dataPtr);
			(*dataPtr) += transitionSize;
		}
	}
}

void TileSet::QuickLoadBaseTiles(uint8 **dataPtr)
{
	uint32			baseTileCount = *(uint32 *)(*dataPtr);
	(*dataPtr) += sizeof(uint32);

	for (uint32 i = 0; i < baseTileCount; ++i)
    {
	    BaseTile * baseTile = new BaseTile();
		baseTile->QuickRead(dataPtr, m_mapped);

		m_baseTiles[baseTile->GetTileNum()] = baseTile;
	}
}

void TileSet::QuickLoadRiverTransforms(uint8 **dataPtr)
{
	uint16		numRiverTransforms = *(uint16 *)(*dataPtr);;
	(*dataPtr) += sizeof(uint16);

	if (numRiverTransforms > 0)
    {
		m_riverTransforms = new sint16*[numRiverTransforms];
		m_numRiverTransforms = numRiverTransforms;
		m_riverData = new Pixel16*[numRiverTransforms];

		for (uint16 i = 0; i < numRiverTransforms; ++i)
        {
			m_riverTransforms[i] = (sint16 *)(*dataPtr);
			(*dataPtr) += (sizeof(sint16)*k_RIVER_TRANSFORM_SIZE);

			uint32 len = *(uint32 *)(*dataPtr);
			(*dataPtr) += sizeof(uint32);

			if (len > 0)
            {
				m_riverData[i] = (Pixel16 *)(*dataPtr);
				(*dataPtr) += len;
			}
            else
            {
				m_riverData[i] = NULL;
			}
		}
	}
}

void TileSet::QuickLoadImprovements(uint8 **dataPtr)
{
	uint16		numImprovements = *(uint16 *)(*dataPtr);
	(*dataPtr) += sizeof(numImprovements);

	for (uint16 i = 0; i < numImprovements; ++i)
    {
		uint16 impNum = *(uint16 *)(*dataPtr);
		(*dataPtr) += sizeof(uint16);

		uint32 len = *(uint32 *)(*dataPtr);
		(*dataPtr) += sizeof(uint32);

		if (len > 0)
        {
			m_improvementData[impNum] = (Pixel16 *)(*dataPtr);
			(*dataPtr) += len;
		}
        else
        {
			m_improvementData[impNum] = NULL;
		}
	}
}

void TileSet::QuickLoadMegaTiles(uint8 **dataPtr)
{
	m_numMegaTiles = *(uint16 *)(*dataPtr);
	(*dataPtr) += sizeof(m_numMegaTiles);

	for (uint16 i = 0; i < m_numMegaTiles; ++i)
    {
		uint16	megaLen = *(uint16 *)(*dataPtr);
		(*dataPtr) += sizeof(megaLen);

		m_megaTileLengths[i] = megaLen;

		if (megaLen > 0)
        {
			memcpy(m_megaTileData[i], (MegaTileStep *)(*dataPtr), sizeof(MegaTileStep) * megaLen);
			(*dataPtr) += (megaLen * sizeof(MegaTileStep));
		}
	}
}

void TileSet::QuickLoad(void)
{
	FILE *  file = c3files_fopen(C3DIR_TILES, TileSetFile(), "rb");
	Assert(file != NULL);
	if (file)
    {
	    size_t	fileSize = 0;

		if (m_tileSetData == NULL)
        {
			if (c3files_fseek(file, 0, SEEK_END)) goto Error;

            fpos_t	pos;
			if (c3files_fgetpos(file, &pos)) goto Error;

#ifndef LINUX
			fileSize = (uint32)pos;
#else
			fileSize = pos.__pos;
#endif

			if (c3files_fseek(file, 0, SEEK_SET)) goto Error;

			m_tileSetData = new uint8[fileSize];
		}
        else
        {
			Cleanup();
		}

		size_t const count =
            c3files_fread((void *)m_tileSetData, 1, fileSize, file);
		if (count != (size_t)fileSize) goto Error;

		c3files_fclose(file);
	}

    {
	    uint8 * dataPtr = reinterpret_cast<uint8 *>(m_tileSetData);

	    QuickLoadTransforms(&dataPtr);
	    QuickLoadTransitions(&dataPtr);
	    QuickLoadBaseTiles(&dataPtr);
	    QuickLoadRiverTransforms(&dataPtr);
	    QuickLoadImprovements(&dataPtr);
	    QuickLoadMegaTiles(&dataPtr);
    }

	LoadMapIcons();
	m_quick = TRUE;

	return;

Error:
	if (file != NULL)
		fclose(file);

	delete [] m_tileSetData;
    m_tileSetData = NULL;

	c3errors_FatalDialog("Tile Set", "Unable to load tileset.");
}

void TileSet::QuickLoadMapped(void)
{
	MBCHAR  path[_MAX_PATH];
	g_civPaths->FindFile(C3DIR_TILES, TileSetFile(), path);

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

	sint32	size = GetFileSize(m_fileHandle, NULL);
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
	int rc = stat(path, &st);
	if (rc == -1) {
		c3errors_FatalDialog("Tile Set", "Unable to load tileset.");
		return;
	}
	m_MMapSize = st.st_size;
	m_fd = open(path, O_RDONLY);
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

	uint8 * dataPtr = m_tileSetData;

	QuickLoadTransforms(&dataPtr);
	QuickLoadTransitions(&dataPtr);
	QuickLoadBaseTiles(&dataPtr);

	QuickLoadRiverTransforms(&dataPtr);
	QuickLoadImprovements(&dataPtr);
	QuickLoadMegaTiles(&dataPtr);

	LoadMapIcons();

	m_quick = TRUE;
}
