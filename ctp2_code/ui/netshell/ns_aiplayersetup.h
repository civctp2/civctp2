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

#ifndef __NS_AIPLAYERSETUP_H__
#define __NS_AIPLAYERSETUP_H__

#include "ns_object.h"

class nf_AIPlayer:public NETFunc::AIPlayer {
public:

	static sint32 m_version;

	nf_AIPlayer(void);
	nf_AIPlayer(NETFunc::AIPlayer *p);

	void SetKey(NETFunc::KeyStruct *k);

	void Pack();
	void Unpack();

	
	void Reset();

	int	GetTribe() const { return m_tribe; }
	void SetTribe( int tribe )
	{ m_tribe = tribe; }

	int	GetCivPoints() const { return m_civPoints; }
	void SetCivPoints( int civPoints )
	{ m_civPoints = civPoints; }

	int	GetPwPoints() const { return m_pwPoints; }
	void SetPwPoints( int pwPoints )
	{ m_pwPoints = pwPoints; }

protected:
	int m_tribe;
	int m_civPoints;
	int m_pwPoints;
};


class ns_AIPlayerSetup : public ns_Object<NETFunc::AIPlayer, ns_AIPlayerSetup>
{
public:
#if defined(ACTIVISION_ORIGINAL)	
	ns_AIPlayerSetup( NETFunc::AIPlayer *aiplayer ) : ns_Object<NETFunc::AIPlayer, ns_AIPlayerSetup>(aiplayer) {};
#else
	ns_AIPlayerSetup(NETFunc::AIPlayer * player);
#endif
	void Update( NETFunc::AIPlayer *aiplayer );

	char *m_name;
};

#endif 
