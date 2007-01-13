










#include "c3.h"
#include "c3errors.h"

#include "CivilisationDB.h"
#include "StrDB.h"
#include "ErrMsg.h"
#include "Unit.h"
#include "civarchive.h"
#include "Token.h"
#include "globals.h"

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

#define shiftbit(i) ((uint64)((uint64)(0x01) << (uint64)(i)))		


	extern StringDB *g_theStringDB ;
	
	extern sint32 g_parse_civilisation_abort ;


TokenData g_CivilisationDB_token_data [TOKEN_CIVILISATION_MAX_VAL] = {
	{ TOKEN_CIV_LEADER_NAME, "CIV_LEADER_NAME" },
	{ TOKEN_CIV_LEADER_NAME_FEMALE, "CIV_LEADER_NAME_FEMALE"},
	{ TOKEN_CIV_PERSONALITY_MALE, "CIV_PERSONALITY_MALE"},
	{ TOKEN_CIV_PERSONALITY_FEMALE, "CIV_PERSONALITY_FEMALE"},
    { TOKEN_CIV_PERSONALITY_DESCRIPTION, "CIV_PERSONALITY_DESCRIPTION"},
	{ TOKEN_CIV_CITY_NAME, "CIV_CITY_NAME" },
	{ TOKEN_CIV_IS_CAPITAL, "CITY_IS_CAPITAL"},
	{ TOKEN_CIV_SINGULAR, "CIV_SINGULAR"},				
	{ TOKEN_CIV_PLURAL, "CIV_PLURAL"},
	{ TOKEN_CIV_EMISSARY_PHOTO, "CIV_EMISSARY_PHOTO" },
	{ TOKEN_CIV_COUNTRY, "CIV_COUNTRY"},				
	{ TOKEN_CIV_CITY_STYLE, "CIV_CITY_STYLE" },
    { TOKEN_CIV_PARCHMENT, "CIV_PARCHMENT"},
	} ;









CivilisationDatabase::CivilisationDatabase()
	{
	}













CivilisationDatabase::CivilisationDatabase(CivArchive &archive)
	{
	Serialize(archive) ;
	}









CivilisationDatabase::~CivilisationDatabase()
	{
	}









void CivilisationDatabase::Serialize(CivArchive &archive)
	{
	sint32 i ;

    CHECKSERIALIZE

#define CIVDB_MAGIC	0xAEFFAEFF
	if (archive.IsStoring())
		{
		archive.PerformMagic(CIVDB_MAGIC) ;
		archive<<m_nRec ;
		archive<<m_max_nRec ;

		for (i=0; i<m_nRec; i++)
			m_rec[i].Serialize(archive) ;

		}
	else
		{
		archive.TestMagic(CIVDB_MAGIC) ;
		archive>>m_nRec ;
		archive>>m_max_nRec ;

		Assert(0<m_nRec) ;
		m_rec = new CivilisationRecord[m_max_nRec] ;
		for (i=0; i<m_nRec; i++)
			m_rec[i].Serialize(archive) ;

		}

	}








BOOL CivilisationDatabase::Initialise(char *filename, C3DIR dir)
	{
	g_parse_civilisation_abort = FALSE ;
	if(!ParseCivilisationDatabase(filename, dir)) 
		return (FALSE) ;

	Assert(m_nRec) ;

	return (TRUE) ;
	}









BOOL CivilisationDatabase::ParseCivilisationDatabase(char *filename, C3DIR dir)
	{
	Token	*civilisationToken = new Token(filename, TOKEN_CIVILISATION_MAX_VAL - TOKEN_MAX, g_CivilisationDB_token_data, dir) ;

	sint32 count = 0 ;

	sint32	n ;

    if (civilisationToken->GetType() != TOKEN_NUMBER)
		{ 
		c3errors_ErrorDialog(civilisationToken->ErrStr(), "Expected number of Civilisations not found") ;
        return (FALSE) ;
		}

	civilisationToken->GetNumber(n) ;
	if (n >= k_MAX_CIVILISATIONS)
		{
		c3errors_ErrorDialog(civilisationToken->ErrStr(), "Too many civilisations declared") ;

		return (FALSE) ;
		}

	SetSize(n) ;

	while (ParseACivilisation(civilisationToken, count))
		count++ ;

	delete civilisationToken ;

	if (count < n)
		{
		c3errors_ErrorDialog(civilisationToken->ErrStr(), "Expected number of Civilisations not found") ;

		return (FALSE) ;
		}

	if (g_parse_civilisation_abort) 
		return (FALSE) ;

	return (TRUE) ;
	}









BOOL CivilisationDatabase::ParseACivilisation(Token *civilisationToken, const sint32 count)
	{
	char str[k_MAX_NAME_LEN] ;

	StringId str_id ;
	
	if (civilisationToken->Next() == TOKEN_EOF)
		return (FALSE) ;
	
	if (civilisationToken->GetType() != TOKEN_STRING)
		{
		c3errors_ErrorDialog(civilisationToken->ErrStr(), "Civilisation id expected") ;
		return (FALSE) ;
		}
	else
		{
		civilisationToken->GetString(str) ;
		if (!g_theStringDB->GetStringID(str, str_id))
			{
			c3errors_ErrorDialog(civilisationToken->ErrStr(), "Could not find %s in string database", str) ;
			return (FALSE) ;
			}

		}
	
	StringId u ;

	AddRec(str_id, u) ;
	
#ifdef _DEBUG
	if(!(rand() % 10000)) {
		static saidItOnce = FALSE;
		if(!saidItOnce && strstr(g_theStringDB->GetNameStr(str_id), "WarMany")) {
			c3errors_ErrorDialog("Designer", "I don't like these civilization names");
			saidItOnce = TRUE;
		}
	}
#endif
	return (m_rec[count].ParseRecord(civilisationToken)) ;
	}
