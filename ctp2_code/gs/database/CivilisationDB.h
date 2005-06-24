//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ header
// Description  : Civilisation database
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
// _MSC_VER		
// - Compiler version (for the Microsoft C++ compiler only)
//
//----------------------------------------------------------------------------
//
// Modifications from the original Activision code:
//
// - Corrected Asserts.
//
//----------------------------------------------------------------------------

#ifdef HAVE_PRAGMA_ONCE
#pragma once
#endif

#ifndef __CIVILISATION_DATABASE_H__
#define __CIVILISATION_DATABASE_H__

#include "GameObj_types.h"
#include "DB.h"
#include "CivilisationRec.h"
#include "c3files.h"

class CivArchive ;
class Token ;

class CivilisationDatabase : public Database<CivilisationRecord>
	{
	private:

	public:
		CivilisationDatabase () ;
		CivilisationDatabase (CivArchive &archive) ;
		~CivilisationDatabase () ;
		void Serialize(CivArchive &archive) ;

		

		BOOL Initialise(char *filename, C3DIR dir) ;
	    BOOL ParseCivilisationDatabase(char *filename, C3DIR dir);
		BOOL ParseACivilisation(Token *token, sint32 count) ;

		sint32 GetCivilisations(void) const { return (m_nRec) ; }

		StringId GetLeaderName(const CIV_INDEX civ) const
			{
			Assert(civ>=0) ; 
			Assert(civ < (CIV_INDEX)(m_nRec)) ;
			return (m_rec[civ].GetLeaderName()) ;
			}
		
		StringId GetLeaderNameFemale(const CIV_INDEX civ) const
			{
			Assert(civ>=0) ; 
			Assert(civ < (CIV_INDEX)(m_nRec)) ;
			return (m_rec[civ].GetLeaderNameFemale()) ;
			}

        StringId GetPersonalityDescription(const CIV_INDEX civ) const
        { 
            Assert(civ>=0); 
            Assert(civ < (CIV_INDEX)(m_nRec));
            return (m_rec[civ].GetPersonalityDescription()); 
        } 


		StringId GetPluralCivName(const CIV_INDEX civ) const
			{
			Assert(civ>=0) ; 
			Assert(civ < (CIV_INDEX)(m_nRec)) ;
			return (m_rec[civ].GetPluralCivName()) ;
			}
		
		StringId GetCountryName(const CIV_INDEX civ) const
			{
			Assert(civ>=0) ; 
			Assert(civ < (CIV_INDEX)(m_nRec)) ;
			return (m_rec[civ].GetCountryName()) ;
			}

		StringId GetSingularCivName(const CIV_INDEX civ) const
			{
			Assert(civ>=0) ; 
			Assert(civ < (CIV_INDEX)(m_nRec)) ;
			return (m_rec[civ].GetSingularCivName()) ;
			}

		StringId GetEmissaryPhoto( const CIV_INDEX civ ) const
		{
			Assert( civ >= 0 );
			Assert( civ < (CIV_INDEX)(m_nRec));
			return ( m_rec[civ].GetEmissaryPhoto() );
		}

		sint32 GetNumCities(const CIV_INDEX civ) const
			{
			Assert(civ>=0) ; 
			Assert(civ < (CIV_INDEX)(m_nRec)) ;
			return (m_rec[civ].GetNumCities()) ;
			}

		StringId GetCityName(const CIV_INDEX civ, const sint32 city)
			{
			Assert(civ>=0) ; 
			Assert(civ < (CIV_INDEX)(m_nRec)) ;
			return (m_rec[civ].GetCityName(city)) ;
			}

		StringId GetCapital(const CIV_INDEX civ) const
			{
			Assert(civ>=0) ; 
			Assert(civ < (CIV_INDEX)(m_nRec)) ;
			return (m_rec[civ].GetCapital()) ;
			}

		sint32 GetCityStyle( const CIV_INDEX civ ) const {
			Assert( civ >= 0 );
			Assert( civ < (CIV_INDEX)(m_nRec) );
			return ( m_rec[civ].GetCityStyle() );
		}

	} ;

extern CivilisationDatabase *g_theCivilisationDB ;

#endif

