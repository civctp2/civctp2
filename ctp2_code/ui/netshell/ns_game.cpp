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
//----------------------------------------------------------------------------
//
// Modifications from the original Activision code:
//
// - Corrected strange access of non-static members from static data.
//
//----------------------------------------------------------------------------


#include "c3.h"

#include "netshell.h"
#include "ns_game.h"



ns_Game::ns_Game(NETFunc::Game * game) 
:	ns_Object<NETFunc::Game, ns_Game>(game)
{
	list.push_back(Struct(ICON,		&m_launched));
	list.push_back(Struct(STRING,	&m_name));
	list.push_back(Struct(ICON,		&m_locked));
	list.push_back(Struct(ICON,		&m_closed));
	list.push_back(Struct(INT,		&m_players));
};


void ns_Game::Update( NETFunc::Game *game ) {
	SetMine(game->IsCurrentSession());
	m_locked = strlen(game->GetPassword()) ? g_netshell->GetTrueBmp() : 0;
	m_closed = (game->IsClosed() || game->IsHostile() || game->GetFree() == 0) ? g_netshell->GetTrueBmp() : 0;
	m_launched = game->IsLaunched() ? g_netshell->GetTrueBmp() : 0;
	m_name = game->GetName();
	m_players = game->GetPlayers();

}

