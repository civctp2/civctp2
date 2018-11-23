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
#include "ns_player.h"

ns_Player::ns_Player(NETFunc::Player * player)
:	ns_Object<NETFunc::Player, ns_Player>(player)
{
	list.push_back(Struct(ICON,		&m_mute));
	list.push_back(Struct(STRING,	&m_name));
	list.push_back(Struct(INT,		&m_ping));
};

void ns_Player::Update( NETFunc::Player *player ) {
	SetMine(player->IsMe());
	m_mute = player->IsMuted() ? g_netshell->GetTrueBmp() : 0;
	m_name = player->GetName();
	m_ping = player->GetLatency();
}
