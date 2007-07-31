










#include "c3.h"

#include "netshell.h"
#include "ns_session.h"



int ns_Session::count = 4;
ns_Session::Struct ns_Session::list[] = {
	{ICON,		(Data)&m_locked},
	{ICON,		(Data)&m_closed},
	{STRING,	(Data)&m_name},
	{INT,		(Data)&m_players}
};


void ns_Session::Update( NETFunc::Session *session ) {
	SetMine(session->IsCurrentSession());
	m_locked = strlen(session->GetPassword()) ? g_netshell->GetTrueBmp() : 0;
	m_closed = session->IsClosed() ? g_netshell->GetTrueBmp() : 0;
	m_name = session->GetName();
	m_players = session->GetPlayers();
}

