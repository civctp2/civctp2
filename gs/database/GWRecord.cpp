











#include "c3.h"
#include "c3errors.h"
#include "CivArchive.h"
#include "Token.h"

#include "StrDB.h"
#include "DB.h"
#include "GovernmentRecord.h"
#include "GWRecord.h"

#include "GWToken.h"
#include "AdvanceRecord.h"
#include "TerrainRecord.h"
#include "gstypes.h"
#include "terrainutil.h"

	extern	StringDB	*g_theStringDB ;

	extern	TokenData	g_ImpDB_token_data[] ;


	sint32	g_parse_gw_abort ;









GWRecord::GWRecord()
	{
	memset((uint8*)&m_changes, 0, (uint8*)&m_max_bonus - (uint8*)&m_changes + sizeof(m_max_bonus));

	sint32	i ;

    Assert(g_theStringDB) ;
    Assert(g_theAdvanceDB) ;
    Assert(g_theGovernmentDB) ;
	
	m_changes = 0 ;
	for (i=0; i<MAX_CHANGES; i++)
		{
		m_original[i] = -1;
		m_flip[i] = -1;
		m_prob[i] = 0 ;
		}

	m_next_to_ocean_bonus = 0 ;
	m_neighbour_bonus = 0 ;
	m_max_bonus = 0 ;
	}










sint32 GWRecord::ParseGWRecord(Token *pollToken)
	{ 
	BOOL	done ;

    sint32	tmp ;

	double	dtmp ;

	if (!token_ParseAnOpenBraceNext(pollToken))
		return (FALSE) ;

	
	if (!token_ParseValNext(pollToken, TOKEN_GW_POLLUTION_NEXT_TO_OCEAN, tmp))
		return (FALSE) ;

	m_next_to_ocean_bonus = tmp * 0.01f ;

	
	if (!token_ParseValNext(pollToken, TOKEN_GW_POLLUTION_NEIGHBOUR_BONUS, tmp))
		return (FALSE) ;

	m_neighbour_bonus = tmp * 0.01f ;

	
	if (!token_ParseValNext(pollToken, TOKEN_GW_POLLUTION_MAX_BONUS, tmp))
		return (FALSE) ;

	m_max_bonus = tmp * 0.01f ;

	
	m_changes = 0 ;
	done = FALSE ;
	while (!done)
		{
		if (pollToken->Next() == TOKEN_CLOSE_BRACE)
			{
			done = true ;
			}
		else
			{
			
			switch (pollToken->GetType())
				{
				case TOKEN_GW_POLLUTION_TILE_DESERT :
					m_original[m_changes] = TERRAIN_DESERT ;
					break ;

				case TOKEN_GW_POLLUTION_TILE_FOREST :
					m_original[m_changes] = TERRAIN_FOREST ;
					break ;

				case TOKEN_GW_POLLUTION_TILE_GLACIER :
					m_original[m_changes] = TERRAIN_GLACIER ;
					break ;

				case TOKEN_GW_POLLUTION_TILE_GRASSLAND :
					m_original[m_changes] = TERRAIN_GRASSLAND ;
					break ;

				case TOKEN_GW_POLLUTION_TILE_HILL :
					m_original[m_changes] = TERRAIN_HILL ;
					break ;

				case TOKEN_GW_POLLUTION_TILE_JUNGLE :
					m_original[m_changes] = TERRAIN_JUNGLE ;
					break ;

				case TOKEN_GW_POLLUTION_TILE_MOUNTAIN :
					m_original[m_changes] = TERRAIN_MOUNTAIN ;
					break ;

				case TOKEN_GW_POLLUTION_TILE_BROWN_HILL :
					m_original[m_changes] = TERRAIN_BROWN_HILL ;
					break ;

				case TOKEN_GW_POLLUTION_TILE_BROWN_MOUNTAIN :
					m_original[m_changes] = TERRAIN_BROWN_MOUNTAIN ;
					break ;

				case TOKEN_GW_POLLUTION_TILE_WHITE_HILL :
					m_original[m_changes] = TERRAIN_WHITE_HILL ;
					break ;

				case TOKEN_GW_POLLUTION_TILE_WHITE_MOUNTAIN :
					m_original[m_changes] = TERRAIN_WHITE_MOUNTAIN ;
					break ;

				case TOKEN_GW_POLLUTION_TILE_PLAINS :
					m_original[m_changes] = TERRAIN_PLAINS ;
					break ;

				case TOKEN_GW_POLLUTION_TILE_SWAMP :
					m_original[m_changes] = TERRAIN_SWAMP ;
					break ;

				case TOKEN_GW_POLLUTION_TILE_TUNDRA :
					m_original[m_changes] = TERRAIN_TUNDRA ;
					break ;

				default :
					Assert(FALSE) ;
					c3errors_ErrorDialog (pollToken->ErrStr(), "unknown tile type") ;
	
					return (FALSE) ;

				}

			
			if (pollToken->Next() != TOKEN_NUMBER)
				{
				c3errors_ErrorDialog (pollToken->ErrStr(), "number expected for probability") ;

				return (FALSE) ;
				}

			pollToken->GetFloat(dtmp) ;
			m_prob[m_changes] = dtmp * 0.01f ;
			
			switch (pollToken->Next())
				{
				case TOKEN_GW_POLLUTION_TILE_TO_DESERT :
					m_flip[m_changes] = TERRAIN_DESERT ;
					break ;

				case TOKEN_GW_POLLUTION_TILE_TO_PLAINS :
					m_flip[m_changes] = TERRAIN_PLAINS ;
					break ;

				case TOKEN_GW_POLLUTION_TILE_TO_JUNGLE :
					m_flip[m_changes] = TERRAIN_JUNGLE ;
					break ;

				case TOKEN_GW_POLLUTION_TILE_TO_TUNDRA :
					m_flip[m_changes] = TERRAIN_TUNDRA ;
					break ;

				case TOKEN_GW_POLLUTION_TILE_TO_FOREST:
					m_flip[m_changes] = TERRAIN_FOREST ;
					break ;

				case TOKEN_GW_POLLUTION_TILE_TO_SWAMP :
					m_flip[m_changes] = TERRAIN_SWAMP ;
					break ;

				case TOKEN_GW_POLLUTION_TILE_TO_MOUNTAIN :
					m_flip[m_changes] = TERRAIN_MOUNTAIN ;
					break ;

				case TOKEN_GW_POLLUTION_TILE_TO_BROWN_HILL :
					m_flip[m_changes] = TERRAIN_BROWN_HILL ;
					break ;

				case TOKEN_GW_POLLUTION_TILE_TO_BROWN_MOUNTAIN :
					m_flip[m_changes] = TERRAIN_BROWN_MOUNTAIN ;
					break ;

				case TOKEN_GW_POLLUTION_TILE_TO_WHITE_HILL :
					m_flip[m_changes] = TERRAIN_WHITE_HILL ;
					break ;

				case TOKEN_GW_POLLUTION_TILE_TO_WHITE_MOUNTAIN :
					m_flip[m_changes] = TERRAIN_WHITE_MOUNTAIN ;
					break ;

				case TOKEN_GW_POLLUTION_TILE_TO_OCEAN :
					m_flip[m_changes] = TERRAIN_WATER_SHALLOW ;
					break ;

				case TOKEN_GW_POLLUTION_TILE_TO_GRASSLAND :
					m_flip[m_changes] = TERRAIN_GRASSLAND ;
					break ;

				default :
					Assert(FALSE) ;
					c3errors_ErrorDialog (pollToken->ErrStr(), "unknown tile type") ;
					return (FALSE) ;

				}

			m_changes++ ;
			if (m_changes > MAX_CHANGES)
				{
				c3errors_ErrorDialog(pollToken->ErrStr(), "Too many pollution changes defined") ;
				return (FALSE) ;
				}

			}

		}

    if (g_parse_gw_abort)
		return (FALSE) ;
	
	return (TRUE) ;
	}












void GWRecord::Serialize(CivArchive &archive)
	{
	if (archive.IsStoring())
		{
		Record::Serialize(archive);
		archive <<	m_changes ;

		archive.StoreArray((sint32*)m_original, MAX_CHANGES);
		archive.StoreArray((sint32*)m_flip, MAX_CHANGES);

		 archive.StoreArrayString(m_prob, MAX_CHANGES);
		 archive.PutDoubleString(m_next_to_ocean_bonus);
		archive.PutDoubleString(m_neighbour_bonus);
		archive.PutDoubleString(m_max_bonus);
		}
	else
		{
		}

	}
