//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : 
// Id           : $Id:$
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
// - None
//
//----------------------------------------------------------------------------
//
// Modifications from the original Activision code:
//
// - Corrected strange access of non-static members from static data.
//
//----------------------------------------------------------------------------

#include "c3.h"

#include "netshell.h"
#include "ns_gamesetup.h"



sint32 nf_GameSetup::m_version = 105;


nf_GameSetup::nf_GameSetup(NETFunc::KeyStruct *k):NETFunc::GameSetup() {
	SetKey(k);
	Init();
}

nf_GameSetup::nf_GameSetup(NETFunc::Game *g):NETFunc::GameSetup(g) {
	Init();
}

nf_GameSetup::nf_GameSetup(void):NETFunc::GameSetup() {
	Init();

	
	
	
	SetSyncLaunch( true );
}

void nf_GameSetup::Init( void ) {
	SetPlayStyle(0);
	SetPlayStyleValue(0);



	SetHandicapping(0);
	SetDynamicJoin(0);

	SetBloodlust(0);



	SetPollution(1); 
	SetCivPoints(100); 
	SetPwPoints(1); 


	SetStartAge(0);
	SetEndAge(4); 
	SetMapSize(0);
	SetWorldType1(5); 
	SetWorldType2(5); 
	SetWorldType3(5); 
	SetWorldType4(5); 
	SetWorldType5(5); 
	SetWorldType6(5); 
	SetWorldShape(0);
	SetDifficulty1(0);
	SetDifficulty2(0);

	memset( m_tribeSlots, 0, sizeof( m_tribeSlots ) );
	memset( m_savedTribeSlots, 0, sizeof( m_savedTribeSlots ) );

	m_numAvailUnits = 0;
	memset( m_excludedUnits, 0, sizeof( m_excludedUnits ) );
	m_numAvailImprovements = 0;
	memset( m_excludedImprovements, 0, sizeof( m_excludedImprovements ) );
	m_numAvailWonders = 0;
	memset( m_excludedWonders, 0, sizeof( m_excludedWonders ) );


	SetSavedId( 0 );
}

void nf_GameSetup::SetKey(NETFunc::KeyStruct *k) {
	memcpy(&key, k, sizeof(NETFunc::KeyStruct));
}

void nf_GameSetup::Pack()
{
	GameSetup::Pack();

	Push( m_playstyle );
	Push( m_playstylevalue );



	Push( m_handicapping );
	Push( m_dynamicJoin );

	Push( m_bloodlust );



	Push( m_pollution );
	Push( m_civPoints );
	Push( m_pwPoints );


	Push( m_startAge );
	Push( m_endAge );
	Push( m_mapSize );
	Push( m_worldType1 );
	Push( m_worldType2 );
	Push( m_worldType3 );
	Push( m_worldType4 );
	Push( m_worldType5 );
	Push( m_worldType6 );
	Push( m_worldShape );
	Push( m_difficulty1 );
	Push( m_difficulty2 );

	sint32 i;
	for ( i = 0; i < k_NS_MAX_PLAYERS; i++ )
		Push( m_tribeSlots[ i ] );
	for ( i = 0; i < k_NS_MAX_PLAYERS; i++ )
		Push( m_savedTribeSlots[ i ] );

	sint32 count;

	Push( m_numAvailUnits );
	count = ( m_numAvailUnits >> 3 ) + 1;
	for ( i = 0; i < count; i++ )
		Push( m_excludedUnits[ i ] );

	Push( m_numAvailImprovements );
	count = ( m_numAvailImprovements >> 3 ) + 1;
	for ( i = 0; i < count; i++ )
		Push( m_excludedImprovements[ i ] );

	Push( m_numAvailWonders );
	count = ( m_numAvailWonders >> 3 ) + 1;
	for ( i = 0; i < count; i++ )
		Push( m_excludedWonders[ i ] );
}

void nf_GameSetup::Unpack()
{
	GameSetup::Unpack();

	if(size < 1)
		return;

	Pop( m_playstyle );
	Pop( m_playstylevalue );



	Pop( m_handicapping );
	Pop( m_dynamicJoin );

	Pop( m_bloodlust );



	Pop( m_pollution );
	Pop( m_civPoints );
	Pop( m_pwPoints );


	Pop( m_startAge );
	Pop( m_endAge );
	Pop( m_mapSize );
	Pop( m_worldType1 );
	Pop( m_worldType2 );
	Pop( m_worldType3 );
	Pop( m_worldType4 );
	Pop( m_worldType5 );
	Pop( m_worldType6 );
	Pop( m_worldShape );
	Pop( m_difficulty1 );
	Pop( m_difficulty2 );

	sint32 i;
	for ( i = 0; i < k_NS_MAX_PLAYERS; i++ )
		Pop( m_tribeSlots[ i ] );
	for ( i = 0; i < k_NS_MAX_PLAYERS; i++ )
		Pop( m_savedTribeSlots[ i ] );

	sint32 count;

	Pop( m_numAvailUnits );
	count = ( m_numAvailUnits >> 3 ) + 1;
	for ( i = 0; i < count; i++ )
		Pop( m_excludedUnits[ i ] );

	Pop( m_numAvailImprovements );
	count = ( m_numAvailImprovements >> 3 ) + 1;
	for ( i = 0; i < count; i++ )
		Pop( m_excludedImprovements[ i ] );

	Pop( m_numAvailWonders );
	count = ( m_numAvailWonders >> 3 ) + 1;
	for ( i = 0; i < count; i++ )
		Pop( m_excludedWonders[ i ] );
}















void nf_GameSetup::SetSavedId( uint32 savedId )
{
	uint32 *buff = (uint32 *)GetUserField();
	
	buff[ 1 ] = savedId;
	SetUserField( (char *)buff, 2 * sizeof( uint32 ) );
}
uint32 nf_GameSetup::GetSavedId( void )
{
	uint32 *buff = (uint32 *)GetUserField();
	return buff[ 1 ];
}



ns_GameSetup::ns_GameSetup(nf_GameSetup * game) 
:	ns_Object<nf_GameSetup, ns_GameSetup>(game) 
{
	list.push_back(Struct(STRING,	&m_name));
};


void ns_GameSetup::Update( nf_GameSetup *gamesetup ) {
	SetMine(false);
	m_name = gamesetup->GetName();
}
