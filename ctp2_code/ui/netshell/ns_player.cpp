










#include "c3.h"

#include "netshell.h"
#include "ns_player.h"



int ns_Player::count = 3;
ns_Player::Struct ns_Player::list[] = {
	{ICON,		(Data)&m_mute},
	{STRING,	(Data)&m_name},
	{INT,		(Data)&m_ping}
};


void ns_Player::Update( NETFunc::Player *player ) {
	SetMine(player->IsMe());
	m_mute = player->IsMuted() ? g_netshell->GetTrueBmp() : 0;
	m_name = player->GetName();
	m_ping = player->GetLatency();
}
