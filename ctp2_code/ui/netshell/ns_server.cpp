










#include "c3.h"

#include "ns_server.h"



int ns_Server::count = 3;
ns_Server::Struct ns_Server::list[] = {
	{STRING,	(Data)&m_name},
	{INT,		(Data)&m_players},
	{INT,		(Data)&m_ping}
};


void ns_Server::Update( NETFunc::Server *server ) {
	SetMine(false);
	m_name = server->GetName();
	m_players = server->GetPlayers();
	m_ping = server->GetPing();
}

