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
#include "ns_lobby.h"



#if defined(ACTIVISION_ORIGINAL)
int ns_Lobby::count = 3;
ns_Lobby::Struct ns_Lobby::list[] = {
	{ICON,		(Data)&m_closed},
	{STRING,	(Data)&m_name},
	{INT,		(Data)&m_players}
};
#else
ns_Lobby::ns_Lobby(NETFunc::Lobby * lobby) 
:	ns_Object<NETFunc::Lobby, ns_Lobby>(lobby) 
{
	list.push_back(Struct(ICON,		&m_closed));
	list.push_back(Struct(STRING,	&m_name));
	list.push_back(Struct(INT,		&m_players));
};
#endif


void ns_Lobby::Update( NETFunc::Lobby *lobby ) {
	SetMine(lobby->IsCurrentSession());
	m_closed = lobby->IsClosed() ? g_netshell->GetTrueBmp() : 0;
	m_name = lobby->GetName();
	m_players = lobby->GetPlayers();
}

