










#include "c3.h"

#include "netshell.h"
#include "ns_lobby.h"



int ns_Lobby::count = 3;
ns_Lobby::Struct ns_Lobby::list[] = {
	{ICON,		(Data)&m_closed},
	{STRING,	(Data)&m_name},
	{INT,		(Data)&m_players}
};


void ns_Lobby::Update( NETFunc::Lobby *lobby ) {
	SetMine(lobby->IsCurrentSession());
	m_closed = lobby->IsClosed() ? g_netshell->GetTrueBmp() : 0;
	m_name = lobby->GetName();
	m_players = lobby->GetPlayers();
}

