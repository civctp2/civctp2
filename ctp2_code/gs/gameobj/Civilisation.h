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
// _MSC_VER
// - When defined, allows Microsoft C++ extensions.
// - When not defined, generates standard C++.
//
// Note: For the blocks with _MSC_VER preprocessor directives, the following
//       is implied: the (_MSC_VER) preprocessor directive lines and the blocks
//       between #else and #endif are modified Apolyton code. The blocks
//       between #if and #else are the original Activision code.
//
//----------------------------------------------------------------------------
//
// Modifications from the original Activision code:
//
// - pragma commented out
// - Fixed city style enumeration removed.
//
//----------------------------------------------------------------------------

#ifdef HAVE_PRAGMA_ONCE
#pragma once
#endif

#ifndef __CIVILISATION_H__
#define __CIVILISATION_H__

#include "ID.h"
#include "GameObj_types.h"
#include "CivilisationData.h"

//----------------------------------------------------------------------------
//
// City style values
//
// Valid values range from 0 up to (not including) the number of records of
// g_theCityStyleDB. Some special values have been defined here, because
// CityStyleRecord is a generated file.
//
// CITY_STYLE_EDITOR	: use the value from the scenario editor.
// CITY_STYLE_GENERIC	: use the value from a less specific object (e.g. a
//                        city may inherit the style from its civilisation).
// CITY_STYLE_DEFAULT	: use when all else fails.
//
//----------------------------------------------------------------------------

	sint32 const	CITY_STYLE_EDITOR	= -2;
	sint32 const	CITY_STYLE_GENERIC	= -1;
	sint32 const	CITY_STYLE_DEFAULT	=  0;

class Civilisation : public ID
	{
	public:
		Civilisation() : ID() { return ; }
		Civilisation(sint32 val) : ID(val) { return ; }
		Civilisation(uint32 val) : ID(val) { return ; }
#if defined(WIN32)
		Civilisation(const int val) : ID(val) { return ; }
		Civilisation(const unsigned int val) : ID(val) { return ; }
#endif

		CivilisationData *operator -> () const { return AccessData(); }

		void KillCivilisation() ;
		void Kill() { KillCivilisation() ; } ;
		void RemoveAllReferences() ;

		const CivilisationData *GetData() const ;
		CivilisationData *AccessData() const;
		const CivilisationRecord *GetDBRec() const;

		void Castrate() {} ;
		PLAYER_INDEX GetOwner(void) const { return (GetData()->GetOwner()) ; }
		CIV_INDEX GetCivilisation(void) const { return (GetData()->GetCivilisation()) ; }
		GENDER GetGender(void) const { return (GetData()->GetGender()); }

		const MBCHAR *GetLeaderName(void) { return (AccessData()->GetLeaderName()) ; }
		void GetPluralCivName(MBCHAR *s) { AccessData()->GetPluralCivName(s) ; }
		void GetCountryName(MBCHAR *s) { AccessData()->GetCountryName(s) ; }
		void GetSingularCivName(MBCHAR *s) { AccessData()->GetSingularCivName(s) ; }

		sint32 GetCityStyle( void ) { return AccessData()->GetCityStyle(); }

		void ResetCiv(CIV_INDEX newCivIndex, GENDER gender) { AccessData()->ResetCiv(newCivIndex, gender); }
	} ;

PLAYER_INDEX civilisation_NewCivilisationOrVandals(PLAYER_INDEX old_owner);
void civilisation_CreateNewPlayer(sint32 pi, sint32 old_owner);

extern Civilisation	*g_theCivilisation ;
#else
class Civilisation;
#endif
