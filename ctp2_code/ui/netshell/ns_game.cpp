










#include "c3.h"

#include "netshell.h"
#include "ns_game.h"



int ns_Game::count = 5;
ns_Game::Struct ns_Game::list[] = {
	{ICON,		(Data)&m_launched},
	{STRING,	(Data)&m_name},
	{ICON,		(Data)&m_locked},
	{ICON,		(Data)&m_closed},
	{INT,		(Data)&m_players}

};


void ns_Game::Update( NETFunc::Game *game ) {
	SetMine(game->IsCurrentSession());
	m_locked = strlen(game->GetPassword()) ? g_netshell->GetTrueBmp() : 0;
	m_closed = (game->IsClosed() || game->IsHostile() || game->GetFree() == 0) ? g_netshell->GetTrueBmp() : 0;
	m_launched = game->IsLaunched() ? g_netshell->GetTrueBmp() : 0;
	m_name = game->GetName();
	m_players = game->GetPlayers();

}

