











#include "c3.h"
#include "c3errors.h"
#include "CivArchive.h"
#include "Token.h"

#include "StrDB.h"
#include "TerrainRecord.h"
#include "DB.h"
#include "GovernmentRecord.h"
#include "PollutionRecord.h"

#include "PollutionToken.h"
#include "AdvanceRecord.h"










	sint32	g_parse_pollution_abort ;









PollutionRecord::PollutionRecord()
	{






	for (sint32 i=0; i<MAX_POLLUTION_TRIGGERS; i++) {
		m_triggerLevel[i] = 0 ;
		m_triggerType[i] = POLLUTION_DISASTER_TYPE_INVALID ;
	}

	m_numTriggers = 0;
	}










sint32 PollutionRecord::ParsePollutionRecord(Token *pollutionToken)
{
	sint32 i = 0;

	while (pollutionToken->Next() == TOKEN_POLLUTION_TRIGGER) {

		
		if (pollutionToken->Next() != TOKEN_NUMBER)
			{
			c3errors_ErrorDialog (pollutionToken->ErrStr(), "number expected for trigger level") ;

			return (FALSE) ;
			}

		pollutionToken->GetNumber(m_triggerLevel[i]) ;

		
		switch (pollutionToken->Next())
			{
			case TOKEN_POLLUTION_OZONE_DISASTER :
				m_triggerType[i] = POLLUTION_DISASTER_TYPE_OZONE ;
				break ;

			case TOKEN_POLLUTION_FLOOD_DISASTER :
				m_triggerType[i] = POLLUTION_DISASTER_TYPE_FLOOD ;
				break ;

			case TOKEN_POLLUTION_WARNING :
				m_triggerType[i] = POLLUTION_DISASTER_TYPE_WARNING ;
				break ;

			default :
				m_triggerType[i] = POLLUTION_DISASTER_TYPE_INVALID ;
				c3errors_ErrorDialog (pollutionToken->ErrStr(), "number expected for probability") ;
				return (FALSE) ;

			}

		if (g_parse_pollution_abort)
			return (FALSE) ;

		i++;

		if (i>=MAX_POLLUTION_TRIGGERS) {
			g_parse_pollution_abort = TRUE ;
			c3errors_ErrorDialog(pollutionToken->ErrStr(), "pollution trigger list too long") ;
		}
	}
	
	m_numTriggers = i;

	return (TRUE) ;
}












void PollutionRecord::Serialize(CivArchive &archive)
	{
	if (archive.IsStoring())
		{
		Record::Serialize(archive);
		archive << m_numTriggers;
		archive.StoreArray(m_triggerLevel, MAX_POLLUTION_TRIGGERS);
		archive.StoreArray(m_triggerType, MAX_POLLUTION_TRIGGERS);
		}
	else
		{
		}

	}
