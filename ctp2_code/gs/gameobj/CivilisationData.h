//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ header
// Description  : 
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
//
// Compiler flags
// 
// _MSC_VER		
// - Use Microsoft C++ extensions when set.
//
//----------------------------------------------------------------------------
//
// Modifications from the original Activision code:
//
// - #pragma once commented out
// - Import structure modified to allow mingw compilation.
//
//----------------------------------------------------------------------------

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef __CIVILISATION_DATA_H__
#define __CIVILISATION_DATA_H__

#if defined(_MSC_VER)

#include "Gameobj.h"
#include "ID.h"
#include "CivilisationRec.h"
#include "gstypes.h"

extern enum CIV_INDEX ;
enum GENDER;

#else	// _MSC_VER

//----------------------------------------------------------------------------
// Library imports
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Exported names
//----------------------------------------------------------------------------

class	CivilisationData;

//----------------------------------------------------------------------------
// Project imports
//----------------------------------------------------------------------------

#include "c3types.h"			// MBCHAR, sint..., uint...
#include "civarchive.h"			// CivArchive
#include "Civilisation.h"		// GENDER
#include "Civilisationpool.h"	// CIV_INDEX
#include "CivilisationRec.h"	// k_MAX_CITY_NAMES
#include "dbtypes.h"			// k_MAX_NAME_LEN
#include "Gameobj.h"			// GAMEOBJ
#include "gstypes.h"			// PLAYER_INDEX
#include "ID.h"					// ID

//----------------------------------------------------------------------------
// Class declarations
//----------------------------------------------------------------------------


#endif	// _MSC_VER

class CivilisationData : public GAMEOBJ
	{
	public:
		
		

		PLAYER_INDEX	m_owner ;									

		uint8			m_cityname_count[k_MAX_CITY_NAMES] ;		
		
		CIV_INDEX		m_civ ;										

		GENDER			m_gender;

		sint32			m_cityStyle;								

		MBCHAR			m_leader_name[k_MAX_NAME_LEN],
                        m_personality_description[k_MAX_NAME_LEN],
						m_civilisation_name[k_MAX_NAME_LEN],
						m_country_name[k_MAX_NAME_LEN],
						m_singular_name[k_MAX_NAME_LEN] ;
		
		

		
		
		

		
		

		friend class NetCivilization;

	public:
		CivilisationData(const ID &id) ;
		CivilisationData(const ID &id, PLAYER_INDEX owner, CIV_INDEX civ, GENDER gender) ;
		CivilisationData(CivArchive &archive) ;

		void Serialize(CivArchive &archive) ;

		PLAYER_INDEX GetOwner(void) const { return (m_owner) ; }	
		CIV_INDEX GetCivilisation(void) const { return (m_civ) ; }	
		GENDER GetGender(void) const { return m_gender; }

		sint32 GetAnyCityName(void) const ;
		sint32 GetCapitalName(void) const ;

		void GetCityName(const sint32 name, MBCHAR *s) const;
		void UseCityName(const sint32 name) ;
		void ReleaseCityName(const sint32 name) ;
		sint32 GetUseCount(const sint32 name) const ;

		MBCHAR *GetLeaderName(void) ;
		void SetLeaderName(const MBCHAR *s) ;

        void SetPersonalityDescription(const MBCHAR* s);
        MBCHAR* GetPersonalityDescription(void);

		void GetPluralCivName(MBCHAR *s) ;
		void SetPluralCivName(const MBCHAR *s) ;
		void GetCountryName(MBCHAR *s) ;
		void SetCountryName(const MBCHAR *s) ;
		void GetSingularCivName(MBCHAR *s) ;
		void SetSingularCivName(const MBCHAR *s) ;

		sint32 GetCityStyle( void ) { return m_cityStyle; }
		void SetCityStyle( sint32 cityStyle ) { m_cityStyle = cityStyle; }

		
		void ResetCiv(CIV_INDEX newCivIndex, GENDER gender);

		void ResetStrings();
	} ;

#endif
