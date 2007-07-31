
#pragma once
#ifndef __MAPFILE_H__
#define __MAPFILE_H__

class CivArchive;
template <class T> class CTPDatabase;
class UnitRecord;
class TerrainImprovementRecord;

#include "CivArchive.h"
#include "StrDB.h"


class MapFileChunk {
public:
	uint32 m_id;
	sint32 m_size;

	bool Save(FILE *outfile);
};

class MapFileCityData {
public:
	sint32 m_size;
	uint64 m_improvements;
	uint64 m_wonders;
	uint8 m_owner;
	
	MBCHAR m_name[k_MAX_NAME_LEN];
	void Serialize(CivArchive &archive) {
		sint32 len;

		if(archive.IsStoring()) {
			archive << m_size;
			archive << m_improvements;
			archive << m_wonders;
			archive << m_owner;
			
			len = strlen(m_name) + 1;
			archive << len;
			archive.Store((uint8*)m_name, len * sizeof(MBCHAR));
		} else {
			archive >> m_size;
			archive >> m_improvements;
			archive >> m_wonders;
			archive >> m_owner;
			
			archive >> len;
			archive.Load((uint8*)m_name, len * sizeof(MBCHAR));
		}
	}
};

class MapFile {
public:
	MapFile();
	~MapFile();

	bool Load(const MBCHAR *filename);
	bool Save(const MBCHAR *filename);

	bool SaveMap(FILE *outfile);

	template <class T> bool SaveDBNames(FILE *outfile, uint32 chunkId, CTPDatabase<T> *theDB)
	{
		
		
		
		CivArchive archive;
		archive.SetStore();
		sint32 i;
		archive.PutSINT32(theDB->NumRecords());

		for(i = 0; i < theDB->NumRecords(); i++) {
			const char *idStr;
			if(theDB->Get(i)->GetName() < 0) {
				idStr = theDB->Get(i)->GetNameText();
			} else {
				idStr = g_theStringDB->GetIdStr(theDB->Get(i)->GetName());
			}
			Assert(idStr);
			if(idStr) {
				sint16 len = strlen(idStr);
				Assert(len > 0);
				archive << len;
				archive.Store((uint8*)idStr, len);
			}
		}
		
		m_chunk.m_id = chunkId;
		m_chunk.m_size = archive.StreamLen();
		
		if(!m_chunk.Save(outfile))
			return false;
		
		if(fwrite(archive.GetStream(), 1, archive.StreamLen(), outfile) != archive.StreamLen())
			return false;
		
		return true;
	}		

	bool SaveTerrain(FILE *);
	bool SaveTerrainEnv(FILE *);
	bool SaveCities(FILE *);
	bool SaveUnits(FILE *);
	bool SaveImprovements(FILE *);
	bool SaveVision(FILE *file);
	bool SaveAdvances(FILE *file);
	
	bool SaveHuts(FILE *file);
	bool SaveCivilizations(FILE *file);

	bool LoadMap(FILE *infile);

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

private:
	MapFileChunk m_chunk;

	sint32 *m_unitTypeMap;
	sint32 *m_improvementTypeMap;
	sint32 *m_advanceTypeMap;
};

#endif
