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
//----------------------------------------------------------------------------
//
// Modifications from the original Activision code:
//
// - Corrected strange access of non-static members from static data.
//
//----------------------------------------------------------------------------

#ifndef __NS_PLAYERSETUP_H__
#define __NS_PLAYERSETUP_H__

#include "ns_object.h"

#define k_PS_MAXLEN 48

class nf_PlayerSetup:public NETFunc::PlayerSetup {
public:

	static sint32 m_version;

	nf_PlayerSetup(NETFunc::KeyStruct *k);
	nf_PlayerSetup(NETFunc::Player *p);
	nf_PlayerSetup(void);
	void SetKey(NETFunc::KeyStruct *k);
	void Pack();
	void Unpack();

	void Reset();

	const char *GetIcq() const { return (const char *)m_icq; }
	void SetIcq( const char *icq )
	{
		memset( m_icq, 0, sizeof( m_icq ) );
		if ( icq )
			strncpy( m_icq, icq, k_PS_MAXLEN );
	}

	const char *GetEmail() const { return (const char *)m_email; }
	void SetEmail( const char *email )
	{
		memset( m_email, 0, sizeof( m_email ) );
		if ( email )
			strncpy( m_email, email, k_PS_MAXLEN );
	}

	const char *GetLocation() const { return (const char *)m_location; }
	void SetLocation( const char *location )
	{
		memset( m_location, 0, sizeof( m_icq ) );
		if ( location )
			strncpy( m_location, location, k_PS_MAXLEN );
	}

	int	GetExperience() const { return m_experience; }
	void SetExperience( int experience )
	{ m_experience = experience; }





	int	GetTribe()
	{
		if ( GetBlobLen() == 0 )
			return 0;
		return *(int *)GetBlob();
	}
	void SetTribe( int tribe )
	{
		*(int *)GetBlob() = tribe;
		SetBlobLen( 3 * sizeof(int) );
	}

	int	GetCivPoints()
	{
		if ( GetBlobLen() == 0 )
			return 0;
		return *(int *)(GetBlob() + sizeof(int));
	}
	void SetCivPoints( int civPoints )
	{
		*(int *)(GetBlob() + sizeof(int)) = civPoints;
		SetBlobLen( 3 * sizeof(int) );
	}

	int	GetPwPoints()
	{
		if ( GetBlobLen() == 0 )
			return 0;
		return *(int *)(GetBlob() + 2 * sizeof(int));
	}
	void SetPwPoints( int pwPoints )
	{
		*(int *)(GetBlob() + 2 * sizeof(int)) = pwPoints;
		SetBlobLen( 3 * sizeof(int) );
	}

private:
	char m_icq[ k_PS_MAXLEN + 1 ];
	char m_email[ k_PS_MAXLEN + 1 ];
	char m_location[ k_PS_MAXLEN + 1 ];
	int m_experience;
};

class ns_PlayerSetup : public ns_Object<nf_PlayerSetup, ns_PlayerSetup>
{
public:
	ns_PlayerSetup(nf_PlayerSetup * playersetup);
	void Update( nf_PlayerSetup *playersetup );

	char *m_name;
};

#endif
