//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
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
//----------------------------------------------------------------------------
//
// Modifications from the original Activision code:
//
// - Corrected strange access of non-static members from static data.
//
//----------------------------------------------------------------------------

#include "c3.h"

#include "ns_playersetup.h"


sint32 nf_PlayerSetup::m_version = 103;


nf_PlayerSetup::nf_PlayerSetup(NETFunc::KeyStruct *k):NETFunc::PlayerSetup() {
	SetKey(k);
	SetIcq(NULL);
	SetEmail(NULL);
	SetLocation(NULL);
	SetExperience(0);
}

nf_PlayerSetup::nf_PlayerSetup(NETFunc::Player *p):NETFunc::PlayerSetup(p) {
	SetIcq(NULL);
	SetEmail(NULL);
	SetLocation(NULL);
	SetExperience(0);
}

nf_PlayerSetup::nf_PlayerSetup(void):NETFunc::PlayerSetup() {
	SetIcq(NULL);
	SetEmail(NULL);
	SetLocation(NULL);
	SetExperience(0);
}

void nf_PlayerSetup::SetKey(NETFunc::KeyStruct *k) {
	memcpy(&key, k, sizeof(NETFunc::KeyStruct));
}

void nf_PlayerSetup::Pack()
{
	PlayerSetup::Pack();
	Push( m_icq );
	Push( m_email );
	Push( m_location );
	Push( m_experience );
}

void nf_PlayerSetup::Unpack()
{
	PlayerSetup::Unpack();
	Pop( m_icq );
	Pop( m_email );
	Pop( m_location );
	Pop( m_experience );
}

void nf_PlayerSetup::Reset()
{
	SetGroup( 0 );
	SetTribe( 0 );
	SetCivPoints( 0 );
	SetPwPoints( 0 );
}



ns_PlayerSetup::ns_PlayerSetup(nf_PlayerSetup * player) 
:	ns_Object<nf_PlayerSetup, ns_PlayerSetup>(player) 
{
	list.push_back(Struct(STRING,	&m_name));
};


void ns_PlayerSetup::Update( nf_PlayerSetup *playersetup ) {
	SetMine(false);
	m_name = playersetup->GetName();
}
