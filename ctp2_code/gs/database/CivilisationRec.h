//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ header
// Description  : 
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
// Compiler flags
//
//----------------------------------------------------------------------------
//
// Modifications from the original Activision code:
//
// - #pragma once commented out.
//
//----------------------------------------------------------------------------
#if defined(HAVE_PRAGMA_ONCE)
#pragma once
#endif

#ifndef __CIVILISATION_REC_H__
#define __CIVILISATION_REC_H__

class CivilisationRecord;

#define k_MAX_CITY_NAMES		500
#define k_CAPITAL_UNDEFINED		-1
#define k_CITY_NAME_UNDEFINED	-1

#include "Rec.h"        // Record
#include "dbtypes.h"    // StringId

class Token;
class CivArchive ;

class CivilisationRecord : public Record
	{
	public:
	    
		
		StringId	m_leader_name,
					m_leader_name_female,
                    m_personality_description,
					m_country_name,
					m_singular_name,
					m_plural_name,
					m_emissary_photo;

		sint32	m_capital,
				m_cityStyle,		
				m_numCities,
			    m_parchment;  

		StringId	*m_cityname;

		
		

		
		
		
		char *m_personality_male, *m_personality_female;
		
		
		
   
		CivilisationRecord() ;
		~CivilisationRecord();
		void Serialize(CivArchive &archive) ;

		sint32 ParseRecord(Token *token) ;
		sint32 ParseCityList(Token *token) ;
		StringId GetLeaderName(void) { return (m_leader_name) ; }
		StringId GetLeaderNameFemale(void) { return (m_leader_name_female); }
        StringId GetPersonalityDescription() { return m_personality_description; } 
		StringId GetPluralCivName(void) { return (m_plural_name) ; }
		StringId GetCountryName(void) { return (m_country_name) ; }
		StringId GetSingularCivName(void) { return (m_singular_name) ; }
		StringId GetEmissaryPhoto( void ) const { return ( m_emissary_photo ); }
		sint32 GetNumCities(void) { return (m_numCities) ; }
		StringId GetCityName(const sint32 city) ;
		sint32 GetParchment() const { return m_parchment; }

		StringId GetCapital(void) const { return (m_capital) ; }

		sint32 GetCityStyle( void ) { return m_cityStyle; }
		const char *GetMalePersonality() const { return m_personality_male; }
		const char *GetFemalePersonality() const { return m_personality_female; }
	} ;

#endif
