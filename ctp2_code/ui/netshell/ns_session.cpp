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

#include "c3.h"

#include "netshell.h"
#include "ns_session.h"



#if defined(ACTIVISION_ORIGINAL)
int ns_Session::count = 4;
ns_Session::Struct ns_Session::list[] = {
	{ICON,		(Data)&m_locked},
	{ICON,		(Data)&m_closed},
	{STRING,	(Data)&m_name},
	{INT,		(Data)&m_players}
};
#else
ns_Session::ns_Session(NETFunc::Session * session) 
:	ns_Object<NETFunc::Session, ns_Session>(session) 
{
	list.push_back(Struct(ICON,		&m_locked));
	list.push_back(Struct(ICON,		&m_closed));
	list.push_back(Struct(STRING,	&m_name));
	list.push_back(Struct(INT,		&m_players));
};
#endif

void ns_Session::Update( NETFunc::Session *session ) {
	SetMine(session->IsCurrentSession());
	m_locked = strlen(session->GetPassword()) ? g_netshell->GetTrueBmp() : 0;
	m_closed = session->IsClosed() ? g_netshell->GetTrueBmp() : 0;
	m_name = session->GetName();
	m_players = session->GetPlayers();
}

