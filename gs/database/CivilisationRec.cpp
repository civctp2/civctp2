










#include "c3.h"
#include "c3errors.h"

#include "StrDB.h"
#include "CivArchive.h"
#include "CivilisationRec.h"
#include "Token.h"


enum TOKEN_CIVILISATION_FLAG
	{
	TOKEN_CIV_LEADER_NAME = TOKEN_MAX + 1,
	TOKEN_CIV_LEADER_NAME_FEMALE,
	TOKEN_CIV_PERSONALITY_MALE,
	TOKEN_CIV_PERSONALITY_FEMALE,
    TOKEN_CIV_PERSONALITY_DESCRIPTION,
	TOKEN_CIV_CITY_NAME,
	TOKEN_CIV_IS_CAPITAL,
	TOKEN_CIV_SINGULAR,
	TOKEN_CIV_PLURAL,
	TOKEN_CIV_EMISSARY_PHOTO,
	TOKEN_CIV_COUNTRY,
	TOKEN_CIV_CITY_STYLE,
	TOKEN_CIV_PARCHMENT,
	TOKEN_CIVILISATION_MAX_VAL
	} ;


	extern	StringDB	*g_theStringDB ;

	sint32	g_parse_civilisation_abort ;










CivilisationRecord::CivilisationRecord() 
	{
	m_capital = k_CAPITAL_UNDEFINED ;
	m_numCities = 0 ;
	m_cityname = NULL;
	m_cityStyle = -1;
	m_personality_male = NULL;
	m_personality_female = NULL;
	}

CivilisationRecord::~CivilisationRecord()
{
	if(m_cityname) {
		delete [] m_cityname;
		m_cityname = NULL;
	}

	if(m_personality_male) {
		delete [] m_personality_male;
	}

	if(m_personality_female) {
		delete [] m_personality_female;
	}
	m_personality_male = NULL;
	m_personality_female = NULL;
}











void CivilisationRecord::Serialize(CivArchive &archive)
{
	
	CHECKSERIALIZE
		
	if (archive.IsStoring()) {
		Record::Serialize(archive);
		archive.StoreChunk((uint8 *)&m_leader_name, ((uint8 *)&m_numCities)+sizeof(m_numCities));
		archive.Store((uint8*)m_cityname, m_numCities * sizeof(StringId));
	} else {
		Record::Serialize(archive);
		archive.LoadChunk((uint8 *)&m_leader_name, ((uint8 *)&m_numCities)+sizeof(m_numCities));
		m_cityname = new StringId[m_numCities];
		archive.Load((uint8*)m_cityname, m_numCities * sizeof(StringId));
	}
}










sint32 CivilisationRecord::ParseRecord(Token *token)
	{ 
	MBCHAR	s[k_MAX_NAME_LEN] ;
	char text[k_MAX_TEXT_LEN]; 

	StringId str_id ;
	
	if (!token_ParseAnOpenBraceNext(token))
		return (FALSE) ;

	
	
	if (token->Next() != TOKEN_CIV_LEADER_NAME)
		{
		c3errors_ErrorDialog(token->ErrStr(), "expected civilisation leader name token") ;

		return (FALSE) ;
		}


	if (token->Next() != TOKEN_STRING)
		{
		c3errors_ErrorDialog(token->ErrStr(), "civilisation leader name not found") ;

		return (FALSE) ;
		}
	else
		{
		token->GetString(s) ;
		if (!g_theStringDB->GetStringID(s, str_id))
			{
			c3errors_ErrorDialog(token->ErrStr(), "Could not find %s in string database", s) ;

			return (FALSE) ;
			}

		m_leader_name = str_id ;
		}


	
	
	if (token->Next() != TOKEN_CIV_LEADER_NAME_FEMALE)
		{
		c3errors_ErrorDialog(token->ErrStr(), "expected civilisation leader name token") ;

		return (FALSE) ;
		}


	if (token->Next() != TOKEN_STRING)
		{
		c3errors_ErrorDialog(token->ErrStr(), "civilisation female leader name not found") ;

		return (FALSE) ;
		}
	else
		{
		token->GetString(s) ;
		if (!g_theStringDB->GetStringID(s, str_id))
			{
			c3errors_ErrorDialog(token->ErrStr(), "Could not find %s in string database", s) ;

			return (FALSE) ;
			}

		m_leader_name_female = str_id ;
		}

	if(token->Next() != TOKEN_CIV_PERSONALITY_MALE) {
		c3errors_ErrorDialog(token->ErrStr(), "expected CIV_PERSONALITY_MALE");
		return FALSE;
	}
	if(token->Next() != TOKEN_QUOTED_STRING) {
		c3errors_ErrorDialog(token->ErrStr(), "expected quoted personality name");
		return FALSE;
	}
	token->GetString(text);
	m_personality_male = new char[strlen(text) + 1];
	strcpy(m_personality_male, text);

	if(token->Next() != TOKEN_CIV_PERSONALITY_FEMALE) {
		c3errors_ErrorDialog(token->ErrStr(), "expected CIV_PERSONALITY_FEMALE");
		return FALSE;
	}
	if(token->Next() != TOKEN_QUOTED_STRING) {
		c3errors_ErrorDialog(token->ErrStr(), "expected quoted personality name");
		return FALSE;
	}
	token->GetString(text);
	m_personality_female = new char[strlen(text) + 1];
	strcpy(m_personality_female, text);



    if (token->Next() != TOKEN_CIV_PERSONALITY_DESCRIPTION) { 
        c3errors_ErrorDialog(token->ErrStr(), "expected ai personality description");
		return FALSE;
    } 
	if (token->Next() != TOKEN_STRING) {
		c3errors_ErrorDialog(token->ErrStr(), "personality description string id not found");
		return (FALSE) ;
    } else {
		token->GetString(s) ;
		if (!g_theStringDB->GetStringID(s, str_id)) {
			c3errors_ErrorDialog(token->ErrStr(), "Could not find %s in string database", s) ;

			return (FALSE) ;
		}

		m_personality_description = str_id ;
	}

	
	
	if (token->Next() != TOKEN_CIV_COUNTRY)
		{
		c3errors_ErrorDialog(token->ErrStr(), "expected civilisation leader name token") ;

		return (FALSE) ;
		}


	if (token->Next() != TOKEN_STRING)
		{
		c3errors_ErrorDialog(token->ErrStr(), "country name not found") ;

		return (FALSE) ;
		}
	else
		{
		token->GetString(s) ;
		if (!g_theStringDB->GetStringID(s, str_id))
			{
			c3errors_ErrorDialog(token->ErrStr(), "Could not find %s in string database", s) ;

			return (FALSE) ;
			}

		m_country_name = str_id ;
		}

	
	
	if (token->Next() != TOKEN_CIV_SINGULAR)
		{
		c3errors_ErrorDialog(token->ErrStr(), "expected civilisation leader name token") ;

		return (FALSE) ;
		}


	if (token->Next() != TOKEN_STRING)
		{
		c3errors_ErrorDialog(token->ErrStr(), "singular name not found") ;

		return (FALSE) ;
		}
	else
		{
		token->GetString(s) ;
		if (!g_theStringDB->GetStringID(s, str_id))
			{
			c3errors_ErrorDialog(token->ErrStr(), "Could not find %s in string database", s) ;

			return (FALSE) ;
			}

		m_singular_name = str_id ;
		}

	
	
	if (token->Next() != TOKEN_CIV_PLURAL)
		{
		c3errors_ErrorDialog(token->ErrStr(), "expected civilisation leader name token") ;

		return (FALSE) ;
		}


	if (token->Next() != TOKEN_STRING)
		{
		c3errors_ErrorDialog(token->ErrStr(), "singular name not found") ;

		return (FALSE) ;
		}
	else
		{
		token->GetString(s) ;
		if (!g_theStringDB->GetStringID(s, str_id))
			{
			c3errors_ErrorDialog(token->ErrStr(), "Could not find %s in string database", s) ;

			return (FALSE) ;
			}

		m_plural_name = str_id ;
		}

	
	
	if (token->Next() != TOKEN_CIV_EMISSARY_PHOTO)
		{
		c3errors_ErrorDialog(token->ErrStr(), "expected civilisation emissary photo token") ;

		return (FALSE) ;
		}


	if (token->Next() != TOKEN_STRING)
		{
		c3errors_ErrorDialog(token->ErrStr(), "singular name not found") ;

		return (FALSE) ;
		}
	else
		{
		token->GetString(s) ;
		if (!g_theStringDB->GetStringID(s, str_id))
			{
			c3errors_ErrorDialog(token->ErrStr(), "Could not find %s in string database", s) ;

			return (FALSE) ;
			}

		m_emissary_photo = str_id ;
		}

	if(token->Next() != TOKEN_CIV_PARCHMENT) {
		c3errors_ErrorDialog(token->ErrStr(), "Expected parchment index");
		return FALSE;
	}

	if(token->Next() != TOKEN_NUMBER) {
		c3errors_ErrorDialog(token->ErrStr(), "Parchment index not found");
		return FALSE;
	}

	token->GetNumber(m_parchment);

	if ( token->Next() != TOKEN_CIV_CITY_STYLE ) {
		c3errors_ErrorDialog( token->ErrStr(), "expected city style token" );
		return FALSE;
	}

	if ( token->Next() != TOKEN_NUMBER ) {
		c3errors_ErrorDialog( token->ErrStr(), "style index not found" );
		return FALSE;
	}

	token->GetNumber( m_cityStyle );

	ParseCityList(token) ;

	if (token->GetType() != TOKEN_CLOSE_BRACE)
		return (FALSE) ;

	return (TRUE) ;
	}


sint32 CivilisationRecord::ParseCityList(Token *token)
	{
	MBCHAR	s[k_MAX_NAME_LEN] ;

	StringId str_id ;
	StringId cityname[k_MAX_CITY_NAMES];

	m_numCities = 0 ;
	token->Next() ;
	while (token->GetType() == TOKEN_CIV_CITY_NAME)
		{
		if (m_numCities >= k_MAX_CITY_NAMES)
			{
			c3errors_ErrorDialog(token->ErrStr(), "Too many city names defined") ;
			g_parse_civilisation_abort = TRUE ;

			return (FALSE) ;
			}

		if (token->Next() != TOKEN_STRING)
			{
			c3errors_ErrorDialog(token->ErrStr(), "City name expected") ;
			g_parse_civilisation_abort = TRUE ;

			return (FALSE) ;
			}

		token->GetString(s) ;
		if (!g_theStringDB->GetStringID(s, str_id))
			{
			c3errors_ErrorDialog(token->ErrStr(), "Could not find %s in string database", s) ;
			g_parse_civilisation_abort = TRUE ;

			return (FALSE) ;
			}

		cityname[m_numCities] = str_id ;
		if (token->Next() == TOKEN_CIV_IS_CAPITAL)
			{
			if (m_capital == k_CAPITAL_UNDEFINED)
				m_capital = m_numCities ;
			else
				{
				c3errors_ErrorDialog(token->ErrStr(), "Capital for civilisation is defined more than once") ;
				g_parse_civilisation_abort = TRUE ;

				return (FALSE) ;
				}

			token->Next() ;
			}

		m_numCities++ ;
		}

	m_cityname = new StringId[m_numCities];
	memcpy(m_cityname, cityname, m_numCities * sizeof(StringId));

	return (TRUE) ;
	}


StringId CivilisationRecord::GetCityName(const sint32 city)
	{
	Assert(city>=0) ;
	Assert(city<=m_numCities) ;

	return (m_cityname[city]) ;
	}
