#include "c3.h"
#include "c3errors.h"

#include "PollutionDB.h"
#include "StrDB.h"
#include "Unit.h"
#include "RandGen.h"
#include "PollutionToken.h"

	extern	StringDB	*g_theStringDB ;

	extern	RandomGenerator	*g_rand ;

	extern	sint32	g_parse_pollution_abort ;


#define shiftbit(i) ((uint64)((uint64)(0x01) << (uint64)(i)))		









PollutionDatabase::PollutionDatabase()
	{
	}









sint32 PollutionDatabase::Initialise(char *filename, C3DIR dir)
	{
	
	g_parse_pollution_abort = FALSE ;
	if(!ParsePollutionDatabase(filename, dir))
		return (FALSE) ;

	Assert(m_nRec) ;
	return (TRUE) ;
	}

	







PollutionDatabase::PollutionDatabase(CivArchive &archive)
	{
	Serialize(archive) ;
	}









TokenData	g_PollutionDB_token_data[TOKEN_POLLUTION_MAX_VAL] =
	{
		{TOKEN_POLLUTION_TRIGGER_LIST_SMALL_MAP, "POLLUTION_TRIGGER_LIST_SMALL_MAP"},
		{TOKEN_POLLUTION_TRIGGER_LIST_MEDIUM_MAP, "POLLUTION_TRIGGER_LIST_MEDIUM_MAP"},
		{TOKEN_POLLUTION_TRIGGER_LIST_LARGE_MAP, "POLLUTION_TRIGGER_LIST_LARGE_MAP"},
		{TOKEN_POLLUTION_TRIGGER_LIST_GIGANTIC_MAP, "POLLUTION_TRIGGER_LIST_GIGANTIC_MAP"},
		{TOKEN_POLLUTION_TRIGGER, "POLLUTION_TRIGGER"},
		{TOKEN_POLLUTION_OZONE_DISASTER, "POLLUTION_OZONE_DISASTER"},
		{TOKEN_POLLUTION_FLOOD_DISASTER, "POLLUTION_FLOOD_DISASTER"},
		{TOKEN_POLLUTION_WARNING, "POLLUTION_WARNING"},

	} ;











sint32 PollutionDatabase::ParsePollutionDatabase(char *filename, C3DIR dir)
	{
	Token	*pollutionToken = new Token(filename, TOKEN_POLLUTION_MAX_VAL - TOKEN_MAX, g_PollutionDB_token_data, dir) ;
	
	SetSize(MAX_POLLUTION_TRIGGERS) ;

	ParseAPollution(pollutionToken, TOKEN_POLLUTION_TRIGGER_LIST_SMALL_MAP);
	ParseAPollution(pollutionToken, TOKEN_POLLUTION_TRIGGER_LIST_MEDIUM_MAP);
	ParseAPollution(pollutionToken, TOKEN_POLLUTION_TRIGGER_LIST_LARGE_MAP);
	ParseAPollution(pollutionToken, TOKEN_POLLUTION_TRIGGER_LIST_GIGANTIC_MAP);

	delete pollutionToken ;

	if (g_parse_pollution_abort) 
		return (FALSE) ;

	return (TRUE) ;
	}











sint32 PollutionDatabase::ParseAPollution(Token *pollutionToken, TOKEN_POLLUTION which)
	{
	sint32  rec = 0;

	
	if (pollutionToken->GetType() == TOKEN_EOF)
		return (FALSE) ;
	
	if (pollutionToken->GetType() != which)
		{
		g_parse_pollution_abort = TRUE ;
		c3errors_ErrorDialog(pollutionToken->ErrStr(), "pollution trigger list required") ;
		return (FALSE) ;
		}

	if (pollutionToken->Next() != TOKEN_OPEN_BRACE)
		{
		g_parse_pollution_abort = TRUE ;
		c3errors_ErrorDialog(pollutionToken->ErrStr(), "pollution trigger list requires an opening brace") ;
		return (FALSE) ;
		}

		AddRec(0, rec);
		if (!m_rec[rec].ParsePollutionRecord(pollutionToken))
			{
			g_parse_pollution_abort = TRUE ;
			return (FALSE) ;
			}

		rec++ ;




















	if (pollutionToken->GetType() != TOKEN_CLOSE_BRACE)
		{
		g_parse_pollution_abort = TRUE ;
		c3errors_ErrorDialog(pollutionToken->ErrStr(), "pollution trigger list requires a closing brace") ;
		return (FALSE) ;
		}

	pollutionToken->Next();

	return (TRUE) ;
	}









PollutionDatabase::~PollutionDatabase()
	{
	}
















































