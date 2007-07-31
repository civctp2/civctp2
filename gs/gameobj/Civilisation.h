










#pragma once
#ifndef __CIVILISATION_H__
#define __CIVILISATION_H__

#include "ID.h"
#include "CivilisationData.h"

enum CIV_INDEX ;

enum GENDER {
	GENDER_MALE,
	GENDER_FEMALE,

	GENDER_RANDOM,
	
	GENDER_MAX
};

enum CITY_STYLE {
	CITY_STYLE_ROMAN,
	CITY_STYLE_ARABIAN,
	CITY_STYLE_ASIAN,

	CITY_STYLE_MAX
};

class Civilisation : public ID
	{
	public:
		
		

		
		

		
		
		

		
		

	public:
		Civilisation() : ID() { return ; }
		Civilisation(sint32 val) : ID(val) { return ; }
		Civilisation(uint32 val) : ID(val) { return ; }
		Civilisation(const int val) : ID(val) { return ; }
		Civilisation(const unsigned int val) : ID(val) { return ; }

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

		MBCHAR *GetLeaderName(void) { return (AccessData()->GetLeaderName()) ; }
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

class Civilisation ;

#endif
