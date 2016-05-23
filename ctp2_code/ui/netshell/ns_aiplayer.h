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

#ifndef __NS_AIPLAYER_H__
#define __NS_AIPLAYER_H__

#include "ns_object.h"

class ns_AIPlayer : public ns_Object<NETFunc::AIPlayer, ns_AIPlayer>
{
public:
	ns_AIPlayer(NETFunc::AIPlayer * aiplayer);

	void Update( NETFunc::AIPlayer *aiplayer );

	char *m_name;

	char *m_tribe;

	int m_civpoints;
	int m_pwpoints;
};

#endif
