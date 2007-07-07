//----------------------------------------------------------------------------//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : Base Tile
// Id           : $Id:$
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

#include "c3.h"

#include "c3files.h"

#include "pixelutils.h"
#include "tileutils.h"
#include "TileInfo.h"
#include "tileset.h"
#include "BaseTile.h"

BaseTile::BaseTile()
:
    m_flags         (0),
    m_baseType      (0),
    m_tileNum       (0),
    m_tileDataLen   (0),
    m_hatDataLen    (0),
    m_tileData      (NULL),
    m_hatData       (NULL)
{
}

BaseTile::~BaseTile()
{
	if (WasQuickLoaded())
	{
		// Do nothing
	}
	else
	{
		if (m_tileData)
			delete[] m_tileData;

		if (m_hatData)
			delete[] m_hatData;
	}
}

BOOL BaseTile::Read(FILE *file)
{
	Assert(file != NULL);
	if (file == NULL) return FALSE;

	c3files_fread((void *)&m_tileNum    , 1, sizeof(m_tileNum), file);
	c3files_fread((void *)&m_baseType   , 1, sizeof(m_baseType), file);

	c3files_fread((void *)&m_flags      , 1, sizeof(m_flags), file);
	c3files_fread((void *)&m_tileDataLen, 1, sizeof(m_tileDataLen), file);

	m_tileData = new Pixel16[m_tileDataLen/2];
	c3files_fread((void *)m_tileData    , 1, m_tileDataLen, file);

	uint16	size;
	c3files_fread((void *)&size         , 1, sizeof(size), file);

	Pixel16		*hatData;
	if (size > 0)
	{
		hatData = new Pixel16[size/2];
		c3files_fread((void *)hatData, 1, size, file);
	}
	else
	{
		hatData = NULL;
	}

	m_hatDataLen = size;
	m_hatData    = hatData;

	return TRUE;
}

BOOL BaseTile::QuickRead(uint8 **dataPtr, BOOL mapped)
{
	m_tileNum      = *(uint16 *)(*dataPtr);
	(*dataPtr)    += sizeof(uint16);

	m_baseType     = *(uint8 *)(*dataPtr);
	(*dataPtr)    += sizeof(uint8);

	m_flags        = *(uint8 *)(*dataPtr);
	(*dataPtr)    += sizeof(uint8);

	m_tileDataLen  = *(uint16 *)(*dataPtr);
	(*dataPtr)    += sizeof(uint16);

	m_tileData     = (Pixel16 *)(*dataPtr);
	(*dataPtr)    += (m_tileDataLen);

	uint16 size    = *(uint16 *)(*dataPtr);
	(*dataPtr)    += sizeof(uint16);

	Pixel16		*hatData;
	if (size > 0)
	{
		hatData = (Pixel16 *)(*dataPtr);
		(*dataPtr) += size;
	}
	else
	{
		hatData = NULL;
	}

	m_hatDataLen = size;
	m_hatData    = hatData;

	m_flags |= k_BTF_QUICKLOADED;

	return TRUE;
}


