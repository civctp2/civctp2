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

#ifndef __NS_LOBBY_H__
#define __NS_LOBBY_H__

#include "ns_object.h"

class ns_Lobby : public ns_Object<NETFunc::Lobby, ns_Lobby>
{
public:
	ns_Lobby(NETFunc::Lobby * lobby);
	void Update( NETFunc::Lobby *lobby );

	const char *m_closed;
	const char *m_name;
	int m_players;
};

#endif // __NS_LOBBY_H__
