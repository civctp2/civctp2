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

#include "ns_server.h"



ns_Server::ns_Server(NETFunc::Server * server) 
:	ns_Object<NETFunc::Server, ns_Server>(server) 
{
	list.push_back(Struct(STRING,	&m_name));
	list.push_back(Struct(INT,		&m_players));
	list.push_back(Struct(INT,		&m_ping));
};


void ns_Server::Update( NETFunc::Server *server ) {
	SetMine(false);
	m_name = server->GetName();
	m_players = server->GetPlayers();
	m_ping = server->GetPing();
}

