//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ header
// Description  : Map file handling
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
// Modifications from the original Activision code:
// - Added comments, updated dependencies, made items private
//
//----------------------------------------------------------------------------
//
/// \file   gs/utility/MapFile.h
/// \brief  Map file handling (declarations)

#ifdef HAVE_PRAGMA_ONCE
#pragma once
#endif

#ifndef __MAPFILE_H__
#define __MAPFILE_H__

//----------------------------------------------------------------------------
// Library dependencies
//----------------------------------------------------------------------------

#include <cstdio>       // FILE
#include <cstring>      // strlen

//----------------------------------------------------------------------------
// Export overview
//----------------------------------------------------------------------------

class MapFile;

//----------------------------------------------------------------------------
// Project dependencies
//----------------------------------------------------------------------------

#include "c3types.h"        // MBCHAR, uintN, sintN
#include "civarchive.h"	    // CivArchive
#include "StrDB.h"          // g_theStringDB

template <class T> class CTPDatabase;

//----------------------------------------------------------------------------
// Class declarations
//----------------------------------------------------------------------------

class MapFile
{
public:
	MapFile();
	~MapFile();

	bool Load(const MBCHAR * filename);
	bool Save(const MBCHAR * filename);

private:
    /// Common part for all information blocks: type and size
    class Chunk
    {
    public:
	    uint32 m_id;
	    uint32 m_size;

	    bool Save(FILE * outfile);
    };

	Chunk       m_chunk;

	sint32 *    m_unitTypeMap;
	sint32 *    m_improvementTypeMap;
	sint32 *    m_advanceTypeMap;

	bool LoadMap(FILE * infile);
	bool LoadTerrain(uint8 *buf, sint32 size);
	bool LoadTerrainEnv(uint8 *buf, sint32 size);
	bool LoadUnits(uint8 *buf, sint32 size);
	bool LoadUnitTypes(uint8 *buf, sint32 size);
	bool LoadCities(uint8 *buf, sint32 size);
	bool LoadOldCities(uint8 *buf, sint32 size);
	bool LoadImprovements(uint8 *buf, sint32 size);
	bool LoadImprovementTypes(uint8 *buf, sint32 size);
	bool LoadVision(uint8 *buf, sint32 size);
	bool LoadAdvanceNames(uint8 *buf, sint32 size);
	bool LoadAdvances(uint8 *buf, sint32 size);
	bool LoadHuts(uint8 *buf, sint32 size);
	bool LoadCivilizations(uint8 *buf, sint32 size);

	bool SaveMap(FILE * outfile);
	bool SaveTerrain(FILE *);
	bool SaveTerrainEnv(FILE *);
	bool SaveCities(FILE *);
	bool SaveUnits(FILE *);
	bool SaveImprovements(FILE *);
	bool SaveVision(FILE *file);
	bool SaveAdvances(FILE *file);
	bool SaveHuts(FILE *file);
	bool SaveCivilizations(FILE *file);

	template <class T> bool SaveDBNames(FILE *outfile, uint32 chunkId, CTPDatabase<T> *theDB)
	{
		CivArchive archive;
		archive.SetStore();
		archive.PutSINT32(theDB->NumRecords());

		for (sint32 i = 0; i < theDB->NumRecords(); i++)
        {
			const char *idStr;
			if(theDB->Get(i)->GetName() < 0) {
				idStr = theDB->Get(i)->GetNameText();
			} else {
				idStr = g_theStringDB->GetIdStr(theDB->Get(i)->GetName());
			}
			Assert(idStr);
			if(idStr) {
				uint16 len = static_cast<uint16>(strlen(idStr));
				Assert(len > 0);
				archive << len;
				archive.Store((uint8*)idStr, len);
			}
		}

		m_chunk.m_id = chunkId;
		m_chunk.m_size = archive.StreamLen();

		return m_chunk.Save(outfile) &&
               (fwrite(archive.GetStream(), 1, archive.StreamLen(), outfile) ==
                    archive.StreamLen()
               );
	}

};

#endif
