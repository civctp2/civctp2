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
// Compiler flags
// 
// ACTIVISION_ORIGINAL		
// - When defined, generates the original Activision code.
// - When not defined, generates the modified Apolyton code.
//
//----------------------------------------------------------------------------
//
// Modifications from the original Activision code:
//
// - Corrected strange access of non-static members from static data.
//
//----------------------------------------------------------------------------

#ifndef __NS_GAMESETUP_H__
#define __NS_GAMESETUP_H__


#include "ns_object.h"
#include "ns_tribes.h"
#include "ns_units.h"
#include "ns_improvements.h"
#include "ns_wonders.h"

#define k_NS_MAX_HUMANS		4	
#define k_NS_MAX_PLAYERS	8	

struct TribeSlot
{
	sint8	isFemale;	
	sint8	isAI;		
	uint16	key;		
	sint32	tribe;		
};


class nf_GameSetup:public NETFunc::GameSetup {
public:

	static sint32 m_version;

	nf_GameSetup(NETFunc::KeyStruct *k);
	nf_GameSetup(NETFunc::Game *g);
	nf_GameSetup(void);
	void Init( void );
	void SetKey(NETFunc::KeyStruct *k);
	void Pack();
	void Unpack();

	
	void Reset() {}

	int	GetPlayStyle() const { return m_playstyle; }
	void SetPlayStyle( int playstyle )
	{ m_playstyle = playstyle; }

	int	GetPlayStyleValue() const { return m_playstylevalue; }
	void SetPlayStyleValue( int playstylevalue )
	{ m_playstylevalue = playstylevalue; }









	char GetHandicapping() const { return m_handicapping; }
	void SetHandicapping( char handicapping )
	{ m_handicapping = handicapping; }

	char GetDynamicJoin() const { return m_dynamicJoin; }
	void SetDynamicJoin( char dynamicJoin )
	{ m_dynamicJoin = dynamicJoin; }





	char GetBloodlust() const { return m_bloodlust; }
	void SetBloodlust( char bloodlust )
	{ m_bloodlust = bloodlust; }













	char GetPollution() const { return m_pollution; }
	void SetPollution( char pollution )
	{ m_pollution = pollution; }

	int GetCivPoints() const { return m_civPoints; }
	void SetCivPoints( int civPoints )
	{ m_civPoints = civPoints; }

	int GetPwPoints() const { return m_pwPoints; }
	void SetPwPoints( int pwPoints )
	{ m_pwPoints = pwPoints; }





	char GetStartAge() const { return m_startAge; }
	void SetStartAge( char startAge )
	{ m_startAge = startAge; }

	char GetEndAge() const { return m_endAge; }
	void SetEndAge( char endAge )
	{ m_endAge = endAge; }

	char GetMapSize() const { return m_mapSize; }
	void SetMapSize( char mapSize )
	{ m_mapSize = mapSize; }

	char GetWorldType1() const { return m_worldType1; }
	void SetWorldType1( char worldType1 )
	{ m_worldType1 = worldType1; }

	char GetWorldType2() const { return m_worldType2; }
	void SetWorldType2( char worldType2 )
	{ m_worldType2 = worldType2; }

	char GetWorldType3() const { return m_worldType3; }
	void SetWorldType3( char worldType3 )
	{ m_worldType3 = worldType3; }

	char GetWorldType4() const { return m_worldType4; }
	void SetWorldType4( char worldType4 )
	{ m_worldType4 = worldType4; }

	char GetWorldType5() const { return m_worldType5; }
	void SetWorldType5( char worldType5 )
	{ m_worldType5 = worldType5; }

	char GetWorldType6() const { return m_worldType6; }
	void SetWorldType6( char worldType6 )
	{ m_worldType6 = worldType6; }

	char GetWorldShape() const { return m_worldShape; }
	void SetWorldShape( char worldShape )
	{ m_worldShape = worldShape; }

	char GetDifficulty1() const { return m_difficulty1; }
	void SetDifficulty1( char difficulty1 )
	{ m_difficulty1 = difficulty1; }

	char GetDifficulty2() const { return m_difficulty2; }
	void SetDifficulty2( char difficulty2 )
	{ m_difficulty2 = difficulty2; }

	TribeSlot *GetTribeSlots( void ) const { return (TribeSlot *)m_tribeSlots; }
	void SetTribeSlot( int slot, BOOL isFemale, BOOL isAI, uint16 key, sint32 tribe)
	{
		m_tribeSlots[ slot ].isFemale = (sint8)isFemale;
		m_tribeSlots[ slot ].isAI = (sint8)isAI;
		m_tribeSlots[ slot ].key = key;
		m_tribeSlots[ slot ].tribe = tribe;
	}

	TribeSlot *GetSavedTribeSlots( void ) const
	{ return (TribeSlot *)m_savedTribeSlots; }
	void SetSavedTribeSlot( int slot, BOOL isFemale, BOOL isAI, uint16 key, sint32 tribe)
	{
		m_savedTribeSlots[ slot ].isFemale = (sint8)isFemale;
		m_savedTribeSlots[ slot ].isAI = (sint8)isAI;
		m_savedTribeSlots[ slot ].key = key;
		m_savedTribeSlots[ slot ].tribe = tribe;
	}

	char GetUnit( int i ) const
	{ return m_excludedUnits[ i >> 3 ] & ( 1 << ( (uint32)i % 8 ) ); }
	void SetUnit( char unit, int i )
	{ unit ?
		m_excludedUnits[ i >> 3 ] |= ( 1 << ( (uint32)i % 8 ) ) :
		m_excludedUnits[ i >> 3 ] &= ~( 1 << ( (uint32)i % 8 ) ); }

	char GetNumAvailUnits( void ) const { return m_numAvailUnits; }
	void SetNumAvailUnits( int numAvailUnits )
	{ m_numAvailUnits = numAvailUnits; }

	char GetImprovement( int i ) const
	{ return m_excludedImprovements[ i >> 3 ] & ( 1 << ( (uint32)i % 8 ) ); }
	void SetImprovement( char improvement, int i )
	{ improvement ?
		m_excludedImprovements[ i >> 3 ] |= ( 1 << ( (uint32)i % 8 ) ) :
		m_excludedImprovements[ i >> 3 ] &= ~( 1 << ( (uint32)i % 8 ) ); }

	char GetNumAvailImprovements( void ) const {return m_numAvailImprovements; }
	void SetNumAvailImprovements( int numAvailImprovements )
	{ m_numAvailImprovements = numAvailImprovements; }

	char GetWonder( int i ) const
	{ return m_excludedWonders[ i >> 3 ] & ( 1 << ( (uint32)i % 8 ) ); }
	void SetWonder( char wonder, int i )
	{ wonder ?
		m_excludedWonders[ i >> 3 ] |= ( 1 << ( (uint32)i % 8 ) ) :
		m_excludedWonders[ i >> 3 ] &= ~( 1 << ( (uint32)i % 8 ) ); }

	char GetNumAvailWonders( void ) const { return m_numAvailWonders; }
	void SetNumAvailWonders( int numAvailWonders )
	{ m_numAvailWonders = numAvailWonders; }

	



	
	void SetSavedId( uint32 savedId );
	uint32 GetSavedId( void );

private:
	int m_playstyle;
	int m_playstylevalue;



	char m_handicapping;
	char m_dynamicJoin;

	char m_bloodlust;



	char m_pollution;
	int m_civPoints;
	int m_pwPoints;


	char m_startAge;
	char m_endAge;
	char m_mapSize;
	char m_worldType1;
	char m_worldType2;
	char m_worldType3;
	char m_worldType4;
	char m_worldType5;
	char m_worldType6;
	char m_worldShape;
	char m_difficulty1;
	char m_difficulty2;

	TribeSlot m_tribeSlots[ k_NS_MAX_PLAYERS ];
	TribeSlot m_savedTribeSlots[ k_NS_MAX_PLAYERS ];

	int m_numAvailUnits;
	char m_excludedUnits[ ( k_UNITS_MAX >> 3 ) + 1 ];
	int m_numAvailImprovements;
	char m_excludedImprovements[ ( k_IMPROVEMENTS_MAX >> 3 ) + 1 ];
	int m_numAvailWonders;
	char m_excludedWonders[ ( k_WONDERS_MAX >> 3 ) + 1 ];
};


class ns_GameSetup : public ns_Object<nf_GameSetup, ns_GameSetup>
{
public:
#if defined(ACTIVISION_ORIGINAL)	
	ns_GameSetup( nf_GameSetup *gamesetup ) : ns_Object<nf_GameSetup, ns_GameSetup>(gamesetup) {};
#else
	ns_GameSetup(nf_GameSetup * gamesetup);
#endif
	void Update( nf_GameSetup *gamesetup );

	char *m_name;
};


#endif 
