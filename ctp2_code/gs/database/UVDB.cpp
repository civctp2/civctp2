#include "c3.h"
#include "c3errors.h"

#include "Globals.h"
#include "Token.h"
#include "UVDB.h"
#include "ErrMsg.h"
#include "civarchive.h"

	sint32	g_parse_uv_abort ;

enum TOKEN_UV_POLLUTION
	{
	TOKEN_OZONE_TRIGGER = TOKEN_MAX + 1,
	TOKEN_UV_NORTH_POLAR_PROB,
	TOKEN_UV_UPPER_NORTH_MERIDIAN_PROB,
	TOKEN_UV_LOWER_NORTH_MERIDIAN_PROB,
	TOKEN_UV_EQUATORIAL_PROB,
	TOKEN_UV_UPPER_SOUTH_MERIDIAN_PROB,
	TOKEN_UV_LOWER_SOUTH_MERIDIAN_PROB,
	TOKEN_UV_SOUTH_POLAR_PROB,

	TOKEN_OZONE_DEAD_TILES,
	TOKEN_OZONE_DEAD_RESOURCES,
	TOKEN_UV_DEAD_TILE_DESERT,
	TOKEN_UV_DEAD_TILE_FORESTS,
	TOKEN_UV_DEAD_TILE_GRASSLANDS,
	TOKEN_UV_DEAD_TILE_HILLS,
	TOKEN_UV_DEAD_TILE_MOUNTAINS,
	TOKEN_UV_DEAD_TILE_PLAINS,
	TOKEN_UV_DEAD_TILE_SWAMPS,
	TOKEN_UV_DEAD_TILE_TUNDRA,
	TOKEN_UV_DEAD_TILE_CONTINENTAL_SHELF,
	TOKEN_UV_DEAD_RESOURCE_PALM_OIL,
	TOKEN_UV_DEAD_RESOURCE_OIL,
	TOKEN_UV_DEAD_RESOURCE_PHEASANT,
	TOKEN_UV_DEAD_RESOURCE_SILK,
	TOKEN_UV_DEAD_RESOURCE_SPICE,
	TOKEN_UV_DEAD_RESOURCE_TUSKS,
	TOKEN_UV_DEAD_RESOURCE_BLUBBER,
	TOKEN_UV_DEAD_RESOURCE_ALIEN_CORPSES,
	TOKEN_UV_DEAD_RESOURCE_CLAY,
	TOKEN_UV_DEAD_RESOURCE_TOBACCO,
	TOKEN_UV_DEAD_RESOURCE_COTTON,
	TOKEN_UV_DEAD_RESOURCE_COAL,
	TOKEN_UV_DEAD_RESOURCE_GRAPES,
	TOKEN_UV_DEAD_RESOURCE_GRANITE,
	TOKEN_UV_DEAD_RESOURCE_BANANAS,
	TOKEN_UV_DEAD_RESOURCE_JADE,
	TOKEN_UV_DEAD_RESOURCE_HARDWOOD,
	TOKEN_UV_DEAD_RESOURCE_IRON,
	TOKEN_UV_DEAD_RESOURCE_GOLD,
	TOKEN_UV_DEAD_RESOURCE_SILVER,
	TOKEN_UV_DEAD_RESOURCE_BUFFALO,
	TOKEN_UV_DEAD_RESOURCE_WHEAT,
	TOKEN_UV_DEAD_RESOURCE_CORN,
	TOKEN_UV_DEAD_RESOURCE_PEAT,
	TOKEN_UV_DEAD_RESOURCE_MANGROVE,
	TOKEN_UV_DEAD_RESOURCE_MINK,
	TOKEN_UV_DEAD_RESOURCE_CARIBOU,
	TOKEN_UV_DEAD_RESOURCE_FISH,
	TOKEN_UV_DEAD_RESOURCE_WHALES,
	TOKEN_UV_DEAD_RESOURCE_CRABS,
	TOKEN_UV_DEAD_RESOURCE_DIAMONDS,
	TOKEN_UV_DEAD_RESOURCE_MAGNESIUM_NODULE,
	TOKEN_UV_DEAD_RESOURCE_PHOSPHORUS,
	TOKEN_UV_DEAD_RESOURCE_PLANKTON_UPWELLING,
	TOKEN_UV_DEAD_RESOURCE_GIANT_CLAM,
	TOKEN_UV_DEAD_RESOURCE_GIANT_SQUID,

    TOKEN_UV_POLLUTION_MAX_VAL
	} ;








OzoneDatabase::OzoneDatabase()
	{
	memset(m_dead_food, 0, sizeof(m_dead_food)) ;
	memset(m_dead_shield, 0, sizeof(m_dead_shield)) ;
	memset(m_dead_trade, 0, sizeof(m_dead_trade)) ;
	memset(m_dead_gold, 0, sizeof(m_dead_gold)) ;
	memset(m_meridian_prob, 0, sizeof(m_meridian_prob)) ;
	memset(m_meridian_phase_bonus, 0, sizeof(m_meridian_phase_bonus)) ;

	}









sint32 OzoneDatabase::Initialise(char *filename, C3DIR dir)
	{

	g_parse_uv_abort = FALSE ;
	if(!ParseOzoneDatabase(filename, dir))
		return (FALSE) ;

	return (TRUE) ;
	}









OzoneDatabase::OzoneDatabase(CivArchive &archive)
	{
	Serialize(archive) ;
	}









TokenData	g_UVDB_token_data[TOKEN_UV_POLLUTION_MAX_VAL] =
	{
		{TOKEN_OZONE_TRIGGER, "OZONE_TRIGGER"},
		{TOKEN_UV_NORTH_POLAR_PROB, "NORTH_POLAR_PROB"},
		{TOKEN_UV_UPPER_NORTH_MERIDIAN_PROB, "UPPER_NORTH_MERIDIAN_PROB"},
		{TOKEN_UV_LOWER_NORTH_MERIDIAN_PROB, "LOWER_NORTH_MERIDIAN_PROB"},
		{TOKEN_UV_EQUATORIAL_PROB, "EQUATORIAL_PROB"},
		{TOKEN_UV_UPPER_SOUTH_MERIDIAN_PROB, "UPPER_SOUTH_MERIDIAN_PROB"},
		{TOKEN_UV_LOWER_SOUTH_MERIDIAN_PROB, "LOWER_SOUTH_MERIDIAN_PROB"},
		{TOKEN_UV_SOUTH_POLAR_PROB, "SOUTH_POLAR_PROB"},

		{TOKEN_OZONE_DEAD_TILES, "OZONE_DEAD_TILES"},
		{TOKEN_OZONE_DEAD_RESOURCES, "OZONE_DEAD_RESOURCES"},
		{TOKEN_UV_DEAD_TILE_DESERT, "DEAD_TILE_DESERT"},
		{TOKEN_UV_DEAD_TILE_FORESTS, "DEAD_TILE_FORESTS"},
		{TOKEN_UV_DEAD_TILE_GRASSLANDS, "DEAD_TILE_GRASSLANDS"},
		{TOKEN_UV_DEAD_TILE_HILLS, "DEAD_TILE_HILLS"},
		{TOKEN_UV_DEAD_TILE_MOUNTAINS, "DEAD_TILE_MOUNTAINS"},
		{TOKEN_UV_DEAD_TILE_PLAINS, "DEAD_TILE_PLAINS"},
		{TOKEN_UV_DEAD_TILE_SWAMPS, "DEAD_TILE_SWAMPS"},
		{TOKEN_UV_DEAD_TILE_TUNDRA, "DEAD_TILE_TUNDRA"},
		{TOKEN_UV_DEAD_TILE_CONTINENTAL_SHELF, "DEAD_TILE_CONTINENTAL_SHELF"},
		{TOKEN_UV_DEAD_RESOURCE_PALM_OIL, "DEAD_RESOURCE_PALM_OIL"},
		{TOKEN_UV_DEAD_RESOURCE_OIL, "DEAD_RESOURCE_OIL"},
		{TOKEN_UV_DEAD_RESOURCE_PHEASANT, "DEAD_RESOURCE_PHEASANT"},
		{TOKEN_UV_DEAD_RESOURCE_SILK, "DEAD_RESOURCE_SILK"},
		{TOKEN_UV_DEAD_RESOURCE_SPICE, "DEAD_RESOURCE_SPICE"},
		{TOKEN_UV_DEAD_RESOURCE_TUSKS, "DEAD_RESOURCE_TUSKS"},
		{TOKEN_UV_DEAD_RESOURCE_BLUBBER, "DEAD_RESOURCE_BLUBBER"},
		{TOKEN_UV_DEAD_RESOURCE_ALIEN_CORPSES, "DEAD_RESOURCE_ALIEN_CORPSES"},
		{TOKEN_UV_DEAD_RESOURCE_CLAY, "DEAD_RESOURCE_CLAY"},
		{TOKEN_UV_DEAD_RESOURCE_TOBACCO, "DEAD_RESOURCE_TOBACCO"},
		{TOKEN_UV_DEAD_RESOURCE_COTTON, "DEAD_RESOURCE_COTTON"},
		{TOKEN_UV_DEAD_RESOURCE_COAL, "DEAD_RESOURCE_COAL"},
		{TOKEN_UV_DEAD_RESOURCE_GRAPES, "DEAD_RESOURCE_GRAPES"},
		{TOKEN_UV_DEAD_RESOURCE_GRANITE, "DEAD_RESOURCE_GRANITE"},
		{TOKEN_UV_DEAD_RESOURCE_BANANAS, "DEAD_RESOURCE_BANANAS"},
		{TOKEN_UV_DEAD_RESOURCE_JADE, "DEAD_RESOURCE_JADE"},
		{TOKEN_UV_DEAD_RESOURCE_HARDWOOD, "DEAD_RESOURCE_HARDWOOD"},
		{TOKEN_UV_DEAD_RESOURCE_IRON, "DEAD_RESOURCE_IRON"},
		{TOKEN_UV_DEAD_RESOURCE_GOLD, "DEAD_RESOURCE_GOLD"},
		{TOKEN_UV_DEAD_RESOURCE_SILVER, "DEAD_RESOURCE_SILVER"},
		{TOKEN_UV_DEAD_RESOURCE_BUFFALO, "DEAD_RESOURCE_BUFFALO"},
		{TOKEN_UV_DEAD_RESOURCE_WHEAT, "DEAD_RESOURCE_WHEAT"},
		{TOKEN_UV_DEAD_RESOURCE_CORN, "DEAD_RESOURCE_CORN"},
		{TOKEN_UV_DEAD_RESOURCE_PEAT, "DEAD_RESOURCE_PEAT"},
		{TOKEN_UV_DEAD_RESOURCE_MANGROVE, "DEAD_RESOURCE_MANGROVE"},
		{TOKEN_UV_DEAD_RESOURCE_MINK, "DEAD_RESOURCE_MINK"},
		{TOKEN_UV_DEAD_RESOURCE_CARIBOU, "DEAD_RESOURCE_CARIBOU"},
		{TOKEN_UV_DEAD_RESOURCE_FISH, "DEAD_RESOURCE_FISH"},
		{TOKEN_UV_DEAD_RESOURCE_WHALES, "DEAD_RESOURCE_WHALES"},
		{TOKEN_UV_DEAD_RESOURCE_CRABS, "DEAD_RESOURCE_CRABS"},
		{TOKEN_UV_DEAD_RESOURCE_DIAMONDS, "DEAD_RESOURCE_DIAMONDS"},
		{TOKEN_UV_DEAD_RESOURCE_MAGNESIUM_NODULE, "DEAD_RESOURCE_MAGNESIUM_NODULE"},
		{TOKEN_UV_DEAD_RESOURCE_PHOSPHORUS, "DEAD_RESOURCE_PHOSPHORUS"},
		{TOKEN_UV_DEAD_RESOURCE_PLANKTON_UPWELLING, "DEAD_RESOURCE_UPWELLING"},
		{TOKEN_UV_DEAD_RESOURCE_GIANT_CLAM, "DEAD_RESOURCE_GIANT_CLAM"},
		{TOKEN_UV_DEAD_RESOURCE_GIANT_SQUID, "DEAD_RESOURCE_GIANT_SQUID"},
	} ;











sint32 OzoneDatabase::ParseOzoneDatabase(char *filename, C3DIR dir)
	{
	Token	*uvToken = new Token(filename, TOKEN_UV_POLLUTION_MAX_VAL - TOKEN_MAX, g_UVDB_token_data, dir) ;

	ParseAnOzone(uvToken) ;
	uvToken->Next() ;

	delete uvToken ;

	if (g_parse_uv_abort)
		return (FALSE) ;

	return (TRUE) ;
	}











sint32 OzoneDatabase::ParseAnOzone(Token *uvToken)
	{

	if (uvToken->GetType() == TOKEN_EOF)
		return (FALSE) ;

	if (uvToken->GetType() == TOKEN_OZONE_TRIGGER)
		{
		ParseUVTriggerRecord(uvToken) ;
		}
	else
		{
		c3errors_ErrorDialog(uvToken->ErrStr(), "pollution of type ozone trigger required") ;

		return (FALSE) ;
		}

	return (TRUE) ;
	}









OzoneDatabase::~OzoneDatabase()
	{
	}









sint32 OzoneDatabase::ParseUVTriggerRecord(Token *uvToken)
	{
	double	tmp ;

	if (!token_ParseAnOpenBraceNext(uvToken))
		return (FALSE) ;


	if (uvToken->Next() != TOKEN_UV_NORTH_POLAR_PROB)
		{
		c3errors_ErrorDialog (uvToken->ErrStr(), "expected north polar region probability found") ;
		g_parse_uv_abort = TRUE ;

		return (FALSE) ;
		}

	if (uvToken->Next() != TOKEN_NUMBER)
		{
		c3errors_ErrorDialog (uvToken->ErrStr(), "expected north polar region probability found") ;
		g_parse_uv_abort = TRUE ;

		return (FALSE) ;
		}

	uvToken->GetFloat(tmp) ;
	m_meridian_prob[REGION_TYPE_NORTH_POLAR] = tmp * 0.01f ;

	if (uvToken->Next() != TOKEN_NUMBER)
		{
		c3errors_ErrorDialog (uvToken->ErrStr(), "expected north polar region phase bonus found") ;
		g_parse_uv_abort = TRUE ;

		return (FALSE) ;
		}

	uvToken->GetFloat(tmp) ;
	m_meridian_phase_bonus[REGION_TYPE_NORTH_POLAR] = tmp * 0.01f ;


	if (uvToken->Next() != TOKEN_UV_UPPER_NORTH_MERIDIAN_PROB)
		{
		c3errors_ErrorDialog (uvToken->ErrStr(), "expected upper north meridian region probability found") ;
		g_parse_uv_abort = TRUE ;

		return (FALSE) ;
		}

	if (uvToken->Next() != TOKEN_NUMBER)
		{
		c3errors_ErrorDialog (uvToken->ErrStr(), "expected upper north meridian region probability found") ;
		g_parse_uv_abort = TRUE ;

		return (FALSE) ;
		}

	uvToken->GetFloat(tmp) ;
	m_meridian_prob[REGION_TYPE_UPPER_NORTH_MERIDIAN] = tmp * 0.01f ;

	if (uvToken->Next() != TOKEN_NUMBER)
		{
		c3errors_ErrorDialog (uvToken->ErrStr(), "expected upper north meridian region phase bonus found") ;
		g_parse_uv_abort = TRUE ;

		return (FALSE) ;
		}

	uvToken->GetFloat(tmp) ;
	m_meridian_phase_bonus[REGION_TYPE_UPPER_NORTH_MERIDIAN] = tmp * 0.01f ;


	if (uvToken->Next() != TOKEN_UV_LOWER_NORTH_MERIDIAN_PROB)
		{
		c3errors_ErrorDialog (uvToken->ErrStr(), "expected lower north meridian region probability found") ;
		g_parse_uv_abort = TRUE ;

		return (FALSE) ;
		}

	if (uvToken->Next() != TOKEN_NUMBER)
		{
		c3errors_ErrorDialog (uvToken->ErrStr(), "expected lower north meridian region probability found") ;
		g_parse_uv_abort = TRUE ;

		return (FALSE) ;
		}

	uvToken->GetFloat(tmp) ;
	m_meridian_prob[REGION_TYPE_LOWER_NORTH_MERIDIAN] = tmp * 0.01f ;

	if (uvToken->Next() != TOKEN_NUMBER)
		{
		c3errors_ErrorDialog (uvToken->ErrStr(), "expected lower north meridian region phase bonus found") ;
		g_parse_uv_abort = TRUE ;

		return (FALSE) ;
		}

	uvToken->GetFloat(tmp) ;
	m_meridian_phase_bonus[REGION_TYPE_LOWER_NORTH_MERIDIAN] = tmp * 0.01f ;


	if (uvToken->Next() != TOKEN_UV_EQUATORIAL_PROB)
		{
		c3errors_ErrorDialog (uvToken->ErrStr(), "expected equatorial region probability found") ;
		g_parse_uv_abort = TRUE ;

		return (FALSE) ;
		}

	if (uvToken->Next() != TOKEN_NUMBER)
		{
		c3errors_ErrorDialog (uvToken->ErrStr(), "expected equatorial region probability found") ;
		g_parse_uv_abort = TRUE ;

		return (FALSE) ;
		}

	uvToken->GetFloat(tmp) ;
	m_meridian_prob[REGION_TYPE_EQUATORIAL] = tmp * 0.01f ;

	if (uvToken->Next() != TOKEN_NUMBER)
		{
		c3errors_ErrorDialog (uvToken->ErrStr(), "expected equatorial region phase bonus found") ;
		g_parse_uv_abort = TRUE ;

		return (FALSE) ;
		}

	uvToken->GetFloat(tmp) ;
	m_meridian_phase_bonus[REGION_TYPE_EQUATORIAL] = tmp * 0.01f ;


	if (uvToken->Next() != TOKEN_UV_UPPER_SOUTH_MERIDIAN_PROB)
		{
		c3errors_ErrorDialog (uvToken->ErrStr(), "expected upper south meridian region probability found") ;
		g_parse_uv_abort = TRUE ;

		return (FALSE) ;
		}

	if (uvToken->Next() != TOKEN_NUMBER)
		{
		c3errors_ErrorDialog (uvToken->ErrStr(), "expected upper south meridian region probability found") ;
		g_parse_uv_abort = TRUE ;

		return (FALSE) ;
		}

	uvToken->GetFloat(tmp) ;
	m_meridian_prob[REGION_TYPE_UPPER_SOUTH_MERIDIAN] = tmp * 0.01f ;

	if (uvToken->Next() != TOKEN_NUMBER)
		{
		c3errors_ErrorDialog (uvToken->ErrStr(), "expected upper south meridian region phase bonus found") ;
		g_parse_uv_abort = TRUE ;

		return (FALSE) ;
		}

	uvToken->GetFloat(tmp) ;
	m_meridian_phase_bonus[REGION_TYPE_UPPER_SOUTH_MERIDIAN] = tmp * 0.01f ;




	if (uvToken->Next() != TOKEN_UV_LOWER_SOUTH_MERIDIAN_PROB)
		{
		c3errors_ErrorDialog (uvToken->ErrStr(), "expected lower south meridian region probability found") ;
		g_parse_uv_abort = TRUE ;

		return (FALSE) ;
		}

	if (uvToken->Next() != TOKEN_NUMBER)
		{
		c3errors_ErrorDialog (uvToken->ErrStr(), "expected lower south meridian region probability found") ;
		g_parse_uv_abort = TRUE ;

		return (FALSE) ;
		}

	uvToken->GetFloat(tmp) ;
	m_meridian_prob[REGION_TYPE_LOWER_SOUTH_MERIDIAN] = tmp * 0.01f ;

	if (uvToken->Next() != TOKEN_NUMBER)
		{
		c3errors_ErrorDialog (uvToken->ErrStr(), "expected lower south meridian region phase bonus found") ;
		g_parse_uv_abort = TRUE ;

		return (FALSE) ;
		}

	uvToken->GetFloat(tmp) ;
	m_meridian_phase_bonus[REGION_TYPE_LOWER_SOUTH_MERIDIAN] = tmp * 0.01f ;


	if (uvToken->Next() != TOKEN_UV_SOUTH_POLAR_PROB)
		{
		c3errors_ErrorDialog (uvToken->ErrStr(), "expected south polar region probability found") ;
		g_parse_uv_abort = TRUE ;

		return (FALSE) ;
		}

	if (uvToken->Next() != TOKEN_NUMBER)
		{
		c3errors_ErrorDialog (uvToken->ErrStr(), "expected south polar region probability found") ;
		g_parse_uv_abort = TRUE ;

		return (FALSE) ;
		}

	uvToken->GetFloat(tmp) ;
	m_meridian_prob[REGION_TYPE_SOUTH_POLAR] = tmp * 0.01f ;

	if (uvToken->Next() != TOKEN_NUMBER)
		{
		c3errors_ErrorDialog (uvToken->ErrStr(), "expected south polar region phase bonus found") ;
		g_parse_uv_abort = TRUE ;

		return (FALSE) ;
		}

	uvToken->GetFloat(tmp) ;
	m_meridian_phase_bonus[REGION_TYPE_SOUTH_POLAR] = tmp * 0.01f ;





































	if (uvToken->Next() != TOKEN_CLOSE_BRACE)
		return (FALSE) ;

	return (TRUE) ;
	}








sint32 OzoneDatabase::ParseUVDeadTilesRecord(Token *uvToken)
	{
	if (!token_ParseAnOpenBraceNext(uvToken))
		return (FALSE) ;

	if (uvToken->Next() != TOKEN_UV_DEAD_TILE_DESERT)
		{
		c3errors_ErrorDialog (uvToken->ErrStr(), "expected dead desert tile production not found") ;
		g_parse_uv_abort = TRUE ;

		return (FALSE) ;
		}

	if (!ParseDeadTileNumbers(uvToken, m_dead_food[DEAD_TILE_TYPE_DESERT], m_dead_shield[DEAD_TILE_TYPE_DESERT], m_dead_trade[DEAD_TILE_TYPE_DESERT]))
		return (FALSE) ;

	if (uvToken->Next() != TOKEN_UV_DEAD_TILE_FORESTS)
		{
		c3errors_ErrorDialog (uvToken->ErrStr(), "expected dead forests tile production not found") ;
		g_parse_uv_abort = TRUE ;

		return (FALSE) ;
		}

	if (!ParseDeadTileNumbers(uvToken, m_dead_food[DEAD_TILE_TYPE_FORESTS], m_dead_shield[DEAD_TILE_TYPE_FORESTS], m_dead_trade[DEAD_TILE_TYPE_FORESTS]))
		return (FALSE) ;

	if (uvToken->Next() != TOKEN_UV_DEAD_TILE_GRASSLANDS)
		{
		c3errors_ErrorDialog (uvToken->ErrStr(), "expected dead grasslands tile production not found") ;
		g_parse_uv_abort = TRUE ;

		return (FALSE) ;
		}

	if (!ParseDeadTileNumbers(uvToken, m_dead_food[DEAD_TILE_TYPE_GRASSLANDS], m_dead_shield[DEAD_TILE_TYPE_GRASSLANDS], m_dead_trade[DEAD_TILE_TYPE_GRASSLANDS]))
		return (FALSE) ;

	if (uvToken->Next() != TOKEN_UV_DEAD_TILE_HILLS)
		{
		c3errors_ErrorDialog (uvToken->ErrStr(), "expected dead hills tile production not found") ;
		g_parse_uv_abort = TRUE ;

		return (FALSE) ;
		}

	if (!ParseDeadTileNumbers(uvToken, m_dead_food[DEAD_TILE_TYPE_HILLS], m_dead_shield[DEAD_TILE_TYPE_HILLS], m_dead_trade[DEAD_TILE_TYPE_HILLS]))
		return (FALSE) ;

	if (uvToken->Next() != TOKEN_UV_DEAD_TILE_MOUNTAINS)
		{
		c3errors_ErrorDialog (uvToken->ErrStr(), "expected dead mountains tile production not found") ;
		g_parse_uv_abort = TRUE ;

		return (FALSE) ;
		}

	if (!ParseDeadTileNumbers(uvToken, m_dead_food[DEAD_TILE_TYPE_MOUNTAINS], m_dead_shield[DEAD_TILE_TYPE_MOUNTAINS], m_dead_trade[DEAD_TILE_TYPE_MOUNTAINS]))
		return (FALSE) ;

	if (uvToken->Next() != TOKEN_UV_DEAD_TILE_PLAINS)
		{
		c3errors_ErrorDialog (uvToken->ErrStr(), "expected dead plains tile production not found") ;
		g_parse_uv_abort = TRUE ;

		return (FALSE) ;
		}

	if (!ParseDeadTileNumbers(uvToken, m_dead_food[DEAD_TILE_TYPE_PLAINS], m_dead_shield[DEAD_TILE_TYPE_PLAINS], m_dead_trade[DEAD_TILE_TYPE_PLAINS]))
		return (FALSE) ;

	if (uvToken->Next() != TOKEN_UV_DEAD_TILE_SWAMPS)
		{
		c3errors_ErrorDialog (uvToken->ErrStr(), "expected dead swamps tile production not found") ;
		g_parse_uv_abort = TRUE ;

		return (FALSE) ;
		}

	if (!ParseDeadTileNumbers(uvToken, m_dead_food[DEAD_TILE_TYPE_SWAMPS], m_dead_shield[DEAD_TILE_TYPE_SWAMPS], m_dead_trade[DEAD_TILE_TYPE_SWAMPS]))
		return (FALSE) ;

	if (uvToken->Next() != TOKEN_UV_DEAD_TILE_TUNDRA)
		{
		c3errors_ErrorDialog (uvToken->ErrStr(), "expected dead tundra tile production not found") ;
		g_parse_uv_abort = TRUE ;

		return (FALSE) ;
		}

	if (!ParseDeadTileNumbers(uvToken, m_dead_food[DEAD_TILE_TYPE_TUNDRA], m_dead_shield[DEAD_TILE_TYPE_TUNDRA], m_dead_trade[DEAD_TILE_TYPE_TUNDRA]))
		return (FALSE) ;

	if (uvToken->Next() != TOKEN_UV_DEAD_TILE_CONTINENTAL_SHELF)
		{
		c3errors_ErrorDialog (uvToken->ErrStr(), "expected dead continental shelf tile production not found") ;
		g_parse_uv_abort = TRUE ;

		return (FALSE) ;
		}

	if (!ParseDeadTileNumbers(uvToken, m_dead_food[DEAD_TILE_TYPE_CONTINENTAL_SHELF], m_dead_shield[DEAD_TILE_TYPE_CONTINENTAL_SHELF], m_dead_trade[DEAD_TILE_TYPE_CONTINENTAL_SHELF]))
		return (FALSE) ;

	if (uvToken->Next() != TOKEN_CLOSE_BRACE)
		return (FALSE) ;

	return (TRUE) ;
	}









sint32 OzoneDatabase::ParseDeadTileNumbers(Token *uvToken, sint32 &food, sint32 &shield, sint32 &trade)
	{
	if (uvToken->Next() != TOKEN_NUMBER)
		{
		c3errors_ErrorDialog (uvToken->ErrStr(), "expected dead tile food production not found") ;
		g_parse_uv_abort = TRUE ;

		return (FALSE) ;
		}

	uvToken->GetNumber(food) ;

	if (uvToken->Next() != TOKEN_NUMBER)
		{
		c3errors_ErrorDialog (uvToken->ErrStr(), "expected dead tile shield production not found") ;
		g_parse_uv_abort = TRUE ;

		return (FALSE) ;
		}

	uvToken->GetNumber(shield) ;

	if (uvToken->Next() != TOKEN_NUMBER)
		{
		c3errors_ErrorDialog (uvToken->ErrStr(), "expected dead tile trade production not found") ;
		g_parse_uv_abort = TRUE ;

		return (FALSE) ;
		}

	uvToken->GetNumber(trade) ;

	return (TRUE) ;
	}









sint32 OzoneDatabase::ParseUVDeadResourcesRecord(Token *uvToken)
	{
	sint32	i = 0 ;

	if (!token_ParseAnOpenBraceNext(uvToken))
		return (FALSE) ;

	if (uvToken->Next() != TOKEN_UV_DEAD_RESOURCE_PALM_OIL)
		{
		c3errors_ErrorDialog (uvToken->ErrStr(), "expected dead palm oil resource production not found") ;
		g_parse_uv_abort = TRUE ;

		return (FALSE) ;
		}

	if (!ParseDeadResourceNumbers(uvToken, m_dead_food[i], m_dead_shield[i], m_dead_trade[i], m_dead_gold[i]))
		return (FALSE) ;

	i++ ;

	if (uvToken->Next() != TOKEN_UV_DEAD_RESOURCE_OIL)
		{
		c3errors_ErrorDialog (uvToken->ErrStr(), "expected dead oil resource production not found") ;
		g_parse_uv_abort = TRUE ;

		return (FALSE) ;
		}

	if (!ParseDeadResourceNumbers(uvToken, m_dead_food[i], m_dead_shield[i], m_dead_trade[i], m_dead_gold[i]))
		return (FALSE) ;

	i++ ;

	if (uvToken->Next() != TOKEN_UV_DEAD_RESOURCE_PHEASANT)
		{
		c3errors_ErrorDialog (uvToken->ErrStr(), "expected dead pheasant resource production not found") ;
		g_parse_uv_abort = TRUE ;

		return (FALSE) ;
		}

	if (!ParseDeadResourceNumbers(uvToken, m_dead_food[i], m_dead_shield[i], m_dead_trade[i], m_dead_gold[i]))
		return (FALSE) ;

	i++ ;

	if (uvToken->Next() != TOKEN_UV_DEAD_RESOURCE_SILK)
		{
		c3errors_ErrorDialog (uvToken->ErrStr(), "expected dead silk resource production not found") ;
		g_parse_uv_abort = TRUE ;

		return (FALSE) ;
		}

	if (!ParseDeadResourceNumbers(uvToken, m_dead_food[i], m_dead_shield[i], m_dead_trade[i], m_dead_gold[i]))
		return (FALSE) ;

	i++ ;

	if (uvToken->Next() != TOKEN_UV_DEAD_RESOURCE_SPICE)
		{
		c3errors_ErrorDialog (uvToken->ErrStr(), "expected dead spice resource production not found") ;
		g_parse_uv_abort = TRUE ;

		return (FALSE) ;
		}

	if (!ParseDeadResourceNumbers(uvToken, m_dead_food[i], m_dead_shield[i], m_dead_trade[i], m_dead_gold[i]))
		return (FALSE) ;

	i++ ;

	if (uvToken->Next() != TOKEN_UV_DEAD_RESOURCE_TUSKS)
		{
		c3errors_ErrorDialog (uvToken->ErrStr(), "expected dead tusks resource production not found") ;
		g_parse_uv_abort = TRUE ;

		return (FALSE) ;
		}

	if (!ParseDeadResourceNumbers(uvToken, m_dead_food[i], m_dead_shield[i], m_dead_trade[i], m_dead_gold[i]))
		return (FALSE) ;

	i++ ;

	if (uvToken->Next() != TOKEN_UV_DEAD_RESOURCE_BLUBBER)
		{
		c3errors_ErrorDialog (uvToken->ErrStr(), "expected dead blubber resource production not found") ;
		g_parse_uv_abort = TRUE ;

		return (FALSE) ;
		}

	if (!ParseDeadResourceNumbers(uvToken, m_dead_food[i], m_dead_shield[i], m_dead_trade[i], m_dead_gold[i]))
		return (FALSE) ;

	i++ ;

	if (uvToken->Next() != TOKEN_UV_DEAD_RESOURCE_ALIEN_CORPSES)
		{
		c3errors_ErrorDialog (uvToken->ErrStr(), "expected dead alien corpses resource production not found") ;
		g_parse_uv_abort = TRUE ;

		return (FALSE) ;
		}

	if (!ParseDeadResourceNumbers(uvToken, m_dead_food[i], m_dead_shield[i], m_dead_trade[i], m_dead_gold[i]))
		return (FALSE) ;

	i++ ;

	if (uvToken->Next() != TOKEN_UV_DEAD_RESOURCE_CLAY)
		{
		c3errors_ErrorDialog (uvToken->ErrStr(), "expected dead clay resource production not found") ;
		g_parse_uv_abort = TRUE ;

		return (FALSE) ;
		}

	if (!ParseDeadResourceNumbers(uvToken, m_dead_food[i], m_dead_shield[i], m_dead_trade[i], m_dead_gold[i]))
		return (FALSE) ;

	i++ ;

	if (uvToken->Next() != TOKEN_UV_DEAD_RESOURCE_TOBACCO)
		{
		c3errors_ErrorDialog (uvToken->ErrStr(), "expected dead tobacco resource production not found") ;
		g_parse_uv_abort = TRUE ;

		return (FALSE) ;
		}

	if (!ParseDeadResourceNumbers(uvToken, m_dead_food[i], m_dead_shield[i], m_dead_trade[i], m_dead_gold[i]))
		return (FALSE) ;

	i++ ;

	if (uvToken->Next() != TOKEN_UV_DEAD_RESOURCE_COTTON)
		{
		c3errors_ErrorDialog (uvToken->ErrStr(), "expected dead cotton resource production not found") ;
		g_parse_uv_abort = TRUE ;

		return (FALSE) ;
		}

	if (!ParseDeadResourceNumbers(uvToken, m_dead_food[i], m_dead_shield[i], m_dead_trade[i], m_dead_gold[i]))
		return (FALSE) ;

	i++ ;

	if (uvToken->Next() != TOKEN_UV_DEAD_RESOURCE_COAL)
		{
		c3errors_ErrorDialog (uvToken->ErrStr(), "expected dead coal resource production not found") ;
		g_parse_uv_abort = TRUE ;

		return (FALSE) ;
		}

	if (!ParseDeadResourceNumbers(uvToken, m_dead_food[i], m_dead_shield[i], m_dead_trade[i], m_dead_gold[i]))
		return (FALSE) ;

	i++ ;

	if (uvToken->Next() != TOKEN_UV_DEAD_RESOURCE_GRAPES)
		{
		c3errors_ErrorDialog (uvToken->ErrStr(), "expected dead grapes oil resource production not found") ;
		g_parse_uv_abort = TRUE ;

		return (FALSE) ;
		}

	if (!ParseDeadResourceNumbers(uvToken, m_dead_food[i], m_dead_shield[i], m_dead_trade[i], m_dead_gold[i]))
		return (FALSE) ;

	i++ ;

	if (uvToken->Next() != TOKEN_UV_DEAD_RESOURCE_GRANITE)
		{
		c3errors_ErrorDialog (uvToken->ErrStr(), "expected dead granite oil resource production not found") ;
		g_parse_uv_abort = TRUE ;

		return (FALSE) ;
		}

	if (!ParseDeadResourceNumbers(uvToken, m_dead_food[i], m_dead_shield[i], m_dead_trade[i], m_dead_gold[i]))
		return (FALSE) ;

	i++ ;

	if (uvToken->Next() != TOKEN_UV_DEAD_RESOURCE_BANANAS)
		{
		c3errors_ErrorDialog (uvToken->ErrStr(), "expected dead bananas resource production not found") ;
		g_parse_uv_abort = TRUE ;

		return (FALSE) ;
		}

	if (!ParseDeadResourceNumbers(uvToken, m_dead_food[i], m_dead_shield[i], m_dead_trade[i], m_dead_gold[i]))
		return (FALSE) ;

	i++ ;

	if (uvToken->Next() != TOKEN_UV_DEAD_RESOURCE_JADE)
		{
		c3errors_ErrorDialog (uvToken->ErrStr(), "expected dead jade resource production not found") ;
		g_parse_uv_abort = TRUE ;

		return (FALSE) ;
		}

	if (!ParseDeadResourceNumbers(uvToken, m_dead_food[i], m_dead_shield[i], m_dead_trade[i], m_dead_gold[i]))
		return (FALSE) ;

	i++ ;

	if (uvToken->Next() != TOKEN_UV_DEAD_RESOURCE_HARDWOOD)
		{
		c3errors_ErrorDialog (uvToken->ErrStr(), "expected dead hardwood resource production not found") ;
		g_parse_uv_abort = TRUE ;

		return (FALSE) ;
		}

	if (!ParseDeadResourceNumbers(uvToken, m_dead_food[i], m_dead_shield[i], m_dead_trade[i], m_dead_gold[i]))
		return (FALSE) ;

	i++ ;

	if (uvToken->Next() != TOKEN_UV_DEAD_RESOURCE_IRON)
		{
		c3errors_ErrorDialog (uvToken->ErrStr(), "expected dead iron resource production not found") ;
		g_parse_uv_abort = TRUE ;

		return (FALSE) ;
		}

	if (!ParseDeadResourceNumbers(uvToken, m_dead_food[i], m_dead_shield[i], m_dead_trade[i], m_dead_gold[i]))
		return (FALSE) ;

	i++ ;

	if (uvToken->Next() != TOKEN_UV_DEAD_RESOURCE_GOLD)
		{
		c3errors_ErrorDialog (uvToken->ErrStr(), "expected dead gold resource production not found") ;
		g_parse_uv_abort = TRUE ;

		return (FALSE) ;
		}

	if (!ParseDeadResourceNumbers(uvToken, m_dead_food[i], m_dead_shield[i], m_dead_trade[i], m_dead_gold[i]))
		return (FALSE) ;

	i++ ;

	if (uvToken->Next() != TOKEN_UV_DEAD_RESOURCE_SILVER)
		{
		c3errors_ErrorDialog (uvToken->ErrStr(), "expected dead silver resource production not found") ;
		g_parse_uv_abort = TRUE ;

		return (FALSE) ;
		}

	if (!ParseDeadResourceNumbers(uvToken, m_dead_food[i], m_dead_shield[i], m_dead_trade[i], m_dead_gold[i]))
		return (FALSE) ;

	i++ ;

	if (uvToken->Next() != TOKEN_UV_DEAD_RESOURCE_BUFFALO)
		{
		c3errors_ErrorDialog (uvToken->ErrStr(), "expected dead buffalo resource production not found") ;
		g_parse_uv_abort = TRUE ;

		return (FALSE) ;
		}

	if (!ParseDeadResourceNumbers(uvToken, m_dead_food[i], m_dead_shield[i], m_dead_trade[i], m_dead_gold[i]))
		return (FALSE) ;

	i++ ;

	if (uvToken->Next() != TOKEN_UV_DEAD_RESOURCE_WHEAT)
		{
		c3errors_ErrorDialog (uvToken->ErrStr(), "expected dead wheat resource production not found") ;
		g_parse_uv_abort = TRUE ;

		return (FALSE) ;
		}

	if (!ParseDeadResourceNumbers(uvToken, m_dead_food[i], m_dead_shield[i], m_dead_trade[i], m_dead_gold[i]))
		return (FALSE) ;

	i++ ;

	if (uvToken->Next() != TOKEN_UV_DEAD_RESOURCE_CORN)
		{
		c3errors_ErrorDialog (uvToken->ErrStr(), "expected dead corn resource production not found") ;
		g_parse_uv_abort = TRUE ;

		return (FALSE) ;
		}

	if (!ParseDeadResourceNumbers(uvToken, m_dead_food[i], m_dead_shield[i], m_dead_trade[i], m_dead_gold[i]))
		return (FALSE) ;

	i++ ;

	if (uvToken->Next() != TOKEN_UV_DEAD_RESOURCE_PEAT)
		{
		c3errors_ErrorDialog (uvToken->ErrStr(), "expected dead peat resource production not found") ;
		g_parse_uv_abort = TRUE ;

		return (FALSE) ;
		}

	if (!ParseDeadResourceNumbers(uvToken, m_dead_food[i], m_dead_shield[i], m_dead_trade[i], m_dead_gold[i]))
		return (FALSE) ;

	i++ ;

	if (uvToken->Next() != TOKEN_UV_DEAD_RESOURCE_MANGROVE)
		{
		c3errors_ErrorDialog (uvToken->ErrStr(), "expected dead mangrove resource production not found") ;
		g_parse_uv_abort = TRUE ;

		return (FALSE) ;
		}

	if (!ParseDeadResourceNumbers(uvToken, m_dead_food[i], m_dead_shield[i], m_dead_trade[i], m_dead_gold[i]))
		return (FALSE) ;

	i++ ;

	if (uvToken->Next() != TOKEN_UV_DEAD_RESOURCE_MINK)
		{
		c3errors_ErrorDialog (uvToken->ErrStr(), "expected dead mink resource production not found") ;
		g_parse_uv_abort = TRUE ;

		return (FALSE) ;
		}

	if (!ParseDeadResourceNumbers(uvToken, m_dead_food[i], m_dead_shield[i], m_dead_trade[i], m_dead_gold[i]))
		return (FALSE) ;

	i++ ;

	if (uvToken->Next() != TOKEN_UV_DEAD_RESOURCE_CARIBOU)
		{
		c3errors_ErrorDialog (uvToken->ErrStr(), "expected dead caribou resource production not found") ;
		g_parse_uv_abort = TRUE ;

		return (FALSE) ;
		}

	if (!ParseDeadResourceNumbers(uvToken, m_dead_food[i], m_dead_shield[i], m_dead_trade[i], m_dead_gold[i]))
		return (FALSE) ;

	i++ ;

	if (uvToken->Next() != TOKEN_UV_DEAD_RESOURCE_FISH)
		{
		c3errors_ErrorDialog (uvToken->ErrStr(), "expected dead fish resource production not found") ;
		g_parse_uv_abort = TRUE ;

		return (FALSE) ;
		}

	if (!ParseDeadResourceNumbers(uvToken, m_dead_food[i], m_dead_shield[i], m_dead_trade[i], m_dead_gold[i]))
		return (FALSE) ;

	i++ ;

	if (uvToken->Next() != TOKEN_UV_DEAD_RESOURCE_WHALES)
		{
		c3errors_ErrorDialog (uvToken->ErrStr(), "expected dead whales resource production not found") ;
		g_parse_uv_abort = TRUE ;

		return (FALSE) ;
		}

	if (!ParseDeadResourceNumbers(uvToken, m_dead_food[i], m_dead_shield[i], m_dead_trade[i], m_dead_gold[i]))
		return (FALSE) ;

	i++ ;

	if (uvToken->Next() != TOKEN_UV_DEAD_RESOURCE_CRABS)
		{
		c3errors_ErrorDialog (uvToken->ErrStr(), "expected dead crabs resource production not found") ;
		g_parse_uv_abort = TRUE ;

		return (FALSE) ;
		}

	if (!ParseDeadResourceNumbers(uvToken, m_dead_food[i], m_dead_shield[i], m_dead_trade[i], m_dead_gold[i]))
		return (FALSE) ;

	i++ ;

	if (uvToken->Next() != TOKEN_UV_DEAD_RESOURCE_DIAMONDS)
		{
		c3errors_ErrorDialog (uvToken->ErrStr(), "expected dead diamonds resource production not found") ;
		g_parse_uv_abort = TRUE ;

		return (FALSE) ;
		}

	if (!ParseDeadResourceNumbers(uvToken, m_dead_food[i], m_dead_shield[i], m_dead_trade[i], m_dead_gold[i]))
		return (FALSE) ;

	i++ ;

	if (uvToken->Next() != TOKEN_UV_DEAD_RESOURCE_MAGNESIUM_NODULE)
		{
		c3errors_ErrorDialog (uvToken->ErrStr(), "expected dead magnesium nodule resource production not found") ;
		g_parse_uv_abort = TRUE ;

		return (FALSE) ;
		}

	if (!ParseDeadResourceNumbers(uvToken, m_dead_food[i], m_dead_shield[i], m_dead_trade[i], m_dead_gold[i]))
		return (FALSE) ;

	i++ ;

	if (uvToken->Next() != TOKEN_UV_DEAD_RESOURCE_PHOSPHORUS)
		{
		c3errors_ErrorDialog (uvToken->ErrStr(), "expected dead phosphorus resource production not found") ;
		g_parse_uv_abort = TRUE ;

		return (FALSE) ;
		}

	if (!ParseDeadResourceNumbers(uvToken, m_dead_food[i], m_dead_shield[i], m_dead_trade[i], m_dead_gold[i]))
		return (FALSE) ;

	i++ ;

	if (uvToken->Next() != TOKEN_UV_DEAD_RESOURCE_PLANKTON_UPWELLING)
		{
		c3errors_ErrorDialog (uvToken->ErrStr(), "expected dead plankton upwelling resource production not found") ;
		g_parse_uv_abort = TRUE ;

		return (FALSE) ;
		}

	if (!ParseDeadResourceNumbers(uvToken, m_dead_food[i], m_dead_shield[i], m_dead_trade[i], m_dead_gold[i]))
		return (FALSE) ;

	i++ ;

	if (uvToken->Next() != TOKEN_UV_DEAD_RESOURCE_GIANT_CLAM)
		{
		c3errors_ErrorDialog (uvToken->ErrStr(), "expected dead giant clam resource production not found") ;
		g_parse_uv_abort = TRUE ;

		return (FALSE) ;
		}

	if (!ParseDeadResourceNumbers(uvToken, m_dead_food[i], m_dead_shield[i], m_dead_trade[i], m_dead_gold[i]))
		return (FALSE) ;

	i++ ;

	if (uvToken->Next() != TOKEN_UV_DEAD_RESOURCE_GIANT_SQUID)
		{
		c3errors_ErrorDialog (uvToken->ErrStr(), "expected dead giant squid `resource production not found") ;
		g_parse_uv_abort = TRUE ;

		return (FALSE) ;
		}

	if (!ParseDeadResourceNumbers(uvToken, m_dead_food[i], m_dead_shield[i], m_dead_trade[i], m_dead_gold[i]))
		return (FALSE) ;

	if (uvToken->Next() != TOKEN_CLOSE_BRACE)
		{
		c3errors_ErrorDialog (uvToken->ErrStr(), "missing closing brace") ;
		g_parse_uv_abort = TRUE ;

		return (FALSE) ;
		}

	return (TRUE) ;
	}









sint32 OzoneDatabase::ParseDeadResourceNumbers(Token *uvToken, sint32 &food, sint32 &shield, sint32 &trade, sint32 &gold)
	{
	if (uvToken->Next() != TOKEN_NUMBER)
		{
		c3errors_ErrorDialog (uvToken->ErrStr(), "expected dead tile food production not found") ;
		g_parse_uv_abort = TRUE ;

		return (FALSE) ;
		}

	uvToken->GetNumber(food) ;

	if (uvToken->Next() != TOKEN_NUMBER)
		{
		c3errors_ErrorDialog (uvToken->ErrStr(), "expected dead tile shield production not found") ;
		g_parse_uv_abort = TRUE ;

		return (FALSE) ;
		}

	uvToken->GetNumber(shield) ;

	if (uvToken->Next() != TOKEN_NUMBER)
		{
		c3errors_ErrorDialog (uvToken->ErrStr(), "expected dead tile trade production not found") ;
		g_parse_uv_abort = TRUE ;

		return (FALSE) ;
		}

	uvToken->GetNumber(trade) ;

	if (uvToken->Next() != TOKEN_NUMBER)
		{
		c3errors_ErrorDialog (uvToken->ErrStr(), "expected dead tile gold base production not found") ;
		g_parse_uv_abort = TRUE ;

		return (FALSE) ;
		}

	uvToken->GetNumber(food) ;

	return (TRUE) ;
	}













void OzoneDatabase::Serialize(CivArchive &archive)
{
#define UVDB_MAGIC	0x52139876
	if (archive.IsStoring()) {
		archive.PerformMagic(UVDB_MAGIC) ;
		 archive.StoreArrayString(m_meridian_prob, k_NUM_MERIDIANS + 1);
		 archive.StoreArrayString(m_meridian_phase_bonus, k_NUM_MERIDIANS+1);

		archive.StoreArray(m_dead_food, k_MAX_RESOURCES);
		archive.StoreArray(m_dead_shield, k_MAX_RESOURCES);
		archive.StoreArray(m_dead_trade, k_MAX_RESOURCES);
		archive.StoreArray(m_dead_gold, k_MAX_RESOURCES);
	}
	else {
	}
}
