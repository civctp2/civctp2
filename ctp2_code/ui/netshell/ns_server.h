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

#ifndef __NS_SERVER_H__
#define __NS_SERVER_H__

#include "ns_object.h"


class ns_Server : public ns_Object<NETFunc::Server, ns_Server>
{
public:
#if defined(ACTIVISION_ORIGINAL)
	ns_Server( NETFunc::Server *server ) : ns_Object<NETFunc::Server, ns_Server>(server) {};
#else
	ns_Server(NETFunc::Server * server);
#endif
	void Update( NETFunc::Server *server );

	char *m_name;
	int m_players;
	int m_ping;
};


#endif 
