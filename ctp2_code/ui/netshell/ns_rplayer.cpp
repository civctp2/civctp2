









#include "c3.h"

#include "ns_rplayer.h"



int ns_RPlayer::count = 2;
ns_RPlayer::Struct ns_RPlayer::list[] = {
	{STRING,	(Data)&m_name},
	{INT,		(Data)&m_ping}
};


void ns_RPlayer::Update( NETFunc::Player *player ) {
	SetMine(false);
	m_name = player->GetName();
	m_ping = 100;
}
