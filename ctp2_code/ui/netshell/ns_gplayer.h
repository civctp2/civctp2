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

#ifndef __NS_GPLAYER_H__
#define __NS_GPLAYER_H__

#include "ns_object.h"


class ns_GPlayer : public ns_Object<NETFunc::Player, ns_GPlayer>
{
public:
#if defined(ACTIVISION_ORIGINAL)	
	ns_GPlayer( NETFunc::Player *player ) : ns_Object<NETFunc::Player, ns_GPlayer>(player) {}
#else
	ns_GPlayer(NETFunc::Player * player);
#endif
	void Update( NETFunc::Player *player );

	char *m_host;
	char *m_launched;
	char *m_name;
	int m_ping;
	char *m_tribe;

	int m_civpoints;
	int m_pwpoints;
};

#endif 
