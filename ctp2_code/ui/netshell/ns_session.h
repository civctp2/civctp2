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

#ifndef __NS_SESSION_H__
#define __NS_SESSION_H__

#include "ns_object.h"


class ns_Session : public ns_Object<NETFunc::Session, ns_Session>
{
public:
#if defined(ACTIVISION_ORIGINAL)	
	ns_Session( NETFunc::Session *session ) : ns_Object<NETFunc::Session, ns_Session>(session) {};
#else
	ns_Session(NETFunc::Session * session);
#endif
	void Update( NETFunc::Session *session );

	char *m_locked;	
	char *m_closed;
	char *m_name;
	int m_players;
};


#endif 
