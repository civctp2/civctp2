











#include "c3.h"
#include "c3errors.h"

#include "GWDB.h"
#include "StrDB.h"

#include "Unit.h"
#include "RandGen.h"
#include "GWToken.h"
#include "Globals.h"
#include "terrainutil.h"

	extern	StringDB	*g_theStringDB ;

	extern	RandomGenerator	*g_rand ;

	extern	sint32	g_parse_gw_abort ;


#define shiftbit(i) ((uint64)((uint64)(0x01) << (uint64)(i)))		









GlobalWarmingDatabase::GlobalWarmingDatabase()
	{
	}









sint32 GlobalWarmingDatabase::Initialise(char *filename, C3DIR dir)
	{
	
	g_parse_gw_abort = FALSE ;
	if(!ParseGlobalWarmingDatabase(filename, dir))
		return (FALSE) ;

	Assert(m_nRec) ;
	return (TRUE) ;
	}

	







GlobalWarmingDatabase::GlobalWarmingDatabase(CivArchive &archive)
	{
	Serialize(archive) ;
	}









TokenData	g_GWDB_token_data[TOKEN_GW_POLLUTION_MAX_VAL] =
	{
		{TOKEN_GLOBAL_WARMING_TRIGGER, "GLOBAL_WARMING_TRIGGER"},
		{TOKEN_GW_POLLUTION_NEXT_TO_OCEAN, "POLLUTION_NEXT_TO_OCEAN"},
		{TOKEN_GW_POLLUTION_NEIGHBOUR_BONUS, "POLLUTION_NEIGHBOUR_BONUS"},
		{TOKEN_GW_POLLUTION_MAX_BONUS, "POLLUTION_MAX_BONUS"},
		{TOKEN_GW_POLLUTION_TILE_DESERT, "POLLUTION_TILE_DESERT"},
		{TOKEN_GW_POLLUTION_TILE_FOREST, "POLLUTION_TILE_FOREST"},
		{TOKEN_GW_POLLUTION_TILE_GLACIER, "POLLUTION_TILE_GLACIER"},
		{TOKEN_GW_POLLUTION_TILE_GRASSLAND, "POLLUTION_TILE_GRASSLAND"},
		{TOKEN_GW_POLLUTION_TILE_HILL, "POLLUTION_TILE_HILL"},
		{TOKEN_GW_POLLUTION_TILE_JUNGLE, "POLLUTION_TILE_JUNGLE"},
		{TOKEN_GW_POLLUTION_TILE_MOUNTAIN, "POLLUTION_TILE_MOUNTAIN"},
		{TOKEN_GW_POLLUTION_TILE_BROWN_HILL, "POLLUTION_TILE_BROWN_HILL"},
		{TOKEN_GW_POLLUTION_TILE_BROWN_MOUNTAIN, "POLLUTION_TILE_BROWN_MOUNTAIN"},
		{TOKEN_GW_POLLUTION_TILE_WHITE_HILL, "POLLUTION_TILE_WHITE_HILL"},
		{TOKEN_GW_POLLUTION_TILE_WHITE_MOUNTAIN, "POLLUTION_TILE_WHITE_MOUNTAIN"},
		{TOKEN_GW_POLLUTION_TILE_PLAINS, "POLLUTION_TILE_PLAINS"},
		{TOKEN_GW_POLLUTION_TILE_SWAMP, "POLLUTION_TILE_SWAMP"},
		{TOKEN_GW_POLLUTION_TILE_TUNDRA, "POLLUTION_TILE_TUNDRA"},
		{TOKEN_GW_POLLUTION_TILE_TO_PLAINS, "POLLUTION_TILE_TO_PLAINS"},
		{TOKEN_GW_POLLUTION_TILE_TO_JUNGLE, "POLLUTION_TILE_TO_JUNGLE"},
		{TOKEN_GW_POLLUTION_TILE_TO_TUNDRA, "POLLUTION_TILE_TO_TUNDRA"},
		{TOKEN_GW_POLLUTION_TILE_TO_FOREST, "POLLUTION_TILE_TO_FOREST"},
		{TOKEN_GW_POLLUTION_TILE_TO_DESERT, "POLLUTION_TILE_TO_DESERT"},
		{TOKEN_GW_POLLUTION_TILE_TO_SWAMP, "POLLUTION_TILE_TO_SWAMP"},
		{TOKEN_GW_POLLUTION_TILE_TO_MOUNTAIN, "POLLUTION_TILE_TO_MOUNTAIN"},
		{TOKEN_GW_POLLUTION_TILE_TO_BROWN_HILL, "POLLUTION_TILE_TO_BROWN_HILL"},
		{TOKEN_GW_POLLUTION_TILE_TO_BROWN_MOUNTAIN, "POLLUTION_TILE_TO_BROWN_MOUNTAIN"},
		{TOKEN_GW_POLLUTION_TILE_TO_WHITE_HILL, "POLLUTION_TILE_TO_WHITE_HILL"},
		{TOKEN_GW_POLLUTION_TILE_TO_WHITE_MOUNTAIN, "POLLUTION_TILE_TO_WHITE_MOUNTAIN"},
		{TOKEN_GW_POLLUTION_TILE_TO_OCEAN, "POLLUTION_TILE_TO_OCEAN"},
		{TOKEN_GW_POLLUTION_TILE_TO_GRASSLAND, "POLLUTION_TILE_TO_GRASSLAND"},
	} ;











sint32 GlobalWarmingDatabase::ParseGlobalWarmingDatabase(char *filename, C3DIR dir)
	{
	Token	*gwToken = new Token(filename,	TOKEN_GW_POLLUTION_MAX_VAL - TOKEN_MAX, g_GWDB_token_data, dir) ;
	
	SetSize(1) ;

	ParseAGlobalWarming(gwToken) ;

	delete gwToken ;

	if (g_parse_gw_abort) 
		return (FALSE) ;

	return (TRUE) ;
	}











sint32 GlobalWarmingDatabase::ParseAGlobalWarming(Token *gwToken)
	{
	sint32	rec ;

	
	if (gwToken->GetType() == TOKEN_EOF)
		return (FALSE) ;
	
	switch (gwToken->GetType())
		{
		case TOKEN_GLOBAL_WARMING_TRIGGER :
			AddRec(0, rec) ;
			return (m_rec[0].ParseGWRecord(gwToken)) ;

		default :
			c3errors_ErrorDialog(gwToken->ErrStr(), "pollution of type global warming required") ;
			return (FALSE) ;
			break ;

		}
	
	
	}









double GlobalWarmingDatabase::ChangeProbability(const TERRAIN_TYPES terrain, TERRAIN_TYPES &newtype) const
	{
	double	probability = 0.0f ;

	sint32	found ;

	for (found=0; (found<m_rec[0].m_changes) && (m_rec[0].m_original[found] != terrain); found++) ;
	
	if (found < m_rec[0].m_changes)
		{
		newtype = m_rec[0].m_flip[found] ;
		probability = m_rec[0].m_prob[found] ;
		}
	else
		newtype = -1;

	Assert((newtype>=TERRAIN_FOREST) && (newtype<TERRAIN_MAX)) ;

	return (probability) ;
	}









TERRAIN_TYPES GlobalWarmingDatabase::Change(const double baseProb, const TERRAIN_TYPES terrain) const
	{
	TERRAIN_TYPES	newTerrainType ;

	double	probability ;

	double	lastChance = 999.0f ;

	sint32	i ;

	probability = g_rand->NextF() ;
	newTerrainType = terrain ;
	for (i=0; i<m_rec[0].m_changes; i++)
		{
		if (terrain == m_rec[0].m_original[i]) 
			if (m_rec[0].m_prob[i] < lastChance)
				if (probability < (m_rec[0].m_prob[i] + baseProb))
					{
					lastChance = m_rec[0].m_prob[i] ;
					newTerrainType = m_rec[0].m_flip[i] ;
					}

		}

	return (newTerrainType) ;
	}

	







GlobalWarmingDatabase::~GlobalWarmingDatabase()
	{
	}




































