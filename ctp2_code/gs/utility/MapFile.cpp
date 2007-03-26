//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : Map file handling
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
//
// Modifications from the original Activision code:
//
// - Replaced non-standard sizeof(enum) occurrences
// - Corrected invalid index in LoadAdvances
// - Repaired memory leaks
//
//----------------------------------------------------------------------------
//
/// \file   gs/utility/MapFile.cpp
/// \brief  Map file handling (definitions)

#include "c3.h"
#include "MapFile.h"

#include "AdvanceRecord.h"              // g_theAdvanceDB
#include <algorithm>
#include "AICause.h"
#include "Cell.h"
#include "cellunitlist.h"
#include "civarchive.h"
#include "Civilisation.h"
#include "CTPDatabase.h"
#include "dbtypes.h"                    // k_MAX_NAME_LEN
#include "director.h"                   // g_director
#include "MessageBoxDialog.h"           // MessageBoxDialog
#include "net_util.h"                   // PULL/PUSH macros
#include "player.h"                     // g_player 
#include "profileDB.h"
#include "TerrainImprovementRecord.h"
#include "TerrImprove.h"
#include "tiledmap.h"
#include "UnitData.h"
#include "UnitRecord.h"
#include "unitutil.h"
#include "Vision.h"
#include "World.h"                      // g_theWorld

#define k_TERRAIN_HEADER 'TERR'
#define k_TERRAIN_ENV_HEADER 'TENV'
#define k_CITIES_HEADER 'CITY'
#define k_NEW_CITIES_HEADER 'NCTY'
#define k_UNIT_TYPES_HEADER 'UTYP'
#define k_UNITS_HEADER 'UNIT'
#define k_IMPROVEMENT_TYPES_HEADER 'ITYP'
#define k_IMPROVEMENTS_HEADER 'IMPS'
#define k_VISION_HEADER 'VISN'
#define k_ADVANCE_TYPES_HEADER 'ATYP'
#define k_PLAYER_ADVANCES_HEADER 'PADV'
#define k_HUTS_HEADER 'HUTS'
#define k_CIVS_HEADER 'CIVS'

namespace
{
size_t const    k_MAPFILE_NAME_LEN  = 32;
size_t const    k_CIVS_BLOCK_LENGTH = 
    k_MAX_PLAYERS * (sizeof(uint32) + k_MAPFILE_NAME_LEN);

class MapFileCityData 
{
public:
    explicit MapFileCityData(Unit city)
    :
        m_size              (city.PopCount()),
        m_improvements      (city.GetImprovements()),
        m_wonders           (city.GetData()->GetCityData()->GetBuiltWonders()),
        m_owner             (static_cast<uint8>(city.GetOwner()))
    {
		strncpy(m_name, city.GetName(), k_MAPFILE_NAME_LEN); 
    }

	void Serialize(CivArchive &archive) 
    {
		uint32 len;

		if (archive.IsStoring()) 
        {
			archive << m_size;
			archive << m_improvements;
			archive << m_wonders;
			archive << m_owner;
			
			len = strlen(m_name) + 1;
			archive << len;
			archive.Store((uint8*)m_name, len * sizeof(MBCHAR));
		} 
        else 
        {
			archive >> m_size;
			archive >> m_improvements;
			archive >> m_wonders;
			archive >> m_owner;
			
			archive >> len;
			archive.Load((uint8*)m_name, len * sizeof(MBCHAR));
		}
	}

private:
	sint32  m_size;
	uint64  m_improvements;
	uint64  m_wonders;
	uint8   m_owner;
	MBCHAR  m_name[k_MAX_NAME_LEN];
};

} // namespace

bool MapFile::Chunk::Save(FILE * outfile)
{
	bool res = true;
	if (fwrite((uint8*)&m_id, 1, sizeof(m_id), outfile) != sizeof(m_id))
		res = false;
	fflush(outfile);

	if (fwrite((uint8*)&m_size, 1, sizeof(m_size), outfile) != sizeof(m_size))
		res = false;
	fflush(outfile);

	return res;
}

MapFile::MapFile()
:
    m_chunk                 (),
    m_unitTypeMap           (NULL),
    m_improvementTypeMap    (NULL),
    m_advanceTypeMap        (NULL)
{
}

MapFile::~MapFile()
{
    delete [] m_unitTypeMap;
    delete [] m_improvementTypeMap;
    delete [] m_advanceTypeMap; 
}

bool MapFile::Load(const MBCHAR *filename)
{
	FILE *infile = fopen(filename, "rb");
	if(!infile)
		return false;

	bool res = LoadMap(infile);

	fclose(infile);

	return res;
}

bool MapFile::Save(const MBCHAR *filename)
{
	FILE *outfile = fopen(filename, "wb");
	if(!outfile)
		return false;

	bool res = SaveMap(outfile);

	fclose(outfile);

	return res;
}


bool MapFile::SaveMap(FILE *outfile)
{
	if(!SaveTerrain(outfile)) return false;
	if(!SaveVision(outfile)) return false;
	if(!SaveTerrainEnv(outfile)) return false;
	if(!SaveUnits(outfile)) return false;
	if(!SaveCities(outfile)) return false;
	if(!SaveImprovements(outfile)) return false;
	if(!SaveAdvances(outfile)) return false;
	
	if(!SaveHuts(outfile)) return false;
	if (!SaveCivilizations(outfile)) return false;

	return true;
}

bool MapFile::SaveTerrain(FILE *outfile)
{
    Assert(g_theWorld);
    size_t const  xSize = g_theWorld->GetXWidth();
    size_t const  ySize = g_theWorld->GetYHeight();

    m_chunk.m_size      = (xSize * ySize) + sizeof(uint16) * 2;
    m_chunk.m_id        = k_TERRAIN_HEADER;

    uint8 * terrain     = new uint8[xSize * ySize];
    uint8 * tptr        = terrain;
    for (size_t y = 0; y < ySize; ++y) 
    {
        for (size_t x = 0; x < xSize; ++x) 
        {
            *tptr++ = static_cast<uint8>(g_theWorld->GetCell(x, y)->GetTerrain());
        }
    }

    if (!m_chunk.Save(outfile)) 
    {
		MessageBoxDialog::Information("Error saving terrain.","ErrSaveTerrain");
		delete [] terrain;
		return false;
    }

    uint16 value        = static_cast<uint16>(xSize);
    if (fwrite(&value, sizeof(value), 1, outfile) != 1)
    {
		MessageBoxDialog::Information("Error saving terrain.","ErrSaveTerrain");
		delete [] terrain;
		return false;
    }

    value = static_cast<uint16>(ySize);
    if (fwrite(&value, sizeof(value), 1, outfile) != 1)
    {
		MessageBoxDialog::Information("Error saving terrain.","ErrSaveTerrain");
		delete [] terrain;
		return false;
    }

    if (fwrite(terrain, 1, xSize * ySize, outfile) != xSize * ySize) 
    {
		MessageBoxDialog::Information("Error saving terrain.","ErrSaveTerrain");
		delete [] terrain;
		return false;
    }

    delete [] terrain;
    return true;
}

bool MapFile::SaveTerrainEnv(FILE *outfile)
{
	m_chunk.m_size = ((g_theWorld->GetXWidth() * g_theWorld->GetYHeight()) * sizeof(uint32)) + sizeof(sint16) * 2;
	m_chunk.m_id = k_TERRAIN_ENV_HEADER;

	uint32 *env = new uint32[m_chunk.m_size];
	uint32 *eptr = env;
	sint16 x, y;
	for(y = 0; y < g_theWorld->GetYHeight(); y++) {
		for(x = 0; x < g_theWorld->GetXWidth(); x++) {
			*eptr = g_theWorld->GetCell(x, y)->GetEnv();
			eptr++;
		}
	}


    if (!m_chunk.Save(outfile)) 
    {
		MessageBoxDialog::Information("Error saving terrain env.","ErrSaveTerrainEnv");
		delete [] env;
		return false;
	}

	x = (sint16)g_theWorld->GetXWidth();
	if(fwrite(&x, sizeof(x), 1, outfile) != 1)
	{
		MessageBoxDialog::Information("Error saving terrain env.","ErrSaveTerrainEnv");
		delete [] env;
		return false;
	}

	y = (sint16)g_theWorld->GetYHeight();
	if(fwrite(&y, sizeof(y), 1, outfile) != 1)
	{
		MessageBoxDialog::Information("Error saving terrain env.","ErrSaveTerrainEnv");
		delete [] env;
		return false;
	}

	if(fwrite(env, 1, x * y * sizeof(uint32), outfile) != (uint32)((x * y) * sizeof(uint32))) 
    {
		MessageBoxDialog::Information("Error saving terrain env.","ErrSaveTerrainEnv");
		delete [] env;
		return false;
	}

	delete [] env;
	return true;
}

bool MapFile::SaveCities(FILE *outfile)
{
	m_chunk.m_id = k_NEW_CITIES_HEADER;
	CivArchive archive;
	archive.SetStore();
	sint32 cityCount = 0;

	sint16 x, y;
	for(y = 0; y < g_theWorld->GetYHeight(); y++) {
		for(x = 0; x < g_theWorld->GetXWidth(); x++) {
			Cell *cell = g_theWorld->GetCell(x, y);
			if (cell->HasCity()) 
            {
				archive << x;
				archive << y;
				MapFileCityData cd  = MapFileCityData(cell->GetCity());
				cd.Serialize(archive);
				cityCount++;
			}
		}
	}
	if(cityCount < 1) {
		
		return true;
	}
	m_chunk.m_size = archive.StreamLen() + sizeof(cityCount);
	if(!m_chunk.Save(outfile))
	{
		
		MessageBoxDialog::Information("Error saving cities.","ErrSaveCities");
		return false;
	}

	if(fwrite(&cityCount, sizeof(cityCount), 1, outfile) != 1)
	{
		
		MessageBoxDialog::Information("Error saving cities.","ErrSaveCities");
		return false;
	}

	if(fwrite(archive.GetStream(), 1, archive.StreamLen(), outfile) != archive.StreamLen())
	{
		
		MessageBoxDialog::Information("Error saving cities.","ErrSaveCities");
		return false;
	}

	return true;
}

bool MapFile::SaveUnits(FILE *outfile)
{
	
	
	if(!(SaveDBNames(outfile, k_UNIT_TYPES_HEADER, g_theUnitDB)))
	{
		
		MessageBoxDialog::Information("Error saving units.","ErrSaveUnits");
		return false;
	}

	
	sint32 numCellsWithUnits = 0;
	CivArchive archive;
	archive.SetStore();

	for (int y = 0; y < g_theWorld->GetYHeight(); y++) 
    {
		for (int x = 0; x < g_theWorld->GetXWidth(); x++) 
        {
			Cell *cell = g_theWorld->GetCell(x, y);
			CellUnitList *units = cell->UnitArmy();
			if(units) {
				archive << static_cast<uint16>(x);
				archive << static_cast<uint16>(y);
				archive << static_cast<uint32>(units->Num());
                for (sint32 i = 0; i < units->Num(); i++) {
					archive.PutUINT8((uint8)units->Access(i).GetOwner());
					archive << static_cast<sint32>(units->Access(i).GetType());
				}
				numCellsWithUnits++;
			}
		}
	}

	m_chunk.m_id = k_UNITS_HEADER;
	m_chunk.m_size = archive.StreamLen() + sizeof(numCellsWithUnits);
	if(!m_chunk.Save(outfile))
	{
		
		MessageBoxDialog::Information("Error saving units.","ErrSaveUnits");
		return false;
	}

	if(fwrite(&numCellsWithUnits, sizeof(numCellsWithUnits), 1, outfile) != 1)
	{
		
		MessageBoxDialog::Information("Error saving units.","ErrSaveUnits");
		return false;
	}

	if(fwrite(archive.GetStream(), 1, archive.StreamLen(), outfile) != archive.StreamLen())
	{
		
		MessageBoxDialog::Information("Error saving units.","ErrSaveUnits");
		return false;
	}

	return true;
}

bool MapFile::SaveImprovements(FILE *outfile)
{
	
	if(!SaveDBNames(outfile, k_IMPROVEMENT_TYPES_HEADER, g_theTerrainImprovementDB))
	{
		
		MessageBoxDialog::Information("Error saving improvements.","ErrSaveImprovements");
		return false;
	}

	
	sint32 numCells = 0;
	CivArchive archive;
	archive.SetStore();

	for (int y = 0; y < g_theWorld->GetYHeight(); y++) 
    {
		for (int x = 0; x < g_theWorld->GetXWidth(); x++) 
        {
			Cell *cell = g_theWorld->GetCell(x, y);
			if (cell->GetNumDBImprovements() > 0) 
            {
				archive << static_cast<uint16>(x);
				archive << static_cast<uint16>(y);
				archive.PutUINT8((uint8)cell->GetNumDBImprovements());
				for (sint32 i = 0; i < cell->GetNumDBImprovements(); i++) 
                {
					archive << static_cast<sint32>(cell->GetDBImprovement(i));
				}
				numCells++;
			}
		}
	}
	
	m_chunk.m_id = k_IMPROVEMENTS_HEADER;
	m_chunk.m_size = archive.StreamLen() + sizeof(numCells);
	if(!m_chunk.Save(outfile))
	{
		
		MessageBoxDialog::Information("Error saving improvements.","ErrSaveImprovements");
		return false;
	}

	if(fwrite(&numCells, sizeof(numCells), 1, outfile) != 1)
	{
		
		MessageBoxDialog::Information("Error saving improvements.","ErrSaveImprovements");
		return false;
	}

	if(fwrite(archive.GetStream(), 1, archive.StreamLen(), outfile) != archive.StreamLen())
	{
		
		MessageBoxDialog::Information("Error saving improvements.","ErrSaveImprovements");
		return false;
	}

	return true;
}


bool MapFile::SaveVision(FILE *outfile)
{
	m_chunk.m_size = (g_theWorld->GetXWidth() * g_theWorld->GetYHeight() * sizeof(uint16)) + sizeof(sint16) * 2 + sizeof(sint8);
	m_chunk.m_id = k_VISION_HEADER;
	for (uint8 p = 0; p < k_MAX_PLAYERS; p++) 
    {
		if(!g_player[p]) continue;

		if(!m_chunk.Save(outfile))
		{
			
			MessageBoxDialog::Information("Error saving vision.","ErrSaveVision");
			return false;
		}

		if(fwrite(&p, sizeof(uint8), 1, outfile) != 1)
		{
			
			MessageBoxDialog::Information("Error saving vision.","ErrSaveVision");
			return false;
		}

		sint16 w = (sint16)g_theWorld->GetXWidth();
		sint16 h = (sint16)g_theWorld->GetYHeight();
		if(fwrite(&w, sizeof(sint16), 1, outfile) != 1)
		{
			
			MessageBoxDialog::Information("Error saving vision.","ErrSaveVision");
			return false;
		}

		if(fwrite(&h, sizeof(sint16), 1, outfile) != 1)
		{
			
			MessageBoxDialog::Information("Error saving vision.","ErrSaveVision");
			return false;
		}

		for(int x = 0; x < w; x++) 
        {
			for(int y = 0; y < h; y++) 
            {
				if(fwrite(&g_player[p]->m_vision->m_array[x][y], 1, sizeof(uint16), outfile) != sizeof(uint16))
				{
					
					MessageBoxDialog::Information("Error saving vision.","ErrSaveVision");
					return false;
				}
			}
		}
	}
	return true;
}

bool MapFile::SaveAdvances(FILE *outfile)
{
    if (!SaveDBNames(outfile, k_ADVANCE_TYPES_HEADER, g_theAdvanceDB))
    {
        MessageBoxDialog::Information("Error saving advances.","ErrSaveAdvances");
        return false;
    }
	
    m_chunk.m_size = sizeof(uint8) + sizeof(uint16) + g_theAdvanceDB->NumRecords();
    m_chunk.m_id = k_PLAYER_ADVANCES_HEADER;

    uint16 na = static_cast<uint16>(g_theAdvanceDB->NumRecords());

    for (uint8 p = 0; p < k_MAX_PLAYERS; p++) 
    {
		if (!g_player[p]) continue;

		if (!m_chunk.Save(outfile))
		{
			MessageBoxDialog::Information("Error saving advances.","ErrSaveAdvances");
			return false;
		}

		if (fwrite(&p, sizeof(uint8), 1, outfile) != 1)
		{
			MessageBoxDialog::Information("Error saving advances.","ErrSaveAdvances");
			return false;
		}


		if (fwrite(&na, sizeof(uint16), 1, outfile) != 1)
		{
			MessageBoxDialog::Information("Error saving advances.","ErrSaveAdvances");
			return false;
		}

            for (sint32 a = 0; a < static_cast<sint32>(na); a++) 
            {
			uint8 hasAdv = g_player[p]->HasAdvance(a);
			if (fwrite(&hasAdv, sizeof(uint8), 1, outfile) != 1)
			{
				MessageBoxDialog::Information("Error saving advances.","ErrSaveAdvances");
				return false;
			}
		}
	}

	return true;
}



bool MapFile::SaveHuts(FILE *outfile)
{
	m_chunk.m_size = g_theWorld->GetXWidth() * g_theWorld->GetYHeight() * sizeof(uint8) + sizeof(sint16) * 2;
	m_chunk.m_id = k_HUTS_HEADER;

	uint8 *terrain = new uint8[m_chunk.m_size];
	uint8 *tptr = terrain;
	sint16 x, y;
	MapPoint mappoint;
	uint8 zero = 0;
	uint8 one = 1;
	for (y = 0; y < g_theWorld->GetYHeight(); y++) 
    {
		for (x = 0; x < g_theWorld->GetXWidth(); x++) 
        {
			mappoint.Set(x,y);
            *tptr++ = g_theWorld->GetGoodyHut(mappoint) ? one : zero;
		}
	}

	if(!m_chunk.Save(outfile)) {
		MessageBoxDialog::Information("Error saving huts.","ErrSaveHuts");
		delete [] terrain;
		return false;
	}

	x = (sint16)g_theWorld->GetXWidth();
	if(fwrite(&x, sizeof(x), 1, outfile) != 1)
	{
		MessageBoxDialog::Information("Error saving huts.","ErrSaveHuts");
		delete [] terrain;
		return false;
	}

	y = (sint16)g_theWorld->GetYHeight();
	if(fwrite(&y, sizeof(y), 1, outfile) != 1)
	{
		MessageBoxDialog::Information("Error saving huts.","ErrSaveHuts");
		delete [] terrain;
		return false;
	}

	if(fwrite(terrain, 1, x * y, outfile) != (uint32)x * y) {
		MessageBoxDialog::Information("Error saving huts.","ErrSaveHuts");
		delete [] terrain;
		return false;
	}

	delete [] terrain;
	return true;
}


bool MapFile::SaveCivilizations(FILE *outfile)
{
	m_chunk.m_size = k_CIVS_BLOCK_LENGTH;
	m_chunk.m_id = k_CIVS_HEADER;

	uint8 *civs = new uint8[m_chunk.m_size];
	uint8 *ptr = civs;

	for (int i = 0; i < k_MAX_PLAYERS; i++)
	{
		uint32 * longPtr = (uint32 *)ptr;

		if (g_player[i])
		{
			*longPtr = g_player[i]->m_civilisation->GetCivilisation();
		}
		else
		{
			
			*longPtr = 0;
		}
		ptr += sizeof(uint32);
		
		size_t length;
		if (g_player[i])
		{
			MBCHAR const * pName = g_player[i]->GetLeaderName();
			length = strlen(pName);
			for (size_t j = 0; j < length; j++)
			{
				*ptr++ = pName[j];
			}
		}
		else
		{
			length = 0; 
		}

		
		
		for(uint32 j = 0; j < (k_MAPFILE_NAME_LEN - length); j++)
		{
			*ptr++ = (uint8)0;
		}
	}



	if (!m_chunk.Save(outfile)) {
		MessageBoxDialog::Information("Error saving civilizations.","ErrSaveCivs");
		delete [] civs;
		return false;
	}

	if (fwrite(civs, 1, k_CIVS_BLOCK_LENGTH, outfile) != k_CIVS_BLOCK_LENGTH)
	{
		MessageBoxDialog::Information("Error saving civilizations.","ErrSaveCivs");
		delete [] civs;
		return false;
	}

	delete [] civs;
	return true;
}






bool MapFile::LoadMap(FILE *infile)
{
    while (!feof(infile)) 
    {
		uint32 chunkId;
		sint32 r = fread(&chunkId, 1, sizeof(chunkId), infile);
		if(r != sizeof(chunkId))
			return feof(infile) ? true : false;
		
		sint32 chunkSize;
		r = fread(&chunkSize, 1, sizeof(chunkSize), infile);
		if(r != sizeof(chunkSize))
		{
			MessageBoxDialog::Information("Error loading map: can't read chunksize.","ErrLoadMapChunk");
			return false;
		}

		uint8 * buf = new uint8[chunkSize];
#define LoadMapStop()	{ delete [] buf; return false; }

		r = fread(buf, 1, chunkSize, infile);
		if (r != chunkSize)
		{
			
			if (feof(infile))
			{
				MessageBoxDialog::Information("Error loading mapfile. Unexpected end of file found.","ErrLoadMapEOF");
			}
			if (ferror(infile))
			{
				MessageBoxDialog::Information("Error loading mapfile. File error occurred.","ErrLoadMapFileError");
			}

 			LoadMapStop();
		}

		switch(chunkId) {
			case k_TERRAIN_HEADER:           if(!LoadTerrain(buf, chunkSize)) LoadMapStop(); break;				
			case k_TERRAIN_ENV_HEADER:       if(!LoadTerrainEnv(buf, chunkSize)) LoadMapStop(); break;
			case k_UNITS_HEADER:             if(!LoadUnits(buf, chunkSize)) LoadMapStop(); break;
			case k_UNIT_TYPES_HEADER:        if(!LoadUnitTypes(buf, chunkSize)) LoadMapStop(); break;
			case k_NEW_CITIES_HEADER:        if(!LoadCities(buf, chunkSize)) LoadMapStop(); break;
			case k_CITIES_HEADER:            if(!LoadOldCities(buf, chunkSize)) LoadMapStop(); break;
			case k_IMPROVEMENT_TYPES_HEADER: if(!LoadImprovementTypes(buf, chunkSize)) LoadMapStop(); break;
			case k_IMPROVEMENTS_HEADER:      if(!LoadImprovements(buf, chunkSize)) LoadMapStop(); break;
			case k_VISION_HEADER:            if(!LoadVision(buf, chunkSize)) LoadMapStop(); break;
			case k_ADVANCE_TYPES_HEADER:     if(!LoadAdvanceNames(buf, chunkSize)) LoadMapStop(); break;
			case k_PLAYER_ADVANCES_HEADER:   if(!LoadAdvances(buf, chunkSize)) LoadMapStop(); break;
			
			case k_HUTS_HEADER:				 if(!LoadHuts(buf, chunkSize)) LoadMapStop(); break;
			case k_CIVS_HEADER:			     if(!LoadCivilizations(buf, chunkSize)) LoadMapStop(); break;
			default:
				Assert("Unknown chunk type" == 0);
				break;
		}

		delete [] buf;
#undef LoadMapStop
	}

	return true;
}


extern sint32 g_isCheatModeOn;
extern void gameinit_ResetMapSize();

bool MapFile::LoadTerrain(uint8 *buf, sint32 size)
{
	sint32 pos = 0;
	sint16 x, y;
	sint16 w = (sint16)g_theWorld->GetXWidth();
	sint16 h = (sint16)g_theWorld->GetYHeight();

	g_isCheatModeOn = TRUE;

	for(y = 0; y < h; y++) {
		for(x = 0; x < w; x++) {
			Cell *cell = g_theWorld->GetCell(x,y);
			while(cell->GetNumUnits() > 0) {
				cell->AccessUnit(0).Kill(CAUSE_REMOVE_ARMY_UNKNOWN, -1);
			}

			if(cell->GetCity().m_id != 0) {
				cell->GetCity().Kill(CAUSE_REMOVE_ARMY_UNKNOWN, -1);
			}

			while(cell->GetNumImprovements() > 0) {
				cell->AccessImprovement(0).Kill();
			}

			while(cell->GetNumDBImprovements() > 0) {
				cell->RemoveDBImprovement(cell->GetDBImprovement(0));
			}
		}
	}

	sint32 i;
	for(i = 0; i < k_MAX_PLAYERS; i++) {
		if(g_player[i]) {
			g_player[i]->m_vision->SetTheWholeWorldUnexplored();
		}

	}
	g_director->AddCopyVision();
	g_director->CatchUp();

	g_isCheatModeOn = FALSE;

	PULLSHORT(w);
	PULLSHORT(h);

	bool yWrapOk = (h % w == 0);

	g_theWorld->Reset(w, h, yWrapOk ? g_theProfileDB->IsYWrap() : FALSE, g_theProfileDB->IsXWrap());
	
	if(size != (sint32)((w * h) + (sizeof(sint16) * 2)))
	{
		
		MessageBoxDialog::Information("Error loading terrain.","ErrLoadTerrain");
		return false;
	}

	for(y = 0; y < h; y++) {
		for(x = 0; x < w; x++) {
			uint8 terr;
			PULLBYTE(terr);
			g_theWorld->GetCell(x, y)->SetTerrain((sint32)terr);
		}
	}
	
	gameinit_ResetMapSize();
	return true;
}

bool MapFile::LoadTerrainEnv(uint8 *buf, sint32 size)
{
	sint32 pos = 0;
	sint16 w, h;
	PULLSHORT(w);
	PULLSHORT(h);

	

	if(size != (sint32)((w * h * sizeof(uint32)) + (sizeof(sint16) * 2)))
	{
		
		MessageBoxDialog::Information("Error loading terrain env.","ErrLoadTerrainEnv");
		return false;
	}

	sint16 x, y;
	for(y = 0; y < h; y++) {
		for(x = 0; x < w; x++) {
			uint32 env;
			PULLLONG(env);
			env &= ~(k_MASK_ENV_CITY | k_MASK_ENV_CITY_RADIUS); 
			g_theWorld->GetCell(x, y)->SetEnv(env);
		}
	}

	for(y = 0; y < h; y++) {
		for(x = 0; x < w; x++) {
			g_theWorld->GetCell(x, y)->CalcMovementType();
			g_theWorld->GetCell(x, y)->CalcTerrainMoveCost();
		}
	}

	g_theWorld->NumberContinents();
	return true;
}

bool MapFile::LoadUnits(uint8 *buf, sint32 size)
{
	sint32 numCellsWithUnits;
	sint32 pos = 0;

	PULLLONG(numCellsWithUnits);
	sint32 i, j;
	for(i = 0; i < numCellsWithUnits; i++) {
		sint16 x, y;
		sint32 numUnits;
		PULLSHORT(x);
		PULLSHORT(y);
		PULLLONG(numUnits);
		for(j = 0; j < numUnits; j++) {
			uint8 owner;
			sint32 type;
			PULLBYTE(owner);
			PULLLONG(type);
			if(!g_player[owner]) {
				DPRINTF(k_DBG_GAMESTATE, ("WARNING: Player %d does not exist, can't create unit"));
			} else {
				if(m_unitTypeMap[type] >= 0) {
					MapPoint pos(x,y);
					g_player[owner]->CreateUnit(m_unitTypeMap[type], pos, Unit(), FALSE, CAUSE_NEW_ARMY_CHEAT);
				}
			}
		}
	}
			
	return true;
}

bool MapFile::LoadUnitTypes(uint8 *buf, sint32 size)
{
    sint32 pos = 0;

    sint32 numTypes;
    PULLLONG(numTypes);
    delete [] m_unitTypeMap;
    m_unitTypeMap = new sint32[numTypes];
    std::fill(m_unitTypeMap, m_unitTypeMap + numTypes, CTPRecord::INDEX_INVALID);

    char dbName[k_MAX_NAME_LEN];
    for (sint32 i = 0; i < numTypes; i++) 
    {
        PULLSTRING(dbName);
        sint32 strId;
        if (!g_theStringDB->GetStringID(dbName, strId)) 
        {
            DPRINTF(k_DBG_GAMESTATE, ("WARNING: Unit %s does not exist in string DB\n", dbName));
        } 
        else if (!g_theUnitDB->GetNamedItem(strId, m_unitTypeMap[i])) 
        {
            DPRINTF(k_DBG_GAMESTATE, ("WARNING: Unit %s does not exist\n", dbName));
        }
    }
		
    return true;
}

bool MapFile::LoadCities(uint8 *buf, sint32 size)
{
	sint32 pos = 0;
	sint32 numCities;
	PULLLONG(numCities);
	sint32 i;
	for(i = 0; i < numCities; i++) {
		sint16 x, y;
		sint32 citySize;
		uint64 improvements;
		uint64 wonders;
		sint32 citytype;
		uint8 owner;

		PULLSHORT(x);
		PULLSHORT(y);
		PULLLONG(citySize);
		PULLLONG64(improvements);
		PULLLONG64(wonders);
		PULLBYTE(owner);

		
		sint32 len;
		MBCHAR name[k_MAPFILE_NAME_LEN + 1];
		PULLLONG(len);
		for (int j = 0; j < len; j++)
		{
			PULLBYTE(name[j]);
		}


		if(g_theWorld->IsLand(x, y)) {
			citytype = unitutil_GetLandCity();
		} else {
			citytype = unitutil_GetSeaCity();
		}

		if(!g_player[owner])
			continue;

		MapPoint pos(x,y);
		Unit city = g_player[owner]->CreateCity(citytype, pos, CAUSE_NEW_CITY_CHEAT, NULL, -1);
		city.CD()->ChangePopulation(citySize - city.CD()->PopCount());
		city.CD()->SetImprovements(improvements);
		city.CD()->SetWonders(wonders);
		
		city.CD()->SetName(name);

		g_player[owner]->m_builtWonders |= wonders;


	}

	return true;
}

bool MapFile::LoadOldCities(uint8 *buf, sint32 size)
{
	sint32 x, y;
	for(x = 0; x < g_theWorld->GetXWidth(); x++) {
		for(y = 0; y < g_theWorld->GetYHeight(); y++) {
			Cell *cell = g_theWorld->GetCell(x, y);
			cell->SetEnv(cell->GetEnv() & ~(k_MASK_ENV_CITY | k_MASK_ENV_CITY_RADIUS));
		}
	}

	sint32 pos = 0;
	sint32 numCities;
	PULLLONG(numCities);
	sint32 i;
	for(i = 0; i < numCities; i++) {
		sint16 x, y;
		sint32 citySize;
		uint64 improvements;
		uint64 wonders;
		sint32 citytype;
		uint8 owner;

		PULLSHORT(x);
		PULLSHORT(y);
		PULLLONG(citySize);
		PULLLONG64(improvements);
		PULLLONG64(wonders);
		PULLBYTE(owner);

		if(g_theWorld->IsLand(x, y)) {
			citytype = unitutil_GetLandCity();
		} else {
			citytype = unitutil_GetSeaCity();
		}

		if(!g_player[owner])
			continue;

		MapPoint pos(x,y);
		Unit city = g_player[owner]->CreateCity(citytype, pos, CAUSE_NEW_CITY_CHEAT, NULL, -1);
		Assert(city.IsValid());
		if(city.IsValid()) {
			city.CD()->ChangePopulation(citySize - city.CD()->PopCount());
			city.CD()->SetImprovements(improvements);
			city.CD()->SetWonders(wonders);
		}

		g_player[owner]->m_builtWonders |= wonders;


	}

	return true;
}

bool MapFile::LoadImprovements(uint8 *buf, sint32 size)
{
	sint32 numCells;
	sint32 pos = 0;
	PULLLONG(numCells);

	sint32 i;
	for(i = 0; i < numCells; i++) {
		sint16 x, y;
		uint8 numImprovements;
		PULLSHORT(x);
		PULLSHORT(y);
		PULLBYTE(numImprovements);

		uint8 j;
		for(j = 0; j < numImprovements; j++) {
			sint32 type;
			PULLLONG(type);
			if(m_improvementTypeMap[type] < 0)
				continue;
		
			g_theWorld->GetCell(x, y)->InsertDBImprovement(m_improvementTypeMap[type]);
		}
	}

	return true;
}

bool MapFile::LoadImprovementTypes(uint8 *buf, sint32 size)
{
    sint32 pos = 0;
    
    sint32 numTypes;
    PULLLONG(numTypes);
    delete [] m_improvementTypeMap;
    m_improvementTypeMap = new sint32[numTypes];
    std::fill(m_improvementTypeMap, m_improvementTypeMap + numTypes, CTPRecord::INDEX_INVALID);

    char dbName[k_MAX_NAME_LEN];
    for (sint32 i = 0; i < numTypes; i++) 
    {
        PULLSTRING(dbName);
        if(!g_theTerrainImprovementDB->GetNamedItem(dbName, m_improvementTypeMap[i])) 
        {
            DPRINTF(k_DBG_GAMESTATE, ("WARNING: Improvement %s does not exist\n", dbName));
        }
    }
		
    return true;
}

bool MapFile::LoadVision(uint8 *buf, sint32 size)
{
	uint8 p;
	sint16 w, h;
	sint32 pos = 0;

	PULLBYTE(p);
	PULLSHORT(w);
	PULLSHORT(h);
	Assert(w == g_theWorld->GetXWidth());
	Assert(h == g_theWorld->GetYHeight());

	if(!g_player[p])
		return true;

	sint32 x, y;
	for(x = 0; x < w; x++) {
		for(y = 0; y < h; y++) {
			PULLSHORT(g_player[p]->m_vision->m_array[x][y]);
			g_player[p]->m_vision->m_array[x][y] &= 0x8000;
		}
	}

	g_director->AddCopyVision();
	g_director->CatchUp();

	return true;
}

bool MapFile::LoadAdvanceNames(uint8 *buf, sint32 size)
{
    sint32 pos = 0; // Used in PULL macros

    sint32 numTypes;
    PULLLONG(numTypes);

    delete [] m_advanceTypeMap;
    m_advanceTypeMap = new sint32[numTypes];
    std::fill(m_advanceTypeMap, m_advanceTypeMap + numTypes, CTPRecord::INDEX_INVALID);

    char dbName[k_MAX_NAME_LEN];
    for (sint32 i = 0; i < numTypes; i++) 
    {
        PULLSTRING(dbName);
        
        sint32 strId;
        if (!g_theStringDB->GetStringID(dbName, strId)) 
        {
            DPRINTF(k_DBG_GAMESTATE, ("WARNING: Advance %s does not exist in string db\n", dbName));
        } 
        else if (!g_theAdvanceDB->GetNamedItem(strId, m_advanceTypeMap[i])) 
        {
            DPRINTF(k_DBG_GAMESTATE, ("WARNING: Advance %s does not exist\n", dbName));
        }
    }
    
    return true;
}

/// Load advances of a single player
/// @param buf Input stream from file
/// @param a_Size Size of input stream
bool MapFile::LoadAdvances(uint8 * buf, sint32 a_Size)
{
    sint32 pos = 0; // Used in PULL macros

    uint8 p;
    PULLBYTE(p);
            
    Assert(g_player[p]);
    if (!g_player[p])
        return true;

    Assert(!g_player[p]->m_disableChooseResearch);
    g_player[p]->m_disableChooseResearch = TRUE;

    uint16 na;
    PULLSHORT(na);
    for (size_t i = 0; i < na; ++i) 
    {
        uint8 hasAdv;
        PULLBYTE(hasAdv);
        if (hasAdv && (m_advanceTypeMap[i] >= 0)) 
        {
            g_player[p]->m_advances->SetHasAdvance(m_advanceTypeMap[i]);
        }		
    }
    
    g_player[p]->m_disableChooseResearch = FALSE;

    Assert(pos <= a_Size);
    return true;
}


bool MapFile::LoadHuts(uint8 *buf, sint32 size)
{

	sint32 pos = 0; 
	sint16 w, h;
	PULLSHORT(w);
	PULLSHORT(h);


	if(size != (sint32)((w * h * sizeof(uint8)) + 2 * sizeof(sint16)))
	{
		
		MessageBoxDialog::Information("Error loading huts.","ErrLoadHuts");
		return false;
	}

	sint16 x, y;
	for(y = 0; y < h; y++) {
		for(x = 0; x < w; x++) {
			uint8 isHut;
			PULLBYTE(isHut);
			if (isHut)
			{
				g_theWorld->GetCell(x, y)->CreateGoodyHut();
			}
		}
	}
	return true;
}


bool MapFile::LoadCivilizations(uint8 *buf, sint32 size)
{
	sint32 pos = 0; 
	uint32 currNation;

	
	if (size != k_CIVS_BLOCK_LENGTH)
	{
		
		MessageBoxDialog::Information("Error loading civilizations.","ErrLoadCivs");
		return false;
	}

	for (int i = 0; i < k_MAX_PLAYERS; i++)
	{
		
		
		PULLLONG(currNation);
		if (g_player[i])
		{
			g_player[i]->m_civilisation->ResetCiv((CIV_INDEX)currNation, g_player[i]->m_civilisation->GetGender());
			MBCHAR name[k_MAPFILE_NAME_LEN];
			for (int j = 0; j < k_MAPFILE_NAME_LEN; j++)
			{
				PULLBYTE(name[j]);
			}

			

			if (name[0]) 
			{
				g_player[i]->m_civilisation->AccessData()->SetLeaderName(name);
				
				
				if(i == g_theProfileDB->GetPlayerIndex()) {
					g_theProfileDB->SetLeaderName(name);
				}
			}

		}
		else
		{
			for (int j = 0; j < k_MAPFILE_NAME_LEN + 4; j++) 
			{
				uint8 foo;
				PULLBYTE(foo);
			}
		}
	}

	return true;
}
